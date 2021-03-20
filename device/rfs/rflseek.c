/* rflseek.c - rflseek */

#include <xinu.h>

/*------------------------------------------------------------------------
 * rflseek  -  Change the current position in an open file
 *------------------------------------------------------------------------
 */
devcall	rflseek (
	  struct dentry	*devptr,	/* Entry in device switch table	*/
	  uint32 pos			/* New file position		*/
	)
{
	struct	rflcblk	*rfptr;		/* Pointer to control block	*/

	/* Wait for exclusive access */

	wait(Rf_data.rf_mutex);

	/* Verify remote file device is open */

	rfptr = &rfltab[devptr->dvminor];
	if (rfptr->rfstate == RF_FREE) {
		signal(Rf_data.rf_mutex);
		return SYSERR;
	}

	/* Set the new position */

	rfptr->rfpos = pos;
	signal(Rf_data.rf_mutex);
	return OK;
}
