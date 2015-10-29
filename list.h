#ifndef LIST_H
#define LIST_H

typedef int tag_t;
typedef struct {
  list_item* prev;
  list_item* next;
  void* item;
} list_item;

typedef struct {
  list_item* head;
  list_item* tail;
  tag_t tag;
} list;



#endif
