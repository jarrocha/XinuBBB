/* mark.c - markinit, mark */

#include <xinu.h>

int32	*marks[MAXMARK];		/* Pointers to marked locations	*/
int32	nmarks;				/* Number of marked locations	*/
sid32	mkmutex;			/* Mutual exclusion semaphore	*/

/*------------------------------------------------------------------------
 *  markinit  -  Called once at system startup
 *------------------------------------------------------------------------
 */
void	markinit(void)
{
	nmarks = 0;
	mkmutex = semcreate(1);
}


/*------------------------------------------------------------------------
 *  mark  -  Mark a specified memory location
 *------------------------------------------------------------------------
 */
status	mark(
	  int32	*loc			/* Location to mark		*/
	)
{

	/* If location is already marked, do nothing */

	if ( (*loc>=0) && (*loc<nmarks) && (marks[*loc]==loc) ) {
		return OK;
	}

	/* If no more memory marks are available, indicate an error */

	if (nmarks >= MAXMARK) {
		return SYSERR;
	}

	/* Obtain exclusive access and mark the specified location */

	wait(mkmutex);
	marks[ (*loc) = nmarks++ ] = loc;
	signal(mkmutex);
	return OK;
}
