/* ran2.h 3.1 5/28/93 */
/* Portable Random Number Generator 1 */
/* From "Portable Random Number Generators */
/* William H. Press and Saul A Teukolsky */
/* Computers in Phyics, Vol. 6, No. 5, Sep/Oct 1992 */

#ifndef MRANDOM
#include "mrandom.h"
#endif

#define IM1 2147483563
#define NTAB 32

/* Information for mrandom */
#define RNGstatesize_8    3+NTAB
#define RNGseedsize_8     1
#define RNGrange_8        IM1
#define RNGname_8         "(Press and Teukolsky's ran1)\n"
#define RNGreturns_8      RET_DOUBLE
#define RNGstatetype_8    STATE_LONG
#define RNGdgen_8         _ran2
#define RNGlgen_8         0
#define RNGseed_8         _ran2_seed
#define RNGcheck_8        _ran2_check

/* "Minimal" random number generator of Park and Miller.  Returns a
	uniform random deviate between 0.0 and 1.0.  Set or reset
	idum to any integer value (except the unlikely value mask) to
	initialize the sequence; idum _must_ not be altered between
	calls for successive deviates in a sequence */
double ran2(/* long *idum */);

/* mrandom interface routines */
double _ran2(/*RNGdata **/);
void _ran2_seed(/*RNGdata *, long **/);
int _ran2_check(/*RNGdata **/);
