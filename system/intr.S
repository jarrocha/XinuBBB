/* intr.S - enable, disable, restore, halt, pause, irq_except (ARM) */

#include <armv7a.h>

	.text
	.globl	disable
	.globl	restore
	.globl	enable
	.globl	pause
	.globl	halt
	.globl	irq_except
	.globl	irq_dispatch
	.globl	initevec
	.globl	expjmpinstr

/*------------------------------------------------------------------------
 * disable  -  Disable interrupts and return the previous state
 *------------------------------------------------------------------------
 */
disable:
	mrs	r0, cpsr	/* Copy the CPSR into r0		*/
	cpsid	i		/* Disable interrupts			*/
	mov	pc, lr		/* Return the CPSR			*/

/*------------------------------------------------------------------------
 * restore  -  Restore interrupts to value given by mask argument
 *------------------------------------------------------------------------
 */
restore:
	push	{r1, r2}	/* Save r1, r2 on stack			*/
	mrs	r1, cpsr	/* Copy CPSR into r1			*/
	ldr	r2, =0x01F00220
	and	r1, r1, r2	/* Extract flags and other important	*/
	bic	r0, r0, r2	/*    bits from the mask		*/
	orr	r1, r1, r0
	msr	cpsr_cfsx, r1	/* Restore the CPSR			*/
	pop	{r1, r2}	/* Restore r1, r2			*/
	mov	pc, lr		/* Return to caller			*/

/*------------------------------------------------------------------------
 * enable  -  Enable interrupts
 *------------------------------------------------------------------------
 */
enable:
	cpsie	i		/* Enable interrupts			*/
	mov	pc, lr		/* Return				*/

/*------------------------------------------------------------------------
 * pause or halt  -  Place the processor in a hard loop
 *------------------------------------------------------------------------
 */
halt:
pause:
	cpsid	i		/* Disable interrupts		*/
dloop:	b	dloop		/* Dead loop			*/

/*------------------------------------------------------------------------
 * irq_except  -  Dispatch an IRQ exception to higher level IRQ dispatcher
 *------------------------------------------------------------------------
 */
irq_except:
	sub	lr, lr, #4	/* Correct the return address		*/
	srsdb	sp!, #19	/* Save return state on the supervisor	*/
				/*   mode stack				*/
	cps	#19		/* Change to supervisor mode		*/
	push	{r0-r12, lr}	/* Save all registers			*/
	bl	irq_dispatch	/* Call IRQ dispatch			*/
	pop	{r0-r12, lr}	/* Restore all registers		*/
	rfeia	sp!		/* Return from the exception using info	*/
				/*   stored on the stack		*/

/*------------------------------------------------------------------------
 * defexp_handler - Default Exception handler
 *------------------------------------------------------------------------
 */
defexp_handler:
	ldr	r0, =expmsg1
	mov	r1, lr
	bl	kprintf
	ldr	r0, =expmsg2
	bl	panic

/*------------------------------------------------------------------------
 * initevec - Initialize the exception vector
 *------------------------------------------------------------------------
 */
initevec:
	mrc	p15, 0, r0, c1, c0, 0 /* Read the c1-control register	*/
	bic	r0, r0, #ARMV7A_C1CTL_V/* V bit = 0, normal exp. base	*/
	mcr	p15, 0, r0, c1, c0, 0 /* Write the c1-control register	*/
	ldr	r0, =ARMV7A_EV_START  /* Exception base address		*/
	mcr	p15, 0, r0, c12, c0, 0/* Store excp. base addr. in c12	*/
	ldr	r0, =ARMV7A_EV_START  /* Start address of exp. vector	*/
	ldr	r1, =ARMV7A_EV_END    /* End address of exp. vector	*/
	ldr	r2, =expjmpinstr      /* Copy the exp jump instr	*/
	ldr	r2, [r2]	      /*   into register r2		*/
expvec:	str	r2, [r0]	      /* Store the jump instruction	*/
	add	r0, r0, #4	      /*   in the exception vector	*/
	cmp	r0, r1
	bne	expvec
	ldr	r0, =ARMV7A_EH_START  /* Install the default exception	*/
	ldr	r1, =ARMV7A_EH_END    /*   handler for all exceptions	*/
	ldr	r2, =defexp_handler
exphnd: str	r2, [r0]
	add	r0, r0, #4
	cmp	r0, r1
	bne	exphnd
	ldr	r0, =ARMV7A_IRQH_ADDR /* Install the IRQ handler to	*/
	ldr	r1, =irq_except       /*   override the default		*/
	str	r1, [r0]	      /*   exception handler		*/
	mov	pc, lr

/*------------------------------------------------------------------------
 * expjmpinstr - A PC relative jump instruction, copied into exp. vector
 *------------------------------------------------------------------------
 */
expjmpinstr:
	ldr	pc, [pc, #24]
