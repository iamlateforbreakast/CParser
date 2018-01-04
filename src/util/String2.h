/* String.h */

#ifndef _STRING2_H_
#define _STRING2_H_

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
String* String_append(String* this, String* str2);
unsigned int String_filter(String* this, String* filter);
void String_print(String* this, const char*displayString);
String* String_sprint(String* this, const char* displayString);
String* String_subString(String* this, unsigned int pos, unsigned int length);
int String_toInt(String* this);
String* String_searchAndReplace(String* this, String* search, String* replace);
char* String_getTerminatedBuffer(String* this);
#endif
