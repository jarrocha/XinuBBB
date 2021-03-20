/* dot2ip.c - dot2ip */

#include <xinu.h>

/*------------------------------------------------------------------------
 * dot2ip  -  Convert a string of dotted decimal to an unsigned integer
 *------------------------------------------------------------------------
 */
uint32	dot2ip (
	  char	 *dotted,		/* IP address in dotted decimal	*/
	  uint32 *result		/* Location to which binary IP	*/
					/*    address will be stored	*/
					/*    (host byte order)		*/
	)
{
	int32	seg;			/* Counts segments		*/
	int32	nch;			/* Counts chars within segment	*/
	char	ch;			/* Next character		*/
	uint32	ipaddr;			/* IP address in binary		*/
	int32	val;			/* Binary value of one segment	*/

	/* Input must have the form  X.X.X.X, where X is 1 to 3 digits	*/

	ipaddr = 0;
	for (seg=0 ; seg<4 ; seg++) {    /* For each segment		*/
	    val = 0;
	    for (nch=0 ; nch<4; nch++) { /* Up to four chars per segment*/
		ch = *dotted++;
		if ( (ch==NULLCH) || (ch == '.') ) {
			if (nch == 0) {
				return SYSERR;
			} else {
				break;
			}
		}

		/*  Non-digit is an error */

		if ( (ch<'0') || (ch>'9') ) {
			return SYSERR;
		}

		val = 10*val + (ch-'0');
	    }

	    if (val > 255) {	/* Out of valid range */
		return SYSERR;
	    }
	    ipaddr = (ipaddr << 8) | val;

	    if (ch == NULLCH) {
		break;
	    }
	}
	if ( (seg != 3) || (ch != NULLCH) ) {
		return SYSERR;
	}
	*result = ipaddr;
	return OK;
}
