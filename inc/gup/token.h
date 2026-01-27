/*
 * Copyright (c) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#ifndef GUP_TOKEN_H
#define GUP_TOKEN_H 1

#include <stdint.h>
#include <stddef.h>

/*
 * Represents valid token types
 */
typedef enum {
    TT_NONE,        /* <NONE> */
    TT_PLUS,        /* '+' */
    TT_MINUS,       /* '-' */
    TT_STAR,        /* '*' */
    TT_SLASH,       /* '/' */
    TT_GT,          /* '>' */
    TT_LT,          /* '<' */
} tt_t;

struct token {
    tt_t type;
    union {
        char c;
    };
};

#endif  /* !GUP_TOKEN_H */
