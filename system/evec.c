/* evec.c -- initintc, set_evec, irq_dispatch */

#include <xinu.h>
#include <stdio.h>

/*#define STKTRACE*/
/*#define REGDUMP*/

extern	void	userret(void);

uint32	intc_vector[128];	/* Interrupt vector	*/
char	expmsg1[] = "Unhandled exception. Link Register: 0x%x";
char	expmsg2[] = "**** EXCEPTION ****";
/*------------------------------------------------------------------------
 * initintc - Initialize the Interrupt Controller
 *------------------------------------------------------------------------
 */
int32	initintc()
{
	struct	intc_csreg *csrptr = (struct intc_csreg *)0x48200000;

	/* Reset the interrupt controller */

	csrptr->sysconfig |= (INTC_SYSCONFIG_SOFTRESET);

	/* Wait until reset is complete */

	while((csrptr->sysstatus & INTC_SYSSTATUS_RESETDONE) == 0);

	return OK;
}

/*------------------------------------------------------------------------
 * set_evec - set exception vector to point to an exception handler
 *------------------------------------------------------------------------
 */
int32	set_evec(uint32 xnum, uint32 handler)
{
	struct	intc_csreg *csrptr = (struct intc_csreg *)0x48200000;
	uint32	bank;	/* bank number in int controller	*/
	uint32	mask;	/* used to set bits in bank		*/

	/* There are only 127 interrupts allowed 0-126 */

	if(xnum > 127) {
		return SYSERR;
	}

	/* Install the handler */

	intc_vector[xnum] = handler;

	/* Get the bank number based on interrupt number */

	bank = (xnum/32);

	/* Get the bit inside the bank */

	mask = (0x00000001 << (xnum%32));

	/* Reset the bit to enable that interrupt number */

	csrptr->banks[bank].mir &= (~mask);

	return OK;
}

/*-------------------------------------------------------------------------
 * irq_dispatch - call the handler for specific interrupt
 *-------------------------------------------------------------------------
 */
void	irq_dispatch()
{
	struct	intc_csreg *csrptr = (struct intc_csreg *)0x48200000;
	uint32	xnum;		/* Interrupt number of device	*/
	interrupt (*handler)(); /* Pointer to handler function	*/

	/* Get the interrupt number from the Interrupt controller */

	xnum = csrptr->sir_irq & 0x7F;

	/* Defer scheduling until interrupt is acknowledged */

	resched_cntl(DEFER_START);

	/* If a handler is set for the interrupt, call it */

	if(intc_vector[xnum]) {
		handler = ( interrupt(*)() )intc_vector[xnum];
		handler(xnum);
	}

	/* Acknowledge the interrupt */

	csrptr->control |= (INTC_CONTROL_NEWIRQAGR);

	/* Resume scheduling */

	resched_cntl(DEFER_STOP);
}

#if 0
static long *fp;
/*------------------------------------------------------------------------
 * trap -- print some debugging info when a trap occurred
 * Note: Need some more work.
 *------------------------------------------------------------------------
*/
#define REGDUMP

void trap(int inum)
{
	long		*sp;
	intmask 	mask;

	mask = disable();
	kprintf("TRAP\n");
	//asm("movl	%ebp,fp");
	sp = fp + 15;	/* eflags/CS/eip/ebp/regs/trap#/Xtrap/ebp */
	kprintf("Xinu trap!\n");
	if (inum < 16)
		kprintf("exception %d (%s) currpid %d (%s)\n", inum,
			inames[inum], currpid, proctab[currpid].prname);
	else
		kprintf("exception %d currpid %d (%s)\n", inum, currpid,
			proctab[currpid].prname);
#ifdef REGDUMP
	kprintf("eflags %X ", *sp--);
	sp--;	/* skip OLD CS */
	kprintf("eip %X ", *sp);
	sp--;
	if (inum == 8 || (inum >= 10 && inum <= 14)) {
		kprintf("error code %08x (%u)\n", *sp, *sp);
		sp--;
	}
	sp--;	/* skip FP for Xint call */
	kprintf("register dump:\n");
	kprintf("eax %08X (%u)\n", *sp, *sp);
	sp--;
	kprintf("ecx %08X (%u)\n", *sp, *sp);
	sp--;
	kprintf("edx %08X (%u)\n", *sp, *sp);
	sp--;
	kprintf("ebx %08X (%u)\n", *sp, *sp);
	sp--;
	kprintf("esp %08X (%u)\n", *sp, *sp);
	sp--;
	kprintf("ebp %08X (%u)\n", *sp, *sp);
	fp = sp;
	sp--;
	kprintf("esi %08X (%u)\n", *sp, *sp);
	sp--;
	kprintf("edi %08X (%u)\n", *sp, *sp);
	sp--;
#endif // REGDUMP
#ifdef STKTRACE
	stacktrace(currpid);
#endif // STKTRACE

	panic("Trap processing complete...\n");
}
#endif
