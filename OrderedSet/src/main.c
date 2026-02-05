#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "orderedset.h"

typedef struct {
  int key;
  char* value;
} Entry;

typedef struct {
  char* trad;
  char* simp;
  char* pinyin;
  char* translation;
} ChineseDictEntry;

char* xstrdup(const char* s);
int entry_cmp(const void* a, const void* b);
int cdentry_cmp(const void* a, const void* b);
void entry_destroy(void* p);
void cdentry_destroy(void* p);
Entry* entry_create(int key, const char* value);
ChineseDictEntry* cdentry_create(char* tr, char* si, char* pi, char* tl);
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

int entry_cmp(const void* a, const void* b) {
  const Entry* ea = a;
  const Entry* eb = b;
  if (ea->key < eb->key)
    return -1;
  if (ea->key > eb->key)
    return 1;
  return 0;
}

int cdentry_cmp(const void* a, const void* b) {
  const ChineseDictEntry* ea = a;
  const ChineseDictEntry* eb = b;
  return strcmp(ea->trad, eb->trad);
}

void entry_destroy(void* p) {
  Entry* e = p;
  printf("freeing key=%d value=\"%s\"\n", e->key, e->value);
  free(e->value);
  free(e);
}

void cdentry_destroy(void* p) {
  ChineseDictEntry* e = p;
  free(e->trad);
  free(e->simp);
  free(e->pinyin);
  free(e->translation);
  free(e);
}

Entry* entry_create(int key, const char* value) {
  Entry* e = malloc(sizeof(*e));
  assert(e);
  e->key = key;
  e->value = xstrdup(value);
  assert(e->value);
  return e;
}

ChineseDictEntry* cdentry_create(char* tr, char* si, char* pi, char* tl) {
  ChineseDictEntry* e = malloc(sizeof(*e));
  e->trad = tr;
  e->simp = si;
  e->pinyin = pi;
  e->translation = tl;
  return e;
}

/* ---------- Tests ---------- */

void example1(void) {
  printf("EXAMPLE 1\n---------\n\nCreating set...\n");
  OrderedSet* set = set_initialize(entry_cmp, entry_destroy);
  assert(set);
  /* Insert */
  printf("\nInserting elements...\n");
  set_insert(set, entry_create(5, "five"));
  set_insert(set, entry_create(1, "one"));
  set_insert(set, entry_create(3, "three"));
  set_insert(set, entry_create(9, "nine"));
  set_insert(set, entry_create(7, "seven"));
  assert(set_size(set) == 5);
  /* Replace */
  printf("\nReplacing key 3...\n");
  set_insert(set, entry_create(3, "THREE"));
  assert(set_size(set) == 5);
  /* Lookup */
  Entry probe = {.key = 3};
  Entry* found = set_get(set, &probe);
  assert(found);
  assert(strcmp(found->value, "THREE") == 0);
  printf("Lookup key=3 → \"%s\"\n", found->value);
  /* Iteration */
  printf("\nIterating (sorted order, all):\n");
  int expected_keys[] = {1, 3, 5, 7, 9};
  int i = 0;
  OrderedSetIter* it = set_iter_begin(set, NULL, NULL);
  while (set_iter_has_next(it)) {
    Entry* e = set_iter_next(it);
    printf("  %d → %s\n", e->key, e->value);
    assert(e->key == expected_keys[i++]);
  }
  set_iter_destroy(it);
  /* Iteration 2 ( >=3 && < 7 */
  printf("\nIterating (sorted order, >=3 && < 7):\n");
  Entry lo = {.key = 3};
  Entry hi = {.key = 7};
  it = set_iter_begin(set, &lo, &hi);
  while (set_iter_has_next(it)) {
    Entry* e = set_iter_next(it);
    printf("  %d → %s\n", e->key, e->value);
  }
  set_iter_destroy(it);
  assert(i == 5);
  /* Remove */
  printf("\nRemoving key 5...\n");
  Entry probe2 = {.key = 5};
  set_remove(set, &probe2);
  assert(set_size(set) == 4);
  assert(!set_includes(set, &probe2));
  /* Final iteration */
  printf("\nAfter removal:\n");
  it = set_iter_begin(set, NULL, NULL);
  while (set_iter_has_next(it)) {
    Entry* e = set_iter_next(it);
    printf("  %d → %s\n", e->key, e->value);
  }
  set_iter_destroy(it);
  /* Destroy */
  printf("\nDestroying set...\n");
  set_destroy(set);
  printf("\nAll tests passed ✅\n");
}

void example2(void) {
  printf("EXAMPLE 2\n---------\n");
  OrderedSet* set = set_initialize(cdentry_cmp, cdentry_destroy);
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
    char* tab = NULL;
    char* trad = NULL;
    char* simp = NULL;
    char* pinyin = NULL;
    char* translation = NULL;
    int index = 0;
    while ((tab = strchr(field, '\t'))) {
      *tab = '\0';
      index++;
      switch (index) {
        case 1:
          trad = xstrdup(field);
          break;
        case 2:
          simp = xstrdup(field);
          break;
        case 3:
          pinyin = xstrdup(field);
          break;
      }
      field = tab + 1;
    }
    translation = xstrdup(field);
    set_insert(set, cdentry_create(trad, simp, pinyin, translation));
  }
  free(line);
  fclose(fp);
  printf("set size = %ld\n", set_size(set));

  puts("Iterating the first 20 entries");
  OrderedSetIter* it = set_iter_begin(set, NULL, NULL);
  int i = 0;
  while (set_iter_has_next(it)) {
    ChineseDictEntry* e = set_iter_next(it);
    printf("%02d)  %s → %s\n", i + 1, e->trad, e->translation);
    i++;
    if (i == 20)
      break;
  }
  set_iter_destroy(it);

  puts("Iterating all starting 法蘭克...");
  ChineseDictEntry lo = {.trad = "法蘭克"};
  ChineseDictEntry hi = {.trad = "法蘭克\xFF\xFF\xFF"};
  it = set_iter_begin(set, &lo, &hi);
  i = 0;
  while (set_iter_has_next(it)) {
    ChineseDictEntry* e = set_iter_next(it);
    printf("%02d)  %s → %s\n", i + 1, e->trad, e->translation);
    i++;
  }
  set_iter_destroy(it);

  puts("Searching and getting");
  char* s = xstrdup("我");
  ChineseDictEntry search = (ChineseDictEntry){.trad = s};
  ChineseDictEntry* found = set_get(set, &search);
  if (found) {
    printf("Entry found: %s\n", search.trad);
    printf("  Trad  : %s\n", found->trad);
    printf("  Simp  : %s\n", found->simp);
    printf("  Pinyin: %s\n", found->pinyin);
    printf("  Transl: %s\n", found->translation);
  } else {
    printf("Entry '%s' not found\n", search.trad);
  }
  free(s);

  s = xstrdup("法蘭克福");
  search = (ChineseDictEntry){.trad = s};
  found = set_get(set, &search);
  if (found) {
    printf("Entry found: %s\n", search.trad);
    printf("  Trad  : %s\n", found->trad);
    printf("  Simp  : %s\n", found->simp);
    printf("  Pinyin: %s\n", found->pinyin);
    printf("  Transl: %s\n", found->translation);
  } else {
    printf("Entry '%s' not found\n", search.trad);
  }
  free(s);

  s = xstrdup("哎呀");
  search = (ChineseDictEntry){.trad = s};
  found = set_get(set, &search);
  if (found) {
    printf("Entry found: %s\n", search.trad);
    printf("  Trad  : %s\n", found->trad);
    printf("  Simp  : %s\n", found->simp);
    printf("  Pinyin: %s\n", found->pinyin);
    printf("  Transl: %s\n", found->translation);
  } else {
    printf("Entry '%s' not found\n", search.trad);
  }
  free(s);

  s = xstrdup("法蘭克福福");
  search = (ChineseDictEntry){.trad = s};
  found = set_get(set, &search);
  if (found) {
    printf("Entry found: %s\n", search.trad);
    printf("  Trad  : %s\n", found->trad);
    printf("  Simp  : %s\n", found->simp);
    printf("  Pinyin: %s\n", found->pinyin);
    printf("  Transl: %s\n", found->translation);
  } else {
    printf("Entry '%s' not found\n", search.trad);
  }
  free(s);

  puts("Destroy");
  puts("---------");
  set_destroy(set);
}

int main(void) {
  example1();
  example2();
  return 0;
}
