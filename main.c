#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "collect.h"

#define INVALID_ERROR "Invalid Input Format"

void die() {
    printf(INVALID_ERROR);
    exit(EXIT_FAILURE);
}

/* getFile opens the filePath as a file pointer
  if the file cannot be opened, this function will exit the program
  the return value cannot be null
*/

FILE* getFile(char* filePath) {
  FILE* filePtr = fopen(filePath, "r");

  if (filePtr == NULL) {
      die();
  }

  return filePtr;
}

int compareTweet(const void* a, const void* b) {
  const tweet_count *first = a, *second = b;
  return first->count - second->count;
}

int main(int argc, char* argv[]) {
    FILE* csvFile;
    if(argc == 2) {
        csvFile = getFile(argv[1]);
    } else {
        die();
    }

    tweet_vector rows = getTweets(csvFile);
    collected_tweets tweets = collectTweets(rows.tweets, rows.length);
    
    qsort(tweets.tweeters, tweets.length, sizeof(tweet_count), compareTweet);

    for(size_t i = 0; i < tweets.length; i++) {
      tweet_count t = tweets.tweeters[i];
      printf("%s, %s\n", t.name, t.count);
    }

    return 0;
}
