/* FileMgr.h */

#ifndef _FILEMGR_H_
#define _FILEMGR_H_

#include "Common.h"

#include "String2.h"
#include "List.h"

#include <dirent.h>

typedef struct FileMgr FileMgr;

PUBLIC FileMgr* FileMgr_new();
PUBLIC void FileMgr_delete(FileMgr* this);
PUBLIC String* FileMgr_load(FileMgr* this, String* fileName);
PUBLIC void FileMgr_close(FileMgr* this, String* fileName);
PUBLIC FileMgr* FileMgr_getFileMgr();
PUBLIC void FileMgr_initialise(FileMgr* this, String* initialPath);
PUBLIC void FileMgr_printAllFiles(FileMgr* this);
PUBLIC String* FileMgr_getRootPath(FileMgr* this);
#endif
