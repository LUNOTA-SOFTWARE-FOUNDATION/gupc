/*
 * Copyright (c) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#ifndef GUP_CODEGEN_H
#define GUP_CODEGEN_H 1

#include "gup/state.h"
#include "gup/ast.h"

/*
 * Resolve an AST node and generate machine code
 *
 * @state: Compiler state
 * @root:  AST root node
 *
 * Returns zero on success
 */
int cg_resolve_node(struct gup_state *state, struct ast_node *root);

#endif  /* !GUP_CODEGEN_H */
