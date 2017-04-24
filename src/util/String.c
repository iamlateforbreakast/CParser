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
	printf("Allocating %s\n", this->buffer);
    return this;
}

void String_delete(String* this)
{
    printf("Deleting %s\n", this->buffer);
    Memory_free(this->buffer, this->length);
    this->length=0;
    Memory_free(this, sizeof(String));
}

String* String_newFromFile(const char* fileName)
{
    String* this = NULL;

    char* buffer = NULL;

    FILE* f=fopen(fileName,"rb");
    if (f)
    {
	fseek(f, 0, SEEK_END);
	this->length=ftell(f);
	fseek(f, 0 , SEEK_SET);
        
	buffer = (char*)Memory_alloc(this->length);
	fclose(f);
        this = String_new(buffer);
    }
    else
    {
	/* File cannot be found */
    }


    return this;
}

void String_cat(String* this, const char* str2)
{
}

int String_cmp(String* this, const char* str2)
{
    int result = 0;

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
   sprintf(buffer, "%s%s", displayString, this->buffer);
   buffer[length-1]=0;
   printf("%s\n", buffer);
   Memory_free(buffer,sizeof(char));
}

unsigned int String_filter(String* this, String* filter)
{
   unsigned int result=1;
   unsigned int i;
   
   for (i=filter->length; (i>0) && (result==1); i--)
   {
      printf("String_filter: %c %c\n", this->buffer[i-1], filter->buffer[i-1]);
      if ((this->buffer[i-1]!=filter->buffer[i-1]))
	     result = 0;
   }
   
   return result;
}