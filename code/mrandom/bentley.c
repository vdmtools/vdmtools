/* bentley.c 3.1 5/28/93 */
/*  This is mrandom's RNG Algorithm #2				    */

/*  Original Author: Jon Bentley				    */
/*    (Source code obtained from DIMACS shared account, March 1992.)*/
/*  Header file/mrandom interface by Clark Thomborson, May 1992     */
/*  Vectorized interface by Clark Thomborson, June 1992		    */
/*  Updated mrandom interface by Robert Plotkin, Feb/March 1993     */
/*                                                                  */
/*  This file contains a set of c-language functions for generating */
/*  uniform integers.   This is a COMPLETELY PORTABLE generator.    */
/*  It will give IDENTICAL sequences of random numbers for any      */
/*  architecture with at least 30-bit integers, regardless of the   */
/*  integer representation, MAXINT value, or roundoff/truncation    */
/*  method, etc.                                                    */

/*  This Truly Remarkable RNG is described more fully in            */
/*  J. Bentley's column, ``The Software Exploratorium ''            */
/*  to appear in Unix Review in 1991.                               */ 
/*  It is based on one in Knuth, Vol 2, Section 3.2.2 (Algorithm A) */ 

#include "bentley.h"

/*----RNG Global Variables-------*/ 

#define arr rngstate
#define a (rngstate[RNGstatesize_2 - 2])
#define b (rngstate[RNGstatesize_2 - 1])

/*********************************/
/* External interface to mrandom */
/*********************************/
/* Generating procedure */
long _lprand(RNGdata *rng)
{
return(lprand(RNGstate));
}

/* Seeding procedure */
void _lprand_seed(rng, seed)
RNGdata *rng;
long *seed;
{
sprand(*seed, RNGstate);
}

/* Checking procedure */
int _lprand_check(rng)
RNGdata *rng;
{
return(ckrand(RNGstate));
}


/*----RNG Initializer------------*/
/* Call once before using lprand */ 

extern void sprand (seed, rngstate)
long seed; 
long rngstate[RNGstatesize_2];
{
  int i, ii;
  long last, next, v[55];
  arr[0] = last = seed; 
  next = 1;
  for (i=1; i < 55; i++) {
    ii = ( 21 * i ) % 55;
    arr[ii] = next;
    next = last - next; 
    if (next < 0)
      next += PRANDMAX;
    last = arr[ii];
  }

  a = 0; /* invariant: (b-a-24)%55 = 0 */
  b = 24; 
  /* cycle 165 times (more would probably be better -cdt) */
  for (i=0;i<165;i++) {
    lprand(rngstate);
  }
}

/*---------RNG---------------------*/
/* Returns a long integer in the range 0...PRANDMAX-1 */

extern long lprand(rngstate)
long rngstate[RNGstatesize_2];
{
  long t,ta,tb,retval;
  
  /* get local copies of a,b from state array */ 
  ta = a;
  tb = b;
  
  if (ta-- == 0) ta = 54;
  if (tb-- == 0) tb = 54;
  
  t = arr[ta] - arr[tb];
  if (t < 0) t+= PRANDMAX;
  
  arr[ta] = t;
  retval = t;
  
  /* update state array */
  a = ta;
  b = tb;
  
  return(retval);
}

/*---------CKRAND---------------------*/
/* Returns 0 if rngstate is corrupted */

extern int ckrand(rngstate)
long rngstate[RNGstatesize_2];
{
  if ( (b-a-24)%55 != 0 ) return(0); else return(1);
}

/*-----------------------------------------------*/
/* This is a little driver program so you can    */
/* test the code.              */
/* Typing: a.out 0 3 1         */
/* should produce              */
/*     921674862               */
/*     250065336               */
/*     377506581               */
/*  Typing: a.out 1000000 1 2  */ 
/*  should produce             */
/*     57265995                */

#ifdef DEBUG
main(argc, argv)
int argc;
int *argv;
{
  int i;
  long j;
  int n;
  int m; 
  long seed; 
  long rngstate[RNGstatesize_2];

  m = atoi(argv[1]);    /* Number to discard initially */ 
  n = atoi(argv[2]);    /* Number to print */ 
  seed = atoi(argv[3]); /* Seed */ 

  sprand(seed,rngstate);

  if ( !ckrand(rngstate) ) printf("Error detected by ckrand()\n");

  for (i=0; i < m; i++) j = lprand(rngstate);
  for (i=0; i < n; i++) printf("%ld\n", lprand(rngstate));

  if ( !ckrand(rngstate) ) printf("Error detected by ckrand()\n");
} 
#endif               

