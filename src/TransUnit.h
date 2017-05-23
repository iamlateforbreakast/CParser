/* TransUnit.h */

#ifndef _TRANSUNIT_H_
#define _TRANSUNIT_H_

#include "Common.h"
#include "StringProcessor.h"

typedef struct TransUnit
{
    StringProcessor* processor;
} TransUnit;

TransUnit* TransUnit_new();
void TransUnit_delete(TransUnit* this);
void TransUnit_addUnit(const char* fileName);
unsigned char TransUnit_readCharFromProcessedStream(TransUnit* this);
unsigned int TransUnit_match(TransUnit* this, String* keyword);
String* TransUnit_readIdentifier(TransUnit* this);
String* TransUnit_readConstantInteger(TransUnit* this);
#endif
