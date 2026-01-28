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
    TT_IDENT,       /* <IDENT> */
    TT_NEWLINE,     /* '\n' */
    TT_DEFINE,      /* '#define' */
    TT_IFDEF,       /* '#ifdef' */
    TT_ENDIF,       /* '#endif' */
    TT_PLUS,        /* '+' */
    TT_MINUS,       /* '-' */
    TT_STAR,        /* '*' */
    TT_SLASH,       /* '/' */
    TT_GT,          /* '>' */
    TT_LT,          /* '<' */
    TT_GTE,         /* '>=' */
    TT_LTE,         /* '<=' */
    TT_LPAREN,      /* '(' */
    TT_RPAREN,      /* ')' */
    TT_LBRACE,      /* '{' */
    TT_RBRACE,      /* '}' */
    TT_PUB,         /* 'pub' */
    TT_PROC,        /* 'proc' */
    TT_VOID,        /* 'void' */
} tt_t;

struct token {
    tt_t type;
    union {
        char c;
        char *s;
    };
};

#endif  /* !GUP_TOKEN_H */
