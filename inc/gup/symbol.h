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
 * @id:         Symbol ID
 * @v_const:    Constant value associated with symbol
 * @link:       Queue link
 */
struct symbol {
    char *name;
    symid_t id;
    size_t v_const;
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
