
/* 
 *  M_APM  -  mapmsqrt.c
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
 *      $Id: MAPMSQRT.C,v 1.1.1.1 2001/10/04 13:02:57 paulm Exp $
 *
 *      This file contains the SQRT function.
 *
 *      $Log: MAPMSQRT.C,v $
 *      Revision 1.1.1.1  2001/10/04 13:02:57  paulm
 *      Import of dir
 *
 *      Revision 1.11  2000/07/11 17:56:22  mike
 *      make better estimate for initial precision
 *
 *      Revision 1.10  1999/07/21 02:48:45  mike
 *      added some comments
 *
 *      Revision 1.9  1999/07/19 00:25:44  mike
 *      adjust local precision again
 *
 *      Revision 1.8  1999/07/19 00:09:41  mike
 *      adjust local precision during loop
 *
 *      Revision 1.7  1999/07/18 22:57:08  mike
 *      change to dynamically changing local precision and
 *      change tolerance checks using integers
 *
 *      Revision 1.6  1999/06/19 21:18:00  mike
 *      changed local static variables to MAPM stack variables
 *
 *      Revision 1.5  1999/05/31 01:40:39  mike
 *      minor update to normalizing the exponent
 *
 *      Revision 1.4  1999/05/31 01:21:41  mike
 *      optimize for large exponents
 *
 *      Revision 1.3  1999/05/12 20:59:35  mike
 *      use a better 'guess' function
 *
 *      Revision 1.2  1999/05/10 21:15:26  mike
 *      added some comments
 *
 *      Revision 1.1  1999/05/10 20:56:31  mike
 *      Initial revision
 */

#include "m_apm_lc.h"

/****************************************************************************/
void	m_apm_sqrt(rr,places,aa)
M_APM	rr, aa;
int	places;
{
M_APM   last_x, guess, tmpN, tmp8, tmp9;
int	ii, nexp, tolerance, maxp, local_precision;

if (aa->m_apm_sign <= 0)
  {
   if (aa->m_apm_sign == -1)
     fprintf(stderr,"Warning! ... \'m_apm_sqrt\', Negative argument\n");

   rr->m_apm_datalength = 1;
   rr->m_apm_sign       = 0;
   rr->m_apm_exponent   = 0;
   rr->m_apm_data[0]    = 0;
   return;
  }

last_x    = M_get_stack_var();
guess     = M_get_stack_var();
tmpN      = M_get_stack_var();
tmp8      = M_get_stack_var();
tmp9      = M_get_stack_var();

m_apm_copy(tmpN, aa);
nexp = 0;

/* 
    normalize the input number (make the exponent near 0) so
    the 'guess' function will not over/under flow on large
    magnitude exponents.
*/

if (aa->m_apm_exponent > 2)
  {
   nexp = aa->m_apm_exponent / 2;
   tmpN->m_apm_exponent -= 2 * nexp;
  }

if (aa->m_apm_exponent < -2)
  {
   nexp = aa->m_apm_exponent / 2 - 1;
   tmpN->m_apm_exponent -= 2 * nexp;
  }

M_get_sqrt_guess(guess, tmpN);

tolerance = -(places + 2);
maxp      = places + 8;

m_apm_multiply(tmp8, guess, guess);
m_apm_subtract(tmp9, tmpN, tmp8);

if ((local_precision = 16 - tmp9->m_apm_exponent) < 30)
  local_precision = 30;

m_apm_negate(last_x, MM_Ten);

/*   Use the following iteration to calculate the sqrt :

                                 N
         X     =  0.5  *  [ X + --- ]
          n+1                    X
*/

ii = 0;

while (TRUE)
  {
   m_apm_divide(tmp9, local_precision, tmpN, guess);
   m_apm_add(tmp8, tmp9, guess);
   m_apm_multiply(guess, tmp8, MM_0_5);

   /* force at least 2 iterations so 'last_x' has valid data */

   if (ii != 0)
     {
      m_apm_subtract(tmp8, guess, last_x);

      if ((tmp8->m_apm_exponent < tolerance) || (tmp8->m_apm_sign == 0))
        break;
     }

   if (ii == 0)
     local_precision *= 2;
   else
     local_precision += 2 - 2 * tmp8->m_apm_exponent;

   if (local_precision > maxp)
     local_precision = maxp;
  
   m_apm_copy(last_x, guess);
   ii++;
  }

m_apm_round(rr, places, guess);
rr->m_apm_exponent += nexp;
M_restore_stack(5);
}
/****************************************************************************/
