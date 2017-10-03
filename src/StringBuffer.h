/* StringBuffer.h */

#ifndef _STRINGBUFFER_H_
#define _STRINGBUFFER_H_

#include "Common.h"

typedef struct StringBuffer
{
    unsigned int pos;
    unsigned int line;
    unsigned int column;
    String* s;
    String* name;
    String* path;
} StringBuffer;

StringBuffer* StringBuffer_new(String* s, String* name);
void StringBuffer_delete(StringBuffer* this);
StringBuffer* StringBuffer_newFromFile(const char* fileName);
unsigned char StringBuffer_readChar(StringBuffer* this);
unsigned char StringBuffer_peekChar(StringBuffer* this);
unsigned int StringBuffer_isEOF(StringBuffer* this);
unsigned int StringBuffer_match(StringBuffer* this, String* pattern);
String* StringBuffer_readback(StringBuffer* this, unsigned int nbChar);
String* StringBuffer_getName(StringBuffer* this);
#endif
