CC = clang
AS = clang
LD = ld.lld

CFLAGS = -ffreestanding -nostdlib -O1 -g -Wall -Wextra -Wpedantic -std=c99

GRUB = i686-elf-grub

TARGET = -arch i386 -target i386-unknown-none-elf

BIN = trunix

OBJS =\
boot.S.o \
main.S.o \
tty.S.o \
print.c.o \
interrupt.S.o \
#kernel.c.o \

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

offsets: offsets.c
	$(CC) $< -o $@

clean:
	rm -rf isodir
	rm -f offsets
	rm -f *.o
	rm -f $(BIN)
	rm -f $(BIN).iso
