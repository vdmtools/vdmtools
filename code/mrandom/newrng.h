/* newrng.h 3.1 5/28/93 */
/* RNG header file template */
/* Robert Plotkin
/* 5/3/93 */

#include "mrandom.h"

/* Information for mrandom */
#define RNGstatesize_n    
#define RNGseedsize_2
#define RNGrange_2
#define RNGname_2
#define RNGreturns_2
#define RNGstatetype_2
#define RNGdgen_2
#define RNGlgen_2
#define RNGseed_2
#define RNGcheck_2

/* mrandom interface routines */
long newrng_gen(/*RNGdata **/);
void newrng_seed(/*RNGdata *, long **/);
int newrng_check(/*RNGdata **/);
