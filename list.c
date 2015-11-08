#include "list.h"
#include <memory.h>
#include <stdlib.h>



list* list_new() {
  list* l = (list *) calloc(1, sizeof(list));
  return l;
}

list* list_new_with_tag(tag_t tag) {
  list* l = list_new();
  l->tag = tag;
  return l;
}



void list_push_back_with_tag(list* l, void* item, tag_t tag) {
  l->size++;
  if(l->tail == NULL) {
    l->head = l->tail = (list_item *) calloc(1, sizeof(list_item));
    l->tail->item = item;
    l->tail->tag = tag;
  }
  else {
    list_item* tail = l->tail;
    tail->next = (list_item *) calloc(1, sizeof(list_item));
    tail->next->prev = tail;
    tail->next->item = item;
    tail->next->tag = tag;
 }
}

void list_append(list* a, list* b) {
  //append list b to list a at the end of the list, list b is untouched
  list_item* head = b->head;
  while(head) {
    list_push_back_with_tag(a, head->item, head->tag);
    head++;
  }
}
    
void* list_peek_back(list* l) {
  if(l && l->tail) {
    return l->tail->item;
  }
  return NULL;
}

void* list_pop_back(list* l) {
  if(l) {
    if(l->tail) {
      l->size--;
      list_item* tail = l->tail;
      void* item = tail->item;
      l->tail = l->tail->prev;
      l->tail->next = NULL;
      free(tail);
      return item;
    }
    else {
      return null;
    }
  }
  return NULL;
}

list* list_copy(list* l) {
  //return a copy of the list
  list* new_list = list_new_with_tag(l->tag);
  list_item* head = l->head;
  while(head) {
    list_push_back_with_tag(new_list, head->item, head->tag);
    head = head->next;
  }
  return new_list;
}

void list_delete(list* l) {
  if(l) {
    list_item* item = l->head;
    while(item) {
      list_item* next = item->next;
      free(item);
      item = next;
    }
    free(l);
  }
}
