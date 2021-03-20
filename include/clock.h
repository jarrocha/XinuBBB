/* clock.h */

extern	uint32	clktime;	/* current time in secs since boot	*/
extern  uint32  count1000;      /* ms since last clock tick             */

extern	qid16	sleepq;		/* queue for sleeping processes		*/
extern	int32	slnonempty;	/* nonzero if sleepq is nonempty	*/
extern	int32	*sltop;		/* ptr to key in first item on sleepq	*/
extern	uint32	preempt;	/* preemption counter			*/

struct am335x_timer1ms {
	uint32	tidr;		/* Identification register	*/
	uint32	res1[3];	/* Reserved			*/
	uint32	tiocp_cfg;	/* OCP Interface register	*/
	uint32	tistat;		/* Status register		*/
	uint32	tisr;		/* Interrupt status register	*/
	uint32	tier;		/* Interrupt enable register	*/
	uint32	twer;		/* Wakeup enable register	*/
	uint32	tclr;		/* Optional features		*/
	uint32	tcrr;		/* Internal counter value	*/
	uint32	tldr;		/* Timer load value		*/
	uint32	ttgr;		/* Trigger register		*/
	uint32	twps;		/* Write posting register	*/
	uint32	tmar;		/* Match register		*/
	uint32	tcar1;		/* Capture register 1		*/
	uint32	tsicr;		/* Synchronous interface control*/
	uint32	tcar2;		/* Capture register 2		*/
	uint32	tpir;		/* Positive increment register	*/
	uint32	tnir;		/* Negative increment register	*/
	uint32	tcvr;		/* 1ms control register		*/
	uint32	tocr;		/* Overflow mask register	*/
	uint32	towr;		/* no. of overflows		*/
};

#define AM335X_TIMER1MS_ADDR			0x44E31000
#define AM335X_TIMER1MS_IRQ			67

#define AM335X_TIMER1MS_TIOCP_CFG_SOFTRESET	0x00000002
#define AM335X_TIMER1MS_TISTAT_RESETDONE	0x00000001

#define AM335X_TIMER1MS_TISR_MAT_IT_FLAG	0x00000001
#define AM335X_TIMER1MS_TISR_OVF_IT_FLAG	0x00000002
#define AM335X_TIMER1MS_TISR_TCAR_IT_FLAG	0x00000004

#define AM335X_TIMER1MS_TIER_MAT_IT_ENA		0x00000001
#define AM335X_TIMER1MS_TIER_OVF_IT_ENA		0x00000002
#define AM335X_TIMER1MS_TIER_TCAR_IT_ENA	0x00000004

#define AM335X_TIMER1MS_TCLR_ST			0x00000001
#define AM335X_TIMER1MS_TCLR_AR			0x00000002

#define AM335X_TIMER1MS_CLKCTRL_ADDR		0x44E004C4
#define AM335X_TIMER1MS_CLKCTRL_EN		0x00000002
