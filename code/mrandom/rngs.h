/* rngs.h 3.1 5/28/93 */
/* These are the "built in" rngs */
/* Robert Plotkin 1/27/93 */

#ifndef MRANDOM
#include "mrandom.h"
#endif

/* RNGstate[0] for random(), table length 32 */
#define MRNGSTATE0	3

/* RNG state sizes */
#define RNGstatesize_0 1
#define	RNGstatesize_1 32
#define	RNGstatesize_4 2
#define	RNGstatesize_5 1

/* RNG seed sizes */ 
#define RNGseedsize_0 2
#define RNGseedsize_1 1
#define RNGseedsize_4 2
#define RNGseedsize_5 1

/* RNG output ranges */
#define RNGrange_0 (MAXLONG+1.0)
#define RNGrange_1 (MAXLONG+1.0)
#define RNGrange_4 (MAXLONG+1.0)
/* According to my Sun man pages, System V unix has a different rand() */
#ifdef __SYSTEM_FIVE
#define RNGrange_5 (MAXSHORT+1.0)
#else
#define RNGrange_5 (MAXLONG+1.0)
#endif

/* RNG return types */
#define RNGreturns_0 4
#define RNGreturns_1 4
#define RNGreturns_4 4
#define RNGreturns_5 4

/* RNG state/seed types */
#define RNGstatetype_0 STATE_LONG
#define RNGstatetype_1 STATE_LONG
#define RNGstatetype_4 STATE_INT
#define RNGstatetype_5 STATE_LONG

/* Procedures for returning a random number */
#define RNGdgen_0 0
#define RNGlgen_0 _triv
#define RNGdgen_1 0
#define RNGlgen_1 _random
#define RNGdgen_4 0
#define RNGlgen_4 _nrand48
#define RNGdgen_5 0
#define RNGlgen_5 _rand

/* RNG seed procedures */
#define RNGseed_0 _triv_seed
#define RNGseed_1 _random_seed
#define RNGseed_4 _nrand48_seed
#define RNGseed_5 _rand_seed

/* RNG check procedures */
#define RNGcheck_0 _triv_check
#define RNGcheck_1 _random_check
#define RNGcheck_4 _nrand48_check
#define RNGcheck_5 _rand_check

/* RNG names */
#define RNGname_0	"(trivial RNG: long state=seed1; state += seed2)\n"
#define RNGname_1	"(4.3bsd random.c: Non-linear additive feedback)\n"
#define RNGname_4	"(4.3bsd nrand48.c: 48-bit multiplicative)\n"
#define RNGname_5	"(4.3bsd rand.c: 32-bit multiplicative)\n"

/********************************/
/* triv                         */
/********************************/
/* A trivial RNG, which can be set up to return a constant or an
 * arithmetic progression.  Can come in handy for debugging. 
 */
long _triv(rng)
RNGdata *rng;
{
  RNGstate[0] += RNGseed[1];
  RNGstate[0] &= MAXLONG; /* clear sign bit */
  return(RNGstate[0]);
}

void _triv_seed(rng, seed)
RNGdata *rng;
long *seed;
{
RNGstate[0] = seed[0];
}

int _triv_check(rng)
RNGdata *rng;
{
return(RNGstate[0] >= 0);
}

/* bsd4.3 random */
long _random(rng)
RNGdata *rng;
{
return(random());
}

void _random_seed(rng, seed)
RNGdata *rng;
long *seed;
{
/* Call the 4.3bsd random() initialization routine */
initstate(seed[0], RNGstate, RNGstatesize*4);
}

int _random_check(rng)
RNGdata *rng;
{
return(RNGstate[0] == MRNGSTATE0);
}

/* 4.3bsd nrand48 */
long _nrand48(rng)
RNGdata *rng;
{
return(nrand48(RNGstate));
}

void _nrand48_seed(rng, seed)
RNGdata *rng;
long *seed;
{
unsigned short *sarray;

/* Copy seeds into the state array */
  sarray = (unsigned short *) RNGstate;
  sarray[0] = seed[1]&0xFFFF; /* LSBs of seed */
  sarray[1] = seed[0]&0xFFFF;
  sarray[2] = (seed[0]>>16)&0xFFFF; /* MSBs of seed */
  sarray[3] = 0; /* stuff a 0 into the other half of RNGstate[1] */
}

int _nrand48_check(rng)
RNGdata *rng;
{
unsigned short *sarray;

sarray = (unsigned short *) RNGstate;
return(sarray[3] == 0);
}

/* 4.3bsd rand */
long _rand(rng)
RNGdata *rng;
{
/* Very inefficient for many calls!!! */
srand(RNGstate[0]); /* In case RNGs were switched */
return(RNGstate[0]=rand());
}

void _rand_seed(rng, seed)
RNGdata *rng;
long *seed;
{
/* Use ran() */
RNGstate[0] = seed[0]; /* The srand() call will be made later */
}

int _rand_check(rng)
RNGdata *rng;
{
return(RNGstate[0] >= 0);
}

