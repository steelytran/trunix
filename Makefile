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

CC := clang
AS := clang
LD := ld.lld
OBJCOPY := llvm-objcopy

include kernel/Makefile.inc

INCLUDE := -isystem include
TARGET := -arch i386 -target i386-unknown-none-elf

LDFLAGS := -T kernel/linker.ld 
ASFLAGS := $(INCLUDE) $(TARGET)
CFLAGS := -ffreestanding -nostdlib -Wall -Wextra \
	 -Wpedantic -g -std=c99 $(INCLUDE) $(TARGET)

GRUB := i686-elf-grub

BIN := trunix

OBJS ?=

.PHONY: all clean iso qemu

all: iso

$(BIN): $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

%.S.o: %.S
	$(AS) $(ASFLAGS) -c $< -o $@

%.c.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

iso: $(BIN)
	mkdir -p isodir/boot/grub
	cp $< isodir/boot/$<
	cp grub.cfg isodir/boot/grub/grub.cfg
	#tar --numeric-owner -cvf isodir/boot/initrd -C initrd .
	$(GRUB)-mkrescue -o $<.iso isodir

qemu: iso
	qemu-system-i386 \
	-cdrom trunix.iso \
	-display cocoa,zoom-to-fit=on \
	-no-reboot -no-shutdown

clean:
	rm -rf isodir
	rm -f *.o
	rm -f */*.o
	rm -f $(BIN)
	rm -f $(BIN).iso
