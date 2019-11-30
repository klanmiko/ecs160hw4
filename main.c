#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "collect.h"

#define INVALID_ERROR "Invalid Input Format"

/* Prints that there is an invalid error and exits the program
  */
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

/* Returns the tweet_count (which contains the name and tweet count) 
 with the greater tweet count. Used as a comparator function for qsort()
*/
int compareTweet(const void* a, const void* b) {
  const tweet_count *first = a, *second = b;
  return second->count - first->count;
}

int main(int argc, char* argv[]) {
    FILE* csvFile;
    // Checks for two command line arguments
    if(argc == 2) {
        csvFile = getFile(argv[1]);
    } else {
        die();
    }

    // Creates a list of tweets containing rows of tweets
    tweet_vector rows = getTweets(csvFile);
    collected_tweets tweets = collectTweets(rows.tweets, rows.length);

    // Free the space allocated for rows of tweets
    for(size_t i = 0; i < rows.length; i++) {
      free(rows.tweets[i]);
    }
    free(rows.tweets);
    
    // Sorts tweeters according to their tweet count
    qsort(tweets.tweeters, tweets.length, sizeof(tweet_count), compareTweet);

    int top = tweets.length < 10 ? tweets.length : 10;

    // Print out the top tweeters and their tweet counts
    for(size_t i = 0; i < top; i++) {
      tweet_count t = tweets.tweeters[i];
      printf("%s: %d\n", t.name, t.count);
    }

    for(size_t i = 0; i < tweets.length; i++) {
      free(tweets.tweeters[i].name);
    }

    // Free the tweeters and close file
    if(tweets.tweeters != NULL) free(tweets.tweeters);
    fclose(csvFile);

    return 0;
}
