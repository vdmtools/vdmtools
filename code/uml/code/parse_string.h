/***
*  * WHAT
*  *    Functions for parsing strings representing parts of a VDM[SL|++]
*  *    specification. Currently functions exists for parsing expressions,
*  *    typedefinitions and patterns.
*  *    
*  *    
*  **  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/uml/code/parse_string.h,v $
*  * VERSION
*  *    $Revision: 1.6 $
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

#include "contextinfo.h"
#include "metaiv.h"
#include "AS.h"

class UMLPARSE
{
public:
  static bool str2type(const wstring & s, TYPE_AS_Type& type);
  static bool str2expr(const wstring & s, TYPE_AS_Expr& pat);
  static bool str2pat(const wstring & s, TYPE_AS_Pattern& pat);

  static Tuple Seq2Type(const Sequence & s);
  static Tuple Seq2Expr(const Sequence & s);
  static Tuple Seq2Pat(const Sequence & s);

  static bool parse_identifier(const wstring & s);
  static bool parse_expr(const wstring & ws, Record& res);

  static bool ParseIdentifierSeq(const Sequence & s);
};
