/* rdsread.c - rdsread */

#include <xinu.h>

/*------------------------------------------------------------------------
 * rdsread  -  Read a block from a remote disk
 *------------------------------------------------------------------------
 */
devcall	rdsread (
	  struct dentry	*devptr,	/* Entry in device switch table	*/
	  char	*buff,			/* Buffer to hold disk block	*/
	  int32	blk			/* Block number of block to read*/
	)
{
	struct	rdscblk	*rdptr;		/* Pointer to control block	*/
	struct	rdbuff	*bptr;		/* Pointer to buffer possibly	*/
					/*   in the request list	*/
	struct	rdbuff	*nptr;		/* Pointer to "next" node on a	*/
					/*   list			*/
	struct	rdbuff	*pptr;		/* Pointer to "previous" node	*/
					/*   on a list			*/
	struct	rdbuff	*cptr;		/* Pointer that walks the cache	*/

	/* If device not currently in use, report an error */

	rdptr = &rdstab[devptr->dvminor];
	if (rdptr->rd_state != RD_OPEN) {
		return SYSERR;
	}

	/* Ensure rdsprocess is runnning */

	if ( ! rdptr->rd_comruns ) {
		rdptr->rd_comruns = TRUE;
		resume(rdptr->rd_comproc);
	}

	/* Search the cache for specified block */

	bptr = rdptr->rd_chnext;
	while (bptr != (struct rdbuff *)&rdptr->rd_ctnext) {
		if (bptr->rd_blknum == blk) {
			if (bptr->rd_status == RD_INVALID) {
				break;
			}
			memcpy(buff, bptr->rd_block, RD_BLKSIZ);
			return OK;
		}
		bptr = bptr->rd_next;
	}

	/* Search the request list for most recent occurrence of block */

	bptr = rdptr->rd_rtprev;  /* Start at tail of list */

	while (bptr != (struct rdbuff *)&rdptr->rd_rhnext) {
	    if (bptr->rd_blknum == blk)  {

		/* If most recent request for block is write, copy data */

		if (bptr->rd_op == RD_OP_WRITE) {
			memcpy(buff, bptr->rd_block, RD_BLKSIZ);
			return OK;
		}
		break;
	    }
	    bptr = bptr->rd_prev;
	}

	/* Allocate a buffer and add read request to tail of req. queue */

	bptr = rdsbufalloc(rdptr);	
	bptr->rd_op = RD_OP_READ;
	bptr->rd_refcnt = 1;
	bptr->rd_blknum = blk;
	bptr->rd_status = RD_INVALID;
	bptr->rd_pid = getpid();

	/* Insert new request into list just before tail */

	pptr = rdptr->rd_rtprev;
	rdptr->rd_rtprev = bptr;
	bptr->rd_next = pptr->rd_next;
	bptr->rd_prev = pptr;
	pptr->rd_next = bptr;

	/* Prepare to receive message when read completes */

	recvclr();

	/* Signal the semaphore to start communication */

	signal(rdptr->rd_reqsem);

	/* Block to wait for a message */

	bptr = (struct rdbuff *)receive();
	if (bptr == (struct rdbuff *)SYSERR) {	
		return SYSERR;
	}
	memcpy(buff, bptr->rd_block, RD_BLKSIZ);
	bptr->rd_refcnt--;
	if (bptr->rd_refcnt <= 0) {

		/* Look for previous item in cache with the same block	*/
		/*    number to see if this item was only being kept	*/
		/*    until pending read completed			*/
				
		cptr = rdptr->rd_chnext;
		while (cptr != bptr) {
			if (cptr->rd_blknum == blk) {

				/* Unlink from cache */

				pptr = bptr->rd_prev;
				nptr = bptr->rd_next;
				pptr->rd_next = nptr;
				nptr->rd_prev = pptr;

				/* Add to the free list */

				bptr->rd_next = rdptr->rd_free;
				rdptr->rd_free = bptr;
				break;
			}
			cptr = cptr->rd_next;
		}
	}
	return OK;
}
