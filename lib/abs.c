/* abs.c - abs */

/*------------------------------------------------------------------------
 *  abs  -  Return the absolute value of an integer.
 *------------------------------------------------------------------------
 */
int	abs(
	  int		arg
	)
{
    if (arg < 0)
        arg = -arg;
    return (arg);
}
