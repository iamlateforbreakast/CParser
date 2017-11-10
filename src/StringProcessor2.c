/* StringProcessor.c */

#include "StringProcessor.h"
#include "FileMgr.h"
#include "SdbMgr.h"
#include "Common.h"
#include "Token.h"

/**************************************************
**************************************************/
typedef enum{
  E_INCLUDE=0,
  E_DEFINE,
  E_ENDIF,
  E_IF,
  E_ELSE
} PreprocessorDirective;

typedef struct{
  String name;
  Token token;
} Keyword;

/**************************************************
**************************************************/
PRIVATE unsigned int  StringProcessor_processDirective(StringProcessor* this);
PRIVATE unsigned int StringProcessor_readFileName(StringProcessor* this, String** includeFileName);
PRIVATE void StringProcessor_readDefine(StringProcessor* this);
PRIVATE void StringProcessor_openNewBufferFromFile(StringProcessor* this, String* fileName);
PRIVATE unsigned int StringProcessor_isIncFileIgnored(StringProcessor* this, String* fileName);
PRIVATE unsigned int StringProcessor_evaluateCondition(StringProcessor* this);
PRIVATE unsigned int StringProcessor_processComment(StringProcessor* this);
PRIVATE unsigned char StringProcessor_readChar(StringProcessor* this);
PRIVATE unsigned int StringProcessor_readDirective(StringProcessor* this);
PRIVATE void StringProcessor_readSingleLineComment(StringProcessor* this);
PRIVATE void StringProcessor_readMultiLineComment(StringProcessor* this);
PRIVATE unsigned int StringProcessor_checkForMacro(StringProcessor* this, String* identifier);
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
 
static const Keyword keywords[] = {{ .name = {.buffer="int", .length=3}, .token=TOK_INT }};

                                                     {"float", 5}, TOK_FLOAT },
                                                     {"auto", 4}, TOK_AUTO },
                                                     {"break", 5}, TOK_BREAK },
                                                     {"case", 4}, TOK_CASE },
                                                     {"char", 4 }, TOK_CHAR },
                                                     {"const", 5}, TOK_CONST },
                                                     {"continue", 8}, TOK_CONTINUE },
                                                     {"default", 7}, TOK_DEFAULT },
                                                     {"do", 2}, TOK_DO },
                                                     {"double", 6}, TOK_DOUBLE},
                                                     {"else", 4}, TOK_ELSE},
                                                     {"enum", 4}, TOK_ENUM},
                                                     {"extern", 6}, TOK_EXTERN},
                                                     {"for", 3}, TOK_FOR},
                                                     {"goto", 4}, TOK_GOTO},
                                                     {"if", 2}, TOK_IF},
                                                     {"inline", 6}, TOK_INLINE},
                                                     {"long", 4}, TOK_LONG},
                                                     {"register", 8}, TOK_REGISTER},
                                                     {"restrict", 8}, TOK_RESTRICT},
                                                     {"return", 6}, TOK_RETURN},
                                                     {"short", 5}, TOK_SHORT},
                                                     {"signed", 6}, TOK_SIGNED},
                                                     {"sizeof", 6}, TOK_SIZEOF},
                                                     {"static", 3}, TOK_STATIC},
                                                     {"typedef", 7}, TOK_TYPEDEF},
                                                     {"struct", 6}, TOK_STRUCT},
                                                     {"union", 5}, TOK_UNION},
                                                     {"unsigned", 8}, TOK_UNSIGNED},
                                                     {"void", 4}, TOK_VOID },
                                                     {"volatile", 8}, TOK_VOLATILE },
                                                     {"while", 5}, TOK_WHILE }}; 
/**************************************************
@brief StringProcessor_new - TBD
 * 
 * @param[in]   initialFileName  : initial file name
 * @param[out]  None    : None
 *
 * @return TBD
**************************************************/
StringProcessor* StringProcessor_new(String* fileContent)
{
  StringProcessor* this = NULL;
  
  this=(StringProcessor*)Memory_alloc(sizeof(StringProcessor));
  Memory_set(this->buffers, 0, sizeof(StringBuffer*) * NB_MAX_BUFFERS);

  this->currentBuffer = NULL;
  this->nbOpenBuffers = 0;  
  StringProcessor_openNewBufferFromFile(this, fileContent);
  this->macros = Map_new();
  
  return this;
}

/**************************************************
 @brief StringProcessor_processDirective
 
 TBD
 
 @param: TBD
 @return: TBD.
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
 @brief StringProcessor_processDirective
 
 TBD
 
 @param: TBD
 @return: TBD.
**************************************************/
Token* StringProcessor_getToken(StringProcessor* this)
{
  unsigned char c = 0;
  unsigned char d = 0;
  Token* nextToken = NULL;
  String* fName = NULL;
  unsigned int line = 0;
  unsigned int col = 0;
  String* identifier = NULL;
  
  while (nextToken==NULL)
  {
    c = StringProcessor_readChar(this);
    
    if (c==0)
    {
      nextToken = Token_new(TOK_EOF, "EOF", 0, NULL, line, col);
    }
    else if ((c==10) || (c==13))
    {
      // Ignore
    }
    else if (c=='/')
    {
        d = StringBuffer_peekChar(this->currentBuffer);
        if (d=='/')
        {
          StringProcessor_readSingleLineComment(this);
        }
        else if (d=='*')
        {
          StringProcessor_readMultiLineComment(this);
        }
    }
    else if (c=='#')
    {
      if (StringProcessor_readDirective(this))
      {
      }
      else
      {
        //Create token
      } 
    }
    else if (((c>='a') && (c<='z')) || ((c>='A') && (c<='Z')) || (c=='_'))
    {
      identifier = StringProcessor_readIdentifier(this);
      // Check if it is a Macro call
      // else check it is a keyword
      
      // else create a token identifier
      nextToken = Token_new(TOK_IDENTIFIER, "IDENTIFIER", identifier, NULL, line, col);
    }
    else if ((c=='+') || (c=='-') || ((c>='0') && (c<='9')) || (c=='\''))
    {
      //StringProcessor_readNumber(this);
    }
    else
    {
      nextToken = Token_new(TOK_UNKNOWN, "UNKOWN", (void*)((intptr_t)c), NULL, line, col);
    }
  }

  return nextToken;
}

/**************************************************
 @brief StringProcessor_readChar
 
 TBD
 
 @param: TBD
 @return: TBD.
**************************************************/
PRIVATE unsigned char StringProcessor_readChar(StringProcessor* this)
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
   
   current_c = StringBuffer_readChar(this->currentBuffer);
  }
  
  return current_c;
}

/**************************************************
 @brief StringProcessor_processDirective
 
 TBD
 
 @param: TBD
 @return: TBD.
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
    //result = result + StringProcessor_readSpaces(this);
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
 @brief StringProcessor_readIdentifier
 
 TBD
 
 @param: TBD
 @return: TBD.
**************************************************/
String* StringProcessor_readIdentifier(StringProcessor* this)
{
  String* result = NULL;
  unsigned int length = 1;
  unsigned char c;
  
  c = StringBuffer_peekChar(this->currentBuffer);
  while ((c>='a' && c<='z') || (c>='A' && c <='Z') || (c>='0' && c<='9') ||(c=='_'))
   {
    length++;
    c = StringBuffer_readChar(this->currentBuffer);
    c = StringBuffer_peekChar(this->currentBuffer);
  }
  result = StringBuffer_readback(this->currentBuffer, length);
  
  return result;
}

/**************************************************
 @brief StringProcessor_readInteger
 
 TBD
 
 @param: TBD
 @return: TBD.
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
PRIVATE unsigned int StringProcessor_checkForMacro(StringProcessor* this, String* identifier)
{
  unsigned int result=0;
  
  return result;
}

/**************************************************
**************************************************/
unsigned int StringProcessor_checkKeyword(StringProcessor* this, String* identifier)
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
  //(void)StringProcessor_readSpaces(this);
  
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
    //(void)StringProcessor_readSpaces(this);
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
PRIVATE void StringProcessor_readSingleLineComment(StringProcessor* this)
{

}

/**************************************************
**************************************************/
PRIVATE void StringProcessor_readMultiLineComment(StringProcessor* this)
{

}

/**************************************************
**************************************************/
PRIVATE unsigned int StringProcessor_isLetter(StringProcessor* this, unsigned char c)
{
  unsigned int result = 0;

  return result;
}

/**************************************************
**************************************************/
PRIVATE unsigned int StringProcessor_isDigit(StringProcessor* this, unsigned char c)
{
  unsigned int result = 0;

  return result;
}

/**************************************************
**************************************************/
PRIVATE unsigned int StringProcessor_readDirective(StringProcessor* this)
{
  unsigned int result = 0;

  return result;
}