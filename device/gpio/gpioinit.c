/* gpioinit.c - gpioinit */

#include <xinu.h>

struct	gpiocblk	gpiotab[NGPIO];

/*------------------------------------------------------------------------
 *  gpioinit  -  Initialize settings for a gpio device
 *------------------------------------------------------------------------
 */
devcall	gpioinit(
	  struct dentry	*devptr		/* Entry in device switch table	*/
	)
{
	struct gpio_csreg *csrptr;	/* Pointer ot GPIO CSRs		*/
	struct gpiocblk	*gpioptr;	/* Pointer ot GPIO Control block*/
	uint32 *fclkreg;		/* FCLK Register for debounce	*/

	csrptr = (struct gpio_csreg *)(devptr->dvcsr);
	gpioptr = &gpiotab[devptr->dvminor];

	/* Get the FCLK Register for the corresponding GPIO module */

	if(csrptr == GPIO1_BASE)	{
		fclkreg = PRCM_FCLK_GPIO1;
	} else if(csrptr == GPIO2_BASE)	{
		fclkreg = PRCM_FCLK_GPIO2;
	} else if(csrptr == GPIO3_BASE)	{
		fclkreg = PRCM_FCLK_GPIO3;
	} else {
		/* GPIO0 FCLK is ON by default */
		fclkreg = NULL;
	}

	/* Initialize register values */

	csrptr->sysconfig = 0x0;
	
	/* Disable output for all pins */

	csrptr->oe = 0xFFFFFFFF;

	/* Clear all pending interrupts */

	csrptr->irqstatus0 = 0xFFFFFFFF;
	csrptr->irqstatus1 = 0xFFFFFFFF;

	/* Disable all interrupts */

	csrptr->irqclear0 = 0xFFFFFFFF;
	csrptr->irqclear1 = 0xFFFFFFFF;
	
	/* Clear all pins */

	csrptr->clear_data = 0xFFFFFFFF;
	
	/* Clear wakeup settings */

	csrptr->irqwaken0 = 0x0;
	csrptr->irqwaken1 = 0x0;

	/* Clear all interrupt trigger settings */

	csrptr->rising	= 0x0;
	csrptr->falling = 0x0;
	csrptr->level0	= 0x0;
	csrptr->level1	= 0x0;

	/* Disable debouncing */

	csrptr->deb_ena = 0x0;

	/* Setup debouncing clock */

	if(fclkreg != NULL)	{
		*fclkreg |= PRCM_FCLK_BIT;
	}

	/* Interrupt A */

	set_evec( devptr->dvirq, (uint32)devptr->dvintr );

	/* Interrupt B is the next interrupt line */

	set_evec( (devptr->dvirq)+1, (uint32)devptr->dvintr );

	gpioptr->gphookfn = NULL;

	return OK;
}
