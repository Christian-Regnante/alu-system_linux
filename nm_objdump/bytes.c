#include "hnm.h"

uint16_t get16(const elf_info_t *elf, const unsigned char *p)
{
	uint16_t value = (uint16_t)((p[0] << 8) | p[1]);

	if (!elf->big_endian)
		value = (uint16_t)((value >> 8) | (value << 8));
	return (value);
}

uint32_t get32(const elf_info_t *elf, const unsigned char *p)
{
	uint32_t value = ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16);

	value |= ((uint32_t)p[2] << 8) | p[3];
	if (!elf->big_endian)
		value = ((value >> 24) & 0xff) | ((value >> 8) & 0xff00) |
			((value << 8) & 0xff0000) | ((value << 24) & 0xff000000);
	return (value);
}

uint64_t get64(const elf_info_t *elf, const unsigned char *p)
{
	uint64_t value = 0;
	int i;

	for (i = 0; i < 8; i++)
		value = (value << 8) | p[i];
	if (!elf->big_endian)
	{
		value = ((value >> 56) & 0xff) |
			((value >> 40) & 0xff00) |
			((value >> 24) & 0xff0000) |
			((value >> 8) & 0xff000000) |
			((value << 8) & 0xff00000000UL) |
			((value << 24) & 0xff0000000000UL) |
			((value << 40) & 0xff000000000000UL) |
			((value << 56) & 0xff00000000000000UL);
	}
	return (value);
}
