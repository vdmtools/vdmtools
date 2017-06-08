/***
*  * WHAT
*  *    SD: State Designator. 
*  * Id
*  *    $Id: evalsd.cc,v 1.66 2006/09/22 11:14:03 vdmtools Exp $
*  * SPECIFICATION VERSION
*  *    evalsd.cc: Implementation of eval_state.vdm  1.133
*  *               Provides implementation of
*  *                 EvalStateDesignator
*  *                 LookUpSD
*  *                 ModifyValueId
*  *                 ModifyValue
*  *                 ModifyInstanceVar
*  * <<<<Remember>>>> update version number in libman.cc
***/

#include "metaiv.h"
#include "AS.h"
#include "stackeval.h"
#include "libman.h"
#include "astaux.h"
#include "asquotes.h"
#include "evalaux.h"
#include "rt_errmsg.h"
#include "GLOBAL.h"
#include "semvalues.h"
#ifdef VICE
#include "valcond.h"
#endif // VICE
#include "settings.h"
#include "tbutils.h"

#ifdef VDMPP
// From eval_state.vdm
// ModifyInstanceVar
// obj_ref : SEM`OBJ_Ref
// index_l : seq of (AS`Name | SEM`VAL)
// val_v : SEM`VAL
// ==> SEM`OBJ_Ref
TYPE_SEM_OBJ_uRef EvalState::ModifyInstanceVar(const TYPE_SEM_OBJ_uRef & obj_ref, 
                                               const SEQ<Record> & index_l,
                                               const TYPE_SEM_VAL & val_v)
{
  // Dereference the sequence index
  const Record & index (index_l.Hd()); // (AS`Name | SEM`VAL)

  // If this sequence index is not a name, something has gone wrong
  if (index.Is(TAG_TYPE_AS_Name))
  {
// 20091124
    TYPE_GLOBAL_OBJ_uDesc obj_v (Lookup_obj_tab(obj_ref));
    TYPE_SEM_OBJ obj (obj_v.GetRecord(pos_GLOBAL_OBJ_uDesc_sem));
    TYPE_SEM_InsStrct ins (obj.GetMap(pos_SEM_OBJ_ins));
    const TYPE_AS_Name & clsnm_ (obj.GetRecord(pos_SEM_OBJ_tp));
    TYPE_AS_Name clsnm (clsnm_);

// not in spec
// 20090822 -->
    if (!ins[clsnm].DomExists(index))
    {
      Set dom_ins (ins.Dom());
      dom_ins.RemElem(clsnm_);
      Set nm_s;
      Generic nm;
      for (bool bb = dom_ins.First(nm); bb ; bb = dom_ins.Next(nm))
      {
        if (ins[nm].DomExists(index))
          nm_s.Insert(nm);
      }
      Tuple eoc (ExistsOneChild (nm_s, nm_s));
      if (eoc.GetBoolValue(1))
        clsnm = eoc.GetRecord(2);
    }
// <-- 20090822

    TYPE_GLOBAL_ValueMap insmap (ins[clsnm]); // map AS`Name to (SEM`VAL * AS`Access)
    if (insmap.DomExists(index))
    {
      Tuple t (insmap[index]); // (SEM`VAL * AS`Access)
      const TYPE_AS_Access & access (t.GetField(2));
      if (access == Int(PUBLIC_AS))
      {
        insmap.ImpModify(index, mk_(ModifyValue(t.GetField(1), index_l.Tl(), val_v), access));
        ins.ImpModify(clsnm, insmap);
        obj.SetField(pos_SEM_OBJ_ins, ins);
        obj_v.SetField(pos_GLOBAL_OBJ_uDesc_sem, obj);
// 20091124
        Update_obj_tab(obj_ref, obj_v);

// 20100818 -->
        if (Settings.INV())
        {
          theStackMachine().PushEmptyEnv();
          theStackMachine().PushCurObj(obj_ref, obj_ref.GetRecord(pos_SEM_OBJ_uRef_tp), theStackMachine().GetCurCl());

          bool invOk = CheckGlobInv();

          theStackMachine().PopCurObj();
          theStackMachine().PopEnvL();

          if (!invOk)
          {
            theStackMachine().SetCid(index.GetInt(pos_AS_Name_cid)); 
            RTERR::Error(L"ModifyInstanceVar", RTERR_INST_INV_BROKEN, Nil(), Nil(), Sequence());
          }
        }
// <-- 20100818
      }
      else
      {
        RTERR::Error(L"ModifyInstanceVar", RTERR_NOT_IN_SCOPE,
                     M42Sem(AUX::SingleNameToString(ASTAUX::Combine2Names(clsnm, index)), NULL), Nil(), Sequence());
      }
    }
    else
    {
      RTERR::Error (L"ModifyInstanceVar", RTERR_REF_UNKNOWN, obj_ref, Nil(), Sequence());
    }
  }
  else // if (!index.Is(TAG_TYPE_AS_Name))
  {
    RTERR::Error(L"ModifyInstanceVar", RTERR_REF_UNKNOWN, obj_ref, Nil(), Sequence());
  }
  return obj_ref;
}
#endif //VDMPP

// IsMapType
// tp : AS`Type
// ==> bool
bool EvalState::IsMapType(const TYPE_AS_Type & tp) const
{
  switch(tp.GetTag()) {
    case TAG_TYPE_AS_GeneralMap0Type: { return true; }
    case TAG_TYPE_AS_GeneralMap1Type: { return true; }
    case TAG_TYPE_AS_InjectiveMap0Type:  { return true; }
    case TAG_TYPE_AS_InjectiveMap1Type:  { return true; }
    case TAG_TYPE_AS_UnionType: {
      const SEQ<TYPE_AS_Type> tps (tp.GetSequence(pos_AS_UnionType_tps));
      bool exists = false;
      size_t len_tps = tps.Length();
      for (size_t idx = 1; (idx <= len_tps) && !exists; idx++)
        exists = IsMapType(tps[idx]);
      return exists;
    }
    case TAG_TYPE_AS_OptionalType: { return IsMapType(tp.GetRecord(pos_AS_OptionalType_tp)); }
    case TAG_TYPE_AS_BracketedType: { return IsMapType(tp.GetRecord(pos_AS_BracketedType_tp)); }
    default: { return false; }
  }
}

// GetRngType
// tp : AS`Type
// ==> AS`Type
TYPE_AS_Type EvalState::GetRngType(const TYPE_AS_Type & tp) const
{
  switch(tp.GetTag()) {
    case TAG_TYPE_AS_GeneralMap0Type: { return tp.GetRecord(pos_AS_GeneralMap0Type_maprng); }
    case TAG_TYPE_AS_GeneralMap1Type: { return tp.GetRecord(pos_AS_GeneralMap1Type_maprng); }
    case TAG_TYPE_AS_InjectiveMap0Type: { return tp.GetRecord(pos_AS_InjectiveMap0Type_maprng); }
    case TAG_TYPE_AS_InjectiveMap1Type: { return tp.GetRecord(pos_AS_InjectiveMap1Type_maprng); }
    case TAG_TYPE_AS_UnionType: {
      const SEQ<TYPE_AS_Type> tps (tp.GetSequence(pos_AS_UnionType_tps));
      SEQ<TYPE_AS_Type> tp_l;
      size_t len_tps = tps.Length();
      for (size_t idx = 1; idx <= len_tps; idx++)
      {
        const TYPE_AS_Type & t (tps[idx]);
        switch(t.GetTag()) {
          case TAG_TYPE_AS_GeneralMap0Type: {
            tp_l.ImpAppend(t.GetRecord(pos_AS_GeneralMap0Type_maprng));
            break;
          }
          case TAG_TYPE_AS_GeneralMap1Type: {
            tp_l.ImpAppend(t.GetRecord(pos_AS_GeneralMap1Type_maprng));
            break;
          }
          case TAG_TYPE_AS_InjectiveMap0Type: {
            tp_l.ImpAppend(t.GetRecord(pos_AS_InjectiveMap0Type_maprng));
            break;
          }
          case TAG_TYPE_AS_InjectiveMap1Type: {
            tp_l.ImpAppend(t.GetRecord(pos_AS_InjectiveMap1Type_maprng));
            break;
          }
        }
      }
      switch(tp_l.Length()) {
        case 0: {
          RTERR::Error (L"GetRngType", RTERR_INTERNAL_ERROR, Nil(), Nil(), Sequence());
          return TYPE_AS_Type(); 
        }
        case 1: {
          return tp_l[1];
        }
        default: {
          return TYPE_AS_UnionType().Init(tp_l, NilContextId);
        }
      }
    }
    case TAG_TYPE_AS_OptionalType: { return GetRngType(tp.GetRecord(pos_AS_OptionalType_tp)); }
    case TAG_TYPE_AS_BracketedType: { return GetRngType(tp.GetRecord(pos_AS_BracketedType_tp)); }
    default: {
      RTERR::Error (L"GetRngType", RTERR_INTERNAL_ERROR, Nil(), Nil(), Sequence());
      return TYPE_AS_Type(); 
    }
  }
}

// UpdateVal
// val : SEM`VAL
// tp : AS`Type
// index_l : seq of (AS`Name | SEM`VAL)
// ==> SEM`VAL
TYPE_SEM_VAL EvalState::UpdateVal (const TYPE_SEM_VAL & val, const TYPE_AS_Type & tp, const Sequence & index_l) const
{
  if (IsMapType(tp) && !index_l.IsEmpty())
  {
    TYPE_AS_Type maprng (GetRngType(tp));
   
    TYPE_SEM_VAL key (index_l.Hd());
    switch(val.GetTag()) {
      case TAG_TYPE_SEM_MAP: {
        Map m (val.GetMap(pos_SEM_MAP_v));
        if (m.DomExists(key))
          m.ImpModify(key, UpdateVal(m[key], maprng, index_l.Tl()));
        else
          m.ImpModify(key, UpdateVal(sem_undef, maprng, index_l.Tl()));
        return mk_SEM_MAP(m);
      }
      case TAG_TYPE_SEM_UNDEF: {
        Map m;
        m.ImpModify(key, UpdateVal(sem_undef, maprng, index_l.Tl()));
        return mk_SEM_MAP(m);
      }
      default: {
        return val;
      }
    }
  }
  return val;
}

// CheckMap
// lusd : 
// ==> 
Tuple EvalState::CheckMap (const Tuple & lusd) const
{
//  const TYPE_SEM_VAL & scomp_v (lusd.GetRecord(1)); // SEM`VAL
  const Sequence & index_l (lusd.GetSequence(2)); // seq of (AS`Name | SEM`VAL)
  const Generic & glres (lusd.GetSequence(3)); // global lookup result
  const Generic & llres (lusd.GetSequence(4)); // local lookup result

  if (!llres.IsNil() && Tuple(llres).GetBoolValue(1))
  {
    TYPE_SEM_ValTp gs (Tuple(llres).GetRecord(2));
    const TYPE_SEM_VAL & val (gs.GetRecord(pos_SEM_ValTp_val));
    const TYPE_AS_Type & tp (gs.GetRecord(pos_SEM_ValTp_tp)); 

    return mk_(UpdateVal(val, tp, index_l.Tl()), index_l, glres, llres);
  }
#ifdef VDMSL
  if (!glres.IsNil() && Tuple(glres).GetBoolValue(1))
  {
    TYPE_GLOBAL_State gs (Tuple(glres).GetRecord(2));
    const TYPE_SEM_VAL & val (gs.GetRecord(pos_GLOBAL_State_val));
    const TYPE_AS_Type & tp (gs.GetRecord(pos_GLOBAL_State_tp)); 

    return mk_(UpdateVal(val, tp, index_l.Tl()), index_l, glres, llres);
  }
#endif // VDMSL
#ifdef VDMPP
  if (!glres.IsNil() && Tuple(glres).GetBoolValue(1) && Tuple(glres).GetBoolValue(2))
  {
    const TYPE_SEM_VAL & val (Tuple(glres).GetRecord(3));
    const TYPE_AS_Type & tp (Tuple(glres).GetRecord(4)); 

    return mk_(UpdateVal(val, tp, index_l.Tl()), index_l, glres, llres);
  }
#endif // VDMPP
  return lusd;
}

// From eval_state.vdm
// EvalStateDesignator
// sd : STKM`StateDesignator
// val_v : SEM`VAL
void EvalState::EvalStateDesignator (const TYPE_STKM_StateDesignator & sd, const TYPE_SEM_VAL & val_v)
{
  Tuple lusd (CheckMap(LookUpSD (sd))); // SEM`VAL * seq of (AS`Name | SEM`VAL) 

  const TYPE_SEM_VAL & scomp_v (lusd.GetRecord(1)); // SEM`VAL
  const Sequence & index_l (lusd.GetSequence(2));   // seq of (AS`Name | SEM`VAL)
  const Generic & glres (lusd.GetSequence(3));      // global lookup result
  const Generic & llres (lusd.GetSequence(4));      // local lookup result

  TYPE_SEM_VAL val (ModifyValue(scomp_v, index_l.Tl(), val_v));
  ModifyValueId(index_l.Hd(), val, glres, llres);
}

// Auxiliary function - no corresponding operation in spec
// ExpandTypeDef
// tp : GLOBAL`Type
// ==> GLOBAL`Type * AS`Name
Tuple EvalState::ExpandTypeDef(const TYPE_AS_Type & tp)
{
  TYPE_GLOBAL_Type tmp_type (tp);
  TYPE_AS_Name tag;
  while (tmp_type.Is (TAG_TYPE_AS_TypeName)) {
    const TYPE_AS_Name & tn (tmp_type.GetRecord(pos_AS_TypeName_name));
    // SL : bool * [AS`Type] * [AS`Invariant] * [AS`Equal] * [AS`Order] * [AS`Name] * [AS`Access]
    // PP : bool * [GLOBAL`Type] * [AS`Invariant] * [AS`Equal] * [AS`Order] * [AS`Name] * [AS`Access]
    //Tuple itd (IsTypeDef(tn));
    Tuple itd (GetCachedTypeDef(tn));
    if (itd.GetBool(1)) {
      tmp_type = itd.GetRecord(2); // AS`Type, GLOBAL`Type
      tag = tn;
    }
    else  {
      RTERR::Error (L"ExpandTypeDef", RTERR_TYPE_NOT_IN_SCOPE,
                    M42Sem(AUX::SingleNameToString(tn), NULL), Nil(), Sequence());
    }
  }
  return mk_(tmp_type, tag);
}

// From eval_state.vdm
#if defined(VDMSL) || defined(VDMPP)
// LookUpSD
// sd : STKM`StateDesignator (= AS`Name | STKM`FieldRef | STKM`MapOrSeqRef)
// VDMSL
// ==> SEM`VAL * seq of (AS`Name | SEM`VAL) * [ bool * [GLOBAL`State]] * [ bool * [SEM`ValTp]]
// VDMPP
// ==> SEM`VAL * seq of (AS`Name | SEM`VAL) *
//     [bool * bool * [SEM`VAL] * [AS`Type] * [AS`Name] * [AS`Access]] * [ bool * [SEM`ValTp]]
Tuple EvalState::LookUpSD (const TYPE_STKM_StateDesignator & sd)
{
  switch(sd.GetTag ()) {
    case TAG_TYPE_AS_Name: {
      // a state designator name can only be a single identifier
      Tuple ils (theStackMachine().IsLocalState(sd)); // bool * [GLOBAL`State]
      if (ils.GetBoolValue(1)) { // sd is in Local State
        if (ils.GetField(2).IsNil()) {
          RTERR::Error (L"LookUpSD", RTERR_NOT_IN_SCOPE,
                        M42Sem(AUX::SingleNameToString(sd), NULL), Nil(), Sequence());
          return mk_(sem_undef, Sequence(), Nil(), Nil()); 
        }
        const TYPE_SEM_ValTp & ls_var (ils.GetRecord(2));
        const TYPE_SEM_VAL & val (ls_var.GetRecord(pos_SEM_ValTp_val));

        TYPE_SEM_VAL pval_v (val.Is(TAG_TYPE_SEM_UNDEF) ? CreateUndefVal(ls_var.GetRecord(pos_SEM_ValTp_tp)) : val);

        // *index_l is an empty seq here.
        //SEQ<Record> index_l (mk_sequence(sd)); // seq of (AS`Name | SEM`VAL)

        //return mk_(pval_v, index_l, Nil(), ils);
        return mk_(pval_v, mk_sequence(sd), Nil(), ils);
      }

#ifdef VDMSL
      Tuple igs (IsGlobalState(sd, theStackMachine().CurrentModule())); // bool * [GLOBAL`State]
      if (igs.GetBoolValue(1)) { // sd is in Global State
        const TYPE_GLOBAL_State & gs_var (igs.GetRecord(2));
        const TYPE_SEM_VAL & val (gs_var.GetRecord(pos_GLOBAL_State_val));
        const TYPE_AS_Type & type (gs_var.GetRecord(pos_GLOBAL_State_tp));

        TYPE_SEM_VAL pval_v (val.Is(TAG_TYPE_SEM_UNDEF) ? CreateUndefVal(type) : val);

        // index_l is an empty seq here.
        //SEQ<Record> index_l (mk_sequence(sd)); // seq of (AS`Name | SEM`VAL)

        //return mk_(pval_v, index_l, igs, Nil());
        return mk_(pval_v, mk_sequence(sd), igs, Nil());
      }
#endif // VDMSL

#ifdef VDMPP
      // ==> bool * bool * [SEM`VAL] * [AS`Type] * [AS`Name] * [AS`Access]
      Tuple iios (IsInObjScope(sd, Nil()));
      if (iios.GetBoolValue(1)) { // sd is in Object Scope
        if (iios.GetField(3).IsNil()) { // [SEM`VAL]
          RTERR::Error (L"LookUpSD", RTERR_INSTVAR_NOT_IN_SCOPE,
                        M42Sem(AUX::SingleNameToString(sd), NULL), Nil(), Sequence());
        }
        const TYPE_SEM_VAL & val (iios.GetRecord(3));
        const TYPE_AS_Type & type (iios.GetRecord(4));

        TYPE_SEM_VAL pval_v (val.Is(TAG_TYPE_SEM_UNDEF) ? CreateUndefVal(type) : val);

        // index_l is an empty seq here.
        //SEQ<Record> index_l (mk_sequence(sd)); // seq of (AS`Name | SEM`VAL)

        //return mk_(pval_v, index_l, iios, Nil());
        return mk_(pval_v, mk_sequence(sd), iios, Nil());
      }

      Tuple lus (LookUpStatic(sd));
      if (lus.GetBoolValue(1)) {
        const TYPE_SEM_VAL & val (lus.GetRecord(2));
        const TYPE_AS_Name & clnm (lus.GetRecord(3));
        const TYPE_AS_Access & access (lus.GetRecord(4));

        TYPE_AS_Name thename (AUX::ExtractName(sd));
        TYPE_AS_Type type (GetInstVarsTp(clnm)[thename]);

        TYPE_SEM_VAL pval_v (val.Is(TAG_TYPE_SEM_UNDEF) ? CreateUndefVal(type) : val);

        //SEQ<Record> index_l (mk_sequence(sd)); // seq of (AS`Name | SEM`VAL)

        Tuple res (mk_(Bool(true), Bool(false), pval_v, type, clnm, access));
        //return mk_(pval_v, index_l, res, Nil());
        return mk_(pval_v, mk_sequence(sd), res, Nil());
      }

      TYPE_SEM_VAL val (LookUp(sd));
      if (val.Is(TAG_TYPE_SEM_OBJ_uRef)) {
        //SEQ<Record> index_l (mk_sequence(sd)); // seq of (AS`Name | SEM`VAL)
        //return mk_(val, index_l, Nil(), Nil());
        return mk_(val, mk_sequence(sd), Nil(), Nil());
      }
#endif // VDMPP

      RTERR::Error (L"LookUpSD", RTERR_STATE_DESIG_UNKNOWN,
                    M42Sem(AUX::SingleNameToString(sd), NULL), Nil(), Sequence());
    }
    case TAG_TYPE_STKM_FieldRef: {
      const TYPE_STKM_StateDesignator & fsd (sd.GetRecord (pos_STKM_FieldRef_var));
      const TYPE_AS_Name & sel (sd.GetRecord (pos_STKM_FieldRef_sel));
// 20141210 -->
#ifdef VDMPP
      if (fsd.Is(TAG_TYPE_AS_SelfExpr)) {
        return LookUpSD(sel);
      }
#endif // VDMPP
// <-- 20141210
      Tuple lusd (LookUpSD (fsd));
      const TYPE_SEM_VAL & scomp_v (lusd.GetRecord(1)); // SEM`VAL
      SEQ<Record> index_l (lusd.GetSequence(2)); // seq of (AS`Name | SEM`VAL)
      index_l.ImpAppend(sel);
      return mk_(scomp_v, index_l, lusd.GetField(3), lusd.GetField(4));
    }
    case TAG_TYPE_STKM_MapOrSeqRef: {
      const TYPE_STKM_StateDesignator & msd (sd.GetRecord (pos_STKM_MapOrSeqRef_var));
      const TYPE_SEM_VAL & arg (sd.GetRecord (pos_STKM_MapOrSeqRef_arg));
      Tuple lusd (LookUpSD (msd));
      const TYPE_SEM_VAL & scomp_v (lusd.GetRecord(1)); // SEM`VAL
      SEQ<Record> index_l (lusd.GetSequence(2)); // seq of (AS`Name | SEM`VAL)
      index_l.ImpAppend(arg);
      return mk_(scomp_v, index_l, lusd.GetField(3), lusd.GetField(4));
    }
    default: {
      RTERR::Error (L"LookUpSD", RTERR_STATE_DESIG_UNKNOWN, Nil(), Nil(), Sequence());
    }
  }
  return mk_(sem_undef, Sequence(), Nil(), Nil());  // dummy
}

// CreateUnvefVal
// type : GLOBAL`Type
// ==> SEM`VAL
TYPE_SEM_VAL EvalState::CreateUndefVal(const TYPE_GLOBAL_Type & type)
{
  Tuple etd (ExpandTypeDef(type)); // GLOBAL`Type * AS`Name
  const TYPE_GLOBAL_Type & tmp_type (etd.GetRecord(1));

  if (tmp_type.Is (TAG_TYPE_AS_CompositeType)) {
    const TYPE_AS_Name & tag (etd.GetRecord(2));
#ifdef VDMSL
    TYPE_AS_Name quotedtag (AUX::UniqueTagName(tag));
#endif // VDMSL
#ifdef VDMPP
//    Tuple etn (AUX::ExtractTagName(tag));            
    Tuple etn (ExtractTagNameCached(tag));            
    const TYPE_AS_Name & quotedtag (etn.GetRecord(1));
#endif // VDMPP
    return SemRec::CreateSemRecordUndefinedFields(tmp_type, quotedtag);
  }
  else {
    return sem_undef;
  }
}

// ModifyValueId
// id : AS`Name
// val_v : SEM`VAL
// VDMSL
// glres : [bool * [GLOBAL`State]]
// VDMPP
// glres : [bool * bool * [SEM`VAL] * [AS`Type] * [AS`Name] * [AS`Access]]
// llres : [bool * [SEM`ValTp]]
// ==> ()
void EvalState::ModifyValueId(const TYPE_AS_Name & id, const TYPE_SEM_VAL & val_v,
                              const Generic & glres_, const Generic & llres_)
{
  if (!llres_.IsNil()) { // sd is in Local State
    Tuple ils (llres_);
    const TYPE_SEM_ValTp & ls_var (ils.GetRecord(2)); // SEM`ValTp
    const TYPE_AS_Type & tp (ls_var.GetRecord(pos_SEM_ValTp_tp)); // AS`Type
    if (Settings.DTC() && !SubType (val_v, tp)) {
      RTERR::Error (L"ModifyValueId", RTERR_TYPE_INCOMP, val_v, tp, Sequence());
    }
    else {
      theStackMachine().SetLocalState(id, TYPE_SEM_ValTp().Init(val_v, tp)); 
    }
  }
  else {
#ifdef VDMSL
    if (!glres_.IsNil()) { // sd is in Global State
      Tuple igs (glres_);
      const TYPE_GLOBAL_State & gs_var (igs.GetRecord(2)); // GLOBAL`State
      const TYPE_AS_Type & tp (gs_var.GetRecord(pos_GLOBAL_State_tp));  // AS`Type
      if (Settings.DTC() && !SubType (val_v, tp))
        RTERR::Error (L"ModifyValueId", RTERR_TYPE_INCOMP, val_v, tp, Sequence());
      else
        SetGlobalState(id, theStackMachine().CurrentModule(), TYPE_GLOBAL_State().Init(val_v, tp)); 
    }
#endif // VDMSL
#ifdef VDMPP
    if (!glres_.IsNil()) { // sd is in Object Scope
      Tuple iios (glres_);
      if (Settings.DTC() && !SubType (val_v, iios.GetRecord(4))) {
          RTERR::Error (L"ModifyValueId", RTERR_TYPE_INCOMP, val_v, iios.GetRecord(4), Sequence());
      }
      SetInstanceVar(id, val_v, iios);
    }
    else if (val_v.Is(TAG_TYPE_SEM_OBJ_uRef) && IsInObjTab(val_v)) {
    }
    else {
      RTERR::Error(L"ModifyValueId", RTERR_INTERNAL_ERROR,
                   M42Sem(AUX::SingleNameToString(id), NULL), Nil(), Sequence());
    }
#endif // VDMPP
  }
}

// ModifyValue
// scomp_v : SEM`VAL
// index_l : seq of (AS`Name | SEM`VAL)
// val_v : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EvalState::ModifyValue(const TYPE_SEM_VAL & scomp_v,
                                    const SEQ<Record> & index_l,
                                    const TYPE_SEM_VAL & val_v)
{
  if (index_l.IsEmpty ()) {
    return val_v;
  }
  else {
    switch(scomp_v.GetTag ()) {
#ifdef VDMPP
      case TAG_TYPE_SEM_OBJ_uRef: {
        return ModifyInstanceVar(scomp_v, index_l, val_v);
      }
#endif //VDMPP
      case TAG_TYPE_DYNSEM_SEM_REC: {
        const TYPE_AS_Name & tag (scomp_v.GetRecord(pos_DYNSEM_SEM_SemRecord_tag));
// 20101102 -->
        //Tuple lurs (AUX::LookUpRecSel(tag)); // bool * GLOBAL`RecSel
        Tuple lurs (GetCachedRecSel(tag));
// <-- 20101102
        if (lurs.GetBoolValue(1)) {
          const MAP<TYPE_AS_Name, Int> & pos (lurs.GetTuple(2).GetMap (2));  // map AS`Name to nat
          const TYPE_AS_Name & sel (index_l.Hd());
          if (pos.DomExists (sel)) {
            const Record & value (scomp_v.GetRecord(pos_DYNSEM_SEM_SemRecord_value));
            SEQ<TYPE_SEM_VAL> tmp_v (value.GetFields());
            int index = pos[sel].GetValue();
            const TYPE_SEM_VAL & val (tmp_v[index]);
// 20131227 -->
            //tmp_v.ImpModify(index, ModifyValue(val, index_l.Tl(), val_v));
            //return SemRec::mk_SEM_REC(tag, tmp_v);
            TYPE_SEM_VAL newval (ModifyValue(val, index_l.Tl(), val_v));
            bool checked(false);
            //tmp_v.ImpModify(index, newval);
            //TYPE_SEM_VAL newrec (SemRec::mk_SEM_REC(tag, tmp_v)); 
            if (Settings.DTC()) {
#ifdef VDMSL
              int modulepushed = false; // We only push a new module if name is qualified.
              if (AUX::IsRealName (tag)) {
                TYPE_AS_Name mod_nm (ExtractModule(tag)); 
                if (mod_nm != theStackMachine().CurrentModule()) {
                  theStackMachine().PushModule(mod_nm);
                  modulepushed = true;
                }
              }
#endif //VDMSL
              const SEQ<TYPE_AS_Type> & tp_l (lurs.GetTuple(2).GetSequence (3));  // seq of AS`Type
              if (!SubType (newval, tp_l[index]))
                RTERR::Error (L"ModifyValueId", RTERR_TYPE_INCOMP, newval, tp_l[index], Sequence());
#ifdef VDMSL
              if (modulepushed) {
                theStackMachine().PopModule();
              }
#endif // VDMSL
              //newrec.SetField(pos_DYNSEM_SEM_SemRecord_checked, Bool(true));
              checked = true;
            }
            tmp_v.ImpModify(index, newval);
            TYPE_SEM_VAL newrec (SemRec::mk_SEM_REC(tag, tmp_v)); 
            newrec.SetField(pos_DYNSEM_SEM_SemRecord_checked, Bool(checked));
            return newrec;
// <-- 20131227
          }
          else {
            RTERR::Error (L"ModifyValue", RTERR_RECORD_FIELD_ID_UNKNOWN,
                          M42Sem(AUX::SingleNameToString(sel), NULL), Nil(), Sequence());
          }
        }
        else {
          //RTERR::Error (L"ModifyValue", RTERR_REC_EXPECTED,
          RTERR::Error (L"ModifyValue", RTERR_TAG_UNKNOWN,
                        M42Sem(AUX::SingleNameToString(tag), NULL), Nil(), Sequence());
        }
        return TYPE_SEM_VAL(); // dummy
      } // TAG_TYPE_DYNSEM_SEM_REC
      case TAG_TYPE_SEM_MAP: {
        MAP<TYPE_SEM_VAL, TYPE_SEM_VAL> map_v (scomp_v.GetMap(pos_SEM_MAP_v));
        const TYPE_SEM_VAL & key (index_l.Hd());
        if (map_v.DomExists(key)) {
          const TYPE_SEM_VAL & val (map_v[key]);
          map_v.ImpModify(key, ModifyValue(val, index_l.Tl(), val_v)); 
          return mk_SEM_MAP(map_v);
        }
        else { // Key is't in map.
          if (index_l.Tl().IsEmpty()) {
            map_v.ImpModify(key, val_v); 
            return mk_SEM_MAP(map_v);
          }
          else {
            RTERR::Error(L"ModifyValue", RTERR_INDEXED_ASSIGN, Nil(), Nil(), Sequence());
          }
        }
        return TYPE_SEM_VAL(); // dummy
      } // TAG_TYPE_SEM_MAP
      case TAG_TYPE_SEM_SEQ: {
        if (AUX::IsInt (index_l.Hd())) { // Index is int.
          SEQ<TYPE_SEM_VAL> seq_v (scomp_v.GetSequence(pos_SEM_SEQ_v));
          int64_t index = index_l.Hd().GetReal(pos_SEM_NUM_v).GetIntValue();
          if ((1 <= index) && (index <= seq_v.Length())) {
            const TYPE_SEM_VAL & val (seq_v[index]);
            seq_v.ImpModify(index, ModifyValue(val, index_l.Tl(), val_v));
            return mk_SEM_SEQ(seq_v);
          }
          else {
            RTERR::Error (L"ModifyValue", RTERR_ILLEGAL_INDICES, Nil(), Nil(), Sequence());
          }
        }
        else {
          RTERR::Error (L"ModifyValue", RTERR_NAT_EXPECTED, Nil(), Nil(), Sequence());
        }
        return TYPE_SEM_VAL(); // dummy
      } // TAG_TYPE_SEM_SEQ
      default: {
        RTERR::Error (L"ModifyValue", RTERR_REF_UNKNOWN, scomp_v, Nil(), Sequence());
        return TYPE_SEM_VAL(); // dummy
      }
    }
  }
  return TYPE_SEM_VAL(); // dummy
}

#endif // defined(VDMSL) || defined(VDMPP)

