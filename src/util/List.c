/* List.c */

#include "List.h"

#include "Common.h"
 
List* List_new()
{
  List* this = NULL;

  this = (List*)Memory_alloc(sizeof(List));
  return this;
}

void List_delete(List* this)
{
  Memory_free(this, sizeof(List));   
}

void List_insert(List* this, void* item)
{
  ListNode* newNode = NULL;
  
  newNode = Memory_alloc(sizeof(ListNode));
  newNode->item = item;
  newNode->next = this->head;
  newNode->prev = NULL;
  this->head = newNode;
}

unsigned int List_find(List* this, void* item)
{
  unsigned int result = 0;
  
  return result;
}

void List_remove(List* this)
{
}

void List_iterator(List* this, void *(f)(void* t))
{
  ListNode* iterator = this->head;
  
  while (iterator!=NULL)
  {
    f(iterator->item);
    iterator = iterator->next;
  }
}