/* TokenList.c */

#include "TokenList.h"
#include "Common.h"

TokenList* TokenList_new()
{
    TokenList* this;
    
    this = (TokenList*)Memory_alloc(sizeof(TokenList));
    return this;
}

void TokenList_delete(TokenList* this)
{
    //TransUnit_free(this->currentTransUnit);
    Memory_free(this, sizeof(TokenList));
}

void TokenList_initialise(TokenList* this, String* fileName)
{
    //this->currentTransUnit = TransUnit_new();
    //TransUnit_loadFromFile(fileName);
}

void TokenList_getTokenFromTransUnit(TokenList* this)
{
    /*
    c = TransUnit_getChar(this->currentTransUnit);

    while (!isEOF)
    {
        if {}()=+-!
        if space 
	if digit
        if letter
        c = TransUnit_getChar(this->currentTransUnit);
    }
    */
}
