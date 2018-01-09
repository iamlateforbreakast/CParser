/**********************************************//**
  @file String2.c
  @brief TBD
**************************************************/

#include "Common.h"

#include "String2.h"

#include <string.h>

/**********************************************//**
 @brief String_new
 
 This function allocate a String object.
 
 @param: none
 @return: const char* 
**************************************************/
String* String_new(const char* string)
{
    String* this;

    this = (String*)Memory_alloc(sizeof(String));
    this->length = strlen(string);
    this->buffer = (char*)Memory_alloc(sizeof(char)*this->length);
    this->object.delete = &String_delete;
    this->object.copy = NULL;
    this->object.refCount = 1;
    memcpy(this->buffer, string, this->length);
	  //printf("String: Allocating %s\n", this->buffer);
    return this;
}

/**********************************************//**
 @brief String_delete
 
 This function de-allocate a String object.
 
 @param: String* - String to de-allocate
 @return: none 
**************************************************/
void String_delete(String* this)
{
    //printf("Deleting %s\n", this->buffer);
    if (this!=NULL)
    {
      if (this->object.refCount == 1)
      {
        Memory_free(this->buffer, this->length);
        this->length=0;
        Memory_free(this, sizeof(String));
        this = NULL;
      }
      else if (this->object.refCount >1)
      {
        this->object.refCount--;
      }
      else
      {
        printf("Error\n");
      }
    }
    else
    {
      //printf("String.c: String_delete: NULL pointer\n");
    }
    this = NULL;
}

/**********************************************//**
 @brief String_cat
 
 TBD
 
 @param: TBD
 @return: TBD
**************************************************/
void String_cat(String* this, const char* str2)
{
  char* catBuffer;
  unsigned int length;
	
  length = this->length + strlen(str2);
	catBuffer = Memory_alloc(length);
	memcpy(catBuffer, this->buffer, this->length);
	memcpy(catBuffer+this->length, str2, strlen(str2));
	Memory_free(this->buffer, this->length);
	this->length = length;
	this->buffer = catBuffer;
	this->object.delete = &String_delete;
  this->object.copy = NULL;
	//String_print(this, "Cat string: ");
	//printf("String: Cat: %d\n", this->length);
	//printf("String: Cat: %d\n", strlen(this->buffer));
  //ssprintf("String: Cat: %d\n", strlen(str2));
}

/**********************************************//**
 @brief String_append
 
 TBD
 
 @param: TBD
 @return: TBD
**************************************************/
String* String_append(String* this, String* str2)
{
  String* result = NULL;
  char* buffer = NULL;
  unsigned int length;
  
  if (this==NULL)
  {
    this = String_dup(str2);
    result = this;
  }
  else
  {
    length = this->length + str2->length;
    buffer = Memory_alloc(length);
  
    memcpy(buffer, this->buffer, this->length);
    memcpy(buffer + this->length, str2->buffer, str2->length);
    Memory_free(this->buffer, this->length);
  
    this->buffer = buffer;
    this->length = length;
    this->object.delete = &String_delete;
    this->object.copy = NULL;
    result = this;
  }
  //String_print(this,"String_append.c: Return = ");
  return result;
}

/**********************************************//**
 @brief String_dup
 
 This function creates a copy of a given String object.
 
 @param: String* - Reference to a String object.
 @return: String* - Reference to the created String object.
**************************************************/
String* String_dup(String* this)
{
  String* duplicatedString = NULL;
   
  if (this!=NULL)
  {
    duplicatedString = (String*)Memory_alloc(sizeof(String));
    duplicatedString->length = this->length;
    duplicatedString->buffer = (char*)Memory_alloc(sizeof(char)*this->length);
    duplicatedString->object.delete = &String_delete;
    duplicatedString->object.copy = NULL;
    duplicatedString->object.refCount = 1;
    memcpy(duplicatedString->buffer, this->buffer, this->length);
	}
  else
  {
    printf("String2.c: String_dup this=NULL\n");
  }
  
	return duplicatedString;
}

/**********************************************//**
 @brief String_cmp
 
 This function compares a String object to a constant string.
 
 @param: String* - String object to compare.
 @param: const char* - Const string to compare to.
 @return: int - 0 no match, 1 match
**************************************************/
int String_cmp(String* this, const char* str2)
{
  int result = 0;

  if ((this->length == strlen(str2)) && (strncmp(this->buffer, str2, this->length)==0))
	  result=1;
  //printf("String_cmp: %d %s %s\n", this->length, this->buffer, str2);
	
  return result;
}

/**********************************************//**
 @brief String_match
 
 TBD
 
 @param: TBD
 @return: TBD
**************************************************/
int String_match(String* this, unsigned int pos,String* str2)
{
  int result = 0;

  if ((pos<this->length) && (strncmp(this->buffer + pos, str2->buffer, str2->length)==0))
	  result=1;
  //printf("String_cmp: %d %s %s\n", this->length, this->buffer, str2);
	
  return result;
}

/**********************************************//**
 @brief String_getBuffer
 
 TBD
 
 @param: TBD
 @return: TBD
**************************************************/
//char* String_getBuffer(String* this)
//{
//    return this->buffer;
//}

/**********************************************//**
 @brief String_print
 
 TBD
 
 @param: TBD
 @return: TBD
**************************************************/
void String_print(String* this, const char*displayString)
{
  char* buffer;
  unsigned int length;
  
  if (this!=NULL)
  {  
    length = this->length + strlen(displayString) + 1;

    buffer = Memory_alloc(length);
    memcpy(buffer, displayString, strlen(displayString));
    memcpy(buffer+strlen(displayString), this->buffer, this->length);
    buffer[length-1]=0;
    printf("%s\n", buffer);
    Memory_free(buffer,length);
    //printf("String_print: Here\n");
  }
  else
  {
    printf("String2.c: String_print this=NULL\n");
  }
}

/**********************************************//**
 @brief String_sprint
 
 TBD
 
 @param: TBD
 @return: TBD
**************************************************/
String* String_sprint(String* this, const char* displayString)
{
  char buffer[512] = { 0 };
  char s[512] = { 0 };
  String* result = NULL;
  
  result = (String*)Memory_alloc(sizeof(String));
  
  memcpy(s, this->buffer, this->length);
  snprintf(buffer, 512, displayString, s);
  result->length = strlen(buffer)+1;
  
  result->buffer = (char*)Memory_alloc(result->length);
  result->object.delete = &String_delete;
  result->object.copy = NULL;
  result->object.refCount = 1;
  memcpy(result->buffer, buffer, result->length);
  
  return result;
}

/**********************************************//**
 @brief String_filter
 
 TBD
 
 @param: TBD
 @return: TBD
**************************************************/
unsigned int String_filter(String* this, String* filter)
{
  unsigned int result=1;
  unsigned int i,j,k;
  
  j=this->length-1;
  k=filter->length-1; 

  for (i=0;(i<filter->length) && (result==1); i++)
  {
    //printf("String_filter: %c %c\n", this->buffer[j-i], filter->buffer[k-i]);
    if ((this->buffer[j-i]!=filter->buffer[k-i]))
	    result = 0;
  }
   
  return result;
}

/**********************************************//**
 @brief String_subString
 
 TBD
 
 @param: TBD
 @return: TBD
**************************************************/
String* String_subString(String* this, unsigned int pos, unsigned int length)
{
  String* result = NULL;
  
  result = (String*)Memory_alloc(sizeof(String));
  result->length = length;
  result->buffer = (char*)Memory_alloc(sizeof(char)*length);
  result->object.delete = &String_delete;
  result->object.copy = NULL;
  result->object.refCount = 1;
  memcpy(result->buffer, this->buffer+pos, length);
    
  return result;
}

/**********************************************//**
 @brief String_toInt
 
 TBD
 
 @param: TBD
 @return: TBD
**************************************************/
int String_toInt(String* this)
{
  int result = 0;
  char tmp[20] = { 0 };
  
  memcpy(tmp, this->buffer, this->length);
  result = strtol(tmp, NULL, 0);
  
  return result;
}

/**********************************************//**
 @brief String_searchAndReplace
 
 TBD
 
 @param: TBD
 @return: TBD
**************************************************/
String* String_searchAndReplace(String* this, String* search, String* replace)
{
  String* result = NULL;
  String* tmpStr = NULL;
  unsigned int i,j;
  
  i = 0;
  j = 0;
  
  if ((this==NULL) || (search==NULL) || (replace==NULL))
  {
    result = this;
  }
  else
  {
    while ( (j+search->length)<=this->length)
    {
      if (String_match(this,j,search))
      {
        if (j==0)
        {
          result = String_dup(replace);
        }
        else
        {
          tmpStr = String_subString(this, i, j-i);
          tmpStr = String_append(tmpStr, replace);
          result = String_append(result, tmpStr);
          String_delete(tmpStr);
        } 

        j = j + search->length;
        i = j;
      }
      else
      {
        j++;
      }
    }
    tmpStr = String_subString(this, i, j-i);
    result = String_append(result, tmpStr);
    String_delete(tmpStr);
  }

  return result;
}
