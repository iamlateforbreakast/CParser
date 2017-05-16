/* StringProcessor.c */

#include "StringProcessor.h"
#include "FileMgr.h"

#include "Common.h"

#include <string.h>

void StringProcessor_processDirective(StringProcessor* this);

StringProcessor* StringProcessor_new(String* initialFileContent)
{
  StringProcessor* this = NULL;
  StringBuffer* newBuffer = NULL;
  
  this=(StringProcessor*)Memory_alloc(sizeof(StringProcessor));
  memset(this->buffers, 0, sizeof(StringBuffer*) * NB_MAX_BUFFERS);
  
  newBuffer = StringBuffer_new(initialFileContent);
  this->buffers[0] = newBuffer;
  
  this->currentBuffer = this->buffers[0];
  
  return this;
}

void StringProcessor_delete(StringProcessor* this)
{
  int i=0;
  //FileMgr*  f = CParser_getFileMgr();
  
  printf("StringProcessor.c: delete\n");
  
  for (i=0; i<NB_MAX_BUFFERS; i++)
  {
    if (this->buffers[i]!=NULL) StringBuffer_delete(this->buffers[i]);
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
  unsigned char current_c = 0;
  unsigned char peek_c = 0;
  
  if (!StringBuffer_isEOF(this->currentBuffer))
  {
    // 1. Read current buffer char
    current_c = StringBuffer_readChar(this->currentBuffer);
    // 2. If char = '#' then process directive
    if (current_c=='#')
    {
      StringProcessor_processDirective(this);
      // Move position after processing of directive
    }
    // 3. Check if a macro used
    peek_c = current_c;
    //while (StringProcessor_checkForMacro(peek_c))
    //{
    //  peek_c = StringBuffer_peekChar(this->currentBuffer);
    //}
  }
  
  return current_c;
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

unsigned int StringProcessor_checkForMacro(StringProcessor* this)
{
  unsigned int result=0;
  
  return result;
}

unsigned int StringProcessor_match(StringProcessor* this, String* pattern)
{  
  return (StringBuffer_match(this->currentBuffer, pattern));
}
