/* ethhandler.c - ethhandler */

#include <xinu.h>

/*------------------------------------------------------------------------
 * ethhandler - TI AM335X Ethernet Interrupt Handler
 *------------------------------------------------------------------------
 */
interrupt ethhandler (
		uint32	xnum	/* IRQ number	*/
	)
{
	struct	eth_a_csreg *csrptr;		/* Ethernet CSR pointer	*/
	struct	eth_a_tx_desc *tdescptr;	/* Tx desc pointer	*/
	struct	eth_a_rx_desc *rdescptr;	/* Rx desc pointer	*/
	struct	ethcblk *ethptr = &ethertab[0];	/* Ethernet ctl blk ptr	*/

	csrptr = (struct eth_a_csreg *)ethptr->csr;

	if(xnum == ETH_AM335X_TXINT) {	/* Transmit interrupt */

		/* Get pointer to first desc in queue	*/

		tdescptr = (struct eth_a_tx_desc *)ethptr->txRing +
							ethptr->txHead;

		/* Defer scheduling until all descs are processed */

		resched_cntl(DEFER_START);

		while(semcount(ethptr->osem) < (int32)ethptr->txRingSize) {

			/* If desc owned by DMA, check if we need to	*/
			/* Restart the transmission			*/

			if(tdescptr->stat & ETH_AM335X_TDS_OWN) {
				if(csrptr->stateram->tx_hdp[0] == 0) {
					csrptr->stateram->tx_hdp[0] =
							(uint32)tdescptr;
				}
				break;
			}

			/* Acknowledge the interrupt	*/

			csrptr->stateram->tx_cp[0] = (uint32)tdescptr;

			/* Increment the head index of the queue	*/
			/* And go to the next descriptor in queue	*/

			ethptr->txHead++;
			tdescptr++;
			if(ethptr->txHead >= ethptr->txRingSize) {
				ethptr->txHead = 0;
				tdescptr = (struct eth_a_tx_desc *)
							ethptr->txRing;
			}

			/* Signal the output semaphore */

			signal(ethptr->osem);
		}

		/* Acknowledge the transmit interrupt */

		csrptr->cpdma->eoi_vector = 0x2;

		/* Resume rescheduling	*/

		resched_cntl(DEFER_STOP);
	}
	else if(xnum == ETH_AM335X_RXINT) {	/* Receive interrupt */

		/* Get the pointer to last desc in the queue	*/

		rdescptr = (struct eth_a_rx_desc *)ethptr->rxRing +
							ethptr->rxTail;

		/* Defer scheduling until all descriptors are processed	*/

		resched_cntl(DEFER_START);

		while(semcount(ethptr->isem) < (int32)ethptr->rxRingSize) {

			/* Check if we need to restart the DMA	*/

			if(rdescptr->stat & ETH_AM335X_RDS_OWN) {
				if(csrptr->stateram->rx_hdp[0] == 0) {
					csrptr->stateram->rx_hdp[0] =
							(uint32)rdescptr;
				}
				break;
			}

			/* Acknowledge the interrupt	*/

			csrptr->stateram->rx_cp[0] = (uint32)rdescptr;

			/* Increment the tail index of the queue	*/
			/* And go to the next descriptor in the queue	*/

			ethptr->rxTail++;
			rdescptr++;
			if(ethptr->rxTail >= ethptr->rxRingSize) {
				ethptr->rxTail = 0;
				rdescptr = (struct eth_a_rx_desc *)
							ethptr->rxRing;
			}

			/* Signal the input semaphore	*/

			signal(ethptr->isem);
		}

		/* Acknowledge the receive interrupt */

		csrptr->cpdma->eoi_vector = 0x1;

		/* Resume rescheduling	*/

		resched_cntl(DEFER_STOP);
	}
}
