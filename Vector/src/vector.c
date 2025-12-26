#include "vector.h"

#include <stdlib.h>
#include <string.h>

#define VEC_GROWTH_FACTOR 2

/* ---------- Internal ---------- */

static int vector_resize(vector* vec, size_t new_cap) {
  vect_elem* p = realloc(vec->data, new_cap * sizeof(vect_elem));
  if (!p)
    return -1;
  vec->data = p;
  vec->capacity = new_cap;
  return 0;
}

/* ---------- Lifecycle ---------- */

vector* vector_create(const size_t initial_capacity,
                      vec_key_cmp_func cmp_func,
                      vec_free_func free_func) {
  if (!cmp_func || initial_capacity == 0)
    return NULL;

  vector* vec = malloc(sizeof(vector));
  if (!vec)
    return NULL;

  vec->data = malloc(initial_capacity * sizeof(vect_elem));
  if (!vec->data) {
    free(vec);
    return NULL;
  }

  vec->size = 0;
  vec->capacity = initial_capacity;
  vec->sorted = 0;
  vec->cmp_func = cmp_func;
  vec->free_func = free_func;

  return vec;
}

void vector_clear(vector* vec) {
  if (!vec)
    return;

  if (vec->free_func) {
    for (size_t i = 0; i < vec->size; i++)
      vec->free_func(vec->data[i].key, vec->data[i].value);
  }

  vec->size = 0;
  vec->sorted = 0;
}

void vector_destroy(vector* vec) {
  if (!vec)
    return;
  vector_clear(vec);
  free(vec->data);
  free(vec);
}

/* ---------- Modification ---------- */

int vector_push_back(vector* vec, void* key, void* value) {
  if (vec->size == vec->capacity) {
    if (vector_resize(vec, vec->capacity * VEC_GROWTH_FACTOR) != 0)
      return -1;
  }
  vec->data[vec->size++] = (vect_elem){key, value};
  vec->sorted = 0;
  return 0;
}

int vector_insert_after(vector* vec,
                        const size_t index,
                        void* key,
                        void* value) {
  if (index >= vec->size)
    return -1;

  if (vec->size == vec->capacity) {
    if (vector_resize(vec, vec->capacity * VEC_GROWTH_FACTOR) != 0)
      return -1;
  }

  memmove(&vec->data[index + 2], &vec->data[index + 1],
          (vec->size - index - 1) * sizeof(vect_elem));

  vec->data[index + 1] = (vect_elem){key, value};
  vec->size++;
  vec->sorted = 0;
  return 0;
}

int vector_delete(vector* vec, const size_t index) {
  if (index >= vec->size)
    return -1;

  if (vec->free_func)
    vec->free_func(vec->data[index].key, vec->data[index].value);

  memmove(&vec->data[index], &vec->data[index + 1],
          (vec->size - index - 1) * sizeof(vect_elem));

  vec->size--;
  return 0;
}

/* ---------- Access ---------- */

vect_elem* vector_get(vector* vec, const size_t index) {
  if (index >= vec->size)
    return NULL;
  return &vec->data[index];
}

/* ---------- Stable Merge Sort ---------- */

static void merge(vector* vec, vect_elem* tmp, size_t l, size_t m, size_t r) {
  size_t i = l, j = m, k = l;

  while (i < m && j < r) {
    if (vec->cmp_func(tmp[i].key, tmp[j].key) <= 0)
      vec->data[k++] = tmp[i++];
    else
      vec->data[k++] = tmp[j++];
  }

  while (i < m)
    vec->data[k++] = tmp[i++];
  while (j < r)
    vec->data[k++] = tmp[j++];
}

static void merge_sort(vector* vec, vect_elem* tmp, size_t l, size_t r) {
  if (r - l < 2)
    return;

  size_t m = (l + r) / 2;
  merge_sort(vec, tmp, l, m);
  merge_sort(vec, tmp, m, r);

  memcpy(&tmp[l], &vec->data[l], (r - l) * sizeof(vect_elem));
  merge(vec, tmp, l, m, r);
}

void vector_sort_stable(vector* vec) {
  if (!vec || vec->size < 2)
    return;

  vect_elem* tmp = malloc(vec->size * sizeof(vect_elem));
  if (!tmp)
    return;

  merge_sort(vec, tmp, 0, vec->size);
  free(tmp);
  vec->sorted = 1;
}

/* ---------- Binary Search ---------- */

vect_elem* vector_binary_search(const vector* vec, const void* key) {
  if (!vec || !vec->sorted)
    return NULL;

  size_t l = 0, r = vec->size;
  while (l < r) {
    size_t m = (l + r) / 2;
    int c = vec->cmp_func(key, vec->data[m].key);
    if (c == 0)
      return &vec->data[m];
    if (c < 0)
      r = m;
    else
      l = m + 1;
  }
  return NULL;
}

/* ---------- Iteration ---------- */

void vector_iterate(const vector* vec,
                    vec_iter_func fn,
                    const size_t limit,
                    void* ud) {
  if (!vec || !fn)
    return;
  size_t count = 0;
  for (size_t i = 0; i < vec->size; i++) {
    fn(i, vec->data[i].key, vec->data[i].value, ud);
    count++;
    if (limit != 0)
      if (count >= limit)
        return;
  }
}

void vector_iterate_reverse(const vector* vec,
                            vec_iter_func fn,
                            const size_t limit,
                            void* ud) {
  if (!vec || !fn)
    return;
  size_t count = 0;
  for (size_t i = vec->size; i-- > 0;) {
    fn(i, vec->data[i].key, vec->data[i].value, ud);
    count++;
    if (limit != 0)
      if (count >= limit)
        return;
  }
}

void vector_iterate_sorted(vector* vec,
                           vec_iter_func fn,
                           const size_t limit,
                           void* ud) {
  if (!vec || !fn)
    return;
  if (!vec->sorted)
    vector_sort_stable(vec);
  vector_iterate(vec, fn, limit, ud);
}

void vector_iterate_sorted_reverse(vector* vec,
                                   vec_iter_func fn,
                                   const size_t limit,
                                   void* ud) {
  if (!vec || !fn)
    return;
  if (!vec->sorted)
    vector_sort_stable(vec);
  vector_iterate_reverse(vec, fn, limit, ud);
}

/* ---------- Size / Is sorted? ---------- */

size_t vector_size(const vector* vec) {
  return vec ? vec->size : 0;
}

int vector_is_sorted(const vector* vec) {
  return vec ? vec->sorted : 0;
}
