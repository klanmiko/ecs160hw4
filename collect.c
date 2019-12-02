#include "collect.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "util.h"

int name_sort(const void *a, const void *b)
{
    return strcmp(*(const char **)a, *(const char **)b);
}

collected_tweets collectTweets(char **names, size_t n_rows)
{
    // no tweeters
    if (n_rows == 0)
    {
        return (collected_tweets){NULL, 0};
    }

    // the tweet at this index in tweeters is uninitialized
    size_t index = 0, t_size = 1;
    tweet_count *tweeters = (tweet_count *)malloc(sizeof(tweet_count));
    if(tweeters == NULL) {
        die("Malloc failed to allocate tweeters");
    }

    qsort(names, n_rows, sizeof(char *), name_sort);
    char *prev_name = NULL;
    size_t prev_count = 1;

    // iterate over the rows
    for (size_t i = 0; i < n_rows; i++)
    {
        char *name = names[i];
        // if a new name is seen, record count to array
        if (prev_name != NULL)
        {
            if (strcmp(name, prev_name) != 0)
            {
                tweet_count *t = &(tweeters[index++]);
                t->count = prev_count;

                size_t l = strlen(prev_name) + 1;
                t->name = (char*)malloc(sizeof(char) * l);

                if(t->name == NULL) die("Failed to allocate space for name of tweet\n");

                strncpy(t->name, prev_name, l);

                // if the next free index = the size, resize the array to 2n + 1
                if (index == t_size)
                {
                    tweeters = (tweet_count *)realloc(tweeters, (t_size * 2 + 1) * sizeof(tweet_count));
                    t_size = t_size * 2 + 1;

                    if (tweeters == NULL)
                    {
                        die("Failed to reallocate space for tweeters\n");
                    }
                }

                prev_count = 1;
            }
            else if (strcmp(name, prev_name) == 0)
            {
                prev_count++;
            }
        }

        prev_name = name;
    }

    tweet_count *t = &(tweeters[index++]);
    t->count = prev_count;

    size_t l = strlen(prev_name) + 1;
    t->name = (char*)malloc(sizeof(char) * l);
    
    if(t->name == NULL) die("Failed to allocate space for name of tweet\n");

    strncpy(t->name, prev_name, l);

    return (collected_tweets){tweeters, index};
}
