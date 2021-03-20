/* start.S - start, bzero */

#include <armv7a.h>

/*------------------------------------------------------------------------
 * start  -  Initial entry point for a Xinu image (ARM)
 *------------------------------------------------------------------------
 */
	.text
	.globl	start			/* Declare entry point global	*/

start:
	/* Load the stack pointer with end of memory */

	ldr	sp, =MAXADDR

	/* Enable the Instruction Cache	*/

	mrc	p15, 0, r0, c1, c0, 0
	orr	r0, r0, #ARMV7A_C1CTL_I
	mcr	p15, 0, r0, c1, c0, 0

	/* Use bzero (below) to zero out the BSS area */

	ldr	r0, =edata
	ldr	r1, =end
	bl	bzero

	/* Call nulluser to initialize the Xinu system  */
	/*    (Note: the call never returns)		*/

	bl	nulluser

	/* Function to zero memory (r0 is lowest addr; r1 is highest)	*/

bzero:
	mov	r2, #0			/* Round address to multiple	*/
	add	r0, r0, #3		/*   of four by adding 3 and	*/
	and	r0, r0, #0xFFFFFFFC	/*   taking the result module 4	*/
bloop:	cmp	r0, r1			/* Loop until last address	*/
	bhs	bexit			/*   has been reached		*/
	str	r2, [r0]		/* Zero four-byte word of memory*/
	add	r0, r0, #4		/* Move to next word		*/
	b	bloop			/* Continue to iterate		*/
bexit:	mov	pc, lr			/* Return to caller		*/
