/* token.c */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h> // <--- QUAN TRONG: Phai co thu vien nay de dung toupper
#include "token.h"

struct {
  char string[MAX_IDENT_LEN + 1];
  TokenType tokenType;
} keywords[] = {
  {"PROGRAM", KW_PROGRAM},
  {"CONST", KW_CONST},
  {"TYPE", KW_TYPE},
  {"VAR", KW_VAR},
  {"INTEGER", KW_INTEGER},
  {"CHAR", KW_CHAR},
  {"ARRAY", KW_ARRAY},
  {"OF", KW_OF},
  {"FUNCTION", KW_FUNCTION},
  {"PROCEDURE", KW_PROCEDURE},
  {"BEGIN", KW_BEGIN},
  {"END", KW_END},
  {"CALL", KW_CALL},
  {"IF", KW_IF},
  {"THEN", KW_THEN},
  {"ELSE", KW_ELSE},
  {"WHILE", KW_WHILE},
  {"DO", KW_DO},
  {"FOR", KW_FOR},
  {"TO", KW_TO},
  // Tu khoa moi Bai 2
  {"BYTES", KW_BYTES},
  {"REPEAT", KW_REPEAT},
  {"UNTIL", KW_UNTIL}
};

int keywordCount = sizeof(keywords) / sizeof(keywords[0]);

TokenType checkKeyword(char *string) {
  char temp[MAX_IDENT_LEN + 1];
  int i;
  
  // Chuyen chuoi dau vao thanh chu hoa
  for (i = 0; string[i] != '\0' && i < MAX_IDENT_LEN; i++) {
    temp[i] = toupper(string[i]);
  }
  temp[i] = '\0';

  // So sanh voi danh sach tu khoa
  for (i = 0; i < keywordCount; i++) {
    if (strcmp(keywords[i].string, temp) == 0) {
      return keywords[i].tokenType;
    }
  }
  
  return TK_NONE;
}

Token* makeToken(TokenType tokenType, int lineNo, int colNo) {
  Token *token = (Token*)malloc(sizeof(Token));
  token->tokenType = tokenType;
  token->lineNo = lineNo;
  token->colNo = colNo;
  return token;
}