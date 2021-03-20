/* arp_dump.c - arp_dump */

#include <xinu.h>

/*------------------------------------------------------------------------
 * arp_dump  -  Display the contents of an RP cache entry
 *------------------------------------------------------------------------
 */
void	arp_dump(
	  struct arpentry *arptr	/* Pointer to ARP cache entry	*/
	)
{
	int32	i;			/* Index for a MAC address	*/

	kprintf("State=%d, PID=%d  IP=%08x, HW=", arptr->arstate,
		arptr->arpid, arptr->arpaddr);
	kprintf(" %02X", arptr->arhaddr[0]);
	for (i = 1; i < ARP_HALEN; i++) {
			printf(":%02X", arptr->arhaddr[i]);
	}
	kprintf("\n");
	return;
}
