# c standard library
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

LIBC = lib/libc/libc.a

LOBJS = \
lib/libc/itoa.o \
lib/libc/assert.o \
lib/libc/stdlib/malloc.o \
lib/libc/stdio/stdio.o \
lib/libc/stdio/stdio.o \
lib/libc/stdio/fputc.o \
lib/libc/stdio/fflush.o \
lib/libc/stdio/printf.o \
lib/libc/stdio/fprintf.o \
lib/libc/stdio/vfprintf.o \
lib/libc/sys/mman/mmap.o \
lib/libc/fcntl/open.o \
lib/libc/unistd/fork.o \
lib/libc/unistd/execve.o \
lib/libc/unistd/read.o \
lib/libc/unistd/write.o \
lib/libc/string/memcmp.o \
lib/libc/string/memcpy.o \
lib/libc/string/memmove.o \
lib/libc/string/memset.o \
lib/libc/string/strcmp.o \
lib/libc/string/strlcpy.o \
lib/libc/string/strlen.o \
lib/libc/string/strncmp.o \
lib/libc/string/strncpy.o \
lib/libc/string/strerror.o \

$(LIBC): $(LOBJS)
	$(AR) rcs $@ $^

%.S.o: %.S
	$(AS) $(ASFLAGS) -c $< -o $@

%.c.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

CLEANFILES += $(LOBJS) $(LIBC)
