/* SdbMgr.h */

#ifndef _SDBMGR_H_
#define _SDBMGR_H_

#include "Common.h"

#include <sqlite3.h>

typedef struct SdbMgr
{
  String* name;
  sqlite3* db;
  unsigned int refCount;
} SdbMgr;

SdbMgr* SdbMgr_new();
void SdbMgr_delete(SdbMgr* this);
SdbMgr* SdbMgr_getSdbMgr();
unsigned int SdbMgr_open(SdbMgr* this, String* sdbName);
unsigned int SdbMgr_execute(SdbMgr* this, const char* statement);
#endif