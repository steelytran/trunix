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
AR = llvm-ar
LD = ld.lld
OBJCOPY = llvm-objcopy
GRUB = i686-elf-grub

ASFLAGS = $(INCLUDE) $(TARGET)
CFLAGS = -static -std=c99 -ffreestanding -nostdlib \
-g3 -glldb -fno-pic -fno-pie -fno-builtin -mno-mmx \
-mno-sse -mno-sse2 -mno-3dnow -fno-strict-aliasing \
-fno-stack-protector -mgeneral-regs-only -Wno-int-conversion \
-Wno-pointer-integer-compare $(INCLUDE) $(TARGET) 

INCLUDE = -isystem include
TARGET = -arch i386 -target i386-unknown-none-elf

.PHONY: all clean iso qemu qemu-gdb
all: iso

include lib/libc/Makefile.inc
include sbin/init/Makefile.inc
include kernel/Makefile.inc

iso: $(BIN) $(LIBC) $(INIT)
	mkdir -p iso/boot/grub
	cp $< iso/boot/$<
	tar --numeric-owner -czvf iso/boot/initrd -C initrd .
	cp grub.cfg iso/boot/grub/grub.cfg
	$(GRUB)-mkrescue -o $<.iso iso

qemu: iso
	qemu-system-i386 \
	-cdrom trunix.iso \
	-display cocoa,zoom-to-fit=on \
	-serial mon:stdio \
	-no-reboot -no-shutdown

qemu-gdb: iso
	sh -c 'set -m; \
	qemu-system-i386 \
	-cdrom trunix.iso \
	-display cocoa,zoom-to-fit=on \
	-no-reboot -no-shutdown \
	-serial file:log \
	-s -S & \
	lldb trunix -o "gdb-remote 1234"'

clean:
	rm -rf $(CLEANFILES)
