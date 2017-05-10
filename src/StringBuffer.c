/* StringBuffer.c */

#include "StringBuffer.h"

StringBuffer* StringBuffer_new()
{
  StringBuffer* this;

  this=(StringBuffer*)Memory_alloc(sizeof(StringBuffer));
  this->pos = 0;
    
  return this;
}

void StringBuffer_delete(StringBuffer* this)
{
    String_delete(this->s);
    String_delete(this->name);
    Memory_free(this, sizeof(StringBuffer));
}

StringBuffer* StringBuffer_newFromFile(const char* fileName)
{
  StringBuffer* this;
#if 0
  this=StringBuffer_new();
   
  this->s = String_newFromFile(fileName);
  this->name = String_new(fileName);
#endif

  return this;
}
