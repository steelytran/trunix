#include <string.h>

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
