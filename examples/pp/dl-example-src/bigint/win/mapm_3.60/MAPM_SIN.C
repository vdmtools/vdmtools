
/* 
 *  M_APM  -  mapm_sin.c
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
 *      $Id: MAPM_SIN.C,v 1.1.1.1 2001/10/04 13:02:57 paulm Exp $
 *
 *      This file contains the top level (user callable) SIN / COS / TAN
 *	functions.
 *
 *      $Log: MAPM_SIN.C,v $
 *      Revision 1.1.1.1  2001/10/04 13:02:57  paulm
 *      Import of dir
 *
 *      Revision 1.12  2000/07/11 23:34:42  mike
 *      adjust loop break-out for AGM PI algorithm
 *
 *      Revision 1.11  2000/07/11 20:19:47  mike
 *      use new algorithm to compute PI (AGM)
 *
 *      Revision 1.10  2000/05/21 01:07:57  mike
 *      use _sin_cos in _tan function
 *
 *      Revision 1.9  2000/05/19 17:13:56  mike
 *      use local copies of PI variables & recompute
 *      on the fly as needed
 *
 *      Revision 1.8  1999/09/21 21:03:06  mike
 *      make sure the sign of 'sin' from M_cos_to_sin is non-zero
 *      before assigning it from the original angle.
 *
 *      Revision 1.7  1999/09/18 03:27:27  mike
 *      added m_apm_sin_cos
 *
 *      Revision 1.6  1999/07/09 22:50:33  mike
 *      skip limit to PI when not needed
 *
 *      Revision 1.5  1999/06/20 23:42:29  mike
 *      use new function for COS function
 *
 *      Revision 1.4  1999/06/20 19:27:12  mike
 *      changed local static variables to MAPM stack variables
 *
 *      Revision 1.3  1999/05/17 03:54:56  mike
 *      init globals in TAN function also
 *
 *      Revision 1.2  1999/05/15 02:18:31  mike
 *      add check for number of decimal places
 *
 *      Revision 1.1  1999/05/10 20:56:31  mike
 *      Initial revision
 */

#include "m_apm_lc.h"

/****************************************************************************/
void	m_apm_sin(r,places,a)
M_APM	r, a;
int	places;
{
M_APM	tmp3;

tmp3 = M_get_stack_var();
M_limit_angle_to_pi(tmp3, (places + 6), a);
M_5x_sin(r, places, tmp3);
M_restore_stack(1);
}
/****************************************************************************/
void	m_apm_cos(r,places,a)
M_APM	r, a;
int	places;
{
M_APM	tmp3;

tmp3 = M_get_stack_var();
M_limit_angle_to_pi(tmp3, (places + 6), a);
M_4x_cos(r, places, tmp3);
M_restore_stack(1);
}
/****************************************************************************/
void	m_apm_sin_cos(sinv,cosv,places,aa)
M_APM	sinv, cosv, aa;
int	places;
{
M_APM	tmp5, tmp6, tmp7;

tmp5 = M_get_stack_var();
tmp6 = M_get_stack_var();
tmp7 = M_get_stack_var();

M_limit_angle_to_pi(tmp5, (places + 6), aa);

MM_skip_limit_PI_check = TRUE;
m_apm_cos(tmp7, (places + 4), tmp5);
MM_skip_limit_PI_check = FALSE;

/*
 *   compute sin(x) = sqrt(1 - cos(x) ^ 2).
 *
 *   note that the sign of 'sin' will always be positive after the
 *   sqrt call. we need to adjust the sign based on what quadrant
 *   the original angle is in.
 */

M_cos_to_sin(tmp6, (places + 4), tmp7);
if (tmp6->m_apm_sign != 0)
  tmp6->m_apm_sign = tmp5->m_apm_sign;
 
m_apm_round(sinv, places, tmp6);
m_apm_round(cosv, places, tmp7);
M_restore_stack(3);
}
/****************************************************************************/
void	m_apm_tan(r,places,a)
M_APM	r, a;
int	places;
{
M_APM	tmps, tmpc, tmp0;

tmps = M_get_stack_var();
tmpc = M_get_stack_var();
tmp0 = M_get_stack_var();

m_apm_sin_cos(tmps, tmpc, (places + 4), a);
 
/* tan(x) = sin(x) / cos(x) */

m_apm_divide(tmp0, (places + 4), tmps, tmpc);
m_apm_round(r, places, tmp0);
M_restore_stack(3);
}
/****************************************************************************/
void	M_limit_angle_to_pi(rr, places, aa)
M_APM	rr, aa;
int	places;
{
M_APM	tmp7, tmp8, tmp9;

if (MM_skip_limit_PI_check)
  {
   m_apm_copy(rr, aa);
   return;
  }

M_check_PI_places(places);

tmp9 = M_get_stack_var();
m_apm_copy(tmp9, MM_lc_PI);

if (m_apm_compare(aa, tmp9) == 1)       /*  > PI  */
  {
   tmp7 = M_get_stack_var();
   tmp8 = M_get_stack_var();

   m_apm_add(tmp7, aa, tmp9);
   m_apm_integer_divide(tmp9, tmp7, MM_lc_2_PI);
   m_apm_multiply(tmp8, tmp9, MM_lc_2_PI);
   m_apm_subtract(tmp9, aa, tmp8);
   m_apm_round(rr, places, tmp9);

   M_restore_stack(3);
   return;
  }

tmp9->m_apm_sign = -1;
if (m_apm_compare(aa, tmp9) == -1)       /*  < -PI  */
  {
   tmp7 = M_get_stack_var();
   tmp8 = M_get_stack_var();

   m_apm_add(tmp7, aa, tmp9);
   m_apm_integer_divide(tmp9, tmp7, MM_lc_2_PI);
   m_apm_multiply(tmp8, tmp9, MM_lc_2_PI);
   m_apm_subtract(tmp9, aa, tmp8);
   m_apm_round(rr, places, tmp9);

   M_restore_stack(3);
   return;
  }

m_apm_copy(rr, aa);
M_restore_stack(1);
}
/****************************************************************************/
/*
 *	check if our local copy of PI is precise enough
 *	for our purpose. if not, calculate PI so it's
 *	as precise as desired, accurate to 'places' decimal
 *	places.
 */
void	M_check_PI_places(places)
int	places;
{
int     dplaces;

dplaces = places + 2;

if (dplaces > MM_lc_PI_digits)
  {
   MM_lc_PI_digits = dplaces + 2;

   /* compute PI using the AGM  (see right below) */

   M_calculate_PI_AGM(MM_lc_PI, (dplaces + 5));

   m_apm_multiply(MM_lc_HALF_PI, MM_0_5, MM_lc_PI);
   m_apm_multiply(MM_lc_2_PI, MM_Two, MM_lc_PI);
  }
}
/****************************************************************************/
/*
 *      Calculate PI using the AGM (Arithmetic-Geometric Mean)
 *
 *      Init :  A0  = 1
 *              B0  = 1 / sqrt(2)
 *              Sum = 1
 *
 *      Iterate: n = 1...
 *
 *
 *      A   =  0.5 * [ A    +  B   ]
 *       n              n-1     n-1
 *
 *
 *      B   =  sqrt [ A    *  B   ]
 *       n             n-1     n-1
 *
 *
 *
 *      C   =  0.5 * [ A    -  B   ]
 *       n              n-1     n-1
 *
 *
 *                      2      n+1
 *     Sum  =  Sum  -  C   *  2
 *                      n
 *
 *
 *      At the end when C  is 'small enough' :
 *                       n
 *
 *                    2 
 *      PI  =  4  *  A    /  Sum
 *                    n+1
 *
 */
void	M_calculate_PI_AGM(outv, places)
M_APM   outv;
int     places;
{
M_APM   tmp1, tmp2, a0, b0, c0, a1, b1, sum, pow_2;
int     dflag, dplaces, nn;

tmp1  = m_apm_init();
tmp2  = m_apm_init();
a0    = m_apm_init();
b0    = m_apm_init();
c0    = m_apm_init();
a1    = m_apm_init();
b1    = m_apm_init();
sum   = m_apm_init();
pow_2 = m_apm_init();

dplaces = places + 16;
dflag   = FALSE;

m_apm_copy(a0, MM_One);
m_apm_copy(sum, MM_One);
m_apm_copy(pow_2, MM_Four);

m_apm_sqrt(tmp1, dplaces, MM_Two);
m_apm_multiply(b0, tmp1, MM_0_5);

while (TRUE)
  {
   m_apm_add(tmp1, a0, b0);
   m_apm_multiply(a1, MM_0_5, tmp1);

   if (dflag)
     break;

   m_apm_multiply(tmp1, a0, b0);
   m_apm_sqrt(b1, dplaces, tmp1);

   m_apm_subtract(tmp1, a0, b0);
   m_apm_multiply(c0, MM_0_5, tmp1);

   /*
    *   the net 'PI' calculated from this iteration will
    *   be accurate to ~4 X the value of (c0)'s exponent.
    *   this was determined experimentally. we will be
    *   (overly?) conservative and assume the ratio is 
    *   only 3.8 ( or 19 / 5). this won't have any effect
    *   for normal/real life use of this library.
    */

   nn = (-(c0->m_apm_exponent) * 19) / 5;

   if (nn >= dplaces)
     dflag = TRUE;

   m_apm_multiply(tmp1, c0, c0);
   m_apm_multiply(tmp2, tmp1, pow_2);
   m_apm_subtract(tmp1, sum, tmp2);
   m_apm_round(sum, dplaces, tmp1);

   m_apm_copy(a0, a1);
   m_apm_copy(b0, b1);

   m_apm_multiply(tmp1, pow_2, MM_Two);
   m_apm_copy(pow_2, tmp1);
  }

m_apm_multiply(tmp1, a1, a1);
m_apm_multiply(tmp2, tmp1, MM_Four);
m_apm_divide(tmp1, dplaces, tmp2, sum);
m_apm_round(outv, places, tmp1);

m_apm_free(pow_2);
m_apm_free(sum);
m_apm_free(b1);
m_apm_free(a1);
m_apm_free(c0);
m_apm_free(b0);
m_apm_free(a0);
m_apm_free(tmp2);
m_apm_free(tmp1);
}
/****************************************************************************/
/*
 *   compute  r = sqrt(1 - a ^ 2).
 */
void	M_cos_to_sin(r,places,a)
M_APM	r, a;
int	places;
{
M_APM	tmp1, tmp2;

tmp1 = M_get_stack_var();
tmp2 = M_get_stack_var();

m_apm_multiply(tmp1, a, a);
m_apm_subtract(tmp2, MM_One, tmp1);
m_apm_sqrt(r, places, tmp2);
M_restore_stack(2);
}
/****************************************************************************/
