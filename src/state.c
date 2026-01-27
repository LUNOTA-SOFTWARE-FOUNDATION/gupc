/*
 * Copyright (c) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "gup/state.h"

int
gup_state_init(struct gup_state *res, const char *in_path, const char *out_path)
{
    if (res == NULL || in_path == NULL) {
        errno = -EINVAL;
        return -1;
    }

    if (out_path == NULL) {
        return -1;
    }

    memset(res, 0, sizeof(*res));
    res->in_fd = open(in_path, O_RDONLY);
    if (res->in_fd < 0) {
        return -1;
    }

    res->line_num = 1;
    return 0;
}

void
gup_state_destroy(struct gup_state *state)
{
    if (state == NULL) {
        return;
    }

    close(state->in_fd);
}
