/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>

unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);
/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */
int resched()
{
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */
        int sum=0;
	
	//step1 : save current process as ready and run scheduling algorithm
	optr =  &proctab[currpid];
	if (optr->pstate == PRCURR) 
	{
		optr->pstate = PRREADY;
		insert(currpid,rdyhead,optr->pprio);
	}
	
	//step 2 - finding sum of all priorities==============
	
 	int node = q[rdyhead].qnext;
		
	while(node != rdytail)
	{
		sum=sum+q[node].qkey;
		node=q[node].qnext;
	}
	
	int random_number;
	if(sum>0)
	{
		random_number = rand()%sum;
	}
	
	//step 3 - find process according random_scheduler_algorithm
	currpid=0;
	if(sum>0)
	{
		currpid = q[rdytail].qprev;
		
		if (currpid == rdyhead)
		{
			currpid = 0;
		}
		else
		{
			while(currpid != rdyhead)   //currpid
			{
			
				//kprintf("\ncompared pid is %d\n",q[currpid].qkey);
				if(random_number > q[currpid].qkey)
				{
					random_number = random_number - q[currpid].qkey;
					currpid= q[currpid].qprev;
//					kprintf("\nchecking for next pid\n");
				}
				else
				{
//					kprintf("\nRunning pid: %d \n",currpid);
					break;
				}
				
			}
			if(currpid == rdyhead)
			{
				currpid=q[currpid].qnext;
			}	
		}
	}
	//=================================================


	//step-4 set currpid as running process
	dequeue(currpid);
	nptr = &proctab[currpid];

	nptr->pstate = PRCURR;		/* mark it currently running	*/
#ifdef	RTCLOCK
	preempt = QUANTUM;		/* reset preemption counter	*/
#endif
	
	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
	/*
	/* The OLD process returns here when resumed. */
	return OK;


}
