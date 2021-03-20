/* ptreset.c - ptreset */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  ptreset  -  Reset a port, freeing waiting processes and messages and
			leaving the port ready for further use
 *------------------------------------------------------------------------
 */
syscall	ptreset(
	  int32		portid,		/* ID of port to reset		*/
	  int32		(*disp)(int32)	/* Function to call to dispose	*/
	)				/*   of waiting messages	*/
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	ptentry	*ptptr;		/* Pointer to port table entry	*/

	mask = disable();
	if ( isbadport(portid) ||
	     (ptptr= &porttab[portid])->ptstate != PT_ALLOC ) {
		restore(mask);
		return SYSERR;
	}
	_ptclear(ptptr, PT_ALLOC, disp);
	restore(mask);
	return OK;
}
