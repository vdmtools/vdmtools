/* *  * WHAT
*  *    Evaluation functions for expressions in interpreter.
*  * FILE
*  *    $Source: /mnt/cvsroot/toolbox/code/eval/evalexpr.cc,v $
*  * VERSION
*  *    $Revision: 1.164 $
*  * DATE
*  *    $Date: 2006/09/22 11:14:03 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    VDM-SL Toolbox
*  * STATUS
*  *    Under development.
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
*  * SPECIFICATION VERSION
*  *    evalexpr.cc: Implementation of eval_expr.vdm 1.76
***/

#include <math.h>
#include "evalexpr.h"
#include "libman.h"
#include "freevars.h"
#include "astaux.h"
#include "tbdebug.h"
#include "pattern.h"
#include "rt_info.h"
#include "rt_errmsg.h"
#include "semvalues.h"
#include "asquotes.h"
#include "tbutils.h"
#include "RTERR.h"
#include "evalaux.h"
#include "intconvquotes.h"
#include "settings.h"

// EvalSetRangeExpr
// lb_v : SEM`VAL
// ub_v : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalSetRangeExpr (const TYPE_SEM_VAL & lb_v, const TYPE_SEM_VAL & ub_v)
{
  if (lb_v.Is(TAG_TYPE_SEM_NUM) && ub_v.Is(TAG_TYPE_SEM_NUM))
  {
    int64_t lb = lb_v.GetReal(pos_SEM_NUM_v).Ceil().GetIntValue();
    int64_t ub = ub_v.GetReal(pos_SEM_NUM_v).Floor().GetIntValue();
    SET<TYPE_SEM_VAL> res_sv;
    for (int64_t d = lb; d <= ub; d++)
      res_sv.Insert (mk_SEM_NUM(Real(d)));
    return mk_SEM_SET(res_sv);
  }
  else if (!lb_v.Is(TAG_TYPE_SEM_NUM))
    return RTERR::ErrorVal(L"EvalSetRangeExpr", RTERR_LOWER_BOUND_NOT_A_NUMBER, lb_v, Nil(), Sequence());
  else
    return RTERR::ErrorVal(L"EvalSetRangeExpr", RTERR_UPPER_BOUND_NOT_A_NUMBER, ub_v, Nil(), Sequence());
}

/** Sequence Expressions part ***********************************************/
/** Reference : Doc. iptes-ifad-12-v1.0  ************************************/
/****************************************************************************/

// EvalSubSequenceExpr
// seq_v : SEM`VAL
// from_v : SEM`VAL
// to_v : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalSubSequenceExpr (const TYPE_SEM_VAL & seq_v,
                                        const TYPE_SEM_VAL & from_v,
                                        const TYPE_SEM_VAL & to_v)
{
  if (seq_v.Is(TAG_TYPE_SEM_SEQ) && from_v.Is(TAG_TYPE_SEM_NUM) && to_v.Is(TAG_TYPE_SEM_NUM))
  {
    int64_t from = from_v.GetReal(pos_SEM_NUM_v).Ceil().GetIntValue();
    int64_t to = to_v.GetReal(pos_SEM_NUM_v).Floor().GetIntValue();
    return mk_SEM_SEQ(seq_v.GetSequence(pos_SEM_SEQ_v).SubSequence(from, to));
  }
  else if (!seq_v.Is(TAG_TYPE_SEM_SEQ))
    return RTERR::ErrorVal (L"EvalSubSequenceExpr", RTERR_SEQ_EXPECTED, seq_v, Nil(), Sequence());
  else if (!from_v.Is(TAG_TYPE_SEM_NUM))
    return RTERR::ErrorVal (L"EvalSubSequenceExpr", RTERR_INT_EXPECTED, from_v, Nil(), Sequence());
  else // !to_v.Is(TAG_TYPE_SEM_NUM)
    return RTERR::ErrorVal (L"EvalSubSequenceExpr", RTERR_INT_EXPECTED, to_v, Nil(), Sequence());
}

// EvalSeqModifyMapOverrideExpr
// seqmap_v : SEM`VAL
// map_v : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalSeqModifyMapOverrideExpr (const TYPE_SEM_VAL & seqmap_v, const TYPE_SEM_VAL & map_v)
{
  switch (map_v.GetTag()) {
    case TAG_TYPE_SEM_MAP: {
      switch(seqmap_v.GetTag()) {
        case TAG_TYPE_SEM_MAP:   { return EvalMapOverrideExpr (seqmap_v, map_v); }
        case TAG_TYPE_SEM_SEQ:   { return EvalSeqModifyExpr (seqmap_v, map_v); }
/*
        case TAG_TYPE_SEM_TUPLE: { return EvalTupleModifyExpr (seqmap_v, map_v); }
//
        case TAG_TYPE_SEM_SET: {
          if (!map_v.Is(TAG_TYPE_SEM_MAP)) {
            return RTERR::ErrorVal (L"EvalSeqModifyMapOverrideExpr", RTERR_MAP_EXPECTED, map_v, Nil(), Sequence());
          }
          else {
            // ( s \ dom m) union rng (s <: m)
            const Map & m (map_v.GetMap(pos_SEM_MAP_v));
            const Set & s (seqmap_v.GetSet(pos_SEM_SET_v));
            return mk_SEM_SET(s.Diff(m.Dom()).Union(m.DomRestrictedTo(s).Rng()));
          }
          break;
        }
//
*/
        default: {
          return RTERR::ErrorVal (L"EvalSeqModifyMapOverrideExpr",
                                  RTERR_MAP_OR_SEQ_EXPECTED, seqmap_v, Nil(), Sequence());
        }
      }
    }
    default: {
      return RTERR::ErrorVal (L"EvalSeqModifyMapOverrideExpr", RTERR_MAP_EXPECTED, map_v, Nil(), Sequence());
    }
  }
}

// EvalMapOverrideExpr
// VAL1 : SEM`VAL
// VAL2 : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalMapOverrideExpr (const TYPE_SEM_VAL & VAL1, const TYPE_SEM_VAL & VAL2)
{
  return mk_SEM_MAP(VAL1.GetMap(pos_SEM_MAP_v).Override(VAL2.GetMap(pos_SEM_MAP_v)));
}

// EvalSeqModifyExpr
// seq_v : SEM`VAL
// modifier_sv : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalSeqModifyExpr (const TYPE_SEM_VAL & seq_v, const TYPE_SEM_VAL & modifier_sv)
{
  SEQ<TYPE_SEM_VAL> res_lv (seq_v.GetSequence(pos_SEM_SEQ_v));
  const MAP<TYPE_SEM_VAL, TYPE_SEM_VAL> & modifiers (modifier_sv.GetMap(pos_SEM_MAP_v));

  SET<TYPE_SEM_VAL> dom_modifiers (modifiers.Dom()); 
  Generic d_v;
  for (bool bb = dom_modifiers.First (d_v); bb; bb = dom_modifiers.Next (d_v))
  {
    if (d_v.Is(TAG_TYPE_SEM_NUM) ? Record(d_v).GetReal(pos_SEM_NUM_v).IsNatOne() : false)
    {
      int64_t d = Record(d_v).GetReal(pos_SEM_NUM_v).GetIntValue();
      if (d <= res_lv.Length ()) {
        res_lv.ImpModify (d, modifiers[d_v]);
      }
      else
        return RTERR::ErrorVal (L"EvalSeqModify", RTERR_ILLEGAL_INDICES, d_v, Nil(), Sequence());
    }
    else
      return RTERR::ErrorVal (L"EvalSeqModify", RTERR_INT_EXPECTED, d_v, Nil(), Sequence());
  }
  return mk_SEM_SEQ(res_lv);
}

// EvalTupleModifyExpr
// seq_v : SEM`VAL
// modifier_sv : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalTupleModifyExpr (const TYPE_SEM_VAL & seq_v, const TYPE_SEM_VAL & modifier_sv)
{
  SEQ<TYPE_SEM_VAL> res_lv (seq_v.GetSequence(pos_SEM_TUPLE_v));
  const MAP<TYPE_SEM_VAL, TYPE_SEM_VAL> & modifiers (modifier_sv.GetMap(pos_SEM_MAP_v));

  SET<TYPE_SEM_VAL> dom_modifiers (modifiers.Dom()); 
  Generic d_v;
  for (bool bb = dom_modifiers.First (d_v); bb; bb = dom_modifiers.Next (d_v))
  {
    if (d_v.Is(TAG_TYPE_SEM_NUM) ? Record(d_v).GetReal(pos_SEM_NUM_v).IsNatOne() : false)
    {
      int64_t d = Record(d_v).GetReal(pos_SEM_NUM_v).GetIntValue();
      if (d <= res_lv.Length ()) {
        res_lv.ImpModify (d, modifiers[d_v]);
      }
      else
        return RTERR::ErrorVal (L"EvalSeqModify", RTERR_ILLEGAL_INDICES, d_v, Nil(), Sequence());
    }
    else
      return RTERR::ErrorVal (L"EvalSeqModify", RTERR_INT_EXPECTED, d_v, Nil(), Sequence());
  }
  return mk_SEM_TUPLE(res_lv);
}

/** Record Expressions part *************************************************/
/** Reference : Doc. iptes-ifad-12-v1.0  ************************************/
/****************************************************************************/

#ifdef VDMSL
// 'tag' is assumed to be a valid and expanded tag. This way we can
// avoid calling UniqueTagName and IsRecSel again.
// ConstructSEMRecFields
// tag : AS`Name
// val_l : seq of SEM`VAL
// ==> SEM`VAL
TYPE_DYNSEM_SEM_SemRecord EXPR::ConstructSEMRecFields(const TYPE_AS_Name & tag, const SEQ<TYPE_SEM_VAL> & val_l)
{
  // Remark the implementation differs from the specification here,
  // because of the different implementation of the semantic value of records.
  return SemRec::mk_SEM_REC(tag, val_l);
}

// EvalRecordConstructorExpr
// tag_o : AS`Name
// val_l : seq of SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalRecordConstructorExpr (const TYPE_AS_Name & tag, const SEQ<TYPE_SEM_VAL> & val_l) /*notconst*/
{
//  Tuple etn (AUX::ExtractTagName(tag)); // [AS`Name] * bool
  Tuple etn (theState().ExtractTagNameCached(tag)); // [AS`Name] * bool
  if (etn.GetBoolValue(2))
  {
    const TYPE_AS_Name & the_tag (etn.GetRecord(1));
//    Tuple lurs (AUX::LookUpRecSel(tag)); // bool * GLOBAL`RecSel
    Tuple lurs (theState().GetCachedRecSel(the_tag)); // bool * GLOBAL`RecSel
    //GLOBAL`RecSel =  nat * (map AS`Name to nat) * seq of AS`Type
    const Tuple & recsel (lurs.GetTuple(2)); // GLOBAL`RecSel
    if (val_l.Length () == recsel.GetIntValue (1))
    {
      TYPE_DYNSEM_SEM_SemRecord res_v (ConstructSEMRecFields(the_tag, val_l));

      if (Settings.DTC())
      {
//        Tuple itd (AUX::IsTypeDef(the_tag)); // bool * [AS`Type] * [AS`Invariant] * [AS`Equal] * [AS`Order] * [AS`Name]
        Tuple itd (theState().GetCachedTypeDef(the_tag)); // bool * [AS`Type] * [AS`Invariant] * [AS`Equal] * [AS`Order] * [AS`Name]
        if (itd.GetBoolValue(1)) {
          TYPE_AS_TypeName tnm;
          tnm.Init(the_tag, the_tag.GetInt(pos_AS_Name_cid));
          if (! theState().SubType (res_v, tnm)) {
            if (theState().RealSubType (res_v, tnm, false)) {
              return RTERR::ErrorVal (L"EvalRecordConstructorExpr", RTERR_TYPE_INV_BROKEN, res_v, tnm, Sequence());
            }
            else {
              return RTERR::ErrorVal (L"EvalRecordConstructorExpr", RTERR_TYPE_INCOMP, res_v, tnm, Sequence());
            }
          }
        }
        else {
          const SEQ<TYPE_AS_Type> & type_l (recsel.GetSequence (3)); // seq of AS`Type
          // NOTE: SEM`REC implimentation is different from spec.
          SEQ<TYPE_SEM_VAL> v (res_v.GetRecord(pos_DYNSEM_SEM_SemRecord_value).GetFields());
          size_t len_type_l = type_l.Length();
          for (size_t index = 1; index <= len_type_l; index++) {
            if(!theState().SubType (v[index], type_l[index]))
              return RTERR::ErrorVal (L"EvalRecordConstructorExpr", RTERR_TYPE_INCOMP,
                                      v[index], type_l[index], Sequence());
          }
        }
        res_v.SetField(pos_DYNSEM_SEM_SemRecord_checked, Bool(true));
      } // end of if(Settings.DTC())
      return res_v;
    }
    else
      return RTERR::ErrorVal (L"EvalRecordConstructorExpr", RTERR_RECORD_SIZE_WRONG, Nil(), Nil(), Sequence());
  }
  else
    return RTERR::ErrorVal (L"EvalRecordConstructorExpr", RTERR_TAG_UNKNOWN, Nil(), Nil(), Sequence());
}
#endif // VDMSL

#ifdef VDMPP
// Note, the implementation of this function differs from the specification.
// This is because of the different implementation of the semantic record values.

// ConstructSEMRecFields
// sem_tag : AS`Name
// val_l : seq of SEM`VAL
// ==> SEM`VAL
TYPE_DYNSEM_SEM_SemRecord EXPR::ConstructSEMRecFields(const TYPE_AS_Name & sem_tag, const SEQ<TYPE_SEM_VAL> & val_l)
{
//  if (!AUX::LookUpRecSel(sem_tag).GetBoolValue(1)) {
  if (!theState().GetCachedRecSel(sem_tag).GetBoolValue(1))
    return RTERR::ErrorVal (L"ConstructSEMRecFields", RTERR_LOCAL_COMPOSE_TYPEDEF, Nil(), Nil(), Sequence());
  else
    return SemRec::mk_SEM_REC(sem_tag, val_l);
}

// EvalRecordConstructorExpr
// tag : AS`Name
// fields : seq of SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalRecordConstructorExpr(const TYPE_AS_Name & tag, const SEQ<TYPE_SEM_VAL> & fields)
{
//  Tuple etn (AUX::ExtractTagName(tag)); // [AS`Name] * bool
  Tuple etn (theState().ExtractTagNameCached(tag)); // [AS`Name] * bool
  if (etn.GetBoolValue(2)) {
    const TYPE_AS_Name & the_tag (etn.GetRecord(1)); // AS`Name

//    Tuple lurs (AUX::LookUpRecSel(the_tag)); // bool * [GLOBAL`RecSel]
    Tuple lurs (theState().GetCachedRecSel(the_tag)); // bool * [GLOBAL`RecSel]
    //GLOBAL`RecSel =  nat * (map AS`Name to nat) * seq of AS`Type
    const Tuple & recsel (lurs.GetTuple(2)); // GLOBAL`RecSel
    if ( recsel.GetIntValue(1) == fields.Length() ) {
      TYPE_DYNSEM_SEM_SemRecord res_v (ConstructSEMRecFields(the_tag, fields));
      if (Settings.DTC()) {
//        Tuple itd (AUX::IsTypeDef(the_tag));
        Tuple itd (theState().GetCachedTypeDef(the_tag));
        // ==> bool * [GLOBAL`Type] * [AS`Invariant] * [AS`Equal] * [AS`Order] * [AS`Name] * [AS`Access]

        if (itd.GetBoolValue(1)) {
          TYPE_AS_TypeName tnm;
          tnm.Init(tag, tag.GetInt(pos_AS_Name_cid));
          // must use TypeName for check invariant
          if (!theState().SubType(res_v, tnm)) {
            if (theState().RealSubType (res_v, tnm, false))
              return RTERR::ErrorVal (L"EvalRecordConstructorExpr", RTERR_TYPE_INV_BROKEN, res_v, tnm, Sequence());
            else
              return RTERR::ErrorVal (L"EvalRecordConstructorExpr", RTERR_TYPE_INCOMP, res_v, tnm, Sequence());
          }
        }
        else {
          const SEQ<TYPE_AS_Type> & type_l (recsel.GetSequence(3)); // seq of AS`Type
          // NOTE: SEM`REC implimentation is different from spec.
          SEQ<TYPE_SEM_VAL> v (res_v.GetRecord(pos_DYNSEM_SEM_SemRecord_value).GetFields());
          size_t len_type_l = type_l.Length ();
          for (size_t index = 1; index <= len_type_l; index++)
          {
            TYPE_AS_Type type (type_l[index]);
//
            // if TypeName is in field 
            if ((the_tag.GetSequence(pos_AS_Name_ids).Length() == 2) && type.Is(TAG_TYPE_AS_TypeName))
            {
              const TYPE_AS_Name & nm (type.GetRecord(pos_AS_TypeName_name));
              if (!theState().IsAClass(nm) && (nm.GetSequence(pos_AS_Name_ids).Length() == 1))
              {
                type.SetField(pos_AS_TypeName_name, ASTAUX::Combine2Names(the_tag, nm));
              }
            }
//
            if (! theState().SubType (v[index], type))
              return RTERR::ErrorVal(L"EvalRecordConstructorExpr", RTERR_TYPE_INCOMP, v[index], type_l[index], Sequence());
          }
        }
        res_v.SetField(pos_DYNSEM_SEM_SemRecord_checked, Bool(true));
      }
      return res_v;
    }
    else
      return RTERR::ErrorVal (L"EvalRecordConstructorExpr", RTERR_RECORD_SIZE_WRONG, Nil(), Nil(), Sequence());
  }
  else
    return RTERR::ErrorVal (L"EvalRecordConstructorExpr", RTERR_TAG_UNKNOWN, Nil(), Nil(), Sequence());
}
#endif // VDMPP

#ifdef VDMSL
// EvalRecordModifierExpr
// rec_v : SEM`VAL
// fid_l : (seq of AS`Name)
// val_l : (seq of SEM`VAL)
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalRecordModifierExpr (const TYPE_SEM_VAL & rec_v,
                                           const SEQ<TYPE_AS_Name> & fid_l,
                                           const SEQ<TYPE_SEM_VAL> & val_l)
{
  if (!rec_v.Is(TAG_TYPE_DYNSEM_SEM_REC))
    return RTERR::ErrorVal (L"EvalRecordModifierExpr", RTERR_REC_EXPECTED, Nil(), Nil(), Sequence());
  else {
    const TYPE_AS_Name & tag (rec_v.GetRecord(pos_DYNSEM_SEM_SemRecord_tag));

    if (!AUX::IsTypeStructExported (tag))
      return RTERR::ErrorVal (L"EvalRecordModifierExpr", RTERR_TYPE_NOT_EXPORTED, Nil(), Nil(), Sequence());
    else {
//      Tuple lurs (AUX::LookUpRecSel(tag)); // bool * GLOBAL`RecSel
      Tuple lurs (theState().GetCachedRecSel(tag)); // bool * GLOBAL`RecSel
      if (lurs.GetBoolValue(1))
      {
        Tuple recsel (lurs.GetTuple(2)); // GLOBAL`RecSel
             //GLOBAL`RecSel =  nat * (map AS`Name to nat) * seq of AS`Type
        const MAP<TYPE_AS_Name,Int> & pos (recsel.GetMap (2)); // map AS`Name to nat
        const SEQ<TYPE_AS_Type> & type_l (recsel.GetSequence (3));  // seq of AS`Type

        // NOTE: SEM`REC implimentation is different from spec.
        SEQ<TYPE_SEM_VAL> tmp_v (rec_v.GetRecord(pos_DYNSEM_SEM_SemRecord_value).GetFields());

        size_t len_fid_l = fid_l.Length();
        for (size_t i = 1; i <= len_fid_l; i++)
        {
          const TYPE_AS_Name & fid (fid_l[i]);

          if (!pos.DomExists (fid)) {
            vdm_iplog << L"Unknown record field was '" << ASTAUX::ASName2String (fid) << L"'" << endl << flush;
            return RTERR::ErrorVal (L"EvalRecordModifierExpr", RTERR_RECORD_FIELD_ID_UNKNOWN, Nil(), Nil(), Sequence());
          }
          else {
            const TYPE_SEM_VAL & new_v (val_l[i]);
            int index = pos[fid].GetValue ();
            tmp_v.ImpModify (index, new_v);
            if (Settings.DTC())
            {
              theStackMachine().PushModule (theState().ExtractModule(tag));
              if (!theState().SubType (new_v, type_l[index]))
                return RTERR::ErrorVal (L"EvalRecordModifierExpr", RTERR_TYPE_INCOMP, new_v, type_l[index], Sequence());
              theStackMachine().PopModule ();
            }
          }
        }

        TYPE_DYNSEM_SEM_SemRecord res_v (SemRec::mk_SEM_REC(tag, tmp_v));
        res_v.SetField(pos_DYNSEM_SEM_SemRecord_checked, Bool(true));

        if (Settings.DTC())
        {
          // push mudule if needed before checking the fields
          theStackMachine().PushModule (theState().ExtractModule(tag));

//          Tuple itd (AUX::IsTypeDef(tag)); // bool * [AS`Type] * [AS`Invariant] * [AS`Equal] * [AS`Order] * [AS`Name]
          Tuple itd (theState().GetCachedTypeDef(tag)); // bool * AS`TypeDef
          if (itd.GetBoolValue(1)) {
            TYPE_AS_TypeName tnm;
            tnm.Init(tag, tag.GetInt(pos_AS_Name_cid));
            if (!theState().SubType(res_v, tnm)) {
              if (theState().RealSubType (res_v, tnm, false))
                return RTERR::ErrorVal (L"EvalRecordModifierExpr", RTERR_TYPE_INV_BROKEN, res_v, tnm, Sequence());
              else
                return RTERR::ErrorVal (L"EvalRecordModifierExpr", RTERR_TYPE_INCOMP, res_v, tnm, Sequence());
            }
          }
          theStackMachine().PopModule ();
          //res_v.SetField(pos_DYNSEM_SEM_SemRecord_checked, Bool(true));
        }  // end of DTC()
        return (res_v);
      }
      else {
        vdm_iplog << L"Unknown record name was " << ASTAUX::ASName2String (tag) << endl << flush;
        return RTERR::ErrorVal (L"EvalRecordModifierExpr", RTERR_TAG_UNKNOWN, Nil(), Nil(), Sequence());
      }
    }
  }
//  return TYPE_SEM_VAL (); // to avoid warnings
}
#endif // VDMSL

#ifdef VDMPP
// EvalRecordModifierExpr
// rec_v : SEM`VAL
// fid_l : (seq of AS`Name)
// val_l : (seq of SEM`VAL)
// ==>  SEM`VAL
TYPE_SEM_VAL EXPR::EvalRecordModifierExpr(const TYPE_SEM_VAL & rec_v,
                                          const SEQ<TYPE_AS_Name> & fid_l,
                                          const SEQ<TYPE_SEM_VAL> & val_l)
{
  if ( !rec_v.Is(TAG_TYPE_DYNSEM_SEM_REC) )
    return RTERR::ErrorVal(L"EvalRecordModifierExpr", RTERR_REC_EXPECTED, rec_v, Nil(), Sequence());
  else {
    const TYPE_AS_Name & tag (rec_v.GetRecord(pos_DYNSEM_SEM_SemRecord_tag));

//    Tuple etn (AUX::ExtractTagName(tag)); // [AS`Name] * bool
    Tuple etn (theState().ExtractTagNameCached(tag)); // [AS`Name] * bool
    if (!etn.GetBoolValue(2)) // isinscope_tag
    {
      return RTERR::ErrorVal(L"EvalRecordModifierExpr", RTERR_TAG_UNKNOWN,
                          EvalState::M42Sem(AUX::SingleNameToString(tag), NULL), Nil(), Sequence());
    }
    else {
      const TYPE_AS_Name & tagname (etn.GetRecord(1));
//      Tuple lurs (AUX::LookUpRecSel(tagname)); // bool * GLOBAL`RecSel
      Tuple lurs (theState().GetCachedRecSel(tagname)); // bool * GLOBAL`RecSel
      if (!lurs.GetBoolValue(1))
        return RTERR::ErrorVal (L"EvalRecordModifierExpr", RTERR_TAG_UNKNOWN,
                             EvalState::M42Sem(AUX::SingleNameToString(tagname), NULL), Nil(), Sequence());
      const Tuple & recsel (lurs.GetTuple(2)); // GLOBAL`RecSel
       //  GLOBAL`RecSel = nat * (map AS`Name to nat) * seq of AS`Type
      const MAP<TYPE_AS_Name, Int> & pos (recsel.GetMap(2));    // map AS`Name to nat
      const SEQ<TYPE_AS_Type> & type_l (recsel.GetSequence(3)); // seq of AS`Type

      // NOTE: SEM`REC implimentation is different from spec.
      SEQ<TYPE_SEM_VAL> tmp_v (rec_v.GetRecord(pos_DYNSEM_SEM_SemRecord_value).GetFields());

      size_t len_fid_l = fid_l.Length();
      for (size_t i = 1; i <= len_fid_l; i++)
      {
        const TYPE_AS_Name & fid (fid_l[i]);
        if (!pos.DomExists(fid))
          return RTERR::ErrorVal (L"EvalRecordModifierExpr", RTERR_RECORD_FIELD_ID_UNKNOWN,
                               EvalState::M42Sem(AUX::SingleNameToString(fid), NULL), Nil(), Sequence());
        else {
          const TYPE_SEM_VAL & new_v (val_l[i]);
          int index = pos[fid].GetValue();
          tmp_v.ImpModify(index, new_v);
          if (Settings.DTC()) {
            TYPE_AS_Type type (type_l[index]);
//            Tuple itd (AUX::IsTypeDef(tagname));
        // ==> bool * [GLOBAL`Type] * [AS`Invariant] * [AS`Equal] * [AS`Order] * [AS`Name] * [AS`Access]
            Tuple itd (theState().GetCachedTypeDef(tagname));
            if (itd.GetBoolValue(1)) {
              const TYPE_AS_Name & defcl (itd.GetRecord(6)); // [AS`Name]
              theStackMachine().PushClNmCurObj(defcl, defcl);

              if (!theState().SubType (new_v, type))
                return RTERR::ErrorVal (L"EvalRecordModifierExpr", RTERR_TYPE_INCOMP, new_v, type_l[index], Sequence());
              theStackMachine().PopClNmCurObj();
            }
// 20140121 -->
            else
            {
              if (!theState().SubType (new_v, type))
                return RTERR::ErrorVal (L"EvalRecordModifierExpr", RTERR_TYPE_INCOMP, new_v, type_l[index], Sequence());
            }
// <-- 20140121
          }
        }
      }

      // create new SEM`REC
      TYPE_DYNSEM_SEM_SemRecord res_v (SemRec::mk_SEM_REC(tag, tmp_v));
      res_v.SetField(pos_DYNSEM_SEM_SemRecord_checked, Bool(true));

      if (Settings.DTC()) { // check for invatiant
//        Tuple itd (AUX::IsTypeDef(tagname));
        Tuple itd (theState().GetCachedTypeDef(tagname));
        if (itd.GetBoolValue(1)) {
          TYPE_AS_TypeName tnm;
          tnm.Init(tag, tag.GetInt(pos_AS_Name_cid));
          // must use TYpeName for check invariant
          if (! theState().SubType(res_v, tnm)) {
            if (theState().RealSubType (res_v, tnm, false)) {
              return RTERR::ErrorVal (L"EvalRecordModifierExpr", RTERR_TYPE_INV_BROKEN, res_v, tnm, Sequence());
            }
            else {
              return RTERR::ErrorVal (L"EvalRecordModifierExpr", RTERR_TYPE_INCOMP, res_v, tnm, Sequence());
            }
          }
        }
        //res_v.SetField(pos_DYNSEM_SEM_SemRecord_checked, Bool(true));
      } // Settings.DTC()
      return res_v;
    }
  }
//  return TYPE_SEM_VAL(); // dummy
}
#endif // VDMPP

// EvalFieldSelectExpr
// record_v : SEM`VAL
// field : AS`Name
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalFieldSelectExpr (const TYPE_SEM_VAL & record_v, const TYPE_AS_Name & field)
{
  //RT_REMOVED if (RT_INFO) Set_RT_INFO (field);  // mark <field> as covered
  switch (record_v.GetTag()) {
    case TAG_TYPE_DYNSEM_SEM_REC: { return EvalFieldRecordSelect(record_v, field); }
#ifdef VDMPP
    case TAG_TYPE_SEM_OBJ_uRef:   { return EvalFieldOBJRefSelect(record_v, field); }
#endif// VDMPP
    default: {
// 20150106 -->
      if (theStackMachine().IsRuntimeErrorException(record_v))
        return record_v;
// <-- 20150106
#ifdef VDMSL
      return RTERR::ErrorVal (L"EvalFieldSelectExpr", RTERR_REC_EXPECTED, record_v, Nil(), Sequence());
#endif //VDMSL
#ifdef VDMPP
      return RTERR::ErrorVal (L"EvalFieldSelectExpr", RTERR_OBJ_RECORD_EXP, record_v, Nil(), Sequence());
#endif //VDMPP
    }
  }
}

#ifdef VDMPP
// EvalFieldOBJRefSelect
// objref : SEM`OBJ_Ref
// field : AS`Name
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalFieldOBJRefSelect(const TYPE_SEM_OBJ_uRef & objref, const TYPE_AS_Name & field)
{
  TYPE_SEM_VAL res_v (theStackMachine().EvalFieldOBJRefSelect(objref, field));

  switch (res_v.GetTag()) {
    case TAG_TYPE_SEM_ExplOP: {
      res_v.SetField(pos_SEM_ExplOP_objref, objref);
      return res_v;
    }
    case TAG_TYPE_SEM_OverOPFN: {
      res_v.SetField(pos_SEM_OverOPFN_objref, objref);
      return res_v;
    }
    case TAG_TYPE_SEM_CompExplFN: {
      res_v.SetField(pos_SEM_CompExplFN_objref, objref);
// 20140823 -->
      const SEQ<TYPE_SEM_ExplFN> & fn_l (res_v.GetSequence(pos_SEM_CompExplFN_fl));
      SEQ<TYPE_SEM_ExplFN> new_fn_l;
      size_t len_fn_l = fn_l.Length();
      for (size_t idx = 1; idx <= len_fn_l; idx++) {
        TYPE_SEM_ExplFN fn (fn_l[idx]);
        fn.SetField(pos_SEM_ExplFN_objref, objref);
        new_fn_l.ImpAppend(fn);
      }
      res_v.SetField(pos_SEM_CompExplFN_fl, new_fn_l);
// <-- 20140823
      return res_v;
    }
    case TAG_TYPE_SEM_ExplPOLY: {
      res_v.SetField(pos_SEM_ExplPOLY_objref, objref);
      return res_v;
    }
  }
  return res_v;
}
#endif //VDMPP

// EvalFieldRecordSelect
// record_v : SEM`REC
// field : AS`Name
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalFieldRecordSelect(const TYPE_DYNSEM_SEM_SemRecord & record_v, const TYPE_AS_Name & field)
{
  const TYPE_AS_Name & tag (record_v.GetRecord(pos_DYNSEM_SEM_SemRecord_tag));
#ifdef VDMSL
  if (!AUX::IsTypeStructExported (tag)) {
    vdm_iplog << L"Non-exported type was " << ASTAUX::ASName2String (tag) << endl << flush;
    return RTERR::ErrorVal (L"EvalFieldSelectExpr", RTERR_TYPE_NOT_EXPORTED, Nil(), Nil(), Sequence());
  }

//  Tuple lurs (AUX::LookUpRecSel(tag)); // bool * GLOBAL`RecSel
  Tuple lurs (theState().GetCachedRecSel(tag)); // bool * GLOBAL`RecSel
  if (lurs.GetBoolValue(1))
  {
    const Tuple & recsel (lurs.GetTuple(2)); // GLOBAL`RecSel
    //GLOBAL`RecSel = nat * (map AS`Name to nat) * seq of AS`Type
    const MAP<TYPE_AS_Name, Int> & pos (recsel.GetMap(2)); // map AS`Name to nat
#endif //VDMSL

#ifdef VDMPP
//  Tuple etn (AUX::ExtractTagName(tag)); // [AS`Name] * bool
  Tuple etn (theState().ExtractTagNameCached(tag)); // [AS`Name] * bool
  if (etn.GetBoolValue(2))
  {
//    Tuple lurs (AUX::LookUpRecSel(etn.GetField(1))); // bool * GLOBAL`RecSel
    Tuple lurs (theState().GetCachedRecSel(etn.GetField(1))); // bool * GLOBAL`RecSel
    const Tuple & recsel (lurs.GetTuple(2)); // GLOBAL`RecSel
    //GLOBAL`RecSel = nat * (map AS`Name to nat) * seq of AS`Type
    const MAP<TYPE_AS_Name, Int> & pos (recsel.GetMap(2)); // map AS`Name to nat
#endif //VDMPP

    if (pos.DomExists (field))
    {
      return record_v.GetRecord(pos_DYNSEM_SEM_SemRecord_value).GetRecord(pos[field].GetValue ());
    }
    else {
      vdm_iplog << L"Unknown record field was '" << ASTAUX::ASName2String (field) << L"'" << endl << flush;
      return RTERR::ErrorVal (L"EvalFieldSelectExpr", RTERR_RECORD_FIELD_ID_UNKNOWN, Nil(), Nil(), Sequence());
    }
  }
  else {
    vdm_iplog << L"Unknown record name was " << ASTAUX::ASName2String (tag) << endl << flush;
    return RTERR::ErrorVal (L"EvalFieldSelectExpr", RTERR_TAG_UNKNOWN, Nil(), Nil(), Sequence());
  }
}

// MakeTypeMap (not in spec)
// typevars : seq of AS`TypeVar
// types : seq of AS`Type
// -> map AS`TypeVar to AS`Type
MAP<TYPE_AS_TypeVar,TYPE_AS_Type> EXPR::MakeTypeMap (const SEQ<TYPE_AS_TypeVar> & typevars,
                                                     const SEQ<TYPE_AS_Type> & types)
{
  MAP<TYPE_AS_TypeVar,TYPE_AS_Type> typemap;
  size_t len_typevars = typevars.Length ();
  for (size_t idx = 1; idx <= len_typevars; idx++)
    typemap.Insert (typevars[idx], types[idx]);
  return typemap;
}

// SubstTypeList
// tp : seq of AS`Type
// tv_l : seq of AS`TypeVar
// tp_l : seq of AS`Type
// +> seq of AS`Type
SEQ<TYPE_AS_Type> EXPR::SubstTypeList (const SEQ<TYPE_AS_Type> & tp,
                                       const SEQ<TYPE_AS_TypeVar> & tv_l,
                                       const SEQ<TYPE_AS_Type> & tp_l)
{
  SEQ<TYPE_AS_Type> res;
  size_t len_tp = tp.Length();
  for (size_t index = 1; index <= len_tp; index++)
    res.ImpAppend (SubstType (tp[index], tv_l, tp_l));

  return res;
}

// ConvertPolyToFn
// Poly : SEM`ExplPOLY
// inst : seq of AS`Type
// ==> SEM`CompExplFN
TYPE_SEM_CompExplFN EXPR::ConvertPolyToFn (const TYPE_SEM_ExplPOLY & poly, const SEQ<TYPE_AS_Type> & type_l)
{
  const SEQ<TYPE_AS_TypeVar> & tpp (poly.GetSequence(pos_SEM_ExplPOLY_tpparms));

  if (tpp.Length () == type_l.Length ())
  {
    const TYPE_AS_FnType & tp (poly.GetRecord(pos_SEM_ExplPOLY_tp));
    const Sequence & parms (poly.GetSequence(pos_SEM_ExplPOLY_parms));
    const TYPE_AS_Access & access (poly.GetField(pos_SEM_ExplPOLY_access));
    Sequence new_parms (parms);

    TYPE_AS_FnType newtp;
    switch(tp.GetTag()) {
      case TAG_TYPE_AS_PartialFnType: {
        SEQ<TYPE_AS_Type> fndom (SubstTypeList (tp.GetSequence (pos_AS_PartialFnType_fndom), tpp, type_l));
        TYPE_AS_Type fnrng (SubstType (tp.GetRecord (pos_AS_PartialFnType_fnrng), tpp, type_l));
        newtp = TYPE_AS_PartialFnType().Init(fndom, fnrng, NilContextId);
        break;
      }
      case TAG_TYPE_AS_TotalFnType: {
        SEQ<TYPE_AS_Type> fndom (SubstTypeList (tp.GetSequence (pos_AS_TotalFnType_fndom), tpp, type_l));
        TYPE_AS_Type fnrng (SubstType (tp.GetRecord (pos_AS_TotalFnType_fnrng), tpp, type_l));
        newtp = TYPE_AS_TotalFnType().Init(fndom, fnrng, NilContextId);
        break;
      }
    }

    TYPE_SEM_ExplFN res_fn;
    res_fn.Init (newtp,
                 new_parms,
                 poly.GetInt(pos_SEM_ExplPOLY_instr),
                 poly.GetRecord(pos_SEM_ExplPOLY_env),
                 MakeTypeMap (tpp, type_l),
                 poly.GetField(pos_SEM_ExplPOLY_fnName),
                 poly.GetRecord(pos_SEM_ExplPOLY_modName),
                 Bool(false),
                 poly.GetField(pos_SEM_ExplPOLY_objref),
                 access);
                 
    return SemRec::CompFN(res_fn);
  }
  else
    return RTERR::ErrorVal (L"ConvertPolyToFn", RTERR_WRONG_NO_OF_ARGS, Nil(), Nil(), Sequence());
}

// SubstType
// tp : AS`Type
// tv_l : AS`TypeVarList
// tp_l : seq of AS`Type
// ==> AS`Type
TYPE_AS_Type EXPR::SubstType (const TYPE_AS_Type & tp,
                              const SEQ<TYPE_AS_TypeVar> & tv_l,
                              const SEQ<TYPE_AS_Type> & tp_l)
{
  switch(tp.GetTag()) { 
    case TAG_TYPE_AS_BooleanType:
    case TAG_TYPE_AS_NumericType:
    case TAG_TYPE_AS_TokenType:
    case TAG_TYPE_AS_CharType:
    case TAG_TYPE_AS_VoidType:
    case TAG_TYPE_AS_QuoteType:
    case TAG_TYPE_AS_TypeName: {
      return tp;
    }
    case TAG_TYPE_AS_BracketedType: { 
      return TYPE_AS_BracketedType().Init(SubstType (tp.GetRecord(pos_AS_BracketedType_tp), tv_l, tp_l),
                                          tp.GetInt(pos_AS_BracketedType_cid));
    }
    case TAG_TYPE_AS_CompositeType: {
      const SEQ<TYPE_AS_Field> & fields (tp.GetSequence(pos_AS_CompositeType_fields));
      SEQ<TYPE_AS_Field> newf;
      size_t len_fields = fields.Length();
      for (size_t idx = 1; idx <= len_fields; idx++) {
        const TYPE_AS_Field & fld (fields[idx]);
        newf.ImpAppend (TYPE_AS_Field().Init(fld.GetField(pos_AS_Field_sel),
                                             SubstType (fld.GetRecord(pos_AS_Field_type), tv_l, tp_l),
                                             fld.GetBool(pos_AS_Field_dc),
                                             fld.GetInt(pos_AS_Field_cid)));
      }
      return TYPE_AS_CompositeType().Init(tp.GetRecord(pos_AS_CompositeType_name),
                                          newf,
                                          tp.GetInt(pos_AS_CompositeType_cid));
    }
    case TAG_TYPE_AS_UnionType: {
      const SEQ<TYPE_AS_Type> & utp_l (tp.GetSequence(pos_AS_UnionType_tps));
      SEQ<TYPE_AS_Type> nutp_l;
      size_t len_utp_l = utp_l.Length();
      for (size_t idx = 1; idx <= len_utp_l; idx++)
        nutp_l.ImpAppend (SubstType (utp_l[idx], tv_l, tp_l));
      return TYPE_AS_UnionType().Init(nutp_l, tp.GetInt(pos_AS_UnionType_cid));
    }
    case TAG_TYPE_AS_ProductType: {
      const SEQ<TYPE_AS_Type> & ptp_l (tp.GetSequence(pos_AS_ProductType_tps));
      SEQ<TYPE_AS_Type> nptp_l;
      size_t len_ptp_l = ptp_l.Length();
      for (size_t idx = 1; idx <= len_ptp_l; idx++)
        nptp_l.ImpAppend (SubstType (ptp_l[idx], tv_l, tp_l));
      return TYPE_AS_ProductType().Init(nptp_l, tp.GetInt(pos_AS_ProductType_cid));
    }

    case TAG_TYPE_AS_OptionalType: {
      return TYPE_AS_OptionalType().Init(SubstType (tp.GetRecord(pos_AS_OptionalType_tp), tv_l, tp_l),
                                         tp.GetInt(pos_AS_OptionalType_cid));
    }

    case TAG_TYPE_AS_Set0Type: {
      return TYPE_AS_Set0Type().Init(SubstType (tp.GetRecord(pos_AS_Set0Type_elemtp), tv_l, tp_l),
                                    tp.GetInt(pos_AS_Set0Type_cid));
    }

    case TAG_TYPE_AS_Set1Type: {
      return TYPE_AS_Set1Type().Init(SubstType (tp.GetRecord(pos_AS_Set1Type_elemtp), tv_l, tp_l),
                                    tp.GetInt(pos_AS_Set1Type_cid));
    }

    case TAG_TYPE_AS_Seq0Type: {
      return TYPE_AS_Seq0Type().Init(SubstType (tp.GetRecord(pos_AS_Seq0Type_elemtp), tv_l, tp_l),
                                     tp.GetInt(pos_AS_Seq0Type_cid));
    }

    case TAG_TYPE_AS_Seq1Type: {
      return TYPE_AS_Seq1Type().Init(SubstType (tp.GetRecord(pos_AS_Seq1Type_elemtp), tv_l, tp_l),
                                     tp.GetInt(pos_AS_Seq1Type_cid));
    }

    case TAG_TYPE_AS_GeneralMap0Type: {
      return TYPE_AS_GeneralMap0Type().Init(SubstType (tp.GetRecord(pos_AS_GeneralMap0Type_mapdom), tv_l, tp_l),
                                           SubstType (tp.GetRecord(pos_AS_GeneralMap0Type_maprng), tv_l, tp_l),
                                           tp.GetInt(pos_AS_GeneralMap0Type_cid));
    }

    case TAG_TYPE_AS_GeneralMap1Type: {
      return TYPE_AS_GeneralMap1Type().Init(SubstType (tp.GetRecord(pos_AS_GeneralMap1Type_mapdom), tv_l, tp_l),
                                           SubstType (tp.GetRecord(pos_AS_GeneralMap1Type_maprng), tv_l, tp_l),
                                           tp.GetInt(pos_AS_GeneralMap1Type_cid));
    }

    case TAG_TYPE_AS_InjectiveMap0Type: {
      return TYPE_AS_InjectiveMap0Type().Init(SubstType (tp.GetRecord(pos_AS_InjectiveMap0Type_mapdom), tv_l, tp_l),
                                             SubstType (tp.GetRecord(pos_AS_InjectiveMap0Type_maprng), tv_l, tp_l),
                                             tp.GetInt(pos_AS_InjectiveMap0Type_cid));
    }

    case TAG_TYPE_AS_InjectiveMap1Type: {
      return TYPE_AS_InjectiveMap1Type().Init(SubstType (tp.GetRecord(pos_AS_InjectiveMap1Type_mapdom), tv_l, tp_l),
                                             SubstType (tp.GetRecord(pos_AS_InjectiveMap1Type_maprng), tv_l, tp_l),
                                             tp.GetInt(pos_AS_InjectiveMap1Type_cid));
    }

    case TAG_TYPE_AS_PartialFnType: {
      const SEQ<TYPE_AS_Type> & pfdom (tp.GetSequence(pos_AS_PartialFnType_fndom));
      SEQ<TYPE_AS_Type> npfdom;
      size_t len_pfdom = pfdom.Length();
      for (size_t idx = 1; idx <= len_pfdom; idx++)
        npfdom.ImpAppend (SubstType (pfdom[idx], tv_l, tp_l));
      return TYPE_AS_PartialFnType().Init(npfdom,
                                          SubstType (tp.GetRecord(pos_AS_PartialFnType_fnrng), tv_l, tp_l),
                                          tp.GetInt(pos_AS_PartialFnType_cid));
    }

    case TAG_TYPE_AS_TotalFnType: {
      const SEQ<TYPE_AS_Type> & tfdom (tp.GetSequence(pos_AS_TotalFnType_fndom));
      SEQ<TYPE_AS_Type> ntfdom;
      size_t len_tfdom = tfdom.Length();
      for (size_t idx = 1; idx <= len_tfdom; idx++)
        ntfdom.ImpAppend (SubstType (tfdom[idx], tv_l, tp_l));
      return TYPE_AS_TotalFnType().Init(ntfdom,
                                        SubstType (tp.GetRecord(pos_AS_TotalFnType_fnrng), tv_l, tp_l),
                                        tp.GetInt(pos_AS_TotalFnType_cid));
    }

    case TAG_TYPE_AS_OpType: {
      const SEQ<TYPE_AS_Type> & opdom (tp.GetSequence(pos_AS_OpType_opdom));
      SEQ<TYPE_AS_Type> nopdom;
      size_t len_opdom = opdom.Length();
      for (size_t idx = 1; idx <= len_opdom; idx++)
        nopdom.ImpAppend (SubstType (opdom[idx], tv_l, tp_l));
      return TYPE_AS_OpType().Init(nopdom,
                                   SubstType (tp.GetRecord(pos_AS_OpType_oprng), tv_l, tp_l),
                                   tp.GetInt(pos_AS_OpType_cid));
    }

    case TAG_TYPE_AS_TypeVar: {
      const TYPE_AS_Name & id (tp.GetRecord(pos_AS_TypeVar_name));
      size_t len_tv_l = tv_l.Length ();
      for (size_t idx = 1; idx <= len_tv_l; idx++) {
        if (id == tv_l[idx].GetRecord(pos_AS_TypeVar_name))
          return tp_l[idx];
      }
      TYPE_SEM_VAL seq (EvalState::M42Sem(AUX::SingleNameToString(id), NULL));
      RTERR::Error (L"SubstType", RTERR_TYPE_UNKNOWN, seq, Nil(), Sequence());
      return tp; // Dummy return.
    }

    default: {
      return tp; // Dummy return.
    }
  }
}

#ifdef VDMSL
// IsFunction
// id : AS`Name
// ==> bool
bool EXPR::IsFunction (const TYPE_AS_Name & loc_name, const TYPE_AS_Name & mod_name)
{
  const TYPE_GLOBAL_SigmaMO & sigmamo (theState().GetModule(mod_name));
  return sigmamo.GetMap(pos_GLOBAL_SigmaMO_explfns).DomExists (loc_name);
}

// GetFunctionVal
// id : AS`Name
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::GetFunctionVal (const TYPE_AS_Name & loc_name, const TYPE_AS_Name & mod_name)
{
  const TYPE_GLOBAL_SigmaMO & sigmamo (theState().GetModule(mod_name));
  //return sigmamo.get_explfns()[loc_name];
  return sigmamo.GetMap(pos_GLOBAL_SigmaMO_explfns)[loc_name];
}

// IsOperation
// id : AS`Name
// ==> bool
bool EXPR::IsOperation (const TYPE_AS_Name & loc_name, const TYPE_AS_Name & mod_name)
{
  const TYPE_GLOBAL_SigmaMO & sigmamo (theState().GetModule(mod_name));
  return sigmamo.GetMap(pos_GLOBAL_SigmaMO_explops).DomExists (loc_name);
}

// GetOperationVal
// id : AS`Name
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::GetOperationVal (const TYPE_AS_Name & loc_name, const TYPE_AS_Name & mod_name)
{
  const TYPE_GLOBAL_SigmaMO & sigmamo (theState().GetModule(mod_name));
  return sigmamo.GetMap(pos_GLOBAL_SigmaMO_explops)[loc_name];
}

// IsPoly
// id : AS`Name
// ==> bool
bool EXPR::IsPoly (const TYPE_AS_Name & loc_name, const TYPE_AS_Name & mod_name)
{
  const TYPE_GLOBAL_SigmaMO & sigmamo (theState().GetModule(mod_name));
  return sigmamo.GetMap(pos_GLOBAL_SigmaMO_explpolys).DomExists (loc_name);
}

// GetPolyVal
// id : AS`Name
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::GetPolyVal (const TYPE_AS_Name & loc_name, const TYPE_AS_Name & mod_name)
{
  const TYPE_GLOBAL_SigmaMO & sigmamo (theState().GetModule(mod_name));
  return sigmamo.GetMap(pos_GLOBAL_SigmaMO_explpolys)[loc_name];
}
#endif // VDMSL


// EvalMapInverse
// val_v : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalMapInverse (const TYPE_SEM_VAL & val_v)
{
  if (val_v.Is(TAG_TYPE_SEM_MAP))
  {
    const Map & map_v (val_v.GetMap(pos_SEM_MAP_v));
    if (map_v.Dom ().Card () == map_v.Rng ().Card ())
      return mk_SEM_MAP(map_v.Inverse());
    else
      return RTERR::ErrorVal (L"EvalMapInverse", RTERR_NO_INJECTIVE_MAP, val_v, Nil(), Sequence());
  }
  else
    return RTERR::ErrorVal (L"EvalMapInverse", RTERR_MAP_EXPECTED, val_v, Nil(), Sequence());
}

////Num Operators
//
// EvalNumUnaryExpr
// opr : AS`UnaryOp
// op_v : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalNumUnaryExpr (const Int & opr, const TYPE_SEM_VAL & op_v)
{
  if (op_v.Is(TAG_TYPE_SEM_NUM))
  {
    switch (opr.GetValue()) {
      case NUMPLUS :  { return op_v; }
      case NUMMINUS : { return mk_SEM_NUM(op_v.GetReal(pos_SEM_NUM_v).Minus()); }
      case NUMABS :   { return mk_SEM_NUM(op_v.GetReal(pos_SEM_NUM_v).Abs()); }
      case FLOOR:     { return mk_SEM_NUM(op_v.GetReal(pos_SEM_NUM_v).Floor()); }
      default: {
        return RTERR::ErrorVal (L"EvalNumUnaryExpr", RTERR_OPERATOR_UNKNOWN, Nil(), Nil(), Sequence());
      }
    }
  }
  else
    return RTERR::ErrorVal (L"EvalNumUnaryExpr", RTERR_NUM_EXPECTED, op_v, Nil(), Sequence());
}

////Logical Operators
//
// EvalLogUnaryExpr
// opr : AS`UnaryOp
// op_v : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalLogUnaryExpr (const Int & opr, const TYPE_SEM_VAL & op_v)
{
  if (op_v.Is(TAG_TYPE_SEM_BOOL))
  {
    switch (opr) {
      case NOT:
        return (op_v.GetBoolValue(pos_SEM_BOOL_v) ? sem_false : sem_true);
      default: {
        return RTERR::ErrorVal (L"EvalLogUnaryExpr", RTERR_OPERATOR_UNKNOWN, Nil(), Nil(), Sequence());
      }
    }
  }
  else
    return RTERR::ErrorVal (L"EvalLogUnaryExpr", RTERR_BOOL_EXPECTED, op_v, Nil(), Sequence());
}

// EvalEqualtyExpr
// op1_v : SEM`VAL
// opr : AS`BinaryOp
// op2_v : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalEqualityExpr(const TYPE_SEM_VAL& op1_v, const Int & opr, const TYPE_SEM_VAL& op2_v)
{
  Tuple ie (AUX::IsEq(op1_v,op2_v));
  if (ie.GetBoolValue(1)) {
    TYPE_STKM_SubProgram sp;
    sp.ImpAppend(TYPE_INSTRTP_LOOKUP().Init(ie.GetRecord(2)));
    sp.ImpAppend(TYPE_INSTRTP_PUSH().Init(mk_sequence(ie.GetRecord(3),ie.GetRecord(4))));
    sp.ImpAppend(TYPE_INSTRTP_APPLY());
    Tuple res (theStackMachine().EvalAuxProgram(sp, SEQ<Char>(L"Order evaluation"), false));
    const TYPE_STKM_EvaluationState & eval_state (res.GetRecord(1));
    if (eval_state.Is(TAG_TYPE_STKM_Success)) {
      const TYPE_SEM_VAL & Eq_v (res.GetRecord(2));
      if (Eq_v.Is(TAG_TYPE_SEM_BOOL)) {
        switch(opr.GetValue()) {
          case EQ: { // "="
            return Eq_v;
            break;
          }
          case NE: { // "<>"
            return (Eq_v.GetBoolValue(pos_SEM_BOOL_v) ? sem_false : sem_true);
            break;
          }
        }
      }
      else {
        return RTERR::ErrorVal(L"EvalEqNeBinaryExpr", RTERR_BOOL_EXPECTED, Eq_v, Nil(), Sequence());
      }
    }
    else {
      return RTERR::ErrorVal(L"EvalEqNeBinaryExpr", RTERR_INTERNAL_ERROR, Nil(), Nil(), Sequence());
    }
  }
  return EvalEqNeBinaryExpr(op1_v, opr, op2_v); 
}

// EvalEqNeBinaryExpr
// op1_v : SEM`VAL
// opr : AS`BinaryOp
// op2_v : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalEqNeBinaryExpr(const TYPE_SEM_VAL& op1_v, const Int & opr, const TYPE_SEM_VAL& op2_v)
{
  switch(opr.GetValue()) {
    case EQ: { // "="
      if (op1_v.Is(TAG_TYPE_SEM_UNDEF)) {
        return RTERR::ErrorVal (L"EvalEqNeBinaryExpr", RTERR_UNDEF_ENCOUNTERED, op1_v, Nil(), Sequence());
      }
      else {
        return ((op1_v == op2_v) ? sem_true : sem_false);
      }
    }
    case NE: { // "<>"
      if (op1_v.Is(TAG_TYPE_SEM_UNDEF)) {
        return RTERR::ErrorVal (L"EvalEqNeBinaryExpr", RTERR_UNDEF_ENCOUNTERED, op1_v, Nil(), Sequence());
      }
      else {
        return ((op1_v == op2_v) ? sem_false : sem_true);
      }
    }
    case EQUIV: { // "<=>"
      if (op1_v.Is(TAG_TYPE_SEM_BOOL) && op2_v.Is(TAG_TYPE_SEM_BOOL)) {
        return ((op1_v == op2_v) ? sem_true : sem_false);
      }
      else if (!op1_v.Is(TAG_TYPE_SEM_BOOL)) {
        return RTERR::ErrorVal (L"EvalEqNeBinaryExpr", RTERR_TWO_BOOL_EXPECTED, op1_v, Nil(), Sequence());
      }
      else {
        return RTERR::ErrorVal (L"EvalEqNeBinaryExpr", RTERR_TWO_BOOL_EXPECTED, op2_v, Nil(), Sequence());
      }
    }
    default: { return RTERR::ErrorVal(L"EvalEqNeBinaryExpr", RTERR_OPERATOR_UNKNOWN, Nil(), Nil(), Sequence()); }
  }
}

//
////Set Operators
//

// EvalSetUnaryExpr
// opr : AS`UnaryOp
// op_v : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalSetUnaryExpr (const Int & opr, const TYPE_SEM_VAL & op_v)
{
  switch(opr.GetValue()) {
    case SETCARD:           { return EvalSetCard(op_v); }
    case SETPOWER:          { return EvalSetPower(op_v); }
    case SETDISTRUNION:     { return EvalSetDistrUnion(op_v); }
    case SETDISTRINTERSECT: { return EvalSetDistrIntersect(op_v); }
    default: { return RTERR::ErrorVal (L"EvalSetUnaryExpr", RTERR_OPERATOR_UNKNOWN, Nil(), Nil(), Sequence()); }
  }
}

// EvalSeqUnaryExpr
// opr : AS`UnaryOp
// op_v : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalSeqUnaryExpr (const Int & opr, const TYPE_SEM_VAL & op_v)
{
  switch (opr.GetValue()) {
    case SEQHEAD:       { return EvalSeqHead(op_v); }
    case SEQTAIL:       { return EvalSeqTail(op_v); }
    case SEQLEN:        { return EvalSeqLen(op_v); }
    case SEQELEMS:      { return EvalSeqElems(op_v); }
    case SEQINDICES:    { return EvalSeqIndices(op_v); }
    case SEQDISTRCONC:  { return EvalSeqDistrConc(op_v); }
    case SEQREVERSE:    { return EvalSeqReverse(op_v); }
    default: { return RTERR::ErrorVal (L"EvalSeqUnaryExpr", RTERR_OPERATOR_UNKNOWN, Nil(), Nil(), Sequence()); }
  }
}

// EvalSeqHead
// op_v : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalSeqHead (const TYPE_SEM_VAL & op_v)
{
  if (op_v.Is(TAG_TYPE_SEM_SEQ))
  {
    const SEQ<TYPE_SEM_VAL> & op_lv (op_v.GetSequence(pos_SEM_SEQ_v));
    if (!op_lv.IsEmpty ())
      return op_lv.Hd ();
    else
      return RTERR::ErrorVal (L"EvalSeqHead", RTERR_ZERO_LENGTH_DETECTED, op_v, Nil(), Sequence());
  }
  else
    return RTERR::ErrorVal (L"EvalSeqHead", RTERR_SEQ_EXPECTED, op_v, Nil(), Sequence());
}

// EvalSeqTail
// op_v : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalSeqTail (const TYPE_SEM_VAL & op_v)
{
  if (op_v.Is(TAG_TYPE_SEM_SEQ))
  {
    const SEQ<TYPE_SEM_VAL> & op_lv (op_v.GetSequence(pos_SEM_SEQ_v));
    if (!op_lv.IsEmpty ())
      return mk_SEM_SEQ(op_lv.Tl());
    else
      return RTERR::ErrorVal (L"EvalSeqTail", RTERR_ZERO_LENGTH_DETECTED, op_v, Nil(), Sequence());
  }
  else
    return RTERR::ErrorVal (L"EvalSeqTail", RTERR_SEQ_EXPECTED, op_v, Nil(), Sequence());
}

// EvalSeqLen
// op_v : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalSeqLen (const TYPE_SEM_VAL & op_v)
{
  if (op_v.Is(TAG_TYPE_SEM_SEQ))
    return mk_SEM_NUM(Real(op_v.GetSequence(pos_SEM_SEQ_v).Length ()));
  else
    return RTERR::ErrorVal (L"EvalSeqLen", RTERR_SEQ_EXPECTED, op_v, Nil(), Sequence());
}

// EvalSeqElems
// op_v : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalSeqElems (const TYPE_SEM_VAL & op_v)
{
  if (op_v.Is(TAG_TYPE_SEM_SEQ))
    return mk_SEM_SET(op_v.GetSequence(pos_SEM_SEQ_v).Elems());
  else
    return RTERR::ErrorVal (L"EvalSeqElems", RTERR_SEQ_EXPECTED, op_v, Nil(), Sequence());
}

// EvalSeqIndices
// op_v : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalSeqIndices (const TYPE_SEM_VAL & op_v)
{
  if (op_v.Is(TAG_TYPE_SEM_SEQ))
  {
    size_t max = op_v.GetSequence(pos_SEM_SEQ_v).Length ();
    SET<TYPE_SEM_VAL> inds;
    for (size_t i = 1; i <= max; i++) {
      inds.Insert (mk_SEM_NUM(Real(i)));
    }
    return mk_SEM_SET(inds);
  }
  else
    return RTERR::ErrorVal (L"EvalSeqIndices", RTERR_SEQ_EXPECTED, op_v, Nil(), Sequence());
}

// EvalSeqDistrConc
// op_v : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalSeqDistrConc (const TYPE_SEM_VAL & op_v)
{
  if (op_v.Is(TAG_TYPE_SEM_SEQ))
  {
    const SEQ<TYPE_SEM_VAL> & op_lv (op_v.GetSequence(pos_SEM_SEQ_v));
    bool all_is_SEQ = true;
    SEQ<TYPE_SEM_VAL> res_lv;
    size_t len = op_lv.Length();
    for(size_t i = 1; (i <= len) && all_is_SEQ; i++) {
      const TYPE_SEM_VAL & val (op_lv[i]);
      all_is_SEQ = val.Is(TAG_TYPE_SEM_SEQ);
      if (all_is_SEQ)
        res_lv.ImpConc(val.GetSequence(pos_SEM_SEQ_v));
    }
    if (all_is_SEQ)
      return mk_SEM_SEQ(res_lv);
    else
      return RTERR::ErrorVal (L"EvalSeqDistrConc", RTERR_ALL_SEQS_EXPECTED, op_v, Nil(), Sequence());
  }
  else
    return RTERR::ErrorVal (L"EvalSeqDistrConc", RTERR_SEQ_EXPECTED, op_v, Nil(), Sequence());
}

// EvalSeqReverse
// op_v : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalSeqReverse (const TYPE_SEM_VAL & op_v)
{
  if (op_v.Is(TAG_TYPE_SEM_SEQ))
    return mk_SEM_SEQ(op_v.GetSequence(pos_SEM_SEQ_v).Reverse());
  else
    return RTERR::ErrorVal (L"EvalSeqReverse", RTERR_SEQ_EXPECTED, op_v, Nil(), Sequence());
}

// EvalSetCard
// op_v : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalSetCard (const TYPE_SEM_VAL & op_v)
{
  if (op_v.Is(TAG_TYPE_SEM_SET))
    return mk_SEM_NUM(Real(op_v.GetSet(pos_SEM_SET_v).Card()));
  else
    return RTERR::ErrorVal (L"EvalSetCard", RTERR_SET_EXPECTED, op_v, Nil(), Sequence());
}

// EvalSetPower
// op_v : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalSetPower (const TYPE_SEM_VAL & op_v)
{
  if (op_v.Is(TAG_TYPE_SEM_SET))
  {
    const SET<TYPE_SEM_VAL> & op_sv (op_v.GetSet(pos_SEM_SET_v));
    if (op_sv.Card () < 26)
    {
      Set ps (op_sv.Power()); // set of set of SEM`VAL
      SET<TYPE_SEM_VAL> ps_sv;
      Generic sub;
      for(bool bb = ps.First(sub); bb; bb = ps.Next(sub))
        ps_sv.Insert (mk_SEM_SET(sub));
      return mk_SEM_SET(ps_sv);
    }
    else
      return RTERR::ErrorVal (L"EvalSetPower", RTERR_SET_TOO_BIG, op_v, Nil(), Sequence());
  }
  else
    return RTERR::ErrorVal (L"EvalSetPower", RTERR_SET_EXPECTED, op_v, Nil(), Sequence());
}

// EvalSetDistrUnion
// op_v : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalSetDistrUnion (const TYPE_SEM_VAL & op_v)
{
  if (op_v.Is(TAG_TYPE_SEM_SET))
  {
    SET<TYPE_SEM_VAL> op_sv (op_v.GetSet(pos_SEM_SET_v)); // not Ref
    bool all_is_Set = true;
    Set tmp_ssv; // set of set of SEM`VAL
    Generic elm_v;
    for (bool bb = op_sv.First (elm_v); bb && all_is_Set; bb = op_sv.Next (elm_v))
    {
      TYPE_SEM_VAL val (elm_v);
      all_is_Set = val.Is(TAG_TYPE_SEM_SET);
      if(all_is_Set)
        tmp_ssv.Insert(val.GetSet(pos_SEM_SET_v));
    }

    if (all_is_Set)
      return mk_SEM_SET(tmp_ssv.DUnion());
    else
      return RTERR::ErrorVal (L"EvalSetDistrUnion", RTERR_ALL_SETS_EXPECTED, op_v, Nil(), Sequence());
  }
  else
    return RTERR::ErrorVal (L"EvalSetDistrUnion", RTERR_SET_EXPECTED, op_v, Nil(), Sequence());
}

// EvalSetDistrIntersect
// op_v : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalSetDistrIntersect (const TYPE_SEM_VAL & op_v)
{
  if (op_v.Is(TAG_TYPE_SEM_SET))
  {
    SET<TYPE_SEM_VAL> op_sv (op_v.GetSet(pos_SEM_SET_v)); // not Ref
    if (!op_sv.IsEmpty()) {

      // forall elm_v in set elems op_lv & is_SEM`SET(elm_v) 
      bool all_is_Set = true;
      Set tmp_ssv; // set of set of SEM`VAL
      Generic elm_v;
      for (bool bb = op_sv.First (elm_v); bb && all_is_Set; bb = op_sv.Next (elm_v))
      {
        TYPE_SEM_VAL val (elm_v);
        all_is_Set = val.Is(TAG_TYPE_SEM_SET);
        if(all_is_Set)
          tmp_ssv.Insert(val.GetSet(pos_SEM_SET_v));
      }

      if (all_is_Set)
      {
        return mk_SEM_SET(tmp_ssv.DInter());
      }
      else
        return RTERR::ErrorVal (L"EvalSetDistrIntersect", RTERR_ALL_SETS_EXPECTED, op_v, Nil(), Sequence());
    }
    else
      return RTERR::ErrorVal (L"EvalSetDistrIntersect", RTERR_NONEMPTYSET_EXPECTED, op_v, Nil(), Sequence());
  }
  else
    return RTERR::ErrorVal (L"EvalSetDistrIntersect", RTERR_SET_EXPECTED, op_v, Nil(), Sequence());
}

// EvalComposeExpr
// op1_v : SEM`VAL
// op2_v : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalComposeExpr (const TYPE_SEM_VAL & op1_v, const TYPE_SEM_VAL & op2_v)
{
  if (op1_v.Is(TAG_TYPE_SEM_CompExplFN) && op2_v.Is(TAG_TYPE_SEM_CompExplFN))
  {
    return (EvalComposeFctExpr (op1_v, op2_v));
  }
  else if (op1_v.Is(TAG_TYPE_SEM_MAP) && op2_v.Is(TAG_TYPE_SEM_MAP))
  {
    return (EvalComposeMap (op1_v, op2_v));
  }
  else
    return RTERR::ErrorVal (L"EvalComposeExpr", RTERR_TWO_FN_OR_MAP_EXPECTED, Nil(), Nil(), Sequence());
}

// EvalComposeExpr
// op1_v : SEM`MAP
// op2_v : SEM`MAP
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalComposeMap (const TYPE_SEM_MAP & op1_v, const TYPE_SEM_MAP & op2_v)
{
  const Map & map1_v (op1_v.GetMap(pos_SEM_MAP_v));
  const Map & map2_v (op2_v.GetMap(pos_SEM_MAP_v));

  if (map2_v.Rng ().SubSet (map1_v.Dom ())) {
    Set dom_map2_v (map2_v.Dom());
    MAP<TYPE_SEM_VAL, TYPE_SEM_VAL> res_map;
    Generic i;
    for (bool bb = dom_map2_v.First (i); bb; bb = dom_map2_v.Next (i))
      res_map.Insert (i, map1_v[map2_v[i]]);
    return mk_SEM_MAP(res_map);
  }
  else
    return RTERR::ErrorVal (L"EvalComposeExpr", RTERR_NOT_RNG_DOM_SUBSET, Nil(), Nil(), Sequence());
}

// EvalComposeFctExpr
// fvl1R : SEM`CompExplFN
// fvl2R : SEM`CompExplFN
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalComposeFctExpr (const TYPE_SEM_CompExplFN & fvl1R, const TYPE_SEM_CompExplFN & fvl2R)
{
//  SEQ<TYPE_SEM_ExplFN> fvl2 (fvl2R.get_fl());
//  return TYPE_SEM_CompExplFN().Init(fvl2.ImpConc(fvl1R.get_fl()), Nil());
  return TYPE_SEM_CompExplFN().Init(fvl2R.GetSequence(pos_SEM_CompExplFN_fl)
                                      .Conc(fvl1R.GetSequence(pos_SEM_CompExplFN_fl)), Nil());
}

// EvalIterateExpr
// op1_v : SEM`VAL
// op2_v : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalIterateExpr (const TYPE_SEM_VAL & op1_v, const TYPE_SEM_VAL & op2_v)
{
  switch(op1_v.GetTag()) {
    case TAG_TYPE_SEM_NUM: {
      if (op2_v.Is(TAG_TYPE_SEM_NUM))
        return (EvalNumBinOp (op1_v, NUMEXP, op2_v));
      else
        return RTERR::ErrorVal (L"EvalIterateExpr", RTERR_NUM_EXPECTED, Nil(), Nil(), Sequence());
    }
    case TAG_TYPE_SEM_MAP:        {
      if (AUX::IsNat(op2_v))
        return IterateMap (op1_v, op2_v);
      else
        return RTERR::ErrorVal (L"EvalIterateExpr", RTERR_NAT_EXPECTED, Nil(), Nil(), Sequence());
    }
    case TAG_TYPE_SEM_CompExplFN: {
      if (AUX::IsNat(op2_v))
        return (IterateFct (op1_v, op2_v));
      else
        return RTERR::ErrorVal (L"EvalIterateExpr", RTERR_NAT_EXPECTED, Nil(), Nil(), Sequence());
    }
    default: {
      return RTERR::ErrorVal (L"EvalIterateExpr", RTERR_FN_OR_MAP_OR_NUM_EXPECTED, Nil(), Nil(), Sequence());
    }
  }
}

// IterateFct
// fn_v : SEM`CompExplFN
// num : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::IterateFct (const TYPE_SEM_CompExplFN & fn_v, const TYPE_SEM_NUM & num)
{
  int n = num.GetReal(pos_SEM_NUM_v).GetIntValue();
  if (n >= 0)
  {
    switch(n) {
      case 0: {
        const SEQ<TYPE_SEM_ExplFN> & fl (fn_v.GetSequence(pos_SEM_CompExplFN_fl));
        size_t len_fl = fl.Length();
        SEQ<TYPE_SEM_ExplFN> res_seq;
        for (size_t i = 1; i <= len_fl; i++) {
          TYPE_SEM_ExplFN f (fl[i]); // This is an ExplFN
          f.SetField(pos_SEM_ExplFN_instr, Nil()); // The field inst.
          res_seq.ImpAppend(f);
        }
        return TYPE_SEM_CompExplFN().Init(res_seq, Nil());
      }
      case 1: {
        return fn_v;
      }
      default: {
        SEQ<TYPE_SEM_ExplFN> res_seq;
        for (int i = 0; i < n; i++) {
          res_seq.ImpConc(fn_v.GetSequence(pos_SEM_CompExplFN_fl));
        }
        return TYPE_SEM_CompExplFN().Init(res_seq, Nil());
      }
    }
  }
  else
    return RTERR::ErrorVal (L"IterateFct", RTERR_NAT_EXPECTED, num, Nil(), Sequence());
}

// IterateMap
// MapV : SEM`VAL
// n : nat
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::IterateMap (const TYPE_SEM_MAP & MapV, const TYPE_SEM_NUM & num)
{
  int n = num.GetReal(pos_SEM_NUM_v).GetIntValue();
  if ( n >= 0 ) {
    switch(n) {
      case 0: {
        SET<TYPE_SEM_VAL> dom_map_v (MapV.GetMap(pos_SEM_MAP_v).Dom());
        MAP<TYPE_SEM_VAL,TYPE_SEM_VAL> res;
        Generic i;
        for (bool bb = dom_map_v.First (i); bb; bb = dom_map_v.Next (i))
          res.Insert (i, i);
        return mk_SEM_MAP(res);
      }
      case 1: {
        return MapV;
      }
      default: {
        const Map & map_v (MapV.GetMap(pos_SEM_MAP_v));
        SET<TYPE_SEM_VAL> dom_map_v (map_v.Dom());
        if (!map_v.Rng ().SubSet (dom_map_v)) {
          return RTERR::ErrorVal (L"IterateMap", RTERR_NOT_RNG_DOM_SUBSET, MapV, Nil(), Sequence());
        }
        TYPE_SEM_VAL tmp_res (IterateMap (MapV, mk_SEM_NUM(Real(n - 1))));
        const Map & tmp_v (tmp_res.GetMap(pos_SEM_MAP_v));
        MAP<TYPE_SEM_VAL,TYPE_SEM_VAL> res;
        Generic i;
        for (bool bb = dom_map_v.First (i); bb; bb = dom_map_v.Next (i))
          res.Insert (i, map_v[tmp_v[i]]);
        return mk_SEM_MAP(res);
      }
    }
  }
  else {
    return RTERR::ErrorVal (L"IterateMap", RTERR_NAT_EXPECTED, Int(n), Nil(), Sequence());
  }
}

// EvalOrderExpr
// op1_v : SEM`VAL
// opr : AS`BinaryOp
// op2_v : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalOrderExpr (const TYPE_SEM_VAL & op1_v, const Int & opr, const TYPE_SEM_VAL & op2_v)
{
  Tuple io (AUX::IsOrd(op1_v,op2_v));
  if (io.GetBoolValue(1)) {
    TYPE_STKM_SubProgram sp;
    sp.ImpAppend(TYPE_INSTRTP_LOOKUP().Init(io.GetRecord(2)));
      switch(opr.GetValue()) {
      case NUMLT:
      case NUMGE: {
        sp.ImpAppend(TYPE_INSTRTP_PUSH().Init(mk_sequence(io.GetRecord(3),io.GetRecord(4))));
        break;
      }
      case NUMLE:
      case NUMGT: {
        sp.ImpAppend(TYPE_INSTRTP_PUSH().Init(mk_sequence(io.GetRecord(4),io.GetRecord(3))));
        break;
      }
    }
    sp.ImpAppend(TYPE_INSTRTP_APPLY());
    switch(opr.GetValue()) {
      case NUMLE:
      case NUMGE: {
        sp = theCompiler().ConcIfThenElse(sp,
                TYPE_STKM_SubProgram().ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_false)),
                TYPE_STKM_SubProgram().ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_true)));
        break;
      }
    }
    Tuple res (theStackMachine().EvalAuxProgram(sp, SEQ<Char>(L"Order evaluation"), false));
    const TYPE_STKM_EvaluationState & eval_state (res.GetRecord(1));
    if (eval_state.Is(TAG_TYPE_STKM_Success)) {
      const TYPE_SEM_VAL & Ord_v (res.GetRecord(2));
      if (Ord_v.Is(TAG_TYPE_SEM_BOOL)) {
        return Ord_v;
      }
      else {
        return RTERR::ErrorVal(L"EvalOrderExpr", RTERR_BOOL_EXPECTED, Ord_v, Nil(), Sequence());
      }
    }
    else {
      return RTERR::ErrorVal(L"EvalOrderExpr", RTERR_INTERNAL_ERROR, Nil(), Nil(), Sequence());
    }
  }
  else {
    return EvalNumBinaryExpr( op1_v, opr, op2_v );
  }
}

// EvalNumBinaryExpr
// op1_v : SEM`VAL
// opr : AS`BinaryOp
// op2_v : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalNumBinaryExpr (const TYPE_SEM_VAL & op1_v, const Int & opr, const TYPE_SEM_VAL & op2_v)
{
// 20130409 -->  -- experimental
/*
  if (op1_v.Is(TAG_TYPE_SEM_SEQ) &&
      (op2_v.Is(TAG_TYPE_SEM_NUM) || op2_v.Is(TAG_TYPE_SEM_BOOL)) &&
      (opr == Int(NUMPLUS)))
  {
    Sequence l (op1_v.GetSequence(pos_SEM_SEQ_v)); 
    bool forall = true;
    size_t len_l = l.Length();
    for (size_t i = 1; (i <= len_l) && forall; i++)
      forall = l[i].Is(TAG_TYPE_SEM_CHAR);

    if (forall)
    {
      Sequence num_l (op2_v.Is(TAG_TYPE_SEM_NUM) ? op2_v.GetReal(pos_SEM_NUM_v).ascii()
                                                 : op2_v.GetReal(pos_SEM_BOOL_v).ascii());
      size_t len_num_l = num_l.Length();
      for (size_t i = 1; i <= len_num_l; i++)
        l.ImpAppend(mk_SEM_CHAR(num_l[i]));
      return mk_SEM_SEQ(l);
    }
  }
*/
// <-- 20130409
// 20140324 -->  -- experimental
/*
  if ((opr.GetValue() == NUMMULT) && op1_v.Is(TAG_TYPE_SEM_SET) && op2_v.Is(TAG_TYPE_SEM_SET)) {
    Set s (op1_v.GetSet(pos_SEM_SET_v).DirectProduct(op2_v.GetSet(pos_SEM_SET_v)));
    Set ps;
    Generic e;
    for (bool bb = s.First(e); bb; bb = s.Next(e))
      ps.Insert(mk_SEM_SET(e));
    return mk_SEM_SET(ps);
  }
*/
// <-- 20140324
  if (op1_v.Is(TAG_TYPE_SEM_NUM) && op2_v.Is(TAG_TYPE_SEM_NUM)) {
    switch(opr.GetValue()) {
      case NUMPLUS:
      case NUMMINUS:
      case NUMMULT:
      case NUMDIV:
      case NUMLT:
      case NUMLE:
      case NUMGT:
      case NUMGE:
        return EvalNumBinOp (op1_v, opr, op2_v);
      case NUMMOD:
        return EvalNumMod (op1_v, op2_v);
      case NUMREM:
        return EvalNumRem (op1_v, op2_v);
      case INTDIV:
        return EvalIntDiv (op1_v, op2_v);
      default: { return RTERR::ErrorVal (L"EvalNumBinaryExpr", RTERR_OPERATOR_UNKNOWN, Nil(), Nil(), Sequence()); }
    }
  }
  else if (!op1_v.Is(TAG_TYPE_SEM_NUM))
    return RTERR::ErrorVal (L"EvalNumBinaryExpr", RTERR_NUM_EXPECTED, op1_v, Nil(), Sequence());
  else
    return RTERR::ErrorVal (L"EvalNumBinaryExpr", RTERR_NUM_EXPECTED, op2_v, Nil(), Sequence());
}

// EvalNumBinOp
// VAL1 : SEM`VAL
// opr : AS`BinaryOp
// VAL2 : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalNumBinOp (const TYPE_SEM_VAL & VAL1, int opr, const TYPE_SEM_VAL & VAL2)
{
  switch(opr) {
    case NUMPLUS:  { return mk_SEM_NUM(VAL1.GetReal(pos_SEM_NUM_v).Add(VAL2.GetReal(pos_SEM_NUM_v))); }
    case NUMMINUS: { return mk_SEM_NUM(VAL1.GetReal(pos_SEM_NUM_v).Sub(VAL2.GetReal(pos_SEM_NUM_v))); }
    case NUMMULT:  { return mk_SEM_NUM(VAL1.GetReal(pos_SEM_NUM_v).Mult(VAL2.GetReal(pos_SEM_NUM_v))); }
    case NUMDIV: {
      const Real & n2 (VAL2.GetReal(pos_SEM_NUM_v));
      if (n2.IsZero())
        return RTERR::ErrorVal (L"EvalNumBinOp", RTERR_DIVISION_WITH_ZERO, Nil(), Nil(), Sequence());
      else
        return mk_SEM_NUM(VAL1.GetReal(pos_SEM_NUM_v).Div(n2));
      break;
    }
    case NUMEXP:
      // NUMEXP is used also for iterations, but we leave it here
      // so we can use this to calculate numeric exponentiation
      return mk_SEM_NUM(VAL1.GetReal(pos_SEM_NUM_v).Pow(VAL2.GetReal(pos_SEM_NUM_v)));

    // Relational operators
    case NUMGE:
      return (VAL1.GetReal(pos_SEM_NUM_v).GreaterEqual(VAL2.GetReal(pos_SEM_NUM_v)) ? sem_true : sem_false);
    case NUMGT:
      return (VAL1.GetReal(pos_SEM_NUM_v).GreaterThan(VAL2.GetReal(pos_SEM_NUM_v)) ? sem_true : sem_false);
    case NUMLE:
      return (VAL1.GetReal(pos_SEM_NUM_v).LessEqual(VAL2.GetReal(pos_SEM_NUM_v)) ? sem_true : sem_false);
    case NUMLT:
      return (VAL1.GetReal(pos_SEM_NUM_v).LessThan(VAL2.GetReal(pos_SEM_NUM_v)) ? sem_true : sem_false);
    default: { return RTERR::ErrorVal (L"EvalNumBinOp", RTERR_OPERATOR_UNKNOWN, Nil(), Nil(), Sequence()); }
  }
}

// EvalIntDiv
// VAL1 : SEM`VAL
// VAL2 : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalIntDiv (const TYPE_SEM_VAL & VAL1, const TYPE_SEM_VAL & VAL2)
{
  if ((VAL1.Is(TAG_TYPE_SEM_NUM) ? VAL1.GetReal(pos_SEM_NUM_v).IsInt() : false) &&
      (VAL2.Is(TAG_TYPE_SEM_NUM) ? VAL2.GetReal(pos_SEM_NUM_v).IsInt() : false))
  {
    const Real & n2 (VAL2.GetReal(pos_SEM_NUM_v));

    if (n2.IsZero())
      return RTERR::ErrorVal (L"EvalIntDiv", RTERR_DIVISION_WITH_ZERO, Nil(), Nil(), Sequence());
    else
      return mk_SEM_NUM(VAL1.GetReal(pos_SEM_NUM_v).IntDiv(n2));
  }
  else
    return RTERR::ErrorVal (L"EvalIntDiv", RTERR_TWO_INT_EXPECTED, Nil(), Nil(), Sequence());
}

// EvalNumRem
// op1_v : SEM`VAL
// op2_v : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalNumRem (const TYPE_SEM_VAL & op1_v, const TYPE_SEM_VAL & op2_v)
{
  if ((op1_v.Is(TAG_TYPE_SEM_NUM) ? op1_v.GetReal(pos_SEM_NUM_v).IsInt() : false) &&
      (op2_v.Is(TAG_TYPE_SEM_NUM) ? op2_v.GetReal(pos_SEM_NUM_v).IsInt() : false))
  {
    const Real & n2 (op2_v.GetReal(pos_SEM_NUM_v));

    if (n2.IsZero())
      return RTERR::ErrorVal (L"EvalNumRem", RTERR_DIVISION_WITH_ZERO, Nil(), Nil(), Sequence());
    else
      // n1 rem n2 = n1 - n2 * (n1 div n2)
      return mk_SEM_NUM(op1_v.GetReal(pos_SEM_NUM_v).Rem(n2));
  }
  else
    return RTERR::ErrorVal (L"EvalNumRem", RTERR_TWO_INT_EXPECTED, Nil(), Nil(), Sequence());
}

// EvalNumMod
// op1_v : SEM`VAL
// op2_v : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalNumMod (const TYPE_SEM_VAL & op1_v, const TYPE_SEM_VAL & op2_v)
{
  if ((op1_v.Is(TAG_TYPE_SEM_NUM) ? op1_v.GetReal(pos_SEM_NUM_v).IsInt() : false) &&
      (op2_v.Is(TAG_TYPE_SEM_NUM) ? op2_v.GetReal(pos_SEM_NUM_v).IsInt() : false))
  {
    const Real & n2 (op2_v.GetReal(pos_SEM_NUM_v));

    if (n2.IsZero())
      return RTERR::ErrorVal (L"EvalNumMod", RTERR_DIVISION_WITH_ZERO, Nil(), Nil(), Sequence());
    else
      // n1 mod n2 = n1 - n2 * floor(n1/n2)
      return mk_SEM_NUM(op1_v.GetReal(pos_SEM_NUM_v).Mod(n2));
  }
  else
    return RTERR::ErrorVal (L"EvalNumMod", RTERR_TWO_INT_EXPECTED, Nil(), Nil(), Sequence());
}

// EvalSetBinaryExpr
// op1_v : SEM`VAL
// opr : AS`BinaryOp
// op2_v : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalSetBinaryExpr (const TYPE_SEM_VAL & op1_v, const Int & opr, const TYPE_SEM_VAL & op2_v)
{
  switch(opr.GetValue()) {
    case INSET:        { return EvalInSet (op1_v, op2_v); }
    case NOTINSET:     { return EvalNotInSet (op1_v, op2_v); }
    case SETUNION:     { return EvalSetUnion (op1_v, op2_v); }
    case SETINTERSECT: { return EvalSetIntersect (op1_v, op2_v); }
    case SETMINUS:     { return EvalSetMinus (op1_v, op2_v); }
    case SUBSET:       { return EvalSubSet (op1_v, op2_v); }
    case PROPERSUBSET: { return EvalProperSubSet (op1_v, op2_v); }
    default: { return RTERR::ErrorVal (L"EvalSetBinaryExpr", RTERR_OPERATOR_UNKNOWN, Nil(), Nil(), Sequence()); }
  } // end of switch
}

// EvalInSet
// op1_v : SEM`VAL
// VAL2 : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalInSet (const TYPE_SEM_VAL & op1_v, const TYPE_SEM_VAL & VAL2)
{
  if (VAL2.Is(TAG_TYPE_SEM_SET))
    return ((VAL2.GetSet(pos_SEM_SET_v).InSet (op1_v)) ? sem_true : sem_false);
  else
    return RTERR::ErrorVal (L"EvalInSet", RTERR_SET_EXPECTED, VAL2, Nil(), Sequence());
}

// EvalNotInSet
// op1_v : SEM`VAL
// VAL2 : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalNotInSet (const TYPE_SEM_VAL & op1_v, const TYPE_SEM_VAL & VAL2)
{
  if (VAL2.Is(TAG_TYPE_SEM_SET))
    return ((VAL2.GetSet(pos_SEM_SET_v).InSet (op1_v)) ? sem_false : sem_true);
  else
    return RTERR::ErrorVal (L"EvalNotInSet", RTERR_SET_EXPECTED, VAL2, Nil(), Sequence());
}

// EvalSetUnion
// VAL1 : SEM`VAL
// VAL2 : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalSetUnion (const TYPE_SEM_VAL & VAL1, const TYPE_SEM_VAL & VAL2)
{
  if (VAL1.Is(TAG_TYPE_SEM_SET) && VAL2.Is(TAG_TYPE_SEM_SET))
    return mk_SEM_SET(VAL1.GetSet(pos_SEM_SET_v).Union(VAL2.GetSet(pos_SEM_SET_v)));
  else
    return RTERR::ErrorVal (L"EvalSetUnion", RTERR_TWO_SET_EXPECTED, Nil(), Nil(), Sequence());
}

// EvalSetIntersect
// VAL1 : SEM`VAL
// VAL2 : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalSetIntersect (const TYPE_SEM_VAL & VAL1, const TYPE_SEM_VAL & VAL2)
{
  if (VAL1.Is(TAG_TYPE_SEM_SET) && VAL2.Is(TAG_TYPE_SEM_SET))
    return mk_SEM_SET(VAL1.GetSet(pos_SEM_SET_v).Intersect(VAL2.GetSet(pos_SEM_SET_v)));
  else
    return RTERR::ErrorVal (L"EvalSetIntersect", RTERR_TWO_SET_EXPECTED, Nil(), Nil(), Sequence());
}

// EvalSetMinus
// VAL1 : SEM`VAL
// VAL2 : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalSetMinus (const TYPE_SEM_VAL & VAL1, const TYPE_SEM_VAL & VAL2)
{
  if (VAL1.Is(TAG_TYPE_SEM_SET) && VAL2.Is(TAG_TYPE_SEM_SET))
    return mk_SEM_SET(VAL1.GetSet(pos_SEM_SET_v).Diff(VAL2.GetSet(pos_SEM_SET_v)));
  else
    return RTERR::ErrorVal (L"EvalSetMinus", RTERR_TWO_SET_EXPECTED, Nil(), Nil(), Sequence());
}

// EvalSubSet
// VAL1 : SEM`VAL
// VAL2 : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalSubSet (const TYPE_SEM_VAL & VAL1, const TYPE_SEM_VAL & VAL2)
{
  if (VAL1.Is(TAG_TYPE_SEM_SET) && VAL2.Is(TAG_TYPE_SEM_SET))
    return ((VAL1.GetSet(pos_SEM_SET_v).SubSet(VAL2.GetSet(pos_SEM_SET_v))) ? sem_true : sem_false);
  else
    return RTERR::ErrorVal (L"EvalSubSet", RTERR_TWO_SET_EXPECTED, Nil(), Nil(), Sequence());
}

// EvalProperSubSet
// VAL1 : SEM`VAL
// VAL2 : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalProperSubSet (const TYPE_SEM_VAL & VAL1, const TYPE_SEM_VAL & VAL2)
{
  if (VAL1.Is(TAG_TYPE_SEM_SET) && VAL2.Is(TAG_TYPE_SEM_SET))
  {
    const SET<TYPE_SEM_VAL> & op1_sv (VAL1.GetSet(pos_SEM_SET_v));
    const SET<TYPE_SEM_VAL> & op2_sv (VAL2.GetSet(pos_SEM_SET_v));
    return ((op1_sv.SubSet (op2_sv) && (op1_sv.Card () != op2_sv.Card ())) ? sem_true : sem_false);
  }
  else
    return RTERR::ErrorVal (L"EvalProperSubSet", RTERR_TWO_SET_EXPECTED, Nil(), Nil(), Sequence());
}

//Sequence Operators
// return type: TYPE_SEM_VAL ('hd') | svt_seq | svt_num |

// EvalSeqBinaryExpr
// op1_v : SEM`VAL
// opr : AS`BinaryOp
// op2_v : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalSeqBinaryExpr (const TYPE_SEM_VAL & op1_v, const Int & opr, const TYPE_SEM_VAL & op2_v)
{
  switch (opr.GetValue()) {
    case SEQCONC:
      return EvalSeqConc(op1_v, op2_v);
    default: { return RTERR::ErrorVal (L"EvalSeqBinaryExpr", RTERR_OPERATOR_UNKNOWN, Nil(), Nil(), Sequence()); }
  }
}

// EvalSeqConc
// op1_v : SEM`VAL
// op2_v : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalSeqConc (const TYPE_SEM_VAL & op1_v, const TYPE_SEM_VAL & op2_v)
{
// 20130412 -->
  if (op1_v.Is(TAG_TYPE_SEM_SEQ) && (op2_v.Is(TAG_TYPE_SEM_NUM) || op2_v.Is(TAG_TYPE_SEM_BOOL)))
  {
    Sequence l (op1_v.GetSequence(pos_SEM_SEQ_v)); 
    bool forall = true;
    size_t len_l = l.Length();
    for (size_t i = 1; (i <= len_l) && forall; i++)
      forall = l[i].Is(TAG_TYPE_SEM_CHAR);

    if (forall)
    {
      Sequence num_l (op2_v.Is(TAG_TYPE_SEM_NUM) ? op2_v.GetReal(pos_SEM_NUM_v).ascii()
                                                 : op2_v.GetReal(pos_SEM_BOOL_v).ascii());
      size_t len_num_l = num_l.Length();
      for (size_t i = 1; i <= len_num_l; i++)
        l.ImpAppend(mk_SEM_CHAR(num_l[i]));
      return mk_SEM_SEQ(l);
    }
  }
// <-- 20130412
  if (op1_v.Is(TAG_TYPE_SEM_SEQ) && op2_v.Is(TAG_TYPE_SEM_SEQ))
    return mk_SEM_SEQ(op1_v.GetSequence(pos_SEM_SEQ_v).Conc (op2_v.GetSequence(pos_SEM_SEQ_v)));
  else
    return RTERR::ErrorVal (L"EvalSeqConc", RTERR_TWO_SEQ_EXPECTED, Nil(), Nil(), Sequence());
}

//Map Operators
// return TYPE_SEM_SET | svt_map
// EvalMapUnaryExpr
// opr : AS`UnaryOp
// op_v : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalMapUnaryExpr (const Int & opr, const TYPE_SEM_VAL & op_v)
{
  switch(opr.GetValue()) {
    case MAPDOM:        { return EvalMapDom(op_v); }
    case MAPRNG:        { return EvalMapRng(op_v); }
    case MAPDISTRMERGE: { return EvalMapDistrMerge(op_v); }
    case MAPINVERSE:    { return EvalMapInverse(op_v); }
    default: { return RTERR::ErrorVal (L"EvalMapUnaryExpr", RTERR_OPERATOR_UNKNOWN, Nil(), Nil(), Sequence()); }
  }
}

// EvalMapBinaryExpr
// op1_v : SEM`VAL
// opr : AS`BinaryOp
// op2_v : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalMapBinaryExpr (const TYPE_SEM_VAL & op1_v, const Int & opr, const TYPE_SEM_VAL & op2_v)
{
  switch (opr.GetValue()) {
    case MAPMERGE:     { return EvalMapMerge (op1_v, op2_v); }
    case MAPDOMRESTTO: { return EvalMapDomRestTo (op1_v, op2_v); }
    case MAPDOMRESTBY: { return EvalMapDomRestBy (op1_v, op2_v); }
    case MAPRNGRESTTO: { return EvalMapRngRestTo (op1_v, op2_v); }
    case MAPRNGRESTBY: { return EvalMapRngRestBy (op1_v, op2_v); }
    default: { return RTERR::ErrorVal (L"EvalMapBinaryExpr", RTERR_OPERATOR_UNKNOWN, Nil(), Nil(), Sequence()); }
  }
}

// EvalMapMerge
// VAL1 : SEM`VAL
// VAL2 : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalMapMerge (const TYPE_SEM_VAL & VAL1, const TYPE_SEM_VAL & VAL2)
{
  if (VAL1.Is(TAG_TYPE_SEM_MAP) && VAL2.Is(TAG_TYPE_SEM_MAP))
  {
    Map op1 (VAL1.GetMap(pos_SEM_MAP_v));
    const Map & op2 (VAL2.GetMap(pos_SEM_MAP_v));

    if (op1.IsCompatible(op2))
      return mk_SEM_MAP(op1.ImpOverride (op2));
    else
      return RTERR::ErrorVal (L"EvalMapMerge", RTERR_DUPLICATES_NOT_EQUAL, Nil(), Nil(), Sequence());
  }
  else
    return RTERR::ErrorVal (L"EvalMapMerge", RTERR_TWO_MAP_EXPECTED, Nil(), Nil(), Sequence());
}

// EvalMapDom
// VAL1 : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalMapDom (const TYPE_SEM_VAL & VAL1)
{
  if (VAL1.Is(TAG_TYPE_SEM_MAP))
    return mk_SEM_SET(VAL1.GetMap(pos_SEM_MAP_v).Dom());
  else
    return RTERR::ErrorVal (L"EvalMapDom", RTERR_MAP_EXPECTED, VAL1, Nil(), Sequence());
}

// EvalMapRng
// VAL1 : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalMapRng (const TYPE_SEM_VAL & VAL1)
{
  if (VAL1.Is(TAG_TYPE_SEM_MAP))
    return mk_SEM_SET(VAL1.GetMap(pos_SEM_MAP_v).Rng());
  else
    return RTERR::ErrorVal (L"EvalMapRng", RTERR_MAP_EXPECTED, VAL1, Nil(), Sequence());
}

// EvalMapDistrMerge
// VAL1 : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalMapDistrMerge (const TYPE_SEM_VAL & VAL1)
{
  if (VAL1.Is(TAG_TYPE_SEM_SET))
  {
    SET<TYPE_SEM_VAL> op_sv (VAL1.GetSet(pos_SEM_SET_v)); // not ref for iteration

    // forall elm_v in set op_sv & is_SEM`MAP(elm_v)
    Generic elm_v;
    bool all_is_MAP = true;
    Map res_dv;
    for (bool bb = op_sv.First (elm_v); bb && all_is_MAP; bb = op_sv.Next (elm_v))
    { 
      TYPE_SEM_VAL val (elm_v);
      if (val.Is(TAG_TYPE_SEM_MAP)) {
        const Map & elm_mv (val.GetMap(pos_SEM_MAP_v));
        if(res_dv.IsCompatible(elm_mv))
          res_dv.ImpOverride (elm_mv); // OK since duplicates are identical
        else
          return RTERR::ErrorVal (L"EvalMapDistrMerge", RTERR_MAP_MERGE_DOM_OVERLAP, VAL1, Nil(), Sequence());
      }
      else
        all_is_MAP = false;
    }

    if (all_is_MAP)
      return mk_SEM_MAP(res_dv);
    else
      return RTERR::ErrorVal (L"EvalMapDistrMerge", RTERR_ALL_MAPS_EXPECTED, VAL1, Nil(), Sequence());
  }
  else
    return RTERR::ErrorVal (L"EvalMapDistrMerge", RTERR_SET_EXPECTED, VAL1, Nil(), Sequence());
}

// EvalMapDomRestTo
// VAL_S : SEM`VAL
// VAL_M : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalMapDomRestTo (const TYPE_SEM_VAL & VAL_S, const TYPE_SEM_VAL & VAL_M)
{
  if (VAL_S.Is(TAG_TYPE_SEM_SET) && VAL_M.Is(TAG_TYPE_SEM_MAP))
    return mk_SEM_MAP(VAL_M.GetMap(pos_SEM_MAP_v).DomRestrictedTo(VAL_S.GetSet(pos_SEM_SET_v)));
  else
    return RTERR::ErrorVal (L"EvalMapDomRestTo", RTERR_SET_AND_MAP_EXPECTED, Nil(), Nil(), Sequence());
}

// EvalMapDomRestBy
// VAL_S : SEM`VAL
// VAL_M : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalMapDomRestBy (const TYPE_SEM_VAL & VAL_S, const TYPE_SEM_VAL & VAL_M)
{
  if (VAL_S.Is(TAG_TYPE_SEM_SET) && VAL_M.Is(TAG_TYPE_SEM_MAP))
    return mk_SEM_MAP(VAL_M.GetMap(pos_SEM_MAP_v).DomRestrictedBy(VAL_S.GetSet(pos_SEM_SET_v)));
  else
    return RTERR::ErrorVal (L"EvalMapDomRestBy", RTERR_SET_AND_MAP_EXPECTED, Nil(), Nil(), Sequence());
}

// EvalMapRngRestTo
// VAL_M : SEM`VAL
// VAL_S : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalMapRngRestTo (const TYPE_SEM_VAL & VAL_M, const TYPE_SEM_VAL & VAL_S)
{
  if (VAL_M.Is(TAG_TYPE_SEM_MAP) && VAL_S.Is(TAG_TYPE_SEM_SET))
    return mk_SEM_MAP(VAL_M.GetMap(pos_SEM_MAP_v).RngRestrictedTo(VAL_S.GetSet(pos_SEM_SET_v)));
  else
    return RTERR::ErrorVal (L"EvalMapRngResrTo", RTERR_MAP_AND_SET_EXPECTED, Nil(), Nil(), Sequence());
}

// EvalMapRngRestBy
// VAL_M : SEM`VAL
// VAL_S : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalMapRngRestBy (const TYPE_SEM_VAL & VAL_M, const TYPE_SEM_VAL & VAL_S)
{
  if (VAL_M.Is(TAG_TYPE_SEM_MAP) && VAL_S.Is(TAG_TYPE_SEM_SET))
    return mk_SEM_MAP(VAL_M.GetMap(pos_SEM_MAP_v).RngRestrictedBy(VAL_S.GetSet(pos_SEM_SET_v)));
  else
    return RTERR::ErrorVal (L"EvalMapRngResrBy", RTERR_MAP_AND_SET_EXPECTED, Nil(), Nil(), Sequence());
}

// EvalMapApply
// MapV : SEM`MAP
// arg_lv : seq of SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalMapApply (const TYPE_SEM_MAP & MapV, const SEQ<TYPE_SEM_VAL> & arg_lv)
{
  if (1 == arg_lv.Length ())
  {
    const TYPE_SEM_VAL & arg_v (arg_lv.Hd ());
    Generic res;
    if (MapV.GetMap(pos_SEM_MAP_v).DomExists (arg_v, res))
      return (res);
    else
      return RTERR::ErrorVal (L"EvalMapApply", RTERR_ARG_NOT_IN_DOM, arg_v, Nil(), Sequence());
  }
  else
    return RTERR::ErrorVal (L"EvalMapApply", RTERR_WRONG_NO_OF_ARGS, arg_lv, Nil(), Sequence());
}

// EvalSeqApply
// SeqV : SEM`SEQ
// arg_lv : seq of SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalSeqApply (const TYPE_SEM_SEQ & SeqV, const SEQ<TYPE_SEM_VAL> & arg_lv)
{
  if (1 == arg_lv.Length ())
  {
    const TYPE_SEM_VAL & arg_v (arg_lv.Hd ());
    if (arg_v.Is(TAG_TYPE_SEM_NUM) ? arg_v.GetReal(pos_SEM_NUM_v).IsNatOne() : false)
    {
      int64_t arg = arg_v.GetReal(pos_SEM_NUM_v).GetIntValue ();
      const SEQ<TYPE_SEM_VAL> & bd_lv (SeqV.GetSequence(pos_SEM_SEQ_v));
//      if ((1 <= arg) && (arg <= bd_lv.Length ()))
      if (arg <= bd_lv.Length ())
        return (bd_lv[arg]);
      else
        return RTERR::ErrorVal (L"EvalSeqApply", RTERR_ILLEGAL_INDICES, arg_v, Nil(), Sequence());
    }
    else
      return RTERR::ErrorVal (L"EvalSeqApply", RTERR_NAT_EXPECTED, arg_v, Nil(), Sequence());
  }
  else
    return RTERR::ErrorVal (L"EvalSeqApply", RTERR_WRONG_NO_OF_ARGS, arg_lv, Nil(), Sequence());
}

// EvalEquals (not in spec)
// val1 : SEM`VAL
// val2 : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EXPR::EvalEquals (const TYPE_SEM_VAL & val1, const TYPE_SEM_VAL & val2)
{
  if (val1.GetTag() == val2.GetTag())
  {
    switch(val1.GetTag()) {
#ifdef VDMPP
      case TAG_TYPE_SEM_OBJ_uRef: {
        if (val1 == val2)
          return sem_true;

        const TYPE_AS_Name & clnm1 (val1.GetRecord(pos_SEM_OBJ_uRef_tp));
        const TYPE_AS_Name & clnm2 (val2.GetRecord(pos_SEM_OBJ_uRef_tp));

        if (theState().IsClient(clnm1, clnm2))
          return sem_false;

        TYPE_SEM_OBJ sem1 (theState().GetSemObjInTab(val1));
        TYPE_SEM_OBJ sem2 (theState().GetSemObjInTab(val2));
 
        const TYPE_SEM_InsStrct & is1 (sem1.GetMap(pos_SEM_OBJ_ins));
        const TYPE_SEM_InsStrct & is2 (sem2.GetMap(pos_SEM_OBJ_ins));

        Set us (is1.Dom().Union(is2.Dom()));
        Set is (is1.Dom().Intersect(is2.Dom()));
        Set ds1 (is1.Dom().Diff(is));
        Set ds2 (is2.Dom().Diff(is));

//        if (is1.Dom() != is2.Dom())
//          return sem_false;

        if (!ds1.IsEmpty())
        {
          bool forall = true;
          Generic e;
          for (bool bb = ds1.First(e); bb && forall; bb = ds1.Next(e))
            forall = is1[e].IsEmpty();
          if (!forall)
            return sem_false;
        }
        if (!ds2.IsEmpty())
        {
          bool forall = true;
          Generic e;
          for (bool bb = ds2.First(e); bb && forall; bb = ds2.Next(e))
            forall = is2[e].IsEmpty();
          if (!forall)
            return sem_false;
        }
          
        bool forall = true;
        if (!is.IsEmpty())
        {
          Generic e1;
          for (bool bb = is.First(e1); bb && forall; bb = is.Next(e1))
          {
            TYPE_GLOBAL_ValueMap vm1 (is1[e1]);
            TYPE_GLOBAL_ValueMap vm2 (is2[e1]);
            forall = (vm1.Dom() == vm2.Dom());
            if (!vm1.IsEmpty())
            {
              Set dom_vm1 (vm1.Dom());
              Generic vme1;
              for (bool cc = dom_vm1.First(vme1); cc && forall; cc = dom_vm1.Next(vme1))
              {
                type_dU2P t1 (vm1[vme1]);
                type_dU2P t2 (vm2[vme1]);
                forall = EvalEquals(t1.GetRecord(1), t2.GetRecord(1)).GetBoolValue(pos_SEM_BOOL_v);
              }
            }
          }
        }
        return (forall ? sem_true : sem_false);
        break;
      }
#endif // VDMPP
      case TAG_TYPE_SEM_SEQ: {
        const SEQ<TYPE_SEM_VAL> & v1 (val1.GetSequence(pos_SEM_TUPLE_v));
        const SEQ<TYPE_SEM_VAL> & v2 (val2.GetSequence(pos_SEM_TUPLE_v));
        size_t len_v1 = v1.Length() ;
        bool forall = (len_v1 == (size_t)v2.Length());
        for (size_t idx = 1; (idx <= len_v1) && forall; idx++)
          forall = EvalEquals(v1[idx], v2[idx]).GetBoolValue(pos_SEM_BOOL_v);
        return (forall ? sem_true : sem_false);
        break;
      }
      case TAG_TYPE_SEM_SET: {
        SET<TYPE_SEM_VAL> v1 (val1.GetSet(pos_SEM_SET_v));
        SET<TYPE_SEM_VAL> v2 (val2.GetSet(pos_SEM_SET_v));
        bool forall = (v1.Card() == v2.Card());
        if (!v1.IsEmpty())
        {
          Generic e1;
          for (bool bb = v1.First(e1); bb && forall; bb = v1.Next(e1))
          {
            bool exists = false;
            Generic e2;
            for (bool cc = v2.First(e2); cc && !exists; cc = v2.First(e2))
              exists = EvalEquals(e1, e2).GetBoolValue(pos_SEM_BOOL_v);
            forall = exists;
            if(exists)
              v2.RemElem(e2);
          }
        }
        return (forall ? sem_true : sem_false);
        break;
      }
      case TAG_TYPE_SEM_MAP: {
        const Map & v1 (val1.GetMap(pos_SEM_MAP_v));
        Map v2 (val2.GetMap(pos_SEM_MAP_v));
        bool forall = (v1.Size() == v2.Size());
        if (!v1.IsEmpty())
        {
          Set dom_v1 (v1.Dom());
          Generic e1;
          for (bool bb = dom_v1.First(e1); bb && forall; bb = dom_v1.Next(e1))
          {
            const TYPE_SEM_VAL & r1 (v1[e1]);
            bool exists = false;
            Set dom_v2 (v2.Dom());
            Generic e2;
            for (bool cc = dom_v2.First(e2); cc && !exists; cc = dom_v2.First(e2))
              exists = EvalEquals(e1, e2).GetBoolValue(pos_SEM_BOOL_v) &&
                       EvalEquals(r1, v2[e2]).GetBoolValue(pos_SEM_BOOL_v);
            forall = exists;
            if(exists)
              v2.RemElem(e2);
          }
        }
        return (forall ? sem_true : sem_false);
        break;
      }
      case TAG_TYPE_SEM_TUPLE: {
        const SEQ<TYPE_SEM_VAL> & v1 (val1.GetSequence(pos_SEM_TUPLE_v) );
        const SEQ<TYPE_SEM_VAL> & v2 (val2.GetSequence(pos_SEM_TUPLE_v) );
        size_t len_v1 = v1.Length() ;
        bool forall = (len_v1 == (size_t)v2.Length());
        for (size_t idx = 1; (idx <= len_v1) && forall; idx++)
          forall = EvalEquals(v1[idx], v2[idx]).GetBoolValue(pos_SEM_BOOL_v);
        return (forall ? sem_true : sem_false);
        break;
      }
      case TAG_TYPE_DYNSEM_SEM_REC: {
        const TYPE_AS_Name & tag1 (val1.GetRecord(pos_DYNSEM_SEM_SemRecord_tag));
        const TYPE_AS_Name & tag2 (val2.GetRecord(pos_DYNSEM_SEM_SemRecord_tag));
        SEQ<TYPE_SEM_VAL> v1 (val1.GetRecord(pos_DYNSEM_SEM_SemRecord_value).GetFields());
        SEQ<TYPE_SEM_VAL> v2 (val2.GetRecord(pos_DYNSEM_SEM_SemRecord_value).GetFields());
        size_t len_v1 = v1.Length();
        bool forall = ((tag1 == tag2) && (len_v1 == (size_t)v2.Length()));
        for (size_t idx = 1; (idx <= len_v1) && forall; idx++)
          forall = EvalEquals(v1[idx], v2[idx]).GetBoolValue(pos_SEM_BOOL_v);
        return (forall ? sem_true : sem_false);
        break;
      }
      default: {
        return ((val1 == val2) ? sem_true : sem_false);
      }
    }
  }
  return sem_false;
}
