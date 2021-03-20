/* ramwrite.c  -  ramwrite */

#include <xinu.h>
#include <ramdisk.h>

/*------------------------------------------------------------------------
 * ramwrite  -  Write a block to a ram disk
 *------------------------------------------------------------------------
 */
devcall	ramwrite (
	  struct dentry	*devptr,	/* Entry in device switch table	*/
	  char	*buff,			/* Buffer containing a block	*/
	  int32	blk			/* Block number to write	*/
	)
{
	int32	bpos;			/* Byte position of blk		*/

	bpos = RM_BLKSIZ * blk;
	memcpy(&Ram.disk[bpos], buff, RM_BLKSIZ);
	return OK;
}
