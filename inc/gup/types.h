/*
 * Copyright (c) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#ifndef GUP_TYPES_H
#define GUP_TYPES_H 1

#include "gup/token.h"

/*
 * Valid program data types
 *
 * @GUP_TYPE_BAD:   Invalid type
 * @GUP_TYPE_VOID:  Type absent
 */
typedef enum {
    GUP_TYPE_BAD,
    GUP_TYPE_VOID
} gup_type_t;

/*
 * Represents a valid data type
 *
 * @type: Data type class
 */
struct data_type {
    gup_type_t type;
};

/*
 * Convert a token type into a program type
 *
 * @tt: Token type to convert
 *
 * Returns GUP_TYPE_BAD on failure
 */
static inline gup_type_t
tok_to_type(tt_t tt)
{
    switch (tt) {
    case TT_VOID:   return GUP_TYPE_VOID;
    default:        return GUP_TYPE_BAD;
    }

    return GUP_TYPE_BAD;
}

#endif  /* !GUP_TYPES_H */
