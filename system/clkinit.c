/* clkinit.c - clkinit (BeagleBone Black) */

#include <xinu.h>

uint32	clktime;		/* Seconds since boot			*/
uint32  count1000;              /* ms since last clock tick             */
qid16	sleepq;			/* Queue of sleeping processes		*/
uint32	preempt;		/* Preemption counter			*/

/*------------------------------------------------------------------------
 * clkinit  -  Initialize the clock and sleep queue at startup
 *------------------------------------------------------------------------
 */
void	clkinit(void)
{
	volatile struct am335x_timer1ms *csrptr =
	(volatile struct am335x_timer1ms *)AM335X_TIMER1MS_ADDR;
			/* Pointer to timer CSR in BBoneBlack	*/
	volatile uint32 *clkctrl =
			(volatile uint32 *)AM335X_TIMER1MS_CLKCTRL_ADDR;

	*clkctrl = AM335X_TIMER1MS_CLKCTRL_EN;
	while((*clkctrl) != 0x2) /* Do nothing */ ;

	/* Reset the timer module */

	csrptr->tiocp_cfg |= AM335X_TIMER1MS_TIOCP_CFG_SOFTRESET;

	/* Wait until the reset is complete */

	while((csrptr->tistat & AM335X_TIMER1MS_TISTAT_RESETDONE) == 0)
		/* Do nothing */ ;

	/* Set interrupt vector for clock to invoke clkint */

	set_evec(AM335X_TIMER1MS_IRQ, (uint32)clkhandler);

	sleepq = newqueue();	/* Allocate a queue to hold the delta	*/
				/*   list of sleeping processes		*/

	preempt = QUANTUM;	/* Set the preemption time		*/

	clktime = 0;		/* Start counting seconds		*/
        count1000 = 0;
	/* The following values are calculated for a	*/
	/*   timer that generates 1ms tick rate		*/

	csrptr->tpir = 1000000;
	csrptr->tnir = 0;
	csrptr->tldr = 0xFFFFFFFF - 26000;

	/* Set the timer to auto reload */

	csrptr->tclr = AM335X_TIMER1MS_TCLR_AR;

	/* Start the timer */

	csrptr->tclr |= AM335X_TIMER1MS_TCLR_ST;

	/* Enable overflow interrupt which will generate */
	/*   an interrupt every 1 ms			 */

	csrptr->tier = AM335X_TIMER1MS_TIER_OVF_IT_ENA;

	/* Kickstart the timer */

	csrptr->ttgr = 1;

	return;
}
