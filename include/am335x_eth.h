/* am335x_eth.h - Ethernet device definitions for AM335X SoC */

struct	eth_a_ale {
	uint32	idver;
	uint32	res1;
	uint32	ctrl;
	uint32	res2;
	uint32	prescale;
	uint32	res3;
	uint32	unknown_vlan;
	uint32	res4;
	uint32	tblctl;
	uint32	res5[4];
	uint32	tblw2;
	uint32	tblw1;
	uint32	tblw0;
	uint32	portctl[6];
};

#define ETH_AM335X_ALECTL_EN	0x80000000 /* Enable ALE	*/
#define ETH_AM335X_ALECTL_BY	0x00000010 /* Bypass Mode	*/

#define ETH_AM335X_ALEPCTL_FWD	0x00000003 /* Port Forward state*/

struct	eth_a_cpdma {
	uint32	tx_idver;
	uint32	tx_ctrl;
	uint32	tx_teardown;
	uint32	res0;
	uint32	rx_idver;
	uint32	rx_ctrl;
	uint32	rx_teardown;
	uint32	reset;
	uint32	dmactrl;
	uint32	dmastat;
	uint32	rx_buf_off;
	uint32	emctrl;
	uint32	tx_pri_rate[8];
	byte	res1[48];
	uint32	tx_intstat_raw;
	uint32	tx_intstat_masked;
	uint32	tx_intmask_set;
	uint32	tx_intmask_clear;
	uint32	in_vector;
	uint32	eoi_vector;
	byte	res2[8];
	uint32	rx_intstat_raw;
	uint32	rx_intstat_masked;
	uint32	rx_intmask_set;
	uint32	rx_intmask_clear;
	uint32	dma_intstat_raw;
	uint32	dma_intstat_masket;
	uint32	dma_intmask_set;
	uint32	dma_intmask_clear;
	uint32	rx_pendthresh[8];
	uint32	rx_freebuffer[8];
};

struct	eth_a_stateram {
	uint32	tx_hdp[8];
	uint32	rx_hdp[8];
	uint32	tx_cp[8];
	uint32	rx_cp[8];
};

struct	eth_a_sl {
	uint32	idver;
	uint32	macctrl;
	uint32	macstat;
	uint32	reset;
	uint32	rx_maxlen;
	uint32	bofftest;
	uint32	rx_pause;
	uint32	tx_pause;
	uint32	emctrl;
	uint32	rx_pri_map;
	uint32	tx_gap;
};

#define ETH_AM335X_SLCTL_FD	0x00000001	/* Full Duplex	*/
#define ETH_AM335X_SLCTL_LB	0x00000002	/* Loopback 	*/
#define ETH_AM335X_SLCTL_EN	0x00000020	/* Rx/Tx Enable	*/
#define ETH_AM335X_SLCTL_GIG	0x00000080	/* Gigabit mode	*/

struct	eth_a_ss {
	uint32	idver;
	uint32	ctrl;
	uint32	reset;
	uint32	stat_port_en;
	uint32	ptype;
	uint32	soft_idle;
	uint32	thru_rate;
	uint32	gap_thresh;
	uint32	tx_start_wds;
	uint32	flow_ctrl;
	uint32	vlan_type;
	uint32	ts_ltype;
	uint32	dlr_ltype;
};

struct	eth_a_wr {
	uint32	idver;
	uint32	reset;
	uint32	ctrl;
	uint32	int_ctrl;
	uint32	c0_rx_thresh_en;
	uint32	c0_rx_en;
	uint32	c0_tx_en;
	uint32	c0_misc_en;
	uint32	res1[8];
	uint32	c0_rx_thresh_stat;
	uint32	c0_rx_stat;
	uint32	c0_tx_stat;
	uint32	c0_misc_stat;
};

struct	eth_a_mdio {
	uint32	ver;
	uint32	ctrl;
	uint32	alive;
	uint32	link;
	uint32	linkintraw;
	uint32	linkintmasked;
	byte	res1[8];
	uint32	userintraw;
	uint32	userintmasked;
	uint32	userintmaskset;
	uint32	userintmaskclr;
	byte	res2[80];
	uint32	useraccess0;
	uint32	userphysel0;
	uint32	useraccess1;
	uint32	userphysel1;
};

#define ETH_AM335X_MDIOCTL_EN	0x40000000

#define ETH_AM335X_MDIOUA_GO	0x80000000 /* Perorm MDIO access*/
#define ETH_AM335X_MDIOUA_WR	0x40000000 /* Write access	*/
#define ETH_AM335X_MDIOUA_ACK	0x20000000 /* Read Ack		*/
#define ETH_AM335X_MDIOUA_DM	0x0000ffff /* MDIO Data Mask	*/

struct	eth_a_csreg {
	volatile struct	eth_a_ale		*ale;
	volatile struct	eth_a_cpdma 	*cpdma;
	volatile struct	eth_a_stateram 	*stateram;
	volatile struct	eth_a_sl	 	*sl;
	volatile struct	eth_a_ss		*ss;
	volatile struct eth_a_wr		*wr;
	volatile struct	eth_a_mdio	 	*mdio;
};

struct	eth_a_rx_desc {
	struct	eth_a_rx_desc *next;
	uint32	buffer;
	uint16	buflen;
	uint16	bufoff;
	uint16	packlen;
	uint16	stat;
};

#define ETH_AM335X_RDS_SOP	0x8000	/* Start of packet	*/
#define ETH_AM335X_RDS_EOP	0x4000
#define ETH_AM335X_RDS_OWN	0x2000
#define ETH_AM335X_RDS_EOQ	0x1000

#define ETH_AM335X_RX_RING_SIZE	32

struct	eth_a_tx_desc {
	struct	eth_a_tx_desc *next;
	uint32	buffer;
	uint16	buflen;
	uint16	bufoff;
	uint16	packlen;
	uint16	stat;
};

#define ETH_AM335X_TDS_SOP	0x8000
#define ETH_AM335X_TDS_EOP	0x4000
#define ETH_AM335X_TDS_OWN	0x2000
#define ETH_AM335X_TDS_EOQ	0x1000
#define ETH_AM335X_TDS_DIR	0x0010
#define ETH_AM335X_TDS_P1	0x0001

#define ETH_AM335X_TX_RING_SIZE	16

#define ETH_AM335X_ALE_ADDR		0x4A100D00
#define ETH_AM335X_CPDMA_ADDR		0x4A100800
#define ETH_AM335X_STATERAM_ADDR	0x4A100A00
#define ETH_AM335X_SL1_ADDR		0x4A100D80
#define ETH_AM335X_MDIO_ADDR		0x4A101000
#define ETH_AM335X_SS_ADDR		0x4A100000
#define ETH_AM335X_WR_ADDR		0x4A101200

#define ETH_AM335X_RXINT		41
#define ETH_AM335X_TXINT		42

#define	ETH_AM335X_INIT_DELAY	1000000
