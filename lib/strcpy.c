/* strcpy.c - strcpy */

/*------------------------------------------------------------------------
 * strcpy - Copy the string given by the second argument into the first
 *------------------------------------------------------------------------
 */
int	strcpy(
	  char		*tar,		/* target string		*/
	  char		*src		/* source string		*/
	)
{
	while ( (*tar++ = *src++) != '\0') {
		;
	}
	return 0;
}
