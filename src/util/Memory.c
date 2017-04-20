/* Memory.c */

#include "Common.h"

unsigned int Memory_nbBytesAllocated = 0;

void* Memory_alloc(unsigned int nbBytes)
{
    void* p = NULL;

    printf("Malloc: %d\n", nbBytes);
    p = malloc(nbBytes);
    Memory_nbBytesAllocated += nbBytes;

    return p;
}

void Memory_free(void* p, unsigned int nbBytes)
{
    if (p!=NULL)
    {
        printf("Free: %d\n", nbBytes);
        free(p);

        Memory_nbBytesAllocated = Memory_nbBytesAllocated - nbBytes;
    }
}
