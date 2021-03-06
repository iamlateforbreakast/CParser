/* Memory.c */

#include "Common.h"

#include <string.h>

#define DEBUG (0)

unsigned int Memory_isTracingEnabled = 1;
unsigned int Memory_nbBytesAllocated = 0;
unsigned int Memory_maxNbBytesAllocated = 0;
unsigned int Memory_allocRequestId = 0;
unsigned int Memory_freeRequestId = 0;
void* Memory_maxAddress = (void*)0x0;
void* Memory_minAddress = (void*)0xFFFFFFFF;

void* Memory_alloc(unsigned int nbBytes)
{
    void* p = NULL;

    //printf("Malloc: %d\n", nbBytes);
    p = malloc(nbBytes);
    Memory_nbBytesAllocated += nbBytes;
    if (Memory_nbBytesAllocated>Memory_maxNbBytesAllocated) Memory_maxNbBytesAllocated = Memory_nbBytesAllocated;
    Memory_allocRequestId++;
    
    if (Memory_isTracingEnabled) 
    {
      TRACE(("Memory: Malloc %d|%d %p\n", Memory_allocRequestId, nbBytes,p));
    }
    if (p+nbBytes> (void*)Memory_maxAddress) Memory_maxAddress = p + nbBytes;
    return p;
}

void Memory_free(void* p, unsigned int nbBytes)
{
    
    if (p!=NULL)
    {
        Memory_freeRequestId++;
        if (Memory_isTracingEnabled) 
        {
          TRACE(("Memory: Free %d|%d %p\n", Memory_freeRequestId, nbBytes,p));
        }
        free(p);

        Memory_nbBytesAllocated = Memory_nbBytesAllocated - nbBytes;
        if (p<Memory_minAddress) 
        {
          Memory_minAddress = p;
        }
    }
}

void Memory_enableTracing(unsigned int isEnabled)
{
  if (isEnabled < 2) Memory_isTracingEnabled = isEnabled;
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
  printf("Lower Address %p\n", Memory_minAddress);
  printf("Upper Address %p\n", Memory_maxAddress);
}