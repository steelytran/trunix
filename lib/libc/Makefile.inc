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

COBJS = \
lib/libc/unistd/fork.o

$(LIBC): $(COBJS)
	$(AR) rcs $@ $^

%.S.o: %.S
	$(AS) $(ASFLAGS) -c $< -o $@

%.c.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

CLEANFILES += $(COBJS) $(LIBC)
