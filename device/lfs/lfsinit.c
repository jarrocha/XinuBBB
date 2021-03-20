/* lfsinit.c - lfsinit */

#include <xinu.h>

struct	lfdata	Lf_data;

/*------------------------------------------------------------------------
 * lfsinit  -  Initialize the local file system master device
 *------------------------------------------------------------------------
 */
devcall	lfsinit (
	  struct dentry *devptr		/* Entry in device switch table */
	)
{
	/* Assign ID of disk device that will be used */

	Lf_data.lf_dskdev = LF_DISK_DEV;

	/* Create a mutual exclusion semaphore */

	Lf_data.lf_mutex = semcreate(1);

	/* Zero directory area (for debugging) */

	memset((char *)&Lf_data.lf_dir, NULLCH, sizeof(struct lfdir));

	/* Initialize directory to "not present" in memory */

	Lf_data.lf_dirpresent = Lf_data.lf_dirdirty = FALSE;

	return OK;
}
