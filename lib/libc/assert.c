#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

__dead void
__assert_fail(const char *expr, const char *file, int line, const char *func)
{
	fprintf(stderr, "Assertation failed: %s (%s: %s: %d)\n",
	    expr, file, func, line);

	abort();
}
