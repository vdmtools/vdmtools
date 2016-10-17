/* pcrand.h 3.1 5/28/93 */
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
*/

#ifndef MRANDOM
#include "mrandom.h"
#endif

/* constants defining this multiplicative generator */
#define PRIME1 2147483563L
#define PRIME2 2147483399L

/* Information for mrandom */
#define RNGstatesize_3  2
#define RNGseedsize_3   2
#define RNGrange_3      (PRIME1-2.0)
#define RNGreturns_3    RET_LONG
#define RNGstatetype_3  STATE_LONG
#define RNGdgen_3      0
#define RNGlgen_3      _lpcrand
#define RNGseed_3       _lpcrand_seed
#define RNGcheck_3      _lpcrand_check
#define RNGname_3       "(Portable Combined Multiplicative)\n"


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

*/

long lpcrand(/* long rngstate[RNGstatesize_3] */);

/* ***************************************************

Name:           setran

Purpose:        This routine sets the random number sequence, checking to see
		if the seeds are legal.  Prints message to stderr if illegal.

Sample Call:    setran(1L,1L,rngstate);

Inputs:         long seed1;             starting seed for 1st generator
                long seed2;             starting seed for 2nd generator
		long rngstate[2];	where seed1 and seed2 are stored

Outputs:        none

Returns:        none

Algorithm:      copy seed1, seed2 into rngstate[]; call checkran() to
		confirm that seeds are legal.  Print error message to
		stderr if illegal.

*************************************************** */
void setran(/* long seed1, seed2, rngstate[RNGstatesize_3] */);

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
int checkran(/* rngstate[RNGstatesize_3] */);

/* mrandom interface routines */
int _lpcrand_check(RNGdata *);
long _lpcrand(/*RNGdata **/);
void _lpcrand_seed(/*RNGdata *, long **/);
