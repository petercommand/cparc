#ifndef LIST_H
#define LIST_H

typedef int tag_t;
struct list_item;
typedef struct list_item list_item;
typedef struct list_item {
  list_item* prev;
  list_item* next;
  void* item;
  tag_t tag;
} list_item;

typedef struct {
  list_item* head;
  list_item* tail;
  tag_t tag;
} list;

list* list_new();
list* list_new_with_tag(tag_t tag);
void list_push_back_with_tag(list* l, void* item, tag_t tag);
void list_append(list* a, list* b);
void* list_peek_back(list* l);
void* list_pop_back(list* l);
list* list_copy(list* l);
void list_delete(list* l);
#endif
