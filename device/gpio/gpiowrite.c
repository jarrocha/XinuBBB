/* gpiowrite.c - gpiowrite */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  gpiowrite  -  Write value of gpio pins
 *------------------------------------------------------------------------
 */
devcall	gpiowrite(
	  struct dentry	*devptr,	/* Entry in device switch table	*/
	  char *value,			/* Pointer to 32-bit buffer	*/
	  int32 pinmask			/* Pins to be written written	*/
	)
{
	struct gpio_csreg *csrptr;	/* Pointer to GPIO CSRs		*/
	uint32 val;			/* Value to be written		*/

	csrptr = (struct gpio_csreg *)(devptr->dvcsr);
	val = * (uint32 *)value;

	/* Set bits that should become one */

	csrptr->set_data = pinmask & val;	

	/* Clear bits that should become zero */

	csrptr->clear_data = pinmask & ~val;

	return OK;
}
