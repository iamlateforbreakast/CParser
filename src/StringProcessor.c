/* StringProcessor.c */

#include "StringProcessor.h"
#include "FileMgr.h"

#include "Common.h"

#include <string.h>

unsigned int  StringProcessor_processDirective(StringProcessor* this);
unsigned int StringProcessor_readFileName(StringProcessor* this, String** includeFileName);
unsigned int StringProcessor_readSpaces(StringProcessor* this);
void StringProcessor_readDefine(StringProcessor* this);
void StringProcessor_openNewBufferFromFile(StringProcessor* this, String* fileName);
PRIVATE unsigned int StringProcessor_isIncFileIgnored(StringProcessor* this, String* fileName);

/**************************************************
**************************************************/
static const String incFilesToIgnore[] = { { .buffer="stdio.h", .length=7 },
                                           { .buffer="string.h", .length=8 },
                                           { .buffer="stdlib.h", .length=8 },
                                           { .buffer="errno.h", .length=7 },
                                           { .buffer="unistd.h", .length=8 } };

/**************************************************
@brief StringProcessor_new - TBD
 * 
 * @param[in]   initialFileName  : initial file name
 * @param[out]  None    : None
 *
 * @return TBD
**************************************************/
StringProcessor* StringProcessor_new(String* initialFileName)
{
  StringProcessor* this = NULL;
  
  this=(StringProcessor*)Memory_alloc(sizeof(StringProcessor));
  Memory_set(this->buffers, 0, sizeof(StringBuffer*) * NB_MAX_BUFFERS);

  this->currentBuffer = NULL;
  this->nbOpenBuffers = 0;  
  StringProcessor_openNewBufferFromFile(this, initialFileName);
  this->macros = Map_new();

  
  return this;
}

/**************************************************
**************************************************/
void StringProcessor_delete(StringProcessor* this)
{
  int i=0;
  //FileMgr*  f = CParser_getFileMgr();
  
  //printf("StringProcessor.c: delete\n");
  
  for (i=0; i<NB_MAX_BUFFERS; i++)
  {
    if (this->buffers[i]!=NULL) StringBuffer_delete(this->buffers[i]);
  }
  //StringBuffer_delete(this->currentBuffer);
  this->currentBuffer = NULL;
  Map_delete(this->macros, (void(*)(void*))&String_delete);
  //close all H files f.close
  Memory_free(this, sizeof(StringProcessor));
}

/**************************************************
**************************************************/
unsigned char StringProcessor_readTransUnitChar(StringProcessor* this)
{ 
  unsigned char current_c = 0;
  //unsigned char peek_c = 0;
  unsigned int isDirective = 1;
  unsigned int isExit = 0;
  
  if (StringBuffer_isEOF(this->currentBuffer))
  {
    // Are there more buffers to process?
    if (this->nbOpenBuffers>1)
    {
      printf("Finished this H file!\n");
      this->nbOpenBuffers--;
      StringBuffer_delete(this->currentBuffer);
      this->buffers[this->nbOpenBuffers] = NULL;
      this->currentBuffer = this->buffers[this->nbOpenBuffers-1];
    }
    else
    {
      printf("Changing C file!\n");
      isExit = 1;
    }
  }
  while ((isDirective) && (!isExit))
  {
    isDirective = StringProcessor_processDirective(this);
    //isDirective = isDirective + StringProcessor_checkForMacro(peek_c);
  }
  if (!isExit) current_c = StringBuffer_readChar(this->currentBuffer);
  
  return current_c;
}

/**************************************************
**************************************************/
unsigned int  StringProcessor_processDirective(StringProcessor* this)
{
  unsigned int result = 0;
  
  String* includeToken = NULL;
  String* defineToken = NULL;
  String* ifdefToken = NULL;
  String* endifToken = NULL;
  String* elifToken = NULL;
  String* quoteToken = NULL;
  String* bracketOpenToken = NULL;
  String* bracketCloseToken = NULL;
  String* includeFileName = NULL;
  
  includeToken = String_new("#include");
  defineToken = String_new("#define");
  ifdefToken = String_new("#ifdef");
  endifToken = String_new("#endif");
  elifToken = String_new("#elif");
  quoteToken = String_new("\"");
  bracketOpenToken = String_new("<");
  bracketCloseToken = String_new(">");
  
  // 1. If StringBuffer_compare(current, "#include") then
  if (StringProcessor_match(this, includeToken))
  {
    // Read include file name
    result = result + StringProcessor_readSpaces(this);
    result = result + StringProcessor_match(this, quoteToken) + StringProcessor_match(this, bracketOpenToken);
    result = result + StringProcessor_readFileName(this, &includeFileName);
    result = result + StringProcessor_match(this, quoteToken) + StringProcessor_match(this, bracketCloseToken);
    if (result)
    {
      if (!StringProcessor_isIncFileIgnored(this, includeFileName))
      {
        StringProcessor_openNewBufferFromFile(this, includeFileName);
      }
      String_delete(includeFileName);
    }
  } 
  // 2. If StringBuffer_compare(current, "#define") then  
  else if (StringProcessor_match(this, defineToken))
  {
    StringProcessor_readDefine(this);
  } 
  else if (StringProcessor_match(this, ifdefToken))
  {  //       evaluate condition
  // 5. If StringBuffer_compare(current, "#endif") and isProcessingCondtion = TRUE
  } 
  else if (StringProcessor_match(this, endifToken))
  {
    //       isProcessingCOndtion = FALSE
  }

  String_delete(includeToken);
  String_delete(defineToken);
  String_delete(ifdefToken);
  String_delete(endifToken);
  String_delete(elifToken);
  String_delete(quoteToken);
  String_delete(bracketOpenToken);
  String_delete(bracketCloseToken);
  
  return result;
}

/**************************************************
**************************************************/
String* StringProcessor_readIdentifier(StringProcessor* this)
{
  String* result = NULL;
  unsigned int length = 0;
  unsigned char c;
  
  c = StringBuffer_peekChar(this->currentBuffer);
  if ((c>='a' && c<='z') || (c>='A' && c <='Z') || (c=='_'))
  {
    length++;
    c = StringBuffer_readChar(this->currentBuffer);
    c = StringBuffer_peekChar(this->currentBuffer);
    while ((c>='a' && c<='z') || (c>='A' && c <='Z') || (c>='0' && c<='9') ||(c=='_'))
    {
      length++;
      c = StringBuffer_readChar(this->currentBuffer);
      c = StringBuffer_peekChar(this->currentBuffer);
    }
    result = StringBuffer_readback(this->currentBuffer, length);
  }
  
  return result;
}

/**************************************************
**************************************************/
String* StringProcessor_readInteger(StringProcessor* this)
{
  String* result = 0;
  unsigned int length = 0;
  unsigned char c;
  
  c = StringBuffer_peekChar(this->currentBuffer);

  if (c>='0' && c<='9')
  {
    length++;
    c = StringBuffer_readChar(this->currentBuffer);
    c = StringBuffer_peekChar(this->currentBuffer);
    while (c>='0' && c<='9')
    {
      length++;
      c = StringBuffer_readChar(this->currentBuffer);
      c = StringBuffer_peekChar(this->currentBuffer);
    }
    result = StringBuffer_readback(this->currentBuffer, length);
  }
  
  return result;
}

/**************************************************
**************************************************/
unsigned int StringProcessor_checkForMacro(StringProcessor* this)
{
  unsigned int result=0;
  
  return result;
}

/**************************************************
**************************************************/
unsigned int StringProcessor_match(StringProcessor* this, String* pattern)
{  
  return (StringBuffer_match(this->currentBuffer, pattern));
}

/**************************************************
**************************************************/
unsigned int StringProcessor_readSpaces(StringProcessor* this)
{
  unsigned int result = 0;
  unsigned char c = 0;
  
  c = StringBuffer_peekChar(this->currentBuffer);
  while ((c==32)||(c==10))
  {
    c = StringBuffer_readChar(this->currentBuffer);
    result++;
    c = StringBuffer_peekChar(this->currentBuffer);
  }
  
  return result;
}

/**************************************************
**************************************************/
unsigned int StringProcessor_readFileName(StringProcessor* this, String** includeFileName)
{
  unsigned int result = 0;
  unsigned char c = 0;
  String* fileName = NULL;
  
  c = StringBuffer_peekChar(this->currentBuffer);
  
  while (((c>='a') && (c<='z')) || ((c>='A') && (c<='Z')) ||
         ((c>='0') && (c<='9')) || (c=='_') || (c=='.') || 
          (c=='-') || (c=='/'))
  {
    result++;
    c = StringBuffer_readChar(this->currentBuffer);
    c = StringBuffer_peekChar(this->currentBuffer);
  }
  
  fileName = StringBuffer_readback(this->currentBuffer, result);
  String_print(fileName, "#include: ");
  
  *includeFileName = fileName;
  
  return result;
}

/**************************************************
**************************************************/
String* StringProcessor_getFileName(StringProcessor* this)
{
  return StringBuffer_getName(this->currentBuffer);
}

/**************************************************
**************************************************/
void StringProcessor_readDefine(StringProcessor* this)
{
  unsigned int result = 0;
  String* defineMacro = NULL;
  String* macroBody = NULL;
  unsigned char c = 0;
  
  //printf("Read define: result=%d\n", result);
  (void)StringProcessor_readSpaces(this);
  
  c = StringBuffer_peekChar(this->currentBuffer);
    
  while (((c>='a') && (c<='z')) || ((c>='A') && (c<='Z')) ||
         ((c>='0') && (c<='9')) || (c=='_'))
  {
    result++;
    c = StringBuffer_readChar(this->currentBuffer);
    c = StringBuffer_peekChar(this->currentBuffer);
  }
  

  
  defineMacro = StringBuffer_readback(this->currentBuffer, result);
  String_print(defineMacro, "#define: ");

  if (c!=10)
  {
    (void)StringProcessor_readSpaces(this);
    result =0;
  
    c = StringBuffer_peekChar(this->currentBuffer);
    
    while (c!=10)
    {
      result++;
      c = StringBuffer_readChar(this->currentBuffer);
      c = StringBuffer_peekChar(this->currentBuffer);
    }
    //printf("Read define: result=%d\n", result);
    macroBody = StringBuffer_readback(this->currentBuffer, result);
    String_print(macroBody, "#define: ");
  }
  if (!Map_insert(this->macros, defineMacro, (void*)macroBody))
  {
    String_print(defineMacro, "StringProcessor.c: Could not store macro ");
  }
  if (Map_find(this->macros, defineMacro))
  {
    String_print(defineMacro, "StringProcessor.c: Found the macro again->");
  }
}

/**************************************************
**************************************************/
void StringProcessor_openNewBufferFromFile(StringProcessor* this, String* fileName)
{
  String* fileContent = NULL;
  FileMgr* fileMgr = FileMgr_getFileMgr();
  
  if (this->nbOpenBuffers < NB_MAX_BUFFERS)
  {
    fileContent = FileMgr_searchAndLoad(fileMgr, fileName);
    if (fileContent!=NULL)
    {
      this->buffers[this->nbOpenBuffers] = StringBuffer_new(fileContent);
      this->currentBuffer = this->buffers[this->nbOpenBuffers];

      String_print(fileName, "Processing file ");
      printf("-----------------------------------------------\n");
      this->nbOpenBuffers++;
    }
    else
    {
      String_print(fileName,"StringProcessor.c: Cannot load ");
      String_print(fileName,"StringProcessor.c: Ignoring ");
    }      
  }
  else
  {
    String_print(fileName, "StringProcessor.c: No available buffer to load ");
  }
  
  FileMgr_delete(fileMgr);
}

/**************************************************
**************************************************/
PRIVATE unsigned int StringProcessor_isIncFileIgnored(StringProcessor* this, String* fileName)
{
  unsigned int isFound = 0;
  unsigned int i = 0;
  
  for (i=0; (i<sizeof(incFilesToIgnore)/sizeof(String) && (!isFound)); i++)
  {
    if (String_match(fileName, 0, &(incFilesToIgnore[i]))) isFound = 1;
  }
  return isFound;
}
