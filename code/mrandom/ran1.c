/* ran1.c 3.1 5/28/93 */
/* Portable Random Number Generator 1 */
/* From "Portable Random Number Generators */
/* William H. Press and Saul A Teukolsky */
/* Computers in Phyics, Vol. 6, No. 5, Sep/Oct 1992 */

/* Ported to C on Athena for use with mrandom: January 13, 1993 */
/* by Robert Plotkin */

#include "ran1.h"

#define IA	16807
#define IQ	127773
#define IR	2836
#define MASK	123459876
#define NDIV (1+(IM-1)/NTAB)
#define EPS 1.2e-7
#define RNMX 1.0-EPS

#define iy RNGstate[1]
#define iv (RNGstate+2)

static double am=1.0/IM;
#define max(a,b) (a>b ? a:b)
#define min(a,b) (a<b ? a:b)

/*********************************/
/* External interface to mrandom */
/*********************************/
/* "Minimal" random number generator of Park and Miller with */
 /* Bays-Durham shuffled and added safeguards.  Returns a uniform */
 /* random deviate between 0.0 and 1.0 (exclusive of the endpoint */
 /* values).  Call with idum a negative integer to initialize; */
 /* thereafter, do not alter idum between successive deviates in a */
 /* sequence.  RNMX should approximate the largest floating value that */
 /* is less than 1. */
double _ran1(rng)
RNGdata *rng;
{
double ran_num;
long j,k;
long *idum;

idum=RNGstate;

if ((*idum) <=0 || iy==0) { /* Initialize */
     (*idum)=max(-(*idum),1); /* Be sure to prevent idum = 0 */
     for (j=NTAB+7;j>=0;j--) { /* Load the shuffle table (after 8 warm-ups) */
       k=(*idum)/IQ;
       (*idum)=IA*((*idum)-k*IQ)-IR*k;
       if ((*idum) < 0) (*idum)+=IM;
       if (j < NTAB) iv[j]=(*idum);
     }
     iy=iv[0];
     }

k=(*idum)/IQ; /* Start here when not initializing */
(*idum)=IA*((*idum)-k*IQ)-IR*k; /* Compute idum=mod(IA*idum,IM) without
			     overflows by Schrage's method */
if ((*idum) < 0) (*idum)+=IM;
j=iy/NDIV; /* Will be in the range 0:NTAB-1 */
iy=iv[j]; /* Output previously stored value and refill the shuffle table */
iv[j]=(*idum);
ran_num=min(am*iy,RNMX); /* Because users don't expect endpoint values */

return(ran_num);
}

void _ran1_seed(rng, seed)
RNGdata *rng;
long *seed;
{
iy=0;
RNGstate[0]=seed[0];
}

int _ran1_check(rng)
RNGdata *rng;
{
return(RNGstate[0] >= 0);
}
