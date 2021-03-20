/* close.c - close */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  close  -  Close a device
 *------------------------------------------------------------------------
 */
syscall	close(
	  did32		descrp		/* Descriptor for device	*/
	)
{
	intmask		mask;		/* Saved interrupt mask		*/
	struct	dentry	*devptr;	/* Entry in device switch table	*/
	int32		retval;		/* Value to return to caller	*/

	mask = disable();
	if (isbaddev(descrp)) {
		restore(mask);
		return SYSERR;
	}
	devptr = (struct dentry *) &devtab[descrp];
	retval = (*devptr->dvclose) (devptr);
	restore(mask);
	return retval;
}
