/* ttyread.c - ttyread */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  ttyread  -  Read character(s) from a tty device (interrupts disabled)
 *------------------------------------------------------------------------
 */
devcall	ttyread(
	  struct dentry	*devptr,	/* Entry in device switch table	*/
	  char	*buff,			/* Buffer of characters		*/
	  int32	count 			/* Count of character to read	*/
	)
{
	struct	ttycblk	*typtr;		/* Pointer to tty control block	*/
	int32	avail;			/* Characters available in buff.*/
	int32	nread;			/* Number of characters read	*/
	int32	firstch;		/* First input character on line*/
	char	ch;			/* Next input character		*/

	if (count < 0) {
		return SYSERR;
	}
	typtr= &ttytab[devptr->dvminor];

	if (typtr->tyimode != TY_IMCOOKED) {

		/* For count of zero, return all available characters */

		if (count == 0) {
			avail = semcount(typtr->tyisem);
			if (avail == 0) {
				return 0;
			} else {
				count = avail;
			}
		}
		for (nread = 0; nread < count; nread++) {
			*buff++ = (char) ttygetc(devptr);
		}
		return nread;
	}

	/* Block until input arrives */

	firstch = ttygetc(devptr);

	/* Check for End-Of-File */

	if (firstch == EOF) {
		return EOF;
	}

	/* Read up to a line */

	ch = (char) firstch;
	*buff++ = ch;
	nread = 1;
	while ( (nread < count) && (ch != TY_NEWLINE) &&
			(ch != TY_RETURN) ) {
		ch = ttygetc(devptr);
		*buff++ = ch;
		nread++;
	}
	return nread;
}
