CC = clang
AS = clang
LD = ld.lld

CFLAGS = -ffreestanding -nostdlib

GRUB = i686-elf-grub

TARGET = -arch i386 -target i386-unknown-none-elf

BIN = trunix

OBJS =\
init.S.o \
kernel.c.o \
boot.S.o \
print.c.o \
tty.S.o \

.PHONY: all clean iso

all: $(BIN)

$(BIN): $(OBJS)
	$(LD) -T linker.ld $^ -o $@

%.S.o: %.S
	$(AS) $(TARGET) -c $< -o $@

%.c.o: %.c
	$(CC) $(TARGET) $(CFLAGS) -c $< -o $@

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
