/* kprintf.c -  kputc, kgetc, kprintf */

#include <xinu.h>
#include <stdarg.h>

/*------------------------------------------------------------------------
 * kputc - use polled I/O to write a character to the console serial line
 *------------------------------------------------------------------------
 */
syscall kputc(
	  byte	c			/* character to write		*/
	)
{
	struct	dentry	*devptr;
	volatile struct	uart_csreg	*csrptr;
	intmask	mask;

	/* Disable interrupts */
	mask = disable();

	/* Get CSR address of the console */

	devptr = (struct dentry *) &devtab[CONSOLE];
	csrptr = (struct uart_csreg *) devptr->dvcsr;

	/* wait for UART transmit queue to empty */

	while ( (csrptr->lsr & UART_LSR_THRE) == 0 ) {
		;
	}

	/* write the character */

	csrptr->buffer = c;

	/* Honor CRLF - when writing NEWLINE also send CARRIAGE RETURN	*/

	if (c == '\n') {
		while ( (csrptr->lsr & UART_LSR_THRE) == 0 ) {
			;
		}
		csrptr->buffer = '\r';
	}

	restore(mask);
	return OK;
}

/*------------------------------------------------------------------------
 * kgetc - use polled I/O to read a character from the console serial line
 *------------------------------------------------------------------------
 */
syscall kgetc(void)
{
	int irmask;
	volatile struct uart_csreg *regptr;
	byte c;
	struct	dentry	*devptr;
	intmask	mask;

	/* Disable interrupts */
	mask = disable();

	devptr = (struct dentry *) &devtab[CONSOLE];
	regptr = (struct uart_csreg *)devptr->dvcsr;

	irmask = regptr->ier;		/* Save UART interrupt state.   */
	regptr->ier = 0;		/* Disable UART interrupts.     */

	while (0 == (regptr->lsr & UART_LSR_DR)) {
		; /* Do Nothing */
	}

	/* Read character from Receive Holding Register */

	c = regptr->rbr;
	regptr->ier = irmask;		/* Restore UART interrupts.     */

	restore(mask);
	return c;
}

extern	void	_doprnt(char *, va_list, int (*)(int), int);

/*------------------------------------------------------------------------
 * kprintf  -  use polled I/O to print formatted output on the console
 *------------------------------------------------------------------------
 */
syscall kprintf(char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    _doprnt(fmt, ap, (int (*)(int))kputc, (int)&devtab[CONSOLE]);
    va_end(ap);
    return OK;
}
