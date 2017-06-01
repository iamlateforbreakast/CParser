/* CParser.c */

#include "Common.h"
#include "CParser.h"
#include "FileList.h"
#include "TokenList.h"
#include "Token.h"
#include "SdbMgr.h"
#include "FileMgr.h"

CParser* cparser;

CParser* CParser_new()
{
  CParser* this;

  this=(CParser*)Memory_alloc(sizeof(CParser));

  this->fileList = FileList_new();
  this->sdbName = NULL;
  this->initialLocation = NULL;
  
  return this;
}

void CParser_delete(CParser* this)
{ 
  FileList_delete(this->fileList);
  String_delete(this->sdbName);
  String_delete(this->initialLocation);
  
  Memory_free(this, sizeof(CParser));

}

void CParser_parse(CParser* this, const char* dirName)
{
  String* filter=NULL;
  String* currPath = NULL;
  String* cFileContent=NULL;
  Token* newToken = NULL;
  SdbMgr* sdbMgr = NULL;
  FileMgr* fileMgr = FileMgr_getFileMgr();
  
  // Open DB
  this->sdbName = String_new("TESTDB");
  sdbMgr = SdbMgr_getSdbMgr();
  SdbMgr_open(sdbMgr, this->sdbName);
  
  // Initialise root location
  currPath = FileMgr_getCurrentDir(fileMgr);
  this->initialLocation = String_new(dirName);

  SdbMgr_execute(sdbMgr, "CREATE TABLE Root_Location (" \
                         "directory text NOT NULL" \
                         ")");
  SdbMgr_execute(sdbMgr, "INSERT INTO Root_Location ( directory )" \
                         "VALUES ('C:/\Temp/\CParser2')");

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
  String_delete(currPath);
  String_delete(filter);
  FileMgr_delete(fileMgr);
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
