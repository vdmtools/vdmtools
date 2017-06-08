/***
*  * WHAT
*  *    Code generator auxiliary functions which are 
*  *    dependent of the VDM C++ Library (libvdm.a)
*  * ID
*  *    $Id: cg_aux.h,v 1.14 2005/05/27 00:21:34 vdmtools Exp $
*  * PROJECT
*  *    Toolbox
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __cg_aux_h__
#define __cg_aux_h__

#include <math.h>
#include "metaiv.h"

Set Permute(const Sequence &);
Sequence Sort(const Set &);
bool IsInteger(const Generic &);
//Set GenAllComb(const Sequence&);

#endif // __cg_aux_h__
