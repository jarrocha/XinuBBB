/* net.c - net_init, netin, eth_hton */

#include <xinu.h>
#include <stdio.h>

struct	network	NetData;
bpid32	netbufpool;
uint32	netportseed;

/*------------------------------------------------------------------------
 * net_init  -  Initialize network data structures and processes
 *------------------------------------------------------------------------
 */

void	net_init (void)
{
	int32	nbufs;			/* Total no of buffers		*/

	/* Initialize the network data structure */

	memset((char *)&NetData, NULLCH, sizeof(struct network));

	/* Obtain the Ethernet MAC address */

	control(ETHER0, ETH_CTRL_GET_MAC, (int32)NetData.ethucast, 0);

	memset((char *)NetData.ethbcast, 0xFF, ETH_ADDR_LEN);

	/* Initialize the random port seed */

	netportseed = getticks();

	/* Create the network buffer pool */

	nbufs = UDP_SLOTS * UDP_QSIZ + ICMP_SLOTS * ICMP_QSIZ + 1;

	netbufpool = mkbufpool(PACKLEN, nbufs);

	/* Initialize the ARP cache */

	arp_init();

	/* Initialize UDP */

	udp_init();

	/* Initialize ICMP */

	icmp_init();

	/* Initialize the IP output queue */

	ipoqueue.iqhead = 0;
	ipoqueue.iqtail = 0;
	ipoqueue.iqsem = semcreate(0);
	if((int32)ipoqueue.iqsem == SYSERR) {
		panic("Cannot create ip output queue semaphore");
		return;
	}

	/* Create the IP output process */

	resume(create(ipout, NETSTK, NETPRIO, "ipout", 0, NULL));

	/* Create a network input process */

	resume(create(netin, NETSTK, NETPRIO, "netin", 0, NULL));
}


/*------------------------------------------------------------------------
 * netin  -  Repeatedly read and process the next incoming packet
 *------------------------------------------------------------------------
 */

process	netin ()
{
	struct	netpacket *pkt;		/* Ptr to current packet	*/
	int32	retval;			/* Return value from read	*/

	/* Do forever: read a packet from the network and process */

	while(1) {

		/* Allocate a buffer */

		pkt = (struct netpacket *)getbuf(netbufpool);

		/* Obtain next packet that arrives */

		retval = read(ETHER0, (char *)pkt, PACKLEN);
		if(retval == SYSERR) {
			panic("Cannot read from Ethernet\n");
		}

		/* Convert Ethernet Type to host order */

		eth_ntoh(pkt);

		/* Demultiplex on Ethernet type */

		switch (pkt->net_ethtype) {

		    case ETH_ARP:			/* Handle ARP	*/
			arp_in((struct arppacket *)pkt);
			continue;

		    case ETH_IP:			/* Handle IP	*/
			ip_in(pkt);
			continue;
	
		    case ETH_IPv6:			/* Handle IPv6	*/
			freebuf((char *)pkt);
			continue;

		    default:	/* Ignore all other incoming packets	*/
			freebuf((char *)pkt);
			continue;
		}
	}
}

/*------------------------------------------------------------------------
 * eth_hton  -  Convert Ethernet type field to network byte order
 *------------------------------------------------------------------------
 */
void 	eth_hton(
	  struct netpacket *pktptr
	)
{
	pktptr->net_ethtype = htons(pktptr->net_ethtype);
}


/*------------------------------------------------------------------------
 * eth_ntoh  -  Convert Ethernet type field to host byte order
 *------------------------------------------------------------------------
 */
void 	eth_ntoh(
	  struct netpacket *pktptr
	)
{
	pktptr->net_ethtype = ntohs(pktptr->net_ethtype);
}

/*------------------------------------------------------------------------
 * getport  -  Retrieve a random port number 
 *------------------------------------------------------------------------
 */
uint16 	getport()
{
	netportseed = 1103515245 * netportseed + 12345;
	return 50000 + ((uint16)((netportseed >> 16)) % 15535);
}
