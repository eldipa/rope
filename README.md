# Rope

Implementation of a ``rope`` a data structure that allow insertions,
deletions and random access of a string in an efficient manner.

## Primitives

### Create & destroy primitives

The creation is ``O(1)`` and the destruction is ``O(log N)``.

```cpp
// Create a rope
// If data is not null, then initialize the rope with that
// string of the given length
// Create an empty rope otherwise
struct rope_t* rope_create(const char* data, unsigned int length);

// Release the resources of the node.
void rope_destroy(struct rope_t *root);
```

### Lower primitives

All of them are ``O(log N)`` in the worst case.

```cpp
// Return the char in the given position
char rope_char(struct rope_t* root, unsigned int position);

// Return the length of the string hold in the rope
unsigned int rope_length(struct rope_t *root);

// Concatenate two rope trees and return the concatenation.
struct rope_t* rope_concat(struct rope_t* left, struct rope_t* right);

// Split the rope tree in two at the given position.
// The rope root will hold all the strings with lower positions
// than the given in the parameter.
// The ropo returned will hold all the strings with higher postions
// than the given.
struct rope_t *rope_split(struct rope_t *root, unsigned int position);

// Calculate the absolute position (unsigned) from a given signed one:
//  - if the given position is positive or zero, that is used as the position
//    counting from the begin of the rope
//  - if it is negative, this is interpreted as the position counting from the
//    the begin: -1 means the last valid position eg.
unsigned int rope_position(struct rope_t *root, int signed_position);
```

### Higher primitives

All of them are ``O(log N)`` in the worst case except the iteration over
the tree which is ``O(N)``.

```cpp
// Insert the string data in the rope at the given position.
// Return the resulting new rope
struct rope_t* rope_insert(struct rope_t* root, unsigned int position,
        char* data, unsigned int length);

// Remove the string data in the rope that span the from and to positions.
// Return the resulting new rope
struct rope_t* rope_delete(struct rope_t *root, unsigned int from_position,
        unsigned int to_position);

// Iterate all string chunks (leafs in the rope tree)
// and for each string call the cb callback.
// This callback will receive:
//  - the string
//  - the length of the string
//  - the ctx parameter
void rope_iter(struct rope_t* root, node_cb cb, void *ctx);
```
