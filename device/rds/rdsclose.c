/* rdsclose.c - rdsclose */

#include <xinu.h>

/*------------------------------------------------------------------------
 * rdsclose  -  Close a remote disk device
 *------------------------------------------------------------------------
 */
devcall	rdsclose (
	  struct dentry	*devptr		/* Entry in device switch table	*/
	)
{
	struct	rdscblk	*rdptr;		/* Ptr to control block entry	*/
	struct	rdbuff	*bptr;		/* Ptr to buffer on a list	*/
	struct	rdbuff	*nptr;		/* Ptr to next buff on the list	*/
	int32	nmoved;			/* Number of buffers moved	*/

	/* Device must be open */

	rdptr = &rdstab[devptr->dvminor];
	if (rdptr->rd_state != RD_OPEN) {
		return SYSERR;
	}

	/* Request queue must be empty */

	if (rdptr->rd_rhnext != (struct rdbuff *)&rdptr->rd_rtnext) {
		return SYSERR;
	}

	/* Move all buffers from the cache to the free list */

	bptr = rdptr->rd_chnext;
	nmoved = 0;
	while (bptr != (struct rdbuff *)&rdptr->rd_ctnext) {
		nmoved++;

		/* Unlink buffer from cache */

		nptr = bptr->rd_next;
		(bptr->rd_prev)->rd_next = nptr;
		nptr->rd_prev = bptr->rd_prev;

		/* Insert buffer into free list */

		bptr->rd_next = rdptr->rd_free;
	
		rdptr->rd_free = bptr;
		bptr->rd_status = RD_INVALID;

		/* Move to next buffer in the cache */

		bptr = nptr;
	}

	/* Set the state to indicate the device is closed */

	rdptr->rd_state = RD_FREE;
	return OK;
}
