#ifndef KRAKENTRAP_SYMBOLS_H
#define KRAKENTRAP_SYMBOLS_H

#include <stddef.h>

typedef struct {
        char *name;
        unsigned long addr;
        unsigned long size;
        unsigned char type;
        unsigned char bind;
} symbol_t;

typedef struct {
        symbol_t *symbols;
        size_t count;
} symbol_table_t;

int load_symbol_table(const char *path, symbol_table_t *table);
void free_symbol_table(symbol_table_t *table);

symbol_t *find_symbol_by_name(symbol_table_t *table, const char *name);
symbol_t *find_symbol_by_addr(symbol_table_t *table, unsigned long addr);

void print_symbol_table(symbol_table_t *table);

#endif
