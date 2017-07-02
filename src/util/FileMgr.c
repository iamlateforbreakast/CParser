/* FileMgr.c */

#include "FileMgr.h"

#include "List.h"
#include <unistd.h>
#include <string.h>
#include <dirent.h>

PRIVATE FileMgr* fileMgr = NULL;

typedef struct FileDesc
{
  String* name;
  String* fullName;
} FileDesc;

/**************************************************
**************************************************/
PRIVATE List* FileMgr_listFilesInDir(FileMgr* this);
PRIVATE List* FileMgr_listDirInDir(FileMgr* this);
PRIVATE void FileMgr_mergePath(String* path1, String* path2);

/**************************************************
**************************************************/
FileMgr* FileMgr_new()
{
    FileMgr* this;

    this = (FileMgr*)Memory_alloc(sizeof(FileMgr));

    this->files = NULL;
    this->activePath = NULL;
    this->activeDir = NULL;
    this->rootPath = NULL;
    return this;
}

/**************************************************
**************************************************/
void FileMgr_delete(FileMgr* this)
{
    Memory_free(this, sizeof(FileMgr));
}

/**************************************************
**************************************************/
String* FileMgr_load(FileMgr* this, String* fileName)
{
  String* fileContent=NULL;

  char buffer[255] = { 0 };

  fileContent = (String*)Memory_alloc(sizeof(String));
  memcpy(buffer, fileName->buffer, fileName->length);

  FILE* f=fopen(buffer,"rb");
  if (f)
  {
	  fseek(f, 0, SEEK_END);
	  fileContent->length=ftell(f);
	  fseek(f, 0 , SEEK_SET);

	  fileContent->buffer = (char*)Memory_alloc(fileContent->length);
    fread(fileContent->buffer,fileContent->length, 1, f);

	  fclose(f);
  }
  else
  {
	/* File cannot be found */
  }

  return fileContent;
}

/**************************************************
**************************************************/
void FileMgr_close(FileMgr* this, String* fileName)
{
}

/**************************************************
**************************************************/
FileMgr* FileMgr_getFileMgr()
{
  FileMgr* this;

  if (fileMgr==NULL)
  {
    this = FileMgr_new();
  }
  else
  {
    this = fileMgr;
  }
  this->refCount++;

  return this;
}

/**************************************************
**************************************************/
String* FileMgr_getCurrentDir(FileMgr* this)
{
  String* result = NULL;
  char buffer[255] = { 0 };

  if (getcwd(buffer, sizeof(buffer)) != NULL)
  {
    result=String_new(buffer);
    String_print(result, "Current Directory is ");
  }

  return result;
}

/**************************************************
**************************************************/
void FileMgr_initialise(FileMgr* this, String* initialPath)
{
  this->rootPath = FileMgr_getCurrentDir(this);
  // Merge current dir with initialPath
  FileMgr_mergePath(this->rootPath, initialPath);
  //this->files = FileMgr_listAllFiles(this);
  // change directory to inital path
}

/**************************************************
**************************************************/
List* FileMgr_listAllFiles(FileMgr* this)
{
  List* allFilesInDir = NULL;
  List* allDirInDir = NULL;
  List* result = NULL;
  String* currentDirName = NULL;

  if (this->activePath)
  {
    result = List_new();
    currentDirName = FileMgr_getCurrentDir(this);
    // List all files and add to list of all files
    allFilesInDir = FileMgr_listFilesInDir(currentDirName);
    List_merge(result, allFilesInDir);
    List_delete(allFilesInDir, NULL);
    // for each dir in list Dir call FileMgr_listAllFiles();
    allDirInDir = FileMgr_listDirInDir(this);
    // For each dir in allDirInDir
    // change dir to dir
    // list all files
    //List_delete(allDirInDir, NULL);
    closedir(this->activeDir);
  }
  else
  {
    printf("FileMgr.c: No current active directory\n");
  }

  return result;
}

/**************************************************
**************************************************/
void FileMgr_changeDirectory(FileMgr* this, String* newDir)
{
  List* result = NULL;
  DIR* d = NULL;
  struct dirent * dir; // for the directory entries
  String* directoryItem;
  String* newDirectory;
  char directory[255]={0};

  if (this->activeDir!=NULL)
  {
    closedir(this->activeDir);
  }

  if (newDir->buffer[0] == '/')
  {
    this->activePath = newDir;
    memcpy(directory, newDir->buffer, newDir->length);
    this->activeDir = opendir(directory);
  }
  else
  {
    String_cat(this->activePath, "/");
    String_append(this->activePath, newDir);
    memcpy(directory, this->activePath->buffer, this->activePath->length);
    this->activeDir = opendir(directory);
  }
  //this->activePath = FileMgr_getCurrentDir(this);
}

/**************************************************
**************************************************/
List* FileMgr_listFilesInDir(FileMgr* this)
{
  struct dirent *dir = NULL;
  FileDesc* fileDesc = NULL;
  List* result = NULL;
  String* directoryItem = NULL;

  result = List_new();

  while (((dir = readdir(this->activeDir)) != NULL) && (dir->d_type != DT_DIR))
  {
    fileDesc = Memory_alloc(sizeof(FileDesc));
    fileDesc->fullName =  String_dup(this->activePath);
    String_cat(fileDesc->fullName, "/");
    //String_append(fileDesc->fullName, fileName);
	  directoryItem=String_new(dir->d_name);

    List_insert(result, (void*)fileDesc);
  }

  return result;
}

/**************************************************
**************************************************/
List* FileMgr_listDirInDir(FileMgr* this)
{
  struct dirent *dir = NULL;
  FileDesc* fileDesc = NULL;
  List* result = NULL;
  String* directoryItem = NULL;

  result = List_new();

  while (((dir = readdir(this->activeDir)) != NULL) && (dir->d_type == DT_DIR))
  {
    fileDesc = Memory_alloc(sizeof(FileDesc));
    fileDesc->fullName =  String_dup(this->activePath);
    String_cat(fileDesc->fullName, "/");
    //String_append(fileDesc->fullName, fileName);
	  directoryItem=String_new(dir->d_name);

    List_insert(result, (void*)fileDesc);
  }

  return result;
}

/**************************************************
**************************************************/
void FileMgr_mergePath(String* path1, String* path2)
{
  #if 0
  String* result = NULL;
  String** path1Token = NULL;
  String** path2Token = NULL;

  unsigned int p1_idx = 0;
  unsigned int p2_idx = 0;

  path1Token = String_tokenize(path1,"/");
  path2Token = String_tokenize(path2,"/");

  for (p2_idx=0; p2_idx<sizeof(path2Token);p2_idx++)
  {
    if (String_cmp(path2[p2_idx], ".."))
    {}
    else if (String_cmp(path2[p2_idx], "."))
    {}
    else{
    }
  }
  #endif
}
