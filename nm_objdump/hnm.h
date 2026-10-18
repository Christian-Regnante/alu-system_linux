#ifndef HNM_H
#define HNM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>

#define ELFCLASS32 1
#define ELFCLASS64 2
#define ELFDATA2LSB 1
#define ELFDATA2MSB 2

#define SHT_INIT_ARRAY 14
#define SHT_FINI_ARRAY 15
#define SHT_PREINIT_ARRAY 16
#define SHT_STRTAB 3

#define SHT_SYMTAB 2
#define SHT_DYNSYM 11
#define SHT_NOBITS 8
#define SHT_VERSYM 0x6fffffff
#define SHT_VERNEED 0x6ffffffe

#define SHN_UNDEF 0
#define SHN_ABS 0xfff1
#define SHN_COMMON 0xfff2

#define STB_LOCAL 0
#define STB_WEAK 2
#define STB_GNU_UNIQUE 10

#define STT_OBJECT 1
#define STT_SECTION 3
#define STT_FILE 4

#define SHF_WRITE 0x1
#define SHF_ALLOC 0x2
#define SHF_EXECINSTR 0x4

typedef struct elf_info
{
	unsigned char *data;
	size_t size;
	int is_64;
	int big_endian;
	unsigned char osabi;
	uint64_t shoff;
	uint16_t shentsize;
	uint16_t shnum;
	uint16_t shstrndx;
} elf_info_t;

typedef struct section_info
{
	uint32_t type;
	uint64_t flags;
	uint64_t addr;
	uint64_t offset;
	uint64_t size;
	uint32_t link;
	uint64_t entsize;
} section_info_t;

typedef struct symbol_info
{
	uint64_t value;
	uint16_t shndx;
	unsigned char bind;
	unsigned char type;
	const char *name;
} symbol_info_t;

uint16_t get16(const elf_info_t *elf, const unsigned char *p);
uint32_t get32(const elf_info_t *elf, const unsigned char *p);
uint64_t get64(const elf_info_t *elf, const unsigned char *p);

int read_elf(const char *path, elf_info_t *elf);
int get_sections(elf_info_t *elf, section_info_t **sections);
char *symbol_version(const elf_info_t *elf, const section_info_t *sections,
		const char *name);
int print_symbols(elf_info_t *elf, section_info_t *sections,
		const char *path);

#endif
