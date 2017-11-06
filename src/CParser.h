/* CParser.h */

#ifndef _CPARSER_H_
#define _CPARSER_H_

#include "StringProcessor.h"
#include "Grammar.h"

typedef struct CParser
{
    StringProcessor* stringProcessor;
	Grammar* grammar;
	String* initialLocation;
	String* sdbName;
	unsigned int isDbReset; 
} CParser;

extern CParser* cparser;

CParser* CParser_new();
void CParser_delete(CParser* this);
void CParser_parse(CParser* this, char* fileName);

#endif
