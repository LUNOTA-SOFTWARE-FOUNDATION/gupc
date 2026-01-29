/*
 * Copyright (c) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#ifndef GUP_MU_H
#define GUP_MU_H 1

#include <stdbool.h>
#include "gup/state.h"

/*
 * Emit a label into assembly
 *
 * @state:  Compiler state
 * @label:  Label to emit
 * @global: If true, label is global
 *
 * Returns zero on success
 */
int mu_emit_label(struct gup_state *state, const char *label, bool global);

/*
 * Emit a return
 *
 * @state: Compiler state
 *
 * Returns zero on success
 */
int mu_emit_ret(struct gup_state *state);

#endif  /* !GUP_MU_H */
