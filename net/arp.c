/* arp.c - arp_init, arp_resolve, arp_in, arp_alloc, arp_ntoh, arp_hton	*/

#include <xinu.h>

struct	arpentry  arpcache[ARP_SIZ];	/* ARP cache			*/

/*------------------------------------------------------------------------
 * arp_init  -  Initialize ARP cache for an Ethernet interface
 *------------------------------------------------------------------------
 */
void	arp_init(void)
{
	int32	i;			/* ARP cache index		*/

	for (i=1; i<ARP_SIZ; i++) {	/* Initialize cache to empty	*/
		arpcache[i].arstate = AR_FREE;
	}
}

/*------------------------------------------------------------------------
 * arp_resolve  -  Use ARP to resolve an IP address to an Ethernet address
 *------------------------------------------------------------------------
 */
status	arp_resolve (
	 uint32	nxthop,			/* Next-hop address to resolve	*/
	 byte	mac[ETH_ADDR_LEN]	/* Array into which Ethernet	*/
	)				/*   address should be placed	*/
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	arppacket apkt;		/* Local packet buffer		*/
	int32	i;			/* Index into arpcache		*/
	int32	slot;			/* ARP table slot to use	*/
	struct	arpentry  *arptr;	/* Ptr to ARP cache entry	*/
	int32	msg;			/* Message returned by recvtime	*/

	/* Use MAC broadcast address for IP limited broadcast */

	if (nxthop == IP_BCAST) {
		memcpy(mac, NetData.ethbcast, ETH_ADDR_LEN);
		return OK;
	}

	/* Use MAC broadcast address for IP network broadcast */

	if (nxthop == NetData.ipbcast) {
		memcpy(mac, NetData.ethbcast, ETH_ADDR_LEN);
		return OK;
	}

	/* Ensure only one process uses ARP at a time */

	mask = disable();

	/* See if next hop address is already present in ARP cache */

	for (i=0; i<ARP_SIZ; i++) {
		arptr = &arpcache[i];
		if (arptr->arstate == AR_FREE) {
			continue;
		}
		if (arptr->arpaddr == nxthop) { /* Adddress is in cache	*/
			break;
		}
	}

	if (i < ARP_SIZ) {	/* Entry was found */

		/* If entry is resolved - handle and return */

		if (arptr->arstate == AR_RESOLVED) {
			memcpy(mac, arptr->arhaddr, ARP_HALEN);
			restore(mask);
			return OK;
		}

		/* Entry is already pending -  return error because	*/
		/*	only one process can be	waiting at a time	*/

		if (arptr->arstate == AR_PENDING) {
			restore(mask);
			return SYSERR;
		}
	}

	/* IP address not in cache -  allocate a new cache entry and	*/
	/*	send an ARP request to obtain the answer		*/

	slot = arp_alloc();
	if (slot == SYSERR) {
		restore(mask);
		return SYSERR;
	}

	arptr = &arpcache[slot];
	arptr->arstate = AR_PENDING;
	arptr->arpaddr = nxthop;
	arptr->arpid = currpid;

	/* Hand-craft an ARP Request packet */

	memcpy(apkt.arp_ethdst, NetData.ethbcast, ETH_ADDR_LEN);
	memcpy(apkt.arp_ethsrc, NetData.ethucast, ETH_ADDR_LEN);
	apkt.arp_ethtype = ETH_ARP;	  /* Packet type is ARP		*/
	apkt.arp_htype = ARP_HTYPE;	  /* Hardware type is Ethernet	*/
	apkt.arp_ptype = ARP_PTYPE;	  /* Protocol type is IP	*/
	apkt.arp_hlen = 0xff & ARP_HALEN; /* Ethernet MAC size in bytes	*/
	apkt.arp_plen = 0xff & ARP_PALEN; /* IP address size in bytes	*/
	apkt.arp_op = 0xffff & ARP_OP_REQ;/* ARP type is Request	*/
	memcpy(apkt.arp_sndha, NetData.ethucast, ARP_HALEN);
	apkt.arp_sndpa = NetData.ipucast; /* IP address of interface	*/
	memset(apkt.arp_tarha, '\0', ARP_HALEN); /* Target HA is unknown*/
	apkt.arp_tarpa = nxthop;	  /* Target protocol address	*/

	/* Convert ARP packet from host to net byte order */

	arp_hton(&apkt);

	/* Convert Ethernet header from host to net byte order */

	eth_hton((struct netpacket *)&apkt);

	/* Send the packet ARP_RETRY times and await response */

	msg = recvclr();
	for (i=0; i<ARP_RETRY; i++) {
		write(ETHER0, (char *)&apkt, sizeof(struct arppacket));
		msg = recvtime(ARP_TIMEOUT);
		if (msg == TIMEOUT) {
			continue;
		} else if (msg == SYSERR) {
			restore(mask);
			return SYSERR;
		} else {	/* entry is resolved */
			break;
 		}
	}

	/* If no response, return TIMEOUT */

	if (msg == TIMEOUT) {
		arptr->arstate = AR_FREE;   /* Invalidate cache entry */
		restore(mask);
		return TIMEOUT;
	}

	/* Return hardware address */

	memcpy(mac, arptr->arhaddr, ARP_HALEN);
	restore(mask);
	return OK;
}


/*------------------------------------------------------------------------
 * arp_in  -  Handle an incoming ARP packet
 *------------------------------------------------------------------------
 */
void	arp_in (
	  struct arppacket *pktptr	/* Ptr to incoming packet	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	arppacket apkt;		/* Local packet buffer		*/
	int32	slot;			/* Slot in cache		*/
	struct	arpentry  *arptr;	/* Ptr to ARP cache entry	*/
	bool8	found;			/* Is the sender's address in	*/
					/*   the cache?			*/

	/* Convert packet from network order to host order */

	arp_ntoh(pktptr);

	/* Verify ARP is for IPv4 and Ethernet */

	if ( (pktptr->arp_htype != ARP_HTYPE) ||
	     (pktptr->arp_ptype != ARP_PTYPE) ) {
		freebuf((char *)pktptr);
		return;
	}

	/* Ensure only one process uses ARP at a time */

	mask = disable();

	/* Search cache for sender's IP address */

	found = FALSE;

	for (slot=0; slot < ARP_SIZ; slot++) {
		arptr = &arpcache[slot];

		/* Skip table entries that are unused */

		if (arptr->arstate == AR_FREE) {
			continue;
		}

		/* If sender's address matches, we've found it */

		if (arptr->arpaddr == pktptr->arp_sndpa) {
			found = TRUE;
			break;
		}
	}

	if (found) {

		/* Update sender's hardware address */

		memcpy(arptr->arhaddr, pktptr->arp_sndha, ARP_HALEN);

		/* If a process was waiting, inform the process */

		if (arptr->arstate == AR_PENDING) {
			/* Mark resolved and notify waiting process */
			arptr->arstate = AR_RESOLVED;
			send(arptr->arpid, OK);
		}
	}

	/* For an ARP reply, processing is complete */

	if (pktptr->arp_op == ARP_OP_RPLY) {
		freebuf((char *)pktptr);
		restore(mask);
		return;
	}

	/* The following is for an ARP request packet: if the local	*/
	/*  machine is not the target or the local IP address is not	*/
	/*  yet known, ignore the request (i.e., processing is complete)*/

	if ((!NetData.ipvalid) ||
			(pktptr->arp_tarpa != NetData.ipucast)) {
		freebuf((char *)pktptr);
		restore(mask);
		return;
	}

	/* Request has been sent to the local machine's address.  So,	*/
	/*   add sender's info to cache, if not already present		*/

	if (!found) {
		slot = arp_alloc();
		if (slot == SYSERR) {	/* Cache is full */
			kprintf("ARP cache overflow on interface\n");
			freebuf((char *)pktptr);
			restore(mask);
			return;
		}
		arptr = &arpcache[slot];
		arptr->arpaddr = pktptr->arp_sndpa;
		memcpy(arptr->arhaddr, pktptr->arp_sndha, ARP_HALEN);
		arptr->arstate = AR_RESOLVED;
	}

	/* Hand-craft an ARP reply packet and send back to requester	*/

	memcpy(apkt.arp_ethdst, pktptr->arp_sndha, ARP_HALEN);
	memcpy(apkt.arp_ethsrc, NetData.ethucast, ARP_HALEN);
	apkt.arp_ethtype= ETH_ARP;		/* Frame carries ARP	*/
	apkt.arp_htype	= ARP_HTYPE;		/* Hardware is Ethernet	*/
	apkt.arp_ptype	= ARP_PTYPE;		/* Protocol is IP	*/
	apkt.arp_hlen	= ARP_HALEN;		/* Ethernet address size*/
	apkt.arp_plen	= ARP_PALEN;		/* IP address size	*/
	apkt.arp_op	= ARP_OP_RPLY;		/* Type is Reply	*/

	/* Insert local Ethernet and IP address in sender fields	*/

	memcpy(apkt.arp_sndha, NetData.ethucast, ARP_HALEN);
	apkt.arp_sndpa = NetData.ipucast;

	/* Copy target Ethernet and IP addresses from request packet */

	memcpy(apkt.arp_tarha, pktptr->arp_sndha, ARP_HALEN);
	apkt.arp_tarpa = pktptr->arp_sndpa;

	/* Convert ARP packet from host to network byte order */

	arp_hton(&apkt);

	/* Convert the Ethernet header to network byte order */

	eth_hton((struct netpacket *)&apkt);

	/* Send the reply */

	write(ETHER0, (char *)&apkt, sizeof(struct arppacket));
	freebuf((char *)pktptr);
	restore(mask);
	return;
}

/*------------------------------------------------------------------------
 * arp_alloc  -  Find a free slot or kick out an entry to create one
 *------------------------------------------------------------------------
 */
int32	arp_alloc ()
{
	int32	slot;			/* Slot in ARP cache		*/

	/* Search for a free slot */

	for (slot=0; slot < ARP_SIZ; slot++) {
		if (arpcache[slot].arstate == AR_FREE) {
			memset((char *)&arpcache[slot],
					NULLCH, sizeof(struct arpentry));
			return slot;
		}
	}

	/* Search for a resolved entry */

	for (slot=0; slot < ARP_SIZ; slot++) {
		if (arpcache[slot].arstate == AR_RESOLVED) {
			memset((char *)&arpcache[slot],
					NULLCH, sizeof(struct arpentry));
			return slot;
		}
	}

	/* At this point, all slots are pending (should not happen) */

	kprintf("ARP cache size exceeded\n");

	return SYSERR;
}

/*------------------------------------------------------------------------
 * arp_ntoh  -  Convert ARP packet fields from net to host byte order
 *------------------------------------------------------------------------
 */
void 	arp_ntoh(
	struct arppacket *pktptr
	)
{
	pktptr->arp_htype = ntohs(pktptr->arp_htype);
	pktptr->arp_ptype = ntohs(pktptr->arp_ptype);
	pktptr->arp_op    = ntohs(pktptr->arp_op);
	pktptr->arp_sndpa = ntohl(pktptr->arp_sndpa);
	pktptr->arp_tarpa = ntohl(pktptr->arp_tarpa);
}

/*------------------------------------------------------------------------
 * arp_hton  -  Convert ARP packet fields from host to net byte order
 *------------------------------------------------------------------------
 */
void 	arp_hton(
	struct arppacket *pktptr
	)
{
	pktptr->arp_htype = htons(pktptr->arp_htype);
	pktptr->arp_ptype = htons(pktptr->arp_ptype);
	pktptr->arp_op    = htons(pktptr->arp_op);
	pktptr->arp_sndpa = htonl(pktptr->arp_sndpa);
	pktptr->arp_tarpa = htonl(pktptr->arp_tarpa);
}
