/* CParser.c */

#include "Common.h"
#include "CParser.h"
#include "FileList.h"
#include "TokenList.h"
#include "Token.h"
#include "SdbMgr.h"
#include "FileMgr.h"
#include "List.h"

CParser* cparser;

/**************************************************
**************************************************/
CParser* CParser_new()
{
  CParser* this;

  this=(CParser*)Memory_alloc(sizeof(CParser));

  this->fileList = FileList_new();
  this->sdbName = NULL;
  this->initialLocation = NULL;
  this->grammar = Grammar_new();
  
  return this;
}

/**************************************************
**************************************************/
void CParser_delete(CParser* this)
{ 
  FileList_delete(this->fileList);
  String_delete(this->sdbName);
  String_delete(this->initialLocation);
  Grammar_delete(this->grammar);
  Memory_free(this, sizeof(CParser));
}

/**************************************************
**************************************************/
void CParser_parse(CParser* this, const char* dirName)
{
  String* filter=NULL;
  //String* currPath = NULL;
  String* cFileName = NULL;
  Token* newToken = NULL;
  SdbMgr* sdbMgr = NULL;
  FileMgr* fileMgr = FileMgr_getFileMgr();
  String* sdbCmd = NULL;
  String stringDirName = { .buffer = dirName, .length = strlen(dirName) };
  
  // Initialise root location
  FileMgr_initialise(fileMgr, &stringDirName);
  this->initialLocation = fileMgr->rootPath;
  
  // Open DB
  this->sdbName = String_new("TESTDB");
  sdbMgr = SdbMgr_getSdbMgr();
  SdbMgr_open(sdbMgr, this->sdbName);
  
  SdbMgr_execute(sdbMgr, "CREATE TABLE Root_Location ( \
                         directory text NOT NULL \
                         )");
                         
  sdbCmd = String_sprint(this->initialLocation, "INSERT INTO Root_Location ( directory ) \
                                    VALUES ('%s')");
  /*SdbMgr_execute(sdbMgr, "INSERT INTO Root_Location ( directory )" \
                         "VALUES ('C:/\Temp/\CParser2')"); */
  String_delete(sdbCmd);
  
  // List all C files in newDirName and sub directories
  filter = String_new(".c");
  FileList_list(this->fileList, this->initialLocation, filter);

  // for each C file add to the TokenList
  cFileName = FileList_loadNextFile(this->fileList);

  while (cFileName!=NULL)
  {
    //Initialise from initial fileName  
    this->tokenList = TokenList_new(cFileName);
    
    newToken = TokenList_getTokenFromTransUnit(this->tokenList);
    //printf("Token Id: %d\n", newToken->id);
    Grammar_pushToken(this->grammar, newToken);
    
    while (newToken->id!=TOK_EOF)
    {
      Token_delete(newToken);
      newToken = TokenList_getTokenFromTransUnit(this->tokenList);
      //printf("Token Id: %d\n", newToken->id);
      Grammar_pushToken(this->grammar, newToken);
    }
    
    Token_delete(newToken);
    TokenList_delete(this->tokenList);
    
    cFileName = FileList_loadNextFile(this->fileList);
  }
  //String_delete(currPath);
  String_delete(filter);
  FileMgr_delete(fileMgr);
}