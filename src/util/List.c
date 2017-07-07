/* List.c */

#include "List.h"

#include "Common.h"

/**************************************************
**************************************************/ 
List* List_new()
{
  List* this = NULL;

  this = (List*)Memory_alloc(sizeof(List));
  return this;
}

/**************************************************
**************************************************/
void List_delete(List* this, void (*f_delete)(void*))
{
  ListNode* p = this->head;
  
  while (p!=NULL)
  {
    this->head = p->next;
	  (*f_delete)(p);
    p = this->head;
  }
  
  Memory_free(this, sizeof(List));   
}

/**************************************************
**************************************************/
void List_insert(List* this, void* item)
{
  ListNode* newNode = NULL;
  
  newNode = Memory_alloc(sizeof(ListNode));
  newNode->item = item;
  newNode->next = this->head;
  newNode->prev = NULL;
  this->head = newNode;
}

/**************************************************
**************************************************/
unsigned int List_find(List* this, void* item)
{
  unsigned int result = 0;
  
  return result;
}

/**************************************************
**************************************************/
void List_remove(List* this)
{
}

/**************************************************
**************************************************/
void List_iterator(List* this, void *(f)(void* t))
{
  ListNode* iterator = this->head;
  
  while (iterator!=NULL)
  {
    f(iterator->item);
    iterator = iterator->next;
  }
}

/**************************************************
**************************************************/
void List_merge(List* this, List* l1)
{
  ListNode* iterator = l1->head;
  
  while (iterator->next!=NULL)
  {
    iterator = iterator->next;
  }
  
  iterator->next = this->head;
  this->head = l1->head;
  
}