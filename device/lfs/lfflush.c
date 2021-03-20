/* lfflush.c - lfflush */

#include <xinu.h>

/*------------------------------------------------------------------------
 * lfflush  -  Flush directory, data block, and index block for an open
 *			file (assumes file mutex is held)
 *------------------------------------------------------------------------
 */
status	lfflush (
	  struct lflcblk  *lfptr	/* Ptr to file pseudo device	*/
	)
{

	if (lfptr->lfstate == LF_FREE) {
		return SYSERR;
	}

	/* Write the directory if it has changed */

	if (Lf_data.lf_dirdirty) {
		write(Lf_data.lf_dskdev, (char *)&Lf_data.lf_dir,
							LF_AREA_DIR);
		Lf_data.lf_dirdirty = FALSE;
	}

	/* Write data block if it has changed */

	if (lfptr->lfdbdirty) {
		write(Lf_data.lf_dskdev, lfptr->lfdblock, lfptr->lfdnum);
		lfptr->lfdbdirty = FALSE;
	}

	/* Write i-block if it has changed */

	if (lfptr->lfibdirty) {
		lfibput(Lf_data.lf_dskdev, lfptr->lfinum, &lfptr->lfiblock);
		lfptr->lfibdirty = FALSE;
	}
	
	return OK;
}
