/* ran0.h 3.1 5/28/93 */
/* Portable Random Number Generator 0 */
/* From "Portable Random Number Generators */
/* William H. Press and Saul A Teukolsky */
/* Computers in Phyics, Vol. 6, No. 5, Sep/Oct 1992 */

#ifndef MRANDOM
#include "mrandom.h"
#endif

#define IM	2147483647

/* Information for mrandom */
#define RNGstatesize_6    1
#define RNGseedsize_6     1
#define RNGrange_6        IM
#define RNGname_6         "(Press and Teukolsky's ran0)\n"
#define RNGreturns_6      RET_DOUBLE
#define RNGstatetype_6    STATE_LONG
#define RNGdgen_6        _ran0
#define RNGlgen_6        0
#define RNGseed_6         _ran0_seed
#define RNGcheck_6        _ran0_check

/* "Minimal" random number generator of Park and Miller.  Returns a
	uniform random deviate between 0.0 and 1.0.  Set or reset
	idum to any integer value (except the unlikely value mask) to
	initialize the sequence; idum _must_ not be altered between
	calls for successive deviates in a sequence */
double ran0(/* long *idum */);

/* mrandom interface routines */
double _ran0(/*RNGdata **/);
void _ran0_seed(/*RNGdata *, long **/);
int _ran0_check(/*RNGdata **/);
