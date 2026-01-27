/*
 * Copyright (c) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#ifndef GUP_STATE_H
#define GUP_STATE_H 1

#include <stdint.h>
#include "gup/token.h"
#include "gup/tokbuf.h"
#include "gup/ptrbox.h"
#include "gup/symbol.h"

/*
 * Represents the compiler state
 *
 * @in_fd:      Input file descriptor
 * @cur_pass:   Current compiler pass (0-based)
 * @line_num:   Current line number
 * @putback:    Lexer putback buffer
 * @last_tok:   Last token
 * @tokbuf:     Parser token buffer
 * @ptrbox:     Global pointer box
 * @symtab:     Global symbol table
 */
struct gup_state {
    int in_fd;
    uint8_t cur_pass;
    size_t line_num;
    char putback;
    struct token last_tok;
    struct tokbuf tokbuf;
    struct ptrbox ptrbox;
    struct symbol_table symtab;
};

/*
 * Initialize the compiler state
 *
 * @res:        Result is written here
 * @in_path:    Input file path
 * @out_path:   Output file path
 *
 * Returns zero on success
 */
int gup_state_init(struct gup_state *res, const char *in_path, const char *out_path);

/*
 * Destroy a previously initialized GUP state
 *
 * @state: State to destroy
 */
void gup_state_destroy(struct gup_state *state);

#endif  /* !GUP_STATE_H */
