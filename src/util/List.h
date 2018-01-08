/* List.h */
#ifndef _LIST_H_
#define _LIST_H_

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

PUBLIC List* List_new();
PUBLIC void List_delete(List* this);
PUBLIC List* List_copy(List* this);
PUBLIC void List_insert(List* this, void* item);
PUBLIC void List_merge(List* this, List* l1);
PUBLIC void List_iterator(List* this, void *(f)(void* t));
PUBLIC void* List_getNext(List* this);
PUBLIC void* List_getHead(List* this);
#endif
