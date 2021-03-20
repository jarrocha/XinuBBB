/* rflread.c - rflread */

#include <xinu.h>

/*------------------------------------------------------------------------
 * rflread  -  Read data from a remote file
 *------------------------------------------------------------------------
 */
devcall	rflread (
	  struct dentry	*devptr,	/* Entry in device switch table	*/
	  char	*buff,			/* Buffer of bytes		*/
	  int32	count 			/* Count of bytes to read	*/
	)
{
	struct	rflcblk	*rfptr;		/* Pointer to control block	*/
	int32	retval;			/* Return value			*/
	struct	rf_msg_rreq  msg;	/* Request message to send	*/
	struct	rf_msg_rres resp;	/* Buffer for response		*/
	int32	i;			/* Counts bytes copied		*/
	char	*from, *to;		/* Used during name copy	*/
	int32	len;			/* Length of name		*/

	/* Wait for exclusive access */

	wait(Rf_data.rf_mutex);

	/* Verify count is legitimate */

	if ( (count <= 0) || (count > RF_DATALEN) ) {
		signal(Rf_data.rf_mutex);
		return SYSERR;
	}

	/* Verify pseudo-device is in use */

	rfptr = &rfltab[devptr->dvminor];

	/* If device not currently in use, report an error */

	if (rfptr->rfstate == RF_FREE) {
		signal(Rf_data.rf_mutex);
		return SYSERR;
	}

	/* Verify pseudo-device allows reading */

	if ((rfptr->rfmode & RF_MODE_R) == 0) {
		signal(Rf_data.rf_mutex);
		return SYSERR;
	}

	/* Form read request */

	msg.rf_type = htons(RF_MSG_RREQ);
	msg.rf_status = htons(0);
	msg.rf_seq = 0;			/* Rfscomm will set sequence	*/
	from = rfptr->rfname;
	to = msg.rf_name;
	memset(to, NULLCH, RF_NAMLEN);	/* Start name as all zero bytes	*/
	len = 0;
	while ( (*to++ = *from++) ) {	/* Copy name to request		*/
		if (++len >= RF_NAMLEN) {
			signal(Rf_data.rf_mutex);
			return SYSERR;
		}
	}
	msg.rf_pos = htonl(rfptr->rfpos);/* Set file position		*/
	msg.rf_len = htonl(count);	/* Set count of bytes to read	*/

	/* Send message and receive response */

	retval = rfscomm((struct rf_msg_hdr *)&msg,
					sizeof(struct rf_msg_rreq),
			 (struct rf_msg_hdr *)&resp,
					sizeof(struct rf_msg_rres) );

	/* Check response */

	if (retval == SYSERR) {
		signal(Rf_data.rf_mutex);
		return SYSERR;
	} else if (retval == TIMEOUT) {
		kprintf("Timeout during remote file read\n");
		signal(Rf_data.rf_mutex);
		return SYSERR;
	} else if (ntohs(resp.rf_status) != 0) {
		signal(Rf_data.rf_mutex);
		return SYSERR;
	}

	/* Copy data to application buffer and update file position */

	for (i=0; i<ntohl(resp.rf_len); i++) {
		*buff++ = resp.rf_data[i];
	}
	rfptr->rfpos += ntohl(resp.rf_len);

	signal(Rf_data.rf_mutex);
	return ntohl(resp.rf_len);
}
