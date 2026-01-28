/*
 * Copyright (c) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "gup/lexer.h"
#include "gup/parser.h"
#include "gup/trace.h"
#include "gup/symbol.h"

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

/* Unexpected end-of-file */
#define ueof(state)                 \
    trace_error(                    \
        (state),                    \
        "unexpected end of file\n"  \
    )

#define utok1(state, tok)        \
    trace_error(                 \
        (state),                 \
        "unexpected token %s\n", \
        tokstr(tok)              \
    )

/* Unexpected token */
#define utok(state, exp, got)               \
    trace_error(                            \
        (state),                            \
        "expected %s, got %s instead\n",    \
        (exp),                              \
        (got)                               \
    )

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
    [TT_LTE]      = qtok("<="),
    [TT_LPAREN]   = qtok("("),
    [TT_RPAREN]   = qtok(")"),
    [TT_LBRACE]   = qtok("{"),
    [TT_RBRACE]   = qtok("}"),
    [TT_VOID]     = qtok("void")
};

/*
 * Parser-side token scan function
 *
 * @state: Compiler state
 * @tok:   Token result
 *
 * Returns zero on success
 */
static int
parse_scan(struct gup_state *state, struct token *tok)
{
    if (state == NULL || tok == NULL) {
        return -1;
    }

    return lexer_scan(state, tok);
}

/*
 * Assert that the next token is of a specific type
 *
 * @state: Compiler state
 * @tok:   Token result
 * @what:  Expected token type
 *
 * Returns zero on success
 */
static int
parse_expect(struct gup_state *state, struct token *tok, tt_t what)
{
    if (state == NULL || tok == NULL) {
        return -1;
    }

    if (parse_scan(state, tok) < 0) {
        ueof(state);
        return -1;
    }

    if (tok->type != what) {
        utok(state, tokstr1(what), tokstr(tok));
        return -1;
    }

    return 0;
}

/*
 * Parse a '#define' preprocessor directive
 *
 * @state: Compiler state
 * @tok:   Token type
 *
 * Returns zero on success
 */
static int
parse_define(struct gup_state *state, struct token *tok)
{
    int error;

    if (state == NULL || tok == NULL) {
        return -1;
    }

    if (tok->type != TT_DEFINE) {
        return -1;
    }

    /* EXPECT <IDENT> */
    if (parse_expect(state, tok, TT_IDENT) < 0) {
        return -1;
    }

    error = symbol_new(
        &state->symtab,
        tok->s,
        SYMBOL_MACRO,
        NULL
    );

    if (error < 0) {
        return -1;
    }

    return 0;
}

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
        if (parse_define(state, tok) < 0) {
            return -1;
        }
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
