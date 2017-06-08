
/* 
 *  M_APM  -  mapm_exp.c
 *
 *  Copyright (C) 1999, 2000   Michael C. Ring
 *
 *  Permission to use, copy, and distribute this software and its
 *  documentation for any purpose with or without fee is hereby granted, 
 *  provided that the above copyright notice appear in all copies and 
 *  that both that copyright notice and this permission notice appear 
 *  in supporting documentation.
 *
 *  Permission to modify the software is granted, but not the right to
 *  distribute the modified code.  Modifications are to be distributed 
 *  as patches to released version.
 *  
 *  This software is provided "as is" without express or implied warranty.
 */

/*
 *      $Id: mapm_exp.c,v 1.1.1.1 2001/10/04 13:02:56 paulm Exp $
 *
 *      This file contains the EXP function.
 *
 *      $Log: mapm_exp.c,v $
 *      Revision 1.1.1.1  2001/10/04 13:02:56  paulm
 *      Import of dir
 *
 *      Revision 1.19  2000/08/22 21:34:41  mike
 *      increase local precision
 *
 *      Revision 1.18  2000/05/18 22:05:22  mike
 *      move _pow to a separate file
 *
 *      Revision 1.17  2000/05/04 23:21:01  mike
 *      use global var 256R
 *
 *      Revision 1.16  2000/03/30 21:33:30  mike
 *      change termination of raw_exp to use ints, not MAPM numbers
 *
 *      Revision 1.15  2000/02/05 22:59:46  mike
 *      adjust decimal places on calculation
 *
 *      Revision 1.14  2000/02/04 20:45:21  mike
 *      re-compute log(2) on the fly if we need a more precise value
 *
 *      Revision 1.13  2000/02/04 19:35:14  mike
 *      use just an approx log(2) for the integer divide
 *
 *      Revision 1.12  2000/02/04 16:47:32  mike
 *      use the real algorithm for EXP
 *
 *      Revision 1.11  1999/09/18 01:27:40  mike
 *      if X is 0 on the pow function, return 0 right away
 *
 *      Revision 1.10  1999/06/19 20:54:07  mike
 *      changed local static MAPM to stack variables
 *
 *      Revision 1.9  1999/06/01 22:37:44  mike
 *      adjust decimal places passed to raw function
 *
 *      Revision 1.8  1999/06/01 01:44:03  mike
 *      change threshold from 1000 to 100 for 65536 divisor
 *
 *      Revision 1.7  1999/06/01 01:03:31  mike
 *      vary 'q' instead of checking input against +/- 10 and +/- 40
 *
 *      Revision 1.6  1999/05/15 01:54:27  mike
 *      add check for number of decimal places
 *
 *      Revision 1.5  1999/05/15 01:09:49  mike
 *      minor tweak to POW decimal places
 *
 *      Revision 1.4  1999/05/13 00:14:00  mike
 *      added more comments
 *
 *      Revision 1.3  1999/05/12 23:39:05  mike
 *      change #places passed to sub functions
 *
 *      Revision 1.2  1999/05/10 21:35:13  mike
 *      added some comments
 *
 *      Revision 1.1  1999/05/10 20:56:31  mike
 *      Initial revision
 */

#include "m_apm_lc.h"

static  M_APM  MM_local_copy_log2;
static  int    MM_log2_digits;

/****************************************************************************/
void	m_apm_exp(r,places,x)
M_APM	r, x;
int	places;
{
M_APM   tmp7, tmp8, tmp9;
char    outstr[32];
int	dplaces, nn, ii;
static  M_APM  MM_exp_log2;
static  M_APM  MM_exp_1024R;
static	int    firsttime1 = TRUE;

if (firsttime1)
  {
   firsttime1         = FALSE;
   MM_log2_digits     = VALID_DECIMAL_PLACES;

   MM_exp_log2        = m_apm_init();
   MM_exp_1024R       = m_apm_init();
   MM_local_copy_log2 = m_apm_init();

   m_apm_set_string(MM_exp_log2, "0.6931471806");       /* ~ log(2) */
   m_apm_set_string(MM_exp_1024R, "9.765625E-4");       /* 1 / 1024 */
   m_apm_copy(MM_local_copy_log2, MM_LOG_2_BASE_E);
  }

tmp7 = M_get_stack_var();
tmp8 = M_get_stack_var();
tmp9 = M_get_stack_var();

/*
    From David H. Bailey's MPFUN Fortran package :

    exp (t) =  (1 + r + r^2 / 2! + r^3 / 3! + r^4 / 4! ...) ^ q * 2 ^ n

    where q = 256, r = t' / q, t' = t - n Log(2) and where n is chosen so
    that -0.5 Log(2) < t' <= 0.5 Log(2).  Reducing t mod Log(2) and
    dividing by 256 insures that -0.001 < r <= 0.001, which accelerates
    convergence in the above series.

    we won't let n == 1 or -1. if n computes out to be 1, we will set
    n = 0. this is so we can still calculate log(2) and log(3).
    if we let n = 1, then we need log(2) in order to compute log(2),
    which is no good ...

    if the new 'n' == 0, we will then use q = 1024 instead of 256 so
    the series still converges as fast.

    since the value of log(2) is critical to this function, log, and
    log10, we will keep our own local copy of it. if we need more
    precision in our log(2), we will re-compute it on the fly to get
    the desired precision.
*/

/* find 'n' and convert it to a normal C int          */
/* we just need an approx log(2) for this calculation */

m_apm_integer_divide(tmp9, x, MM_exp_log2);
m_apm_to_integer_string(outstr, tmp9);
nn = atoi(outstr);

if (abs(nn) == 1)
  nn = 0;

if (nn == 0)
  {
   ii = 10;
   dplaces = places + 8;
   m_apm_multiply(tmp9, x, MM_exp_1024R);
  }
else
  {
   ii = 8;
   dplaces = places + 6;

   /* does our local copy of log(2) have enough precision?? */
   /* if not, go calculate a more precise value for log(2)  */

   if (dplaces > MM_log2_digits)
     {
      MM_log2_digits = dplaces + 2;
      m_apm_log(MM_local_copy_log2, (dplaces + 8), MM_Two);
     }

   m_apm_set_long(tmp7, (long)nn);
   m_apm_multiply(tmp8, tmp7, MM_local_copy_log2);
   m_apm_subtract(tmp7, x, tmp8);
   m_apm_multiply(tmp9, tmp7, MM_5x_256R);
  }

/* perform the series expansion ... */

M_raw_exp(tmp8, dplaces, tmp9);

/*
 *   raise result to the 256 OR 1024 power (q = 256 OR 1024) 
 *
 *   note : x ^ 256    == (((x ^ 2) ^ 2) ^ 2) ... 8  times
 *        : x ^ 1024   == (((x ^ 2) ^ 2) ^ 2) ... 10 times
 */

while (--ii >= 0)   
  {
   m_apm_multiply(tmp9, tmp8, tmp8);
   m_apm_round(tmp8, dplaces, tmp9);
  }

/* now compute 2 ^ N */

m_apm_integer_pow(tmp7, dplaces, MM_Two, nn);

m_apm_multiply(tmp9, tmp7, tmp8);
m_apm_round(r, places, tmp9);
M_restore_stack(3);                    /* restore the 3 locals we used here */
}
/****************************************************************************/
/*
	calculate the exponential function using the following series :

                              x^2     x^3     x^4    x^5
	exp(x) == 1  +  x  +  ---  +  ---  +  ---  + ---  ...
                               2!      3!      4!     5!

*/
void	M_raw_exp(rr,places,xx)
M_APM	rr, xx;
int	places;
{
M_APM   tmp0, digit, term;
int	tolerance,  local_precision;
long    m1;

tmp0  = M_get_stack_var();
term  = M_get_stack_var();
digit = M_get_stack_var();

local_precision = places + 6;
tolerance       = -(places + 2);

m_apm_add(rr, MM_One, xx);
m_apm_copy(term, xx);

m1 = 2;

while (TRUE)
  {
   m_apm_set_long(digit, m1);
   m_apm_multiply(tmp0, term, xx);
   m_apm_divide(term, local_precision, tmp0, digit);
   m_apm_add(tmp0, rr, term);
   m_apm_copy(rr, tmp0);

   if ((term->m_apm_exponent < tolerance) || (term->m_apm_sign == 0))
     break;

   m1++;
  }

M_restore_stack(3);                    /* restore the 3 locals we used here */
}
/****************************************************************************/
