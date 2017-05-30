/* Grammar.c */

#include "Grammar.h"

#include "Common.h"
#include "Token.h"

Grammar* Grammar_new()
{
  Grammar* this = NULL;
  
  this = (Grammar*)Memory_alloc(sizeof(Grammar));
  
  return this;
}

void Grammar_delete(Grammar* this)
{
  Memory_free(this, sizeof(Grammar));
}

#if 0
void Grammar_pushToken(Token* token)
{
  if token == TYPEDEF
  {
    state = 
  }

}
#endif