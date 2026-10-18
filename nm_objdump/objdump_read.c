#include "objdump.h"

static int load_bytes(const char *path, elf_info_t *elf)
{
	int fd;
	ssize_t bytes;
	unsigned char ident[16];

	fd = open(path, O_RDONLY);
	if (fd < 0)
		return (-1);
	if (read(fd, ident, sizeof(ident)) != (ssize_t)sizeof(ident) ||
			ident[0] != 0x7f || ident[1] != 'E' ||
			ident[2] != 'L' || ident[3] != 'F')
	{
		close(fd);
		return (-1);
	}
	elf->is_64 = ident[4] == ELFCLASS64;
	elf->big_endian = ident[5] == ELFDATA2MSB;
	elf->osabi = ident[7];
	if (lseek(fd, 0, SEEK_END) == (off_t)-1)
	{
		close(fd);
		return (-1);
	}
	elf->size = (size_t)lseek(fd, 0, SEEK_CUR);
	lseek(fd, 0, SEEK_SET);
	elf->data = malloc(elf->size);
	if (!elf->data)
	{
		close(fd);
		return (-1);
	}
	bytes = read(fd, elf->data, elf->size);
	close(fd);
	if (bytes != (ssize_t)elf->size)
	{
		free(elf->data);
		return (-1);
	}
	return (0);
}

static void parse_header(elf_info_t *elf)
{
	if (elf->is_64)
	{
		elf->shoff = get64(elf, elf->data + 0x28);
		elf->shentsize = get16(elf, elf->data + 0x3a);
		elf->shnum = get16(elf, elf->data + 0x3c);
		elf->shstrndx = get16(elf, elf->data + 0x3e);
	}
	else
	{
		elf->shoff = get32(elf, elf->data + 0x20);
		elf->shentsize = get16(elf, elf->data + 0x2e);
		elf->shnum = get16(elf, elf->data + 0x30);
		elf->shstrndx = get16(elf, elf->data + 0x32);
	}
}

int objdump_read(const char *path, elf_info_t *elf)
{
	if (load_bytes(path, elf) != 0)
	{
		dprintf(STDERR_FILENO,
				"./hobjdump: %s: File format not recognized\n", path);
		return (-1);
	}
	parse_header(elf);
	return (0);
}

static void fill_section(const elf_info_t *elf, section_info_t *section,
		const unsigned char *raw)
{
	if (elf->is_64)
	{
		section->type = get32(elf, raw + 4);
		section->flags = get64(elf, raw + 8);
		section->addr = get64(elf, raw + 16);
		section->offset = get64(elf, raw + 24);
		section->size = get64(elf, raw + 32);
		section->link = get32(elf, raw + 40);
		section->entsize = get64(elf, raw + 56);
	}
	else
	{
		section->type = get32(elf, raw + 4);
		section->flags = get32(elf, raw + 8);
		section->addr = get32(elf, raw + 12);
		section->offset = get32(elf, raw + 16);
		section->size = get32(elf, raw + 20);
		section->link = get32(elf, raw + 24);
		section->entsize = get32(elf, raw + 36);
	}
}

int objdump_sections(elf_info_t *elf, section_info_t **sections)
{
	section_info_t *array;
	uint16_t i;

	if (elf->shoff == 0 || elf->shnum == 0)
		return (-1);
	array = calloc(elf->shnum, sizeof(*array));
	if (!array)
		return (-1);
	for (i = 0; i < elf->shnum; i++)
	{
		fill_section(elf, &array[i],
				elf->data + elf->shoff + (i * elf->shentsize));
	}
	*sections = array;
	return (0);
}
