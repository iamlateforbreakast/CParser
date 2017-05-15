/* StringBuffer.c */

#include "StringBuffer.h"

StringBuffer* StringBuffer_new(String* s)
{
  StringBuffer* this;

  this=(StringBuffer*)Memory_alloc(sizeof(StringBuffer));
  this->pos = 0;
  this->s = s;
  
  return this;
}

void StringBuffer_delete(StringBuffer* this)
{
    String_delete(this->s);
    //String_delete(this->name);
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

unsigned char StringBuffer_readChar(StringBuffer* this)
{
  unsigned char result=0;
  
  if (this->pos<this->s->length)
  {
    result=this->s->buffer[this->pos];
    this->pos++;
  }
  
  return result;
}

unsigned char StringBuffer_peekChar(StringBuffer* this)
{
  unsigned char result;
  
  return result;
}

unsigned int StringBuffer_match(StringBuffer* this, String* pattern)
{
  return (String_match(this->s, this->pos, pattern));
}

unsigned int StringBuffer_isEOF(StringBuffer* this)
{
  unsigned int result = 0;
  
  if (this->pos>=this->s->length) result = 1;
  
  return result;
}
