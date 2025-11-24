/* reader.c */
#include <stdio.h>
#include "reader.h"

FILE *inputStream;
int lineNo, colNo;
int currentChar;

void readChar(void) {
  currentChar = getc(inputStream);
  colNo++;
  if (currentChar == '\n') {
    lineNo++;
    colNo = 0;
  }
}

int openInputStream(char *fileName) {
  inputStream = fopen(fileName, "rt");
  if (inputStream == NULL)
    return 0;
  lineNo = 1;
  colNo = 0;
  readChar();
  return 1;
}

void closeInputStream(void) {
  fclose(inputStream);
}