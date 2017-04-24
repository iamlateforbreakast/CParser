/* StringProcessor.h */

#ifndef _STRINGPROCESSOR_H_
#define _STRINGPROCESSOR_H_

#include "StringBuffer.h"

#define NB_MAX_BUFFERS (20)
#define NB_MAX_SEARCH_PATH (20)

typedef struct StringProcessor
{
    StringBuffer* buffers[NB_MAX_BUFFERS];
    String* searchPath[NB_MAX_SEARCH_PATH];
    //StringBuffer* currentBuffer;
} StringProcessor;

StringProcessor* StringProcessor_new();
void StringProcessor_delete(StringProcessor* this);
void StringProcessor_addFile(StringProcessor* this, const char* path, const char* fileName);
unsigned char StringProcessor_readTransUnitChar(StringProcessor* this);
#endif
