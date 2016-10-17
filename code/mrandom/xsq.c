/* xsq.c 3.1 5/28/93 */
/* xsq.c: routines for interpreting X-squared test statistics
 * 
 * Author: Clark Thomborson
 * Original version (named chisq.c): May 1992
 * Revised July 1992 to be more accurate on the extreme upper tail:
 *   now uses Wallace's approximation, and Thomborson's multinomial
 *   correction.
 */

#include <math.h>

#define min(x,y)	( ((x)<(y)) ? (x) : (y) )
#define max(x,y)	( ((x)>(y)) ? (x) : (y) )
#define LOG2PI		1.837877		/* log(2*pi) */

double t; /* temp for efficient evaluation of squares and cubes by macro */
#define square(x)	( t=(x), t*t )
#define cube(x)		( t=(x), t*t*t )

/* Evaluate an X-squared variate xsq (for n items placed in k bins,
 * with all bins equi-probable), returning the number of standard
 * deviations the equivalent normal deviate differs from its expectation.
 * That is, if this function returns -2.326, the xsq value is at the
 * p=0.01 point of its distribution.
 *
 * The value of *approx is set to 1 if the calculated stddev has more than
 * 10% relative error.
 *
 * See Thomborson's SODA '93 submission for an explanation of this code.
 */
double xsq_to_stddev(xsq,k,n,approx)
double xsq,k,n;
int *approx;
{
  double stddev,v,wsq,w,a,keff,twologp,u;

  xsq = xsq + k/n;  /* apply a "continuity correction" */
  v = k-1.0; /* degrees of freedom in Chi-squared approximation */
  /* Wallace's approximation */
  wsq = xsq - v - v*log(xsq/v);
  if (wsq<0.0) wsq=0.0; /* avoid sqrt(-0.0), as a result of round-off errors */
  w = sqrt(wsq);
  a = sqrt(2.0/v)/3.0;
  if (xsq <= v) { /* lower tail: use Wallace's approx */
    stddev = -w+a;
  } else { /* upper tail: min(multinomial approx, Wallace's approx) */
    keff = k/(1.0+xsq/n) * (1.0 - exp(-(xsq+n)/k));
    twologp = (2.0*n - 1.0 - 2.0*keff) * log(k/keff) - 2.0*keff + LOG2PI;
    /* if twologp is small, make it huge (so it won't affect the output) */
    if (twologp < 2.83) twologp = 1000.0;
    /* convert 2*log(p) to (approx.) quantile of normal distribution */
    u = sqrt(twologp - log(twologp - LOG2PI) - LOG2PI);
    stddev = min(u,w+a);
  }
  if (fabs(stddev) > n/sqrt(2.0*v)) {
    *approx = 1;
  } else {
    *approx = 0;
  }
  return(stddev);
}

/* Print a low-precision float, using "%f" format where possible.
 * Uses "%f" format if this would print at most 3 significant digits;
 * otherwise uses "%.2g".  Always prints at least 2 significant digits,
 * never prints more than 3.
 */
print2digits(f)
double f;
{
  double a;
  a = fabs(f);
  if (a > 1000.0)
    printf("%#.2g", f);
  else if (a > 10.0)
    printf("%.0f", f);
  else if (a > 1.0)
    printf("%.1f", f);
  else if (a > 0.1)
    printf("%.2f", f);
  else if (a > 0.01)
    printf("%.3f", f);
  else
    printf("%#.2g", f);
}

/* Print a message describing the significance of a test statistic = xsq
 * computed by placing n items into k bins, with each bin equally likely.
 *
 * The estimate of the number of standard deviations can be inaccurate
 * for small df, moderate or small n, and large +/- stddev.  The symbol
 * "~" instead of "=" appears in the print line in these cases.
 *
 * If n is too small, a message is printed indicating that little can be
 * concluded from this xsq statistic.
 */
extern void interpret_xsq(xsq,k,n)
double xsq, k, n;
{
  double stddev;/* temp for calculating # of std devations */
  int i; /* loop counter */
  int robust; /* true if we can do a 2-tailed test with p < 1e-6 */
  int accurate; /* true if the stddev estimate is accurate */ 
  double nrobust,ngiveup; /* min n to give (robust,adequate) test results */
  double smallxsq; /* temp for nrobust and ngiveup calculations */

  /* Give a diagnostic message if some parameter value is ridiculous */
  if (k < 2.0 || xsq < 0.0 || n < 1.0) {
    printf("Argument error in interpret_xsq(%f,%f,%f)!\n",xsq,k,n);
    exit(1);
  }

  /* I have two tests for small n.  The less restrictive test, below,
   * checks whether the computed, continuity-corrected xsq value
   * could ever fall below the 5% point on the Chi-squared distribution.
   * In all cases, if there is a xsq outcome below the 5% point, there
   * are (many) above the 95% point, so a two-sided test is reasonably
   * accurate.
   *
   * The smallest xsq value is (n mod k)(k - (n mod k))/n.  For the case
   * of n > k, a monotone envelope is
   *		smallestxsq1 = k^2/(4*n)
   * For the case of n <= k, the smallest xsq value is just
   * 		smallestxsq2 = k - n
   * We thus use max(smallestxsq1, smallestxsq2) =
   * 		smallxsq = max(k-n, k*k/(4*max(n,k)) )
   * as a monotone envelope for this function.
   *
   * My "ngiveup" calculation evaluates smallxsq for n = (3*sqrt(k))*(1.1)^i,
   * for i=0,1,... until the resulting xsq_to_stddev value falls below -1.645.
   * The factor of 1.1 implies that we'll overshoot the smallest possible
   * value of ngiveup by at most 10%.  (Alternatively, we could symbolically
   * invert the calculations in the xsq_to_stddev() function, perhaps
   * approximating in terms of a power series.  This would take a lot
   * of my time: I'd rather let your CPU do a bit of extra work when
   * evaluating ngiveup.) 
   *
   * For large k, ngiveup is 3*sqrt(k).  For very small k, ngiveup 
   * is moderately large: 725 at k=2, 51 at k=3, 24 at k=4, 17 at k=5,
   * 13 at k=6, ..., 11 at k = 10, then rising with 3*sqrt(k) for k>10.
   */

  ngiveup = ceil(3.0*sqrt(k));
  for (i=0; i<500; i++) {
    smallxsq = max(k-ngiveup, k*k/(4*max(ngiveup,k)) );
    if (xsq_to_stddev(smallxsq,k,ngiveup,&accurate) < -1.645) break;
    ngiveup = ceil(ngiveup*1.1);
  }

  /* My nrobust calculation is similar to the ngiveup calculation,
   * except that the cutoff probability is 0.5*10^{-6}, i.e. the
   * stddev value must fall below -4.892.  This allows an xsq value
   * to be rejected at the 1-in-a-million level.
   *
   * is 60; for k = 10, nrobust is 182; for k == 5, nrobust is about
   * 5600; for k == 3, nrobust is approximately 10^7; for k == 2,
   * nrobust is about 3*10^{13}.  For k >= 50, nrobust < k.
   *
   * Perhaps I should use only a 1-tailed test on xsq for k<4,
   * since a 2-tailed test with small failure probability requires
   * an ``unreasonably-large'' amount of data.
   *
   * The statistician's "rule of thumb" for Chi-squared testing
   * is n >= 5*k, which is much larger than my nrobust for large k,
   * and much smaller than my ngiveup for small k.  I have found
   * no reason to test for n >= 5*k.  Indeed, Kendall and Stuart
   * (Vol 2, 2nd ed, p. 440) say there is "No general theoretical
   * basis for this rule."
   */

  nrobust = ceil(7.0*sqrt(k));
  for (i=0; i<500; i++) {
    smallxsq = max(k-nrobust, k*k/(4*max(nrobust,k)) );
    if (xsq_to_stddev(smallxsq,k,nrobust,&accurate) < -4.892) break;
    nrobust = ceil(nrobust*1.1);
  }

  /* quit if sample is too small */
  if (n<ngiveup) {
    printf("  This test requires ");
    print2digits(ngiveup);
    printf(" or more random generates.\n");
    return;
  }

  if (n<nrobust) {
    printf("  Note: at least ");
    print2digits(nrobust);
    printf(" random generates are needed\n");
    printf("    for a two-tailed test with confidence p > 1-1.0e-6\n");
  };

  printf("  X^2 = ");
  print2digits(xsq);
  printf(", k = ");
  print2digits(k);
  printf(", equivalent normal standard deviation ");
  printf(accurate? "= " : "~ ");
  stddev = xsq_to_stddev(xsq,k,n,&accurate);
  print2digits(stddev);
  printf("\n", stddev);

  if (fabs(stddev) > 6.1) {
    printf("\n");
    printf("  THIS RNG IS FAULTY!  Confidence level p > 1-1.0e-9\n");
    printf("\n");
  } else if (fabs(stddev) > 4.9) {
    printf("\n");
    printf("  This rng is faulty!  Confidence level p > 1-1.0e-6\n");
    printf("\n");
  } else if (fabs(stddev) > 2.6) {
    printf("\n");
    printf("  This rng may be faulty.  Confidence level p > 1-1.0e-3\n");
    printf("\n");
  } else {
    printf("  This message should appear ");
    if (!accurate) {
      printf("about ");
    }
    if (fabs(stddev) > 1.645) {
      printf("10%% of the time.\n");
    } else {
      printf("90%% of the time.\n");
    }
  }
} /* end interpret_xsq */

/* end of xsq.c */
