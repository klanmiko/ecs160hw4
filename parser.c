#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "parser.h"

#define STRIP_NEWLINE(s) { size_t l = strlen(s); \
    if(l > 0 && s[l - 1] == '\n') s[l - 1] = '\0'; \
    if(l > 1 && s[l - 2] == '\r') s[l - 2] = '\0'; }

#include "util.h"

char* readName(char* line, size_t numCols, size_t nameIndex, bool nameQuoted);
header_info readHeaderQuick(char* line);
void checkQuotation(char* line, size_t numCols, bool* columnsQuoted);

tweet_vector getTweets(FILE* fPtr) {
    // header parsing variables
    char* line = NULL;
    size_t len = 0;

    size_t numCols = 0, nameIndex = 0;
    __ssize_t nread;

    bool *columnsQuoted;

    // Read the header
    if ((nread = getline(&line, &len, fPtr)) != -1) {
        STRIP_NEWLINE(line)

        checkQuotation(line, 0, NULL);
        checkLineLength(nread);

        header_info info = readHeaderQuick(line);
        numCols = info.numCols;
        nameIndex = info.nameIndex;
        columnsQuoted = info.columnsQuoted;
    } else {
        die("Failed to read header\n");
    }

    char** tweets = (char**)malloc(sizeof(char*));
    if(tweets == NULL) die("Failed to allocate size for tweets\n");

    size_t numTweets = 0, tweets_size = 1;

    while ((nread = getline(&line, &len, fPtr)) != -1) {
        STRIP_NEWLINE(line)
        
        checkLineLength(nread);
        checkQuotation(line, numCols, columnsQuoted);

        char* name = readName(line, numCols, nameIndex, columnsQuoted[nameIndex]);

        tweets[numTweets++] = name;

        if(numTweets == tweets_size) {
            tweets = (char**)realloc(tweets, (tweets_size * 2 + 1)*sizeof(char*));
            if(tweets == NULL) die("Failed to reallocate more space for tweets\n");

            tweets_size = tweets_size * 2 + 1;
        }
    }

    if(numTweets >= 20000) die("Number of rows in the CSV exceeds 20000\n");

    free(line);
    free(columnsQuoted);

    if(numTweets == 0) {
        free(tweets);
        tweets = NULL;
    }

    tweet_vector names = {tweets, numTweets};

    return names;
}

char* findName(char* line) {
    char* name = NULL;
    while(name = strstr(line, "name")) {
        size_t start_index = name - line;
        size_t end_index = start_index + strlen("name");

        if(start_index > 0 && line[start_index - 1 ] == '"') {
            start_index--;
            end_index++;    
        }

        if((start_index == 0 || line[start_index - 1] == ',') && (line[end_index] == ',' || line[end_index] == '\0')) {
            return name;
        }

        line = &line[end_index];
    }
    
    return NULL;    
}

void checkDuplicates(char *line) {
    size_t l = strlen(line);
    char* copy = malloc(sizeof(char) * (l + 1)), *t = copy;
    strncpy(copy, line, l);
    copy[l] = '\0';

    size_t index = 0, size = 1;
    char** columnNames = malloc(sizeof(char*) * size);

    char* current = NULL;
    while((current = strsep(&copy, ","))) {
        size_t l = strlen(current);
        if(l > 1 && *current == '"') {
            current[l - 1] = '\0';
            current++;
        }

        for(size_t i = 0; i < index; i++) {
            if(strcmp(current, columnNames[i]) == 0) {
                die("Duplicate column names");
            }
        }

        columnNames[index++] = current;
        if(index == size) {
            columnNames = realloc(columnNames, sizeof(char*) * (2*size + 1));
            size = 2*size + 1;
        }
    }

    free(columnNames);
    free(t);
}

// assumes checkQuotation has run
header_info readHeaderQuick(char* line) {
    // check duplicates
    checkDuplicates(line);
    
    char* name = findName(line);
    if(name == NULL) {
        die("No name field found in header\n");
    }

    size_t columnCount = 0, nameIndex = 0, startIndex = 0;
    size_t length = strlen(line) + 1;
    size_t index = name - line;

    bool* columnsQuoted = (bool*)malloc(sizeof(bool));
    size_t b_size = 1;

    for(size_t i = 0; i < length; i++) {
        if(line[i] == ',' || line[i] == '\0') {
            if(i != 0 && line[i - 1] == '"' && line[startIndex] == '"') {
                columnsQuoted[columnCount] = true;
            } else {
                columnsQuoted[columnCount] = false;
            }

            startIndex = i + 1;
            columnCount++;

            if(columnCount == b_size && i != length - 1) {
                columnsQuoted = realloc(columnsQuoted, (2 * b_size + 1) * sizeof(bool));
                if(!columnsQuoted) die("Failed to reallocate space for boolean array columnsQuoted\n");

                b_size = 2*b_size + 1;
            }
        }
        if(i == index) {
            nameIndex = columnCount;
        }
    }
    
    header_info info = {columnCount, nameIndex, columnsQuoted};
    return info;
}


/* returns true if all values are quoted, false otherwise
    calls die() if only some are quoted */

void checkQuotation(char* line, size_t numCols, bool* columnsQuoted) {
    size_t startIndex = 0, colIdx = 0;
    char c;
    for (size_t i = 0; i < strlen(line) + 1; i++) {
        if (line[i] == ',' || line[i] == '\0') {
            size_t endIndex = i - 1;
            
            // check that the field is not ,,
            if(!(endIndex == -1 || endIndex < startIndex)) {
                // if the current field is quoted
                if (line[startIndex] == '\"' && line[endIndex] == '\"' && startIndex != endIndex) {
                    if (columnsQuoted && (colIdx == numCols || !columnsQuoted[colIdx])) {
                        die("Current field is quoted but shouldn't be\n");
                    }
                } else { // if the current field is not quoted
                    if (line[startIndex] == '\"' || line[endIndex] == '\"') {
                        die("Current field has mismatched quotation marks\n");
                    }
                    if (columnsQuoted && (colIdx == numCols || columnsQuoted[colIdx])) {
                        die("Current field isn't quoted but should be\n");
                    }
                }
            }

            startIndex = i + 1;
            colIdx++;
        }
    }
}

void inline checkLineLength(size_t len) {
    if (len > 1024) {
        die("Line is more than 1024 characters long\n");
    }
}

char* readName(char* line, size_t numCols, size_t nameIndex, bool nameQuoted) {
    size_t i = 0;
    size_t numColsSeen = 0;
    size_t startNameIndex = 0;
    size_t endNameIndex = 0;
    bool inNameCol = false;


    char c;
    while ((c = line[i]) != '\0') {
        if (numColsSeen == nameIndex && !inNameCol) {
            startNameIndex = i;
            inNameCol = true;
        }

        if (c == ',') {
            numColsSeen++;
            if (inNameCol == true) {
                inNameCol = false;
                endNameIndex = i;
            }
        }

        // ,"a", -> a
        // ,a", -> INVALID
        // ,a"a, -> a"a
        // ,""a"", -> "a"
        // ,, -> name is an empty string, still counts
        i++;
    }

    if(inNameCol) {
        endNameIndex = i;
    }

    if (numColsSeen + 1 != numCols) {
        die("Line does not have the same number of columns as the header\n");
    }

    if(nameQuoted) {
        endNameIndex -= 1;
        startNameIndex += 1;
    }
    
    if(endNameIndex < startNameIndex) {
        die("Index of start of the name is greater than index of end of the name\n");
    }

    int length = endNameIndex - startNameIndex + 1;
    char* name = (char*)malloc(sizeof(char)*length);
    if(name == NULL) {
        die("Failed to allocate space for the name of line\n");
    }
    
    strncpy(name, &line[startNameIndex], length - 1);

    name[length - 1] = '\0'; // Append the null character to the end of the name

    return name;
}

/*
header_info readHeader(char* line, bool nameQuoted) {
    size_t numCols = 0, i = 0, nameIndex = 0;
    char c;
    char* currCol = (char*)malloc(sizeof(char));
    size_t currColI = 0, currCol_size = 1;
    bool seenNameCol = false; // Check in case there are two "name" columns

    while ((c = line[i]) != '\0') {
        if (c == ',') {
            // If you reach a comma, check if the current column is a "name" column
            currCol[currColI] = '\0';

            if (strcmp("\"name\"", currCol) == 0) {

                // If you already saw a name column, die
                if (seenNameCol) {
                    die();
                }

                nameIndex = numCols;
                seenNameCol = true;
            }

            numCols++;
            free(currCol);
            currColI = 0, currCol_size = 0;
            currCol = (char*)malloc(sizeof(char));
        } else {
            // Otherwise, keep adding characters to the current column
            currCol = (char*)realloc(currCol, (currCol_size + 1)*sizeof(char));
            currCol[currColI] = c;
            currColI++;
            currCol_size++;
        }
        
        i++;
    }

    // Checks if the last column is a "name" column
    currCol[currColI] = '\0';

    if (strcmp("\"name\"", currCol) == 0) {

        if (seenNameCol) {
            die();
        }

        nameIndex = numCols;
        seenNameCol = true;
    }

    numCols++;
    free(currCol);

    // If there are no columns in the header or the name column doesn't exist, die
    if (numCols == 0 || !seenNameCol) {
        die();
    }

    // Return a struct containing the number of columns and index of name col
    header_info info = {numCols, nameIndex};
    return info;
}
*/
