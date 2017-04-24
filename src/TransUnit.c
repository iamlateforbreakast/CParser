/* Translation Unit */

#include "TransUnit.h"

#include "Common.h"

TransUnit* TransUnit_new()
{
    TransUnit* this;

    this = (TransUnit*)Memory_alloc(sizeof(TransUnit));

    return this;
}

void TransUnit_delete(TransUnit* this)
{
    Memory_free(this, sizeof(TransUnit));
}

void TransUnit_loadFromFile(TransUnit* this, const char* fileName)
{
    String path;

    StringProcessor_addFile(this->processor, "*", fileName);
}

/*
 *
 */
unsigned char TransUnit_readCharFromProcessedStream(TransUnit* this)
{
    unsigned char c;
   //this->processor->readChar();
   //if // readsingleLineComment
   //if /* readMultiLine comment   

    return c;
}
