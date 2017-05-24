/* CParser.c */

#include "Common.h"
#include "CParser.h"
#include "FileList.h"
#include "TokenList.h"
#include "Token.h"
#include "SdbMgr.h"

CParser* cparser;

CParser* CParser_new()
{
  CParser* this;

  this=(CParser*)Memory_alloc(sizeof(CParser));


  this->fileList = FileList_new();
  this->fileMgr = FileMgr_new();
  this->sdbName = NULL;
  this->initialLocation = NULL;
  
  return this;
}

void CParser_delete(CParser* this)
{ 
  FileList_delete(this->fileList);
  String_delete(this->sdbName);
  String_delete(this->initialLocation);
  FileMgr_delete(this->fileMgr);
  
  Memory_free(this, sizeof(CParser));

}

void CParser_parse(CParser* this, const char* dirName)
{
  String* newDirName=NULL;
  String* filter=NULL;
  String* cFileContent=NULL;
  Token* newToken = NULL;
  SdbMgr* sdbMgr = NULL;
  
  // Open DB
  this->sdbName = String_new("TESTDB");
  sdbMgr = SdbMgr_getSdbMgr();
  SdbMgr_open(sdbMgr, this->sdbName);
  
  // Initialise root location
  this->initialLocation = String_new(dirName);
  SdbMgr_execute(sdbMgr, "CREATE TABLE Root_Location (directory text NOT NULL)");
  
  // List all C files in newDirName and sub directories
  filter = String_new(".c");
  FileList_list(this->fileList, this->initialLocation, filter);

  // for each C file add to the TokenList
  cFileContent = FileList_loadNextFile(this->fileList);
  printf("Processing C file %d\n", cFileContent->length);

  

  while (cFileContent!=NULL)
  {
    //Initialise from initial fileName  
    this->tokenList = TokenList_new(cFileContent);
    
    newToken = TokenList_getTokenFromTransUnit(this->tokenList);
    printf("Token Id: %d\n", newToken->id);
    
    while (newToken->id!=TOK_EOF)
    {
      Token_delete(newToken);
      newToken = TokenList_getTokenFromTransUnit(this->tokenList);
      printf("Token Id: %d\n", newToken->id);
    }
    
    Token_delete(newToken);
    TokenList_delete(this->tokenList);
    
    cFileContent = FileList_loadNextFile(this->fileList);
    //close C file
    //Grammar_process()
  }
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
