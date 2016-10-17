/***
*  * WHAT
*  *    Header file for VDM-SL/VDM++ position information
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/lib/position.h,v $
*  * VERSION
*  *    $Revision: 1.6 $
*  * DATE
*  *    $Date: 1999/09/07 07:11:21 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    VDM-SL Toolbox / Afrodite
*  * STATUS
*  *    Under development.
*  * AUTHOR
*  *    Henrik Voss + $Author: hanne $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/


#ifndef __position_h__
#define __position_h__

#include "metaiv.h"
#include "contextinfo.h"

class POSITION
{
public:
  static void SetPosition(const Record &);
  static void SetPosition(const TYPE_CI_ContextId &);
  static TYPE_CI_FileId GetErrFile();
  static TYPE_CI_Line GetErrLine();
  static TYPE_CI_Column GetErrCol();
  static int GetLength();

private:
  static TYPE_CI_ContextId err_cid;

  static Tuple GetPos();
};

#endif //__position_h__

