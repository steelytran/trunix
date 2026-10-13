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
CFLAGS = -ffreestanding -nostdlib -nobuiltininc -fno-builtin \
-fno-strict-aliasing -fno-stack-protector -fno-pic -fno-pie \
-mno-sse -mno-sse2 -mno-3dnow -mstack-alignment=4 -mno-mmx \
-mgeneral-regs-only -Wno-pointer-integer-compare -Wno-int-conversion \
-Wno-incompatible-function-pointer-types -g3 -glldb -static -std=c99 \
$(INCLUDE) $(TARGET) 

INCLUDE = -isystem include
TARGET = -arch i386 -target i386-unknown-none-elf

.PHONY: all clean iso qemu qemu-gdb
all: iso

include lib/libc/Makefile.inc
include sbin/init/Makefile.inc
include kernel/Makefile.inc

iso: $(BIN) $(LIBC) $(INIT) $(SH)
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
	-s -S & \
	lldb trunix -o "gdb-remote 1234"'

clean:
	rm -rf $(CLEANFILES)
