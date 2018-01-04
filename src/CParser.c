/**********************************************//**
  @file CParser.c
  
  @brief This file contains the implementation of the class CParser.
  
  The class CParser parses a directory and generates an SQLite database
  containing information about the source code contained in this directory.
**************************************************/

#include "Common.h"
#include "CParser.h"
#include "FileList.h"
#include "StringProcessor.h"
#include "Token.h"
#include "SdbMgr.h"
#include "FileMgr.h"
#include "List.h"
#include "Grammar.h"

struct CParser
{
  StringProcessor* stringProcessor;  /**< Reference to the associated StringProcessor object */
	Grammar* grammar;                      /**< Reference to the associated Grammar object */
	String* initialLocation;                    /**< Reference to the intial location */
	String* sdbName;                          /**<Reference to the database name */
	unsigned int isDbReset; 
};

/**************************************************
**************************************************/
PRIVATE void CParser_processFile(CParser* this, String* fileName);
PRIVATE void CParser_createTables(CParser* this, SdbMgr* sdbMgr);

/**************************************************
 @brief CParser_new
 
 This function allocate a new CParser object
 and return a pointer to it.
 
 @param: none
 @return: CParser* - Pointer to new object CParser
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
 
 This method frees a CParser object.
 
 @param: CParser* - Reference to a CParser object
 @return: none
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
  String* fileName = NULL;
  SdbMgr* sdbMgr = SdbMgr_getSdbMgr();
  FileMgr* fileMgr = FileMgr_getFileMgr();
  String* sdbCmd = NULL;
  String stringDirName = { .buffer = dirName, .length = strlen(dirName) };
  String sdbName = { .buffer = "TESTDB", .length = strlen("TESTDB") };
  String filter = { .buffer = "*.c", .length = 3 };
  List* l = NULL;
  List* filesInDir = NULL;
  
  
  // Initialise root location
  FileMgr_initialise(fileMgr, &stringDirName);
  this->initialLocation = FileMgr_getRootPath(fileMgr);
  
    // Open DB
  this->sdbName = &sdbName;
  SdbMgr_open(sdbMgr, this->sdbName);
  
  // Create DB tables
  CParser_createTables(this, sdbMgr);
  
  sdbCmd = String_sprint(this->initialLocation, "INSERT INTO Root_Location ( directory ) "
                                                "VALUES ('%s');");
  SdbMgr_execute(sdbMgr, sdbCmd->buffer);
  
  FileMgr_printAllFiles(fileMgr);
  filesInDir = FileMgr_getFiles(fileMgr);
  
  fileName = ((String*)List_getHead(filesInDir));
  while (fileName!=NULL)
  {
    sdbCmd = String_sprint(fileName, "INSERT INTO Files (name) VALUES ('%s');");
    SdbMgr_execute(sdbMgr, sdbCmd->buffer);
    String_delete(sdbCmd);
    fileName = ((String*)List_getNext(l));
  }
  
  l = FileMgr_filterFiles(fileMgr, &filter);
  cFileName = ((String*)List_getHead(l));
  while (cFileName!=NULL)
  {
    CParser_processFile(this, cFileName);
    cFileName = ((String*)List_getNext(l));
  }

  List_delete(l, (void (*)(void*))(NULL));
  List_delete(filesInDir, (void (*)(void*))(NULL));
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

  sprintf(cmd, "INSERT INTO Translation_Units ( id, name ) "
               "VALUES ('%d','%s');", fileId, name);

  fileId++;
  SdbMgr_execute(sdbMgr, cmd);
  SdbMgr_delete(sdbMgr);

  //Initialise from initial fileName  
  this->stringProcessor = StringProcessor_new(fileName);
    
  newToken = StringProcessor_getToken(this->stringProcessor);
  //printf("Token Id: %d\n", newToken->id);
  Grammar_pushToken(this->grammar, newToken);
    
  while (newToken->id!=TOK_EOF)
  {
    Token_delete(newToken);
    newToken = StringProcessor_getToken(this->stringProcessor);
    //printf("Token Id: %d\n", newToken->id);
    Grammar_pushToken(this->grammar, newToken);
  }
  /* Flush EOF token and reset grammar */
  //TODO: Not needed ? Grammar_pushToken(this->grammar, newToken);
  
  Token_delete(newToken);
  StringProcessor_delete(this->stringProcessor);
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
    SdbMgr_execute(sdbMgr, "DROP TABLE IF EXISTS Function_Declarations;");
    SdbMgr_execute(sdbMgr, "DROP TABLE IF EXISTS Variable_Declarations;");
    SdbMgr_execute(sdbMgr, "DROP TABLE IF EXISTS Type_Declarations;");
    SdbMgr_execute(sdbMgr, "DROP TABLE IF EXISTS Translation_Units;");
    SdbMgr_execute(sdbMgr, "DROP TABLE IF EXISTS Files;");
    SdbMgr_execute(sdbMgr, "DROP TABLE IF EXISTS Nodes;");
    SdbMgr_execute(sdbMgr, "DROP TABLE IF EXISTS Includes;");
    SdbMgr_execute(sdbMgr, "DROP TABLE IF EXISTS Comments;");
    SdbMgr_execute(sdbMgr, "DROP TABLE IF EXISTS Defines;");
  }
  SdbMgr_execute(sdbMgr, "CREATE TABLE Root_Location ( "
                         "directory text NOT NULL "
                         ");");
  SdbMgr_execute(sdbMgr, "CREATE TABLE Files ( "
                         "id integer PRIMARY KEY, "
                         "name text NOT NULL "
                         ");");
  SdbMgr_execute(sdbMgr, "CREATE TABLE Function_Declarations ("
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
  SdbMgr_execute(sdbMgr, "CREATE TABLE Type_Declarations ("
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
  SdbMgr_execute(sdbMgr, "CREATE TABLE Variable_Declarations ("
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
  SdbMgr_execute(sdbMgr, "CREATE TABLE Type_Declarations ("
                         "id integer PRIMARY_KEY, "
                         "name text NOT NULL, "
                         "scope text NOT NULL, "
                         "rtype_rank integer, "
                         "rtype_id integer, "
                         "fname text NOT NULL, "
                         "line integer NOT NULL, "
                         "column integer NOT NULL"
                         ");");
  SdbMgr_execute(sdbMgr, "CREATE TABLE Translation_Units ("
                         "id integer PRIMARY_KEY, "
                         "file_id integer"
                         ");");
  SdbMgr_execute(sdbMgr, "CREATE TABLE Includes ("
                         "id integer PRIMARY_KEY, "
                         "type integer,"
                         "file_id integer"
                         ");");
  SdbMgr_execute(sdbMgr, "CREATE TABLE Nodes ("
                         "id integer PRIMARY_KEY, "
                         "type_id integer,"
                         "item_id integer,"
                         "tu_id integer,"
                         "file_id,"
                         "next_id integer"
                         ");");
  SdbMgr_execute(sdbMgr, "CREATE TABLE Comments ("
                         "id integer PRIMARY_KEY,"
                         "type_id integer,"
                         "content text NOT NULL"
                         ");");
  SdbMgr_execute(sdbMgr, "CREATE TABLE Defines ("
                         "id integer PRIMARY KEY,"
                         "name text NOT NULL,"
                         "body text NOT NULL"
                         ");");
}
