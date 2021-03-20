/* xsh_arp.c - xsh_arp */

#include <xinu.h>
#include <stdio.h>
#include <string.h>

static	void	arp_dmp();
/*------------------------------------------------------------------------
 * xsh_arp - display the current ARP cache for an interface
 *------------------------------------------------------------------------
 */
shellcmd xsh_arp(int nargs, char *args[])
{
	/* For argument '--help', emit help about the 'arp' command	*/

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\tDisplays information from the ARP cache\n");
		printf("Options:\n");
		printf("\t--help\t display this help and exit\n");
		return 0;
	}

	/* Dump the Entire ARP cache */
	printf("\n");
	arp_dmp();

	return 0;
}


/*------------------------------------------------------------------------
 * arp_dmp - dump the ARP cache
 *------------------------------------------------------------------------
 */
static	void arp_dmp ()
{
	int32	i, j;			/* index into the ARP table	*/
	struct	arpentry *arptr;	/* pointer to entry in cache	*/

	/* Print entries from the ARP table */

	printf("ARP cache:\n");
	printf("   State Pid    IP Address    Hardware Address\n");
	printf("   ----- --- --------------- -----------------\n");
	for (i = 0; i < ARP_SIZ; i++) {
		arptr = &arpcache[i];
		if (arptr->arstate == AR_FREE) {
			continue;
		}
		switch(arptr->arstate) {
		    case AR_PENDING:	printf("   PEND "); break;
		    case AR_RESOLVED:	printf("   RESLV"); break;
		    default:		printf("   ?????"); break;
		}
		if (arptr->arstate == AR_PENDING) {
			printf("%4d ", arptr->arpid);
		} else {
			printf("     ");
		}
		printf("%3d.", (arptr->arpaddr & 0xFF000000) >> 24);
		printf("%3d.", (arptr->arpaddr & 0x00FF0000) >> 16);
		printf("%3d.", (arptr->arpaddr & 0x0000FF00) >> 8);
		printf("%3d",  (arptr->arpaddr & 0x000000FF));

		printf(" %02X", arptr->arhaddr[0]);
		for (j = 1; j < ARP_HALEN; j++) {
			printf(":%02X", arptr->arhaddr[j]);
		}
		printf("\n");
	}
	printf("\n");
	return;
}
