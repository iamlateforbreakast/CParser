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
PRIVATE void CParser_processFile(CParser* this, String* fileName);
PRIVATE void CParser_createTables(CParser* this, SdbMgr* sdbMgr);

/**************************************************
 @brief CParser_new
 
 TBD
 
 @param: TBD
 @return: TBD.
**************************************************/
PUBLIC CParser* CParser_new()
{
  CParser* this;

  this=(CParser*)Memory_alloc(sizeof(CParser));

  this->sdbName = NULL;
  this->initialLocation = NULL;
  this->grammar = Grammar_new();
  this->isDbReset = 1;
  return this;
}

/**************************************************
 @brief CParser_delete
 
 TBD
 
 @param: TBD
 @return: TBD.
**************************************************/
PUBLIC void CParser_delete(CParser* this)
{ 
  this->sdbName = NULL;
  String_delete(this->initialLocation);
  Grammar_delete(this->grammar);
  Memory_free(this, sizeof(CParser));
}

/**************************************************
 @brief CParser_parse
 
 TBD
 
 @param: TBD
 @return: TBD.
**************************************************/
PUBLIC void CParser_parse(CParser* this, char* dirName)
{
  String* cFileName = NULL;

  SdbMgr* sdbMgr = SdbMgr_getSdbMgr();
  FileMgr* fileMgr = FileMgr_getFileMgr();
  String* sdbCmd = NULL;
  String stringDirName = { .buffer = dirName, .length = strlen(dirName) };
  String sdbName = { .buffer = "TESTDB", .length = strlen("TESTDB") };
  String filter = { .buffer = "*.c", .length = 3 };
  List* l = NULL;
  ListNode* pNode, *pTemp = NULL;
  
  // Initialise root location
  FileMgr_initialise(fileMgr, &stringDirName);
  this->initialLocation = FileMgr_getRootPath(fileMgr);
  FileMgr_printAllFiles(fileMgr);

  
  // Open DB
  this->sdbName = &sdbName;
  SdbMgr_open(sdbMgr, this->sdbName);
  
  // Create DB tables
  CParser_createTables(this, sdbMgr);
  
  sdbCmd = String_sprint(this->initialLocation, "INSERT INTO Root_Location ( directory )"
                                                "VALUES ('%s')");
  SdbMgr_execute(sdbMgr, sdbCmd->buffer);
  
  
  
  String_delete(sdbCmd);
  
  l = FileMgr_filterFiles(fileMgr, &filter);
  pNode = l->head;
  while (pNode!=NULL)
  {
    cFileName = ((String*)pNode->item);
    CParser_processFile(this, cFileName);
    pTemp = pNode;
    pNode = pNode->next;
    Memory_free(pTemp, sizeof(ListNode));
    
  }

  Memory_free(l, sizeof(List));
  SdbMgr_delete(sdbMgr);
  FileMgr_delete(fileMgr);
}

/**************************************************
 @brief CParser_processFile
 
 TBD
 
 @param: TBD
 @return: TBD.
**************************************************/
PRIVATE void CParser_processFile(CParser* this, String* fileName)
{
  Token* newToken = NULL;
  SdbMgr* sdbMgr = SdbMgr_getSdbMgr();
  char cmd[255];
  char name[255];
  static unsigned int fileId = 0;
  
  memset(name, 0, 255);
  memset(cmd, 0, 255);
  memcpy(&name[0], &fileName->buffer[0], fileName->length);

  sprintf(cmd, "INSERT INTO Files ( id, name ) "
               "VALUES ('%d','%s');", fileId, name);

  fileId++;
  SdbMgr_execute(sdbMgr, cmd);
  SdbMgr_delete(sdbMgr);

  //Initialise from initial fileName  
  this->tokenList = TokenList_new(fileName);
    
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
}

/**************************************************
 @brief CParser_createTables
 
 TBD
 
 @param: TBD
 @return: TBD.
**************************************************/
PRIVATE void CParser_createTables(CParser* this, SdbMgr* sdbMgr)
{

  if (this->isDbReset)
  {
    SdbMgr_execute(sdbMgr, "DROP TABLE IF EXISTS Root_Location;");
    SdbMgr_execute(sdbMgr, "DROP TABLE IF EXISTS Declarations;");
    SdbMgr_execute(sdbMgr, "DROP TABLE IF EXISTS Files;");
  }
  SdbMgr_execute(sdbMgr, "CREATE TABLE Root_Location ( "
                         "directory text NOT NULL "
                         ");");
  SdbMgr_execute(sdbMgr, "CREATE TABLE Files ( "
                         "id integer PRIMARY KEY, "
                         "name text NOT NULL "
                         ");");
  SdbMgr_execute(sdbMgr, "CREATE TABLE Declarations ("
                         "id integer PRIMARY_KEY, "
                         "name text NOT NULL, "
                         "type text NOT NULL, "
                         "scope text NOT NULL, "
                         "rtype_rank integer, "
                         "rtype_id integer, "
                         "fname text NOT NULL, "
                         "line integer NOT NULL, "
                         "column integer NOT NULL"
                         ");");
                          
}