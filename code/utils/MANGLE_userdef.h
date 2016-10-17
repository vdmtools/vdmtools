/***
*  * WHAT
*  *  
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/utils/MANGLE_userdef.h,v $
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

#ifndef __MANGLE_userdef_h__
#define __MANGLE_userdef_h__

#include "main_tag.h"
#include "AS.h"

#define DEF_MANGLE_USERIMPL 1

#define DEF_MANGLE_Mangle 1
#define DEF_MANGLE_IsMangled 1
#define DEF_MANGLE_GetUnmangledName 1

#define TYPE_MANGLE_MangleToken Generic

class MANGLE
{
private:
  static Char sepalator;
public:
  static TYPE_AS_Name Mangle (const TYPE_AS_Name &, const SEQ<TYPE_AS_Type> &);
  static Tuple UnMangle (const TYPE_AS_Name &);
  static bool IsMangled(const TYPE_AS_Name &);
  static TYPE_AS_Name GetUnmangledName(const TYPE_AS_Name &);
  static SEQ<TYPE_AS_Type> MethType2Seq (const Generic & p_tp);

  // for UML interface
  static wstring MangleString(const wstring & p_nm, const SEQ<TYPE_AS_Type> & p_tps);
  static bool IsMangled(const wstring & p_nm);
  static wstring unmangleStem(const wstring &);
  static Sequence unmangleSeqStem(const Sequence & p_nm);
private:
  static Tuple UnMangleTypeSeq(const wstring & p_str);

  static SEQ<Char> stringify (const TYPE_AS_Type & p_tp);
//  static SEQ<Char> stringifyBasicType (const TYPE_AS_BasicType &);
  static SEQ<Char> stringifyNumericType (const TYPE_AS_NumericType &);
  static SEQ<Char> stringifyUnionType (const SEQ<TYPE_AS_Type> &);
  static SEQ<Char> stringifyProductType (const SEQ<TYPE_AS_Type> &);
  static SEQ<Char> stringifyFnType (const SEQ<TYPE_AS_Type> &, const TYPE_AS_Type &);
  static SEQ<Char> nm2string (const TYPE_AS_Name &);
  static SEQ<Char> lit2string (const TYPE_AS_QuoteLit &);
  static SEQ<Char> stringifyTypeSeq (const SEQ<TYPE_AS_Type> &, const Char &);

  static SEQ<TYPE_AS_Type> unstringifyTypeSeq (const SEQ<Char> & p_str);
  static SEQ<TYPE_MANGLE_MangleToken> LexMangle(const SEQ<Char> & p_str);
  static SEQ<TYPE_AS_Type> ParseMangle(const SEQ<TYPE_MANGLE_MangleToken> & p_mantokL);
  static Tuple ExtractParameterList(const SEQ<TYPE_MANGLE_MangleToken> p_tokL, int p_start);
  static Tuple ExtractType(const SEQ<TYPE_MANGLE_MangleToken> p_tokL);
};

class UNMANGLE
{
public:
  static SEQ<TYPE_AS_Type> unmangle_main(const wstring & nm);
  static int unmangleLex();
  static wstring bslashUs2wstring(const string & str);

private:
  static bool is_sep(wchar_t c);
  static int hexDigit2int(char c);

public:
  static string textLit;
  static SEQ<TYPE_AS_Type> unmangledType;

private:
  static string inputStr;
  static int inputLen;
  static int currentPos;
  static const int sepalator;
  static bool maybetypename;
};
#endif // __MANGLE_userdef_h__

