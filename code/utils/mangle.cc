//
// mangle.cc
//
// Implementation of mangle.vdm v1.2
//
// $Id: mangle.cc,v 1.7 2001/10/05 12:24:27 mikhail Exp $
//

#include "mangle_impl.h"
#include "asquotes.h"
#include "unmangle.h"
#include "contextinfo.h"
#include "astaux.h"
#include "intconvquotes.h"
#include "tb_wstring.h"
#include <stdlib.h> // for atoi

Char MANGLE::sepalator (L'-');
//Char MANGLE::sepalator (L'_');

// vdm_MANGLE_Mangle
// p_nm : AS`Name
// p_tps : seq of AS`Type
// -> AS`Name
TYPE_AS_Name vdm_MANGLE_Mangle(const TYPE_AS_Name & p_nm,
                               const SEQ<TYPE_AS_Type> & p_tps)
{
  return MANGLE::Mangle(p_nm, p_tps);
}

// vdm_MANGLE_IsMangled
// p_nm : AS`Name
// -> bool
Bool vdm_MANGLE_IsMangled(const TYPE_AS_Name & p_nm)
{
  return Bool(MANGLE::IsMangled(p_nm));
}

// vdm_MANGLE_GetUnmangledName
// p_nm : AS`Name
// -> AS`Name
TYPE_AS_Name vdm_MANGLE_GetUnmangledName(const TYPE_AS_Name& p_nm)
{
  return MANGLE::GetUnmangledName(p_nm);
}

// Mangle
// p_nm : AS`Name
// p_tps : seq of AS`Type
// -> AS`Name
TYPE_AS_Name MANGLE::Mangle(const TYPE_AS_Name & p_nm,
                            const SEQ<TYPE_AS_Type> & p_tps)
{
  int l_idx (p_nm.GetSequence(pos_AS_Name_ids).Length());
  TYPE_AS_Id l_arity (MkId(Int(p_tps.Length()).ascii()));
  TYPE_AS_Id l_stringRep (stringifyTypeSeq(p_tps, sepalator));

  TYPE_AS_Id l_newId (p_nm.GetSequence(pos_AS_Name_ids)[l_idx]);
  l_newId.ImpAppend(Char(L'#'))
         .ImpConc(l_arity)
         .ImpAppend(Char(L'|'))
         .ImpConc(l_stringRep);

  TYPE_AS_Ids l_newIds (p_nm.get_ids());
  l_newIds.ImpModify(l_idx, l_newId);

  TYPE_AS_Name result (p_nm);
  result.set_ids(l_newIds);
  return result;
}


// MangleString
wstring MANGLE::MangleString(const wstring & p_nm,
                             const SEQ<TYPE_AS_Type> & p_tps) 
{
  TYPE_AS_Name asNm (MkNameFromId(MkId(p_nm), NilContextId));
  TYPE_AS_Name mangledName (Mangle(asNm, p_tps));
  return Id2String(GetFirstId(mangledName));  
}

// UnMangle
// p_nm : AS`Name
// -> AS`Name * nat * seq of AS`Type
Tuple MANGLE::UnMangle(const TYPE_AS_Name& p_nm)
{
  int l_idx = p_nm.GetSequence(pos_AS_Name_ids).Length();
  Sequence l_mangledId (p_nm.GetSequence(pos_AS_Name_ids)[l_idx]);

  wstring l_nm;
  l_mangledId.GetString(l_nm);
  string::size_type l_hashIdx = l_nm.find('#');
  string::size_type l_barIdx = l_nm.find('|',l_hashIdx); // bar must occur after hash
  if (l_hashIdx == wstring::npos ||
      l_barIdx == wstring::npos) // Should never happen
  {
    return mk_(Nil(), Nil(), Nil());
  }
  else
  {
    int l_arity = atoi(TBWSTR::wstring2string(l_nm.substr(l_hashIdx+1, l_barIdx-1)).c_str());
    wstring l_tpString (l_nm.substr(l_barIdx+1, l_nm.length()));
    SEQ<TYPE_AS_Type> l_tps (unmangle_main(l_tpString));
    TYPE_AS_Id l_newId ((Generic)Sequence(l_nm.substr(0, l_hashIdx)));
    TYPE_AS_Name l_resName (p_nm);
    TYPE_AS_Ids l_newIds (l_resName.get_ids());
    l_newIds.ImpModify(l_idx, l_newId);
    l_resName.set_ids(l_newIds);
    return mk_(l_resName, Int(l_arity), l_tps);
  }
}

// IsMangled
// p_nm : AS`Name
// -> bool
bool MANGLE::IsMangled(const TYPE_AS_Name & p_nm)
{
  TYPE_AS_Ids ids (p_nm.get_ids());
  int l_idx = ids.Length();

  return ids[l_idx].Elems().InSet(Char(L'#'));
}

// IsMangled
// p_nm : AS`Name
// -> bool
bool MANGLE::IsMangled(const wstring & p_nm)
{
  return Sequence(p_nm).Elems().InSet(Char(L'#'));
}

// GetUnmangledName
// p_nm : AS`Name
// -> AS`Name
TYPE_AS_Name MANGLE::GetUnmangledName(const TYPE_AS_Name & p_nm)
{
  int l_idx = p_nm.GetSequence(pos_AS_Name_ids).Length();
  TYPE_AS_Id l_mangledId (p_nm.GetSequence(pos_AS_Name_ids)[l_idx]);

  wstring l_nm;
  l_mangledId.GetString(l_nm);
  string::size_type l_hashIdx = l_nm.find('#');

  if (l_hashIdx == wstring::npos) // Should never happen
  {
    return MkNameFromVoid();
  }
  else
  {
    TYPE_AS_Id l_newId (MkId(l_nm.substr(0, l_hashIdx)));
    TYPE_AS_Name l_resName (p_nm);
    TYPE_AS_Ids l_newIds (l_resName.get_ids());
    l_newIds.ImpModify(l_idx, l_newId);
    l_resName.set_ids(l_newIds);
    return l_resName;
  }
}

wstring MANGLE::unmangleStem(const wstring & p_nm)
{
  string::size_type l_hashIdx = p_nm.find('#');
  return p_nm.substr(0, l_hashIdx);
}

Sequence MANGLE::unmangleSeqStem(const Sequence & p_nm)
{
  wstring str;
  p_nm.GetString(str);
  return Sequence(unmangleStem(str));
}

// methtype2seq
// p_tp : AS`OpType | AS`FnType | AS`ParameterTypes
// -> seq of AS`Type
SEQ<TYPE_AS_Type> MANGLE::MethType2Seq (const Generic & p_tp)
{
  if (p_tp.IsRecord()) {
    TYPE_AS_Type tp (p_tp);
    switch(tp.GetTag()) {
      case TAG_TYPE_AS_OpType:
        return tp.GetSequence(pos_AS_OpType_opdom);
      case TAG_TYPE_AS_PartialFnType:
        return tp.GetSequence(pos_AS_PartialFnType_fndom);
      case TAG_TYPE_AS_TotalFnType:
        return tp.GetSequence(pos_AS_TotalFnType_fndom);
      default:
        return SEQ<TYPE_AS_Type>(); // dummy
    }
  }
  else {
    SEQ<TYPE_AS_PatTypePair> p_tpSeq (p_tp);
    SEQ<TYPE_AS_Type> result;
    Generic g;
    for (bool bb = p_tpSeq.First(g); bb; bb = p_tpSeq.Next(g)) {
      TYPE_AS_PatTypePair ptr (g);
      result.ImpAppend(ptr.GetRecord(pos_AS_PatTypePair_tp));
    }
    return result;
  }
}

// stringify
// p_tp : AS`Type
// -> seq of char
SEQ<Char> MANGLE::stringify (const TYPE_AS_Type & p_tp)
{
  switch (p_tp.GetTag()){
    case TAG_TYPE_AS_BasicType: {
      return stringifyBasicType(p_tp);
      break;
    }
    case TAG_TYPE_AS_QuoteType: {
      TYPE_AS_Id result (SEQ<Char>(L"q").ImpAppend(sepalator));
      result.ImpConc(lit2string(p_tp.GetField(pos_AS_QuoteType_lit)));
      return result;
      break;
    }
    case TAG_TYPE_AS_UnionType: {
      TYPE_AS_Id result (SEQ<Char>(L"u("));
      result.ImpConc(stringifyUnionType(p_tp.GetSequence(pos_AS_UnionType_tps)))
            .ImpAppend(Char(L')'));
      return result;
      break;
    }
    case TAG_TYPE_AS_ProductType: {
      TYPE_AS_Id result (SEQ<Char>(L"p("));
      result.ImpConc(stringifyProductType(p_tp.GetSequence(pos_AS_ProductType_tps)))
            .ImpAppend(Char(L')'));
      return result;
      break;
    }
    case TAG_TYPE_AS_OptionalType: {
      TYPE_AS_Id result (SEQ<Char>(L"o").ImpAppend(sepalator));
      result.ImpConc(stringify(p_tp.GetRecord(pos_AS_OptionalType_tp)));
      return result;
      break;
    }
    case TAG_TYPE_AS_SetType: {
      TYPE_AS_Id result (SEQ<Char>(L"t").ImpAppend(sepalator));
      result.ImpConc(stringify(p_tp.GetRecord(pos_AS_SetType_elemtp)));
      return result;
      break;
    }
    case TAG_TYPE_AS_Seq0Type: {
      TYPE_AS_Id result (SEQ<Char>(L"s").ImpAppend(sepalator));
      result.ImpConc(stringify(p_tp.GetRecord(pos_AS_Seq0Type_elemtp)));
      return result;
      break;
    }
    case TAG_TYPE_AS_Seq1Type: {
      TYPE_AS_Id result (SEQ<Char>(L"s").ImpAppend(sepalator));
      result.ImpConc(stringify(p_tp.GetRecord(pos_AS_Seq1Type_elemtp)));
      return result;
      break;
    }
    case TAG_TYPE_AS_GeneralMapType: {
      TYPE_AS_Id result (SEQ<Char>(L"m").ImpAppend(sepalator));
      result.ImpConc(stringify(p_tp.GetRecord(pos_AS_GeneralMapType_mapdom)))
            .ImpAppend(sepalator)
            .ImpConc(stringify(p_tp.GetRecord(pos_AS_GeneralMapType_maprng)));
      return result;
      break;
    }
    case TAG_TYPE_AS_InjectiveMapType: {
      TYPE_AS_Id result (SEQ<Char>(L"m").ImpAppend(sepalator));
      result.ImpConc(stringify(p_tp.GetRecord(pos_AS_InjectiveMapType_mapdom)))
            .ImpAppend(sepalator)
            .ImpConc(stringify(p_tp.GetRecord(pos_AS_InjectiveMapType_maprng)));
      return result;
      break;
    }
    case TAG_TYPE_AS_CompositeType: {
      TYPE_AS_Id result (SEQ<Char>(L"n").ImpAppend(sepalator));
      result.ImpConc(nm2string(p_tp.GetField(pos_AS_CompositeType_name)));
      return result;
      break;
    }
    case TAG_TYPE_AS_TypeName: {
      TYPE_AS_Id result (SEQ<Char>(L"n").ImpAppend(sepalator));
      result.ImpConc(nm2string(p_tp.GetField(pos_AS_TypeName_name)));
      return result;
      break;
    }
    case TAG_TYPE_AS_PartialFnType: {
      return stringifyFnType(p_tp.GetSequence(pos_AS_PartialFnType_fndom),
                             p_tp.GetRecord(pos_AS_PartialFnType_fnrng));
      break;
    }
    case TAG_TYPE_AS_TotalFnType: {
      return stringifyFnType(p_tp.GetSequence(pos_AS_TotalFnType_fndom),
                             p_tp.GetRecord(pos_AS_TotalFnType_fnrng));
      break;
    }
    case TAG_TYPE_AS_BracketedType: {
      SEQ<Char> result (SEQ<Char>(L"b").ImpAppend(sepalator));
      result.ImpConc(stringify(p_tp.GetRecord(pos_AS_BracketedType_tp)));
      return result;
      break;
    }
    case TAG_TYPE_AS_TypeVar: {
      SEQ<Char> result (SEQ<Char>(L"v").ImpAppend(sepalator));
      result.ImpConc(nm2string(p_tp.GetField(1)));
      return result;
      break;
    }
    default: {
      return SEQ<Char>(L"others");
      break;
    }
  }
}

// stringifyBasicType
// p_tp : AS`BasicType
// -> seq of char
SEQ<Char> MANGLE::stringifyBasicType (const TYPE_AS_BasicType & p_tp)
{
  Generic bt (p_tp.GetField(1));
  Int btI;
  if (bt.IsInt())
    btI = bt;
  else
    btI = INT2Q::Quote2Integer(bt);

  switch (btI.GetValue()) {
    case AS_BOOLEAN: {
      return SEQ<Char>(L"B");
      break;
    }
    case NATONE: {
      return SEQ<Char>(L"M");
      break;
    }
    case NAT: {
      return SEQ<Char>(L"N");
      break;
    }
    case INTEGER: {
      return SEQ<Char>(L"I");
      break;
    }
    case RAT: {
      return SEQ<Char>(L"Q");
      break;
    }
    case REAL: {
      return SEQ<Char>(L"R");
      break;
    }
    case AS_TOKEN: {
      return SEQ<Char>(L"T");
      break;
    }
    case AS_CHAR: {
      return SEQ<Char>(L"C");
      break;
    }
    default: {
      return SEQ<Char>(L"?");
      break;
    }
  }
}

// stringifyUnionType
// p_tps : seq of AS`Type
// -> seq of char
SEQ<Char> MANGLE::stringifyUnionType (const SEQ<TYPE_AS_Type> & p_tps)
{
  return stringifyTypeSeq(p_tps, sepalator);
}

// stringifyProductType
// p_tps : seq of AS`Type
// -> seq of char
SEQ<Char> MANGLE::stringifyProductType (const SEQ<TYPE_AS_Type> & p_tps)
{
  return stringifyTypeSeq(p_tps, sepalator);
}

// stringifyFnType
// p_fd : (AS`DiscretionaryType | AS`AllType) * AS`Type
// -> seq of char
SEQ<Char> MANGLE::stringifyFnType (const SEQ<TYPE_AS_Type> & p_fd,
                                    const TYPE_AS_Type& p_fr)
{
//  SEQ<Char> result (L"p(f(");
//  SEQ<Char> result (L"f(p(");
//  result.ImpConc(stringifyTypeSeq(p_fd, sepalator))
//        .ImpConc(SEQ<Char>(L"))"))
//        .ImpConc(stringify(p_fr));
  SEQ<Char> result (L"f-p(");
  result.ImpConc(stringifyTypeSeq(p_fd, sepalator))
        .ImpConc(SEQ<Char>(L")-"))
        .ImpConc(stringify(p_fr));
  return result; 
}

// nm2string
// p_nm : AS`Name
// -> seq of char
SEQ<Char> MANGLE::nm2string (const TYPE_AS_Name& p_nm)
{
  TYPE_AS_Ids ids (p_nm.get_ids());
  SEQ<Char> result (ids[1]);
  if (ids.Length() == 2){
    result.ImpAppend(Char(L'`'))
          .ImpConc(ids[2]);
  }
  return result;
}

// lit2string
// p_nm : AS`QuoteLit
// -> seq of char
SEQ<Char> MANGLE::lit2string (const TYPE_AS_QuoteLit& p_nm)
{
  return p_nm.get_val();
}

// stringifyTypeSeq
// p_tps : seq of AS`Type
// p_sep : char
// -> seq of char
SEQ<Char> MANGLE::stringifyTypeSeq (const SEQ<TYPE_AS_Type> & p_tps,
                                     const Char & p_sep)
{
  if (p_tps.IsEmpty())
    return SEQ<Char>();
  else {
    SEQ<Char> result (stringify(p_tps.Hd()));
    if (p_tps.Length() != 1) {
      result.ImpAppend(p_sep)
            .ImpConc(stringifyTypeSeq(p_tps.Tl(), p_sep));
    }
    return result;
  }
}

