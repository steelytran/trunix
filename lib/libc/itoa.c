char *
itoa(int n, unsigned base)
{
	int j, i = 0;
	char c;
	static char s[33];
	unsigned int un;
	const static char hex[] = "0123456789abcdef";

	if (n == 0) {
		s[0] = '0';
		s[1] = '\0';
		return s;
	}

	if (n < 0 && base == 10) {
		s[i++] = '-';
		un = (unsigned int)(-n);
		j = 1;
	} else {
		un = (unsigned int)n;
		j = 0;
	}

	while (un > 0) {
		s[i++] = hex[un % base];
		un /= base;
	} 

	s[i] = '\0';

	for (--i; j < i; --i, ++j) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}

	return s;
}
