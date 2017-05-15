/* StringBuffer.h */

#ifndef _STRINGBUFFER_H_
#define _STRINGBUFFER_H_

#include "Common.h"

typedef struct StringBuffer
{
    unsigned int pos;
    String* s;
    String* name;
    String* path;
} StringBuffer;

StringBuffer* StringBuffer_new(String* s);
void StringBuffer_delete(StringBuffer* this);
StringBuffer* StringBuffer_newFromFile(const char* fileName);
unsigned char StringBuffer_readChar(StringBuffer* this);
unsigned char StringBuffer_peekChar(StringBuffer* this);
unsigned int StringBuffer_isEOF(StringBuffer* this);
#endif
