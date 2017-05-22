/* SdbMgr.c */

#include "SdbMgr.h"

#include "Common.h"

SdbMgr* SdbMgr_new()
{
    SdbMgr* this;

    this = (SdbMgr*)Memory_alloc(sizeof(SdbMgr));

    return this;
}

void SdbMgr_delete(SdbMgr* this)
{
    Memory_free(this, sizeof(SdbMgr));
}
