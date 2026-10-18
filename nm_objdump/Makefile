CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic
NM_OBJ = main.o bytes.o read_elf.o symbols.o version.o
OBJDUMP_OBJ = objdump_main.o objdump_info.o objdump_read.o objdump_dump.o bytes.o

all: hnm hobjdump

hnm: $(NM_OBJ)
	$(CC) $(CFLAGS) -o hnm $(NM_OBJ)

hobjdump: $(OBJDUMP_OBJ)
	$(CC) $(CFLAGS) -o hobjdump $(OBJDUMP_OBJ)

main.o: main.c hnm.h
	$(CC) $(CFLAGS) -c main.c

bytes.o: bytes.c hnm.h
	$(CC) $(CFLAGS) -c bytes.c

read_elf.o: read_elf.c hnm.h
	$(CC) $(CFLAGS) -c read_elf.c

symbols.o: symbols.c hnm.h
	$(CC) $(CFLAGS) -c symbols.c

version.o: version.c hnm.h
	$(CC) $(CFLAGS) -c version.c

objdump_main.o: objdump_main.c objdump.h hnm.h
	$(CC) $(CFLAGS) -c objdump_main.c

objdump_read.o: objdump_read.c objdump.h hnm.h
	$(CC) $(CFLAGS) -c objdump_read.c

objdump_dump.o: objdump_dump.c objdump.h hnm.h
	$(CC) $(CFLAGS) -c objdump_dump.c

objdump_info.o: objdump_info.c objdump.h hnm.h
	$(CC) $(CFLAGS) -c objdump_info.c

clean:
	rm -f $(NM_OBJ) $(OBJDUMP_OBJ) hnm hobjdump

re: clean all

.PHONY: all clean re
