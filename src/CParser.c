/* CParser.c */

#include "Common.h"
#include "CParser.h"

CParser* CParser_new()
{
    CParser* this;

    this=(CParser*)Memory_alloc(sizeof(CParser));

    return this;
}

void CParser_delete(CParser* this)
{
    Memory_free(this, sizeof(CParser));
}

void CParser_parse(CParser* this, const char* fileName)
{
    //Initialise from initial fileName
    //TokenList_initialise(fileName);
    
    /*while (TokensList_get())
	populate functions from list of tokens
	populate global vars
	populate local vars
	populate code blocks */
}

/*void CParser_populateFunctions()
{
    if Grammar_matchFunctions(element)
    {
    }
}

void CParser_populateGlobalVars()
{
}*/
