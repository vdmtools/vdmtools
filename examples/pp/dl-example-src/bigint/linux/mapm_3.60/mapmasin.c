
/* 
 *  M_APM  -  mapmasin.c
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
 *      $Id: mapmasin.c,v 1.1.1.1 2001/10/04 13:02:56 paulm Exp $
 *
 *      This file contains the 'ARC' family of functions; ARC-SIN, ARC-COS,
 *	ARC-TAN, and ARC-TAN2.
 *
 *      $Log: mapmasin.c,v $
 *      Revision 1.1.1.1  2001/10/04 13:02:56  paulm
 *      Import of dir
 *
 *      Revision 1.16  2000/05/28 23:58:41  mike
 *      minor optimization to arc-tan2
 *
 *      Revision 1.15  2000/05/19 17:13:29  mike
 *      use local copies of PI variables & recompute
 *      on the fly as needed
 *
 *      Revision 1.14  2000/03/27 21:43:23  mike
 *      dtermine how many iterations should be required at
 *      run time for arc-sin and arc-cos
 *
 *      Revision 1.13  1999/09/21 21:00:33  mike
 *      make sure the sign of 'sin' from M_cos_to_sin is non-zero
 *      before assigning it from the original angle.
 *
 *      Revision 1.12  1999/07/21 03:05:06  mike
 *      added some comments
 *
 *      Revision 1.11  1999/07/19 02:33:39  mike
 *      reset local precision again
 *
 *      Revision 1.10  1999/07/19 02:18:05  mike
 *      more fine tuning of local precision
 *
 *      Revision 1.9  1999/07/19 00:08:34  mike
 *      adjust local precision during iterative loops
 *
 *      Revision 1.8  1999/07/18 22:35:56  mike
 *      make arc-sin and arc-cos use dynamically changing
 *      precision to speed up iterative routines for large N
 *
 *      Revision 1.7  1999/07/09 22:52:00  mike
 *      skip limit PI check when not needed
 *
 *      Revision 1.6  1999/07/09 00:10:39  mike
 *      use better method for arc sin and arc cos
 *
 *      Revision 1.5  1999/07/08 22:56:20  mike
 *      replace local MAPM constant with a global
 *
 *      Revision 1.4  1999/06/20 16:55:01  mike
 *      changed local static variables to MAPM stack variables
 *
 *      Revision 1.3  1999/05/15 02:10:27  mike
 *      add check for number of decimal places
 *
 *      Revision 1.2  1999/05/10 21:10:21  mike
 *      added some comments
 *
 *      Revision 1.1  1999/05/10 20:56:31  mike
 *      Initial revision
 */

#include "m_apm_lc.h"
#include <math.h>

/****************************************************************************/
void	m_apm_arctan2(rr,places,yy,xx)
M_APM	rr, yy, xx;
int	places;
{
M_APM   tmp5, tmp6, tmp7;
int	ix, iy;

tmp5 = M_get_stack_var();
tmp6 = M_get_stack_var();
tmp7 = M_get_stack_var();

iy = yy->m_apm_sign;
ix = xx->m_apm_sign;

if (ix == 0)       /* x == 0 */
  {
   if (iy == 0)    /* y == 0 */
     {
      fprintf(stderr,"Warning! ... \'m_apm_arctan2\', Both Arguments == 0\n");
   
      rr->m_apm_datalength = 1;
      rr->m_apm_sign       = 0;
      rr->m_apm_exponent   = 0;
      rr->m_apm_data[0]    = 0;

      goto ATAN2_DONE;
     }

   M_check_PI_places(places);
   m_apm_round(rr, places, MM_lc_HALF_PI);
   rr->m_apm_sign = iy;
   goto ATAN2_DONE;
  }

if (iy == 0)
  {
   if (ix == 1)
     {
      rr->m_apm_datalength = 1;
      rr->m_apm_sign       = 0;
      rr->m_apm_exponent   = 0;
      rr->m_apm_data[0]    = 0;
     }
   else
     {
      M_check_PI_places(places);
      m_apm_round(rr, places, MM_lc_PI);
     } 

   goto ATAN2_DONE;
  }

/*
 *    the special cases have been handled, now do the real work
 */

m_apm_divide(tmp6, (places + 4), yy, xx);
m_apm_arctan(tmp5, (places + 4), tmp6);

if (ix == 1)         /* 'x' is positive */
  {
   m_apm_round(rr, places, tmp5);
  }
else                 /* 'x' is negative */
  {
   M_check_PI_places(places);

   if (iy == 1)      /* 'y' is positive */
     {
      m_apm_add(tmp7, tmp5, MM_lc_PI);
      m_apm_round(rr, places, tmp7);
     }
   else              /* 'y' is negative */
     {
      m_apm_subtract(tmp7, tmp5, MM_lc_PI);
      m_apm_round(rr, places, tmp7);
     }
  }

ATAN2_DONE:

M_restore_stack(3);
}
/****************************************************************************/
/*
        Calculate arctan using the identity :

                                      x
        arctan (x) == arcsin [ --------------- ]
                                sqrt(1 + x^2)

*/
void	m_apm_arctan(rr,places,xx)
M_APM	rr, xx;
int	places;
{
M_APM   tmp8, tmp9;

tmp8 = M_get_stack_var();
tmp9 = M_get_stack_var();

m_apm_multiply(tmp9, xx, xx);
m_apm_add(tmp8, tmp9, MM_One);
m_apm_sqrt(tmp9, (places + 4), tmp8);
m_apm_divide(tmp8, (places + 4), xx, tmp9);
m_apm_arcsin(rr, places, tmp8);
M_restore_stack(2);
}
/****************************************************************************/
void	m_apm_arcsin(r,places,x)
M_APM	r, x;
int	places;
{
M_APM   tmp0, tmp1, tmp2, tmp3, current_x;
int	ii, maxiter, maxp, tolerance, local_precision;
char	sbuf[64];

current_x = M_get_stack_var();
tmp0      = M_get_stack_var();
tmp1      = M_get_stack_var();
tmp2      = M_get_stack_var();
tmp3      = M_get_stack_var();

m_apm_absolute_value(tmp0, x);

ii = m_apm_compare(tmp0, MM_One);

if (ii == 1)       /* |x| > 1 */
  {
   fprintf(stderr,"Warning! ... \'m_apm_arcsin\', |Argument| > 1\n");

   r->m_apm_datalength = 1;
   r->m_apm_sign       = 0;
   r->m_apm_exponent   = 0;
   r->m_apm_data[0]    = 0;

   M_restore_stack(5);
   return;
  }

if (ii == 0)       /* |x| == 1, arcsin = +/- PI / 2 */
  {
   M_check_PI_places(places);
   m_apm_round(r, places, MM_lc_HALF_PI);
   r->m_apm_sign = x->m_apm_sign;

   M_restore_stack(5);
   return;
  }

if (m_apm_compare(tmp0, MM_0_85) == 1)        /* check if > 0.85 */
  {
   M_cos_to_sin(tmp2, (places + 4), x);
   m_apm_arccos(r, places, tmp2);
   r->m_apm_sign = x->m_apm_sign;

   M_restore_stack(5);
   return;
  }

tolerance       = -(places + 2);
maxp            = places + 4 - x->m_apm_exponent;
local_precision = 25 - x->m_apm_exponent;

/*
 *      compute the maximum number of iterations
 *	that should be needed to calculate to
 *	the desired accuracy.  [ constant below ~= 1 / log(2) ]
 */

maxiter = (int)(log((double)(places + 2)) * 1.442695) + 1;

if (maxiter < 5)
  maxiter = 5;

M_get_asin_guess(current_x, x);

/*    Use the following iteration to solve for arc-sin :

                      sin(X) - N
      X     =  X  -  ------------
       n+1              cos(X)
*/

ii = 0;

while (TRUE)
  {
   MM_skip_limit_PI_check = TRUE;
   m_apm_cos(tmp1, local_precision, current_x);
   MM_skip_limit_PI_check = FALSE;

   M_cos_to_sin(tmp2, local_precision, tmp1);
   if (tmp2->m_apm_sign != 0)
     tmp2->m_apm_sign = current_x->m_apm_sign;

   m_apm_subtract(tmp3, tmp2, x);
   m_apm_divide(tmp0, local_precision, tmp3, tmp1);

   m_apm_subtract(tmp2, current_x, tmp0);
   m_apm_copy(current_x, tmp2);

   if (ii != 0)
     {
      if ((tmp0->m_apm_exponent < tolerance) || (tmp0->m_apm_sign == 0))
        break;
     }

   if (++ii == maxiter)
     {
      fprintf(stderr,
          "Warning! ... \'m_apm_arcsin\', max iteration count reached\n");

      m_apm_absolute_value(tmp1, tmp0);
      m_apm_to_string(sbuf, 4, tmp1);

      fprintf(stderr,"arc-sin solution accurate to %s \n",sbuf);
      break;
     }

   local_precision += 2 - 2 * tmp0->m_apm_exponent;

   if (local_precision > maxp)
     local_precision = maxp;
  }

m_apm_round(r, places, current_x);
M_restore_stack(5);
}
/****************************************************************************/
void	m_apm_arccos(r,places,x)
M_APM	r, x;
int	places;
{
M_APM   tmp0, tmp1, tmp2, tmp3, current_x;
int	ii, maxiter, maxp, tolerance, local_precision;
char    sbuf[64];

current_x = M_get_stack_var();
tmp0      = M_get_stack_var();
tmp1      = M_get_stack_var();
tmp2      = M_get_stack_var();
tmp3      = M_get_stack_var();

m_apm_absolute_value(tmp0, x);

ii = m_apm_compare(tmp0, MM_One);

if (ii == 1)       /* |x| > 1 */
  {
   fprintf(stderr,"Warning! ... \'m_apm_arccos\', |Argument| > 1\n");

   r->m_apm_datalength = 1;
   r->m_apm_sign       = 0;
   r->m_apm_exponent   = 0;
   r->m_apm_data[0]    = 0;

   M_restore_stack(5);
   return;
  }

if (ii == 0)       /* |x| == 1, arccos = 0, PI */
  {
   if (x->m_apm_sign == 1)
     {
      r->m_apm_datalength = 1;
      r->m_apm_sign       = 0;
      r->m_apm_exponent   = 0;
      r->m_apm_data[0]    = 0;
     }
   else
     {
      M_check_PI_places(places);
      m_apm_round(r, places, MM_lc_PI);
     }

   M_restore_stack(5);
   return;
  }

if (m_apm_compare(tmp0, MM_0_85) == 1)        /* check if > 0.85 */
  {
   M_cos_to_sin(tmp2, (places + 4), x);

   if (x->m_apm_sign == 1)
     {
      m_apm_arcsin(r, places, tmp2);
     }
   else
     {
      M_check_PI_places(places);
      m_apm_arcsin(tmp3, (places + 4), tmp2);
      m_apm_subtract(tmp1, MM_lc_PI, tmp3);
      m_apm_round(r, places, tmp1);
     }

   M_restore_stack(5);
   return;
  }

tolerance       = -(places + 2);
maxp            = places + 4;
local_precision = 25;

/*
 *      compute the maximum number of iterations
 *	that should be needed to calculate to
 *	the desired accuracy.  [ constant below ~= 1 / log(2) ]
 */

maxiter = (int)(log((double)(places + 2)) * 1.442695) + 1;

if (maxiter < 5)
  maxiter = 5;

M_get_acos_guess(current_x, x);

/*    Use the following iteration to solve for arc-cos :

                      cos(X) - N
      X     =  X  +  ------------
       n+1              sin(X)
*/

ii = 0;

while (TRUE)
  {
   MM_skip_limit_PI_check = TRUE;
   m_apm_cos(tmp1, local_precision, current_x);
   MM_skip_limit_PI_check = FALSE;

   M_cos_to_sin(tmp2, local_precision, tmp1);
   if (tmp2->m_apm_sign != 0)
     tmp2->m_apm_sign = current_x->m_apm_sign;

   m_apm_subtract(tmp3, tmp1, x);
   m_apm_divide(tmp0, local_precision, tmp3, tmp2);

   m_apm_add(tmp2, current_x, tmp0);
   m_apm_copy(current_x, tmp2);

   if (ii != 0)
     {
      if ((tmp0->m_apm_exponent < tolerance) || (tmp0->m_apm_sign == 0))
        break;
     }

   if (++ii == maxiter)
     {
      fprintf(stderr,
          "Warning! ... \'m_apm_arccos\', max iteration count reached\n");

      m_apm_absolute_value(tmp1, tmp0);
      m_apm_to_string(sbuf, 4, tmp1);

      fprintf(stderr,"arc-cos solution accurate to %s \n",sbuf);
      break;
     }

   local_precision += 2 - 2 * tmp0->m_apm_exponent;

   if (local_precision > maxp)
     local_precision = maxp;
  }

m_apm_round(r, places, current_x);
M_restore_stack(5);
}
/****************************************************************************/
