/* gpiocontrol.c - gpiocontrol */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  gpiocontrol  -  Control a gpio device pin settings
 *------------------------------------------------------------------------
 */
devcall	gpiocontrol(
	  struct dentry	*devptr,	/* Entry in device switch table	*/
	  int32	 func,			/* Function to perform		*/
	  int32	 arg1,			/* Argument 1 for request	*/
	  int32	 arg2			/* Argument 2 for request	*/
	)
{
	struct gpio_csreg *csrptr;	/* Pointer ot the CSR for GPIO	*/
	struct	gpiocblk  *gpioptr;	/* Pointer to the control block	*/

	csrptr = (struct gpio_csreg *)(devptr->dvcsr);
	gpioptr = &gpiotab[devptr->dvminor];

	/* Process the request */

	switch ( func )	{

	/* Enable output capability for pins in arg1 */

	case GPIO_OUTPUT_ENABLE:
		csrptr->oe &= (~arg1);
		return (devcall)OK;

	/* Disable output capability for pins in arg1 */

	case GPIO_OUTPUT_DISABLE:
		csrptr->oe |= arg1;
		return (devcall)OK;

	/* Register arg1 function pointer to be called from interrupt */

	case GPIO_REG_INT_HANDLER:
		gpioptr->gphookfn = (gpiointhook)arg1;
		return (devcall)OK;

	/* Configure debounce capability for pins in arg2 */

	case GPIO_DEB_SET_TIME:
		if(arg1 > 0)	{
			csrptr->deb_time = (arg1-1) & 0xFF;
			csrptr->deb_ena |= arg2;
		}
		else	{
			csrptr->deb_ena &= (~arg2);
		}
		return (devcall)OK;

	/* Configure interrupt capability for pins in arg2 */

	case GPIO_INTERRUPT_CTL:

		/* No lines affected */

		if((arg1&GPIO_INT_ALL_LINES) == 0)	{ 
			return (devcall)SYSERR;
		}

		/* Disable lines and triggers */

		if((arg1&GPIO_INT_ALL_TRIG) == 0)	{ 

			/* Flags affect line 0 */

			if((arg1&GPIO_INT_LINE0_EN) != 0) {
				csrptr->irqclear0 = arg2;
			}

			/* Flags affect line 1 */

			if((arg1&GPIO_INT_LINE1_EN) != 0) { 
				csrptr->irqclear1 = arg2;
			}
			
			/* Disable all triggers */

			csrptr->rising &= (~arg2);
			csrptr->falling &= (~arg2);
			csrptr->level0 &= (~arg2);
			csrptr->level1 &= (~arg2);

			return (devcall)OK;
		}

		/* Enable lines and triggers */

		if((arg1&GPIO_INT_LINE0_EN) != 0)	{ 
				csrptr->irqset0 = arg2;
		}
		if((arg1&GPIO_INT_LINE1_EN) != 0)	{ 
				csrptr->irqset1 = arg2;
		}
		if((arg1&GPIO_INT_RISE_TRIG) != 0)	{
			csrptr->rising |= arg2;
		}
		if((arg1&GPIO_INT_FALL_TRIG) != 0)	{
			csrptr->falling |= arg2;
		}
		if((arg1&GPIO_INT_LVL0_TRIG) != 0)	{
			csrptr->level0 |= arg2;
		}
		if((arg1&GPIO_INT_LVL1_TRIG) != 0)	{
			csrptr->level1 |= arg2;
		}

		return (devcall)OK;

	/* Read input from a single pin arg1 */

	case GPIO_READ_PIN:
		arg2 = csrptr->datain & PIN_MASK(arg1);
		return (arg2>0);

	/* Write value arg2 to a single output pin arg1 */

	case GPIO_WRITE_PIN:
		if(arg2){
			csrptr->set_data = PIN_MASK(arg1);
		}
		else	{
			csrptr->clear_data = PIN_MASK(arg1);
		}
		return (devcall)OK;

	default:
		return (devcall)SYSERR;
	}
}
