/*
 * Copyright (c) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include "gup/lexer.h"
#include "gup/state.h"
#include "gup/ptrbox.h"

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
    case '\r':
    case '\t':
    case ' ':
    case '\f':
        return true;
    }

    return false;
}

/*
 * Put a character in the lexer putback buffer to be
 * grabbed later
 *
 * @state:  Compiler state
 * @c:      Character to putback
 */
static inline void
lexer_putback(struct gup_state *state, char c)
{
    if (state == NULL) {
        return;
    }

    state->putback = c;
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
     * If there is anything that we can accept in the
     * putback buffer, take it. Otherwise skip it and
     * start looking for more characters.
     */
    if ((c = state->putback) != '\0') {
        state->putback = '\0';
        if (accept_ws && lexer_is_ws(c))
            return c;
        if (!accept_ws && !lexer_is_ws(c))
            return c;
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

/*
 * Scan for an identifiers
 *
 * @state: Compiler state
 * @lc:    Last character
 * @res:   Token result
 *
 * Returns zero on success
 */
static int
lexer_scan_ident(struct gup_state *state, int lc, struct token *res)
{
    char c, *buf;
    size_t bufcap, bufsz;

    if (state == NULL || res == NULL) {
        errno = -EINVAL;
        return -1;
    }

    switch (lc) {
    case '_':
    case '#':
        break;
    default:
        if (!isalpha(lc)) {
            errno = -EINVAL;
            return -1;
        }
    }

    bufcap = 8;
    bufsz = 0;
    if ((buf = malloc(bufcap)) == NULL) {
        errno = -ENOMEM;
        return -1;
    }

    buf[bufsz++] = lc;
    for (;;) {
        c = lexer_consume(state, true);
        if (!isalnum(c) && c != '_') {
            lexer_putback(state, c);
            buf[bufsz] = '\0';
            break;
        }

        buf[bufsz++] = c;
        if (bufsz >= bufcap - 1) {
            bufcap += 8;
            buf = realloc(buf, bufcap);
        }

        if (buf == NULL) {
            errno = -ENOMEM;
            return -1;
        }
    }

    res->type = TT_IDENT;
    res->s = ptrbox_strdup(&state->ptrbox, buf);
    free(buf);
    return 0;
}

/*
 * Check if an identifier token is actually a keyword
 * and override it if so
 *
 * @tok: Token to check
 */
static int
lexer_check_kw(struct token *tok)
{
    if (tok == NULL) {
        errno = -EINVAL;
        return -1;
    }

    if (tok->type != TT_IDENT) {
        errno = -EINVAL;
        return -1;
    }

    switch (*tok->s) {
    case '#':
        if (strcmp(tok->s, "#define") == 0) {
            tok->type = TT_DEFINE;
            return 0;
        }

        if (strcmp(tok->s, "#ifdef") == 0) {
            tok->type = TT_IFDEF;
            return 0;
        }

        if (strcmp(tok->s, "#ifndef") == 0) {
            tok->type = TT_IFNDEF;
            return 0;
        }

        if (strcmp(tok->s, "#endif") == 0) {
            tok->type = TT_ENDIF;
            return 0;
        }

        break;
    case 'v':
        if (strcmp(tok->s, "void") == 0) {
            tok->type = TT_VOID;
            return 0;
        }

        break;
    case 'p':
        if (strcmp(tok->s, "proc") == 0) {
            tok->type = TT_PROC;
            return 0;
        }

        if (strcmp(tok->s, "pub") == 0) {
            tok->type = TT_PUB;
            return 0;
        }

        break;
    }

    return -1;
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
    case '\n':
        res->type = TT_NEWLINE;
        res->c = c;
        return 0;
    case '+':
        res->type = TT_PLUS;
        res->c = c;
        return 0;
    case '-':
        res->type = TT_MINUS;
        res->c = c;
        if ((c = lexer_consume(state, false)) != '>') {
            lexer_putback(state, c);
            return 0;
        }

        res->type = TT_ARROW;
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
        if ((c = lexer_consume(state, true)) != '=') {
            lexer_putback(state, c);
            return 0;
        }

        res->type = TT_GTE;
        return 0;
    case '<':
        res->type = TT_LT;
        res->c = c;
        if ((c = lexer_consume(state, true)) != '=') {
            lexer_putback(state, c);
            return 0;
        }

        res->type = TT_LTE;
        return 0;
    case '(':
        res->type = TT_LPAREN;
        res->c = c;
        return 0;
    case ')':
        res->type = TT_RPAREN;
        res->c = c;
        return 0;
    case '{':
        res->type = TT_LBRACE;
        res->c = c;
        return 0;
    case '}':
        res->type = TT_RBRACE;
        res->c = c;
        return 0;
    case ';':
        res->type = TT_SEMI;
        res->c = c;
        return 0;
    default:
        if (lexer_scan_ident(state, c, res) == 0) {
            lexer_check_kw(res);
            return 0;
        }

        break;
    }

    return -1;
}
