/* icu.h */

#define	ICU1	0x20		/* I/O port address, 8259A #1		*/
#define	ICU2	0xA0		/* I/O port address, 8258A #2		*/

#define	OCR	ICU1		/* Operation Command Register		*/
#define	IMR	(ICU1+1)	/* Interrupt Mask Register		*/

#define	EOI	0x20		/* non-specific end of interrupt	*/
