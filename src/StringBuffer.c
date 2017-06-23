/* StringBuffer.c */

#include "StringBuffer.h"

#include "Common.h"

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

#if 0
StringBuffer* StringBuffer_newFromFile(const char* fileName)
{
  StringBuffer* this;
  this=StringBuffer_new();
   
  this->s = String_newFromFile(fileName);
  this->name = String_new(fileName);

  return this;
}
#endif

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
  unsigned char result = 0;
  
  if (this->pos<this->s->length)
  {
    result=this->s->buffer[this->pos];
  }
  
  return result;
}

unsigned int StringBuffer_match(StringBuffer* this, String* pattern)
{
  unsigned int result = 0;
  
  //String_print(pattern, "Matching: ");
  //printf("Pos: %d\n", this->pos);
  
  if (String_match(this->s, this->pos, pattern))
  {
    result = 1;
    this->pos = this->pos + pattern->length;
    //String_print(pattern, "Match found: %s\n");
    //printf("Pos: %d\n", this->pos);
  }
  return result;
}

unsigned int StringBuffer_isEOF(StringBuffer* this)
{
  unsigned int result = 0;
  
  if (this->pos>=this->s->length) result = 1;
  
  return result;
}

String* StringBuffer_readback(StringBuffer* this, unsigned int nbChar)
{
  String* result = NULL;
  
  result = String_subString(this->s, this->pos-nbChar, nbChar);

  return result;
}

String* StringBuffer_getName(StringBuffer* this)
{
  return this->name;
}