/* StringProcessor.c */

#include "StringProcessor.h"

#include "Common.h"

void StringProcessor_processDirective(StringProcessor* this);

StringProcessor* StringProcessor_new()
{
  StringProcessor* this;

  this=(StringProcessor*)Memory_alloc(sizeof(StringProcessor));
  return this;
}

void StringProcessor_delete(StringProcessor* this)
{
  Memory_free(this, sizeof(StringProcessor));
}

void StringProcessor_addFile(StringProcessor* this, const char* path, const char* fileName)
{
#if 0
  String* newCFile;
  StringBuffer* newBuffer;
  
  newCFile = FileMgr_loadFile(fileName);
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
