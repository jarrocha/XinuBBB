/* icmp.c - icmp_init, icmp_in, icmp_register, icmp_recv, icmp_send,	*/
/*		icmp_release, icmp_cksum, icmp_hton, icmp_ntoh		*/

#include <xinu.h>

struct	icmpentry icmptab[ICMP_SLOTS];	/* Table of processes using ping*/

/*------------------------------------------------------------------------
 * icmp_init  -  Initialize icmp table
 *------------------------------------------------------------------------
 */
void	icmp_init(void) {

	int32	i;			/* table index */

	for(i=0; i<ICMP_SLOTS; i++) {
		icmptab[i].icstate = ICMP_FREE;
	}
	return;
}

/*------------------------------------------------------------------------
 * icmp_in  -  Handle an incoming icmp packet
 *------------------------------------------------------------------------
 */
void	icmp_in(
	  struct netpacket *pkt		/* Pointer to incoming packet	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	int32	slot;			/* Slot in ICMP table		*/
	struct	icmpentry *icmptr;	/* Pointer to icmptab entry	*/
	struct	netpacket *replypkt;	/* Pointer to reply packet	*/

	mask = disable();

	/* Discard all ICMP messages except ping */

	if ( (pkt->net_ictype != ICMP_ECHOREPLY) &&
	     (pkt->net_ictype != ICMP_ECHOREQST)  )	{
		freebuf((char *)pkt);
		restore(mask);
		return;
	}

	/* Handle Echo Request message */

	if (pkt->net_ictype == ICMP_ECHOREQST) {

		/* Send echo reply message */

		replypkt = icmp_mkpkt(pkt->net_ipsrc,ICMP_ECHOREPLY,
				pkt->net_icident, pkt->net_icseq,
				(char *) &pkt->net_icdata,
				pkt->net_iplen-IP_HDR_LEN-ICMP_HDR_LEN);
		if ((int32)replypkt != SYSERR) {
			ip_enqueue(replypkt);
		}
		freebuf((char *)pkt);
		restore(mask);
		return;
	}

	/* Handle Echo Reply message: verify that ID is valid */

	slot = pkt->net_icident;
	if ( (slot < 0) || (slot >= ICMP_SLOTS) ) {
		freebuf((char *)pkt);
		restore(mask);
		return;
	}

	/* Verify that slot in table is in use and IP address	*/
	/*    in incomming packet matches IP address in table	*/

	icmptr = &icmptab[slot];
	if ( (icmptr->icstate == ICMP_FREE) ||
	     (pkt->net_ipsrc != icmptr->icremip) ) {
		freebuf((char *)pkt);	/* discard packet */
		restore(mask);
		return;
	}

	/* Add packet to queue */

	icmptr->iccount++;
	icmptr->icqueue[icmptr->ictail++] = pkt;
	if (icmptr->ictail >= ICMP_QSIZ) {
		icmptr->ictail = 0;
	}
	if (icmptr->icstate == ICMP_RECV) {
		icmptr->icstate = ICMP_USED;
		send (icmptr->icpid, OK);
	}
	restore(mask);
	return;
}

/*------------------------------------------------------------------------
 * icmp_register  -  Register a remote IP address for ping replies
 *------------------------------------------------------------------------
 */
int32	icmp_register (
	 uint32	remip			/* Remote IP address		*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	int32	i;			/* Index into icmptab		*/
	int32	freeslot;		/* Index of slot to use		*/
	struct	icmpentry *icmptr;	/* Pointer to icmptab entry	*/

	mask = disable();

	/* Find a free slot in the table */

	freeslot = -1;
	for (i=0; i<ICMP_SLOTS; i++) {
		icmptr = &icmptab[i];
		if (icmptr->icstate == ICMP_FREE) {
			if (freeslot == -1) {
				freeslot = i;
			}
		} else if (icmptr->icremip == remip) {
			restore(mask);
			return SYSERR;	/* Already registered */
		}
	}
	if (freeslot == -1) {  /* No free entries in table */

		restore(mask);
		return SYSERR;
	}

	/* Fill in table entry */

	icmptr = &icmptab[freeslot];
	icmptr->icstate = ICMP_USED;
	icmptr->icremip = remip;
	icmptr->iccount = 0;
	icmptr->ichead = icmptr->ictail = 0;
	icmptr->icpid = -1;
	restore(mask);
	return freeslot;
}

/*------------------------------------------------------------------------
 * icmp_recv  -  Receive an icmp echo reply packet
 *------------------------------------------------------------------------
 */
int32	icmp_recv (
	 int32	icmpid,			/* ICMP slot identifier		*/
	 char   *buff,			/* Buffer to ICMP data		*/
	 int32	len,			/* Length of buffer		*/
	 uint32	timeout			/* Time to wait in msec		*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	icmpentry *icmptr;	/* Pointer to icmptab entry	*/
	umsg32	msg;			/* Message from recvtime()	*/
	struct	netpacket *pkt;		/* Pointer to packet being read	*/
	int32	datalen;		/* Length of ICMP data area	*/
	char	*icdataptr;		/* Pointer to icmp data		*/
	int32	i;			/* Counter for data copy	*/

	/* Verify that the ID is valid */

	if ( (icmpid < 0) || (icmpid >= ICMP_SLOTS) ) {
		return SYSERR;
	}

	/* Insure only one process touches the table at a time */

	mask = disable();

	/* Verify that the ID has been registered and is idle */

	icmptr = &icmptab[icmpid];
	if (icmptr->icstate != ICMP_USED) {
		restore(mask);
		return SYSERR;
	}

	if (icmptr->iccount == 0) {		/* No packet is waiting */
		icmptr->icstate = ICMP_RECV;
		icmptr->icpid = currpid;
		msg = recvclr();
		msg = recvtime(timeout);	/* Wait for a reply */
		icmptr->icstate = ICMP_USED;
		if (msg == TIMEOUT) {
			restore(mask);
			return TIMEOUT;
		} else if (msg != OK) {
			restore(mask);
			return SYSERR;
		}
	}

	/* Packet has arrived -- dequeue it */

	pkt = icmptr->icqueue[icmptr->ichead++];
	if (icmptr->ichead >= ICMP_SLOTS) {
		icmptr->ichead = 0;
	}
	icmptr->iccount--;

	/* Copy data from ICMP message into caller's buffer */

	datalen = pkt->net_iplen - IP_HDR_LEN - ICMP_HDR_LEN;
	icdataptr = (char *) &pkt->net_icdata;
	for (i=0; i<datalen; i++) {
		if (i >= len) {
			break;
		}
		*buff++ = *icdataptr++;
	}
	freebuf((char *)pkt);
	restore(mask);
	return i;
}

/*------------------------------------------------------------------------
 * icmp_send  -  Send an icmp packet
 *------------------------------------------------------------------------
 */
status	icmp_send (
	 uint32	remip,			/* Remote IP address to use	*/
	 uint16	type,			/* ICMP type (req. or reply)	*/
	 uint16	ident,			/* ICMP identifier value	*/
	 uint16	seq,			/* ICMP sequence number		*/
	 char	*buf,			/* pointer to data buffer	*/
	 int32	len			/* Length of data in buffer	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	netpacket *pkt;		/* Packet returned by icmp_mkpkt*/
	int32	retval;			/* Value returned by ip_send	*/

	mask = disable();

	/* Form a packet to send */

	pkt = icmp_mkpkt(remip, type, ident, seq, buf, len);
	if ((int32)pkt == SYSERR) {
		return SYSERR;
	}

	/* Send the packet */

	retval = ip_send(pkt);
	restore(mask);
	return retval;
}


/*------------------------------------------------------------------------
 * icmp_mkpkt  -  Make an icmp packet by filling in fields
 *------------------------------------------------------------------------
 */
struct	netpacket *icmp_mkpkt (
	 uint32	remip,			/* Remote IP address to use	*/
	 uint16	type,			/* ICMP type (req. or reply)	*/
	 uint16	ident,			/* ICMP identifier value	*/
	 uint16	seq,			/* ICMP sequence number		*/
	 char	*buf,			/* Pointer to data buffer	*/
	 int32	len			/* Length of data in buffer	*/
	)
{
	struct	netpacket *pkt;		/* pointer to packet buffer	*/
	static	uint32	ipident=32767;	/* IP ident field		*/

	/* Allocate packet */

	pkt = (struct netpacket *)getbuf(netbufpool);

	if ((int32)pkt == SYSERR) {
		panic("icmp_mkpkt: cannot get a network buffer\n");
	}

	/* Create icmp packet in pkt */

	memcpy(pkt->net_ethsrc, NetData.ethucast, ETH_ADDR_LEN);
        pkt->net_ethtype = 0x800;	/* Type is IP */
	pkt->net_ipvh = 0x45;		/* IP version and hdr length	*/
	pkt->net_iptos = 0x00;		/* Type of service		*/
	pkt->net_iplen= IP_HDR_LEN+ICMP_HDR_LEN+len;/* datagram length	*/
	pkt->net_ipid = ipident++;	/* Datagram gets next IDENT	*/
	pkt->net_ipfrag = 0x0000;	/* IP flags & fragment offset	*/
	pkt->net_ipttl = 0xff;		/* IP time-to-live		*/
	pkt->net_ipproto = IP_ICMP;	/* Datagram carries icmp	*/
	pkt->net_ipcksum = 0x0000;	/* Initial checksum		*/
	pkt->net_ipsrc = NetData.ipucast; /* IP source address	*/
	pkt->net_ipdst = remip;		/* IP destination address	*/


	pkt->net_ictype = type;		/* ICMP type			*/
	pkt->net_iccode = 0;		/* Code is zero for ping	*/
	pkt->net_iccksum = 0x0000;	/* Temporarily zero the cksum	*/
	pkt->net_icident = ident;	/* ICMP identification		*/
	pkt->net_icseq = seq;		/* ICMP sequence number		*/
	memcpy(pkt->net_icdata, buf, len);

	/* Return packet to caller */

	return pkt;
}


/*------------------------------------------------------------------------
 * icmp_release  -  Release a previously-registered ICMP icmpid
 *------------------------------------------------------------------------
 */
status	icmp_release (
	 int32	icmpid			/* Slot in icmptab to release	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	icmpentry *icmptr;	/* Pointer to icmptab entry	*/
	struct	netpacket *pkt;		/* Pointer to packet		*/

	mask = disable();

	/* Check arg and insure entry in table is in use */

	if ( (icmpid < 0) || (icmpid >= ICMP_SLOTS) ) {
		restore(mask);
		return SYSERR;
	}
	icmptr = &icmptab[icmpid];
	if (icmptr->icstate != ICMP_USED) {
		restore(mask);
		return SYSERR;
	}

	/* Remove each packet from the queue and free the buffer */

	resched_cntl(DEFER_START);
	while (icmptr->iccount > 0) {
		pkt = icmptr->icqueue[icmptr->ichead++];
		if (icmptr->ichead >= ICMP_SLOTS) {
			icmptr->ichead = 0;

		}
		freebuf((char *)pkt);
		icmptr->iccount--;
	}

	/* Mark the entry free */

	icmptr->icstate = ICMP_FREE;
	resched_cntl(DEFER_STOP);
	restore(mask);
	return OK;
}

/*------------------------------------------------------------------------
 * icmp_cksum  -  Compute a checksum for a specified set of data bytes
 *------------------------------------------------------------------------
 */
uint16	icmp_cksum (
	 char	*buf,			/* Buffer of items for checksum	*/
	 int32	buflen			/* Size of buffer in bytes	*/
	)
{
	int32	scount;			/* Number of 16-bit values buf	*/
	uint32	cksum;			/* Checksum being computed	*/
	uint16	*sptr;			/* Walks along buffer		*/
	uint16	word;			/* One 16-bit word		*/

	/* Walk along buffer and sum all 16-bit values */

	scount = buflen >> 1;		/* Divide by 2 and round down	*/
	sptr = (uint16 *)buf;
	cksum = 0;
	for (; scount > 0; scount--) {
		word = (uint32) *sptr++;
		cksum += ntohs(word);
	}

	/* If buffer lenght is odd, add last byte */

	if ( (buflen & 0x01) !=0 ) {
		cksum += 0xFFFF & ((uint32) (*((byte *) sptr) << 8));
	}
	cksum += (cksum >> 16);
        cksum = 0xffff & ~cksum;
        return (uint16) (0xffff & cksum);
}


/*------------------------------------------------------------------------
 * icmp_hton  -  Convert ICMP ping fields to network byte order
 *------------------------------------------------------------------------
 */
void	icmp_hton (
	  struct netpacket *pktptr
	)
{
	pktptr->net_iccksum = htons(pktptr->net_iccksum);
	pktptr->net_icident = htons(pktptr->net_icident);
	pktptr->net_icseq = htons(pktptr->net_icseq);
}


/*------------------------------------------------------------------------
 * icmp_ntoh  -  Convert ICMP ping fields to host byte order
 *------------------------------------------------------------------------
 */
void	icmp_ntoh (
	  struct netpacket *pktptr
	)
{
	pktptr->net_iccksum = ntohs(pktptr->net_iccksum);
	pktptr->net_icident = ntohs(pktptr->net_icident);
	pktptr->net_icseq = ntohs(pktptr->net_icseq);
}
