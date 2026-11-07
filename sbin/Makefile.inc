# /sbin/
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
TEST = initrd/sbin/test

$(INIT): sbin/init/init.o
	$(LD) --sysroot=. -L=/lib -lc $^ lib/crt0.o -o $@

sbin/init/init.o: sbin/init/init.c
	$(CC) $(CFLAGS) --sysroot=. -c $< -o $@


$(TEST): sbin/test/test.o
	$(LD) --sysroot=. -L=/lib -lc $^ lib/crt0.o -o $@

sbin/test/test.o: sbin/test/test.c
	$(CC) $(CFLAGS) --sysroot=. -c $< -o $@

CLEANFILES += $(INIT) $(TEST) sbin/init/init.o sbin/test/test.o
