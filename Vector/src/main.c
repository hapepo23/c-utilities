#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vector.h"

typedef struct {
  char* trad;
  char* simp;
  char* pinyin;
  char* translation;
} ChineseDictEntry;

char* xstrdup(const char* s);
int int_cmp(const void* a, const void* b);
int str_cmp(const void* a, const void* b);
void free_pair(void* key, void* value);
char* make_str(const char* s);
void print_vector(const vector* vec, const char* label);
void print_cb(size_t idx, void* k, void* v, void* ud);
void print_chin(size_t index, void* key, void* data, void* user_data);
void free_chin(void* key, void* data);
void example1(void);
void example2(void);
int main(void);

/* ---------- Helpers ---------- */

char* xstrdup(const char* s) {
  size_t len = strlen(s) + 1;
  char* p = malloc(len);
  if (p)
    memcpy(p, s, len);
  return p;
}

int int_cmp(const void* a, const void* b) {
  int ia = *(const int*)a;
  int ib = *(const int*)b;
  return ia - ib;
}

int str_cmp(const void* a, const void* b) {
  const char* sa = a;
  const char* sb = b;
  return strcmp(sa, sb);
}

void free_pair(void* key, void* value) {
  free(key);
  free(value);
}

int* make_int(int v) {
  int* p = malloc(sizeof(int));
  *p = v;
  return p;
}

char* make_str(const char* s) {
  char* p = malloc(strlen(s) + 1);
  strcpy(p, s);
  return p;
}

void print_vector(const vector* vec, const char* label) {
  printf("\n%s (size=%zu, sorted=%d)\n", label, vec->size, vec->sorted);
  for (size_t i = 0; i < vec->size; i++) {
    printf("  [%zu] key=%d value=%s\n", i, *(int*)vec->data[i].key,
           (char*)vec->data[i].value);
  }
}

void print_cb(size_t idx, void* k, void* v, void* ud) {
  (void)ud;
  printf("[%zu] %d -> %s\n", idx, *(int*)k, (char*)v);
}

void print_chin(size_t index, void* key, void* data, void* user_data) {
  (void)user_data; /* unused */
  (void)key;       /* unused */
  ChineseDictEntry* c = data;
  printf("[%zu] Trad: %s, Simp: %s, Pinyin: %s, Transl: %s\n", index, c->trad,
         c->simp, c->pinyin, c->translation);
}

void free_chin(void* key, void* data) {
  free(key);
  ChineseDictEntry* d = data;
  free(d->trad);
  free(d->simp);
  free(d->pinyin);
  free(d->translation);
  free(d);
}

/* ---------- Tests ---------- */

void example1(void) {
  puts("Example 1\n-------");
  printf("Creating vector...\n");

  vector* vec = vector_create(2, int_cmp, free_pair);
  if (!vec) {
    fprintf(stderr, "Failed to create vector\n");
    return;
  }

  /* Push back */
  printf("Testing push_back...\n");
  vector_push_back(vec, make_int(3), make_str("three"));
  vector_push_back(vec, make_int(1), make_str("one"));
  vector_push_back(vec, make_int(2), make_str("two"));

  print_vector(vec, "After push_back");

  /* Insert after */
  printf("\nTesting insert_after (after index 0)...\n");
  vector_insert_after(vec, 0, make_int(99), make_str("ninety-nine"));

  print_vector(vec, "After insert_after");

  /* Access by index */
  printf("\nTesting vector_get...\n");
  vect_elem* e = vector_get(vec, 2);
  if (e) {
    printf("Access index 2: key=%d value=%s\n", *(int*)e->key, (char*)e->value);
  } else {
    printf("Access index 2: not found\n");
  }
  e = vector_get(vec, 17);
  if (e) {
    printf("Access index 17: key=%d value=%s\n", *(int*)e->key,
           (char*)e->value);
  } else {
    printf("Access index 17: not found\n");
  }

  /* Delete */
  printf("\nTesting delete (index 1)...\n");
  printf("Result=%d\n", vector_delete(vec, 1));
  printf("\nTesting delete (index 17)...\n");
  printf("Result=%d\n", vector_delete(vec, 17));

  print_vector(vec, "After delete");

  /* Stable sort */
  printf("\nTesting stable sort...\n");

  /* Add duplicate keys to test stability */
  vector_push_back(vec, make_int(2), make_str("two-A"));
  vector_push_back(vec, make_int(2), make_str("two-B"));
  vector_push_back(vec, make_int(2), make_str("two-C"));

  print_vector(vec, "Before stable sort");

  vector_sort_stable(vec);

  print_vector(vec, "After stable sort");

  printf("\nVerifying stability for key=2:\n");
  for (size_t i = 0; i < vec->size; i++) {
    if (*(int*)vec->data[i].key == 2) {
      printf("  %s\n", (char*)vec->data[i].value);
    }
  }

  /* Binary search */
  printf("\nTesting binary search...\n");
  int key = 3;
  char* ee = vector_binary_search(vec, &key);
  if (ee) {
    printf("Found key=%d value=%s\n", key, ee);
  } else {
    printf("Key %d not found\n", key);
  }
  key = 999;
  ee = vector_binary_search(vec, &key);
  if (ee) {
    printf("Found key=%d value=%s\n", key, ee);
  } else {
    printf("Key %d not found\n", key);
  }

  /* Clear */
  printf("\nTesting clear...\n");
  vector_clear(vec);
  print_vector(vec, "After clear");

  /* Reuse after clear */
  printf("\nTesting reuse after clear...\n");
  vector_push_back(vec, make_int(10), make_str("ten"));
  vector_push_back(vec, make_int(20), make_str("twenty"));

  print_vector(vec, "After reuse");

  /* Destroy */
  printf("\nDestroying vector...\n");
  vector_destroy(vec);

  /* Another test */
  vec = vector_create(2, int_cmp, free_pair);
  if (!vec) {
    fprintf(stderr, "Failed to create vector\n");
    return;
  }

  vector_push_back(vec, make_int(3), make_str("three"));
  vector_push_back(vec, make_int(1), make_str("one"));
  vector_push_back(vec, make_int(2), make_str("two"));
  vector_insert_after(vec, 0, make_int(99), make_str("ninety-nine"));

  printf("\nIndex order:\n");
  vector_iterate(vec, print_cb, 0, NULL);

  printf("\nReverse index order:\n");
  vector_iterate_reverse(vec, print_cb, 0, NULL);

  vector_delete(vec, 1);

  vector_push_back(vec, make_int(2), make_str("two-A"));
  vector_push_back(vec, make_int(2), make_str("two-B"));

  printf("\nSorted order:\n");
  vector_iterate_sorted(vec, print_cb, 0, NULL);

  printf("\nSorted reverse order:\n");
  vector_iterate_sorted_reverse(vec, print_cb, 0, NULL);

  key = 2;
  ee = vector_binary_search(vec, &key);
  printf("\nBinary search key=2 -> %s\n", ee);

  vector_clear(vec);
  printf("\nAfter clear, size=%zu\n", vec->size);

  vector_destroy(vec);

  printf("\n\n");
}

void example2(void) {
  puts("Example 2\n-------");

  vector* chin = vector_create(10, str_cmp, free_chin);
  if (!chin) {
    fprintf(stderr, "Failed to create\n");
    abort();
  }

  FILE* fp = fopen("data/handedict.txt", "r");
  char* line = NULL;
  size_t len = 0;

  if (fp == NULL) {
    perror("Failed to open file");
    abort();
  }

  while (getline(&line, &len, fp) != -1) {
    line[strcspn(line, "\n")] = '\0';

    char* field = line;
    char* tab;
    ChineseDictEntry* d = malloc(sizeof(ChineseDictEntry));

    d->trad = NULL;
    d->simp = NULL;
    d->pinyin = NULL;
    d->translation = NULL;
    int index = 0;

    while ((tab = strchr(field, '\t'))) {
      *tab = '\0';
      index++;
      switch (index) {
        case 1:
          d->trad = xstrdup(field);
          break;
        case 2:
          d->simp = xstrdup(field);
          break;
        case 3:
          d->pinyin = xstrdup(field);
          break;
      }
      field = tab + 1;
    }
    d->translation = xstrdup(field);
    vector_push_back(chin, xstrdup(d->trad), d);
  }
  free(line);
  fclose(fp);

  printf("-------\nSize: %ld\n", vector_size(chin));
  printf("-------\nIs Sorted?: %d\n", vector_is_sorted(chin));
  printf("-------\nThe first 20 unsorted items:\n");
  vector_iterate(chin, print_chin, 20, NULL);

  ChineseDictEntry* ce;
  vect_elem* e = vector_get(chin, 19);
  if (e) {
    ce = e->value;
    printf("Access index 19: key=%s value=%s\n", (char*)e->key,
           ce->translation);
  } else {
    printf("Access index 19: not found\n");
  }

  vector_sort_stable(chin);

  const char* key1 = "就在前面";
  ce = vector_binary_search(chin, key1);
  if (ce) {
    printf("Entry found: %s\n", key1);
    printf("  Trad  : %s\n", ce->trad);
    printf("  Simp  : %s\n", ce->simp);
    printf("  Pinyin: %s\n", ce->pinyin);
    printf("  Transl: %s\n", ce->translation);
  } else {
    printf("Entry '%s' not found\n", key1);
  }

  const char* key2 = "尸身";
  ce = vector_binary_search(chin, key2);
  if (ce) {
    printf("Entry found: %s\n", key2);
    printf("  Trad  : %s\n", ce->trad);
    printf("  Simp  : %s\n", ce->simp);
    printf("  Pinyin: %s\n", ce->pinyin);
    printf("  Transl: %s\n", ce->translation);
  } else {
    printf("Entry '%s' not found\n", key2);
  }

  const char* key3 = "愛和平";
  ce = vector_binary_search(chin, key3);
  if (ce) {
    printf("Entry found: %s\n", key3);
    printf("  Trad  : %s\n", ce->trad);
    printf("  Simp  : %s\n", ce->simp);
    printf("  Pinyin: %s\n", ce->pinyin);
    printf("  Transl: %s\n", ce->translation);
  } else {
    printf("Entry '%s' not found\n", key3);
  }

  const char* key4 = "abcd";
  ce = vector_binary_search(chin, key4);
  if (ce) {
    printf("Entry found: %s\n", key4);
    printf("  Trad  : %s\n", ce->trad);
    printf("  Simp  : %s\n", ce->simp);
    printf("  Pinyin: %s\n", ce->pinyin);
    printf("  Transl: %s\n", ce->translation);
  } else {
    printf("Entry '%s' not found\n", key4);
  }

  printf("-------\nThe first 20 sorted items:\n");
  vector_iterate_sorted(chin, print_chin, 20, NULL);

  e = vector_get(chin, 19);
  if (e) {
    ce = e->value;
    printf("Access index 19: key=%s value=%s\n", (char*)e->key,
           ce->translation);
  } else {
    printf("Access index 19: not found\n");
  }

  printf("-------\nSize: %ld\n", vector_size(chin));
  printf("-------\nIs Sorted?: %d\n", vector_is_sorted(chin));
  puts("-------");

  vector_destroy(chin);
}

int main(void) {
  example1();
  example2();
  return 0;
}