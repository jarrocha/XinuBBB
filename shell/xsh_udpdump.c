/* xsh_udpdump.c - xsh_udpdump */

#include <xinu.h>
#include <stdio.h>
#include <string.h>

/*------------------------------------------------------------------------
 * xsh_udpdump - shell command to dump the UDP registered ports
 *------------------------------------------------------------------------
 */
shellcmd xsh_udpdump(int nargs, char *args[])
{
	int32	i;			/* index into udptab		*/
	char	*udpstate[] = {		/* names for entry states	*/
		"free ", "used ", "recv "};
	struct	udpentry *uptr;		/* ptr to entry in udptab	*/
	uint32	remip;			/* variables to hold the info	*/
	int32	r1,r2,r3,r4;		/* from an entry for printing	*/
	int32	remprt, locprt;
	int32	state;
	pid32	pid;


	/* For argument '--help', emit help about the 'udpdump' command	*/

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\tDisplays registerd UDP ports\n");
		printf("Options:\n");
		printf("\t--help\t display this help and exit\n");
		return 0;
	}

	/* Check for valid number of arguments */

	if (nargs > 1) {
		fprintf(stderr, "%s: no argumentd expected\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
				args[0]);
		return 1;
	}

	/* Print header for items from UDP table */

	printf("%5s %5s %5s    %9s    %8s %8s %3s %4s\n",
		"Entry", "Iface", "State", "Remote IP", "Rem Port",
		"Loc Port", "Pid", "Pkts");
	printf("%5s %5s %5s %15s %8s %8s %3s %4s\n",
		"-----", "-----", "-----", "---------------", "--------",
		"--------", "---", "----");

	/* Output information for each valid entry in udptab */
	for (i = 0; i < UDP_SLOTS; i++) {
	    uptr = &udptab[i];
	    if (uptr->udstate == UDP_FREE) {  /* skip unused slots	*/
		printf("%3d   ---- slot is free ---\n", i);
		continue;
	    }
	    remip = uptr->udremip;
	    r1 = (remip >> 24) & 0xff;
	    r2 = (remip >> 16) & 0xff;
	    r3 = (remip >>  8) & 0xff;
	    r4 = (remip      ) & 0xff;
	    remprt = uptr->udremport;
	    locprt = uptr->udlocport;
	    pid = uptr->udpid;
	    state = uptr->udstate;
	    printf("  %4s %3d.%3d.%3d.%3d %5d    %6d%5d%6d\n",
	      udpstate[state], r1, r2, r3, r4, remprt, locprt, pid,
							uptr->udcount);
	}
	return 0;
}
