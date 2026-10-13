#include <stdio.h>

int
fputc(int c, FILE *s)
{
	if (c != '\n' && s->_ptr < s->_end)
		*s->_ptr++ = c;
	else
		 __overflow(s, c);
}
