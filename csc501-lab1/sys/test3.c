#include <conf.h>
#include <kernel.h>
#include <q.h>

/* test3.c
 * This test program creates three processes, prA, prB, and prC, at
 * priority 10.  The main process has priority 20.
 *
 * The main routine then calls chprio to change the priorities of prA, prB
 * and prC to be 30, while it remains at priority 20.
 */

int prch(), prA, prB, prC;

main()
{
	int i;

        kprintf("\n\nTEST3:\n");

	resume(prA = create(prch,2000,10,"proc A",1,'A'));
	resume(prB = create(prch,2000,10,"proc B",1,'B'));
	resume(prC = create(prch,2000,10,"proc C",1,'C'));

	chprio(prA, 30);
	chprio(prB, 30);
	chprio(prC, 30);

	while (1) {
		kprintf("%c", 'D');
		for (i=0; i< 10000; i++);
	}
}

prch(c)
char c;
{
	int i;

	while(1) {
		kprintf("%c", c);
		for (i=0; i< 10000; i++);
	}
}
