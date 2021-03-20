/* ramopen.c  -  ramopen */

#include <xinu.h>

/*------------------------------------------------------------------------
 * ramopen  -  Open a ram disk
 *------------------------------------------------------------------------
 */

devcall	ramopen (
	 struct	dentry	*devptr,	/* Entry in device switch table	*/
	 char	*name,			/* Unused for a ram disk	*/
	 char	*mode			/* Unused for a ram disk	*/
	)
{
	/* No action -- just return the device descriptor */

	return devptr->dvnum;
}
