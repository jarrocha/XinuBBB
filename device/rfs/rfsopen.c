/* rfsopen.c - rfsopen */

#include <xinu.h>

/*------------------------------------------------------------------------
 * rfsopen  -  Allocate a remote file pseudo-device for a specific file
 *------------------------------------------------------------------------
 */

devcall	rfsopen (
	 struct	dentry	*devptr,	/* Entry in device switch table	*/
	 char	*name,			/* File name to use		*/
	 char	*mode			/* Mode chars: 'r' 'w' 'o' 'n'	*/
	)
{
	struct	rflcblk	*rfptr;		/* Ptr to control block entry	*/
	struct	rf_msg_oreq msg;	/* Message to be sent		*/
	struct	rf_msg_ores resp;	/* Buffer to hold response	*/
	int32	retval;			/* Return value from rfscomm	*/
	int32	len;			/* Counts chars in name		*/
	char	*nptr;			/* Pointer into name string	*/
	char	*fptr;			/* Pointer into file name	*/
	int32	i;			/* General loop index		*/

	/* Wait for exclusive access */

	wait(Rf_data.rf_mutex);

	/* Search control block array to find a free entry */

	for(i=0; i<Nrfl; i++) {
		rfptr = &rfltab[i];
		if (rfptr->rfstate == RF_FREE) {
			break;
		}
	}
	if (i >= Nrfl) {		/* No free table slots remain	*/
		signal(Rf_data.rf_mutex);
		return SYSERR;
	}

	/* Copy name into free table slot */

	nptr = name;
	fptr = rfptr->rfname;
	len = 0;
	while ( (*fptr++ = *nptr++) != NULLCH) {
		len++;
		if (len >= RF_NAMLEN) {	/* File name is too long	*/
			signal(Rf_data.rf_mutex);
			return SYSERR;
		}
	}

	/* Verify that name is non-null */

	if (len==0) {
		signal(Rf_data.rf_mutex);
		return SYSERR;
	}

	/* Parse mode string */

	if ( (rfptr->rfmode = rfsgetmode(mode)) == SYSERR ) {
		signal(Rf_data.rf_mutex);
		return SYSERR;
	}

	/* Form an open request to create a new file or open an old one */

	msg.rf_type = htons(RF_MSG_OREQ);/* Request a file open		*/
	msg.rf_status = htons(0);
	msg.rf_seq = 0;			/* Rfscomm fills in seq. number	*/
	nptr = msg.rf_name;
	memset(nptr, NULLCH, RF_NAMLEN);/* Initialize name to zero bytes*/
	while ( (*nptr++ = *name++) != NULLCH ) { /* Copy name to req.	*/
		;
	}
	msg.rf_mode = htonl(rfptr->rfmode); /* Set mode in request	*/

	/* Send message and receive response */

	retval = rfscomm((struct rf_msg_hdr *)&msg,
					sizeof(struct rf_msg_oreq),
			 (struct rf_msg_hdr *)&resp,
					sizeof(struct rf_msg_ores) );

	/* Check response */

	if (retval == SYSERR) {
		signal(Rf_data.rf_mutex);
		return SYSERR;
	} else if (retval == TIMEOUT) {
		kprintf("Timeout during remote file open\n\r");
		signal(Rf_data.rf_mutex);
		return SYSERR;
	} else if (ntohs(resp.rf_status) != 0) {
		signal(Rf_data.rf_mutex);
		return SYSERR;
	}

	/* Set initial file position */

	rfptr->rfpos = 0;

	/* Mark state as currently used */

	rfptr->rfstate = RF_USED;

	/* Return device descriptor of newly created pseudo-device */

	signal(Rf_data.rf_mutex);
	return rfptr->rfdev;
}
