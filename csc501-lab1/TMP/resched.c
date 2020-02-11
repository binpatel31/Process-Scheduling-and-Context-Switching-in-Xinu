/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lab1.h>

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
        
	if (getschedclass()==RANDOMSCHED)
	{
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
				for (currpid=q[rdytail].qprev;currpid != rdyhead; currpid= q[currpid].qprev)
				//loop to finf next currpid
				{
					//kprintf("\ncompared pid is %d\n",q[currpid].qkey);
					if(random_number > q[currpid].qkey)
					{
						random_number = random_number - q[currpid].qkey;
//						kprintf("\nchecking for next pid\n");
					}
					else
					{
//						kprintf("\nRunning pid: %d \n",currpid);
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
		
		/* The OLD process returns here when resumed. */
		return OK;
	}
	else if (getschedclass()==LINUXSCHED)
	{
		// step 1 - update goodness and do all checks for previous process
		//================================================================
		optr = &proctab[currpid];
		int max_goodness=0;
		int max_goodness_pid=0;
               	
		//update the current process goodness at each clock tick ---not per epoch 
                optr->goodness = optr->goodness - optr->counter + preempt;
                
		//check for time exhaust
		if (currpid == NULLPROC || preempt <= 0) 
		{    
		 //this means when the null process is there or prempt = 0 means used all the allocated time.
                        optr->goodness = 0;
			optr->counter = 0;
                } 
                else
                {
        	         optr->counter = preempt;
                }
		

		// step 2 - find max goodness process to run
                //===============================================================
                
		int process = q[rdyhead].qnext;
		
		while(process != rdytail)
		{
			if (proctab[process].goodness > max_goodness)
			{
				max_goodness = proctab[process].goodness;
				max_goodness_pid = process; 
			}
			process = q[process].qnext;	
		}

		// START OF NEW EPOCH if no process is running and goodness of all=0 or allocated time exhausted 
                if(max_goodness==0)
		{
		   if(optr->pstate != PRCURR || optr->counter == 0) 
		   {    
			process=0;
			if(NPROC > 0)  //update all process goodness and counter
			{
				for(process=0;process < NPROC;++process)
				{
					if(proctab[process].pstate != PRFREE)
					{
						//for process never executed or exhausted time quantum ...then quantum is = priority for new epoch
						if(proctab[process].counter== (0 || proctab[process].quantum))
						{
							proctab[process].counter = proctab[process].pprio;
						}
						else
						{
							proctab[process].counter = proctab[process].pprio + ((proctab[process].counter)/2);
						}
						 proctab[process].quantum = proctab[process].counter;
						 proctab[process].goodness = proctab[process].pprio + proctab[process].counter;
					}
				}
			}
			int t=0;
			while(t==0)
			{
                		preempt=optr->counter;
				++t;
			}       
			
			if (max_goodness == 0)
			{
				if(currpid != NULLPROC) 
				{
                         	 	//context switch as it's time quantum is up
                                	if (optr->pstate == PRCURR) 
					{
                                       		optr->pstate = PRREADY;
                                        	insert(currpid, rdyhead, optr->pprio);
                                	}
                                	nptr = &proctab[NULLPROC];
                                	nptr->pstate = PRCURR;
                                	dequeue(NULLPROC);
                                	currpid = NULLPROC;
                                	#ifdef  RTCLOCK
                                        	preempt = QUANTUM;
                               		#endif
                                	ctxsw((int) &optr->pesp, (int) optr->pirmask, (int) &nptr->pesp, (int) nptr->pirmask);
                                	return OK;
                        	}
			}
			if(max_goodness==0)
			{
				if(currpid == NULLPROC)
				{
					return OK;
				}
			} 
		 }
                }
		
		//if current process used all it's quota or not have max goodness or is not running then switch it with max_goodness process 
		else if (optr->pstate != PRCURR || optr->counter == 0 || optr->goodness < max_goodness) 
		{
			if(max_goodness>0)
			{
				//context switch if this is the case AND USE PROCESS WITH MAX GOODNESS
                        	if (optr->pstate == PRCURR) 
				{
                                	optr->pstate = PRREADY;
                                	insert(currpid, rdyhead, optr->pprio);
                        	}
                        	nptr = &proctab[max_goodness_pid];
                        	nptr->pstate = PRCURR;
                        	currpid = max_goodness_pid;
				dequeue(max_goodness_pid);
                        	preempt = nptr->counter;
                        	ctxsw((int) &optr->pesp, (int) optr->pirmask, (int) &nptr->pesp, (int) nptr->pirmask);
                		return OK;
			}
                }

		else 
                {   
			preempt = optr->counter;
                        return OK;
                } 
	}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	else
	{	//DEFAULT XINU SCHEDULING
		if (((optr= &proctab[currpid])->pstate == PRCURR)) 
		{
			if (lastkey(rdytail)<optr->pprio)
			{
				return(OK);
			}
		}
		if (optr->pstate == PRCURR) 
		{
			optr->pstate = PRREADY;
			insert(currpid,rdyhead,optr->pprio);
		}

		nptr = &proctab[ (currpid = getlast(rdytail)) ];
		nptr->pstate = PRCURR;		/* mark it currently running	*/
		#ifdef	RTCLOCK
			preempt = QUANTUM;		/* reset preemption counter	*/
		#endif
	
		ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
	

		return OK;
	}	
	return OK;
}

