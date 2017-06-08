
/* 
 *  M_APM  -  mapm_set.c
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
 *      $Id: mapm_set.c,v 1.1.1.1 2001/10/04 13:02:56 paulm Exp $
 *
 *      This file contains the functions necessary to get C 'longs' and
 *	'strings' into the MAPM number system. It also contains the function
 *	to get a string from a MAPM number.
 *
 *      $Log: mapm_set.c,v $
 *      Revision 1.1.1.1  2001/10/04 13:02:56  paulm
 *      Import of dir
 *
 *      Revision 1.8  2000/05/04 23:49:19  mike
 *      put in more efficient set_long function
 *
 *      Revision 1.7  2000/02/03 22:47:15  mike
 *      use MAPM_* generic memory function
 *
 *      Revision 1.6  1999/07/12 22:23:17  mike
 *      tweak output string when input == 0
 *
 *      Revision 1.5  1999/07/12 02:07:56  mike
 *      fix dec_places error (was == -1, should be < 0)
 *
 *      Revision 1.4  1999/06/19 21:36:57  mike
 *      added some comments
 *
 *      Revision 1.3  1999/06/19 21:35:19  mike
 *      changed local static variables to MAPM stack variables
 *
 *      Revision 1.2  1999/05/13 21:32:41  mike
 *      added check for illegal chars in string parse
 *
 *      Revision 1.1  1999/05/10 20:56:31  mike
 *      Initial revision
 */

#include "m_apm_lc.h"

/****************************************************************************/
void	m_apm_set_long(atmp,mm)
M_APM	atmp;
long	mm;
{
int     len, ii, nbytes;
char	*p, *buf, ch, buf2[64];

/* if zero, return right away */

if (mm == 0)
  {
   atmp->m_apm_datalength = 1;
   atmp->m_apm_exponent   = 0;
   atmp->m_apm_sign       = 0;
   atmp->m_apm_data[0]    = 0;
   return;
  }

sprintf(buf2,"%ld",mm);
buf = buf2;

if (mm < 0)
  {
   atmp->m_apm_sign = -1;
   buf++;                     /* get past '-' sign */
  }
else
  {
   atmp->m_apm_sign = 1;
  }

len = strlen(buf);
atmp->m_apm_exponent = len;

/* least significant nibble of ODD data-length must be 0 */

if ((len & 1) != 0)
  {
   buf[len] = '0';
  }

/* remove any trailing '0' ... */

while (TRUE)
  {
   if (buf[--len] != '0')
     break;
  }

atmp->m_apm_datalength = ++len;

nbytes = (len + 1) >> 1;
p = buf;

for (ii=0; ii < nbytes; ii++)
  {
   ch = *p++ - '0';
   atmp->m_apm_data[ii] = 10 * ch + *p++ - '0';
  }
}
/****************************************************************************/
void	m_apm_set_string(ctmp,s_in)
M_APM	ctmp;
char	*s_in;
{
char	ch, *cp, *s, *p;
void	*vp;
int	i, j, zflag, exponent, sign;
static	char *M_buf  = NULL;
static  int   M_lbuf = 0;

if (M_lbuf == 0)
  {
   M_lbuf = 256;
   if ((M_buf = (char *)MAPM_MALLOC(256)) == NULL)
     {
      fprintf(stderr,"\'m_apm_set_string\', Out of memory\n");
      exit(12);
     }
  }

if ((i = strlen(s_in)) > (M_lbuf - 4))
  {
   M_lbuf = i + 32;
   if ((vp = MAPM_REALLOC(M_buf, M_lbuf)) == NULL)
     {
      fprintf(stderr,"\'m_apm_set_string\', Out of memory\n");
      exit(14);
     }

   M_buf = vp;
  }

s = M_buf;
strcpy(s,s_in);

/* default == zero ... */

ctmp->m_apm_datalength   = 1;
ctmp->m_apm_exponent     = 0;
ctmp->m_apm_sign         = 0;
ctmp->m_apm_data[0]      = 0;

p = s;

while (TRUE)
  {
   if (*p == ' ' || *p == '\t')
     p++;
   else
     break;
  }

if (*p == '\0')
  return;

sign = 1;             /* assume number is positive */

if (*p == '+')        /* scan by optional '+' sign */
  p++;
else
  {
   if (*p == '-')     /* check if number negative */
     {
      sign = -1;
      p++;
     }
  }

M_lowercase(p);       /* convert string to lowercase */
exponent = 0;         /* default */
   
if ((cp = strstr(p,"e")) != NULL)
  {
   exponent = atoi(cp + sizeof(char));
   *cp = '\0';          /* erase the exponent now */
  }

j = M_strposition(p,".");        /* is there a decimal point ?? */
if (j == -1)
  {
   strcat(p,".");                /* if not, append one */
   j = M_strposition(p,".");     /* now find it ... */
  }

if (j > 0)                       /* normalize number and adjust exponent */
  {
   exponent += j;
   memmove((p+1),p,(j * sizeof(char)));
  }

p++;        /* scan past implied decimal point now in column 1 (index 0) */

i = strlen(p);
ctmp->m_apm_datalength = i;

if ((i & 1) != 0)   /* if odd number of digits, append a '0' to make it even */
  strcat(p,"0");    

j = strlen(p) >> 1;  /* number of bytes in encoded M_APM number */

/* do we need more memory to hold this number */

if (j > ctmp->m_apm_malloclength)
  {
   if ((vp = MAPM_REALLOC(ctmp->m_apm_data, (j + 256))) == NULL)
     {
      fprintf(stderr,"\'m_apm_set_string\', Out of memory\n");
      exit(10);
     }
  
   ctmp->m_apm_malloclength = j + 252;
   ctmp->m_apm_data = (UCHAR *)vp;
  }

zflag = TRUE;

for (i=0; i < j; i++)
  {
   ch = *p++ - '0';
   if ((ch = (10 * ch + *p++ - '0')) != 0)
     zflag = FALSE;

   if (((int)ch & 0xFF) >= 100)
     {
      fprintf(stderr,
      "Warning! \'m_apm_set_string\' : Non-digit char found in parse \n");

      fprintf(stderr,"Text = [%s]\n",s_in);

      m_apm_copy(ctmp, MM_Zero);
      return;
     }

   ctmp->m_apm_data[i]   = ch;
   ctmp->m_apm_data[i+1] = 0;
  }

ctmp->m_apm_exponent = exponent;
ctmp->m_apm_sign     = sign;

if (zflag)
  {
   ctmp->m_apm_exponent   = 0;
   ctmp->m_apm_sign       = 0;
   ctmp->m_apm_datalength = 1;
  }
else
  {
   M_apm_normalize(ctmp);
  }
}
/****************************************************************************/
void	m_apm_to_string(s,places,mtmp)
char	*s;
int	places;
M_APM	mtmp;
{
M_APM   ctmp;
char	*cp;
int	i, index, first, max_i, num_digits, dec_places;
UCHAR	numdiv, numrem;

ctmp = M_get_stack_var();
dec_places = places;

if (dec_places < 0)
  m_apm_copy(ctmp, mtmp);
else
  m_apm_round(ctmp, dec_places, mtmp);

if (ctmp->m_apm_sign == 0)
  {
   if (dec_places < 0)
      strcpy(s,"0.0E+0");
   else
     {
      strcpy(s,"0");

      if (dec_places > 0)
        strcat(s,".");

      for (i=0; i < dec_places; i++)
        strcat(s,"0");

      strcat(s,"E+0");
     }

   M_restore_stack(1);
   return;
  }

max_i = (ctmp->m_apm_datalength + 1) >> 1;

if (dec_places < 0)
  num_digits = ctmp->m_apm_datalength;
else
  num_digits = dec_places + 1;

cp = s;

if (ctmp->m_apm_sign == -1)
  *cp++ = '-';

first = TRUE;

i = 0;
index = 0;

while (TRUE)
  {
   if (index >= max_i)
     {
      numdiv = 0;
      numrem = 0;
     }
   else
      M_get_div_rem_10((int)ctmp->m_apm_data[index],&numdiv,&numrem);

   index++;

   *cp++ = numdiv + '0';

   if (++i == num_digits)
     break;

   if (first)
     {
      first = FALSE;
      *cp++ = '.';
     }

   *cp++ = numrem + '0';

   if (++i == num_digits)
     break;
  }

i = ctmp->m_apm_exponent - 1;
if (i >= 0)
  sprintf(cp,"E+%d",i);
else
  sprintf(cp,"E%d",i);

M_restore_stack(1);
}
/****************************************************************************/
