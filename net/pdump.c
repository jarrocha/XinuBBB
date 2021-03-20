/* pdump.c - packet dump function */

#include <xinu.h>

/*------------------------------------------------------------------------
 * pdump - dump a packet assuming all fields are in network byte order
 *------------------------------------------------------------------------
 */
void	pdump(struct  netpacket *pptr)
{
	struct	arppacket *aptr;

	kprintf("%02x:%02x:%02x:%02x:%02x:%02x >",
			pptr->net_ethsrc[0],
			pptr->net_ethsrc[1],
			pptr->net_ethsrc[2],
			pptr->net_ethsrc[3],
			pptr->net_ethsrc[4],
			pptr->net_ethsrc[5]
	       );
	kprintf(" %02x:%02x:%02x:%02x:%02x:%02x, ",
			pptr->net_ethdst[0],
			pptr->net_ethdst[1],
			pptr->net_ethdst[2],
			pptr->net_ethdst[3],
			pptr->net_ethdst[4],
			pptr->net_ethdst[5]
	       );

	kprintf("ethertype ");

	switch (ntohs(pptr->net_ethtype)) {

		case 0x0806:
			kprintf("ARP (0x%04x), length %d: ",
					ntohs(pptr->net_ethtype),
					sizeof(struct arppacket));
			aptr = (struct arppacket *)pptr;
			
			if ( (ntohs(aptr->arp_htype) != ARP_HTYPE) ||
			     (ntohs(aptr->arp_ptype) != ARP_PTYPE) ) {
				break;
			}

			kprintf("hard addr len=%d), proto addr len=%d), ",
					aptr->arp_hlen,
					aptr->arp_plen);

			if (ntohs(aptr->arp_op) == ARP_OP_REQ) {
				kprintf("Request who-has %d.%d.%d.%d tell %d.%d.%d.%d, ",
						(ntohl(aptr->arp_tarpa)>>24)&0xff,
						(ntohl(aptr->arp_tarpa)>>16)&0xff,
						(ntohl(aptr->arp_tarpa)>>8)&0xff,
						ntohl(aptr->arp_tarpa)&0xff,
						(ntohl(aptr->arp_sndpa)>>24)&0xff,
						(ntohl(aptr->arp_sndpa)>>16)&0xff,
						(ntohl(aptr->arp_sndpa)>>8)&0xff,
						ntohl(aptr->arp_sndpa)&0xff);
			} else if (ntohs(aptr->arp_op) == ARP_OP_RPLY) {
				kprintf("Reply %d.%d.%d.%d is-at %02x:%02x:%02x:%02x:%02x:%02x, ",
						(ntohl(aptr->arp_sndpa)>>24)&0xff,
						(ntohl(aptr->arp_sndpa)>>16)&0xff,
						(ntohl(aptr->arp_sndpa)>>8)&0xff,
						ntohl(aptr->arp_sndpa)&0xff,
						aptr->arp_sndha[0],
						aptr->arp_sndha[1],
						aptr->arp_sndha[2],
						aptr->arp_sndha[3],
						aptr->arp_sndha[4],
						aptr->arp_sndha[5]);
			} else {
				break;
			}

			kprintf("length %d\n", sizeof(struct arppacket) - ETH_HDR_LEN);
			break;

		case 0x0800:
			kprintf("IPv4 (0x%04x), length %d: ",
					ntohs(pptr->net_ethtype),
					ntohs(pptr->net_iplen) + ETH_HDR_LEN);
			
			kprintf("(");
			kprintf("tos 0x%01x, ttl %d, id %d, offset %d ",
					pptr->net_iptos,
					pptr->net_ipttl,
					ntohs(pptr->net_ipid),
					(ntohs(pptr->net_ipfrag) & 0x1fff));

			kprintf("flags ");
			if (ntohs(pptr->net_ipfrag) & 0x4000)
				kprintf("[DF], ");
			else if (ntohs(pptr->net_ipfrag) & 0x2000)
				kprintf("[MF], " );
			else
				kprintf("[none], ");

			kprintf("[ip checksum %s], ",
					ipcksum(pptr) == 0 ? "ok" : "fail");

			if (pptr->net_ipproto == IP_UDP) {
				kprintf("proto UDP (%d), length %d",
						pptr->net_ipproto, ntohs(pptr->net_iplen));
				kprintf(")\n");
				kprintf("\t%d.%d.%d.%d > ",
						(ntohl(pptr->net_ipsrc)>>24)&0xff,
						(ntohl(pptr->net_ipsrc)>>16)&0xff,
						(ntohl(pptr->net_ipsrc)>>8)&0xff,
						(ntohl(pptr->net_ipsrc)&0xff));
				kprintf("%d.%d.%d.%d: ",
						(ntohl(pptr->net_ipdst)>>24)&0xff,
						(ntohl(pptr->net_ipdst)>>16)&0xff,
						(ntohl(pptr->net_ipdst)>>8)&0xff,
						(ntohl(pptr->net_ipdst)&0xff));
				kprintf("[udp checksum none] ");
				kprintf("UDP, src port %d, dst port %d, length %d\n",
						ntohs(pptr->net_udpsport),
						ntohs(pptr->net_udpdport),
						ntohs(pptr->net_udplen) - UDP_HDR_LEN);
				break;
			} else if (pptr->net_ipproto == IP_ICMP) {
				kprintf("proto ICMP (%d), length %d",
						pptr->net_ipproto, ntohs(pptr->net_iplen));
				kprintf(")\n");
				kprintf("\t%d.%d.%d.%d > ",
						(ntohl(pptr->net_ipsrc)>>24)&0xff,
						(ntohl(pptr->net_ipsrc)>>16)&0xff,
						(ntohl(pptr->net_ipsrc)>>8)&0xff,
						(ntohl(pptr->net_ipsrc)&0xff));
				kprintf("%d.%d.%d.%d: ",
						(ntohl(pptr->net_ipdst)>>24)&0xff,
						(ntohl(pptr->net_ipdst)>>16)&0xff,
						(ntohl(pptr->net_ipdst)>>8)&0xff,
						(ntohl(pptr->net_ipdst)&0xff));

				if (pptr->net_ictype == ICMP_ECHOREQST)
					kprintf("ICMP echo request, ");
				else if (pptr->net_ictype == ICMP_ECHOREPLY)
					kprintf("ICMP echo reply, ");
				else
					break;

				kprintf("id %d, seq %d\n",
						ntohs(pptr->net_icident),
						ntohs(pptr->net_icseq));
				
				break;
			}
			else
				break;
		default:
			kprintf("unknown\n");
			break;

	}

	return;
}

/*------------------------------------------------------------------------
 * pdumph - dump a packet assuming all fields are in host byte order
 *------------------------------------------------------------------------
 */
void	pdumph(struct  netpacket *pptr)
{
	struct	arppacket *aptr;

	kprintf("%02x:%02x:%02x:%02x:%02x:%02x >",
			pptr->net_ethsrc[0],
			pptr->net_ethsrc[1],
			pptr->net_ethsrc[2],
			pptr->net_ethsrc[3],
			pptr->net_ethsrc[4],
			pptr->net_ethsrc[5]
	       );
	kprintf(" %02x:%02x:%02x:%02x:%02x:%02x, ",
			pptr->net_ethdst[0],
			pptr->net_ethdst[1],
			pptr->net_ethdst[2],
			pptr->net_ethdst[3],
			pptr->net_ethdst[4],
			pptr->net_ethdst[5]
	       );

	kprintf("ethertype ");

	switch (pptr->net_ethtype) {

		case 0x0806:
			kprintf("ARP (0x%04x), length %d: ",
					pptr->net_ethtype,
					sizeof(struct arppacket));
			aptr = (struct arppacket *)pptr;
			
			if ( (aptr->arp_htype != ARP_HTYPE) ||
			     (aptr->arp_ptype != ARP_PTYPE) ) {
				break;
			}

			kprintf("hard addr len=%d), proto addr len=%d), ",
					aptr->arp_hlen,
					aptr->arp_plen);

			if (aptr->arp_op == ARP_OP_REQ) {
				kprintf("Request who-has %d.%d.%d.%d tell %d.%d.%d.%d, ",
						(aptr->arp_tarpa>>24)&0xff,
						(aptr->arp_tarpa>>16)&0xff,
						(aptr->arp_tarpa>>8)&0xff,
						aptr->arp_tarpa&0xff,
						(aptr->arp_sndpa>>24)&0xff,
						(aptr->arp_sndpa>>16)&0xff,
						(aptr->arp_sndpa>>8)&0xff,
						aptr->arp_sndpa&0xff);
			} else if (aptr->arp_op == ARP_OP_RPLY) {
				kprintf("Reply %d.%d.%d.%d is-at %02x:%02x:%02x:%02x:%02x:%02x, ",
						(aptr->arp_sndpa>>24)&0xff,
						(aptr->arp_sndpa>>16)&0xff,
						(aptr->arp_sndpa>>8)&0xff,
						aptr->arp_sndpa&0xff,
						aptr->arp_sndha[0],
						aptr->arp_sndha[1],
						aptr->arp_sndha[2],
						aptr->arp_sndha[3],
						aptr->arp_sndha[4],
						aptr->arp_sndha[5]);
			} else {
				break;
			}

			kprintf("length %d\n", sizeof(struct arppacket) - ETH_HDR_LEN);
			break;

		case 0x0800:
			kprintf("IPv4 (0x%04x), length %d: ",
					pptr->net_ethtype,
					pptr->net_iplen + ETH_HDR_LEN);
			
			kprintf("(");
			kprintf("tos 0x%01x, ttl %d, id %d, offset %d ",
					pptr->net_iptos,
					pptr->net_ipttl,
					pptr->net_ipid,
					pptr->net_ipfrag & 0x1fff);

			kprintf("flags ");
			if (pptr->net_ipfrag & 0x4000)
				kprintf("[DF], ");
			else if (pptr->net_ipfrag & 0x2000)
				kprintf("[MF], " );
			else
				kprintf("[none], ");

			kprintf("[ip checksum %s], ",
					ipcksum(pptr) == 0 ? "ok" : "fail");

			if (pptr->net_ipproto == IP_UDP) {
				kprintf("proto UDP (%d), length %d",
						pptr->net_ipproto, pptr->net_iplen);
				kprintf(")\n");
				kprintf("\t%d.%d.%d.%d > ",
						(pptr->net_ipsrc>>24)&0xff,
						(pptr->net_ipsrc>>16)&0xff,
						(pptr->net_ipsrc>>8)&0xff,
						(pptr->net_ipsrc&0xff));
				kprintf("%d.%d.%d.%d: ",
						(pptr->net_ipdst>>24)&0xff,
						(pptr->net_ipdst>>16)&0xff,
						(pptr->net_ipdst>>8)&0xff,
						(pptr->net_ipdst&0xff));
				kprintf("[udp checksum none] ");
				kprintf("UDP, src port %d, dst port %d, length %d\n",
						pptr->net_udpsport,
						pptr->net_udpdport,
						pptr->net_udplen - UDP_HDR_LEN);
				break;
			} else if (pptr->net_ipproto == IP_ICMP) {
				kprintf("proto ICMP (%d), length %d",
						pptr->net_ipproto, pptr->net_iplen);
				kprintf(")\n");
				kprintf("\t%d.%d.%d.%d > ",
						(pptr->net_ipsrc>>24)&0xff,
						(pptr->net_ipsrc>>16)&0xff,
						(pptr->net_ipsrc>>8)&0xff,
						(pptr->net_ipsrc&0xff));
				kprintf("%d.%d.%d.%d: ",
						(pptr->net_ipsrc>>24)&0xff,
						(pptr->net_ipsrc>>16)&0xff,
						(pptr->net_ipsrc>>8)&0xff,
						(pptr->net_ipsrc&0xff));

				if (pptr->net_ictype == ICMP_ECHOREQST)
					kprintf("ICMP echo request, ");
				else if (pptr->net_ictype == ICMP_ECHOREPLY)
					kprintf("ICMP echo reply, ");
				else
					break;

				kprintf("id %d, seq %d\n",
						pptr->net_icident,
						pptr->net_icseq);
				kprintf("icmp ckeckcum %s\n",
					icmp_cksum((char *)&pptr->net_ictype,
					ntohs(pptr->net_iplen)-IP_HDR_LEN)==0?
					"OK":"failed");
				break;
			}
			else
				break;
		default:
			kprintf("unknown\n");
			break;

	}

	return;
}
