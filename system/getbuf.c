/* getbuf.c - getbuf */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  getbuf  -  Get a buffer from a preestablished buffer pool
 *------------------------------------------------------------------------
 */
char    *getbuf(
          bpid32        poolid          /* Index of pool in buftab	*/
        )
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	bpentry	*bpptr;		/* Pointer to entry in buftab	*/
	struct	bpentry	*bufptr;	/* Pointer to a buffer		*/

	mask = disable();

	/* Check arguments */

	if ( (poolid < 0  ||  poolid >= nbpools) ) {
		restore(mask);
		return (char *)SYSERR;

	}
	bpptr = &buftab[poolid];

	/* Wait for pool to have > 0 buffers and allocate a buffer */

	wait(bpptr->bpsem);
	bufptr = bpptr->bpnext;

	/* Unlink buffer from pool */

	bpptr->bpnext = bufptr->bpnext;

	/* Record pool ID in first four bytes of buffer	and skip */

	*(bpid32 *)bufptr = poolid;
	bufptr = (struct bpentry *)(sizeof(bpid32) + (char *)bufptr);
	restore(mask);
	return (char *)bufptr;
}
