/***
*  * WHAT
*  *    Free variables lookup in expressions to build closure environments
*  * FILE
*  *    $Source: /mnt/cvsroot/toolbox/code/eval/freevars.cc,v $
*  * VERSION
*  *    $Revision: 1.64 $
*  * DATE
*  *    $Date: 2006/09/22 11:14:04 $
*  * STATUS
*  *    $State: Exp $
*  * PROJECT
*  *    VDM-SL Toolbox
*  * AUTHOR
*  *    Kees de Bruin (KDB) + $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
*  * SPECIFICATION VERSION
*  *    freevars.cc: Implementation of eval_free.vdm 1.19
***/

#include "pattern.h"
#include "freevars.h"
#include "libman.h"
#include "rt_errmsg.h"
#include "asquotes.h"
#include "RTERR.h"
#include "evalaux.h"

SET<TYPE_AS_Name> Free::eset;
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::emap;

// IdentInPattern
// patbind : AS`PatternBind
// ==> set of AS`Name
SET<TYPE_AS_Name> Free::IdentInPattern (const TYPE_AS_PatternBind & patbind)
{
  switch(patbind.GetTag()) {
    case TAG_TYPE_AS_PatternName: {
      const Generic & id (patbind.GetField(pos_AS_PatternName_nm));
      SET<TYPE_AS_Name> res_set;
      if (!id.IsNil ())
        res_set.Insert (id);
      return (res_set);
    }
    case TAG_TYPE_AS_MatchVal: {
//      return SET<TYPE_AS_Name>();
      return eset;
    }
    case TAG_TYPE_AS_SetEnumPattern: {
      const SEQ<TYPE_AS_Pattern> & elems (patbind.GetSequence(pos_AS_SetEnumPattern_Elems));
      SET<TYPE_AS_Name> res_set (eset);
      size_t len_elems = elems.Length();
      for (size_t i = 1; i <= len_elems; i++)
        res_set.ImpUnion (IdentInPattern (elems[i]));
      return (res_set);
    }
    case TAG_TYPE_AS_SetUnionPattern: {
      SET<TYPE_AS_Name> res_set (IdentInPattern (patbind.GetRecord(pos_AS_SetUnionPattern_lp)));
      res_set.ImpUnion (IdentInPattern (patbind.GetRecord(pos_AS_SetUnionPattern_rp)));
      return (res_set);
    }
    case TAG_TYPE_AS_SeqEnumPattern: {
      const SEQ<TYPE_AS_Pattern> & elems (patbind.GetSequence(pos_AS_SeqEnumPattern_els));
      SET<TYPE_AS_Name> res_set (eset);
      size_t len_elems = elems.Length();
      for (size_t i = 1; i <= len_elems; i++ )
        res_set.ImpUnion (IdentInPattern (elems[i]));
      return (res_set);
    }
    case TAG_TYPE_AS_SeqConcPattern: {
      SET<TYPE_AS_Name> res_set (IdentInPattern (patbind.GetRecord(pos_AS_SeqConcPattern_lp)));
      res_set.ImpUnion (IdentInPattern (patbind.GetRecord(pos_AS_SeqConcPattern_rp)));
      return (res_set);
    }
    case TAG_TYPE_AS_MapEnumPattern: {
      const SEQ<TYPE_AS_Pattern> & mls (patbind.GetSequence(pos_AS_MapEnumPattern_mls));
      SET<TYPE_AS_Name> res_set (eset);
      size_t len_mls = mls.Length();
      for (size_t i = 1; i <= len_mls; i++ )
        res_set.ImpUnion (IdentInPattern (mls[i]));
      return (res_set);
    }
    case TAG_TYPE_AS_MapMergePattern: {
      SET<TYPE_AS_Name> res_set (IdentInPattern (patbind.GetRecord(pos_AS_MapMergePattern_lp)));
      res_set.ImpUnion (IdentInPattern (patbind.GetRecord(pos_AS_MapMergePattern_rp)));
      return (res_set);
    }
    case TAG_TYPE_AS_MapletPattern: {
      SET<TYPE_AS_Name> res_set (IdentInPattern (patbind.GetRecord(pos_AS_MapletPattern_dp)));
      res_set.ImpUnion (IdentInPattern (patbind.GetRecord(pos_AS_MapletPattern_rp)));
      return (res_set);
    }
    case TAG_TYPE_AS_RecordPattern: {
      const SEQ<TYPE_AS_Pattern> & fields (patbind.GetSequence(pos_AS_RecordPattern_fields));
      SET<TYPE_AS_Name> res_set (eset);
      size_t len_fields = fields.Length();
      for (size_t i = 1; i <= len_fields; i++ )
        res_set.ImpUnion (IdentInPattern (fields[i]));
      return (res_set);
    }
    case TAG_TYPE_AS_TuplePattern: {
      const SEQ<TYPE_AS_Pattern> & fields (patbind.GetSequence(pos_AS_TuplePattern_fields));
      SET<TYPE_AS_Name> res_set (eset);
      size_t len_fields = fields.Length();
      for (size_t i = 1; i <= len_fields; i++ )
        res_set.ImpUnion (IdentInPattern (fields[i]));
      return (res_set);
    }
#ifdef VDMPP
    case TAG_TYPE_AS_ObjectPattern: {
      const SEQ<TYPE_AS_FieldPattern> & fields (patbind.GetSequence(pos_AS_ObjectPattern_fields));
      SET<TYPE_AS_Name> res_set (eset);
      size_t len_fields = fields.Length();
      for (size_t i = 1; i <= len_fields; i++ )
        res_set.ImpUnion (IdentInPattern (fields[i].GetRecord(pos_AS_FieldPattern_pat)));
      return (res_set);
    }
#endif // VDMPP
    case TAG_TYPE_AS_SetBind: {
      return IdentInPattern(patbind.GetRecord(pos_AS_SetBind_pat));
    }
    case TAG_TYPE_AS_TypeBind: {
      return IdentInPattern(patbind.GetRecord(pos_AS_TypeBind_pat));
    }
    default:
      RTERR::Error (L"IdentInPattern", RTERR_PATTERN_UNKNOWN, Nil(), Nil(),Sequence());
  }
  return eset;
}

// IdentInBind
// bind : AS`Bind
// ==> set of AS`Name
SET<TYPE_AS_Name> Free::IdentInBind (const TYPE_AS_Bind & bind)
{
  switch (bind.GetTag()) {
    case TAG_TYPE_AS_SetBind:  { return IdentInPattern(bind.GetRecord(pos_AS_SetBind_pat)); }
    case TAG_TYPE_AS_TypeBind: { return IdentInPattern(bind.GetRecord(pos_AS_TypeBind_pat)); }
    default: {
      RTERR::Error (L"IdentInBind", RTERR_PATTERN_UNKNOWN, Nil(), Nil(),Sequence());
      return SET<TYPE_AS_Name>(); //dummy
    }
  }
}

// IdentInMultBind
// bind : AS`MultBind
// ==> set of AS`Name
SET<TYPE_AS_Name> Free::IdentInMultBind (const TYPE_AS_MultBind & bind)
{
  switch(bind.GetTag()) {
    case TAG_TYPE_AS_MultSetBind: {
      SET<TYPE_AS_Name> id_s (eset);
      const SEQ<TYPE_AS_Pattern> & pat_l (bind.GetSequence(pos_AS_MultSetBind_pat));
      size_t len = pat_l.Length();
      for (size_t i = 1; i <= len; i++)
        id_s.ImpUnion (IdentInPattern (pat_l[i]));
      return id_s;
    }
    case TAG_TYPE_AS_MultTypeBind: {
      SET<TYPE_AS_Name> id_s (eset);
      const SEQ<TYPE_AS_Pattern> & pat_l (bind.GetSequence(pos_AS_MultTypeBind_pat));
      size_t len = pat_l.Length();
      for (size_t i = 1; i <= len; i++)
        id_s.ImpUnion (IdentInPattern (pat_l[i]));
      return id_s;
    }
    default: {
      RTERR::Error (L"IdentInMultBind", RTERR_PATTERN_UNKNOWN, Nil(), Nil(),Sequence());
      return SET<TYPE_AS_Name>(); //dummy
    }
  }
}

// IdentInMultBindSeq
// bind_l : seq of AS`MultBind
// ==> set of AS`Name
SET<TYPE_AS_Name> Free::IdentInMultBindSeq (const SEQ<TYPE_AS_MultBind> & bind_l)
{
  SET<TYPE_AS_Name> id_s (eset);
  size_t len_bind_l = bind_l.Length();
  for (size_t index = 1; index <= len_bind_l; index++)
    id_s.ImpUnion (IdentInMultBind (bind_l[index]));
  return (id_s);
}

// FreeInBind
// bind : AS`Bind
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInBind (const TYPE_AS_Bind & bind, const SET<TYPE_AS_Name> & id_s)
{
  switch(bind.GetTag()){
    case TAG_TYPE_AS_SetBind:
      return FreeInExpr (bind.GetRecord(pos_AS_SetBind_Set), id_s);
    case TAG_TYPE_AS_TypeBind:
    default:
      return emap;
  }
}

// FreeInMultBindSeq
// bind_l : seq of AS`MultBind
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInMultBindSeq (const SEQ<TYPE_AS_MultBind> & bind_l,
                                                         const SET<TYPE_AS_Name> & id_s)
{
  SET<TYPE_AS_Name> newid_s (id_s);
  MAP<TYPE_AS_Name, TYPE_SEM_VAL> res_m (emap);
  size_t len_bind_l = bind_l.Length();
  for (size_t index = 1; index <= len_bind_l; index++)
  {
    const TYPE_AS_MultBind & bind (bind_l[index]);
    //newid_s.ImpUnion (IdentInMultBind (bind));
    switch(bind.GetTag()) {
      case TAG_TYPE_AS_MultSetBind: {
        const TYPE_AS_Expr & set_e (bind.GetRecord(pos_AS_MultSetBind_Set));
        MAP<TYPE_AS_Name, TYPE_SEM_VAL> tmp_m (FreeInExpr (set_e, newid_s));
        res_m.ImpOverride (tmp_m);
        newid_s.ImpUnion (tmp_m.Dom ());
        break;
      }
      case TAG_TYPE_AS_MultTypeBind: {
        break;
      }
      default:
        break;
    }
    newid_s.ImpUnion (IdentInMultBind (bind));
  }
  return (res_m);
}

// FreeInExpr
// expr : AS`Expr
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInExpr (const TYPE_AS_Expr & expr, const SET<TYPE_AS_Name> & id_s)
{
//wcout << expr.GetTag() << endl;
  switch (expr.GetTag ()) {
    case TAG_TYPE_AS_DefExpr:                  { return FreeInDefExpr (expr, id_s); }
    case TAG_TYPE_AS_LetExpr:                  { return FreeInLetExpr (expr, id_s); }
    case TAG_TYPE_AS_LetBeSTExpr:              { return FreeInLetBeSTExpr (expr, id_s); }
    case TAG_TYPE_AS_IfExpr:                   { return FreeInIfExpr (expr, id_s); }
    case TAG_TYPE_AS_CasesExpr:                { return FreeInCasesExpr (expr, id_s); }
    case TAG_TYPE_AS_PrefixExpr:               { return FreeInPrefixExpr (expr, id_s); }
    case TAG_TYPE_AS_BinaryExpr:               { return FreeInBinaryExpr (expr, id_s); }
    case TAG_TYPE_AS_AllOrExistsExpr:          { return FreeInAllOrExistsExpr (expr, id_s); }
    case TAG_TYPE_AS_ExistsUniqueExpr:         { return FreeInExistsUniqueExpr (expr, id_s); }
    case TAG_TYPE_AS_IotaExpr:                 { return FreeInIotaExpr (expr, id_s); }
    case TAG_TYPE_AS_SetEnumerationExpr:       { return FreeInSetEnumerationExpr (expr, id_s); }
    case TAG_TYPE_AS_SetComprehensionExpr:     { return FreeInSetComprehensionExpr (expr, id_s); }
    case TAG_TYPE_AS_SetRangeExpr:             { return FreeInSetRangeExpr (expr, id_s); }
    case TAG_TYPE_AS_SeqEnumerationExpr:       { return FreeInSeqEnumerationExpr (expr, id_s); }
    case TAG_TYPE_AS_SeqComprehensionExpr:     { return FreeInSeqComprehensionExpr (expr, id_s); }
    case TAG_TYPE_AS_SubSequenceExpr:          { return FreeInSubSequenceExpr (expr, id_s); }
    case TAG_TYPE_AS_SeqModifyMapOverrideExpr: { return FreeInSeqModifyMapOverrideExpr (expr, id_s); }
    case TAG_TYPE_AS_MapEnumerationExpr:       { return FreeInMapEnumerationExpr (expr, id_s); }
    case TAG_TYPE_AS_MapComprehensionExpr:     { return FreeInMapComprehensionExpr (expr, id_s); }
    case TAG_TYPE_AS_TupleConstructorExpr:     { return FreeInTupleConstructorExpr (expr, id_s); }
    case TAG_TYPE_AS_RecordConstructorExpr:    { return FreeInRecordConstructorExpr (expr, id_s); }
    case TAG_TYPE_AS_RecordModifierExpr:       { return FreeInRecordModifierExpr (expr, id_s); }
    case TAG_TYPE_AS_TokenConstructorExpr:     { return FreeInTokenConstructorExpr(expr, id_s); }
    case TAG_TYPE_AS_TupleSelectExpr:          { return FreeInTupleSelectExpr(expr, id_s); }
    case TAG_TYPE_AS_TypeJudgementExpr:        { return FreeInTypeJudgementExpr(expr, id_s); }
    case TAG_TYPE_AS_PreConditionApplyExpr:    { return FreeInPreConditionApplyExpr(expr, id_s); }
    case TAG_TYPE_AS_ApplyExpr:                { return FreeInApplyExpr (expr, id_s); }
    case TAG_TYPE_AS_LambdaExpr:               { return FreeInLambdaExpr (expr, id_s); }
    case TAG_TYPE_AS_FieldSelectExpr:          { return FreeInFieldSelectExpr (expr, id_s); }
    case TAG_TYPE_AS_FctTypeInstExpr:          { return FreeInFctTypeInstExpr (expr, id_s); }
    case TAG_TYPE_AS_IsExpr:                   { return FreeInIsExpr (expr, id_s); }
    case TAG_TYPE_AS_NarrowExpr:               { return FreeInNarrowExpr (expr, id_s); }
    case TAG_TYPE_AS_BoolLit:
    case TAG_TYPE_AS_CharLit:
    case TAG_TYPE_AS_TextLit:
    case TAG_TYPE_AS_QuoteLit:
    case TAG_TYPE_AS_RealLit:
    case TAG_TYPE_AS_NilLit:                   { return emap; }
    case TAG_TYPE_AS_Name:                     { return FreeInName (expr, id_s); }
#ifdef VDMSL
    case TAG_TYPE_AS_OldName:                  { return FreeInOldName (expr, id_s); }
#endif //VDMSL
    case TAG_TYPE_AS_UndefinedExpr:            { return emap; }
    case TAG_TYPE_AS_BracketedExpr:            { return FreeInBracketedExpr (expr, id_s); }
    default: { return emap; }
  }
}

// FreeInDefExpr
// DefE : AS`DefExpr
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInDefExpr (const TYPE_AS_DefExpr & DefE, const SET<TYPE_AS_Name> & id_s)
{
  type_dd2PL def_l (DefE.GetSequence(pos_AS_DefExpr_Def));  // seq of (PatternBind * Expr)
  const TYPE_AS_Expr & in_e (DefE.GetRecord(pos_AS_DefExpr_In));

  MAP<TYPE_AS_Name, TYPE_SEM_VAL> res_m (emap);
  SET<TYPE_AS_Name> newid_s (id_s);
  size_t len_def_l = def_l.Length();
  for (size_t index = 1; index <= len_def_l; index++)
  {
    const Tuple & t (def_l[index]); // (PatternBind * Expr)
    const TYPE_AS_PatternBind & pat_p (t.GetRecord (1));
    const TYPE_AS_Expr & val_e (t.GetRecord (2));

    res_m.ImpOverride (FreeInExpr (val_e, id_s));
    newid_s.ImpUnion (IdentInPattern (pat_p));
  }
  newid_s.ImpUnion (res_m.Dom ());
  res_m.ImpOverride (FreeInExpr (in_e, newid_s));
  return (res_m);
}

// FreeInLetExpr
// LetE : AS`LetExpr
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInLetExpr (const TYPE_AS_LetExpr & LetE, const SET<TYPE_AS_Name> & id_s)
{
  const SEQ<TYPE_AS_LocalDef> & localdef (LetE.GetSequence(pos_AS_LetExpr_localdef)); // seq of LocalDef
  const TYPE_AS_Expr & in_e (LetE.GetRecord(pos_AS_LetExpr_body));

  SET<TYPE_AS_Name> newid_s (id_s);
  size_t len_localdef = localdef.Length();
  for (size_t index = 1; index <= len_localdef; index++)
  {
    const TYPE_AS_LocalDef & ldef (localdef[index]);
    switch(ldef.GetTag()) {
      case TAG_TYPE_AS_ValueDef: {
        newid_s.ImpUnion (IdentInPattern (ldef.GetRecord(pos_AS_ValueDef_pat)));
        break;
      }
      case TAG_TYPE_AS_ExplFnDef: {
        newid_s.Insert (ldef.GetRecord(pos_AS_ExplFnDef_nm));
        break;
      }
      case TAG_TYPE_AS_ImplFnDef: {
        newid_s.Insert (ldef.GetRecord(pos_AS_ImplFnDef_nm));
        break;
      }
      case TAG_TYPE_AS_ExtExplFnDef: {
        newid_s.Insert (ldef.GetRecord(pos_AS_ExtExplFnDef_nm));
        break;
      }
      default:
        break;
    }
  }

  MAP<TYPE_AS_Name, TYPE_SEM_VAL> res_m (emap);
  for (size_t index2 = 1; index2 <= len_localdef; index2++)
  {
    const TYPE_AS_LocalDef & ldef (localdef[index2]);
    switch(ldef.GetTag()) {
      case TAG_TYPE_AS_ValueDef: {
        const TYPE_AS_Expr & val (ldef.GetRecord(pos_AS_ValueDef_val));
        MAP<TYPE_AS_Name, TYPE_SEM_VAL> tmp_m (FreeInExpr (val, newid_s));
        res_m.ImpOverride (tmp_m);
        newid_s.ImpUnion (tmp_m.Dom ());
        break;
      }
      case TAG_TYPE_AS_ExplFnDef:
      case TAG_TYPE_AS_ImplFnDef:
      case TAG_TYPE_AS_ExtExplFnDef: {
        MAP<TYPE_AS_Name, TYPE_SEM_VAL> tmp_m (FreeInFnDef (ldef, newid_s));
        res_m.ImpOverride (tmp_m);
        newid_s.ImpUnion (tmp_m.Dom ());
        break;
      }
      default:
        break;
    }
  }
  res_m.ImpOverride (FreeInExpr (in_e, newid_s));
  return (res_m);
}

// FreeInFnDef
// fn : AS`FnDef
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInFnDef (const TYPE_AS_FnDef & fn, const SET<TYPE_AS_Name> & id_s)
{
  SET<TYPE_AS_Name> newid_s (id_s);
  switch(fn.GetTag()) {
    case TAG_TYPE_AS_ExplFnDef: {
      const SEQ<TYPE_AS_Parameters> & parms (fn.GetSequence(pos_AS_ExplFnDef_parms));
      const TYPE_AS_FnBody & body (fn.GetRecord(pos_AS_ExplFnDef_body));
      const Generic & pre_e (fn.GetField(pos_AS_ExplFnDef_fnpre));
      const Generic & post_e (fn.GetField(pos_AS_ExplFnDef_fnpost));

      size_t len_parms = parms.Length();
      for (size_t i = 1; i <= len_parms; i++)
      {
        SEQ<TYPE_AS_Pattern> pat_l (parms[i]);
        size_t len_pat_l = pat_l.Length();
        for (size_t j = 1; j <= len_pat_l; j++)
          newid_s.ImpUnion (IdentInPattern (pat_l[j]));
      }
      MAP<TYPE_AS_Name, TYPE_SEM_VAL> res_m (emap);
      if ( (body.GetField(pos_AS_FnBody_body)).IsRecord() )
        res_m = FreeInExpr (body.GetRecord(pos_AS_FnBody_body), newid_s);
      newid_s.ImpUnion (res_m.Dom ());
      MAP<TYPE_AS_Name, TYPE_SEM_VAL> tmp_m (FreeInPreExpr (pre_e, newid_s));
      res_m.ImpOverride (tmp_m);
      newid_s.ImpUnion (tmp_m.Dom ());
      res_m.ImpOverride (FreeInPostExpr (post_e, newid_s));
      return (res_m);
    }
    case TAG_TYPE_AS_ImplFnDef: {
      const SEQ<TYPE_AS_PatTypePair> & partps (fn.GetSequence(pos_AS_ImplFnDef_partps));
      const Generic & pre_e (fn.GetField(pos_AS_ImplFnDef_fnpre));
      const TYPE_AS_Expr & post_e (fn.GetRecord(pos_AS_ImplFnDef_fnpost));

      size_t len_partps = partps.Length();
      for (size_t i = 1; i <= len_partps; i++)
      {
        TYPE_AS_PatTypePair ptp (partps[i]);
        const SEQ<TYPE_AS_Pattern> & pat_l (ptp.GetSequence(pos_AS_PatTypePair_pats));
        size_t len_pat_l = pat_l.Length();
        for (size_t j = 1; j <= len_pat_l; j++)
          newid_s.ImpUnion (IdentInPattern (pat_l[j]));
      }
      MAP<TYPE_AS_Name, TYPE_SEM_VAL> res_m (FreeInPreExpr (pre_e, newid_s));
      newid_s.ImpUnion (res_m.Dom ());
      res_m.ImpOverride (FreeInPostExpr (post_e, newid_s));
      return (res_m);
    }
    case TAG_TYPE_AS_ExtExplFnDef: {
      const SEQ<TYPE_AS_PatTypePair> & partps (fn.GetSequence(pos_AS_ExtExplFnDef_partps));
      const TYPE_AS_FnBody & body (fn.GetRecord(pos_AS_ExtExplFnDef_body));
      const Generic & pre_e (fn.GetField(pos_AS_ExtExplFnDef_fnpre));
      const Generic & post_e (fn.GetField(pos_AS_ExtExplFnDef_fnpost));

      size_t len_partps = partps.Length();
      for (size_t i = 1; i <= len_partps; i++)
      {
        TYPE_AS_PatTypePair ptp (partps[i]);
        const SEQ<TYPE_AS_Pattern> & pat_l (ptp.GetSequence(pos_AS_PatTypePair_pats));
        size_t len_pat_l = pat_l.Length();
        for (size_t j = 1; j <= len_pat_l; j++)
          newid_s.ImpUnion (IdentInPattern (pat_l[j]));
      }
      MAP<TYPE_AS_Name, TYPE_SEM_VAL> res_m (emap);
      if ( (body.GetField(pos_AS_FnBody_body)).IsRecord() )
        res_m = FreeInExpr (body.GetRecord(pos_AS_FnBody_body), newid_s);
      res_m.ImpOverride (FreeInPreExpr (pre_e, newid_s));
      newid_s.ImpUnion (res_m.Dom ());
      res_m.ImpOverride (FreeInPostExpr (post_e, newid_s));
      return (res_m);
    }
    default:
      return emap;
  }
}

// FreeInPreExpr
// PreE : [AS`Expr]
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInPreExpr (const Generic & PreE, const SET<TYPE_AS_Name> & id_s)
{
  if (PreE.IsNil ())
//    return MAP<TYPE_AS_Name, TYPE_SEM_VAL> ();
    return emap;
  else
    return (FreeInExpr (PreE, id_s));
}

// FreeInPostExpr
// PostE : [AS`Expr]
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInPostExpr (const Generic & PostE, const SET<TYPE_AS_Name> & id_s)
{
  if (PostE.IsNil ())
//    return MAP<TYPE_AS_Name, TYPE_SEM_VAL> ();
    return emap;
  else
    return (FreeInExpr (PostE, id_s));
}

// FreeInLetBeSTExpr
// LetBE : AS`LetBeSTExpr
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInLetBeSTExpr (const TYPE_AS_LetBeSTExpr & LetBE,
                                                         const SET<TYPE_AS_Name> & id_s)
{
  const SEQ<TYPE_AS_MultBind> & lhs (LetBE.GetSequence(pos_AS_LetBeSTExpr_lhs));
  const Generic & st_e (LetBE.GetField(pos_AS_LetBeSTExpr_St));
  const TYPE_AS_Expr & in_e (LetBE.GetRecord(pos_AS_LetBeSTExpr_In));

  SET<TYPE_AS_Name> newid_s (id_s);

  MAP<TYPE_AS_Name, TYPE_SEM_VAL> res_m (FreeInMultBindSeq (lhs, id_s));
  newid_s.ImpUnion (res_m.Dom ());
  newid_s.ImpUnion (IdentInMultBindSeq (lhs));

  if (!st_e.IsNil())
  {
    MAP<TYPE_AS_Name, TYPE_SEM_VAL> tmp_m (FreeInExpr (st_e, newid_s));
    res_m.ImpOverride (tmp_m);
    newid_s.ImpUnion (tmp_m.Dom ());
  }
  res_m.ImpOverride (FreeInExpr (in_e, newid_s));
  return (res_m);
}

// FreeInIfExpr
// IfE : AS`IfExpr
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInIfExpr (const TYPE_AS_IfExpr & IfE,
                                                    const SET<TYPE_AS_Name> & id_s)
{
  const TYPE_AS_Expr & test_e (IfE.GetRecord(pos_AS_IfExpr_test));
  const TYPE_AS_Expr & cons_e (IfE.GetRecord(pos_AS_IfExpr_cons));
  const TYPE_AS_Expr & altn_e (IfE.GetRecord(pos_AS_IfExpr_altn));
  const SEQ<TYPE_AS_ElseifExpr> & elif_l (IfE.GetSequence(pos_AS_IfExpr_elsif));

  SET<TYPE_AS_Name> newid_s (id_s);
  MAP<TYPE_AS_Name, TYPE_SEM_VAL> res_m (FreeInExpr (test_e, newid_s));
  newid_s.ImpUnion (res_m.Dom ());
  MAP<TYPE_AS_Name, TYPE_SEM_VAL> tmp_m (FreeInExpr (cons_e, newid_s));
  res_m.ImpOverride (tmp_m);
  newid_s.ImpUnion (tmp_m.Dom ());
  tmp_m = FreeInElseifExpr (elif_l, newid_s);
  res_m.ImpOverride (tmp_m);
  newid_s.ImpUnion (tmp_m.Dom ());
  res_m.ImpOverride (FreeInExpr (altn_e, newid_s));
  return (res_m);
}

// FreeInElseifExpr
// elif_l : seq of AS`ElseifExpr
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInElseifExpr (
                                  const SEQ<TYPE_AS_ElseifExpr> & elif_l,
                                  const SET<TYPE_AS_Name> & id_s)
{
  SET<TYPE_AS_Name> newid_s (id_s);
  MAP<TYPE_AS_Name, TYPE_SEM_VAL> res_m (emap);
  size_t len_elif_l = elif_l.Length();
  for (size_t i = 1; i <= len_elif_l; i++)
  {
    TYPE_AS_ElseifExpr eif (elif_l[i]);
    const TYPE_AS_Expr & test_e (eif.GetRecord(pos_AS_ElseifExpr_test));
    const TYPE_AS_Expr & cons_e (eif.GetRecord(pos_AS_ElseifExpr_cons));

    MAP<TYPE_AS_Name, TYPE_SEM_VAL> test_m (FreeInExpr (test_e, newid_s));
    res_m.ImpOverride (test_m);
    newid_s.ImpUnion (test_m.Dom ());

    MAP<TYPE_AS_Name, TYPE_SEM_VAL> cons_m (FreeInExpr (cons_e, newid_s));
    res_m.ImpOverride (cons_m);
    newid_s.ImpUnion (cons_m.Dom ());
  }
  return (res_m);
}

// FreeInCasesExpr
// CasesE : AS`CasesExpr
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInCasesExpr (const TYPE_AS_CasesExpr & CasesE,
                                                       const SET<TYPE_AS_Name> & id_s)
{
  const TYPE_AS_Expr & sel_e (CasesE.GetRecord(pos_AS_CasesExpr_sel));
  const SEQ<TYPE_AS_CaseAltn> & altns_l (CasesE.GetSequence(pos_AS_CasesExpr_altns));
  const Generic & others_e (CasesE.GetField(pos_AS_CasesExpr_Others));

  SET<TYPE_AS_Name> newid_s (id_s);
  MAP<TYPE_AS_Name, TYPE_SEM_VAL> res_m (FreeInExpr (sel_e, newid_s));
  newid_s.ImpUnion (res_m.Dom ());

  size_t len_altns_l = altns_l.Length();
  for (size_t i = 1; i <= len_altns_l; i++)
  {
    TYPE_AS_CaseAltn ca (altns_l[i]);
    const SEQ<TYPE_AS_Pattern> & match_lp (ca.GetSequence(pos_AS_CaseAltn_match));
    const TYPE_AS_Expr & body_e (ca.GetRecord(pos_AS_CaseAltn_body));
    size_t len_match_lp = match_lp.Length();
    for (size_t j = 1; j <= len_match_lp; j++)
      newid_s.ImpUnion (IdentInPattern (match_lp[j]));
    MAP<TYPE_AS_Name, TYPE_SEM_VAL> tmp_m (FreeInExpr (body_e, newid_s));
    res_m.ImpOverride (tmp_m);
    newid_s.ImpUnion (tmp_m.Dom ());
  }
  if (!others_e.IsNil())
    res_m.ImpOverride (FreeInExpr (others_e, newid_s));
  return (res_m);
}

// FreeInPrefixExpr
// PrefE : AS`PrefixExpr
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInPrefixExpr (const TYPE_AS_PrefixExpr & PrefE,
                                                        const SET<TYPE_AS_Name> & id_s)
{
  return (FreeInExpr (PrefE.GetRecord(pos_AS_PrefixExpr_arg), id_s));
}

// FreeInBinaryExpr
// BinE : AS`BinaryExpr
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInBinaryExpr (const TYPE_AS_BinaryExpr & BinE,
                                                        const SET<TYPE_AS_Name> & id_s)
{
  SET<TYPE_AS_Name> newid_s (id_s);
  MAP<TYPE_AS_Name, TYPE_SEM_VAL> res_m (FreeInExpr (BinE.GetRecord(pos_AS_BinaryExpr_left), newid_s));
  newid_s.ImpUnion (res_m.Dom ());
  res_m.ImpOverride (FreeInExpr (BinE.GetRecord(pos_AS_BinaryExpr_right), newid_s));
  return (res_m);
}

// FreeInAllOrExistsExpr
// AllE : AS`AllOrExistsExpr
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInAllOrExistsExpr (const TYPE_AS_AllOrExistsExpr & AllE,
                                                             const SET<TYPE_AS_Name> & id_s)
{
  const SEQ<TYPE_AS_MultBind> & bind_l (AllE.GetSequence(pos_AS_AllOrExistsExpr_bind));
  const TYPE_AS_Expr & pred_e (AllE.GetRecord(pos_AS_AllOrExistsExpr_pred));

  SET<TYPE_AS_Name> newid_s (id_s);
  MAP<TYPE_AS_Name, TYPE_SEM_VAL> res_m (FreeInMultBindSeq (bind_l, newid_s));
  newid_s.ImpUnion (res_m.Dom ());
  newid_s.ImpUnion (IdentInMultBindSeq (bind_l));
  res_m.ImpOverride (FreeInExpr (pred_e, newid_s));
  return (res_m);
}

// FreeInExistsUniqueExpr
// ExistsE : AS`ExistsUniqueExpr
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInExistsUniqueExpr (const TYPE_AS_ExistsUniqueExpr & ExistsE,
                                                              const SET<TYPE_AS_Name> & id_s)
{
  const TYPE_AS_Bind & bind (ExistsE.GetRecord(pos_AS_ExistsUniqueExpr_bind));
  const TYPE_AS_Expr & pred_e (ExistsE.GetRecord(pos_AS_ExistsUniqueExpr_pred));

  MAP<TYPE_AS_Name, TYPE_SEM_VAL> res_m (FreeInBind (bind, id_s));
  SET<TYPE_AS_Name> newid_s (id_s);
  newid_s.ImpUnion (IdentInBind (bind));
  newid_s.ImpUnion (res_m.Dom ());
  res_m.ImpOverride (FreeInExpr (pred_e, newid_s));
  return (res_m);
}

// FreeInSetEnumerationExpr
// EnumE : AS`SetEnumerationExpr
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInSetEnumerationExpr (const TYPE_AS_SetEnumerationExpr & EnumE,
                                                                const SET<TYPE_AS_Name> & id_s)
{
  const SEQ<TYPE_AS_Expr> & elm_le (EnumE.GetSequence(pos_AS_SetEnumerationExpr_els));

  MAP<TYPE_AS_Name, TYPE_SEM_VAL> res_m (emap);
  SET<TYPE_AS_Name> newid_s (id_s);
  size_t len_elm_le = elm_le.Length();
  for (size_t i = 1; i <= len_elm_le; i++)
  {
    MAP<TYPE_AS_Name, TYPE_SEM_VAL> tmp_m (FreeInExpr (elm_le[i], newid_s));
    res_m.ImpOverride (tmp_m);
    newid_s.ImpUnion (tmp_m.Dom ());
  }
  return (res_m);
}

// FreeInSetComprehensionExpr
// CompE : AS`SetComprehensionExpr
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInSetComprehensionExpr (const TYPE_AS_SetComprehensionExpr & CompE,
                                                                  const SET<TYPE_AS_Name> & id_s)
{
  const TYPE_AS_Expr & elem_e (CompE.GetRecord(pos_AS_SetComprehensionExpr_elem));
  const SEQ<TYPE_AS_MultBind> & bind_l (CompE.GetSequence(pos_AS_SetComprehensionExpr_bind));
  const Generic & pred_e (CompE.GetField(pos_AS_SetComprehensionExpr_pred));

  MAP<TYPE_AS_Name, TYPE_SEM_VAL> res_m (FreeInMultBindSeq (bind_l, id_s));
  SET<TYPE_AS_Name> newid_s (id_s);
  newid_s.ImpUnion (res_m.Dom ());
  newid_s.ImpUnion (IdentInMultBindSeq (bind_l));
  MAP<TYPE_AS_Name, TYPE_SEM_VAL> tmp_m (FreeInExpr (elem_e, newid_s));
  res_m.ImpOverride (tmp_m);
  newid_s.ImpUnion (tmp_m.Dom ());
  if (!pred_e.IsNil())
    res_m.ImpOverride (FreeInExpr (pred_e, newid_s));
  return (res_m);
}

// FreeInSetRangeExpr
// RangeE : AS`SetRangeExpr
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL

MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInSetRangeExpr (const TYPE_AS_SetRangeExpr & RangeE,
                                                          const SET<TYPE_AS_Name> & id_s)
{
  MAP<TYPE_AS_Name, TYPE_SEM_VAL> res_m (FreeInExpr (RangeE.GetRecord(pos_AS_SetRangeExpr_lb), id_s));
  SET<TYPE_AS_Name> newid_s (id_s);
  newid_s.ImpUnion (res_m.Dom ());
  res_m.ImpOverride (FreeInExpr (RangeE.GetRecord(pos_AS_SetRangeExpr_ub), newid_s));
  return (res_m);
}

// FreeInSeqEnumerationExpr
// EnumE : AS`SeqEnumerationExpr
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInSeqEnumerationExpr (const TYPE_AS_SeqEnumerationExpr & EnumE,
                                                                const SET<TYPE_AS_Name> & id_s)
{
  const SEQ<TYPE_AS_Expr> & els_l (EnumE.GetSequence(pos_AS_SeqEnumerationExpr_els));

  MAP<TYPE_AS_Name, TYPE_SEM_VAL> res_m (emap);
  SET<TYPE_AS_Name> newid_s (id_s);
  size_t len_els_l = els_l.Length();
  for (size_t i = 1; i <= len_els_l; i++)
  {
    MAP<TYPE_AS_Name, TYPE_SEM_VAL> tmp_m (FreeInExpr (els_l[i], newid_s));
    res_m.ImpOverride (tmp_m);
    newid_s.ImpUnion (tmp_m.Dom ());
  }
  return (res_m);
}

// FreeInSeqComprehensionExpr
// CompE : AS`SeqComprehensionExpr
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInSeqComprehensionExpr (const TYPE_AS_SeqComprehensionExpr & CompE,
                                                                  const SET<TYPE_AS_Name> & id_s)
{
  const TYPE_AS_Expr & elem_e  (CompE.GetRecord(pos_AS_SeqComprehensionExpr_elem));
  const TYPE_AS_SetBind & bind (CompE.GetRecord(pos_AS_SeqComprehensionExpr_bind));
  const Generic & pred_e       (CompE.GetField(pos_AS_SeqComprehensionExpr_pred));

  //MAP<TYPE_AS_Name, TYPE_SEM_VAL> res_m (FreeInBind (bind, id_s));
  MAP<TYPE_AS_Name, TYPE_SEM_VAL> res_m;
  switch (bind.GetTag()) {
    case TAG_TYPE_AS_SetBind: {
      res_m = FreeInExpr (bind.GetRecord(pos_AS_SetBind_Set), id_s);
      break;
    }
    case TAG_TYPE_AS_SeqBind: {
      res_m = FreeInExpr (bind.GetRecord(pos_AS_SeqBind_Seq), id_s);
      break;
    }
  }
  SET<TYPE_AS_Name> newid_s (id_s);
  newid_s.ImpUnion (res_m.Dom ());
  newid_s.ImpUnion (IdentInBind (bind));
  MAP<TYPE_AS_Name, TYPE_SEM_VAL> tmp_m (FreeInExpr (elem_e, newid_s));
  res_m.ImpOverride (tmp_m);
  newid_s.ImpUnion (tmp_m.Dom ());
  if (!pred_e.IsNil())
    res_m.ImpOverride (FreeInExpr (pred_e, newid_s));
  return (res_m);
}

// FreeInSubSequenceExpr
// SubE : AS`SubSequenceExpr
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInSubSequenceExpr (const TYPE_AS_SubSequenceExpr & SubE,
                                                             const SET<TYPE_AS_Name> & id_s)
{
  const TYPE_AS_Expr & seq_e  (SubE.GetRecord(pos_AS_SubSequenceExpr_sequence));
  const TYPE_AS_Expr & from_e (SubE.GetRecord(pos_AS_SubSequenceExpr_frompos));
  const TYPE_AS_Expr & to_e   (SubE.GetRecord(pos_AS_SubSequenceExpr_topos));

  MAP<TYPE_AS_Name, TYPE_SEM_VAL> res_m (FreeInExpr (seq_e, id_s));
  SET<TYPE_AS_Name> newid_s (id_s);
  newid_s.ImpUnion (res_m.Dom ());
  MAP<TYPE_AS_Name, TYPE_SEM_VAL> tmp_m (FreeInExpr (from_e, newid_s));
  res_m.ImpOverride (tmp_m);
  newid_s.ImpUnion (tmp_m.Dom ());
  res_m.ImpOverride (FreeInExpr (to_e, newid_s));
  return (res_m);
}

// FreeInSeqModifyMapOverrideExpr
// ModE : AS`SeqModifyMapOverrideExpr
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInSeqModifyMapOverrideExpr (const TYPE_AS_SeqModifyMapOverrideExpr & ModE,
                                                                      const SET<TYPE_AS_Name> & id_s)
{
  const TYPE_AS_Expr & seqmap_e (ModE.GetRecord(pos_AS_SeqModifyMapOverrideExpr_seqmap));
  const TYPE_AS_Expr & map_e    (ModE.GetRecord(pos_AS_SeqModifyMapOverrideExpr_mapexp));

  MAP<TYPE_AS_Name, TYPE_SEM_VAL> tmp_m (FreeInExpr (seqmap_e, id_s));
  SET<TYPE_AS_Name> newid_s (id_s);
  newid_s.ImpUnion (tmp_m.Dom ());
  MAP<TYPE_AS_Name, TYPE_SEM_VAL> res_m (FreeInExpr (map_e, newid_s));
  res_m.ImpOverride (tmp_m);
  return (res_m);
}

// FreeInMapEnumerationExpr
// EnumE : AS`MapEnumerationExpr
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInMapEnumerationExpr (const TYPE_AS_MapEnumerationExpr & EnumE,
                                                                const SET<TYPE_AS_Name> & id_s)
{
  const SEQ<TYPE_AS_Maplet> & els_l (EnumE.GetSequence(pos_AS_MapEnumerationExpr_els));

  SET<TYPE_AS_Name> newid_s (id_s);
  MAP<TYPE_AS_Name, TYPE_SEM_VAL> res_m (emap);
  size_t len_els_l = els_l.Length();
  for (size_t i = 1; i <= len_els_l; i++)
  {
    TYPE_AS_Maplet m (els_l[i]);
    const TYPE_AS_Expr & dom_e (m.GetRecord(pos_AS_Maplet_mapdom));
    const TYPE_AS_Expr & rng_e (m.GetRecord(pos_AS_Maplet_maprng));
    MAP<TYPE_AS_Name, TYPE_SEM_VAL> tmp_m (FreeInExpr (dom_e, newid_s));
    newid_s.ImpUnion (tmp_m.Dom ());
    res_m.ImpOverride (tmp_m);
    tmp_m = FreeInExpr (rng_e, newid_s);
    newid_s.ImpUnion (tmp_m.Dom ());
    res_m.ImpOverride (tmp_m);
  }
  return (res_m);
}

// FreeInMapComprehensionExpr
// CompE : AS`MapComprehensionExpr
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInMapComprehensionExpr (const TYPE_AS_MapComprehensionExpr & CompE,
                                                                  const SET<TYPE_AS_Name> & id_s)
{
  const TYPE_AS_Maplet & elem_e        (CompE.GetRecord(pos_AS_MapComprehensionExpr_elem));
  const SEQ<TYPE_AS_MultBind> & bind_l (CompE.GetSequence(pos_AS_MapComprehensionExpr_bind));
  const Generic & pred_e               (CompE.GetField(pos_AS_MapComprehensionExpr_pred));

  MAP<TYPE_AS_Name, TYPE_SEM_VAL> res_m (FreeInMultBindSeq (bind_l, id_s));
  SET<TYPE_AS_Name> newid_s (id_s);
  newid_s.ImpUnion (res_m.Dom ());
  newid_s.ImpUnion (IdentInMultBindSeq (bind_l));
  MAP<TYPE_AS_Name, TYPE_SEM_VAL> tmp_m (FreeInExpr (elem_e.GetRecord(pos_AS_Maplet_mapdom), newid_s));
  newid_s.ImpUnion (tmp_m.Dom ());
  res_m.ImpOverride (tmp_m);
  tmp_m = FreeInExpr (elem_e.GetRecord(pos_AS_Maplet_maprng), newid_s);
  newid_s.ImpUnion (tmp_m.Dom ());
  res_m.ImpOverride (tmp_m);
  if (!pred_e.IsNil())
    res_m.ImpOverride (FreeInExpr (pred_e, newid_s));
  return (res_m);
}

// FreeInTupleConstructorExpr
// TupleE : AS`TupleConstructorExpr
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInTupleConstructorExpr (const TYPE_AS_TupleConstructorExpr & TupleE,
                                                                  const SET<TYPE_AS_Name> & id_s)
{
  const SEQ<TYPE_AS_Expr> & fields_le (TupleE.GetSequence(pos_AS_TupleConstructorExpr_fields));

  MAP<TYPE_AS_Name, TYPE_SEM_VAL> res_m (emap);
  SET<TYPE_AS_Name> newid_s (id_s);
  size_t len_fields_le = fields_le.Length();
  for (size_t index = 1; index <= len_fields_le; index++)
  {
    MAP<TYPE_AS_Name, TYPE_SEM_VAL> tmp_m (FreeInExpr (fields_le[index], newid_s));
    res_m.ImpOverride (tmp_m);
    newid_s.ImpUnion (tmp_m.Dom ());
  }
  return (res_m);
}

// FreeInTupleSelectExpr
// tupleS : AS`TupleSelectExpr
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInTupleSelectExpr(const TYPE_AS_TupleSelectExpr & tupleS,
                                                            const SET<TYPE_AS_Name> & id_s)
{
  return FreeInExpr(tupleS.GetRecord(pos_AS_TupleSelectExpr_tuple), id_s);
}

// FreeInRecordConstructorExpr
// RecE : AS`RecordConstructorExpr
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInRecordConstructorExpr (const TYPE_AS_RecordConstructorExpr & RecE,
                                                                   const SET<TYPE_AS_Name> & id_s)
{
  const SEQ<TYPE_AS_Expr> & fields_le (RecE.GetSequence(pos_AS_RecordConstructorExpr_fields));

  MAP<TYPE_AS_Name, TYPE_SEM_VAL> res_m (emap);
  SET<TYPE_AS_Name> newid_s (id_s);
  size_t len_fields_le = fields_le.Length();
  for (size_t i = 1; i <= len_fields_le; i++)
  {
    MAP<TYPE_AS_Name, TYPE_SEM_VAL> tmp_m (FreeInExpr (fields_le[i], newid_s));
    res_m.ImpOverride (tmp_m);
    newid_s.ImpUnion (tmp_m.Dom ());
  }
  return (res_m);
}

// FreeInTokenConstructorExpr
// tokenE : AS`TokenConstructorExpr
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInTokenConstructorExpr(const TYPE_AS_TokenConstructorExpr & tokenE,
                                                                 const SET<TYPE_AS_Name> & id_s)
{
  return FreeInExpr(tokenE.GetRecord(pos_AS_TokenConstructorExpr_field), id_s);
}

// FreeInRecordModifierExpr
// ModE : AS`RecordModifierExpr
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInRecordModifierExpr (const TYPE_AS_RecordModifierExpr & ModE,
                                                                const SET<TYPE_AS_Name> & id_s)
{
  const TYPE_AS_Expr & rec_e (ModE.GetRecord(pos_AS_RecordModifierExpr_rec));
  const SEQ<TYPE_AS_RecordModification> & modifiers (ModE.GetSequence(pos_AS_RecordModifierExpr_modifiers));

  MAP<TYPE_AS_Name, TYPE_SEM_VAL> res_m (FreeInExpr (rec_e, id_s));
  SET<TYPE_AS_Name> newid_s (id_s);
  newid_s.ImpUnion (res_m.Dom ());

  size_t len_modifiers = modifiers.Length();
  for (size_t i =1; i <= len_modifiers; i++)
  {
    const TYPE_AS_RecordModification & rm (modifiers[i]);
    const TYPE_AS_Expr & expr (rm.GetRecord(pos_AS_RecordModification_newexpr));
    MAP<TYPE_AS_Name, TYPE_SEM_VAL> tmp_m (FreeInExpr (expr, newid_s));
    res_m.ImpOverride (tmp_m);
    newid_s.ImpUnion (tmp_m.Dom ());
  }
  return (res_m);
}

// FreeInApplyExpr
// ApplyE : AS`ApplyExpr
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInApplyExpr (const TYPE_AS_ApplyExpr & ApplyE,
                                                       const SET<TYPE_AS_Name> & id_s)
{
  const TYPE_AS_Expr & fn_e (ApplyE.GetRecord(pos_AS_ApplyExpr_fct));
  const SEQ<TYPE_AS_Expr> & arg_le (ApplyE.GetSequence(pos_AS_ApplyExpr_arg));

  MAP<TYPE_AS_Name, TYPE_SEM_VAL> res_m (FreeInExpr (fn_e, id_s));
  SET<TYPE_AS_Name> newid_s (id_s);
  newid_s.ImpUnion (res_m.Dom ());
  size_t len_arg_le = arg_le.Length();
  for (size_t i = 1; i <= len_arg_le; i++)
  {
    Map arg_m (FreeInExpr (arg_le[i], newid_s));
    res_m.ImpOverride (arg_m);
    newid_s.ImpUnion (arg_m.Dom ());
  }
  return (res_m);
}

// FreeInFctTypeInstExpr
// FctE : AS`FctTypeInstExpr
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInFctTypeInstExpr (const TYPE_AS_FctTypeInstExpr & FctE,
                                                             const SET<TYPE_AS_Name> & id_s)
{
  return (FreeInExpr (FctE.GetRecord(pos_AS_FctTypeInstExpr_polyfct), id_s));
}

// FreeInIsExpr
// IsE : AS`IsExpr
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInIsExpr (const TYPE_AS_IsExpr & IsE,
                                                    const SET<TYPE_AS_Name> & id_s)
{
  return (FreeInExpr (IsE.GetRecord(pos_AS_IsExpr_arg), id_s));
}
 
// FreeInNarrowExpr
// IsE : AS`NarrowExpr
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInNarrowExpr (const TYPE_AS_NarrowExpr & NarrowE,
                                                        const SET<TYPE_AS_Name> & id_s)
{
  return (FreeInExpr (NarrowE.GetRecord(pos_AS_NarrowExpr_expr), id_s));
}

// FreeInLambdaExpr
// LambdaE : AS`LambdaExpr
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInLambdaExpr (const TYPE_AS_LambdaExpr & LambdaE,
                                                        const SET<TYPE_AS_Name> & id_s)
{
  const SEQ<TYPE_AS_TypeBind> & tb_l (LambdaE.GetSequence(pos_AS_LambdaExpr_parm));
  SET<TYPE_AS_Name> newid_s (id_s);
  size_t len_tb_l = tb_l.Length();
  for (size_t i = 1; i <= len_tb_l; i++)
    newid_s.ImpUnion (IdentInBind (tb_l[i]));
  return (FreeInExpr (LambdaE.GetRecord(pos_AS_LambdaExpr_body), newid_s));
}

// FreeInFieldSelectExpr
// FieldE : AS`FieldSelectExpr
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInFieldSelectExpr (const TYPE_AS_FieldSelectExpr & FieldE,
                                                             const SET<TYPE_AS_Name> & id_s)
{
  return (FreeInExpr (FieldE.GetRecord(pos_AS_FieldSelectExpr_rec), id_s));
}

// FreeInTypeJudgementExpr
// tj : AS`TypeJudgementExpr
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInTypeJudgementExpr(const TYPE_AS_TypeJudgementExpr & tj,
                                                              const SET<TYPE_AS_Name> & id_s)
{
  return FreeInExpr(tj.GetRecord(pos_AS_TypeJudgementExpr_expr), id_s);
}

// FreeInPreConditionApplyExpr
// pcaE : AS`PreConditionApplyExpr
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInPreConditionApplyExpr(const TYPE_AS_PreConditionApplyExpr & pcaE,
                                                                  const SET<TYPE_AS_Name> & id_s)
{
  const TYPE_AS_Expr & fct (pcaE.GetRecord(pos_AS_PreConditionApplyExpr_fct));
  const SEQ<TYPE_AS_Expr> & arg (pcaE.GetSequence(pos_AS_PreConditionApplyExpr_arg));

  MAP<TYPE_AS_Name, TYPE_SEM_VAL> res_m (FreeInExpr(fct, id_s));
  SET<TYPE_AS_Name> newid_s (id_s);
  newid_s.ImpUnion(res_m.Dom());

  size_t len_arg = arg.Length();
  for (size_t i =1; i <= len_arg; i++)
  {
    MAP<TYPE_AS_Name, TYPE_SEM_VAL> tmp_m (FreeInExpr(arg[i], newid_s));
    res_m.ImpOverride(tmp_m);
    newid_s.ImpUnion(tmp_m.Dom());
  }
  return res_m;
}

// FreeInName
// NameE : AS`Name
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInName (const TYPE_AS_Name & NameE,
                                                  const SET<TYPE_AS_Name> & id_s)
{
  if (id_s.InSet (NameE))
//    return MAP<TYPE_AS_Name, TYPE_SEM_VAL> ();
    return emap;
  else {
    Generic val (LookUpInTopEnv (NameE));
    if (val.IsNil ())
//      return MAP<TYPE_AS_Name, TYPE_SEM_VAL> ();
      return emap;
    else {
      MAP<TYPE_AS_Name, TYPE_SEM_VAL> res_m;
      res_m.Insert (NameE, val);
      return (res_m);
    }
  }
}

#ifdef VDMSL
// FreeInOldName
// OldE : AS`OldName
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInOldName (const TYPE_AS_OldName & OldE,
                                                     const SET<TYPE_AS_Name> & id_s)
{
  TYPE_AS_Name old_name;
  old_name.Init(OldE.GetSequence(pos_AS_OldName_ids), OldE.GetInt(pos_AS_OldName_cid));
  if (id_s.InSet (old_name)) {
    return emap;
  }
  else {
    if (theStackMachine().IsOldState(old_name)) {
      TYPE_GLOBAL_State st (theStackMachine().GetOldState(old_name));
      MAP<TYPE_AS_Name, TYPE_SEM_VAL> res_m;
      res_m.Insert (old_name, st.GetRecord(pos_GLOBAL_State_val));
      return (res_m);
    }
    else {
      RTERR::Error (L"FreeInOldName", RTERR_INTERNAL_ERROR, Nil(), Nil(),Sequence());
      return emap;
    }
  }
}
#endif //VDMSL

// FreeInBracketedExpr
// BrackE : AS`BracketedExpr
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInBracketedExpr (const TYPE_AS_BracketedExpr & BrackE,
                                                           const SET<TYPE_AS_Name> & id_s)
{
  return (FreeInExpr (BrackE.GetRecord(pos_AS_BracketedExpr_expr), id_s));
}

// FreeInIotaExpr
// IotaE : AS`IotaExpr
// id_s : set of AS`Name
// ==> map AS`Name to SEM`VAL
MAP<TYPE_AS_Name, TYPE_SEM_VAL> Free::FreeInIotaExpr (const TYPE_AS_IotaExpr & IotaE,
                                                      const SET<TYPE_AS_Name> & id_s)
{
  const TYPE_AS_Bind & bind (IotaE.GetRecord(pos_AS_IotaExpr_bind));
  const TYPE_AS_Expr & pred_e (IotaE.GetRecord(pos_AS_IotaExpr_pred));

  MAP<TYPE_AS_Name, TYPE_SEM_VAL> res_m (FreeInBind (bind, id_s));
  SET<TYPE_AS_Name> newid_s (id_s);
  newid_s.ImpUnion (IdentInBind (bind));
  newid_s.ImpUnion (res_m.Dom ());
  res_m.ImpOverride (FreeInExpr (pred_e, newid_s));
  return (res_m);
}

// FreeMapToBlkEnv
// id_m : map AS`Name to SEM`VAL
// ==> SEM`BlkEnv
TYPE_SEM_BlkEnv Free::FreeMapToBlkEnv (const MAP<TYPE_AS_Name, TYPE_SEM_VAL> & id_m)
{
  return AUX::MkBlkEnvFromIdMap(id_m, sem_read_only);
}

// LookUpInTopEnv
// name : AS`Name
// ==> [SEM`VAL]
Generic Free::LookUpInTopEnv(const TYPE_AS_Name & name)
{
  // FIXME: check other functions in this file for update to
  // semvalues.h types.

  Tuple infer (theStackMachine().IsLocalVal(name));
  if (infer.GetBool(1)) {
    return Record(infer.GetRecord(2)).GetRecord(pos_SEM_ValTp_val);
  }
  else {
    return Nil();
  }
}

