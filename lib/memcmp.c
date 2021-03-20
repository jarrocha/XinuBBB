/* memcmp.c - memcmp */

/*------------------------------------------------------------------------
 *  memcmp  -  Compare two equal-size blocks of memory.  If there are no
 *			differences, return 0.  Otherwise return >0 or <0
 *			if the first differing byte is greater or less
 *------------------------------------------------------------------------
 */
int	memcmp(
	  const void	*s1,		/* first memory location	*/
	  const void	*s2,		/* second memory location	*/
	  int		n		/* length to compare		*/
	)
{
    const unsigned char *c1;
    const unsigned char *c2;

    for (c1 = s1, c2 = s2; n > 0; n--, c1++, c2++)
    {
        if (*c1 != *c2)
        {
            return ((int)*c1) - ((int)*c2);
        }
    }
    return 0;
}
