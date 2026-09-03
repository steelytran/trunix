CC = clang
AS = clang
LD = ld.lld

CFLAGS = -ffreestanding -nostdlib -O1 -g -Wall -Wextra -Wpedantic -std=c99
INCLUDE = -isystem include

GRUB = i686-elf-grub

TARGET = -arch i386 -target i386-unknown-none-elf

BIN = trunix

OBJS =\
io.S.o \
interrupt.S.o \
boot.S.o \
kernel.S.o \
tty.S.o \
string.S.o \
kernel.c.o \
print.c.o \

.PHONY: all clean iso

all: $(BIN)

$(BIN): $(OBJS)
	$(LD) -T linker.ld $^ -o $@

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
