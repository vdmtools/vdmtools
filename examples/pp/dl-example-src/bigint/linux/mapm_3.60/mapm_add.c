
/* 
 *  M_APM  -  mapm_add.c
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
 *      $Id: mapm_add.c,v 1.1.1.1 2001/10/04 13:02:56 paulm Exp $
 *
 *      This file contains basic addition/subtraction functions
 *
 *      $Log: mapm_add.c,v $
 *      Revision 1.1.1.1  2001/10/04 13:02:56  paulm
 *      Import of dir
 *
 *      Revision 1.1  1999/05/10 20:56:31  mike
 *      Initial revision
 *
 *
 */

#include "m_apm_lc.h"

static	M_APM	M_work1 = NULL;
static	M_APM	M_work2 = NULL;
static	int	M_add_firsttime = TRUE;

/****************************************************************************/
void	m_apm_add(r,a,b)
M_APM	r, a, b;
{
int	j, carry, sign, aexp, bexp, adigits, bdigits;

if (M_add_firsttime)
  {
   M_add_firsttime = FALSE;
   M_work1 = m_apm_init();
   M_work2 = m_apm_init();
  }

if (a->m_apm_sign == 0)
  {
   m_apm_copy(r,b);
   return;
  }

if (b->m_apm_sign == 0)
  {
   m_apm_copy(r,a);
   return;
  }
  
if (a->m_apm_sign == 1 && b->m_apm_sign == -1)
  {
   b->m_apm_sign = 1;
   m_apm_subtract(r,a,b);
   b->m_apm_sign = -1;
   return;
  }

if (a->m_apm_sign == -1 && b->m_apm_sign == 1)
  {
   a->m_apm_sign = 1;
   m_apm_subtract(r,b,a);
   a->m_apm_sign = -1;
   return;
  }

sign = a->m_apm_sign;         /* signs are the same, result will be same */

aexp = a->m_apm_exponent;
bexp = b->m_apm_exponent;

m_apm_copy(M_work1, a);
m_apm_copy(M_work2, b);

/*
 *  scale by at least 1 factor of 10 in case the MSB carrys
 */

if (aexp == bexp)
  {
   M_apm_scale(M_work1, 2);   /* shift 2 digits == 1 byte for efficiency */
   M_apm_scale(M_work2, 2);
  }
else
  {
   if (aexp > bexp)
     {
      M_apm_scale(M_work1, 2);
      M_apm_scale(M_work2, (aexp + 2 - bexp));
     }
   else            /*  aexp < bexp  */
     {
      M_apm_scale(M_work2, 2);
      M_apm_scale(M_work1, (bexp + 2 - aexp));
     }
  }

adigits = M_work1->m_apm_datalength;
bdigits = M_work2->m_apm_datalength;

if (adigits >= bdigits)
  {
   m_apm_copy(r, M_work1);
   j = (bdigits + 1) >> 1;
   carry = 0;

   while (TRUE)
     {
      j--;
      r->m_apm_data[j] += carry + M_work2->m_apm_data[j];

      if (r->m_apm_data[j] >= 100)
        {
         r->m_apm_data[j] -= 100;
	 carry = 1;
	}
      else
         carry = 0;

      if (j == 0) 
        break;
     }
  }
else
  {
   m_apm_copy(r, M_work2);
   j = (adigits + 1) >> 1;
   carry = 0;

   while (TRUE)
     {
      j--;
      r->m_apm_data[j] += carry + M_work1->m_apm_data[j];

      if (r->m_apm_data[j] >= 100)
        {
         r->m_apm_data[j] -= 100;
	 carry = 1;
	}
      else
         carry = 0;

      if (j == 0) 
        break;
     }
  }

r->m_apm_sign = sign;

M_apm_normalize(r);
}
/****************************************************************************/
void	m_apm_subtract(r,a,b)
M_APM	r, a, b;
{
int	itmp, j, flag, icompare, sign, aexp, bexp, 
	borrow, adigits, bdigits;

if (M_add_firsttime)
  {
   M_add_firsttime = FALSE;
   M_work1 = m_apm_init();
   M_work2 = m_apm_init();
  }

if (b->m_apm_sign == 0)
  {
   m_apm_copy(r,a);
   return;
  }
  
if (a->m_apm_sign == 0)
  {
   m_apm_copy(r,b);
   r->m_apm_sign = -(r->m_apm_sign);
   return;
  }

if (a->m_apm_sign == 1 && b->m_apm_sign == -1)
  {
   b->m_apm_sign = 1;
   m_apm_add(r,a,b);
   b->m_apm_sign = -1;
   return;
  }

if (a->m_apm_sign == -1 && b->m_apm_sign == 1)
  {
   b->m_apm_sign = -1;
   m_apm_add(r,a,b);
   b->m_apm_sign = 1;
   return;
  }

/* now, the signs are the same  */
/* make a positive working copy */

m_apm_absolute_value(M_work1, a);
m_apm_absolute_value(M_work2, b);

/* are they the same??  if so, the result is zero */

if ((icompare = m_apm_compare(M_work1, M_work2)) == 0)
  {
   r->m_apm_sign       = 0;
   r->m_apm_exponent   = 0;
   r->m_apm_data[0]    = 0;
   r->m_apm_datalength = 1;
   return;
  }

if (icompare == 1)             /*  |a| > |b|  (do A-B)  */
  {
   flag = TRUE;
   sign = a->m_apm_sign;     
  }
else                           /*  |b| > |a|  (do B-A)  */
  {
   flag = FALSE;
   sign = -(a->m_apm_sign);     
  }

aexp = M_work1->m_apm_exponent;
bexp = M_work2->m_apm_exponent;

if (aexp > bexp)
  M_apm_scale(M_work2, (aexp - bexp));

if (aexp < bexp)
  M_apm_scale(M_work1, (bexp - aexp));

adigits = M_work1->m_apm_datalength;
bdigits = M_work2->m_apm_datalength;

if (adigits > bdigits)
  M_apm_pad(M_work2, adigits);

if (adigits < bdigits)
  M_apm_pad(M_work1, bdigits);

if (flag)		/* perform A-B,  M_work1 - M_work2 */
  {
   m_apm_copy(r, M_work1);
   j = (r->m_apm_datalength + 1) >> 1;
   borrow = 0;

   while (TRUE)
     {
      j--;
      itmp = (int)r->m_apm_data[j] - (int)M_work2->m_apm_data[j] - borrow;

      if (itmp >= 0)
        {
         r->m_apm_data[j] = (UCHAR)itmp;
	 borrow = 0;
        }
      else
        {
         r->m_apm_data[j] = (UCHAR)(100 + itmp);
	 borrow = 1;
	}

      if (j == 0) 
        break;
     }
  }
else   		/* perform B-A,  M_work2 - M_work1 */
  {
   m_apm_copy(r, M_work2);
   j = (r->m_apm_datalength + 1) >> 1;
   borrow = 0;

   while (TRUE)
     {
      j--;
      itmp = (int)r->m_apm_data[j] - (int)M_work1->m_apm_data[j] - borrow;

      if (itmp >= 0)
        {
         r->m_apm_data[j] = (UCHAR)itmp;
	 borrow = 0;
        }
      else
        {
         r->m_apm_data[j] = (UCHAR)(100 + itmp);
	 borrow = 1;
	}

      if (j == 0) 
        break;
     }
  }
   
r->m_apm_sign = sign;

M_apm_normalize(r);
}
/****************************************************************************/
