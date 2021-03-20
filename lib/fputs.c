/* fputs.c - fputs */

extern int putc(int, char);

/*------------------------------------------------------------------------
 *  fputs  -  Write a null-terminated string to a device (file).
 *			  Return result of last putc.
 *------------------------------------------------------------------------
 */
int	fputs(
	  char		*s,		/* string to write		*/
	  int		dev		/* device to write to		*/
	)
{
    int r = 0, c;

    while ((c = (*s++)))
    {
        r = putc(dev, c);
    }
    return r;
}
