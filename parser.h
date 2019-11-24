#ifndef PARSER_H
#define PARSER_H

typedef struct {
    char** tweets;
    size_t length;
} tweet_vector;

tweet_vector getTweets(FILE* fPtr);

#endif