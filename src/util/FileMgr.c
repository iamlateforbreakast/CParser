/* FileMgr.c */

#include "FileMgr.h"

#include <unistd.h>
#include <string.h>

static FileMgr* fileMgr = NULL;

typedef struct FileDesc
{
  String* name;
  String* fullName;
} FileDesc;

List* FileMgr_listFilesInDir(FileMgr* this);
List* FileMgr_listDirInDir(FileMgr* this);

/**************************************************
**************************************************/
FileMgr* FileMgr_new(String* rootPath)
{
    FileMgr* this;

    this = (FileMgr*)Memory_alloc(sizeof(FileMgr));

    this->files = List_new();
    this->activeDir = NULL;
    this->rootPath = String_dup(rootPath);
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
void FileMgr_listAllFiles(FileMgr* this, String* newDir)
{ 
  List* allFilesInDir = NULL;
  List* allDirInDir = NULL;
  
  allFilesInDir = List_new();
  // ChangeDir to newDir
  /* TBD: FileMgr_changeDirectory(root + newDir);*/
  // List all files and add to list of all files
  allFilesInDir = FileMgr_listFilesInDir(this);
  List_merge(this->files, allFilesInDir);
  List_delete(allFilesInDir, NULL);
  // for each dir in list Dir call FileMgr_listAllFiles();
  allDirInDir = FileMgr_listDirInDir(this);
  List_iterator(allDirInDir, &FileMgr_listAllFiles);
  List_delete(allDirInDir, NULL);
  closedir(this->activeDir);
}

/**************************************************
**************************************************/
void FileMgr_changeDirectory(FileMgr* this, String* newDir)
{
  char directory[255]={0};

  if (this->activeDir!=NULL)
  {
    closedir(this->activeDir);
  }
  memcpy(directory, newDir->buffer, newDir->length);
  this->activeDir = opendir(directory);
  this->activePath = newDir;
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
