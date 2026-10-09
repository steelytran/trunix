# init
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

INCLUDE = -isystem ../../include
TARGET = -arch i386 -target i386-unknown-none-elf

LDFLAGS = -L../../lib/libc -lc
ASFLAGS = $(INCLUDE) $(TARGET)
CFLAGS =  -static -std=c99 -ffreestanding \
-g3 -glldb -nostdlib $(INCLUDE) $(TARGET)

CFLAGS += -fno-pic -fno-pie -fno-builtin -mno-mmx \
-mno-sse -mno-sse2 -mno-3dnow -fno-strict-aliasing \
-fno-stack-protector -mgeneral-regs-only

BIN = ../../initrd/sbin/init

.POSIX:
.SUFFIXES:
.PHONY: all clean

all: $(BIN)

$(BIN): init.o crt0.o
	$(LD) $(LDFLAGS) $^ -o $@

init.o: init.c
	$(CC) $(CFLAGS) -c $< -o $@

crt0.o: crt0.S
	$(AS) $(ASFLAGS) -c $< -o $@

clean:
	rm -f init.o crt0.o
