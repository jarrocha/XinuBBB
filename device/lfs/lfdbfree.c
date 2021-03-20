/* lfdbfree.c - lfdbfree */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  lfdbfree  -  Free a data block given its block number (assumes
 *			directory mutex is held)
 *------------------------------------------------------------------------
 */
status	lfdbfree(
	  did32		diskdev,	/* ID of disk device to use	*/
	  dbid32	dnum		/* ID of data block to free	*/
	)
{
	struct	lfdir	*dirptr;	/* Pointer to directory		*/
	struct	lfdbfree buf;		/* Buffer to hold data block	*/

	dirptr = &Lf_data.lf_dir;
	buf.lf_nextdb = dirptr->lfd_dfree;
	dirptr->lfd_dfree = dnum;
	write(diskdev, (char *)&buf,   dnum);
	write(diskdev, (char *)dirptr, LF_AREA_DIR);

	return OK;
}
