/* getc.c - getc */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  getc  -  Obtain one byte from a device
 *------------------------------------------------------------------------
 */
syscall	getc(
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
	retval = (*devptr->dvgetc) (devptr);
	restore(mask);
	return retval;
}
