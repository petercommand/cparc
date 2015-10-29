#include "list.h"

list* list_new() {
  list* l = (list *) calloc(sizeof(list));
  return l;
}

list* list_new_with_tag(tag_t tag) {
  list* l = list_new();
  l.tag = tag;
}

list* list_push_back_with_tag(list* l, void* item, tag_t tag) {
  if(l->tail == NULL) {
    l->head = l->tail = (list *) calloc(sizeof(list_item));
    l->tail->item = item;
    l->tail->tag = tag;
  }
  else {
    list_item* tail = l->tail;
    tail->next = (list *) calloc(sizeof(list_item));
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
  if(l && l->tail) {
    list_item* tail = l->tail;
    void* item = tail->item;
    l->tail = l->tail->prev;
    l->tail->next = NULL;
    free(tail);
    return item;
  }
  return NULL;
}

list* list_copy(list* l) {
  //return a copy of the list
  list* new_list = list_new();
  list_item* head = l->head;
  while(head) {
    new_list.push_back(head->item);
    head = head->next;
  }
  return new_list;
}

void list_delete(list* l) {
  if(l) {
    list_item* list_item = l->head;
    while(list_item) {
      list_item* next = list_item->next;
      free(list_item);
      list_item = next;
    }
    free(l);
  }
}
