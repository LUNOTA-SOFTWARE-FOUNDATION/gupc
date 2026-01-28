/*
 * Copyright (c) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "gup/tokbuf.h"

#define TOKBUF_GROW_SIZE 4

int
tokbuf_init(struct tokbuf *res)
{
    if (res == NULL) {
        errno = -EINVAL;
        return -1;
    }

    memset(res, 0, sizeof(*res));
    res->entry_cap = TOKBUF_GROW_SIZE;
    res->ring = malloc(res->entry_cap * sizeof(struct token));
    if (res->ring == NULL) {
        errno = -ENOMEM;
        return -1;
    }

    return 0;
}

int
tokbuf_push(struct tokbuf *buf, struct token *tok)
{
    if (buf == NULL || tok == NULL) {
        errno = -EINVAL;
        return -1;
    }

    /* Push and grow as needed */
    buf->ring[buf->head++] = *tok;
    if (buf->head >= buf->entry_cap) {
        buf->entry_cap += TOKBUF_GROW_SIZE;
        buf->ring = realloc(
            buf->ring,
            buf->entry_cap * sizeof(struct token)
        );
    }

    if (buf->ring == NULL) {
        errno = -EINVAL;
        return -1;
    }

    return 0;
}

struct token *
tokbuf_pop(struct tokbuf *buf)
{
    if (buf == NULL) {
        return NULL;
    }

    if (buf->tail == buf->head) {
        return NULL;
    }

    return &buf->ring[buf->tail++];
}

void
tokbuf_destroy(struct tokbuf *buf)
{
    if (buf == NULL) {
        return;
    }

    free(buf->ring);
    buf->ring = NULL;
}
