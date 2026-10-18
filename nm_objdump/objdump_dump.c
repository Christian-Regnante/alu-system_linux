#include "objdump.h"

static int skip_section(const section_info_t *section, const char *name)
{
	if (section->type == 0 || section->type == SHT_SYMTAB ||
			section->type == SHT_NOBITS || section->size == 0)
		return (1);
	if ((section->type == SHT_REL || section->type == SHT_RELA) &&
			!(section->flags & SHF_ALLOC))
		return (1);
	if (section->type == SHT_STRTAB &&
			(strcmp(name, ".strtab") == 0 ||
			 strcmp(name, ".shstrtab") == 0))
		return (1);
	return (0);
}

static int address_width(const section_info_t *section)
{
	uint64_t end = section->addr + section->size;
	int width = 0;

	while (end != 0)
	{
		width++;
		end >>= 4;
	}
	if (width < 4)
		width = 4;
	return (width);
}

static void print_section(const elf_info_t *elf,
		const section_info_t *section, const char *name)
{
	uint64_t i;
	unsigned char byte;
	unsigned int group, offset;
	int width = address_width(section);

	printf("Contents of section %s:\n", name);
	for (i = 0; i < section->size; i += 16)
	{
		printf(" %0*lx ", width,
				(unsigned long)(section->addr + i));
		for (group = 0; group < 4; group++)
		{
			for (offset = 0; offset < 4; offset++)
			{
				if (i + (group * 4) + offset < section->size)
					printf("%02x", elf->data[section->offset +
							i + (group * 4) + offset]);
				else
					printf("  ");
			}
			printf(group == 3 ? "  " : " ");
		}
		for (offset = 0; offset < 16; offset++)
		{
			if (i + offset < section->size)
			{
				byte = elf->data[section->offset + i + offset];
				printf("%c", byte >= 32 && byte <= 126 ? byte : '.');
			}
			else
				printf(" ");
		}
		printf("\n");
	}
}

int objdump_dump(const char *path, elf_info_t *elf,
		section_info_t *sections)
{
	uint16_t i;
	uint64_t entry = elf->is_64 ? get64(elf, elf->data + 0x18) :
		get32(elf, elf->data + 0x18);
	const char *name;

	printf("\n%s:     file format %s\n", path, target_name(elf));
	print_flags(elf, sections);
	printf("start address 0x%0*lx\n\n", elf->is_64 ? 16 : 8,
			(unsigned long)entry);

	for (i = 0; i < elf->shnum; i++)
	{
		name = (const char *)(elf->data +
				sections[elf->shstrndx].offset +
				get32(elf, elf->data + elf->shoff +
					(i * elf->shentsize)));

		if (skip_section(&sections[i], name))
			continue;

		print_section(elf, &sections[i], name);
	}

	return (0);
}
