/* Map.c */

#include "Map.h"

#include "Common.h"

Map* Map_new()
{
  Map* this = NULL;

  this = Memory_alloc(sizeof(Map));
  //memset((void*)this->htable, (int)0, (size_t)sizeof(this->htable));
  Memory_set(this->htable, 0, sizeof(this->htable));
  
  return this;
}

void Map_delete(Map* this, void (*f_delete)(void*))
{
  unsigned int i = 0;
  for (i=0; i<HTABLE_SIZE; i++)
  {
    if (this->htable[i].s!=NULL)
    {
      String_delete(this->htable[i].s);
      (*f_delete)(this->htable[i].p);
    }
  }
  Memory_free(this, sizeof(Map));
}

unsigned int Map_insert(Map* this,String* s, void* p)
{
  unsigned int result = 0;
  String* subString = NULL;
  unsigned int key = 0;
  unsigned int i = 0;
  
  for (i=1; (i<=s->length) && (result==0); i++)
  {
    subString = String_subString(s,0,i);
    key = Map_hash(this,subString, i);
    String_delete(subString);
    if (this->htable[key].p == NULL)
    {
      this->htable[key].s = s;
      this->htable[key].p = p;
      result = 1;
    }
    else if (i==s->length) 
    {
      printf("Map.c: Error: needs to go in list\n");
    }
    else
    {
    }
  }
  return result;
}

void* Map_find(Map* this, String* s)
{
  void* result = NULL;
  String* subString = NULL;
  unsigned int key = 0;
  unsigned int i = 0;
  
  for (i=1; i<=s->length; i++)
  { 
    subString = String_subString(s,0,i);
    key = Map_hash(this, subString, i);
    String_delete(subString);
    if (this->htable[key].s == NULL)
    {
      i = s->length+1; //Exit FOR loop
    }
    else
    {
      if (String_match(this->htable[key].s, 0, s))
      {
        result = this->htable[key].p;
      }
      else
      {
        // Carry on searching
      }
    }
  }
  
  return result;
}

unsigned int Map_hash(Map* this, String* s, unsigned int len)
{
  unsigned int result = 0;
  unsigned int i = 0;
  
  for (i=0; i<len; i++)
  {
    result = result + s->buffer[i];
  }
  result = result % HTABLE_SIZE;
  /*
          hash += key[i], hash += ( hash << 10 ), hash ^= ( hash >> 6 );
    }
    hash += ( hash << 3 ), hash ^= ( hash >> 11 ), hash += ( hash << 15 );
    */
  return result;
}