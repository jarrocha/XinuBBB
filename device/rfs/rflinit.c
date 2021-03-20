/* rflinit.c - rflinit */

#include <xinu.h>

struct	rflcblk	rfltab[Nrfl];		/* Remote file control blocks	*/

/*------------------------------------------------------------------------
 *  rflinit  -  Initialize a remote file device
 *------------------------------------------------------------------------
 */
devcall	rflinit(
	  struct dentry	*devptr		/* Entry in device switch table	*/
	)
{
	struct	rflcblk	*rflptr;	/* Ptr. to control block entry	*/
	int32	i;			/* Walks through name arrary	*/

	rflptr = &rfltab[ devptr->dvminor ];

	/* Initialize entry to unused */

	rflptr->rfstate = RF_FREE;
	rflptr->rfdev = devptr->dvnum;
	for (i=0; i<RF_NAMLEN; i++) {
		rflptr->rfname[i] = NULLCH;
	}
	rflptr->rfpos = rflptr->rfmode = 0;
	return OK;
}
