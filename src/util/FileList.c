/* FileList.c */

#include "FileList.h"

#include "Common.h"

#include <dirent.h> 
#include <stdio.h> 
#include <string.h>

FileList* FileList_new()
{
    FileList* this;

    this=(FileList*)Memory_alloc(sizeof(FileList));

    return this;
}

void FileList_delete(FileList* this)
{
    Memory_free(this, sizeof(FileList));
}

void FileList_list(FileList* this, const char* dirName, const char* filter)
{
}
