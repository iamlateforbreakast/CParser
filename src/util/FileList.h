/* FileList.h */

#ifndef _FILELIST_H_
#define _FILELIST_H_

#include "String.h"

typedef struct FileInfo FileInfo;

struct FileInfo
{
  String* name;
  String* path;
  //FileType type;
  //FileDate date;
  //FileSize size
  FileInfo* next;
};

typedef struct FileList
{
  FileInfo* head;
  FileInfo* currentFile;
} FileList;

FileList* FileList_new();
void FileList_delete(FileList* this);
void FileList_addFile(FileList* this, String* fileName, String* path);
void FileList_list(FileList* this, String* dirName, String* filter);
String* FileList_loadNextFile(FileList* this);
#endif
