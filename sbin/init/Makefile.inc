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

INIT = initrd/sbin/init

$(INIT): sbin/init/init.o sbin/init/crt0.o
	$(LD) -Llib/libc -lc $^ -o $@

sbin/init/init.o: sbin/init/init.c
	$(CC) $(CFLAGS) -c $< -o $@

sbin/init/crt0.o: sbin/init/crt0.S
	$(AS) $(ASFLAGS) -c $< -o $@

CLEANFILES += $(INIT) sbin/init/init.o sbin/init/crt0.o
