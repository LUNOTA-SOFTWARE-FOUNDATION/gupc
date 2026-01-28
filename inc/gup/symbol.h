/*
 * Copyright (c) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#ifndef GUP_SYMBOL_H
#define GUP_SYMBOL_H 1

#include <sys/queue.h>
#include <sys/types.h>
#include <stdint.h>
#include <stddef.h>
#include "gup/tokbuf.h"

/* Symbol ID */
typedef size_t symid_t;

/*
 * Represents valid program symbol times
 *
 * @SYMBOL_NONE:  Symbol has a no type
 * @SYMBOL_MACRO: Symbol is a macro
 */
typedef enum {
    SYMBOL_NONE,
    SYMBOL_MACRO
} symbol_type_t;

/*
 * Represents a program symbol
 *
 * @name:       Symbol name
 * @type:       Symbol type
 * @id:         Symbol ID
 * @mactok:     Macro tokens
 * @link:       Queue link
 */
struct symbol {
    char *name;
    symbol_type_t type;
    symid_t id;
    struct tokbuf mactok;
    TAILQ_ENTRY(symbol) link;
};

/*
 * A symbol table holds a list of program symbols
 *
 * @entries:        Symbol queue
 * @symbol_count:   Number of symbols total
 */
struct symbol_table {
    TAILQ_HEAD(, symbol) entries;
    size_t symbol_count;
};

/*
 * Initialize a program symbol table
 *
 * @table:  Table to initialize
 *
 * Returns zero on success
 */
int symbol_table_init(struct symbol_table *table);

/*
 * Lookup a symbol using its name
 *
 * @table: Symbol table to look up from
 * @name:  Name of symbol to lookup
 *
 * Returns the symbol on success, otherwise a value of NULL
 * on failure or entry not found.
 */
struct symbol *symbol_from_name(struct symbol_table *table, const char *name);

/*
 * Lookup a symbol using its ID
 *
 * @table: Symbol table to look up from
 * @id:    Symbol ID to lookup
 *
 * Returns the symbol on success, otherwise a value of NULL
 * on failure or entry not found.
 */
struct symbol *symbol_from_id(struct symbol_table *table, symid_t id);

/*
 * Allocate a new symbol
 *
 * @table: Symbol table to allocate to
 * @name:  Name of symbol to add
 * @type:  Symbol type to add
 *
 * Returns zero on success
 */
int symbol_new(
    struct symbol_table *table, const char *name,
    symbol_type_t type, struct symbol **res
);

/*
 * Destroy a symbol table
 *
 * @table: Symbol table to destroy
 */
void symbol_table_destroy(struct symbol_table *table);

#endif  /* !GUP_SYMBOL_H */
