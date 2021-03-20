/* lfckfmt.c - lfckfmt */

#include <xinu.h>
#include <ramdisk.h>

/*------------------------------------------------------------------------
 * lfckfmt  -  Check the format of an initially-created disk
 *------------------------------------------------------------------------
 */
status	lfsckfmt (
	  did32		disk		/* ID of an open disk device	*/
	)
{
	uint32	ibsectors;		/* Number of sectors of i-blocks*/
	struct	lfdir	dir;		/* Buffer to hold the directory	*/
	uint32	dblks;			/* Total free data blocks	*/
	struct	lfiblk	iblock;		/* Space for one i-block	*/
	struct	lfdbfree dblock;	/* Data block on the free list	*/
	int32	lfiblks;		/* Total free index blocks	*/
	int32	retval;
	ibid32	nextib;
	dbid32	nextdb;

	/* Read directory */

	retval = read(disk,(char *)&dir, LF_AREA_DIR);
	if (retval == SYSERR) {
		panic("cannot read directory");
	}
	kprintf("Have read directory from disk device %d\n\r",
		disk);

	/* Check to see if directory contains a Xinu file system */

	if (lfscheck(&dir) == SYSERR) {
		panic("directory does not contain a Xinu file system");
	}
	kprintf("Directory corresponds to a local Xinu file system\n");

	/* Follow index block list */

	lfiblks = 0;
	nextib = dir.lfd_ifree;
	kprintf("initial index block is %d\n\r", nextib);
	while (nextib != LF_INULL) {
		lfiblks++;
		lfibget(disk, nextib, &iblock);
		nextib = iblock.ib_next;
	}
	ibsectors = (lfiblks + 6) /7;
	kprintf("Found %d index blocks (%d sectors)\n\r", lfiblks, ibsectors);

	/* Follow data block list */

	dblks = 0;
	nextdb = dir.lfd_dfree;
	kprintf("initial data block is %d\n\r", nextdb);
	while (nextdb != LF_DNULL) {
		dblks++;
		read(disk, (char *)&dblock, nextdb);
		nextdb = dblock.lf_nextdb;
	}
	kprintf("Found %d data blocks\n\r", dblks);
	return OK;
}
