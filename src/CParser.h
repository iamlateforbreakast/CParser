/* CParser.h */

#ifndef _CPARSER_H_
#define _CPARSER_H_

//#include "ElementTable.h"
#include "TokenList.h"
#include "FileList.h"

typedef struct CParser
{
    // ElementTable functions
    // ElementTable global vars
    // ElementTable local vars
    // ElementTable code blocks
    TokenList* tokenList;
    FileList* fileList;
} CParser;

CParser* CParser_new();
void CParser_delete(CParser* this);
void CParser_parse(CParser* this, const char* fileName);

#endif
