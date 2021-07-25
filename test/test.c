#include "../rope/rope.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

struct collector {
    char *data;
    unsigned int written;
};

void _collect(const char *data, unsigned int length, void *ctx) {
    struct collector *collector = (struct collector*)ctx;

    if (length) {
        collector->data = realloc(collector->data,
                collector->written + length + 1 + 1);

        if (!collector->data)
            return; // fail

        char *dst = collector->data + collector->written;

        memcpy(dst, data, length);
        dst += length;
        collector->written += length;

        dst[0] = '|';
        dst += 1;
        collector->written += 1;
    }
}

void collect(struct rope_t* root, struct collector *collector) {
    if (collector->data) {
        free(collector->data);
    }

    collector->written = 0;
    collector->data = malloc(1);    // yes, always reserve X plus 1

    rope_iter(root, _collect, collector);
    collector->data[collector->written] = 0; // just in case
}

void dump(struct rope_t *root) {
    struct collector collector = {0};
    collect(root, &collector);
    printf("%s\n", collector.data);
    free(collector.data);
}

int _test(struct collector *collector, const char *expected, int lineno) {
    if (memcmp(collector->data, expected, strlen(expected)) != 0) {
        printf("\nTest failed, line %i", lineno);
        printf("\n      found: %64s", collector->data);
        printf("\n   expected: %64s\n", expected);

        return -1;
    }
    else {
        printf(".");
        return 0;
    }
}

int _test_c(struct rope_t *root, unsigned int pos, char expected, int lineno) {
    char found = rope_char(root, pos);
    if (found != expected) {
        printf("\nTest failed, line %i", lineno);
        printf("\n      found: %c", found);
        printf("\n   expected: %c\n", expected);

        return -1;
    }
    else {
        printf(".");
        return 0;
    }
}

#define test(a, b)      (_test((a), (b), __LINE__))
#define test_c(a, b, c) (_test_c((a), (b), (c), __LINE__))


int main() {
    struct collector collector = {0};

    struct rope_t *X = rope_create(0, 0);
    X = rope_insert(X, 0, "olamundo", 8);
    X = rope_insert(X, 0, "H", 1);
    collect(X, &collector);
    test(&collector, "H|olamundo|");

    rope_destroy(X);

    struct rope_t *hello = rope_create("helo", 4);
    collect(hello, &collector);
    test(&collector, "helo");

    for (unsigned int i = 0; i < 4; ++i)
        test_c(hello, i, "helo"[i]);

    struct rope_t *world = rope_create("world", 5);             // world 5
    collect(world, &collector);
    test(&collector, "world");

    struct rope_t *hello_world = rope_concat(hello, world);
    collect(hello_world, &collector);
    test(&collector, "helo|world");

    for (unsigned int i = 0; i < 9; ++i)
        test_c(hello_world, i, "heloworld"[i]);

    struct rope_t *word = rope_create("word", 4);
    struct rope_t *last = rope_split(word, 2);

    collect(word, &collector);
    test(&collector, "wo");
    collect(last, &collector);
    test(&collector, "rd");

    rope_destroy(last);
    rope_destroy(word);


    word = rope_create("word", 4);
    last = rope_split(word, 0);

    collect(word, &collector);
    test(&collector, "");
    collect(last, &collector);
    test(&collector, "word");

    rope_destroy(last);
    rope_destroy(word);


    struct rope_t *tail = rope_split(hello_world, 2); // helo|world
    collect(hello_world, &collector);
    test(&collector, "he");
    collect(tail, &collector);
    test(&collector, "lo|world");

    rope_destroy(hello_world);
    hello_world = tail;

    tail = rope_split(hello_world, 4);   // lo|world
    collect(hello_world, &collector);
    test(&collector, "lo|wo");
    collect(tail, &collector);
    test(&collector, "rld");

    rope_destroy(tail);


    tail = rope_split(hello_world, 0);   // lo|wo

    collect(hello_world, &collector);
    test(&collector, "");
    collect(tail, &collector);
    test(&collector, "lo|wo");

    rope_destroy(hello_world);
    hello_world = tail;

    tail = rope_split(hello_world, 2);   // lo|wo

    collect(hello_world, &collector);
    test(&collector, "lo");
    collect(tail, &collector);
    test(&collector, "wo");

    rope_destroy(hello_world);
    hello_world = tail;
    rope_destroy(hello_world);


    hello = rope_create("helo", 4);
    world = rope_create("world", 5);
    hello_world = rope_concat(hello, world);             // helo|world

    hello_world = rope_insert(hello_world, 3, "l", 1);

    collect(hello_world, &collector);
    test(&collector, "hel|l|o|world");


    tail = rope_split(hello_world, 1);
    rope_destroy(hello_world);
    hello_world = tail;

    collect(hello_world, &collector);
    test(&collector, "el|l|o|world");


    hello_world = rope_insert(hello_world, 0, "H", 1);
    hello_world = rope_insert(hello_world, 10, "!", 1);
    hello_world = rope_insert(hello_world, rope_position(hello_world, -1), "!", 1);

    collect(hello_world, &collector);
    test(&collector, "H|el|l|o|world|!|!");

    rope_destroy(hello_world);


    hello_world = rope_concat(rope_create("Hello", 5), rope_create("my", 2));
    tail = rope_concat(rope_create("s", 1), rope_create("Simon", 5));
    word = rope_concat(rope_create("na", 2), rope_create("mei", 3));

    hello_world = rope_concat(hello_world, rope_concat(word, tail));

    collect(hello_world, &collector);
    test(&collector, "Hello|my|na|mei|s|Simon");


    hello_world = rope_insert(hello_world, 5, " ", 1);
    hello_world = rope_insert(hello_world, 8, " ", 1);
    hello_world = rope_insert(hello_world, 13, " ", 1);
    hello_world = rope_insert(hello_world, 16, " ", 1);

    collect(hello_world, &collector);
    test(&collector, "Hello| |my| |na|me| |i|s| |Simon");


    hello_world = rope_delete(hello_world, 0, 6);

    collect(hello_world, &collector);
    test(&collector, "my| |na|me| |i|s| |Simon");


    hello_world = rope_delete(hello_world, 6, 9);

    collect(hello_world, &collector);
    test(&collector, "my| |na|m|s| |Simon");

    rope_destroy(hello_world);
    free(collector.data);

    printf("\n");
    return 0;
}
