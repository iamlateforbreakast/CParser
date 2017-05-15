/* String.h */

#ifndef _STRING_H_
#define _STRING_H_

#include "Common.h"

typedef struct String
{
    char* buffer;
    int length;
} String;

String* String_new(const char* string);
String* String_newFromFile(const char* fileName);
void String_delete(String* this);
String* String_createFromFile(const char* fileName);
char* String_getBuffer(String* this);
void String_cat(String* this, const char* str2);
int String_cmp(String* this, const char* str2);
String* String_dup(String* this);
int String_match(String* this, unsigned int pos,String* str2);
#endif
