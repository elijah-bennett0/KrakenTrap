#include <stdio.h>
#include <stdlib.h>
#include <elf.h>

#include <krakentrap/color.h>
#include <krakentrap/symbols.h> // define struct there

int load_symbol_table(const char *path, symbol_table_t *table) {

	FILE *fp = fopen(path, "rb");
	if (!fp) {
		perror("Couldnt read file");
		return 1;
	}

	Elf64_Ehdr header;

	if (fread(&header, sizeof(header), 1, fp) != 1) {
		perror("Couldnt read header");
		return 1;
	}

	// first 4 bytes of an elf should be \x7fELF
	if (header.e_ident[EI_MAG0] != ELFMAG0 ||
		header.e_ident[EI_MAG1] != ELFMAG1 ||
		header.e_ident[EI_MAG2] != ELFMAG2 ||
		header.e_ident[EI_MAG3] != ELFMAG3) {
		printf(KT_ERR "Not a valid elf file\n");
		fclose(fp);
		return 1;
	}

	if (header.e_ident[EI_CLASS] != ELFCLASS64) {
		printf("Only 64-bit ELF supported for now\n");
		return 1;
	}

	

}

void free_symbol_table(symbol_table_t *table) {

}

symbol_t *find_symbol_by_name(symbol_table_t *table, const char *name) {

}

symbol_t *find_symbol_by_addr(symbol_table_t *table, unsigned long addr) {

}

void print_symbol_table(symbol_table_t *table) {

}
