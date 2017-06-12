/* String.h */

#include <string.h>

#include "Common.h"

#include "String.h"


String* String_new(const char* string)
{
    String* this;

    this = (String*)Memory_alloc(sizeof(String));
    this->length = strlen(string);
    this->buffer = (char*)Memory_alloc(sizeof(char)*this->length);
    memcpy(this->buffer, string, this->length);
	  //printf("String: Allocating %s\n", this->buffer);
    return this;
}

void String_delete(String* this)
{
    //printf("Deleting %s\n", this->buffer);
    if (this!=NULL)
    {
      Memory_free(this->buffer, this->length);
      this->length=0;
      Memory_free(this, sizeof(String));
    }
    else
    {
      //printf("String.c: String_delete: NULL pointer\n");
    }
}

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
	
	//String_print(this, "Cat string: ");
	//printf("String: Cat: %d\n", this->length);
	//printf("String: Cat: %d\n", strlen(this->buffer));
  //ssprintf("String: Cat: %d\n", strlen(str2));
}

void String_append(String* this, String* str2)
{
  char* buffer = NULL;
  unsigned int length;
  
  length = this->length + str2->length;
  buffer = Memory_alloc(length);
  
  memcpy(buffer, this->buffer, this->length);
  memcpy(buffer + this->length, str2->buffer, str2->length);
  Memory_free(this->buffer, this->length);
  
  this->buffer = buffer;
  this->length = length;
  
  //String_print(this,"String_append.c: Return = ");
}

String* String_dup(String* this)
{
  String* duplicatedString;
   
  duplicatedString = (String*)Memory_alloc(sizeof(String));
  duplicatedString->length = this->length;
  duplicatedString->buffer = (char*)Memory_alloc(sizeof(char)*this->length);
  memcpy(duplicatedString->buffer, this->buffer, this->length);
	
	return duplicatedString;
}

int String_cmp(String* this, const char* str2)
{
  int result = 0;

  if (strncmp(this->buffer, str2, this->length)==0)
	  result=1;
  //printf("String_cmp: %d %s %s\n", this->length, this->buffer, str2);
	
  return result;
}

int String_match(String* this, unsigned int pos,String* str2)
{
  int result = 0;

  if ((pos<this->length) && (strncmp(this->buffer + pos, str2->buffer, str2->length)==0))
	  result=1;
  //printf("String_cmp: %d %s %s\n", this->length, this->buffer, str2);
	
  return result;
}

char* String_getBuffer(String* this)
{
    return this->buffer;
}

void String_print(String* this, const char*displayString)
{
  char* buffer;
  unsigned int length;
   
  length = this->length + strlen(displayString) + 1;

  buffer = Memory_alloc(length);
  memcpy(buffer, displayString, strlen(displayString));
  memcpy(buffer+strlen(displayString), this->buffer, this->length);
  buffer[length-1]=0;
  printf("%s\n", buffer);
  Memory_free(buffer,length);
  //printf("String_print: Here\n");
}

String* String_sprint(String* this, const char*displayString)
{
  char* buffer[512] = {0};
  String* result = NULL;
  unsigned int length = 0;
  
  memcpy(buffer, this->buffer, this->length);
  sprintf(buffer, displayString, this->buffer);
  result->length = strlen(buffer);
  memcpy(result->buffer, buffer, result->length);
  
  return result;
}

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

String* String_subString(String* this, unsigned int pos, unsigned int length)
{
  String* result = NULL;
  
  result = Memory_alloc(sizeof(String));
  result->length = length;
  result->buffer = (char*)Memory_alloc(sizeof(char)*length);
  memcpy(result->buffer, this->buffer+pos, length);
    
  return result;
}

int String_toInt(String* this)
{
  int result = 0;
  char tmp[20] = { 0 };
  
  memcpy(tmp, this->buffer, this->length);
  result = atoi(tmp);
  
  return result;
}