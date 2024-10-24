#ifndef TEST
#include <common.h>
#endif

#ifdef TEST
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#define paddr_t uint32_t
#define word_t uint32_t
#endif

#include <fcntl.h>
#include <elf.h>
#include <unistd.h>

typedef struct SymbolEntry {
	char name[32];	// Locate at strtab
	unsigned char info;
	paddr_t address;
	word_t size;
} SymbolEntry;

static SymbolEntry* sym_entrys = NULL;
static uint32_t sym_num = 0;
static uint32_t call_depth = 0;
static uint32_t trace_func_call_flag = 0;

void init_symtab_entrys(FILE *file);
char *get_strtab(Elf32_Shdr *strtab, FILE *file);
void parse_elf(const char *elf_file);
void print_sym_entrys();
char *get_function_name_by_addres(paddr_t addr);
void trace_func_call(paddr_t pc, paddr_t target);
void trace_func_ret(paddr_t pc);

void parse_elf(const char *elf_file) {
	if (elf_file == NULL) {
		return;
	}
	
	Log("The elf file is %s\n", elf_file);
	trace_func_call_flag = 1;
	FILE *file = fopen(elf_file, "rb");
	assert(file != NULL);

	init_symtab_entrys(file);
	print_sym_entrys();
}

char *get_function_name_by_addres(paddr_t addr) {
	for (int i = 0; i < sym_num; i++) {
		if (ELF32_ST_TYPE(sym_entrys[i].info) == STT_FUNC) {
			if (addr >= sym_entrys[i].address && addr < (sym_entrys[i].size + sym_entrys[i].address)) {
				return sym_entrys[i].name;
			}
		}
	}
	return NULL;
}

void init_symtab_entrys(FILE *elf_file) {
	if (elf_file == NULL) assert(0);
	// Get ELF header
	Elf32_Ehdr ehdr;
	int result = fread(&ehdr, sizeof(Elf32_Ehdr), 1, elf_file);
	assert(&ehdr != NULL && result == 1);

	// Get Section header by ELF header
	Elf32_Shdr *shdrs = malloc(sizeof(Elf32_Shdr) * ehdr.e_shnum);
	result = fseek(elf_file, ehdr.e_shoff, SEEK_SET);
	assert(result == 0);
	result = fread(shdrs, sizeof(Elf32_Shdr), ehdr.e_shnum, elf_file);
	assert(result != 0);

	// Get Symtab from Section headr entrys
	Elf32_Shdr *symtab = NULL;
	for (int i = 0; i < ehdr.e_shnum; i++) {
		if (shdrs[i].sh_type == SHT_SYMTAB) {
			symtab = shdrs + i;
 	  }
  }
	assert(symtab != NULL);

	// Get entry num and offset
	uint32_t entry_num = symtab->sh_size / symtab->sh_entsize;
	sym_num = entry_num;	// Set global entry num
	uint32_t offset = symtab->sh_offset;


	// Get symtab entrys
	Elf32_Sym *symbol_tables = malloc(sizeof(Elf32_Sym) * entry_num);
	result = fseek(elf_file, offset, SEEK_SET);
	assert(result == 0);
	result = fread(symbol_tables, sizeof(Elf32_Sym), entry_num, elf_file);
	assert(result != 0);

	// Initialize sym_entrys
	sym_entrys = malloc(sizeof(SymbolEntry) * entry_num);
	char *str = get_strtab(&shdrs[ehdr.e_shnum - 2], elf_file);
	assert(str != NULL);
	for (int i = 0; i < entry_num; i++) {
		strcpy(sym_entrys[i].name, str + symbol_tables[i].st_name);
		sym_entrys[i].info = symbol_tables[i].st_info;
		sym_entrys[i].address = (paddr_t) symbol_tables[i].st_value;
		sym_entrys[i].size = (word_t) symbol_tables[i].st_size;
	}

	// Free ELF headers, Symbol Entrys structure arrays and str
	free(shdrs);
	free(symbol_tables);
	free(str);
}


void print_sym_entrys() {
	assert(sym_entrys != NULL);
	for (int i = 0; i < sym_num; i++) {
		printf("Num:%2d, Name: %20s, Info: %d, Addr:%08x, size: %04x\n",
		i, sym_entrys[i].name, sym_entrys[i].info, sym_entrys[i].address, sym_entrys[i].size);
	}
}



void trace_func_call(paddr_t pc, paddr_t target) {
	if (trace_func_call_flag == 0) return; //No elf file
	++call_depth;

	if (call_depth <= 2) return; // ignore _trm_init & main
	
	char *name  = get_function_name_by_addres(target);
	// Example output: 0x800001f8:     call [f0@0x80000010]

	ftrace_write(FMT_PADDR ": %*scall [%s@" FMT_PADDR "]\n", 
		pc,
		(call_depth-3)*2, "", 
		name?name:"???",
		target
	);
}

void trace_func_ret(paddr_t pc) {
	if (trace_func_call_flag == 0) return; //No elf file

	if (call_depth <= 2) return; // ignore _trm_init & main

	char *name = get_function_name_by_addres(pc);
	ftrace_write(FMT_PADDR ": %*sret [%s]\n",
		pc,
		(call_depth-3)*2, "",
		name?name:"???"
	);

	--call_depth;
}


char *get_strtab(Elf32_Shdr *strtab, FILE *file) {
	char *str = malloc(strtab->sh_size);

	int result = fseek(file, strtab->sh_offset, SEEK_SET);
	assert(result == 0);
  result = fread(str, 1, strtab->sh_size, file);
	assert(result != 0);

	return str;
}

#ifdef TEST
int main() {
	char *elf_file = "/home/crx/study/ics2023/nemu/src/monitor/ftrace/string.elf";
	parse_elf(elf_file);
	return 0;
}
#endif
