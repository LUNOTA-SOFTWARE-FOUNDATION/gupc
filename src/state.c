/*
 * Copyright (c) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "gup/state.h"
#include "gup/symbol.h"

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
    if (tokbuf_init(&res->tokbuf) < 0) {
        perror("tokbuf_init");
        return -1;
    }

    if (symbol_table_init(&res->symtab) < 0) {
        tokbuf_destroy(&res->tokbuf);
        return -1;
    }

    if (ptrbox_init(&res->ptrbox) < 0) {
        tokbuf_destroy(&res->tokbuf);
        symbol_table_destroy(&res->symtab);
        return -1;
    }

    res->in_fd = open(in_path, O_RDONLY);
    if (res->in_fd < 0) {
        tokbuf_destroy(&res->tokbuf);
        ptrbox_destroy(&res->ptrbox);
        symbol_table_destroy(&res->symtab);
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
    tokbuf_destroy(&state->tokbuf);
    ptrbox_destroy(&state->ptrbox);
    symbol_table_destroy(&state->symtab);
}
