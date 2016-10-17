/***
*  * WHAT
*  *  
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/utils/UTIL_userdef.h,v $
*  * VERSION
*  *    $Revision: 1.3 $
*  * DATE
*  *    $Date: 2005/01/19 05:01:25 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    VDM-SL Toolbox.
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __UTIL_userdef_h__
#define __UTIL_userdef_h__

#include "main_tag.h"
#include "AS.h"

#define DEF_UTIL_USERIMPL 1

#define DEF_UTIL_ReplacePercent 1
#define DEF_UTIL_itoa 1
#define DEF_UTIL_minset 1

class UTIL
{
public:
  static Sequence set2seq(const Set &);
  static Real minset(const SET<Real> &);
  static SEQ<Char> itoa(const Int & n);
  static SEQ<Char> ReplacePercent(const SEQ<Char> &, const SEQ< SEQ<Char> > &);
  static Int String2Num(const SEQ<Char> &);
  //static SEQ< SEQ<Char> > split(const SEQ<Char> &, char);
  static Sequence split(const Sequence &, const Common &);
};

#endif // __UTIL_userdef_h__
