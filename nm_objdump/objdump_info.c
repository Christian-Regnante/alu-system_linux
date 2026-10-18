#include "objdump.h"

/**
 * target_name - returns the BFD target name for an ELF file
 * @elf: pointer to the parsed ELF file information
 *
 * Return: a string describing the ELF target format
 */
const char *target_name(const elf_info_t *elf)
{
	uint16_t machine = get16(elf, elf->data + 0x12);

	if (machine == EM_X86_64)
		return ("elf64-x86-64");
	if (machine == EM_386)
		return ("elf32-i386");
	if (elf->is_64)
		return (elf->big_endian ? "elf64-big" : "elf64-little");
	return (elf->big_endian ? "elf32-big" : "elf32-little");
}

/**
 * architecture_name - returns the architecture name for an ELF file
 * @elf: pointer to the parsed ELF file information
 *
 * Return: a string describing the ELF architecture
 */
static const char *architecture_name(const elf_info_t *elf)
{
	uint16_t machine = get16(elf, elf->data + 0x12);

	if (machine == EM_X86_64)
		return ("i386:x86-64");
	if (machine == EM_386)
		return ("i386");
	return ("UNKNOWN!");
}

void print_flags(const elf_info_t *elf, const section_info_t *sections)
{
	uint16_t type = get16(elf, elf->data + 0x10);
	uint16_t phnum = elf->is_64 ? get16(elf, elf->data + 0x38) :
		get16(elf, elf->data + 0x2c);
	uint32_t flags = 0;
	uint16_t i;
	int first = 1;

	if (type == 1)
		flags |= 0x1;
	if (type == 2)
		flags |= 0x2;
	if (type == 3)
		flags |= 0x40;
	if (phnum > 0)
		flags |= 0x100;
	for (i = 0; i < elf->shnum; i++)
	{
		if ((sections[i].type == SHT_SYMTAB ||
				sections[i].type == SHT_DYNSYM) &&
				sections[i].size > 0)
			flags |= 0x10;
	}
	printf("architecture: %s, flags 0x%08x:\n",
			architecture_name(elf), flags);
	if (flags & 0x1)
		printf("%sHAS_RELOC", first ? "" : ", "), first = 0;
	if (flags & 0x2)
		printf("%sEXEC_P", first ? "" : ", "), first = 0;
	if (flags & 0x10)
		printf("%sHAS_SYMS", first ? "" : ", "), first = 0;
	if (flags & 0x40)
		printf("%sDYNAMIC", first ? "" : ", "), first = 0;
	if (flags & 0x100)
		printf("%sD_PAGED", first ? "" : ", "), first = 0;
	printf("\n");
}
