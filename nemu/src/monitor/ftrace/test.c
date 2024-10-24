#ifdef TEST
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <elf.h>
#include <assert.h>

char *file_path = "/home/crx/study/ics2023/nemu/src/monitor/ftrace/string.elf";

char *get_string_from_shstrtab(Elf32_Shdr *shstrtab, FILE *file);

void test_read_section_header() {
	
	FILE *file = fopen(file_path, "rb");
	assert(file != NULL && "Failed to open file");

	// Get ELF header in ehdr
	Elf32_Ehdr ehdr;
	fread(&ehdr, sizeof(Elf32_Ehdr), 1, file);
	assert(&ehdr != NULL);

	// Get Section header
	Elf32_Shdr *shdrs = malloc(sizeof(Elf32_Shdr) * ehdr.e_shnum);
	assert(shdrs != NULL);
	fseek(file, ehdr.e_shoff, SEEK_SET);
	fread(shdrs, sizeof(Elf32_Shdr), ehdr.e_shnum, file);

	Elf32_Off symtab_off = 0;
	Elf32_Off strtab_off = 0;

	// Get shstrtab string
	char *name_start = get_string_from_shstrtab(&shdrs[ehdr.e_shnum - 1], file);
	assert(name_start != NULL);

	for (int i = 0; i < ehdr.e_shnum; i++) {
		printf("Num: %2d, ", i);
	//printf("name_offset:%4x, ", shdrs[i].sh_name);
		printf("Name: %-20s ,", name_start + shdrs[i].sh_name); //TODO
		if(shdrs[i].sh_type == SHT_SYMTAB) printf("type:       SYMTAB, ");
		else
		printf("type: %12d, "     , shdrs[i].sh_type);
		printf("Addr: %08x, "      , shdrs[i].sh_addr);
		printf("Off : %06x, "      , shdrs[i].sh_offset);
		printf("Size: %06x, "      , shdrs[i].sh_size);
		printf("ES  : %02x, "      , shdrs[i].sh_entsize);
		printf("\n");
	}

	free(shdrs);
	fclose(file);
}

void test_show_symtab() {
	FILE *file = fopen(file_path, "rb");
	assert(file != NULL);

	// Get ELF header
	Elf32_Ehdr ehdr;
	fread(&ehdr, sizeof(Elf32_Ehdr), 1, file);
	assert(&ehdr != NULL);

	// Get Section header
	Elf32_Shdr *shdrs = malloc(sizeof(Elf32_Shdr) * ehdr.e_shnum);
	fseek(file, ehdr.e_shoff, SEEK_SET);
	fread(shdrs, sizeof(Elf32_Shdr), ehdr.e_shnum, file);

	// Find symtab entry in file
	Elf32_Shdr *symtab;
	for (int i = 0; i < ehdr.e_shnum; i++) {
		if (shdrs[i].sh_type == SHT_SYMTAB) {
			symtab = shdrs + i;
		}
	}
	assert(symtab != NULL);


	// Get entry num and offset
	uint32_t entry_num = symtab->sh_size / symtab->sh_entsize;
	assert(entry_num == 49);
	uint32_t offset = symtab->sh_offset;
	assert(offset == 0x1338);

	// Get symtab entrys
	Elf32_Sym *symbol_tables = malloc(sizeof(Elf32_Sym) * entry_num);
	fseek(file, offset, SEEK_SET);
	fread(symbol_tables, sizeof(Elf32_Sym), entry_num, file);

	// Print entrys in symbol table
	char *str = get_string_from_shstrtab(&shdrs[ehdr.e_shnum - 2], file);
	assert(str != NULL);

	printf("   Num:    Value  Size Type    Name\n");
	for (int i = 0; i < entry_num; i++) {
		printf("%6d: ", i);
		printf("%08x  ", symbol_tables[i].st_value);
		printf("%4d "  , symbol_tables[i].st_size);
		if (ELF32_ST_TYPE(symbol_tables[i].st_info) == STT_FUNC) {
			printf("FUNC    ");
		}else
		printf("        ");

		printf(" %s", str + symbol_tables[i].st_name);
		printf(" %04x\n", symbol_tables[i].st_name);
	}
	free(str);
	free(shdrs);
	free(symbol_tables);
}



char *get_string_from_shstrtab(Elf32_Shdr *shstrtab, FILE *file) {
	char *str = malloc(shstrtab->sh_size);

	fseek(file, shstrtab->sh_offset, SEEK_SET);
	fread(str, 1, shstrtab->sh_size, file);

	return str;
}

int main() {
	//test_read_section_header();
	test_show_symtab();
	return 0;
}
#endif
