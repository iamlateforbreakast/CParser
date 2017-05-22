/* TokenList.h */

#ifndef _TOKENLIST_H_
#define _TOKENLIST_H_

#include "TransUnit.h"

typedef struct TokenList
{
    TransUnit* transUnit;
	String** keyword;
	unsigned int nbKeywords;
} TokenList;

TokenList* TokenList_new();
void TokenList_delete(TokenList* this);
void TokenList_initialise(TokenList* this, String* fileName);
unsigned int TokenList_getTokenFromTansUnit(TokenList* this);
#endif
