/* strncmp.c - strncmp */

/*------------------------------------------------------------------------
 *  strncmp  -  Compare at most n bytes of two strings, returning
 *			>0 if s1>s2,  0 if s1=s2,  and <0 if s1<s2
 *------------------------------------------------------------------------
 */
int	strncmp(
	  char		*s1,		/* First memory location	*/
	  char		*s2,		/* Second memory location	*/
	  int		n		/* Length to compare		*/
	)
{

    while (--n >= 0 && *s1 == *s2++)
    {
        if (*s1++ == '\0')
        {
            return 0;
        }
    }
    return (n < 0 ? 0 : *s1 - *--s2);
}
