
/* 
 *  M_APM  -  mapmipwr.c
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
 *      $Id: mapmipwr.c,v 1.1.1.1 2001/10/04 13:02:56 paulm Exp $
 *
 *      This file contains the Integer Power function.
 *
 *      $Log: mapmipwr.c,v $
 *      Revision 1.1.1.1  2001/10/04 13:02:56  paulm
 *      Import of dir
 *
 *      Revision 1.3  2000/05/24 17:03:35  mike
 *      return 1 when input is 0^0
 *
 *      Revision 1.2  1999/09/18 01:34:35  mike
 *      minor tweaks
 *
 *      Revision 1.1  1999/09/18 01:33:09  mike
 *      Initial revision
 */

#include "m_apm_lc.h"

/****************************************************************************/
void	m_apm_integer_pow(rr,places,aa,mexp)
M_APM	rr, aa;
int	places, mexp;
{
M_APM   tmp0, tmpy, tmpz;
int	nexp, ii, signflag, local_precision;

if (mexp == 0)
  {
   m_apm_copy(rr, MM_One);
   return;
  }
else
  {
   if (mexp > 0)
     {
      signflag = 0;
      nexp     = mexp;
     }
   else
     {
      signflag = 1;
      nexp     = -mexp;
     }
  }

if (aa->m_apm_sign == 0)
  {
   rr->m_apm_datalength = 1;
   rr->m_apm_sign       = 0;
   rr->m_apm_exponent   = 0;
   rr->m_apm_data[0]    = 0;
   return;
  }

tmp0 = M_get_stack_var();
tmpy = M_get_stack_var();
tmpz = M_get_stack_var();

local_precision = places + 8;

m_apm_copy(tmpy, MM_One);
m_apm_copy(tmpz, aa);

while (TRUE)
  {
   ii   = nexp & 1;
   nexp = nexp >> 1;

   if (ii != 0)                       /* exponent -was- odd */
     {
      m_apm_multiply(tmp0, tmpy, tmpz);
      m_apm_round(tmpy, local_precision, tmp0);

      if (nexp == 0)
        break;
     }

   m_apm_multiply(tmp0, tmpz, tmpz);
   m_apm_round(tmpz, local_precision, tmp0);
  }

if (signflag)
  {
   m_apm_divide(tmp0, (places + 6), MM_One, tmpy);
   m_apm_round(rr, places, tmp0);
  }
else
  {
   m_apm_round(rr, places, tmpy);
  }

M_restore_stack(3);
}
/****************************************************************************/
