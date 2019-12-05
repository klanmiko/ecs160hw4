#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int findField(char *buf, const char *fname, const char sep) {


  int ret= -1; 
  int pos = 1; 
  const char *tok; 
  for(tok = strtok(buf,&sep);
      tok && *tok;
      tok = strtok(NULL, &sep))
    {
      //      printf("\n Next field %s at %d", tok,pos);
      if(strcmp(fname,tok) == 0)  {
	ret = pos;
	return ret;
      }
      else pos++;
    }

  return ret;
}

int findTweeter(char **table, const char *thisone, int count) {
  int pos = 0;
  while(pos < count) { 
    if(strcmp(table[pos], thisone)==0)
      return pos;
    else pos++;
  }
  return -1 ;
}


const char* getField(char* line, int num)
{
  const char* tok;
  for (tok = strtok(line, ",");
       tok && *tok;
       tok = strtok(NULL, ","))
    {
      if (!--num)
	return tok;
    }
  return NULL;
}

int main(int argc, char** argv)
{
  printf("Input file: %s\n", argv[1]);
  FILE* stream = fopen(argv[1], "r");

  char line[1024];
  char *tweeters[25000];
  int nums[25000];
  char *thisguy; 
  int thispos;
  int fpos = -1; 
  float lines = 0;
  int twCount=0;
  int toppers[25000];
  int i,big;

  /* READER HEADER */

  fgets(line, 1024, stream);
  fpos = findField(line,"\"name\"",',');
 
  while (fgets(line, 1024, stream)) {
    thisguy = strdup(getField(line,9));
    thispos = findTweeter(tweeters,thisguy,twCount);
    if(thispos <0) {
      tweeters[twCount] = thisguy;
      thispos=twCount++;
      nums[thispos] =0;
    }
    nums[thispos]=nums[thispos]+1;
  }


  for(thispos=0; thispos<twCount;thispos++){
    big=thispos;
    for(i= thispos;i<twCount;i++){
      if( nums[big] <= nums[i])
	big = i;
    }
    toppers[thispos]= big;
    nums[big] = -1;
  }
  for(i=0;(i<10 && i<twCount);i++)
    printf("\nThe top tweeters no %d is %s",i,tweeters[toppers[i]]);

}



