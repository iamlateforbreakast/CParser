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

StringBuffer* StringBuffer_new();
void StringBuffer_delete(StringBuffer* this);
StringBuffer* StringBuffer_newFromFile(const char* fileName);

#endif
