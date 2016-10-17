/* newrng.c 3.1 5/28/93 */
/* RNG source file template */
/* Robert Plotkin
/* 5/3/93 */

#include "newrng.h"

/* Generating procedure */
/* Only one of the following two procedures should be defined, */
/* depending on the kind of value that your RNG returns        */

long newrng_lgen(rng)
RNGdata *rng;
{
/* Your generating procedure goes here */
}

double newrng_dgen(rng)
RNGdata *rng;
{
/* Your generating procedure goes here */
}

/* Seeding procedure */
void newrng_seed(rng,seed)
RNGdata *rng;
long *seed;
{
/* Your seeding procedure goes here */
}

/* Checking procedure */
int newrng_check(rng)
RNGdata *rng;
{
/* Your checking procedure goes here */
}
