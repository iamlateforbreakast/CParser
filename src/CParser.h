/* CParser.h */

#ifndef _CPARSER_H_
#define _CPARSER_H_

//#include "ElementTable.h"
#include "TokenList.h"
#include "Grammar.h"

typedef struct CParser
{
    TokenList* tokenList;
	Grammar* grammar;
	String* initialLocation;
	String* sdbName;
} CParser;

extern CParser* cparser;

CParser* CParser_new();
void CParser_delete(CParser* this);
void CParser_parse(CParser* this, char* fileName);

#endif
