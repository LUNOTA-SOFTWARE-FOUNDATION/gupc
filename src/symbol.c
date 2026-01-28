/*
 * Copyright (c) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "gup/symbol.h"

int
symbol_table_init(struct symbol_table *table)
{
    if (table == NULL) {
        errno = -EINVAL;
        return -1;
    }

    table->symbol_count = 0;
    TAILQ_INIT(&table->entries);
    return 0;
}

int
symbol_new(struct symbol_table *table, const char *name, symbol_type_t type,
    struct symbol **res)
{
    struct symbol *symbol;

    if (table == NULL || name == NULL) {
        errno = -EINVAL;
        return -1;
    }

    symbol = malloc(sizeof(*symbol));
    if (symbol == NULL) {
        errno = -ENOMEM;
        return -1;
    }

    memset(symbol, 0, sizeof(*symbol));
    if ((symbol->name = strdup(name)) == NULL) {
        free(symbol);
        errno = -ENOMEM;
        return -1;
    }

    /*
     * If this is a macro symbol, allocate a token buffer to
     * be associated with it.
     */
    if (type == SYMBOL_MACRO) {
        if (tokbuf_init(&symbol->mactok) < 0) {
            free(symbol);
            return -1;
        }
    }

    symbol->id = table->symbol_count++;
    symbol->type = type;
    TAILQ_INSERT_TAIL(&table->entries, symbol, link);

    if (res != NULL) {
        *res = symbol;
    }

    return 0;
}

void
symbol_table_destroy(struct symbol_table *table)
{
    struct symbol *symbol;

    if (table == NULL) {
        return;
    }

    symbol = TAILQ_FIRST(&table->entries);
    while (symbol != NULL) {
        TAILQ_REMOVE(&table->entries, symbol, link);
        if (symbol->type == SYMBOL_MACRO) {
            tokbuf_destroy(&symbol->mactok);
        }

        free(symbol->name);
        free(symbol);
        symbol = TAILQ_FIRST(&table->entries);
    }
}
