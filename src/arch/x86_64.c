/*
 * Copyright (c) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#include <stdio.h>
#include <errno.h>
#include "gup/mu.h"

int
mu_emit_label(struct gup_state *state, const char *label, bool global)
{
    if (state == NULL || label == NULL) {
        errno = -EINVAL;
        return -1;
    }

    if (global) {
        fprintf(
            state->out_fp,
            "[global %s]\n",
            label
        );
    }

    fprintf(
        state->out_fp,
        "%s:\n",
        label
    );

    return 0;
}

int
mu_emit_ret(struct gup_state *state)
{
    if (state == NULL) {
        errno = -EINVAL;
        return -1;
    }

    fprintf(
        state->out_fp,
        "\tret\n"
    );

    return 0;
}
