/* qsort.c - qsort, qs1, qsexc, qstexc */

static int (*qscmp) (char *, char *);
static int qses;
static void qs1(char *, char *);
static void qsexc(char *, char *);
static void qstexc(char *, char *, char *);

/*------------------------------------------------------------------------
 *  qsort  -  Use quicksort to sort an array
 *------------------------------------------------------------------------
 */
void	qsort(
	  char		*a,		/* Array to sort		*/
	  unsigned	n,		/* Length of the array		*/
	  int		es,		/* Pivot value			*/
	  int		(*fc)(char *, char *)	/* Comparison function	*/
	)
{
    qscmp = fc;
    qses = es;
    qs1(a, a + n * es);
}

/*------------------------------------------------------------------------
 *  qs1  -  internal quicksort function
 *------------------------------------------------------------------------
 */
static void	qs1(
		  char		*a,
		  char		*l
		)
{
    register char *i, *j;
    register int es;
    char *lp, *hp;
    int c;
    unsigned n;

    es = qses;

  start:
    if ((n = l - a) <= es)
    {
        return;
    }
    n = es * (n / (2 * es));
    hp = lp = a + n;
    i = a;
    j = l - es;
    for (;;)
    {
        if (i < lp)
        {
            if ((c = (*qscmp) (i, lp)) == 0)
            {
                qsexc(i, lp -= es);
                continue;
            }
            if (c < 0)
            {
                i += es;
                continue;
            }
        }

      loop:
        if (j > hp)
        {
            if ((c = (*qscmp) (hp, j)) == 0)
            {
                qsexc(hp += es, j);
                goto loop;
            }
            if (c > 0)
            {
                if (i == lp)
                {
                    qstexc(i, hp += es, j);
                    i = lp += es;
                    goto loop;
                }
                qsexc(i, j);
                j -= es;
                i += es;
                continue;
            }
            j -= es;
            goto loop;
        }

        if (i == lp)
        {
            if (lp - a >= l - hp)
            {
                qs1(hp + es, l);
                l = lp;
            }
            else
            {
                qs1(a, lp);
                a = hp + es;
            }
            goto start;
        }

        qstexc(j, lp -= es, i);
        j = hp -= es;
    }
}

/*------------------------------------------------------------------------
 *  qsexc  -  internal quicksort function
 *------------------------------------------------------------------------
 */
static void	qsexc(
		  char		*i,
		  char		*j
		)
{
    register char *ri, *rj, c;
    int n;

    n = qses;
    ri = i;
    rj = j;
    do
    {
        c = *ri;
        *ri++ = *rj;
        *rj++ = c;
    }
    while (--n);
}

/*------------------------------------------------------------------------
 *  qstexc  -  internal quicksort function
 *------------------------------------------------------------------------
 */
static void	qstexc(
		  char		*i,
		  char		*j,
		  char		*k
		)
{
    register char *ri, *rj, *rk;
    int c;
    int n;

    n = qses;
    ri = i;
    rj = j;
    rk = k;
    do
    {
        c = *ri;
        *ri++ = *rk;
        *rk++ = *rj;
        *rj++ = c;
    }
    while (--n);
}
