/* conf.c (GENERATED FILE; DO NOT EDIT) */

#include <xinu.h>


extern	devcall	ioerr(void);
extern	devcall	ionull(void);

/* Device independent I/O switch */

struct	dentry	devtab[NDEVS] =
{
/**
 * Format of entries is:
 * dev-number, minor-number, dev-name,
 * init, open, close,
 * read, write, seek,
 * getc, putc, control,
 * dev-csr-address, intr-handler, irq
 */

/* CONSOLE is tty */
	{ 0, 0, "CONSOLE",
	  (void *)ttyinit, (void *)ionull, (void *)ionull,
	  (void *)ttyread, (void *)ttywrite, (void *)ioerr,
	  (void *)ttygetc, (void *)ttyputc, (void *)ttycontrol,
	  (void *)0x44e09000, (void *)ttyhandler, 72 },

/* GPIO0 is gpio */
	{ 1, 0, "GPIO0",
	  (void *)gpioinit, (void *)ionull, (void *)ionull,
	  (void *)gpioread, (void *)gpiowrite, (void *)ioerr,
	  (void *)ionull, (void *)ionull, (void *)gpiocontrol,
	  (void *)0x44e07000, (void *)gpiohandler, 96 },

/* GPIO1 is gpio */
	{ 2, 1, "GPIO1",
	  (void *)gpioinit, (void *)ionull, (void *)ionull,
	  (void *)gpioread, (void *)gpiowrite, (void *)ioerr,
	  (void *)ionull, (void *)ionull, (void *)gpiocontrol,
	  (void *)0x4804c000, (void *)gpiohandler, 98 },

/* GPIO2 is gpio */
	{ 3, 2, "GPIO2",
	  (void *)gpioinit, (void *)ionull, (void *)ionull,
	  (void *)gpioread, (void *)gpiowrite, (void *)ioerr,
	  (void *)ionull, (void *)ionull, (void *)gpiocontrol,
	  (void *)0x481ac000, (void *)gpiohandler, 32 },

/* GPIO3 is gpio */
	{ 4, 3, "GPIO3",
	  (void *)gpioinit, (void *)ionull, (void *)ionull,
	  (void *)gpioread, (void *)gpiowrite, (void *)ioerr,
	  (void *)ionull, (void *)ionull, (void *)gpiocontrol,
	  (void *)0x481ae000, (void *)gpiohandler, 62 },

/* NULLDEV is null */
	{ 5, 0, "NULLDEV",
	  (void *)ionull, (void *)ionull, (void *)ionull,
	  (void *)ionull, (void *)ionull, (void *)ioerr,
	  (void *)ionull, (void *)ionull, (void *)ioerr,
	  (void *)0x0, (void *)ioerr, 0 },

/* ETHER0 is eth */
	{ 6, 0, "ETHER0",
	  (void *)ethinit, (void *)ioerr, (void *)ioerr,
	  (void *)ethread, (void *)ethwrite, (void *)ioerr,
	  (void *)ioerr, (void *)ioerr, (void *)ethcontrol,
	  (void *)0x0, (void *)ethhandler, 0 },

/* NAMESPACE is nam */
	{ 7, 0, "NAMESPACE",
	  (void *)naminit, (void *)namopen, (void *)ioerr,
	  (void *)ioerr, (void *)ioerr, (void *)ioerr,
	  (void *)ioerr, (void *)ioerr, (void *)ioerr,
	  (void *)0x0, (void *)ioerr, 0 },

/* RDISK is rds */
	{ 8, 0, "RDISK",
	  (void *)rdsinit, (void *)rdsopen, (void *)rdsclose,
	  (void *)rdsread, (void *)rdswrite, (void *)ioerr,
	  (void *)ioerr, (void *)ioerr, (void *)rdscontrol,
	  (void *)0x0, (void *)ionull, 0 },

/* RAM0 is ram */
	{ 9, 0, "RAM0",
	  (void *)raminit, (void *)ramopen, (void *)ramclose,
	  (void *)ramread, (void *)ramwrite, (void *)ioerr,
	  (void *)ioerr, (void *)ioerr, (void *)ioerr,
	  (void *)0x0, (void *)ionull, 0 },

/* RFILESYS is rfs */
	{ 10, 0, "RFILESYS",
	  (void *)rfsinit, (void *)rfsopen, (void *)ioerr,
	  (void *)ioerr, (void *)ioerr, (void *)ioerr,
	  (void *)ioerr, (void *)ioerr, (void *)rfscontrol,
	  (void *)0x0, (void *)ionull, 0 },

/* RFILE0 is rfl */
	{ 11, 0, "RFILE0",
	  (void *)rflinit, (void *)ioerr, (void *)rflclose,
	  (void *)rflread, (void *)rflwrite, (void *)rflseek,
	  (void *)rflgetc, (void *)rflputc, (void *)ioerr,
	  (void *)0x0, (void *)ionull, 0 },

/* RFILE1 is rfl */
	{ 12, 1, "RFILE1",
	  (void *)rflinit, (void *)ioerr, (void *)rflclose,
	  (void *)rflread, (void *)rflwrite, (void *)rflseek,
	  (void *)rflgetc, (void *)rflputc, (void *)ioerr,
	  (void *)0x0, (void *)ionull, 0 },

/* RFILE2 is rfl */
	{ 13, 2, "RFILE2",
	  (void *)rflinit, (void *)ioerr, (void *)rflclose,
	  (void *)rflread, (void *)rflwrite, (void *)rflseek,
	  (void *)rflgetc, (void *)rflputc, (void *)ioerr,
	  (void *)0x0, (void *)ionull, 0 },

/* RFILE3 is rfl */
	{ 14, 3, "RFILE3",
	  (void *)rflinit, (void *)ioerr, (void *)rflclose,
	  (void *)rflread, (void *)rflwrite, (void *)rflseek,
	  (void *)rflgetc, (void *)rflputc, (void *)ioerr,
	  (void *)0x0, (void *)ionull, 0 },

/* RFILE4 is rfl */
	{ 15, 4, "RFILE4",
	  (void *)rflinit, (void *)ioerr, (void *)rflclose,
	  (void *)rflread, (void *)rflwrite, (void *)rflseek,
	  (void *)rflgetc, (void *)rflputc, (void *)ioerr,
	  (void *)0x0, (void *)ionull, 0 },

/* RFILE5 is rfl */
	{ 16, 5, "RFILE5",
	  (void *)rflinit, (void *)ioerr, (void *)rflclose,
	  (void *)rflread, (void *)rflwrite, (void *)rflseek,
	  (void *)rflgetc, (void *)rflputc, (void *)ioerr,
	  (void *)0x0, (void *)ionull, 0 },

/* RFILE6 is rfl */
	{ 17, 6, "RFILE6",
	  (void *)rflinit, (void *)ioerr, (void *)rflclose,
	  (void *)rflread, (void *)rflwrite, (void *)rflseek,
	  (void *)rflgetc, (void *)rflputc, (void *)ioerr,
	  (void *)0x0, (void *)ionull, 0 },

/* RFILE7 is rfl */
	{ 18, 7, "RFILE7",
	  (void *)rflinit, (void *)ioerr, (void *)rflclose,
	  (void *)rflread, (void *)rflwrite, (void *)rflseek,
	  (void *)rflgetc, (void *)rflputc, (void *)ioerr,
	  (void *)0x0, (void *)ionull, 0 },

/* RFILE8 is rfl */
	{ 19, 8, "RFILE8",
	  (void *)rflinit, (void *)ioerr, (void *)rflclose,
	  (void *)rflread, (void *)rflwrite, (void *)rflseek,
	  (void *)rflgetc, (void *)rflputc, (void *)ioerr,
	  (void *)0x0, (void *)ionull, 0 },

/* RFILE9 is rfl */
	{ 20, 9, "RFILE9",
	  (void *)rflinit, (void *)ioerr, (void *)rflclose,
	  (void *)rflread, (void *)rflwrite, (void *)rflseek,
	  (void *)rflgetc, (void *)rflputc, (void *)ioerr,
	  (void *)0x0, (void *)ionull, 0 },

/* LFILESYS is lfs */
	{ 21, 0, "LFILESYS",
	  (void *)lfsinit, (void *)lfsopen, (void *)ioerr,
	  (void *)ioerr, (void *)ioerr, (void *)ioerr,
	  (void *)ioerr, (void *)ioerr, (void *)ioerr,
	  (void *)0x0, (void *)ionull, 0 },

/* LFILE0 is lfl */
	{ 22, 0, "LFILE0",
	  (void *)lflinit, (void *)ioerr, (void *)lflclose,
	  (void *)lflread, (void *)lflwrite, (void *)lflseek,
	  (void *)lflgetc, (void *)lflputc, (void *)lflcontrol,
	  (void *)0x0, (void *)ionull, 0 },

/* LFILE1 is lfl */
	{ 23, 1, "LFILE1",
	  (void *)lflinit, (void *)ioerr, (void *)lflclose,
	  (void *)lflread, (void *)lflwrite, (void *)lflseek,
	  (void *)lflgetc, (void *)lflputc, (void *)lflcontrol,
	  (void *)0x0, (void *)ionull, 0 },

/* LFILE2 is lfl */
	{ 24, 2, "LFILE2",
	  (void *)lflinit, (void *)ioerr, (void *)lflclose,
	  (void *)lflread, (void *)lflwrite, (void *)lflseek,
	  (void *)lflgetc, (void *)lflputc, (void *)lflcontrol,
	  (void *)0x0, (void *)ionull, 0 },

/* LFILE3 is lfl */
	{ 25, 3, "LFILE3",
	  (void *)lflinit, (void *)ioerr, (void *)lflclose,
	  (void *)lflread, (void *)lflwrite, (void *)lflseek,
	  (void *)lflgetc, (void *)lflputc, (void *)lflcontrol,
	  (void *)0x0, (void *)ionull, 0 },

/* LFILE4 is lfl */
	{ 26, 4, "LFILE4",
	  (void *)lflinit, (void *)ioerr, (void *)lflclose,
	  (void *)lflread, (void *)lflwrite, (void *)lflseek,
	  (void *)lflgetc, (void *)lflputc, (void *)lflcontrol,
	  (void *)0x0, (void *)ionull, 0 },

/* LFILE5 is lfl */
	{ 27, 5, "LFILE5",
	  (void *)lflinit, (void *)ioerr, (void *)lflclose,
	  (void *)lflread, (void *)lflwrite, (void *)lflseek,
	  (void *)lflgetc, (void *)lflputc, (void *)lflcontrol,
	  (void *)0x0, (void *)ionull, 0 },

/* SPI0 is spi */
	{ 28, 0, "SPI0",
	  (void *)spiinit, (void *)ionull, (void *)ionull,
	  (void *)ionull, (void *)ionull, (void *)ionull,
	  (void *)ionull, (void *)ionull, (void *)spicontrol,
	  (void *)0x48030000, (void *)ionull, 0 },

/* SPI1 is spi */
	{ 29, 1, "SPI1",
	  (void *)spiinit, (void *)ionull, (void *)ionull,
	  (void *)ionull, (void *)ionull, (void *)ionull,
	  (void *)ionull, (void *)ionull, (void *)spicontrol,
	  (void *)0x481a0000, (void *)ionull, 0 }
};
