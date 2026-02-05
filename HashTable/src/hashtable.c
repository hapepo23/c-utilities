#include "hashtable.h"

#include <stdlib.h>

/* Internal Helper */
static int ht_resize(hash_table* ht, const size_t new_capacity);

hash_table* ht_create(const size_t capacity,
                      hash_func hash,
                      key_eq_func key_eq,
                      ht_free_func free_key,
                      ht_free_func free_value) {
  size_t c = (capacity == 0 ? HT_INITIAL_CAPACITY : capacity);
  hash_table* ht = malloc(sizeof(hash_table));
  if (!ht)
    return NULL;
  ht->capacity = c;
  ht->size = 0;
  ht->hash = hash;
  ht->key_eq = key_eq;
  ht->buckets = calloc(c, sizeof(ht_entry*));
  ht->free_key = free_key;
  ht->free_value = free_value;
  return ht;
}

void ht_destroy(hash_table* ht) {
  for (size_t i = 0; i < ht->capacity; i++) {
    ht_entry* e = ht->buckets[i];
    while (e) {
      ht_entry* next = e->next;
      if (ht->free_key)
        ht->free_key(e->key);
      if (ht->free_value)
        ht->free_value(e->value);
      free(e);
      e = next;
    }
  }
  free(ht->buckets);
  free(ht);
}

int ht_insert(hash_table* ht, void* key, void* value) {
  double load = (double)ht->size / ht->capacity;
  if (load > HT_MAX_LOAD_FACTOR) {
    if (ht_resize(ht, ht->capacity * 2) != 0)
      return -1;
  }
  size_t idx = ht->hash(key) % ht->capacity;
  ht_entry* e = ht->buckets[idx];
  while (e) {
    if (ht->key_eq(e->key, key)) {
      if (ht->free_key)
        ht->free_key(key);
      if (ht->free_value)
        ht->free_value(e->value);
      e->value = value;
      return 0;
    }
    e = e->next;
  }
  ht_entry* new_entry = malloc(sizeof(ht_entry));
  if (!new_entry)
    return -1;
  new_entry->key = key;
  new_entry->value = value;
  new_entry->next = ht->buckets[idx];
  ht->buckets[idx] = new_entry;
  ht->size++;
  return 0;
}

void* ht_get(const hash_table* ht, const void* key) {
  size_t idx = ht->hash(key) % ht->capacity;
  ht_entry* e = ht->buckets[idx];
  while (e) {
    if (ht->key_eq(e->key, key))
      return e->value;
    e = e->next;
  }
  return NULL;
}

int ht_remove(hash_table* ht, const void* key) {
  size_t idx = ht->hash(key) % ht->capacity;
  ht_entry* e = ht->buckets[idx];
  ht_entry* prev = NULL;
  while (e) {
    if (ht->key_eq(e->key, key)) {
      if (prev)
        prev->next = e->next;
      else
        ht->buckets[idx] = e->next;
      if (ht->free_key)
        ht->free_key(e->key);
      if (ht->free_value)
        ht->free_value(e->value);
      free(e);
      ht->size--;
      return 0;
    }
    prev = e;
    e = e->next;
  }
  return -1;
}

void ht_foreach(const hash_table* ht,
                ht_iter_func func,
                const size_t limit,
                void* user_data) {
  if (!ht || !func)
    return;
  size_t count = 0;
  for (size_t i = 0; i < ht->capacity; i++) {
    ht_entry* e = ht->buckets[i];
    while (e) {
      func(e->key, e->value, user_data);
      count++;
      if (limit != 0)
        if (count >= limit)
          return;
      e = e->next;
    }
  }
}

static int ht_resize(hash_table* ht, const size_t new_capacity) {
  ht_entry** new_buckets = calloc(new_capacity, sizeof(ht_entry*));
  if (!new_buckets)
    return -1;
  /* Rehash all entries */
  for (size_t i = 0; i < ht->capacity; i++) {
    ht_entry* e = ht->buckets[i];
    while (e) {
      ht_entry* next = e->next;
      size_t idx = ht->hash(e->key) % new_capacity;
      e->next = new_buckets[idx];
      new_buckets[idx] = e;
      e = next;
    }
  }
  free(ht->buckets);
  ht->buckets = new_buckets;
  ht->capacity = new_capacity;
  return 0;
}

size_t ht_size(const hash_table* ht) {
  return ht ? ht->size : 0;
}
