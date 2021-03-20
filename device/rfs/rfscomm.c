/* rfscomm.c - rfscomm */

#include <xinu.h>

/*------------------------------------------------------------------------
 * rfscomm  -  Handle communication with RFS server (send request and
 *		receive a reply, including sequencing and retries)
 *------------------------------------------------------------------------
 */
int32	rfscomm (
	 struct	rf_msg_hdr *msg,	/* Message to send		*/
	 int32	mlen,			/* Message length		*/
	 struct	rf_msg_hdr *reply,	/* Buffer for reply		*/
	 int32	rlen			/* Size of reply buffer		*/
	)
{
	int32	i;			/* Counts retries		*/
	int32	retval;			/* Return value			*/
	int32	seq;			/* Sequence for this exchange	*/
	int16	rtype;			/* Reply type in host byte order*/
	int32	slot;			/* UDP slot			*/

	/* For the first time after reboot, register the server port */

	if ( ! Rf_data.rf_registered ) {
		if ( (slot = udp_register(Rf_data.rf_ser_ip,
				Rf_data.rf_ser_port,
				Rf_data.rf_loc_port)) == SYSERR) {
			return SYSERR;
		}
		Rf_data.rf_udp_slot = slot;
		Rf_data.rf_registered = TRUE;
	}

	/* Assign message next sequence number */

	seq = Rf_data.rf_seq++;
	msg->rf_seq = htonl(seq);

	/* Repeat RF_RETRIES times: send message and receive reply */

	for (i=0; i<RF_RETRIES; i++) {

		/* Send a copy of the message */

		retval = udp_send(Rf_data.rf_udp_slot, (char *)msg,
			mlen);
		if (retval == SYSERR) {
			kprintf("Cannot send to remote file server\n");
			return SYSERR;
		}

		/* Receive a reply */

		retval = udp_recv(Rf_data.rf_udp_slot, (char *)reply,
			rlen, RF_TIMEOUT);

		if (retval == TIMEOUT) {
			continue;
		} else if (retval == SYSERR) {
			kprintf("Error reading remote file reply\n");
			return SYSERR;
		}

		/* Verify that sequence in reply matches request */

		if (ntohl(reply->rf_seq) != seq) {
			continue;
		}

		/* Verify the type in the reply matches the request */

		rtype = ntohs(reply->rf_type);
		if (rtype != ( ntohs(msg->rf_type) | RF_MSG_RESPONSE) ) {
			continue;
		}

		return retval;		/* Return length to caller */
	}

	/* Retries exhausted without success */

	kprintf("Timeout on exchange with remote file server\n");
	return TIMEOUT;
}
