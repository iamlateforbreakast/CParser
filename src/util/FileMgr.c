/* FileMgr.c */

#include "FileMgr.h"

FileMgr* FileMgr_new()
{
    FileMgr* this;

    this = (FileMgr*)Memory_alloc(sizeof(FileMgr*));

    return this;
}

void FileMgr_delete(FileMgr* this)
{
    Memory_free(this, sizeof(FileMgr));
}

String* FileMgr_load(FileMgr* this, String* fileName, String* path)
{
#if 0
  String* result = NULL;
  char* buffer = NULL;

  result = (String*)Memory_alloc(sizeof(String));

  FILE* f=fopen(fileName,"rb");
  if (f)
  {
	  fseek(f, 0, SEEK_END);
	  this->length=ftell(f);
	  fseek(f, 0 , SEEK_SET);
        
	  buffer = (char*)Memory_alloc(this->length);
	  fclose(f);
    this = String_new(buffer);
  }
  else
  {
	/* File cannot be found */
  }
#endif
}

void FileMgr_close(FileMgr* this, String* fileName)
{
}
