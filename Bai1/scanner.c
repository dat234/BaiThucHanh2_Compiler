/* scanner.c */
#include <stdio.h>
#include <stdlib.h>
#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"

extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];

/********************************************************
 * Hàm: skipBlank
 * Mục đích: Bỏ qua các ký tự không có ý nghĩa ngữ nghĩa
 * (khoảng trắng, tab, xuống dòng) để con trỏ file đi đến
 * ký tự bắt đầu của token tiếp theo.
 ********************************************************/
void skipBlank() {
  // Vòng lặp chạy liên tục nếu ký tự hiện tại là khoảng trắng
  while (currentChar != EOF && charCodes[currentChar] == CHAR_SPACE)
    readChar();
}

/********************************************************
 * Hàm: skipComment
 * Mục đích: Bỏ qua toàn bộ nội dung trong chú thích (* ... *)
 * Logic: Máy trạng thái chờ đợi cặp ký tự kết thúc *)
 ********************************************************/
void skipComment() {
  while (1) {
    // Đọc ký tự tiếp theo
    readChar();
    
    if (currentChar == EOF) {
      error(ERR_ENDOFCOMMENT, lineNo, colNo); // Lỗi: Hết file mà chưa đóng comment
      return;
    }
    
    // Logic kiểm tra ký tự kết thúc: Gặp dấu * thì phải kiểm tra tiếp ký tự sau
    if (charCodes[currentChar] == CHAR_TIMES) {
      readChar();
      
      if (currentChar == EOF) {
        error(ERR_ENDOFCOMMENT, lineNo, colNo);
        return;
      }
      
      // Nếu sau dấu * là dấu ) thì đúng là kết thúc comment -> Thoát hàm
      if (charCodes[currentChar] == CHAR_RPAR) {
        readChar(); // Đọc qua dấu ) để sẵn sàng cho token mới
        return;
      }
      // Nếu không phải ), vòng lặp tiếp tục chạy
    }
  }
}

/********************************************************
 * Hàm: readIdentKeyword
 * Mục đích: Đọc một từ và phân loại nó là Tên biến (Ident)
 * hay Từ khóa (Keyword).
 * Quy tắc KPL: Bắt đầu bằng chữ cái, theo sau là chữ hoặc số.
 ********************************************************/
Token* readIdentKeyword(void) {
  int count = 0;
  Token* token = makeToken(TK_IDENT, lineNo, colNo);
  
  // Vòng lặp gom nhóm các ký tự chữ và số liên tiếp
  while (currentChar != EOF && 
         (charCodes[currentChar] == CHAR_LETTER || charCodes[currentChar] == CHAR_DIGIT)) {
    // Chỉ lưu ký tự nếu độ dài chưa vượt quá giới hạn cho phép
    if (count < MAX_IDENT_LEN) {
        token->string[count++] = (char)currentChar;
    }
    readChar();
  }
  // Kết thúc chuỗi bằng ký tự null để thành chuỗi C hợp lệ
  token->string[count] = '\0';

  // Kiểm tra độ dài từ vựng
  if (count > MAX_IDENT_LEN) {
    error(ERR_IDENTTOOLONG, token->lineNo, token->colNo);
  } else {
    // Kiểm tra xem từ vừa đọc có trùng với Từ khóa nào không
    // Nếu có -> update tokenType, nếu không -> giữ nguyên là TK_IDENT
    TokenType type = checkKeyword(token->string);
    if (type != TK_NONE) token->tokenType = type;
  }
  return token;
}

/********************************************************
 * Hàm: readNumber
 * Mục đích: Đọc một chuỗi số nguyên.
 ********************************************************/
Token* readNumber(void) {
  int count = 0;
  Token* token = makeToken(TK_NUMBER, lineNo, colNo);
  
  // Gom nhóm các ký tự số liên tiếp
  while (currentChar != EOF && charCodes[currentChar] == CHAR_DIGIT) {
     if (count < MAX_IDENT_LEN) {
         token->string[count++] = (char)currentChar;
     }
    readChar();
  }
  token->string[count] = '\0';
  
  // Chuyển chuỗi ký tự thành giá trị số nguyên (int)
  token->value = atoi(token->string);
  return token;
}

/********************************************************
 * Hàm: readConstChar
 * Mục đích: Đọc hằng ký tự dạng 'x'.
 ********************************************************/
Token* readConstChar(void) {
  Token* token = makeToken(TK_CHAR, lineNo, colNo);
  
  readChar(); // Bỏ qua dấu nháy đơn mở đầu '
  
  if (currentChar == EOF) {
    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
  }
  
  // Lưu ký tự bên trong
  token->string[0] = currentChar;
  token->string[1] = '\0';
  
  readChar(); // Đọc tiếp để kiểm tra dấu đóng
  
  if (charCodes[currentChar] == CHAR_SINGLEQUOTE) {
    readChar(); // Bỏ qua dấu nháy đơn kết thúc '
    return token;
  } else {
    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo); // Lỗi: Thiếu dấu đóng '
    return token;
  }
}

/********************************************************
 * Hàm: getToken
 * Mục đích: Hàm điều phối chính (Scanner).
 * Logic: Dựa vào ký tự đầu tiên (Lookahead) để quyết định 
 * gọi hàm xử lý phù hợp hoặc tạo Token tương ứng.
 ********************************************************/
Token* getToken(void) {
  Token *token;
  int ln, cn;

  if (currentChar == EOF) 
      return makeToken(TK_EOF, lineNo, colNo);

  // Sử dụng bảng phân loại ký tự để switch
  switch (charCodes[currentChar]) {
  case CHAR_SPACE: skipBlank(); return getToken(); // Đệ quy để lấy token tiếp theo sau khi bỏ khoảng trắng
  case CHAR_LETTER: return readIdentKeyword();
  case CHAR_DIGIT: return readNumber();
  
  // Các toán tử đơn giản 1 ký tự
  case CHAR_PLUS: 
    token = makeToken(SB_PLUS, lineNo, colNo); readChar(); return token;
  case CHAR_MINUS:
    token = makeToken(SB_MINUS, lineNo, colNo); readChar(); return token;
  case CHAR_TIMES:
    token = makeToken(SB_TIMES, lineNo, colNo); readChar(); return token;
  case CHAR_SLASH:
    token = makeToken(SB_SLASH, lineNo, colNo); readChar(); return token;
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

  // --- XỬ LÝ CÁC TRƯỜNG HỢP CÓ THỂ LÀ TOKEN GHÉP (Lookahead) ---

  case CHAR_LPAR: // Gặp dấu '(' -> Có thể là (, (. hoặc (*
    ln = lineNo; cn = colNo;
    readChar(); // Nhìn trước ký tự tiếp theo
    
    if (currentChar == EOF) 
        return makeToken(SB_LPAR, ln, cn);

    if (charCodes[currentChar] == CHAR_PERIOD) { // Gặp (. -> Khai báo mảng
        readChar(); return makeToken(SB_LSEL, ln, cn);
    } else if (charCodes[currentChar] == CHAR_TIMES) { // Gặp (* -> Bắt đầu Comment
        readChar(); skipComment(); return getToken(); // Bỏ qua comment và tìm token tiếp
    } else { // Không phải . hay * -> Chính là dấu ngoặc đơn (
        return makeToken(SB_LPAR, ln, cn);
    }

  case CHAR_LT: // Gặp dấu '<' -> Có thể là < hoặc <=
    ln = lineNo; cn = colNo;
    readChar();
    if (charCodes[currentChar] == CHAR_EQ) { // Gặp = -> Token là <=
        readChar(); return makeToken(SB_LE, ln, cn);
    }
    return makeToken(SB_LT, ln, cn); // Token là <

  case CHAR_GT: // Gặp dấu '>' -> Có thể là > hoặc >=
    ln = lineNo; cn = colNo;
    readChar();
    if (charCodes[currentChar] == CHAR_EQ) { // Gặp = -> Token là >=
        readChar(); return makeToken(SB_GE, ln, cn);
    }
    return makeToken(SB_GT, ln, cn); // Token là >

  case CHAR_EXCLAIMATION: // Gặp dấu '!' -> Bắt buộc phải là !=
     ln = lineNo; cn = colNo;
     readChar();
     if (charCodes[currentChar] == CHAR_EQ) {
         readChar(); return makeToken(SB_NEQ, ln, cn);
     }
     // Nếu sau ! không phải = -> Lỗi cú pháp
     error(ERR_INVALIDSYMBOL, ln, cn);
     return makeToken(TK_NONE, ln, cn);

  case CHAR_COLON: // Gặp dấu ':' -> Có thể là : hoặc :=
    ln = lineNo; cn = colNo;
    readChar();
    if (charCodes[currentChar] == CHAR_EQ) { // Gặp = -> Phép gán :=
        readChar(); return makeToken(SB_ASSIGN, ln, cn);
    }
    return makeToken(SB_COLON, ln, cn); // Dấu :

  default: // Ký tự lạ không có trong bảng chữ cái
    token = makeToken(TK_NONE, lineNo, colNo);
    error(ERR_INVALIDSYMBOL, lineNo, colNo);
    readChar();
    return token;
  }
}