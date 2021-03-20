/* ttyhandle_out.c - ttyhandle_out */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  ttyhandle_out  -  Handle an output on a tty device by sending more
 *		     characters to the device FIFO (interrupts disabled)
 *------------------------------------------------------------------------
 */
void	ttyhandle_out(
	 struct	ttycblk	*typtr,		/* Ptr to ttytab entry		*/
	 struct	uart_csreg *csrptr	/* Address of UART's CSRs	*/
	)
{
	
	int32	ochars;			/* Number of output chars sent	*/
					/*   to the UART		*/
	int32	avail;			/* Available chars in output buf*/
	int32	uspace;			/* Space left in onboard UART	*/
					/*   output FIFO		*/
	uint32 	ier = 0;

	/* If output is currently held, simply ignore the call */

	if (typtr->tyoheld) {
		return;
	}

	/* If echo and output queues empty, turn off interrupts */

	if ( (typtr->tyehead == typtr->tyetail) &&
	     (semcount(typtr->tyosem) >= TY_OBUFLEN) ) {
		ier = csrptr->ier;
		csrptr->ier = ier & ~UART_IER_ETBEI;
		return;
	}
	
	/* Initialize uspace to the available space in the Tx FIFO */

	uspace = UART_FIFO_SIZE - csrptr->txfifo_lvl;

	/* While onboard FIFO is not full and the echo queue is	*/
	/*   nonempty, xmit chars from the echo queue		*/

	while ( (uspace>0) &&  typtr->tyehead != typtr->tyetail) {
		csrptr->buffer = *typtr->tyehead++;
		if (typtr->tyehead >= &typtr->tyebuff[TY_EBUFLEN]) {
			typtr->tyehead = typtr->tyebuff;
		}
		uspace--;
	}

	/* While onboard FIFO is not full and the output queue is	*/
	/*   nonempty, transmit chars from the output queue		*/

	ochars = 0;
	avail = TY_OBUFLEN - semcount(typtr->tyosem);
	while ( (uspace>0) &&  (avail > 0) ) {
		csrptr->buffer = *typtr->tyohead++;
		if (typtr->tyohead >= &typtr->tyobuff[TY_OBUFLEN]) {
			typtr->tyohead = typtr->tyobuff;
		}
		avail--;
		uspace--;
		ochars++;
	}
	if (ochars > 0) {
		signaln(typtr->tyosem, ochars);
	}

	if ( (typtr->tyehead == typtr->tyetail) &&
	     (semcount(typtr->tyosem) >= TY_OBUFLEN) ) {
		ier = csrptr->ier;
		csrptr->ier = (ier & ~UART_IER_ETBEI);
	}
	return;
}
