/* spi.h - SPI definitions */

struct	spi_csreg {
	volatile uint32	rev;		/* SPI revision			*/
	volatile uint32	res1[67];	/* Reserved address space	*/
	volatile uint32	sysconfig;	/* System configuration register*/
	volatile uint32	sysstatus;	/* System status register	*/
	volatile uint32	irqstatus;	/* IRQ status register		*/
	volatile uint32	irqenable;	/* IRQ Enable register		*/
	volatile uint32 res2;		/* Reserved address space	*/
	volatile uint32	syst;		/* System test register		*/
	volatile uint32 modulctrl;	/* Module control register	*/
	struct {
	 volatile uint32 chconf;	/* Channel conf register	*/
	 volatile uint32 chstat;	/* Channel status register	*/
	 volatile uint32 chctrl;	/* Channel control register	*/
	 volatile uint32 chtx;		/* Channel xmit register	*/
	 volatile uint32 chrx;		/* Channel receive register	*/
	} ch[3];
	volatile uint32 xferlevel;	/* Transfer level register	*/
	volatile uint32 daftx;		/* FIFO xmit register		*/
	volatile uint32 dafrx;		/* FIFO receive register	*/
};

#define	SPI_0_ADDR	0x48030000
#define	SPI_1_ADDR	0x481A0000

#define	SPI_0_PADCTL_SCLK	0x44E10950
#define	SPI_0_PADCTL_D0		0x44E10954
#define	SPI_0_PADCTL_D1		0x44E10958
#define	SPI_0_PADCTL_CS0	0x44E1095C

#define	SPI_1_PADCTL_SCLK	0x44E10990
#define	SPI_1_PADCTL_D0		0x44E10994
#define	SPI_1_PADCTL_D1		0x44E10998
#define	SPI_1_PADCTL_CS0	0x44E1099C

/* Bit definitions of SPI registers */

#define	SPI_SYSCONFIG_SOFTRESET	0x00000002

#define	SPI_SYSSTATUS_RESETDONE	0x00000001

#define	SPI_MODULCTRL_SINGLE	0x00000001
#define	SPI_MODULCTRL_PIN34	0x00000002
#define	SPI_MODULCTRL_MS	0x00000004

#define	SPI_CHCONF_PHA		0x00000001
#define	SPI_CHCONF_POL		0x00000002
#define	SPI_CHCONF_CLKD		0x0000003c
#define	SPI_CHCONF_EPOL		0x00000040
#define	SPI_CHCONF_WL		0x00000F80
#define	SPI_CHCONF_TRM		0x00003000
#define	SPI_CHCONF_DMAW		0x00004000
#define	SPI_CHCONF_DMAR		0x00008000
#define	SPI_CHCONF_DPE0		0x00010000
#define	SPI_CHCONF_DPE1		0x00020000
#define	SPI_CHCONF_IS		0x00040000
#define	SPI_CHCONF_TURBO	0x00080000
#define	SPI_CHCONF_FORCE	0x00100000
#define	SPI_CHCONF_SBE		0x00800000
#define	SPI_CHCONF_SBPOL	0x01000000
#define	SPI_CHCONF_TCS		0x06000000
#define	SPI_CHCONF_FFEW		0x08000000
#define	SPI_CHCONF_FFER		0x10000000
#define	SPI_CHCONF_CLKG		0x20000000

#define	SPI_CHCTRL_EN		0x00000001

#define	SPI_CHSTAT_RXS		0x00000001
#define	SPI_CHSTAT_TXS		0x00000002

struct	spi_transfer {
	byte	*txbuf;	/* Transmit buffer address	*/
	byte	*rxbuf;	/* Receive buffer address	*/
	int32	length;	/* No. of  bytes to transfer	*/
};

#define	SPI_CTRL_TRANSFER	1
