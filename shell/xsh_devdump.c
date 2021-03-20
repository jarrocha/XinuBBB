/* xsh_devdump.c  - xsh_devdump */

#include <xinu.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * xsh_devdump - shell command to print info from the device switch table
 *------------------------------------------------------------------------
 */
shellcmd xsh_devdump (
	 int	nargs,			/* number of arguments		*/
	 char	*args[]			/* list of arguments		*/
 	)
{
	struct	dentry	*devptr;	/* pointer to device entry	*/
	int32	i;			/* walks through device table	*/

	/* No arguments are expected */

	if (nargs > 1) {
		fprintf(stderr, "No arguments allowed\n");
		return 1;
	}

	/* Walk through device table */

	printf("Device     Name     Minor\n");
	printf("------ ------------ -----\n");

	for (i = 0; i < NDEVS; i++) {
		devptr = &devtab[i];
		printf("%4d   %-12s %3d\n", i, devptr->dvname,
			devptr->dvminor);
	}
	return 0;
}
