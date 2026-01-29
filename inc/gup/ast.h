/*
 * Copyright (c) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#ifndef GUP_AST_H
#define GUP_AST_H 1

#include "gup/symbol.h"
#include "gup/state.h"

/*
 * Represents valid AST node types
 *
 * @AST_NONE:  This node has no type
 * @AST_PROC:  This node is a procedure
 */
typedef enum {
    AST_NONE,
    AST_PROC
} ast_type_t;

/*
 * Represents a valid abstract syntax tree node
 *
 * @type:       AST node type
 * @epilogue:   End of block if set
 * @left:       Left node
 * @right:      Right node
 */
struct ast_node {
    ast_type_t type;
    uint8_t epilogue : 1;
    struct ast_node *left;
    struct ast_node *right;
    union {
        symid_t symid;
    };
};

/*
 * Allocate a new abstract syntax tree node
 *
 * @state: Compiler state
 * @type:  AST node type
 * @res:   Result node is written here
 *
 * Returns zero on success
 */
int ast_node_allocate(struct gup_state *state, ast_type_t type, struct ast_node **res);

#endif  /* !GUP_AST_H */
