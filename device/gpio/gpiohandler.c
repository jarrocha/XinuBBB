/* gpiohandler.c - gpiohandler */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  gpiohandler  -  Handle an interrupt for a gpio device
 *------------------------------------------------------------------------
 */
void gpiohandler(
		uint32	xnum	/* IRQ number	*/
	)
{
	struct gpio_csreg *csrptr;	/* GPIO CSR pointer		*/
	gpiointhook gphookfn;		/* The inerrupt function	*/

	/* Clear all interrupts */

	if(xnum == GPIO0_INT_A)	{
		csrptr = (struct gpio_csreg *)(GPIO0_BASE);
		gphookfn = gpiotab[0].gphookfn;
		if(gphookfn != NULL)
			gphookfn(xnum, csrptr->irqstatus0);
		csrptr->irqstatus0 = 0xFFFFFFFF;
		return;
	}
	if(xnum == GPIO0_INT_B)	{
		csrptr = (struct gpio_csreg *)(GPIO0_BASE);
		gphookfn = gpiotab[0].gphookfn;
		if(gphookfn != NULL)
			gphookfn(xnum, csrptr->irqstatus1);
		csrptr->irqstatus1 = 0xFFFFFFFF;
		return;
	}

	if(xnum == GPIO1_INT_A)	{
		csrptr = (struct gpio_csreg *)(GPIO1_BASE);
		gphookfn = gpiotab[1].gphookfn;
		if(gphookfn != NULL)
			gphookfn(xnum, csrptr->irqstatus0);
		csrptr->irqstatus0 = 0xFFFFFFFF;
		return;
	}
	if(xnum == GPIO1_INT_B)	{
		csrptr = (struct gpio_csreg *)(GPIO1_BASE);
		gphookfn = gpiotab[1].gphookfn;
		if(gphookfn != NULL)
			gphookfn(xnum, csrptr->irqstatus1);
		csrptr->irqstatus1 = 0xFFFFFFFF;
		return;
	}

	if(xnum == GPIO2_INT_A)	{
		csrptr = (struct gpio_csreg *)(GPIO2_BASE);
		gphookfn = gpiotab[2].gphookfn;
		if(gphookfn != NULL)
			gphookfn(xnum, csrptr->irqstatus0);
		csrptr->irqstatus0 = 0xFFFFFFFF;
		return;
	}
	if(xnum == GPIO2_INT_B)	{
		csrptr = (struct gpio_csreg *)(GPIO2_BASE);
		gphookfn = gpiotab[2].gphookfn;
		if(gphookfn != NULL)
			gphookfn(xnum, csrptr->irqstatus1);
		csrptr->irqstatus1 = 0xFFFFFFFF;
		return;
	}

	if(xnum == GPIO3_INT_A)	{
		csrptr = (struct gpio_csreg *)(GPIO3_BASE);
		gphookfn = gpiotab[3].gphookfn;
		if(gphookfn != NULL)
			gphookfn(xnum, csrptr->irqstatus0);
		csrptr->irqstatus0 = 0xFFFFFFFF;
		return;
	}
	if(xnum == GPIO3_INT_B)	{
		csrptr = (struct gpio_csreg *)(GPIO3_BASE);
		gphookfn = gpiotab[3].gphookfn;
		if(gphookfn != NULL)
			gphookfn(xnum, csrptr->irqstatus1);
		csrptr->irqstatus1 = 0xFFFFFFFF;
		return;
	}
}
