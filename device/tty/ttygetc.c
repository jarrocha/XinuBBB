/* ttygetc.c - ttygetc */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  ttygetc  -  Read one character from a tty device (interrupts disabled)
 *------------------------------------------------------------------------
 */
devcall	ttygetc(
	  struct dentry	*devptr		/* Entry in device switch table	*/
	)
{
	char	ch;			/* Character to return		*/
	struct	ttycblk	*typtr;		/* Pointer to ttytab entry	*/

	typtr = &ttytab[devptr->dvminor];

	/* Wait for a character in the buffer and extract one character	*/

	wait(typtr->tyisem);
	ch = *typtr->tyihead++;

	/* Wrap around to beginning of buffer, if needed */

	if (typtr->tyihead >= &typtr->tyibuff[TY_IBUFLEN]) {
		typtr->tyihead = typtr->tyibuff;
	}

	/* In cooked mode, check for the EOF character */

	if ( (typtr->tyimode == TY_IMCOOKED) && (typtr->tyeof) &&
	     (ch == typtr->tyeofch) ) {
		return (devcall)EOF;
	}

	return (devcall)ch;
}
