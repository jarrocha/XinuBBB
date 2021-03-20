/* dns.h */

/* Constants and data structures used for DNS */

#define	DNSTIMEOUT	3000		/* Timeout for a response (ms)	*/
#define	DNSRETRY	3		/* Number of times to retry	*/
#define	DNSPORT		53		/* Well-known DNS UDP port	*/
#define	DNSLPORT	51525		/* Local UDP port to use	*/
#define	DNSDATASIZ	500		/* Size of the data area	*/


/* Format of a DNS Query/Response packet */

struct	dnspkt {
	uint16	id;			/* DNS Query ID			*/
	struct	{
		byte	rd:1;		/* Recursion Desired		*/
		byte	tc:1;		/* Truncation			*/
		byte	aa:1;		/* Authoritative Answer		*/
		byte	opcode:4;	/* Operation Code		*/
		byte	qr:1;		/* Query=0, Response=1		*/
	};
	struct	{
		byte	rcode:4;	/* Response Code		*/
		byte	z:3;		/* Reserved, must be 0		*/
		byte	ra:1;		/* Recursion Available		*/
	};
	uint16	qucount;		/* No. of Questions		*/
	uint16	ancount;		/* No. of RRs in Answer		*/
	uint16	nscount;		/* No of Name Server RRs	*/
	uint16	arcount;		/* No. of RRs in Additional info*/
	char	data[DNSDATASIZ];	/* DNS data area		*/
};

/* QType Values */

#define	DNS_QT_A	1		/* DNS Address Type (A)		*/
#define DNS_QT_NS	2		/* DNS Name Server Type		*/

/* QClass values */

#define	DNS_QC_IN	1		/* DNS QClass Internet		*/

/* Format of a DNS Question */

struct	dns_q {
	char	*qname;			/* Domain Name in the query	*/
	uint16	*qtype;			/* Question Type		*/
	uint16	*qclass;		/* Question Class		*/
};

/* Format of a DNS Resource Record */

struct	dns_rr {
	char	*rname;			/* Domain Name			*/
	uint16	*rtype;			/* Resource Record Type		*/
	uint16	*rclass;		/* Resource Record Class	*/
	uint32	*ttl;			/* Resource Record Time-to-Live	*/
	uint16	*rdlen;			/* Resource Record RD Length	*/
	char	*rdata;			/* Resource Record Data area	*/
};
