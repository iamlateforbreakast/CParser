/* TokenList.h */

#ifndef _TOKENLIST_H_
#define _TOKENLIST_H_

#include "TransUnit.h"

typedef struct TokenList
{
    TransUnit* transUnit;
} TokenList;

TokenList* TokenList_new();
void TokenList_delete(TokenList* this);
void TokenList_initialise(TokenList* this, String* fileName);
void TokenList_getTokenFromTansUnit(TokenList* this);

#endif
