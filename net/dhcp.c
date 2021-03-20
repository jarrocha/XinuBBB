/* dhcp.c - getlocalip */

#include <xinu.h>

/*------------------------------------------------------------------------
 * dhcp_get_opt_val  -	Retrieve a pointer to the value for a specified
 *			  DHCP options key
 *------------------------------------------------------------------------
 */
char* 	dhcp_get_opt_val(
	  const struct dhcpmsg* dmsg, 	/* DHCP Message			*/
	  uint32 dmsg_size, 		/* Size of DHCP Message		*/
	  uint8 option_key		/* Option key to retrieve	*/
	)
{
	unsigned char* opt_tmp;
	unsigned char* eom;

	eom = (unsigned char*)dmsg + dmsg_size - 1;
	opt_tmp = (unsigned char*)dmsg->dc_opt;

	while(opt_tmp < eom) {

		/* If the option value matches return the value */

		if((*opt_tmp) == option_key) {

			/* Offset past the option value and the size 	*/

			return (char*)(opt_tmp+2);
		}
		opt_tmp++;	/* Move to length octet */
		opt_tmp += *(uint8*)opt_tmp + 1;
	}

	/* Option value not found */

	return NULL;
}

/*------------------------------------------------------------------------
 * dhcp_bld_bootp_msg  -  Set the common fields for all DHCP messages
 *------------------------------------------------------------------------
 */
void 	dhcp_bld_bootp_msg(struct dhcpmsg* dmsg)
{
	uint32	xid;			/* Xid used for the exchange	*/

	memcpy(&xid, NetData.ethucast, 4); /* Use 4 bytes from MAC as	*/
					   /*    unique XID		*/
	memset(dmsg, 0x00, sizeof(struct dhcpmsg));

	dmsg->dc_bop = 0x01;	     	/* Outgoing request		*/
	dmsg->dc_htype = 0x01;		/* Hardware type is Ethernet	*/
	dmsg->dc_hlen = 0x06;		/* Hardware address length	*/
	dmsg->dc_hops = 0x00;		/* Hop count			*/
	dmsg->dc_xid = htonl(xid);	/* Xid (unique ID)		*/
	dmsg->dc_secs = 0x0000;		/* Seconds			*/
	dmsg->dc_flags = 0x0000;	/* Flags			*/
	dmsg->dc_cip = 0x00000000;	/* Client IP address		*/
	dmsg->dc_yip = 0x00000000;	/* Your IP address		*/
	dmsg->dc_sip = 0x00000000;	/* Server IP address		*/
	dmsg->dc_gip = 0x00000000;	/* Gateway IP address		*/
	memset(&dmsg->dc_chaddr,'\0',16);/* Client hardware address	*/
	memcpy(&dmsg->dc_chaddr, NetData.ethucast, ETH_ADDR_LEN);
	memset(&dmsg->dc_bootp,'\0',192);/* Zero the bootp area		*/
	dmsg->dc_cookie = htonl(0x63825363); /* Magic cookie for DHCP	*/
}

/*------------------------------------------------------------------------
 * dhcp_bld_disc  -  handcraft a DHCP Discover message in dmsg
 *------------------------------------------------------------------------
 */
int32 	dhcp_bld_disc(struct dhcpmsg* dmsg)
{
	uint32  j = 0;

	dhcp_bld_bootp_msg(dmsg);

	dmsg->dc_opt[j++] = 0xff & 53;	/* DHCP message type option	*/
	dmsg->dc_opt[j++] = 0xff &  1;	/* Option length		*/
	dmsg->dc_opt[j++] = 0xff &  1;	/* DHCP Dicover message		*/
	dmsg->dc_opt[j++] = 0xff &  0;	/* Options padding		*/

	dmsg->dc_opt[j++] = 0xff & 55;	/* DHCP parameter request list	*/
	dmsg->dc_opt[j++] = 0xff &  2;	/* Option length		*/
	dmsg->dc_opt[j++] = 0xff &  1;	/* Request subnet mask 		*/
	dmsg->dc_opt[j++] = 0xff &  3;	/* Request default router addr->*/
	dmsg->dc_opt[j++] = 0xff;	/* End of options		*/

	return (uint32)((char *)&dmsg->dc_opt[j] - (char *)dmsg + 1);
}

/*------------------------------------------------------------------------
 * dhcp_bld_req - handcraft a DHCP request message in dmsg
 *------------------------------------------------------------------------
 */
int32 	dhcp_bld_req(
	  struct dhcpmsg* dmsg,		/* DHCP message to build	*/
	  const struct dhcpmsg* dmsg_offer, /* DHCP offer message	*/
	  uint32 dsmg_offer_size	/* Size of DHCP offer message	*/
	)
{
	uint32  j = 0;
	uint32* server_ip;        	/* Take the DHCP server IP addr	*/
					/*   from DHCP offer message	*/

	dhcp_bld_bootp_msg(dmsg);
	dmsg->dc_sip = dmsg_offer->dc_sip; /* Server IP address		*/
	dmsg->dc_opt[j++] = 0xff & 53;	/* DHCP message type option	*/
	dmsg->dc_opt[j++] = 0xff &  1;	/* Option length		*/
	dmsg->dc_opt[j++] = 0xff &  3;	/* DHCP Request message		*/
	dmsg->dc_opt[j++] = 0xff &  0;	/* Options padding		*/

	dmsg->dc_opt[j++] = 0xff & 50;	/* Requested IP			*/
	dmsg->dc_opt[j++] = 0xff &  4;	/* Option length		*/
	memcpy(((void *)&dmsg->dc_opt[j]), &dmsg_offer->dc_yip, 4);
	j += 4;

	/* Retrieve the DHCP server IP from the DHCP options */
	server_ip = (uint32*)dhcp_get_opt_val(dmsg_offer,
				    dsmg_offer_size, DHCP_SERVER_ID);

	if(server_ip == 0) {
		kprintf("Unable to get server IP add. from DHCP Offer\n");
		return SYSERR;
	}

	dmsg->dc_opt[j++] = 0xff & 54;	/* Server IP			*/
	dmsg->dc_opt[j++] = 0xff &  4;	/* Option length		*/
	memcpy((void *)&dmsg->dc_opt[j], server_ip, 4);
	j += 4;
	dmsg->dc_opt[j++] = 0xff;	/* End of options		*/

	return (uint32)((char *)&dmsg->dc_opt[j] - (char *)dmsg + 1);
}

/*------------------------------------------------------------------------
 * getlocalip - use DHCP to obtain an IP address
 *------------------------------------------------------------------------
 */
uint32	getlocalip(void)
{
	int32	slot;			/* UDP slot to use		*/
	struct	dhcpmsg dmsg_snd;	/* Holds outgoing DHCP messages	*/
	struct	dhcpmsg dmsg_rvc;	/* Holds incoming DHCP messages	*/

	int32	i, j;			/* Retry counters		*/
	int32	len;			/* Length of data sent		*/
	int32	inlen;			/* Length of data received	*/
	char	*optptr;		/* Pointer to options area	*/
	char	*eop;			/* Address of end of packet	*/
	int32	msgtype;		/* Type of DCHP message		*/
	uint32	addrmask;		/* Address mask for network	*/
	uint32	routeraddr;		/* Default router address	*/
	uint32	dnsaddr;		/* DNS server address		*/
	uint32	ntpaddr;		/* NTP server address		*/
	uint32	tmp;			/* Used for byte conversion	*/
	uint32* tmp_server_ip;		/* Temporary DHCP server pointer*/

	slot = udp_register(0, UDP_DHCP_SPORT, UDP_DHCP_CPORT);
	if (slot == SYSERR) {
		kprintf("getlocalip: cannot register with UDP\n");
		return SYSERR;
	}

	len = dhcp_bld_disc(&dmsg_snd);
	if(len == SYSERR) {
		kprintf("getlocalip: Unable to build DHCP discover\n");
		return SYSERR;
	}

	for (i = 0; i < DHCP_RETRY; i++) {
	    udp_sendto(slot, IP_BCAST, UDP_DHCP_SPORT,
						(char *)&dmsg_snd, len);

	    /* Read 3 incoming DHCP messages and check for an offer	*/
	    /* 	or wait for three timeout periods if no message */
	    /* 	arrives.					*/

	    for (j=0; j<3; j++) {
		inlen = udp_recv(slot, (char *)&dmsg_rvc,
				    sizeof(struct dhcpmsg), 2000);
		if (inlen == TIMEOUT) {
			continue;
		} else if (inlen == SYSERR) {
			return SYSERR;
		}
		/* Check that incoming message is a valid	*/
		/*   response (ID matches our request)		*/

		if (dmsg_rvc.dc_xid != dmsg_snd.dc_xid) {
			continue;
		}

		eop = (char *)&dmsg_rvc + inlen - 1;
		optptr = (char *)&dmsg_rvc.dc_opt;
		msgtype = addrmask = routeraddr = dnsaddr = ntpaddr = 0;

		while (optptr < eop) {

		    switch (*optptr) {
			case 53:	/* Message type */
				msgtype = 0xff & *(optptr+2);
				break;

			case 1:		/* Subnet mask */
				memcpy((void *)&tmp, optptr+2, 4);
				addrmask = ntohl(tmp);
				break;

			case 3:		/* Router address */
				memcpy((void *)&tmp, optptr+2, 4);
				routeraddr = ntohl(tmp);
				break;

			case 6:		/* DNS server address */
				memcpy((void *)&tmp, optptr+2, 4);
				dnsaddr = ntohl(tmp);
				break;

			case 42:	/* NTP server address */
				memcpy((void *)&tmp, optptr+2, 4);
				ntpaddr = ntohl(tmp);
				break;

		    }
		    optptr++;	/* Move to length octet */
		    optptr += (0xff & *optptr) + 1;
		}

		if (msgtype == 0x02) {	/* Offer - send request	*/
			len = dhcp_bld_req(&dmsg_snd, &dmsg_rvc, inlen);
			if(len == SYSERR) {
				kprintf("getlocalip: %s\n",
				  "Unable to build DHCP request");
				return SYSERR;
			}
			udp_sendto(slot, IP_BCAST, UDP_DHCP_SPORT,
					(char *)&dmsg_snd, len);
			continue;

		} else if (dmsg_rvc.dc_opt[2] != 0x05) {
			/* If not an ack skip it */
			continue;
		}
		if (addrmask != 0) {
			NetData.ipmask = addrmask;
		}
		if (routeraddr != 0) {
			NetData.iprouter = routeraddr;
		}
		if (dnsaddr != 0) {
			NetData.dnsserver = dnsaddr;
		}
		if (ntpaddr != 0) {
			NetData.ntpserver = ntpaddr;
		}

		NetData.ipucast = ntohl(dmsg_rvc.dc_yip);
		NetData.ipprefix = NetData.ipucast & NetData.ipmask;
		NetData.ipbcast = NetData.ipprefix | ~NetData.ipmask;
		NetData.ipvalid = TRUE;
		udp_release(slot);

		/* Retrieve the boot server IP */
		if (dot2ip((char*)dmsg_rvc.sname,
					    &NetData.bootserver) != OK) {
			/* Could not retrieve the boot server from	*/
			/*  the  BOOTP fields, so use the DHCP server	*/
			/*  address					*/
			tmp_server_ip = (uint32*)dhcp_get_opt_val(
					&dmsg_rvc, len, DHCP_SERVER_ID);
			memcpy((char *)&tmp, tmp_server_ip, 4);
			NetData.bootserver = ntohl(tmp);
		}
		memcpy(NetData.bootfile, dmsg_rvc.bootfile,
					     sizeof(dmsg_rvc.bootfile));
		return NetData.ipucast;
	    }
	}

	kprintf("DHCP failed to get a response\n");
	udp_release(slot);
	return (uint32)SYSERR;
}

