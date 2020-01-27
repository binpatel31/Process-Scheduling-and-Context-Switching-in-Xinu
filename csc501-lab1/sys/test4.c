#include <conf.h>
#include <kernel.h>
#include <q.h>

/* test4.c
 * This test program creates three processes, prA, prB, and prC, at
 * priorities 10, 9, and 10 respectively.  The main process has priority 20.
 *
 * The main routine then calls chprio to change the priority of prB to be 15,
 * while it changes its own priority to 5.
 */

int prch(), prA, prB, prC;

main()
{
	int i;

        kprintf("\n\nTEST4:\n");

	resume(prA = create(prch,2000,10,"proc A",1,'A'));
	resume(prB = create(prch,2000, 9,"proc B",1,'B'));
	resume(prC = create(prch,2000,10,"proc C",1,'C'));


	chprio(prB,      15);
	chprio(getpid(),  5);

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
