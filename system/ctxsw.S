/* ctxsw.S - ctxsw (for ARM) */

	.text
	.globl	ctxsw

/*------------------------------------------------------------------------
 * ctxsw -  ARM context switch; the call is ctxsw(&old_sp, &new_sp)
 *------------------------------------------------------------------------
 */

ctxsw:
	push	{r0-r11, lr}		/* Push regs 0 - 11 and lr	*/
	push	{lr}			/* Push return address		*/
	mrs	r2, cpsr		/* Obtain status from coprocess.*/
	push	{r2}			/*   and push onto stack	*/
	str	sp, [r0]		/* Save old process's SP	*/
	ldr	sp, [r1]		/* Pick up new process's SP	*/
	pop	{r0}			/* Use status as argument and	*/
	bl	restore			/*   call restore to restore it	*/
	pop	{lr}			/* Pick up the return address	*/
	pop	{r0-r12}		/* Restore other registers	*/
	mov	pc, r12			/* Return to the new process	*/
