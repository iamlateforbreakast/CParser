/**********************************************//**
  @file FileMgr.c
  @brief TBD
**************************************************/

#include "FileMgr.h"

#include "Common.h"
#include "List.h"
#include <unistd.h>
#include <string.h>
#include <dirent.h>
//#include <stat.h>

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
PRIVATE String* FileMgr_getCurrentDir(FileMgr* this);
PRIVATE unsigned int FileMgr_matchWildcard(FileMgr* this, String* fileName, String* filter);
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
    Memory_free(this->files, sizeof(List));
    this->files = NULL;
    //String_delete(this->activePath);
    String_delete(this->rootPath);
    Memory_free(this, sizeof(FileMgr));
    this = NULL;
  }

}

/**************************************************
 @brief FileMgr_load
 
 TBD
 
 @param: TBD
 @return: TBD.
**************************************************/
PUBLIC String* FileMgr_load(FileMgr* this, String* fileName)
{
  String* fileContent=NULL;

  char buffer[255] = { 0 };

  // find filename in list of files
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
 @brief FileMgr_searchFile
 
 TBD
 
 @param: TBD
 @return: TBD.
**************************************************/
PUBLIC String* FileMgr_searchAndLoad(FileMgr* this, String* fileName)
{
  ListNode* pNode = NULL;
  String* result = NULL;
  
  if (this->files != NULL)
  {
    pNode = this->files->head;
  
    while(pNode!=NULL)
    {
      if (String_match(fileName, 0,((FileDesc*)pNode->item)->name) || String_match(fileName, 0,((FileDesc*)pNode->item)->fullName))
      {
        result =FileMgr_load(this,((FileDesc*)pNode->item)->fullName);
      }
      pNode = pNode->next;
    }
  }
  
  return result;
}

/**************************************************
 @brief FileMgr_getFiles
 
 TBD
 
 @param: TBD
 @return: TBD.
**************************************************/
PUBLIC List* FileMgr_getFiles(FileMgr* this)
{
  List* result = NULL;
  FileDesc* fd = NULL;
  
  if (this->files != NULL)
  {
    fd = (FileDesc*)List_getHead(this->files);
    result = List_new();
  
    while(fd!=NULL)
    {
      List_insert(result, fd->fullName);
      fd = List_getNext(this->files);
    }
    this->files->current = this->files->head;
  }
  
  return result;
}

/**************************************************
 @brief FileMgr_filterFiles
 
 TBD
 
 @param: TBD
 @return: TBD.
**************************************************/
PUBLIC List* FileMgr_filterFiles(FileMgr* this, String* filter)
{
  List* result = NULL;
  //ListNode* pNode = NULL;
  FileDesc* fd = NULL;
  
  if (this->files != NULL)
  {
    fd = (FileDesc*)List_getHead(this->files);
    result = List_new();
  
    while(fd!=NULL)
    {
      if (FileMgr_matchWildcard(this, fd->name, filter))
      {
        List_insert(result, fd->fullName);
      }
      fd = List_getNext(this->files);
    }
    this->files->current = this->files->head;
  }
  
  return result;
}

/**************************************************
 @brief FileMgr_close
 
 TBD
 
 @param: TBD
 @return: TBD.
**************************************************/
void FileMgr_close(FileMgr* this, String* fileName)
{
}

/**************************************************
 @brief FileMgr_getFileMgr
 
 TBD
 
 @param: TBD
 @return: TBD.
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
 @brief FileMgr_getCurrentDir
 
 TBD
 
 @param: TBD
 @return: TBD.
**************************************************/
PRIVATE String* FileMgr_getCurrentDir(FileMgr* this)
{
  String* result = NULL;
  char buffer[255] = { 0 };
  
  if (getcwd(buffer, sizeof(buffer)) != NULL)
  {
    result=String_new(buffer);
  }
  
  return result;
}

/**************************************************
**************************************************/
PUBLIC void FileMgr_initialise(FileMgr* this, String* initialPath)
{

  if (initialPath->buffer[0] == '/')
  {
    this->rootPath = String_dup(initialPath);
  }
  else
  {
    this->rootPath = FileMgr_getCurrentDir(this);
    // Merge current dir with initialPath
    FileMgr_mergePath(this, this->rootPath, initialPath);
  }
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
 @brief FileMgr_printAllFiles
 
 TBD
 
 @param: TBD
 @return: TBD.
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
 @brief FileMgr_getRootPath
 
 TBD
 
 @param: TBD
 @return: TBD.
**************************************************/
PUBLIC String* FileMgr_getRootPath(FileMgr* this)
{
  return String_dup(this->rootPath);
}

/**************************************************
 @brief FileMgr_listAllFiles
 
 TBD
 
 @param: TBD
 @return: TBD.
**************************************************/
PRIVATE List* FileMgr_listAllFiles(FileMgr* this)
{
  List* allFilesInDir = NULL;
  List* allDirInDir = NULL;
  List* result = NULL;
  ListNode* pNode = NULL;

  result = List_new();
  // List all files and add to list of all files
  allFilesInDir = FileMgr_listFilesInDir(this);
  List_merge(result, allFilesInDir);
  
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

  List_delete(allDirInDir, (void (*)(void *))&String_delete);
  
  closedir(this->activeDir);
  this->activeDir = NULL;

  return result;
}

/**************************************************
 @brief FileMgr_changeDirectory
 
 TBD
 
 @param: TBD
 @return: TBD.
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
 @brief FileMgr_listFilesInDir
 
 TBD
 
 @param: TBD
 @return: TBD.
**************************************************/
PRIVATE List* FileMgr_listFilesInDir(FileMgr* this)
{
  struct dirent *dir = NULL;
  FileDesc* fileDesc = NULL;
  List* result = NULL;
  
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
 @brief FileMgr_listDirInDir
 
 This function lists all directories in the 
 active directory.
 
 @param: none
 @return: List* - List of directories names.
**************************************************/
PRIVATE List* FileMgr_listDirInDir(FileMgr* this)
{
  struct dirent *dir = NULL;
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
  //String_print(path1, "FileMgr: active path ");
}

/**************************************************
 @brief FileMgr_matchWildcard
 
 This function matches a fileName with a wildcard
 pattern
 
 @param [in]     fileName: String* - fileName
 @param [in]     filter: String* - wildcard filter
 @return: unsigned int: 0 no match, 1 match
**************************************************/
PRIVATE unsigned int FileMgr_matchWildcard(FileMgr* this, String* fileName, String* wildcard)
{
  unsigned int isMatch = 1;
  unsigned int f_idx = 0;
  unsigned int w_idx = 0;
 
  for (w_idx = 0; (w_idx < wildcard->length) && (isMatch == 1); w_idx++)
  {
    if ((wildcard->buffer[w_idx]=='*') && (w_idx + 1 < wildcard->length))
    {  
      while ((f_idx < fileName->length) && (fileName->buffer[f_idx]!= wildcard->buffer[w_idx+1]))
      {
        f_idx++;
      }
      if (f_idx == fileName->length) isMatch = 0;
    }
    else if (wildcard->buffer[w_idx]=='?')
    {
    }
    else if (wildcard->buffer[w_idx]=='[')
    {
    }
    else if (wildcard->buffer[w_idx]==']')
    {
    }
    else if (fileName->buffer[f_idx]!= wildcard->buffer[w_idx])
    {
      isMatch = 0; 
    } else
    {
      if (f_idx < fileName->length) f_idx++;
    }
  }
  // Still some char in fileName not matched by filter
  if (f_idx < fileName->length) isMatch = 0;
  
  return isMatch;
}
