/* interrupt.h */

extern	uint16	girmask;	/* mask of interrupts being serviced	*/
				/* a bit of 1 corresponds to an		*/
				/* interrupt that is allowed.		*/

struct intc_bank {
	uint32	itr;
	uint32	mir;
	uint32	mir_clear;
	uint32	mir_set;
	uint32	isr_set;
	uint32	isr_clear;
	uint32	pending_irq;
	uint32	pending_fiq;
};

struct intc_csreg {
	uint32	revision;
	uint32	res1[3];
	uint32	sysconfig;
	uint32	sysstatus;
	uint32	res2[10];
	uint32	sir_irq;
	uint32	sir_fiq;
	uint32	control;
	uint32	protection;
	uint32	idle;
	uint32	res3[3];
	uint32	irq_priority;
	uint32	fiq_priority;
	uint32	threshold;
	uint32	res4[5];
	struct	intc_bank banks[4];
	uint32	ilr[128];
};

#define INTC_SYSCONFIG_SOFTRESET	0x00000002
#define INTC_SYSSTATUS_RESETDONE	0x00000001

#define INTC_SIR_IRQ_MASK	0x0000007F

#define INTC_CONTROL_NEWIRQAGR	0x00000001
