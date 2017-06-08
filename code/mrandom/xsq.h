/* xsq.h 3.1 5/28/93 */
/* xsq.h: header for xsq.c, giving access to a routine for 
 * printing interpretations of X-squared test statistics
 * 
 * Author: Clark Thomborson
 * Original version (named chisq.c): May 1992
 * Revised July 1992 to be more accurate on the extreme upper tail:
 *   now uses Wallace's approximation, and Thomborson's multinomial
 *   correction.
 */


void interpret_xsq(/* double xsq,k,n; */);
/* Print a message describing the significance of a test statistic = xsq
 * computed by placing n items into k bins, with each bin equally likely.
 *
 * The estimate of the number of standard deviations can be inaccurate
 * for small df, moderate or small n, and large +/- stddev.  The word
 * "approx." appears in the print line in these cases.
 *
 * If n is too small, a message is printed indicating that little can be
 * concluded from this xsq statistic.
 */
