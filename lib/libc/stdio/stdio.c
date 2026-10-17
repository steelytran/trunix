#include <stdio.h>
#include <unistd.h>
#include <stddef.h>
#include <sys/types.h>

static unsigned char in[BUFSIZ + UNGET];
static unsigned char out[BUFSIZ + UNGET];
static unsigned char err[UNGET];

size_t __stdio_read();
size_t __stdio_write();
off_t __stdio_seek();
int __stdio_close();

const FILE __stdin = {
	._fd = 0,
	._read = __stdio_read,
	._seek = __stdio_seek,
	._close = __stdio_close,
	._buf = in + UNGET,
	._end = in + UNGET + BUFSIZ,
	._ptr = in + UNGET,
	._sz = BUFSIZ,
	._mode = _IOLBF
};

const FILE __stdout = {
	._fd = 1,
	._write = __stdio_write,
	._seek = __stdio_seek,
	._close = __stdio_close,
	._buf = out + UNGET,
	._end = out + UNGET + BUFSIZ,
	._ptr = out + UNGET,
	._sz = BUFSIZ,
	._mode = _IOLBF
};

const FILE __stderr = {
	._fd = 2,
	._write = __stdio_write,
	._seek = __stdio_seek,
	._close = __stdio_close,
	._buf = err + UNGET,
	._end = err + UNGET,
	._ptr = err + UNGET,
	._sz = 0,
	._mode = _IONBF
};

size_t
__stdio_write(FILE *s, const unsigned char *buf, size_t sz)
{
	write(s->_fd, buf, sz);
}

size_t
__stdio_read()
{
	return 0;
}

int
__stdio_close()
{
	return 0;
}

off_t
__stdio_seek()
{
	return 0;
}
