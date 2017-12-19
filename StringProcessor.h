/* StringProcessor.h */

#ifndef _STRINGPROCESSOR_H_
#define _STRINGPROCESSOR_H_

#include "StringBuffer.h"
#include "Map.h"
#include "Token.h"

#define NB_MAX_BUFFERS (20)

typedef struct StringProcessor
{
    StringBuffer* buffers[NB_MAX_BUFFERS];	
    StringBuffer* currentBuffer;
	unsigned int nbOpenBuffers;
	Map* macros;
	String* currentFilename;
} StringProcessor;

StringProcessor* StringProcessor_new();
void StringProcessor_delete(StringProcessor* this);
void StringProcessor_addFile(StringProcessor* this, String* file);
Token* StringProcessor_getToken(StringProcessor* this);
#endif
