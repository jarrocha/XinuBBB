/* fscanf.c - fscanf, getch, ungetch */

#ifndef	EMPTY
#define EMPTY (-1)
#endif
#ifndef	EOF
#define EOF   (-2)
#endif

extern int getc(int);

static int getch(int, int);
static int ungetch(int, int);
extern int _doscan(register char *, register int **,
                   int (*getc) (int, int), int (*ungetc) (int, int), int,
                   int);

/*------------------------------------------------------------------------
 *  fscanf  -  Read from a device (file) according to a format.
 *			   Return result of _doscan.
 *------------------------------------------------------------------------
 */
int	fscanf(
	  int		dev,		/* device to use		*/
	  char		*fmt,		/* format string		*/
	  int		args		/* num of args in format	*/
	)
{
    int buf;

    buf = EMPTY;
    return (_doscan
            (fmt, (int **)&args, getch, ungetch, dev, (int)(int)&buf));
}

/*------------------------------------------------------------------------
 *  getch  -  Get a character from a device with pushback.
 *------------------------------------------------------------------------
 */
static int	getch(
		  int		dev,	/* device to use		*/
		  int		abuf	/* buffer to use		*/
		)
{
    int *buf = (int *)abuf;

    if (*buf != EOF && *buf != EMPTY)
    {
        *buf = getc(dev);
    }
/* 	if( *buf != EOF ) */
/* 	{ *buf = control(dev, TTY_IOC_NEXTC, 0, 0); } */
    return (*buf);
}

/*------------------------------------------------------------------------
 *  ungetch  -  Pushback a character for getch.
 *------------------------------------------------------------------------
 */
static int	ungetch(
		  int		dev,	/* device to use		*/
		  int		abuf	/* buffer to use		*/
		)
{
    int *buf = (int *)abuf;

    *buf = EMPTY;
    return (*buf);
}
