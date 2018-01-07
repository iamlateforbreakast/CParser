/* Object.c */

#include "Common.h"
#include "Object.h"

struct Object
{
  void (*delete)(Object* this);
  void (*copy)(Object* this);
};

Object* Object_new()
{
  Object* result = NULL;

  return result;
}

void Object_delete(Object* this)
{
}
