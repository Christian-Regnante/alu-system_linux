# nm_objdump

`hnm` is a simplified implementation of GNU `nm -p`.

## Usage

```bash
./hnm [objfile ...]
```

## Build

```bash
make
```

## Clean

```bash
make clean
```

The program supports 32-bit and 64-bit ELF files, little-endian and
big-endian ELF files, and uses either `.symtab` or `.dynsym` when
available.
