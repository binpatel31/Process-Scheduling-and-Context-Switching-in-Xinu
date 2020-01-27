#include <conf.h>
#include <kernel.h>
#include <q.h>

/* test2.c
 * This test program creates three processes, prA, prB, and prC, at
 * priority 30.  The main process has priority 20.
 */

int prch(), prA, prB, prC;

main()
{
	int i;

        kprintf("\n\nTEST2:\n");

	prA = create(prch,2000,30,"proc A",1,'A');
	prB = create(prch,2000,30,"proc B",1,'B');
	prC = create(prch,2000,30,"proc C",1,'C');

	resume(prC);
	resume(prB);
	resume(prA);

	while(1) {
		kprintf("%c", 'D');
		for (i = 0; i< 10000; i++);
	}
}

prch(c)
char c;
{
	int i;

	while(1) {
		kprintf("%c", c);
		for (i = 0; i< 10000; i++);
	}
}

