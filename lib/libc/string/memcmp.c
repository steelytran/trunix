#include <string.h>

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
