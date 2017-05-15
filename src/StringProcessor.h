/* StringProcessor.h */

#ifndef _STRINGPROCESSOR_H_
#define _STRINGPROCESSOR_H_

#include "StringBuffer.h"

#define NB_MAX_BUFFERS (20)

typedef struct StringProcessor
{
    StringBuffer* buffers[NB_MAX_BUFFERS];	
    StringBuffer* currentBuffer;
} StringProcessor;

StringProcessor* StringProcessor_new();
void StringProcessor_delete(StringProcessor* this);
void StringProcessor_addFile(StringProcessor* this, String* file);
unsigned char StringProcessor_readTransUnitChar(StringProcessor* this);
unsigned int StringProcessor_checkForMacro(StringProcessor* this);
unsigned int StringProcessor_match(StringProcessor* this, String* pattern);
#endif
