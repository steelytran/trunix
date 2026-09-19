#include <sys/kthread.h>
#include <trunix/trunix.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define ELFMAGIC 0x464C457F

struct elfhdr {
  uint32_t magic;
  uint8_t elf[12];
  uint16_t type;
  uint16_t machine;
  uint32_t version;
  uint32_t entry;
  uint32_t ph_off;
  uint32_t sh_off;
  uint32_t flags;
  uint16_t hdrsize;
  uint16_t phe_size;
  uint16_t ph_n;
  uint16_t she_size;
  uint16_t sh_n;
  uint16_t shstrndx;
};

struct proghdr {
	uint32_t type;
	uint32_t flags;
	uint32_t off;
	uint32_t vaddr;
	uint32_t paddr;
	uint32_t filesz;
	uint32_t memsz;
	uint32_t align;
};

/*
int
execve(char *path, char **argv)
{
	uint32_t *pt;
	uint32_t *pd;
	uint8_t *init;
	struct elfhdr *elf;
	struct proghdr *ph;
	int i;

	pt = mmap(NULL, 0x1000, PROT_WRITE, 0, 0, 0);
	pd = mmap(NULL, 0x1000, PROT_WRITE, 0, 0, 0);

	for (i = 2; i < k.ino_n; ++i)
		if (strcmp(path, k.tnodes[i].name) == 0) {
			init = (uint8_t *)k.inodes[i].addr;
			break;
		}

	elf = (struct elfhdr *)init;
	if (elf->magic != ELFMAGIC)
		panic();

	ph = (struct proghdr *)(init + elf->ph_off);

	for (i = 0; i < elf->ph_n; ++i)
		if (ph[i].type == 0)
			continue;
}
*/
