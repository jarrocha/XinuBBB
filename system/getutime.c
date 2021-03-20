/* getutime.c - getutime */

#include <xinu.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * getutime  -  Obtain time in seconds past Jan 1, 1970, UCT (GMT)
 *------------------------------------------------------------------------
 */
status	getutime(
	  uint32  *timvar		/* Location to store the result	*/
	)
{
	uint32	now;			/* Current time in xinu format	*/
	int32	retval;			/* Return value from call	*/
	uid32	slot;			/* Slot in UDP table		*/
	struct	ntp {			/* Format of an NTP message	*/
		byte	livn;		/* LI:2 VN:3 and mode:3 fields	*/
		byte	strat;		/* Stratum			*/
		byte	poll;		/* Poll interval		*/
		byte	precision;	/* Precision			*/
		uint32	rootdelay;	/* Root delay			*/
		uint32	rootdisp;	/* Root dispersion		*/
		uint32	refid;		/* Reference identifier		*/
		uint32	reftimestamp[2];/* Reference timestamp		*/
		uint32	oritimestamp[2];/* Originate timestamp		*/
		uint32	rectimestamp[2];/* Receive timestamp		*/
		uint32	trntimestamp[2];/* Transmit timestamp		*/
	} ntpmsg;

	if (Date.dt_bootvalid) {	/* Return time from local info	*/
		*timvar = Date.dt_boot + clktime;
		return OK;
	}

	/* Verify that we have obtained an IP address */

	if (getlocalip() == SYSERR) {
		return SYSERR;
	}

	/* If the DHCP response did not contain an NTP server address	*/
	/*	use the default server					*/

	if (NetData.ntpserver == 0) {
		if (dot2ip(TIMESERVER, &NetData.ntpserver) == SYSERR) {
			return SYSERR;
		}
	}

	/* Contact the time server to get the date and time */

	slot = udp_register(NetData.ntpserver, TIMERPORT, TIMELPORT);
	if (slot == SYSERR) {
		fprintf(stderr,"getutime: cannot register a udp port %d\n",
					TIMERPORT);
		return SYSERR;
	}

	/* Send a request message to the NTP server */

	memset((char *)&ntpmsg, 0x00, sizeof(ntpmsg));
	ntpmsg.livn = 0x1b;	/* Client request, protocol version 3 */
	retval = udp_send(slot,	(char *)&ntpmsg, sizeof(ntpmsg));
	if (retval == SYSERR) {
		fprintf(stderr,"getutime: cannot send to the server\n");
		udp_release(slot);
		return SYSERR;
	}

	/* Read the response from the NTP server */

	retval = udp_recv(slot, (char *) &ntpmsg, sizeof(ntpmsg),
							 TIMETIMEOUT);
	if ( (retval == SYSERR) || (retval == TIMEOUT) ) {
		udp_release(slot);
		return SYSERR;
	}
	udp_release(slot);

	/* Extract the seconds since Jan 1900 and convert */

	now = ntim2xtim( ntohl(ntpmsg.trntimestamp[0]) );
	Date.dt_boot = now - clktime;
	Date.dt_bootvalid = TRUE;
	*timvar = now;
	return OK;
}
