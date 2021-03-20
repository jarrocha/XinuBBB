/* ports.h - isbadport */

#define	NPORTS		30		/* Maximum number of ports	*/
#define	PT_MSGS		100		/* Total messages in system	*/
#define	PT_FREE		1		/* Port is free			*/
#define	PT_LIMBO	2		/* Port is being deleted/reset	*/
#define	PT_ALLOC	3		/* Port is allocated		*/

struct	ptnode	{			/* Node on list of messages 	*/
	uint32	ptmsg;			/* A one-word message		*/
	struct	ptnode	*ptnext;	/* Pointer to next node on list	*/
};

struct	ptentry	{			/* Entry in the port table	*/
	sid32	ptssem;			/* Sender semaphore		*/
	sid32	ptrsem;			/* Receiver semaphore		*/
	uint16	ptstate;		/* Port state (FREE/LIMBO/ALLOC)*/
	uint16	ptmaxcnt;		/* Max messages to be queued	*/
	int32	ptseq;			/* Sequence changed at creation	*/
	struct	ptnode	*pthead;	/* List of message pointers	*/
	struct	ptnode	*pttail;	/* Tail of message list		*/
};

extern	struct	ptnode	*ptfree;	/* List of free nodes		*/
extern	struct	ptentry	porttab[];	/* Port table			*/
extern	int32	ptnextid;		/* Next port ID to try when	*/
					/*   looking for a free slot	*/

#define	isbadport(portid)	( (portid)<0 || (portid)>=NPORTS )
