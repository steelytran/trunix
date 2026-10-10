#include <sys/param.h>
#include <string.h>

size_t
strlcpy(char *dst, const char *src, size_t len)
{
	size_t i = 0, sz = 0;

	for (;; ++sz)
		if (src[sz] == '\0')
			break;

	for (; i < MIN(sz, len); ++i)
		dst[i] = src[i];

	dst[i] = '\0';

	return sz;
}
