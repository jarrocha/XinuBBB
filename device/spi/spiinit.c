/* spiinit.c - spiinit */

#include <xinu.h>

/*------------------------------------------------------------------------
 * spiinit  -  SPI initialization function
 *------------------------------------------------------------------------
 */
int32	spiinit (
		 struct	dentry *devptr
		)
{
	struct	spi_csreg *csrptr;	/* SPI control and status regs	*/

	/* Set the CSR address and the pads */

	if(devptr->dvminor == 0) {
		csrptr = (struct spi_csreg *)SPI_0_ADDR;
		am335x_padctl(SPI_0_PADCTL_SCLK, AM335X_PADCTL_PEN |
						 AM335X_PADCTL_PUP |
						 AM335X_PADCTL_RXTX |
						 0);
		am335x_padctl(SPI_0_PADCTL_D0, AM335X_PADCTL_PEN |
						 AM335X_PADCTL_PUP |
						 AM335X_PADCTL_TX |
						 0);
		am335x_padctl(SPI_0_PADCTL_D1, AM335X_PADCTL_PEN |
						 AM335X_PADCTL_PUP |
						 AM335X_PADCTL_RXTX |
						 0);
		am335x_padctl(SPI_0_PADCTL_CS0, AM335X_PADCTL_PEN |
						 AM335X_PADCTL_PUP |
						 AM335X_PADCTL_TX |
						 0);
	}
	else {
		*((uint32 *)0x44E00050) = 0x2;
		csrptr = (struct spi_csreg *)SPI_1_ADDR;
		am335x_padctl(SPI_1_PADCTL_SCLK, AM335X_PADCTL_PEN |
						 AM335X_PADCTL_PUP |
						 AM335X_PADCTL_RXTX |
						 3);
		am335x_padctl(SPI_1_PADCTL_D0, AM335X_PADCTL_PEN |
						 AM335X_PADCTL_PUP |
						 AM335X_PADCTL_TX |
						 3);
		am335x_padctl(SPI_1_PADCTL_D1, AM335X_PADCTL_PEN |
						 AM335X_PADCTL_PUP |
						 AM335X_PADCTL_RXTX |
						 3);
		am335x_padctl(SPI_1_PADCTL_CS0, AM335X_PADCTL_PEN |
						 AM335X_PADCTL_PUP |
						 AM335X_PADCTL_RXTX |
						 3);
	}

	/* Reset the SPI device */

	csrptr->sysconfig |= SPI_SYSCONFIG_SOFTRESET;
	while((csrptr->sysstatus & SPI_SYSSTATUS_RESETDONE) == 0);

	/* Put the device in single channel mode */

	csrptr->modulctrl = SPI_MODULCTRL_SINGLE;

	/* Set the default frequency and CS level */

	csrptr->ch[0].chconf = SPI_CHCONF_CLKD | SPI_CHCONF_EPOL |
			    (SPI_CHCONF_WL & 0x00000380) |
			     SPI_CHCONF_DPE1 | SPI_CHCONF_IS |
			     SPI_CHCONF_FORCE;

	/* Enable the channel */

	csrptr->ch[0].chctrl |= SPI_CHCTRL_EN;

	return OK;
}
