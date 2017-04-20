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

    return this;
}

void String_delete(String* this)
{
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

char* String_getBuffer(String* this)
{
    return this->buffer;
}
