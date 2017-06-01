/* Memory.c */

#include "Common.h"

#include <string.h>

unsigned int Memory_nbBytesAllocated = 0;
unsigned int Memory_maxNbBytesAllocated = 0;
unsigned int Memory_allocRequestId = 0;
unsigned int Memory_freeRequestId = 0;
unsigned long long int Memory_minAddress = 0xFFFFFFFF;
unsigned long long int Memory_maxAddress = 0x0;

void* Memory_alloc(unsigned int nbBytes)
{
    void* p = NULL;

    //printf("Malloc: %d\n", nbBytes);
    p = malloc(nbBytes);
    Memory_nbBytesAllocated += nbBytes;
    if (Memory_nbBytesAllocated>Memory_maxNbBytesAllocated) Memory_maxNbBytesAllocated = Memory_nbBytesAllocated;
    Memory_allocRequestId++;
    
    //printf("Memory: Malloc %d|%d %x\n", Memory_allocRequestId, nbBytes,p);
    if (p+nbBytes> Memory_maxAddress) Memory_maxAddress = p + nbBytes;
    return p;
}

void Memory_free(void* p, unsigned int nbBytes)
{
    
    if (p!=NULL)
    {
        Memory_freeRequestId++;
        //printf("Memory: Free %d|%d %x\n", Memory_freeRequestId, nbBytes,p);
        free(p);

        Memory_nbBytesAllocated = Memory_nbBytesAllocated - nbBytes;
        if (p<Memory_minAddress) Memory_minAddress = p;
    }
}

void Memory_set(void* p, unsigned int val, unsigned int size)
{
   memset(p, (int)val, size);
}

void Memory_report()
{
	printf("Nb bytes not freed: %d\n", Memory_nbBytesAllocated);
  printf("Max nb bytes used: %d\n", Memory_maxNbBytesAllocated);
  printf("Nb alloc request %d\n", Memory_allocRequestId);
  printf("Nb free requests %d\n", Memory_freeRequestId);
  printf("Lower Address 0x%llx\n", Memory_minAddress);
  printf("Upper Address 0x%llx\n", Memory_maxAddress);
}