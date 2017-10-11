/* List.h */

#ifndef _LIST_H_
#define _LIST_H_

typedef struct ListNode ListNode;

typedef struct List List;

List* List_new();
void List_delete(List* this, void (*f_delete)(void*));
void List_insert(List* this, void* item);
void List_merge(List* this, List* l1);
void List_iterator(List* this, void *(f)(void* t));
#endif
