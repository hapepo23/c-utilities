#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>

/* Comparison function for keys */
typedef int (*vec_key_cmp_func)(const void* a, const void* b);

/* Free function for elements */
typedef void (*vec_free_func)(void* key, void* value);

/* Iteration callback */
typedef void (*vec_iter_func)(size_t index,
                              void* key,
                              void* value,
                              void* user_data);

typedef struct {
  void* key;
  void* value;
} vect_elem;

typedef struct {
  vect_elem* data;
  size_t size;
  size_t capacity;
  int sorted;
  vec_key_cmp_func cmp_func;
  vec_free_func free_func;
} vector;

/* Lifecycle */
vector* vector_create(const size_t initial_capacity,
                      vec_key_cmp_func cmp_func,
                      vec_free_func free_func);
void vector_clear(vector* vec);
void vector_destroy(vector* vec);

/* Modification */
int vector_push_back(vector* vec, void* key, void* value);
int vector_insert_after(vector* vec,
                        const size_t index,
                        void* key,
                        void* value);
int vector_delete(vector* vec, const size_t index);

/* Access */
vect_elem* vector_get(vector* vec, const size_t index);

/* Sorting & searching */
void vector_sort_stable(vector* vec);
vect_elem* vector_binary_search(const vector* vec, const void* key);

/* Iteration unsorted */
void vector_iterate(const vector* vec,
                    vec_iter_func fn,
                    const size_t limit,
                    void* user_data);
void vector_iterate_reverse(const vector* vec,
                            vec_iter_func fn,
                            const size_t limit,
                            void* user_data);

/* Iteration sorted, with automatic sorting */
void vector_iterate_sorted(vector* vec,
                           vec_iter_func fn,
                           const size_t limit,
                           void* user_data);
void vector_iterate_sorted_reverse(vector* vec,
                                   vec_iter_func fn,
                                   const size_t limit,
                                   void* user_data);

/* Size, is Sorted? */
size_t vector_size(const vector* vec);
int vector_is_sorted(const vector* vec);

#endif
