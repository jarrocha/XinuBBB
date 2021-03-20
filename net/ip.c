/* ip.c - ip_in, ip_send, ip_local, ip_out, ipcksum, ip_hton, ip_ntoh,	*/
/*		 ipout, ip_enqueue					*/

#include <xinu.h>

struct	iqentry	ipoqueue;		/* Queue of outgoing packets	*/

/*------------------------------------------------------------------------
 * ip_in  -  Handle an IP packet that has arrived over a network
 *------------------------------------------------------------------------
 */

void	ip_in(
	  struct netpacket *pktptr	/* Pointer to the packet	*/
	)
{
	int32	icmplen;		/* Length of ICMP message	*/

	/* Verify checksum */

	if (ipcksum(pktptr) != 0) {
		kprintf("IP header checksum failed\n\r");
		freebuf((char *)pktptr);
		return;
	}

	/* Convert IP header fields to host order */

	ip_ntoh(pktptr);

	/* Ensure version and length are valid */

	if (pktptr->net_ipvh != 0x45) {
		kprintf("IP version failed\n\r");
		freebuf((char *)pktptr);
		return;
	}

	/* Verify encapsulated prototcol checksums and then convert	*/
	/*	the encapsulated headers to host byte order		*/

	switch (pktptr->net_ipproto) {

	    case IP_UDP:
		/* Skipping UDP checksum for now */
		udp_ntoh(pktptr);
		break;

	    case IP_ICMP:
		icmplen = pktptr->net_iplen - IP_HDR_LEN;
		if (icmp_cksum((char *)&pktptr->net_ictype,icmplen) != 0){
			freebuf((char *)pktptr);
			return;
		}
		icmp_ntoh(pktptr);
		break;

	    default:
		break;
	}

	/* Deliver 255.255.255.255 to local stack */

	if (pktptr->net_ipdst == IP_BCAST) {
		ip_local(pktptr);
		return;
	}

	/* If we do not yet have a valid address, accept UDP packets	*/
	/*	(to get DHCP replies) and drop others			*/

	if (!NetData.ipvalid) {
		if (pktptr->net_ipproto == IP_UDP) {
			ip_local(pktptr);
			return;
		} else {
			freebuf((char *)pktptr);
			return;
		}
	}

	/* If packet is destined for us, accept it; otherwise, drop it	*/

	if ( (pktptr->net_ipdst == NetData.ipucast) ||
	     (pktptr->net_ipdst == NetData.ipbcast) ||
	     (pktptr->net_ipdst == IP_BCAST)  ) {
		ip_local(pktptr);
		return;
	} else {

		/* Drop the packet */
		freebuf((char *)pktptr);
		return;
	}
}


/*------------------------------------------------------------------------
 * ip_send  -  Send an outgoing IP datagram from the local stack
 *------------------------------------------------------------------------
 */

status	ip_send(
	  struct netpacket *pktptr	/* Pointer to the packet	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	uint32	dest;			/* Destination of the datagram	*/
	int32	retval;			/* Return value from functions	*/
	uint32	nxthop;			/* Next-hop address		*/

	mask = disable();

	/* Pick up the IP destination address from the packet */

	dest = pktptr->net_ipdst;

	/* Loop back to local stack if destination 127.0.0.0/8 */

	if ((dest&0xff000000) == 0x7f000000) {
		ip_local(pktptr);
		restore(mask);
		return OK;
	}

	/* Loop back if the destination matches our IP unicast address	*/

	if (dest == NetData.ipucast) {
		ip_local(pktptr);
		restore(mask);
		return OK;
	}

	/* Broadcast if destination is 255.255.255.255 */

	if ( (dest == IP_BCAST) ||
	     (dest == NetData.ipbcast) ) {
		memcpy(pktptr->net_ethdst, NetData.ethbcast,
							ETH_ADDR_LEN);
		retval = ip_out(pktptr);
		restore(mask);
		return retval;
	}

	/* If destination is on the local network, next hop is the	*/
	/*	destination; otherwise, next hop is default router	*/


	if ( (dest & NetData.ipmask) == NetData.ipprefix) {

		/* Next hop is the destination itself */
		nxthop = dest;

	} else {

		/* Next hop is default router on the network */
		nxthop = NetData.iprouter;

	}

	if (nxthop == 0) {	/* Dest. invalid or no default route	*/
		freebuf((char *)pktptr);
		return SYSERR;
	}

	/* Resolve the next-hop address to get a MAC address */

	retval = arp_resolve(nxthop, pktptr->net_ethdst);
	if (retval != OK) {
		freebuf((char *)pktptr);
		return SYSERR;
	}

	/* Send the packet */

	retval = ip_out(pktptr);
	restore(mask);
	return retval;
}


/*------------------------------------------------------------------------
 * ip_local  -  Deliver an IP datagram to the local stack
 *------------------------------------------------------------------------
 */
void	ip_local(
	  struct netpacket *pktptr	/* Pointer to the packet	*/
	)
{
	/* Use datagram contents to determine how to process */

	switch (pktptr->net_ipproto) {

	    case IP_UDP:
		udp_in(pktptr);
		return;

	    case IP_ICMP:
		icmp_in(pktptr);
		return;

	    default:
		freebuf((char *)pktptr);
		return;
	}
}


/*------------------------------------------------------------------------
 *  ip_out  -  Transmit an outgoing IP datagram
 *------------------------------------------------------------------------
 */
status	ip_out(
	  struct netpacket *pktptr	/* Pointer to the packet	*/
	)
{
	uint16	cksum;			/* Checksum in host byte order	*/
	int32	len;			/* Length of ICMP message	*/	
	int32	pktlen;			/* Length of entire packet	*/
	int32	retval;			/* Value returned by write	*/

	/* Compute total packet length */

	pktlen = pktptr->net_iplen + ETH_HDR_LEN;

	/* Convert encapsulated protocol to network byte order */

	switch (pktptr->net_ipproto) {

	    case IP_UDP:

			pktptr->net_udpcksum = 0;
			udp_hton(pktptr);

			/* ...skipping UDP checksum computation */

			break;

	    case IP_ICMP:
			icmp_hton(pktptr);

			/* Compute ICMP checksum */

			pktptr->net_iccksum = 0;
			len = pktptr->net_iplen-IP_HDR_LEN;
			cksum = icmp_cksum((char *)&pktptr->net_ictype,
								len);
			pktptr->net_iccksum = 0xffff & htons(cksum);
			break;

	    default:
			break;
	}

	/* Convert IP fields to network byte order */

	ip_hton(pktptr);

	/* Compute IP header checksum */

	pktptr->net_ipcksum = 0;
	cksum = ipcksum(pktptr);
	pktptr->net_ipcksum = 0xffff & htons(cksum);

	/* Convert Ethernet fields to network byte order */

	eth_hton(pktptr);

	/* Send packet over the Ethernet */

	retval = write(ETHER0, (char*)pktptr, pktlen);
	freebuf((char *)pktptr);

	if (retval == SYSERR) {
		return SYSERR;
	} else {
		return OK;
	}
}

/*------------------------------------------------------------------------
 * ipcksum  -  Compute the IP header checksum for a datagram
 *------------------------------------------------------------------------
 */

uint16	ipcksum(
	 struct  netpacket *pkt		/* Pointer to the packet	*/
	)
{
	uint16	*hptr;			/* Ptr to 16-bit header values	*/
	int32	i;			/* Counts 16-bit values in hdr	*/
	uint16	word;			/* One 16-bit word		*/
	uint32	cksum;			/* Computed value of checksum	*/

	hptr= (uint16 *) &pkt->net_ipvh;

	/* Sum 16-bit words in the packet */

	cksum = 0;
	for (i=0; i<10; i++) {
		word = *hptr++;
		cksum += (uint32) htons(word);
	}

	/* Add in carry, and take the ones-complement */

	cksum += (cksum >> 16);
	cksum = 0xffff & ~cksum;

	/* Use all-1s for zero */

	if (cksum == 0xffff) {
		cksum = 0;
	}
	return (uint16) (0xffff & cksum);
}


/*------------------------------------------------------------------------
 * ip_ntoh  -  Convert IP header fields to host byte order
 *------------------------------------------------------------------------
 */
void 	ip_ntoh(
	  struct netpacket *pktptr
	)
{
	pktptr->net_iplen = ntohs(pktptr->net_iplen);
	pktptr->net_ipid = ntohs(pktptr->net_ipid);
	pktptr->net_ipfrag = ntohs(pktptr->net_ipfrag);
	pktptr->net_ipsrc = ntohl(pktptr->net_ipsrc);
	pktptr->net_ipdst = ntohl(pktptr->net_ipdst);
}

/*------------------------------------------------------------------------
 * ip_hton  -  Convert IP header fields to network byte order
 *------------------------------------------------------------------------
 */
void 	ip_hton(
	  struct netpacket *pktptr

	)
{
	pktptr->net_iplen = htons(pktptr->net_iplen);
	pktptr->net_ipid = htons(pktptr->net_ipid);
	pktptr->net_ipfrag = htons(pktptr->net_ipfrag);
	pktptr->net_ipsrc = htonl(pktptr->net_ipsrc);
	pktptr->net_ipdst = htonl(pktptr->net_ipdst);
}


/*------------------------------------------------------------------------
 *  ipout  -  Process that transmits IP packets from the IP output queue
 *------------------------------------------------------------------------
 */

process	ipout(void)
{
	struct	netpacket *pktptr;	/* Pointer to next the packet	*/
	struct	iqentry   *ipqptr;	/* Pointer to IP output queue	*/
	uint32	destip;			/* Destination IP address	*/
	uint32	nxthop;			/* Next hop IP address		*/
	int32	retval;			/* Value returned by functions	*/

	ipqptr = &ipoqueue;

	while(1) {

		/* Obtain next packet from the IP output queue */

		wait(ipqptr->iqsem);
		pktptr = ipqptr->iqbuf[ipqptr->iqhead++];
		if (ipqptr->iqhead >= IP_OQSIZ) {
			ipqptr->iqhead= 0;
		}

		/* Fill in the MAC source address */

		memcpy(pktptr->net_ethsrc, NetData.ethucast, ETH_ADDR_LEN);

		/* Extract destination address from packet */

		destip = pktptr->net_ipdst;


		/* Sanity check: packets sent to ipout should *not*	*/
		/*	contain	a broadcast address.			*/

		if ((destip == IP_BCAST)||(destip == NetData.ipbcast)) {
			kprintf("ipout: encountered a broadcast\n");
			freebuf((char *)pktptr);
			continue;
		}

		/* Check whether destination is the local computer */

		if (destip == NetData.ipucast) {
			ip_local(pktptr);
			continue;
		}

		/* Check whether destination is on the local net */

		if ( (destip & NetData.ipmask) == NetData.ipprefix) {

			/* Next hop is the destination itself */

			nxthop = destip;
		} else {

			/* Next hop is default router on the network */

			nxthop = NetData.iprouter;
		}

		if (nxthop == 0) {  /* Dest. invalid or no default route*/
			freebuf((char *)pktptr);
			continue;
		}

		/* Use ARP to resolve next-hop address */

		retval = arp_resolve(nxthop, pktptr->net_ethdst);
		if (retval != OK) {
			freebuf((char *)pktptr);
			continue;
		}

		/* Use ipout to Convert byte order and send */

		ip_out(pktptr);
	}
}


/*------------------------------------------------------------------------
 *  ip_enqueue  -  Deposit an outgoing IP datagram on the IP output queue
 *------------------------------------------------------------------------
 */
status	ip_enqueue(
	  struct netpacket *pktptr	/* Pointer to the packet	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	iqentry	*iptr;		/* Ptr. to network output queue	*/

	/* Ensure only one process accesses output queue at a time */

	mask = disable();

	/* Enqueue packet on network output queue */

	iptr = &ipoqueue;
	if (semcount(iptr->iqsem) >= IP_OQSIZ) {
		kprintf("ipout: output queue overflow\n");
		freebuf((char *)pktptr);
		restore(mask);
		return SYSERR;
	}
	iptr->iqbuf[iptr->iqtail++] = pktptr;
	if (iptr->iqtail >= IP_OQSIZ) {
		iptr->iqtail = 0;
	}
	signal(iptr->iqsem);
	restore(mask);
	return OK;	
}
