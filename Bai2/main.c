/* main.c - Khong can scanner.h */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
// #include "scanner.h"  <-- Bo dong nay di
#include "reader.h"
#include "token.h"

// --- KHAI BÁO THỦ CÔNG CÁC HÀM TỪ FILE KHÁC ---
// Thay vì include scanner.h, ta khai báo trực tiếp ở đây để main biết hàm tồn tại
extern Token* getToken(void);
extern void initCharCodes(void);
// ----------------------------------------------

// Hàm in token ra màn hình
void printToken(Token *token) {
    printf("%d-%d:", token->lineNo, token->colNo);
    
    switch (token->tokenType) {
        case TK_NONE: printf("TK_NONE\n"); break;
        case TK_IDENT: printf("TK_IDENT(%s)\n", token->string); break;
        case TK_NUMBER: printf("TK_NUMBER(%s)\n", token->string); break;
        case TK_CHAR: printf("TK_CHAR(\'%s\')\n", token->string); break;
        case TK_EOF: printf("TK_EOF\n"); break;
        
        // Bai 2
        case TK_STRING: printf("TK_STRING(\"%s\")\n", token->string); break;
        case KW_BYTES: printf("KW_BYTES\n"); break;
        case KW_REPEAT: printf("KW_REPEAT\n"); break;
        case KW_UNTIL: printf("KW_UNTIL\n"); break;
        case SB_POWER: printf("SB_POWER\n"); break;
        case SB_MOD: printf("SB_MOD\n"); break;

        // Tu khoa
        case KW_PROGRAM: printf("KW_PROGRAM\n"); break;
        case KW_CONST: printf("KW_CONST\n"); break;
        case KW_TYPE: printf("KW_TYPE\n"); break;
        case KW_VAR: printf("KW_VAR\n"); break;
        case KW_INTEGER: printf("KW_INTEGER\n"); break;
        case KW_CHAR: printf("KW_CHAR\n"); break;
        case KW_ARRAY: printf("KW_ARRAY\n"); break;
        case KW_OF: printf("KW_OF\n"); break;
        case KW_FUNCTION: printf("KW_FUNCTION\n"); break;
        case KW_PROCEDURE: printf("KW_PROCEDURE\n"); break;
        case KW_BEGIN: printf("KW_BEGIN\n"); break;
        case KW_END: printf("KW_END\n"); break;
        case KW_CALL: printf("KW_CALL\n"); break;
        case KW_IF: printf("KW_IF\n"); break;
        case KW_THEN: printf("KW_THEN\n"); break;
        case KW_ELSE: printf("KW_ELSE\n"); break;
        case KW_WHILE: printf("KW_WHILE\n"); break;
        case KW_DO: printf("KW_DO\n"); break;
        case KW_FOR: printf("KW_FOR\n"); break;
        case KW_TO: printf("KW_TO\n"); break;

        // Ky hieu
        case SB_SEMICOLON: printf("SB_SEMICOLON\n"); break;
        case SB_COLON: printf("SB_COLON\n"); break;
        case SB_PERIOD: printf("SB_PERIOD\n"); break;
        case SB_COMMA: printf("SB_COMMA\n"); break;
        case SB_ASSIGN: printf("SB_ASSIGN\n"); break;
        case SB_EQ: printf("SB_EQ\n"); break;
        case SB_NEQ: printf("SB_NEQ\n"); break;
        case SB_LT: printf("SB_LT\n"); break;
        case SB_LE: printf("SB_LE\n"); break;
        case SB_GT: printf("SB_GT\n"); break;
        case SB_GE: printf("SB_GE\n"); break;
        case SB_PLUS: printf("SB_PLUS\n"); break;
        case SB_MINUS: printf("SB_MINUS\n"); break;
        case SB_TIMES: printf("SB_TIMES\n"); break;
        case SB_SLASH: printf("SB_SLASH\n"); break;
        case SB_LPAR: printf("SB_LPAR\n"); break;
        case SB_RPAR: printf("SB_RPAR\n"); break;
        case SB_LSEL: printf("SB_LSEL\n"); break;
        case SB_RSEL: printf("SB_RSEL\n"); break;
        
        default: printf("UNKNOWN(%d)\n", token->tokenType);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <input file>\n", argv[0]);
        return 1;
    }

    // Goi ham khoi tao (da khai bao extern o tren)
    initCharCodes(); 

    if (openInputStream(argv[1]) == 0) {
        printf("Cannot open input file %s\n", argv[1]);
        return 1;
    }

    Token *token;
    while (1) {
        // Goi ham getToken (da khai bao extern o tren)
        token = getToken();
        if (token->tokenType == TK_EOF) break;
        printToken(token);
        free(token);
    }

    closeInputStream();
    return 0;
}