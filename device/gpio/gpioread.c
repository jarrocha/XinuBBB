/* gpioread.c - gpioread */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  gpioread  -  Read aspecified set of GPIO pins
 *------------------------------------------------------------------------
 */
devcall	gpioread(
	  struct dentry	*devptr,	/* Entry in device switch table	*/
	  char *value,			/* Pointer to a 32bit buffer	*/
	  int32 pinmask			/* Pins from which to read	*/
	)
{
	struct gpio_csreg *csrptr;	/* Pointer to GPIO CSRs		*/

	/* Obtain the address of the CSR from the device switch table	*/

	csrptr = (struct gpio_csreg *)(devptr->dvcsr);
	
	/* Access the GPIO pins, use the mask to select a subset, and	*/
	/*	store the result in the specified buffer		*/

	*((uint32 *)value) = csrptr->datain & pinmask;

	return OK;
}
