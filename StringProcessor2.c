/**********************************************//**
  @file StringProcessor.c
  
  @brief TBD
  @details TBD
**************************************************/
#include "StringProcessor.h"
#include "FileMgr.h"
#include "SdbMgr.h"
#include "Common.h"
#include "Token.h"
#include "List.h"

#include <stdint.h>

/**************************************************
**************************************************/
/* typedef enum
{
  E_INCLUDE=0,
  E_DEFINE,
  E_ENDIF,
  E_IF,
  E_ELSE
} PreprocessorDirective; */

typedef struct
{
  char* name;
  TokenId token;
} Keyword;

typedef struct
{
  String* name;
  String* body;
  List* parameters;
} MacroDefinition;

/**************************************************
**************************************************/
PRIVATE void StringProcessor_readDefine(StringProcessor* this);
PRIVATE void StringProcessor_openNewBufferFromFile(StringProcessor* this, String* fileName);
PRIVATE unsigned int StringProcessor_isIncFileIgnored(StringProcessor* this, String* fileName);
PRIVATE unsigned int StringProcessor_evaluateCondition(StringProcessor* this);
PRIVATE unsigned char StringProcessor_readChar(StringProcessor* this, unsigned int consume);
PRIVATE unsigned int StringProcessor_readDirective(StringProcessor* this);
PRIVATE void StringProcessor_readSingleLineComment(StringProcessor* this);
PRIVATE void StringProcessor_readMultiLineComment(StringProcessor* this);
PRIVATE unsigned int StringProcessor_checkForMacro(StringProcessor* this, String* identifier);
PRIVATE Token* StringProcessor_checkKeyword(StringProcessor* this, String* identifier);
PRIVATE String* StringProcessor_readNumber(StringProcessor* this);
PRIVATE unsigned int StringProcessor_isLetter(StringProcessor* this, unsigned char c);
PRIVATE void StringProcessor_openNewBufferFromString(StringProcessor* this, String* content, String* bufferName);
PRIVATE void StringProcessor_deleteMacroDefinition(MacroDefinition* this);
PRIVATE void StringProcessor_readInclude(StringProcessor* this);
PRIVATE Token* StringProcessor_checkOpKeyword(StringProcessor* this);
PRIVATE unsigned int StringProcessor_isEndOfLine(StringProcessor* this, unsigned char c);
PRIVATE unsigned int StringProcessor_match(StringProcessor* this, String* pattern);
PRIVATE String* StringProcessor_readIdentifier(StringProcessor* this);
/**************************************************
**************************************************/
static const String incFilesToIgnore[] = { { .buffer="stdio.h", .length=7 },
                                           { .buffer="string.h", .length=8 },
                                           { .buffer="stdlib.h", .length=8 },
                                           { .buffer="errno.h", .length=7 },
                                           { .buffer="unistd.h", .length=8 } ,
                                           { .buffer="dirent.h", .length=8 }};

static const String includeToken = { .buffer="#include", .length=8 };
static const String defineToken = { .buffer="#define", .length=7 };
static const String ifdefToken = { .buffer="#ifdef", .length=6 };
static const String ifndefToken = { .buffer="#ifndef", .length=7 };
static const String endifToken = { .buffer="#endif", .length=6 };
static const String elifToken = { .buffer="#elif", .length=5 };
static const String elseToken = {.buffer="#else",.length=5};
 
static const Keyword keywords[] = {{ .name = "int", .token=TOK_INT },
                                   {.name = "float", .token=TOK_FLOAT },
                                   {.name = "auto", .token=TOK_AUTO },
                                   {.name = "break", .token=TOK_BREAK },
                                   {.name = "case", .token=TOK_CASE },
                                   {.name = "char", .token=TOK_CHAR },
                                   {.name = "const", .token=TOK_CONST },
                                   {.name = "continue", .token=TOK_CONTINUE },
                                   {.name = "default", .token=TOK_DEFAULT },
                                   {.name = "do", .token=TOK_DO },
                                   {.name = "double", .token=TOK_DOUBLE},
                                   {.name = "else", .token=TOK_ELSE},
                                   {.name = "enum", .token=TOK_ENUM},
                                   {.name = "extern", .token=TOK_EXTERN},
                                   {.name = "for", .token=TOK_FOR},
                                   {.name = "goto", .token=TOK_GOTO},
                                   {.name = "if", .token=TOK_IF},
                                   {.name = "inline", .token=TOK_INLINE},
                                   {.name = "long", .token=TOK_LONG},
                                   {.name = "register", .token=TOK_REGISTER},
                                   {.name = "restrict", .token=TOK_RESTRICT},
                                   {.name = "return", .token=TOK_RETURN},
                                   {.name = "short", .token=TOK_SHORT},
                                   {.name = "signed", .token=TOK_SIGNED},
                                   {.name = "sizeof", .token=TOK_SIZEOF},
                                   {.name = "static", .token=TOK_STATIC},
                                   {.name = "typedef", .token=TOK_TYPEDEF},
                                   {.name = "struct", .token=TOK_STRUCT},
                                   {.name = "union", .token=TOK_UNION},
                                   {.name = "unsigned", .token=TOK_UNSIGNED},
                                   {.name = "void", .token=TOK_VOID },
                                   {.name = "volatile", .token=TOK_VOLATILE },
                                   {.name = "while", .token=TOK_WHILE}};
static const Keyword symbolicKeywords[] =  {{.name = "...", .token=TOK_ELLIPSIS},
                                   {.name = ">>=", .token=TOK_RASSIGN},
                                   {.name = "<<=", .token=TOK_LASSIGN},
                                   {.name = "+=", .token=TOK_ADD_ASSIGN},
                                   {.name = "-=", .token=TOK_SUB_ASSIGN},
                                   {.name = "*=", .token=TOK_MUL_ASSIGN},
                                   {.name = "/=", .token=TOK_DIV_ASSIGN},
                                   {.name = "%=", .token=TOK_MOD_ASSIGN},
                                   {.name = "&=", .token=TOK_AND_ASSIGN},
                                   {.name = "^=", .token=TOK_XOR_ASSIGN},
                                   {.name = "|=", .token=TOK_OR_ASSIGN},
                                   {.name = ">>", .token=TOK_RIGHT_OP},
                                   {.name = "<<", .token=TOK_LEFT_OP},
                                   {.name = "++", .token=TOK_INC_OP},
                                   {.name = "--", .token=TOK_DEC_OP},
                                   {.name = "->", .token=TOK_PTR_OP},
                                   {.name = "&&", .token=TOK_AND_OP},
                                   {.name = "||", .token=TOK_OR_OP},
                                   {.name = "<=", .token=TOK_LE_OP},
                                   {.name = "=>", .token=TOK_GE_OP},
                                   {.name = "==", .token=TOK_EQ_OP},
                                   {.name = "!=", .token=TOK_NE_OP}};
                                                    
/**************************************************
@brief StringProcessor_new - TBD
 * 
 * @param[in]   initialFileName  : initial file name
 * @param[out]  None    : None
 *
 * @return TBD
**************************************************/
PUBLIC StringProcessor* StringProcessor_new(String* fileContent)
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
PUBLIC void StringProcessor_delete(StringProcessor* this)
{
  int i=0;
  
  //printf("StringProcessor.c: delete\n");
  
  for (i=0; i<NB_MAX_BUFFERS; i++)
  {
    if (this->buffers[i]!=NULL) StringBuffer_delete(this->buffers[i]);
  }
  
  this->currentBuffer = NULL;
  
  Map_delete(this->macros, (void(*)(void*))&StringProcessor_deleteMacroDefinition);
  //close all H files f.close
  Memory_free(this, sizeof(StringProcessor));
}

PRIVATE void StringProcessor_deleteMacroDefinition(MacroDefinition* this)
{
  String_delete(this->body);
  List_delete(this->parameters,(void(*)(void*))String_delete);
  Memory_free(this, sizeof(MacroDefinition));
}

/**************************************************
 @brief StringProcessor_getToken
 
 TBD
 
 @param: TBD
 @return: TBD.
**************************************************/
PUBLIC Token* StringProcessor_getToken(StringProcessor* this)
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
    else if (StringProcessor_isEndOfLine(this,c))
    {
      // Do nothing
    }
    else if (c==32)
    {
       c = StringProcessor_readChar(this,1);
    }
    else if (c=='/')
    {
        c = StringProcessor_readChar(this,1);
        d = StringProcessor_readChar(this,0);
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
      // else check it is a keyword
      nextToken = StringProcessor_checkKeyword(this, identifier);
      if (nextToken == NULL)
      {
        // Check if it is a Macro call
        if (StringProcessor_checkForMacro(this, identifier))
        {
          String_delete(identifier);
        }
        else
        {
          nextToken = Token_new(TOK_IDENTIFIER, "IDENTIFIER", identifier, NULL, 0, 0);
        }
      }
      else
      {
        String_delete(identifier);
      }
    }
    else if ((c>='0') && (c<='9'))
    {
      number = StringProcessor_readNumber(this);
      tmpInt = String_toInt(number);
      String_delete(number);
      nextToken = Token_new(TOK_CONSTANT, "CONSTANT", (void*)((uintptr_t)tmpInt), NULL, 0, 0);
    }
    else
    {
      nextToken = StringProcessor_checkOpKeyword(this);
      if (nextToken == NULL)
      {
        c = StringProcessor_readChar(this,1);
        nextToken = Token_new(TOK_UNKNOWN, "UNKOWN", (void*)((intptr_t)c), NULL, 0, 0);
      }
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
PRIVATE unsigned int  StringProcessor_readDirective(StringProcessor* this)
{
  unsigned int result = 0;
  unsigned char c = 0;
  
  // 1. If StringBuffer_compare(current, "#include") then
  if (StringProcessor_match(this, (String*)&includeToken))
  {
    StringProcessor_readInclude(this);
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
 @brief StringProcessor_readInclude
 
 TBD
 
 @param: TBD
 @return: TBD.
**************************************************/
PRIVATE void StringProcessor_readInclude(StringProcessor* this)
{
  unsigned char c = 0;
  unsigned int length = 0;
  String* fileName = NULL;
  
  c = StringProcessor_readChar(this,0);
  
  while (c==32)
  {
    c = StringProcessor_readChar(this,1);
    c = StringProcessor_readChar(this,0);
  }
  
  if ((c=='"') || (c=='<'))
  { 
    c = StringProcessor_readChar(this,1);
    c = StringProcessor_readChar(this,0);
    
    while (((c>='a') && (c<='z')) || ((c>='A') && (c<='Z')) ||
          ((c>='0') && (c<='9')) || (c=='_') || (c=='.') || 
           (c=='/'))
    {
      length++;
      c = StringProcessor_readChar(this,1);
      c = StringProcessor_readChar(this,0);
    }
    fileName = StringBuffer_readback(this->currentBuffer, length);
    String_print(fileName, "#include: ");
    
    if ((c=='"') || (c=='>'))
    {
      c = StringProcessor_readChar(this,1);
      c = StringProcessor_readChar(this,0);
      
      if (!StringProcessor_isIncFileIgnored(this, fileName))
      {
        StringProcessor_openNewBufferFromFile(this, fileName);
      }
      String_delete(fileName);
    }
  }
}

/**************************************************
 @brief StringProcessor_readIdentifier
 
 TBD
 
 @param: TBD
 @return: TBD.
**************************************************/
PRIVATE String* StringProcessor_readIdentifier(StringProcessor* this)
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
  unsigned int paramLength = 0;
  unsigned char c = 0;
  MacroDefinition* macroDefinition = NULL;
  String* macroExpansion = NULL;
  ListNode* p = NULL;
  
  // if identifier is a defined macro
  if  (Map_find(this->macros, identifier, (void**)&macroDefinition))
  {
    result = 1;
    macroExpansion = String_dup(macroDefinition->body);
    String_print(macroExpansion, "MacroExpansion: Initial value ");
    
    c = StringProcessor_readChar(this,0);
    if (c=='(')
    {
      c = StringProcessor_readChar(this,1);
      c = StringProcessor_readChar(this,0);
      p = macroDefinition->parameters->head;
      while (c!=')')
      {
        if (c=='(')
        {
          while (c!=')')
          {
            paramLength++;
            c = StringProcessor_readChar(this,1);
            c = StringProcessor_readChar(this,0);
          }
          paramLength++;
          c = StringProcessor_readChar(this,1);
          c = StringProcessor_readChar(this,0);
        }
        else
        {
          while ((c!=',') && (c!=')'))
          {
            paramLength++;
            c = StringProcessor_readChar(this,1);
            c = StringProcessor_readChar(this,0);
          }
        }
        parameterValue = StringBuffer_readback(this->currentBuffer, paramLength);
        String_print(parameterValue, "MacroExpansion: parameter ");
        macroExpansion = String_searchAndReplace(macroExpansion, (String*)p->item, parameterValue);
        String_print(macroExpansion, "MacroExpansion: Result ");
        String_delete(parameterValue);
        p = p->next;
        paramLength = 0;
        if (c==',')
        {
          c = StringProcessor_readChar(this,1);
          c = StringProcessor_readChar(this,0);
        }
      }
      c = StringProcessor_readChar(this,1);
    }
    if (macroExpansion!=NULL)
    {
      StringProcessor_openNewBufferFromString(this, macroExpansion, this->currentBuffer->name);
    }
  }
  return result;
}

/**************************************************
**************************************************/
PRIVATE Token* StringProcessor_checkKeyword(StringProcessor* this, String* identifier)
{
  Token* result=NULL;
  unsigned int i = 0;
  
  for (i=0; i<sizeof(keywords)/sizeof(Keyword); i++)
  {
    if (String_cmp(identifier, keywords[i].name))
    {
      result = Token_new(keywords[i].token, keywords[i].name, 0, NULL, 0, 0);
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
PRIVATE Token* StringProcessor_checkOpKeyword(StringProcessor* this)
{
  Token* result=NULL;
  unsigned int i = 0;
  String* tmpStr=NULL;
  
  for (i=0; i<sizeof(symbolicKeywords)/sizeof(Keyword); i++)
  {
    tmpStr = String_new(symbolicKeywords[i].name);
    if (StringProcessor_match(this, tmpStr))
    {
      result = Token_new(keywords[i].token, symbolicKeywords[i].name, 0, NULL, 0, 0);
      if (result)
      {
        i = sizeof(symbolicKeywords)/sizeof(Keyword);
      }
    }
    String_delete(tmpStr);
  }
  
  return result;
}

/**************************************************
**************************************************/
PRIVATE unsigned int StringProcessor_match(StringProcessor* this, String* pattern)
{  
  return (StringBuffer_match(this->currentBuffer, pattern));
}

/**************************************************
**************************************************/
PRIVATE void StringProcessor_readDefine(StringProcessor* this)
{
  unsigned int result = 0;
  MacroDefinition* macroDefinition = NULL;
  String* parameter = NULL;
  unsigned char c = 0;
  unsigned int paramLength = 0;
  
    c = StringProcessor_readChar(this,0);
      
  while (c==32)
  {
    c = StringProcessor_readChar(this,1);
    c = StringProcessor_readChar(this,0);
  }
    
  while (StringProcessor_isLetter(this, c))
  {
    result++;
    c = StringProcessor_readChar(this,1);
    c = StringProcessor_readChar(this,0);
  }
  macroDefinition = Memory_alloc(sizeof(MacroDefinition));
  macroDefinition->name = StringBuffer_readback(this->currentBuffer, result);
  macroDefinition->parameters = NULL;
  macroDefinition->body = NULL;
  String_print(macroDefinition->name, "#define: ");
  
  if (c=='(')
  {
    c = StringProcessor_readChar(this,1);
    c = StringProcessor_readChar(this,0);
    while (c!=')')
    {
      while ((c!=',') && (c!=')'))
      {
        paramLength++;
        c = StringProcessor_readChar(this,1);
        c = StringProcessor_readChar(this,0);
      }
      if (macroDefinition->parameters == NULL)
      {
        macroDefinition->parameters = List_new();
      }
      parameter = StringBuffer_readback(this->currentBuffer, paramLength);
      paramLength = 0;
      
      List_insert(macroDefinition->parameters, parameter);
      if (c==',')
      {
        c = StringProcessor_readChar(this,1);
        c = StringProcessor_readChar(this,0);
      }
    }
    
        c = StringProcessor_readChar(this,1);
  }
  
  if ((c!=10) && (c!=13))
  {
        c = StringProcessor_readChar(this,0);
    while (c==32)
    {
        c = StringProcessor_readChar(this,1);
        c = StringProcessor_readChar(this,0);
    }
    result =0;
  
    while ((c!=10) && (c!=13))
    {
      result++;
      c = StringProcessor_readChar(this,1);
      c = StringProcessor_readChar(this,0);
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
PRIVATE void StringProcessor_openNewBufferFromFile(StringProcessor* this, String* fileName)
{
  String* fileContent = NULL;
  FileMgr* fileMgr = FileMgr_getFileMgr();
  
  fileContent = FileMgr_searchAndLoad(fileMgr, fileName);
  if (fileContent!=NULL)
  {
    StringProcessor_openNewBufferFromString(this, fileContent, fileName);
    String_print(fileName, "Processing file ");
    printf("-----------------------------------------------\n");
  }
  else
  {
    String_print(fileName,"StringProcessor.c: Cannot load ");
    String_print(fileName,"StringProcessor.c: Ignoring ");
  }      
  
  FileMgr_delete(fileMgr);
}

/**************************************************
**************************************************/
PRIVATE void StringProcessor_openNewBufferFromString(StringProcessor* this, String* content, String* bufferName)
{ 
  if (this->nbOpenBuffers < NB_MAX_BUFFERS)
  {
    this->buffers[this->nbOpenBuffers] = StringBuffer_new(content, bufferName);
    this->currentBuffer = this->buffers[this->nbOpenBuffers];

    this->nbOpenBuffers++;
  }
  else
  {
    String_print(bufferName, "Not enough buffer to open ");
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
  String* comment =NULL;
  SdbMgr* sdbMgr = SdbMgr_getSdbMgr();
  String* sdbCmd = NULL;
  
  c = StringProcessor_readChar(this,1);
  c = StringProcessor_readChar(this,0);
  
  while (c!=10)
  {
    length++;
    c = StringProcessor_readChar(this,1);
    c = StringProcessor_readChar(this,0);
  }
  
  comment = StringBuffer_readback(this->currentBuffer, length);
  sdbCmd = String_sprint(comment, "INSERT INTO Comments ( content ) VALUES ('%s')");
  SdbMgr_execute(sdbMgr, sdbCmd->buffer);
  
  String_delete(sdbCmd);
  String_delete(comment);
  SdbMgr_delete(sdbMgr);
}

/**************************************************
**************************************************/
PRIVATE void StringProcessor_readMultiLineComment(StringProcessor* this)
{
  unsigned char c = 0;
  unsigned length = 2;
  unsigned int isFound = 0;
  String* comment =NULL;
  SdbMgr* sdbMgr = SdbMgr_getSdbMgr();
  String* sdbCmd = NULL;
  
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
  comment = StringBuffer_readback(this->currentBuffer, length);
  sdbCmd = String_sprint(comment, "INSERT INTO Comments ( content ) VALUES ('%s')");
  SdbMgr_execute(sdbMgr, sdbCmd->buffer);
  
  String_delete(sdbCmd);
  String_delete(comment);
  SdbMgr_delete(sdbMgr);
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
  
  if (c==10)
  {
    c = StringProcessor_readChar(this,1);
  }
  else if (c==13)
  {
    c = StringProcessor_readChar(this,1);
    c = StringProcessor_readChar(this,0);
    /* Check for Windows End of line */
    if (c==10)
    {
      c = StringProcessor_readChar(this,1);
    }
    result = 1;
  }
  return result;
}
