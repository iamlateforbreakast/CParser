/* FileList.c */

#include "FileList.h"
#include "FileMgr.h"

#include "Common.h"

#include <dirent.h> 
#include <stdio.h> 
#include <string.h>


FileList* FileList_new()
{
  FileList* this;

  this=(FileList*)Memory_alloc(sizeof(FileList));

  this->head=NULL;
  this->currentFile = this->head;
  
  return this;
}

void FileList_delete(FileList* this)
{
  FileInfo* fileInfo;
	
  while (this->head!=NULL)
  {
    fileInfo = this->head->next;
    String_delete(this->head->name);
    //printf("FileList.c: Free fullName %d\n", this->head->fullName->length);
    String_delete(this->head->fullName);
    //printf("FileList.c: Free fileInfo %x\n", this->head);
    Memory_free(this->head, sizeof(FileInfo));
    this->head = fileInfo;
  }
	
  Memory_free(this, sizeof(FileList));
}

void FileList_addFile(FileList* this, String* fileName, String* path)
{
  FileInfo* fileInfo;

  fileInfo = (FileInfo*)Memory_alloc(sizeof(FileInfo));
  fileInfo->name = fileName;

  fileInfo->fullName =  String_dup(path);
  String_cat(fileInfo->fullName, "/");
  String_append(fileInfo->fullName, fileName);
  String_print(fileInfo->fullName,"FileInfo fullname=");

  fileInfo->next = this->head;
  
  if (this->currentFile==this->head) this->currentFile=fileInfo;
  this->head = fileInfo;
}

void FileList_list(FileList* this, String* dirName, String* filter)
{
  DIR* d;
  struct dirent * dir; // for the directory entries
  String* directoryItem;
  String* newDirectory;
  char directory[255]={0};

  memcpy(directory, dirName->buffer, dirName->length);

  //printf("FileList_list: dirName->length=%d\n", dirName->length);
  d=opendir(directory);
  //String_print(dirName, "Looking in directory: ");

  if (d!=NULL)
    {
      while ((dir = readdir(d)) != NULL)
      {
        //printf("d_name: %d %s\n", strlen(dir->d_name), dir->d_name);
	      directoryItem=String_new(dir->d_name);
        if((dir->d_type != DT_DIR)&&(String_filter(directoryItem, filter))!=0)
        {
          FileList_addFile(this, directoryItem, dirName);
          String_print(directoryItem, "File: ");
        }
        else
        {
          if(dir -> d_type == DT_DIR 
            && String_cmp(directoryItem,".")==0 
            && String_cmp(directoryItem,"..")==0 )
          {
            newDirectory = String_dup(dirName);
            String_cat(newDirectory, "/");
            String_cat(newDirectory, dir->d_name);
            FileList_list(this, newDirectory, filter);	 

            String_delete(newDirectory);
          }
          String_delete(directoryItem);
        }   
      }
    closedir(d); // finally close the directory
	}
}

String* FileList_loadNextFile(FileList* this)
{
  FileInfo* currentFile = this->currentFile;
  String*   fileContent = NULL; 
  FileMgr*  f = FileMgr_getFileMgr();

  if (currentFile!=NULL)
  {
      String_print(currentFile->fullName, "Processing file ");
      printf("-----------------------------------------------\n");
      fileContent = FileMgr_load(f, currentFile->fullName);
      this->currentFile = this->currentFile->next;
  }

  return fileContent;
}
