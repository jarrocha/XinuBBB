/* ramclose.c  -  ramclose */

#include <xinu.h>

/*------------------------------------------------------------------------
 * Ramclose  -  Close a ram disk
 *------------------------------------------------------------------------
 */
devcall	ramclose (
	  struct dentry	*devptr		/* Entry in device switch table	*/
	)
{
	return OK;
}
