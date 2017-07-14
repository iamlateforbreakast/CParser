/* SdbMgr.c */

#include "SdbMgr.h"

#include "Common.h"

static SdbMgr* sdbMgr = NULL;

void SdbMgr_close(SdbMgr* this);
static int SdbMgr_execCallback(void* this, int argc, char **argv, char **azColName);

SdbMgr* SdbMgr_new()
{
    SdbMgr* this = NULL;

    this = (SdbMgr*)Memory_alloc(sizeof(SdbMgr));
    this->refCount = 0;
    
    return this;
}

void SdbMgr_delete(SdbMgr* this)
{
    this->refCount = this->refCount - 1;
    
    if (this->refCount==0)
    {
      SdbMgr_close(this);
      Memory_free(this, sizeof(SdbMgr));
    }
}

SdbMgr* SdbMgr_getSdbMgr()
{
  SdbMgr* this = NULL;
  
  if (sdbMgr==NULL)
  {
    this = SdbMgr_new();
  }
  else
  {
    this = sdbMgr;
  }
  this->refCount++;
  
  return this;
}

unsigned int SdbMgr_open(SdbMgr* this, String* sdbName)
{
  unsigned int result = 0;
  
  result = sqlite3_open(sdbName->buffer, &(this->db));
  
  return result;
}

void SdbMgr_close(SdbMgr* this)
{
  sqlite3_close(this->db);
}

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

static int SdbMgr_execCallback(void* this, int argc, char **argv, char **azColName)
{
  printf("SdbMgr_execCallback: called!\n");
  
  return 0;
}