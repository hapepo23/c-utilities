#include "linkedlist.h"

#include <stdlib.h>

linked_list* ll_create(ll_cmp_func cmp,
                       ll_free_func free_key,
                       ll_free_func free_data) {
  if (!cmp)
    return NULL;
  linked_list* list = malloc(sizeof(*list));
  if (!list)
    return NULL;
  list->head = list->tail = NULL;
  list->size = 0;
  list->cmp = cmp;
  list->free_key = free_key;
  list->free_data = free_data;
  return list;
}

void ll_destroy(linked_list* list) {
  if (!list)
    return;
  ll_node* cur = list->head;
  while (cur) {
    ll_node* next = cur->next;
    if (list->free_key)
      list->free_key(cur->key);
    if (list->free_data)
      list->free_data(cur->data);
    free(cur);
    cur = next;
  }
  free(list);
}

int ll_insert(linked_list* list, void* key, void* data) {
  if (!list)
    return -1;
  ll_node* cur = list->head;
  while (cur) {
    int cmp = list->cmp(key, cur->key);
    if (cmp == 0) {
      if (list->free_data)
        list->free_data(cur->data);
      if (list->free_key)
        list->free_key(key); /* discard new key */
      cur->data = data;
      return 0;
    }
    if (cmp < 0)
      break;
    cur = cur->next;
  }
  /* Key not found: create new node */
  ll_node* n = malloc(sizeof(*n));
  if (!n)
    return -1;
  n->key = key;
  n->data = data;
  n->prev = n->next = NULL;
  if (!cur) {
    /* insert at tail */
    n->prev = list->tail;
    if (list->tail)
      list->tail->next = n;
    else
      list->head = n;
    list->tail = n;
  } else {
    /* insert before cur */
    n->next = cur;
    n->prev = cur->prev;
    if (cur->prev)
      cur->prev->next = n;
    else
      list->head = n;
    cur->prev = n;
  }
  list->size++;
  return 0;
}

void* ll_get(const linked_list* list, const void* key) {
  if (!list)
    return NULL;
  ll_node* cur = list->head;
  while (cur) {
    int cmp = list->cmp(key, cur->key);
    if (cmp == 0)
      return cur->data;
    if (cmp < 0)
      return NULL; /* early exit due to sorting */
    cur = cur->next;
  }
  return NULL;
}

int ll_remove(linked_list* list, const void* key) {
  if (!list || !list->head)
    return -1;
  ll_node* cur = list->head;
  while (cur) {
    int cmp = list->cmp(key, cur->key);
    if (cmp == 0) {
      /* unlink node */
      if (cur->prev)
        cur->prev->next = cur->next;
      else
        list->head = cur->next;
      if (cur->next)
        cur->next->prev = cur->prev;
      else
        list->tail = cur->prev;
      /* free owned resources */
      if (list->free_key)
        list->free_key(cur->key);
      if (list->free_data)
        list->free_data(cur->data);
      free(cur);
      list->size--;
      return 0;
    }
    /* early exit because list is sorted */
    if (cmp < 0)
      return -1;
    cur = cur->next;
  }
  return -1;
}

void ll_foreach(const linked_list* list,
                ll_iter_func func,
                const size_t limit,
                void* user_data) {
  if (!list || !func)
    return;
  size_t count = 0;
  for (ll_node* cur = list->head; cur; cur = cur->next) {
    func(cur->key, cur->data, user_data);
    count++;
    if (limit != 0)
      if (count >= limit)
        return;
  }
}

void ll_foreach_reverse(const linked_list* list,
                        ll_iter_func func,
                        const size_t limit,
                        void* user_data) {
  if (!list || !func)
    return;
  size_t count = 0;
  ll_node* cur = list->tail;
  while (cur) {
    func(cur->key, cur->data, user_data);
    count++;
    if (limit != 0)
      if (count >= limit)
        return;
    cur = cur->prev;
  }
}

size_t ll_size(const linked_list* list) {
  return list ? list->size : 0;
}
