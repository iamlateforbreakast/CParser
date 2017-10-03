/* StringProcessor.h */

#ifndef _STRINGPROCESSOR_H_
#define _STRINGPROCESSOR_H_

#include "StringBuffer.h"
#include "Map.h"

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
unsigned char StringProcessor_readTransUnitChar(StringProcessor* this, String* f, unsigned int *l, unsigned int *c);
unsigned int StringProcessor_checkForMacro(StringProcessor* this);
unsigned int StringProcessor_match(StringProcessor* this, String* pattern);
String* StringProcessor_readIdentifier(StringProcessor* this);
String* StringProcessor_readInteger(StringProcessor* this);
String* StringProcessor_getFileName(StringProcessor* this);
#endif
