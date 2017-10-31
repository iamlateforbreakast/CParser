/* StringProcessor.c */

#include "StringProcessor.h"
#include "FileMgr.h"

#include "Common.h"

#include <string.h>

typedef enum{
  E_INCLUDE=0,
  E_DEFINE,
  E_ENDIF,
  E_IF,
  E_ELSE
} PreprocessorDirective;

unsigned int  StringProcessor_processDirective(StringProcessor* this);
unsigned int StringProcessor_readFileName(StringProcessor* this, String** includeFileName);
unsigned int StringProcessor_readSpaces(StringProcessor* this);
void StringProcessor_readDefine(StringProcessor* this);
void StringProcessor_openNewBufferFromFile(StringProcessor* this, String* fileName);
PRIVATE unsigned int StringProcessor_isIncFileIgnored(StringProcessor* this, String* fileName);
PRIVATE unsigned int StringProcessor_evaluateCondition(StringProcessor* this);
PRIVATE unsigned int StringProcessor_processComment(StringProcessor* this);
/**************************************************
**************************************************/
static const String incFilesToIgnore[] = { { .buffer="stdio.h", .length=7 },
                                           { .buffer="string.h", .length=8 },
                                           { .buffer="stdlib.h", .length=8 },
                                           { .buffer="errno.h", .length=7 },
                                           { .buffer="unistd.h", .length=8 } };

static const String includeToken = { .buffer="#include", .length=8 };
static const String defineToken = { .buffer="#define", .length=7 };
static const String ifdefToken = { .buffer="#ifdef", .length=6 };
static const String ifndefToken = { .buffer="#ifndef", .length=7 };
static const String endifToken = { .buffer="#endif", .length=6 };
static const String elifToken = { .buffer="#elif", .length=5 };
static const String quoteToken = { .buffer="\"", .length=1 };
static const String bracketOpenToken = { .buffer="<", .length=1 };
static const String bracketCloseToken = { .buffer=">", .length=1 };
 
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

Token* StringProcessor_getTokenFromTransUnit(TokenList* this)
{
  unsigned char c = 0;
  Token* nextToken = NULL;
  String* fName = NULL;
  unsigned int line = 0;
  unsigned int col = 0;
  
  while (nextToken==NULL)
  {
    c = StringProcessor_readTransUnitChar();
    
    if (c=='/)
    {
        d = StringBuffer_peekChar(this->currentBuffer);
        if (d=='/')
        {
          //readSingleLineComment
        }
        else if (d=='*')
        {
          //readMultiLineComment
        }
    }
    else if (c=='#')
    {
    }
    else 
    {
    }
    nextToken = TokenList_checkKeyword(this);

    if (nextToken==NULL) nextToken = TokenList_checkIntegerConstant(this);

    if (nextToken==NULL) nextToken = TokenList_checkIdentifier(this);

    if (nextToken==NULL)
    {
      c = StringProcessor_readTransUnitChar(this->stringProcessor, fName, &line, &col);
      if (c==0)
      {
        nextToken = Token_new(TOK_EOF, "EOF", 0, NULL, line, col);
        //printf("Read char EOF\n");
      }
      else if ((c!=10) && (c!=32) && (c!=13))
      {
        nextToken = Token_new(TOK_UNKNOWN, "UNKOWN", (void*)((intptr_t)c), NULL, line, col);
        //printf("Accepted: Read char: %c %d\n",c,c);
      }
      else
      {
        //nextToken = NULL;
        //printf("Ignore: Read char: %c %d\n",c,c);
      }
    }
  }

  return nextToken;
}

/**************************************************
**************************************************/
unsigned char StringProcessor_readTransUnitChar(StringProcessor* this, String* f, unsigned int *l, unsigned int *c)
{ 
  unsigned char current_c = 0;
  unsigned int isExit = 0;
  
  while ((!isExit) && (current_c==0))
  {
    current_c = 0;
    
    /* Check for End of current file */
    if (StringBuffer_isEOF(this->currentBuffer))
    {
      /* Are there more buffers to process? */
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
    else if (StringProcessor_processComment(this))
    {

    }
    else if (StringProcessor_processDirective(this))
    {
    }
    else
    {
      current_c = StringBuffer_readChar(this->currentBuffer);
    }
  }
  
  *l = this->currentBuffer->line;
  *c = this->currentBuffer->column;
  f = this->currentBuffer->name;
  
  return current_c;
}

/**************************************************
**************************************************/
unsigned int  StringProcessor_processDirective(StringProcessor* this)
{
  unsigned int result = 0;
  unsigned char c = 0;
  String* includeFileName = NULL;
  
  // 1. If StringBuffer_compare(current, "#include") then
  if (StringProcessor_match(this, (String*)&includeToken))
  {
    // Read include file name
    result = result + StringProcessor_readSpaces(this);
    result = result + StringProcessor_match(this, (String*)&quoteToken) + StringProcessor_match(this, (String*)&bracketOpenToken);
    result = result + StringProcessor_readFileName(this, &includeFileName);
    result = result + StringProcessor_match(this, (String*)&quoteToken) + StringProcessor_match(this, (String*)&bracketCloseToken);
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
  else if (StringProcessor_match(this, (String*)&defineToken))
  {
    StringProcessor_readDefine(this);
  } 
  else if (StringProcessor_match(this, (String*)&ifdefToken))
  {  //       evaluate condition
  // 5. If StringBuffer_compare(current, "#endif") and isProcessingCondtion = TRUE
  }
  else if (StringProcessor_match(this, (String*)&ifndefToken))
  {  //       evaluate condition
    if (!StringProcessor_evaluateCondition(this))
	{
	  //this->isProcessingCondition = TRUE;
	}
	else
	{
	  // Jump to elif or endif
	  while (!StringProcessor_match(this, (String*)&endifToken))
	  {
	    c = StringBuffer_readChar(this->currentBuffer);
	  }
	}
  // 5. If StringBuffer_compare(current, "#endif") and isProcessingCondtion = TRUE
  } 
  else if (StringProcessor_match(this, (String*)&endifToken))
  {
    //       isProcessingCondition = FALSE
  }
  
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
  if (c=='0')
  {
    length++;
    c = StringBuffer_readChar(this->currentBuffer);
    c = StringBuffer_peekChar(this->currentBuffer);
    if (c=='x')
    {
      length++;
      c = StringBuffer_readChar(this->currentBuffer);
      c = StringBuffer_peekChar(this->currentBuffer);
      while ((c>='0' && c<='9')||(c>='A' && c<='F'))
      {
        length++;
        c = StringBuffer_readChar(this->currentBuffer);
        c = StringBuffer_peekChar(this->currentBuffer);
      }
    }
    else
    {
      while (c>='0' && c<='9')
      {
        length++;
        c = StringBuffer_readChar(this->currentBuffer);
        c = StringBuffer_peekChar(this->currentBuffer);
      }
    }
    result = StringBuffer_readback(this->currentBuffer, length);
  }
  else if (c>='0' && c<='9')
  {
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
  if (Map_find(this->macros, defineMacro, NULL))
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
      this->buffers[this->nbOpenBuffers] = StringBuffer_new(fileContent, fileName);
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
    if (String_match(fileName, 0, (String*)&(incFilesToIgnore[i]))) isFound = 1;
  }
  return isFound;
}

/**************************************************
**************************************************/
PRIVATE unsigned int StringProcessor_evaluateCondition(StringProcessor* this)
{
  unsigned int result = 0;
  unsigned char c = 0;
  String* macroName = NULL;
  
  c = StringBuffer_peekChar(this->currentBuffer);
  
  while (c==32)
  {
    c = StringBuffer_readChar(this->currentBuffer);
	c = StringBuffer_peekChar(this->currentBuffer);
  }
    
  while (((c>='a') && (c<='z')) || ((c>='A') && (c<='Z')) ||
         ((c>='0') && (c<='9')) || (c=='_'))
  {
    result++;
    c = StringBuffer_readChar(this->currentBuffer);
    c = StringBuffer_peekChar(this->currentBuffer);
  }
  
  macroName = StringBuffer_readback(this->currentBuffer, result);
  
  result = Map_find(this->macros, macroName, NULL);

  return result;
}

/**************************************************
**************************************************/
PRIVATE unsigned int StringProcessor_processComment(StringProcessor* this)
{
  unsigned int result = 0;
  String* singleLineComment = NULL;
  String* multiLineStartToken = NULL;
  String* multiLineEndToken = NULL;
  unsigned int isFound = 0;
  unsigned char c = 0;
  
  singleLineComment = String_new("//");
  multiLineStartToken = String_new("/*");
  multiLineEndToken = String_new("*/");
  
  if (StringProcessor_match(this, singleLineComment))
  {
    c = StringBuffer_readChar(this->currentBuffer);
      
    while (c!=10)
    {
      c = StringBuffer_readChar(this->currentBuffer);
    }
    result = 1;
  }
  else if (StringProcessor_match(this, multiLineStartToken))
  {
    isFound = StringProcessor_match(this, multiLineEndToken);
    while (!isFound)
    {
      c = StringBuffer_readChar(this->currentBuffer);
      isFound = StringProcessor_match(this, multiLineEndToken);
    }
    result = 1;
  }
  
  String_delete(singleLineComment);
  String_delete(multiLineStartToken);
  String_delete(multiLineEndToken);

  return result;
}