/***
*  * WHAT
*  *  
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/java2vdm++/JAVAERR_userdef.h,v $
*  * VERSION
*  *    $Revision: 1.2 $
*  * DATE
*  *    $Date: 2005/01/19 05:01:29 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    VDM-SL Toolbox.
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __JAVAERR_userdef_h__
#define __JAVAERR_userdef_h__

#include "main_tag.h"
#include "metaiv.h"

#define DEF_JAVAERR_USERIMPL 1
#define DEF_JAVAERR_GenErr 1

Sequence GetJavaErrors();
void ClearJavaErrors();
int GetNumJavaErrs();
int GetNumJavaWarnings();

#endif // __JAVAERR_userdef_h__
