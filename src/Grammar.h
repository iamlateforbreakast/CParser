/* Grammar.h */

#ifndef _GRAMMAR_H_
#define _GRAMMAR_H_

#include "String.h"
#include "Token.h"

typedef struct Declarator
{
  String* name;
} Declarator;

typedef struct Grammar{
  unsigned int directDeclaratorCnt;
  unsigned int declaratorCnt;
  unsigned int initDeclaratorListCnt;
  unsigned int declarationSpecifiersCnt;
  unsigned int declarationCnt;
  unsigned int functionDeclarationCnt;
  unsigned int compountStatementCnt;
  Declarator declarator;
} Grammar;

Grammar* Grammar_new();
void Grammar_delete(Grammar* this);
void Grammar_pushToken(Grammar* this, Token* token);

#endif