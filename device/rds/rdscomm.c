/* rdscomm.c - rdscomm */

#include <xinu.h>

/*------------------------------------------------------------------------
 * rdscomm  -  handle communication with a remote disk server (send a
 *		 request and receive a reply, including sequencing and
 *		 retries)
 *------------------------------------------------------------------------
 */
status	rdscomm (
	  struct rd_msg_hdr *msg,	/* Message to send		*/
	  int32		    mlen,	/* Message length		*/
	  struct rd_msg_hdr *reply,	/* Buffer for reply		*/
	  int32		    rlen,	/* Size of reply buffer		*/
	  struct rdscblk    *rdptr	/* Ptr to device control block	*/
	)
{
	intmask		mask;		/* Saved interrupt mask		*/
	int32		i;		/* Counts retries		*/
	int32		retval;		/* Return value			*/
	int32		seq;		/* Sequence for this exchange	*/
	uint32		localip;	/* Local IP address		*/
	int16		rtype;		/* Reply type in host byte order*/
	bool8		xmit;		/* Should we transmit again?	*/
	int32		slot;		/* UDP slot			*/

	/* Disable interrupts while testing status */

	mask = disable();

	/* Register the server port, if not registered */

	if ( ! rdptr->rd_registered ) {
		slot = udp_register(0, rdptr->rd_ser_port,
						rdptr->rd_loc_port);
		if(slot == SYSERR) {
			restore(mask);
			return SYSERR;
		}
		rdptr->rd_udpslot = slot;
		rdptr->rd_registered = TRUE;
	}

	/* Get the local IP address */

	if ( NetData.ipvalid == FALSE ) {
		localip = getlocalip();
		if((int32)localip == SYSERR) {
			restore(mask);
			return SYSERR;
		}
	}
	restore(mask);

	/* Retrieve the saved UDP slot number  */

	slot = rdptr->rd_udpslot;

	/* Assign message next sequence number */

	seq = rdptr->rd_seq++;
	msg->rd_seq = htonl(seq);

	/* Repeat RD_RETRIES times: send message and receive reply */

	xmit = TRUE;
	for (i=0; i<RD_RETRIES; i++) {
	    if (xmit) {

		/* Send a copy of the message */

		retval = udp_sendto(slot, rdptr->rd_ser_ip, rdptr->rd_ser_port,
					(char *)msg, mlen);
		if (retval == SYSERR) {
			kprintf("Cannot send to remote disk server\n\r");
			return SYSERR;
		}
	    } else {
		xmit = TRUE;
	    }

	    /* Receive a reply */

	    retval = udp_recv(slot, (char *)reply, rlen,
						RD_TIMEOUT);

	    if (retval == TIMEOUT) {
		continue;
	    } else if (retval == SYSERR) {
		kprintf("Error reading remote disk reply\n\r");
		return SYSERR;
	    }

	    /* Verify that sequence in reply matches request */

		
	    if (ntohl(reply->rd_seq) < seq) {
		xmit = FALSE;
	    } else if (ntohl(reply->rd_seq) != seq) {
			continue;
	    }

	    /* Verify the type in the reply matches the request */

	    rtype = ntohs(reply->rd_type);
	    if (rtype != ( ntohs(msg->rd_type) | RD_MSG_RESPONSE) ) {
		continue;
	    }

	    /* Check the status */

	    if (ntohs(reply->rd_status) != 0) {
		return SYSERR;
	    }

	    return OK;
	}

	/* Retries exhausted without success */

	kprintf("Timeout on exchange with remote disk server\n\r");
	return TIMEOUT;
}
