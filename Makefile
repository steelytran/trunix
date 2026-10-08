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
INIT = initrd/sbin/init

UNPAGED_OBJS = \
head.S.o \
init.c.o \
paging.c.o \
pg_utils.S.o \
util.S.o \
string.S.o \
com.S.o \
serial.c.o \
util.c.o \

K_OBJS = \
$(addprefix unpaged_,$(UNPAGED_OBJS)) \
main.c.o \
gate.S.o \
interrupt.S.o \
paging.c.o \
pg_utils.S.o \
mem.c.o \
string.S.o \
util.S.o \
util.c.o \
com.S.o \
serial.c.o \
initrd.c.o \
string.c.o \
proc.c.o \
switch.S.o \

OBJS = $(addprefix kernel/,$(K_OBJS))

kernel/unpaged_%: kernel/%
	$(OBJCOPY) --prefix-symbols=__k_unpaged_ $< $@

.PHONY: all clean iso qemu qemu-gdb
all: iso

$(BIN): $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

%.S.o: %.S
	$(AS) $(ASFLAGS) -c $< -o $@

%.c.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(INIT): sbin/init.S
	$(AS) $(TARGET) -static -c $< -o $<.o
	$(LD) $<.o -o $@

iso: $(BIN) $(INIT)
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
