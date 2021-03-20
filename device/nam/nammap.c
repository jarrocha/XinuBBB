/* nammap.c - nammap, namrepl, namcpy */

#include <xinu.h>

status	namcpy(char *, char *, int32);
did32	namrepl(char *, char[]);

/*------------------------------------------------------------------------
 *  nammap  -  Using namespace, map name to new name and new device
 *------------------------------------------------------------------------
 */
devcall	nammap(
	 char	*name,			/* The name to map		*/
	 char	newname[NM_MAXLEN],	/* Buffer for mapped name	*/
	 did32 namdev			/* ID of the namespace device	*/
	)
{
	did32	newdev;			/* Device descriptor to return	*/
	char	tmpname[NM_MAXLEN];	/* Temporary buffer for name	*/
	int32	iter;			/* Number of iterations		*/

	/* Place original name in temporary buffer and null terminate */

	if (namcpy(tmpname, name, NM_MAXLEN) == SYSERR) {
		return SYSERR;
	}

	/* Repeatedly substitute the name prefix until a non-namespace	*/
	/*   device is reached or an iteration limit is exceeded	*/

	for (iter=0; iter<nnames ; iter++) {
		newdev = namrepl(tmpname, newname);
		if (newdev != namdev) {
			return newdev;	/* Either valid ID or SYSERR	*/
		}
		namcpy(tmpname, newname, NM_MAXLEN);
	}
	return SYSERR;
}

/*------------------------------------------------------------------------
 *  namrepl  -  Use the name table to perform prefix substitution
 *------------------------------------------------------------------------
 */
did32	namrepl(
	 char	*name,			/* Original name		*/
	 char	newname[NM_MAXLEN]	/* Buffer for mapped name	*/
	)
{
	int32	i;			/* Iterate through name table	*/
	char	*pptr;			/* Walks through a prefix	*/
	char	*rptr;			/* Walks through a replacement	*/
	char	*optr;			/* Walks through original name	*/
        char    *nptr;                  /* Walks through new name       */
	char	olen;			/* Length of original name	*/
					/*   including the NULL byte	*/
	int32	plen;			/* Length of a prefix string	*/
					/*   *not* including NULL byte	*/
	int32	rlen;			/* Length of replacment string	*/
	int32	remain;			/* Bytes in name beyond prefix	*/
	struct	nmentry	*namptr;	/* Pointer to a table entry	*/

	/* Search name table for first prefix that matches */

	for (i=0; i<nnames; i++) {
		namptr = &nametab[i];
		optr = name;		/* Start at beginning of name	*/
		pptr = namptr->nprefix;	/* Start at beginning of prefix	*/
		
		/* Compare prefix to string and count prefix size */

		for (plen=0; *pptr != NULLCH ; plen++) {
			if (*pptr != *optr) {
				break;
			}
			pptr++;
			optr++;
		}
		if (*pptr != NULLCH) {	/* Prefix does not match */
			continue;
		}

		/* Found a match - check that replacement string plus	*/
		/* bytes remaining at the end of the original name will	*/
		/* fit into new name buffer.  Ignore null on replacement*/
		/* string, but keep null on remainder of name.		*/

		olen = namlen(name ,NM_MAXLEN);
		rlen = namlen(namptr->nreplace,NM_MAXLEN) - 1;
		remain = olen - plen;
		if ( (rlen + remain) > NM_MAXLEN) {
			return (did32)SYSERR;
		}

		/* Place replacement string followed by remainder of	*/
		/*   original name (and null) into the new name buffer	*/

		
                nptr = newname;
                rptr = namptr->nreplace;
		for (; rlen>0 ; rlen--) {
			*nptr++ = *rptr++;
		}
		for (; remain>0 ; remain--) {
			*nptr++ = *optr++;
		}
		return namptr->ndevice;
	}
	return (did32)SYSERR;
}

/*------------------------------------------------------------------------
 *  namcpy  -  Copy a name from one buffer to another, checking length
 *------------------------------------------------------------------------
 */
status	namcpy(
	 char		*newname,	/* Buffer to hold copy		*/
	 char		*oldname,	/* Buffer containing name	*/
    	 int32		buflen		/* Size of buffer for copy	*/
	)
{
        char    *nptr;                  /* Point to new name            */
        char    *optr;                  /* Point to old name            */
	int32	cnt;			/* Count of characters copied	*/

        nptr = newname;
        optr = oldname;

	for (cnt=0; cnt<buflen; cnt++) {
		if ( (*nptr++ = *optr++) == NULLCH) {
			return OK;
		}
	}
	return SYSERR;		/* Buffer filled before copy completed	*/
}
