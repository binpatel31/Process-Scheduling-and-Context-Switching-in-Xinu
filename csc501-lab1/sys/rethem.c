/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
//#include <math.h>
#include <sched.h>


unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);
extern int a_cnt, b_cnt, c_cnt;
int new_prio_rand, proc_id_next;

/*-----------------------------------------------------------------------
 *  * resched  --  reschedule processor to highest priority ready process
 *   *
 *    * Notes:	Upon entry, currpid gives current process id.
 *     *		Proctab[currpid].pstate gives correct NEXT state for
 *      *			current process if other than PRREADY.
 *       *------------------------------------------------------------------------
 *        */
int resched()
{
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */

	optr = &proctab[currpid];
	/* no switch needed if current process priority higher than next*/
	/* next process chosen based on scheduler policy		*/

    if(scheduler == PROPORTIONALSHARE){
		int max, next;
		getMaxProcess(&next, &max);

		optr->goodness -= (optr->counter - preempt);
		if (currpid == NULLPROC || preempt == 0) {     //this means when the null process is there or prempt = 0 means used all the allocated time. 
			optr->goodness = 0;
		}
		
		if(currpid == NULLPROC)	
			optr->counter = 0;     
		else 
			optr->counter = preempt;

		if((optr->pstate != PRCURR || optr->counter == 0) && max == 0){
			updateGoodness();                                        //case when no process can run more 
			
			if (max == 0) {
				if (currpid == NULLPROC) {
					return OK;
				} else {
					if (optr->pstate == PRCURR) {
						optr->pstate = PRREADY;
						insert(currpid, rdyhead, optr->pprio);
					}
					nptr = &proctab[NULLPROC];
					nptr->pstate = PRCURR;
					dequeue(NULLPROC);
					currpid = NULLPROC;
			
					#ifdef	RTCLOCK
						preempt = QUANTUM;
					#endif
					ctxsw((int) &optr->pesp, (int) optr->pirmask, (int) &nptr->pesp, (int) nptr->pirmask);
					return OK;
				}
			}
		} else if (optr->goodness > 0 && optr->goodness >= max && optr->pstate == PRCURR) {
			return OK;
		}else if (max > 0 && (optr->pstate != PRCURR || optr->counter == 0 || optr->goodness < max)) {
			if (optr->pstate == PRCURR) {
				optr->pstate = PRREADY;
				insert(currpid, rdyhead, optr->pprio);
			}
			
			nptr = &proctab[next];
			nptr->pstate = PRCURR;
			dequeue(next);
			currpid = next;
			preempt = nptr->counter;
		}

    } else if(scheduler == 3){
		if(optr->pstate == PRCURR){
			optr->pstate = PRREADY;
			insert(currpid, rdyhead, optr->pprio);
		}
		new_prio_rand = (int) expdev(0.1);
		proc_id_next = get_next_process_exp(new_prio_rand);

		if(proc_id_next == EMPTY)
			currpid = NULLPROC;
		else 
			currpid = dequeue(proc_id_next);
		
		nptr = &proctab[currpid];

		nptr->pstate = PRCURR;
		#ifdef RTCLOCK	
			preempt = QUANTUM;
		#endif
	} else {
		if ( (optr->pstate == PRCURR) &&
				(lastkey(rdytail)<optr->pprio)) {
				return(OK);
		}
			
		/* force context switch */
		
		if (optr->pstate == PRCURR) {
			optr->pstate = PRREADY;
			insert(currpid,rdyhead,optr->pprio);
		}
		
		/* remove highest priority process at end of ready list */
		
		nptr = &proctab[ (currpid = getlast(rdytail)) ];
		nptr->pstate = PRCURR;		/* mark it currently running	*/
		#ifdef	RTCLOCK
			preempt = QUANTUM;		/* reset preemption counter	*/
		#endif
		
	}

	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);

	/* The OLD process returns here when resumed. */
	return OK;
}

void getMaxProcess(int *next, int *max){
	*next = 0;
	*max = 0;
	
	int iter = q[rdytail].qprev;
	while(iter != rdyhead){
		if(proctab[iter].goodness > *max){
			*next = iter;
			*max = proctab[iter].goodness;
		}
		iter = q[iter].qprev;
	}
}

void updateGoodness(){
	int temp_process_id;
	struct pentry *temp_process;

	if(NPROC > 0){
		do{
			temp_process = &proctab[temp_process_id];
			if(temp_process->pstate != PRFREE){
				temp_process->counter = temp_process->pprio + (temp_process->counter)/2;
				temp_process->goodness = temp_process->pprio + temp_process->counter;
			}
			temp_process_id++;
		}while(temp_process_id < NPROC);
	}
}
