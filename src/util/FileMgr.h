/* FileMgr.h */

#ifndef _FILEMGR_H_
#define _FILEMGR_H_

#include "Common.h"

#include "String2.h"
#include "List.h"

#include <dirent.h>

typedef struct FileMgr
{
  List* files;
  DIR* activeDir;
  unsigned int refCount;
  String* rootPath;
  String* activePath;
} FileMgr;

FileMgr* FileMgr_new();
void FileMgr_delete(FileMgr* this);
String* FileMgr_load(FileMgr* this, String* fileName);
void FileMgr_close(FileMgr* this, String* fileName);
FileMgr* FileMgr_getFileMgr();
String* FileMgr_getCurrentDir(FileMgr* this);
#endif
