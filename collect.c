#include "collect.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void die();

int name_sort(const void *a, const void *b)
{
    return strcmp(*(const char **)a, *(const char **)b);
}

collected_tweets collectTweets(char **names, size_t n_rows)
{

    // the tweet at this index in tweeters is uninitialized
    size_t index = 0, t_size = 1;
    tweet_count *tweeters = (tweet_count *)malloc(sizeof(tweet_count));

    if (n_rows == 0 && tweeters == NULL)
    {
        die();
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

                if(t->name == NULL) die();

                strncpy(t->name, prev_name, l);

                // if the next free index = the size, resize the array to 2n + 1
                if (index == t_size)
                {
                    tweeters = (tweet_count *)realloc(tweeters, (t_size * 2 + 1) * sizeof(tweet_count));
                    t_size = t_size * 2 + 1;

                    if (tweeters == NULL)
                    {
                        die();
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
    
    if(t->name == NULL) die();

    strncpy(t->name, prev_name, l);

    collected_tweets returnval = {tweeters, index};

    // If there are more 20,000 lines in the CSV, die
    if(index>19999) die();

    return returnval;
}