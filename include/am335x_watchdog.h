/* am335x_watchdog.h */

/* Control and Status registers for watchdog timer */

struct	watchdog_csreg {
	volatile uint32 widr;	/* Identification register	*/
	volatile uint32 res1[3];/* Reserved space		*/
	volatile uint32	wdsc;	/* System control register	*/
	volatile uint32	wdst;	/* Status register		*/
	volatile uint32	wisr;	/* Interrupt status register	*/
	volatile uint32	wier;	/* Interrupt enable register	*/
	volatile uint32	res2;	/* Reserved space		*/
	volatile uint32	wclr;	/* Control register		*/
	volatile uint32	wcrr;	/* Counter register		*/
	volatile uint32	wldr;	/* Load register		*/
	volatile uint32	wtgr;	/* Trigger register		*/
	volatile uint32	wwps;	/* Write posting bits register	*/
	volatile uint32	res3[3];/* Reserved space		*/
	volatile uint32	wdly;	/* Delay configuration register	*/
	volatile uint32	wspr;	/* Start/stop register		*/
	volatile uint32	res4[2];/* Reserved space		*/
	volatile uint32	wirqstatraw;
				/* Raw interrupt status register*/
	volatile uint32	wirqstat;/* Interrupt status register	*/
	volatile uint32	wirqenset;
				/* Interrupt enable set register*/
	volatile uint32	wirqenclr;
				/* Interrupt enable clr register*/
};

#define	WDTADDR	0x44E35000
