
/* 
 *  M_APM  -  mapmhsin.c
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
 *      $Id: mapmhsin.c,v 1.1.1.1 2001/10/04 13:02:56 paulm Exp $
 *
 *      This file contains the Hyperbolic SIN, COS, & TAN functions.
 *
 *      $Log: mapmhsin.c,v $
 *      Revision 1.1.1.1  2001/10/04 13:02:56  paulm
 *      Import of dir
 *
 *      Revision 1.1  2000/04/03 18:16:26  mike
 *      Initial revision
 *
 */

#include "m_apm_lc.h"

/****************************************************************************/
/*
 *      sinh(x) == 0.5 * [ exp(x) - exp(-x) ]
 */
void	m_apm_sinh(rr, places, aa)
M_APM   rr, aa;
int     places;
{
M_APM	tmp1, tmp2, tmp3;
int     local_precision;

tmp1 = M_get_stack_var();
tmp2 = M_get_stack_var();
tmp3 = M_get_stack_var();

local_precision = places + 4;

m_apm_exp(tmp1, local_precision, aa);
m_apm_divide(tmp2, local_precision, MM_One, tmp1);
m_apm_subtract(tmp3, tmp1, tmp2);
m_apm_multiply(tmp1, tmp3, MM_0_5);
m_apm_round(rr, places, tmp1);

M_restore_stack(3);
}
/****************************************************************************/
/*
 *      cosh(x) == 0.5 * [ exp(x) + exp(-x) ]
 */
void	m_apm_cosh(rr, places, aa)
M_APM   rr, aa;
int     places;
{
M_APM	tmp1, tmp2, tmp3;
int     local_precision;

tmp1 = M_get_stack_var();
tmp2 = M_get_stack_var();
tmp3 = M_get_stack_var();

local_precision = places + 4;

m_apm_exp(tmp1, local_precision, aa);
m_apm_divide(tmp2, local_precision, MM_One, tmp1);
m_apm_add(tmp3, tmp1, tmp2);
m_apm_multiply(tmp1, tmp3, MM_0_5);
m_apm_round(rr, places, tmp1);

M_restore_stack(3);
}
/****************************************************************************/
/*
 *      tanh(x) == [ exp(x) - exp(-x) ]  /  [ exp(x) + exp(-x) ]
 */
void	m_apm_tanh(rr, places, aa)
M_APM   rr, aa;
int     places;
{
M_APM	tmp1, tmp2, tmp3, tmp4;
int     local_precision;

tmp1 = M_get_stack_var();
tmp2 = M_get_stack_var();
tmp3 = M_get_stack_var();
tmp4 = M_get_stack_var();

local_precision = places + 4;

m_apm_exp(tmp1, local_precision, aa);
m_apm_divide(tmp2, local_precision, MM_One, tmp1);
m_apm_subtract(tmp3, tmp1, tmp2);
m_apm_add(tmp4, tmp1, tmp2);
m_apm_divide(tmp1, local_precision, tmp3, tmp4);
m_apm_round(rr, places, tmp1);

M_restore_stack(4);
}
/****************************************************************************/
