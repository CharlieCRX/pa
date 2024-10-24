/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>

char* get_section_name(FILE *file, Elf32_Shdr *shdrs, int shnum, Elf32_Word name_idx) {
    // 假设字符串表在最后一个节中
    Elf32_Shdr *strtab_hdr = &shdrs[shnum - 1]; 
    char *str_table = malloc(strtab_hdr->sh_size);
    
    fseek(file, strtab_hdr->sh_offset, SEEK_SET);
    fread(str_table, strtab_hdr->sh_size, 1, file);

    return str_table + name_idx; // 返回对应的字符串
}

void print_symbol_info(Elf32_Sym *symbol, char *str_table) {
    const char *symbol_name = str_table + symbol->st_name;

    // 获取符号的类型和绑定
    unsigned int type = ELF32_ST_TYPE(symbol->st_info);
    unsigned int bind = ELF32_ST_BIND(symbol->st_info);

    // 输出符号信息
    printf("Symbol Name: %s\n", symbol_name);
    printf("  Value: 0x%x\n", symbol->st_value);
    printf("  Size: %u\n", symbol->st_size);
    printf("  Type: ");
    switch (type) {
        case STT_NOTYPE: printf("NOTYPE\n"); break;
        case STT_OBJECT: printf("OBJECT\n"); break;
        case STT_FUNC: printf("FUNCTION\n"); break;
        case STT_TLS: printf("TLS\n"); break;
        default: printf("Unknown\n"); break;
    }

    printf("  Bind: ");
    switch (bind) {
        case STB_LOCAL: printf("LOCAL\n"); break;
        case STB_GLOBAL: printf("GLOBAL\n"); break;
        case STB_WEAK: printf("WEAK\n"); break;
        default: printf("Unknown\n"); break;
    }
}

void print_symbols(FILE *file, Elf32_Shdr *shdrs, int shnum, Elf32_Off symtab_offset, Elf32_Off strtab_offset) {
    Elf32_Sym *symbols = malloc(sizeof(Elf32_Sym) * 100); // 假设最多有100个符号
    fseek(file, symtab_offset, SEEK_SET);
    fread(symbols, sizeof(Elf32_Sym), 100, file);

    char *str_table = malloc(256); // 假设字符串表大小为256字节
    fseek(file, strtab_offset, SEEK_SET);
    fread(str_table, 256, 1, file);

    for (int i = 0; i < 100; i++) {
        print_symbol_info(&symbols[i], str_table);
    }

    free(symbols);
    free(str_table);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <elf-file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *file = fopen(argv[1], "rb");
    if (file == NULL) {
        perror("Cannot open file");
        return EXIT_FAILURE;
    }

    Elf32_Ehdr ehdr;
    fread(&ehdr, sizeof(ehdr), 1, file);

    Elf32_Shdr *shdrs = malloc(sizeof(Elf32_Shdr) * ehdr.e_shnum);
    fseek(file, ehdr.e_shoff, SEEK_SET);
    fread(shdrs, sizeof(Elf32_Shdr), ehdr.e_shnum, file);

    Elf32_Off symtab_offset = 0;
    Elf32_Off strtab_offset = 0;

    // 查找符号表和字符串表
    for (int i = 0; i < ehdr.e_shnum; i++) {
        char *section_name = get_section_name(file, shdrs, ehdr.e_shnum, shdrs[i].sh_name);
        if (shdrs[i].sh_type == SHT_SYMTAB) {
            symtab_offset = shdrs[i].sh_offset;
        }
        if (shdrs[i].sh_type == SHT_STRTAB && strcmp(section_name, ".strtab") == 0) {
            strtab_offset = shdrs[i].sh_offset;
        }
    }

    print_symbols(file, shdrs, ehdr.e_shnum, symtab_offset, strtab_offset);

    free(shdrs);
    fclose(file);
    return EXIT_SUCCESS;
}
*/
