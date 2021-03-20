/* strrchr.c - strrchr */

/*------------------------------------------------------------------------
 *  strrchr  -  Return a pointer to the location in a string at which a
 *			particular character last appears or NULL if the
 *			char is not found
 *------------------------------------------------------------------------
 */
char	*strrchr(
	  const char	*s,		/* String to search		*/
	  int		c		/* Character to locate		*/
	)
{
    char *r = 0;

    for (; *s != '\0'; s++)
    {
        if (*s == (const char)c)
        {
            r = (char *)s;
        }
    }

    if ((const char)c == *s)
    {
        return (char *)s;
    }

    return r;
}
