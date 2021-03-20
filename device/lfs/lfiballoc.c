/* lfiballoc.c - lfiballoc */

#include <xinu.h>

/*------------------------------------------------------------------------
 * lfiballoc  -  Allocate a new index block from free list on disk
 *			(assumes directory mutex held)
 *------------------------------------------------------------------------
 */
ibid32	lfiballoc (void)
{
	ibid32	ibnum;		/* ID of next block on the free list	*/
	struct	lfiblk	iblock;	/* Buffer to hold an index block	*/

	/* Get ID of first index block on free list */

	ibnum = Lf_data.lf_dir.lfd_ifree;
	if (ibnum == LF_INULL) {	/* Ran out of free index blocks */
		panic("out of index blocks");
	}
	lfibget(Lf_data.lf_dskdev, ibnum, &iblock);

	/* Unlink index block from the directory free list */

	Lf_data.lf_dir.lfd_ifree = iblock.ib_next;

	/* Write a copy of the directory to disk after the change */

	write(Lf_data.lf_dskdev, (char *) &Lf_data.lf_dir, LF_AREA_DIR);
	Lf_data.lf_dirdirty = FALSE;

	return ibnum;
}
