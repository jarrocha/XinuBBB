/* xsh_uptime.c - xsh_uptime */

#include <xinu.h>
#include <stdio.h>
#include <string.h>

/*------------------------------------------------------------------------
 * xsh_uptime - shell to print the time the system has been up
 *------------------------------------------------------------------------
 */
shellcmd xsh_uptime(int nargs, char *args[])
{
	uint32	days, hrs, mins, secs;	/* days, hours, minutes,  and	*/
					/*  seconds since system boot	*/
	uint32	secperday = 86400;	/* seconds in a day		*/
	uint32	secperhr  =  3600;	/* seconds in an hour		*/
	uint32	secpermin =    60;	/* seconds in a minute		*/	

	/* For argument '--help', emit help about the 'uptime' command	*/

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\tDisplays time since the system booted\n");
		printf("Options:\n");
		printf("\t--help\t display this help and exit\n");
		return 0;
	}

	/* Check for valid number of arguments */

	if (nargs > 1) {
		fprintf(stderr, "%s: too many arguments\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
				args[0]);
		return 1;
	}

	secs = clktime;		/* total seconds since boot */

	/* subtract number of whole days */

	days  = secs/secperday;
	secs -= days*secperday;

	/* subtract number of hours */

	hrs   = secs/secperhr;
	secs -= hrs*secperhr;

	/* subtract number of minutes */

	mins  = secs/secpermin;
	secs -= mins*secpermin;

	printf("Xinu has been up ");
	if (days > 0) {
		printf(" %d day(s) ", days);
	}

	if (hrs > 0) {
		printf(" %d hour(s) ", hrs);
	}

	if (mins > 0) {
		printf(" %d minute(s) ", mins);
	}

	if (secs > 0) {
		printf(" %d second(s) ", secs);
	}
	printf("\n");

	return 0;
}
