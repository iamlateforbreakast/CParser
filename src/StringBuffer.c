/**********************************************//**
  @file StringBuffer.c
  
  @brief This file contains the implementation of the class StringBuffer.
  
  The class StringBuffer stores a String, allow to read sequentially
  the characters of the String. It is also possible to read back a number of
  characters from the current reading position.
**************************************************/

#include "StringBuffer.h"

#include "Common.h"

/**********************************************//**
  @brief Allocate a StringBuffer object.
  
  @param String
  @param String
**************************************************/
PUBLIC StringBuffer* StringBuffer_new(String* s, String* name)
{
  StringBuffer* this;

  this=(StringBuffer*)Memory_alloc(sizeof(StringBuffer));
  this->pos = 0;
  this->line = 0;
  this->column = 0;
  this->s = s;
  this->name = String_dup(name);
  return this;
}

/**********************************************//**
  @brief De-allocate a String object.
  
  @param StringBuffer
**************************************************/
PUBLIC void StringBuffer_delete(StringBuffer* this)
{
    String_delete(this->s);
    String_delete(this->name);
    Memory_free(this, sizeof(StringBuffer));
}

/**********************************************//**
  @brief Read a character from the buffer.
  
  @param StringBuffer
**************************************************/
PUBLIC unsigned char StringBuffer_readChar(StringBuffer* this)
{
  unsigned char result=0;
  
  if (this->pos<this->s->length)
  {
    result=this->s->buffer[this->pos];
    this->pos++;
    this->column++;
  }
 
  if (result == '\n')
  {
    this->line++;
    this->column = 0;
  } 

  return result;
}

/**********************************************//**
  @brief Peek the next character to be read.
  
  @param  StringBuffer
**************************************************/
PUBLIC unsigned char StringBuffer_peekChar(StringBuffer* this)
{
  unsigned char result = 0;
  
  if (this->pos<this->s->length)
  {
    result=this->s->buffer[this->pos];
  }
  
  return result;
}

/**********************************************//**
  @brief Match a pattern with the next characters a StringBuffer object.
  
  @param StringBuffer
  @param String
**************************************************/
PUBLIC unsigned int StringBuffer_match(StringBuffer* this, String* pattern)
{
  unsigned int result = 0;
  
  //String_print(pattern, "Matching: ");
  //printf("Pos: %d\n", this->pos);
  
  if (String_match(this->s, this->pos, pattern))
  {
    result = 1;
    this->pos = this->pos + pattern->length;
    //String_print(pattern, "Match found: %s\n");
    //printf("Pos: %d\n", this->pos);
  }
  return result;
}

/**********************************************//**
  @brief Check if there are any character left to read
  in the StringBuffer object.
  
  @param StringBuffer
**************************************************/
PUBLIC unsigned int StringBuffer_isEOF(StringBuffer* this)
{
  unsigned int result = 0;
  
  if (this->pos>=this->s->length) result = 1;
  
  return result;
}

/**********************************************//**
  @brief Read back a number of characters from a StringBuffer object.
  
  @param StringBuffer
  @param unsigned char
**************************************************/
PUBLIC String* StringBuffer_readback(StringBuffer* this, unsigned int nbChar)
{
  String* result = NULL;
  
  result = String_subString(this->s, this->pos-nbChar, nbChar);

  return result;
}

String* StringBuffer_getName(StringBuffer* this)
{
  return this->name;
}
