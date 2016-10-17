/* ultra.h 3.1 5/28/93 */
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

#ifndef MRANDOM
#include "mrandom.h"
#endif

#define N  37   /* size of table */
#define N2 24   /* The shorter lag */

/* Information for mrandom */
#define RNGstatesize_9  N*2+4
#define RNGseedsize_9   2
#define RNGrange_9      4294967296.0
#define RNGname_9       "Marsaglia's Ultra\n"
#define RNGreturns_9    RET_DOUBLE
#define RNGstatetype_9  STATE_LONG
#define RNGdgen_9      _uni
#define RNGlgen_9      0
#define RNGseed_9       _rinit
#define RNGcheck_9      _ultra_check

/* mrandom interface routines */
double _uni(/*RNGdata **/);
void _rinit(/*RNGdata *, long **/);
int _ultra_check(/*RNGdata **/);
