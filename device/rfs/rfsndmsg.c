/* rfsndmsg.c - rfsndmsg */

#include <xinu.h>

/*------------------------------------------------------------------------
 * rfsndmsg  -  Create and send a message that only has header fields
 *------------------------------------------------------------------------
 */
status	rfsndmsg (
	 uint16 type,			/* Message type			*/
	 char	*name			/* Null-terminated file name	*/
	)
{
	struct	rf_msg_hdr  req;	/* Request message to send	*/
	struct	rf_msg_hdr resp;	/* Buffer for response		*/
	int32	retval;			/* Return value			*/
	char	*to;			/* Used during name copy	*/

	/* Form a request */

	req.rf_type = htons(type);
	req.rf_status = htons(0);
	req.rf_seq = 0;			/* Rfscomm will set sequence	*/
	to = req.rf_name;
	while ( (*to++ = *name++) ) {	/* Copy name to request		*/
		;
	}

	/* Send message and receive response */

	retval = rfscomm(&req,  sizeof(struct rf_msg_hdr),
			 &resp,	sizeof(struct rf_msg_hdr) );

	/* Check response */

	if (retval == SYSERR) {
		return SYSERR;
	} else if (retval == TIMEOUT) {
		kprintf("Timeout during remote file server access\n");
		return SYSERR;
	} else if (ntohl(resp.rf_status) != 0) {
		return SYSERR;
	}

	return OK;
}
