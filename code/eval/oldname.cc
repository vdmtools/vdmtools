/***
*  * WHAT
*  *    OldName substitution for post condition predicates in operations
*  * FILE
*  *    $Source: /mnt/cvsroot/toolbox/code/eval/oldname.cc,v $
*  * VERSION
*  *    $Revision: 1.50 $
*  * DATE
*  *    $Date: 2006/09/22 11:14:05 $
*  * STATUS
*  *    $State: Exp $
*  * REFERENCES
*  *    
*  * PROJECT
*  *    VDM-SL Toolbox
*  * AUTHOR
*  *    Kees de Bruin (KdB) + $Author: vdmtools $
*  * SPECIFICATION VERSION
*  *    oldvars.cc: Implementation of eval_old.vdm 1.10
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "metaiv.h"
#include "asquotes.h"
#include "AS.h"
#include "astaux.h"
#include "rt_errmsg.h"
#include "oldname.h"
#include "libman.h"

    // in the generation of the post condition predicates for operations,
    // we have to give a pattern for the input and output state. to avoid
    // duplicate pattern id's, we have to create two unique sets of id's.
    // in a post condition, old names are indicated by appending a ~ at the
    // end of an identifier, parsed as an oldname. therefore, the user cannot
    // create pattern id's with a ~ in the id. this is used to create a
    // set of unique pattern id's for the input state. all pattern id's are
    // appended with a ~, and all old names in the post expression are
    // replaced with new normal names appended with a ~.

// OldNameInPattern
// pat : AS`PatternBind
// ==> AS`PatternBind
TYPE_AS_PatternBind OLD::OldNameInPattern (const TYPE_AS_PatternBind & pat)
{
  switch(pat.GetTag())
  {
    case TAG_TYPE_AS_PatternName:
      return (pat);
    case TAG_TYPE_AS_MatchVal: {
      TYPE_AS_MatchVal res_pat (pat);
      res_pat.SetField(pos_AS_MatchVal_val, OldNameInExpr (pat.GetRecord(pos_AS_MatchVal_val)));
      return (res_pat);
    }
    case TAG_TYPE_AS_SetEnumPattern: {
      TYPE_AS_SetEnumPattern res_pat (pat);
      const SEQ<TYPE_AS_Pattern> & els_l (pat.GetSequence(pos_AS_SetEnumPattern_Elems));
      SEQ<TYPE_AS_Pattern> l;
      size_t len_els_l = els_l.Length();
      for (size_t index = 1; index <= len_els_l; index++)
        l.ImpAppend (OldNameInPattern (els_l[index]));
      res_pat.SetField(pos_AS_SetEnumPattern_Elems, l);
      return (res_pat);
    }
    case TAG_TYPE_AS_SeqEnumPattern: {
      TYPE_AS_SeqEnumPattern res_pat (pat);
      const SEQ<TYPE_AS_Pattern> & els_l (pat.GetSequence(pos_AS_SeqEnumPattern_els));
      SEQ<TYPE_AS_Pattern> l;
      size_t len_els_l = els_l.Length();
      for (size_t index = 1; index <= len_els_l; index++)
        l.ImpAppend (OldNameInPattern (els_l[index]));
      res_pat.SetField(pos_AS_SeqEnumPattern_els, l);
      return (res_pat);
    }
    case TAG_TYPE_AS_MapEnumPattern: {
      TYPE_AS_MapEnumPattern res_pat (pat);
      const SEQ<TYPE_AS_MapletPattern> & mls_l (pat.GetSequence(pos_AS_MapEnumPattern_mls));
      SEQ<TYPE_AS_Pattern> l;
      size_t len_mls_l = mls_l.Length();
      for (size_t index = 1; index <= len_mls_l; index++)
        l.ImpAppend (OldNameInPattern (mls_l[index]));
      res_pat.SetField(pos_AS_MapEnumPattern_mls, l);
      return (res_pat);
    }
    case TAG_TYPE_AS_SetUnionPattern: {
      TYPE_AS_SetUnionPattern res_pat (pat);
      res_pat.SetField(pos_AS_SetUnionPattern_lp, OldNameInPattern (pat.GetRecord(pos_AS_SetUnionPattern_lp)));
      res_pat.SetField(pos_AS_SetUnionPattern_rp, OldNameInPattern (pat.GetRecord(pos_AS_SetUnionPattern_rp)));
      return (res_pat);
    }
    case TAG_TYPE_AS_SeqConcPattern: {
      TYPE_AS_SeqConcPattern res_pat (pat);
      res_pat.SetField(pos_AS_SeqConcPattern_lp, OldNameInPattern (pat.GetRecord(pos_AS_SeqConcPattern_lp)));
      res_pat.SetField(pos_AS_SeqConcPattern_rp, OldNameInPattern (pat.GetRecord(pos_AS_SeqConcPattern_rp)));
      return (res_pat);
    }
    case TAG_TYPE_AS_MapMergePattern: {
      TYPE_AS_MapMergePattern res_pat (pat);
      res_pat.SetField(pos_AS_MapMergePattern_lp, OldNameInPattern (pat.GetRecord(pos_AS_MapMergePattern_lp)));
      res_pat.SetField(pos_AS_MapMergePattern_rp, OldNameInPattern (pat.GetRecord(pos_AS_MapMergePattern_rp)));
      return (res_pat);
    }
    case TAG_TYPE_AS_MapletPattern: {
      TYPE_AS_MapletPattern res_pat (pat);
      res_pat.SetField(pos_AS_MapletPattern_dp, OldNameInPattern (pat.GetRecord(pos_AS_MapletPattern_dp)));
      res_pat.SetField(pos_AS_MapletPattern_rp, OldNameInPattern (pat.GetRecord(pos_AS_MapletPattern_rp)));
      return (res_pat);
    }
    case TAG_TYPE_AS_RecordPattern: {
      TYPE_AS_RecordPattern res_pat (pat);
      const SEQ<TYPE_AS_Pattern> & fields (pat.GetSequence(pos_AS_RecordPattern_fields));
      SEQ<TYPE_AS_Pattern> l;
      size_t len_fields = fields.Length();
      for (size_t index = 1; index <= len_fields; index++)
        l.ImpAppend (OldNameInPattern (fields[index]));
      res_pat.SetField(pos_AS_RecordPattern_fields, l);
      return (res_pat);
    }
    case TAG_TYPE_AS_TuplePattern: {
      TYPE_AS_TuplePattern res_pat (pat);
      const SEQ<TYPE_AS_Pattern> & fields (pat.GetSequence(pos_AS_TuplePattern_fields));
      SEQ<TYPE_AS_Pattern> l;
      size_t len_fields = fields.Length();
      for (size_t index = 1; index <= len_fields; index++)
        l.ImpAppend (OldNameInPattern (fields[index]));
      res_pat.SetField(pos_AS_TuplePattern_fields, l);
      return (res_pat);
    }
#ifdef VDMPP
    case TAG_TYPE_AS_ObjectPattern: {
      const SEQ<TYPE_AS_FieldPattern> & fields (pat.GetSequence(pos_AS_ObjectPattern_fields));
      TYPE_AS_ObjectPattern res_pat (pat);
      SEQ<TYPE_AS_FieldPattern> l;
      size_t len_fields = fields.Length();
      for (size_t index = 1; index <= len_fields; index++)
        l.ImpAppend (OldNameInPattern (fields[index]));
      res_pat.SetField(pos_AS_ObjectPattern_fields, l);
      return (res_pat);
    }
    case TAG_TYPE_AS_FieldPattern: {
      TYPE_AS_ObjectPattern res_pat (pat);
      res_pat.SetField(pos_AS_FieldPattern_pat, OldNameInPattern(pat.GetRecord(pos_AS_FieldPattern_pat)));
      return (res_pat);
    }
#endif // VDMPP
    case TAG_TYPE_AS_SetBind: {
      TYPE_AS_SetBind res_pat (pat);
      res_pat.SetField(pos_AS_SetBind_pat, OldNameInPattern (pat.GetRecord(pos_AS_SetBind_pat)));
      res_pat.SetField(pos_AS_SetBind_Set, OldNameInExpr (pat.GetRecord(pos_AS_SetBind_Set)));
      return (res_pat);
    }
    case TAG_TYPE_AS_TypeBind: {
      TYPE_AS_TypeBind res_pat (pat);
      res_pat.SetField(pos_AS_TypeBind_pat, OldNameInPattern (pat.GetRecord(pos_AS_TypeBind_pat)));
      return (res_pat);
    }
    default: {
      RTERR::Error (L"OldNameInPattern", RTERR_PATTERN_UNKNOWN, Nil(), Nil(), Sequence());
    }
  }
  return Record(); // Dummy return.
}

// OldNameInBind
// bind : AS`Bind
// ==> AS`Bind
TYPE_AS_Bind OLD::OldNameInBind (const TYPE_AS_Bind & bind)
{
  return (OldNameInPattern (bind));
}

// OldNameInMultBindSeq
// bind_l : seq of AS`MultBind
// ==> seq of AS`MultBind
SEQ<TYPE_AS_MultBind> OLD::OldNameInMultBindSeq (const SEQ<TYPE_AS_MultBind> & bind_l)
{
  SEQ<TYPE_AS_MultBind> new_l;
  size_t len_bind_l = bind_l.Length();
  for (size_t index = 1; index <= len_bind_l; index++)
  {
    const TYPE_AS_MultBind & mb (bind_l[index]);
    switch(mb.GetTag()) {
      case TAG_TYPE_AS_MultSetBind: {
        const SEQ<TYPE_AS_Pattern> & pat_l (mb.GetSequence(pos_AS_MultSetBind_pat));
        SEQ<TYPE_AS_Pattern> tmp_l;
        size_t len_pat_l = pat_l.Length();
        for (size_t idx = 1; idx <= len_pat_l; idx++)
          tmp_l.ImpAppend (OldNameInPattern (pat_l[idx]));
  
        TYPE_AS_MultSetBind new_bind (mb);
        new_bind.SetField(pos_AS_MultSetBind_pat, tmp_l);
        new_l.ImpAppend (new_bind);
        break;
      }
      case TAG_TYPE_AS_MultTypeBind: {
        const SEQ<TYPE_AS_Pattern> & pat_l (mb.GetSequence(pos_AS_MultTypeBind_pat));
        SEQ<TYPE_AS_Pattern> tmp_l;
        size_t len_pat_l = pat_l.Length();
        for (size_t idx = 1; idx <= len_pat_l; idx++)
          tmp_l.ImpAppend (OldNameInPattern (pat_l[idx]));

        TYPE_AS_MultTypeBind new_bind (mb);
        new_bind.SetField(pos_AS_MultTypeBind_pat, tmp_l);
        new_l.ImpAppend (new_bind);
        break;
      }
      default:
        break;
    }
  }
  return (new_l);
}

// OldNameInExpr
// expr : AS`Expr
// ==> AS`Expr
TYPE_AS_Expr OLD::OldNameInExpr (const TYPE_AS_Expr & expr)
{
  switch (expr.GetTag ()) {
  case TAG_TYPE_AS_DefExpr:
    return OldNameInDefExpr (expr); break;
  case TAG_TYPE_AS_LetExpr:
    return OldNameInLetExpr (expr); break;
  case TAG_TYPE_AS_LetBeSTExpr:
    return OldNameInLetBeSTExpr (expr); break;
  case TAG_TYPE_AS_IfExpr:
    return OldNameInIfExpr (expr); break;
  case TAG_TYPE_AS_CasesExpr:
    return OldNameInCasesExpr (expr); break;
  case TAG_TYPE_AS_PrefixExpr:
    return OldNameInPrefixExpr (expr); break;
  case TAG_TYPE_AS_BinaryExpr:
    return OldNameInBinaryExpr (expr); break;
  case TAG_TYPE_AS_AllOrExistsExpr:
    return OldNameInAllOrExistsExpr (expr); break;
  case TAG_TYPE_AS_ExistsUniqueExpr:
    return OldNameInExistsUniqueExpr (expr); break;
  case TAG_TYPE_AS_SetEnumerationExpr:
    return OldNameInSetEnumerationExpr (expr); break;
  case TAG_TYPE_AS_SetComprehensionExpr:
    return OldNameInSetComprehensionExpr (expr); break;
  case TAG_TYPE_AS_SetRangeExpr:
    return OldNameInSetRangeExpr (expr); break;
  case TAG_TYPE_AS_SeqEnumerationExpr:
    return OldNameInSeqEnumerationExpr (expr); break;
  case TAG_TYPE_AS_SeqComprehensionExpr:
    return OldNameInSeqComprehensionExpr (expr); break;
  case TAG_TYPE_AS_SubSequenceExpr:
    return OldNameInSubSequenceExpr (expr); break;
  case TAG_TYPE_AS_SeqModifyMapOverrideExpr:
    return OldNameInSeqModifyMapOverrideExpr (expr); break;
  case TAG_TYPE_AS_MapEnumerationExpr:
    return OldNameInMapEnumerationExpr (expr); break;
  case TAG_TYPE_AS_MapComprehensionExpr:
    return OldNameInMapComprehensionExpr (expr); break;
  case TAG_TYPE_AS_TupleConstructorExpr:
    return OldNameInTupleConstructorExpr (expr); break;
  case TAG_TYPE_AS_RecordConstructorExpr:
    return OldNameInRecordConstructorExpr (expr); break;
  case TAG_TYPE_AS_RecordModifierExpr:
    return OldNameInRecordModifierExpr (expr); break;
  case TAG_TYPE_AS_TokenConstructorExpr:
    return OldNameInTokenConstructorExpr (expr); break;
  case TAG_TYPE_AS_ApplyExpr:
    return OldNameInApplyExpr (expr); break;
  case TAG_TYPE_AS_LambdaExpr:
    return OldNameInLambdaExpr (expr); break;
  case TAG_TYPE_AS_FieldSelectExpr:
    return OldNameInFieldSelectExpr (expr); break;
  case TAG_TYPE_AS_FctTypeInstExpr:
    return OldNameInFctTypeInstExpr (expr); break;
  case TAG_TYPE_AS_IsExpr:
    return OldNameInIsExpr (expr); break;
  case TAG_TYPE_AS_NarrowExpr:
    return OldNameInNarrowExpr (expr); break;
  case TAG_TYPE_AS_IotaExpr:
    return OldNameInIotaExpr (expr); break;
  case TAG_TYPE_AS_UndefinedExpr:
  case TAG_TYPE_AS_Name:
  case TAG_TYPE_AS_BoolLit:
  case TAG_TYPE_AS_CharLit:
  case TAG_TYPE_AS_TextLit:
  case TAG_TYPE_AS_QuoteLit:
  case TAG_TYPE_AS_RealLit:
  case TAG_TYPE_AS_NumLit:
  case TAG_TYPE_AS_NilLit:
    return (expr); break;
  case TAG_TYPE_AS_OldName:
    return OldNameInOldName (expr); break;
  case TAG_TYPE_AS_BracketedExpr:
    return OldNameInBracketedExpr (expr); break;
  case TAG_TYPE_AS_Macro:
    return (expr); break;
  }

  RTERR::Error (L"OldNameInExpr", RTERR_EXPR_UNKNOWN, Nil(), Nil(), Sequence());
  return Record ();//to avoid warnings from gcc
}

// OldNameInDefExpr
// expr : AS`DefExpr
// ==> AS`Expr
TYPE_AS_Expr OLD::OldNameInDefExpr (const TYPE_AS_DefExpr & expr)
{
  const Sequence & def_l (expr.GetSequence(pos_AS_DefExpr_Def));
  type_dd2PL new_l;
  size_t len_def_l = def_l.Length();
  for (size_t index = 1; index <= len_def_l; index++)
  {
    const type_dd2P & tmp (def_l[index]);
    type_dd2P new_elm;
    new_elm.SetField (1, OldNameInPattern (tmp.GetRecord (1)));
    new_elm.SetField (2, OldNameInExpr (tmp.GetRecord (2)));
    new_l.ImpAppend (new_elm);
  }

  TYPE_AS_DefExpr new_expr (expr);
  new_expr.SetField(pos_AS_DefExpr_Def, new_l);
  new_expr.SetField(pos_AS_DefExpr_In, OldNameInExpr (expr.GetRecord(pos_AS_DefExpr_In)));

  return (new_expr);
}

// OldNameInLetExpr
// expr : AS`LetExpr
// ==> AS`Expr
TYPE_AS_Expr OLD::OldNameInLetExpr (const TYPE_AS_LetExpr & expr)
{
  const SEQ<TYPE_AS_LocalDef> & localdef (expr.GetSequence(pos_AS_LetExpr_localdef));
  SEQ<TYPE_AS_LocalDef> new_l;
  size_t len_localdef = localdef.Length();
  for (size_t index = 1; index <= len_localdef; index++)
  {
    const TYPE_AS_LocalDef & ldef (localdef[index]);
    switch(ldef.GetTag()) {
      case TAG_TYPE_AS_ValueDef: {
        TYPE_AS_ValueDef new_tmp (ldef);
        new_tmp.SetField(pos_AS_ValueDef_pat, OldNameInPattern (ldef.GetRecord(pos_AS_ValueDef_pat)));
        new_tmp.SetField(pos_AS_ValueDef_val, OldNameInExpr (ldef.GetRecord(pos_AS_ValueDef_val)));
        new_tmp.SetField(pos_AS_ValueDef_access, Int(NOT_INITIALISED_AS));
        new_tmp.SetField(pos_AS_ValueDef_stat, Bool(false));
        new_l.ImpAppend (new_tmp);
        break;
      }
      case TAG_TYPE_AS_ExplFnDef:
      case TAG_TYPE_AS_ImplFnDef:
      case TAG_TYPE_AS_ExtExplFnDef: {
        new_l.ImpAppend (ldef);
        break;
      }
      default:
        break;
    }
  }
  TYPE_AS_LetExpr new_expr (expr);
  new_expr.SetField(pos_AS_LetExpr_localdef, new_l);
  new_expr.SetField(pos_AS_LetExpr_body, OldNameInExpr (expr.GetRecord(pos_AS_LetExpr_body)));
  return (new_expr);
}

// OldNameInLetBeSTExpr
// expr : AS`LetBeSTExpr
// ==> AS`Expr
TYPE_AS_Expr OLD::OldNameInLetBeSTExpr (const TYPE_AS_LetBeSTExpr & expr)
{
  TYPE_AS_LetBeSTExpr new_expr (expr);
  new_expr.SetField(pos_AS_LetBeSTExpr_lhs, OldNameInMultBindSeq (expr.GetSequence(pos_AS_LetBeSTExpr_lhs)));
  new_expr.SetField(pos_AS_LetBeSTExpr_St, OldNameInGenericExpr (expr.GetField(pos_AS_LetBeSTExpr_St)));
  new_expr.SetField(pos_AS_LetBeSTExpr_In, OldNameInExpr (expr.GetRecord(pos_AS_LetBeSTExpr_In)));
  return (new_expr);
}

// OldNameInIfExpr
// expr : AS`IfExpr
// ==> AS`Expr
TYPE_AS_Expr OLD::OldNameInIfExpr (const TYPE_AS_IfExpr & expr)
{
  const SEQ<TYPE_AS_ElseifExpr> & elif (expr.GetSequence(pos_AS_IfExpr_elsif));
  SEQ<TYPE_AS_ElseifExpr> new_l;
  size_t len_elif = elif.Length();
  for (size_t index = 1; index <= len_elif; index++)
  {
    const TYPE_AS_ElseifExpr & tmp (elif[index]);
    TYPE_AS_ElseifExpr new_tmp (tmp);
    new_tmp.SetField(pos_AS_ElseifExpr_test, OldNameInExpr (tmp.GetRecord(pos_AS_ElseifExpr_test)));
    new_tmp.SetField(pos_AS_ElseifExpr_cons, OldNameInExpr (tmp.GetRecord(pos_AS_ElseifExpr_cons)));
    new_l.ImpAppend (new_tmp);
  }
  
  TYPE_AS_IfExpr new_expr (expr);
  new_expr.SetField(pos_AS_IfExpr_test, OldNameInExpr (expr.GetRecord(pos_AS_IfExpr_test)));
  new_expr.SetField(pos_AS_IfExpr_cons, OldNameInExpr (expr.GetRecord(pos_AS_IfExpr_cons)));
  new_expr.SetField(pos_AS_IfExpr_elsif, new_l);
  new_expr.SetField(pos_AS_IfExpr_altn, OldNameInExpr (expr.GetRecord(pos_AS_IfExpr_altn)));

  return (new_expr);
}

// OldNameInCasesExpr
// expr : AS`CasesExpr
// ==> AS`Expr
TYPE_AS_Expr OLD::OldNameInCasesExpr (const TYPE_AS_CasesExpr & expr)
{
  const SEQ<TYPE_AS_CaseAltn> & altn (expr.GetSequence(pos_AS_CasesExpr_altns));
  SEQ<TYPE_AS_CaseAltn> new_l;
  size_t len_altn = altn.Length();
  for (size_t index = 1; index <= len_altn; index++)
  {
    const TYPE_AS_CaseAltn & tmp (altn[index]);
    TYPE_AS_CaseAltn new_tmp (tmp);
    const SEQ<TYPE_AS_Pattern> & match (tmp.GetSequence(pos_AS_CaseAltn_match));
    SEQ<TYPE_AS_Pattern> pat_lp;
    size_t len_match = match.Length();
    for (size_t idx = 1; idx <= len_match; idx++)
      pat_lp.ImpAppend (OldNameInPattern (match[idx]));
    new_tmp.SetField(pos_AS_CaseAltn_match, pat_lp);
    new_tmp.SetField(pos_AS_CaseAltn_body, OldNameInExpr (tmp.GetRecord(pos_AS_CaseAltn_body)));
    new_l.ImpAppend(new_tmp);
  }

  TYPE_AS_CasesExpr new_expr (expr);
  new_expr.SetField(pos_AS_CasesExpr_sel, OldNameInExpr (expr.GetRecord(pos_AS_CasesExpr_sel)));
  new_expr.SetField(pos_AS_CasesExpr_altns, new_l);
  new_expr.SetField(pos_AS_CasesExpr_Others, OldNameInGenericExpr (expr.GetField(pos_AS_CasesExpr_Others)));

  return (new_expr);
}

// OldNameInPrefixExpr
// expr : AS`PrefixExpr
// ==> AS`Expr
TYPE_AS_Expr OLD::OldNameInPrefixExpr (const TYPE_AS_PrefixExpr & expr)
{
  TYPE_AS_PrefixExpr new_expr (expr);
  new_expr.SetField(pos_AS_PrefixExpr_arg, OldNameInExpr (expr.GetRecord(pos_AS_PrefixExpr_arg)));
  return (new_expr);
}

// OldNameInBinaryExpr
// expr : AS`BinaryExpr
// ==> AS`Expr
TYPE_AS_Expr OLD::OldNameInBinaryExpr (const TYPE_AS_BinaryExpr & expr)
{
  TYPE_AS_BinaryExpr new_expr (expr);
  new_expr.SetField(pos_AS_BinaryExpr_left, OldNameInExpr (expr.GetRecord(pos_AS_BinaryExpr_left)));
  new_expr.SetField(pos_AS_BinaryExpr_right, OldNameInExpr (expr.GetRecord(pos_AS_BinaryExpr_right)));
  return (new_expr);
}

// OldNameInAllOrExistsExpr
// expr : AS`AllOrExistsExpr
// ==> AS`Expr
TYPE_AS_Expr OLD::OldNameInAllOrExistsExpr (const TYPE_AS_AllOrExistsExpr & expr)
{
  TYPE_AS_AllOrExistsExpr new_expr (expr);
  new_expr.SetField(pos_AS_AllOrExistsExpr_bind, OldNameInMultBindSeq (expr.GetSequence(pos_AS_AllOrExistsExpr_bind)));
  new_expr.SetField(pos_AS_AllOrExistsExpr_pred, OldNameInExpr (expr.GetRecord(pos_AS_AllOrExistsExpr_pred)));
  return (new_expr);
}

// OldNameInExistsUniqueExpr
// expr : AS`ExistsUniqueExpr
// ==> AS`Expr
TYPE_AS_Expr OLD::OldNameInExistsUniqueExpr (const TYPE_AS_ExistsUniqueExpr & expr)
{
  TYPE_AS_ExistsUniqueExpr new_expr (expr);
  new_expr.SetField(pos_AS_ExistsUniqueExpr_bind, OldNameInBind (expr.GetRecord(pos_AS_ExistsUniqueExpr_bind)));
  new_expr.SetField(pos_AS_ExistsUniqueExpr_pred, OldNameInExpr (expr.GetRecord(pos_AS_ExistsUniqueExpr_pred)));
  return (new_expr);
}

// OldNameInSetEnumerationExpr
// expr : AS`SetEnumerationExpr
// ==> AS`Expr
TYPE_AS_Expr OLD::OldNameInSetEnumerationExpr (const TYPE_AS_SetEnumerationExpr & expr)
{
  const SEQ<TYPE_AS_Expr> & elm_le (expr.GetSequence(pos_AS_SeqEnumerationExpr_els));
  SEQ<TYPE_AS_Expr> new_l;
  size_t len_elm_le = elm_le.Length();
  for (size_t index = 1; index <= len_elm_le; index++)
    new_l.ImpAppend (OldNameInExpr (elm_le[index]));

  TYPE_AS_SetEnumerationExpr new_expr (expr);
  new_expr.SetField(pos_AS_SeqEnumerationExpr_els, new_l);
  return (new_expr);
}

// OldNameInSetComprehensionExpr
// expr : AS`SetComprehensionExpr
// ==> AS`Expr
TYPE_AS_Expr OLD::OldNameInSetComprehensionExpr (const TYPE_AS_SetComprehensionExpr & expr)
{
  TYPE_AS_SetComprehensionExpr new_expr (expr);
  new_expr.SetField(pos_AS_SetComprehensionExpr_elem, OldNameInExpr (expr.GetRecord(pos_AS_SetComprehensionExpr_elem)));
  new_expr.SetField(pos_AS_SetComprehensionExpr_bind, OldNameInMultBindSeq (expr.GetSequence(pos_AS_SetComprehensionExpr_bind)));
  new_expr.SetField(pos_AS_SetComprehensionExpr_pred, OldNameInGenericExpr (expr.GetField(pos_AS_SetComprehensionExpr_pred)));
  return (new_expr);
}

// OldNameInSetRangeExpr
// expr : AS`SetRangeExpr
// ==> AS`Expr
TYPE_AS_Expr OLD::OldNameInSetRangeExpr (const TYPE_AS_SetRangeExpr & expr)
{
  TYPE_AS_SetRangeExpr new_expr (expr);
  new_expr.SetField(pos_AS_SetRangeExpr_lb, OldNameInExpr (expr.GetRecord(pos_AS_SetRangeExpr_lb)));
  new_expr.SetField(pos_AS_SetRangeExpr_ub, OldNameInExpr (expr.GetRecord(pos_AS_SetRangeExpr_ub)));
  return (new_expr);
}

// OldNameInSeqEnumerationExpr
// expr : AS`SeqEnumerationExpr
// ==> AS`Expr
TYPE_AS_Expr OLD::OldNameInSeqEnumerationExpr (const TYPE_AS_SeqEnumerationExpr & expr)
{
  const SEQ<TYPE_AS_Expr> & els_l (expr.GetSequence(pos_AS_SeqEnumerationExpr_els));
  SEQ<TYPE_AS_Expr> new_l;
  size_t len_els_l = els_l.Length();
  for (size_t index = 1; index <= len_els_l; index++)
    new_l.ImpAppend (OldNameInExpr (els_l[index]));

  TYPE_AS_SeqEnumerationExpr new_expr (expr);
  new_expr.SetField(pos_AS_SeqEnumerationExpr_els, new_l);
  return (new_expr);
}

// OldNameInSeqComprehensionExpr
// expr : AS`SeqComprehensionExpr
// ==> AS`Expr
TYPE_AS_Expr OLD::OldNameInSeqComprehensionExpr (const TYPE_AS_SeqComprehensionExpr & expr)
{
  TYPE_AS_SeqComprehensionExpr new_expr (expr);
  new_expr.SetField(pos_AS_SeqComprehensionExpr_elem, OldNameInExpr (expr.GetRecord(pos_AS_SeqComprehensionExpr_elem)));
  //new_expr.SetField(pos_AS_SeqComprehensionExpr_bind, OldNameInBind (expr.GetRecord(pos_AS_SeqComprehensionExpr_bind)));
  const Record & bind (expr.GetRecord(pos_AS_SeqComprehensionExpr_bind));
  switch (bind.GetTag()) {
    case TAG_TYPE_AS_SetBind: {
      TYPE_AS_SetBind new_bind (bind);
      new_bind.SetField(pos_AS_SetBind_pat, OldNameInPattern (bind.GetRecord(pos_AS_SetBind_pat)));
      new_bind.SetField(pos_AS_SetBind_Set, OldNameInExpr (bind.GetRecord(pos_AS_SetBind_Set)));
      new_expr.SetField(pos_AS_SeqComprehensionExpr_bind, new_bind);
      break;
    }
    case TAG_TYPE_AS_SeqBind: {
      TYPE_AS_SeqBind new_bind (bind);
      new_bind.SetField(pos_AS_SeqBind_pat, OldNameInPattern (bind.GetRecord(pos_AS_SeqBind_pat)));
      new_bind.SetField(pos_AS_SeqBind_Seq, OldNameInExpr (bind.GetRecord(pos_AS_SeqBind_Seq)));
      new_expr.SetField(pos_AS_SeqComprehensionExpr_bind, new_bind);
      break;
    }
  }
  new_expr.SetField(pos_AS_SeqComprehensionExpr_pred, OldNameInGenericExpr (expr.GetField(pos_AS_SeqComprehensionExpr_pred)));
  return (new_expr);
}

// OldNameInSubSequenceExpr
// expr : AS`SubSequenceExpr
// ==> AS`Expr
TYPE_AS_Expr OLD::OldNameInSubSequenceExpr (const TYPE_AS_SubSequenceExpr & expr)
{
  TYPE_AS_SubSequenceExpr new_expr (expr);
  new_expr.SetField(pos_AS_SubSequenceExpr_sequence, OldNameInExpr (expr.GetRecord(pos_AS_SubSequenceExpr_sequence)));
  new_expr.SetField(pos_AS_SubSequenceExpr_frompos, OldNameInExpr (expr.GetRecord(pos_AS_SubSequenceExpr_frompos)));
  new_expr.SetField(pos_AS_SubSequenceExpr_topos, OldNameInExpr (expr.GetRecord(pos_AS_SubSequenceExpr_topos)));
  return (new_expr);
}

// OldNameInSeqModifyMapOverrideExpr
// expr : AS`SeqModifyMapOverrideExpr
// ==> AS`Expr
TYPE_AS_Expr OLD::OldNameInSeqModifyMapOverrideExpr (const TYPE_AS_SeqModifyMapOverrideExpr & expr)
{
  TYPE_AS_SeqModifyMapOverrideExpr new_expr (expr);
  new_expr.SetField(pos_AS_SeqModifyMapOverrideExpr_seqmap, OldNameInExpr (expr.GetRecord(pos_AS_SeqModifyMapOverrideExpr_seqmap)));
  new_expr.SetField(pos_AS_SeqModifyMapOverrideExpr_mapexp, OldNameInExpr (expr.GetRecord(pos_AS_SeqModifyMapOverrideExpr_mapexp)));
  return (new_expr);
}

// OldNameInMapEnumerationExpr
// expr : AS`MapEnumerationExpr
// ==> AS`Expr
TYPE_AS_Expr OLD::OldNameInMapEnumerationExpr (const TYPE_AS_MapEnumerationExpr & expr)
{
  const SEQ<TYPE_AS_Maplet> & els_l (expr.GetSequence(pos_AS_MapEnumerationExpr_els));
  SEQ<TYPE_AS_Maplet> new_l;
  size_t len_els_l = els_l.Length();
  for (size_t index = 1; index <= len_els_l; index++)
    new_l.ImpAppend (OldNameInMaplet (els_l[index]));

  TYPE_AS_MapEnumerationExpr new_expr (expr);
  new_expr.SetField(pos_AS_MapEnumerationExpr_els, new_l);
  return (new_expr);
}

// OldNameInMapComprehensionExpr
// expr : AS`MapComprehensionExpr
// ==> AS`Expr
TYPE_AS_Expr OLD::OldNameInMapComprehensionExpr (const TYPE_AS_MapComprehensionExpr & expr)
{
  TYPE_AS_MapComprehensionExpr new_expr (expr);
  new_expr.SetField(pos_AS_MapComprehensionExpr_elem, OldNameInMaplet (expr.GetRecord(pos_AS_MapComprehensionExpr_elem)));
  new_expr.SetField(pos_AS_MapComprehensionExpr_bind, OldNameInMultBindSeq (expr.GetSequence(pos_AS_MapComprehensionExpr_bind)));
  new_expr.SetField(pos_AS_MapComprehensionExpr_pred, OldNameInGenericExpr (expr.GetField(pos_AS_MapComprehensionExpr_pred)));
  return (new_expr);
}

// OldNameInTupleConstructorExpr
// expr : AS`TupleConstructorExpr
// ==> AS`Expr
TYPE_AS_Expr OLD::OldNameInTupleConstructorExpr (const TYPE_AS_TupleConstructorExpr & expr)
{
  const SEQ<TYPE_AS_Expr> & fields (expr.GetSequence(pos_AS_TupleConstructorExpr_fields));
  SEQ<TYPE_AS_Expr> new_l;
  size_t len_fields = fields.Length();
  for (size_t index = 1; index <= len_fields; index++)
    new_l.ImpAppend (OldNameInExpr (fields[index]));

  TYPE_AS_TupleConstructorExpr new_expr (expr);
  new_expr.SetField(pos_AS_TupleConstructorExpr_fields, new_l);
  return (new_expr);
}

// OldNameInRecordConstructorExpr
// expr : AS`RecordConstructorExpr
// ==> AS`Expr
TYPE_AS_Expr OLD::OldNameInRecordConstructorExpr (const TYPE_AS_RecordConstructorExpr & expr)
{
  const SEQ<TYPE_AS_Expr> & fields (expr.GetSequence(pos_AS_RecordConstructorExpr_fields));
  SEQ<TYPE_AS_Expr> new_l;
  size_t len_fields = fields.Length();
  for (size_t index = 1; index <= len_fields; index++)
    new_l.ImpAppend (OldNameInExpr (fields[index]));
  
  TYPE_AS_RecordConstructorExpr new_expr (expr);
  new_expr.SetField(pos_AS_RecordConstructorExpr_fields, new_l);
  return (new_expr);
}

// OldNameInRecordModifierExpr
// expr : AS`RecordModifierExpr
// ==> AS`Expr
TYPE_AS_Expr OLD::OldNameInRecordModifierExpr (const TYPE_AS_RecordModifierExpr & expr)
{
  const SEQ<TYPE_AS_RecordModification> & mod_l (expr.GetSequence(pos_AS_RecordModifierExpr_modifiers));
  SEQ<TYPE_AS_RecordModification> new_l;
  size_t len_mod_l = mod_l.Length();
  for (size_t index = 1; index <= len_mod_l; index++)
  {
    const TYPE_AS_RecordModification & tmp (mod_l[index]);
    TYPE_AS_RecordModification new_tmp (tmp);
    new_tmp.set_newexpr (OldNameInExpr (tmp.GetRecord(pos_AS_RecordModification_newexpr)));
    new_l.ImpAppend (new_tmp);
  }

  TYPE_AS_RecordModifierExpr new_expr (expr);
  new_expr.SetField(pos_AS_RecordModifierExpr_rec, OldNameInExpr (expr.GetRecord(pos_AS_RecordModifierExpr_rec)));
  new_expr.SetField(pos_AS_RecordModifierExpr_modifiers, new_l);
  return (new_expr);
}

// OldNameInTokenConstructorExpr
// expr : AS`TokenConstructorExpr
// ==> AS`Expr
TYPE_AS_Expr OLD::OldNameInTokenConstructorExpr (const TYPE_AS_TokenConstructorExpr & expr)
{
  TYPE_AS_TokenConstructorExpr new_expr (expr);
  new_expr.SetField(pos_AS_TokenConstructorExpr_field, OldNameInExpr (expr.GetRecord(pos_AS_TokenConstructorExpr_field)));
  return (new_expr);
}

// OldNameInApplyExpr
// expr : AS`ApplyExpr
// ==> AS`Expr
TYPE_AS_Expr OLD::OldNameInApplyExpr (const TYPE_AS_ApplyExpr & expr)
{
  const SEQ<TYPE_AS_Expr> & arg_l (expr.GetSequence(pos_AS_ApplyExpr_arg));
  SEQ<TYPE_AS_Expr> new_l;
  size_t len_arg_l = arg_l.Length();
  for (size_t index = 1; index <= len_arg_l; index++)
    new_l.ImpAppend (OldNameInExpr (arg_l[index]));

  TYPE_AS_ApplyExpr new_expr (expr);
  new_expr.SetField(pos_AS_ApplyExpr_fct, OldNameInExpr (expr.GetRecord(pos_AS_ApplyExpr_fct)));
  new_expr.SetField(pos_AS_ApplyExpr_arg, new_l);
  return (new_expr);
}

// OldNameInFieldSelectExpr
// expr : AS`FieldSelectExpr
// ==> AS`Expr
TYPE_AS_Expr OLD::OldNameInFieldSelectExpr (const TYPE_AS_FieldSelectExpr & expr)
{
  TYPE_AS_FieldSelectExpr new_expr (expr);
  new_expr.SetField(pos_AS_FieldSelectExpr_rec, OldNameInExpr (expr.GetRecord(pos_AS_FieldSelectExpr_rec)));
  return (new_expr);
}

// OldNameInFctTypeInstExpr
// expr : AS`FctTypeInstExpr
// ==> AS`Expr
TYPE_AS_Expr OLD::OldNameInFctTypeInstExpr (const TYPE_AS_FctTypeInstExpr & expr)
{
  TYPE_AS_FctTypeInstExpr new_expr (expr);
  new_expr.SetField(pos_AS_FctTypeInstExpr_polyfct, OldNameInExpr (expr.GetRecord(pos_AS_FctTypeInstExpr_polyfct)));
  return (new_expr);
}

// OldNameInLambdaExpr
// expr : AS`LambdaExpr
// ==> AS`Expr
TYPE_AS_Expr OLD::OldNameInLambdaExpr (const TYPE_AS_LambdaExpr & expr)
{
  TYPE_AS_LambdaExpr new_expr (expr);
  new_expr.SetField(pos_AS_LambdaExpr_body, OldNameInExpr (expr.GetRecord(pos_AS_LambdaExpr_body)));
  return (new_expr);
}

// OldNameInIsExpr
// expr : AS`IsExpr
// ==> AS`Expr
TYPE_AS_Expr OLD::OldNameInIsExpr (const TYPE_AS_IsExpr & expr)
{
  TYPE_AS_IsExpr new_expr (expr);
  new_expr.SetField(pos_AS_IsExpr_arg, OldNameInExpr (expr.GetRecord(pos_AS_IsExpr_arg)));
  return (new_expr);
}

// OldNameInnarrowExpr
// expr : AS`NarrowExpr
// ==> AS`Expr
TYPE_AS_Expr OLD::OldNameInNarrowExpr (const TYPE_AS_NarrowExpr & expr)
{
  TYPE_AS_NarrowExpr new_expr (expr);
  new_expr.SetField(pos_AS_NarrowExpr_expr, OldNameInExpr (expr.GetRecord(pos_AS_NarrowExpr_expr)));
  return (new_expr);
}

// OldNameInOldName
// expr : AS`OldName
// ==> AS`Expr
TYPE_AS_Expr OLD::OldNameInOldName (const TYPE_AS_OldName & expr)
{
  TYPE_AS_Ids id_l (expr.get_ids ());
  TYPE_AS_Id id_q (id_l[id_l.Length ()]);
  id_q.ImpAppend(Char('~'));
  TYPE_AS_Name new_expr (ASTAUX::MkNameFromId (id_q, expr.GetInt(pos_AS_OldName_cid)));
  return (new_expr);
}

// OldNameInBracketedExpr
// expr : AS`BracketedExpr
// ==> AS`Expr
TYPE_AS_Expr OLD::OldNameInBracketedExpr (const TYPE_AS_BracketedExpr & expr)
{
  TYPE_AS_BracketedExpr new_expr (expr);
  new_expr.SetField(pos_AS_BracketedExpr_expr, OldNameInExpr (expr.GetRecord(pos_AS_BracketedExpr_expr)));
  return (new_expr);
}

// OldNameInIotaExpr
// expr : AS`IotaExpr
// ==> AS`Expr
TYPE_AS_Expr OLD::OldNameInIotaExpr (const TYPE_AS_IotaExpr & expr)
{
  TYPE_AS_IotaExpr new_expr (expr);
  new_expr.SetField(pos_AS_IotaExpr_bind, OldNameInBind (expr.GetRecord(pos_AS_IotaExpr_bind)));
  new_expr.SetField(pos_AS_IotaExpr_pred, OldNameInExpr (expr.GetRecord(pos_AS_IotaExpr_pred)));
  return (new_expr);
}

//
//
// Functions *NOT* in specification
//
//

// OldNameInGenericExpr
// expr : [AS`Expr]
// ==> [AS`Expr]
Generic OLD::OldNameInGenericExpr (const Generic & expr)
{
  if (expr.IsNil ())
    return (expr);
  else
    return (OldNameInExpr (expr));
}

// OldNameInMaplet
// maplet : AS`Maplet
// ==> AS`Maplet
TYPE_AS_Maplet OLD::OldNameInMaplet (const TYPE_AS_Maplet & maplet)
{
  TYPE_AS_Maplet new_maplet (maplet);
  new_maplet.SetField(pos_AS_Maplet_mapdom, OldNameInExpr (maplet.GetRecord(pos_AS_Maplet_mapdom)));
  new_maplet.SetField(pos_AS_Maplet_maprng, OldNameInExpr (maplet.GetRecord(pos_AS_Maplet_maprng)));
  return (new_maplet);
}

