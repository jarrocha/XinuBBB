/* xsh_ls.c - xsh_ls */

#include <xinu.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * xhs_ls - list the contents of the local file system directory
 *------------------------------------------------------------------------
 */
shellcmd xsh_ls(int nargs, char *args[])
{
	struct	lfdir	*dirptr;	/* Ptr to in-memory directory	*/
	int32	i;
	struct	ldentry	*ldptr;		/* Ptr to an entry in directory	*/

	/* For argument '--help', emit help about the 'ls' command	*/

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\tlists the names of files\n");
		printf("Options:\n");
		printf("\t--help\t display this help and exit\n");
		return 0;
	}

	if (nargs != 1) {
		printf("illegal option: try --help for more information\n");
		return 1;
	}

	/* Obtain copy of directory if not already present in memory	*/

	dirptr = &Lf_data.lf_dir;
	wait(Lf_data.lf_mutex);
	if (! Lf_data.lf_dirpresent) {
	    if (read(Lf_data.lf_dskdev, (char *)dirptr, LF_AREA_DIR) == SYSERR ) {
		signal(Lf_data.lf_mutex);
		fprintf(stderr,"cannot read the directory\n");
	    }
	    if (lfscheck(dirptr) == SYSERR ) {
		fprintf(stderr, "THe disk does not contain a Xinu file system\n");
		signal(Lf_data.lf_mutex);
		return 1;
	    }
	    Lf_data.lf_dirpresent = TRUE;
	}
	signal(Lf_data.lf_mutex);

	/* Search directory and list the file names */

	for (i=0; i<dirptr->lfd_nfiles; i++) {
		ldptr = &dirptr->lfd_files[i];
		printf("%s\n", ldptr->ld_name);
	}
	return 0;
}
