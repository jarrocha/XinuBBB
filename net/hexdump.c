/* hexdump.c - hexdump */

#include <xinu.h>

/*------------------------------------------------------------------------
 * hexdump - dump an Ethernet/IP/UDP packet in hexadecimal
 *------------------------------------------------------------------------
 */
void	hexdump	(
	  char *p, int32 len		/* ptr to packet and length of	*/
					/*  payload (IP datagram)	*/
	)
{
	int32	i;			/* counts bytes			*/
	int32	word;			/* counts 32 bits words		*/

	for (i=0; i<6; i++) {
		kprintf("%02x", 0xff & *p++);
	}
	kprintf("  ");

	for (i=0; i<6; i++) {
		kprintf("%02x", 0xff & *p++);
	}
	kprintf("  ");

	for (i=0; i<2; i++) {
		kprintf("%02x", 0xff & *p++);
	}
	kprintf("\t\tEnd Ethernet header\n");

	for (word=0,i=0; i<len; i++) {
		if ((i % 4) == 0) {
			word++;
			if (word == 6) {
				kprintf("\tHV TOS LENGTH    ID FRAG    TTL PRO CKSUM    IP_SRC IP_DST\n");
			} else if (word==8) {
				kprintf("\t\t\t\tSRC_PRT DEST-PORT    LENGTH CKSUM\n");
				word = 17;
			} else if ((word % 16) ==0) {
				kprintf("\n");
			} else if (i != 0) {
				kprintf(" ");
			}
		}
		kprintf("%02x", 0xff & *p++);
	}
	kprintf("\n");
	return;
}
/*------------------------------------------------------------------------
 * hexadump - dump an Ethernet/ARP packet in hexadecimal
 *------------------------------------------------------------------------
 */
void	hexadump	(
	  char *p, int32 len		/* ptr to packet and length of	*/
					/*  payload (IP datagram)	*/
	)
{
	int32	i;			/* counts bytes			*/

	for (i=0; i<6; i++) {
		kprintf("%02x", 0xff & *p++);
	}
	kprintf("  ");

	for (i=0; i<6; i++) {
		kprintf("%02x", 0xff & *p++);
	}
	kprintf("  ");

	for (i=0; i<2; i++) {
		kprintf("%02x", 0xff & *p++);
	}
	kprintf("\t\tEnd Ethernet header\n");

	
	for (i=0; i<2; i++) {
		kprintf("%02x", 0xff & *p++);
	}
	kprintf("  ");

	for (i=0; i<2; i++) {
		kprintf("%02x", 0xff & *p++);
	}
	kprintf("  ");
	kprintf("%02x  ", 0xff & *p++);
	kprintf("%02x   ", 0xff & *p++);

	for (i=0; i<2; i++) {
		kprintf("%02x", 0xff & *p++);
	}
	kprintf("  ");

	kprintf("\tHTYP PTYP HLEN PLEN OP\n");



	for (i=0; i<6; i++) {
		kprintf("%02x", 0xff & *p++);
	}
	kprintf("  ");

	for (i=0; i<2; i++) {
		kprintf("%02x", 0xff & *p++);
	}
	kprintf("  ");

	for (i=0; i<6; i++) {
		kprintf("%02x", 0xff & *p++);
	}
	kprintf("  ");

	for (i=0; i<2; i++) {
		kprintf("%02x", 0xff & *p++);
	}
	kprintf("\tSHA   SPA  THA  TPA\n");
	return;
}
