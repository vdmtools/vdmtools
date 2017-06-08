// mangle.h
// exported interface for mangle.cc
//
// $Id: mangle_impl.h,v 1.2 2001/06/12 15:05:00 paulm Exp $
//

#ifndef __mangle_h__
#define __mangle_h__

#include "metaiv.h"
#include "AS.h"

class MANGLE
{
private:
  static Char sepalator;
public:
  static TYPE_AS_Name Mangle (const TYPE_AS_Name &,
                              const SEQ<TYPE_AS_Type> &);
  static Tuple UnMangle (const TYPE_AS_Name &);
  static bool IsMangled(const TYPE_AS_Name &);
  static TYPE_AS_Name GetUnmangledName(const TYPE_AS_Name &);
  static SEQ<TYPE_AS_Type> MethType2Seq (const Generic & p_tp);

  // for UML interface
  static wstring MangleString(const wstring & p_nm,
                              const SEQ<TYPE_AS_Type> & p_tps);
  static bool IsMangled(const wstring & p_nm);
  static wstring unmangleStem(const wstring &);
  static Sequence unmangleSeqStem(const Sequence & p_nm);
private:

  static SEQ<Char> stringify (const TYPE_AS_Type & p_tp);
  static SEQ<Char> stringifyBasicType (const TYPE_AS_BasicType &);
  static SEQ<Char> stringifyUnionType (const SEQ<TYPE_AS_Type> &);
  static SEQ<Char> stringifyProductType (const SEQ<TYPE_AS_Type> &);
  static SEQ<Char> stringifyFnType (const SEQ<TYPE_AS_Type> &,
                                    const TYPE_AS_Type &);
  static SEQ<Char> nm2string (const TYPE_AS_Name &);
  static SEQ<Char> lit2string (const TYPE_AS_QuoteLit &);
  static SEQ<Char> stringifyTypeSeq (const SEQ<TYPE_AS_Type> &,
                                     const Char &);
};

#endif //__mangle_h__
