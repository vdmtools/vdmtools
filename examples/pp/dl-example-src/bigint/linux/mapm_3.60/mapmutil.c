
/* 
 *  M_APM  -  mapmutil.c
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
 *      $Id: mapmutil.c,v 1.1.1.1 2001/10/04 13:02:56 paulm Exp $
 *
 *      This file contains various utility functions needed by the 
 *	library in addition to some basic user callable functions.
 *
 *      $Log: mapmutil.c,v $
 *      Revision 1.1.1.1  2001/10/04 13:02:56  paulm
 *      Import of dir
 *
 *      Revision 1.11  2000/08/22 20:21:54  mike
 *      fix m_apm_exponent with exactly 0 as the input
 *
 *      Revision 1.10  2000/08/22 00:01:26  mike
 *      add zero check in is_integer
 *
 *      Revision 1.9  2000/08/21 23:34:44  mike
 *      add new function _is_integer
 *
 *      Revision 1.8  2000/08/01 22:29:02  mike
 *      add sizeof int function call
 *
 *      Revision 1.7  2000/05/19 16:21:03  mike
 *      delete M_check_dec_places, no longer needed
 *
 *      Revision 1.6  2000/04/04 17:06:37  mike
 *      initialize C++ refcount struct element to 1
 *
 *      Revision 1.5  2000/02/03 22:49:56  mike
 *      use MAPM_* generic memory function
 *
 *      Revision 1.4  1999/09/18 03:06:41  mike
 *      fix m_apm_exponent
 *
 *      Revision 1.3  1999/09/18 02:59:11  mike
 *      added new functions
 *
 *      Revision 1.2  1999/05/15 02:21:14  mike
 *      add check for number of decimal places
 *
 *      Revision 1.1  1999/05/10 20:56:31  mike
 *      Initial revision
 */

#include "m_apm_lc.h"

static  UCHAR	*M_mul_div = NULL;
static  UCHAR   *M_mul_rem = NULL;

static  UCHAR   M_mul_div_10[100];
static	UCHAR   M_mul_rem_10[100];

static	UCHAR	*M_div_array = NULL;
static	UCHAR	*M_rem_array = NULL;

static	int	M_div_rem_size = 0;
static	int	M_util_firsttime = TRUE;

static	M_APM	M_work_0_5;

/****************************************************************************/
M_APM	m_apm_init()
{
static	int firsttime = TRUE;
M_APM	atmp;

if (firsttime)
  {
   firsttime = FALSE;
   M_init_util_data();
   M_init_trig_globals();
  }

if ((atmp = MAPM_MALLOC(sizeof(struct M_APM_struct))) == NULL)
  {
   fprintf(stderr,"\'m_apm_init\', Out of memory\n");
   exit(4);
  }

atmp->m_apm_id           = M_APM_IDENT;
atmp->m_apm_malloclength = 80;
atmp->m_apm_datalength   = 1;
atmp->m_apm_refcount     = 1;           /* not for us, for MAPM C++ class */
atmp->m_apm_exponent     = 0;
atmp->m_apm_sign         = 0;

if ((atmp->m_apm_data = (UCHAR *)MAPM_MALLOC(84)) == NULL)
  {
   fprintf(stderr,"\'m_apm_init\', Out of memory\n");
   exit(6);
  }

atmp->m_apm_data[0] = 0;
return(atmp);
}
/****************************************************************************/
void	m_apm_free(atmp)
M_APM	atmp;
{
if (atmp->m_apm_id == M_APM_IDENT)
  {
   atmp->m_apm_id = 0x0FFFFFF0L;
   MAPM_FREE(atmp->m_apm_data);
   MAPM_FREE(atmp);
  }
}
/****************************************************************************/
/*
 *      just a dummy wrapper to keep some compilers from complaining
 */
int 	M_get_sizeof_int()
{
return(sizeof(int));
}
/****************************************************************************/
void	M_init_util_data()
{
int	k;
UCHAR   ndiv, nrem;

if (M_mul_div != NULL)
  return;

M_div_array = (UCHAR *)MAPM_MALLOC(256 * sizeof(UCHAR));
M_rem_array = (UCHAR *)MAPM_MALLOC(256 * sizeof(UCHAR));
M_div_rem_size = 252;

M_mul_div = (UCHAR *)MAPM_MALLOC(10000 * sizeof(UCHAR));
M_mul_rem = (UCHAR *)MAPM_MALLOC(10000 * sizeof(UCHAR));

if (M_div_array == NULL || M_rem_array == NULL ||
    M_mul_div == NULL || M_mul_rem == NULL)
  {
   fprintf(stderr,"M_init_util_data() could not allocate memory\n");
   exit(10);
  }

ndiv = 0;
nrem = 0;

for (k=0; k < 100; k++)
  {
   M_mul_div_10[k] = ndiv;
   M_mul_rem_10[k] = nrem;

   if (++nrem == 10)
     {
      nrem = 0;
      ndiv++;
     }
  }

ndiv = 0;
nrem = 0;

for (k=0; k < 10000; k++)
  {
   M_mul_div[k] = ndiv;
   M_mul_rem[k] = nrem;

   if (++nrem == 100)
     {
      nrem = 0;
      ndiv++;
     }
  }
}
/****************************************************************************/
void	M_get_div_rem(tbl_lookup,ndiv,nrem)
int	tbl_lookup;
UCHAR   *ndiv, *nrem;
{
*ndiv = M_mul_div[tbl_lookup];
*nrem = M_mul_rem[tbl_lookup];
}
/****************************************************************************/
void	M_get_div_rem_10(tbl_lookup,ndiv,nrem)
int	tbl_lookup;
UCHAR   *ndiv, *nrem;
{
*ndiv = M_mul_div_10[tbl_lookup];
*nrem = M_mul_rem_10[tbl_lookup];
}
/****************************************************************************/
void	m_apm_round(btmp,places,atmp) 
M_APM	btmp, atmp;
int	places;
{
int	ii;

if (M_util_firsttime)
  {
   M_util_firsttime = FALSE;

   M_work_0_5 = m_apm_init();
   m_apm_set_string(M_work_0_5, "5");
  }

ii = places + 1;

if (atmp->m_apm_datalength <= ii)
  {
   m_apm_copy(btmp,atmp);
   return;
  }

M_work_0_5->m_apm_exponent = atmp->m_apm_exponent - ii;

if (atmp->m_apm_sign > 0)
  m_apm_add(btmp, atmp, M_work_0_5);
else
  m_apm_subtract(btmp, atmp, M_work_0_5);

btmp->m_apm_datalength = ii;
M_apm_normalize(btmp);
}
/****************************************************************************/
int	m_apm_sign(atmp)
M_APM	atmp;
{
return(atmp->m_apm_sign);
}
/****************************************************************************/
int	m_apm_exponent(atmp)
M_APM	atmp;
{
if (atmp->m_apm_sign == 0)
  return(0);
else
  return(atmp->m_apm_exponent - 1);
}
/****************************************************************************/
int	m_apm_significant_digits(atmp)
M_APM	atmp;
{
return(atmp->m_apm_datalength);
}
/****************************************************************************/
int	m_apm_is_integer(atmp)
M_APM	atmp;
{
if (atmp->m_apm_sign == 0)
  return(1);

if (atmp->m_apm_exponent >= atmp->m_apm_datalength)
  return(1);
else
  return(0);
}
/****************************************************************************/
void	m_apm_negate(d,s)
M_APM	d;
M_APM	s;
{
m_apm_copy(d,s);
if (d->m_apm_sign != 0)
    d->m_apm_sign = -(d->m_apm_sign);
}
/****************************************************************************/
void	m_apm_absolute_value(d,s)
M_APM	d;
M_APM	s;
{
m_apm_copy(d,s);
if (d->m_apm_sign != 0)
    d->m_apm_sign = 1;
}
/****************************************************************************/
void	m_apm_copy(dest,src)
M_APM	dest;
M_APM	src;
{
int	j;
void	*vp;

j = (src->m_apm_datalength + 1) >> 1;
if (j > dest->m_apm_malloclength)
  {
   if ((vp = MAPM_REALLOC(dest->m_apm_data,(j + 256))) == NULL)
     {
      fprintf(stderr,"\'m_apm_copy\', Out of memory\n");
      exit(14);
     }
   
   dest->m_apm_malloclength = j + 252;
   dest->m_apm_data = (UCHAR *)vp;
  }

dest->m_apm_datalength = src->m_apm_datalength;
dest->m_apm_exponent   = src->m_apm_exponent;
dest->m_apm_sign       = src->m_apm_sign;

memcpy(dest->m_apm_data,src->m_apm_data,j);
}
/****************************************************************************/
int	m_apm_compare(ltmp,rtmp)
M_APM	ltmp;
M_APM	rtmp;
{
int	llen, rlen, lsign, rsign, i, j, lexp, rexp;

llen  = ltmp->m_apm_datalength;
rlen  = rtmp->m_apm_datalength;

lsign = ltmp->m_apm_sign;
rsign = rtmp->m_apm_sign;

lexp  = ltmp->m_apm_exponent;
rexp  = rtmp->m_apm_exponent;

if (rsign == 0)
  return(lsign);

if (lsign == 0)
  return(-rsign);

if (lsign == -1 && rsign == 1)
  return(-1);

if (lsign == 1 && rsign == -1)
  return(1);

/* signs are the same, check the exponents */

if (lexp > rexp)
  goto E1;

if (lexp < rexp)
  goto E2;

/* signs and exponents are the same, check the data */

if (llen < rlen)
  j = (llen + 1) >> 1;
else
  j = (rlen + 1) >> 1;

for (i=0; i < j; i++)
  {
   if (ltmp->m_apm_data[i] > rtmp->m_apm_data[i])
     goto E1;

   if (ltmp->m_apm_data[i] < rtmp->m_apm_data[i])
     goto E2;
  }

if (llen == rlen)
   return(0);
else
  {
   if (llen > rlen)
     goto E1;
   else
     goto E2;
  }

E1:

if (lsign == 1)
  return(1);
else
  return(-1);

E2:

if (lsign == 1)
  return(-1);
else
  return(1);
}
/****************************************************************************/
void	M_apm_normalize(atmp)
M_APM	atmp;
{
int	i, index, datalength, exponent;
UCHAR   numdiv, numrem, numdiv1, numrem1;

if (atmp->m_apm_sign == 0)
  return;

datalength = atmp->m_apm_datalength;
exponent   = atmp->m_apm_exponent;

/* make sure trailing bytes/chars are 0                */
/* the following function will adjust the 'datalength' */
/* we want the original value and will fix it later    */

M_apm_pad(atmp,(datalength + 3));

while (TRUE)			/* remove lead-in '0' if any */
  {
   M_get_div_rem_10((int)atmp->m_apm_data[0],&numdiv,&numrem);

   if (numdiv >= 1)      /* number is normalized, done here */
     break;

   index = (datalength + 1) >> 1;

   if (numrem == 0)      /* both nibbles are 0, we can move full bytes */
     {
      memmove(atmp->m_apm_data, (atmp->m_apm_data + 1), index);
      datalength -= 2;
      exponent -= 2;
     }
   else
     {
      for (i=0; i < index; i++)
        {
         M_get_div_rem_10((int)atmp->m_apm_data[i],&numdiv,&numrem);
         M_get_div_rem_10((int)atmp->m_apm_data[i+1],&numdiv1,&numrem1);
   
         atmp->m_apm_data[i] = 10 * numrem + numdiv1;
        }
   
      datalength--;
      exponent--;
     }
  }

while (TRUE)			/* remove trailing '0' if any */
  {
   index = ((datalength + 1) >> 1) - 1;
   M_get_div_rem_10((int)atmp->m_apm_data[index],&numdiv,&numrem);

   if (numrem != 0)		/* last digit non-zero, all done */
     break;

   if ((datalength & 1) != 0)   /* if odd, then first char must be non-zero */
     {
      if (numdiv != 0)
        break;
     }

   if (datalength == 1)
     {
      atmp->m_apm_sign = 0;
      exponent = 0;
      break;
     }
     
   datalength--;
  }

atmp->m_apm_datalength = datalength;
atmp->m_apm_exponent   = exponent;
}
/****************************************************************************/
void	M_apm_scale(ctmp,count)
M_APM	ctmp;
int	count;
{
int	i, numb, ct;
UCHAR	*chp, *pdiv, *prem;
void	*vp;

ct = count;

i = (ctmp->m_apm_datalength + ct + 1) >> 1;
if (i > ctmp->m_apm_malloclength)
  {
   if ((vp = MAPM_REALLOC(ctmp->m_apm_data,(i + 256))) == NULL)
     {
      fprintf(stderr,"\'M_apm_scale\', Out of memory\n");
      exit(18);
     }
   
   ctmp->m_apm_malloclength = i + 252;
   ctmp->m_apm_data = (UCHAR *)vp;
  }

if ((ct & 1) != 0)          /* move odd number first */
  {
   ct--;
   numb = (ctmp->m_apm_datalength + 1) >> 1;

   if (numb > M_div_rem_size)
     {
      if ((vp = MAPM_REALLOC(M_div_array,(numb + 256))) == NULL)
        {
         fprintf(stderr,"\'M_apm_scale\', Out of memory\n");
         exit(18);
        }

      M_div_array = (UCHAR *)vp;
      M_div_rem_size = numb + 252;

      if ((vp = MAPM_REALLOC(M_rem_array,(numb + 256))) == NULL)
        {
         fprintf(stderr,"\'M_apm_scale\', Out of memory\n");
         exit(18);
        }

      M_rem_array = (UCHAR *)vp;
     }

   pdiv = M_div_array;
   prem = M_rem_array;
   chp  = ctmp->m_apm_data;

   while (TRUE)
     {
      M_get_div_rem_10((int)(*chp++),pdiv,prem);
      pdiv++;
      prem++;

      if (--numb == 0)
        break;
     }

   *pdiv = 0;
   *prem = 0;

   pdiv = M_div_array;
   prem = M_rem_array;

   ctmp->m_apm_exponent++;
   ctmp->m_apm_datalength++;

   numb = (ctmp->m_apm_datalength + 1) >> 1;
   chp  = ctmp->m_apm_data;
   *chp = *pdiv++;

   while (--numb != 0)
     {
      chp++;
      *chp = *prem * 10 + *pdiv;
      pdiv++;
      prem++;
     }
  }

/* ct is even here */

if (ct > 0)
  {
   numb = (ctmp->m_apm_datalength + 1) >> 1;
   i    = ct >> 1;
   
   memmove((ctmp->m_apm_data + i), ctmp->m_apm_data, numb);
   memset(ctmp->m_apm_data, 0, i);
   
   ctmp->m_apm_datalength += ct;
   ctmp->m_apm_exponent += ct;
  }
}
/****************************************************************************/
void	M_apm_pad(ctmp,new_length)
M_APM	ctmp;
int	new_length;
{
int	num1, numb, ct;
UCHAR	numdiv, numrem;
void	*vp;

ct = new_length;
if (ctmp->m_apm_datalength >= ct)
  return;
  
numb = (ct + 1) >> 1;
if (numb > ctmp->m_apm_malloclength)
  {
   if ((vp = MAPM_REALLOC(ctmp->m_apm_data,(numb + 256))) == NULL)
     {
      fprintf(stderr,"\'M_apm_pad\', Out of memory\n");
      exit(18);
     }
   
   ctmp->m_apm_malloclength = numb + 252;
   ctmp->m_apm_data = (UCHAR *)vp;
  }

num1 = (ctmp->m_apm_datalength + 1) >> 1;

if ((ctmp->m_apm_datalength & 1) != 0)
  {
   M_get_div_rem_10((int)ctmp->m_apm_data[num1 - 1],&numdiv,&numrem);
   ctmp->m_apm_data[num1 - 1] = 10 * numdiv;
  }

memset((ctmp->m_apm_data + num1), 0, (numb - num1));
ctmp->m_apm_datalength = ct;
}
/****************************************************************************/
/*
 *      this function will convert a string to lowercase
 */
char    *M_lowercase(s)
char    *s;
{
char    *p;

p = s;

while (TRUE)
  {
   if (*p >= 'A' && *p <= 'Z')
     *p += 'a' - 'A';

   if (*p++ == '\0')  break;
  }
return(s);
}
/****************************************************************************/
/*    returns char position of first occurence of s2 in s1
	  or -1 if no match found
*/
int     M_strposition(s1,s2)
char    *s1, *s2;
{
register char  ch1, ch2;
char           *p0, *p1, *p2;
int            ct;

ct = -1;
p0 = s1;

if (*s2 == '\0')  return(-1);

while (TRUE)
  {
   ct++;
   p1  = p0;
   p2  = s2;
   ch2 = *p2;
   
   while (TRUE)                    /* scan until first char matches */
     {
      if ((ch1 = *p1) == '\0')  return(-1);
      if (ch1 == ch2)           break;
      p1++;
      ct++;
     }

   p2++;                           /* check remainder of 2 strings */
   p1++;
   p0 = p1;

   while (TRUE)
     {
      if ((ch2 = *p2) == '\0')  return(ct);
      if (*p1 != ch2)           break;
      p1++;
      p2++;
     }
  }
}
/****************************************************************************/

/*
      debug_dsp(cc)
      M_APM cc;
      {
static char buffer[8192];

m_apm_to_string(buffer, -1, cc);
printf("(dsp func) = [%s]\n",buffer);

      }
*/

