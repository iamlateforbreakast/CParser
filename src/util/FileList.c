/* FileList.c */

#include "FileList.h"

#include "Common.h"

#include <dirent.h> 
#include <stdio.h> 


FileList* FileList_new()
{
    FileList* this;

    this=(FileList*)Memory_alloc(sizeof(FileList));

    return this;
}

void FileList_delete(FileList* this)
{
    FileInfo* fileInfo;
	
    while (this->head!=NULL)
    {
        fileInfo = this->head->next;
        Memory_free(this->head, sizeof(FileInfo));
        this->head = fileInfo;
    }
	
    Memory_free(this, sizeof(FileList));
}

void FileList_addFile(FileList* this, String* fileName)
{
    FileInfo* fileInfo;
	
    fileInfo = (FileInfo*)Memory_alloc(sizeof(FileInfo));
    fileInfo->name = fileName;
    
    fileInfo->next = this->head;
    this->head = fileInfo;

}

void FileList_list(FileList* this, String* dirName, String* filter)
{
   DIR* d;
   struct dirent * dir; // for the directory entries
   String* directoryItem;
   
   d=opendir(dirName->buffer);
   if (d!=NULL)
   {
       while ((dir = readdir(d)) != NULL)
       {
	  //printf("d_name: %d %s\n", strlen(dir->d_name), dir->d_name);
          directoryItem=String_new(dir->d_name);
          if((dir->d_type != DT_DIR)&&(String_filter(directoryItem, filter))!=0)
	  {
             FileList_addFile(this, directoryItem);
	     String_print(directoryItem, "File: ");
          }
	  else
	  {
             if(dir -> d_type == DT_DIR && String_cmp(directoryItem,".")!=0 && String_cmp(directoryItem,"..")!=0 )
             {
			     //String_cat(directoryItem, "/");
			     //String_cat(directoryItem, dir->d_name);
                 //FileList_list(this, directoryItem, filter);	 
             }
	     String_delete(directoryItem);
	  }

       }
       closedir(d); // finally close the directory
   }
}
