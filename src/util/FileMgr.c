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

String* FileMgr_load(FileMgr* this, String* fileName)
{
  String* fileContent=NULL;

  String* result = NULL;
  char buffer[255] = { 0 };

  fileContent = (String*)Memory_alloc(sizeof(String));
  memcpy(buffer, fileName->buffer, fileName->length);
  
  FILE* f=fopen(buffer,"rb");
  if (f)
  {
	  fseek(f, 0, SEEK_END);
	  fileContent->length=ftell(f);
	  fseek(f, 0 , SEEK_SET);
        
	  fileContent->buffer = (char*)Memory_alloc(fileContent->length);
	  fclose(f);
  }
  else
  {
	/* File cannot be found */
  }

  return fileContent;
}

void FileMgr_close(FileMgr* this, String* fileName)
{
}
