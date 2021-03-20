/* strchr.c - strchr */

/*------------------------------------------------------------------------
 *  strchr  -  Returns a pointer to the location in a string at which a
 *			   character appears or NULL if char not found
 *------------------------------------------------------------------------
 */
char	*strchr(
	  const char	*s,		/* String to search		*/
	  int		c		/* Character to locate		*/
	)
{
    for (; *s != '\0'; s++)
    {
        if (*s == (const char)c)
        {
            return (char *)s;
        }
    }

    if ((const char)c == *s)
    {
        return (char *)s;
    }

    return 0;
}
