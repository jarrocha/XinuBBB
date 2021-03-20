/* lfibget.c - lfibget */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  lfibget  -  Get an index block from disk given its number (assumes
 *			mutex is held)
 *------------------------------------------------------------------------
 */
void	lfibget(
	  did32		diskdev,	/* Device ID of disk to use	*/
	  ibid32	inum,		/* ID of index block to fetch	*/
	  struct lfiblk	*ibuff		/* Buffer to hold index block	*/
	)	
{
	char	*from, *to;		/* Pointers used in copying	*/
	int32	i;			/* Loop index used during copy	*/
	char	dbuff[LF_BLKSIZ];	/* Buffer to hold disk block	*/

	/* Read disk block that contains the specified index block */

	read(diskdev, dbuff, ib2sect(inum));

	/* Copy specified index block to caller's ibuff */

	from = dbuff + ib2disp(inum);
	to = (char *)ibuff;
	for (i=0 ; i<sizeof(struct lfiblk) ; i++)
		*to++ = *from++;
	return;
}
