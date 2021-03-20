/* ptsend.c - ptsend */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  ptsend  -  Send a message to a port by adding it to the queue
 *------------------------------------------------------------------------
 */
syscall	ptsend(
	  int32		portid,		/* ID of port to use		*/
	  umsg32	msg		/* Message to send		*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	ptentry	*ptptr;		/* Pointer to table entry	*/
	int32	seq;			/* Local copy of sequence num.	*/
	struct	ptnode	*msgnode;	/* Allocated message node 	*/
	struct	ptnode	*tailnode;	/* Last node in port or NULL	*/

	mask = disable();
	if ( isbadport(portid) ||
	     (ptptr= &porttab[portid])->ptstate != PT_ALLOC ) {
		restore(mask);
		return SYSERR;
	}

	/* Wait for space and verify port has not been reset */

	seq = ptptr->ptseq;		/* Record original sequence	*/
	if (wait(ptptr->ptssem) == SYSERR
	    || ptptr->ptstate != PT_ALLOC
	    || ptptr->ptseq != seq) {
		restore(mask);
		return SYSERR;
	}
	if (ptfree == NULL) {
		panic("Port system ran out of message nodes");
	}

	/* Obtain node from free list by unlinking */

	msgnode = ptfree;		/* Point to first free node	*/
	ptfree  = msgnode->ptnext;	/* Unlink from the free list	*/
	msgnode->ptnext = NULL;		/* Set fields in the node	*/
	msgnode->ptmsg  = msg;

	/* Link into queue for the specified port */

	tailnode = ptptr->pttail;
	if (tailnode == NULL) {		/* Queue for port was empty	*/
		ptptr->pttail = ptptr->pthead = msgnode;
	} else {			/* Insert new node at tail	*/
		tailnode->ptnext = msgnode;
		ptptr->pttail = msgnode;
	}
	signal(ptptr->ptrsem);
	restore(mask);
	return OK;
}
