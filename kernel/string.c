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

#include <string.h>
#include <stddef.h>

int memcmp(const void *, const void *, size_t);
int strcmp(const char *, const char *);
size_t strlen(const char *);
char *strncpy(char *, const char *, size_t);

int
memcmp(const void *s1, const void *s2, size_t n)
{
	size_t i;

	unsigned char *str1 = (unsigned char *)s1;
	unsigned char *str2 = (unsigned char *)s2;

	for (i = 0; i < n; ++i) {
		if (str1[i] == str2[i])
			continue;

		return str1[i] - str2[i];
	}

	return 0;
}

int
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

size_t
strlen(const char *s)
{
	size_t n = 0;

	while(*s) {
		++s;
		++n;
	}

	return n;
}

char *
strncpy(char *dst, const char *src, size_t len)
{
	size_t i;

	for (i = 0; i < len; ++i) {
		if (src[i] == '\0')
			break;

		dst[i] = src[i];
	}

	return dst;
}

/*
int
strncmp(const char *s1, const char *s2, size_t len)
{
	int i = 0;

	while (s1[i] != '\0' && s2[i] != '\0') {
		if (i >= len)
			return s1[i] - s2[i];

		if (s1[i] == s2[i]) {
			++i;
			continue;
		}

		return s1[i] - s2[i];
	}

	return 0;

}
*/
