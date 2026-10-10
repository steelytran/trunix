# trunix kernel
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
kernel/fs.c.o \
kernel/string.c.o \
kernel/proc.c.o \
kernel/switch.S.o \
kernel/console.c.o \
kernel/fbdev.c.o \

BIN = trunix

kernel/unpaged_%: kernel/%
	$(OBJCOPY) --prefix-symbols=__k_unpaged_ $< $@

$(BIN): $(KOBJS)
	$(LD) -T kernel/linker.ld $^ -o $@

%.S.o: %.S
	$(AS) $(ASFLAGS) -c $< -o $@

%.c.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

CLEANFILES += $(KOBJS)
