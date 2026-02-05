#include "orderedset.h"
#include <stdlib.h>

/* ============================================================
 * Internal structures
 * ============================================================ */

typedef struct Node {
  void* elem;
  struct Node* left;
  struct Node* right;
  int height;
} Node;

struct OrderedSet {
  Node* root;
  size_t size;
  Comparator cmp;
  Destructor dtor;
};

struct OrderedSetIter {
  Node** stack;
  size_t top;
  size_t capacity;
  Comparator cmp;
  const void* low;
  const void* high;
};

/* ============================================================
 * Utility helpers
 * ============================================================ */

static int max(int a, int b) {
  return a > b ? a : b;
}

static int height(Node* n) {
  return n ? n->height : 0;
}

static int balance(Node* n) {
  return height(n->left) - height(n->right);
}

static Node* node_create(void* elem) {
  Node* n = malloc(sizeof(Node));
  if (!n)
    return NULL;
  n->elem = elem;
  n->left = n->right = NULL;
  n->height = 1;
  return n;
}

/* ============================================================
 * Rotations
 * ============================================================ */

static Node* rotate_right(Node* y) {
  Node* x = y->left;
  Node* t = x->right;

  x->right = y;
  y->left = t;

  y->height = 1 + max(height(y->left), height(y->right));
  x->height = 1 + max(height(x->left), height(x->right));

  return x;
}

static Node* rotate_left(Node* x) {
  Node* y = x->right;
  Node* t = y->left;

  y->left = x;
  x->right = t;

  x->height = 1 + max(height(x->left), height(x->right));
  y->height = 1 + max(height(y->left), height(y->right));

  return y;
}

/* ============================================================
 * AVL insert (with replace)
 * ============================================================ */

static Node* node_insert(Node* n,
                         Comparator cmp,
                         Destructor dtor,
                         void* elem,
                         bool* inserted) {
  if (!n) {
    *inserted = true;
    return node_create(elem);
  }

  int c = cmp(elem, n->elem);
  if (c == 0) {
    /* replace */
    if (dtor)
      dtor(n->elem);
    n->elem = elem;
    *inserted = false;
    return n;
  }

  if (c < 0)
    n->left = node_insert(n->left, cmp, dtor, elem, inserted);
  else
    n->right = node_insert(n->right, cmp, dtor, elem, inserted);

  n->height = 1 + max(height(n->left), height(n->right));
  int b = balance(n);

  /* LL */
  if (b > 1 && cmp(elem, n->left->elem) < 0)
    return rotate_right(n);

  /* RR */
  if (b < -1 && cmp(elem, n->right->elem) > 0)
    return rotate_left(n);

  /* LR */
  if (b > 1 && cmp(elem, n->left->elem) > 0) {
    n->left = rotate_left(n->left);
    return rotate_right(n);
  }

  /* RL */
  if (b < -1 && cmp(elem, n->right->elem) < 0) {
    n->right = rotate_right(n->right);
    return rotate_left(n);
  }

  return n;
}

/* ============================================================
 * Remove
 * ============================================================ */

static Node* node_min(Node* n) {
  while (n->left)
    n = n->left;
  return n;
}

static Node* node_remove(Node* n,
                         Comparator cmp,
                         Destructor dtor,
                         const void* key,
                         bool* removed) {
  if (!n)
    return NULL;

  int c = cmp(key, n->elem);
  if (c < 0) {
    n->left = node_remove(n->left, cmp, dtor, key, removed);
  } else if (c > 0) {
    n->right = node_remove(n->right, cmp, dtor, key, removed);
  } else {
    *removed = true;

    if (dtor)
      dtor(n->elem);

    if (!n->left || !n->right) {
      Node* tmp = n->left ? n->left : n->right;
      free(n);
      return tmp;
    }

    Node* m = node_min(n->right);
    n->elem = m->elem;
    n->right = node_remove(n->right, cmp, NULL, m->elem, removed);
  }

  n->height = 1 + max(height(n->left), height(n->right));
  int b = balance(n);

  /* rebalance */
  if (b > 1 && balance(n->left) >= 0)
    return rotate_right(n);

  if (b > 1 && balance(n->left) < 0) {
    n->left = rotate_left(n->left);
    return rotate_right(n);
  }

  if (b < -1 && balance(n->right) <= 0)
    return rotate_left(n);

  if (b < -1 && balance(n->right) > 0) {
    n->right = rotate_right(n->right);
    return rotate_left(n);
  }

  return n;
}

/* ============================================================
 * Destroy
 * ============================================================ */

static void node_destroy(Node* n, Destructor dtor) {
  if (!n)
    return;
  node_destroy(n->left, dtor);
  node_destroy(n->right, dtor);
  if (dtor)
    dtor(n->elem);
  free(n);
}

/* ============================================================
 * Public API
 * ============================================================ */

OrderedSet* set_initialize(Comparator cmp, Destructor dtor) {
  OrderedSet* s = malloc(sizeof(OrderedSet));
  if (!s)
    return NULL;
  s->root = NULL;
  s->size = 0;
  s->cmp = cmp;
  s->dtor = dtor;
  return s;
}

void set_destroy(OrderedSet* set) {
  if (!set)
    return;
  node_destroy(set->root, set->dtor);
  free(set);
}

size_t set_size(const OrderedSet* set) {
  return set ? set->size : 0;
}

bool set_includes(const OrderedSet* set, const void* key) {
  return set_get(set, key) != NULL;
}

void* set_get(const OrderedSet* set, const void* key) {
  Node* n = set->root;
  while (n) {
    int c = set->cmp(key, n->elem);
    if (c == 0)
      return n->elem;
    n = (c < 0) ? n->left : n->right;
  }
  return NULL;
}

void set_insert(OrderedSet* set, void* elem) {
  bool inserted = false;
  set->root = node_insert(set->root, set->cmp, set->dtor, elem, &inserted);
  if (inserted)
    set->size++;
}

void set_remove(OrderedSet* set, const void* key) {
  bool removed = false;
  set->root = node_remove(set->root, set->cmp, set->dtor, key, &removed);
  if (removed)
    set->size--;
}

/* ============================================================
 * Iterator (in-order)
 * ============================================================ */

static void push_left_range(OrderedSetIter* it, Node* n) {
  while (n) {
    /* If n->elem < low, discard left subtree */
    if (it->low && it->cmp(n->elem, it->low) < 0) {
      n = n->right;
      continue;
    }
    /* If n->elem >= high, discard right subtree */
    if (it->high && it->cmp(n->elem, it->high) >= 0) {
      n = n->left;
      continue;
    }
    /* Node is within range, push and go left */
    it->stack[it->top++] = n;
    n = n->left;
  }
}

OrderedSetIter* set_iter_begin(const OrderedSet* set,
                               const void* low,
                               const void* high) {
  OrderedSetIter* it = malloc(sizeof(OrderedSetIter));
  it->capacity = set->size + 1;
  it->stack = malloc(sizeof(Node*) * it->capacity);
  it->top = 0;
  it->cmp = set->cmp;
  it->low = low;
  it->high = high;
  push_left_range(it, set->root);
  return it;
}

bool set_iter_has_next(const OrderedSetIter* it) {
  return it->top > 0;
}

void* set_iter_next(OrderedSetIter* it) {
  while (it->top > 0) {
    Node* n = it->stack[--it->top];
    void* elem = n->elem;
    /* Explore right subtree */
    push_left_range(it, n->right);
    /* Final guard (cheap, safe) */
    if (it->low && it->cmp(elem, it->low) < 0)
      continue;
    if (it->high && it->cmp(elem, it->high) >= 0)
      continue;
    return elem;
  }
  return NULL;
}

void set_iter_destroy(OrderedSetIter* it) {
  free(it->stack);
  free(it);
}
