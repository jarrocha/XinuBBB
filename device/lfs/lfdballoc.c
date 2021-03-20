/* lfdballoc.c - lfdballoc */

#include <xinu.h>

#define  DFILL  '+'		/* character used to fill a disk block	*/

/*------------------------------------------------------------------------
 * lfdballoc  -  Allocate a new data block from free list on disk
 *			(assumes directory mutex held)
 *------------------------------------------------------------------------
 */
dbid32	lfdballoc (
	  struct lfdbfree *dbuff /* Addr. of buffer to hold data block	*/
	)
{
	dbid32	dnum;		/* ID of next d-block on the free list	*/
	int32	retval;		/* Return value				*/

	/* Get the ID of first data block on the free list */

	dnum = Lf_data.lf_dir.lfd_dfree;
	if (dnum == LF_DNULL) {	/* Ran out of free data blocks */
		panic("out of data blocks");
	}
	retval = read(Lf_data.lf_dskdev, (char *)dbuff, dnum);
	if (retval == SYSERR) {
		panic("lfdballoc cannot read disk block\n\r");
	}

	/* Unlink d-block from in-memory directory */

	Lf_data.lf_dir.lfd_dfree = dbuff->lf_nextdb;
	write(Lf_data.lf_dskdev, (char *)&Lf_data.lf_dir, LF_AREA_DIR);
	Lf_data.lf_dirdirty = FALSE;

	/* Fill data block to erase old data */

	memset((char *)dbuff, DFILL, LF_BLKSIZ);
	return dnum;
}
