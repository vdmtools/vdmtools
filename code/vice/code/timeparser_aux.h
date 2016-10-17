/***
*  * WHAT
*  *    headers for auxilliary functions defined in timeparser.y (VICE)
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/vice/code/timeparser_aux.h,v $
*  * VERSION
*  *    $Revision: 1.5 $
*  * DATE
*  *    $Date: 2005/11/28 08:46:24 $
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

#ifndef _timeparser_aux_h
#define _timeparser_aux_h

#include <iostream>
#include "TIMEMAP.h"
#include "metaiv.h"

bool tp_main(wostream*, const wchar_t*,Set&,TYPE_TIMEMAP_Timemap&);
void tp_OutputErrors(wostream&,Set);

#endif // _timeparser_aux_h
