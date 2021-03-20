/* ptrecv.c - ptrecv */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  ptrecv  -  Receive a message from a port, blocking if port empty
 *------------------------------------------------------------------------
 */
uint32	ptrecv(
	  int32		portid		/* ID of port to use		*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	ptentry	*ptptr;		/* Pointer to table entry	*/
	int32	seq;			/* Local copy of sequence num.	*/
	umsg32	msg;			/* Message to return		*/
	struct	ptnode	*msgnode;	/* First node on message list	*/

	mask = disable();
	if ( isbadport(portid) ||
	     (ptptr= &porttab[portid])->ptstate != PT_ALLOC ) {
		restore(mask);
		return (uint32)SYSERR;
	}

	/* Wait for message and verify that the port is still allocated */

	seq = ptptr->ptseq;		/* Record orignal sequence	*/
	if (wait(ptptr->ptrsem) == SYSERR || ptptr->ptstate != PT_ALLOC
	    || ptptr->ptseq != seq) {
		restore(mask);
		return (uint32)SYSERR;
	}

	/* Dequeue first message that is waiting in the port */

	msgnode = ptptr->pthead;
	msg = msgnode->ptmsg;
	if (ptptr->pthead == ptptr->pttail)	/* Delete last item	*/
		ptptr->pthead = ptptr->pttail = NULL;
	else
		ptptr->pthead = msgnode->ptnext;
	msgnode->ptnext = ptfree;		/* Return to free list	*/
	ptfree = msgnode;
	signal(ptptr->ptssem);
	restore(mask);
	return msg;
}
