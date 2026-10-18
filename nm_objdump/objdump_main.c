#include "objdump.h"

static int process_file(const char *path)
{
	elf_info_t elf;
	section_info_t *sections = NULL;
	int status;

	if (objdump_read(path, &elf) != 0)
		return (-1);
	if (objdump_sections(&elf, &sections) != 0)
	{
		free(elf.data);
		return (-1);
	}
	status = objdump_dump(path, &elf, sections);
	free(sections);
	free(elf.data);
	return (status);
}

int main(int argc, char **argv)
{
	int i;
	int status = 0;

	if (argc < 2)
	{
		dprintf(STDERR_FILENO, "Usage: hobjdump [objfile ...]\n");
		return (1);
	}
	for (i = 1; i < argc; i++)
	{
		if (process_file(argv[i]) != 0)
			status = 1;
	}
	return (status);
}
