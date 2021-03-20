/* fputc.c - fputc */

extern int putc(int, char);

#ifndef	SYSERR
#define	SYSERR   (-1)
#endif
#ifndef	EOF
#define EOF      (-2)
#endif

/*------------------------------------------------------------------------
 *  fputc  -  Write a character string to a device (file).
 *			  Return character written, EOF if error.
 *------------------------------------------------------------------------
 */
int	fputc(
	  int		c,		/* char to write		*/
	  int		dev		/* device to use		*/
	)
{
    if (SYSERR == (int)putc(dev, c))
    {
        return EOF;
    }
    else
    {
        return c;
    }
}
