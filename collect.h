#ifndef COLLECT_H
#define COLLECT_H

typedef struct {
    char* name;
    unsigned int count;
} tweet_count;

typedef struct {
    tweet_count* tweeters;
    size_t length;
} collected_tweets;

collected_tweets* collectTweets(char** rows, size_t n_rows);

#endif