/*
 * Copyright (c) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#include <stddef.h>
#include <errno.h>
#include "gup/codegen.h"
#include "gup/trace.h"
#include "gup/symbol.h"
#include "gup/mu.h"

/*
 * Emit machine code for a procedure
 *
 * @state: Compiler state
 * @root:  AST node root
 */
static int
cg_emit_proc(struct gup_state *state, struct ast_node *root)
{
    struct symbol *symbol;
    int retval;

    if (state == NULL || root == NULL) {
        return -1;
    }

    if (root->type != AST_PROC) {
        return -1;
    }

    symbol = symbol_from_id(&state->symtab, root->symid);
    if (symbol == NULL) {
        trace_error(state, "proc root symbol unresolved\n");
        return -1;
    }

    if (root->epilogue) {
        retval = mu_emit_ret(state);
    } else {
        retval = mu_emit_label(state, symbol->name, symbol->pub);
    }

    return retval;
}

int
cg_resolve_node(struct gup_state *state, struct ast_node *root)
{
    if (state == NULL || root == NULL) {
        errno = -EINVAL;
        return -1;
    }

    switch (root->type) {
    case AST_PROC:
        if (cg_emit_proc(state, root) < 0) {
            return -1;
        }

        break;
    default:
        trace_error(state, "unknown ast node %d\n", root->type);
        return -1;
    }

    return 0;
}
