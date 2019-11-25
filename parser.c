#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "parser.h"

void die();
char* readName(char* line, size_t numCols, size_t nameIndex, bool nameQuoted);
header_info readHeaderQuick(char* line, bool linesQuoted);
bool checkQuotation(char* line);

tweet_vector getTweets(FILE* fPtr) {
    // header parsing variables
    char* header = NULL;
    size_t headerLen = 0, numCols = 0, nameIndex = 0;
    __ssize_t nread;

    bool linesQuoted;

    // Read the header
    if ((nread = getline(&header, &headerLen, fPtr)) != -1) {
        size_t length = strlen(header);

        if(header[length - 1] == '\n') header[length - 1] = '\0';

        linesQuoted = checkQuotation(header);
        header_info info = readHeaderQuick(header, linesQuoted);
        numCols = info.numCols;
        nameIndex = info.nameIndex;
    } else {
        die();
    }

    char* line = NULL;
    size_t len = 0;
    size_t numTweets = 0;

    char** tweets = (char**)malloc(sizeof(char*));
    size_t index = 0, tweets_size = 1;


    while ((nread = getline(&line, &len, fPtr)) != -1) {
        size_t length = strlen(line);
        if(line[length - 1] == '\n') line[length - 1] = '\0';
        if(checkQuotation(line) != linesQuoted) {
            die();
        }

        char* name = readName(line, numCols, nameIndex, linesQuoted);

        tweets[index] = name;
        numTweets++;
        index++;

        if(index == tweets_size) {
            tweets = (char**)realloc(tweets, (tweets_size * 2 + 1)*sizeof(char*));
            tweets_size = tweets_size * 2 + 1;
        }
    }

    free(line);
    free(header);

    tweet_vector names = {tweets, numTweets};

    return names;
}

char* findName(char* line, bool nameQuoted) {
    char* name = NULL;
    while(name = strstr(line, "name")) {
        size_t start_index = name - line;
        size_t end_index = start_index + strlen("name");

        if(nameQuoted) {
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

header_info readHeaderQuick(char* line, bool nameQuoted) {
    char* name = findName(line, nameQuoted);
    if(name == NULL) {
        die();
    }
    // check there isn't another name column
    if(findName(name + strlen("name"), nameQuoted) != NULL) {
        die();
    }

    size_t columnCount = 0, nameIndex = 0;
    size_t length = strlen(line);
    size_t index = name - line;

    for(size_t i = 0; i < length; i++) {
        if(line[i] == ',') {
            columnCount++;
        }
        if(i == index) {
            nameIndex = columnCount;
        }
    }
    
    header_info info = {columnCount, nameIndex};
    return info;
}


/* returns true if all values are quoted, false otherwise
    calls die() if only some are quoted */

bool checkQuotation(char* line) {
    bool isQuoted = false;
    bool readNewField = true;
    size_t startIndex = 0, endIndex;
    bool isFirstFieldQuoted = false;
    char c;
    for (size_t i = 0; i < strlen(line) + 1; i++) {
        if (line[i] == ',' || line[i] == '\0') {
            endIndex = i - 1;
            // if the current field is quoted
            if (line[startIndex] == '\"' && line[endIndex] == '\"') {
                if (startIndex == 0) {
                    isQuoted = true;
                }
                if (!isQuoted) {
                    die();
                }
            } else { // if the current field is not quoted
                if (line[startIndex] == '\"' || line[endIndex] == '\"') {
                    die();
                }
                if (startIndex == 0) {
                    isQuoted = false;
                }
                if (isQuoted) {
                    die();
                }
            }

            startIndex = i + 1;
        }
    }

    
}

char* readName(char* line, size_t numCols, size_t nameIndex, bool nameQuoted) {
    size_t i = 0;
    char c;
    size_t numColsSeen = 0;
    size_t startNameIndex = 0;
    size_t endNameIndex = 0;
    bool inNameCol = false;

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

    if (numColsSeen != numCols) {
        die();
    }

    if(nameQuoted) {
        endNameIndex -= 1;
        startNameIndex += 1;
    }
    
    if(endNameIndex < startNameIndex) {
        die();
    }

    int length = endNameIndex - startNameIndex + 1;
    char* name = (char*)malloc(sizeof(char)*length);
    
    strncpy(name, &line[startNameIndex], length - 1);

    name[length - 1] = '\0'; // Append the null character to the end of the name

    return name;
}

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