#include <string.h>

int
strncmp(const char *s1, const char *s2, size_t len)
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
