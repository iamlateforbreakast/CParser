/* OptionMgr.c */

#include "OptionMgr.h"

#include "Common.h"

OptionMgr* OptionMgr_new()
{
    OptionMgr* this;

    this = (OptionMgr*)Memory_alloc(sizeof(OptionMgr));

    return this;
}

void OptionMgr_delete(OptionMgr* this)
{
    Memory_free(this, sizeof(OptionMgr));
}
