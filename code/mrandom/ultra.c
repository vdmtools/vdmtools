/* ultra.c 3.1 5/28/93 */
/*
FSU - ULTRA	The greatest random number generator that ever was
		or ever will be.  Way beyond Super-Duper.
		(Just kidding, but we think its a good one.)

Authors:	Arif Zaman (arif@stat.fsu.edu) and
		George Marsaglia (geo@stat.fsu.edu).

Date:		27 May 1992

Version:	1.05

Copyright:	To obtain permission to incorporate this program into
		any commercial product, please contact the authors at
		the e-mail address given above or at

		Department of Statistics and
		Supercomputer Computations Research Institute
		Florida State University
		Tallahassee, FL 32306.

See Also:	README		for a brief description
		ULTRA.DOC	for a detailed description

-----------------------------------------------------------------------
                Modified by Robert Plotkin for use with mrandom, 4/93
*/
/*
   File: ULTRA.C

   This is the ULTRA random number generator written entirely in C.

   This may serve as a model for an assembler version of this routine.
   The programmer should avoid simply duplicating and instead use the
   usual assembler features to increase the speed of this routine.

   Especially the subroutine SWB should be replaced by the one
   machine instruction (usually called subtract-with-borrow) that
   is available in almost every hardware.

   For people not familiar with 8086 assembler, it may help to
   consult this when reading the assembler code. This program should
   be a dropin replacement for the assembler versions, but is about
   half as fast.
*/

#include "ultra.h"

#define N  37           /* size of table        */
#define N2 24           /* The shorter lag      */

#define swb32   RNGstate
#define swbseed (RNGstate+N)
#define swb32p  ((long *)RNGstate[N+N])
#define swb32n  RNGstate[N+N+1]
#define flags   ((char)RNGstate[N+N+2]) /* The carry flag for
					   the SWB generator	*/
#define congx   ((long unsigned)RNGstate[N+N+3]) /* Seed */

/* SWB is the subtract-with-borrow operation which should be one line
   in assembler code. This should be done by using the hardware s-w-b
   operation in the SWBfill routine.

   What has been done here is to look at the msb of x, y and z=x-y-c.
   Using these three bits, one can determine if a borrow bit is needed
   or not according to the following table:

	msbz=0  msby=0  msby=1          msbz=1  msby=0  msby=1

	msbx=0  0       1               msbx=0  1       1
	msbx=1  0       0               msbx=1  0       1

   PS: note that the definition is very carefully written because the
   calls to SWB have y and z as the same memory location, so y must
   be tested before z is assigned a value.
*/
#define SWB(c,x,y,z) \
c = (y<0) ? (((z=x-y-c) < 0) || (x>=0)) : (((z=x-y-c) < 0) && (x>=0))

/*
  The first two lines of this macro are the heart of the system and
  should be written is assembler to be as fast as possible. It may even
  make sense to unravel the loop and simply write 37 consecutive SWB
operations!  */
void SWBfill(x,rng)
long *x;
RNGdata *rng;
{ short i;

  for (i=0;  i<N2; i++) SWB(flags,swbseed[i+N-N2],swbseed[i],swbseed[i]);
  for (i=N2; i<N;  i++) SWB(flags,swbseed[i  -N2],swbseed[i],swbseed[i]);
  for (i=0;  i<N;  i++) *(x++) = swbseed[i] ^ (congx = congx * 69069);
}

long swb32fill(rng)
RNGdata *rng;
{ long temp;
  swb32p = swb32;
  SWBfill(swb32,rng);
  swb32n = N-1;
  return *(swb32p++);
}

long  i32bit(rng)
RNGdata *rng;
{ return  (swb32n--) ? *(swb32p++) : swb32fill(rng); }

long  i31bit(rng)
RNGdata *rng;
{ return ((swb32n--) ? *(swb32p++) : swb32fill(rng)) & 0x7FFFFFFF; }

#define two2neg31  ( (2.0/0x10000) / 0x10000 )
#define two2neg32  ( (1.0/0x10000) / 0x10000 )

/*********************************/
/* External interface to mrandom */
/*********************************/
double _uni(rng)
RNGdata *rng;
{
  long temp;

  temp = i31bit(rng);
  if (temp & 0xFF000000) { return temp * two2neg31; }
  return (temp + i32bit(rng) * two2neg32) * two2neg31;
}

/* rinit initializes the constants and fills the seed array one bit at
   a time by taking the leading bit of the xor of a shift register
   and a congruential sequence. The same congruential generator continues
   to be used as a mixing generator for the Subtract-with-borrow generator
   to produce the `ultra' random numbers

   Since this is called just once, speed doesn't matter much and it might
   be fine to leave this subroutine coded just as it is.

   PS:	there are quick and easy ways to fill this, but since random number
	generators are really "randomness amplifiers", it is important to
	start off on the right foot. This is why we take such care here.
*/
void _rinit(rng, seed)
RNGdata *rng;
unsigned long *seed;
{
  short i,j;
  unsigned long tidbits;
  unsigned long congy, shrgx;

  congy=seed[0];
  shrgx=seed[1];

  congx=congy*2+1;
  for (i=0;i<N;i++) {
    for (j=32;j>0;j--) {
      congx = congx * 69069;
      shrgx = shrgx ^ (shrgx >> 15);
      shrgx = shrgx ^ (shrgx << 17);
      tidbits = (tidbits>>1) | (0x80000000 & (congx^shrgx));
    }
    swbseed[i] = tidbits;
  }
  swb32n = 0;
  flags = 0;
}

int _ultra_check(rng)
RNGdata *rng;
{
return(1);
}
