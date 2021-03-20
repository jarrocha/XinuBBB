/* control.c - control */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  control  -  Control a device or a driver (e.g., set the driver mode)
 *------------------------------------------------------------------------
 */
syscall	control(
	  did32		descrp,		/* Descriptor for device	*/
	  int32		func,		/* Specific control function	*/
	  int32		arg1,		/* Specific argument for func	*/
	  int32		arg2		/* Specific argument for func	*/
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
	retval = (*devptr->dvcntl) (devptr, func, arg1, arg2);
	restore(mask);
	return retval;
}
