/* lflcontrol.c - lflcontrol */

#include <xinu.h>

/*------------------------------------------------------------------------
 * lflcontrol  -  Provide control functions for a local file pseudo-device
 *------------------------------------------------------------------------
 */
devcall	lflcontrol (
	 struct dentry	*devptr,	/* Entry in device switch table	*/
	 int32	func,			/* A control function		*/
	 int32	arg1,			/* Argument #1			*/
	 int32	arg2			/* Argument #2			*/
	)
{
	struct	lflcblk	*lfptr;		/* Ptr to open file table entry	*/
	int32	retval;			/* Return value from func. call	*/

	/* Obtain exclusive use of the file */

	lfptr = &lfltab[devptr->dvminor];
	wait(lfptr->lfmutex);

	/* If file is not open, return an error */

	if (lfptr->lfstate != LF_USED) {
		signal(lfptr->lfmutex);
		return SYSERR;
	}

	switch (func) {

	/* Truncate a file */

	case LF_CTL_TRUNC:
		wait(Lf_data.lf_mutex);
		retval = lftruncate(lfptr);
		signal(Lf_data.lf_mutex);
		signal(lfptr->lfmutex);
		return retval;	

	default:
		kprintf("lfcontrol: function %d not valid\n\r", func);
		signal(lfptr->lfmutex);
		return SYSERR;
	}
}
