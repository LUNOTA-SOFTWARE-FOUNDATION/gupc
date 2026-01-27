/*
 * Copyright (c) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#include <stdint.h>
#include <stddef.h>
#include <errno.h>
#include <unistd.h>
#include "gup/lexer.h"
#include "gup/state.h"

/*
 * Returns true if the given input character counts
 * as a whitespace
 *
 * @c: Character to test as whitespace
 */
static inline bool
lexer_is_ws(char c)
{
    switch (c) {
    case '\n':
    case '\r':
    case '\t':
    case ' ':
    case '\f':
        return true;
    }

    return false;
}

/*
 * Consume a single character from the input source file
 *
 * @state:      Compiler state
 * @accept_ws:  If true, accept whitespace
 *
 * Returns the character consumed, otherwise '\0' on end-of-file
 * or error.
 */
static char
lexer_consume(struct gup_state *state, bool accept_ws)
{
    char c;

    if (state == NULL) {
        return '\0';
    }

    /*
     * Begin reading bytes from the input source and if we
     * can, skip all whitespace encountered.
     */
    while (read(state->in_fd, &c, 1) > 0) {
        if (c == '\n')
            ++state->line_num;
        if (!accept_ws && lexer_is_ws(c))
            continue;

        return c;
    }

    return '\0';
}

int
lexer_scan(struct gup_state *state, struct token *res)
{
    char c;

    if (state == NULL || res == NULL) {
        errno = -EINVAL;
        return -1;
    }

    /* Consume a single byte excluding whitespace */
    if ((c = lexer_consume(state, false)) == '\0') {
        return -1;
    }

    switch (c) {
    case '+':
        res->type = TT_PLUS;
        res->c = c;
        return 0;
    case '-':
        res->type = TT_MINUS;
        res->c = c;
        return 0;
    case '*':
        res->type = TT_STAR;
        res->c = c;
        return 0;
    case '/':
        res->type = TT_SLASH;
        res->c = c;
        return 0;
    case '>':
        res->type = TT_GT;
        res->c = c;
        return 0;
    case '<':
        res->type = TT_LT;
        res->c = c;
        return 0;
    default:
        break;
    }

    return -1;
}
