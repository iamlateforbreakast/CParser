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
PUBLIC FileMgr* FileMgr_new()
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
PUBLIC void FileMgr_delete(FileMgr* this)
{
    Memory_free(this, sizeof(FileMgr));
}

/**************************************************
**************************************************/
PUBLIC String* FileMgr_load(FileMgr* this, String* fileName)
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
PUBLIC FileMgr* FileMgr_getFileMgr()
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
PUBLIC void FileMgr_initialise(FileMgr* this, String* initialPath)
{
  this->rootPath = FileMgr_getCurrentDir(this);
  // Merge current dir with initialPath
  FileMgr_mergePath(this->rootPath, initialPath);
  // Parse all files contained in the root path
  //this->files = FileMgr_listAllFiles(this);
}

/**************************************************
**************************************************/
PUBLIC unsigned int FileMgr_isFile(FileMgr* this, String* fullFileName)
{
  unsigned int result = 0;
  
  #if 0
  DIR* directory = opendir(name)
  #endif
  return result;
}

/**************************************************
**************************************************/
PRIVATE List* FileMgr_listAllFiles(FileMgr* this)
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
    allFilesInDir = FileMgr_listFilesInDir(this);
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
PRIVATE List* FileMgr_listFilesInDir(FileMgr* this)
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
PRIVATE List* FileMgr_listDirInDir(FileMgr* this)
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
 @brief FileMgr_mergePath
 
 This function merges 2 paths into one.
 
 @param [in/out] path1: String* - Path to merge
 @param [in]     path2: String* - Path to merge
 @return: none
**************************************************/
PRIVATE void FileMgr_mergePath(String* path1, String* path2)
{
  unsigned int p2_idx = 0;
  unsigned int p1_idx = path1->length;
  char buffer[255];
  char* token = path2->buffer;
  
  // Initialise result buffer
  memset(buffer, 0, 255);
  memcpy(buffer, path1->buffer, p1_idx);
  
  buffer[p1_idx] = '/';
  p1_idx++;
  
  for (p2_idx=0; p2_idx<path2->length; p2_idx++)
  {
    if ((path2->buffer[p2_idx] == '/') || (p2_idx > (path2->length - 1)))
    {
      //Ignore ./ in path2
      if (memcmp(token, ".", 1) == 0)
      {
        p1_idx = p1_idx - 1;
        token = path2->buffer + p2_idx + 1;
      }
      //Take ../ into account
      else if (memcmp(token, "..", 2) == 0)
      {
        p1_idx = p1_idx - 3;
        while ((path1->buffer[p1_idx] != '/') && (p1_idx > 0))
        {
          p1_idx = p1_idx - 1;
        }
        p2_idx = p2_idx + 1;
        token = path2->buffer + p2_idx;
      }
      else
      {
        p2_idx = p2_idx + 1;
        token = path2->buffer + p2_idx;
      }
    }
    else
    {
      // Copy char from p2 to p1
      buffer[p1_idx] = path2->buffer[p2_idx];
      p1_idx++;
    }
  }
  
  Memory_free(path1->buffer, path1->length);
  path1->length = p1_idx;
  path1->buffer = Memory_alloc(path1->length);
  memcpy(path1->buffer, buffer, path1->length);
}
