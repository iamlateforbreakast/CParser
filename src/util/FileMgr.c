/* FileMgr.c **************************************
**************************************************/

#include "FileMgr.h"

#include "Common.h"
#include "List.h"
#include <unistd.h>
#include <string.h>
#include <dirent.h>
//#include <stat.H>

PRIVATE FileMgr* fileMgr = NULL;

/**************************************************
**************************************************/
struct FileMgr
{
  List* files;
  DIR* activeDir;
  unsigned int refCount;
  String* rootPath;
  String* activePath;
};

typedef struct FileDesc
{
  String* name;
  String* fullName;
  // struct stat stat;
} FileDesc;

/**************************************************
**************************************************/
PRIVATE List* FileMgr_listAllFiles(FileMgr* this);
PRIVATE List* FileMgr_listFilesInDir(FileMgr* this);
PRIVATE List* FileMgr_listDirInDir(FileMgr* this);
PRIVATE void FileMgr_mergePath(FileMgr* this, String* path1, String* path2);
PRIVATE void FileMgr_changeDirectory(FileMgr* this, String* newDir);

/**************************************************
 @brief FileMgr_new
 
 This function allocate a FileMgr object.
 
 @param: none
 @return: FileMgr* - New allocated FileMgr pointer
**************************************************/
PUBLIC FileMgr* FileMgr_new()
{
    FileMgr* this;

    this = (FileMgr*)Memory_alloc(sizeof(FileMgr));

    this->files = NULL;
    this->activePath = NULL;
    this->activeDir = NULL;
    this->rootPath = NULL;
    this->refCount = 0;
    
    return this;
}

/**************************************************
**************************************************/
PUBLIC void FileMgr_delete(FileMgr* this)
{
  ListNode* pNode = NULL;
  
  this->refCount = this->refCount - 1;
  
  if (this->refCount==0)
  {
    // Only destroy the file list if not null
    if (this->files!=NULL)
    {
      pNode = this->files->head;
      while (pNode!=NULL)
      {
        this->files->head = pNode->next;
        String_delete(((FileDesc*)pNode->item)->name);
        String_delete(((FileDesc*)pNode->item)->fullName);
        Memory_free(pNode->item, sizeof(FileDesc));
        Memory_free(pNode, sizeof(ListNode));
        pNode = this->files->head;
      }
    }
    if (this->activeDir!=NULL)
    {
      closedir(this->activeDir);
    }
    //String_delete(this->activePath);
    String_delete(this->rootPath);
    Memory_free(this, sizeof(FileMgr));
  }

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
  if (fileMgr==NULL)
  {
    fileMgr = FileMgr_new();
  }

  fileMgr->refCount++;
  
  return fileMgr;
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
  FileMgr_mergePath(this, this->rootPath, initialPath);
  // Parse all files contained in the root path
  FileMgr_changeDirectory(this, this->rootPath);
  this->files = FileMgr_listAllFiles(this);
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
PUBLIC void FileMgr_printAllFiles(FileMgr* this)
{
  ListNode* pFile = NULL;
  
  printf("FileMgr: List all files\n");
  if (this->files!=NULL)
  {
    pFile = this->files->head;
    while (pFile!=NULL)
    {
      String_print(((FileDesc*)pFile->item)->fullName, "File ");
      pFile = pFile->next;
    }
  }
  else
  {
     printf("No files\n");
  }
}

/**************************************************
**************************************************/
PRIVATE List* FileMgr_listAllFiles(FileMgr* this)
{
  List* allFilesInDir = NULL;
  List* allDirInDir = NULL;
  List* result = NULL;
  ListNode* pNode = NULL;

  result = List_new();
  //currentDirName = FileMgr_getCurrentDir(this); <= incorrect
  // List all files and add to list of all files
  Memory_enableTracing(1);
  allFilesInDir = FileMgr_listFilesInDir(this);

  List_merge(result, allFilesInDir);
  //Memory_enableTracing(0);
  #if 0
  
  // for each dir in list Dir call FileMgr_listAllFiles();
  allDirInDir = FileMgr_listDirInDir(this);

  pNode = allDirInDir->head;
  while (pNode!=NULL)
  {
    FileMgr_changeDirectory(this, ((String*)pNode->item));
    allFilesInDir = FileMgr_listAllFiles(this);
    List_merge(result, allFilesInDir);
    pNode = pNode->next;
  }
   #endif 

  List_delete(allDirInDir, &String_delete);

  
  
  closedir(this->activeDir);

  return result;
}

/**************************************************
**************************************************/
PRIVATE void FileMgr_changeDirectory(FileMgr* this, String* newDir)
{

  char directory[255]={0};

  if (this->activeDir!=NULL)
  {
    closedir(this->activeDir);
    //String_delete(this->activePath);
  }
  if (newDir->buffer[0] == '/')
  {
    //this->activePath = String_dup(newDir)?;
    this->activePath = newDir;
    memcpy(directory, newDir->buffer, newDir->length);
    this->activeDir = opendir(directory);
  }
  else
  {
    printf("FileMgr.c: only accepting absolute path\n");
  }
}

/**************************************************
**************************************************/
PRIVATE List* FileMgr_listFilesInDir(FileMgr* this)
{
  struct dirent *dir = NULL;
  FileDesc* fileDesc = NULL;
  List* result = NULL;
  String* directoryItem = NULL;
  
  if (this->activeDir == NULL) String_print(this->activePath, "FileMgr: Unable to open directory ");
  result = List_new();
  
  while ((this->activeDir != NULL) && (dir = readdir(this->activeDir)) != NULL) 
  {
    if (dir->d_type != DT_DIR)
    {
      fileDesc = Memory_alloc(sizeof(FileDesc));
      fileDesc->name = String_new(dir->d_name);
      fileDesc->fullName = String_dup(this->activePath);
      FileMgr_mergePath(this, fileDesc->fullName, fileDesc->name);
      List_insert(result, (void*)fileDesc);
    } 
  }
  
  return result;
}


/**************************************************
**************************************************/
PUBLIC String* FileMgr_getRootPath(FileMgr* this)
{
  return String_dup(this->rootPath);
}

/**************************************************
**************************************************/
PRIVATE List* FileMgr_listDirInDir(FileMgr* this)
{
  struct dirent *dir = NULL;
  FileDesc* fileDesc = NULL;
  List* result = NULL;
  String* directoryItem = NULL;
  String* path = NULL;
  char directory[255] = { 0 };
  
  memcpy(directory, this->activePath->buffer, this->activePath->length);
  this->activeDir = opendir(directory);
  
  if (this->activeDir == NULL) String_print(this->activePath, "FileMgr: Unable to open directory ");
  result = List_new();
  
  while ((this->activeDir!=NULL) && (dir = readdir(this->activeDir)) != NULL)
  {
    if (dir->d_type == DT_DIR)
    {
      directoryItem=String_new(dir->d_name);
      if (!String_cmp(directoryItem,"..") && !String_cmp(directoryItem,"."))
      {
        path =  String_dup(this->activePath);
        FileMgr_mergePath(this, path, directoryItem);
        List_insert(result, (void*)path);
      }
      String_delete(directoryItem);
    }
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
PRIVATE void FileMgr_mergePath(FileMgr* this, String* path1, String* path2)
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
      //Take ../ into account
      if (memcmp(token, "..", 2) == 0)
      {
        p1_idx = p1_idx - 4;
        while ((path1->buffer[p1_idx] != '/') && (p1_idx > 0))
        {
          p1_idx = p1_idx - 1;
        }
        p1_idx = p1_idx + 1;
        token = path2->buffer + p2_idx + 1;
      }
      //Ignore ./ in path2
      else if (memcmp(token, ".", 1) == 0)
      {
        p1_idx = p1_idx - 1;
        token = path2->buffer + p2_idx + 1;
      }
      else
      {
        //p2_idx = p2_idx + 1;
        token = path2->buffer + p2_idx + 1;
        buffer[p1_idx] = '/';
        p1_idx++;
      }
    }
    else
    {
      // Copy char from p2 to p1
      buffer[p1_idx] = path2->buffer[p2_idx];
      p1_idx++;
    }
  }
  
  //Place merged path into path1
  Memory_free(path1->buffer, path1->length);
  path1->length = p1_idx;
  path1->buffer = Memory_alloc(path1->length);
  memcpy(path1->buffer, buffer, path1->length);
  String_print(path1, "FileMgr: active path ");
}
