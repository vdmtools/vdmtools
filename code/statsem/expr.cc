/***
*  * ID
*  *    $Id: expr.cc,v 1.72 2006/06/13 07:02:12 vdmtools Exp $
*  * SPECIFICATION VERSION
*  *    expr.cc: Implementation of expr.vdm 1.131
***/

#include "statsem.h"
#include "contextinfo.h"
#include "astaux.h"
#include "settings.h"
#include "intconvquotes.h"

//wcout << L"infer: " << INT2Q::h2gAS(infer) << endl;

#ifndef NOSS

//-- Note here that the TypeRep set in the return value is caused by
//-- field select expressions with overloaded operations/functions
//
// wf_Expr
// i : TYPE`Ind
// exp : AS`Expr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_Expr (const Int & i, const TYPE_AS_Expr & exp, const TYPE_REP_TypeRep & exptp)
{
//wcout << L"wf_Expr: " << exp.GetTag() << endl;
//wcout << INT2Q::h2gAS(exp) << endl;
  switch(exp.GetTag()) {
    case TAG_TYPE_AS_Name:                     { return wf_Name(i, exp); }
    case TAG_TYPE_AS_BinaryExpr:               { return wf_BinaryExpr(i, exp, exptp); }
    case TAG_TYPE_AS_PrefixExpr:               { return wf_PrefixExpr(i, exp, exptp); }
    case TAG_TYPE_AS_DefExpr:                  { return wf_DefExpr(i, exp, exptp); }
    case TAG_TYPE_AS_LetExpr:                  { return wf_LetExpr(i, exp, exptp); }
    case TAG_TYPE_AS_LetBeSTExpr:              { return wf_LetBeSTExpr(i, exp, exptp); }
    case TAG_TYPE_AS_IfExpr:                   { return wf_IfExpr(i, exp, exptp); }
    case TAG_TYPE_AS_CasesExpr:                { return wf_CasesExpr(i, exp, exptp); }
    case TAG_TYPE_AS_AllOrExistsExpr:          { return wf_AllOrExistsExpr(i, exp, exptp); }
    case TAG_TYPE_AS_ExistsUniqueExpr:         { return wf_ExistsUniqueExpr(i, exp, exptp); }
    case TAG_TYPE_AS_SeqEnumerationExpr:       { return wf_SeqEnumerationExpr(i, exp, exptp); }
    case TAG_TYPE_AS_SeqComprehensionExpr:     { return wf_SeqComprehensionExpr(i, exp, exptp); }
    case TAG_TYPE_AS_SubSequenceExpr:          { return wf_SubSequenceExpr(i, exp, exptp); }
    case TAG_TYPE_AS_SeqModifyMapOverrideExpr: { return wf_SeqModifyMapOverrideExpr (i, exp, exptp); }
    case TAG_TYPE_AS_SetEnumerationExpr:       { return wf_SetEnumerationExpr(i, exp, exptp); }
    case TAG_TYPE_AS_SetComprehensionExpr:     { return wf_SetComprehensionExpr(i, exp, exptp); }
    case TAG_TYPE_AS_SetRangeExpr:             { return wf_SetRangeExpr(i, exp, exptp); }
    case TAG_TYPE_AS_MapEnumerationExpr:       { return wf_MapEnumerationExpr(i, exp, exptp); }
    case TAG_TYPE_AS_MapComprehensionExpr:     { return wf_MapComprehensionExpr(i, exp, exptp); }
    case TAG_TYPE_AS_TupleConstructorExpr:     { return wf_TupleConstructorExpr(i, exp, exptp); }
    case TAG_TYPE_AS_TupleSelectExpr:          { return wf_TupleSelectExpr (i, exp ,exptp); }
    case TAG_TYPE_AS_TypeJudgementExpr:        { return wf_TypeJudgementExpr (i, exp, exptp); }
    case TAG_TYPE_AS_PreConditionApplyExpr:    { return wf_PreConditionApplyExpr (i, exp, exptp); }
    case TAG_TYPE_AS_TokenConstructorExpr:     { return wf_TokenConstructorExpr(i, exp, exptp); }
    case TAG_TYPE_AS_RecordConstructorExpr:    { return wf_RecordConstructorExpr(i, exp, exptp); }
    case TAG_TYPE_AS_RecordModifierExpr:       { return wf_RecordModifierExpr(i, exp, exptp); }
    case TAG_TYPE_AS_ApplyExpr:                { return wf_ApplyExpr(i, exp, exptp); }
    case TAG_TYPE_AS_FieldSelectExpr:          { return wf_FieldSelectExpr(i, exp, exptp); }
    case TAG_TYPE_AS_LambdaExpr:               { return wf_LambdaExpr(i, exp, exptp); }
    case TAG_TYPE_AS_IsExpr:                   { return wf_IsExpr(i, exp, exptp); }
    case TAG_TYPE_AS_NarrowExpr:               { return wf_NarrowExpr(i, exp, exptp); }
    case TAG_TYPE_AS_BoolLit:
    case TAG_TYPE_AS_RealLit:
    case TAG_TYPE_AS_NumLit:
    case TAG_TYPE_AS_CharLit:
    case TAG_TYPE_AS_TextLit:
    case TAG_TYPE_AS_QuoteLit:
    case TAG_TYPE_AS_NilLit:                   { return wf_Literal (i, exp, exptp); }
    //case TAG_TYPE_AS_Name:                     { return wf_Name(i, exp); }
    case TAG_TYPE_AS_OldName:                  { return wf_OldName(i, exp); }
    case TAG_TYPE_AS_UndefinedExpr: {
      const TYPE_CI_ContextId & cid (exp.GetInt(pos_AS_UndefinedExpr_cid));
      GetCI().SetTypeInfo (cid, exptp);
#ifdef VDMSL
      if (Settings.VDMSLmode())
      {
        //---------------------------------------------------
        // Error message #350
        // Undefined-Expr is not supported in standard VDM-SL
        //---------------------------------------------------
        GenErr (exp, ERR, 350, Sequence());
        return mk_(Bool(false), exptp);
      }
#endif // VDMSL
      return mk_(Bool(true), exptp);
    }
    case TAG_TYPE_AS_IotaExpr:                 { return wf_IotaExpr(i, exp, exptp); }
    case TAG_TYPE_AS_FctTypeInstExpr:          { return wf_FctTypeInstExpr(i, exp, exptp); }
    case TAG_TYPE_AS_BracketedExpr:            { return wf_BracketedExpr(i, exp, exptp); }
    case TAG_TYPE_AS_Macro:                    { return wf_Macro(i, exp, exptp); }
#ifdef VDMPP
    case TAG_TYPE_AS_NewExpr:                  { return wf_NewExpr (i, exp, exptp); }
    case TAG_TYPE_AS_ThreadIdExpr:             { return wf_ThreadIdExpr(i, exp, exptp); }
    case TAG_TYPE_AS_SelfExpr:                 { return wf_SelfExpr(i, exp, exptp); }
    case TAG_TYPE_AS_SameClassExpr:
    case TAG_TYPE_AS_SameBaseClassExpr:        { return wf_SameClassExpr(i,exp, exptp); }
    case TAG_TYPE_AS_IsOfClassExpr:
    case TAG_TYPE_AS_IsOfBaseClassExpr:        { return wf_IsOfClassExpr(i,exp,exptp); }
    case TAG_TYPE_AS_ActExpr:                  { return wf_ActExpr (i, exp, exptp); }
    case TAG_TYPE_AS_FinExpr:                  { return wf_FinExpr (i, exp, exptp); }
    case TAG_TYPE_AS_ActiveExpr:               { return wf_ActiveExpr (i, exp, exptp); }
    case TAG_TYPE_AS_WaitingExpr:              { return wf_WaitingExpr (i, exp, exptp); }
    case TAG_TYPE_AS_ReqExpr:                  { return wf_ReqExpr (i, exp, exptp); }
#ifdef VICE
    case TAG_TYPE_AS_CurTimeExpr:              { return wf_CurTimeExpr (i, exp, exptp); }
#endif // VICE
#endif // VDMPP
    default: {
        InternalError (L"wf_Expr");
    }
  }
  return mk_(Bool(false), rep_alltp);
}

// wf_DefExpr
// i : TYPE`Ind
// exp : AS`DefExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_DefExpr (const Int & i, const TYPE_AS_DefExpr & exp, const TYPE_REP_TypeRep & exptp)
{
  const type_dd2PL & defs (exp.GetField(pos_AS_DefExpr_Def));   // seq of (PatternBind * Expr)
  const TYPE_AS_Expr & in_e (exp.GetRecord(pos_AS_DefExpr_In));
  const TYPE_CI_ContextId & cid (exp.GetInt(pos_AS_DefExpr_cid));

  SET<TYPE_AS_Name> used;
  Bool reswf (true);

  EnterScope (MAP<TYPE_AS_Name,Tuple> ());

  size_t len_defs = defs.Length();
  for (size_t idx = 1; idx <= len_defs; idx++)
  {
    const Tuple & def (defs[idx]); // (PatternBind * Expr)
    const TYPE_AS_PatternBind & patbind (def.GetRecord (1));
    const TYPE_AS_Expr & expr (def.GetRecord (2));
 
// 20120424 -->
//    Tuple infer (wf_Expr (i, expr, rep_alltp));
    Tuple infer (wf_Expr (i, expr, PatternBind2TypeRep(patbind)));
// <-- 20120424
    const Bool & wf_expr (infer.GetBool(1));
    const TYPE_REP_TypeRep & tp (infer.GetRecord(2));

    Tuple infer2 (wf_PatternBind (i, patbind, tp));
    const Generic & wf_def (infer2.GetField(1)); // [bool]
    const MAP<TYPE_AS_Name,Tuple> & bd (infer2.GetMap(2));

    if (!(wf_def == Bool(true)))
    {
      //---------------------------------
      // Error message #111
      // Pattern in Def-Expr cannot match
      //---------------------------------
      GenErr (expr, ERR, 111, Sequence());
      reswf = Bool(false);
    }

    SET<TYPE_AS_Name> overlap (bd.Dom());
    overlap.ImpIntersect (used);
    if (!overlap.IsEmpty())
    {
      Generic nm;
      for (bool cc = overlap.First(nm); cc; cc = overlap.Next(nm))
      {
        //------------------------------
        // Error message #112
        // L"%1" is redefined in Def-Expr
        //------------------------------
        GenErr (nm, WRN1, 112, mk_sequence(PrintName (nm)));
      }
    }

    reswf = reswf && wf_expr;
    used.ImpUnion (bd.Dom ());
    UpdateScope (bd);
  }

  Tuple infer3 (wf_Expr (i, in_e, exptp));
  const Bool & wf_e (infer3.GetBool(1));
  const TYPE_REP_TypeRep & etp (infer3.GetRecord(2));

  LeaveScope();
  GetCI().SetTypeInfo(cid, etp);
  return mk_(reswf && wf_e, etp);
}

// wf_LetExpr
// i : TYPE`Ind
// exp : AS`LetExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_LetExpr (const Int & i, const TYPE_AS_LetExpr & exp, const TYPE_REP_TypeRep & exptp)
{
  const SEQ<TYPE_AS_LocalDef> & defs (exp.GetSequence(pos_AS_LetExpr_localdef));
  const TYPE_AS_Expr & in_e          (exp.GetRecord(pos_AS_LetExpr_body));
  const TYPE_CI_ContextId & cid      (exp.GetInt(pos_AS_LetExpr_cid));

  EnterScope (Map());

  // TODO: operetion check
  Bool reswf (wf_LocalDefs(i, defs));

  Tuple infer3 (wf_Expr (i, in_e, exptp));
  const Bool & wf_e (infer3.GetBool(1));
  const TYPE_REP_TypeRep & etp (infer3.GetRecord(2));

  LeaveScope();
  GetCI().SetTypeInfo(cid, etp);
  return mk_(reswf && wf_e, etp);
}

// wf_LocalDefs
// i : TYPE`Ind
// defs : seq of AS`LocalDef
// ==> bool
Bool StatSem::wf_LocalDefs(const Int & i, const SEQ<TYPE_AS_LocalDef> & defs)
{
  Tuple infer (ExtractPatterns (i, defs));
  const Bool & wf (infer.GetBool (1)); // bool // not used
  const MAP<TYPE_AS_Name, Tuple> & bd (infer.GetMap (2)); // map AS`Name to (REP`TypeRep * bool)

  //Bool reswf (true);
  Bool reswf (wf);

// 20111213 -->
  SET<TYPE_AS_Name> unused (UsedStateIds(bd.Dom ()));
  Generic stid;
  for (bool cc = unused.First (stid); cc; cc = unused.Next (stid))
    //----------------------------------------------------
    // Error message #18
    // The scope of the state component L"%1" is now hidden
    //----------------------------------------------------
    GenErr(stid, WRN1, 18, mk_sequence(PrintName(stid)));
// <-- 20111213

  Set dom_bd (bd.Dom());
  MAP<TYPE_AS_Name,Tuple> nbd;
  Generic nm;
  for (bool bb = dom_bd.First(nm); bb; bb = dom_bd.Next(nm))
    nbd.Insert(nm, mk_(bd[nm].GetRecord(1), Int(1)));

//  EnterScope (Map());

  size_t len_defs = defs.Length();
  for (size_t idx = 1; idx <= len_defs; idx++)
  {
    const TYPE_AS_LocalDef & val (defs[idx]);

    switch(val.GetTag()) {
      case TAG_TYPE_AS_ValueDef: {
        // TODO: check operation
        Tuple infer2 (wf_ValueDef (i, val));
        const Bool & wf_def (infer2.GetBool (1));
        const MAP<TYPE_AS_Name,Tuple> & realbd (infer2.GetMap (2));
        reswf = wf_def && reswf;
        UpdateScope (realbd);
        break;
      }
      case TAG_TYPE_AS_ExplFnDef: {
        const TYPE_AS_Name & nm (val.GetRecord(pos_AS_ExplFnDef_nm));
        Set nm_s (mk_set(nm, Pre(nm), Post(nm)));
        UpdateScope(nbd.DomRestrictedTo(nm_s));
        reswf = Bool(wf_ExplFunction (i, val)) && reswf;
        break;
      }
      case TAG_TYPE_AS_ImplFnDef: {
        const TYPE_AS_Name & nm (val.GetRecord(pos_AS_ImplFnDef_nm));
        Set nm_s (mk_set(nm, Pre(nm), Post(nm)));
        reswf = Bool(wf_ImplFunction (i, val)) && reswf;
        UpdateScope(nbd.DomRestrictedTo(nm_s));
        break;
      }
      case TAG_TYPE_AS_ExtExplFnDef: {
        const TYPE_AS_Name & nm (val.GetRecord(pos_AS_ExtExplFnDef_nm));
        Set nm_s (mk_set(nm, Pre(nm), Post(nm)));
        UpdateScope(nbd.DomRestrictedTo(nm_s));
        reswf = Bool(wf_ExtExplFunction (i, val)) && reswf;
        break;
      }
    }
  }
  return reswf;
}

// wf_LetBeSTExpr
// i : TYPE`Ind
// exp : AS`LetBeSTExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_LetBeSTExpr (const Int & i, const TYPE_AS_LetBeSTExpr & exp, const TYPE_REP_TypeRep & exptp)
{
  const SEQ<TYPE_AS_MultBind> & lhs (exp.GetSequence(pos_AS_LetBeSTExpr_lhs));
  const Generic & st_e          (exp.GetField(pos_AS_LetBeSTExpr_St));
  const TYPE_AS_Expr & in_e     (exp.GetRecord(pos_AS_LetBeSTExpr_In));
  const TYPE_CI_ContextId & cid (exp.GetInt(pos_AS_LetBeSTExpr_cid));

// 20130729 -->
  PushVDContext(cid);
// <-- 20130729
  Tuple infer (wf_MultiBindList (i, lhs, true)); // [bool] * map AS`Name to REP`TypeRep
// 20130729 -->
  PopVDContext();
// <-- 20130729
  const Generic & wf_sb (infer.GetField(1));
  const MAP<TYPE_AS_Name,Tuple> & bd (infer.GetMap(2));
  Bool reswf (wf_sb == Bool(true));
  if (wf_sb != Bool(true))
  {
    //------------------------------------
    // Error message #113
    // Pattern in Let-Be-Expr cannot match
    //------------------------------------
    GenErr (exp, ERR, 113, Sequence());
  }

  EnterScope(bd);

  if (!st_e.IsNil()) {
    Tuple infer2 (wf_Expr (i, st_e, btp_bool));
    const Bool & wf_st (infer2.GetBool(1));
    const TYPE_REP_TypeRep & sttp (infer2.GetRecord(2));
    bool stcomp = IsCompatible (i, sttp, btp_bool);
    if (!stcomp)
    {
      //-----------------------------------------------
      // Error message #231
      // Predicate for L"%1" is not a boolean expression
      //-----------------------------------------------
      GenErrTp (st_e, ERR, 231, sttp, btp_bool, mk_sequence(SEQ<Char>(L"Let-Be-Stmt")));
    }
    reswf = reswf && wf_st && stcomp;
  }

  Tuple infer3 (wf_Expr (i, in_e, exptp));
  const Bool & wf_e (infer3.GetBool(1));
  const TYPE_REP_TypeRep & etp (infer3.GetRecord(2));
  reswf = reswf && wf_e;

  LeaveScope();

  GetCI().SetTypeInfo(cid, etp);
  return mk_(reswf, etp);
}

// wf_IfExpr
// i : TYPE`Ind
// exp : AS`IfExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_IfExpr (const Int & i, const TYPE_AS_IfExpr & exp, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & test             (exp.GetRecord(pos_AS_IfExpr_test));
  const TYPE_AS_Expr & cons             (exp.GetRecord(pos_AS_IfExpr_cons));
  const SEQ<TYPE_AS_ElseifExpr> & elsif (exp.GetSequence(pos_AS_IfExpr_elsif));
  const TYPE_AS_Expr & altn             (exp.GetRecord(pos_AS_IfExpr_altn));
  const TYPE_CI_ContextId & cid         (exp.GetInt(pos_AS_IfExpr_cid));

// 20141102 -->
  PushVDContext(cid);
// <-- 20141102

  // if
  Tuple infer (wf_Expr (i, test, btp_bool));
  const Bool & wf_test (infer.GetBool(1));
  const TYPE_REP_TypeRep & test_tp (infer.GetRecord(2));
  Bool tcomp (IsCompatible (i, test_tp, btp_bool));

  if (!tcomp.GetValue())
  {
    //-----------------------------------------------
    // Error message #115
    // Test expression in L"%1" is not of boolean type
    //-----------------------------------------------
    GenErrTp (test, ERR, 115, test_tp, btp_bool, mk_sequence(SEQ<Char>(L"If-Expr")));
  }

  Bool reswf (wf_test && tcomp);
  TYPE_REP_TypeRep m_tp;

  // then
  Tuple infer2 (wf_Expr (i, cons, exptp));
  const Bool & wf_cons (infer2.GetBool(1));
  const TYPE_REP_TypeRep & cons_tp (infer2.GetRecord(2));
  Bool ccomp (IsCompatible (POS, cons_tp, exptp));

  if (!ccomp.GetValue())
  {
    //---------------------------------
    // Error message #116
    // then-part is not of correct type
    //---------------------------------
    GenErrTp (cons, ERR, 116, cons_tp, exptp, Sequence());
  }
  reswf = reswf && wf_cons && ccomp;
  m_tp = cons_tp;

  // elseif
  if (!elsif.IsEmpty()) {
    Tuple infer3 (wf_ElseIfExprs (i, elsif, exptp));
    const Bool & wf_e (infer3.GetBool(1));
    const Generic & e_tp (infer3.GetField(2));
    reswf = reswf && wf_e;
    m_tp = MergeTypes(m_tp, e_tp);
  }

  // else
  Tuple infer4 (wf_Expr (i, altn, exptp));
  const Bool & wf_altn (infer4.GetBool(1));
  const TYPE_REP_TypeRep & altn_tp (infer4.GetRecord(2));
  Bool ecomp (IsCompatible (POS, altn_tp, exptp));

  if (!ecomp.GetValue())
  {
    //---------------------------------
    // Error message #117
    // else-part is not of correct type
    //---------------------------------
    GenErrTp (altn, ERR, 117, altn_tp, exptp, Sequence());
  }
  reswf = reswf && wf_altn && ecomp;
  m_tp = MergeTypes(m_tp, altn_tp);

// 20141102 -->
  PopVDContext();
// <-- 20141102

//  TYPE_REP_TypeRep f_tp (e_tp.IsNil() ? cons_tp : MergeTypes (cons_tp, e_tp));
//
//  TYPE_REP_TypeRep m_tp (MergeTypes(f_tp, altn_tp));
//  GetCI().SetTypeInfo (cid, m_tp);
//
//  return mk_(wf_test && wf_cons && wf_e && wf_altn && tcomp && ccomp && ecomp, m_tp);

  GetCI().SetTypeInfo (cid, m_tp);
  return mk_(reswf, m_tp);
}

// wf_ElseIfExprs
// i : TYPE`Ind
// else_l : seq1 of AS`ElseifExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_ElseIfExprs (const Int & i,
                                    const SEQ<TYPE_AS_ElseifExpr> & else_l,
                                    const TYPE_REP_TypeRep & exptp)
{
  Tuple infer (wf_ElseIfExpr (i, else_l.Hd(), exptp));

  if (else_l.Length() == 1)
    return infer;
  else
  {
    const Bool & wf_e (infer.GetBool(1));
    const TYPE_REP_TypeRep & etp (infer.GetRecord(2));

    Tuple infer2 (wf_ElseIfExprs (i, else_l.Tl(), exptp));
    const Bool & wf_rest (infer2.GetBool(1));
    const TYPE_REP_TypeRep & restp (infer2.GetRecord(2));

    return mk_(Bool(wf_rest && wf_e), MergeTypes (etp, restp));
  }
}

// wf_ElseIfExpr
// i : TYPE`Ind
// exp : AS`ElseifExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_ElseIfExpr (const Int & i, const TYPE_AS_ElseifExpr & exp, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & test (exp.GetRecord(pos_AS_ElseifExpr_test));
  const TYPE_AS_Expr & cons (exp.GetRecord(pos_AS_ElseifExpr_cons));

  Tuple infer (wf_Expr (i,test,btp_bool));
  const Bool & wf_t (infer.GetBool(1));
  const TYPE_REP_TypeRep & t_tp (infer.GetRecord(2));
  Bool ecomp (IsCompatible (i, t_tp, btp_bool));

  if (!ecomp.GetValue())
  {
    //-----------------------------------------------
    // Error message #115
    // Test expression in L"%1" is not of boolean type
    //-----------------------------------------------
    GenErrTp (test, ERR,115,t_tp, btp_bool, mk_sequence(SEQ<Char>(L"Else-If-Expr")));
  }

  Tuple infer2 (wf_Expr (i,cons,exptp));
  const Bool & wf_c (infer2.GetBool(1));
  const TYPE_REP_TypeRep & c_tp (infer2.GetRecord(2));
  Bool ccomp (IsCompatible (POS, c_tp, exptp));

  if (!ccomp.GetValue())
  {
    //-----------------------------------
    // Error message #118
    // elseif-part is not of correct type
    //-----------------------------------
    GenErrTp (cons, ERR,118,c_tp, exptp, Sequence());
  }

  return mk_(wf_t && wf_c && ecomp, c_tp);
}

// wf_CasesExpr
// i : TYPE`Ind
// exp : AS`CasesExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_CasesExpr (const Int & i, const TYPE_AS_CasesExpr & exp, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & sel            (exp.GetRecord(pos_AS_CasesExpr_sel));
  const SEQ<TYPE_AS_CaseAltn> & altns (exp.GetSequence(pos_AS_CasesExpr_altns));
  const Generic & oth                 (exp.GetField(pos_AS_CasesExpr_Others));
  const TYPE_CI_ContextId & cid       (exp.GetInt(pos_AS_CasesExpr_cid));

// 20141102 -->
  PushVDContext(cid);
// <-- 20141102

  Tuple infer (wf_Expr(i, sel, rep_alltp));
  const Bool & wf_sel (infer.GetBool(1));
  const TYPE_REP_TypeRep & seltp (infer.GetRecord(2));

  Bool reswf (true);
  Generic restp;
  restp = Nil();

  size_t len_altns = altns.Length();
  for (size_t idx = 1; idx <= len_altns; idx++)
  {
    const TYPE_AS_CaseAltn & altn (altns[idx]);
    const SEQ<TYPE_AS_Pattern> & m (altn.GetSequence(pos_AS_CaseAltn_match));
    const TYPE_AS_Expr & b         (altn.GetRecord(pos_AS_CaseAltn_body));

    Tuple infer2 (wf_CaseAltn (i, m, b, seltp, exptp));
    const Bool & wf    (infer2.GetBool(1));
    const Generic & tp (infer2.GetField(2));

    reswf = Bool(reswf && wf);

    if (restp.IsNil())
      restp = tp;
    else if (!tp.IsNil())
      restp = MergeTypes (restp, tp);
  }

  if (!oth.IsNil())
  {
    Tuple infer3 (wf_Expr (i, oth, exptp));
    const Bool & wf (infer3.GetBool(1));
    const TYPE_REP_TypeRep & tp (infer3.GetRecord(2));

    Bool ocomp (IsCompatible(POS, tp, exptp));
    if (!ocomp.GetValue())
    {
      //-------------------------------------
      // Error message #119
      // others branch is not of correct type
      //-------------------------------------
      GenErrTp(oth, ERR, 119, tp, exptp, Sequence());
      reswf = false;
    }

    reswf = Bool(reswf && wf);
    restp = (restp.IsNil() ? tp : MergeTypes (restp, tp));
  }

// 20141102 -->
  PopVDContext();
// <-- 20141102

  if (restp.IsNil())
    restp = rep_alltp;

  GetCI().SetTypeInfo(cid, restp);
  return mk_(Bool(wf_sel && reswf), restp);
}

// wf_CaseAltn
// i : TYPE`Ind
// m_l : seq1 of AS`Pattern
// b : AS`Expr
// tp : REP`TypeRep
// exptp : REP`TypeRep
// ==> [bool] * [REP`TypeRep]
Tuple StatSem::wf_CaseAltn (const Int & i,
                            const SEQ<TYPE_AS_Pattern> & m_l,
                            const TYPE_AS_Expr & b,
                            const TYPE_REP_TypeRep & tp,
                            const TYPE_REP_TypeRep & exptp)
{
  MAP<TYPE_AS_Name,Tuple> resbd;
  bool reswf = true;
  bool onewf = false;
  Bool bo (true);

  size_t len_m_l = m_l.Length();
  for (size_t idx = 1; idx <= len_m_l; idx++)
  {
    Tuple infer (wf_Pattern(i, m_l[idx], tp));
    const Generic & wf_p (infer.GetField(1)); // [bool]
    const MAP<TYPE_AS_Name,Tuple> & bd (infer.GetMap(2));

    bool j = (wf_p == bo);
    onewf = onewf || j;
    reswf = reswf && j;

    SET<TYPE_AS_Name> dom_resbd (resbd.Dom ());
    dom_resbd.ImpIntersect (bd.Dom ());
    MAP<TYPE_AS_Name,Tuple> tmp_m;
    Generic id;
    for (bool bb = dom_resbd.First (id); bb; bb = dom_resbd.Next (id))
      tmp_m.Insert (id, mk_(MergeTypes (resbd[id].GetRecord(1), bd[id].GetRecord(1)),
                                        resbd[id].GetInt(2) + bd[id].GetInt(2)));

    resbd.ImpOverride (bd);
    resbd.ImpOverride (tmp_m);

// 20130730 -->
    EnterScope(bd);
    Tuple infer2 (wf_Expr (i, b, exptp));
    const Bool & wf_s (infer2.GetBool(1));
    reswf = reswf && wf_s;
    LeaveScope();
// <-- 20130730
  }

  EnterScope(resbd);

  Tuple infer (wf_Expr (i, b, exptp));
  const Bool & wf_s (infer.GetBool(1));
  const TYPE_REP_TypeRep & stp (infer.GetRecord(2));

  LeaveScope();

  if (!IsCompatible(POS, stp, exptp))
  {
    //----------------------------------------
    // Error message #120
    // Case alternative is not of correct type
    //----------------------------------------
    GenErrTp (b, ERR, 120, stp, exptp, Sequence());
    return mk_(Bool(false), exptp);
  }

  if (onewf)
    return mk_(wf_s && reswf, stp);
  else
  {
    if (i == POS)
    {
      //------------------------
      // Error message #121
      // Pattern can never match
      //------------------------
      GenErr (m_l[1], ERR, 121, Sequence());
    }

// 20100805 -->
    //return mk_(Bool (false), Nil());
    if ((i == DEF ) && IsCompatible(i, stp, exptp))
      return mk_(Bool (false), stp);
    else
      return mk_(Bool (false), Nil());
// <-- 20100805
  }
}

// wf_AllOrExistsExpr
// i : TYPE`Ind
// vAllOrExistsExpr : AS`AllOrExistsExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_AllOrExistsExpr (const Int & i,
                                   const TYPE_AS_AllOrExistsExpr & vAllOrExistsExpr,
                                   const TYPE_REP_TypeRep &)
{
  const SEQ<TYPE_AS_MultBind> & bind (vAllOrExistsExpr.GetSequence(pos_AS_AllOrExistsExpr_bind));
  const TYPE_AS_Expr & pred (vAllOrExistsExpr.GetRecord(pos_AS_AllOrExistsExpr_pred));
  const TYPE_CI_ContextId & cid (vAllOrExistsExpr.GetInt(pos_AS_AllOrExistsExpr_cid));

  Tuple infer (wf_MultiBindList (i, bind, false));
  const Generic & wf_smb (infer.GetField(1));
  const MAP<TYPE_AS_Name,Tuple> & bd (infer.GetMap(2));

  if (wf_smb.IsNil())
  {
    //------------------------------
    // Error message #229
    // Binding for L"%1" is erroneous
    //------------------------------
    GenErr (vAllOrExistsExpr, ERR, 229, mk_sequence(SEQ<Char>(L"Quant-Expr")));
//    GetCI().SetTypeInfo (cid, btp_bool);
//    GetCI().SetPidM (cid, bd);
//    return mk_(Bool(false), btp_bool);
  }
  else if (wf_smb == Bool(false))
  {
    //---------------------------------
    // Error message #230
    // Not all bindings are well-formed
    //---------------------------------
    GenErr (vAllOrExistsExpr, ERR, 230, Sequence());
  }

  EnterScope (bd);

  Tuple infer2 (wf_Expr(i, pred, btp_bool));
  const Bool & wf_p (infer2.GetBool(1));
  const TYPE_REP_TypeRep & etp (infer2.GetRecord(2));

  bool predcomp (IsCompatible (i, etp, btp_bool));

  LeaveScope();

  if (!predcomp)
  {
    //-----------------------------------------------
    // Error message #231
    // Predicate for L"%1" is not a boolean expression
    //-----------------------------------------------
    GenErrTp (vAllOrExistsExpr, ERR, 231, etp, btp_bool, mk_sequence(SEQ<Char>(L"Quant-Expr")));
  }

  GetCI().SetTypeInfo (cid, btp_bool);
  //GetCI().SetPidM (cid, bd);
  Generic nm;
  MAP<TYPE_AS_Name, TYPE_REP_TypeRep> bdm;
  SET<TYPE_AS_Name> dom_bd (bd.Dom());
  for (bool bb = dom_bd.First(nm); bb; bb = dom_bd.Next(nm))
    bdm.Insert(nm, bd[nm].GetRecord(1));
  GetCI().SetPidM (cid, bdm);
  return mk_(Bool(wf_smb == Bool(true)) && wf_p && predcomp, btp_bool);
}

// wf_ExistsUniqueExpr
// i : TYPE`Ind
// vExistsUniqueExpr : AS`ExistsUniqueExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_ExistsUniqueExpr (const Int & i,
                                    const TYPE_AS_ExistsUniqueExpr & vExistsUniqueExpr,
                                    const TYPE_REP_TypeRep &)
{
  const TYPE_AS_Bind & bind (vExistsUniqueExpr.GetRecord(pos_AS_ExistsUniqueExpr_bind));
  const TYPE_AS_Expr & pred (vExistsUniqueExpr.GetRecord(pos_AS_ExistsUniqueExpr_pred));
  const TYPE_CI_ContextId & cid (vExistsUniqueExpr.GetInt(pos_AS_ExistsUniqueExpr_cid));

  Tuple infer (wf_Bind(i, bind));
  const Generic & wf_bd (infer.GetField(1));
  const MAP<TYPE_AS_Name,Tuple> & bd (infer.GetMap(2));

  if (wf_bd.IsNil())
  {
    //------------------------------
    // Error message #229
    // Binding for L"%1" is erroneous
    //------------------------------
    GenErr (vExistsUniqueExpr, ERR, 229, mk_sequence(SEQ<Char>(L"Exist-Unique-Expr")));
//    GetCI().SetTypeInfo (cid, btp_bool);
//    GetCI().SetPidM (cid, bd);
//    return mk_(Bool(false), btp_bool);
  }
  else if (wf_bd == Bool(false))
  {
    //---------------------------------
    // Error message #230
    // Not all bindings are well-formed
    //---------------------------------
    GenErr (vExistsUniqueExpr, ERR, 230, Sequence());
  }

  EnterScope (bd);

  Tuple infer2 (wf_Expr(i, pred, btp_bool));
  const Bool & wf_e (infer2.GetBool(1));
  const TYPE_REP_TypeRep & etp (infer2.GetRecord(2));

  bool stcomp (IsCompatible (i, etp, btp_bool));

  LeaveScope();

  if (!stcomp)
  {
    //-----------------------------------------------
    // Error message #231
    // Predicate for L"%1" is not a boolean expression
    //-----------------------------------------------
    GenErrTp (vExistsUniqueExpr, ERR, 231, etp, btp_bool, mk_sequence(SEQ<Char>(L"Exist-Unique-Expr")));
  }

  GetCI().SetTypeInfo (cid, btp_bool);
  
  Generic nm;
  MAP<TYPE_AS_Name, TYPE_REP_TypeRep> bdm;
  SET<TYPE_AS_Name> dom_bd (bd.Dom());
  for (bool bb = dom_bd.First(nm); bb; bb = dom_bd.Next(nm))
    bdm.Insert(nm, bd[nm].GetRecord(1));
  GetCI().SetPidM (cid, bdm);
  return mk_(Bool(wf_bd == Bool(true)) && wf_e && stcomp, btp_bool);
}

// wf_IotaExpr
// i : TYPE`Ind
// vIotaExpr : AS`IotaExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_IotaExpr (const Int & i, const TYPE_AS_IotaExpr & vIotaExpr, const TYPE_REP_TypeRep &)
{
  const TYPE_AS_Bind & bind (vIotaExpr.GetRecord(pos_AS_IotaExpr_bind));
  const TYPE_AS_Expr & pred (vIotaExpr.GetRecord(pos_AS_IotaExpr_pred));
  const TYPE_CI_ContextId & cid (vIotaExpr.GetInt(pos_AS_IotaExpr_cid));

  Tuple infer (wf_BindAndType (i, bind));
  const Generic & wf_bd (infer.GetField (1)); // [bool]
  const MAP<TYPE_AS_Name,Tuple> & bd (infer.GetMap (2));
  const TYPE_REP_TypeRep & restp (infer.GetRecord (3));

  GetCI().SetTypeInfo (cid, restp);

  if (wf_bd.IsNil()) // type error
  {
    //------------------------------
    // Error message #229
    // Binding for L"%1" is erroneous
    //------------------------------
    GenErr (vIotaExpr, ERR, 229, mk_sequence(SEQ<Char>(L"Iota-Expr")));
//    return mk_(Bool(false), restp);
  }
  else if (wf_bd == Bool(false)) // never match
  {
    //------------------------------------------------
    // Error message #232
    // No binding is possible for this iota expression
    //------------------------------------------------
    GenErr (vIotaExpr, ERR, 232, Sequence());
//    return mk_(Bool(false), restp);
  }
//  else // can match
  {
    EnterScope (bd);

    Tuple infer2 (wf_Expr(i, pred, btp_bool));
    const Bool & wf_e (infer2.GetBool(1));
    const TYPE_REP_TypeRep & etp (infer2.GetRecord(2));

    bool stcomp (IsCompatible (i, etp, btp_bool));

    LeaveScope();

    if (!stcomp)
    {
      //-----------------------------------------------
      // Error message #231
      // Predicate for L"%1" is not a boolean expression
      //-----------------------------------------------
      GenErrTp (vIotaExpr, ERR, 231, etp, btp_bool, mk_sequence(SEQ<Char>(L"Iota-Expr")));
    }

    if ((i == DEF) && (Settings.ErrorLevel() >= ERR2))
    {
      //--------------------------------------------------------------------
      // Error message #233
      // More than one value can satisfy the predicate in the iota
      // expression
      //--------------------------------------------------------------------
      GenErr (vIotaExpr, ERR2, 233, Sequence());
      return mk_(Bool(false), restp);
    }
    else
      return mk_(Bool(wf_bd == Bool(true)) && wf_e && stcomp, restp);
  }
}

// wf_SetEnumerationExpr
// i : TYPE`Ind
// SetNum : AS`SetEnumerationExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_SetEnumerationExpr (const Int & i,
                                      const TYPE_AS_SetEnumerationExpr & SetEnum,
                                      const TYPE_REP_TypeRep & exptp)
{
  const SEQ<TYPE_AS_Expr> & els (SetEnum.GetSequence(pos_AS_SetEnumerationExpr_els));
  const TYPE_CI_ContextId & cid (SetEnum.GetInt(pos_AS_SetEnumerationExpr_cid));

  if (els.IsEmpty())
  {
    GetCI().SetTypeInfo (cid, mk_REP_EmptySetTypeRep(rep_alltp));
    return mk_(Bool(true), mk_REP_EmptySetTypeRep(rep_alltp));
  }
  else
  {
    TYPE_REP_TypeRep expelemtp (rep_alltp);
    if (IsCompatible (POS, set_alltp, exptp))
    {
      Generic ust (UnmaskSetType(POS, ExtractSetType(exptp)));
      if (!ust.IsNil ())
        expelemtp = ust;
    }

    SEQ<TYPE_REP_TypeRep> restp_l; // 20101019
    Bool reswf (true);
    Generic restp = Nil();
    size_t len_els = els.Length();
    for (size_t idx = 1; idx <= len_els; idx++)
    {
      Tuple infer (wf_Expr(i, els[idx], expelemtp));
      const Bool & wf (infer.GetBool(1));
      const TYPE_REP_TypeRep & tp (infer.GetRecord(2));

      reswf = reswf && wf;
      restp = (restp.IsNil() ? tp : MergeTypes (restp, tp));
      restp_l.ImpAppend(tp); // 20101019
    }

// 20101019 -->
    if ((i == POS) && IsCompatible(i, restp, expelemtp))
    {
      for (size_t idx = 1; idx <= len_els; idx++)
      {
        if (!IsCompatible(i, restp_l[idx], expelemtp))
        {
          // -------------------------------
          // -- Error message #397
          // -- Value is not type-compatible
          // -------------------------------
          GenErrTp (els[idx], ERR, 397, restp_l[idx], expelemtp, Sequence());
        }
      }
    }
// <-- 20101019

    GetCI().SetTypeInfo (cid, mk_REP_SetTypeRep( restp ));
    return mk_(reswf, mk_REP_SetTypeRep( restp ));
  }
}

// wf_SetComprehensionExpr
// i : TYPE`Ind
// vSetComprehension : AS`SetComprehensionExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_SetComprehensionExpr (const Int & i,
                                        const TYPE_AS_SetComprehensionExpr & vSetComprehension,
                                        const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & elem (vSetComprehension.GetRecord(pos_AS_SetComprehensionExpr_elem));
  const SEQ<TYPE_AS_MultBind> bind (vSetComprehension.GetSequence(pos_AS_SetComprehensionExpr_bind));
  const Generic & pred (vSetComprehension.GetField(pos_AS_SetComprehensionExpr_pred));
  const TYPE_CI_ContextId & cid (vSetComprehension.GetInt(pos_AS_SetComprehensionExpr_cid));

  Tuple infer (wf_MultiBindList (i, bind, false));
  const Generic & wf_bd (infer.GetField(1));
  const MAP<TYPE_AS_Name,Tuple> & bd (infer.GetMap(2));

  TYPE_REP_TypeRep expelemtp (rep_alltp);
  if (IsCompatible(POS, set_alltp, exptp))
  {
    Generic ust (UnmaskSetType (POS, ExtractSetType(exptp)));
    if (!ust.IsNil ())
      expelemtp = ust;
  }

  Generic nm;
  MAP<TYPE_AS_Name, TYPE_REP_TypeRep> bdm;
  SET<TYPE_AS_Name> dom_bd (bd.Dom());
  for (bool bb = dom_bd.First(nm); bb; bb = dom_bd.Next(nm))
    bdm.Insert(nm, bd[nm].GetRecord(1));
  GetCI().SetPidM (cid, bdm);

  if (wf_bd.IsNil())
  {
    //------------------------------
    // Error message #229
    // Binding for L"%1" is erroneous
    //------------------------------
    GenErr (vSetComprehension, ERR, 229, mk_sequence(SEQ<Char>(L"Set-Comprh-Expr")));
    GetCI().SetTypeInfo (cid, mk_REP_SetTypeRep(rep_alltp));
    return mk_(Bool(false), mk_REP_SetTypeRep(rep_alltp));
  }

  if (wf_bd == Bool(false))
  {
    //---------------------------------
    // Error message #230
    // Not all bindings are well-formed
    //---------------------------------
    GenErr (vSetComprehension, ERR, 230, Sequence());
  }

  Bool reswf (wf_bd == Bool(true));

  EnterScope (bd);

  if (!pred.IsNil()) {
    Tuple infer2 (wf_Expr(i, pred, btp_bool));
    const Bool & wf_pr (infer2.GetBool(1));
    const TYPE_REP_TypeRep & pr_tp (infer2.GetRecord(2));

    bool pcomp = IsCompatible (i, pr_tp, btp_bool);

    if (!pcomp)
    {
      //-----------------------------------------------
      // Error message #231
      // Predicate for L"%1" is not a boolean expression
      //-----------------------------------------------
      GenErrTp (pred, ERR, 231, pr_tp, btp_bool, mk_sequence(SEQ<Char>(L"Set-Comprh-Expr")));
    }
    reswf = reswf && wf_pr && pcomp;
  }

  Tuple infer3 (wf_Expr(i, elem, expelemtp));
  const Bool & wf_e (infer3.GetBool(1));
  const TYPE_REP_TypeRep & e_tp (infer3.GetRecord(2));
  reswf = reswf && wf_e;

  LeaveScope();

  TYPE_REP_TypeRep utr (mk_REP_UnionTypeRep(mk_set(mk_REP_EmptySetTypeRep(e_tp), mk_REP_SetTypeRep(e_tp))));
  GetCI().SetTypeInfo (cid, utr);
  return mk_(reswf, utr);
}

// wf_SetRangeExpr
// i : TYPE`Ind
// vSetRange : AS`SetRangeExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_SetRangeExpr (const Int & i, const TYPE_AS_SetRangeExpr & vSetRange, const TYPE_REP_TypeRep &)
{
  const TYPE_AS_Expr & lb (vSetRange.GetRecord(pos_AS_SetRangeExpr_lb));
  const TYPE_AS_Expr & ub (vSetRange.GetRecord(pos_AS_SetRangeExpr_ub));
  const TYPE_CI_ContextId & cid (vSetRange.GetInt(pos_AS_SetRangeExpr_cid));

  Tuple infer (wf_Expr (i,lb, btp_real));
  const Bool & wf_lb (infer.GetBool(1));
  const TYPE_REP_TypeRep & lb_tp (infer.GetRecord(2));

  Tuple infer2 (wf_Expr (i,ub, btp_real));
  const Bool & wf_ub (infer2.GetBool(1));
  const TYPE_REP_TypeRep & ub_tp (infer2.GetRecord(2));

  bool lcomp (IsCompatible (i, lb_tp, btp_real));
  bool ucomp (IsCompatible (i, ub_tp, btp_real));

  if (!lcomp)
  {
    //--------------------------------------------------
    // Error message #234
    // Lower index of Set-Range-Expr is not of type real
    //--------------------------------------------------
    GenErrTp (lb, ERR, 234, lb_tp, btp_real, Sequence());
  }

  if (!ucomp)
  {
    //--------------------------------------------------
    // Error message #235
    // Upper index of Set-Range-Expr is not of type real
    //--------------------------------------------------
    GenErrTp (ub, ERR, 235, ub_tp, btp_real, Sequence());
  }

  TYPE_REP_TypeRep str;
  if (lcomp && ucomp) {
    TYPE_REP_TypeRep numtp (ExtractNumericType(lb_tp));
    TYPE_REP_TypeRep restp;
    switch (numtp.GetTag()) {
      case TAG_TYPE_REP_NumericTypeRep: {
        switch(numtp.GetIntValue(pos_REP_NumericTypeRep_qtp)) {
          case NATONE: { restp = btp_natone; break; }
          case NAT:    { restp = btp_nat;    break; }
          default:     { restp = btp_int;    break; }
        }
        break;
      }
      default: {
        restp = btp_int;
        break;
      }
    }
    str = mk_REP_SetTypeRep(restp);
  }
  else {
    str = rep_alltp;
  }

  GetCI().SetTypeInfo (cid, str);
  return mk_(wf_lb && wf_ub && lcomp && ucomp, str);
}

// wf_SeqEnumerationExpr
// i : TYPE`Ind
// SeqEnum : AS`SeqEnumerationExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRe
Tuple StatSem::wf_SeqEnumerationExpr (const Int & i,
                                      const TYPE_AS_SeqEnumerationExpr & SeqEnum,
                                      const TYPE_REP_TypeRep & exptp)
{
  const SEQ<TYPE_AS_Expr> & els (SeqEnum.GetSequence(pos_AS_SeqEnumerationExpr_els));
  const TYPE_CI_ContextId & cid (SeqEnum.GetInt(pos_AS_SeqEnumerationExpr_cid));

  if (els.IsEmpty())
  {
    TYPE_REP_TypeRep restp (mk_REP_EmptySeqTypeRep(rep_alltp));
    GetCI().SetTypeInfo (cid, restp);
    return mk_(Bool(true), restp);
  }
  else
  {
    TYPE_REP_TypeRep expelemtp (rep_alltp);
    if (IsCompatible(POS, seq_alltp, exptp))
      expelemtp = UnmaskSeqType (POS, ExtractSeqType (exptp));

    SEQ<TYPE_REP_TypeRep> restp_l; // 20101019
    Bool reswf (true);
    Generic restp = Nil();
    size_t len_els = els.Length();
    for (size_t idx = 1; idx <= len_els; idx++)
    {
      Tuple infer (wf_Expr(i, els[idx], expelemtp));
      const Bool & wf (infer.GetBool(1));
      const TYPE_REP_TypeRep & tp (infer.GetRecord(2));

      reswf = reswf && wf;
      restp = (restp.IsNil() ? tp : MergeTypes (restp, tp));
      restp_l.ImpAppend(tp); // 20101019
    }
// 20101019 -->
    if ((i == POS) && IsCompatible(i, restp, expelemtp))
    {
      for (size_t idx = 1; idx <= len_els; idx++)
      {
        if (!IsCompatible(i, restp_l[idx], expelemtp))
        {
          // -------------------------------
          // -- Error message #397
          // -- Value is not type-compatible
          // -------------------------------
          GenErrTp (els[idx], ERR, 397, restp_l[idx], expelemtp, Sequence());
        }
      }
    }
// <-- 20101019
    TYPE_REP_TypeRep str (mk_REP_SeqTypeRep(restp));
    GetCI().SetTypeInfo (cid, str);
    return mk_(reswf, str);
  }
}

// wf_SeqComprehensionExpr
// i : TYPE`Ind
// vSeqComprehension : AS`SeqComprehensionExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_SeqComprehensionExpr (const Int & i,
                                        const TYPE_AS_SeqComprehensionExpr & vSeqComprehension,
                                        const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & elem (vSeqComprehension.GetRecord(pos_AS_SeqComprehensionExpr_elem));
  const TYPE_AS_SetBind & bind (vSeqComprehension.GetRecord(pos_AS_SeqComprehensionExpr_bind));
  const Generic & pred (vSeqComprehension.GetField(pos_AS_SeqComprehensionExpr_pred));
  const TYPE_CI_ContextId & cid (vSeqComprehension.GetInt(pos_AS_SeqComprehensionExpr_cid));

//  Tuple infer (wf_Bind (i, bind));
//  const Generic & wf_bd (infer.GetField(1));
//  const MAP<TYPE_AS_Name,Tuple> & bd (infer.GetMap(2));
  Generic wf_bd;
  MAP<TYPE_AS_Name,Tuple> bd;
  switch (bind.GetTag()) {
    case TAG_TYPE_AS_SetBind: {
      Tuple infer (wf_SetBind (i, bind));
      wf_bd = (infer.GetField(1));
      bd.ImpOverride(infer.GetMap(2));
      break;
    }
    case TAG_TYPE_AS_SeqBind: {
      Tuple infer (wf_SeqBind (i, bind));
      wf_bd = (infer.GetField(1));
      bd.ImpOverride(infer.GetMap(2));
      break;
    }
  }

  TYPE_REP_TypeRep expelemtp (rep_alltp);
  if (IsCompatible(POS, seq_alltp, exptp))
    expelemtp = UnmaskSeqType (POS, ExtractSeqType (exptp));

  //GetCI().SetPidM (cid, bd);
  Generic nm;
  MAP<TYPE_AS_Name, TYPE_REP_TypeRep> bdm;
  SET<TYPE_AS_Name> dom_bd (bd.Dom());
  for (bool bb = dom_bd.First(nm); bb; bb = dom_bd.Next(nm))
    bdm.Insert(nm, bd[nm].GetRecord(1));
  GetCI().SetPidM (cid, bdm);

  SET<Tuple> rng_bd (bd.Rng());
  bool only_num = true;
  Generic g;
  for (bool bb = rng_bd.First(g); bb && only_num; bb = rng_bd.Next(g)) {
    Tuple t(g);
    only_num = IsCompatible (i, t.GetRecord(1), btp_real);
  }

// 20110428 -->
  //bool only_one = (bd.Dom().Card() == 1);
  bool only_one = (bd.Dom().Card() <= 1);
// <-- 20110428

  if ((wf_bd == Bool(true)) && (!only_num || !only_one) && bind.Is(TAG_TYPE_AS_SetBind)) {
    //------------------------------------------------------------------------
    // Error message #236
    // Binding in Seq-Comprh-Expr uses non-numeric values or multiple bindings
    //------------------------------------------------------------------------
    GenErr (vSeqComprehension, ERR, 236, Sequence());
  }

  if (wf_bd.IsNil()) {
    //------------------------------
    // Error message #229
    // Binding for L"%1" is erroneous
    //------------------------------
    GenErr (vSeqComprehension, ERR, 229, mk_sequence(SEQ<Char>(L"Seq-Comprh-Expr")));
    TYPE_REP_TypeRep str (mk_REP_SeqTypeRep(rep_alltp));
    GetCI().SetTypeInfo (cid, str);
    return mk_(Bool(false), str);
  }

  if (wf_bd == Bool(false))
  {
    //---------------------------------
    // Error message #230
    // Not all bindings are well-formed
    //---------------------------------
    GenErr (vSeqComprehension, ERR, 230, Sequence());
  }

  Bool reswf ((wf_bd == Bool(true)) && only_num && only_one);

  EnterScope (bd);

  if (!pred.IsNil()) {
    Tuple infer2 (wf_Expr(i, pred, btp_bool));
    const Bool & wf_pr (infer2.GetBool(1));
    const TYPE_REP_TypeRep & pr_tp (infer2.GetRecord(2));
    bool pcomp = IsCompatible (i, pr_tp, btp_bool);

    if (!pcomp)
    {
      //-----------------------------------------------
      // Error message #231
      // Predicate for L"%1" is not a boolean expression
      //-----------------------------------------------
      GenErrTp (pred, ERR, 231, pr_tp, btp_bool, mk_sequence(SEQ<Char>(L"Seq-Comprh-Expr")));
    }
    reswf = reswf && wf_pr && pcomp;
  }

  Tuple infer3 (wf_Expr(i, elem, expelemtp));
  const Bool & wf_e (infer3.GetBool(1));
  const TYPE_REP_TypeRep & e_tp (infer3.GetRecord(2));
  reswf = reswf && wf_e;

  LeaveScope();

/*
  if (!wf_e)
  {
// 20120907 -->
//    TYPE_REP_TypeRep str (mk_REP_SeqTypeRep(rep_alltp));
    TYPE_REP_TypeRep str (mk_REP_UnionTypeRep(mk_set(mk_REP_EmptySeqTypeRep(e_tp), mk_REP_SeqTypeRep(e_tp))));
// <-- 20120907
    GetCI().SetTypeInfo (cid, str);
    return mk_(Bool(false), str);
  }
  else
  {
// 20120907 -->
//    TYPE_REP_TypeRep utr (mk_REP_UnionTypeRep(mk_set(mk_REP_EmptySeqTypeRep(rep_alltp), mk_REP_SeqTypeRep(e_tp))));
    TYPE_REP_TypeRep utr (mk_REP_UnionTypeRep(mk_set(mk_REP_EmptySeqTypeRep(e_tp), mk_REP_SeqTypeRep(e_tp))));
// <-- 20120907
    GetCI().SetTypeInfo (cid, utr);
    return mk_(Bool(wf_bd == Bool(true)) && wf_pr && pcomp && only_num && only_one, utr);
  }
*/
  TYPE_REP_TypeRep utr (mk_REP_UnionTypeRep(mk_set(mk_REP_EmptySeqTypeRep(e_tp), mk_REP_SeqTypeRep(e_tp))));
  GetCI().SetTypeInfo (cid, utr);
  return mk_(reswf, utr);
}

#endif //! NOSS
