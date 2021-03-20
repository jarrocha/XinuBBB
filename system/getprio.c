/* getprio.c - getprio */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  getprio  -  Return the scheduling priority of a process
 *------------------------------------------------------------------------
 */
syscall	getprio(
	  pid32		pid		/* Process ID			*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	uint32	prio;			/* Priority to return		*/

	mask = disable();
	if (isbadpid(pid)) {
		restore(mask);
		return SYSERR;
	}
	prio = proctab[pid].prprio;
	restore(mask);
	return prio;
}
