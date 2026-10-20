#include <stdio.h>
#include <stdlib.h>
#include <elf.h>
#include <bfd.h>
#include <krakentrap/color.h>
#include <krakentrap/symbols.h> // define struct there

int load_symbol_table(const char *path, symbol_table_t *table) {

        bfd *abfd;
        asymbol **symbols = NULL;
        long storage;
        long count;
	size_t n = 0;

	// if no path or no table structure is provided
	if (!path || !table) {
		return -1;
	}

	table->symbols = NULL;
	table->count = 0;

        bfd_init();
        abfd = bfd_openr(path, NULL);

        if (abfd == NULL) {
                bfd_perror("bfd_openr");
                return 1;
        }

        if (!bfd_check_format(abfd, bfd_object)) {
                bfd_perror("bfd_check_format");
                bfd_close(abfd);
                return 1;
        }

        storage = bfd_get_symtab_upper_bound(abfd);
	// make sure storage is available
	if (storage <= 0) {
		printf(KT_ERR "No static symbol table available\n");
		bfd_close(abfd);
		return -1;
	}


        symbols = malloc((size_t)storage);
	// make sure malloc for the symbols works
	if (symbols == NULL) {
		perror("malloc");
		bfd_close(abfd);
		return -1;
	}

	count = bfd_canonicalize_symtab(abfd, symbols);
        if (count < 0) {
		bfd_perror("bfd_canonicalize_symtab");
		free(symbols);
		bfd_close(abfd);
		return -1;
	}

	table->symbols = calloc((size_t )count, sizeof(symbol_t));
	// if calloc doesnt work right, free and close
	if (count > 0 && table->symbols == NULL) {
		perror("calloc");
		free(symbols);
		bfd_close(abfd);
		return -1;
	}

	for (long i=0; i<count;i++) {
		asymbol *sym = symbols[i];
		symbol_t *dst = &table->symbols[n];
		const char *name = bfd_asymbol_name(sym);
		dst->name = strdup(name);
		dst->addr = (unsigned long)bfd_asymbol_value(sym);
		dst->size = 0; // no size from bfd
		flagword flags = sym->flags;
		dst->type = (flags & BSF_FUNCTION) ? 'F' :
			(flags & BSF_OBJECT) ? 'O' : '?';
		dst->bind = (flags & BSF_GLOBAL) ? 'G' :
			(flags & BSF_WEAK) ? 'W' : 'L';
		n++;

	}

	table->count = n;
	free(symbols);
	bfd_close(abfd);
	return 0;

}

void free_symbol_table(symbol_table_t *table) {

	for (size_t i=0;i<table->count;i++) {
		free(table->symbols[i].name);
	}
	free(table->symbols);
	table->symbols = NULL;
	table->count = 0;
}

/*
table.symbols[0]       // First symbol
table.symbols[0].name  // Name of the first symbol
table.symbols[0].addr  // Address of the first symbol
table.count            // Number of symbols
*/
symbol_t *find_symbol_by_name(symbol_table_t *table, const char *name) {

	// loop through symbol names and if its a match, return it
	for (size_t i=0;i<table->count;i++) {
		if (strcmp(table->symbols[i].name, name) == 0) {
			return &table->symbols[i];
		}
	}
	printf(KT_ERR "COULD NOT FIND: %s", name);
	return NULL;
}

symbol_t *find_symbol_by_addr(symbol_table_t *table, unsigned long addr) {

	for (size_t i=0;i<table->count;i++) {
		if (table->symbols[i].addr == addr) {
			return &table->symbols[i];
		}
	}
	return NULL;
}

void print_symbol_table(symbol_table_t *table) {

	for (size_t i=0;i<table->count;i++) {
		printf(KT_INFO"SYMBOL: "KT_YELLOW"%-50s"KT_RESET"ADDRESS: "KT_BLUE"0x%-10lx"KT_RESET"TYPE: %-1c\tBIND: %-1c\n", 
		(table->symbols[i].name ? table->symbols[i].name : ""),
		(table->symbols[i].addr),
		(table->symbols[i].type ? table->symbols[i].type : '?'),
		(table->symbols[i].bind ? table->symbols[i].bind : '?'));
	}

}
