/* SdbMgr.c */

#include "SdbMgr.h"

#include "Common.h"
#include <sqlite3.h>

struct SdbMgr
{
  String* name;
  sqlite3* db;
  unsigned int isQueryReady;
  unsigned int queryCount;
  char** queryResult;
  unsigned int refCount;
};

PRIVATE SdbMgr* sdbMgr = NULL;

PRIVATE void  SdbMgr_close(SdbMgr* this);

PRIVATE SdbMgr* SdbMgr_new()
{
    SdbMgr* this = NULL;

    this = (SdbMgr*)Memory_alloc(sizeof(SdbMgr));
    this->refCount = 0;
    this->name = NULL;
    this->db = NULL;
    this->queryCount = 0;
    this->isQueryReady = 0;
    this->queryResult = NULL;
    
    return this;
}

PUBLIC void SdbMgr_delete(SdbMgr* this)
{
    this->refCount = this->refCount - 1;
    
    if (this->refCount==0)
    {
      SdbMgr_close(this);
      this->name = NULL;
      this->db = NULL;
      this->isQueryReady = 0;
      this->queryCount = 0;
      this->queryResult = NULL;
      Memory_free(this, sizeof(SdbMgr));
    }
}

PUBLIC SdbMgr* SdbMgr_getSdbMgr()
{ 
  if (sdbMgr==NULL)
  {
    sdbMgr = SdbMgr_new();
  }
  sdbMgr->refCount++;
  
  return sdbMgr;
}

PUBLIC unsigned int SdbMgr_open(SdbMgr* this, String* sdbName)
{
  unsigned int result = 0;
  
  result = sqlite3_open(sdbName->buffer, &(this->db));
  this->name = String_dup(sdbName);
  (void)SdbMgr_execute(this, "PRAGMA synchronous=NORMAL;");
  return result;
}

PRIVATE void SdbMgr_close(SdbMgr* this)
{
  sqlite3_close(this->db);
  String_delete(this->name);
}
#if 0
unsigned int SdbMgr_execute(SdbMgr* this, const char* statement)
{
  unsigned int result = 0;
  char **errmsg = NULL;
  
  result = sqlite3_exec(this->db, statement, SdbMgr_execCallback, this, errmsg);
  if (result!=SQLITE_OK)
  {
    printf("SdbMgr.c: SQLITE_NOK\n");
    
    printf("SdbMgr.c: errmsg=%p\n", errmsg);
    //fprintf(stderr, "SQL error: %s\n", *errmsg);
    //sqlite3_free(*errmsg);
  }
  return result;
}
#endif

unsigned int SdbMgr_execute(SdbMgr* this, const char* statement)
{
  int rc = 0;
  sqlite3_stmt *res = NULL;
  const unsigned char *text = NULL;
  int step = 0;
  
  printf("SdbMgr: %s\n", statement);
  rc = sqlite3_prepare_v2(this->db, statement, -1, &res, 0);
  
  step = sqlite3_step(res);
  
  if (step == SQLITE_ROW)
  {
    //printf("SdbMgr: Query performed\n");
    text = sqlite3_column_text(res, 1);
    printf("SdbMgr: %s\n", text);
    sdbMgr->queryCount = 1;
  }
  sqlite3_finalize(res);
  
  return 0;
}

PUBLIC unsigned int SdbMgr_getQueryCount(SdbMgr* this)
{
  unsigned int result = 0;
  
  result = this->queryCount;
  
  return result;
}

PUBLIC unsigned int SdbMgr_getQueryReady(SdbMgr* this)
{
  unsigned int result = 0;
  
  result = this->isQueryReady;
  
  if (this->isQueryReady) this->isQueryReady = 0;
  
  return result;
}

PUBLIC char** SdbMgr_getQueryResult(SdbMgr* this)
{
  char** result = NULL;
  
  result = this->queryResult;
  
  this->queryResult = NULL;
  
  return result;
}