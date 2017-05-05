/* CParser.c */

#include "Common.h"
#include "CParser.h"
#include "FileList.h"

CParser* CParser_new()
{
  CParser* this;

  this=(CParser*)Memory_alloc(sizeof(CParser));

  this->tokenList = TokenList_new();
  this->fileList = FileList_new();
  this->fileMgr = FileMgr_new();

  return this;
}

void CParser_delete(CParser* this)
{
  TokenList_delete(this->tokenList);
  FileList_delete(this->fileList);
  FileMgr_delete(this->fileMgr);
  
  Memory_free(this, sizeof(CParser));
}

void CParser_parse(CParser* this, const char* dirName)
{
  String* newDirName;
  String* filter;
  String* cFileContent;

  newDirName = String_new(dirName);
  filter = String_new(".c");

  // List all C files in newDirName and sub directories
  FileList_list(this->fileList, newDirName, filter);

  // for each C file add to the TokenList
  cFileContent = FileMgr_load(FileList_getNextFileName(this->fileList));
  
  while (cFileContent!=NULL)
  {
    //Initialise from initial fileName
    TokenList_initialise(this->tokenList, newDirName);
    
    cFileContent = FileMgr_load(FileList_getNextFileName(this->fileList));
    //Grammar_process()
  }
  
  /*while (TokensList_get())
	populate functions from list of tokens
	populate global vars
	populate local vars
	populate code blocks */
  String_delete(cFileContent);
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
