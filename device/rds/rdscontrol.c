/* rdscontrol.c - rdscontrol */

#include <xinu.h>

/*------------------------------------------------------------------------
 * rdscontrol  -  Provide control functions for the remote disk
 *------------------------------------------------------------------------
 */
devcall	rdscontrol (
	 struct dentry	*devptr,	/* Entry in device switch table	*/
	 int32	func,			/* The control function to use	*/
	 int32	arg1,			/* Argument #1			*/
	 int32	arg2			/* Argument #2			*/
	)
{
	struct	rdscblk	*rdptr;		/* Pointer to control block	*/
	struct	rdbuff	*bptr;		/* Ptr to buffer that will be	*/
					/*   placed on the req. queue	*/
	struct	rdbuff	*pptr;		/* Ptr to "previous" node on	*/
					/*   a list			*/
	struct	rd_msg_dreq msg;	/* Buffer for delete request	*/
	struct	rd_msg_dres resp;	/* Buffer for delete response	*/
	char	*to, *from;		/* Used during name copy	*/
	int32	retval;			/* Return value			*/

	/* Verify that device is currently open */

	rdptr = &rdstab[devptr->dvminor];
	if (rdptr->rd_state != RD_OPEN) {
		return SYSERR;
	}

	/* Ensure rdsprocess is runnning */

	if ( ! rdptr->rd_comruns ) {
		rdptr->rd_comruns = TRUE;
		resume(rdptr->rd_comproc);
	}

	switch (func) {

	/* Synchronize writes */

	case RDS_CTL_SYNC:

		/* Allocate a buffer to use for the request list */

		bptr = rdsbufalloc(rdptr);
		if (bptr == (struct rdbuff *)SYSERR) {
			return SYSERR;
		}

		/* Form a sync request */

		bptr->rd_op = RD_OP_SYNC;
		bptr->rd_refcnt = 1;
		bptr->rd_blknum = 0;		/* Unused */
		bptr->rd_status = RD_INVALID;
		bptr->rd_pid = getpid();

		/* Insert new request into list just before tail */

		pptr = rdptr->rd_rtprev;
		rdptr->rd_rtprev = bptr;
		bptr->rd_next = pptr->rd_next;
		bptr->rd_prev = pptr;
		pptr->rd_next = bptr;

		/* Prepare to wait until item is processed */

		recvclr();

		/* Signal then semaphore to start communication */

		signal(rdptr->rd_reqsem);

		/* Block to wait for a message */

		bptr = (struct rdbuff *)receive();
		if (bptr == (struct rdbuff *)SYSERR) {	
			return SYSERR;
		}
		break;

	/* Delete the remote disk (entirely remove it) */

	case RDS_CTL_DEL:

		/* Handcraft a message for the server that requests	*/
		/*	deleting the disk with the specified ID		*/

		msg.rd_type = htons(RD_MSG_DREQ);/* Request deletion	*/
		msg.rd_status = htons(0);
		msg.rd_seq = 0;	/* rdscomm will insert sequence # later	*/
		to = msg.rd_id;
		memset(to, NULLCH, RD_IDLEN);	/* Initialize to zeroes	*/
		from = rdptr->rd_id;
		while ( (*to++ = *from++) != NULLCH ) {	/* copy ID	*/
			;
		}

		/* Send message and receive response */

		retval = rdscomm((struct rd_msg_hdr *)&msg,
					sizeof(struct rd_msg_dreq),
			 (struct rd_msg_hdr *)&resp,
					sizeof(struct rd_msg_dres),
					rdptr);

		/* Check response */

		if (retval == SYSERR) {
			return SYSERR;
		} else if (retval == TIMEOUT) {
			kprintf("Timeout during remote file delete\n\r");
			return SYSERR;
		} else if (ntohs(resp.rd_status) != 0) {
			return SYSERR;
		}

		/* Close local device */

		return rdsclose(devptr);

	default:
		kprintf("rfsControl: function %d not valid\n\r", func);
		return SYSERR;
	}

	return OK;
}
