#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

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

typedef struct {
    char* name;
    unsigned int count;
} tweet_count;

typedef struct {
    tweet_count* tweeters;
    size_t length;
} collected_tweets;

collected_tweets collectTweets(char** rows, size_t n_rows);

#define INVALID_ERROR "Invalid Input Format"

/* Prints that there is an invalid error and exits the program
  */
void die(char* reason) {
    #ifdef DEBUG
    printf("%s", reason);
    #endif
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
      die("Could not open file.");
  }

  return filePtr;
}



#define STRIP_NEWLINE(s) { size_t l = strlen(s); \
    if(l > 0 && s[l - 1] == '\n') s[l - 1] = '\0'; \
    if(l > 1 && s[l - 2] == '\r') s[l - 2] = '\0'; }

char* readName(char* line, size_t numCols, size_t nameIndex, bool nameQuoted);
header_info readHeaderQuick(char* line);
void checkQuotation(char* line, size_t numCols, bool* columnsQuoted);

tweet_vector getTweets(FILE* fPtr) {
    // header parsing variables
    char* line = NULL;
    size_t len = 0;

    size_t numCols = 0, nameIndex = 0;
    __ssize_t nread;

    bool *columnsQuoted;

    // Read the header
    if ((nread = getline(&line, &len, fPtr)) != -1) {
        STRIP_NEWLINE(line)

        checkQuotation(line, 0, NULL);
        checkLineLength(nread);

        header_info info = readHeaderQuick(line);
        numCols = info.numCols;
        nameIndex = info.nameIndex;
        columnsQuoted = info.columnsQuoted;
    } else {
        die("Failed to read header\n");
    }

    char** tweets = (char**)malloc(sizeof(char*));
    if(tweets == NULL) die("Failed to allocate size for tweets\n");

    size_t numTweets = 0, tweets_size = 1;

    while ((nread = getline(&line, &len, fPtr)) != -1) {
        STRIP_NEWLINE(line)
        
        checkLineLength(nread);
        checkQuotation(line, numCols, columnsQuoted);

        char* name = readName(line, numCols, nameIndex, columnsQuoted[nameIndex]);

        tweets[numTweets++] = name;

        if(numTweets == tweets_size) {
            tweets = (char**)realloc(tweets, (tweets_size * 2 + 1)*sizeof(char*));
            if(tweets == NULL) die("Failed to reallocate more space for tweets\n");

            tweets_size = tweets_size * 2 + 1;
        }
    }

    if(numTweets >= 20000) die("Number of rows in the CSV exceeds 20000\n");

    free(line);
    free(columnsQuoted);

    if(numTweets == 0) {
        free(tweets);
        tweets = NULL;
    }

    tweet_vector names = {tweets, numTweets};

    return names;
}

char* findName(char* line) {
    char* name = NULL;
    while(name = strstr(line, "name")) {
        size_t start_index = name - line;
        size_t end_index = start_index + strlen("name");

        if(start_index > 0 && line[start_index - 1 ] == '"') {
            start_index--;
            end_index++;    
        }

        if((start_index == 0 || line[start_index - 1] == ',') && (line[end_index] == ',' || line[end_index] == '\0')) {
            return name;
        }

        line = &line[end_index];
    }
    
    return NULL;    
}

void checkDuplicates(char *line) {
    size_t l = strlen(line);
    char* copy = malloc(sizeof(char) * (l + 1)), *t = copy;
    strncpy(copy, line, l);
    copy[l] = '\0';

    size_t index = 0, size = 1;
    char** columnNames = malloc(sizeof(char*) * size);

    char* current = NULL;
    while((current = strsep(&copy, ","))) {
        size_t l = strlen(current);
        if(l > 1 && *current == '"') {
            current[l - 1] = '\0';
            current++;
        }

        for(size_t i = 0; i < index; i++) {
            if(strcmp(current, columnNames[i]) == 0) {
                die("Duplicate column names");
            }
        }

        columnNames[index++] = current;
        if(index == size) {
            columnNames = realloc(columnNames, sizeof(char*) * (2*size + 1));
            size = 2*size + 1;
        }
    }

    free(columnNames);
    free(t);
}

// assumes checkQuotation has run
header_info readHeaderQuick(char* line) {
    // check duplicates
    checkDuplicates(line);
    
    char* name = findName(line);
    if(name == NULL) {
        die("No name field found in header\n");
    }

    size_t columnCount = 0, nameIndex = 0, startIndex = 0;
    size_t length = strlen(line) + 1;
    size_t index = name - line;

    bool* columnsQuoted = (bool*)malloc(sizeof(bool));
    size_t b_size = 1;

    for(size_t i = 0; i < length; i++) {
        if(line[i] == ',' || line[i] == '\0') {
            if(i != 0 && line[i - 1] == '"' && line[startIndex] == '"') {
                columnsQuoted[columnCount] = true;
            } else {
                columnsQuoted[columnCount] = false;
            }

            startIndex = i + 1;
            columnCount++;

            if(columnCount == b_size && i != length - 1) {
                columnsQuoted = realloc(columnsQuoted, (2 * b_size + 1) * sizeof(bool));
                if(!columnsQuoted) die("Failed to reallocate space for boolean array columnsQuoted\n");

                b_size = 2*b_size + 1;
            }
        }
        if(i == index) {
            nameIndex = columnCount;
        }
    }
    
    header_info info = {columnCount, nameIndex, columnsQuoted};
    return info;
}


/* returns true if all values are quoted, false otherwise
    calls die() if only some are quoted */

void checkQuotation(char* line, size_t numCols, bool* columnsQuoted) {
    size_t startIndex = 0, colIdx = 0;
    char c;
    for (size_t i = 0; i < strlen(line) + 1; i++) {
        if (line[i] == ',' || line[i] == '\0') {
            size_t endIndex = i - 1;
            
            // check that the field is not ,,
            if(!(endIndex == -1 || endIndex < startIndex)) {
                // if the current field is quoted
                if (line[startIndex] == '\"' && line[endIndex] == '\"' && startIndex != endIndex) {
                    if (columnsQuoted && (colIdx == numCols || !columnsQuoted[colIdx])) {
                        die("Current field is quoted but shouldn't be\n");
                    }
                } else { // if the current field is not quoted
                    if (line[startIndex] == '\"' || line[endIndex] == '\"') {
                        die("Current field has mismatched quotation marks\n");
                    }
                    if (columnsQuoted && (colIdx == numCols || columnsQuoted[colIdx])) {
                        die("Current field isn't quoted but should be\n");
                    }
                }
            }

            startIndex = i + 1;
            colIdx++;
        }
    }
}

void inline checkLineLength(size_t len) {
    if (len > 1024) {
        die("Line is more than 1024 characters long\n");
    }
}

char* readName(char* line, size_t numCols, size_t nameIndex, bool nameQuoted) {
    size_t i = 0;
    size_t numColsSeen = 0;
    size_t startNameIndex = 0;
    size_t endNameIndex = 0;
    bool inNameCol = false;


    char c;
    while ((c = line[i]) != '\0') {
        if (numColsSeen == nameIndex && !inNameCol) {
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
        // ,a", -> INVALID
        // ,a"a, -> a"a
        // ,""a"", -> "a"
        // ,, -> name is an empty string, still counts
        i++;
    }

    if(inNameCol) {
        endNameIndex = i;
    }

    if (numColsSeen + 1 != numCols) {
        die("Line does not have the same number of columns as the header\n");
    }

    if(nameQuoted) {
        endNameIndex -= 1;
        startNameIndex += 1;
    }
    
    if(endNameIndex < startNameIndex) {
        die("Index of start of the name is greater than index of end of the name\n");
    }

    int length = endNameIndex - startNameIndex + 1;
    char* name = (char*)malloc(sizeof(char)*length);
    if(name == NULL) {
        die("Failed to allocate space for the name of line\n");
    }
    
    strncpy(name, &line[startNameIndex], length - 1);

    name[length - 1] = '\0'; // Append the null character to the end of the name

    return name;
}

/* Returns the tweet_count (which contains the name and tweet count) 
 with the greater tweet count. Used as a comparator function for qsort()
*/
int compareTweet(const void* a, const void* b) {
  const tweet_count *first = a, *second = b;
  return second->count - first->count;
}


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

int main(int argc, char* argv[]) {
    FILE* csvFile;
    // Checks for two command line arguments
    if(argc == 2) {
        csvFile = getFile(argv[1]);
    } else {
        die("Usage: main <csvFile>");
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
