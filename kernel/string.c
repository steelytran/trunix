/*
 * libc string.h
 * Copyright (C) 2026  spenna
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <sys/cdefs.h>
#include <string.h>

memcmp(void *, void *, u32);
strcmp(const char *, const char *);
u32 strlen(char *);
char *strncpy(char *, const char *, u32);

memcmp(void *s1, void *s2, u32 n)
{
	u32 i;

	unsigned char *str1 = (unsigned char *)s1;
	unsigned char *str2 = (unsigned char *)s2;

	for (i = 0; i < n; ++i) {
		if (str1[i] == str2[i])
			continue;

		return str1[i] - str2[i];
	}

	return 0;
}

strcmp(const char *s1, const char *s2)
{
	int i = 0;

	while (s1[i] != '\0' && s2[i] != '\0') {
		if (s1[i] == s2[i]) {
			++i;
			continue;
		}

		return s1[i] - s2[i];
	}

	return s1[i] - s2[i];
}

u32
strlen(char *s)
{
	u32 n = 0;

	while(*s) {
		++s;
		++n;
	}

	return n;
}

char *
strncpy(char *dst, const char *src, u32 len)
{
	u32 i;

	for (i = 0; i < len; ++i) {
		if (src[i] == '\0')
			break;

		dst[i] = src[i];
	}

	return dst;
}

strncmp(const char *s1, const char *s2, u32 len)
{
	int i = 0;
	unsigned char c1, c2;

	for (; i < len; ++i) {
		c1 = (unsigned char)s1[i];
		c2 = (unsigned char)s2[i];

		if (c1 != c2)
			return c1 - c2;

		if (c1 == '\0')
			break;
	}

	return 0;
}
