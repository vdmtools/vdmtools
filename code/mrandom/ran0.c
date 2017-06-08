/* ran0.c 3.1 5/28/93 */
/* Portable Random Number Generator 0 */
/* From "Portable Random Number Generators */
/* William H. Press and Saul A Teukolsky */
/* Computers in Phyics, Vol. 6, No. 5, Sep/Oct 1992 */

/* Ported to C on Athena: January 12, 1993 */
/* by Robert Plotkin */

#include "ran0.h"

#define IA	16807
#define IQ	127773
#define IR	2836
#define MASK	123459876

static double am=1.0/IM;

/*********************************/
/* External interface to mrandom */
/*********************************/
double _ran0(rng)
RNGdata *rng;
{
return(ran0(RNGstate));
}

void _ran0_seed(rng, seed)
RNGdata *rng;
long *seed;
{
RNGstate[0]=seed[0];
}

int _ran0_check(rng)
RNGdata *rng;
{
return(RNGstate[0] >= 0);
}

/* "Minimal" random number generator of Park and Miller.  Returns a
	uniform random deviate between 0.0 and 1.0.  Set or reset
	idum to any integer value (except the unlikely value mask) to
	initialize the sequence; idum _must_ not be altered between
	calls for successive deviates in a sequence */
double ran0(idum)
long *idum;
{
long k;
double ran_num;

(*idum) ^= MASK;	/* XORing with MASK allows use of zero and other simple
		   bit patterns for idum. */
k=(*idum)/IQ;
(*idum)=IA*((*idum)-k*IQ)-IR*k;	/* Compute idum=mod(IA*idum,IM)
				   without overflows by Schrage's
				   method */
if (*idum < 0)
	(*idum)+=IM;
ran_num=am*(*idum);	/* Convert idum to a floating result */
(*idum) ^= MASK;	/* Unmask before return */
return(ran_num);
}
