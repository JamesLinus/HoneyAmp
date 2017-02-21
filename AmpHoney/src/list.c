#include "main.h"
#include "list.h"

size_t list_size(struct list_t* head)
{
  size_t size = 0;
  FOREACH(head, item)
  {
    ++size;
  }
  return size;
}

bool list_empty(struct list_t* head)
{
  return (head == NULL);
}

struct list_t* list_find(struct list_t* head, void* item_data)
{
  struct list_t* node = NULL;

  FOREACH(head, item)
  {
    if(item->data == item_data)
    {
      node = item;
      break;
    }
  }
  return node;
}

void list_push(struct list_t* head, struct request_t* item_data)
{
  struct list_t* new_node = calloc(1, sizeof(struct list_t));

  new_node->data = item_data;
  new_node->next = head;
  head = new_node;
}

void list_pop(struct list_t* head, struct list_t* target_node)
{
  if(head == NULL)
  {
    return;
  }

  struct list_t* prev_node = NULL;

  if(target_node == head)
  {
    head = head->next;
    return;
  }

  FOREACH(head, item)
  {
    if(item->next == target_node)
    {
      prev_node = item;
    }
  }

  if(prev_node == NULL)
  {
    return;
  }

  prev_node->next = target_node->next;
  free(target_node);
}

void list_destroy(struct list_t* head)
{
  FOREACH(head, item)
  {
    if(item == NULL)
    {
      continue;
    }
    free(item);
  }
}
