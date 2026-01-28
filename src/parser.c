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
    [TT_NEWLINE]  = symtok("newline"),
    [TT_IDENT]    = symtok("ident"),
    [TT_DEFINE]   = qtok("#define"),
    [TT_IFDEF]    = qtok("#ifdef"),
    [TT_IFNDEF]   = qtok("#ifndef"),
    [TT_ENDIF]    = qtok("#endif"),
    [TT_ARROW]    = qtok("->"),
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
    [TT_SEMI]     = qtok(";"),
    [TT_PUB]      = qtok("pub"),
    [TT_PROC]     = qtok("proc"),
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
    struct token *popped;

    if (state == NULL || tok == NULL) {
        return -1;
    }

    switch (state->cur_pass) {
    case 0:
        return lexer_scan(state, tok);
    case 1:
        popped = tokbuf_pop(&state->tokbuf);
        if (popped == NULL) {
            ueof(state);
            return -1;
        }

        *tok = *popped;
        return 0;
    }

    return -1;
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
    struct symbol *macro;
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
        &macro
    );

    if (error < 0) {
        return -1;
    }

    if (parse_scan(state, tok) < 0) {
        ueof(state);
        return -1;
    }

    while (tok->type != TT_NEWLINE) {
        if (tokbuf_push(&macro->mactok, tok) < 0) {
            return -1;
        }

        if (parse_scan(state, tok) < 0) {
            ueof(state);
            return -1;
        }
    }

    return 0;
}

/*
 * Skip to an '#endif' directive
 *
 * @state: Compiler state
 * @tok:   Last token
 *
 * Returns zero on success
 */
static int
parse_skip_to_endif(struct gup_state *state, struct token *tok)
{
    if (state == NULL || tok == NULL) {
        return -1;
    }

    while (tok->type != TT_ENDIF) {
        if (parse_scan(state, tok) < 0) {
            ueof(state);
            return -1;
        }
    }

    --state->ifx_depth;
    return 0;
}

/*
 * Parse an '#ifdef' directive
 *
 * @state: Compiler state
 * @tok:   Last token
 *
 * Returns zero on success
 */
static int
parse_ifdef(struct gup_state *state, struct token *tok)
{
    struct symbol *symbol;

    if (state == NULL || tok == NULL) {
        return -1;
    }

    if (tok->type != TT_IFDEF) {
        return -1;
    }

    /* EXPECT <IDENT> */
    if (parse_expect(state, tok, TT_IDENT) < 0) {
        return -1;
    }

    symbol = symbol_from_name(&state->symtab, tok->s);
    if (symbol == NULL) {
        parse_skip_to_endif(state, tok);
    }

    return 0;
}

/*
 * Parse an '#ifndef' directive
 *
 * @state: Compiler state
 * @tok:   Last token
 *
 * Returns zero on success
 */
static int
parse_ifndef(struct gup_state *state, struct token *tok)
{
    struct symbol *symbol;

    if (state == NULL || tok == NULL) {
        return -1;
    }

    if (tok->type != TT_IFNDEF) {
        return -1;
    }

    /* EXPECT <IDENT> */
    if (parse_expect(state, tok, TT_IDENT) < 0) {
        return -1;
    }

    symbol = symbol_from_name(&state->symtab, tok->s);
    if (symbol != NULL) {
        parse_skip_to_endif(state, tok);
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
    case TT_IFDEF:
        ++state->ifx_depth;
        if (parse_ifdef(state, tok) < 0) {
            return -1;
        }

        break;
    case TT_IFNDEF:
        ++state->ifx_depth;
        if (parse_ifndef(state, tok) < 0) {
            return -1;
        }

        break;
    case TT_ENDIF:
        if (state->ifx_depth == 0) {
            trace_error(
                state,
                "got #endif without previous #if directive\n"
            );

            return -1;
        }

        --state->ifx_depth;
        break;
    case TT_NEWLINE:
        /* Ignored */
        break;
    default:
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
    struct token tok;
    int error;

    while (lexer_scan(state, &tok) == 0) {
        error = parse_preprocess(state, &tok);
        if (error != 0) {
            return -1;
        }
    }

    if (state->ifx_depth > 0) {
        trace_error(
            state,
            "missing #endif after #if type directive\n"
        );

        return -1;
    }

    return 0;
}

static int
parse_proc(struct gup_state *state, struct token *tok)
{
    if (state == NULL || tok == NULL) {
        return -1;
    }

    if (tok->type != TT_PROC) {
        return -1;
    }

    /* EXPECT <IDENT> */
    if (parse_expect(state, tok, TT_IDENT) < 0) {
        return -1;
    }

    /* EXPECT '(' */
    if (parse_expect(state, tok, TT_LPAREN) < 0) {
        return -1;
    }

    /* EXPECT 'void' : TODO: ALLOW ARGUMENTS */
    if (parse_expect(state, tok, TT_VOID) < 0) {
        return -1;
    }

    /* EXPECT ')' */
    if (parse_expect(state, tok, TT_RPAREN) < 0) {
        return -1;
    }

    /* EXPECT '->' */
    if (parse_expect(state, tok, TT_ARROW) < 0) {
        return -1;
    }

    /* EXPECT 'void' : TODO: ALLOW DIFFERENT TYPES */
    if (parse_expect(state, tok, TT_VOID) < 0) {
        return -1;
    }

    /* EXPECT ';' : TODO: ALLOW BODY */
    if (parse_expect(state, tok, TT_SEMI) < 0) {
        return -1;
    }

    return 0;
}

/*
 * Begin parsing tokens
 *
 * @state: Compiler state
 * @tok:   Most recent token
 *
 * Returns zero on success
 */
static int
parse_begin(struct gup_state *state, struct token *tok)
{
    if (state == NULL || tok == NULL) {
        return -1;
    }

    switch (tok->type) {
    case TT_PROC:
        if (parse_proc(state, tok) < 0) {
            return -1;
        }

        break;
    case TT_PUB:
        /* Modifier */
        break;
    default:
        utok1(state, tok);
        return -1;
    }
    return 0;
}

/*
 * Main parse loop
 *
 * @state: Compiler state
 *
 * Returns zero on success
 */
static int
parse_loop(struct gup_state *state)
{
    struct token *tok;

    if (state == NULL) {
        return -1;
    }

    for (;;) {
        tok = tokbuf_pop(&state->tokbuf);
        if (tok == NULL) {
            break;
        }

        if (parse_begin(state, tok) < 0) {
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
    case 1:
        /* Parsing pass */
        retval = parse_loop(state);
        break;
    }

    ++state->cur_pass;
    return retval;
}
