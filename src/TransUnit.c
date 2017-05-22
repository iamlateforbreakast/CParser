/* Translation Unit */

#include "TransUnit.h"

#include "Common.h"

unsigned int TransUnit_readSingleLineComment(TransUnit* this);
unsigned int TransUnit_readMultiLineComment(TransUnit* this);

TransUnit* TransUnit_new(String* fileContent)
{
  TransUnit* this;

  this = (TransUnit*)Memory_alloc(sizeof(TransUnit));
  this->processor = StringProcessor_new(fileContent);
  
  return this;
}

void TransUnit_delete(TransUnit* this)
{
  printf("TransUnit.c: delete\n");
  StringProcessor_delete(this->processor);
  Memory_free(this, sizeof(TransUnit));
}

#if 0
void TransUnit_loadFromFile(TransUnit* this, const char* fileName)
{
  String path;
}
#endif

/*
 *
 */
unsigned char TransUnit_readCharFromProcessedStream(TransUnit* this)
{
  unsigned char c;
  unsigned int isComment = 1;
  
  while (isComment)
  {
    isComment = TransUnit_readSingleLineComment(this);
    isComment = isComment + TransUnit_readMultiLineComment(this);
    isComment=0;
  }
  
  c = StringProcessor_readTransUnitChar(this->processor);  

  return c;
}

unsigned int TransUnit_readSingleLineComment(TransUnit* this)
{
  unsigned int result = 0;
  String* singleLineComment;
  unsigned char c = 0;
  
  singleLineComment = String_new("//");
  if (StringProcessor_match(this->processor, singleLineComment))
  {
    //c = StringProcessor_readTransUnitChar(this->processor);
    //c = StringProcessor_readTransUnitChar(this->processor);
    
    while (c!=10)
    {
      c = StringProcessor_readTransUnitChar(this->processor);
    }
  }
  
  String_delete(singleLineComment);
  
  return result;
}

unsigned int TransUnit_readMultiLineComment(TransUnit* this)
{
  unsigned int result = 0;
  unsigned int isFound = 0;
  unsigned char c;
  
  String* multiLineStartToken;
  String* multiLineEndToken;
  
  multiLineStartToken = String_new("/*");
  multiLineEndToken  = String_new("*/");
  
  if (StringProcessor_match(this->processor, multiLineStartToken))
  {
    //c = StringProcessor_readTransUnitChar(this->processor);
    //c = StringProcessor_readTransUnitChar(this->processor);
    
    isFound = StringProcessor_match(this->processor, multiLineEndToken);
    while (!isFound)
    {
      c = StringProcessor_readTransUnitChar(this->processor);
      isFound = StringProcessor_match(this->processor, multiLineEndToken);
    }
    
    if (isFound)
    {
      //c = StringProcessor_readTransUnitChar(this->processor);
      //c = StringProcessor_readTransUnitChar(this->processor);
    }
  }
  
  String_delete(multiLineStartToken);
  String_delete(multiLineEndToken);
  
  return result;
}

unsigned int TransUnit_match(TransUnit* this, String* keyword)
{
  unsigned int result = 0;
  
  result = StringProcessor_match(this->processor, keyword);
  
  return result;
}

unsigned int TransUnit_readConstantInteger(TransUnit* this)
{
  unsigned int result = 0;
  
  return result;
}

String* TransUnit_readIdentifier(TransUnit* this)
{ 
  return (StringProcessor_readIdentifier(this->processor));
}
