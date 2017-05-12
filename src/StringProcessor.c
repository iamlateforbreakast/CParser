/* StringProcessor.c */

#include "StringProcessor.h"
#include "FileMgr.h"

#include "Common.h"

void StringProcessor_processDirective(StringProcessor* this);

StringProcessor* StringProcessor_new(String* initialFileContent)
{
  StringProcessor* this;
  StringBuffer* newBuffer;
  
  this=(StringProcessor*)Memory_alloc(sizeof(StringProcessor));
  memset(this->buffers, 0, sizeof(StringBuffer*) * NB_MAX_BUFFERS);
  
  newBuffer = StringBuffer_new(initialFileContent);
  this->buffers[0]  = newBuffer;
  
  this->currentBuffer = this->buffers[0];
  
  return this;
}

void StringProcessor_delete(StringProcessor* this)
{
  int i=0;
  //FileMgr*  f = CParser_getFileMgr();
  
  printf("StringProcessor.c: delete\n");
  
  for (i=0; i++; (this->buffers[i] == this->currentBuffer))
  {
    StringBuffer_delete(this->buffers[i]);
  }
  //StringBuffer_delete(this->currentBuffer);
  this->currentBuffer = NULL;
  //close all H files f.close
  Memory_free(this, sizeof(StringProcessor));
}

void StringProcessor_addFile(StringProcessor* this, String* file)
{
#if 0
  String* newCFile;
  StringBuffer* newBuffer;
  
  newBuffer = StringBuffer_new(newCFile);
  this->currentBuffer++;
  buffers[this->currentBuffer]  = newBuffer;
#endif
}

unsigned char StringProcessor_readTransUnitChar(StringProcessor* this)
{ 
  unsigned char c;

  // 1. Read current buffer char
  // 2. If char = '#' then process directive
  // 3. Check if macro used
  
  return c;
}

void StringProcessor_processDirective(StringProcessor* this)
{
  // 1. If StringBuffer_compare(current, "#include") then
  //       current = StringBuffer_new()
  // 2. If StringBuffer_compare(current, "#define") then
  //       read define defintion
  // 3. If StringBuffer_compare(current, "#ifdef")
  //       evaluate condition
  //       jump to appplicable code
  //       isProcessingCondition = TRUE;
  // 4. If StringBuffer_compare(current, "#elif") and isProcessingCondition = TRUE
  //       find "#endif"
  // 5. If StringBuffer_compare(current, "#endif") and isProcessingCondtion = TRUE
  //       isProcessingCOndtion = FALSE
}
