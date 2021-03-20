/* spicontrol.c - spicontrol */

#include <xinu.h>

/*------------------------------------------------------------------------
 * spicontrol  -  Control function for SPI device
 *------------------------------------------------------------------------
 */
devcall	spicontrol (
	struct	dentry	*devptr,	/* Entry in device switch table	*/
	int32	func,			/* Control function		*/
	int32	arg1,			/* Argument 1, if needed	*/
	int32	arg2			/* Argument 2, if needed	*/
	)
{
	struct	spi_csreg *csrptr;	/* SPI control and status regs	*/
	struct	spi_transfer *msg;	/* Message pointer		*/
	int	i;			/* For loop index		*/

	/* Get the pointer to the SPI CSRs */

	csrptr = (struct spi_csreg *)devptr->dvcsr;

	/* Get the pointer to the message */

	msg = (struct spi_transfer *)arg1;

	switch(func) {

	 /* This is a SPI transfer */

	 case SPI_CTRL_TRANSFER:

		for(i = 0; i < msg->length; i++) {

			/* Wait while the transmit register is not empty */

			while((csrptr->ch[0].chstat & SPI_CHSTAT_TXS) == 0);

			/* Send a byte */

			csrptr->ch[0].chtx = msg->txbuf[i];

			/* Wait until there is a byte in the receive register */

			while((csrptr->ch[0].chstat & SPI_CHSTAT_RXS) == 0);

			/* Read the byte from the receive register */

			msg->rxbuf[i] = csrptr->ch[0].chrx;
		}

		/* End the transfer by changing CS level */

		csrptr->ch[0].chconf &= ~SPI_CHCONF_FORCE;
		csrptr->ch[0].chconf |= SPI_CHCONF_FORCE;

		break;
	}

	return OK;
}
