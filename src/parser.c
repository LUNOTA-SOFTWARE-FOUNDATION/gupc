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
    [TT_IDENT]    = symtok("ident"),
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
        if (tokbuf_push(&state->tokbuf, tok) < 0)
            return -1;

        break;
    }

    return 0;
}

/*
 * Begin curating tokens for the preprocessor stage
 *
 * @state: Compiler state
 *
 * Returns zero on success
 */
static int
parse_curate(struct gup_state *state)
{
    int error;

    while (lexer_scan(state, &state->last_tok) == 0) {
        error = parse_preprocess(state, &state->last_tok);
        if (error != 0) {
            return -1;
        }
    }

    return 0;
}

int
gup_parse(struct gup_state *state)
{
    int retval = 0;

    if (state == NULL) {
        return -1;
    }

    switch (state->cur_pass) {
    case 0:
        /* Preprocessor pass */
        retval = parse_curate(state);
        break;
    }

    ++state->cur_pass;
    return retval;
}
