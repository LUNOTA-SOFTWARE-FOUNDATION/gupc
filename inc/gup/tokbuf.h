/*
 * Copyright (c) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#ifndef GUP_TOKBUF_H
#define GUP_TOKBUF_H 1

#include <sys/types.h>
#include <stdint.h>
#include <stddef.h>
#include "gup/token.h"

/*
 * Represents a token buffer used to store tokens
 * during preprocessing
 *
 * @head:       Head index used by preprocessor (producer)
 * @tail:       Tail index used by parser (consumer)
 * @entry_cap:  Capacity of ring, used to know when to expand
 * @ring:       Actual ring used to store tokens
 */
struct tokbuf {
    size_t head;
    size_t tail;
    size_t entry_cap;
    struct token *ring;
};

/*
 * Initialize the token buffer
 *
 * @res: Result is written here
 *
 * Returns zero on success
 */
int tokbuf_init(struct tokbuf *res);

/*
 * Push a token to the token buffer
 *
 * @buf: Token buffer to append to
 * @tok: Token to append
 *
 * XXX: The token provided is simply duplicated
 *
 * Returns zero on success
 */
int tokbuf_push(struct tokbuf *buf, struct token *tok);

/*
 * Pop a token from the start of the token
 * buffer
 *
 * @buf: Buffer to pop from
 *
 * Returns NULL if there are no more tokens
 */
struct token *tokbuf_pop(struct tokbuf *buf);

/*
 * Lookbehind the current token buffer position with n steps
 *
 * @buf: Buffer to lookbehind
 * @n:   Number of steps to lookbehind
 *
 * Returns token that is n steps away from the current position,
 * otherwise NULL on failure.
 */
struct token *tokbuf_lookbehind(struct tokbuf *buf, off_t n);

/*
 * Destroy a token buffer
 *
 * @buf: Token buffer to destroy
 */
void tokbuf_destroy(struct tokbuf *buf);

#endif  /* !GUP_TOKBUF_H */
