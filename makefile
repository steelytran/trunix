# the trunix operating system.
# Copyright (C) 2026  spenna
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

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
printf.c.o init.c.o mem.c.o paging.S.o util.S.o

OBJS = \
$(addprefix unpaged_,$(UNPAGED_OBJS)) \
string.S.o printf.c.o main.c.o gate.S.o mem.c.o \
paging.S.o tty.S.o util.S.o alloc.c.o kthread.c.o \
switch.S.o initrd.c.o string.c.o

.PHONY: all clean iso

#all: $(BIN)
all: iso

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
	tar --numeric-owner -cvf isodir/boot/initrd -C initrd .
	$(GRUB)-mkrescue -o $<.iso isodir

clean:
	rm -rf isodir
	rm -f *.o
	rm -f $(BIN)
	rm -f $(BIN).iso
