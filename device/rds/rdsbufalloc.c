/* rdsbufalloc.c - rdsbufalloc */

#include <xinu.h>

/*------------------------------------------------------------------------
 * rdsbufalloc  -  Allocate a buffer from the free list or the cache
 *------------------------------------------------------------------------
 */
struct rdbuff *rdsbufalloc (
	  struct rdscblk *rdptr		/* Ptr to device control block	*/
	)
{
	struct	rdbuff	*bptr;		/* Pointer to a buffer		*/
	struct	rdbuff	*pptr;		/* Pointer to previous buffer	*/
	struct	rdbuff	*nptr;		/* Pointer to next buffer	*/

	/* Wait for an available buffer */

	wait(rdptr->rd_availsem);

	/* If free list contains a buffer, extract it */

	bptr = rdptr->rd_free;

	if ( bptr != (struct rdbuff *)NULL ) {
		rdptr->rd_free = bptr->rd_next;
		return bptr;
	}

	/* Extract oldest item in cache that has ref count zero (at	*/
	/*   least one such entry must exist because the semaphore	*/
	/*   had a nonzero count)					*/

	bptr = rdptr->rd_ctprev;
	while (bptr != (struct rdbuff *) &rdptr->rd_chnext) {
		if (bptr->rd_refcnt <= 0) {

			/* Remove from cache and return to caller */

			pptr = bptr->rd_prev;
			nptr = bptr->rd_next;
			pptr->rd_next = nptr;
			nptr->rd_prev = pptr;
			return bptr;
		}
		bptr = bptr->rd_prev;
	}
	panic("Remote disk cannot find an available buffer");
	return (struct rdbuff *)SYSERR;
}
