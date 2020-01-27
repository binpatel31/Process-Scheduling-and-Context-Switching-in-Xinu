#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>

int prcha(), prchb(), prA, prB, prC;
int prchb1(), prcha2(), prchb2(), prcha3(), prcha4(), prchb4(), prchb5();
unsigned long ctr100;
int a = 0;
int b = 0;
int a0=0, b0 = 100000;
int s;

main()
{
	int i, j = 0;
	char buf[8];

	kprintf("Please Input:\n");
	while ((i = read(CONSOLE, buf, sizeof(buf))) <1);
	buf[i] = 0;
	s = atoi(buf);
	kprintf("Get %d\n", s);

	switch(s) {
case 1:
	(prA = create(prcha,2000,30, RANDOMSCHED, "proc A",1,'A'));
	(prB = create(prchb,2000,60, RANDOMSCHED, "proc A",1,'B'));
	resume(prA);
	resume(prB);
	break;
case 2:
        (prA = create(prcha,2000,30, RANDOMSCHED, "proc A",1,'A'));
        (prB = create(prchb1,2000,60, RANDOMSCHED, "proc A",1,'B'));
	resume(prA);
	resume(prB);
	break;
case 3:
        (prA = create(prcha2,2000,30, RANDOMSCHED, "proc A",1,'A'));
        (prB = create(prchb2,2000,60, RANDOMSCHED, "proc A",1,'B'));
	resume(prA);
	resume(prB);
	break;
case 4:
	(prA = create(prcha,2000,1, PROPORTIONALSHARE, "proc A",1,'A'));
	(prB = create(prchb,2000,2, PROPORTIONALSHARE, "proc A",1,'B'));
	resume(prA);
	resume(prB);
	break;
case 5:
	(prA = create(prcha4,2000,1, PROPORTIONALSHARE, "proc A",1,'A'));
	(prB = create(prchb4,2000,2, PROPORTIONALSHARE, "proc A",1,'B'));
	resume(prA);
	resume(prB);
	break;
case 6:
	resume(prA = create(prcha,2000,1, PROPORTIONALSHARE, "proc A",1,'A'));
        resume(prB = create(prchb5,2000,2, PROPORTIONALSHARE, "proc A",1,'B'));
	resume(prA);
	resume(prB);
	break;
case 7:
	(prA = create(prcha2,2000,1, PROPORTIONALSHARE, "proc A",1,'A'));
	(prB = create(prchb2,2000,2, PROPORTIONALSHARE, "proc A",1,'B'));
	resume(prA);
	resume(prB);
	break;
case 8:
	(prA = create(prcha,2000,1, PROPORTIONALSHARE, "proc A",1,'A'));
	(prB = create(prchb,2000,60, RANDOMSCHED, "proc A",1,'B'));
	resume(prA);
	resume(prB);
	break;
case 9:
	(prA = create(prcha2,2000,1, PROPORTIONALSHARE, "proc A",1,'A'));
	(prB = create(prchb2,2000,60, RANDOMSCHED, "proc A",1,'B'));
	resume(prA);
	resume(prB);
	break;
case 0:
	(prA = create(prcha3,2000,1, PROPORTIONALSHARE, "proc A",1,'A'));
	(prB = create(prchb,2000,60, RANDOMSCHED, "proc A",1,'B'));
	resume(prA);
	resume(prB);
	break;

	}	
	sleep(10);
	kill(prA);
	kill(prB);
	switch(s) {
case 1:	kprintf("\nTest1 RESULT: A = %d, B = %d (1:2).\n", a, b); break;
case 2: kprintf("\nTest2 RESULT: A0 = %d, B0 = %d (1:2), delta A = %d, delta B = %d (2:1).\n", a0, b0,
		a-a0, b-b0); break;
case 3: kprintf("\nTest3 RESULT: A = %d, B = %d (1:2).\n", a, b); break;
case 4: kprintf("\nTest4 RESULT: A = %d, B = %d (1:2).\n", a, b); break;
case 5: kprintf("\nTest5 RESULT: A0 = %d, B0 = %d (1:2), A = %d, B = %d (1:2).\n", a0, b0,
		a, b); break;
case 6: kprintf("\nTest6 RESULT: A0 = %d, B0 = %d (1:2), delta A = %d, delta B = %d (2:1).\n", a0, b0,
                a-a0, b-b0); break;
case 7: kprintf("\nTest7 RESULT: A = %d, B = %d (1:2).\n", a, b); break;
case 8: kprintf("\nTest8 RESULT: A = %d, B = %d (1:1).\n", a, b); break;
case 9: kprintf("\nTest9 RESULT: A = %d, B = %d (1:1).\n", a, b); break;
case 0: kprintf("\nTest0 RESULT: A0 = %d, B0 = %d (1:1), delta A = %d, delta B = %d (1:4).\n", a0, b0,
                a-a0, b-b0); break;
	}
}

prcha(c)
char c;
{
	int i;
	kprintf("Start... %c\n", c);	
	if (s<4) b=0;

	while(1) {
		for (i=0; i< 10000; i++);
			a++;
	}
}

prchb(c)
char c;
{
	int i;
	if (s>0 && s<4) a=0;

	kprintf("Start... %c\n", c);	
	while(1) {
		for (i=0; i<10000; i++);
			b++;
	}
}

prchb1(c)
char c;
{
        int i;
        kprintf("Start... %c\n", c);
	if (s<4) a = 0;

        while(1) {
                for (i=0; i< 10000; i++);
                        b++;
                if (b == 300)
                        if (c == 'B') {
				a0 = a;
				b0 = b;
                                kprintf("\n\n");
                                chprio(prA, 60);
                                chprio(prB, 30);
                        }
        }
}

prchb5(c)
char c;
{
        int i;
        kprintf("Start... %c\n", c);
	if (s<4) a = 0;

        while(1) {
                for (i=0; i< 10000; i++);
                        b++;
                if (b == 300)
                        if (c == 'B') {
				a0 = a;
				b0 = b;
                                kprintf("\n\n");
                                chprio(prA, 4);
                        }
        }
}

prcha2(c)
char c;
{
        int i;
        sleep(1);
        kprintf("Start... %c\n", c);

        while(1) {
                for (i=0; i< 10000; i++);
                        a++;
        }
}

prchb2(c)
char c;
{
        int i;
        sleep(1);
        kprintf("Start... %c\n", c);

        while(1) {
                for (i=0; i< 10000; i++);
                        b++;
        }
}

prcha3(c)
char c;
{
        int i;
        kprintf("Start... %c\n", c);

        while(1) {
                for (i=0; i< 10000; i++);
                        a++;
                if (a == 300) 
                        if (c=='A') {
a0=a;
b0=b;
                                kprintf("\n\n");
                                //sfrequency(80);
                        }
        }
}

prcha4(c)
char c;
{
        int i;
        kprintf("Start... %c\n", c);
        if (c == 'A') sleep(3);
b0=b;
        while(1) {
                for (i=0; i< 10000; i++);
                        a++;
        }
}

prchb4(c)
char c;
{
        int i;
        kprintf("Start... %c\n", c);

        while(1) {
                for (i=0; i< 10000; i++);
                        b++;
		if (b>b0 && a0==0) a0=a;
        }
}
