# C Utilities

![C](C.png)

## Hash Table

A generic C implementation of a [**hash table**](https://en.wikipedia.org/wiki/Hash_table) using:

- Associated data is stored alongside each key
- Both key and data are handled generically via void *
- Memory management hooks are provided for flexibility
- Separate chaining for collision handling
- User-provided hash and key comparison functions
- Automatic resizing of the hash table
- Iteration over all entries (unsorted) with user-provided function

## Linked List

A generic C implementation of a sorted doubly [**linked list**](https://en.wikipedia.org/wiki/Linked_list) where:

- Keys are sorted using a user-provided comparison function
- Associated data is stored alongside each key
- Both key and data are handled generically via void *
- Memory management hooks are provided for flexibility
- Iteration over all entries (ascending or descending) with user-provided function

## Vector

A generic C implementation of a [**one-dimensional array data structure (Vector)**](https://en.wikipedia.org/wiki/Array_(data_structure)) where:

- Each vector element has a key and value(s) of any type
- Elements may be sorted according to the keys
- Insert at the end or at specified index
- Stable sort (merge sort) and binary search
- Delete by index
- Memory management hooks are provided for flexibility
- Forward & reverse iteration, sorted or unsorted, with user-provided function
