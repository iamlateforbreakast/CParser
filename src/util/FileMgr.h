/* FileMgr.h */

#ifndef _FILEMGR_H_
#define _FILEMGR_H_

#include "Common.h"

typedef struct FileDesc FileDesc;

struct FileDesc
{
  FileDesc* next;
};

typedef struct FileMgr
{
  FileDesc* openFiles;
} FileMgr;

FileMgr* FileMgr_new();
void FileMgr_delete(FileMgr* this);
String* FileMgr_load(FileMgr* this, String* fileName, String* path);
void FileMgr_close(FileMgr* this, String* fileName);

#endif
