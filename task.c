#include <sys/trunix.h>
#include <string.h>

void
switch_to(void (*t)(void))
{
	struct tss_entry tss;

	memset(&tss, 0, sizeof(tss));

	tss.ss0 = 0x10; /* kernel data selector */
	//tss.esp0 = ;

	tss.cs = 0x1B; /* user code selector */
	tss.ss = 0x20; /* user data selector */
	//tss.esp = ;
	tss.eip = t;
	tss.eflags = 0x202;
}
