#ifndef LIST_H
#define LIST_H

#define FOREACH(head,node) for(struct list_t* node = head; node != NULL; node = node->next)

struct list_t
{
  struct request_t* data;
  struct list_t* next;
};

size_t list_size(struct list_t* head);
bool list_empty(struct list_t* head);
struct list_t* list_find(struct list_t* head, void* item_data);
void list_push(struct list_t* head, struct request_t* item_data);
void list_pop(struct list_t* head, struct list_t* target_node);
void list_destroy(struct list_t* head);

#endif
