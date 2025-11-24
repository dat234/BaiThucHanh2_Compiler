/* error.h */
#ifndef __ERROR_H__
#define __ERROR_H__

typedef enum {
  ERR_ENDOFCOMMENT,
  ERR_IDENTTOOLONG,
  ERR_INVALIDCHARCONSTANT,
  ERR_INVALIDSYMBOL,
  ERR_INVALIDSTRINGCONSTANT
} ErrorCode;

void error(ErrorCode err, int lineNo, int colNo);

#endif