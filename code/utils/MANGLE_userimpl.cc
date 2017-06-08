//
// mangle.cc
//
// Implementation of mangle.vdm v1.2
//
// $Id: mangle.cc,v 1.7 2001/10/05 12:24:27 mikhail Exp $
//

#include "MANGLE.h"
#include "MANGLE_anonym.h"
#include "asquotes.h"
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
TYPE_AS_Name vdm_MANGLE_Mangle(const TYPE_AS_Name & p_nm, const type_dL & p_tps)
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
TYPE_AS_Name MANGLE::Mangle(const TYPE_AS_Name & p_nm, const SEQ<TYPE_AS_Type> & p_tps)
{
  int l_idx (p_nm.GetSequence(pos_AS_Name_ids).Length());
  TYPE_AS_Id l_arity (ASTAUX::MkId(Int(p_tps.Length()).ascii()));
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
wstring MANGLE::MangleString(const wstring & p_nm, const SEQ<TYPE_AS_Type> & p_tps) 
{
  TYPE_AS_Name asNm (ASTAUX::MkNameFromId(ASTAUX::MkId(p_nm), NilContextId));
  TYPE_AS_Name mangledName (Mangle(asNm, p_tps));
  return ASTAUX::Id2String(ASTAUX::GetFirstId(mangledName));  
}

// UnMangle
// p_nm : AS`Name
// -> AS`Name * nat * seq of AS`Type
Tuple MANGLE::UnMangle(const TYPE_AS_Name& p_nm)
{
  int l_idx = p_nm.GetSequence(pos_AS_Name_ids).Length();
  SEQ<Char> l_mangledId (p_nm.GetSequence(pos_AS_Name_ids)[l_idx]);

  wstring l_nm;
  l_mangledId.GetString(l_nm);
  string::size_type l_hashIdx = l_nm.find('#');
  string::size_type l_barIdx = l_nm.find('|',l_hashIdx); // bar must occur after hash
  if (l_hashIdx == wstring::npos || l_barIdx == wstring::npos) // Should never happen
  {
    return mk_(Nil(), Nil(), Nil());
  }
  else
  {
    Tuple infer (UnMangleTypeSeq(l_nm));

    TYPE_AS_Name l_resName (p_nm);
    TYPE_AS_Ids ids (l_resName.get_ids());
    int l_idx = ids.Length();
    ids.ImpModify(l_idx, infer.GetSequence(1));
    l_resName.set_ids(ids);
    return mk_(l_resName, infer.GetInt(2), infer.GetSequence(3));
  }
}

// UnMangleTypeSeq
// p_str : seq of char
// -> seq of char * nat * seq of AS`Type
Tuple MANGLE::UnMangleTypeSeq(const wstring & p_str)
{
  string::size_type l_idx = p_str.find('#');
  string::size_type l_baridx = p_str.find('|',l_idx); // bar must occur after hash
  int l_arity = atoi(TBWSTR::wstring2string(p_str.substr(l_idx + 1, l_baridx - 1)).c_str());
  wstring l_tpString (p_str.substr(l_baridx + 1, p_str.length()));

  //SEQ<TYPE_AS_Type> l_tps (UNMANGLE::unmangle_main(l_tpString)); // if fail then leak memory

  SEQ<TYPE_AS_Type> l_tps (unstringifyTypeSeq(SEQ<Char>(l_tpString)));

  TYPE_AS_Id id (SEQ<Char>(p_str.substr(0, l_idx)));
  return mk_(id, Int(l_arity), l_tps);
}

// IsMangled
// p_nm : AS`Name
// -> bool
bool MANGLE::IsMangled(const TYPE_AS_Name & p_nm)
{
  const TYPE_AS_Ids & ids (p_nm.GetSequence(pos_AS_Name_ids));
  int l_idx = ids.Length();
//  return ids[l_idx].Elems().InSet(Char(L'#'));
  wstring nm (ids[l_idx].GetString());
  return (nm.find('#') != wstring::npos);
}

// IsMangled
// p_nm : AS`Name
// -> bool
bool MANGLE::IsMangled(const wstring & p_nm)
{
//  return Sequence(p_nm).Elems().InSet(Char(L'#'));
  return (p_nm.find('#') != wstring::npos);
}

// GetUnmangledName
// p_nm : AS`Name
// -> AS`Name
TYPE_AS_Name MANGLE::GetUnmangledName(const TYPE_AS_Name & p_nm)
{
  const TYPE_AS_Ids & ids (p_nm.GetSequence(pos_AS_Name_ids));
  int l_idx = ids.Length();
  const TYPE_AS_Id & l_mangledId (ids[l_idx]);

  wstring l_nm;
  l_mangledId.GetString(l_nm);
  string::size_type l_hashIdx = l_nm.find('#');

  if (l_hashIdx == wstring::npos) // Should never happen
  {
    return ASTAUX::MkNameFromVoid();
  }
  else
  {
    TYPE_AS_Id l_newId (ASTAUX::MkId(l_nm.substr(0, l_hashIdx)));
    TYPE_AS_Ids l_newIds (ids);
    l_newIds.ImpModify(l_idx, l_newId);
    TYPE_AS_Name l_resName (p_nm);
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
    size_t len_p_tpSeq = p_tpSeq.Length();
    for (size_t idx = 1; idx <= len_p_tpSeq; idx++)
    {
      const TYPE_AS_PatTypePair & ptr (p_tpSeq[idx]);
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
  switch (p_tp.GetTag()) {
    case TAG_TYPE_AS_BooleanType: {
      return SEQ<Char>(L"B");
    }
    case TAG_TYPE_AS_NumericType: {
      return stringifyNumericType(p_tp);
    }
    case TAG_TYPE_AS_TokenType: {
      return SEQ<Char>(L"T");
    }
    case TAG_TYPE_AS_CharType: {
      return SEQ<Char>(L"C");
    }
    case TAG_TYPE_AS_QuoteType: {
      TYPE_AS_Id result (SEQ<Char>(L"q").ImpAppend(sepalator));
      result.ImpConc(lit2string(p_tp.GetRecord(pos_AS_QuoteType_lit)));
      return result;
    }
    case TAG_TYPE_AS_UnionType: {
      return stringifyUnionType(p_tp.GetSequence(pos_AS_UnionType_tps));
    }
    case TAG_TYPE_AS_ProductType: {
      return stringifyUnionType(p_tp.GetSequence(pos_AS_ProductType_tps));
    }
    case TAG_TYPE_AS_OptionalType: {
      TYPE_AS_Id result (SEQ<Char>(L"o").ImpAppend(sepalator));
      result.ImpConc(stringify(p_tp.GetRecord(pos_AS_OptionalType_tp)));
      return result;
    }
    case TAG_TYPE_AS_Set0Type: {
      TYPE_AS_Id result (SEQ<Char>(L"t").ImpAppend(sepalator));
      result.ImpConc(stringify(p_tp.GetRecord(pos_AS_Set0Type_elemtp)));
      return result;
    }
    case TAG_TYPE_AS_Set1Type: {
      TYPE_AS_Id result (SEQ<Char>(L"t").ImpAppend(sepalator));
      result.ImpConc(stringify(p_tp.GetRecord(pos_AS_Set1Type_elemtp)));
      return result;
    }
    case TAG_TYPE_AS_Seq0Type: {
      TYPE_AS_Id result (SEQ<Char>(L"s").ImpAppend(sepalator));
      result.ImpConc(stringify(p_tp.GetRecord(pos_AS_Seq0Type_elemtp)));
      return result;
    }
    case TAG_TYPE_AS_Seq1Type: {
      TYPE_AS_Id result (SEQ<Char>(L"s").ImpAppend(sepalator));
      result.ImpConc(stringify(p_tp.GetRecord(pos_AS_Seq1Type_elemtp)));
      return result;
    }
    case TAG_TYPE_AS_GeneralMap0Type: {
      TYPE_AS_Id result (SEQ<Char>(L"m").ImpAppend(sepalator));
      result.ImpConc(stringify(p_tp.GetRecord(pos_AS_GeneralMap0Type_mapdom)))
            .ImpAppend(sepalator)
            .ImpConc(stringify(p_tp.GetRecord(pos_AS_GeneralMap0Type_maprng)));
      return result;
    }
    case TAG_TYPE_AS_GeneralMap1Type: {
      TYPE_AS_Id result (SEQ<Char>(L"m").ImpAppend(sepalator));
      result.ImpConc(stringify(p_tp.GetRecord(pos_AS_GeneralMap1Type_mapdom)))
            .ImpAppend(sepalator)
            .ImpConc(stringify(p_tp.GetRecord(pos_AS_GeneralMap1Type_maprng)));
      return result;
    }
    case TAG_TYPE_AS_InjectiveMap0Type: {
      TYPE_AS_Id result (SEQ<Char>(L"m").ImpAppend(sepalator));
      result.ImpConc(stringify(p_tp.GetRecord(pos_AS_InjectiveMap0Type_mapdom)))
            .ImpAppend(sepalator)
            .ImpConc(stringify(p_tp.GetRecord(pos_AS_InjectiveMap0Type_maprng)));
      return result;
    }
    case TAG_TYPE_AS_InjectiveMap1Type: {
      TYPE_AS_Id result (SEQ<Char>(L"m").ImpAppend(sepalator));
      result.ImpConc(stringify(p_tp.GetRecord(pos_AS_InjectiveMap1Type_mapdom)))
            .ImpAppend(sepalator)
            .ImpConc(stringify(p_tp.GetRecord(pos_AS_InjectiveMap1Type_maprng)));
      return result;
    }
    case TAG_TYPE_AS_CompositeType: {
      TYPE_AS_Id result (SEQ<Char>(L"n").ImpAppend(sepalator));
      result.ImpConc(nm2string(p_tp.GetRecord(pos_AS_CompositeType_name)));
      return result;
    }
    case TAG_TYPE_AS_TypeName: {
      TYPE_AS_Id result (SEQ<Char>(L"n").ImpAppend(sepalator));
      result.ImpConc(nm2string(p_tp.GetRecord(pos_AS_TypeName_name)));
      return result;
    }
    case TAG_TYPE_AS_PartialFnType: {
      return stringifyFnType(p_tp.GetSequence(pos_AS_PartialFnType_fndom),
                             p_tp.GetRecord(pos_AS_PartialFnType_fnrng));
    }
    case TAG_TYPE_AS_TotalFnType: {
      return stringifyFnType(p_tp.GetSequence(pos_AS_TotalFnType_fndom),
                             p_tp.GetRecord(pos_AS_TotalFnType_fnrng));
    }
    case TAG_TYPE_AS_BracketedType: {
      SEQ<Char> result (SEQ<Char>(L"b").ImpAppend(sepalator));
      result.ImpConc(stringify(p_tp.GetRecord(pos_AS_BracketedType_tp)));
      return result;
    }
    case TAG_TYPE_AS_TypeVar: {
      SEQ<Char> result (SEQ<Char>(L"v").ImpAppend(sepalator));
      result.ImpConc(nm2string(p_tp.GetRecord(pos_AS_TypeVar_name)));
      return result;
    }
    case TAG_TYPE_AS_AllType: {
      return SEQ<Char>(L"a");
    }
    default: {
      return SEQ<Char>(L"others");
    }
  }
}

// stringifyNumericType
// p_tp : AS`NumericType
// -> seq of char
SEQ<Char> MANGLE::stringifyNumericType (const TYPE_AS_NumericType & p_tp)
{
  const Generic & bt (p_tp.GetField(pos_AS_NumericType_qtp));
  Int btI;
  if (bt.IsInt())
    btI = bt;
  else
    btI = INT2Q::Quote2Integer(bt);

  switch (btI.GetValue()) {
    case NATONE:     { return SEQ<Char>(L"M"); }
    case NAT:        { return SEQ<Char>(L"N"); }
    case INTEGER:    { return SEQ<Char>(L"I"); }
    case RAT:        { return SEQ<Char>(L"Q"); }
    case REAL:       { return SEQ<Char>(L"R"); }
    default:         { return SEQ<Char>(L"?"); }
  }
}

// stringifyUnionType
// p_tps : seq of AS`Type
// -> seq of char
SEQ<Char> MANGLE::stringifyUnionType (const SEQ<TYPE_AS_Type> & p_tps)
{
  SEQ<Char> result (L"u(");
  result.ImpConc(stringifyTypeSeq(p_tps, sepalator))
        .ImpConc(SEQ<Char>(L")"));
  return result;
}

// stringifyProductType
// p_tps : seq of AS`Type
// -> seq of char
SEQ<Char> MANGLE::stringifyProductType (const SEQ<TYPE_AS_Type> & p_tps)
{
  SEQ<Char> result (L"p(");
  result.ImpConc(stringifyTypeSeq(p_tps, sepalator))
        .ImpConc(SEQ<Char>(L")"));
  return result;
}

// stringifyFnType
// p_fd : (AS`DiscretionaryType | AS`AllType) * AS`Type
// -> seq of char
SEQ<Char> MANGLE::stringifyFnType (const SEQ<TYPE_AS_Type> & p_fd, const TYPE_AS_Type & p_fr)
{
  SEQ<Char> result (L"f");
  result.ImpAppend(sepalator)
        .ImpConc(stringifyProductType(p_fd))
        .ImpAppend(sepalator)
        .ImpConc(stringify(p_fr));
  return result; 
}

// nm2string
// p_nm : AS`Name
// -> seq of char
SEQ<Char> MANGLE::nm2string (const TYPE_AS_Name & p_nm)
{
  const TYPE_AS_Ids & ids (p_nm.GetSequence(pos_AS_Name_ids));
  SEQ<Char> result (ids[1]);
  if (ids.Length() == 2) {
    result.ImpAppend(Char(L'`'))
          .ImpConc(ids[2]);
  }
  return result;
}

// lit2string
// p_nm : AS`QuoteLit
// -> seq of char
SEQ<Char> MANGLE::lit2string (const TYPE_AS_QuoteLit & p_nm)
{
  return p_nm.get_val();
}

// stringifyTypeSeq
// p_tps : seq of AS`Type
// p_sep : char
// -> seq of char
SEQ<Char> MANGLE::stringifyTypeSeq (const SEQ<TYPE_AS_Type> & p_tps, const Char & p_sep)
{
  SEQ<Char> result;
  size_t len_p_tps = p_tps.Length();
  for (size_t idx = 1; idx <= len_p_tps; idx++)
  {
    if (idx > 1)
      result.ImpAppend(p_sep);
    result.ImpConc(stringify(p_tps[idx]));
  }
  return result;
}

// unstringifyTypeSeq
// p_str : seq of char
// -> seq of AS`Type
SEQ<TYPE_AS_Type> MANGLE::unstringifyTypeSeq (const SEQ<Char> & p_str)
{
  SEQ<TYPE_MANGLE_MangleToken> l_tokL (LexMangle(p_str));
  return ParseMangle(l_tokL);
}

// LexMangle
// p_str : seq of char
// ==> seq of MangleToken
SEQ<TYPE_MANGLE_MangleToken> MANGLE::LexMangle(const SEQ<Char> & p_str)
{
  if (p_str.IsEmpty())
    return SEQ<TYPE_MANGLE_MangleToken>();

  bool found = false;
  size_t len_p_str = p_str.Length();
  size_t index = 1;
  SEQ<TYPE_MANGLE_MangleToken> sep;
  for (size_t i = 1; (i <= len_p_str) && !found; i++) {
    if (p_str[i] == Char(L'-'))
    {
      index = i;
      found = true;
    }
    else if (p_str[i] == Char(L'('))
    {
      sep.ImpAppend(TYPE_MANGLE_LeftBracket());
      index = i;
      found = true;
    }
    else if (p_str[i] == Char(L')'))
    {
      sep.ImpAppend(TYPE_MANGLE_RightBracket());
      index = i;
      found = true;
    }
  }

  SEQ<TYPE_MANGLE_MangleToken> res;
  if (found) {
    if (index > 1)
      res.ImpConc(LexMangle(p_str.SubSequence(1, index - 1)));
    res.ImpConc(sep);
    if (index < len_p_str)
      res.ImpConc(LexMangle(p_str.SubSequence(index + 1, len_p_str)));
    return res;
  }
  else
    res.ImpAppend(p_str);
  return res;
}

// ParseMangle
// p_mantokL : seq of MangleToken
// ==> seq of AS`Type
SEQ<TYPE_AS_Type> MANGLE::ParseMangle(const SEQ<TYPE_MANGLE_MangleToken> & p_mantokL)
{
  if (p_mantokL.IsEmpty())
    return SEQ<TYPE_AS_Type>();
  
  Generic l_mangle (p_mantokL.Hd());
  SEQ<TYPE_MANGLE_MangleToken> l_rest (p_mantokL.Tl());

  if (l_mangle.IsSequence() && (Sequence(l_mangle).Length() == 1))
  {
    
    Char c (Sequence(l_mangle).Hd());
    switch (c.GetValue()) {
      case L'B': {
        return mk_sequence(TYPE_AS_BooleanType().Init(NilContextId)).ImpConc(ParseMangle(l_rest));
      }
      case L'M': {
        return mk_sequence(TYPE_AS_NumericType().Init(Int(NATONE), NilContextId)).ImpConc(ParseMangle(l_rest));
      }
      case L'N': {
        return mk_sequence(TYPE_AS_NumericType().Init(Int(NAT), NilContextId)).ImpConc(ParseMangle(l_rest));
      }
      case L'I': {
        return mk_sequence(TYPE_AS_NumericType().Init(Int(INTEGER), NilContextId)).ImpConc(ParseMangle(l_rest));
      }
      case L'Q': {
        return mk_sequence(TYPE_AS_NumericType().Init(Int(RAT), NilContextId)).ImpConc(ParseMangle(l_rest));
      }
      case L'R': {
        return mk_sequence(TYPE_AS_NumericType().Init(Int(REAL), NilContextId)).ImpConc(ParseMangle(l_rest));
      }
      case L'T': {
        return mk_sequence(TYPE_AS_TokenType().Init(NilContextId)).ImpConc(ParseMangle(l_rest));
      }
      case L'C': {
        return mk_sequence(TYPE_AS_CharType().Init(NilContextId)).ImpConc(ParseMangle(l_rest));
      }
      case L'q': {
        TYPE_AS_QuoteLit ql (TYPE_AS_QuoteLit().Init(l_rest.Hd(), NilContextId));
        return mk_sequence(TYPE_AS_QuoteType().Init(ql, NilContextId)).ImpConc(ParseMangle(l_rest.Tl()));
      }
      case L'u': {
        Tuple epl (ExtractParameterList(l_rest.Tl(), 0));
        SEQ<TYPE_AS_Type> tps (ParseMangle(epl.GetSequence(1)));
        return mk_sequence(TYPE_AS_UnionType().Init(tps, NilContextId)).ImpConc(ParseMangle(epl.GetSequence(2)));
      }
      case L'p': {
        Tuple epl (ExtractParameterList(l_rest.Tl(), 0));
        SEQ<TYPE_AS_Type> tps (ParseMangle(epl.GetSequence(1)));
        return mk_sequence(TYPE_AS_ProductType().Init(tps, NilContextId)).ImpConc(ParseMangle(epl.GetSequence(2)));
      }
      case L'o': {
        Tuple et (ExtractType(l_rest));
        SEQ<TYPE_AS_Type> tps (ParseMangle(et.GetSequence(1)));
        return mk_sequence(TYPE_AS_OptionalType().Init(tps.Hd(), NilContextId)).ImpConc(ParseMangle(et.GetSequence(2)));
      }
      case L't': {
        Tuple et (ExtractType(l_rest));
        SEQ<TYPE_AS_Type> tps (ParseMangle(et.GetSequence(1)));
        return mk_sequence(TYPE_AS_Set0Type().Init(tps.Hd(), NilContextId)).ImpConc(ParseMangle(et.GetSequence(2)));
      }
      case L's': {
        Tuple et (ExtractType(l_rest));
        SEQ<TYPE_AS_Type> tps (ParseMangle(et.GetSequence(1)));
        return mk_sequence(TYPE_AS_Seq0Type().Init(tps.Hd(), NilContextId)).ImpConc(ParseMangle(et.GetSequence(2)));
      }
      case L'm': {
        Tuple et1 (ExtractType(l_rest));
        Tuple et2 (ExtractType(et1.GetSequence(2)));
        SEQ<TYPE_AS_Type> tpsd (ParseMangle(et1.GetSequence(1)));
        SEQ<TYPE_AS_Type> tpsr (ParseMangle(et2.GetSequence(1)));
        return mk_sequence(TYPE_AS_GeneralMap0Type().Init(tpsd.Hd(), tpsr.Hd(), NilContextId)).ImpConc(ParseMangle(et2.GetSequence(2)));
      }
      case L'n': {
        TYPE_AS_Name nm (ASTAUX::MkNameFromStr(Sequence(l_rest.Hd()).GetString(), NilContextId));
        return mk_sequence(TYPE_AS_TypeName().Init(nm, NilContextId)).ImpConc(ParseMangle(l_rest.Tl()));
      }
      case L'v': {
        TYPE_AS_Name nm (ASTAUX::MkNameFromStr(Sequence(l_rest.Hd()).GetString(), NilContextId));
        return mk_sequence(TYPE_AS_TypeVar().Init(nm, NilContextId)).ImpConc(ParseMangle(l_rest.Tl()));
      }
      case L'f': {
        Tuple et1 (ExtractType(l_rest));
        Tuple et2 (ExtractType(et1.GetSequence(2)));
        SEQ<TYPE_AS_Type> tpsd (ParseMangle(et1.GetSequence(1)));
        SEQ<TYPE_AS_Type> tpsr (ParseMangle(et2.GetSequence(1)));
        TYPE_AS_ProductType pt (tpsd.Hd());
        return mk_sequence(TYPE_AS_PartialFnType().Init(pt.get_tps(), tpsr.Hd(), NilContextId)).ImpConc(ParseMangle(et2.GetSequence(2)));
      }
      case L'b': {
        Tuple et (ExtractType(l_rest));
        SEQ<TYPE_AS_Type> tps (ParseMangle(et.GetSequence(1)));
        return mk_sequence(TYPE_AS_BracketedType().Init(tps.Hd(), NilContextId)).ImpConc(ParseMangle(et.GetSequence(2)));
      }
      default: {
        // error
        return SEQ<TYPE_AS_Type>();
      }
    }
  }
  // error
  return SEQ<TYPE_AS_Type>();
}

// ExtractParameterList
// p_tokL : seq of MangleToken * nat
// +> seq of MangleToken * seq of MangleToken
Tuple MANGLE::ExtractParameterList(const SEQ<TYPE_MANGLE_MangleToken> p_tokL, int p_start)
{
  if (p_tokL.IsEmpty())
    return mk_(SEQ<TYPE_MANGLE_MangleToken>(), SEQ<TYPE_MANGLE_MangleToken>()); // error

  TYPE_MANGLE_MangleToken l_tok (p_tokL.Hd());
  SEQ<TYPE_MANGLE_MangleToken> l_rest (p_tokL.Tl());

  if (l_tok.IsRecord() && Record(l_tok).Is(TAG_TYPE_MANGLE_RightBracket))
  {
    if (p_start == 0)
      return mk_(SEQ<TYPE_MANGLE_MangleToken>(), l_rest);
    else
      return mk_(mk_sequence(l_tok), l_rest);
  }
  else if (l_tok.IsRecord() && Record(l_tok).Is(TAG_TYPE_MANGLE_LeftBracket))
  {
    Tuple epl (ExtractParameterList(l_rest, p_start + 1));
    return mk_(mk_sequence(l_tok).ImpConc(epl.GetSequence(1)), epl.GetSequence(2));
  }
  else
  {
    Tuple epl (ExtractParameterList(l_rest, p_start));
    return mk_(mk_sequence(l_tok).ImpConc(epl.GetSequence(1)), epl.GetSequence(2));
  }
}

// ExtractType
// p_tokL : seq of MangleToken
// +> seq of MangleToken * seq of MangleToken
Tuple MANGLE::ExtractType(const SEQ<TYPE_MANGLE_MangleToken> p_tokL)
{
  if (p_tokL.IsEmpty())
    return mk_(SEQ<TYPE_MANGLE_MangleToken>(), SEQ<TYPE_MANGLE_MangleToken>()); // error
  
  TYPE_MANGLE_MangleToken l_tok (p_tokL.Hd());
  SEQ<TYPE_MANGLE_MangleToken> l_rest (p_tokL.Tl());

  if (l_tok.IsSequence() && (Sequence(l_tok).Length() == 1))
  {
    Char c (Sequence(l_tok).Hd());
    switch (c.GetValue()) {
      case L'B':
      case L'M':
      case L'I':
      case L'Q':
      case L'R':
      case L'T':
      case L'C':
      case L'N': {
        return mk_(mk_sequence(l_tok), l_rest);
      }
      case L'q': {
        return mk_(p_tokL.SubSequence(1,2), p_tokL.SubSequence(3, p_tokL.Length()));
      }
      case L'u':
      case L'p': {
        Tuple epl (ExtractParameterList(l_rest, 0));
        return mk_(p_tokL.SubSequence(1,2).ImpConc(epl.GetSequence(1).Tl()), epl.GetSequence(2));
      }
      case L'n':
      case L'v': {
        return mk_(p_tokL.SubSequence(1,2), p_tokL.SubSequence(3, p_tokL.Length()));
      }
      case L'o':
      case L'b':
      case L't':
      case L's': {
        Tuple et (ExtractType(l_rest));
        return mk_(mk_sequence(l_tok).ImpConc(et.GetSequence(1)), et.GetSequence(2));
      }
      case L'm':
      case L'f': {
        Tuple et1 (ExtractType(l_rest));
        Tuple et2 (ExtractType(et1.GetSequence(2)));
        return mk_(mk_sequence(l_tok).ImpConc(et1.GetSequence(1)).ImpConc(et2.GetSequence(1)), et2.GetSequence(2));
      }
      default: {
      }
    }
  }
  return mk_(SEQ<TYPE_MANGLE_MangleToken>(), SEQ<TYPE_MANGLE_MangleToken>()); // error
}

