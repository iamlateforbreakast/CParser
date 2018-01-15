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
PUBLIC Object* Object_getRef(Object* this);
PUBLIC void Object_initialise(Object* this, void (*f_delete)(Object*), void (*f_copy)(Object*));

#endif
