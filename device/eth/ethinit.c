/* ethinit.c - ethinit, eth_phy_read, eth_phy_write */

#include <xinu.h>

struct	eth_a_csreg eth_a_regs;

struct	ethcblk ethertab[1];

/*-----------------------------------------------------------------------
 * eth_phy_read - read a PHY register
 *-----------------------------------------------------------------------
 */
int32	eth_phy_read (
		volatile struct	eth_a_mdio *mdio,/* MDIO CSR pointer	*/
		byte	regadr,	/* PHY Register number	*/
		byte	phyadr,	/* PHY address		*/
		uint32	*value	/* Pointer to value 	*/
	)
{

	/* Ethernet PHY has only 32 registers */

	if(regadr > 31) {
		return SYSERR;
	}

	/* Only 32 possible PHY addresses */

	if(phyadr > 31) {
		return SYSERR;
	}

	/* Wait for the previous access to complete */

	while( (mdio->useraccess0 & ETH_AM335X_MDIOUA_GO) != 0 );

	/* Start the access */

	mdio->useraccess0 = (ETH_AM335X_MDIOUA_GO) |
			    (regadr << 21) |
			    (phyadr << 16);

	/* Wait until the access is complete */

	while( (mdio->useraccess0 & ETH_AM335X_MDIOUA_GO) != 0 );

	/* Check if the access was successful */

	if( (mdio->useraccess0 & ETH_AM335X_MDIOUA_ACK) == 0 ) {
		return SYSERR;
	}

	/* Copy the value read */

	(*value) = mdio->useraccess0 & ETH_AM335X_MDIOUA_DM;

	return OK;
}

/*-----------------------------------------------------------------------
 * eth_phy_write - write a PHY register
 *-----------------------------------------------------------------------
 */
int32	eth_phy_write (
		volatile struct	eth_a_mdio *mdio, /* MDIO CSR pointer	*/
		byte	regadr,	/* PHY register number	*/
		byte	phyadr,	/* PHY address		*/
		uint32	value	/* Value to be written	*/
	)
{

	/* There are only 32 PHY registers */

	if(regadr > 31) {
		return SYSERR;
	}

	/* There are only 32 possible PHY addresses */

	if(phyadr > 31) {
		return SYSERR;
	}

	/* Wait for the previous access to complete */

	while( (mdio->useraccess0 & ETH_AM335X_MDIOUA_GO) != 0);

	/* Start the access */

	mdio->useraccess0 = ETH_AM335X_MDIOUA_GO |
			    ETH_AM335X_MDIOUA_WR |
			    (regadr << 21) |
			    (phyadr << 16) |
			    (value & 0xffff);

	/* Wait for the access to complete */

	while( (mdio->useraccess0 & ETH_AM335X_MDIOUA_GO) != 0);

	return OK;
}

/*-----------------------------------------------------------------------
 * eth_phy_reset - Reset an Ethernet PHY
 *-----------------------------------------------------------------------
 */
int32	eth_phy_reset (
		volatile struct	eth_a_mdio *mdio, /* MDIO CSR pointer	*/
		byte	phyadr			  /* PHY Address	*/
	)
{
	uint32	phyreg;	/* Variable to hold ETH PHY register value	*/
	int32	retries;/* Number of retries				*/
	int32	retval;	/* Return value of functions called here	*/

	/* Read the PHY Control Register */

	retval = eth_phy_read(mdio, ETH_PHY_CTLREG, phyadr, &phyreg);
	if(retval == SYSERR) {
		return SYSERR;
	}

	/* Set the Reset bit and write the register */

	phyreg |= ETH_PHY_CTLREG_RESET;
	eth_phy_write(mdio, ETH_PHY_CTLREG, phyadr, phyreg);

	/* Check if Reset operation is complete */

	for(retries = 0; retries < 10; retries++) {
		if(eth_phy_read(mdio, ETH_PHY_CTLREG, phyadr, &phyreg) == SYSERR) {
			return SYSERR;
		}
		if((phyreg & ETH_PHY_CTLREG_RESET) == 0) {
			break;
		}
		else {
			retries++;
			DELAY(ETH_AM335X_INIT_DELAY);
			continue;
		}
	}
	if(retries >= 3) {
		return SYSERR;
	}

	/* Check if the Link is established */

	for(retries = 0; retries < 10; retries++) {
		if(eth_phy_read(mdio, ETH_PHY_STATREG, phyadr, &phyreg) == SYSERR) {
			return SYSERR;
		}
		if(phyreg & ETH_PHY_STATREG_LINK) {
			break;
		}
		else {
			retries++;
			DELAY(ETH_AM335X_INIT_DELAY);
			continue;
		}
	}
	if(retries >= 3) {
		return SYSERR;
	}

	return OK;
}

/*-----------------------------------------------------------------------
 * ethinit - initialize the TI AM335X ethernet hardware
 *-----------------------------------------------------------------------
 */
int32	ethinit	(
		struct	dentry *devptr
	)
{
	struct	ethcblk *ethptr;		/* Ethernet control blk pointer	*/
	struct	eth_a_tx_desc *tdescptr;/* Tx descriptor pointer	*/
	struct	eth_a_rx_desc *rdescptr;/* Rx descriptor pointer	*/
	struct	netpacket *pktptr;	/* Packet pointer		*/
	struct	eth_a_csreg *csrptr;	/* Ethernet CSR pointer		*/
	uint32	phyreg;			/* Variable to store PHY reg val*/
	int32	retval;			/* Return value			*/
	int32	i;			/* Index variable		*/

	/* Get the Ethernet control block address	*/
	/* from the device table entry			*/

	ethptr = &ethertab[devptr->dvminor];

	/* Store the address of CSRs in the Ethernet control block	*/

	csrptr = &eth_a_regs;
	ethptr->csr = csrptr;

	/* Initialize the addresses of all the submodules	*/

	csrptr->ale = (struct eth_a_ale *)ETH_AM335X_ALE_ADDR;
	csrptr->cpdma = (struct eth_a_cpdma *)ETH_AM335X_CPDMA_ADDR;
	csrptr->sl = (struct eth_a_sl *)ETH_AM335X_SL1_ADDR;
	csrptr->stateram = (struct eth_a_stateram *)
					ETH_AM335X_STATERAM_ADDR;
	csrptr->ss = (struct eth_a_ss *)ETH_AM335X_SS_ADDR;
	csrptr->wr = (struct eth_a_wr *)ETH_AM335X_WR_ADDR;
	csrptr->mdio = (struct eth_a_mdio *)ETH_AM335X_MDIO_ADDR;

	/* Reset all the submodules */

	csrptr->cpdma->reset = 1;
	while(csrptr->cpdma->reset == 1);

	csrptr->sl->reset = 1;
	while(csrptr->sl->reset == 1);

	csrptr->wr->reset = 1;
	while(csrptr->wr->reset == 1) ;

	csrptr->ss->reset = 1;
	while(csrptr->ss->reset == 1) ;

	/* Enable MDIO	*/

	csrptr->mdio->ctrl |= ETH_AM335X_MDIOCTL_EN;

	/* Reset the PHY */

	retval = eth_phy_reset(csrptr->mdio, 0);
	if(retval == SYSERR) {
		kprintf("Cannot reset Ethernet PHY\n");
		return SYSERR;
	}

	retval = eth_phy_read(csrptr->mdio, ETH_PHY_CTLREG, 0, &phyreg);
	if(retval == SYSERR) {
		return SYSERR;
	}

	if( (phyreg & ETH_PHY_CTLREG_SM) == ETH_PHY_10M ) {
		kprintf("Ethernet Link is Up. Speed is 10Mbps\n");
	}
	else if( (phyreg & ETH_PHY_CTLREG_SM) == ETH_PHY_100M ) {
		kprintf("Ethernet Link is Up. Speed is 100Mbps\n");
	}
	else if( (phyreg & ETH_PHY_CTLREG_SM) == ETH_PHY_1000M ) {
		kprintf("Ethernet Link is Up. Speed is 1000Mbps\n");
	}
	else {
		return SYSERR;
	}

	if(phyreg & ETH_PHY_CTLREG_FD) {
		kprintf("Link is Full Duplex\n");
		csrptr->sl->macctrl |= ETH_AM335X_SLCTL_FD;
	}
	else {
		kprintf("Link is Half Duplex\n");
	}

	/* Read the device MAC address */
	for(i = 0; i < 2; i++) {
		ethptr->devAddress[4+i] = *((byte *)(0x44e10630+i));
	}
	for(i = 0; i < 4; i++) {
		ethptr->devAddress[i] = *((byte *)(0x44e10634+i));
	}

	kprintf("MAC Address is: ");
	for(i = 0; i < 5; i++) {
		kprintf("%02X:", ethptr->devAddress[i]);
	}
	kprintf("%02X\n", ethptr->devAddress[5]);

	/* Initialize the rx ring size field */
	ethptr->rxRingSize = ETH_AM335X_RX_RING_SIZE;

	/* Allocate memory for the rx ring */
	ethptr->rxRing = (void*)getmem(sizeof(struct eth_a_rx_desc)*
					ethptr->rxRingSize);
	if((int32)ethptr->rxRing == SYSERR) {
		return SYSERR;
	}

	/* Zero out the rx ring */
	memset((char*)ethptr->rxRing, NULLCH,
		sizeof(struct eth_a_rx_desc)*ethptr->rxRingSize);

	/* Allocate memory for rx buffers */
	ethptr->rxBufs = (void*)getmem(ETH_BUF_SIZE *
					ethptr->rxRingSize);
	if((int32)ethptr->rxBufs == SYSERR) {
		return SYSERR;
	}

	/* Zero out the rx buffers */
	memset((char *)ethptr->rxBufs, NULLCH, ETH_BUF_SIZE *
						ethptr->rxRingSize);

	/* Initialize the rx ring */

	rdescptr = (struct eth_a_rx_desc *)ethptr->rxRing;
	pktptr = (struct netpacket *)ethptr->rxBufs;

	for(i = 0; i < ethptr->rxRingSize; i++) {
		rdescptr->next = rdescptr + 1;
		rdescptr->buffer = (uint32)pktptr->net_ethdst;
		rdescptr->buflen = ETH_BUF_SIZE;
		rdescptr->bufoff = 0;
		rdescptr->stat = ETH_AM335X_RDS_OWN;
		rdescptr++;
		pktptr++;
	}
	(--rdescptr)->next = NULL;

	ethptr->rxHead = 0;
	ethptr->rxTail = 0;
	ethptr->isem = semcreate(0);
	if((int32)ethptr->isem == SYSERR) {
		return SYSERR;
	}

	/* initialize the tx ring size */
	ethptr->txRingSize = ETH_AM335X_TX_RING_SIZE;

	/* Allocate memory for tx ring */
	ethptr->txRing = (void*)getmem(sizeof(struct eth_a_tx_desc)*
					ethptr->txRingSize);
	if((int32)ethptr->txRing == SYSERR) {
		return SYSERR;
	}

	/* Zero out the tx ring */
	memset((char*)ethptr->txRing, NULLCH,
		sizeof(struct eth_a_tx_desc)*ethptr->txRingSize);

	/* Allocate memory for tx buffers */
	ethptr->txBufs = (void*)getmem(ETH_BUF_SIZE *
					ethptr->txRingSize);
	if((int32)ethptr->txBufs == SYSERR) {
		return SYSERR;
	}

	/* Zero out the tx buffers */
	memset((char*)ethptr->txBufs, NULLCH, ETH_BUF_SIZE *
						ethptr->txRingSize);

	/* Initialize the tx ring */

	tdescptr = (struct eth_a_tx_desc *)ethptr->txRing;
	pktptr = (struct netpacket *)ethptr->txBufs;

	for(i = 0; i < ethptr->txRingSize; i++) {
		tdescptr->next = NULL;
		tdescptr->buffer = (uint32)pktptr->net_ethdst;
		tdescptr->buflen = ETH_BUF_SIZE;
		tdescptr->bufoff = 0;
		tdescptr->stat = (ETH_AM335X_TDS_SOP |
				  ETH_AM335X_TDS_EOP |
				  ETH_AM335X_TDS_DIR |
				  ETH_AM335X_TDS_P1);
		tdescptr++;
		pktptr++;
	}

	ethptr->txHead = 0;
	ethptr->txTail = 0;
	ethptr->osem = semcreate(ethptr->txRingSize);
	if((int32)ethptr->osem == SYSERR) {
		return SYSERR;
	}

	/* Enable the ALE and put it into bypass mode */
	csrptr->ale->ctrl = (ETH_AM335X_ALECTL_EN |
			     ETH_AM335X_ALECTL_BY);

	/* Put the ports 0, 1 in forwarding state */
	csrptr->ale->portctl[0] = ETH_AM335X_ALEPCTL_FWD;
	csrptr->ale->portctl[1] = ETH_AM335X_ALEPCTL_FWD;

	/* Start the rx and tx processes in DMA */
	csrptr->cpdma->tx_ctrl = 1;
	csrptr->cpdma->rx_ctrl = 1;

	/* Initialize the head desc pointers for tx and rx */
	csrptr->stateram->tx_hdp[0] = 0;
	csrptr->stateram->rx_hdp[0] = (uint32)ethptr->rxRing;

	/* Enable Rx and Tx in MAC */
	csrptr->sl->macctrl |= ETH_AM335X_SLCTL_EN;

	/* Set interrupt vectors */
	set_evec(ETH_AM335X_TXINT, (uint32)devptr->dvintr);
	set_evec(ETH_AM335X_RXINT, (uint32)devptr->dvintr);

	/* Enable the CPDMA interrupts */
	csrptr->cpdma->tx_intmask_set = 0x1;
	csrptr->cpdma->rx_intmask_set = 0x1;

	/* Route the interrupts to core 0 */
	csrptr->wr->c0_tx_en = 0x1;
	csrptr->wr->c0_rx_en = 0x1;

	return OK;
}
