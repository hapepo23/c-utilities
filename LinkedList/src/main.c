#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linkedlist.h"

typedef struct {
  char* id;
  double score;
} record_t;

typedef struct {
  char* trad;
  char* simp;
  char* pinyin;
  char* translation;
} ChineseDictEntry;

char* xstrdup(const char* s);
int int_cmp(const void* a, const void* b);
int str_cmp(const void* a, const void* b);
void print_item_1(void* key, void* data, void* user_data);
void print_item_2(void* key, void* data, void* user_data);
void print_chin(void* key, void* data, void* user_data);
void record_free(void* ptr);
void free_chin(void* ptr);
void example1(void);
void example2(void);
void example3(void);
int main(void);

char* xstrdup(const char* s) {
  size_t len = strlen(s) + 1;
  char* p = malloc(len);
  if (p)
    memcpy(p, s, len);
  return p;
}

int int_cmp(const void* a, const void* b) {
  return (*(int*)a - *(int*)b);
}

int str_cmp(const void* a, const void* b) {
  const char* sa = a;
  const char* sb = b;
  return strcmp(sa, sb);
}

void print_item_1(void* key, void* data, void* user_data) {
  (void)user_data; /* unused */
  printf("%d = %s\n", *(int*)key, (char*)data);
}

void print_item_2(void* key, void* data, void* user_data) {
  (void)user_data; /* unused */
  const char* k = key;
  const record_t* r = data;
  printf("key=\"%s\", id=%s, score=%.2f\n", k, r->id, r->score);
}

void print_chin(void* key, void* data, void* user_data) {
  (void)user_data; /* unused */
  (void)key;       /* unused */
  ChineseDictEntry* c = data;
  printf("Trad: %s, Simp: %s, Pinyin: %s, Transl: %s\n", c->trad, c->simp,
         c->pinyin, c->translation);
}

void record_free(void* data) {
  const record_t* r = data;
  free(r->id);
  free(data);
}

void free_chin(void* ptr) {
  ChineseDictEntry* d = ptr;
  free(d->trad);
  free(d->simp);
  free(d->pinyin);
  free(d->translation);
  free(d);
}

void example1(void) {
  linked_list* list = ll_create(int_cmp, free, free);

  int values[] = {50, 10, 30};
  char buf[32];

  puts("EXAMPLE 1\n-------");

  for (size_t i = 0; i < 3; ++i) {
    int* k = malloc(sizeof(int));
    *k = values[i];
    snprintf(buf, sizeof(buf), "value-%d", values[i]);
    ll_insert(list, k, xstrdup(buf));
  }

  ll_foreach(list, print_item_1, 0, NULL);
  puts("-------");

  int* k = malloc(sizeof(int));
  *k = 15;
  ll_insert(list, k, xstrdup("value-15"));

  ll_foreach(list, print_item_1, 0, NULL);
  puts("-------");

  puts("Reverse order:");
  ll_foreach_reverse(list, print_item_1, 0, NULL);
  puts("-------");

  printf("Size: %ld\n-------\n", ll_size(list));

  k = malloc(sizeof(int));
  *k = 10;
  ll_insert(list, k, xstrdup("new-value-10"));

  ll_foreach(list, print_item_1, 0, NULL);
  puts("-------");

  int kk = 30;
  printf("Find %d = %s\n-------\n", kk, (char*)ll_get(list, &kk));

  kk = 50;
  printf("Find %d = %s\n-------\n", kk, (char*)ll_get(list, &kk));

  kk = 51;
  printf("Find %d = %s\n-------\n", kk, (char*)ll_get(list, &kk));

  kk = 15;
  ll_remove(list, &kk);

  ll_foreach(list, print_item_1, 0, NULL);
  puts("-------");

  ll_destroy(list);
}

void example2(void) {
  linked_list* list = ll_create(str_cmp, free, record_free);

  const char* keys[] = {"delta", "alpha", "charlie", "bravo"};
  char* ids[] = {"id4", "id1", "id3", "id2"};

  puts("EXAMPLE 2\n-------");

  for (size_t i = 0; i < 4; ++i) {
    record_t* r = malloc(sizeof(*r));
    r->id = xstrdup(ids[i]);
    r->score = (i + 1) * 10.5;
    ll_insert(list, xstrdup(keys[i]), r);
  }

  puts("List contents:");
  ll_foreach(list, print_item_2, 0, NULL);

  printf("-------\nSize: %ld\n", ll_size(list));

  record_t* r = malloc(sizeof(*r));
  r->id = xstrdup("id99");
  r->score = 99.9;
  ll_insert(list, xstrdup("bravo"), r);

  puts("-------\nList contents:");
  ll_foreach(list, print_item_2, 0, NULL);

  const char* find_key = "charlie";
  record_t* found = ll_get(list, find_key);
  if (found)
    printf("-------\nFound \"%s\": id=%s score=%.2f\n", find_key, found->id,
           found->score);

  ll_remove(list, "alpha");

  puts("-------\nAfter removing \"alpha\":");
  ll_foreach(list, print_item_2, 0, NULL);
  puts("-------");

  puts("Reverse order:");
  ll_foreach_reverse(list, print_item_2, 0, NULL);
  puts("-------");

  ll_destroy(list);
}

void example3(void) {
  puts("Example 3\n-------");

  linked_list* chin = ll_create(str_cmp, free, free_chin);
  if (!chin) {
    fprintf(stderr, "Failed to create\n");
    abort();
  }

  FILE* fp = fopen("data/handedict-x.txt", "r");
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
    ll_insert(chin, xstrdup(d->trad), d);
  }
  free(line);
  fclose(fp);

  const char* key1 = "擊鼓";
  ChineseDictEntry* found = ll_get(chin, key1);
  if (found) {
    printf("Entry found: %s\n", key1);
    printf("  Trad  : %s\n", found->trad);
    printf("  Simp  : %s\n", found->simp);
    printf("  Pinyin: %s\n", found->pinyin);
    printf("  Transl: %s\n", found->translation);
  } else {
    printf("Entry '%s' not found\n", key1);
  }

  const char* key2 = "操作方便性";
  found = ll_get(chin, key2);
  if (found) {
    printf("Entry found: %s\n", key2);
    printf("  Trad  : %s\n", found->trad);
    printf("  Simp  : %s\n", found->simp);
    printf("  Pinyin: %s\n", found->pinyin);
    printf("  Transl: %s\n", found->translation);
  } else {
    printf("Entry '%s' not found\n", key2);
  }

  const char* key3 = "書稿";
  found = ll_get(chin, key3);
  if (found) {
    printf("Entry found: %s\n", key3);
    printf("  Trad  : %s\n", found->trad);
    printf("  Simp  : %s\n", found->simp);
    printf("  Pinyin: %s\n", found->pinyin);
    printf("  Transl: %s\n", found->translation);
  } else {
    printf("Entry '%s' not found\n", key3);
  }

  const char* key4 = "abcd";
  found = ll_get(chin, key4);
  if (found) {
    printf("Entry found: %s\n", key4);
    printf("  Trad  : %s\n", found->trad);
    printf("  Simp  : %s\n", found->simp);
    printf("  Pinyin: %s\n", found->pinyin);
    printf("  Transl: %s\n", found->translation);
  } else {
    printf("Entry '%s' not found\n", key4);
  }

  printf("-------\nThe first 20 items:\n");
  ll_foreach(chin, print_chin, 20, NULL);

  printf("-------\nSize: %ld\n", ll_size(chin));
  puts("-------");

  ll_destroy(chin);
}

int main(void) {
  example1();
  example2();
  example3();
  return 0;
}
