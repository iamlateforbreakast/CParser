/* Grammar.h */

#ifndef _GRAMMAR_H_
#define _GRAMMAR_H_

#include "String2.h"
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
  unsigned int structOrUnionCnt;
  unsigned int evaluatedDeclarationSpecifiers;
  unsigned int resultDeclarationSpecifiers;
  unsigned int evaluatedDeclarator;
  unsigned int resultDeclarator;
  Declarator declarator;
} Grammar;

Grammar* Grammar_new();
void Grammar_delete(Grammar* this);
void Grammar_pushToken(Grammar* this, Token* token);

#endif
