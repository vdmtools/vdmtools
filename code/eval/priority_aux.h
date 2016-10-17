/***
*  * WHAT
*  *    headers for auxilliary functions defined in priority.y (VICE)
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/eval/priority_aux.h,v $
*  * VERSION
*  *    $Revision: 1.3 $
*  * DATE
*  *    $Date: 2005/01/21 03:16:06 $
*  * STATUS
*  *    Under development
*  * REFERENCES
*  *    
*  * PROJECT
*  *    Toolbox
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef _priority_aux_h
#define _priority_aux_h

#include <iostream>
#include "metaiv.h"
#include "SCHDTP.h"

SEQ<TYPE_SCHDTP_PriorityEntry> pf_main(wostream & os, const wstring & fname);

#endif //_priority_aux_h
