#include "hnm.h"

static char symbol_letter(const symbol_info_t *symbol,
		const section_info_t *section)
{
	int global = symbol->bind != STB_LOCAL;

	if (symbol->shndx == SHN_UNDEF)
		return (symbol->bind == STB_WEAK ? 'w' : 'U');
	if (symbol->bind == STB_GNU_UNIQUE)
		return ('u');
	if (symbol->bind == STB_WEAK)
		return (symbol->type == STT_OBJECT ? 'V' : 'W');
	if (symbol->shndx == SHN_ABS)
		return ('A');
	if (symbol->shndx == SHN_COMMON)
		return ('C');
	if (section && (section->type == SHT_INIT_ARRAY ||
			section->type == SHT_FINI_ARRAY ||
			section->type == SHT_PREINIT_ARRAY))
		return (global ? 'T' : 't');
	if (!section || !(section->flags & SHF_ALLOC))
		return (global ? 'N' : 'n');
	if (section->flags & SHF_EXECINSTR)
		return (global ? 'T' : 't');
	if (section->type == SHT_NOBITS)
		return (global ? 'B' : 'b');
	if (section->flags & SHF_WRITE)
		return (global ? 'D' : 'd');
	return (global ? 'R' : 'r');
}

static void print_symbol(const elf_info_t *elf,
		const section_info_t *sections, const symbol_info_t *symbol)
{
	int width = elf->is_64 ? 16 : 8;
	char letter;
	char *version;

	letter = symbol_letter(symbol, symbol->shndx < elf->shnum ?
			&sections[symbol->shndx] : NULL);
	if (symbol->shndx == SHN_UNDEF)
		printf("%*s %c %s", width, "", letter, symbol->name);
	else
		printf("%0*lx %c %s", width,
				(unsigned long)symbol->value, letter, symbol->name);
	version = symbol_version(elf, sections, symbol->name);
	if (version)
	{
		printf("@@%s", version);
		free(version);
	}
	printf("\n");
}

static int print_symtab(const elf_info_t *elf,
		const section_info_t *sections, uint16_t sym_i)
{
	unsigned char *sym, *strtab;
	uint64_t count, entry, i;
	symbol_info_t symbol;

	strtab = elf->data + sections[sections[sym_i].link].offset;
	entry = sections[sym_i].entsize ? sections[sym_i].entsize :
		(elf->is_64 ? 24 : 16);
	count = sections[sym_i].size / entry;
	for (i = 0; i < count; i++)
	{
		sym = elf->data + sections[sym_i].offset + (i * entry);
		if (elf->is_64)
		{
			symbol.value = get64(elf, sym + 8);
			symbol.shndx = get16(elf, sym + 6);
			symbol.bind = sym[4] >> 4;
			symbol.type = sym[4] & 0xf;
		}
		else
		{
			symbol.value = get32(elf, sym + 4);
			symbol.shndx = get16(elf, sym + 14);
			symbol.bind = sym[12] >> 4;
			symbol.type = sym[12] & 0xf;
		}
		symbol.name = (const char *)(strtab + get32(elf, sym));
		if (symbol.type == STT_SECTION || symbol.type == STT_FILE ||
				symbol.name[0] == '\0')
			continue;
		print_symbol(elf, sections, &symbol);
	}
	return (0);
}

int print_symbols(elf_info_t *elf, section_info_t *sections,
		const char *path)
{
	uint16_t i;

	for (i = 0; i < elf->shnum; i++)
	{
		if (sections[i].type == SHT_SYMTAB)
			return (print_symtab(elf, sections, i));
	}
	dprintf(STDERR_FILENO, "./hnm: %s: no symbols\n", path);
	return (-1);
}
