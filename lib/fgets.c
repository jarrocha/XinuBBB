/* fgets.c - fgets */

#ifndef	NULL
#define NULL    0
#endif
extern int getc(int);

/*------------------------------------------------------------------------
 *  fgets  -  Read a newline-terminated string from device (file).
 *------------------------------------------------------------------------
 */
char	*fgets(
	  char		*s,		/* buffer for result		*/
	  int		n,		/* maximum read length		*/
	  int		dev		/* device to read from		*/
	)
{
    int c = 0;
    char *cs;

    cs = s;

    /* Read characters until maximum read length, */
    /*  end of line, or end of file               */
    while ((--n > 0) && ((c = getc(dev)) >= 0))
    {
        *cs++ = c;
        if (('\n' == c) || ('\r' == c))
        {
            break;
        }
    }

    /* Check for EOF or empty string */
    if ((c < 0) && (cs == s))
    {
        return NULL;
    }

    /* Terminate string and return */
    *cs++ = '\0';
    return s;
}
