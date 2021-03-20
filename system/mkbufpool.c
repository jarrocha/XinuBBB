/* mkbufpool.c - mkbufpool */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  mkbufpool  -  Allocate memory for a buffer pool and link the buffers
 *------------------------------------------------------------------------
 */
bpid32	mkbufpool(
	  int32		bufsiz,		/* Size of a buffer in the pool	*/
	  int32		numbufs		/* Number of buffers in the pool*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	bpid32	poolid;			/* ID of pool that is created	*/
	struct	bpentry	*bpptr;		/* Pointer to entry in buftab	*/
	char	*buf;			/* Pointer to memory for buffer	*/

	mask = disable();
	if (bufsiz<BP_MINB || bufsiz>BP_MAXB
	    || numbufs<1 || numbufs>BP_MAXN
	    || nbpools >= NBPOOLS) {
		restore(mask);
		return (bpid32)SYSERR;
	}
	/* Round request to a multiple of 4 bytes */

	bufsiz = ( (bufsiz + 3) & (~3) );

	buf = (char *)getmem( numbufs * (bufsiz+sizeof(bpid32)) );
	if ((int32)buf == SYSERR) {
		restore(mask);
		return (bpid32)SYSERR;
	}
	poolid = nbpools++;
	bpptr = &buftab[poolid];
	bpptr->bpnext = (struct bpentry *)buf;
	bpptr->bpsize = bufsiz;
	if ( (bpptr->bpsem = semcreate(numbufs)) == SYSERR) {
		freemem(buf, numbufs * (bufsiz+sizeof(bpid32)) );
		nbpools--;
		restore(mask);
		return (bpid32)SYSERR;
	}
	bufsiz+=sizeof(bpid32);
	for (numbufs-- ; numbufs>0 ; numbufs-- ) {
		bpptr = (struct bpentry *)buf;
		buf += bufsiz;
		bpptr->bpnext = (struct bpentry *)buf;
	}
	bpptr = (struct bpentry *)buf;
	bpptr->bpnext = (struct bpentry *)NULL;
	restore(mask);
	return poolid;
}
