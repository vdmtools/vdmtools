/* ran2.c 3.1 5/28/93 */
/* Portable Random Number Generator 2 */
/* From "Portable Random Number Generators */
/* William H. Press and Saul A Teukolsky */
/* Computers in Phyics, Vol. 6, No. 5, Sep/Oct 1992 */

/* Ported to C on Athena for use with mrandom: January 13, 1993 */
/* by Robert Plotkin */

#include "ran2.h"

#define IM2 2147483388
#define IMM1 IM1-1
#define IA1 40014
#define IA2 40692
#define IQ1 53668
#define IQ2 52774
#define IR1 12211
#define IR2 3791
#define NDIV (1+IMM1/NTAB)
#define EPS 1.2e-7
#define RNMX 1.0-EPS

#define iy    RNGstate[1]
#define idum2 RNGstate[2]
#define iv    (RNGstate+3)

double am=1.0/IM1;
#define max(a,b) (a>b ? a:b)
#define min(a,b) (a<b ? a:b)

/*********************************/
/* External interface to mrandom */
/*********************************/
/* Long period (> 2 X 10^18) random number generator of L'Ecuyer with */
 /* Bays-Durham shuffle and added safeguards.  Returns a uniform */
 /* random deviate between 0.0 and 1.0 (exclusive of the endpoint */
 /* values).  Call with idum a negative integer to initialize; */
 /* thereafter, do not alter idum between successive deviates in a */
 /* sequence.  RNMX should approximate the largest floating value */
 /* that is less than 1. */
double _ran2(rng)
RNGdata *rng;
{
double ran_num;
long j,k;
long *idum;

idum = RNGstate;

if ((*idum) <=0 || iy==0) { /* Initialize */
     (*idum)=max(-(*idum),1); /* Be sure to prevent idum = 0 */
     for (j=NTAB+7;j>=0;j--) { /* Load the shuffle table (after 8 warm-ups) */
       k=(*idum)/IQ1;
       (*idum)=IA1*((*idum)-k*IQ1)-IR1*k;
       if ((*idum) < 0) (*idum)+=IM1;
       if (j < NTAB) iv[j]=(*idum);
     }
     iy=iv[0];
     }

k=(*idum)/IQ1; /* Start here when not initializing */
(*idum)=IA1*((*idum)-k*IQ1)-IR1*k; /* Compute idum=mod(IA1*idum,IM1) without
			     overflows by Schrage's method */
if ((*idum) < 0) (*idum)+=IM1;

k=idum2/IQ2; /* Compute idum2=mod(IA2*idum2,IM2) likewise */
idum2=IA2*(idum2-k*IQ2)-IR2*k; /* Compute idum=mod(IA1*idum,IM1) without
			     overflows by Schrage's method */
if (idum2 < 0) idum2+=IM2;

j=iy/NDIV; /* Will be in the range 0:NTAB-1 */
iy=iv[j]-idum2; /* Output previously stored value and refill the */
		/* shuffle table */
iv[j]=(*idum);
if (iy < 1) iy+=IMM1;
ran_num=min(am*iy,RNMX); /* Because users don't expect endpoint values */
return(ran_num);
}

void _ran2_seed(rng, seed)
RNGdata *rng;
long *seed;
{
iy=0;
idum2=123456789;
RNGstate[0]=seed[0];
}

int _ran2_check(rng)
RNGdata *rng;
{
return(RNGstate[0] >= 0);
}
