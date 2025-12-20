#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashtable.h"

typedef struct {
  int id;
  char* name;
  double balance;
} User;

typedef struct {
  char* trad;
  char* simp;
  char* pinyin;
  char* translation;
} ChineseDictEntry;

char* xstrdup(const char* s);
char* xstrcpy(char* dest, const char* src, const size_t destsize);
void free_user(void* ptr);
void free_chin(void* ptr);
size_t str_hash(const void* key);
size_t int_hash(const void* key);
int str_eq(const void* a, const void* b);
int int_eq(const void* a, const void* b);
void print_user(const void* key, const void* value, void* user_data);
void print_ex3(const void* key, const void* value, void* user_data);
void print_chin(const void* key, const void* value, void* user_data);
uint64_t* create_key(const int value);
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

char* xstrcpy(char* dest, const char* src, const size_t destsize) {
  if (strlen(src) <= destsize - 1)
    return strcpy(dest, src);
  else {
    char* d = strncpy(dest, src, destsize);
    d[destsize - 1] = '\0';
    return d;
  }
}

void free_user(void* ptr) {
  User* u = ptr;
  free(u->name);
  free(u);
}

void free_chin(void* ptr) {
  ChineseDictEntry* d = ptr;
  free(d->trad);
  free(d->simp);
  free(d->pinyin);
  free(d->translation);
  free(d);
}

size_t str_hash(const void* key) {
  const char* s = key;
  size_t h = 5381;
  while (*s)
    h = ((h << 5) + h) + (unsigned char)*s++;
  return h;
}

size_t int_hash(const void* key) {
  uint64_t x = *(const uint64_t*)key;
  /* mix bits (64-bit finalizer) */
  x ^= x >> 33;
  x *= 0xff51afd7ed558ccdULL;
  x ^= x >> 33;
  x *= 0xc4ceb9fe1a85ec53ULL;
  x ^= x >> 33;
  return (size_t)x;
}

int str_eq(const void* a, const void* b) {
  return strcmp((const char*)a, (const char*)b) == 0;
}

int int_eq(const void* a, const void* b) {
  return *(const uint64_t*)a == *(const uint64_t*)b;
}

void print_user(const void* key, const void* value, void* user_data) {
  (void)user_data; /* unused */

  const char* username = key;
  const User* u = value;

  printf("Key: %s, ID: %d, Name: %s, Balance: %.2f\n", username, u->id, u->name,
         u->balance);
}

void print_ex3(const void* key, const void* value, void* user_data) {
  (void)user_data; /* unused */

  printf("Key: %" PRIu64 ", Data: %s\n", *(const uint64_t*)key,
         (const char*)value);
}

void print_chin(const void* key, const void* value, void* user_data) {
  (void)user_data; /* unused */
  (void)key;       /* unused */

  const ChineseDictEntry* c = value;

  printf("Trad: %s, Simp: %s, Pinyin: %s, Transl: %s\n", c->trad, c->simp,
         c->pinyin, c->translation);
}

uint64_t* create_key(const int value) {
  uint64_t* k = malloc(sizeof(uint64_t));
  *k = value;
  return k;
}

void example1(void) {
  printf("\nExample 1 (string keys, string data)\n");

  hash_table* ht = ht_create(0, str_hash, str_eq, NULL, free);
  if (!ht) {
    fprintf(stderr, "Failed to create hash table\n");
    abort();
  }

  ht_insert(ht, "name", xstrdup("Alice"));
  ht_insert(ht, "city", xstrdup("Paris"));

  printf("name=%s\n", (char*)ht_get(ht, "name"));
  printf("city=%s\n", (char*)ht_get(ht, "city"));
  printf("not-here=%s\n", (char*)ht_get(ht, "not-here"));

  ht_destroy(ht);
}

void example2(void) {
  printf("\nExample 2 (string keys, struct data)\n");

  hash_table* users = ht_create(0, str_hash, str_eq, NULL, free_user);
  if (!users) {
    fprintf(stderr, "Failed to create hash table\n");
    abort();
  }

  User* u1 = malloc(sizeof(User));
  User* u2 = malloc(sizeof(User));
  User* u3 = malloc(sizeof(User));
  User* u4 = malloc(sizeof(User));

  u1->name = xstrdup("Alice");
  u1->id = 1001;
  u1->balance = 1250.50;

  u2->name = xstrdup("Bob");
  u2->id = 1002;
  u2->balance = 980.75;

  u3->name = xstrdup("Charlie Brown");
  u3->id = 1003;
  u3->balance = 500.00;

  u4->name = xstrdup("New Bob");
  u4->id = 1004;
  u4->balance = 1000.00;

  ht_insert(users, "alice", u1);
  ht_insert(users, "bob", u2);
  ht_insert(users, "charlie", u3);

  printf("All users:\n");
  ht_foreach(users, print_user, 0, NULL);

  const char* key1 = "bob";
  User* found = ht_get(users, key1);
  if (found) {
    printf("User found: %s\n", key1);
    printf("  ID: %d\n", found->id);
    printf("  Name: %s\n", found->name);
    printf("  Balance: %.2f\n", found->balance);
  } else {
    printf("User '%s' not found\n", key1);
  }
  const char* key2 = "alice";
  found = ht_get(users, key2);
  if (found) {
    printf("User found: %s\n", key2);
    printf("  ID: %d\n", found->id);
    printf("  Name: %s\n", found->name);
    printf("  Balance: %.2f\n", found->balance);
  } else {
    printf("User '%s' not found\n", key2);
  }
  const char* key3 = "charlie";
  found = ht_get(users, key3);
  if (found) {
    printf("User found: %s\n", key3);
    printf("  ID: %d\n", found->id);
    printf("  Name: %s\n", found->name);
    printf("  Balance: %.2f\n", found->balance);
  } else {
    printf("User '%s' not found\n", key3);
  }
  const char* key4 = "dummy";
  found = ht_get(users, key4);
  if (found) {
    printf("User found: %s\n", key4);
    printf("  ID: %d\n", found->id);
    printf("  Name: %s\n", found->name);
    printf("  Balance: %.2f\n", found->balance);
  } else {
    printf("User '%s' not found\n", key4);
  }

  found = ht_get(users, "alice");
  if (found) {
    found->balance += 250.00;
    printf("Updated Alice's balance: %.2f\n", found->balance);
  }
  const char* key5 = "alice";
  found = ht_get(users, key5);
  if (found) {
    printf("User found: %s\n", key5);
    printf("  ID: %d\n", found->id);
    printf("  Name: %s\n", found->name);
    printf("  Balance: %.2f\n", found->balance);
  } else {
    printf("User '%s' not found\n", key5);
  }

  ht_remove(users, "charlie");
  if (!ht_get(users, "charlie")) {
    printf("Charlie successfully removed\n");
  }
  const char* key6 = "charlie";
  found = ht_get(users, key6);
  if (found) {
    printf("User found: %s\n", key6);
    printf("  ID: %d\n", found->id);
    printf("  Name: %s\n", found->name);
    printf("  Balance: %.2f\n", found->balance);
  } else {
    printf("User '%s' not found\n", key6);
  }

  printf("All users:\n");
  ht_foreach(users, print_user, 0, NULL);

  ht_insert(users, "bob", u4);
  printf("bob replaced\n");

  printf("All users:\n");
  ht_foreach(users, print_user, 0, NULL);

  ht_destroy(users);
}

void example3(void) {
  printf("\nExample 3 (integer keys, string data)\n");

  hash_table* ht = ht_create(0, int_hash, int_eq, free, free);
  if (!ht) {
    fprintf(stderr, "Failed to create hash table\n");
    abort();
  }

  ht_insert(ht, create_key(42), xstrdup("Alice"));
  ht_insert(ht, create_key(242), xstrdup("Adolf"));

  printf("All items:\n");
  ht_foreach(ht, print_ex3, 0, NULL);

  ht_insert(ht, create_key(242), xstrdup("Alfred"));
  printf("242 data replaced\n");

  uint64_t lookup = 42;
  printf("42=%s\n", (char*)ht_get(ht, &lookup));
  lookup = 242;
  printf("242=%s\n", (char*)ht_get(ht, &lookup));
  lookup = 67;
  printf("67=%s\n", (char*)ht_get(ht, &lookup));

  printf("All items:\n");
  ht_foreach(ht, print_ex3, 0, NULL);

  ht_destroy(ht);
}

void example4(void) {
  printf("\nExample 4 (string keys, struct data)\n");

  hash_table* chin = ht_create(0, str_hash, str_eq, free, free_chin);
  if (!chin) {
    fprintf(stderr, "Failed to create hash table\n");
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
    ht_insert(chin, xstrdup(d->trad), d);
  }
  free(line);
  fclose(fp);

  const char* key1 = "我";
  ChineseDictEntry* found = ht_get(chin, key1);
  if (found) {
    printf("Entry found: %s\n", key1);
    printf("  Trad  : %s\n", found->trad);
    printf("  Simp  : %s\n", found->simp);
    printf("  Pinyin: %s\n", found->pinyin);
    printf("  Transl: %s\n", found->translation);
  } else {
    printf("Entry '%s' not found\n", key1);
  }

  const char* key2 = "法蘭克福";
  found = ht_get(chin, key2);
  if (found) {
    printf("Entry found: %s\n", key2);
    printf("  Trad  : %s\n", found->trad);
    printf("  Simp  : %s\n", found->simp);
    printf("  Pinyin: %s\n", found->pinyin);
    printf("  Transl: %s\n", found->translation);
  } else {
    printf("Entry '%s' not found\n", key2);
  }

  const char* key3 = "哎呀";
  found = ht_get(chin, key3);
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
  found = ht_get(chin, key4);
  if (found) {
    printf("Entry found: %s\n", key4);
    printf("  Trad  : %s\n", found->trad);
    printf("  Simp  : %s\n", found->simp);
    printf("  Pinyin: %s\n", found->pinyin);
    printf("  Transl: %s\n", found->translation);
  } else {
    printf("Entry '%s' not found\n", key4);
  }

  printf("-------\nThe first 10 items:\n");
  ht_foreach(chin, print_chin, 10, NULL);

  printf("-------\nSize: %ld\n", ht_size(chin));
  puts("-------");

  ht_destroy(chin);
}

int main(void) {
  example1();
  example2();
  example3();
  example4();
  return 0;
}
