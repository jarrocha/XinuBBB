/* strcmp.c - strcmp */

/*------------------------------------------------------------------------
 * strcmp  -  Compare two strings, returning 0 of they are the same <0 if
 *		first is lexically less and >0 if first is lexically >
 *------------------------------------------------------------------------
 */
int	strcmp(
	  char		*str1,
	  char		*str2
	)
{
	while (*str1 == *str2) {
		if (*str1 == '\0') {
	            return 0;
        	}
		str1++;
		str2++;
	}
	if (*str1 < *str2) {
		return -1;
	} else {
		return  1;
	}
}
