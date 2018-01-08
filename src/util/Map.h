/* Map.h */

#ifndef _MAP_H_
#define _MAP_H_

#include "List.h"
#include "String2.h"

#define HTABLE_SIZE (50)

typedef struct Entry
{
  String* s;
  void* p;
  List* l;
} Entry;

typedef struct Map
{
  Entry htable[HTABLE_SIZE];
} Map;

Map* Map_new();
void Map_delete(Map* this);
unsigned int Map_insert(Map* this,String* s, void* p);
unsigned int Map_hash(Map* this, String* s, unsigned int i);
unsigned int Map_find(Map* this, String* s, void** p);
#endif
