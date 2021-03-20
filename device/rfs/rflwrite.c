/* rflwrite.c - rflwrite */

#include <xinu.h>

/*------------------------------------------------------------------------
 * rflwrite  -  Write data to a remote file
 *------------------------------------------------------------------------
 */
devcall	rflwrite (
	  struct dentry	*devptr,	/* Entry in device switch table	*/
	  char	*buff,			/* Buffer of bytes		*/
	  int32	count 			/* Count of bytes to write	*/
	)
{
	struct	rflcblk	*rfptr;		/* Pointer to control block	*/
	int32	retval;			/* Return value			*/
	struct	rf_msg_wreq  msg;	/* Request message to send	*/
	struct	rf_msg_wres resp;	/* Buffer for response		*/
	char	*from, *to;		/* Used to copy name		*/
	int	i;			/* Counts bytes copied into req	*/
	int32	len;			/* Length of name		*/

	/* Wait for exclusive access */

	wait(Rf_data.rf_mutex);

	/* Verify count is legitimate */

	if ( (count <= 0) || (count > RF_DATALEN) ) {
		signal(Rf_data.rf_mutex);
		return SYSERR;
	}

	/* Verify pseudo-device is in use and mode allows writing */

	rfptr = &rfltab[devptr->dvminor];
	if ( (rfptr->rfstate == RF_FREE) ||
	     ! (rfptr->rfmode & RF_MODE_W) ) {
		signal(Rf_data.rf_mutex);
		return SYSERR;
	}

	/* Form write request */

	msg.rf_type = htons(RF_MSG_WREQ);
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
	while ( (*to++ = *from++) ) {	/* Copy name into request	*/
		;
	}
	msg.rf_pos = htonl(rfptr->rfpos);/* Set file position		*/
	msg.rf_len = htonl(count);	/* Set count of bytes to write	*/
	for (i=0; i<count; i++) {	/* Copy data into message	*/
		msg.rf_data[i] = *buff++;
	}
	while (i < RF_DATALEN) {
		msg.rf_data[i++] = NULLCH;
	}

	/* Send message and receive response */

	retval = rfscomm((struct rf_msg_hdr *)&msg,
					sizeof(struct rf_msg_wreq),
			 (struct rf_msg_hdr *)&resp,
				sizeof(struct rf_msg_wres) );

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

	/* Report results to caller */

	rfptr->rfpos += ntohl(resp.rf_len);

	signal(Rf_data.rf_mutex);
	return ntohl(resp.rf_len);
}
