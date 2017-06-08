
/* 
 *  M_APM  -  mapmistr.c
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
 *      $Id: MAPMISTR.C,v 1.1.1.1 2001/10/04 13:02:57 paulm Exp $
 *
 *      This file contains M_APM -> integer string function
 *
 *      $Log: MAPMISTR.C,v $
 *      Revision 1.1.1.1  2001/10/04 13:02:57  paulm
 *      Import of dir
 *
 *      Revision 1.3  2000/02/03 22:48:38  mike
 *      use MAPM_* generic memory function
 *
 *      Revision 1.2  1999/07/18 01:33:04  mike
 *      minor tweak to code alignment
 *
 *      Revision 1.1  1999/07/12 02:06:08  mike
 *      Initial revision
 */

#include "m_apm_lc.h"

/****************************************************************************/
void	m_apm_to_integer_string(s,mtmp)
char	*s;
M_APM	mtmp;
{
void    *vp;
char	*cp, *cpdest, ch, lbuf[256];
int	ct;

vp = NULL;
ct = mtmp->m_apm_exponent;

if (ct <= 0 || mtmp->m_apm_sign == 0)
  {
   strcpy(s, "0");
   return;
  }

if (ct > 240)
  {
   if ((vp = (void *)MAPM_MALLOC((ct + 32) * sizeof(char))) == NULL)
     {
      fprintf(stderr,"\'m_apm_to_integer_string\', Out of memory\n");
      exit(16);
     }

   cp = (char *)vp;
  }
else
  {
   cp = lbuf;
  }

m_apm_to_string(cp, (ct + 2), mtmp);

cpdest = s;
*cpdest++ = *cp++;

if (mtmp->m_apm_sign == 1)
  ct--;

while (TRUE)
  {
   if (ct == 0)
     break;

   if ((ch = *cp++) != '.')
     {
      *cpdest++ = ch;
      ct--;
     }
  }

*cpdest = '\0';

if (vp != NULL)
  MAPM_FREE(vp);
}
/****************************************************************************/
