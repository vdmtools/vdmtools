
/* 
 *  M_APM  -  primenum.c
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
 *      $Id: PRIMENUM.C,v 1.1.1.1 2001/10/04 13:02:57 paulm Exp $
 *
 *      PRIME Number Generator using the MAPM Library
 *
 *	$Log: PRIMENUM.C,v $
 *	Revision 1.1.1.1  2001/10/04 13:02:57  paulm
 *	Import of dir
 *
 *	Revision 1.2  1999/07/12 02:36:23  mike
 *	added more usage and added more comments
 *
 *	Revision 1.1  1999/07/12 02:31:23  mike
 *	Initial revision
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "m_apm.h"

extern int is_number_prime(M_APM);

#define FALSE 0
#define TRUE 1

char	 buffer[2048];

int main(argc, argv)
int argc;  char *argv[];
{
int      ct;
				/* declare the M_APM variables ... */
M_APM    aa_mapm;
M_APM    bb_mapm;
M_APM    cc_mapm;
M_APM    dd_mapm;

if (argc < 2)
  {
   fprintf(stdout,"Usage: primenum number\t\t\t\t[Version 1.0]\n");
   fprintf(stdout,
      "       find the first 10 prime numbers starting with \'number\'\n");

   exit(4);
  }
				/* now initialize the M_APM variables ... */
aa_mapm = m_apm_init();
bb_mapm = m_apm_init();
cc_mapm = m_apm_init();
dd_mapm = m_apm_init();

m_apm_set_string(dd_mapm, argv[1]);

if (m_apm_compare(dd_mapm, MM_Three) == -1)
  {
   m_apm_copy(dd_mapm, MM_Three);
  }

m_apm_integer_divide(aa_mapm, dd_mapm, MM_Two);
m_apm_multiply(bb_mapm, MM_Two, aa_mapm);
m_apm_add(aa_mapm, MM_One, bb_mapm);

ct = 0;

while (TRUE)
  {
   if (is_number_prime(aa_mapm))
     {
      m_apm_to_integer_string(buffer, aa_mapm);
      fprintf(stdout,"%s\n",buffer);

      if (++ct == 10)
        break;
     }

   m_apm_add(cc_mapm, MM_Two, aa_mapm);
   m_apm_copy(aa_mapm, cc_mapm);
  }

m_apm_free(aa_mapm);
m_apm_free(bb_mapm);
m_apm_free(cc_mapm);
m_apm_free(dd_mapm);

exit(0);
}
/****************************************************************************/
/*
 *     functions returns TRUE if the M_APM input number is prime
 *                       FALSE if it is not
 */
int    is_number_prime(input)
M_APM  input;
{
static int firsttimep = TRUE;
static M_APM limit;
static M_APM digit;
static M_APM quot;
static M_APM rem;
static M_APM tmp0;

if (firsttimep)
  {
   firsttimep = FALSE;
   limit      = m_apm_init();
   digit      = m_apm_init();
   quot       = m_apm_init();
   rem        = m_apm_init();
   tmp0       = m_apm_init();
  }

m_apm_sqrt(limit, 80, input);
m_apm_copy(digit, MM_Three);             /* start at '3' to check */

while (TRUE)
  {
   if (m_apm_compare(digit, limit) == 1)
     return (TRUE);

   m_apm_integer_div_rem(quot, rem, input, digit);

   if (m_apm_sign(rem) == 0)   /* remainder == 0 */
     return (FALSE);

   m_apm_add(tmp0, digit, MM_Two);
   m_apm_copy(digit, tmp0);
  }
}
/****************************************************************************/
