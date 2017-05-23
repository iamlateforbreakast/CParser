/* Memory.c */

#include "Common.h"

#include <string.h>

unsigned int Memory_nbBytesAllocated = 0;
unsigned int Memory_maxNbBytesAllocated = 0;

void* Memory_alloc(unsigned int nbBytes)
{
    void* p = NULL;

    //printf("Malloc: %d\n", nbBytes);
    p = malloc(nbBytes);
    Memory_nbBytesAllocated += nbBytes;
    if (Memory_nbBytesAllocated>Memory_maxNbBytesAllocated) Memory_maxNbBytesAllocated = Memory_nbBytesAllocated;
    
    return p;
}

void Memory_free(void* p, unsigned int nbBytes)
{
    if (p!=NULL)
    {
        //printf("Free: %d\n", nbBytes);
        free(p);

        Memory_nbBytesAllocated = Memory_nbBytesAllocated - nbBytes;
    }
}

void Memory_set(void* p, unsigned int val, unsigned int size)
{
   memset(p, (int)val, size);
}
