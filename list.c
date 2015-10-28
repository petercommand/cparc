#include "list.h"

list* list_new() {
  list* l = (list *) calloc(sizeof(list));
  return l;
}
list* list_push_back(list* l, void* item) {
  if(l->tail == NULL) {
    l->head = l->tail = (list *) calloc(sizeof(list_item));
    l->tail->item = item;
  }
  else {
    list_item* tail = l->tail;
    tail->next = (list *) calloc(sizeof(list_item));
    tail->next->prev = tail;
    tail->next->item = item;
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
