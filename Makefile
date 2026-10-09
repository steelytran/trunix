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
GRUB = i686-elf-grub

INCLUDE = -isystem include
TARGET = -arch i386 -target i386-unknown-none-elf

LDFLAGS = -T kernel/linker.ld 
ASFLAGS = $(INCLUDE) $(TARGET)
CFLAGS =  -static -std=c99 -ffreestanding \
-nostdlib -g3 -glldb $(INCLUDE) $(TARGET)

CFLAGS += -fno-pic -fno-pie -fno-builtin -mno-mmx \
-mno-sse -mno-sse2 -mno-3dnow -fno-strict-aliasing \
-fno-stack-protector -mgeneral-regs-only

CFLAGS += -Wno-int-conversion -Wno-pointer-integer-compare

BIN = trunix

KOBJS = \
kernel/unpaged_head.S.o \
kernel/unpaged_init.c.o \
kernel/unpaged_paging.c.o \
kernel/unpaged_pg_utils.S.o \
kernel/unpaged_util.S.o \
kernel/unpaged_string.S.o \
kernel/unpaged_com.S.o \
kernel/unpaged_serial.c.o \
kernel/unpaged_util.c.o \
kernel/main.c.o \
kernel/gate.S.o \
kernel/interrupt.S.o \
kernel/paging.c.o \
kernel/pg_utils.S.o \
kernel/mem.c.o \
kernel/string.S.o \
kernel/util.S.o \
kernel/util.c.o \
kernel/com.S.o \
kernel/serial.c.o \
kernel/initrd.c.o \
kernel/string.c.o \
kernel/proc.c.o \
kernel/switch.S.o \

.PHONY: all clean iso qemu qemu-gdb
all: iso

kernel/unpaged_%: kernel/%
	$(OBJCOPY) --prefix-symbols=__k_unpaged_ $< $@

$(BIN): $(KOBJS)
	$(LD) $(LDFLAGS) $^ -o $@

%.S.o: %.S
	$(AS) $(ASFLAGS) -c $< -o $@

%.c.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

iso: $(BIN)
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
	rm -rf iso
	rm -f *.o
	rm -f */*.o
	rm -f $(BIN)
	rm -f $(BIN).iso
