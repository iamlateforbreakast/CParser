/* TokenList.c */

#include "TokenList.h"
#include "Common.h"
#include "Token.h"

Token* TokenList_checkKeyword(TokenList* this);
Token* TokenList_checkIntegerConstant(TokenList* this);
Token* TokenList_checkIdentifier(TokenList* this);

static const char* keywords[] = { "int", "float", "auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else",
                                 "enum", "extern", "for", "goto", "if", "inline", "long", "register", "restrict", "return", "short", "signed",
                                 "sizeof", "union", "unsigned", "void", "volatile", "while" };
               
TokenList* TokenList_new(String* fileContent)
{
  TokenList* this = NULL;
  unsigned int i = 0;
    
  this = (TokenList*)Memory_alloc(sizeof(TokenList));
  this->transUnit = TransUnit_new(fileContent);
  this->nbKeywords = 30;
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
  
  printf("TokenList.c: delete\n");
  TransUnit_delete(this->transUnit);
    
  for (i=0; i<this->nbKeywords; i++)
  {
    String_delete(this->keyword[i]);
  }
  Memory_free(this->keyword, sizeof(String*) * this->nbKeywords);
  this->nbKeywords = 0;
  Memory_free(this, sizeof(TokenList));
}

void TokenList_initialise(TokenList* this, String* fileBuffer)
{
    
}

Token* TokenList_getTokenFromTransUnit(TokenList* this)
{
  unsigned char c = 0;
  Token* nextToken = NULL;

  while (nextToken==NULL)
  {
    nextToken = TokenList_checkKeyword(this);

    if (nextToken==NULL) nextToken = TokenList_checkIntegerConstant(this);

    if (nextToken==NULL) nextToken = TokenList_checkIdentifier(this);

    if (nextToken==NULL) 
    {
      c = TransUnit_readCharFromProcessedStream(this->transUnit);
      if (c==0)
      {  
        nextToken = Token_new(TOK_EOF, 0, NULL);
        printf("Read char EOF\n");
      }
      else if ((c!=10) && (c!=32))
      {
        nextToken = Token_new(TOK_UNKNOWN, c, NULL);
        printf("Accepted: Read char: %c %d\n",c,c);
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
    if (TransUnit_match(this->transUnit, this->keyword[i]))
    {
      isFound = 1;
      result = Token_new(i, 0, NULL);
    }
  }
  
  return result;
}

Token* TokenList_checkIntegerConstant(TokenList* this)
{
  Token* result=NULL;
  int tmpInt = 0;
  String* tmpStr = NULL;
  
  tmpStr = TransUnit_readConstantInteger(this->transUnit);
  if (tmpStr)
  {
    tmpInt = String_toInt(tmpStr);
    String_delete(tmpStr);
    result = Token_new(TOK_INT, (void*)tmpInt, NULL);
  }
  
  return result;
}

Token* TokenList_checkIdentifier(TokenList* this)
{
  Token* result=NULL;
  String* identifierName = NULL;
  
  identifierName = TransUnit_readIdentifier(this->transUnit);
  
  if (identifierName!=NULL)
  {
    result = Token_new(TOK_IDENTIFIER, (void*)identifierName, NULL);
    String_print(identifierName, "Token identifier: ");
  }
  
  return result;
}
