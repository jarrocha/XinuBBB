/* lfscheck.c - lfscheck */

#include <xinu.h>
#include <ramdisk.h>

/*------------------------------------------------------------------------
 * lfscheck  -  Check a directory to verify it contains a Xinu file system
 *------------------------------------------------------------------------
 */
status	lfscheck (
	  struct lfdir	*dirptr		/* Ptr to an in-core directory	*/
	)
{
	uint32	reverse;		/* LFS_ID in reverse byte order	*/

	/* Verify the File System ID, all 0's and all 1's fields */

	if ( (dirptr->lfd_fsysid != LFS_ID)       ||
	     (dirptr->lfd_allzeros != 0x00000000) ||
	     (dirptr->lfd_allones  != 0xffffffff) ) {
		return SYSERR;
	}

	/* Check the reverse-order File System ID field */

	reverse = ((LFS_ID>>24) & 0x000000ff) | 
		  ((LFS_ID>> 8) & 0x0000ff00) |
		  ((LFS_ID<< 8) & 0x00ff0000) |
		  ((LFS_ID<<24) & 0xff000000) ;

	if (dirptr->lfd_revid != reverse) {
		return SYSERR;
	}

	/* Extra sanity check - verify file count is positive */
	if (dirptr->lfd_nfiles < 0){
		return SYSERR;
	}
	return OK;
}
