
/* 
 *  M_APM  -  mapmgues.c
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
 *      $Id: mapmgues.c,v 1.1.1.1 2001/10/04 13:02:56 paulm Exp $
 *
 *      This file contains the functions that generate the initial 
 *	'guesses' for the sqrt, log, arcsin, and arccos functions.
 *
 *      $Log: mapmgues.c,v $
 *      Revision 1.1.1.1  2001/10/04 13:02:56  paulm
 *      Import of dir
 *
 *      Revision 1.8  2000/04/10 21:13:13  mike
 *      minor tweaks to log_guess
 *
 *      Revision 1.7  2000/04/03 17:25:45  mike
 *      added function to estimate the cube root (cbrt)
 *
 *      Revision 1.6  1999/07/18 01:57:35  mike
 *      adjust arc-sin guess for small exponents
 *
 *      Revision 1.5  1999/07/09 22:32:50  mike
 *      optimize some functions
 *
 *      Revision 1.4  1999/05/12 21:22:27  mike
 *      add more comments
 *
 *      Revision 1.3  1999/05/12 21:00:51  mike
 *      added new sqrt guess function
 *
 *      Revision 1.2  1999/05/11 02:10:12  mike
 *      added some comments
 *
 *      Revision 1.1  1999/05/10 20:56:31  mike
 *      Initial revision
 */

#include "m_apm_lc.h"
#include <math.h>

/****************************************************************************/
void	M_get_sqrt_guess(r,a)
M_APM	r, a;
{
char	buf[48];
double  dd;

m_apm_to_string(buf, 15, a);
dd = atof(buf);
m_apm_set_double(r, sqrt(dd));
}
/****************************************************************************/
void	M_get_cbrt_guess(r,a)
M_APM	r, a;
{
char	buf[48];
double  dd;

m_apm_to_string(buf, 15, a);
dd = atof(buf);
dd = log(dd) / 3.0;
m_apm_set_double(r, exp(dd));
}
/****************************************************************************/
void	M_get_log_guess(r,a)
M_APM	r, a;
{
char	*cp, buf[48];
int     k, iexp;
double  dd;

m_apm_to_string(buf, 15, a);

M_lowercase(buf);
k = M_strposition(buf,"e");
if (k >= 0)
  {
   /*
          use log (x * y) == log(x) + log(y)

	  we need to split up the estimate since we 
	  may have an exponent beyond C's normal range :
	  (> +308 or < -308)

	  for example, we may need to estimate log(7.52E-951)
   */

   cp   = buf + k;
   iexp = atoi(cp + 1);
   *cp  = '\0';
   dd   = log(atof(buf));
   dd  += (double)iexp * 2.3025850929940457;

   m_apm_set_double(r, dd);
  }
else
  {
   dd = log(atof(buf));
   m_apm_set_double(r, dd);
  }
}
/****************************************************************************/
void	M_get_asin_guess(r,a)
M_APM	r, a;
{
char	buf[48];
double  dd;

if (a->m_apm_exponent > -10)
  {
   m_apm_to_string(buf, 15, a);
   dd = atof(buf);
   m_apm_set_double(r, asin(dd));
  }
else
  {
   m_apm_copy(r,a);
  }
}
/****************************************************************************/
void	M_get_acos_guess(r,a)
M_APM	r, a;
{
char	buf[48];
double  dd;

m_apm_to_string(buf, 15, a);
dd = atof(buf);
m_apm_set_double(r, acos(dd));
}
/****************************************************************************/
/*
	convert a C 'double' into an M_APM value. 
*/
void	m_apm_set_double(atmp,dd)
M_APM	atmp;
double	dd;
{
char	*cp, *p, *ps, buf[64];

if (dd == 0.0)                     /* special case for 0 exactly */
   m_apm_set_string(atmp,"0");
else
  {
   sprintf(buf,"%.14E",dd);
   
   cp = strstr(buf,"E");
   if (atoi(cp + sizeof(char)) == 0)
     *cp = '\0';
   
   p = cp;
   
   while (TRUE)
     {
      p--;
      if (*p == '0' || *p == '.')
        *p = ' ';
      else
        break;
     }
   
   ps = buf;
   p  = buf;
   
   while (TRUE)
     {
      if ((*p = *ps) == '\0')
        break;
   
      if (*ps++ != ' ')
        p++;
     }

   m_apm_set_string(atmp,buf);
  }
}
/****************************************************************************/
