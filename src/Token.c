/* Token.c */

#include "Token.h"

#include "Common.h"

//s/tatic int nb = 0;
//static int nbStr = 0;

Token* Token_new(TokenId id, char* text, void* value, String* fileName)
{
  Token* this = NULL;

  this = (Token*)Memory_alloc(sizeof(Token));
  this->id = id;
  this->value = value;
  this->fileName = fileName;
  this->text = text;
  //if (this->id==TOK_IDENTIFIER)
  //{
  //  nbStr++;
  //  printf("Token_new: nbStr=%d\n", nbStr);
  //}
  //nb++;
  //printf("Token_new: nb=%d\n", nb);
  return this;
}

void Token_delete(Token* this)
{
  if (this->id==TOK_IDENTIFIER)
  {
    //nbStr = nbStr-1;
    //printf("Token_new: nbStr=%d\n", nbStr);
    String_delete((String*)(this->value));
  }
  String_delete(this->fileName);
  //nb=nb-1;
  //printf("Token_delete: nb=%d\n", nb);
  Memory_free(this, sizeof(Token));
}
