/* TokenList.c */

#include "TokenList.h"
#include "Common.h"

TokenList* TokenList_new(String* fileContent)
{
    TokenList* this;
    
    this = (TokenList*)Memory_alloc(sizeof(TokenList));
    this->transUnit = TransUnit_new(fileContent);
    
    return this;
}

void TokenList_delete(TokenList* this)
{
    printf("TokenList.c: delete\n");
    TransUnit_delete(this->transUnit);
    Memory_free(this, sizeof(TokenList));
}

void TokenList_initialise(TokenList* this, String* fileBuffer)
{
    
}

unsigned int TokenList_getTokenFromTransUnit(TokenList* this)
{
  unsigned char c;
  
  c = TransUnit_readCharFromProcessedStream(this->transUnit);
  
  printf("Read char %c\n",c);
    /*
    
    while (!isEOF)
    {
        if {}()=+-!
        if space 
	if digit
        if letter
        c = TransUnit_getChar(this->currentTransUnit);
    }
    */
  return c;
}
