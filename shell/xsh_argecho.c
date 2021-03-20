/* xsh_argecho.c - xsh_argecho */

#include <xinu.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * xhs_argecho - display argecho message that lists shell commands
 *------------------------------------------------------------------------
 */
shellcmd xsh_argecho(int nargs, char *args[])
{
	int32	i;

	printf("\n\nThe %d arguments are:\n", nargs);
	for (i = 0; i < nargs; i++) {
		printf("    %2d: %s\n", i, args[i]);
	}
	printf("\n");

	return 0;
}
