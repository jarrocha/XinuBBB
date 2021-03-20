/* getticks.c - getticks */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  getticks  -  Retrieve the number of clock ticks since CPU reset
 *------------------------------------------------------------------------
 */
uint32  	getticks()
{
	uint32 ret;
        asm volatile ( "MRC p15, 0, %0, c9, c13, 0\t\n": "=r"(ret) );
	return ret;
}
