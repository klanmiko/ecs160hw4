#ifndef PARSER_H
#define PARSER_H

typedef struct {
    char** tweets;
    size_t length;
} tweet_vector;

typedef struct {
    size_t numCols;
    size_t nameIndex;
} header_info;

tweet_vector getTweets(FILE* fPtr);
char* readName(char* line, size_t numCols, size_t nameIndex, bool nameQuoted);
header_info readHeaderQuick(char* line, bool linesQuoted);
bool checkQuotation(char* line);

#endif