/* lftruncate.c - lftruncate */

#include <xinu.h>

/*------------------------------------------------------------------------
 * lftruncate  -  Truncate a file by freeing its index and data blocks
 *			(assumes directory mutex held)
 *------------------------------------------------------------------------
 */
status	lftruncate (
	  struct lflcblk *lfptr		/* Ptr to file's cntl blk entry	*/
	)
{
	struct	ldentry	*ldptr;		/* Pointer to file's dir. entry	*/
	struct	lfiblk	iblock;		/* Buffer for one index block	*/
	ibid32	ifree;			/* Start of index blk free list	*/
	ibid32	firstib;		/* First index blk of the file	*/
	ibid32	nextib;			/* Walks down list of the	*/
					/*   file's index blocks	*/
	dbid32	nextdb;			/* Next data block to free	*/
	int32	i;			/* Moves through data blocks in	*/
					/*   a given index block	*/

	ldptr = lfptr->lfdirptr;	/* Get pointer to dir. entry	*/
	if (ldptr->ld_size == 0) {	/* File is already empty */
		return OK;
	}

	/* Clean up the open local file first */

	if ( (lfptr->lfibdirty) || (lfptr->lfdbdirty) ) {
		lfflush(lfptr);
	}
	lfptr->lfpos = 0;
	lfptr->lfinum = LF_INULL;
	lfptr->lfdnum = LF_DNULL;
	lfptr->lfbyte = &lfptr->lfdblock[LF_BLKSIZ];

	/* Obtain ID of first index block on free list */

	ifree = Lf_data.lf_dir.lfd_ifree;

	/* Record file's first i-block and clear directory entry */

	firstib = ldptr->ld_ilist;
	ldptr->ld_ilist = LF_INULL;
	ldptr->ld_size = 0;
	Lf_data.lf_dirdirty = TRUE;

	/* Walk along index block list, disposing of each data block	*/
	/*   and clearing the corresponding pointer.  A note on loop	*/
	/*   termination: last pointer is set to ifree below.		*/

	for (nextib=firstib; nextib!=ifree; nextib=iblock.ib_next) {

		/* Obtain a copy of current index block from disk	*/

		lfibget(Lf_data.lf_dskdev, nextib, &iblock);

		/* Free each data block in the index block		*/

		for (i=0; i<LF_IBLEN; i++) {	/* For each d-block	*/

			/* Free the data block */

			nextdb = iblock.ib_dba[i];
			if (nextdb != LF_DNULL) {
				lfdbfree(Lf_data.lf_dskdev, nextdb);
			}

			/* Clear entry in i-block for this d-block	*/

				iblock.ib_dba[i] = LF_DNULL;
		}

		/* Clear offset (just to make debugging easier)		*/

		iblock.ib_offset = 0;

		/* For the last index block on the list, make it point	*/
		/*	to the current free list			*/

		if (iblock.ib_next == LF_INULL) {
			iblock.ib_next = ifree;
		}

		/* Write cleared i-block back to disk */

		lfibput(Lf_data.lf_dskdev, nextib, &iblock);
	}

	/* Last index block on the file list now points to first node	*/
	/*   on the current free list.  Once we make the free list	*/
	/*   point to the first index block on the file list, the	*/
	/*   entire set of index blocks will be on the free list	*/

	Lf_data.lf_dir.lfd_ifree = firstib;

	/* Indicate that directory has changed and return */

	Lf_data.lf_dirdirty = TRUE;
	
	return OK;
}
