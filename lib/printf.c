/* printf.c - printf*/

#include <xinu.h>
#include <stdio.h>
#include <stdarg.h>

extern void _fdoprnt(char *, va_list, int (*)(did32, char), int);

/*------------------------------------------------------------------------
 *  printf  -  standard C printf function
 *------------------------------------------------------------------------
 */
int	printf(
	  const char		*fmt,
	  ...
	)
{
    va_list ap;
    syscall putc(did32, char);

    va_start(ap, fmt);
    _fdoprnt((char *)fmt, ap, putc, stdout);
    va_end(ap);

    return 0;
}
