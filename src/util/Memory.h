/* Memory.h */

#ifndef _MEMORY_
#define _MEMORY_

void* Memory_alloc(unsigned int nbBytes);
void Memory_free(void* pointer, unsigned int nbBytes);
void Memory_set(void* p, unsigned int val, unsigned int size);
void Memory_report();
#endif
