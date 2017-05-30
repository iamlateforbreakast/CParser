/* Memory.c */

#include "Common.h"

#include <string.h>

unsigned int Memory_nbBytesAllocated = 0;
unsigned int Memory_maxNbBytesAllocated = 0;

void* Memory_alloc(unsigned int nbBytes)
{
    void* p = NULL;

    p = malloc(nbBytes);
    Memory_nbBytesAllocated += nbBytes;
    if (Memory_nbBytesAllocated>Memory_maxNbBytesAllocated) Memory_maxNbBytesAllocated = Memory_nbBytesAllocated;
    
    printf("Memory: Malloc %d %x\n", nbBytes,p);
    return p;
}

void Memory_free(void* p, unsigned int nbBytes)
{
    printf("Memory: Free: %d %x\n", nbBytes,p);
    if (p!=NULL)
    {
        free(p);

        Memory_nbBytesAllocated = Memory_nbBytesAllocated - nbBytes;
    }
}

void Memory_set(void* p, unsigned int val, unsigned int size)
{
   memset(p, (int)val, size);
}
