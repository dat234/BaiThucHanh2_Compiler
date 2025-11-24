/* error.c */
#include <stdio.h>
#include <stdlib.h>
#include "error.h"

void error(ErrorCode err, int lineNo, int colNo) {
  switch (err) {
  case ERR_ENDOFCOMMENT:
    printf("%d-%d:%s\n", lineNo, colNo, "End of comment expected!");
    break;
  case ERR_IDENTTOOLONG:
    printf("%d-%d:%s\n", lineNo, colNo, "Identification too long!");
    break;
  case ERR_INVALIDCHARCONSTANT:
    printf("%d-%d:%s\n", lineNo, colNo, "Invalid const char!");
    break;
  case ERR_INVALIDSYMBOL:
    printf("%d-%d:%s\n", lineNo, colNo, "Invalid symbol!");
    break;
  case ERR_INVALIDSTRINGCONSTANT:
    printf("%d-%d:%s\n", lineNo, colNo, "Invalid string constant! (Missing closing quote)");
    break;
  }
  exit(0);
}