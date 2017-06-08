
/* 
 *  M_APM  -  mapm_mul.c
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
 *      $Id: MAPM_MUL.C,v 1.1.1.1 2001/10/04 13:02:57 paulm Exp $
 *
 *      This file contains basic multiplication function.
 *
 *      $Log: MAPM_MUL.C,v $
 *      Revision 1.1.1.1  2001/10/04 13:02:57  paulm
 *      Import of dir
 *
 *      Revision 1.9  2000/07/09 00:20:03  mike
 *      change break even point again ....
 *
 *      Revision 1.8  2000/07/08 18:51:43  mike
 *      change break even point between this O(n^2)
 *      multiply and the FFT multiply
 *
 *      Revision 1.7  2000/04/14 16:27:45  mike
 *      change the break even point between the 2 multiply
 *      functions since we made the fast one even faster.
 *
 *      Revision 1.6  2000/02/03 22:46:40  mike
 *      use MAPM_* generic memory function
 *
 *      Revision 1.5  1999/09/19 21:10:14  mike
 *      change the break even point between the 2 multiply choices
 *
 *      Revision 1.4  1999/08/09 23:57:17  mike
 *      added more comments
 *
 *      Revision 1.3  1999/08/09 02:38:17  mike
 *      tweak break even point and add comments
 *
 *      Revision 1.2  1999/08/08 18:35:20  mike
 *      add call to fast algorithm if input numbers are large
 *
 *      Revision 1.1  1999/05/10 20:56:31  mike
 *      Initial revision
 */

#include "m_apm_lc.h"

extern void M_fast_multiply(M_APM, M_APM, M_APM);

/****************************************************************************/
void	m_apm_multiply(r,a,b)
M_APM	r, a, b;
{
int	i, j, k, itmp, sign, nexp, ii, jj, index1, index2, 
	indexr, index0, numdigits;
UCHAR   *cp, numdiv, numrem;
void	*vp;

sign = a->m_apm_sign * b->m_apm_sign;
nexp = a->m_apm_exponent + b->m_apm_exponent;

if (sign == 0)      /* one number is zero, result is zero */
  {
   r->m_apm_datalength = 1;
   r->m_apm_sign       = 0;
   r->m_apm_data[0]    = 0;
   r->m_apm_exponent   = 0;
   return;
  }

numdigits = a->m_apm_datalength + b->m_apm_datalength;
index1 = (a->m_apm_datalength + 1) >> 1;
index2 = (b->m_apm_datalength + 1) >> 1;

/* 
 *	If we are multiplying 2 'big' numbers, use the fast algorithm. 
 *
 *	This is a **very** approx break even point between this algorithm
 *      and the FFT multiply. Note that different CPU's, operating systems, 
 *      and compiler's may yield a different break even point. This point
 *      (~96 decimal digits) is how the experiment came out on the 
 *      author's system. 
 */

if (index1 >= 48 && index2 >= 48)
  {
   M_fast_multiply(r, a, b);
   return;
  }

k = (numdigits + 1) >> 1;     /* required size of result, in bytes */

if (k > r->m_apm_malloclength)
  {
   if ((vp = MAPM_REALLOC(r->m_apm_data,(k + 256))) == NULL)
     {
      fprintf(stderr,"\'m_apm_multiply\', Out of memory\n");
      exit(16);
     }
  
   r->m_apm_malloclength = k + 252;
   r->m_apm_data = (UCHAR *)vp;
  }

index0 = index1 + index2;
cp = r->m_apm_data;
memset(cp, 0, index0);
ii = index1 - 1;

for (i=0; i < index1; i++)
  {
   jj = index2 - 1;
   indexr = --index0;

   for (j=0; j < index2; j++)
     {
      itmp = (int)a->m_apm_data[ii] * (int)b->m_apm_data[jj];
      M_get_div_rem(itmp,&numdiv,&numrem);

      cp[indexr]   += numrem;
      cp[indexr-1] += numdiv;

      if (cp[indexr] >= 100)
        {
         cp[indexr] -= 100;
         cp[indexr-1] += 1;
	}

      indexr--;

      if (cp[indexr] >= 100)
        {
         cp[indexr] -= 100;
         cp[indexr-1] += 1;
	}

      jj--;
     }

   ii--;
  }

r->m_apm_sign       = sign;
r->m_apm_exponent   = nexp;
r->m_apm_datalength = numdigits;

M_apm_normalize(r);
}
/****************************************************************************/
