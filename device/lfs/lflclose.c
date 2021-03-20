/* lflclose.c - lflclose.c */

#include <xinu.h>

/*------------------------------------------------------------------------
 * lflclose  -  Close a file by flushing output and freeing device entry
 *------------------------------------------------------------------------
 */
devcall	lflclose (
	  struct dentry *devptr		/* Entry in device switch table */
	)
{
	struct	lflcblk	*lfptr;		/* Ptr to open file table entry	*/

	/* Obtain exclusive use of the file */

	lfptr = &lfltab[devptr->dvminor];
	wait(lfptr->lfmutex);

	/* If file is not open, return an error */

	if (lfptr->lfstate != LF_USED) {
		signal(lfptr->lfmutex);
		return SYSERR;
	}

	/* Write index or data blocks to disk if they have changed */

	if (Lf_data.lf_dirdirty || lfptr->lfdbdirty || lfptr->lfibdirty) {
		lfflush(lfptr);
	}

	/* Set device state to FREE and return to caller */

	lfptr->lfstate = LF_FREE;
	signal(lfptr->lfmutex);
	return OK;
}
