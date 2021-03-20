/* mount.c - mount, namlen */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  mount  -  Add a prefix mapping to the name space
 *------------------------------------------------------------------------
 */
syscall	mount(
	  char		*prefix,	/* Prefix to add		*/
	  char		*replace,	/* Replacement string		*/
	  did32		device		/* Device ID to use		*/
)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	nmentry	*namptr;	/* Pointer to unused table entry*/
	int32	psiz, rsiz;		/* Sizes of prefix & replacement*/
	int32	i;			/* Counter for copy loop	*/

        mask = disable();

	psiz = namlen(prefix, NM_PRELEN);
	rsiz = namlen(replace, NM_REPLLEN);

	/* If arguments are invalid or table is full, return error */

	if ( (psiz == SYSERR)   || (rsiz == SYSERR) ||
	     (isbaddev(device)) || (nnames >= NNAMES) ) {
		restore(mask);
		return SYSERR;
	}

	/* Allocate a slot in the table */

	namptr = &nametab[nnames];	/* Next unused entry in table	*/

	/* copy prefix and replacement strings and record device ID */
	
	for (i=0; i<psiz; i++) {	/* Copy prefix into table entry	*/
		namptr->nprefix[i] = *prefix++;
	}

	for (i=0; i<rsiz; i++) {	/* Copy replacement into entry	*/
		namptr->nreplace[i] = *replace++;
	}

	namptr->ndevice = device;	/* Record the device ID		*/

        nnames++;			/* Increment number of names	*/

	restore(mask);
	return OK;
}


/*------------------------------------------------------------------------
 *  namlen  -  Compute the length of a string stopping at maxlen
 *------------------------------------------------------------------------
 */
int32	namlen(
	  char		*name,		/* Name to use			*/
	  int32		maxlen		/* Maximum length (including a	*/
					/*   NULLCH)			*/
)
{
	int32	i;			/* Count of characters found	*/

	/* Search until a null terminator or length reaches max */

	for (i=0; i < maxlen; i++) {
		if (*name++ == NULLCH) {
			return i+1;	/* Include NULLCH in length */
		}
	}
	return SYSERR;
}
