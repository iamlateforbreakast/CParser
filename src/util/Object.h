/* Object.h */

#ifndef _OBJECT_
#define _OBJECT_

#include "Common.h"

typedef struct Object Object;

struct Object
{
  void (*delete)(Object* this);
  void (*copy)(Object* this);
  unsigned int refCount;
};

PUBLIC Object* Object_new();
PUBLIC void Object_delete(Object* this);

#endif
