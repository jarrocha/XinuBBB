/* ptdelete.c - ptdelete */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  ptdelete  -  Delete a port, freeing waiting processes and messages
 *------------------------------------------------------------------------
 */
syscall	ptdelete(
	  int32		portid,		/* ID of port to delete		*/
	  int32		(*disp)(int32)	/* Function to call to dispose	*/
	)				/*    of waiting messages	*/
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	ptentry	*ptptr;		/* Pointer to port table entry	*/

	mask = disable();
	if ( isbadport(portid) ||
	     (ptptr= &porttab[portid])->ptstate != PT_ALLOC ) {
		restore(mask);
		return SYSERR;
	}
	_ptclear(ptptr, PT_FREE, disp);
	ptnextid = portid;
	restore(mask);
	return OK;
}
