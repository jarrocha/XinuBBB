/* ionull.c - ionull */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  ionull  -  Do nothing (used for "don't care" entries in devtab)
 *------------------------------------------------------------------------
 */
devcall	ionull(void)
{
	return OK;
}
