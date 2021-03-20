/* init.c - init */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  init  -  Initialize a device and its driver
 *------------------------------------------------------------------------
 */
syscall	init(
	  did32		descrp		/* Descriptor for device	*/
	)
{
	intmask		mask;		/* Saved interrupt mask		*/
	struct dentry	*devptr;	/* Entry in device switch table	*/
	int32		retval;		/* Value to return to caller	*/

	mask = disable();
	if (isbaddev(descrp)) {
		restore(mask);
		return SYSERR;
	}
	devptr = (struct dentry *) &devtab[descrp];
	retval = (*devptr->dvinit) (devptr);
	restore(mask);
	return retval;
}
