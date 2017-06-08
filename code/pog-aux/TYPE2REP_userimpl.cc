#include "metaiv.h"
#include "contextinfo.h"
#include "astaux.h"

#ifdef DEF_TYPE2REP_TransType
TYPE_REP_TypeRep vdm_TYPE2REP_TransType (const Record & tp)
{
  return TYPE2REP::TransType(tp);
}
#endif // DEF_TYPE2REP_TransType

// TransType
// VDMSL
// tp : AS`Type
// VDMPP
// tp : (AS`Type | POGTP`ClassType)
// -> REP`TypeRep
TYPE_REP_TypeRep TYPE2REP::TransType (const Record & tp)
{
  switch(tp.GetTag()) {
    case TAG_TYPE_AS_BooleanType: {
      return TYPE_REP_BooleanTypeRep();
    }
    case TAG_TYPE_AS_NumericType: {
      return TYPE_REP_NumericTypeRep().Init(tp.GetField(pos_AS_NumericType_qtp));
    }
    case TAG_TYPE_AS_TokenType: {
      return TYPE_REP_TokenTypeRep();
    }
    case TAG_TYPE_AS_CharType: {
      return TYPE_REP_CharTypeRep();
    }
    case TAG_TYPE_AS_VoidType: {
      return TYPE_REP_UnitTypeRep();
    }
    case TAG_TYPE_AS_QuoteType: {
      const TYPE_AS_QuoteLit & lit (tp.GetRecord(pos_AS_QuoteType_lit));
      return TYPE_REP_QuoteTypeRep().Init(ASTAUX::MkNameFromId(lit.GetSequence(pos_AS_QuoteLit_val), NilContextId));
    }
#ifdef VDMPP
    case TAG_TYPE_POGTP_ClassType: {
      return TYPE_REP_TypeNameRep().Init(tp.GetRecord(pos_POGTP_ClassType_name));
    }
#endif // VDMPP
    case TAG_TYPE_AS_TypeName: {
      return TYPE_REP_TypeNameRep().Init(tp.GetRecord(pos_AS_TypeName_name));
    }
    case TAG_TYPE_AS_TypeVar: {
      return TYPE_REP_TypeVarRep().Init(tp.GetRecord(pos_AS_TypeVar_name));
    }
    case TAG_TYPE_AS_CompositeType: {
      const TYPE_AS_Name & tag (tp.GetRecord(pos_AS_CompositeType_name));
      const SEQ<TYPE_AS_Field> & fields (tp.GetSequence(pos_AS_CompositeType_fields));
      return TYPE_REP_CompositeTypeRep().Init(tag, TYPE2REP::TransFields(fields));
    }
    case TAG_TYPE_AS_UnionType: {
      const SEQ<TYPE_AS_Type> & tplst (tp.GetSequence(pos_AS_UnionType_tps));
      SET<TYPE_REP_TypeRep> tp_s;
      int len_tplst = tplst.Length();
      for (int index = 1; index <= len_tplst; index++)
        tp_s.Insert(TransType(tplst[index]));
      return TYPE_REP_UnionTypeRep().Init(TYPE2REP::FlattenUnion(tp_s));
    }
    case TAG_TYPE_AS_ProductType: {
      const SEQ<TYPE_AS_Type> & tplst (tp.GetSequence(pos_AS_ProductType_tps));
      return TYPE_REP_ProductTypeRep().Init(TYPE2REP::TransTypeList(tplst));
    }
    case TAG_TYPE_AS_BracketedType: {
      return TransType(tp.GetRecord(pos_AS_BracketedType_tp));
    }
    case TAG_TYPE_AS_OptionalType: {
      SET<TYPE_REP_TypeRep> tp_s;
      tp_s.Insert(TYPE_REP_NilTypeRep());
      tp_s.Insert(TransType(tp.GetRecord(pos_AS_OptionalType_tp)));
      return TYPE_REP_UnionTypeRep().Init(TYPE2REP::FlattenUnion(tp_s));
    }
    case TAG_TYPE_AS_Set0Type: {
      TYPE_REP_TypeRep elemtp (TransType(tp.GetRecord(pos_AS_Set0Type_elemtp)));
      SET<TYPE_REP_TypeRep> tp_s;
      tp_s.Insert(mk_REP_EmptySetTypeRep(elemtp));
      tp_s.Insert(mk_REP_SetTypeRep(elemtp));
      return TYPE_REP_UnionTypeRep().Init(tp_s);
    }
    case TAG_TYPE_AS_Set1Type: {
      TYPE_REP_TypeRep elemtp (TransType(tp.GetRecord(pos_AS_Set1Type_elemtp)));
      SET<TYPE_REP_TypeRep> tp_s;
      tp_s.Insert(mk_REP_EmptySetTypeRep(elemtp));
      tp_s.Insert(mk_REP_SetTypeRep(elemtp));
      return TYPE_REP_UnionTypeRep().Init(tp_s);
    }
    case TAG_TYPE_AS_Seq0Type: {
      TYPE_REP_TypeRep elemtp (TransType(tp.GetRecord(pos_AS_Seq0Type_elemtp)));
      SET<TYPE_REP_TypeRep> tp_s;
      tp_s.Insert(mk_REP_EmptySeqTypeRep(elemtp));
      tp_s.Insert(mk_REP_SeqTypeRep(elemtp));
      return TYPE_REP_UnionTypeRep().Init(tp_s);
    }
    case TAG_TYPE_AS_Seq1Type: {
      return TYPE_REP_SeqTypeRep().Init(TransType(tp.GetRecord(pos_AS_Seq1Type_elemtp)));
    }
    case TAG_TYPE_AS_GeneralMap0Type: {
      TYPE_REP_TypeRep mapdom (TransType(tp.GetRecord(pos_AS_GeneralMap0Type_mapdom)));
      TYPE_REP_TypeRep maprng (TransType(tp.GetRecord(pos_AS_GeneralMap0Type_maprng)));
      SET<TYPE_REP_TypeRep> tp_s;
      tp_s.Insert(TYPE_REP_EmptyMapTypeRep().Init(mapdom, maprng));
      tp_s.Insert(TYPE_REP_GeneralMapTypeRep().Init(mapdom, maprng));
      return TYPE_REP_UnionTypeRep().Init(tp_s);
    }
    case TAG_TYPE_AS_GeneralMap1Type: {
      TYPE_REP_TypeRep mapdom (TransType(tp.GetRecord(pos_AS_GeneralMap1Type_mapdom)));
      TYPE_REP_TypeRep maprng (TransType(tp.GetRecord(pos_AS_GeneralMap1Type_maprng)));
      SET<TYPE_REP_TypeRep> tp_s;
      tp_s.Insert(TYPE_REP_EmptyMapTypeRep().Init(mapdom, maprng));
      tp_s.Insert(TYPE_REP_GeneralMapTypeRep().Init(mapdom, maprng));
      return TYPE_REP_UnionTypeRep().Init(tp_s);
    }
    case TAG_TYPE_AS_InjectiveMap0Type: {
      TYPE_REP_TypeRep mapdom (TransType(tp.GetRecord(pos_AS_InjectiveMap0Type_mapdom)));
      TYPE_REP_TypeRep maprng (TransType(tp.GetRecord(pos_AS_InjectiveMap0Type_maprng)));
      SET<TYPE_REP_TypeRep> tp_s;
      tp_s.Insert(TYPE_REP_EmptyMapTypeRep().Init(mapdom, maprng));
      tp_s.Insert(TYPE_REP_InjectiveMapTypeRep().Init(mapdom, maprng));
      return TYPE_REP_UnionTypeRep().Init(tp_s);
    }
    case TAG_TYPE_AS_InjectiveMap1Type: {
      TYPE_REP_TypeRep mapdom (TransType(tp.GetRecord(pos_AS_InjectiveMap1Type_mapdom)));
      TYPE_REP_TypeRep maprng (TransType(tp.GetRecord(pos_AS_InjectiveMap1Type_maprng)));
      SET<TYPE_REP_TypeRep> tp_s;
      tp_s.Insert(TYPE_REP_EmptyMapTypeRep().Init(mapdom, maprng));
      tp_s.Insert(TYPE_REP_InjectiveMapTypeRep().Init(mapdom, maprng));
      return TYPE_REP_UnionTypeRep().Init(tp_s);
    }
    case TAG_TYPE_AS_OpType: {
      const SEQ<TYPE_AS_Type> & d (tp.GetSequence(pos_AS_OpType_opdom));
      const TYPE_AS_Type & r (tp.GetRecord(pos_AS_OpType_oprng));
      return TYPE_REP_OpTypeRep().Init(TYPE2REP::TransTypeList(d), TransType(r));
    }
    case TAG_TYPE_AS_PartialFnType: {
      const SEQ<TYPE_AS_Type> & d (tp.GetSequence(pos_AS_PartialFnType_fndom));
      const TYPE_AS_Type & r (tp.GetField(pos_AS_PartialFnType_fnrng));
      return TYPE_REP_PartialFnTypeRep().Init(TYPE2REP::TransTypeList(d), TransType(r));
    }
    case TAG_TYPE_AS_TotalFnType: {
      const SEQ<TYPE_AS_Type> & d (tp.GetSequence(pos_AS_TotalFnType_fndom));
      const TYPE_AS_Type & r (tp.GetField(pos_AS_TotalFnType_fnrng));
      return TYPE_REP_TotalFnTypeRep().Init(TYPE2REP::TransTypeList(d), TransType(r));
    }
    default: {
      wcout << L"Unsupported type(TYPE2REP::TransType)" << endl;
      wcout << tp << endl;
      return TYPE_REP_TypeRep();
    }
  }
}

// TransTypeList
// tplst : seq of AS`Type
// -> seq of REP`TypeRep
SEQ<TYPE_REP_TypeRep> TYPE2REP::TransTypeList (const SEQ<TYPE_AS_Type> & tplst)
{
  SEQ<TYPE_REP_TypeRep> tr_l;
  size_t len_tplst = tplst.Length();
  for (size_t index = 1; index <= len_tplst; index++)
    tr_l.ImpAppend(TYPE2REP::TransType(tplst[index]));
  return tr_l;
}

// TransFields
// f_l : seq of AS`Field
// -> seq of REP`FieldRep
SEQ<TYPE_REP_FieldRep> TYPE2REP::TransFields (const SEQ<TYPE_AS_Field> & f_l)
{
  SEQ<TYPE_REP_FieldRep> fields;
  size_t len_f_l = f_l.Length();
  for (size_t index = 1; index <= len_f_l; index++)
  {
    const TYPE_AS_Field & f (f_l[index]);
    fields.ImpAppend(TYPE_REP_FieldRep().Init(f.GetField(pos_AS_Field_sel),
                                              TYPE2REP::TransType(f.GetRecord(pos_AS_Field_type)),
                                              f.GetBool(pos_AS_Field_dc)));
  }
  return fields; 
}

// FlattenUnion
// tps : set of REP`TypeRep
// -> set of REP`TypeRep
SET<TYPE_REP_TypeRep> TYPE2REP::FlattenUnion (const SET<TYPE_REP_TypeRep> & tps_)
{
  SET<TYPE_REP_TypeRep> tps (tps_);
  SET<TYPE_REP_TypeRep> res;
  Generic g;
  for (bool bb = tps.First(g); bb; bb = tps.Next(g))
  {
    TYPE_REP_TypeRep tp (g);
    switch(tp.GetTag()) {
      case TAG_TYPE_REP_UnionTypeRep: {
        res.ImpUnion(TYPE2REP::FlattenUnion(tp.GetSet(pos_REP_UnionTypeRep_tps))); 
        break;
      }
      default: {
        res.Insert(tp);
        break;
      }
    }
  }
  return res;
}

