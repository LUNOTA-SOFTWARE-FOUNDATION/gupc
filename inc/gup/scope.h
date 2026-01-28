/*
 * Copyright (C) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#ifndef GUP_SCOPE_H
#define GUP_SCOPE_H 1

#include "gup/state.h"

/*
 * Push a new scope to the scope stack
 *
 * @state: Compiler state
 * @tok:   Scope token to push
 *
 * Returns zero on success
 */
int scope_push(struct gup_state *state, tt_t tok);

/*
 * Returns top of the scope stack
 *
 * @state: Compiler state
 */
tt_t scope_top(struct gup_state *state);

/*
 * Pop a new scope from the stack
 *
 * @state: Compiler state
 *
 * Returns the last scope popped from the stack
 */
tt_t scope_pop(struct gup_state *state);

#endif  /* !GUP_SCOPE_H */
