/* StringBuffer.c */

#include "StringBuffer.h"

StringBuffer* StringBuffer_new()
{
    StringBuffer* this;

    this=(StringBuffer*)Memory_alloc(sizeof(StringBuffer));
    this->pos = 0;
    
    return this;
}

void StringBuffer_delete(StringBuffer* this)
{
    String_delete(this->s);
    String_delete(this->name);
    Memory_free(this, sizeof(StringBuffer));
}

StringBuffer* StringBuffer_newFromFile(const char* fileName)
{
    StringBuffer* this;

    this=StringBuffer_new();
   
    this->s = String_newFromFile(fileName);
    this->name = String_new(fileName);

    /*f=fopen(fileName,"rb");
    if (f)
    {
        fseek(f, 0, SEEK_END);
        this->size = ftell(f);
        fseek(f, 0, SEEK_SET);

        //this->buffer = malloc(this->size+1);
        //fread(this->buffer,this->size, 1, f);
        fclose(f);
        
	this->filename = String_new(fileName);

        printf("Loaded file: %s Size: %ld bytes\n", this->name, this->size);
    }
    else
    {
	 File cannot be opened 
    }*/
    return this;
}
