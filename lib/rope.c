#include "rope.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

static
bool is_leaf(struct rope_t *root);

static
struct rope_t* empty();

struct rope_t* rope_create(const char *data, unsigned int length) {
    struct rope_t *node = malloc(sizeof(*node));

    node->left_length = 0;
    node->left = node->right = 0;

    if (data) {
        node->length = length;
        node->data = malloc(sizeof(*data)*length);
        memcpy(node->data, data, length);
    }
    else {
        node->length = 0;
        node->data = 0;
    }

    return node;
}

void rope_destroy(struct rope_t *root) {
    if (!is_leaf(root)) {
        rope_destroy(root->left);
        rope_destroy(root->right);
    }

    if (root->data)
        free(root->data);

    free(root);
}

unsigned int rope_length(struct rope_t *root) {
    return is_leaf(root) ?
        root->length :
        (root->left_length + rope_length(root->right));
}

void rope_iter(struct rope_t* root, node_cb cb, void *ctx) {
    if (is_leaf(root)) {
        cb(root->data, root->length, ctx);
    }
    else {
        rope_iter(root->left, cb, ctx);
        rope_iter(root->right, cb, ctx);
    }
}

char rope_char(struct rope_t* root, unsigned int position) {
    if (is_leaf(root)) {
        return root->data[position];
    }
    else {
        if (root->left_length <= position) {
            return rope_char(root->right, position - root->left_length);
        }
        else {
            return rope_char(root->left, position);
        }
    }
}

struct rope_t* rope_concat(struct rope_t* left, struct rope_t* right) {
    unsigned int len = rope_length(left);

    struct rope_t* concatenation = malloc(sizeof(struct rope_t));

    concatenation->left_length = len;
    concatenation->left = left;
    concatenation->right = right;

    concatenation->length = 0;
    concatenation->data = 0;

    return concatenation;
}

struct rope_t *rope_split(struct rope_t *root, unsigned int position) {
    struct rope_t *tail;
    if (is_leaf(root)) {
        assert(position <= root->length);

        tail = rope_create(&root->data[position], root->length - position);
        root->length = position;

    }
    else {
        if (root->left_length < position) {
            tail = rope_split(root->right, position - root->left_length);

        }
        else {
            if (root->left_length == position) {
                tail = root->right;
            }
            else {
                tail = rope_split(root->left, position);
                tail = rope_concat(tail, root->right);
            }
            root->right = empty();
            root->left_length = position;
        }

    }
    return tail;
}

struct rope_t* rope_insert(struct rope_t* root, unsigned int position,
        char* data, unsigned int length) {
    assert(root);
    assert(data);
    assert(length > 0);

    struct rope_t *head, *tail;

    head = root;
    tail = rope_split(root, position);

    return rope_concat(head, rope_concat(rope_create(data, length), tail));
}


struct rope_t* rope_delete(struct rope_t *root, unsigned int from_position, unsigned int to_position) {
    assert(root);
    assert(to_position > from_position);

    struct rope_t *head, *middle, *tail;

    head = root;
    middle = rope_split(root, from_position);

    tail = rope_split(middle, to_position - from_position);

    rope_destroy(middle);
    return rope_concat(head, tail);
}

unsigned int rope_position(struct rope_t *root, int signed_position) {
    if (signed_position >= 0) {
        return (unsigned int)signed_position;
    }
    else {
        int rope_position = rope_length(root) + signed_position + 1;
        assert(rope_position >= 0);
        return (unsigned int)rope_position;
    }
}


static
bool is_leaf(struct rope_t *root) {
    return !(root->left || root->right);
}

static
struct rope_t* empty() {
    return rope_create(0, 0);
}



