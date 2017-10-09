/* TokenList.c */

#include "TokenList.h"
#include "Common.h"
#include "Token.h"

#include <stdint.h>

Token* TokenList_checkKeyword(TokenList* this);
Token* TokenList_checkIntegerConstant(TokenList* this);
Token* TokenList_checkIdentifier(TokenList* this);

static const char* keywords[] = { "int", "float", "auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else",
                                 "enum", "extern", "for", "goto", "if", "inline", "long", "register", "restrict", "return", "short", "signed",
                                 "sizeof", "static", "typedef", "struct", "union", "unsigned", "void", "volatile", "while" };

TokenList* TokenList_new(String* fileContent)
{
  TokenList* this = NULL;
  unsigned int i = 0;

  this = (TokenList*)Memory_alloc(sizeof(TokenList));
  this->stringProcessor = StringProcessor_new(fileContent);
  this->nbKeywords = 33;
  this->keyword = (String**)Memory_alloc(sizeof(String*) * this->nbKeywords);

  for (i=0; i<this->nbKeywords; i++)
  {
    this->keyword[i] = String_new(keywords[i]);
  }

  return this;
}

void TokenList_delete(TokenList* this)
{
  unsigned int i = 0;

  //printf("TokenList.c: delete\n");
  StringProcessor_delete(this->stringProcessor);

  for (i=0; i<this->nbKeywords; i++)
  {
    String_delete(this->keyword[i]);
  }
  Memory_free(this->keyword, sizeof(String*) * this->nbKeywords);
  this->nbKeywords = 0;
  Memory_free(this, sizeof(TokenList));
}

Token* TokenList_getTokenFromTransUnit(TokenList* this)
{
  unsigned char c = 0;
  Token* nextToken = NULL;
  String* fName = NULL;
  unsigned int line = 0;
  unsigned int col = 0;
  
  while (nextToken==NULL)
  {
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

Token* TokenList_checkKeyword(TokenList* this)
{
  Token* result=NULL;
  unsigned int i = 0;
  unsigned int isFound = 0;
  
  for (i=0; (i<this->nbKeywords) && (isFound==0); i++)
  {
    if (StringProcessor_match(this->stringProcessor, this->keyword[i]))
    {
      isFound = 1;
      result = Token_new(i, (char*)keywords[i], 0, 
                         this->stringProcessor->currentBuffer->name, 
                         this->stringProcessor->currentBuffer->line, 
                         this->stringProcessor->currentBuffer->column);
    }
  }

  return result;
}

Token* TokenList_checkIntegerConstant(TokenList* this)
{
  Token* result=NULL;
  int tmpInt = 0;
  String* tmpStr = NULL;

  tmpStr = StringProcessor_readInteger(this->stringProcessor);
  if (tmpStr)
  {
    tmpInt = String_toInt(tmpStr);
    String_delete(tmpStr);
    result = Token_new(TOK_CONSTANT, "CONSTANT", (void*)((uintptr_t)tmpInt), 
                         this->stringProcessor->currentBuffer->name, 
                         this->stringProcessor->currentBuffer->line, 
                         this->stringProcessor->currentBuffer->column);
  }

  return result;
}

Token* TokenList_checkIdentifier(TokenList* this)
{
  Token* result=NULL;
  String* identifierName = NULL;

  identifierName = StringProcessor_readIdentifier(this->stringProcessor);

  if (identifierName!=NULL)
  {
    result = Token_new(TOK_IDENTIFIER, "IDENTIFIER", (void*)identifierName, 
                       this->stringProcessor->currentBuffer->name, 
                       this->stringProcessor->currentBuffer->line, 
                       this->stringProcessor->currentBuffer->column);
    //String_print(identifierName, "Token identifier: ");
  }

  return result;
}
