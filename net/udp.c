/* udp.c - udp_init, udp_in, udp_register, udp_send, udp_sendto,	*/
/*	        udp_recv, udp_recvaddr, udp_release, udp_ntoh, udp_hton	*/

#include <xinu.h>

struct	udpentry udptab[UDP_SLOTS];	/* Table of UDP endpoints	*/

/*------------------------------------------------------------------------
 * udp_init  -  Initialize all entries in the UDP endpoint table
 *------------------------------------------------------------------------
 */
void	udp_init(void)
{

	int32	i;			/* Index into the UDP table */

	for(i=0; i<UDP_SLOTS; i++) {
		udptab[i].udstate = UDP_FREE;
	}

	return;
}


/*------------------------------------------------------------------------
 * udp_in  -  Handle an incoming UDP packet
 *------------------------------------------------------------------------
 */
void	udp_in(
	  struct netpacket *pktptr	/* Pointer to the packet	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	int32	i;			/* Index into udptab		*/
	struct	udpentry *udptr;	/* Pointer to a udptab entry	*/


	/* Ensure only one process can access the UDP table at a time	*/

	mask = disable();

	for (i=0; i<UDP_SLOTS; i++) {
	    udptr = &udptab[i];
	    if (udptr->udstate == UDP_FREE) {
			continue;
	    }

	    if ((pktptr->net_udpdport == udptr->udlocport)  &&
                    ((udptr->udremport == 0) ||
                        (pktptr->net_udpsport == udptr->udremport)) &&
                 (  ((udptr->udremip==0)     ||
                        (pktptr->net_ipsrc == udptr->udremip)))    ) {

		/* Entry matches incoming packet */

		if (udptr->udcount < UDP_QSIZ) {
			udptr->udcount++;
			udptr->udqueue[udptr->udtail++] = pktptr;
			if (udptr->udtail >= UDP_QSIZ) {
				udptr->udtail = 0;
			}
			if (udptr->udstate == UDP_RECV) {
				udptr->udstate = UDP_USED;
				send (udptr->udpid, OK);
			}
			restore(mask);
			return;
		}
	    }
	}

	/* No match - simply discard packet */

	freebuf((char *) pktptr);
	restore(mask);
	return;
}

/*------------------------------------------------------------------------
 * udp_register  -  Register a remote IP, remote port & local port to
 *		      receive incoming UDP messages from the specified
 *		      remote site sent to the specified local port
 *------------------------------------------------------------------------
 */
uid32	udp_register (
	 uint32	remip,			/* Remote IP address or zero	*/
	 uint16	remport,		/* Remote UDP protocol port	*/
	 uint16	locport			/* Local UDP protocol port	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	int32	slot;			/* Index into udptab		*/
	struct	udpentry *udptr;	/* Pointer to udptab entry	*/

	/* Ensure only one process can access the UDP table at a time	*/

	mask = disable();

	/* See if request already registered */

	for (slot=0; slot<UDP_SLOTS; slot++) {
		udptr = &udptab[slot];
		if (udptr->udstate == UDP_FREE) {
			continue;
		}

		/* Look at this entry in table */

		if ( (remport == udptr->udremport) &&
		     (locport == udptr->udlocport) &&
		     (remip   == udptr->udremip  ) ) {

			/* Request is already in the table */

			restore(mask);
			return SYSERR;
		}
	}

	/* Find a free slot and allocate it */

	for (slot=0; slot<UDP_SLOTS; slot++) {
		udptr = &udptab[slot];
		if (udptr->udstate != UDP_FREE) {
			continue;
		}
		udptr->udlocport = locport;
		udptr->udremport = remport;
		udptr->udremip = remip;
		udptr->udcount = 0;
		udptr->udhead = udptr->udtail = 0;
		udptr->udpid = -1;
		udptr->udstate = UDP_USED;
		restore(mask);
		return slot;
	}

	restore(mask);
	return SYSERR;
}

/*------------------------------------------------------------------------
 * udp_recv  -  Receive a UDP packet
 *------------------------------------------------------------------------
 */
int32	udp_recv (
	 uid32	slot,			/* Slot in table to use		*/
	 char   *buff,			/* Buffer to hold UDP data	*/
	 int32	len,			/* Length of buffer		*/
	 uint32	timeout			/* Read timeout in msec		*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	udpentry *udptr;	/* Pointer to udptab entry	*/
	umsg32	msg;			/* Message from recvtime()	*/
	struct	netpacket *pkt;		/* Pointer to packet being read	*/
	int32	i;			/* Counts bytes copied		*/
	int32	msglen;			/* Length of UDP data in packet	*/
	char	*udataptr;		/* Pointer to UDP data		*/

	/* Ensure only one process can access the UDP table at a time	*/

	mask = disable();

	/* Verify that the slot is valid */

	if ((slot < 0) || (slot >= UDP_SLOTS)) {
		restore(mask);
		return SYSERR;
	}

	/* Get pointer to table entry */

	udptr = &udptab[slot];

	/* Verify that the slot has been registered and is valid */

	if (udptr->udstate != UDP_USED) {
		restore(mask);
		return SYSERR;
	}

	/* Wait for a packet to arrive */

	if (udptr->udcount == 0) {		/* No packet is waiting	*/
		udptr->udstate = UDP_RECV;
		udptr->udpid = currpid;
		msg = recvclr();
		msg = recvtime(timeout);	/* Wait for a packet	*/
		udptr->udstate = UDP_USED;
		if (msg == TIMEOUT) {
			restore(mask);
			return TIMEOUT;
		} else if (msg != OK) {
			restore(mask);
			return SYSERR;
		}
	}

	/* Packet has arrived -- dequeue it */

	pkt = udptr->udqueue[udptr->udhead++];
	if (udptr->udhead >= UDP_QSIZ) {
		udptr->udhead = 0;
	}
	udptr->udcount--;

	/* Copy UDP data from packet into caller's buffer */

	msglen = pkt->net_udplen - UDP_HDR_LEN;
	udataptr = (char *)pkt->net_udpdata;
	if (len < msglen) {
		msglen = len;
	}
	for (i=0; i<msglen; i++) {
		*buff++ = *udataptr++;
	}
	freebuf((char *)pkt);
	restore(mask);
	return msglen;
}

/*------------------------------------------------------------------------
 * udp_recvaddr  -  Receive a UDP packet and record the sender's address
 *------------------------------------------------------------------------
 */
int32	udp_recvaddr (
	 uid32	slot,			/* Slot in table to use		*/
	 uint32	*remip,			/* Loc for remote IP address	*/
	 uint16	*remport,		/* Loc for remote protocol port	*/
	 char   *buff,			/* Buffer to hold UDP data	*/
	 int32	len,			/* Length of buffer		*/
	 uint32	timeout			/* Read timeout in msec		*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	udpentry *udptr;	/* Pointer to udptab entry	*/
	umsg32	msg;			/* Message from recvtime()	*/
	struct	netpacket *pkt;		/* Pointer to packet being read	*/
	int32	msglen;			/* Length of UDP data in packet	*/
	int32	i;			/* Counts bytes copied		*/
	char	*udataptr;		/* Pointer to UDP data		*/

	/* Ensure only one process can access the UDP table at a time	*/

	mask = disable();

	/* Verify that the slot is valid */

	if ((slot < 0) || (slot >= UDP_SLOTS)) {
		restore(mask);
		return SYSERR;
	}

	/* Get pointer to table entry */

	udptr = &udptab[slot];

	/* Verify that the slot has been registered and is valid */

	if (udptr->udstate != UDP_USED) {
		restore(mask);
		return SYSERR;
	}

	/* Wait for a packet to arrive */

	if (udptr->udcount == 0) {		/* No packet is waiting */
		udptr->udstate = UDP_RECV;
		udptr->udpid = currpid;
		msg = recvclr();
		msg = recvtime(timeout);	/* Wait for a packet	*/
		udptr->udstate = UDP_USED;
		if (msg == TIMEOUT) {
			restore(mask);
			return TIMEOUT;
		} else if (msg != OK) {
			restore(mask);
			return SYSERR;
		}
	}

	/* Packet has arrived -- dequeue it */

	pkt = udptr->udqueue[udptr->udhead++];
	if (udptr->udhead >= UDP_QSIZ) {
		udptr->udhead = 0;
	}

	/* Record sender's IP address and UDP port number */

	*remip = pkt->net_ipsrc;
	*remport = pkt->net_udpsport;

	udptr->udcount--;

	/* Copy UDP data from packet into caller's buffer */

	msglen = pkt->net_udplen - UDP_HDR_LEN;
	udataptr = (char *)pkt->net_udpdata;
	if (len < msglen) {
		msglen = len;
	}
	for (i=0; i<msglen; i++) {
		*buff++ = *udataptr++;
	}
	freebuf((char *)pkt);
	restore(mask);
	return msglen;
}

/*------------------------------------------------------------------------
 * udp_send  -  Send a UDP packet using info in a UDP table entry
 *------------------------------------------------------------------------
 */
status	udp_send (
	 uid32	slot,			/* Table slot to use		*/
	 char   *buff,			/* Buffer of UDP data		*/
	 int32	len			/* Length of data in buffer	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	netpacket *pkt;		/* Pointer to packet buffer	*/
	int32	pktlen;			/* Total packet length		*/
	static	uint16 ident = 1;	/* Datagram IDENT field		*/
	char	*udataptr;		/* Pointer to UDP data		*/
	uint32	remip;			/* Remote IP address to use	*/
	uint16	remport;		/* Remote protocol port to use	*/
	uint16	locport;		/* Local protocol port to use	*/
	uint32	locip;			/* Local IP address taken from	*/
					/*   the interface		*/
	struct	udpentry *udptr;	/* Pointer to table entry	*/

	/* Ensure only one process can access the UDP table at a time	*/

	mask = disable();

	/* Verify that the slot is valid */

	if ( (slot < 0) || (slot >= UDP_SLOTS) ) {
		restore(mask);
		return SYSERR;
	}

	/* Get pointer to table entry */

	udptr = &udptab[slot];

	/* Verify that the slot has been registered and is valid */

	if (udptr->udstate == UDP_FREE) {
		restore(mask);
		return SYSERR;
	}

	/* Verify that the slot has a specified remote address */

	remip = udptr->udremip;
	if (remip == 0) {
		restore(mask);
		return SYSERR;
	}

	locip = NetData.ipucast;
	remport = udptr->udremport;
	locport = udptr->udlocport;

	/* Allocate a network buffer to hold the packet */

	pkt = (struct netpacket *)getbuf(netbufpool);

	if ((int32)pkt == SYSERR) {
		restore(mask);
		return SYSERR;
	}

	/* Compute packet length as UDP data size + fixed header size	*/

	pktlen = ((char *)&pkt->net_udpdata - (char *)pkt) + len;

	/* Create a UDP packet in pkt */

	memcpy((char *)pkt->net_ethsrc,NetData.ethucast,ETH_ADDR_LEN);
	pkt->net_ethtype = 0x0800;	/* Type is IP			*/
	pkt->net_ipvh = 0x45;		/* IP version and hdr length	*/
	pkt->net_iptos = 0x00;		/* Type of service		*/
	pkt->net_iplen= pktlen - ETH_HDR_LEN;/* Total IP datagram length*/
	pkt->net_ipid = ident++;	/* Datagram gets next IDENT	*/
	pkt->net_ipfrag = 0x0000;	/* IP flags & fragment offset	*/
	pkt->net_ipttl = 0xff;		/* IP time-to-live		*/
	pkt->net_ipproto = IP_UDP;	/* Datagram carries UDP		*/
	pkt->net_ipcksum = 0x0000;	/* initial checksum		*/
	pkt->net_ipsrc = locip;		/* IP source address		*/
	pkt->net_ipdst = remip;		/* IP destination address	*/

	pkt->net_udpsport = locport;	/* Local UDP protocol port	*/
	pkt->net_udpdport = remport;	/* Remote UDP protocol port	*/
	pkt->net_udplen = (uint16)(UDP_HDR_LEN+len); /* UDP length	*/
	pkt->net_udpcksum = 0x0000;	/* Ignore UDP checksum		*/
	udataptr = (char *) pkt->net_udpdata;
	for (; len>0; len--) {
		*udataptr++ = *buff++;
	}

	/* Call ipsend to send the datagram */

	ip_send(pkt);
	restore(mask);
	return OK;
}


/*------------------------------------------------------------------------
 * udp_sendto  -  Send a UDP packet to a specified destination
 *------------------------------------------------------------------------
 */
status	udp_sendto (
	 uid32	slot,			/* UDP table slot to use	*/
	 uint32	remip,			/* Remote IP address to use	*/
	 uint16	remport,		/* Remote protocol port to use	*/
	 char   *buff,			/* Buffer of UDP data		*/
	 int32	len			/* Length of data in buffer	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	netpacket *pkt;		/* Pointer to a packet buffer	*/
	int32	pktlen;			/* Total packet length		*/
	static	uint16 ident = 1;	/* Datagram IDENT field		*/
	struct	udpentry *udptr;	/* Pointer to a UDP table entry	*/
	char	*udataptr;		/* Pointer to UDP data		*/

	/* Ensure only one process can access the UDP table at a time	*/

	mask = disable();

	/* Verify that the slot is valid */

	if ( (slot < 0) || (slot >= UDP_SLOTS) ) {
		restore(mask);
		return SYSERR;
	}

	/* Get pointer to table entry */

	udptr = &udptab[slot];

	/* Verify that the slot has been registered and is valid */

	if (udptr->udstate == UDP_FREE) {
		restore(mask);
		return SYSERR;
	}

	/* Allocate a network buffer to hold the packet */

	pkt = (struct netpacket *)getbuf(netbufpool);

	if ((int32)pkt == SYSERR) {
		restore(mask);
		return SYSERR;
	}

	/* Compute packet length as UDP data size + fixed header size	*/

	pktlen = ((char *)&pkt->net_udpdata - (char *)pkt) + len;

	/* Create UDP packet in pkt */

	memcpy((char *)pkt->net_ethsrc,NetData.ethucast,ETH_ADDR_LEN);
        pkt->net_ethtype = 0x0800;	/* Type is IP */
	pkt->net_ipvh = 0x45;		/* IP version and hdr length	*/
	pkt->net_iptos = 0x00;		/* Type of service		*/
	pkt->net_iplen= pktlen - ETH_HDR_LEN;/* total IP datagram length*/
	pkt->net_ipid = ident++;	/* Datagram gets next IDENT	*/
	pkt->net_ipfrag = 0x0000;	/* IP flags & fragment offset	*/
	pkt->net_ipttl = 0xff;		/* IP time-to-live		*/
	pkt->net_ipproto = IP_UDP;	/* Datagram carries UDP		*/
	pkt->net_ipcksum = 0x0000;	/* Initial checksum		*/
	pkt->net_ipsrc = NetData.ipucast;/* IP source address		*/
	pkt->net_ipdst = remip;		/* IP destination address	*/
	pkt->net_udpsport = udptr->udlocport;/* local UDP protocol port	*/
	pkt->net_udpdport = remport;	/* Remote UDP protocol port	*/
	pkt->net_udplen = (uint16)(UDP_HDR_LEN+len); /* UDP length	*/
	pkt->net_udpcksum = 0x0000;	/* Ignore UDP checksum		*/
	udataptr = (char *) pkt->net_udpdata;
	for (; len>0; len--) {
		*udataptr++ = *buff++;
	}

	/* Call ipsend to send the datagram */

	ip_send(pkt);
	restore(mask);
	return OK;
}


/*------------------------------------------------------------------------
 * udp_release  -  Release a previously-registered UDP slot
 *------------------------------------------------------------------------
 */
status	udp_release (
	 uid32	slot			/* Table slot to release	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	udpentry *udptr;	/* Pointer to udptab entry	*/
	struct	netpacket *pkt;		/* pointer to packet being read	*/

	/* Ensure only one process can access the UDP table at a time	*/

	mask = disable();

	/* Verify that the slot is valid */

	if ( (slot < 0) || (slot >= UDP_SLOTS) ) {
		restore(mask);
		return SYSERR;
	}

	/* Get pointer to table entry */

	udptr = &udptab[slot];

	/* Verify that the slot has been registered and is valid */

	if (udptr->udstate == UDP_FREE) {
		restore(mask);
		return SYSERR;
	}

	/* Defer rescheduling to prevent freebuf from switching context	*/

	resched_cntl(DEFER_START);
	while (udptr->udcount > 0) {
		pkt = udptr->udqueue[udptr->udhead++];
		if (udptr->udhead >= UDP_QSIZ) {
			udptr->udhead = 0;
		}
		freebuf((char *)pkt);
		udptr->udcount--;
	}
	udptr->udstate = UDP_FREE;
	resched_cntl(DEFER_STOP);
	restore(mask);
	return OK;
}

/*------------------------------------------------------------------------
 * udp_ntoh  -  Convert UDP header fields from net to host byte order
 *------------------------------------------------------------------------
 */
void 	udp_ntoh(
	  struct netpacket *pktptr
	)
{
	pktptr->net_udpsport = ntohs(pktptr->net_udpsport);
	pktptr->net_udpdport = ntohs(pktptr->net_udpdport);
	pktptr->net_udplen = ntohs(pktptr->net_udplen);
	return;
}

/*------------------------------------------------------------------------
 * udp_hton  -  Convert packet header fields from host to net byte order
 *------------------------------------------------------------------------
 */
void 	udp_hton(
	  struct netpacket *pktptr
	)
{
	pktptr->net_udpsport = htons(pktptr->net_udpsport);
	pktptr->net_udpdport = htons(pktptr->net_udpdport);
	pktptr->net_udplen = htons(pktptr->net_udplen);
	return;
}
