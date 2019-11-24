#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "parser.h"

void die();
char* readName(char* line, size_t numCols, size_t nameIndex);

tweet_vector getTweets(FILE* fPtr) {
    char* header = NULL;
    size_t headerLen = 0;
    size_t numCols = 0;
    size_t nameIndex = 0;
    size_t nread;
    char* line = NULL;
    size_t len = 0;
    size_t numTweets = 0;
    char** tweets = NULL;
    
    // Read the header
    if ((nread = getline(&header, &headerLen, fPtr)) > 0) {
    }

    while ((nread = getline(&line, &len, fPtr)) > 0) {
        char* name = readName(line, numCols, nameIndex);
    }


    tweet_vector names = {tweets, numTweets};

    return names;
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
        i++;
    }

    if(inNameCol) {
        endNameIndex = i;
    }

    if (numColsSeen != numCols) {
        die();
    }

    int length = endNameIndex - startNameIndex;
    char* name = (char*)malloc(sizeof(char)*length);
    
    if(strncpy(name, &line[startNameIndex], length) != length) {
        fprintf(stderr, 
            "null character in middle of name column: %s, line: %s, srcline: %s", 
            name, line, __LINE__);
    }

    

    return name;
}
