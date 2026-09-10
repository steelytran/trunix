CC = clang
AS = clang
LD = ld.lld
OBJCOPY = llvm-objcopy

CFLAGS = -ffreestanding -nostdlib -g -Wall -Wextra -Wpedantic -std=c99
INCLUDE = -isystem include

GRUB = i686-elf-grub

TARGET = -arch i386 -target i386-unknown-none-elf

BIN = trunix

UNPAGED_OBJS = \
head.S.o \
tty.S.o \
string.S.o \
print.c.o \
init.c.o \
paging.c.o \
paging.S.o \

OBJS = \
$(addprefix unpaged_,$(UNPAGED_OBJS)) \
main.c.o

.PHONY: all clean iso

all: $(BIN)

$(BIN): $(OBJS)
	$(LD) -T linker.ld $^ -o $@

unpaged_%: %
	$(OBJCOPY) --prefix-symbols=__k_unpaged_ $< $@

%.S.o: %.S
	$(AS) $(INCLUDE) $(TARGET) -c $< -o $@

%.c.o: %.c
	$(CC) $(INCLUDE) $(TARGET) $(CFLAGS) -c $< -o $@

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
