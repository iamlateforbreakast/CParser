/* List.h */

#ifndef _LIST_H_
#define _LIST_H_

typedef struct ListNode ListNode;

struct ListNode
{
  void* item;
  ListNode* next;
  ListNode* prev;
};

typedef struct List
{
  ListNode* head;
  ListNode* current;
} List;

List* List_new();
void List_delete(List* this);
void List_insert(List* this, void* item);
#endif
