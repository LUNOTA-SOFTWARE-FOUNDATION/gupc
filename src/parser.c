/*
 * Copyright (c) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "gup/lexer.h"
#include "gup/parser.h"

/* Convert token to string */
#define tokstr1(type) \
    toktab[(type)]

/* Convert token to string */
#define tokstr(tok)   \
    tokstr1((tok)->type)

/* Quoted token */
#define qtok(str)   \
    "'" str "'"

/* Symbolic token */
#define symtok(str) \
    "[" str "]"

static const char *toktab[] = {
    [TT_NONE]     = symtok("none"),
    [TT_PLUS]     = qtok("+"),
    [TT_MINUS]    = qtok("-"),
    [TT_STAR]     = qtok("*"),
    [TT_SLASH]    = qtok("/"),
    [TT_GT]       = qtok(">"),
    [TT_LT]       = qtok("<")
};

int
gup_parse(struct gup_state *state)
{
    if (state == NULL) {
        return -1;
    }

    while (lexer_scan(state, &state->last_tok) == 0) {
        printf("got token %s\n", tokstr(&state->last_tok));
    }

    return 0;
}
