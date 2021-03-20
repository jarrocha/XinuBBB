/* rfsgetmode.c - rfsgetmode */

#include <xinu.h>

/*------------------------------------------------------------------------
 * rfsgetmode  -  Parse mode argument and generate integer of mode bits
 *------------------------------------------------------------------------
 */

int32	rfsgetmode (
	 char	*mode			/* String of mode characters	*/
	)
{
	int32	mbits;			/* Mode bits to return (in host	*/
					/*    byte order)		*/
	char	ch;			/* Next character in mode string*/

	mbits = 0;

	/* Mode string specifies:					*/
	/*	r - read						*/
	/*	w - write						*/
	/*	o - old (file must exist)				*/
	/*	n - new (create a new file)				*/

	while ( (ch = *mode++) != NULLCH) {
		switch (ch) {

		    case 'r':	if (mbits&RF_MODE_R) {
					return SYSERR;
				}
				mbits |= RF_MODE_R;
				continue;

		    case 'w':	if (mbits&RF_MODE_W) {
					return SYSERR;
				}
				mbits |= RF_MODE_W;
				continue;

		    case 'o':	if (mbits&RF_MODE_O || mbits&RF_MODE_N) {
					return SYSERR;
				}
				mbits |= RF_MODE_O;
				break;

		    case 'n':	if (mbits&RF_MODE_O || mbits&RF_MODE_N) {
					return SYSERR;
				}
				mbits |= RF_MODE_N;
				break;

		    default:	return SYSERR;
		}
	}

	/* If neither read nor write specified, allow both */

	if ( (mbits&RF_MODE_RW) == 0 ) {
		mbits |= RF_MODE_RW;
	}
	return mbits;
}
