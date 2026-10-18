#include "hnm.h"

static int find_section(const elf_info_t *elf,
		const section_info_t *sections, uint32_t type, uint32_t link)
{
	uint16_t i;

	for (i = 0; i < elf->shnum; i++)
	{
		if (sections[i].type == type &&
				(link == 0 || sections[i].link == link))
			return (i);
	}
	return (-1);
}

static uint16_t get_version_index(const elf_info_t *elf,
		const section_info_t *sections, const char *name)
{
	int sym_i, ver_i;
	unsigned char *sym, *strtab;
	uint64_t count, entry, i;
	uint16_t version;

	sym_i = find_section(elf, sections, SHT_DYNSYM, 0);
	if (sym_i < 0)
		return (0);
	ver_i = find_section(elf, sections, SHT_VERSYM, (uint32_t)sym_i);
	if (ver_i < 0)
		return (0);
	strtab = elf->data + sections[sections[sym_i].link].offset;
	entry = sections[sym_i].entsize ? sections[sym_i].entsize :
		(elf->is_64 ? 24 : 16);
	count = sections[sym_i].size / entry;
	for (i = 0; i < count; i++)
	{
		sym = elf->data + sections[sym_i].offset + (i * entry);
		if (strcmp((const char *)(strtab + get32(elf, sym)), name) == 0)
		{
			version = get16(elf, elf->data +
					sections[ver_i].offset + (i * 2));
			return (version);
		}
	}
	return (0);
}

static char *duplicate(const char *source)
{
	char *copy;

	copy = malloc(strlen(source) + 1);
	if (copy)
		strcpy(copy, source);
	return (copy);
}

char *symbol_version(const elf_info_t *elf, const section_info_t *sections,
		const char *name)
{
	int need_i;
	uint32_t i, j, aux_off, next;
	uint16_t index;
	unsigned char *need, *aux;

	if (elf->osabi != 0 && elf->osabi != 3)
		return (NULL);
	index = get_version_index(elf, sections, name);
	if (index <= 1)
		return (NULL);
	need_i = find_section(elf, sections, SHT_VERNEED, 0);
	if (need_i < 0)
		return (NULL);
	need = elf->data + sections[need_i].offset;
	for (i = 0; i < sections[need_i].size;)
	{
		aux_off = get32(elf, need + 8);
		aux = need + aux_off;
		for (j = 0; j < get16(elf, need + 2); j++)
		{
			if (get16(elf, aux + 6) == index)
			{
				return (duplicate((const char *)(elf->data +
						sections[sections[need_i].link].offset +
						get32(elf, aux + 8))));
			}
			next = get32(elf, aux + 12);
			if (next == 0)
				break;
			aux += next;
		}
		next = get32(elf, need + 12);
		if (next == 0)
			break;
		need += next;
		i += next;
	}
	return (NULL);
}
