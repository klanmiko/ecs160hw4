/*
 * Authors:
 * Raisa Gandi Putri
 * Atharva Chalke
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <regex.h>
#include <ctype.h>
#include <unistd.h>

#define MAXCHARS (1024)
#define MAXLENFILE (20000)

/**
 * - - - - - - - - - - - 
 * | DOUBLY LINKED LIST |
 * | DATA STRUCTURE     |
 * | FOR TWEETER DATA   |
 * - - - - - - - - - - --
 */


/* Tweeter, a linked list node */
typedef struct Tweeter
{
    char* name;
    int tweetCount;
    struct Tweeter* prev;
    struct Tweeter* next;
} Tweeter;


/* The linked list */
typedef struct TweeterList
{
    int length;
    Tweeter* front;
    Tweeter* rear;
} TweeterList;


/* Initialize a new tweeter */
Tweeter* createTweeter(char* tweeterName)
{
    Tweeter* newTweeter = (Tweeter*)malloc(sizeof(Tweeter));
    newTweeter->name = tweeterName;
    newTweeter->tweetCount = 1;
    newTweeter->prev = NULL;
    newTweeter->next = NULL;
    return newTweeter;
} // createTweeter()


/* Initialize an empty list */
TweeterList* createTweeterList()
{
    TweeterList* newList = (TweeterList*)malloc(sizeof(TweeterList));
    newList->length = 0;
    newList->front = NULL;
    newList->rear = NULL;
    return newList;
} // createTweeterList()


/* Destructor for tweeter list */
int destroyTweeterList(TweeterList* tweeterList)
{
    if(!tweeterList || tweeterList->length == 0)
        exit(1);
    
    // Deallocate the tweeters
    Tweeter* head = tweeterList->front;
    Tweeter* ptr = head;
    for( ; ptr != NULL; ptr = head)
    {
        head = ptr->next;

	free(ptr->name);
	free(ptr);
    }
    
    // Deallocate tweeter list
    free(tweeterList);
    tweeterList = NULL;
    return 0;
} // destroyTweeterList()


/*
 * How swapping works:
 *
 * From this,
 * - - - - - - -      - - - - - - -       - - - - - -       - - - - - -
 * | leftNode  |  --> | PRIOR     | -->  | CURRENT   | --> | rightNode |
 * | 20 tweets | <--  | 13 tweets | <--  | 15 tweets | <-- | 5 tweets  |
 * - - - - - - -      - - - - - - -       - - - - - -       - - - - - -
 *
 * We want this:
 * - - - - - - -      - - - - - - -       - - - - - -       - - - - - -
 * | leftNode  |  --> | CURRENT   | -->  | PRIOR     | --> | rightNode |
 * | 20 tweets | <--  | 15 tweets | <--  | 13 tweets | <-- | 5 tweets  |
 * - - - - - - -      - - - - - - -       - - - - - -       - - - - - -
 * 
 * While keeping in mind that leftNode and rightNode can be NULL
 */ 
void swapTweeters(TweeterList* tweeterList, Tweeter* current, Tweeter* prior, 
		  Tweeter* leftNode, Tweeter* rightNode)
{
    current->next = prior;
    current->prev = leftNode;

    prior->next = rightNode;
    prior->prev = current;

    // Change front and rear references if necessary
    if(leftNode != NULL)
        leftNode->next = current;
    else
	tweeterList->front = current;

    if(rightNode != NULL)
	rightNode->prev = prior;
    else
	tweeterList->rear = prior;
    
} // swapTweeters()


/*
 * Move tweeter to correct position after being incremented
 *    it's tweetCount should be <= the previous tweeter's count
 *
 * Such that the list stays in descending order of tweet count
 *      from front to rear.
 */
void moveTweeter(TweeterList* tweeterList, Tweeter* current)
{
    Tweeter* prior = current->prev;
    Tweeter* leftNode = NULL;
    Tweeter* rightNode = NULL;

    // Current is at front of list, then no need to move
    if(prior == NULL)
    {
        return;
    }

    while(current != NULL && prior != NULL &&
		    current->tweetCount > prior->tweetCount) 
    {
        if(prior->prev != NULL) // Setting left node
            leftNode = prior->prev;
	else
	    leftNode = NULL;

        if(current->next != NULL) // Setting right node
            rightNode = current->next;
	else
	    rightNode = NULL;

        // Swapping current and prior tweeters
        swapTweeters(tweeterList, current, prior, leftNode, rightNode);	

        prior = current->prev; // Reset prior
    }

} // moveTweeter()


/*
 * Inserting tweeter
 *
 * The doubly linked list is filled in descending order of tweetcount
 *     from the front. i.e. the front tweter has the most tweets
 * 
 * If it's a new tweeter, insert at rear of list
 * Else, we increment the tweetcount for that tweeter
 *       and then move it to the correct position in the list
 *
 * A small example:
 *
 * <front>                   <rear>
 * - - - - - - -      - - - - - - -
 * | John      |  --> | Jesslyn   |
 * | 15 tweets | <--  | 13 tweets |
 * - - - - - - -      - - - - - - -
 */
void insertTweeter(TweeterList* tweeterList, char* tweeterName)
{

    // The first tweeter encountered
    if(tweeterList->length == 0)
    {
	Tweeter* newTweeter = createTweeter(tweeterName);
	tweeterList->front = newTweeter;
	tweeterList->rear = newTweeter;
        tweeterList->length += 1;
	return;
    }
    
    bool foundTweeter = false;
    Tweeter* current = tweeterList->front;
    for( ; current != NULL; current = current->next)
    {
        if(strcmp(current->name, tweeterName) == 0) // Found!
	{
            foundTweeter = true;
	    current->tweetCount += 1;
            moveTweeter(tweeterList, current);
	    free(tweeterName); // We already allocated memory for this name
	    return;
	}	
    }

    if(!foundTweeter) // Tweeter is new to our list
    {
        Tweeter* newTweeter = createTweeter(tweeterName);
	tweeterList->rear->next = newTweeter;
	newTweeter->prev = tweeterList->rear;
	tweeterList->rear = newTweeter;
	tweeterList->length += 1;
    }
   
} // insertTweeter()


/*
 * Print up to the top ten tweeters
 *    based on their tweet counts
 * 
 * Print format:
 *    <tweeter>: <tweet count>
 */
void printTopTenTweeters(TweeterList* tweeterList)
{
    Tweeter* current = tweeterList->front;
    int count = 1;

    for( ; current != NULL && count <= 10; current = current->next, count++)
    {
        printf("%s: %d\n", current->name, current->tweetCount);
    }

} // printTopTenTweeters()


/*
 * Debugging print list
 */
void printList(TweeterList* list)
{
    printf("Printing the list of tweeters...\n");

    if(!list || list->length == 0)
    {
        printf("List is empty\n");
        return;
    }

    Tweeter* current = list->front;
    for( ; current != NULL; current = current->next)
    {
        printf("Name: %s, tweet count: %d\n", current->name, current->tweetCount);
    }

} // printList()

/**
 * _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ 
 * | These are method definitions for  |
 * | ERROR CHECKING FOR FILE VALIDITY  |
 **/ 


/**
 * Returns true if file doesn't exist
 */
bool fileNotExist(char* fileName)
{
    if(access(fileName, F_OK) != -1)
   	return false; // File exists
    return true;
} // fileNotExist()

/**
 * Returns true if filename is null
 */
bool fileIsNull(char* fileName)
{
    if (fileName == NULL)
        return true;
    return false;
} // fileIsNull()


/**
 * Returns true if file is empty
 */
bool fileIsEmpty(char* fileName)
{
    FILE* fp = fopen(fileName, "r");
    bool fileEmpty = false;

    if(fp != NULL)
    {
        fseek (fp, 0, SEEK_END);
	long size = ftell(fp);
	if(size == 0)
	    fileEmpty = true;
    }

    fclose(fp);
    return fileEmpty;
} // fileIsEmpty()


/**
 * Returns true if file does not end with *.csv
 */
bool fileIsNotCSV(char* fileName)
{
    regex_t regex;
    int retVal = regcomp(&regex,"[*.csv]",0);
    if (retVal == 0)
    {
        retVal = regexec(&regex, fileName, 0, NULL, 0);
        if(retVal != 0) // Filename did not match our regex
        {
            regfree(&regex); // Need to free regex
	    return true;
	}
    } 
    regfree(&regex);
    return false;    
} // fileIsNotCSV()

/*
 * Returns true if file has
 * - too many chars per line OR
 * - too many lines
 */
bool fileHasTooManyCharsOrLines(char* fileName)
{
    // Check if we exceed the max chars per line
    FILE* fp = fopen(fileName, "r");
    int rowCount = 0;
    int bufferSize = 1000000; // A large number
    char line[bufferSize];

    while (fgets(line, bufferSize, fp))
    {
        rowCount += 1;
        if (strlen(line) > MAXCHARS)
            return true;
    }

    // Check if we exceed the max num of lines in file
    if(rowCount > MAXLENFILE)
	return true;

    return false;
} // fileHasTooManyCharsOrLines()


/* Returns true if file has empty header */
bool fileHasEmptyHeader(char* header)
{
    bool emptyHeader = true;
   
    for(int pos = 0; pos < strlen(header); pos++)
    {
        if(isspace(header[pos]))
	    continue;
	else
	    emptyHeader = false;
    }
    
    return emptyHeader;

} // fileHasEmptyHeader()


/* Counts the number of tokens in the header */
int countHeaderTokens(char* header)
{
    int tokenCount = 0;
    char* token = NULL;

    while ((token = strsep(&header, ",")) != NULL)
        tokenCount += 1;
    
    return tokenCount;
} // countHeaderTokens()



/** --- LINKED LIST OF HEADER TOKENS --- **/

/* Header token, node in linked list */
typedef struct HeaderToken
{
    char* headername;
    struct HeaderToken* next;
} HeaderToken;


/* Linked list of header names */
typedef struct HeaderList
{
    int length;
    HeaderToken* front;
    HeaderToken* current;
} HeaderList;


/* Create header token */
HeaderToken* createHeaderToken(char* token)
{
    HeaderToken* newToken = (HeaderToken*)malloc(sizeof(HeaderToken));
    newToken->headername = strdup(token);
    newToken->next = NULL;
    return newToken;
} // createHeaderToken()


/* Create header list */
HeaderList* createHeaderList()
{
    HeaderList* headerList = (HeaderList*)malloc(sizeof(HeaderList));
    headerList->length = 0;
    headerList->front = NULL;
    headerList->current = NULL;
    return headerList;
} // createHeaderList()

/* Destructor header list */
int destroyHeaderList(HeaderList* headerList)
{
    if(!headerList || headerList->length == 0)
        exit(1);
    
    // Deallocate the headernames
    HeaderToken* head = headerList->front;
    HeaderToken* ptr = head;
    for( ; ptr != NULL; ptr = head)
    {
        head = ptr->next;
	free(ptr->headername);
	free(ptr);
    }
    
    // Deallocate header list
    free(headerList);
    headerList = NULL;
    return 0;   
} // destroyHeaderList()

/* --- END OF HEADER LINKED LIST --- */


/* Returns true if token is only one quote */
bool checkOnlyQuote(char* token)
{
    if(strlen(token) == 1 && token[0] == '\"')
        return true;
   
    return false;
} // checkOnlyQuote


/* Returns true if a token has a single outer quote */
bool checkSingleOuterQuote(char* token)
{
    bool hasSingleOuterQuote = false;

    // Surrounding quote only at beginning or end of string
    if( (token[0] == '\"' && token[strlen(token)-1] != '\"') ||
        (token[0] != '\"' && token[strlen(token)-1] == '\"') )
    {
        hasSingleOuterQuote = true;
    }
    
    return hasSingleOuterQuote;
} // checkSingleOuterQuote()


/* Returns true if the linked list of headers
 * contains duplicate tokens */
bool checkDuplicateTokens(HeaderList* headerList, char* token)
{
    bool hasDuplicateTokens = false;

    for(HeaderToken* ptr = headerList->front; ptr != NULL; ptr = ptr->next)
    {
        if(strcmp(token, ptr->headername) == 0)
	{
	    hasDuplicateTokens = true;
            break;
	}
    }
    return hasDuplicateTokens;
} // checkDuplicateTokens()


/*
 * Fills a linked list of header tokens
 * Returns false if header is invalid:
 * 1) has duplicate tokens
 * 2) has only one surrounding quote on the token (e.g. blah")
 * 3) has no name token in the header
 */
bool checkHeaderValidity(HeaderList* headerList, char* header, int tokenCount)
{
    char* token = NULL;
    bool hasNameToken = false, hasDuplicateTokens = false;
    bool onlyQuote = false, hasSingleOuterQuote = false;

    while ((token = strsep(&header, ",")) != NULL)
    {
	// Remove newline from token - strtok replaces newline with \0
	// https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input
	if(token[strlen(token)-1] == '\n')
	    strtok(token, "\n");

	// Check for name token
        if(strcmp(token, "name") == 0 || strcmp(token, "\"name\"") == 0)
            hasNameToken = true;

	// Check if header only contains one quote
	onlyQuote = checkOnlyQuote(token);
	// Check single outer quote 
        hasSingleOuterQuote = checkSingleOuterQuote(token); 
        // Check duplicate tokens	
	hasDuplicateTokens = checkDuplicateTokens(headerList, token);
	
	if(onlyQuote || hasDuplicateTokens || hasSingleOuterQuote)
            break;

	HeaderToken* newToken = createHeaderToken(token);
        if(headerList->length == 0) // Empty list
        {
	    headerList->front = newToken;
	    headerList->current = newToken;
            headerList->length += 1;
            continue;
	}
        // Add node to existing list
        headerList->current->next = newToken;
	headerList->current = newToken;
	headerList->length += 1;
    }
    
    if(onlyQuote || !hasNameToken || hasDuplicateTokens || hasSingleOuterQuote)
        return false;

    return true; // Valid header
} // checkHeaderValidity()


/*
 * Fill array of booleans for each header token
 * T if header token surrounded by quotes
 * F otherwise
 */
void fillBoolArr(bool* headerBoolArr, HeaderList* headerList)
{
    int pos = 0;
    HeaderToken* token = headerList->front;
    for( ; token != NULL; token = token->next, pos++)
    {
	char* headername = token->headername;
        if(headername[0] == '\"' 
	       && headername[strlen(headername)-1] == '\"')
	    headerBoolArr[pos] = true;
	else
	    headerBoolArr[pos] = false;
    }
} // fillBoolArr()


/*
 * Returns true if line contains all spaces
 * It can also end with a newline
 */
bool entireLineIsAllSpace(char* line)
{
    bool lineIsAllSpace = true;

    if(line[strlen(line) - 1] == '\n' ||
       isspace(line[strlen(line) - 1]))
       {
           for(int pos = 0; pos < strlen(line) - 1; pos++)
	   {
               if(!isspace(line[pos]))
	       {
	           lineIsAllSpace = false;
		   break;
	       }	       
	   }
       }
       return lineIsAllSpace;
} // entireLineIsAllSpace()


/* 
 * Check if text below header is valid
 *  - text (per line) has the same number of fields as the header
 * A column is valid if:
 * - it has the same format as the header (either surrounded by quotes or no quotes)
 * 
 * Args:
 *   fp = file
 *   headerBoolArr = for each header token, T if surrounded by quotes, else F
 *   headerTokenCount = number of header tokens
 */
bool checkColumnValidity(FILE* fp, bool* headerBoolArr, int headerTokenCount)
{
    char line[MAXCHARS];
    bool numTokensNotEqualHeader = false, tokenIsQuote = false,
             onlyOneSurroundingQuote = false, textAndHeaderNotMatchQuotes = false;
    char* lineCopy = NULL;
    char *lineCopyPtr = NULL;

    while(fgets(line, MAXCHARS, fp) != NULL)
    {
	lineCopy = strdup(line);
	lineCopyPtr = lineCopy;
        char* token = NULL;
	int pos = 0;

	// Line is empty or only contains newline
	if(line[0] == '\0' || line[0] == '\n')
	{
	    continue;
        }

        // Check if entire line is all space
	if(entireLineIsAllSpace(lineCopy))
	    continue;

	while((token = strsep(&lineCopy, ",")) != NULL)            
        {
            if(pos == headerTokenCount)
            {
		numTokensNotEqualHeader = true;
		break;
	    } // Number of tokens in text > num tokens in header

	    if(checkOnlyQuote(token))
	    {
		tokenIsQuote = true;
	    	break;
	    } // Check if token is the literal "

	    if(checkSingleOuterQuote(token))
	    {
		onlyOneSurroundingQuote = true;
		break;
            } // Check if text has only 1 surrounding quote	    

	    bool hasQuotes = false;
	    if(token[0] == '\"' && token[strlen(token-1)] == '\"')
	        hasQuotes = true;

	    if((hasQuotes == 1 && headerBoolArr[pos] == 1) || 
	        (hasQuotes == 0 && headerBoolArr[pos] == 0))
            {
                pos += 1;		    
		continue;
	    } // Text and header match (surrounding quotes)
            else
	    {
	        textAndHeaderNotMatchQuotes = true;
		break;
	    } // The text and header don't match (surrounding quotes) 
	} 

	if(pos != headerTokenCount)
	{
	    numTokensNotEqualHeader = true;
	    break;
        } // Text has fewer columns than header
    } // Go through each line of the text

    free(lineCopyPtr);

    if(numTokensNotEqualHeader || tokenIsQuote 
           || onlyOneSurroundingQuote || textAndHeaderNotMatchQuotes)
    {
	return false;
    }
    return true; // Text below header is formatted OK
} // checkColumnValidity()


/* Destroys header linked list and frees the space for the line */
void cleanUp(HeaderList* headerList, char* lineCopyPtr, FILE* fp)
{
    destroyHeaderList(headerList);
    free(lineCopyPtr);
    fclose(fp);
} // cleanUp()


/*
 * Return true if 
 * 1) file has an invalid header
 * 2) file has an invalid text (below the header)
 */
bool invalidFileContents(char* fileName)
{
    FILE* fp = fopen(fileName, "r");
    char line[MAXCHARS];
    fgets(line, MAXCHARS, fp);
    
    if(fileHasEmptyHeader(line))
    {
	fclose(fp);
        return true;
    }

    char* lineCopy = strdup(line);
    // Store a ptr to the line because line will be modified by strsep() 
    // we will free() this ptr later
    char* lineCopyPtr = lineCopy;
    int headerTokenCount = countHeaderTokens(lineCopy);
    lineCopy = strdup(line); // Need to recopy the string after being parsed

    HeaderList* headerList = createHeaderList();
    bool headerValid = checkHeaderValidity(headerList, lineCopy, headerTokenCount);

    if(!headerValid)
    {
	cleanUp(headerList, lineCopyPtr, fp);
	return true;
    }

    // Create an array of bool for headers - T if surrounded by quotes
    bool* headerBoolArr = (bool*)malloc(sizeof(bool) * headerTokenCount);
    fillBoolArr(headerBoolArr, headerList);
    bool isColumnValid = checkColumnValidity(fp, headerBoolArr, headerTokenCount);
   
    cleanUp(headerList, lineCopyPtr, fp);

    if(!isColumnValid)
    {
	return true;
    }
    return false; // Header is valid    
} // invalidHeader()


/*
 * fileCheck: Checks if the file has a valid input format
 * Args:
 *   - fileName: Name of the file
 * Returns:
 *   - a boolean indicator of whether a file is valid or not. 
 */
bool fileCheck(char* fileName)
{
    if( fileNotExist(fileName)
        || fileIsNull(fileName) 
        || fileIsEmpty(fileName)
        || fileIsNotCSV(fileName) 
        || fileHasTooManyCharsOrLines(fileName)
        || invalidFileContents(fileName) )
    {
        printf("Invalid Input Format\n");
	exit(0);
    }

    return true;
} // fileCheck()



/**
 * - - - - - - - - - - - -
 * | PROCESS THE CSV FILE |
 * | AND TWEETER DATA     |
 */

/*
 * Returns the position of the name column
 * name header can be either the strings:
 * 1. name
 * 2. "name"
 */
int getPosNameColumn(FILE* fp)
{
    int posNameColumn = 0;
    int indexInLine = 1; // Indexing starts at 1
    char line[MAXCHARS];
    char nameHeaderNoQuotes[] = "name";
    char nameHeaderWithQuotes[] = "\"name\"";
    char* token = NULL;

    // Read the file, parse by comma as delimiter
    fgets(line, MAXCHARS, fp);
    char* lineCopy = strdup(line);

    // strsep() will modify lineCopy, we need to save a ptr to it
    // so we can free it later
    char* lineCopyPtr = lineCopy;
    token = strsep(&lineCopy, ",");

    // Find the name column
    while ((token = strsep(&lineCopy, ",")) != NULL)
    {
        if(strcmp(nameHeaderNoQuotes, token) == 0 ||
	   strcmp(nameHeaderWithQuotes, token) == 0)
        {
            posNameColumn = indexInLine;
            break;	    
	}
	indexInLine += 1;
    }
 
    free(lineCopyPtr); 
    lineCopyPtr = NULL;
    fclose(fp); 
    return posNameColumn;

} // getPosNameColumn()


/**
 * Prints (up to) the top ten tweeters & their tweet counts
 * from a given CSV file
 */ 
void processTweeterData(char* fileName, int posNameColumn)
{
    int indexInFile = 0, indexInLine = 0;
    char line[MAXCHARS];
    char* lineCopy = NULL;
    char* lineCopyPtr = NULL;
    char* token = NULL;
    char* name = NULL;
    FILE* fp = fopen(fileName, "r");
    fgets(line, MAXCHARS, fp);
    TweeterList* tweeterList = createTweeterList();

    while (fgets(line, MAXCHARS, fp))
    {
        lineCopy = strdup(line);
	// strsep() will modify lineCopy, need to store ptr to it 
	// so we can free it later
	lineCopyPtr = lineCopy;
	indexInLine = 0;

	// Parse tokens with comma as delimiter
	while( (token = strsep(&lineCopy, ",")) != NULL)
        {
	    if(indexInLine == posNameColumn) // Name column
	    {
	       if(token[0] == '\"' && token[strlen(token)-1] == '\"')
	       {
	            // Remove quotes
		    int nameLength = strlen(token) - 2; // -2 to remove quotes
		    int newNameLength = nameLength + 1; // +1 for null termntr

		    // Get the name from in-between the quotes
		    name = (char*) malloc(newNameLength * sizeof(char));
		    memcpy(name, &token[1], nameLength); 
		    name[newNameLength] = '\0';
	       }
	       else // No quotes in name
               {
	           int size = strlen(token) + 1;
		   name = (char*) malloc(sizeof(char) * size);
		   strncpy(name, token, size);
		   name[size-1] = '\0';
	       }

	       insertTweeter(tweeterList, name);
	    }
	    indexInLine += 1;
	}
        indexInFile += 1;
	free(lineCopyPtr);
    }

    printTopTenTweeters(tweeterList);
    destroyTweeterList(tweeterList);
    
    fclose(fp);
} // processTweeterData()


void readFile(char* fileName)
{
    int posNameColumn = 0;
    FILE* fp = fopen(fileName, "r");

    posNameColumn = getPosNameColumn(fp);
    processTweeterData(fileName, posNameColumn);

} // readFile()



/**
  *- - - - - - - - - - - - 
 * | PROGRAM STARTS HERE  |
 * */
int main(int argc, char* argv[])
{
    bool fileIsValid = false;
    fileIsValid = fileCheck(argv[1]);

    if (fileIsValid) // File is valid THUS FAR
        readFile(argv[1]);

} // main()
