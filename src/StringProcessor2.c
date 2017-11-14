/* StringProcessor.c */

#include "StringProcessor.h"
#include "FileMgr.h"
#include "SdbMgr.h"
#include "Common.h"
#include "Token.h"
#include "List.h"

#include <stdint.h>

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
  TokenId token;
} Keyword;

typedef struct{
  String* name;
  String* body;
  List* parameters;
} MacroDefinition;

/**************************************************
**************************************************/
PRIVATE unsigned int  StringProcessor_processDirective(StringProcessor* this);
PRIVATE unsigned int StringProcessor_readFileName(StringProcessor* this, String** includeFileName);
PRIVATE void StringProcessor_readDefine(StringProcessor* this);
PRIVATE void StringProcessor_openNewBufferFromFile(StringProcessor* this, String* fileName);
PRIVATE unsigned int StringProcessor_isIncFileIgnored(StringProcessor* this, String* fileName);
PRIVATE unsigned int StringProcessor_evaluateCondition(StringProcessor* this);
PRIVATE unsigned int StringProcessor_processComment(StringProcessor* this);
PRIVATE unsigned char StringProcessor_readChar(StringProcessor* this, unsigned int consume);
PRIVATE unsigned int StringProcessor_readDirective(StringProcessor* this);
PRIVATE void StringProcessor_readSingleLineComment(StringProcessor* this);
PRIVATE void StringProcessor_readMultiLineComment(StringProcessor* this);
PRIVATE unsigned int StringProcessor_checkForMacro(StringProcessor* this, String* identifier);
PRIVATE Token* StringProcessor_checkKeyword(StringProcessor* this, String* identifier);
PRIVATE String* StringProcessor_readNumber(StringProcessor* this);
PRIVATE unsigned int StringProcessor_isLetter(StringProcessor* this, unsigned char c);
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
 
static const Keyword keywords[] = {{ .name = {.buffer="int", .length=3}, .token=TOK_INT },
                                                    { .name = {.buffer="float", .length=5}, .token=TOK_FLOAT },
                                                    {.name =  {.buffer="auto", .length=4}, .token=TOK_AUTO },
                                                    {.name = {.buffer="break", 5}, .token=TOK_BREAK },
                                                    {.name = {.buffer="case", 4}, .token=TOK_CASE },
                                                    {.name = {.buffer="char", 4 }, .token=TOK_CHAR },
                                                    {.name = {.buffer="const", 5}, .token=TOK_CONST },
                                                    {.name = {.buffer="continue", 8}, .token=TOK_CONTINUE },
                                                    {.name = {.buffer="default", 7}, .token=TOK_DEFAULT },
                                                    {.name = {.buffer="do", 2}, .token=TOK_DO },
                                                    {.name = {.buffer="double", 6}, .token=TOK_DOUBLE},
                                                    {.name = {.buffer="else", 4}, .token=TOK_ELSE},
                                                    {.name = {.buffer="enum", 4}, .token=TOK_ENUM},
                                                    {.name = {.buffer="extern", 6}, .token=TOK_EXTERN},
                                                    {.name = {.buffer="for", 3}, .token=TOK_FOR},
                                                    {.name = {.buffer="goto", 4}, .token=TOK_GOTO},
                                                    {.name = {.buffer="if", 2}, .token=TOK_IF},
                                                    {.name = {.buffer="inline", 6}, .token=TOK_INLINE},
                                                    {.name = {.buffer="long", 4}, .token=TOK_LONG},
                                                    {.name = {.buffer="register", 8}, .token=TOK_REGISTER},
                                                    {.name = {.buffer="restrict", 8}, .token=TOK_RESTRICT},
                                                    {.name = {.buffer="return", 6}, .token=TOK_RETURN},
                                                    {.name = {.buffer="short", 5}, .token=TOK_SHORT},
                                                    {.name = {.buffer="signed", 6}, .token=TOK_SIGNED},
                                                    {.name = {.buffer="sizeof", 6}, .token=TOK_SIZEOF},
                                                    {.name = {.buffer="static", 3}, .token=TOK_STATIC},
                                                    {.name = {.buffer="typedef", 7}, .token=TOK_TYPEDEF},
                                                    {.name = {.buffer="struct", 6}, .token=TOK_STRUCT},
                                                    {.name = {.buffer="union", 5}, .token=TOK_UNION},
                                                    {.name = {.buffer="unsigned", 8}, .token=TOK_UNSIGNED},
                                                    {.name = {.buffer="void", 4}, .token=TOK_VOID },
                                                    {.name = {.buffer="volatile", 8}, .token=TOK_VOLATILE },
                                                    {.name = {.buffer="while", 5}, .token=TOK_WHILE }};
                                                    
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
 @brief StringProcessor_delete
 
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
  
  //Map_delete(this->macros, (void(*)(void*))&String_delete);
  //close all H files f.close
  Memory_free(this, sizeof(StringProcessor));
}

/**************************************************
 @brief StringProcessor_getToken
 
 TBD
 
 @param: TBD
 @return: TBD.
**************************************************/
Token* StringProcessor_getToken(StringProcessor* this)
{
  unsigned char c = 0;
  unsigned char d = 0;
  Token* nextToken = NULL;
  String* identifier = NULL;
  String* number = NULL;
  unsigned int tmpInt = 0;
  
  while (nextToken==NULL)
  {
    c = StringProcessor_readChar(this,0);
    
    if (c==0)
    {
      nextToken = Token_new(TOK_EOF, "EOF", 0, NULL, 0, 0);
    }
    else if ((c==10) || (c==13))
    {
      c = StringProcessor_readChar(this,1);
    }
    else if (c==32)
    {
       c = StringProcessor_readChar(this,1);
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
      nextToken = StringProcessor_checkKeyword(this, identifier);
      if (nextToken == NULL)
      {
        (void)StringProcessor_checkForMacro(this, identifier);
      }
      else
      {
        String_delete(identifier);
      }
      // else create a token identifier
      if (nextToken == NULL)
      {
        nextToken = Token_new(TOK_IDENTIFIER, "IDENTIFIER", identifier, NULL, 0, 0);
      }
    }
    else if ((c=='+') || (c=='-') || ((c>='0') && (c<='9')) || (c=='\''))
    {
      number = StringProcessor_readNumber(this);
      tmpInt = String_toInt(number);
      nextToken = Token_new(TOK_CONSTANT, "CONSTANT", (void*)((uintptr_t)tmpInt), NULL, 0, 0);
    }
    else
    {
      c = StringProcessor_readChar(this,1);
      nextToken = Token_new(TOK_UNKNOWN, "UNKOWN", (void*)((intptr_t)c), NULL, 0, 0);
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
PRIVATE unsigned char StringProcessor_readChar(StringProcessor* this, unsigned int consume)
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
   if (consume)
   {
     current_c = StringBuffer_readChar(this->currentBuffer);
   }
   else
   {
     current_c = StringBuffer_peekChar(this->currentBuffer);
   }
  }
  
  return current_c;
}

/**************************************************
 @brief StringProcessor_readDirective
 
 TBD
 
 @param: TBD
 @return: TBD.
**************************************************/
unsigned int  StringProcessor_readDirective(StringProcessor* this)
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
  unsigned char c = 0;
  
  c = StringProcessor_readChar(this,1);
  c = StringProcessor_readChar(this,0);
  
  while ((c>='a' && c<='z') || (c>='A' && c <='Z') || (c>='0' && c<='9') ||(c=='_'))
   {
    length++;
    c = StringProcessor_readChar(this,1);
    c = StringProcessor_readChar(this,0);
  }
  result = StringBuffer_readback(this->currentBuffer, length);
  
  return result;
}

/**************************************************
 @brief StringProcessor_readNumber
 
 TBD
 
 @param: TBD
 @return: TBD.
**************************************************/
PRIVATE String* StringProcessor_readNumber(StringProcessor* this)
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
    if ((c=='x') || (c=='X'))
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
  String* parameterValue = NULL;
  String* parameterName = NULL;
  unsigned int paramLength = 0;
  unsigned char c = 0;
  MacroDefinition* macroDefinition = NULL;
  String* macroExpansion = NULL;
  
  // if identifier is a defined macro
  if  (Map_find(this->macros, identifier, (void**)&macroDefinition))
  {
    c = StringBuffer_peekChar(this->currentBuffer);
    if (c=='(')
    {
      c = StringBuffer_readChar(this->currentBuffer);
      c = StringBuffer_peekChar(this->currentBuffer);
      while (c!=')')
      {
        while ((c!=',') && (c!=')'))
        {
          paramLength++;
          c = StringBuffer_readChar(this->currentBuffer);
          c = StringBuffer_peekChar(this->currentBuffer);
        }
        parameterValue = StringBuffer_readback(this->currentBuffer, paramLength);
        parameterName = (String*)List_getNext(macroDefinition->parameters);
        //String_searchAndReplace(macroExpansion, macroDefinition.parameter[i], parameter);
        paramLength = 0;
        if (c==',')
        {
          c = StringBuffer_readChar(this->currentBuffer);
          c = StringBuffer_peekChar(this->currentBuffer);
        }
      }
      c = StringBuffer_readChar(this->currentBuffer);
    }
    //StringProcessor_openNewBufferFromString(this, macroExpansion);
  }
  return result;
}

/**************************************************
**************************************************/
Token* StringProcessor_checkKeyword(StringProcessor* this, String* identifier)
{
  Token* result=NULL;
  unsigned int i = 0;
  
  for (i=0; i<sizeof(keywords)/sizeof(Keyword); i++)
  {
    if (String_match(identifier, 0, &keywords[i].name))
    {
      result = Token_new(keywords[i].token, &keywords[i].name.buffer, 0, NULL, 0, 0);
      if (result)
      {
        i = sizeof(keywords)/sizeof(Keyword);
      }
    }
  }
  
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
  MacroDefinition* macroDefinition = NULL;
  String* parameter = NULL;
  unsigned char c = 0;
  unsigned int paramLength = 0;
  
  c = StringBuffer_peekChar(this->currentBuffer);
      
  while (c==32)
  {
    c = StringBuffer_readChar(this->currentBuffer);
    c = StringBuffer_peekChar(this->currentBuffer);
  }
    
  while (StringProcessor_isLetter(this, c))
  {
    result++;
    c = StringBuffer_readChar(this->currentBuffer);
    c = StringBuffer_peekChar(this->currentBuffer);
  }
  macroDefinition = Memory_alloc(sizeof(MacroDefinition));
  macroDefinition->name = StringBuffer_readback(this->currentBuffer, result);
  macroDefinition->parameters = NULL;
  macroDefinition->body = NULL;
  String_print(macroDefinition->name, "#define: ");

  if (c=='(')
  {
    c = StringBuffer_readChar(this->currentBuffer);
    c = StringBuffer_peekChar(this->currentBuffer);
    while (c!=')')
    {
      while ((c!=',') && (c!=')'))
      {
        paramLength++;
        c = StringBuffer_readChar(this->currentBuffer);
        c = StringBuffer_peekChar(this->currentBuffer);
      }
      if (macroDefinition->parameters == NULL)
      {
        macroDefinition->parameters = List_new();
      }
      parameter = StringBuffer_readback(this->currentBuffer, paramLength);
      paramLength = 0;
      
      List_insert(macroDefinition->parameters, parameter);
    }
    if (c==',')
    {
      c = StringBuffer_readChar(this->currentBuffer);
      c = StringBuffer_peekChar(this->currentBuffer);
    }
    
    c = StringBuffer_readChar(this->currentBuffer);
  }
  
  if (c!=10)
  {
    c = StringBuffer_peekChar(this->currentBuffer);
    while (c==32)
    {
      c = StringBuffer_readChar(this->currentBuffer);
      c = StringBuffer_peekChar(this->currentBuffer);
    }
    result =0;
  
    while (c!=10)
    {
      result++;
      c = StringBuffer_readChar(this->currentBuffer);
      c = StringBuffer_peekChar(this->currentBuffer);
    }
    //printf("Read define: result=%d\n", result);
    macroDefinition->body = StringBuffer_readback(this->currentBuffer, result);
    String_print(macroDefinition->body, "#define: ");
  }
  if (!Map_insert(this->macros, macroDefinition->name, (void*)macroDefinition))
  {
    String_print(macroDefinition->name, "StringProcessor.c: Could not store macro ");
  }
  if (Map_find(this->macros, macroDefinition->name, NULL))
  {
    String_print(macroDefinition->name, "StringProcessor.c: Found the macro again->");
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
void StringProcessor_openNewBufferFromString(StringProcessor* this, String* content)
{
  String* fileContent = NULL;
  
  if (this->nbOpenBuffers < NB_MAX_BUFFERS)
  {
    this->buffers[this->nbOpenBuffers] = StringBuffer_new(content, NULL);
    this->currentBuffer = this->buffers[this->nbOpenBuffers];

    this->nbOpenBuffers++;
  }
  else
  {
    //String_print(fileName, "StringProcessor.c: No available buffer to load ");
  }
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
  unsigned char c = 0;
  unsigned length = 2;
  
  c = StringProcessor_readChar(this,1);
  c = StringProcessor_readChar(this,0);
  
  while (c!=10)
  {
    length++;
    c = StringProcessor_readChar(this,1);
    c = StringProcessor_readChar(this,0);
  }
}

/**************************************************
**************************************************/
PRIVATE void StringProcessor_readMultiLineComment(StringProcessor* this)
{
  unsigned char c = 0;
  unsigned length = 2;
  unsigned int isFound = 0;
  
  c = StringProcessor_readChar(this,1);
  c = StringProcessor_readChar(this,0);
  
  while (!isFound)
  {
    if (c=='*')
    {
      length++;
      c = StringProcessor_readChar(this,1);
      c = StringProcessor_readChar(this,0);
      if (c=='/')
      {
        length++;
        c = StringProcessor_readChar(this,1);
        isFound = 1;
      }
    }
    else
    {
      length++;
      c = StringProcessor_readChar(this,1);
      c = StringProcessor_readChar(this,0);
    }
   
  }
}

/**************************************************
**************************************************/
PRIVATE unsigned int StringProcessor_isLetter(StringProcessor* this, unsigned char c)
{
  unsigned int result = 0;

  if (((c>='a') && (c<='z')) || ((c>='A') && (c<='Z')) || (c=='_'))
  {
    result = 1;
  }
  return result;
}

/**************************************************
**************************************************/
PRIVATE unsigned int StringProcessor_isDigit(StringProcessor* this, unsigned char c)
{
  unsigned int result = 0;

  if  ((c>='0') && (c<='9'))
  {
    result = 1;
  }
  
  return result;
}

/**************************************************
**************************************************/
PRIVATE unsigned int StringProcessor_isEndOfLine(StringProcessor* this, unsigned char c)
{
  unsigned int result = 0;
  
  return result;
}
