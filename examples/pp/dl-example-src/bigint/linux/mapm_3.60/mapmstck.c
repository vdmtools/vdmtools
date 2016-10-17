
/* 
 *  M_APM  -  mapmstck.c
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
 *      $Id: mapmstck.c,v 1.1.1.1 2001/10/04 13:02:56 paulm Exp $
 *
 *      This file contains the stack implementation for using 
 *	local M_APM variables.
 *
 *      $Log: mapmstck.c,v $
 *      Revision 1.1.1.1  2001/10/04 13:02:56  paulm
 *      Import of dir
 *
 *      Revision 1.4  1999/07/09 00:04:47  mike
 *      tweak stack again
 *
 *      Revision 1.3  1999/07/09 00:02:24  mike
 *      increase stack size for new functions
 *
 *      Revision 1.2  1999/06/20 21:13:18  mike
 *      comment out printf debug and set max stack depth
 *
 *      Revision 1.1  1999/06/19 20:32:43  mike
 *      Initial revision
 */

#include "m_apm_lc.h"

static	int	M_stack_ptr = -1;
static	int	M_last_init = -1;

static	M_APM	M_stack_array[32];

/****************************************************************************/
M_APM	M_get_stack_var()
{
if (++M_stack_ptr > M_last_init)
  {
   M_stack_array[M_stack_ptr]     = m_apm_init();
   M_stack_array[M_stack_ptr + 1] = m_apm_init();
   M_stack_array[M_stack_ptr + 2] = m_apm_init();

   M_last_init = M_stack_ptr + 2;

   /* lib v1.20 current max depth reached is 26  */
   /* printf("M_last_init = %d \n",M_last_init); */
  }

return(M_stack_array[M_stack_ptr]);
}
/****************************************************************************/
void	M_restore_stack(count)
int	count;
{
M_stack_ptr -= count;
}
/****************************************************************************/
