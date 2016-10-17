/***
*  * WHAT
*  *    Code generator auxiliary functions which are 
*  *    dependent of the VDM C++ Library (libvdm.a)
*  * ID
*  *    $Id: cg_aux.h,v 1.14 2005/05/27 00:21:34 vdmtools Exp $
*  * PROJECT
*  *    Toolbox
*  * COPYRIGHT
*  *    (C) 2005, CSK
***/

#ifndef _cg_aux_h
#define _cg_aux_h

#include <math.h>
#include "metaiv.h"

#define TOKEN -3


Set Permute(const Sequence&);
Sequence Sort(const Set&);
bool IsInteger(const Generic&);
Set GenAllComb(const Sequence&);

#endif
