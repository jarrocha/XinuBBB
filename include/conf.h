/* conf.h (GENERATED FILE; DO NOT EDIT) */

/* Device switch table declarations */

/* Device table entry */
struct	dentry	{
	int32   dvnum;
	int32   dvminor;
	char    *dvname;
	devcall (*dvinit) (struct dentry *);
	devcall (*dvopen) (struct dentry *, char *, char *);
	devcall (*dvclose)(struct dentry *);
	devcall (*dvread) (struct dentry *, void *, uint32);
	devcall (*dvwrite)(struct dentry *, void *, uint32);
	devcall (*dvseek) (struct dentry *, int32);
	devcall (*dvgetc) (struct dentry *);
	devcall (*dvputc) (struct dentry *, char);
	devcall (*dvcntl) (struct dentry *, int32, int32, int32);
	void    *dvcsr;
	void    (*dvintr)(void);
	byte    dvirq;
};

extern	struct	dentry	devtab[]; /* one entry per device */

/* Device name definitions */

#define CONSOLE              0	/* type tty      */
#define GPIO0                1	/* type gpio     */
#define GPIO1                2	/* type gpio     */
#define GPIO2                3	/* type gpio     */
#define GPIO3                4	/* type gpio     */
#define NULLDEV              5	/* type null     */
#define ETHER0               6	/* type eth      */
#define NAMESPACE            7	/* type nam      */
#define RDISK                8	/* type rds      */
#define RAM0                 9	/* type ram      */
#define RFILESYS            10	/* type rfs      */
#define RFILE0              11	/* type rfl      */
#define RFILE1              12	/* type rfl      */
#define RFILE2              13	/* type rfl      */
#define RFILE3              14	/* type rfl      */
#define RFILE4              15	/* type rfl      */
#define RFILE5              16	/* type rfl      */
#define RFILE6              17	/* type rfl      */
#define RFILE7              18	/* type rfl      */
#define RFILE8              19	/* type rfl      */
#define RFILE9              20	/* type rfl      */
#define LFILESYS            21	/* type lfs      */
#define LFILE0              22	/* type lfl      */
#define LFILE1              23	/* type lfl      */
#define LFILE2              24	/* type lfl      */
#define LFILE3              25	/* type lfl      */
#define LFILE4              26	/* type lfl      */
#define LFILE5              27	/* type lfl      */
#define SPI0                28	/* type spi      */
#define SPI1                29	/* type spi      */

/* Control block sizes */

#define	Nnull	1
#define	Ngpio	4
#define	Ntty	1
#define	Neth	1
#define	Nrds	1
#define	Nram	1
#define	Nrfs	1
#define	Nrfl	10
#define	Nlfs	1
#define	Nlfl	6
#define	Nnam	1
#define	Nspi	2

#define NDEVS 30


/* Configuration and Size Constants */

#define	NPROC	     100	/* number of user processes		*/
#define	NSEM	     100	/* number of semaphores			*/
#define	IRQBASE	     32		/* base ivec for IRQ0			*/
#define	IRQ_TIMER    IRQ_HW5	/* timer IRQ is wired to hardware 5	*/
#define	IRQ_ATH_MISC IRQ_HW4	/* Misc. IRQ is wired to hardware 4	*/
#define CLKFREQ      200000000	/* 200 MHz clock			*/

#define	LF_DISK_DEV	RAM0
