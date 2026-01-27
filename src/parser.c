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
    [TT_DEFINE]   = qtok("#define"),
    [TT_PLUS]     = qtok("+"),
    [TT_MINUS]    = qtok("-"),
    [TT_STAR]     = qtok("*"),
    [TT_SLASH]    = qtok("/"),
    [TT_GT]       = qtok(">"),
    [TT_LT]       = qtok("<"),
    [TT_GTE]      = qtok(">="),
    [TT_LTE]      = qtok("<=")
};

/*
 * Used during the preprocessor stage to take in tokens
 * and look for directives
 */
static int
parse_preprocess(struct gup_state *state, struct token *tok)
{
    if (state == NULL || tok == NULL) {
        return -1;
    }

    switch (tok->type) {
    case TT_DEFINE:
        printf("got '#define'\n");
        break;
    default:
        printf("got token %s\n", tokstr(&state->last_tok));
        tokbuf_push(&state->tokbuf, tok);
        break;
    }

    return 0;
}

int
gup_parse(struct gup_state *state)
{
    if (state == NULL) {
        return -1;
    }

    while (lexer_scan(state, &state->last_tok) == 0) {
        if (state->cur_pass == 0) {
            parse_preprocess(state, &state->last_tok);
            continue;
        }
    }

    ++state->cur_pass;
    return 0;
}
