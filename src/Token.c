/* Token.c */

#include "Token.h"

#include "Common.h"

Token* Token_new(TokenId id, unsigned int value)
{
  Token* this = NULL;

  this = (Token*)Memory_alloc(sizeof(Token));
  this->id = id;
  this->value = value;

  return this;
}

void Token_delete(Token* this)
{
  Memory_free(this, sizeof(Token));
}
