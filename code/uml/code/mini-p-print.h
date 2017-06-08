/***
*  * WHAT
*  *    This file describes a simple ASCII pretty-printer for the following
*  *    types in the AS module: AS`Type, AS`Pattern, AS`Expr (only simple
*  *    expressions).
*  *
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/uml/code/mini-p-print.h,v $
*  * VERSION
*  *    $Revision: 1.10 $
*  * DATE
*  *    $Date: 2001/06/12 15:04:58 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    VDM-SL Toolbox.
*  * AUTHOR
*  *    $Author: paulm $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __mini_p_print_h__
#define __mini_p_print_h__

#include "metaiv.h"
#include "AS.h"

class MPP
{
public:
  static wstring MiniPP(const Generic & g);
  static wstring ListSetOfSeq(const Set & lst);

private:
  static wstring Seq2Ascii(const Sequence & s);
  static wstring RecordModList2Ascii(const Sequence & l);
  static wstring MapletList2Ascii(const Sequence & l);
  static wstring BindList2Ascii(const Sequence & l);
  static wstring LocalDef2Ascii(const Sequence & def_l);
  static wstring PatternList2Ascii(const Sequence & pat_l);
  static wstring List2Ascii(const Sequence & l_);
  static wstring ExprList2Ascii(const Sequence & exp_l);
  static wstring PrintNameCharSeq (const TYPE_AS_Name & rc);
  static wstring Type2Ascii (const Record & tp);
};

#endif // __mini_p_print_h__
