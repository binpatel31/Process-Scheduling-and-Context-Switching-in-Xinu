#include <conf.h>
#include <kernel.h>
#include <q.h>

/* test1.c
 * This test program creates three processes, prA, prB, and prC, at
 * priority 20.  The main process also has priority 20.
 */

int prch(), prA, prB, prC;

main()
{
	int i;

	kprintf("\n\nTEST1:\n");
	resume(prA = create(prch,2000,10,"proc A",1,'A'));
	resume(prB = create(prch,2000,20,"proc B",1,'B'));
	resume(prC = create(prch,2000,30,"proc C",1,'C'));

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
