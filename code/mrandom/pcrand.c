/* pcrand.c 3.1 5/28/93 */
/* **********************


Purpose:        This file has routines to implement a
                Combined Prime Multiplicative Congruential
                Psuedo-Random Number Generator.

		The original algorithms and selection of
		prime numbers comes from two articles in the
		Communications of the ACM: June 1988 (v31 #6)
		the article by Pierre L'Ecuyer called,
		"Efficient and Portable Combined Random Number Generators"
		and the October (v31 #10), Stephen Park and Keith Miller
		"Random Number Generators: Good Ones are Hard to Find."

		This implementation of these algorithms has been released
		to the Public Domain. 91.03.10; 91.09.25.

Author:         Jesse Chisholm
Modified by:	Clark Thomborson, Robert Plotkin

Auditor:

Creation Date:  91.03.04

Modifications by Jesse Chisholm:

	0.0.0	91.03.04	Initial creation from the articles
				mentioned.

	0.0.1	91.03.04	cleaned up some, and added test main(s).

	0.0.2	91.03.06	added support routines for retaining
				continuation of sequence across program
				invokations.

	0.0.3	91.03.10	ported to UNIX

	1.0.0	91.03.10	Released to the Public Domain.

	1.0.1	91.09.25	Re-Released to the Public Domain.

Modifications by Clark Thomborson:

	1.0.2	92.01.01	Added SUN compatibility; allowed RNGHOME
				to differ from $HOME; wrote Unix Makefile

	2.0.0	92.01.02	Rewrote _rand to comply with L'Ecuyer's
				recommendations.  New function is called
				pcrand(), of type double.  Discarded
				nrand(), since it was inaccurate.
				Added RNGFILENAME.  Reformatted statefile
				to include pedigree of current state.

	2.0.1	92.01.06	Rewrote urand() to generate full-range
				(31-bit) integers in an unbiased fashion.	

	2.0.2   92.01.15	Removed a tiny residual bias in urand().

	3.0.0   92.05.06	Interfaced to mrandom().  Discarded
				urand() and non-Sun code.

*************************************************** */

/* #define TEST */

/* include directives */

#include <stdio.h>
#include "pcrand.h"

/*
**      this is the current state of the generator
*/
#define curr_seed1 rngstate[0]
#define curr_seed2 rngstate[1]

/* These constants define the generator */

#define ROOT1 40014L
#define ROOT2 40692L

/*
**      If you should choose to change primes or roots,
**      please be sure that these conditions hold:
**              (QUO > REM)
**              (QUO > 0)
**              (REM > 0)
*/
/* QUO = PRIME / ROOT */
#define QUO1 53668L
#define QUO2 52774L

/* REM = PRIME % ROOT */
#define REM1 12211L
#define REM2 3791L

/*
**      The 10000th seed from 1L for arithmetic checking
*/
#define PRCHK1  1919456777L
#define PRCHK2  2006618587L

/* ***************************************************

Name:           lpcrand

Purpose:        This is the basic routine for generating
                random numbers.  The range is 0..(PRIME1-2).
                The period is humongous.  Roughly 2^62.

Sample Call:    l = lpcrand(rngstate);

Inputs:         rngstate (array of 2 longs, holding seed1 and seed2)

Outputs:        none

Returns:        long l;  MSBs are random

Algorithm:      seed1 = seed1 * ROOT1 % PRIME1
                seed2 = seed2 * ROOT2 % PRIME2
                U = (seed1 - seed2) mod (PRIME1 - 1)
		return (U==0 ? (PRIME1 - 1): U)

        This implementation used the following numbers:

        PRIME           ROOT            RANGE
        2147483563      40014           1..2147483562
        2147483399      40692           1..2147483398

*************************************************** */
/*
**      NOTES ABOUT RND taken from earlier implementation
**
**  This is the basic routine.
**
**      new_seed = old_seed * ROOT % PRIME;
**
**  The table below shows some convenient values
**
**      PRIME       primitive root      alternate root      range
**
**      17              5                   6               1..16
**      257             19                  17              1..256
**      32749           182                 128             1..32748
**      65537           32749               32719           1..65536
**      2147483647      16807               39373           1..2147483646
**      2147483647      48271               69621           1..2147483646
**      2147483647      397204094           630360016       1..2147483646
**
**  This is the best single primitive root for 2^31-1 according to
**  Pierre L'Ecuyer; "Random Numbers for Simulation"; CACM 10/90.
**
**      2147483647      41358                               1..2147483646
**
**  He also lists the prime 4611685301167870637
**  and the primitive root  1968402271571654650
**  as being pretty good.  However, it doesn't lend itself to
**  this algorithm as the quotient is less than the remainder in p/r.
**
**  As a way of checking the arithmetic in any given implementation,
**  the table below shows what the 10,000th seed should be when the
**  generator is started at 1L for various primes and roots.
**
**      PRIME           ROOT    1st seed        10,000th seed
**      ----------      -----   -               ----------
**      2147483647L     16807L  1L              1043618065L
**      2147483647L     48271L  1L               399268537L
**      2147483647L     69621L  1L               190055451L
**      2147483563L     40014L  1L              1919456777L
**      2147483399L     40692L  1L              2006618587L
**
**  There are many other primitive roots available for these
**  or other primes.  These were chosen as esthetically pleasing.
**  Also chosen so that `p / x > p % x' for prime p, and root x.
**
**  A number (x) is a primitive root of a prime (p)
**  iff in the equation:
**
**      t = x^i % p
**
**      when (i == p-1) and (t == 1)
**
**      for all (1 <= i < p-1) then (t >= 2)
**
**  The number of primitive roots for a given prime (p) is the number
**  of numbers less that (p-1) that are relatively prime to (p-1).
**
**  Note: that is how many, not which ones.
** 
**  if x is a primitive root of p, and y is relatively prime to p-1
**  then (x^y % p) is also a primitive root of p.
**  also (x^(p-1-y) % p) is a primitive root of p.
**
*/

long lpcrand(rngstate)
long rngstate[RNGstatesize_3];
{
  long k;
  long U;

  /* the following code is taken from Figure 3 of L'Ecuyer's article */
  k = curr_seed1 / QUO1;
  curr_seed1 = ROOT1 * (curr_seed1 - k * QUO1) - k * REM1;
  if (curr_seed1 < 0) {
    curr_seed1 += PRIME1;
  }

  k = curr_seed2 / QUO2;
  curr_seed2 = ROOT2 * (curr_seed2 - k * QUO2) - k * REM2;
  if (curr_seed2 < 0) {
    curr_seed2 += PRIME2;
  }

  U = curr_seed1 - curr_seed2;
  if (U < 1) {
    U += PRIME1 - 1;
  }

  return(U-1);
}

/* ***************************************************

Name:           checkran

Purpose:        This routine returns 1 if the RNG state looks ok.

Sample Call:    if ( !checkran(rngstate) ) fprintf(stderr,"RNG unitialized!\n");

Inputs:         none

Outputs:        none

Returns:        none

Algorithm:      Return 1 if 0 < seed1 < PRIME1 and 0 < seed2 < PRIME2,
		else return 0

*************************************************** */
extern int checkran(rngstate)
long rngstate[RNGstatesize_3];
{
  if (curr_seed1 <= 0 || curr_seed1 >= PRIME1 ||
      curr_seed2 <= 0 || curr_seed2 >= PRIME2) {
    return(0);
  } else {
    return(1);
  }
}

/* ***************************************************

Name:           setran

Purpose:        This routine sets the random number sequence, checking to see
		if the seeds are legal.  Prints message to stderr if illegal.

Sample Call:    setran(1L,1L,rngstate);

Inputs:         long seed1;             starting seed for 1st generator
                long seed2;             starting seed for 2nd generator
		long rngstate[2];	where seed1 and seed2 are to be stored

Outputs:        none

Returns:        none

Algorithm:      copy seed1, seed2 into rngstate[]; call checkran() to
		confirm that seeds are legal.  Print error message to
		stderr if illegal.

*************************************************** */
void setran(seed1, seed2, rngstate)
long seed1, seed2, rngstate[RNGstatesize_3];
{
  curr_seed1 = seed1;
  curr_seed2 = seed2;
  if ( !checkran(rngstate) ) {
    fprintf(stderr,"Illegal seed values: %ld, %ld", seed1, seed2); 
    fflush(stderr);
  }
}

/*********************************/
/* External interface to mrandom */
/*********************************/
long _lpcrand(RNGdata *rng)
{
return(lpcrand(rng->rngstate));
}

void _lpcrand_seed(rng, seed)
RNGdata *rng;
long *seed;
{
setran(seed[0], seed[1], RNGstate);
}

int _lpcrand_check(rng)
RNGdata *rng;
{
return(checkran(RNGstate));
}

#ifdef TEST

main()
{
int num;
long dd;
int limit;
long rngstate[RNGstatesize_3];

        printf("Testing validity of arithmetic in this implementation\n");
        printf("of the EXTENDED STANDARD RANDOM NUMBER GENERATOR\n\n");

	printf("Testing error check code.\n");
	setran(0L,0L,rngstate);

	/* now try a legal seed */
        setran(1L,1L,rngstate);
        limit = 10000;

        for(num=0; num<limit; num++) {
                dd = lpcrand(rngstate);
                if ((num % 100) == 0) {
                        printf("\nIteration %d", num);
                }
        }
        printf("\rSeed should be %ld,%ld\n        and is %ld,%ld.\n",
                PRCHK1, PRCHK2, curr_seed1, curr_seed2);
        if ((curr_seed1 == PRCHK1) && (curr_seed2 == PRCHK2)) {
                printf ("Arithmetic is performed correctly.\n");
        } else {
                printf ("Arithmetic is performed INCORRECTLY!\n");
        }

        exit(0);
}

#endif /* TEST */

/* end of pcrand.c */
