/* SdbMgr.h */

#ifndef _SDBMGR_H_
#define _SDBMGR_H_

#include "Common.h"

typedef struct SdbMgr SdbMgr;

SdbMgr* SdbMgr_new();
void SdbMgr_delete(SdbMgr* this);
SdbMgr* SdbMgr_getSdbMgr();
unsigned int SdbMgr_open(SdbMgr* this, String* sdbName);
unsigned int SdbMgr_execute(SdbMgr* this, const char* statement);
#endif