/* CParser.c */

#include "Common.h"
#include "CParser.h"
#include "FileList.h"

CParser* cparser;

CParser* CParser_new()
{
  CParser* this;

  this=(CParser*)Memory_alloc(sizeof(CParser));


  this->fileList = FileList_new();
  this->fileMgr = FileMgr_new();

  return this;
}

void CParser_delete(CParser* this)
{
  
  TokenList_delete(this->tokenList);
      
  FileList_delete(this->fileList);
  printf("CParser_delete\n");
  FileMgr_delete(this->fileMgr);
  
  Memory_free(this, sizeof(CParser));

}

void CParser_parse(CParser* this, const char* dirName)
{
  String* newDirName=NULL;
  String* filter=NULL;
  String* cFileContent=NULL;
  int i;
  
  newDirName = String_new(dirName);
  filter = String_new(".c");

  // List all C files in newDirName and sub directories
  FileList_list(this->fileList, newDirName, filter);

  // for each C file add to the TokenList
  cFileContent = FileList_loadNextFile(this->fileList);
  printf("CParser.c: cFileContent %d\n", cFileContent->length);

  

  //while (cFileContent!=NULL)
  {
    //Initialise from initial fileName  
    this->tokenList = TokenList_new(cFileContent);
    
    while (TokenList_getTokenFromTransUnit(this->tokenList)!=0)
    {
    }
    
    TokenList_delete(this->tokenList);
    
    //cFileContent = FileList_loadNextFile(this->fileList);
    //close C file
    //cFileContent = FileMgr_load(this->fileMgr, FileList_getNextFileName(this->fileList));
    //Grammar_process()
  }

  /*while (TokensList_get())
	populate functions from list of tokens
	populate global vars
	populate local vars
	populate code blocks */

  //String_delete(cFileContent);
  String_delete(newDirName);
  String_delete(filter);
}

/*void CParser_populateFunctions()
{
    if Grammar_matchFunctions(element)
    {
    }
}

void CParser_populateGlobalVars()
{
}*/

FileMgr* CParser_getFileMgr()
{
    FileMgr* result=NULL;

    if (cparser!=NULL)
    {
	result = cparser->fileMgr;
    }

    return result;
}
