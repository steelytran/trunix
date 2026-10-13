#include <stdio.h>

int
__overflow(FILE *s, int ch)
{
	unsigned char c = ch;
	fflush(s);
	s->_write(s, &c, 1);

	return 0;
}

int
fflush(FILE *s)
{
	s->_write(s, s->_buf, s->_ptr - s->_buf);
	s->_ptr = s->_buf;

	return 0;
}
