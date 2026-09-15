CC = clang
AS = clang
LD = ld.lld
OBJCOPY = llvm-objcopy

INCLUDE = -isystem include
TARGET = -arch i386 -target i386-unknown-none-elf

ASFLAGS = $(INCLUDE) $(TARGET)
CFLAGS = -ffreestanding -nostdlib -Wall -Wextra -Wpedantic -g -std=c99\
$(INCLUDE) $(TARGET)

GRUB = i686-elf-grub

BIN = trunix

UNPAGED_OBJS = head.S.o tty.S.o string.S.o \
printf.c.o init.c.o mem.c.o paging.S.o util.c.o

OBJS = \
$(addprefix unpaged_,$(UNPAGED_OBJS)) \
string.S.o printf.c.o main.c.o gate.S.o mem.c.o \
paging.S.o tty.S.o util.c.o alloc.c.o

.PHONY: all clean iso

all: $(BIN)

$(BIN): $(OBJS)
	$(LD) -T linker.ld $^ -o $@

unpaged_%: %
	$(OBJCOPY) --prefix-symbols=__k_unpaged_ $< $@

%.S.o: %.S
	$(AS) $(ASFLAGS) -c $< -o $@

%.c.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

iso: $(BIN)
	mkdir -p isodir/boot/grub
	cp $< isodir/boot/$<
	cp grub.cfg isodir/boot/grub/grub.cfg
	$(GRUB)-mkrescue -o $<.iso isodir

clean:
	rm -rf isodir
	rm -f *.o
	rm -f $(BIN)
	rm -f $(BIN).iso
