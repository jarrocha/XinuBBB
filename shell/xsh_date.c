/* xsh_date.c - xsh_date */

#include <xinu.h>
#include <string.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * xsh_date - obtain and print the current month, day, year, and time
 *------------------------------------------------------------------------
 */
shellcmd xsh_date(int nargs, char *args[]) {

	int32	retval;			/* return value			*/
	uint32	now;			/* current local time		*/
	char	datestr[64];		/* printable date in ascii	*/

	/* Output info for '--help' argument */

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Usage: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\tDisplays the current date and time\n");
		printf("Options (one per invocation):\n");
		printf("\t-f\tforce a time server request to be sent\n");
		printf("\t-d\tset daylight savings time on\n");
		printf("\t-s\tset standard time (not daylight savings)\n");
		printf("\t-a\tset daylight savings to automatic\n");
		printf("\t--help\tdisplay this help and exit\n");
		return 0;
	}

	/* Check argument count */

	if (nargs > 2) {
		fprintf(stderr, "%s: too many arguments\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
			args[0]);
		return 1;
	}

	if (nargs == 2) {
		if (strncmp(args[1], "-f", 3) == 0) {
			Date.dt_bootvalid = FALSE;
		} else if (strncmp(args[1], "-d", 3) == 0) {
			Date.dt_daylight = DATE_DST_ON;
		} else if (strncmp(args[1], "-s", 3) == 0) {
			Date.dt_daylight = DATE_DST_OFF;
		} else if (strncmp(args[1], "-a", 3) == 0) {
			Date.dt_daylight = DATE_DST_AUTO;
		} else {
			fprintf(stderr, "%s: invalid argument\n", args[0]);
			fprintf(stderr,
				"Try '%s --help' for more information\n",
				args[0]);
			return 1;
		}
	}

	retval = gettime(&now);
	if (retval == SYSERR) {
		fprintf(stderr,
			"%s: could not obtain the current date\n",
			args[0]);
		return 1;
	}
	ascdate(now, datestr);
	printf("%s\n", datestr);
	return 0;
}
