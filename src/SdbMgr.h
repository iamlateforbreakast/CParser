/* SdbMgr.h */

#ifndef _SDBMGR_H_
#define _SDBMGR_H_

#include "Common.h"

typedef struct SdbMgr SdbMgr;

PUBLIC void SdbMgr_delete(SdbMgr* this);
PUBLIC SdbMgr* SdbMgr_getSdbMgr();
PUBLIC unsigned int SdbMgr_open(SdbMgr* this, String* sdbName);
PUBLIC unsigned int SdbMgr_execute(SdbMgr* this, const char* statement);
PUBLIC unsigned int SdbMgr_getQueryCount(SdbMgr* this);
PUBLIC unsigned int SdbMgr_getQueryReady(SdbMgr* this);
PUBLIC PUBLIC char** SdbMgr_getQueryResult(SdbMgr* this);
#endif
