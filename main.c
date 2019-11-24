#include <stdio.h>
#include <stdlib.h>

#define INVALID_ERROR "Invalid Input Format"

void die() {
    printf(INVALID_ERROR);
    exit(EXIT_FAILURE);
}

FILE* getFile(char* filePath) {
  FILE* filePtr = fopen(filePath, "r");

  if (filePtr == NULL) {
    die();
  } 

  return filePtr;
}

int main(int argc, char* argv[]) {
    if(argc == 2) {
        FILE* csvFile = getFile(argv[1]);
        
    } else {
        die();
    }
    return 0;
}
