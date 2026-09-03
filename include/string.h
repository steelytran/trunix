#ifndef STRING_H
#define STRING_H

#include <stddef.h>

void *memcpy(void *restrict dst, const void *restrict src, size_t n);
void *memcpyl(void *restrict dst, const void *restrict src, size_t n);
void *memcpyw(void *restrict dst, const void *restrict src, size_t n);
void *memset(void *b, int c, size_t len);
void *memsetw(void *b, int c, size_t len);
void *memsetl(void *b, int c, size_t len);

#endif
