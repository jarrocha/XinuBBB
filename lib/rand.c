/* rand.c - rand, rand_r, srand */

#include <stdlib.h>

static unsigned int rand_seed = 1;
extern	int rand_r(unsigned int*);

/*------------------------------------------------------------------------
 *  rand  -  Calculates a uniform random number [0, RAND_MAX]
 *------------------------------------------------------------------------
 */
int	rand()
{
    return rand_r(&rand_seed);
}

/*------------------------------------------------------------------------
 *  rand_r  -  Calculates a uniform random number [0, RAND_MAX]
 *------------------------------------------------------------------------
 */
int	rand_r(
	  unsigned int*		seedp
	)
{
	*seedp = 16807 * (*seedp) % (RAND_MAX + 1);
	return (int)(*seedp);
}

/*------------------------------------------------------------------------
 *  srand  -  Set the seed for rand
 *------------------------------------------------------------------------
 */
void	srand(
	  unsigned int		seed
	)
{
    rand_seed = seed;
}
