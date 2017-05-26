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
  unsigned int refCount;
} FileMgr;

FileMgr* FileMgr_new();
void FileMgr_delete(FileMgr* this);
String* FileMgr_load(FileMgr* this, String* fileName);
void FileMgr_close(FileMgr* this, String* fileName);
FileMgr* FileMgr_getFileMgr();
#endif
