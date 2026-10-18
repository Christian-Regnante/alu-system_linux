#ifndef OBJDUMP_H
#define OBJDUMP_H

#include "hnm.h"

#define EM_386 3
#define EM_X86_64 62

int objdump_read(const char *path, elf_info_t *elf);
int objdump_sections(elf_info_t *elf, section_info_t **sections);
const char *target_name(const elf_info_t *elf);
void print_flags(const elf_info_t *elf, const section_info_t *sections);
int objdump_dump(const char *path, elf_info_t *elf,
		section_info_t *sections);

#endif
