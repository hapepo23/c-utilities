#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stddef.h>

/* Function pointer types */
/* Comparison function: <0 if a < b, =0 if a == b, >0 if a > b */
typedef int (*ll_cmp_func)(const void* a, const void* b);
typedef void (*ll_free_func)(void* ptr);
typedef void (*ll_iter_func)(void* key, void* value, void* user_data);

/* Linked list node */
typedef struct ll_node {
  void* key;
  void* data;
  struct ll_node* prev;
  struct ll_node* next;
} ll_node;

/* Linked list */
typedef struct linked_list {
  struct ll_node* head;
  struct ll_node* tail;
  size_t size;
  ll_cmp_func cmp;
  ll_free_func free_key;
  ll_free_func free_data;
} linked_list;

/* API */
linked_list* ll_create(ll_cmp_func cmp,
                       ll_free_func free_key,
                       ll_free_func free_data);
void ll_destroy(linked_list* list);
int ll_insert(linked_list* list, void* key, void* data);
void* ll_get(const linked_list* list, const void* key);
int ll_remove(linked_list* list, const void* key);
void ll_foreach(const linked_list* list,
                ll_iter_func func,
                const size_t limit,
                void* user_data);
void ll_foreach_reverse(const linked_list* list,
                        ll_iter_func func,
                        const size_t limit,
                        void* user_data);
size_t ll_size(const linked_list* list);

#endif
