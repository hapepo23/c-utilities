# C Utilities

## Hash Table

A generic C implementation of a hash table using:

- Associated data is stored alongside each key
- Both key and data are handled generically via void *
- Memory management hooks are provided for flexibility
- Separate chaining for collision handling
- User-provided hash and key comparison functions
- Automatic resizing of the hash table
- Iteration over all entries (unsorted) with user-provided function

## Linked List

A generic C implementation of a sorted doubly linked list where:

- Keys are sorted using a user-provided comparison function
- Associated data is stored alongside each key
- Both key and data are handled generically via void *
- Memory management hooks are provided for flexibility
- Iteration over all entries (ascending or descending) with user-provided function