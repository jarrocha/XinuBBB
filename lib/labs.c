/* labs.c - labs */

/*------------------------------------------------------------------------
 *  labs  -  Calculates the absolute value of a long.
 *------------------------------------------------------------------------
 */
long	labs(
	  long		arg
	)
{
    if (arg < 0)
        arg = -arg;
    return (arg);
}
