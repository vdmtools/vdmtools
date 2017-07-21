/***
*  * WHAT
*  *    DL Module functions.
*  * FILE
*  *    $Id: evaldl.h,v 1.8 2006/05/23 04:53:01 vdmtools Exp $
*  * PROJECT
*  *    VDM-SL Toolbox
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __evaldl_h__
#define __evaldl_h__

#include "semvalues.h"

struct DLRecInfoData; 

struct DLRecInfoData {
  VDMRecInfoMap * namemap; 
  vdmdlfunctions functions; 
  DLRecInfoData() : namemap(0) { }
}; 

#endif // __evaldl_h__
