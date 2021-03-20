/* strlen.c - strlen */

/*------------------------------------------------------------------------
 * strlen - Compute the length of a null-terminated character string, not
 *			counting the null byte.
 *------------------------------------------------------------------------
 */
int	strlen(
	  char		*str		/* string to use		*/
	)
{
	int	len;

	len = 0;

	while(*str++ != '\0') {
		len++;
	}
	return  len;
}
