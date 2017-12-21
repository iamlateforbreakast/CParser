/**********************************************//** 
  @file Grammar.h
  
  @brief TBD
  @details TBD
**************************************************/
#ifndef _GRAMMAR_H_
#define _GRAMMAR_H_

#include "String2.h"
#include "Token.h"

typedef struct Grammar Grammar;

Grammar* Grammar_new();
void Grammar_delete(Grammar* this);
void Grammar_pushToken(Grammar* this, Token* token);

#endif