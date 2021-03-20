/* lfibput.c - lfibput */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  lfibput  -  Write an index block to disk given its ID (assumes
 *			mutex is held)
 *------------------------------------------------------------------------
 */
status	lfibput(
	  did32		diskdev,	/* ID of disk device		*/
	  ibid32	inum,		/* ID of index block to write	*/
	  struct lfiblk	*ibuff		/* Buffer holding the index blk	*/
	)
{
	dbid32	diskblock;		/* ID of disk sector (block)	*/
	char	*from, *to;		/* Pointers used in copying	*/
	int32	i;			/* Loop index used during copy	*/
	char	dbuff[LF_BLKSIZ];	/* Temp. buffer to hold d-block	*/

	/* Compute disk block number and offset of index block */

	diskblock = ib2sect(inum);
	to = dbuff + ib2disp(inum);
	from = (char *)ibuff;

	/* Read disk block */

	if (read(diskdev, dbuff, diskblock) == SYSERR) {
		return SYSERR;
	}

	/* Copy index block into place */

	for (i=0 ; i<sizeof(struct lfiblk) ; i++) {
		*to++ = *from++;
	}

	/* Write the block back to disk */

	write(diskdev, dbuff, diskblock);
	return OK;
}
