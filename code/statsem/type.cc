/***
*  * ID
*  *    $Id: type.cc,v 1.34 2006/03/22 07:15:37 vdmtools Exp $
*  * SPECIFICATION VERSION
*  *    type.cc: Implementation of type.vdm 1.37
***/

#include "statsem.h"
#include "contextinfo.h"
#include "intconvquotes.h"
#include "settings.h"
#include "tbutils.h"

//wcout << L"intertp: " << INT2Q::h2gAS(intertp) << endl;

#ifndef NOSS

// AuxIsUnionRec
// tp : REP`TypeRep
// union_types : set of REP`TypeRep
// ==> bool
bool StatSem::AuxIsUnionRec (const TYPE_REP_TypeRep & tp, const SET<TYPE_REP_TypeRep> & union_types)
{
  if (union_types.InSet(tp)) {
    return true;
  }
  else {
    switch(tp.GetTag()) {
      case TAG_TYPE_REP_TypeNameRep: {
#ifdef VDMSL
        Generic newtp (LookUpTypeName(tp.GetRecord(pos_REP_TypeNameRep_nm)));
#endif // VDMSL
#ifdef VDMPP
        Generic newtp (StripAccessType (LookUpTypeName(tp.GetRecord(pos_REP_TypeNameRep_nm), true)));
#endif // VDMPP

        if (newtp.IsNil()) {
          return false;
        }
        else {
          return AuxIsUnionRec(newtp, SET<TYPE_REP_TypeRep>(union_types).Insert(tp));
        }
      }
      case TAG_TYPE_REP_UnionTypeRep: {
        SET<TYPE_REP_TypeRep> Utps (tp.GetSet(pos_REP_UnionTypeRep_tps));
        SET<TYPE_REP_TypeRep> ut (union_types);
        ut.Insert (tp);
        bool exists (false);
        Generic utp;
        for (bool bb = Utps.First(utp); bb && !exists; bb = Utps.Next(utp)) {
          exists = AuxIsUnionRec (utp, ut);
        }
        return exists;
      }
      case TAG_TYPE_REP_InvTypeRep: {
        return AuxIsUnionRec(tp.GetRecord(pos_REP_InvTypeRep_shape), union_types);
      }
      default: {
        return false;
      }
    }
  }
}

// IsUnionRecursive
// tp : REP`TypeRep
// ==> bool
bool StatSem::IsUnionRecursive (const TYPE_REP_TypeRep & tp)
{
  if (!this->UnionRecursiveResults.DomExists (tp)) {
    this->UnionRecursiveResults.Insert (tp, Bool(AuxIsUnionRec (tp, Set())));
  }
  return this->UnionRecursiveResults[tp].GetValue();
}

// IsCompatible
// i : Ind
// TpR1 : REP`TypeRep
// TpR2 : REP`TypeRep
// ==> bool
bool StatSem::IsCompatible (const Int & i, const TYPE_REP_TypeRep & TpR1, const TYPE_REP_TypeRep & TpR2)
{
  if (TpR1 == TpR2) {
    return true;
  }
  else {
    return ((i == POS) ? IsOverlapping(TpR1, TpR2, Set()) : IsSubType(TpR1, TpR2, Set()));
  }
}

// IsEmpty
// i : Ind
// tp : REP`TypeRep ==> bool
bool StatSem::IsEmpty (const Int & i, const TYPE_REP_TypeRep & tp)
{
  return IsEmptyAux (i, tp, SET<TYPE_REP_TypeRep>());
}

// IsEmptyAux
// i : Ind
// tp : REP`TypeRep
// empty_tps : set of REP`TypeRep
// ==> bool
bool StatSem::IsEmptyAux (const Int & i, const TYPE_REP_TypeRep & tp, const SET<TYPE_REP_TypeRep> & empty_tps)
{
  if (empty_tps.InSet(tp)) {
    return true;
  }
  else {
    SET<TYPE_REP_TypeRep> new_tps (empty_tps);
    new_tps.Insert(tp);
    switch (tp.GetTag()) {
      case TAG_TYPE_REP_TypeNameRep: {
        if (IsUnionRecursive (tp)) {
          return (i == POS);
        }
        else {
#ifdef VDMSL
          Generic newtp (LookUpTypeName(tp.GetRecord(pos_REP_TypeNameRep_nm)));
#endif // VDMSL
#ifdef VDMPP
          Generic newtp (StripAccessType (LookUpTypeName(tp.GetRecord(pos_REP_TypeNameRep_nm), true)));
#endif // VDMPP

          return (newtp.IsNil() ? false : IsEmptyAux (i, newtp, new_tps));
        }
      }
      case TAG_TYPE_REP_SeqTypeRep: {
        return IsEmptyAux (i, tp.GetRecord(pos_REP_SeqTypeRep_elemtp), new_tps);
      }
      case TAG_TYPE_REP_SetTypeRep: {
        return IsEmptyAux (i, tp.GetRecord(pos_REP_SetTypeRep_elemtp), new_tps);
      }
      case TAG_TYPE_REP_UnionTypeRep: {
        SET<TYPE_REP_TypeRep> t_s (tp.GetSet(pos_REP_UnionTypeRep_tps));
        bool forall(true);
        Generic t;
        for(bool bb = t_s.First(t); bb && forall; bb = t_s.Next(t)) {
          forall = IsEmptyAux(i, t, new_tps);
        }
        return forall;
      }
      case TAG_TYPE_REP_ProductTypeRep: {
        const SEQ<TYPE_REP_TypeRep> & t_l (tp.GetSequence(pos_REP_ProductTypeRep_tps));
        bool exists(false);
        size_t len_t_l = t_l.Length();
        for(size_t idx = 1; (idx <= len_t_l) && !exists; idx++) {
          exists = IsEmptyAux(i, t_l[idx], new_tps);
        }
        return exists;
      }
      case TAG_TYPE_REP_CompositeTypeRep: {
        const SEQ<TYPE_REP_FieldRep> & f_l (tp.GetSequence(pos_REP_CompositeTypeRep_fields));
        bool exists(false);
        size_t len_f_l = f_l.Length();
        for(size_t idx = 1; (idx <= len_f_l) && !exists; idx++) {
          exists = IsEmptyAux(i, f_l[idx].GetRecord(pos_REP_FieldRep_tp), new_tps);
        }
        return exists;
      }
      case TAG_TYPE_REP_InvTypeRep: {
        return (i == POS) || (IsEmptyAux(i, tp.GetRecord(pos_REP_InvTypeRep_shape), new_tps));
      }
      case TAG_TYPE_REP_BooleanTypeRep:
      case TAG_TYPE_REP_NumericTypeRep:
      case TAG_TYPE_REP_TokenTypeRep:
      case TAG_TYPE_REP_CharTypeRep:
      case TAG_TYPE_REP_EmptySeqTypeRep:
      case TAG_TYPE_REP_EmptySetTypeRep:
      case TAG_TYPE_REP_EmptyMapTypeRep:
      case TAG_TYPE_REP_TypeVarRep:
      case TAG_TYPE_REP_PartialFnTypeRep:
      case TAG_TYPE_REP_TotalFnTypeRep:
      case TAG_TYPE_REP_OpTypeRep:
      default: {
        return false;
      }
    }
  }
}

// NegWfClass
// i : Ind
// -> Ind
Int StatSem::NegWfClass (const Int & i) const
{
  return ((i == POS) ? DEF : POS);
}

// IsSubType
// check TpR1 is sub type of TpR2
// TpR1 : REP`TypeRep
// TpR2 : REP`TypeRep
// assump : set of (REP`TypeNameRep * REP`TypeNameRep)
// ==> bool
bool StatSem::IsSubType (const TYPE_REP_TypeRep & TpR1, const TYPE_REP_TypeRep & TpR2, const Set & assump)
{
  if (TpR1 == TpR2) {
    return true;
  }
  switch (TpR1.GetTag()) {
    case TAG_TYPE_REP_ExitTypeRep: { return true; }
    case TAG_TYPE_REP_AllTypeRep: { return false; }
    case TAG_TYPE_REP_InvTypeRep: { return IsSubType (TpR1.GetRecord(pos_REP_InvTypeRep_shape), TpR2, assump); }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> Lhs (TpR1.GetSet(pos_REP_UnionTypeRep_tps));
      bool forall = true;
      Generic tp;
      for (bool bb = Lhs.First(tp); bb && forall; bb = Lhs.Next(tp)) {
        forall = IsSubType(tp, TpR2, assump);
      }
      return forall;
    }
    case TAG_TYPE_REP_TypeNameRep: {
      if (TpR2.Is(TAG_TYPE_REP_TypeNameRep) && assump.InSet(mk_(TpR1, TpR2))) {
        return true;
      }
      if (IsUnionRecursive(TpR1)) {
        return false;
      }
      else {
#ifdef VDMSL
        Generic newtp (LookUpTypeName(TpR1.GetRecord(pos_REP_TypeNameRep_nm)));
#endif // VDMSL
#ifdef VDMPP
        Generic newtp (StripAccessType (LookUpTypeName(TpR1.GetRecord(pos_REP_TypeNameRep_nm), true)));
#endif // VDMPP
        Set new_assump (assump);
        if (TpR2.Is(TAG_TYPE_REP_TypeNameRep)) {
          new_assump.Insert(mk_(TpR1, TpR2));
        }
        return (newtp.IsNil() ? false : IsSubType(newtp, TpR2, new_assump));
      }
    }
#ifdef VDMPP
    case TAG_TYPE_REP_OverTypeRep: {
      SET<TYPE_REP_TypeRep> Lhs (TpR1.GetSet(pos_REP_OverTypeRep_tps));
      bool forall = true;
      Generic tp;
      for (bool bb = Lhs.First(tp); bb && forall; bb = Lhs.Next(tp)) {
        forall = IsSubType(tp, TpR2, assump);
      }
      return forall;
    }
#endif // VDMPP
  }

  switch (TpR2.GetTag()) {
    case TAG_TYPE_REP_AllTypeRep: { return true; }
    case TAG_TYPE_REP_InvTypeRep: {
      if (Settings.ErrorLevel() >= ERR) {
        return false;
      }
      else {
        return IsSubType (TpR1, TpR2.GetRecord(pos_REP_InvTypeRep_shape), assump);
      }
    }
    case TAG_TYPE_REP_TypeNameRep: {
      if (IsUnionRecursive(TpR2)) {
        return false;
      }
      else {
#ifdef VDMSL
        Generic newtp (LookUpTypeName(TpR2.GetRecord(pos_REP_TypeNameRep_nm)));
#endif // VDMSL
#ifdef VDMPP
        Generic newtp (StripAccessType (LookUpTypeName(TpR2.GetRecord(pos_REP_TypeNameRep_nm), true)));
#endif // VDMPP
        return (newtp.IsNil() ? false : IsSubType(TpR1, newtp, assump));
      }
    }
    case TAG_TYPE_REP_NumericTypeRep: {
      if (TpR1.Is(TAG_TYPE_REP_NumericTypeRep)) {
        const Int & BTp1 (TpR1.GetInt(pos_REP_NumericTypeRep_qtp));
        const Int & BTp2 (TpR2.GetInt(pos_REP_NumericTypeRep_qtp));

        switch(BTp2) {
          case REAL:
          case RAT: { return true; }
          case INTEGER: {
            SET<Int> AllowedNumTypes (this->intTypes);
            return AllowedNumTypes.InSet(BTp1);
          }
          case NAT: { return (BTp1 == NATONE); }
          default: { return false; }
        }
      }
      else {
        return false;
      }
    }
    case TAG_TYPE_REP_SetTypeRep: {
      if (TpR1.Is(TAG_TYPE_REP_SetTypeRep)) {
        return IsSubType (TpR1.GetRecord(pos_REP_SetTypeRep_elemtp),
                          TpR2.GetRecord(pos_REP_SetTypeRep_elemtp),
                          assump);
      }
      else {
        return false;
      }
    }
    case TAG_TYPE_REP_SeqTypeRep: {
      if (TpR1.Is(TAG_TYPE_REP_SeqTypeRep)) {
        return IsSubType (TpR1.GetRecord(pos_REP_SeqTypeRep_elemtp),
                          TpR2.GetRecord(pos_REP_SeqTypeRep_elemtp),
                          assump);
      }
      else {
        return false;
      }
    }
    case TAG_TYPE_REP_ProductTypeRep: {
      if (TpR1.Is(TAG_TYPE_REP_ProductTypeRep)) {
        const SEQ<TYPE_REP_TypeRep> & PTp1 (TpR1.GetSequence(pos_REP_ProductTypeRep_tps));
        const SEQ<TYPE_REP_TypeRep> & PTp2 (TpR2.GetSequence(pos_REP_ProductTypeRep_tps));

        bool forall (PTp1.Length() == PTp2.Length());
        size_t len_PTp1 = PTp1.Length();
        for (size_t index = 1; (index <= len_PTp1) && forall; index++) {
          forall = IsSubType(PTp1[index], PTp2[index], assump);
        }
        return forall;
      }
      else {
        return false;
      }
    }
    case TAG_TYPE_REP_CompositeTypeRep: {
      if (TpR1.Is(TAG_TYPE_REP_CompositeTypeRep)) {
        const TYPE_AS_Name & LhsName (TpR1.GetRecord(pos_REP_CompositeTypeRep_nm));
        const TYPE_AS_Name & RhsName (TpR2.GetRecord(pos_REP_CompositeTypeRep_nm));
        const SEQ<TYPE_REP_FieldRep> & LhsFields (TpR1.GetSequence(pos_REP_CompositeTypeRep_fields));
        const SEQ<TYPE_REP_FieldRep> & RhsFields (TpR2.GetSequence(pos_REP_CompositeTypeRep_fields));

        if (IsEmptyName (RhsName) && (RhsFields.IsEmpty())) {
          return true;
        }
        else {
#ifdef VDMSL
          TYPE_AS_Name lnm (LhsName.GetSequence(pos_AS_Name_ids).Length() > 1 ? LhsName
                                                                              : ExtName(GetCurMod(), LhsName));
          TYPE_AS_Name rnm (RhsName.GetSequence(pos_AS_Name_ids).Length() > 1 ? RhsName
                                                                              : ExtName(GetCurMod(), RhsName));
#endif // VDMSL
#ifdef VDMPP
          TYPE_AS_Name lnm (LhsName.GetSequence(pos_AS_Name_ids).Length() > 1 ? LhsName
                                                                              : ExtName(GetCurClass(), LhsName));
          TYPE_AS_Name rnm (RhsName.GetSequence(pos_AS_Name_ids).Length() > 1 ? RhsName
                                                                              : ExtName(GetCurClass(), RhsName));
#endif // VDMPP
          bool forall = (lnm == rnm) && (LhsFields.Length() == RhsFields.Length());
          size_t len_LhsFields = LhsFields.Length();
          for (size_t i = 1; (i <= len_LhsFields) && forall; i++) {
            forall = IsSubType(LhsFields[i].GetRecord(pos_REP_FieldRep_tp),
                               RhsFields[i].GetRecord(pos_REP_FieldRep_tp), assump);
          }
          return forall;
        }
      }
      else {
        return false;
      }
    }
    case TAG_TYPE_REP_PartialFnTypeRep: {
      if (TpR1.Is(TAG_TYPE_REP_PartialFnTypeRep) || TpR1.Is(TAG_TYPE_REP_TotalFnTypeRep)) {
        const SEQ<TYPE_REP_TypeRep> & LhsDom (TpR1.GetSequence(1));
        const SEQ<TYPE_REP_TypeRep> & RhsDom (TpR2.GetSequence(1));
        const TYPE_REP_TypeRep & LhsRng (TpR1.GetRecord(2));
        const TYPE_REP_TypeRep & RhsRng (TpR2.GetRecord(2));

        bool forall (LhsDom.Length() == RhsDom.Length ());
        size_t len_LhsDom = LhsDom.Length();
        for (size_t index = 1; (index <= len_LhsDom) && forall; index++) {
          forall = IsSubType(LhsDom[index], RhsDom[index], assump);
        }
        return forall && IsSubType(LhsRng, RhsRng, assump);
      }
      else {
        return false;
      }
    }
    case TAG_TYPE_REP_TotalFnTypeRep: {
      if (TpR1.Is(TAG_TYPE_REP_TotalFnTypeRep)) {
        const SEQ<TYPE_REP_TypeRep> & LhsDom (TpR1.GetSequence(1));
        const SEQ<TYPE_REP_TypeRep> & RhsDom (TpR2.GetSequence(1));
        const TYPE_REP_TypeRep & LhsRng (TpR1.GetRecord(2));
        const TYPE_REP_TypeRep & RhsRng (TpR2.GetRecord(2));

        bool forall (LhsDom.Length() == RhsDom.Length ());
        size_t len_LhsDom = LhsDom.Length();
        for (size_t index = 1; (index <= len_LhsDom) && forall; index++) {
          forall = IsSubType(LhsDom[index], RhsDom[index], assump);
        }
        return forall && IsSubType(LhsRng, RhsRng, assump);
      }
      else {
        return false;
      }
    }
    case TAG_TYPE_REP_GeneralMapTypeRep: {
      if (TpR1.Is(TAG_TYPE_REP_GeneralMapTypeRep) || TpR1.Is(TAG_TYPE_REP_InjectiveMapTypeRep)) {
        const TYPE_REP_TypeRep & LhsDom (TpR1.GetRecord(1));
        const TYPE_REP_TypeRep & LhsRng (TpR1.GetRecord(2));
        const TYPE_REP_TypeRep & RhsDom (TpR2.GetRecord(1));
        const TYPE_REP_TypeRep & RhsRng (TpR2.GetRecord(2));
        return IsSubType(LhsDom, RhsDom, assump) && IsSubType(LhsRng, RhsRng, assump);
      }
      else {
        return false;
      }
    }
    case TAG_TYPE_REP_InjectiveMapTypeRep: {
      if (TpR1.Is(TAG_TYPE_REP_InjectiveMapTypeRep)) {
        const TYPE_REP_TypeRep & LhsDom (TpR1.GetRecord(1));
        const TYPE_REP_TypeRep & LhsRng (TpR1.GetRecord(2));
        const TYPE_REP_TypeRep & RhsDom (TpR2.GetRecord(1));
        const TYPE_REP_TypeRep & RhsRng (TpR2.GetRecord(2));
        return IsSubType(LhsDom, RhsDom, assump) && IsSubType(LhsRng, RhsRng, assump);
      }
      else if (TpR1.Is(TAG_TYPE_REP_GeneralMapTypeRep)) {
        const TYPE_REP_TypeRep & LhsDom (TpR1.GetRecord(pos_REP_GeneralMapTypeRep_mapdom));
        const TYPE_REP_TypeRep & LhsRng (TpR1.GetRecord(pos_REP_GeneralMapTypeRep_maprng));
        const TYPE_REP_TypeRep & RhsDom (TpR2.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom));
        const TYPE_REP_TypeRep & RhsRng (TpR2.GetRecord(pos_REP_InjectiveMapTypeRep_maprng));
        return IsSubType(LhsDom, RhsDom, assump) &&
               IsSubType(LhsRng, RhsRng, assump) &&
               IsOneValueType(POS, LhsDom);
      }
      else {
        return false;
      }
    }
    case TAG_TYPE_REP_QuoteTypeRep: {
      if (TpR1.Is(TAG_TYPE_REP_QuoteTypeRep)) {
        return (TpR1.GetRecord(pos_REP_QuoteTypeRep_lit) == TpR2.GetRecord(pos_REP_QuoteTypeRep_lit));
      }
      else {
        return false;
      }
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> Rhs (TpR2.GetSet(pos_REP_UnionTypeRep_tps));
      bool exists (false);
      Generic tp;
      for (bool bb = Rhs.First(tp); bb && !exists; bb = Rhs.Next(tp)) {
        exists = IsSubType(TpR1, tp, assump);
      }
      return exists;
    }
#ifdef VDMPP
    case TAG_TYPE_REP_ObjRefTypeRep: {
      if (TpR1.Is(TAG_TYPE_REP_ObjRefTypeRep)) {
        if (IsEmptyName (TpR2.GetRecord(pos_REP_ObjRefTypeRep_nm))) {
          return true;
        }
        else {
          return IsSubClass (TpR1.GetRecord(pos_REP_ObjRefTypeRep_nm),
                             TpR2.GetRecord(pos_REP_ObjRefTypeRep_nm));
        }
      }
      else {
        return false;
      }
    }
    case TAG_TYPE_REP_OverTypeRep: {
      SET<TYPE_REP_TypeRep> Rhs (TpR2.GetSet(pos_REP_OverTypeRep_tps));
      bool exists (false);
      Generic tp;
      for (bool bb = Rhs.First(tp); bb && !exists; bb = Rhs.Next(tp)) {
        exists = IsSubType(TpR1, tp, assump);
      }
      return exists;
    }
#endif //VDMPP
    case TAG_TYPE_REP_OpTypeRep: {
      if (TpR1.Is(TAG_TYPE_REP_OpTypeRep)) {
        const SEQ<TYPE_REP_TypeRep> & LhsDom (TpR1.GetSequence(pos_REP_OpTypeRep_Dom));
        const SEQ<TYPE_REP_TypeRep> & RhsDom (TpR2.GetSequence(pos_REP_OpTypeRep_Dom));
        const TYPE_REP_TypeRep & LhsRng (TpR1.GetRecord(pos_REP_OpTypeRep_Rng));
        TYPE_REP_TypeRep RhsRng (TpR2.GetRecord(pos_REP_OpTypeRep_Rng));

        if (RhsRng.Is (TAG_TYPE_REP_ExitTypeRep)) {
          RhsRng = rep_unittp;
        }
        else {
          RhsRng = MergeTypes(RhsRng, mk_REP_ExitTypeRep(rep_alltp));
        }
        bool forall (LhsDom.Length() == RhsDom.Length ());
        size_t len_LhsDom = LhsDom.Length();
        for (size_t index = 1; (index <= len_LhsDom) && forall; index++) {
          forall = IsSubType(LhsDom[index], RhsDom[index], assump);
        }
        return forall && IsSubType(LhsRng, RhsRng, assump);
      }
      else {
        return false;
      }
    }
    case TAG_TYPE_REP_TypeVarRep: {
      if (TpR1.Is(TAG_TYPE_REP_TypeVarRep)) {
        return this->MeasureCheck();
      }
      else {
        return false;
      }
    }
    default: {
      return false;
    } 
  }
  return false; // dummy
}

// IsOverlapping
// TpR1 : REP`TypeRep
// TpR2 : REP`TypeRep
// assump : set of (REP`TypeNameRep * REP`TypeNameRep)
// ==> bool
bool StatSem::IsOverlapping (const TYPE_REP_TypeRep & TpR1, const TYPE_REP_TypeRep & TpR2, const Set & assump)
{
  if (TpR1 == TpR2) {
     return true;
  }
  switch (TpR1.GetTag()) {
    case TAG_TYPE_REP_ExitTypeRep:
    case TAG_TYPE_REP_AllTypeRep: { return true; }
    case TAG_TYPE_REP_InvTypeRep: {
      return IsOverlapping (TpR1.GetRecord(pos_REP_InvTypeRep_shape), TpR2, assump);
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      const SET<TYPE_REP_TypeRep> & Lhs (TpR1.GetSet(pos_REP_UnionTypeRep_tps));

      SET<TYPE_REP_TypeRep> tmpS (mk_set(mk_REP_EmptySeqTypeRep(rep_alltp),
                                         mk_REP_EmptySetTypeRep(rep_alltp),
                                         mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp),
                                         mk_REP_UnionTypeRep(mk_set(mk_REP_EmptySeqTypeRep(rep_alltp),
                                                                    mk_REP_EmptySetTypeRep(rep_alltp))),
                                         mk_REP_UnionTypeRep(mk_set(mk_REP_EmptySeqTypeRep(rep_alltp),
                                                                    mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp))),
                                         mk_REP_UnionTypeRep(mk_set(mk_REP_EmptySetTypeRep(rep_alltp),
                                                                    mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp))),
                                         mk_REP_UnionTypeRep(mk_set(mk_REP_EmptySeqTypeRep(rep_alltp),
                                                                    mk_REP_EmptySetTypeRep(rep_alltp),
                                                                    mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp)))));
                                       
      if ((Lhs.Intersect(tmpS).InSet(TpR2))) {
        return true;
      }
 //     SET<TYPE_REP_TypeRep> tps (Lhs.Diff(mk_set(rep_emptyseq, rep_emptyset, rep_emptymap)));
      SET<TYPE_REP_TypeRep> tps (Lhs.Diff(tmpS));

      bool exists = tps.IsEmpty();
      Generic tp;
      for (bool bb = tps.First(tp); bb && !exists; bb = tps.Next(tp)) {
        exists = IsOverlapping(tp, TpR2, assump);
      }
      return exists;
    }
    case TAG_TYPE_REP_TypeNameRep: {
      if (TpR2.Is(TAG_TYPE_REP_TypeNameRep)) {
        if (assump.InSet(mk_(TpR1, TpR2)) || assump.InSet(mk_(TpR2, TpR1))) {
          return true;
        }
      } 
      if (IsUnionRecursive(TpR1)) {
        return true;
      }
      else {
#ifdef VDMSL
        Generic newtp (LookUpTypeName(TpR1.GetRecord(pos_REP_TypeNameRep_nm)));
#endif // VDMSL
#ifdef VDMPP
        Generic newtp (StripAccessType (LookUpTypeName(TpR1.GetRecord(pos_REP_TypeNameRep_nm), true)));
#endif // VDMPP
        Set new_assump (assump);
        if (TpR2.Is(TAG_TYPE_REP_TypeNameRep)) {
          new_assump.Insert(mk_(TpR1, TpR2)).Insert(mk_(TpR2, TpR1));
        }
        return (newtp.IsNil() ? false : IsOverlapping (newtp, TpR2, new_assump));
      }
    }
#ifdef VDMPP
    case TAG_TYPE_REP_OverTypeRep: {
      SET<TYPE_REP_TypeRep> lhs (TpR1.GetSet(pos_REP_OverTypeRep_tps));
      bool exists = false;
      Generic tp;
      for (bool bb = lhs.First(tp); bb && !exists; bb = lhs.Next(tp)) {
        exists = IsOverlapping(tp, TpR2, assump);
      }
      return exists;
    }
#endif // VDMPP
    case TAG_TYPE_REP_TypeVarRep: { return true; }
    case TAG_TYPE_REP_PolyTypeRep: {
      return IsOverlapping(TpR1.GetRecord(pos_REP_PolyTypeRep_tp), TpR2, assump);
    }
  }

  switch (TpR2.GetTag()) {
    case TAG_TYPE_REP_AllTypeRep: { return true; }
    case TAG_TYPE_REP_TypeNameRep: {
      if (IsUnionRecursive(TpR2)) {
        return true;
      }
      else {
#ifdef VDMSL
        Generic newtp (LookUpTypeName(TpR2.GetRecord(pos_REP_TypeNameRep_nm)));
#endif // VDMSL
#ifdef VDMPP
        Generic newtp (StripAccessType (LookUpTypeName(TpR2.GetRecord(pos_REP_TypeNameRep_nm), true)));
#endif // VDMPP
        return (newtp.IsNil() ? false : IsOverlapping (TpR1, newtp, assump));
      }
    }
    case TAG_TYPE_REP_NumericTypeRep: { return TpR1.Is(TAG_TYPE_REP_NumericTypeRep); }
    case TAG_TYPE_REP_SetTypeRep: {
      if (TpR1.Is(TAG_TYPE_REP_SetTypeRep)) {
        return IsOverlapping (TpR1.GetRecord(pos_REP_SetTypeRep_elemtp),
                              TpR2.GetRecord(pos_REP_SetTypeRep_elemtp),
                              assump);
      }
      else {
        return false;
      }
    }
    case TAG_TYPE_REP_SeqTypeRep: {
      if (TpR1.Is(TAG_TYPE_REP_SeqTypeRep)) {
        return IsOverlapping (TpR1.GetRecord(pos_REP_SeqTypeRep_elemtp),
                              TpR2.GetRecord(pos_REP_SeqTypeRep_elemtp),
                              assump);
      }
      else {
        return false;
      }
    }
    case TAG_TYPE_REP_ProductTypeRep: {
      if (TpR1.Is(TAG_TYPE_REP_ProductTypeRep)) {
        const SEQ<TYPE_REP_TypeRep> & PTp1 (TpR1.GetSequence(pos_REP_ProductTypeRep_tps));
        const SEQ<TYPE_REP_TypeRep> & PTp2 (TpR2.GetSequence(pos_REP_ProductTypeRep_tps));

        bool forall = (PTp1.Length() == PTp2.Length());
        size_t len_PTp1 = PTp1.Length();
        for (size_t i = 1; (i <= len_PTp1) && forall; i++) {
          forall = IsOverlapping(PTp1[i], PTp2[i], assump);
        }
        return forall;
      }
      else {
        return false;
      }
    }
    case TAG_TYPE_REP_CompositeTypeRep: {
      if (TpR1.Is(TAG_TYPE_REP_CompositeTypeRep)) {
        const TYPE_AS_Name & LhsName (TpR1.GetRecord(pos_REP_CompositeTypeRep_nm));
        const TYPE_AS_Name & RhsName (TpR2.GetRecord(pos_REP_CompositeTypeRep_nm));
        const SEQ<TYPE_REP_FieldRep> & LhsFields (TpR1.GetSequence(pos_REP_CompositeTypeRep_fields));
        const SEQ<TYPE_REP_FieldRep> & RhsFields (TpR2.GetSequence(pos_REP_CompositeTypeRep_fields));

        if (IsEmptyName (RhsName) && (RhsFields.IsEmpty())) {
          return true;
        }
        else {
#ifdef VDMSL
          TYPE_AS_Name lnm (LhsName.GetSequence(pos_AS_Name_ids).Length() > 1 ? LhsName
                                                                              : ExtName(GetCurMod(), LhsName));
          TYPE_AS_Name rnm (RhsName.GetSequence(pos_AS_Name_ids).Length() > 1 ? RhsName
                                                                              : ExtName(GetCurMod(), RhsName));
#endif // VDMSL
#ifdef VDMPP
          TYPE_AS_Name lnm (LhsName.GetSequence(pos_AS_Name_ids).Length() > 1 ? LhsName
                                                                              : ExtName(GetCurClass(), LhsName));
          TYPE_AS_Name rnm (RhsName.GetSequence(pos_AS_Name_ids).Length() > 1 ? RhsName
                                                                              : ExtName(GetCurClass(), RhsName));
#endif // VDMPP
          bool forall = (lnm == rnm) && (LhsFields.Length() == RhsFields.Length());
          size_t len_LhsFields = LhsFields.Length();
          for (size_t i = 1; (i <= len_LhsFields) && forall; i++) {
            forall = IsOverlapping(LhsFields[i].GetRecord(pos_REP_FieldRep_tp),
                                   RhsFields[i].GetRecord(pos_REP_FieldRep_tp),
                                   assump);
          }
          return forall;
        }
      }
      else {
        return false;
      }
    }
    case TAG_TYPE_REP_PartialFnTypeRep:
    case TAG_TYPE_REP_TotalFnTypeRep: {
      if (TpR1.Is(TAG_TYPE_REP_PartialFnTypeRep) || TpR1.Is(TAG_TYPE_REP_TotalFnTypeRep)) {
        const SEQ<TYPE_REP_TypeRep> & LhsDom (TpR1.GetSequence(1));
        const SEQ<TYPE_REP_TypeRep> & RhsDom (TpR2.GetSequence(1));
        const TYPE_REP_TypeRep & LhsRng (TpR1.GetRecord(2));
        const TYPE_REP_TypeRep & RhsRng (TpR2.GetRecord(2));

        bool forall (LhsDom.Length() == RhsDom.Length ());
        size_t len_LhsDom = LhsDom.Length();
        for (size_t index = 1; (index <= len_LhsDom) && forall; index++) {
          forall = IsOverlapping(LhsDom[index], RhsDom[index], assump);
        }
        return forall && IsOverlapping(LhsRng, RhsRng, assump);
      }
      else {
        return false;
      }
    }
    case TAG_TYPE_REP_GeneralMapTypeRep:
    case TAG_TYPE_REP_InjectiveMapTypeRep: {
      if (TpR1.Is(TAG_TYPE_REP_GeneralMapTypeRep) || TpR1.Is(TAG_TYPE_REP_InjectiveMapTypeRep)) {
        return IsOverlapping(TpR1.GetRecord(1), TpR2.GetRecord(1), assump) &&
               IsOverlapping(TpR1.GetRecord(2), TpR2.GetRecord(2), assump);
      }
      else {
        return false;
      }
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return IsOverlapping (TpR1, TpR2.GetRecord(pos_REP_InvTypeRep_shape), assump);
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      const SET<TYPE_REP_TypeRep> & Rhs (TpR2.GetSet(pos_REP_UnionTypeRep_tps));

      SET<TYPE_REP_TypeRep> tmpS (mk_set(mk_REP_EmptySeqTypeRep(rep_alltp),
                                         mk_REP_EmptySetTypeRep(rep_alltp),
                                         mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp),
                                         mk_REP_UnionTypeRep(mk_set(mk_REP_EmptySeqTypeRep(rep_alltp),
                                                                    mk_REP_EmptySetTypeRep(rep_alltp))),
                                         mk_REP_UnionTypeRep(mk_set(mk_REP_EmptySeqTypeRep(rep_alltp),
                                                                    mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp))),
                                         mk_REP_UnionTypeRep(mk_set(mk_REP_EmptySetTypeRep(rep_alltp),
                                                                    mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp))),
                                         mk_REP_UnionTypeRep(mk_set(mk_REP_EmptySeqTypeRep(rep_alltp),
                                                                    mk_REP_EmptySetTypeRep(rep_alltp),
                                                                    mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp)))));
      if ((Rhs.Intersect(tmpS).InSet(TpR1))) {
        switch (TpR1.GetTag()) {
          case TAG_TYPE_REP_EmptySeqTypeRep: {
            if (TpR1.GetRecord(pos_REP_EmptySeqTypeRep_elemtp) != rep_alltp) {
              SET<TYPE_REP_TypeRep> tp_s (Rhs.Intersect(tmpS));
              Generic e;
              for (bool bb = tp_s.First(e); bb; bb = tp_s.Next(e)) {
                if (e.Is(TAG_TYPE_REP_EmptySeqTypeRep)) {
                  return IsOverlapping(TpR1.GetRecord(pos_REP_EmptySeqTypeRep_elemtp),
                                       Record(e).GetRecord(pos_REP_EmptySeqTypeRep_elemtp), assump);
                }
              }
            }
          }
        }
        return true;
      }

      // And then the or part
      SET<TYPE_REP_TypeRep> tps (Rhs.Diff(tmpS));

      bool exists = false;
      Generic tp;
      //for (bool bb = Rhs.First(tp); bb && !exists; bb = Rhs.Next(tp))
      for (bool bb = tps.First(tp); bb && !exists; bb = tps.Next(tp)) {
        exists = IsOverlapping(TpR1, tp, assump);
      }
      return exists;
    }
#ifdef VDMPP
    case TAG_TYPE_REP_ObjRefTypeRep: {
      if (TpR1.Is(TAG_TYPE_REP_ObjRefTypeRep)) {
        if (IsEmptyName (TpR1.GetRecord(pos_REP_ObjRefTypeRep_nm))) {
          return true;
        }
        else if (IsEmptyName (TpR2.GetRecord(pos_REP_ObjRefTypeRep_nm))) {
          return true;
        }
        else if (IsSubClass (TpR1.GetRecord(pos_REP_ObjRefTypeRep_nm), TpR2.GetRecord(pos_REP_ObjRefTypeRep_nm))) {
          return true;
        }
        else {
          return IsSubClass (TpR2.GetRecord(pos_REP_ObjRefTypeRep_nm), TpR1.GetRecord(pos_REP_ObjRefTypeRep_nm));
        }
      }
      else {
        return false;
      }
    }
    case TAG_TYPE_REP_OverTypeRep: {
      SET<TYPE_REP_TypeRep> rhs (TpR2.GetSet(pos_REP_OverTypeRep_tps));
      bool exists = false;
      Generic tp;
      for (bool bb = rhs.First(tp); bb && !exists; bb = rhs.Next(tp)) {
        exists = IsOverlapping(TpR1, tp, assump);
      }
      return exists;
    }
#endif //VDMPP
    case TAG_TYPE_REP_OpTypeRep: {
      if (TpR1.Is(TAG_TYPE_REP_OpTypeRep)) {
        const SEQ<TYPE_REP_TypeRep> & LhsDom (TpR1.GetSequence(pos_REP_OpTypeRep_Dom));
        const SEQ<TYPE_REP_TypeRep> & RhsDom (TpR2.GetSequence(pos_REP_OpTypeRep_Dom));
        const TYPE_REP_TypeRep & LhsRng (TpR1.GetRecord(pos_REP_OpTypeRep_Rng));
        TYPE_REP_TypeRep RhsRng (TpR2.GetRecord(pos_REP_OpTypeRep_Rng));

        if (RhsRng.Is (TAG_TYPE_REP_ExitTypeRep)) {
          RhsRng = rep_unittp;
        }
        bool forall (LhsDom.Length() == RhsDom.Length ());
        size_t len_LhsDom = LhsDom.Length();
        for (size_t index = 1; (index <= len_LhsDom) && forall; index++) {
          forall = IsOverlapping(LhsDom[index], RhsDom[index], assump);
        }
        return forall && IsOverlapping(LhsRng, RhsRng, assump); 
      }
#ifdef VICE
      else if (TpR1.Is(TAG_TYPE_REP_AnyOpTypeRep)) {
        return true;
      }
#endif // VICE
      else {
        return false;
     }
    }
#ifdef VICE
    case TAG_TYPE_REP_AnyOpTypeRep: { return TpR1.Is(TAG_TYPE_REP_OpTypeRep); }
#endif // VICE
    case TAG_TYPE_REP_ExitTypeRep: {
      if (TpR1.Is(TAG_TYPE_REP_ExitTypeRep)) {
        return IsOverlapping(TpR1.GetRecord (pos_REP_ExitTypeRep_tp),
                             TpR2.GetRecord (pos_REP_ExitTypeRep_tp), assump);
      }
      else {
        return false;
      }
    }
    case TAG_TYPE_REP_TypeVarRep: { return true; }
    case TAG_TYPE_REP_PolyTypeRep: {
      return IsOverlapping(TpR1, TpR2.GetRecord(pos_REP_PolyTypeRep_tp), assump);
    }
    default: { return false; }
  }
  return false;
}

// IsEquivalent
// i : Ind
// tp1 : REP`TypeRep
// tp2 : REP`TypeRep
// ==> bool
bool StatSem::IsEquivalent (const Int & i, const TYPE_REP_TypeRep & tp1, const TYPE_REP_TypeRep & tp2)
{
  return IsSubType (tp1, tp2, Set()) && IsSubType (tp2, tp1, Set());
}


// IsOneValueType
// i : Ind
// tp : REP`TypeRep
// ==> bool
bool StatSem::IsOneValueType (const Int & i, const TYPE_REP_TypeRep & tp)
{
  return IsOneValueTypeAux (i, tp, SET<TYPE_REP_TypeRep>());
}


// IsOneValueTypeAux
// i : Ind
// tp : REP`TypeRep
// tps : set of REP`TypeRep
// ==> bool
bool StatSem::IsOneValueTypeAux (const Int & i, const TYPE_REP_TypeRep & tp, const SET<TYPE_REP_TypeRep> & tps)
{
  if (tps.InSet(tp)) {
    return true;
  }
  else {
    Set new_tps (tps);
    new_tps.Insert(tp);
    switch (tp.GetTag()) {
      case TAG_TYPE_REP_TypeNameRep: {
        if (IsUnionRecursive(tp)) {
          return i == Int (POS);
        }
        else {
#ifdef VDMSL
          Generic newtp (LookUpTypeName(tp.GetRecord(pos_REP_TypeNameRep_nm)));
          if (newtp.IsNil()) {
#endif // VDMSL
#ifdef VDMPP
          Generic newtp_q (LookUpTypeName(tp.GetRecord(pos_REP_TypeNameRep_nm), true));
          Generic newtp (StripAccessType(newtp_q));
          if (newtp.IsNil() || CheckAccessCurClass (newtp_q).IsNil ()) {
#endif // VDMPP
            return false;
          }
          else {
            return IsOneValueTypeAux (i, newtp, new_tps);
          }
        }
      }
      case TAG_TYPE_REP_InvTypeRep: {
        return ((i == POS) && !IsEmpty(i, tp.GetRecord(pos_REP_InvTypeRep_shape)));
      }
      case TAG_TYPE_REP_QuoteTypeRep: {
        return true;
      }
      case TAG_TYPE_REP_CompositeTypeRep: {
        const SEQ<TYPE_REP_FieldRep> & f_l (tp.GetSequence(pos_REP_CompositeTypeRep_fields));
        bool forall = true;
        size_t len_f_l = f_l.Length();
        for (size_t idx = 1; (idx <= len_f_l) && forall; idx++) {
          forall = IsOneValueTypeAux(i, f_l[idx].GetRecord(pos_REP_FieldRep_tp), new_tps);
        }
        return forall;
      }
      case TAG_TYPE_REP_UnionTypeRep: {
        SET<TYPE_REP_TypeRep> t_s (tp.GetSet(pos_REP_UnionTypeRep_tps));
        bool exists (false);
        Generic t;
        for (bool bb = t_s.First(t); bb && !exists; bb = t_s.Next(t)) {
          if (IsOneValueTypeAux(i, t, new_tps)) {
            SET<TYPE_REP_TypeRep> t_ss (t_s);
            bool forall = true;
            Generic t_;
            for (bool bb = t_ss.First(t_); bb && forall; bb = t_ss.Next(t_)) {
              forall = (IsEquivalent(i, t, t_) || IsEmpty(i, t_));
            }
            exists = forall;
          }
        }
        return exists;
      }
      case TAG_TYPE_REP_ProductTypeRep: {
        const SEQ<TYPE_REP_TypeRep> & t_l (tp.GetSequence(pos_REP_ProductTypeRep_tps));
        bool forall (true);
        size_t len_t_l = t_l.Length();
        for (size_t idx = 1; (idx <= len_t_l) && forall; idx++) {
          forall = IsOneValueTypeAux(i, t_l[idx], new_tps);
        }
        return forall;
      }
      case TAG_TYPE_REP_SetTypeRep: {
        return IsEmpty (i, tp.GetRecord(pos_REP_SetTypeRep_elemtp));
      }
      case TAG_TYPE_REP_SeqTypeRep: {
        return IsEmpty (i, tp.GetRecord(pos_REP_SeqTypeRep_elemtp));
      }
      case TAG_TYPE_REP_GeneralMapTypeRep: {
        return (IsEmpty (i, tp.GetRecord(pos_REP_GeneralMapTypeRep_mapdom))) ||
               (IsEmpty (i, tp.GetRecord(pos_REP_GeneralMapTypeRep_maprng)));
      }
      case TAG_TYPE_REP_InjectiveMapTypeRep: {
        return (IsEmpty (i, tp.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom))) ||
               (IsEmpty (i, tp.GetRecord(pos_REP_InjectiveMapTypeRep_maprng)));
      }
      case TAG_TYPE_REP_TotalFnTypeRep: {
        const SEQ<TYPE_REP_TypeRep> & d_l (tp.GetSequence(pos_REP_TotalFnTypeRep_fndom));
        const TYPE_REP_TypeRep & r (tp.GetRecord(pos_REP_TotalFnTypeRep_fnrng));

        bool b1 (true), b2 (true);
        size_t len_d_l = d_l.Length();
        for (size_t idx = 1; idx <= len_d_l; idx++) {
          const TYPE_REP_TypeRep & d (d_l[idx]);
          b1 = b1 && (IsEmpty (i, d) || (IsEmpty (i, r)));
          b2 = b2 && (IsOneValueTypeAux (i, d, new_tps) && (IsOneValueTypeAux (i, r, new_tps)));
        }
        b1 = b1 || b2;
        return b1;
      }
      case TAG_TYPE_REP_EmptySeqTypeRep:
      case TAG_TYPE_REP_EmptySetTypeRep:
      case TAG_TYPE_REP_EmptyMapTypeRep: {
        return true;
      }
      case TAG_TYPE_REP_BooleanTypeRep:
      case TAG_TYPE_REP_NumericTypeRep:
      case TAG_TYPE_REP_TokenTypeRep:
      case TAG_TYPE_REP_CharTypeRep: {
        return false;
      }
      default: {
        return false;
      }
    }
  }
}

#endif //!NOSS

