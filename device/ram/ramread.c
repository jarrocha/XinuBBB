/* ramread.c  -  ramread */

#include <xinu.h>
#include <ramdisk.h>

/*------------------------------------------------------------------------
 * ramread  -  Read a block from a ram disk
 *------------------------------------------------------------------------
 */
devcall	ramread (
	  struct dentry	*devptr,	/* Entry in device switch table	*/
	  char	*buff,			/* Buffer to hold disk block	*/
	  int32	blk			/* Block number of block to read*/
	)
{
	int32	bpos;			/* Byte position of blk		*/

	bpos = RM_BLKSIZ * blk;
	memcpy(buff, &Ram.disk[bpos], RM_BLKSIZ);
	return OK;
}
