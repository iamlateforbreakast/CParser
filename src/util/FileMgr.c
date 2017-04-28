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

void FileMgr_load()
{
}

void FileMgr_close()
{
}
