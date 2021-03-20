/**
 * @file ethloop.h
 *
 * $Id: ethloop.h 2076 2009-09-24 23:05:39Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _ETHLOOP_H_
#define _ETHLOOP_H_


#define ELOOP_MTU          1500
#define ELOOP_LINKHDRSIZE  ETH_HDR_LEN

#define ELOOP_BUFSIZE      ELOOP_MTU + ELOOP_LINKHDRSIZE
#define ELOOP_NBUF         100

#define ELOOP_CTRL_GETHOLD	1
#define ELOOP_CTRL_SETFLAG  2
#define ELOOP_CTRL_CLRFLAG	3

#define ELOOP_FLAG_HOLDNXT	0x01  /**< place next written pkt in hold  */
#define ELOOP_FLAG_DROPNXT	0x04  /**< drop next written pkt           */
#define ELOOP_FLAG_DROPALL	0x08  /**< drop all written pkts           */

#define ELOOP_STATE_FREE        0
#define ELOOP_STATE_ALLOC       1

/**
 * Loopback device control block
 */
struct ethloop
{
    int state;                      /**< device state                       */
    struct dentry *dev;                    /**< device table entry                 */
    int poolid;                     /**< poolid for the buffer pool         */
    byte flags;                    /**< flags                              */

    /* Packet queue */
    int index;                  /**< index of first packet in buffer    */
    sid32	sem;              /**< number of packets in buffer        */
    int count;                      /**< number of packets in buffer        */
    char *buffer[ELOOP_NBUF];   /**< input buffer                       */
    int pktlen[ELOOP_NBUF];         /**< length of packet in buffer         */

    /* Hold packet */
    sid32	hsem;                 /**< number of held packets             */
    char *hold;                 /**< hold buffer                        */
    int holdlen;                    /**< length of packet in hold buffer    */

    /* Statistics */
    uint32 nout;                      /**< number of packets written          */
};

extern struct ethloop elooptab[];

/* Driver functions */
devcall ethloopInit(struct dentry *);
devcall ethloopOpen(struct dentry *);
devcall ethloopClose(struct dentry *);
devcall ethloopRead(struct dentry *, void *, uint32);
devcall ethloopWrite(struct dentry *, void *, uint32);
devcall ethloopControl(struct dentry *, int, int32, int32);

#endif                          /* _ETHLOOP_H_ */
