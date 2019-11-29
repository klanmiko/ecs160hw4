#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>

typedef struct {
    char** tweets;
    size_t length;
} tweet_vector;

typedef struct {
    size_t numCols;
    size_t nameIndex;
    bool* columnsQuoted;
} header_info;

tweet_vector getTweets(FILE* fPtr);

void checkLineLength(size_t len);
bool* getQuotation(char* line);

#endif