/* List.c */

#include "List.h"

#include "Object.h"
#include "Common.h"



/**************************************************
**************************************************/ 
List* List_new()
{
  List* this = NULL;

  this = (List*)Memory_alloc(sizeof(List));
  this->head = NULL;
  this->current = NULL;
  this->nbNodes = 0;
  
  return this;
}

/**************************************************
**************************************************/
void List_delete(List* this)
{
  ListNode* p = NULL;
  
  if (this!=NULL)
  {
    p = this->head;
  
    while (p!=NULL)
    {
      this->head = p->next;
      
      if (((Object*)p->item)->delete!=NULL)
      {
        ((Object*)p->item)->delete(p->item);
      }
      Memory_free(p, sizeof(ListNode));
      p = this->head;
    }
    this->nbNodes = 0;
    this->current = NULL;
    this->head = NULL;
    Memory_free(this, sizeof(List));
  }    
}

/**************************************************
**************************************************/
List* List_copy(List* this)
{
  List* result = NULL;
  ListNode* p = NULL;
  
  if (this!=NULL)
  {
    result = List_new();
    p = this->head;
    while (p!=NULL)
    {
      List_insert(result, p->item);
      p = p->next;
    }
  }
  
  return result;
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
  if (this->current == NULL)
  {
    this->current = this->head;
  }
  this->nbNodes ++;
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
  ListNode* iterator = NULL;
  
  if ((l1!=NULL) && (l1->head!=NULL))
  {
    iterator = l1->head;
  
    while (iterator->next!=NULL)
    {
      iterator = iterator->next;
    }
  
    iterator->next = this->head;
    this->head = l1->head;
    this->nbNodes += l1->nbNodes;
    this->current = this->head;

  }
  l1->nbNodes = 0;
  l1->current = NULL;
  l1->head = NULL;
  Memory_free(l1, sizeof(List));
}

/**************************************************
**************************************************/
void* List_getNext(List* this)
{
  void* result = NULL;
  
  if (this->current->next!=NULL)
  {
    result = this->current->next->item;
    this->current = this->current->next;
  }

  return result;
}

/**************************************************
**************************************************/
void* List_getHead(List* this)
{
  void* result = NULL;
  
  if (this!=NULL)
  {
    result = this->head->item;
    this->current = this->head;
  }
  
  return result;
}
