/* xsh_netinfo.c - xsh_netinfo */

#include <xinu.h>
#include <string.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * xsh_netinfo - obtain and print the IP address, subnet mask, default
 *			router address and other network information
 *------------------------------------------------------------------------
 */
shellcmd xsh_netinfo(int nargs, char *args[]) {

	uint32	ipaddr;			/* An IP address in binary	*/
	uint32	ipbcast;		/* IP broadcast addr. in binary	*/
	uint32	ipprefix;		/* IP network prefix in binary	*/
	uint32	router;			/* Router address in binary	*/
	uint32	tserver;		/* NTP server address in binary	*/
	uint32	dserver;		/* DNS server address in binary	*/
	char	str[40];		/* Temporary used for formatting*/
	uint32	ipmask;			/* Subnet mask in binary	*/

	/* Output info for '--help' argument */

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Usage: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\tDisplays IP address information\n");
		printf("Options:\n");
		printf("\t-f\tforce a new DHCP request\n");
		printf("\t--help\tdisplay this help and exit\n");
		return OK;
	}

	/* Check argument count */

	if (nargs > 2) {
		fprintf(stderr, "%s: too many arguments\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
			args[0]);
		return SYSERR;
	}

	if (nargs == 2) {
		if (strncmp(args[1], "-f", 3) != 0) {
			fprintf(stderr, "%s: invalid argument\n", args[0]);
			fprintf(stderr,
				"Try '%s --help' for more information\n",
				args[0]);
			return 1;
		}
		NetData.ipvalid = FALSE;
		getlocalip();
	}


	/* IP unicast address in dotted decimal and hex */

	ipaddr = NetData.ipucast;
	sprintf(str, "%d.%d.%d.%d",
		(ipaddr>>24)&0xff, (ipaddr>>16)&0xff,
		(ipaddr>>8)&0xff,        ipaddr&0xff);
	printf("   %-16s  %-16s  0x%08x\n",
		"IP address:", str, ipaddr);

	/* IP network broadcast address in dotted decimal & hex	*/

	ipbcast = NetData.ipbcast;
	sprintf(str, "%d.%d.%d.%d",
		(ipbcast>>24)&0xff, (ipbcast>>16)&0xff,
		(ipbcast>>8)&0xff,        ipbcast&0xff);
	printf("   %-16s  %-16s  0x%08x\n",
		"IP broadcast:", str, ipbcast);

	/* IP network prefix in dotted decimal & hex */

	ipprefix = NetData.ipprefix;
	sprintf(str, "%d.%d.%d.%d",
		(ipprefix>>24)&0xff, (ipprefix>>16)&0xff,
		(ipprefix>>8)&0xff,        ipprefix&0xff);
	printf("   %-16s  %-16s  0x%08x\n",
		"IP prefix:", str, ipprefix);

	/* IP network mask in dotted decimal & hex */

	ipmask = NetData.ipmask;
	ipaddr = NetData.ipucast;
	sprintf(str, "%d.%d.%d.%d",
		(ipmask>>24)&0xff, (ipmask>>16)&0xff,
		(ipmask>>8)&0xff,        ipmask&0xff);
	printf("   %-16s  %-16s  0x%08x\n",
		"Address mask:", str, ipmask);

	/* Default router in dotted decimal & hex */

	router = NetData.iprouter;
	sprintf(str, "%d.%d.%d.%d",
		(router>>24)&0xff, (router>>16)&0xff,
		(router>>8)&0xff,        router&0xff);
	printf("   %-16s  %-16s  0x%08x\n",
		"IP router:", str, router);

	/* NTP time server in dotted decimal & hex */

	tserver = NetData.ntpserver;
	if (tserver != 0) {
		sprintf(str, "%d.%d.%d.%d",
			(tserver>>24)&0xff, (tserver>>16)&0xff,
			(tserver>>8)&0xff,        tserver&0xff);
		printf("   %-16s  %-16s  0x%08x\n",
			"NTP time server:", str, tserver);
	}

	/* DNS server in dotted decimal & hex */

	dserver = NetData.dnsserver;
	if (dserver != 0) {
		sprintf(str, "%d.%d.%d.%d",
			(dserver>>24)&0xff, (dserver>>16)&0xff,
			(dserver>>8)&0xff,        dserver&0xff);
		printf("   %-16s  %-16s  0x%08x\n",
			"DNS server:", str, dserver);
	}

	printf("   %-16s  %02x:%02x:%02x:%02x:%02x:%02x\n",
	"MAC unicast:",
	0xff & NetData.ethucast[0],
	0xff & NetData.ethucast[1],
	0xff & NetData.ethucast[2],
	0xff & NetData.ethucast[3],
	0xff & NetData.ethucast[4],
	0xff & NetData.ethucast[5]);

	printf("   %-16s  %02x:%02x:%02x:%02x:%02x:%02x\n",
	"MAC broadcast:",
	0xff & NetData.ethbcast[0],
	0xff & NetData.ethbcast[1],
	0xff & NetData.ethbcast[2],
	0xff & NetData.ethbcast[3],
	0xff & NetData.ethbcast[4],
	0xff & NetData.ethbcast[5]);

	return OK;
}
