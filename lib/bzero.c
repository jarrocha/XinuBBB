/* bzero.c - bzero */

/*------------------------------------------------------------------------
 *  bzero  -  Clears a block of characters to 0s.
 *------------------------------------------------------------------------
 */
void	bzero(
	  void		*p,
	  int		len
	)
{
    int n;
    char *pch = (char *)p;

    if ((n = len) <= 0)
    {
        return;
    }
    do
    {
        *pch++ = 0;
    }
    while (--n);
}
