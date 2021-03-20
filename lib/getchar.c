/* getchar.c - getchar */

#include <xinu.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 *  getchar  -  DOCUMENT
 *------------------------------------------------------------------------
 */
int	getchar(
	  void
	)
{
    return fgetc(stdin);
}
