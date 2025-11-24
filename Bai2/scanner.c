/* scanner.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"

// Khai báo biến ngoại lai từ reader.c và charcode.c
extern int lineNo;
extern int colNo;
extern int currentChar;
extern CharCode charCodes[];

/********************************************************
 * Hàm: skipBlank
 * Mục đích: Bỏ qua khoảng trắng, tab, xuống dòng
 ********************************************************/
void skipBlank() {
  while (currentChar != EOF && charCodes[currentChar] == CHAR_SPACE)
    readChar();
}

/********************************************************
 * Hàm: skipComment
 * Mục đích: Bỏ qua comment kiểu cũ (* ... *)
 ********************************************************/
void skipComment() {
  while (1) {
    readChar();
    if (currentChar == EOF) {
      error(ERR_ENDOFCOMMENT, lineNo, colNo);
      return;
    }
    // Nếu gặp dấu *, kiểm tra xem ký tự sau có phải ) không
    if (charCodes[currentChar] == CHAR_TIMES) {
      readChar();
      if (currentChar == EOF) {
        error(ERR_ENDOFCOMMENT, lineNo, colNo);
        return;
      }
      if (charCodes[currentChar] == CHAR_RPAR) {
        readChar(); // Đọc qua dấu )
        return;
      }
    }
  }
}

/********************************************************
 * Hàm: readIdentKeyword
 * Mục đích: Đọc tên biến hoặc từ khóa
 ********************************************************/
Token* readIdentKeyword(void) {
  int count = 0;
  Token* token = makeToken(TK_IDENT, lineNo, colNo);
  
  while (currentChar != EOF && 
         (charCodes[currentChar] == CHAR_LETTER || charCodes[currentChar] == CHAR_DIGIT)) {
    if (count < MAX_IDENT_LEN) {
        token->string[count++] = (char)currentChar;
    }
    readChar();
  }
  token->string[count] = '\0';

  if (count > MAX_IDENT_LEN) {
    error(ERR_IDENTTOOLONG, token->lineNo, token->colNo);
  } else {
    TokenType type = checkKeyword(token->string);
    if (type != TK_NONE) token->tokenType = type;
  }
  return token;
}

/********************************************************
 * Hàm: readNumber
 * Mục đích: Đọc số nguyên
 ********************************************************/
Token* readNumber(void) {
  int count = 0;
  Token* token = makeToken(TK_NUMBER, lineNo, colNo);
  
  while (currentChar != EOF && charCodes[currentChar] == CHAR_DIGIT) {
     // Lưu ý: Nếu muốn giới hạn độ dài số thì kiểm tra count ở đây
     if (count < MAX_STRING_LEN) {
        token->string[count++] = (char)currentChar;
     }
     readChar();
  }
  token->string[count] = '\0';
  token->value = atoi(token->string);
  return token;
}

/********************************************************
 * Hàm: readConstChar
 * Mục đích: Đọc hằng ký tự 'x'
 ********************************************************/
Token* readConstChar(void) {
  Token* token = makeToken(TK_CHAR, lineNo, colNo);
  
  readChar(); // Bỏ dấu ' mở
  
  if (currentChar == EOF) {
    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
    return token;
  }
  
  token->string[0] = currentChar;
  token->string[1] = '\0';
  
  readChar(); // Đọc nội dung
  
  if (charCodes[currentChar] == CHAR_SINGLEQUOTE) {
    readChar(); // Bỏ dấu ' đóng
    return token;
  } else {
    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
    return token;
  }
}

/********************************************************
 * Hàm: readString [MỚI CHO BÀI 2]
 * Mục đích: Đọc chuỗi ký tự "..."
 ********************************************************/
Token* readString(void) {
    int count = 0;
    Token* token = makeToken(TK_STRING, lineNo, colNo);
    
    readChar(); // Bỏ qua dấu " mở đầu

    // Đọc liên tục cho đến khi gặp dấu " hoặc hết file
    while (currentChar != EOF && charCodes[currentChar] != CHAR_DOUBLEQUOTE) {
        if (count < MAX_STRING_LEN) {
            token->string[count++] = (char)currentChar;
        }
        readChar();
    }
    token->string[count] = '\0';

    if (currentChar == EOF) {
        // Lỗi: Hết file mà không thấy dấu đóng "
        error(ERR_INVALIDSTRINGCONSTANT, token->lineNo, token->colNo);
    } else {
        readChar(); // Bỏ qua dấu " kết thúc
    }
    return token;
}

/********************************************************
 * Hàm: getToken
 * Mục đích: Bộ não của Scanner
 ********************************************************/
Token* getToken(void) {
  Token *token;
  int ln, cn;

  if (currentChar == EOF) 
      return makeToken(TK_EOF, lineNo, colNo);

  switch (charCodes[currentChar]) {
  case CHAR_SPACE: skipBlank(); return getToken();
  case CHAR_LETTER: return readIdentKeyword();
  case CHAR_DIGIT: return readNumber();
  
  case CHAR_PLUS: 
    token = makeToken(SB_PLUS, lineNo, colNo); readChar(); return token;
  case CHAR_MINUS:
    token = makeToken(SB_MINUS, lineNo, colNo); readChar(); return token;
  
  // --- XỬ LÝ PHÉP NHÂN (*) VÀ LŨY THỪA (**) ---
  case CHAR_TIMES:
    ln = lineNo; cn = colNo;
    readChar(); // Lookahead
    if (currentChar != EOF && charCodes[currentChar] == CHAR_TIMES) {
        // Gặp dấu * thứ 2 -> Đây là **
        readChar(); 
        return makeToken(SB_POWER, ln, cn);
    }
    // Nếu không -> Là dấu * thường
    return makeToken(SB_TIMES, ln, cn);

  // --- XỬ LÝ PHÉP CHIA (/) VÀ COMMENT (//) ---
  case CHAR_SLASH:
    ln = lineNo; cn = colNo;
    readChar(); // Lookahead
    if (currentChar != EOF && charCodes[currentChar] == CHAR_SLASH) {
        // Gặp dấu / thứ 2 -> Đây là comment dòng //
        // Bỏ qua đến hết dòng
        while (currentChar != EOF && currentChar != '\n') {
            readChar();
        }
        // Gọi lại getToken để lấy token tiếp theo (sau khi đã bỏ qua comment)
        return getToken();
    }
    // Nếu không -> Là dấu / thường
    return makeToken(SB_SLASH, ln, cn);

  // --- XỬ LÝ PHÉP MODULO (%) ---
  case CHAR_PERCENT:
    token = makeToken(SB_MOD, lineNo, colNo); readChar(); return token;

  // --- XỬ LÝ STRING (") ---
  case CHAR_DOUBLEQUOTE:
    return readString();

  // --- CÁC KÝ TỰ CÒN LẠI ---
  case CHAR_COMMA:
    token = makeToken(SB_COMMA, lineNo, colNo); readChar(); return token;
  case CHAR_SEMICOLON:
    token = makeToken(SB_SEMICOLON, lineNo, colNo); readChar(); return token;
  case CHAR_EQ: 
    token = makeToken(SB_EQ, lineNo, colNo); readChar(); return token;
  case CHAR_PERIOD:
    token = makeToken(SB_PERIOD, lineNo, colNo); readChar(); return token;
  case CHAR_RPAR:
    token = makeToken(SB_RPAR, lineNo, colNo); readChar(); return token;
  case CHAR_SINGLEQUOTE: 
    return readConstChar();
    
  case CHAR_LPAR: // ( hoặc (. hoặc (*
    ln = lineNo; cn = colNo;
    readChar();
    if (currentChar == EOF) return makeToken(SB_LPAR, ln, cn);
    
    if (charCodes[currentChar] == CHAR_PERIOD) {
        readChar(); return makeToken(SB_LSEL, ln, cn);
    } else if (charCodes[currentChar] == CHAR_TIMES) {
        readChar(); skipComment(); return getToken();
    } else {
        return makeToken(SB_LPAR, ln, cn);
    }

  case CHAR_LT:
    ln = lineNo; cn = colNo; readChar();
    if (charCodes[currentChar] == CHAR_EQ) {
        readChar(); return makeToken(SB_LE, ln, cn);
    }
    return makeToken(SB_LT, ln, cn);

  case CHAR_GT:
    ln = lineNo; cn = colNo; readChar();
    if (charCodes[currentChar] == CHAR_EQ) {
        readChar(); return makeToken(SB_GE, ln, cn);
    }
    return makeToken(SB_GT, ln, cn);

  case CHAR_EXCLAIMATION:
     ln = lineNo; cn = colNo; readChar();
     if (charCodes[currentChar] == CHAR_EQ) {
         readChar(); return makeToken(SB_NEQ, ln, cn);
     }
     error(ERR_INVALIDSYMBOL, ln, cn);
     return makeToken(TK_NONE, ln, cn);

  case CHAR_COLON:
    ln = lineNo; cn = colNo; readChar();
    if (charCodes[currentChar] == CHAR_EQ) {
        readChar(); return makeToken(SB_ASSIGN, ln, cn);
    }
    return makeToken(SB_COLON, ln, cn);

  default:
    token = makeToken(TK_NONE, lineNo, colNo);
    error(ERR_INVALIDSYMBOL, lineNo, colNo);
    readChar();
    return token;
  }
}