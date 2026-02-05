#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stddef.h>

#define HT_MAX_LOAD_FACTOR 0.75
#define HT_INITIAL_CAPACITY 1024

/* Function pointer types */
typedef size_t (*hash_func)(const void* key);
typedef int (*key_eq_func)(const void* a, const void* b);
typedef void (*ht_free_func)(void* ptr);
typedef void (*ht_iter_func)(const void* key,
                             const void* value,
                             void* user_data);

/* Hash table entry */
typedef struct ht_entry {
  void* key;
  void* value;
  struct ht_entry* next;
} ht_entry;

/* Hash table */
typedef struct hash_table {
  size_t capacity;
  size_t size;
  ht_entry** buckets;
  hash_func hash;
  key_eq_func key_eq;
  ht_free_func free_key;
  ht_free_func free_value;
} hash_table;

/* API */
hash_table* ht_create(const size_t capacity,
                      hash_func hash,
                      key_eq_func key_eq,
                      ht_free_func free_key,
                      ht_free_func free_value);
void ht_destroy(hash_table* ht);
int ht_insert(hash_table* ht, void* key, void* value);
void* ht_get(const hash_table* ht, const void* key);
int ht_remove(hash_table* ht, const void* key);
void ht_foreach(const hash_table* ht,
                ht_iter_func func,
                const size_t limit,
                void* user_data);
size_t ht_size(const hash_table* ht);

#endif
