#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "parser.h"

void die();
char* readName(char* line, size_t numCols, size_t nameIndex);
header_info readHeader(char* line);

tweet_vector getTweets(FILE* fPtr) {
    char* header = NULL;
    size_t headerLen = 0, numCols = 0, nameIndex = 0;
    size_t nread;
    char* line = NULL;
    size_t len = 0;
    size_t numTweets = 0;

    char** tweets = (char**)malloc(sizeof(char*));
    size_t index = 0, tweets_size = 1;
    
    // Read the header
    if ((nread = getline(&header, &headerLen, fPtr)) > 0) {
        header_info info = readHeader(header);
        numCols = info.numCols;
        nameIndex = info.nameIndex;
    } else {
        die();
    }

    while ((nread = getline(&line, &len, fPtr)) > 0) {
        char* name = readName(line, numCols, nameIndex);

        // if name == NULL, do something
        if (!name) {
            die(); // I don't think name should ever be NULL
        }

        tweets[index] = name;
        numTweets++;
        index++;

        if(index == tweets_size) {
            tweets = (char**)realloc(tweets, (tweets_size * 2 + 1)*sizeof(char*));
            tweets_size = tweets_size * 2 + 1;
        }

    }

    tweet_vector names = {tweets, numTweets};

    return names;
}

header_info readHeader(char* line) {
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

char* readName(char* line, size_t numCols, size_t nameIndex) {
    size_t i = 0;
    char c;
    size_t numColsSeen = 0;
    size_t startNameIndex = 0;
    size_t endNameIndex = 0;
    bool inNameCol = false;

    while ((c = line[i]) != '\0') {
        if (numColsSeen == nameIndex) {
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
        // ,a", -> a"
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

    int length = endNameIndex - startNameIndex + 1;
    char* name = (char*)malloc(sizeof(char)*length);
    
    if(strncpy(name, &line[startNameIndex], length) != length) {
        fprintf(stderr, 
            "null character in middle of name column: %s, line: %s, srcline: %s", 
            name, line, __LINE__);
    }

    name[length - 1] = '\0'; // Append the null character to the end of the name

    return name;
}
