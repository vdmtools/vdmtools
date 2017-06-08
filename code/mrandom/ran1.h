/* ran1.h 3.1 5/28/93 */
/* Portable Random Number Generator 1 */
/* From "Portable Random Number Generators */
/* William H. Press and Saul A Teukolsky */
/* Computers in Phyics, Vol. 6, No. 5, Sep/Oct 1992 */

#ifndef MRANDOM
#include "mrandom.h"
#endif

#define IM	2147483647
#define NTAB 32

/* Information for mrandom */
#define RNGstatesize_7    2+NTAB
#define RNGseedsize_7     1
#define RNGrange_7        IM
#define RNGname_7         "(Press and Teukolsky's ran1)\n"
#define RNGreturns_7      RET_DOUBLE
#define RNGstatetype_7    STATE_LONG
#define RNGdgen_7        _ran1
#define RNGlgen_7        0
#define RNGseed_7         _ran1_seed
#define RNGcheck_7        _ran1_check

/* "Minimal" random number generator of Park and Miller.  Returns a
	uniform random deviate between 0.0 and 1.0.  Set or reset
	idum to any integer value (except the unlikely value mask) to
	initialize the sequence; idum _must_ not be altered between
	calls for successive deviates in a sequence */
double ran1(/* long *idum */);

/* mrandom interface routines */
double _ran1(/*RNGdata **/);
void _ran1_seed(/*RNGdata *, long **/);
int _ran1_check(/*RNGdata **/);
