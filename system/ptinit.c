/* ptinit.c - ptinit */

#include <xinu.h>

struct	ptnode	*ptfree;		/* List of free message nodes	*/
struct	ptentry	porttab[NPORTS];	/* Port table			*/
int32	ptnextid;			/* Next table entry to try	*/

/*------------------------------------------------------------------------
 *  ptinit  -  Initialize all ports
 *------------------------------------------------------------------------
 */
syscall	ptinit(
	  int32	maxmsgs			/* Total messages in all ports	*/
	)
{
	int32	i;			/* Runs through the port table	*/
	struct	ptnode	*next, *curr;	/* Used to build a free list	*/

	/* Allocate memory for all messages on all ports */

	ptfree = (struct ptnode *)getmem(maxmsgs*sizeof(struct ptnode));
	if (ptfree == (struct ptnode *)SYSERR) {
		panic("ptinit - insufficient memory");
	}

	/* Initialize all port table entries to free */

	for (i=0 ; i<NPORTS ; i++) {
		porttab[i].ptstate = PT_FREE;
		porttab[i].ptseq = 0;
	}
	ptnextid = 0;

	/* Create a free list of message nodes linked together */

	for ( curr=next=ptfree ;  --maxmsgs > 0  ; curr=next ) {
		curr->ptnext = ++next;
	}

	/* Set the pointer in the final node to NULL */

	curr->ptnext = NULL;
	return OK;
}
