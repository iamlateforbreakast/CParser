/* List.c */

#include "List.h"

#include "Common.h"

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
void List_delete(List* this, void (*f_delete)(void*))
{
  ListNode* p = NULL;
  
  if (this!=NULL)
  {
    p = this->head;
  
    while (p!=NULL)
    {
      this->head = p->next;
	    (*f_delete)(p->item);
      Memory_free(p, sizeof(ListNode));
      p = this->head;
    }
    this->nbNodes = 0;
    this->current = NULL;
    Memory_free(this, sizeof(List));
  }    
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

  }
  l1->nbNodes = 0;
  l1->current = NULL;
  l1->head = NULL;
  Memory_free(l1, sizeof(List));
}