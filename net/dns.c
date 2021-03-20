/* dns.c - dnslookup, dns_bldq, dns_geta, dns_getrname */

#include <xinu.h>
#include <string.h>
#include <dns.h>

local	uint32	dns_bldq(char *, char *);
local	uint32	dns_geta(char *, struct dnspkt *);
local	uint32	dns_getrname(char *, char *, char *);

/*------------------------------------------------------------------------
 * dnslookup - Send a DNS Address Query and wait for the Response
 *------------------------------------------------------------------------
 */
uint32	dnslookup (
	char	*dname	/* Domain name to be resolved	*/
	)
{
	struct	dnspkt	qpkt;		/* Query Packet	buffer		*/
	struct	dnspkt	rpkt;		/* Response Packet buffer	*/
	uint32	nsaddr;			/* Name server IP address	*/
	uint32	qlen;			/* Query length			*/
	uid32	slot;			/* UDP Slot			*/
	int32	rlen;			/* Response length		*/
	uint32	ipaddr;			/* IP address from response	*/
	int32	retval;			/* Return value			*/
	int32	i;			/* Loop index			*/

	/* Check if we have a valid name pointer */

	if(dname == NULL) {
		return (uint32)SYSERR;
	}

	/* Obtain the IP address of a DNS server */

	retval = getlocalip();
	nsaddr = NetData.dnsserver;
	if ( (retval == SYSERR) || (NetData.dnsserver == 0) ) {
			kprintf("Cannot find a DNS server\n");
			return (uint32)SYSERR;
	}

	/* Register a UDP Slot */

	slot = udp_register(nsaddr, DNSPORT, DNSLPORT);
	if(slot == SYSERR) {
		return (uint32)SYSERR;
	}

	/* Build the Query message */

	memset((char *)&qpkt, 0, sizeof(struct dnspkt));

	qpkt.id = currpid;
	qpkt.rd = 1;
	qpkt.qucount = htons(1);

	qlen = dns_bldq(dname, qpkt.data);

	ipaddr = (uint32)SYSERR;
	for(i = 0; (ipaddr==(uint32)SYSERR) && (i < DNSRETRY); i++) {

		/* Send the Query message */

		udp_send(slot, (char*)&qpkt, qlen);

		/* Wait for the response */

		rlen = udp_recv(slot, (char*)&rpkt, sizeof(struct dnspkt),
						DNSTIMEOUT);
		if ( (rlen == SYSERR) || (rlen == TIMEOUT) ) {
			continue;
		}
		ipaddr = dns_geta(dname, &rpkt);
	}
	udp_release(slot);
	return ntohl(ipaddr);
}

/*------------------------------------------------------------------------
 * dns_bldq - Build a DNS Question and return the length of the packet
 *------------------------------------------------------------------------
 */
uint32	dns_bldq (
	 char	*dname,			/* Domain Name			*/
	 char	*data			/* Pointer to buffer for data	*/
	)
{
	uint32	qlen;			/* Length of Question		*/
	uint32	dlen;			/* Length of domain name	*/
	byte	*llptr;			/* Label length pointer		*/
	int32	i;			/* Loop index			*/
	uint16	tmp;			/* Temporary for conversion	*/

	/* Get length of the domain name */

	dlen = strlen(dname);

	/* Allocate a length byte for the next label and start at zero	*/

	llptr = (byte*)(data++);
	*llptr = 0;

	/* Initialize the query length */

	qlen = 1;

	/* Add each character in the Domain Name to the question */

	for(i = 0; i < dlen; i++) {

		if(qlen >= DNSDATASIZ) {
			return (uint32)SYSERR;
		}
		if(dname[i] != '.') {
			/* Add normal character to the existing label	*/
			*data++ = dname[i];
			*llptr = *llptr + 1;
		} else {
			/* Dot means we should start a new label */
			llptr = (byte*)(data++);
			*llptr = 0;
		}
		qlen++;
	}

	/* Terminate the query with a zero length */

	*data++ = 0;
	qlen++;

	/* Set the Qtype to a Type A Address */

	tmp = htons(DNS_QT_A);
	memcpy(data, (char *)&tmp, 2);
	data += 2;
	qlen += 2;

	/* Set the QClass  to Internet */

	tmp = htons(DNS_QC_IN);
	memcpy(data, (char *)&tmp, 2);
	qlen += 2;

	/* Return the total packet length */

	return sizeof(struct dnspkt) - DNSDATASIZ + qlen;
}

/*------------------------------------------------------------------------
 * dns_geta - return the best IP address in the Answer Section
 *------------------------------------------------------------------------
 */
uint32	dns_geta (
	char	*dname,			/* Domain Name			*/
	struct	dnspkt *rpkt		/* Pointer to a response packet	*/
	)
{
	uint16	qcount;			/* Number of Questions		*/
	uint16	tmp16;			/* Used for endian conversion	*/
	uint16	acount;			/* Number of Answers		*/
	uint32	ipaddr;			/* IP address from the response	*/
	char	*dptr;			/* Data pointer			*/
	byte	llen;			/* Label length			*/
	int32	i;			/* Loop index			*/

	/* Pick up the count of questions */

	memcpy((char *)&tmp16, (char *) &rpkt->qucount, 2);
	qcount = ntohs(tmp16);
	dptr = rpkt->data;

	/* Skip qcount questions */

	for(i = 0; i < qcount; i++) {

		/* Get the label length */

		llen = *((byte *)dptr);

		/* While we haven't reached the end of this domain name	*/

		while(llen != 0) {

			/* Pointer means end of name */

			if(llen > 63) {
				dptr += 2;
				break;
			}

			/* Move to the next label */

			dptr += (llen + 1);
			llen = *((byte *)dptr);
		}

		/* Move to next question */

		if (llen == 0) {
			dptr += 1;
		}

		/* Skip the type and class */

		dptr += (2 + 2);
	}

	/* Pick up the count of answers */

	memcpy((char *)&tmp16, (char *)&rpkt->ancount, 2);
	acount = ntohs(tmp16);

	/* Set the IP address to zero */

	ipaddr = 0;

	/* Check each answer to see if it matches the local net */

	for(i = 0; i < acount; i++) {

		char	rname[1024];
		uint16	tmptype;
		uint32	tmpip;
		uint16	tmplen;
		uint32	namlen;

		/* Get the name, and the number of bytes it occupied	*/
		/*    in the packet (may be short if the name contains	*/
		/*    pointer (e.g.,back to the question)		*/

		namlen = dns_getrname( (char *)rpkt, dptr, rname);
		dptr += namlen;

		/* Verify that the answer matches and is Type A */

		memcpy((char *)&tmptype, dptr, 2);
		if( (strncmp(dname, rname, strlen(dname)) == 0) &&
		    (ntohs(tmptype) == DNS_QT_A) ) {

			/* Pick up the IP address */

			memcpy((char *)&tmpip, dptr+10, 4);
			if ((ipaddr == 0) ||
				    ((NetData.ipmask&ntohl(tmpip)) ==
					NetData.ipprefix) ) {
				ipaddr = tmpip;
			}
		}


		/* Move past the type, class, and ttl fields to the length */

		dptr += 8;

		/* Move past the length field itself (2 bytes) and data */

		memcpy((char *)&tmplen, dptr, 2);
		dptr += ntohs(tmplen) + 2;
	}

	if (ipaddr != 0) {
		return ipaddr;
	} else {
		return (uint32)SYSERR;
	}
}

/*------------------------------------------------------------------------
 * dns_getrname - Convert a domain name in a RR to a null-terminted string
 *------------------------------------------------------------------------
 */
uint32	dns_getrname (
	char	*sop,			/* Start of Packet		*/
	char	*son,			/* Start of Name		*/
	char	*dst			/* Destination buffer		*/
	)
{
	byte	llen;			/* Label length			*/
	uint16	tmpoff;			/* Temporary to hold offset	*/
	uint16	offset;			/* Offset in host byte order	*/
	char	*sson;			/* Saved start of name		*/
	int32	i;			/* Loop index			*/

	/* Record initial position */

	sson = son;

	/* Pick up length of initial label */

	llen = *son++;

	/* While not at the end of the name, pick up a label */

	while(llen != 0) {

		if(llen <= 63) {
			/* Copy the label into the destination string */
			for(i = 0; i < llen; i++) {
				*dst++ = *son++;
			}
			*dst++ = '.';
			llen = *son++;
		} else {
			/* Handle pointer to remainder of name */
			son--;
			memcpy( (char *)&tmpoff, son, 2);
			offset = ntohs(tmpoff) & 0x3fff;
			son += 2;
			dns_getrname(sop, sop+offset, dst);
			return (son-sson);
		}
	}

	/* Null-terminate the string */

	dst--;
	*dst = NULLCH;

	return (uint32)(son-sson);
}
