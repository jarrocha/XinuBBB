/* in file addargs.c */
extern	status	addargs(pid32, int32, int32[], int32,char *, void *);

/* in file am335x_eth_init.c */
extern	int32	am335x_eth_init(struct ethcblk *);

/* in file arp.c */
extern	void	arp_init(void);
extern	status	arp_resolve(uint32, byte[]);
extern	void	arp_in(struct arppacket *);
extern	int32	arp_alloc(void);
extern	void	arp_ntoh(struct arppacket *);
extern	void	arp_hton(struct arppacket *);

/* in file ascdate.c */
extern	status	ascdate(uint32, char *);

/* in file bufinit.c */
extern	status	bufinit(void);

/* in file chprio.c */
extern	pri16	chprio(pid32, pri16);

/* in file clkupdate.S */
extern	uint32	clkcount(void);

/* in file clkhandler.c */
extern	interrupt clkhandler(void);

/* in file clkinit.c */
extern	void	clkinit(void);

/* in file clkint.S */
extern	void	clkint(void);

/* in file close.c */
extern	syscall	close(did32);

/* in file control.c */
extern	syscall	control(did32, int32, int32, int32);

/* in file create.c */
extern	pid32	create(void *, uint32, pri16, char *, uint32, ...);

/* in file ctxsw.S */
extern	void	ctxsw(void *, void *);

/* in file dhcp.c */
extern	uint32	getlocalip(void);

/* in file dns.c */
extern	uint32	dnslookup(char *);

/* in file dot2ip.c */
extern	uint32	dot2ip(char *, uint32 *);

/* in file ethcontrol.c */
extern	int32	ethcontrol(struct dentry *, int32, int32, int32);

/* in file ethinit.c */
extern 	int32	ethinit(struct dentry *);

/* in file ethhandler.c */
extern	interrupt ethhandler(uint32);

/* in file ethread.c */
extern	int32	ethread(struct dentry *, void *, uint32);

/* in file ethwrite.c */
extern	int32	ethwrite(struct dentry *, void *, uint32);

/* in file evec.c */

extern	int32	initintc(void);
extern	int32	initevec(void);
extern	int32	set_evec(uint32, uint32);
extern	void	trap(int32);

/* in file exception.c */
extern  void exception(int32, int32*);

/* in file freebuf.c */
extern	syscall	freebuf(char *);

/* in file freemem.c */
extern	syscall	freemem(char *, uint32);

/* in file getbuf.c */
extern	char	*getbuf(bpid32);

/* in file getc.c */
extern	syscall	getc(did32);

/* in file getitem.c */
extern	pid32	getfirst(qid16);
extern	pid32	getlast(qid16);
extern	pid32	getitem(pid32);

/* in file getmem.c */
extern	char	*getmem(uint32);

/* in file getpid.c */
extern	pid32	getpid(void);

/* in file getprio.c */
extern	syscall	getprio(pid32);

/* in file getstk.c */
extern	char	*getstk(uint32);

/* in file getticks.c */
extern	uint32	getticks(void);

/* in file gettime.c */
extern	status	gettime(uint32 *);

/* in file getutime.c */
extern	status	getutime(uint32 *);

/* in file gpiocontrol.c */
extern	devcall	gpiocontrol(struct dentry *, int32, int32, int32);

/* in file gpiohandler.c */
extern	void	gpiohandler(uint32);

/* in file gpioinit.c */
extern	devcall	gpioinit(struct dentry *);

/* in file gpioselect.c */
extern devcall gpioselect(uint32, uint32, uint32);

/* in file gpioread.c */
extern	devcall	gpioread(struct dentry *,char *, int32);

/* in file gpiowrite.c */
extern	devcall	gpiowrite(struct dentry *,  char *, int32);

/* in file halt.S */
extern	void	halt(void);

/* in file icmp.c */

extern	void	icmp_init(void);
extern	void	icmp_in(struct netpacket *);
extern	int32	icmp_register(uint32);
extern	int32	icmp_recv(int32, char *, int32, uint32);
extern	status	icmp_send(uint32, uint16, uint16, uint16, char *, int32);
extern	struct	netpacket *icmp_mkpkt(uint32, uint16, uint16, uint16, char *, int32);
extern	status	icmp_release(int32);
extern	uint16	icmp_cksum(char *, int32);
extern	void	icmp_hton(struct netpacket *);
extern	void	icmp_ntoh(struct netpacket *);

/* in file init.c */
extern	syscall	init(did32);

/* in file insert.c */
extern	status	insert(pid32, qid16, int32);

/* in file insertd.c */
extern	status	insertd(pid32, qid16, int32);

/* in file intr.S */
extern	intmask	disable(void);
extern	void	enable(void);

/* in file ioerr.c */
extern	devcall	ioerr(void);

/* in file ionull.c */
extern	devcall	ionull(void);

/* in file ip.c */
extern	void	ip_in(struct netpacket *);
extern	status	ip_send(struct netpacket *);
extern	void	ip_local(struct netpacket *);
extern	status	ip_out(struct netpacket *);
extern	int32	ip_route(uint32);
extern	uint16	ipcksum(struct netpacket *);
extern	void	ip_ntoh(struct netpacket *);
extern	void	ip_hton(struct netpacket *);
extern	process	ipout(void);
extern	status	ip_enqueue(struct netpacket *);

/* in file net.c */
extern	void	net_init(void);
extern	process	netin(void);
extern	process	netout(void);
extern	process	rawin(void);
extern	void	eth_hton(struct netpacket *);
extern	void	eth_ntoh(struct netpacket *);
extern	uint16	getport(void);

/* in file kill.c */
extern	syscall	kill(pid32);

/* in file lexan.c */
extern	int32	lexan(char *, int32, char *, int32 *, int32 [], int32 []);

/* in file lfibclear.c */
extern	void	lfibclear(struct lfiblk *, int32);

/* in file lfibget.c */
extern	void	lfibget(did32, ibid32, struct lfiblk *);

/* in file lfibput.c */
extern	status	lfibput(did32, ibid32, struct lfiblk *);

/* in file lfdbfree.c */
extern	status	lfdbfree(did32, dbid32);

/* in file lfdballoc.c */
extern	dbid32	lfdballoc(struct lfdbfree *);

/* in file lfflush.c */
extern	status	lfflush(struct lflcblk *);

/* in file lfgetmode.c */
extern	int32	lfgetmode(char *);

/* in file lfiballoc.c */
extern	ibid32	lfiballoc(void);

/* in file lflclose.c */
extern	devcall	lflclose(struct dentry *);

/* in file lflcontrol.c */
extern	devcall	lflcontrol(struct dentry *, int32, int32, int32);

/* in file lflgetc.c */
extern	devcall	lflgetc(struct dentry *);

/* in file lflinit.c */
extern	devcall	lflinit(struct dentry *);

/* in file lflputc.c */
extern	devcall	lflputc(struct dentry *, char);

/* in file lflread.c */
extern	devcall	lflread(struct dentry *, char *, int32);

/* in file lflseek.c */
extern	devcall	lflseek(struct dentry *, uint32);

/* in file lflwrite.c */
extern	devcall	lflwrite(struct dentry *, char *, int32);

/* in file lfscheck.c */
extern	status	lfscheck(struct lfdir *);

/* in file lfscreate.c */
extern  status  lfscreate(did32, ibid32, uint32);

/* in file lfsinit.c */
extern	devcall	lfsinit(struct dentry *);

/* in file lfsopen.c */
extern	devcall	lfsopen(struct dentry *, char *, char *);

/* in file lfsetup.c */
extern	status	lfsetup(struct lflcblk *);

/* in file lftruncate.c */
extern	status	lftruncate(struct lflcblk *);

/* in file lpgetc.c */
extern	devcall	lpgetc(struct dentry *);

/* in file lpinit.c */
extern	devcall	lpinit(struct dentry *);

/* in file lpopen.c */
extern	devcall	lpopen(struct dentry *, char *, char *);

/* in file lpputc.c */
extern	devcall	lpputc(struct dentry *, char);

/* in file lpread.c */
extern	devcall	lpread(struct dentry *, char *, int32);

/* in file lpwrite.c */
extern	devcall	lpwrite(struct dentry *, char *, int32);

/* in file mark.c */
extern	void	_mkinit(void);

/* in file memcpy.c */
extern	void	*memcpy(void *, const void *, int32);

/* in file memcpy.c */
extern	int32	*memcmp(void *, const void *, int32);

/* in file memset.c */
extern  void    *memset(void *, const int, int32);

/* in file mkbufpool.c */
extern	bpid32	mkbufpool(int32, int32);

/* in file mount.c */
extern	syscall	mount(char *, char *, did32);
extern	int32	namlen(char *, int32);

/* in file naminit.c */
extern	status	naminit(void);

/* in file nammap.c */
extern	devcall	nammap(char *, char[], did32);
extern	did32	namrepl(char *, char[]);
extern	status	namcpy(char *, char *, int32);

/* in file namopen.c */
extern	devcall	namopen(struct dentry *, char *, char *);

/* in file newqueue.c */
extern	qid16	newqueue(void);

/* in file open.c */
extern	syscall	open(did32, char *, char *);

/* in file panic.c */
extern	void	panic(char *);

/* in file pci.c */
extern	int32	pci_init(void);

/* in file pdump.c */
extern	void	pdump(struct netpacket *);
extern	void	pdumph(struct netpacket *);

/* in file platinit.c */
extern	void	platinit(void);

/* in file ptclear.c */
extern	void	_ptclear(struct ptentry *, uint16, int32 (*)(int32));

/* in file ptcount.c */
extern	int32	ptcount(int32);

/* in file ptcreate.c */
extern	syscall	ptcreate(int32);

/* in file ptdelete.c */
extern	syscall	ptdelete(int32, int32 (*)(int32));

/* in file ptinit.c */
extern	syscall	ptinit(int32);

/* in file ptrecv.c */
extern	uint32	ptrecv(int32);

/* in file ptreset.c */
extern	syscall	ptreset(int32, int32 (*)(int32));

/* in file ptsend.c */
extern	syscall	ptsend(int32, umsg32);

/* in file putc.c */
extern	syscall	putc(did32, char);

/* in file queue.c */
extern	pid32	enqueue(pid32, qid16);
extern	pid32	dequeue(qid16);

/* in file ramclose.c */
extern	devcall	ramclose(struct dentry *);

/* in file raminit.c */
extern	devcall	raminit(struct dentry *);

/* in file ramopen.c */
extern	devcall	ramopen(struct dentry *, char *, char *);

/* in file ramread.c */
extern	devcall	ramread(struct dentry *, char *, int32);

/* in file ramwrite.c */
extern	devcall	ramwrite(struct dentry *, char *, int32);

/* in file rdsclose.c */
extern	devcall	rdsclose(struct dentry *);

/* in file rdscontrol.c */
extern	devcall	rdscontrol(struct dentry *, int32, int32, int32);

/* in file rdsinit.c */
extern	devcall	rdsinit(struct dentry *);

/* in file rdsopen.c */
extern	devcall	rdsopen(struct dentry *, char *, char *);

/* in file rdsread.c */
extern	devcall	rdsread(struct dentry *, char *, int32);

/* in file rdswrite.c */
extern	devcall	rdswrite(struct dentry *, char *, int32);

/* in file rdsbufalloc.c */
extern	struct	rdbuff * rdsbufalloc(struct rdscblk *);

/* in file rdscomm.c */
extern	status	rdscomm(struct rd_msg_hdr *, int32, struct rd_msg_hdr *,
		int32, struct rdscblk *);

/* in file rdsprocess.c */
extern	void	rdsprocess(struct rdscblk *);

/* in file read.c */
extern	syscall	read(did32, char *, uint32);

/* in file ready.c */
extern	status	ready(pid32);

/* in file receive.c */
extern	umsg32	receive(void);

/* in file recvclr.c */
extern	umsg32	recvclr(void);

/* in file recvtime.c */
extern	umsg32	recvtime(int32);

/* in file resched.c */
extern	void	resched(void);
extern	status	resched_cntl(int32);

/* in file intr.S */
extern	void	restore(intmask);

/* in file resume.c */
extern	pri16	resume(pid32);

/* in file rfsgetmode.c */
extern	int32	rfsgetmode(char * );

/* in file rflclose.c */
extern	devcall	rflclose(struct dentry *);

/* in file rfscontrol.c */
extern	devcall	rfscontrol(struct dentry *, int32, int32, int32);

/* in file rflgetc.c */
extern	devcall	rflgetc(struct dentry *);

/* in file rflinit.c */
extern	devcall	rflinit(struct dentry *);

/* in file rflputc.c */
extern	devcall	rflputc(struct dentry *, char );

/* in file rflread.c */
extern	devcall	rflread(struct dentry *, char *, int32 );

/* in file rflseek.c */
extern	devcall	rflseek(struct dentry *, uint32 );

/* in file rflwrite.c */
extern	devcall	rflwrite(struct dentry *, char *, int32 );

/* in file rfsndmsg.c */
extern	status	rfsndmsg(uint16, char *);

/* in file rfsinit.c */
extern	devcall	rfsinit(struct dentry *);

/* in file rfsopen.c */
extern	devcall	rfsopen(struct dentry  *devptr, char *, char *);

/* in file rfscomm.c */
extern	int32	rfscomm(struct rf_msg_hdr *, int32, struct rf_msg_hdr *, int32);

/* in file rdsclose.c */
extern	devcall	rdsclose(struct dentry *);

/* in file rdscontrol.c */
extern	devcall	rdscontrol(struct dentry *, int32, int32, int32);

/* in file rdsinit.c */
extern	devcall	rdsinit(struct dentry *);

/* in file rdsopen.c */
extern	devcall	rdsopen(struct dentry *, char *, char *);

/* in file rdsread.c */
extern	devcall	rdsread(struct dentry *, char *, int32);

/* in file rdswrite.c */
extern	devcall	rdswrite(struct dentry *, char *, int32);

/* in file rdsbufalloc.c */
extern	struct	rdbuff * rdsbufalloc(struct rdscblk *);

/* in file rdscomm.c */
extern	status	rdscomm(struct rd_msg_hdr *, int32, struct rd_msg_hdr *, int32, struct rdscblk *);

/* in file rdsprocess.c */
extern	void	rdsprocess(struct rdscblk *);

/* in file seek.c */
extern	syscall	seek(did32, uint32);

/* in file semcount.c */
extern	syscall	semcount(sid32);

/* in file semcreate.c */
extern	sid32	semcreate(int32);

/* in file semdelete.c */
extern	syscall	semdelete(sid32);

/* in file semreset.c */
extern	syscall	semreset(sid32, int32);

/* in file send.c */
extern	syscall	send(pid32, umsg32);

/* in file shell.c */
extern 	process shell(did32);

/* in file signal.c */
extern	syscall	signal(sid32);

/* in file signaln.c */
extern	syscall	signaln(sid32, int32);

/* in file sleep.c */
extern	syscall	sleepms(int32);
extern	syscall	sleep(int32);

/* in file spicontrol.c */
extern	devcall	spicontrol(struct dentry *, int32, int32, int32);

/* in spiinit.c */
extern	int32	spiinit(struct dentry *);

/* in file start.S */
extern	int32	inb(int32);
extern	int32	inw(int32);
extern	int32	inl(int32);
extern	int32	outb(int32, int32);
extern	int32	outw(int32, int32);
extern	int32	outl(int32, int32);
extern	int32	outsw(int32, int32, int32);
extern	int32	insw(int32, int32 ,int32);

/* in file suspend.c */
extern	syscall	suspend(pid32);

/* in file ttycontrol.c */
extern	devcall	ttycontrol(struct dentry *, int32, int32, int32);

/* in file ttygetc.c */
extern	devcall	ttygetc(struct dentry *);

/* in file ttyhandle_in.c */
extern	void	ttyhandle_in(struct ttycblk *, struct uart_csreg *);

/* in file ttyhandle_out.c */
extern	void	ttyhandle_out(struct ttycblk *, struct uart_csreg *);

/* in file ttyhandler.c */
extern	void	ttyhandler(uint32);

/* in file ttyinit.c */
extern	devcall	ttyinit(struct dentry *);

/* in file ttykickout.c */
extern	void	ttykickout(struct uart_csreg *);

/* in file ttyputc.c */
extern	devcall	ttyputc(struct dentry *, char);

/* in file ttyread.c */
extern	devcall	ttyread(struct dentry *, char *, int32);

/* in file ttywrite.c */
extern	devcall	ttywrite(struct dentry *, char *, int32);

/* in file udp.c */

extern	void	udp_init(void);
extern	void	udp_in(struct netpacket *);
extern	uid32	udp_register(uint32, uint16, uint16);
extern	int32	udp_recv(uid32, char *, int32, uint32);
extern	int32	udp_recvaddr(uid32, uint32 *, uint16 *, char *, int32, uint32);
extern	status	udp_send(uid32, char *, int32);
extern	status	udp_sendto(uid32, uint32, uint16, char *, int32);
extern	status	udp_release(uid32);
extern	void	udp_ntoh(struct netpacket *);
extern	void	udp_hton(struct netpacket *);


/* in file unsleep.c */
extern	syscall	unsleep(pid32);

/* in file userret.c */
extern	void	userret(void);

/* in file wait.c */
extern	syscall	wait(sid32);

/* in file wakeup.c */
extern	void	wakeup(void);

/* in file write.c */
extern	syscall	write(did32, char *, uint32);

/* in file xdone.c */
extern	void	xdone(void);

/* in file yield.c */
extern	syscall	yield(void);

/* NETWORK BYTE ORDER CONVERSION NOT NEEDED ON A BIG-ENDIAN COMPUTER */
#define	htons(x)   ( ( 0xff & ((x)>>8) ) | ( (0xff & (x)) << 8 ) )
#define	htonl(x)   (  (((x)>>24) & 0x000000ff) | (((x)>> 8) & 0x0000ff00) | \
		      (((x)<< 8) & 0x00ff0000) | (((x)<<24) & 0xff000000) )
#define	ntohs(x)   ( ( 0xff & ((x)>>8) ) | ( (0xff & (x)) << 8 ) )
#define	ntohl(x)   (  (((x)>>24) & 0x000000ff) | (((x)>> 8) & 0x0000ff00) | \
		      (((x)<< 8) & 0x00ff0000) | (((x)<<24) & 0xff000000) )
