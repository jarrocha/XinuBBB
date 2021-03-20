/* ethread.c - ethread */

#include <xinu.h>

/*------------------------------------------------------------------------
 * ethread - read an incoming packet on TI AM335X Ethernet
 *------------------------------------------------------------------------
 */
int32	ethread	(
		struct	dentry *devptr,
		void	*buf,
		uint32	count
	)
{
	struct	ethcblk *ethptr;	/* Ethernet ctl blk ptr	*/
	struct	eth_a_csreg *csrptr;	/* Ethernet CSR pointer	*/
	struct	eth_a_rx_desc *rdescptr;/* Rx Desc. pointer	*/
	struct	eth_a_rx_desc *prev;	/* Prev Rx desc pointer	*/
	uint32	retval;			/* Num of bytes returned*/

	ethptr = &ethertab[devptr->dvminor];

	/* Get the pointer to Ethernet CSR */
	csrptr = (struct eth_a_csreg *)ethptr->csr;

	/* Wait for a packet */
	wait(ethptr->isem);

	/* Get pointer to the descriptor */
	rdescptr = (struct eth_a_rx_desc *)ethptr->rxRing +
						ethptr->rxHead;

	/* Read the packet length */
	retval = rdescptr->packlen;
	if(retval > count) {
		retval = count;
	}

	/* Copy the packet into user provided buffer */
	memcpy((char *)buf, (char *)rdescptr->buffer, retval);

	/* Initialize the descriptor for next packet */
	rdescptr->stat = ETH_AM335X_RDS_OWN;
	rdescptr->bufoff = 0;
	rdescptr->buflen = ETH_BUF_SIZE;
	rdescptr->packlen = 0;
	rdescptr->next = NULL;

	/* Insert the descriptor into Rx queue */
	prev = (struct eth_a_rx_desc *)csrptr->stateram->rx_hdp[0];
	if(prev == NULL) {
		kprintf("hdp 0, adding %x\n", rdescptr);
		csrptr->stateram->rx_hdp[0] = (uint32)rdescptr;
	}
	else {
		while(prev->next != NULL) {
			prev = prev->next;
		}
		prev->next = rdescptr;
	}

	/* Increment the head index of rx ring */
	ethptr->rxHead++;
	if(ethptr->rxHead >= ethptr->rxRingSize) {
		ethptr->rxHead = 0;
	}

	return retval;
}
