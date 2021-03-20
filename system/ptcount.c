/* ptcount.c - ptcount */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  ptcount  -  Return the count of messages currently waiting in a port
 *		  (a non-negative count K means the port contains K
 *		   K messages,  including messages from senders that
 *		   are blocked waiting to send a message; a count of
 *		   negative K means a port is empty and K processes
 *		   are blocked waiting to receive a message)
 *------------------------------------------------------------------------
 */
int32	ptcount(
	  int32		portid		/* ID of a port to use		*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	int32	count;			/* Count of messages available	*/
	int32	sndcnt;			/* Count of sender semaphore	*/
	struct	ptentry	*ptptr;		/* Pointer to port table entry	*/

	mask = disable();
	if ( isbadport(portid) ||
		(ptptr= &porttab[portid])->ptstate != PT_ALLOC ) {
			restore(mask);
			return SYSERR;
	}

	/* Get count of messages available */

	count = semcount(ptptr->ptrsem);

	/* If messages are waiting, check for blocked senders */

	if (count >= 0) {
		sndcnt = semcount(ptptr->ptssem);
		if (sndcnt < 0) {	/* -sndcnt senders blocked */
			count += -sndcnt;
		}
	}
	restore(mask);
	return count;
}
