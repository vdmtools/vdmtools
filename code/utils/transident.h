/***
*  * WHAT
*  *    Header file for transident.cc
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/utils/transident.h,v $
*  * VERSION
*  *    $Revision: 1.4 $
*  * DATE
*  *    $Date: 2005/01/21 03:16:06 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    Informa
*  * STATUS
*  *    Under development.
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef transident_h_
#define transident_h_

#include <fstream>
#include <stdlib.h>
#include "metaiv.h"
#include "tag.h"

Generic TransformIdentifiers (Generic g); // from L"Token" to "seq of wchar_t"
Generic TransformIdentifiersBack (Generic g); // form L"seq of wchar_t" to "Token"

#endif
