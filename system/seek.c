/* seek.c - seek */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  seek  -  Position a random access device
 *------------------------------------------------------------------------
 */
syscall	seek(
	  did32		descrp,		/* Descriptor for device	*/
	  uint32	pos		/* Position			*/
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
	retval = (*devptr->dvseek) (devptr, pos);
	restore(mask);
	return retval;
}
