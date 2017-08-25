/* String2.c */

#include "Common.h"

#include "String2.h"

#include <string.h>

/**************************************************
**************************************************/
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

/**************************************************
**************************************************/
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

/**************************************************
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
	
	//String_print(this, "Cat string: ");
	//printf("String: Cat: %d\n", this->length);
	//printf("String: Cat: %d\n", strlen(this->buffer));
  //ssprintf("String: Cat: %d\n", strlen(str2));
}

/**************************************************
**************************************************/
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

/**************************************************
**************************************************/
String* String_dup(String* this)
{
  String* duplicatedString;
   
  duplicatedString = (String*)Memory_alloc(sizeof(String));
  duplicatedString->length = this->length;
  duplicatedString->buffer = (char*)Memory_alloc(sizeof(char)*this->length);
  memcpy(duplicatedString->buffer, this->buffer, this->length);
	
	return duplicatedString;
}

/**************************************************
**************************************************/
int String_cmp(String* this, const char* str2)
{
  int result = 0;

  if (strncmp(this->buffer, str2, this->length)==0)
	  result=1;
  //printf("String_cmp: %d %s %s\n", this->length, this->buffer, str2);
	
  return result;
}

/**************************************************
**************************************************/
int String_match(String* this, unsigned int pos,String* str2)
{
  int result = 0;

  if ((pos<this->length) && (strncmp(this->buffer + pos, str2->buffer, str2->length)==0))
	  result=1;
  //printf("String_cmp: %d %s %s\n", this->length, this->buffer, str2);
	
  return result;
}

/**************************************************
**************************************************/
char* String_getBuffer(String* this)
{
    return this->buffer;
}

/**************************************************
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

/**************************************************
**************************************************/
String* String_sprint(String* this, const char* displayString)
{
  char buffer[512] = { 0 };
  char s[512] = { 0 };
  String* result = NULL;
  
  result = (String*)Memory_alloc(sizeof(String));
  
  memcpy(s, this->buffer, this->length);
  snprintf(buffer, 512, displayString, s);
  result->length = strlen(buffer);
  
  result->buffer = Memory_alloc(result->length);
  memcpy(result->buffer, buffer, result->length);
  
  return result;
}

/**************************************************
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

/**************************************************
**************************************************/
String* String_subString(String* this, unsigned int pos, unsigned int length)
{
  String* result = NULL;
  
  result = Memory_alloc(sizeof(String));
  result->length = length;
  result->buffer = (char*)Memory_alloc(sizeof(char)*length);
  memcpy(result->buffer, this->buffer+pos, length);
    
  return result;
}

/**************************************************
**************************************************/
int String_toInt(String* this)
{
  int result = 0;
  char tmp[20] = { 0 };
  
  memcpy(tmp, this->buffer, this->length);
  result = atoi(tmp);
  
  return result;
}

#if 0
    /* match: search for regexp anywhere in text */
    int match(char *regexp, char *text)
    {
        if (regexp[0] == '^')
            return matchhere(regexp+1, text);
        do {    /* must look even if string is empty */
            if (matchhere(regexp, text))
                return 1;
        } while (*text++ != '\0');
        return 0;
    }

    /* matchhere: search for regexp at beginning of text */
    int matchhere(char *regexp, char *text)
    {
        if (regexp[0] == '\0')
            return 1;
        if (regexp[1] == '*')
            return matchstar(regexp[0], regexp+2, text);
        if (regexp[0] == '$' && regexp[1] == '\0')
            return *text == '\0';
        if (*text!='\0' && (regexp[0]=='.' || regexp[0]==*text))
            return matchhere(regexp+1, text+1);
        return 0;
    }

    /* matchstar: search for c*regexp at beginning of text */
    int matchstar(int c, char *regexp, char *text)
    {
        do {    /* a * matches zero or more instances */
            if (matchhere(regexp, text))
                return 1;
        } while (*text != '\0' && (*text++ == c || c == '.'));
        return 0;
    }
    #endif