/* Memory.h */

#ifndef _MEMORY_
#define _MEMORY_

extern unsigned int Memory_nbBytesAllocated;

void* Memory_alloc(unsigned int nbBytes);
void Memory_free(void* pointer, unsigned int nbBytes);

#endif
