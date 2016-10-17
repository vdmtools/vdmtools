
/* 
 *  M_APM  -  mapm_pow.c
 *
 *  Copyright (C) 2000   Michael C. Ring
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
 *      $Id: mapm_pow.c,v 1.1.1.1 2001/10/04 13:02:56 paulm Exp $
 *
 *      This file contains the POW function.
 *
 *      $Log: mapm_pow.c,v $
 *      Revision 1.1.1.1  2001/10/04 13:02:56  paulm
 *      Import of dir
 *
 *      Revision 1.5  2000/08/22 21:22:29  mike
 *      if parameter yy is an integer, call the more
 *      efficient _integer_pow function
 *
 *      Revision 1.4  2000/08/22 20:42:08  mike
 *      compute more digits in the log calculation
 *
 *      Revision 1.3  2000/05/24 20:08:21  mike
 *      update some comments
 *
 *      Revision 1.2  2000/05/23 23:20:11  mike
 *      return 1 when input is 0^0. 
 *
 *      Revision 1.1  2000/05/18 22:10:43  mike
 *      Initial revision
 */

#include "m_apm_lc.h"

static	M_APM   M_last_xx_input;
static	M_APM   M_last_xx_log;
static	int     M_last_log_digits;
static	int     M_size_flag = 0;

/****************************************************************************/
/*
	Calculate the POW function by calling EXP :

                  Y      A                 
                 X   =  e    where A = Y * log(X)
*/
void	m_apm_pow(rr,places,xx,yy)
M_APM	rr, xx, yy;
int	places;
{
int	mexp, iflag, pflag;
char    lbuf[64];
M_APM   tmp8, tmp9;

/* if yy == 0, return 1 */

if (yy->m_apm_sign == 0)
  {
   m_apm_copy(rr, MM_One);
   return;
  }

/* if xx == 0, return 0 */

if (xx->m_apm_sign == 0)
  {
   rr->m_apm_datalength = 1;
   rr->m_apm_sign       = 0;
   rr->m_apm_exponent   = 0;
   rr->m_apm_data[0]    = 0;
   return;
  }

if (M_size_flag == 0)       /* init locals on first call */
  {
   M_size_flag       = M_get_sizeof_int();
   M_last_log_digits = 0;
   M_last_xx_input   = m_apm_init();
   M_last_xx_log     = m_apm_init();
  }

/*
 *  if 'yy' is a small enough integer, call the more
 *  efficient _integer_pow function.
 */

if (m_apm_is_integer(yy))
  {
   iflag = FALSE;

   if (M_size_flag == 2)            /* 16 bit compilers */
     {
      if (yy->m_apm_exponent <= 4)
        iflag = TRUE;
     }
   else                             /* >= 32 bit compilers */
     {
      if (yy->m_apm_exponent <= 6)
        iflag = TRUE;
     }

   if (iflag)
     {
      m_apm_to_integer_string(lbuf, yy);
      mexp = atoi(lbuf);
      m_apm_integer_pow(rr, places, xx, mexp);
      return;
     }
  }

tmp8 = M_get_stack_var();
tmp9 = M_get_stack_var();

/*
 *    If parameter 'X' is the same this call as it 
 *    was the previous call, re-use the saved log 
 *    calculation from last time.
 */

pflag = FALSE;

if (M_last_log_digits >= places)
  {
   if (m_apm_compare(xx, M_last_xx_input) == 0)
     pflag = TRUE;
  }

if (pflag)
  {
   m_apm_round(tmp9, (places + 8), M_last_xx_log);
  }
else
  {
   m_apm_log(tmp9, (places + 8), xx);

   M_last_log_digits = places + 2;

   /* save the 'X' input value and the log calculation */

   m_apm_copy(M_last_xx_input, xx);
   m_apm_copy(M_last_xx_log, tmp9);
  }

m_apm_multiply(tmp8, tmp9, yy);
m_apm_exp(rr, places, tmp8);
M_restore_stack(2);                    /* restore the 2 locals we used here */
}
/****************************************************************************/
