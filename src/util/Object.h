/* Object.h */

#ifndef _OBJECT_
#define _OBJECT_

typedef struct Object Object;

struct Object
{
  void (*delete)(Object* this);
  void (*copy)(Object* this);
};

Object* Object_new();
void Object_delete(Object* this);

#endif
