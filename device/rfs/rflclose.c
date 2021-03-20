/* rflclose.c - rflclose */

#include <xinu.h>

/*------------------------------------------------------------------------
 * rflclose  -  Close a remote file device
 *------------------------------------------------------------------------
 */
devcall	rflclose (
	  struct dentry	*devptr		/* Entry in device switch table	*/
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

	/* Mark device closed */

	rfptr->rfstate = RF_FREE;
	signal(Rf_data.rf_mutex);
	return OK;
}
