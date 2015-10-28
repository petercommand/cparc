#include "list.h"

list* list_new() {
  list* l = (list *) calloc(sizeof(list));
  return l;
}

void list_delete(list* l) {
  if(l) {
    list_item* list_item = l->head;
    if(list_item) {
      while(list_item) {
        list_item* next = list_item->next;
        free(list_item);
        list_item = next;
      }
    }
    free(l);
  }
}
