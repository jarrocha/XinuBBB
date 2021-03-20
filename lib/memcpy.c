/* memcpy.c - memcpy */

/*------------------------------------------------------------------------
 *  memcpy  -  Copy a block of memory from src to dst, and return a
 *			  pointer to the destination
 *------------------------------------------------------------------------
 */
void	*memcpy(
	  void		*s,	/* Destination address			*/
	  const void	*ct,	/* source address			*/
	  int		n	/* number of bytes to copy		*/
	)
{
    register int i;
    char *dst = (char *)s;
    char *src = (char *)ct;

    for (i = 0; i < n; i++)
    {
        *dst++ = *src++;
    }
    return s;
}
