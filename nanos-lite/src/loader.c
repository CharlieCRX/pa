#include <proc.h>
#include <elf.h>
#include "ramdisk.h"

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

Elf_Phdr *get_pt_load_segments(const char *filename, size_t *count);
void load_segments(Elf_Phdr *pt_load_segments, int num_segments);


static uintptr_t loader(PCB *pcb, const char *filename) {
  size_t count;
  // 获取要加载的段信息
  Elf_Phdr *pt_load_segments = get_pt_load_segments(filename, &count);

  // 加载段到内存中
	load_segments(pt_load_segments, count);
  return 0;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}


// 获取 PT_LOAD 段信息，并将 PT_LOAD 段的数量存储到 count 中
Elf_Phdr *get_pt_load_segments(const char *filename, size_t *num_segments) {

    Elf_Ehdr ehdr;
    
    // Step 1: 读取 ELF 头部
    ramdisk_read(&ehdr, 0, sizeof(Elf_Ehdr));
    
    // 检查 ELF 魔数
    if (memcmp(ehdr.e_ident, ELFMAG, SELFMAG) != 0) {
        printf("Not a valid ELF file.\n");
        return NULL;
    }
    
    // Step 2: 读取所有 Program Headers
    Elf_Phdr *phdrs = malloc(ehdr.e_phnum * sizeof(Elf_Phdr));
    if (phdrs == NULL) {
        printf("Memory allocation for Program Headers failed.\n");
        return NULL;
    }

    ramdisk_read(phdrs, ehdr.e_phoff, ehdr.e_phnum * sizeof(Elf_Phdr));
    
    // Step 3: 筛选出所有 PT_LOAD 段
    Elf_Phdr *pt_load_segments = malloc(ehdr.e_phnum * sizeof(Elf_Phdr));
    if (pt_load_segments == NULL) {
        free(phdrs);
        printf("Memory allocation for PT_LOAD segments failed.\n");
        return NULL;
    }

    int count = 0;
    for (int i = 0; i < ehdr.e_phnum; i++) {
        if (phdrs[i].p_type == PT_LOAD) {
            pt_load_segments[count++] = phdrs[i];
        }
    }
    *num_segments = count;

    free(phdrs);
    return pt_load_segments;
}

// 示例：打印 PT_LOAD 段信息
void print_pt_load_segments(const Elf_Phdr *segments, size_t count) {
    for (size_t i = 0; i < count; i++) {
        const Elf_Phdr *phdr = &segments[i];
        printf("PT_LOAD Segment %zu: Offset 0x%x, Virtual Address 0x%x, Size in File %u, Size in Memory %u\n",
               i, phdr->p_offset, phdr->p_vaddr, phdr->p_filesz, phdr->p_memsz);
    }
}

// 段加载函数，输入参数为pt_load_segments和其数量
void load_segments(Elf_Phdr *pt_load_segments, int num_segments){
  for (int i = 0; i < num_segments; i++) {
    Elf_Phdr *seg = &pt_load_segments[i];

    // 获取段的偏移、虚拟地址、文件大小、内存大小
    size_t offset = seg->p_offset;
    uint32_t vaddr = seg->p_vaddr;
    size_t filesz = seg->p_filesz;
    size_t memsz = seg->p_memsz;

    // 从ramdisk中读取段数据到内存
    ramdisk_read((void *)(vaddr), offset, filesz);

    // 清零 [VirtAddr + FileSiz, VirtAddr + MemSiz) 的内存
    if (memsz > filesz) {
      memset((void *)(vaddr + filesz) , 0, memsz - filesz);
    }
  }
}
