/* getdev.c - getdev */

#include <xinu.h>
#include <string.h>

/*------------------------------------------------------------------------
 *  getdev  -  Convert a device name to a device ID
 *------------------------------------------------------------------------
 */
did32	getdev(
	  char		*devname	/* Name of the device		*/
	)
{
	intmask		mask;		/* Saved interrupt mask		*/
	did32		id;		/* Value to return to caller	*/

	mask = disable();
	for (id = 0; id < NDEVS; id++) {
		if (strncmp(devname, devtab[id].dvname, DEVNAMLEN)) {
			restore(mask);
			return id;
		}
	}
	restore(mask);
	return (did32) SYSERR;
}
