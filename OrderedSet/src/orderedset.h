#ifndef ORDEREDSET_H
#define ORDEREDSET_H

#include <stdbool.h>
#include <stddef.h>

/* User-supplied hooks */
typedef int (*Comparator)(const void* a, const void* b);
typedef void (*Destructor)(void* elem);

/* Opaque types */
typedef struct OrderedSet OrderedSet;
typedef struct OrderedSetIter OrderedSetIter;

/* Core API */
OrderedSet* set_initialize(Comparator cmp, Destructor dtor);
void set_destroy(OrderedSet* set);

size_t set_size(const OrderedSet* set);
bool set_includes(const OrderedSet* set, const void* key);
void* set_get(const OrderedSet* set, const void* key);

/* Insert (replace if key exists) */
void set_insert(OrderedSet* set, void* element);

/* Remove */
void set_remove(OrderedSet* set, const void* key);

/* Iterators (in-order, ascending, range optional) */
OrderedSetIter* set_iter_begin(const OrderedSet* set,
                               const void* low,   /* inclusive, may be NULL */
                               const void* high); /* exclusive, may be NULL */
bool set_iter_has_next(const OrderedSetIter* it);
void* set_iter_next(OrderedSetIter* it);
void set_iter_destroy(OrderedSetIter* it);

#endif /* ORDEREDSET_H */
