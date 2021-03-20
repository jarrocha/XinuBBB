/* sscanf.c - sscanf, sgetch, sungetch */

#define EOF   (-2)

static int sgetch(int, char **);
static int sungetch(int, char **);
extern int _doscan(register char *, register int **,
                   int (*getc) (int, char **),
                   int (*ungetc) (int, char **), int, int);

/*------------------------------------------------------------------------
 *  sscanf  -  Read from a string according to a format.
 *			   Return result of _doscan.
 *------------------------------------------------------------------------
 */
int	sscanf(
	  char		*str,		/* String to read from		*/
	  char		*fmt,		/* Format string		*/
	  int		args		/* Num of args in format string	*/
	)
{
    char *s;

    s = str;
    return (_doscan(fmt, (int **)&args, sgetch, sungetch, 0, (int)&s));
}

/*------------------------------------------------------------------------
 *  sgetch  -  Get the next character from a string
 *------------------------------------------------------------------------
 */
static int	sgetch(
		  int		dummy,	/* Dummy unused variable	*/
		  char		**cpp	/* String to use		*/
		)
{
    return (*(*cpp) == '\0' ? EOF : *(*cpp)++);
}

/*------------------------------------------------------------------------
 *  sungetch  -  Pushback a character in a string
 *------------------------------------------------------------------------
 */
static int	sungetch(
		  int		dummy,	/* Dummy unused variable	*/
		  char		**cpp	/* String for pushback		*/
		)
{
    return (*(*cpp)--);
}
