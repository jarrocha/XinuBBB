/* arp.h */

/* Items related to ARP - definition of cache and the packet format */

#define	ARP_HALEN	6		/* Size of Ethernet MAC address	*/
#define	ARP_PALEN	4		/* Size of IP address		*/

#define	ARP_HTYPE	1		/* Ethernet hardware type	*/
#define ARP_PTYPE	0x0800		/* IP protocol type		*/

#define ARP_OP_REQ	1		/* Request op code		*/
#define ARP_OP_RPLY	2		/* Reply op code		*/

#define	ARP_SIZ		16		/* Number of entries in a cache	*/

#define	ARP_RETRY	3		/* Num. retries for ARP request	*/

#define	ARP_TIMEOUT	300		/* Retry timer in milliseconds	*/

/* State of an ARP cache entry */

#define	AR_FREE		0		/* Slot is unused		*/
#define	AR_PENDING	1		/* Resolution in progress	*/
#define	AR_RESOLVED	2		/* Entry is valid		*/

#pragma pack(2)
struct	arppacket {			/* ARP packet for IP & Ethernet	*/
	byte	arp_ethdst[ETH_ADDR_LEN];/* Ethernet dest. MAC addr	*/
	byte	arp_ethsrc[ETH_ADDR_LEN];/* Ethernet source MAC address */
	uint16	arp_ethtype;		/* Ethernet type field		*/
	uint16	arp_htype;		/* ARP hardware type		*/
	uint16	arp_ptype;		/* ARP protocol type		*/
	byte	arp_hlen;		/* ARP hardware address length	*/
	byte	arp_plen;		/* ARP protocol address length	*/
	uint16	arp_op;			/* ARP operation		*/
	byte	arp_sndha[ARP_HALEN];	/* ARP sender's Ethernet addr 	*/
	uint32	arp_sndpa;		/* ARP sender's IP address	*/
	byte	arp_tarha[ARP_HALEN];	/* ARP target's Ethernet addr	*/
	uint32	arp_tarpa;		/* ARP target's IP address	*/
};
#pragma pack()

struct	arpentry {			/* Entry in the ARP cache	*/
	int32	arstate;		/* State of the entry		*/
	uint32	arpaddr;		/* IP address of the entry	*/
	pid32	arpid;			/* Waiting process or -1 	*/
	byte	arhaddr[ARP_HALEN];	/* Ethernet address of the entry*/
};

extern struct	arpentry arpcache[];
