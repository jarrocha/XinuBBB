/* fgetc.c - fgetc */

extern int getc(int);

#ifndef	EOF
#define EOF      (-2)
#endif

/*------------------------------------------------------------------------
 *  fgetc  -  Read a character from a device (file).
 *			  Return character read, EOF if error.
 *------------------------------------------------------------------------
 */
int	fgetc(
	  int		dev
	)
{
    int result = (int)getc(dev);

    if (0 > result)
    {
        return EOF;
    }
    else
    {
        return result;
    }
}
