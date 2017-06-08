
/* 
 *  M_APM  -  mapmrsin.c
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
 *      $Id: mapmrsin.c,v 1.1.1.1 2001/10/04 13:02:56 paulm Exp $
 *
 *      This file contains the basic series expansion functions for 
 *	the SIN / COS functions.
 *
 *      $Log: mapmrsin.c,v $
 *      Revision 1.1.1.1  2001/10/04 13:02:56  paulm
 *      Import of dir
 *
 *      Revision 1.4  2000/03/30 21:53:48  mike
 *      change compare to terminate series expansion using ints instead
 *      of MAPM numbers
 *
 *      Revision 1.3  1999/06/20 16:23:10  mike
 *      changed local static variables to MAPM stack variables
 *
 *      Revision 1.2  1999/05/12 21:06:36  mike
 *      changed global var names
 *
 *      Revision 1.1  1999/05/10 20:56:31  mike
 *      Initial revision
 */

#include "m_apm_lc.h"

/****************************************************************************/
/*
                                x^3     x^5     x^7    x^9
		 sin(x) == x -  ---  +  ---  -  ---  + ---  ...
                                 3!      5!      7!     9!
*/
void	M_raw_sin(rr,places,xx)
M_APM 	rr, xx;
int 	places;
{
M_APM	sum, term, tmp7, tmp8, tmp9;
int     tolerance, flag;
long	m1, m2;

sum  = M_get_stack_var();
term = M_get_stack_var();
tmp7 = M_get_stack_var();
tmp8 = M_get_stack_var();
tmp9 = M_get_stack_var();

m_apm_copy(sum, xx);
m_apm_copy(term, xx);
m_apm_multiply(tmp8, xx, xx);
m_apm_round(tmp9, (places + 6), tmp8);

tolerance = -(places + 2);

m1   = 2L;
flag = 0;

while (TRUE)
  {
   m2 = m1 * (m1 + 1);
   m_apm_set_long(tmp7, m2);

   m_apm_multiply(tmp8, term, tmp9);
   m_apm_divide(term, (places + 6), tmp8, tmp7);

   if (flag == 0)
     {
      m_apm_subtract(tmp7, sum, term);
      m_apm_copy(sum, tmp7);
     }
   else
     {
      m_apm_add(tmp7, sum, term);
      m_apm_copy(sum, tmp7);
     }

   if ((term->m_apm_exponent < tolerance) || (term->m_apm_sign == 0))
     break;

   m1 += 2;
   flag = 1 - flag;
  }

m_apm_round(rr, places, sum);
M_restore_stack(5);
}
/****************************************************************************/
/*
                                x^2     x^4     x^6    x^8
		 cos(x) == 1 -  ---  +  ---  -  ---  + ---  ...
                                 2!      4!      6!     8!
*/
void	M_raw_cos(rr,places,xx)
M_APM 	rr, xx;
int 	places;
{
M_APM	sum, term, tmp7, tmp8, tmp9;
int     flag, tolerance;
long	m1, m2;

sum  = M_get_stack_var();
term = M_get_stack_var();
tmp7 = M_get_stack_var();
tmp8 = M_get_stack_var();
tmp9 = M_get_stack_var();

m_apm_copy(sum, MM_One);
m_apm_copy(term, MM_One);

m_apm_multiply(tmp8, xx, xx);
m_apm_round(tmp9, (places + 6), tmp8);

tolerance = -(places + 2);

m1   = 1L;
flag = 0;

while (TRUE)
  {
   m2 = m1 * (m1 + 1);
   m_apm_set_long(tmp7, m2);

   m_apm_multiply(tmp8, term, tmp9);
   m_apm_divide(term, (places + 6), tmp8, tmp7);

   if (flag == 0)
     {
      m_apm_subtract(tmp7, sum, term);
      m_apm_copy(sum, tmp7);
     }
   else
     {
      m_apm_add(tmp7, sum, term);
      m_apm_copy(sum, tmp7);
     }

   if ((term->m_apm_exponent < tolerance) || (term->m_apm_sign == 0))
     break;

   m1 += 2;
   flag = 1 - flag;
  }

m_apm_round(rr, places, sum);
M_restore_stack(5);
}
/****************************************************************************/
