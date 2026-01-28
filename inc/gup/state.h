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

/* Maximum scope depth */
#define SCOPE_STACK_MAX 8

/*
 * Represents the compiler state
 *
 * @in_fd:      Input file descriptor
 * @cur_pass:   Current compiler pass (0-based)
 * @line_num:   Current line number
 * @ifx_depth:  #IFXXX directive depth
 * @putback:    Lexer putback buffer
 * @scope_depth: Current scope depth
 * @scope_stack: Used to keep track of scope
 * @tokbuf:     Parser token buffer
 * @ptrbox:     Global pointer box
 * @symtab:     Global symbol table
 */
struct gup_state {
    int in_fd;
    uint8_t cur_pass;
    size_t line_num;
    size_t ifx_depth;
    char putback;
    uint8_t scope_depth;
    tt_t scope_stack[SCOPE_STACK_MAX];
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
