#include <string.h>

size_t
strlen(const char *s)
{
	size_t n = 0;

	for(; *s != '\0'; ++s)
		++n;

	return n;
}
