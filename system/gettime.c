/* gettime.c - gettime */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  gettime  -  Get xinu time in seconds past Jan 1, 1970
 *------------------------------------------------------------------------
 */
status	gettime(
	  uint32  *timvar		/* Location to store the result	*/
	)
{
	uint32	now;			/* Current time (UCT)		*/
	int32	retval;			/* Return value from call	*/

	/* Get current time in UCT representation (GMT) */

	retval = getutime(&now);
	if (retval == SYSERR) {
		return SYSERR;
	}

	/* Adjust to xinu time and store result */

	*timvar = utim2ltim(now);
	return OK;
}
