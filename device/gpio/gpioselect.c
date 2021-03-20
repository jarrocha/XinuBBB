/* gpioselect.h - pin multiplexing and control for the BBB ZCZ package */
#include <xinu.h>

/*------------------------------------------------------------------------
 *  gpioselect  -  Configure modes for GPIO pins
 *------------------------------------------------------------------------
 */
devcall gpioselect( 	
		uint32 pinreg,		/* Pin to be affected		*/
		uint32 pinmode,		/* Multiplexed pin function	*/ 
		uint32 pinsettings	/* Settings for the pin 	*/
)	
{
	volatile uint32* confreg; /* Pin configuration register */

	/* Pick up the pin configuration register address */

	confreg = &(((uint32*)PINMUXBASE)[pinreg]);

	/* Apply the specified settings to the pin */

	*confreg = pinmode|pinsettings;

	return OK;
}
