/* xsh_cat.c - xsh_cat */

#include <xinu.h>
#include <stdio.h>
#include <string.h>

/*------------------------------------------------------------------------
 * xsh_cat - shell command to cat one or more files
 *------------------------------------------------------------------------
 */
shellcmd xsh_cat(int nargs, char *args[])
{
	int32	i;			/* index into proctabl		*/
	int32	nextch;			/* character read from file	*/
	did32	descr;			/* descriptor for a file	*/
	char	*argptr;		/* pointer to next arg string	*/


	/* For argument '--help', emit help about the 'cat' command	*/

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s [file...]\n\n", args[0]);
		printf("Description:\n");
		printf("\twrites contents of files or stdin to stdout\n");
		printf("Options:\n");
		printf("\tfile...\tzero or more file names\n");
		printf("\t--help\t display this help and exit\n");
		return 0;
	}

	if (nargs == 1) {
		nextch = getc(stdin);
		while (nextch != EOF) {
			putc(stdout, nextch);
			nextch = getc(stdin);
		}
		return 0;
	}
	for (i = 1; i < nargs; i++) {
		argptr = args[i];
		if ( (argptr[0] == '-') && (argptr[1] == NULLCH) ) {
			descr = stdin;
		} else {
			descr = open(NAMESPACE, argptr, "ro");
			if (descr == (did32)SYSERR) {
				fprintf(stderr, "%s: cannot open file %s\n",
					args[0], argptr);
				return 1;
			}
		}
		nextch = getc(descr);
		while (nextch != EOF) {
			putc(stdout, nextch);
			nextch = getc(descr);
		}
		close(descr);
	}
	return 0;
}
