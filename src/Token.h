/* Token.h */

#ifndef _TOKEN_H_
#define _TOKEN_H_

#include "String2.h"

typedef enum
{
  TOK_INT=0,
  TOK_FLOAT,
  TOK_AUTO,
  TOK_BREAK,
  TOK_CASE,
  TOK_CHAR,
  TOK_CONST,
  TOK_CONTINUE,
  TOK_DEFAULT,
  TOK_DO,
  TOK_DOUBLE,
  TOK_ELSE,
  TOK_ENUM,
  TOK_EXTERN,
  TOK_FOR,
  TOK_GOTO,
  TOK_IF,
  TOK_INLINE,
  TOK_LONG,
  TOK_REGISTER,
  TOK_RESTRICT,
  TOK_RETURN,
  TOK_SHORT,
  TOK_SIGNED,
  TOK_SIZEOF,
  TOK_STATIC,
  TOK_TYPEDEF,
  TOK_STRUCT,
  TOK_UNION,
  TOK_UNSIGNED,
  TOK_VOID,
  TOK_VOLATILE,
  TOK_WHILE,
  TOK_IDENTIFIER,
  TOK_CONSTANT,
  TOK_UNKNOWN,
  TOK_EOF
} TokenId;

typedef struct Token
{
  TokenId id;
  char * text;
  void* value;
  String* fileName;
  unsigned int line;  
  unsigned int column;
} Token;

Token* Token_new(TokenId id, char* text, void* value, String* fileName, unsigned int l, unsigned int c);
void Token_delete(Token* this);
#endif
