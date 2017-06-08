/* bentley.h 3.1 5/28/93 */
/*  Original Author: Jon Bentley				    */
/*    (Source code obtained from DIMACS shared account, March 1992.)*/
/*								    */
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

#ifndef MRANDOM
#include "mrandom.h"
#endif

#define PRANDMAX          1000000000

/* Information for mrandom */
#define RNGstatesize_2    57
#define RNGseedsize_2     1
#define RNGrange_2        (double)PRANDMAX
#define RNGname_2         "(Knuth/Bentley prand: lagged Fibbonacci)\n"
#define RNGreturns_2      RET_LONG
#define RNGstatetype_2    STATE_LONG
#define RNGdgen_2        0
#define RNGlgen_2        _lprand
#define RNGseed_2         _lprand_seed
#define RNGcheck_2        _lprand_check

/*----RNG Initializer------------*/
/* Call once before using lprand */ 

void sprand (/* long seed, rngstate[RNGstatesize_2] */);

/*---------RNG---------------------*/
/* Returns a long integer in the range 0...PRANDMAX-1 */ 

long lprand(/* long rngstate[RNGstatesize_2] */);

/*---------CKRAND---------------------*/
/* Returns 0 if rngstate is corrupted */

int ckrand(/* long rngstate */);

/* mrandom interface routines */
long _lprand(/*RNGdata **/);
void _lprand_seed(/*RNGdata *, long **/);
int _lprand_check(/*RNGdata **/);
