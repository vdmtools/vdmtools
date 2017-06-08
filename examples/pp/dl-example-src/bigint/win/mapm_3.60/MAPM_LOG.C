
/* 
 *  M_APM  -  mapm_log.c
 *
 *  Copyright (C) 1999   Michael C. Ring
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
 *      $Id: MAPM_LOG.C,v 1.1.1.1 2001/10/04 13:02:57 paulm Exp $
 *
 *      This file contains the LOG and LOG10 functions.
 *
 *      $Log: MAPM_LOG.C,v $
 *      Revision 1.1.1.1  2001/10/04 13:02:57  paulm
 *      Import of dir
 *
 *      Revision 1.13  2000/07/11 18:58:35  mike
 *      do it right this time
 *
 *      Revision 1.12  2000/07/11 18:19:27  mike
 *      estimate a better initial precision
 *
 *      Revision 1.11  2000/05/19 16:14:15  mike
 *      update some comments
 *
 *      Revision 1.10  2000/05/17 23:47:35  mike
 *      recompute a local copy of log E base 10 on the fly
 *      if more precision is needed.
 *
 *      Revision 1.9  2000/03/27 21:44:12  mike
 *      determine how many iterations should be required at
 *      run time for log
 *
 *      Revision 1.8  1999/07/21 02:56:18  mike
 *      added some comments
 *
 *      Revision 1.7  1999/07/19 00:28:51  mike
 *      adjust local precision again
 *
 *      Revision 1.6  1999/07/19 00:10:34  mike
 *      adjust local precision during iterative loop
 *
 *      Revision 1.5  1999/07/18 23:15:54  mike
 *      change local precision dynamically and change
 *      tolerance to integers for faster iterative routine.
 *
 *      Revision 1.4  1999/06/19 21:08:32  mike
 *      changed local static variables to MAPM stack variables
 *
 *      Revision 1.3  1999/05/15 01:34:50  mike
 *      add check for number of decimal places
 *
 *      Revision 1.2  1999/05/10 21:42:32  mike
 *      added some comments
 *
 *      Revision 1.1  1999/05/10 20:56:31  mike
 *      Initial revision
 */

#include "m_apm_lc.h"
#include <math.h>

static	M_APM   M_lc_log_E_base_10;
static  int	M_lc_log_E_digits = -1;

/****************************************************************************/
/*
        Calls the LOG function. The formula used is :

        log10(x)  =  A * log(x) where A = log  (e) [0.43429...] 
                                             10
*/
void	m_apm_log10(r,places,a)
M_APM	r, a;
int	places;
{
int     dplaces;
M_APM   tmp8, tmp9;

tmp8 = M_get_stack_var();
tmp9 = M_get_stack_var();

if (M_lc_log_E_digits < 0)    /* assign local copy on first call */
  {
   M_lc_log_E_digits  = VALID_DECIMAL_PLACES;
   M_lc_log_E_base_10 = m_apm_init();
   m_apm_copy(M_lc_log_E_base_10, MM_LOG_E_BASE_10);
  }

dplaces = places + 4;

/* 
 *   if our local copy of Log  (E) is not accurate enough,
 *                           10
 *   compute a more precise value and save for later.
 */

if (dplaces > M_lc_log_E_digits)
  {
   M_lc_log_E_digits = dplaces + 2;
   m_apm_log(tmp8, (dplaces + 4), MM_Ten);
   m_apm_divide(M_lc_log_E_base_10, (dplaces + 4), MM_One, tmp8);
  }

m_apm_log(tmp9, dplaces, a);
m_apm_multiply(tmp8, tmp9, M_lc_log_E_base_10);
m_apm_round(r, places, tmp8);
M_restore_stack(2);                    /* restore the 2 locals we used here */
}
/****************************************************************************/
void	m_apm_log(r,places,a)
M_APM	r, a;
int	places;
{
M_APM   tmp0, tmp1, last_x, guess;
int	ii, maxiter, maxp, tolerance, local_precision;
char    sbuf[64];

if (a->m_apm_sign <= 0)
  {
   fprintf(stderr,"Warning! ... \'m_apm_log\', Negative argument\n");

   r->m_apm_datalength = 1;
   r->m_apm_sign       = 0;
   r->m_apm_exponent   = 0;
   r->m_apm_data[0]    = 0;
   return;
  }

tmp0      = M_get_stack_var();
tmp1      = M_get_stack_var();
last_x    = M_get_stack_var();
guess     = M_get_stack_var();

tolerance = -(places + 2);
maxp      = places + 8;

M_get_log_guess(guess, a);

m_apm_exp(tmp0, 30, guess);
m_apm_subtract(tmp1, a, tmp0);

local_precision = 16 + tmp0->m_apm_exponent - tmp1->m_apm_exponent;

if (local_precision < 30)
  local_precision = 30;

m_apm_negate(last_x, MM_Ten);

/*
 *      compute the maximum number of iterations
 *	that should be needed to calculate to
 *	the desired accuracy.  [ constant below ~= 1 / log(2) ]
 */

maxiter = (int)(log((double)(places + 2)) * 1.442695) + 1;

if (maxiter < 5)
  maxiter = 5;

/*
     Solve for the log by using the following iteration :

                         N
     X     =  X - 1 + -------      where e ^ X  is  exp(X)
      n+1              e ^ X		  
*/

ii = 0;

while (TRUE)
  {
   m_apm_exp(tmp0, local_precision, guess);

   m_apm_divide(tmp1, local_precision, a, tmp0); 
   m_apm_add(tmp0, tmp1, guess);
   m_apm_subtract(guess, tmp0, MM_One);

   /* force at least 2 iterations so 'last_x' has valid data */

   if (ii != 0)
     {
      m_apm_subtract(tmp1, guess, last_x);

      if ((tmp1->m_apm_exponent < tolerance) || (tmp1->m_apm_sign == 0))
        break;
     }

   if (ii == maxiter)
     {
      fprintf(stderr,
          "Warning! ... \'m_apm_log\', max iteration count reached\n");

      m_apm_absolute_value(tmp0, tmp1);
      m_apm_to_string(sbuf, 4, tmp0);

      fprintf(stderr,"log solution is accurate to %s\n",sbuf);
      break;
     }

   if (ii == 0)
     local_precision *= 2;
   else
     local_precision += 2 - 2 * tmp1->m_apm_exponent;

   if (local_precision > maxp)
     local_precision = maxp;

   m_apm_copy(last_x, guess);
   ii++;
  }

m_apm_round(r, places, guess);
M_restore_stack(4);                    /* restore the 4 locals we used here */
}
/****************************************************************************/
