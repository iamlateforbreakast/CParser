/**********************************************//**
  @file String2.h
  @brief TBD
**************************************************/

#ifndef _STRING2_H_
#define _STRING2_H_

#include "Common.h"

typedef struct String String;

struct String
{
  Object object;
  char* buffer;
  int length;
};

PUBLIC String* String_new(const char* string);
PUBLIC String* String_newFromFile(const char* fileName);
PUBLIC void String_delete(String* this);
PUBLIC String* String_createFromFile(const char* fileName);
PUBLIC char* String_getBuffer(String* this);
PUBLIC void String_cat(String* this, const char* str2);
PUBLIC int String_cmp(String* this, const char* str2);
PUBLIC String* String_dup(String* this);
PUBLIC int String_match(String* this, unsigned int pos,String* str2);
PUBLIC String* String_append(String* this, String* str2);
PUBLIC unsigned int String_filter(String* this, String* filter);
PUBLIC void String_print(String* this, const char*displayString);
PUBLIC String* String_sprint(String* this, const char* displayString);
PUBLIC String* String_subString(String* this, unsigned int pos, unsigned int length);
PUBLIC int String_toInt(String* this);
PUBLIC String* String_searchAndReplace(String* this, String* search, String* replace);
PUBLIC char* String_getTerminatedBuffer(String* this);

#endif
