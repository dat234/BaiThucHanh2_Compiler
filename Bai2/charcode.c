/* charcode.c */
#include "charcode.h"

CharCode charCodes[256];

void initCharCodes() {
  int i;
  for (i = 0; i < 256; i++) charCodes[i] = CHAR_UNKNOWN;
  for (i = 'a'; i <= 'z'; i++) charCodes[i] = CHAR_LETTER;
  for (i = 'A'; i <= 'Z'; i++) charCodes[i] = CHAR_LETTER;
  for (i = '0'; i <= '9'; i++) charCodes[i] = CHAR_DIGIT;
  charCodes['+'] = CHAR_PLUS;
  charCodes['-'] = CHAR_MINUS;
  charCodes['*'] = CHAR_TIMES;
  charCodes['/'] = CHAR_SLASH;
  charCodes['<'] = CHAR_LT;
  charCodes['>'] = CHAR_GT;
  charCodes['!'] = CHAR_EXCLAIMATION;
  charCodes['='] = CHAR_EQ;
  charCodes[','] = CHAR_COMMA;
  charCodes['.'] = CHAR_PERIOD;
  charCodes[':'] = CHAR_COLON;
  charCodes[';'] = CHAR_SEMICOLON;
  charCodes['\''] = CHAR_SINGLEQUOTE;
  charCodes['('] = CHAR_LPAR;
  charCodes[')'] = CHAR_RPAR;
  charCodes[' '] = CHAR_SPACE;
  charCodes['\t'] = CHAR_SPACE;
  charCodes['\n'] = CHAR_SPACE;
  charCodes['\r'] = CHAR_SPACE;
  // [MỚI] Thêm ánh xạ cho ký tự mới
  charCodes['"'] = CHAR_DOUBLEQUOTE;
  charCodes['%'] = CHAR_PERCENT;
}