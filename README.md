# Rope

Implementation of a ``rope`` a data structure that allow insertions,
deletions and random access of a string in an efficient manner.


## Creation

`rope_create` creates a *rope data structure* which it may be optionally
initialized with an arbitrary string.
The operation is ``O(1)``.

<!--
?: .L rope/librope.so
-->

```cpp
?: #include "rope/rope.h"

?: struct rope_t* r1 = rope_create(NULL, 0); // empty
?: struct rope_t* r2 = rope_create("hello", 5); // we are *not* counting the \0
```

<!--
?: #include <string>

?: std::string pprint(const char *s, unsigned length) {
::   return std::string {s, length};
:: }

?: std::string pprint(struct rope_t *r) {
::   unsigned length = 0;
::   char *s = rope_string(r, NULL, &length);
::   auto s2 = std::string {s, length};
::   free(s);
::   return s2;
:: }
-->

`rope_t` is binary safe so it is not needed to pass null-terminated strings.
In fact, the strings can have arbitrary data including several nulls.

```cpp
?: struct rope_t* r3 = rope_create("\0\1\2\3", 4);
```

## To Strings

The string managed by the `rope_t` structure can be retrieved (copied)
in ``O(N)``.

```cpp
?: unsigned l1 = 0;
?: char *s1 = rope_string(r2, NULL, &l1);
```

`rope_string` returns the string, of course, and its length. Remember
that you need this because we don't use null terminated strings.

```cpp
?: pprint(s1, l1)
(std::string) "hello"
```

The returned string must be deallocated with `free()` by you.

```cpp
?: free(s1);
```

If you prefer another place to store the string, you can pass
a non-null buffer. It is up to you to give a buffer **large enough**:

```cpp
?: char s2[64];
?: unsigned l2 = 0;

?: rope_string(r2, s2, &l2);

?: pprint(s2, l2)
(std::string) "hello"
```


<!--
Edge case: the empty string/node

?: rope_string(r1, s2, &l2);
?: pprint(s2, l2)
(std::string) ""

?: l2
(unsigned int) 0
-->

## Iteration

`rope_string` is just a thin wrapper around `rope_iter`. With this
you can iterate over the rope calling an user-defined callback over each
piece of string in the rope.

The callback will receive the piece of string, its length and the `ctx`
parameter.

```cpp
?: void rope_iter(struct rope_t* root, node_cb cb, void *ctx);
```

As you may suspect, this is a `O(N)` operation.

## Split

`rope_split` takes a `rope_t` and
splits it into the *left* and the *right* parts **without** copying in
`O(log N)`,

```cpp
?: struct rope_t *phrase = rope_create("hello world!", 12);

?: struct rope_t *right = rope_split(phrase, 6);

?: pprint(phrase)
(std::string) "hello "

?: pprint(right)
(std::string) "world!"
```

The returned `rope_t` is the right part of the rope and the initial
`rope_t` is the left.

```cpp
?: struct rope_t *left = phrase;
```

The position where the split happen selects the first character
that will form the right part (the 6th character is the `w` in the
initial string).

<!--
Edge case: split at the begin and at the end

?: struct rope_t *test1 = rope_create("hello world!", 12);
?: struct rope_t *test2 = rope_split(test1, 0);

?: pprint(test1)
(std::string) ""
?: pprint(test2)
(std::string) "hello world!"

?: struct rope_t *test3 = rope_split(test2, 12);

?: pprint(test2)
(std::string) "hello world!"
?: pprint(test3)
(std::string) ""
-->

## Concatenation

The interesting thing about *ropes* is that we can concatenate them:
build a single `rope_t` from others **without** copying. In the worst
case this is ``O(log N)``.

```cpp
?: phrase = rope_concat(left, right);
?: pprint(phrase)
(std::string) "hello world!"
```

`rope_concat` takes the ownership of `left` and `right` so they **must not**
be released; releasing `phrase` will be enough.

In fact, `left` and `right` should be considered **invalid**.

```cpp
?: left = right = NULL;
```

So it is okay to do the following:

```cpp
?: phrase = rope_concat(phrase, rope_create("!!", 2));

?: pprint(phrase)
(std::string) "hello world!!!"
```

But it is **not** okay to do something like this:

```cpp
?: rope_concat(phrase, phrase); // wrong!  // byexample: +skip
```

## Insert and Delete

You can insert strings into the rope or remove parts of it **without**
copying in `O(log N)`.

```cpp
?: phrase = rope_insert(phrase, 5, "---", 3);

?: pprint(phrase)
(std::string) "hello--- world!!!"
```

The insertion happens *before* the selected character at the given
position.

Therefore to *prepend* or *append* you can do:

```cpp
?: phrase = rope_insert(phrase, 0, "<", 1); // prepend
?: phrase = rope_insert(phrase, rope_length(phrase), ">", 1); // append

?: pprint(phrase)
(std::string) "<hello--- world!!!>"
```

The deletion takes a range:

```cpp
?: phrase = rope_delete(phrase, 6, 9);

?: pprint(phrase)
(std::string) "<hello world!!!>"
```

The range is inclusive on the left side and exclusive on the right side
(close-open if you want).


## Access

Few extra functions are supported, all of them are ``O(log N)``
in the worst case.

Return the length of the string hold in the rope:

```cpp
?: unsigned length = rope_length(phrase);
?: length
(unsigned int) 16
```


Return the character in the given position:

```cpp
?: rope_char(phrase, 0)
(char) '<'

?: rope_char(phrase, length-1)
(char) '>'
```

Interpret a negative position as counting from the end of the rope
minus 1 (basically `-2` means the last character)

```cpp
?: unsigned last = rope_position(phrase, -2);
?: last
(unsigned int) 15

?: rope_char(phrase, last)
(char) '>'
```

## Clean up

Don't forget to clean up the ropes that *you* own. This is a
``O(log N)`` operation.

```cpp
?: rope_destroy(phrase);
```

