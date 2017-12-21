#ifndef _LIST_H_
#define _LIST_H_

/* List.h */

#include "Common.h"

typedef struct ListNode ListNode;

typedef struct List List;

struct ListNode
{
  void* item;
  ListNode* next;
  ListNode* prev;
};

struct List
{
  ListNode* head;
  ListNode* current;
  unsigned int nbNodes;
};

List* List_new();
void List_delete(List* this, void (*f_delete)(void*));
List* List_copy(List* this);
void List_insert(List* this, void* item);
void List_merge(List* this, List* l1);
void List_iterator(List* this, void *(f)(void* t));
void* List_getNext(List* this);
void* List_getHead(List* this);
#endif
