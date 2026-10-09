#include <unistd.h>

int
main(void)
{
	int x = fork();

	if (x < 0)
		for (;;); /* failed */
	else if (x == 0)
		for (;;); /* child */
	else
		for (;;); /* parent */
}
