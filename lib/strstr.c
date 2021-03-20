/* strstr.c - strstr */

/*------------------------------------------------------------------------
 *  strstr  -  Return a pointer to the location in a string at which a
 *			substring appears or NULL if not found
 *------------------------------------------------------------------------
 */
char	*strstr(
	  const char	*cs,		/* String to search		*/
	  const char	*ct		/* Substring to locate		*/
	)
{
    char *cq;
    char *cr;

    for (; *cs != '\0'; cs++)
    {
        if (*cs == *ct)
        {
            cq = (char *)cs;
            cr = (char *)ct;
            while ((*cq != '\0') && (*cr != '\0'))
            {
                if (*cq != *cr)
                {
                    break;
                }
                cq++;
                cr++;
            }
            if ('\0' == *cr)
            {
                return (char *)cs;
            }
        }
    }
    return 0;
}
