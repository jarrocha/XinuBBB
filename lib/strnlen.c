/* strnlen.c - strnlen */

/*------------------------------------------------------------------------
 *  strnlen  -  Return the length of a string (number of non-NULL bytes),
 *			up to the max specified by n
 *------------------------------------------------------------------------
 */
int	strnlen(
	  const char	*s,		/* string			*/
	  unsigned int	len		/* max length			*/
		)
{
    int n;

    n = 0;
    while (*s++ && n < len)
        n++;

    return (n);
}
