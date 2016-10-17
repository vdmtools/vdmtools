#include "astaux.h"
#include "asquotes.h"


#ifdef DEF_REP2TYPE_SetToSeq
type_typevariableL vdm_REP2TYPE_SetToSeq (const type_typevariableS & s)
{
  return REP2TYPE::SetToSeq(s);
}
#endif // DEF_REP2TYPE_SetToSeq

#ifdef DEF_REP2TYPE_TypeRepToType
TYPE_REP2TYPE_Type vdm_REP2TYPE_TypeRepToType (const TYPE_REP_TypeRep & tyrep,
                                               const TYPE_CI_ContextId & cid)
{
  return REP2TYPE::TypeRepToType(tyrep, cid);
}
#endif // DEF_REP2TYPE_TypeRepToType

// TypeRepToType
// tyrep : REP`TypeRep
// cid : CI`ContextId
// -> Type
TYPE_AS_Type REP2TYPE::TypeRepToType (const TYPE_REP_TypeRep & tyrep,
                                      const TYPE_CI_ContextId & cid)
{
  TYPE_AS_Name bottom_nm (ASTAUX::MkNameFromId(ASTAUX::MkId(L"bottom"), NilContextId));
  TYPE_AS_TypeName bottom (TYPE_AS_TypeName().Init(bottom_nm, NilContextId));
  TYPE_AS_Name top_nm (ASTAUX::MkNameFromId(ASTAUX::MkId(L"top"), NilContextId));
  TYPE_AS_TypeName top (TYPE_AS_TypeName().Init(top_nm, NilContextId));
//  TYPE_AS_Name nil_nm (MkNameFromId(MkId(L"nilType"), NilContextId));
//  TYPE_AS_TypeName nilType (TYPE_AS_TypeName().Init(nil_nm, NilContextId));

  switch (tyrep.GetTag()) {
    case TAG_TYPE_REP_InvTypeRep: {
      return TypeRepToType(tyrep.GetRecord(pos_REP_InvTypeRep_shape), cid);
    }
    case TAG_TYPE_REP_BooleanTypeRep: {
      return TYPE_AS_BooleanType().Init(cid);
    }
    case TAG_TYPE_REP_NumericTypeRep: {
      Generic qtp (tyrep.GetField(pos_REP_NumericTypeRep_qtp));
      return TYPE_AS_NumericType().Init(qtp, cid);
    }
    case TAG_TYPE_REP_TokenTypeRep: {
      return TYPE_AS_TokenType().Init(cid);
    }
    case TAG_TYPE_REP_CharTypeRep: {
      return TYPE_AS_CharType().Init(cid);
    }
    case TAG_TYPE_REP_UnitTypeRep: {
      return TYPE_AS_VoidType().Init(cid);
    }
    case TAG_TYPE_REP_QuoteTypeRep: {
      const TYPE_AS_Name & lit (tyrep.GetRecord(pos_REP_QuoteTypeRep_lit));
      return TYPE_AS_QuoteType().Init(TYPE_AS_QuoteLit().Init(lit.GetSequence(pos_AS_Name_ids).Hd(), cid), cid);
    }
    case TAG_TYPE_REP_CompositeTypeRep: {
      const TYPE_AS_Name & nm (tyrep.GetRecord(pos_REP_CompositeTypeRep_nm));
      const SEQ<TYPE_REP_FieldRep> & fields (tyrep.GetSequence(pos_REP_CompositeTypeRep_fields));
      SEQ<TYPE_AS_Field> fld;
      size_t len_fields = fields.Length();
      for (size_t index = 1; index <= len_fields; index++)
      {
        const TYPE_REP_FieldRep & fr (fields[index]);
        fld.ImpAppend(TYPE_AS_Field().Init(fr.GetField(pos_REP_FieldRep_sel),
                                           TypeRepToType(fr.GetRecord(pos_REP_FieldRep_tp), cid),
                                           fr.GetBool(pos_REP_FieldRep_dc),
                                           cid));
      }
      return TYPE_AS_CompositeType().Init(nm, fld, cid);
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> type_set (tyrep.GetSet(pos_REP_UnionTypeRep_tps));
      SET<TYPE_REP_TypeRep> tyset;
      bool emptyseq_exists = false;
      bool nil_exists = false;
      Generic g;
      for (bool bb = type_set.First(g); bb; bb = type_set.Next(g))
      {
        TYPE_REP_TypeRep ty (g);
        switch(ty.GetTag()) {
          case TAG_TYPE_REP_EmptySeqTypeRep: {
            emptyseq_exists = true;
            break;
          }
          case TAG_TYPE_REP_EmptySetTypeRep:
          case TAG_TYPE_REP_EmptyMapTypeRep:
            break;
          case TAG_TYPE_REP_NilTypeRep: {
            nil_exists = true;
            break;
          }
          default: {
            tyset.Insert(ty);
            break;
          }
        }
      }
      SET<TYPE_AS_Type> argset;
      for (bool cc = tyset.First(g); cc; cc = tyset.Next(g))
      {
        TYPE_REP_TypeRep ty (g);
        switch(ty.GetTag()) {
          case TAG_TYPE_REP_SeqTypeRep: {
            if (emptyseq_exists)
            {
              argset.Insert(TYPE_AS_Seq0Type().Init(
                              TypeRepToType(ty.GetRecord(pos_REP_SeqTypeRep_elemtp), cid),
                              cid));
            }
            else
            {
              argset.Insert(TypeRepToType(ty, cid));
            }
            break;
          } 
          default: {
            argset.Insert(TypeRepToType(ty, cid));
            break;
          }
        }
      }
      SEQ<TYPE_AS_Type> tyseq (REP2TYPE::SetToSeq(argset));
      TYPE_AS_Type as_tp;
      if (tyseq.Length() == 1)
        as_tp = tyseq[1];
      else
        as_tp = TYPE_AS_UnionType().Init(tyseq, cid);

      if (nil_exists)
        return TYPE_AS_OptionalType().Init(as_tp, cid);
      else
        return as_tp;
    }
    case TAG_TYPE_REP_ProductTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & tylist (tyrep.GetSequence(pos_REP_ProductTypeRep_tps));
      size_t len_tylist = tylist.Length();
      SEQ<TYPE_AS_Type> tps;
      for (size_t index = 1; index <= len_tylist; index++)
      {
        tps.ImpAppend(TypeRepToType(tylist[index], cid));
      }
      return TYPE_AS_ProductType().Init(tps, cid);
    }
    case TAG_TYPE_REP_SetTypeRep: {
      return TYPE_AS_Set1Type().Init(
               TypeRepToType(tyrep.GetRecord(pos_REP_SetTypeRep_elemtp), cid),
               cid);
    }
    case TAG_TYPE_REP_SeqTypeRep: {
      return TYPE_AS_Seq1Type().Init(
               TypeRepToType(tyrep.GetRecord(pos_REP_SeqTypeRep_elemtp), cid),
               cid);
    }
    case TAG_TYPE_REP_GeneralMapTypeRep: {
      return TYPE_AS_GeneralMap1Type().Init(
               TypeRepToType(tyrep.GetRecord(pos_REP_GeneralMapTypeRep_mapdom), cid),
               TypeRepToType(tyrep.GetRecord(pos_REP_GeneralMapTypeRep_maprng), cid),
               cid);
    }
    case TAG_TYPE_REP_InjectiveMapTypeRep: {
      return TYPE_AS_InjectiveMap1Type().Init(
               TypeRepToType(tyrep.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom), cid),
               TypeRepToType(tyrep.GetRecord(pos_REP_InjectiveMapTypeRep_maprng), cid),
               cid);
    }
    case TAG_TYPE_REP_PartialFnTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & fndom (tyrep.GetSequence(pos_REP_PartialFnTypeRep_fndom));
      const TYPE_REP_TypeRep & fnrng (tyrep.GetRecord(pos_REP_PartialFnTypeRep_fnrng));
      SEQ<TYPE_AS_Type> tp_l;
      size_t len_fndom = fndom.Length();
      for (size_t index = 1; index <= len_fndom; index++)
        tp_l.ImpAppend(TypeRepToType(fndom[index], cid));
      return TYPE_AS_PartialFnType().Init(tp_l, TypeRepToType(fnrng, cid), cid);
    }
    case TAG_TYPE_REP_TotalFnTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & fndom (tyrep.GetSequence(pos_REP_TotalFnTypeRep_fndom));
      const TYPE_REP_TypeRep & fnrng (tyrep.GetRecord(pos_REP_TotalFnTypeRep_fnrng));
      SEQ<TYPE_AS_Type> tp_l;
      size_t len_fndom = fndom.Length();
      for (size_t index = 1; index <= len_fndom; index++)
        tp_l.ImpAppend(TypeRepToType(fndom[index], cid));
      return TYPE_AS_TotalFnType().Init(tp_l, TypeRepToType(fnrng, cid), cid);
    }
    case TAG_TYPE_REP_OpTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & opdom (tyrep.GetSequence(pos_REP_OpTypeRep_Dom));
      const TYPE_REP_TypeRep & oprng (tyrep.GetRecord(pos_REP_OpTypeRep_Rng));
      SEQ<TYPE_AS_Type> tp_l;
      size_t len_opdom = opdom.Length();
      for (size_t index = 1; index <= len_opdom; index++)
        tp_l.ImpAppend(TypeRepToType(opdom[index], cid));
      return TYPE_AS_OpType().Init(tp_l, TypeRepToType(oprng, cid), cid);
    }
#ifdef VICE
    case TAG_TYPE_REP_AnyOpTypeRep: {
      return top;
    }
#endif // VICE
    case TAG_TYPE_REP_AllTypeRep: {
      return top;
    }
    case TAG_TYPE_REP_TypeNameRep: {
      return TYPE_AS_TypeName().Init(tyrep.GetRecord(pos_REP_TypeNameRep_nm), cid);
    }
    case TAG_TYPE_REP_TypeParRep: {
      return TYPE_AS_TypeName().Init(tyrep.GetRecord(pos_REP_TypeParRep_nm), cid);
    }
    case TAG_TYPE_REP_NilTypeRep: {
      return TYPE_AS_TypeName().Init(ASTAUX::MkNameFromId(ASTAUX::MkId(L"nilType"), NilContextId), cid);
    }
    case TAG_TYPE_REP_EmptySetTypeRep: {
      return TYPE_AS_Set0Type().Init(bottom, cid);
    }
    case TAG_TYPE_REP_EmptySeqTypeRep: {
      return TYPE_AS_Seq0Type().Init(bottom, cid);
    }
    case TAG_TYPE_REP_EmptyMapTypeRep: {
      return TYPE_AS_GeneralMap0Type().Init(bottom, bottom, cid);
    }
#ifdef VDMPP
    case TAG_TYPE_REP_ObjRefTypeRep: {
      return TYPE_AS_TypeName().Init(tyrep.GetRecord(pos_REP_ObjRefTypeRep_nm), cid);
    }
#endif // VDMPP
    case TAG_TYPE_REP_TypeVarRep: {
      return TYPE_AS_TypeVar().Init(tyrep.GetRecord(pos_REP_TypeVarRep_nm), cid);
    }
// 20090610 ->
    case TAG_TYPE_REP_PolyTypeRep: {
      return TypeRepToType(tyrep.GetRecord(pos_REP_PolyTypeRep_tp), cid);
    }
// <- 20090610
    default: {
      wcout << L"Unsupported type(REP2TYPE::TypeRepToType)" << endl;
      wcout << tyrep << endl;
      return TYPE_REP2TYPE_Type();
    }
  }
}

// SetToSeq
// s : set of AS`Type
// -> seq of AS`Type
SEQ<TYPE_AS_Type> REP2TYPE::SetToSeq (const SET<TYPE_AS_Type> & s_)
{
  if (s_.IsEmpty()) return type_typevariableL();

  SET<TYPE_AS_Type> s (s_);
//  return (Generic)vdm_REP2TYPE_s.ToSequence().Reverse();
  Set bt, tn, seqt, sett, mapt, ft, ot;
  Generic g;
  for (bool bb = s.First(g); bb; bb = s.Next(g))
  {
    switch(Record(g).GetTag()) {
      case TAG_TYPE_AS_TypeName: {
        tn.Insert(g);
        break;
      }
      case TAG_TYPE_AS_BooleanType:
      case TAG_TYPE_AS_NumericType:
      case TAG_TYPE_AS_TokenType:
      case TAG_TYPE_AS_CharType: {
        bt.Insert(g);
        break;
      }
      case TAG_TYPE_AS_Seq0Type:
      case TAG_TYPE_AS_Seq1Type: {
        seqt.Insert(g);
        break;
      }
      case TAG_TYPE_AS_TotalFnType:
      case TAG_TYPE_AS_PartialFnType:
      case TAG_TYPE_AS_OpType: {
        ft.Insert(g);
        break;
      }
      default: {
        ot.Insert(g);
        break;
      }
    }
  }
  Sequence res;
  res.ImpConc(tn.ToSequence().Reverse());
  res.ImpConc(seqt.ToSequence().Reverse());
  res.ImpConc(bt.ToSequence().Reverse());
  res.ImpConc(ft.ToSequence().Reverse());
  res.ImpConc(ot.ToSequence().Reverse());
  return (Generic)res;
}

