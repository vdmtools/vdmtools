/***
*  * WHAT
*  * This module contains everything that is needed for the static checker to set
*  * the framework for recursive PO. It contains functions to build the recursion
*  * map, that will be used for generate warnings or errors and also will be later
*  * used by the POG to generate the POs.
*  * FILE
*  *    $Source:$
*  * PROJECT
*  *    VDM-SL Toolbox
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "statsem.h"
#include "rec.h"
#include "astaux.h"

SET<TYPE_AS_Name> SSREC::eset;
Nil SSREC::nil;

// main_RecMap
// fnm : map AS`Name to ENV`FunctionInfo
// -> map AS`Name to ENV`FunctionInfo
Map SSREC::main_RecMap (const Map & fnm)
{
//  return disting_recMap(trans_RecMap(recMapSet2Seq(fnm)), fnm);

/*
  Map recMap1 (recMapSet2Seq(fnm));
  Map recMap2 (trans_RecMap(recMap1));
  Map recMap_res (disting_recMap(recMap2, fnm));
*/

  Map dcm (createDirectCallMap(fnm));
  Map icm (createIndirectCallMap(dcm));
  Map recMap11_res (createRecMap(icm, fnm));

//  return recMap_res;
  return recMap11_res;
}

// disting_recMap
// fnm : map AS`Name to set of seq of AS`Name
// recmap : map AS`Name to ENV`FunctionInfo
// -> map AS`Name to ENV`FunctionInfo
Map SSREC::disting_recMap(const Map & fnm, const Map & recmap)
{
  Map res;
  Set nm_l (fnm.Dom());
  Generic x;
  for (bool bb = nm_l.First(x); bb; bb = nm_l.Next(x))
  {
    TYPE_SSENV_FunctionInfo fi (recmap[x]);
    Set s (fnm[x]);
    SET<TYPE_AS_Name> newrec;
    Generic y_;
    for (bool cc = s.First(y_); cc; cc = s.Next(y_))
    {
      Sequence y (y_);
      if (y[y.Length()] == x)
        newrec.Insert(y.Hd());
    }
    fi.set_rec(newrec);
    res.ImpModify(x, fi);
  }
  return res;
}

// createRecMap(not in spec)
// icm : map AS`Name to set of AS`Name
// recmap : map AS`Name to ENV`FunctionInfo
// -> map AS`Name to ENV`FunctionInfo
Map SSREC::createRecMap(const Map & icm, const Map & recmap)
{
  Map res;
  Set nm_l (recmap.Dom());
  Generic x;
  for (bool bb = nm_l.First(x); bb; bb = nm_l.Next(x))
  {
    TYPE_SSENV_FunctionInfo fi (recmap[x]);
    Set s (fi.get_rec());
    SET<TYPE_AS_Name> newrec;
    Generic y;
    for (bool cc = s.First(y); cc; cc = s.Next(y))
    {
      if ((x == y) || Set(icm[y]).InSet(x))
      {
        newrec.Insert(y);
      }
    }
    fi.set_rec(newrec);
    res.ImpModify(x, fi);
  }
  return res;
}

// recMapSet2Seq
// fnm : map AS`Name to ENV`FunctionInfo
// -> map AS`Name to set of seq of AS`Name
Map SSREC::recMapSet2Seq(const Map & fnm)
{
  Map res;
  Set nm_l (fnm.Dom());
  Generic x;
  for (bool bb = nm_l.First(x); bb; bb = nm_l.Next(x))
  {
    TYPE_SSENV_FunctionInfo fi (fnm[x]);
    SET<TYPE_AS_Name> rec (fi.get_rec());
    Set s;
    Generic y;
    for (bool cc = rec.First(y); cc; cc = rec.Next(y))
    {
      Sequence l;
      l.ImpAppend(y);
      s.Insert(l);
    }
    res.ImpModify(x, s);
  }
  return res;
}

// createDirectCallMap (not in sepc)
// fnm : map AS`Name to ENV`FunctionInfo
// -> map AS`Name to set of AS`Name
Map SSREC::createDirectCallMap(const Map & fnm)
{
  Map res;
  Set nm_s (fnm.Dom());
  Generic nm;
  for (bool bb = nm_s.First(nm); bb; bb = nm_s.Next(nm))
  {
    TYPE_SSENV_FunctionInfo fi (fnm[nm]);
    SET<TYPE_AS_Name> rec (fi.get_rec());
    res.ImpModify(nm, rec);
  }
  return res;
}

// createIndirectCallMap (not in spec)
// dcm : map AS`Name to set of AS`Name
// ->  map AS`Name to set of AS`Name
Map SSREC::createIndirectCallMap(const Map & dcm)
{
  Map res;
  Set nm_s (dcm.Dom());
  Generic nm;
  for (bool bb = nm_s.First(nm); bb; bb = nm_s.Next(nm))
  {
    Set s (dcm[nm]);
    Set tmp_s (createIndirectCallMapAux(s, dcm));
    res.ImpModify(nm, tmp_s);
  }
  return res;
}

// createIndirectCallMapAux
// s : set of AS`Name
// dcm : map AS`Name to set of AS`Name
// -> set of AS`Name
Set SSREC::createIndirectCallMapAux(const Set & s, const Map & dcm)
{
  Set nm_s (s);
  Set res (s);
  Generic nm;
  for (bool cc = nm_s.First(nm); cc; cc = nm_s.Next(nm))
  {
    res.ImpUnion(dcm[nm]);
  }
  if (s.Card() == res.Card())
    return res;
  else
    return createIndirectCallMapAux(res, dcm);
}

// trans_RecMap
// recm : map AS`Name to set of seq of AS`Name
// -> map AS`Name to set of seq of AS`Name
Map SSREC::trans_RecMap(const Map & recm)
{
  Map recm_q (compose_RecMap(recm, recm));
  if (recm == recm_q)
    return recm;
  else
    return trans_RecMap(recm_q);
}

// compose_RecMap
// m : (map AS`Name to set of seq of AS`Name)
// n : (map AS`Name to set of seq of AS`Name)
// -> map AS`Name to set of seq of AS`Name
Map SSREC::compose_RecMap(const Map & m, const Map & n)
{
  Map res;
  Set nm_l (m.Dom());
  Generic x;
  for (bool bb = nm_l.First(x); bb; bb = nm_l.Next(x))
  {
    res.ImpModify(x, compose_auxRecMap(m[x], n));
  }
  return res;
}

// compose_auxRecMap
// trac : set of seq of AS`Name
// fnm : (map AS`Name to set of seq of AS`Name)
// -> set of seq of AS`Name
Set SSREC::compose_auxRecMap(const Set & trac, const Map & fnm)
{
  Set trac_q (trac);
  Set res;
  Generic g;
  for (bool bb = trac_q.First(g); bb; bb = trac_q.Next(g))
  {
    res.ImpUnion(compose_aux2RecMap(g, fnm));
  }
  return res;
}

// compose_aux2RecMap
// trace : seq of AS`Name
// fnm : (map AS`Name to set of seq of AS`Name)
// -> set of seq of AS`Name
Set SSREC::compose_aux2RecMap(const SEQ<TYPE_AS_Name> & trace, const Map & fnm)
{
  TYPE_AS_Name key (trace[trace.Length()]);
  if (fnm.DomExists(key))
  {
    Set s (fnm[key]);

    Set res;
    Generic x_;
    for (bool bb = s.First(x_); bb; bb = s.Next(x_))
    {
      SEQ<TYPE_AS_Name> x (x_);
      TYPE_AS_Name nm (x.Hd());
      SEQ<TYPE_AS_Name> newtrace (trace);
      if (!trace.Elems().InSet(nm))
        newtrace.ImpAppend(nm);
      res.Insert(newtrace);
    }
    return res;
  }
  else
  {
    Set res;
    res.Insert(trace);
    return res;
  }
}

// getFuncAppFnDef
// fnd : AS`FnDef
// sn : set of AS`Name
// -> set of AS`Name
SET<TYPE_AS_Name> SSREC::getFuncAppFnDef(const TYPE_AS_FnDef & fnd, const SET<TYPE_AS_Name> & sn)
{
  switch(fnd.GetTag()) {
    case TAG_TYPE_AS_ExplFnDef:
      return getFuncAppFnBody(fnd.GetRecord(pos_AS_ExplFnDef_body), sn);
    case TAG_TYPE_AS_ExtExplFnDef:
      return getFuncAppFnBody(fnd.GetRecord(pos_AS_ExtExplFnDef_body), sn);
    default:
      return eset;
  }
}

// getFuncAppFnBody
// fn : AS`FnBody
// sn : set of AS`Name
// -> set of AS`Name
SET<TYPE_AS_Name> SSREC::getFuncAppFnBody(const TYPE_AS_FnBody & fn, const SET<TYPE_AS_Name> & sn)
{
  Generic body (fn.GetField(pos_AS_FnBody_body));
  if (body == Int(NOTYETSPEC))
    return eset;
#ifdef VDMPP
  else if (body == Int(SUBRESP))
    return eset;
#endif // VDMPP
  else
    return getFuncAppExpr(body, sn);
}

// getFuncAppExpr
// expr : AS`Expr
// sn : set of AS`Name
// -> set of AS`Name
SET<TYPE_AS_Name> SSREC::getFuncAppExpr(const TYPE_AS_Expr & expr, const SET<TYPE_AS_Name> & sn)
{
//wcout << expr.GetTag() << endl;
//wcout << expr << endl;
  switch(expr.GetTag()) {
    case TAG_TYPE_AS_DefExpr: {
      return getFuncAppDefExpr(expr, sn);
    }
    case TAG_TYPE_AS_LetExpr: {
      return getFuncAppLetExpr(expr, sn);
    }
    case TAG_TYPE_AS_LetBeSTExpr: {
      return getFuncAppLetBeSTExpr(expr, sn);
    }
    case TAG_TYPE_AS_IfExpr: {
      return getFuncAppIfExpr(expr, sn);
    }
    case TAG_TYPE_AS_CasesExpr: {
      return getFuncAppCasesExpr(expr, sn);
    }
    case TAG_TYPE_AS_PrefixExpr: {
      return getFuncAppPrefixExpr(expr, sn);
    }
    case TAG_TYPE_AS_BinaryExpr: {
      return getFuncAppBinaryExpr(expr, sn);
    }
    case TAG_TYPE_AS_AllOrExistsExpr: {
      const SEQ<TYPE_AS_MultBind> & bind_l (expr.GetSequence(pos_AS_AllOrExistsExpr_bind));

      SET<TYPE_AS_Name> res (getFuncAppExpr(expr.GetRecord(pos_AS_AllOrExistsExpr_pred), sn));
      size_t len_bind_l = bind_l.Length();
      for (size_t idx = 1; idx <= len_bind_l; idx++)
      {
        res.ImpUnion(getFuncAppMultBind(bind_l[idx], sn));
      }
      return res;
    }
    case TAG_TYPE_AS_ExistsUniqueExpr: {
      SET<TYPE_AS_Name> res (getFuncAppExpr(expr.GetRecord(pos_AS_ExistsUniqueExpr_pred), sn));
      res.ImpUnion(getFuncAppBind(expr.GetRecord(pos_AS_ExistsUniqueExpr_bind), sn));
      return res;
    }
    case TAG_TYPE_AS_SetEnumerationExpr: {
      const SEQ<TYPE_AS_Expr> & elm_le (expr.GetSequence(pos_AS_SetEnumerationExpr_els));
      SET<TYPE_AS_Name> res;
      size_t len_elm_le = elm_le.Length();
      for (size_t idx = 1; idx <= len_elm_le; idx++)
      {
        res.ImpUnion(getFuncAppExpr(elm_le[idx], sn));
      }
      return res;
    }
    case TAG_TYPE_AS_SetComprehensionExpr: {
      const SEQ<TYPE_AS_MultBind> & bind_l (expr.GetSequence(pos_AS_SetComprehensionExpr_bind));
      const Generic & pred_e (expr.GetField(pos_AS_SetComprehensionExpr_pred));

      SET<TYPE_AS_Name> res (getFuncAppExpr(expr.GetRecord(pos_AS_SetComprehensionExpr_elem), sn));
      if (!pred_e.IsNil())
        res.ImpUnion(getFuncAppExpr(pred_e, sn));
      size_t len_bind_l = bind_l.Length();
      for (size_t idx = 1; idx <= len_bind_l; idx++)
      {
        res.ImpUnion(getFuncAppMultBind(bind_l[idx], sn));
      }
      return res;
    }
    case TAG_TYPE_AS_SetRangeExpr: {
      SET<TYPE_AS_Name> res (getFuncAppExpr(expr.GetRecord(pos_AS_SetRangeExpr_lb), sn));
      res.ImpUnion(getFuncAppExpr(expr.GetRecord(pos_AS_SetRangeExpr_ub), sn));
      return res;
    }
    case TAG_TYPE_AS_SeqEnumerationExpr: {
      const SEQ<TYPE_AS_Expr> & els_l (expr.GetSequence(pos_AS_SeqEnumerationExpr_els));

      SET<TYPE_AS_Name> res;
      size_t len_els_l = els_l.Length();
      for (size_t idx = 1; idx <= len_els_l; idx++)
      {
        res.ImpUnion(getFuncAppExpr(els_l[idx], sn));
      }
      return res;
    }
    case TAG_TYPE_AS_SeqComprehensionExpr: {
      const Generic & pred_e (expr.GetField(pos_AS_SeqComprehensionExpr_pred));
 
      SET<TYPE_AS_Name> res (getFuncAppExpr(expr.GetRecord(pos_AS_SeqComprehensionExpr_elem), sn));
      if (!pred_e.IsNil())
        res.ImpUnion(getFuncAppExpr(pred_e, sn));
      res.ImpUnion(getFuncAppBind(expr.GetRecord(pos_AS_SeqComprehensionExpr_bind), sn));
      return res;
    }
    case TAG_TYPE_AS_SubSequenceExpr: {
      SET<TYPE_AS_Name> res (getFuncAppExpr(expr.GetRecord(pos_AS_SubSequenceExpr_sequence), sn));
      res.ImpUnion(getFuncAppExpr(expr.GetRecord(pos_AS_SubSequenceExpr_frompos), sn));
      res.ImpUnion(getFuncAppExpr(expr.GetRecord(pos_AS_SubSequenceExpr_topos), sn));
      return res;
    }
    case TAG_TYPE_AS_SeqModifyMapOverrideExpr: {
      SET<TYPE_AS_Name> res (getFuncAppExpr(expr.GetRecord(pos_AS_SeqModifyMapOverrideExpr_seqmap), sn));
      res.ImpUnion(getFuncAppExpr(expr.GetRecord(pos_AS_SeqModifyMapOverrideExpr_mapexp), sn));
      return res;
    }
    case TAG_TYPE_AS_MapEnumerationExpr: {
      const SEQ<TYPE_AS_Maplet> & els_l (expr.GetSequence(pos_AS_MapEnumerationExpr_els));
      SET<TYPE_AS_Name> res;
      size_t len_els_l = els_l.Length();
      for (size_t idx = 1; idx <= len_els_l; idx++)
      {
        res.ImpUnion(getFuncAppMaplet(els_l[idx], sn));
      }
      return res;
    }
    case TAG_TYPE_AS_MapComprehensionExpr: {
      return getFuncAppMapCampExpr(expr, sn);
    }
    case TAG_TYPE_AS_TupleConstructorExpr: {
      return getFuncAppTupleConstructorExpr(expr, sn);
    }
    case TAG_TYPE_AS_TupleSelectExpr: {
      return getFuncAppExpr(expr.GetRecord(pos_AS_TupleSelectExpr_tuple), sn);
    }
    case TAG_TYPE_AS_TypeJudgementExpr: {
      return getFuncAppExpr(expr.GetRecord(pos_AS_TypeJudgementExpr_expr), sn);
    }
    case TAG_TYPE_AS_NarrowExpr: {
      return getFuncAppExpr(expr.GetRecord(pos_AS_NarrowExpr_expr), sn);
    }
    case TAG_TYPE_AS_PreConditionApplyExpr: {
      const SEQ<TYPE_AS_Expr> & arg (expr.GetSequence(pos_AS_PreConditionApplyExpr_arg));

      SET<TYPE_AS_Name> res (getFuncAppExpr(expr.GetRecord(pos_AS_PreConditionApplyExpr_fct), sn));
      size_t len_arg = arg.Length();
      for (size_t idx = 1; idx <= len_arg; idx++)
      {
        res.ImpUnion(getFuncAppExpr(arg[idx], sn));
      }
      return res;
    }
    case TAG_TYPE_AS_TokenConstructorExpr: {
      return getFuncAppExpr(expr.GetRecord(pos_AS_TokenConstructorExpr_field), sn);
    }
    case TAG_TYPE_AS_RecordConstructorExpr: {
      const SEQ<TYPE_AS_Expr> & fields_le (expr.GetSequence(pos_AS_RecordConstructorExpr_fields));
      SET<TYPE_AS_Name> res;
      size_t len_fields_le = fields_le.Length();
      for (size_t idx = 1; idx <= len_fields_le;idx++)
      {
        res.ImpUnion(getFuncAppExpr(fields_le[idx], sn));
      }
      return res;
    }
    case TAG_TYPE_AS_RecordModifierExpr: {
      const SEQ<TYPE_AS_RecordModification> & modifiers (expr.GetSequence(pos_AS_RecordModifierExpr_modifiers));
      SET<TYPE_AS_Name> res (getFuncAppExpr(expr.GetRecord(pos_AS_RecordModifierExpr_rec), sn));
      size_t len_modifiers = modifiers.Length();
      for (size_t idx = 1; idx <= len_modifiers; idx++)
      {
        res.ImpUnion(getFuncAppRecordModification(modifiers[idx], sn));
      }
      return res;
    }
    case TAG_TYPE_AS_ApplyExpr: {
      return getFuncAppApplyExpr(expr, sn);
    }
    case TAG_TYPE_AS_FieldSelectExpr: {
      return getFuncAppExpr(expr.GetRecord(pos_AS_FieldSelectExpr_rec), sn);
    }
    case TAG_TYPE_AS_LambdaExpr: {
      SET<TYPE_AS_Name> res (getFuncAppExpr(expr.GetRecord(pos_AS_LambdaExpr_body), sn));
      const SEQ<TYPE_AS_TypeBind> & tb_l (expr.GetSequence(pos_AS_LambdaExpr_parm));
      size_t len_tb_l = tb_l.Length();
      for (size_t idx = 1; idx <= len_tb_l; idx++)
      {
        res.ImpUnion(getFuncAppBind(tb_l[idx], sn));
      }
      return res;
    }
    case TAG_TYPE_AS_IsExpr: {
      return getFuncAppExpr(expr.GetRecord(pos_AS_IsExpr_arg), sn);
    }
    case TAG_TYPE_AS_BoolLit:
    case TAG_TYPE_AS_RealLit:
    case TAG_TYPE_AS_NumLit:
    case TAG_TYPE_AS_CharLit:
    case TAG_TYPE_AS_TextLit:
    case TAG_TYPE_AS_QuoteLit:
    case TAG_TYPE_AS_NilLit: {
      return eset;
    }
    case TAG_TYPE_AS_Name: {
      SET<TYPE_AS_Name> res;
      if (!sn.InSet(expr))
        res.Insert(expr);
      return res;
    }
    case TAG_TYPE_AS_UndefinedExpr: {
      return eset;
    }
    case TAG_TYPE_AS_IotaExpr: {
      SET<TYPE_AS_Name> res (getFuncAppExpr(expr.GetRecord(pos_AS_IotaExpr_pred), sn));
      res.ImpUnion(getFuncAppBind(expr.GetRecord(pos_AS_IotaExpr_bind), sn));
      return res;
    }
    case TAG_TYPE_AS_FctTypeInstExpr: {
//
//      return eset;
//      return getFuncAppExpr(expr.GetRecord(pos_AS_FctTypeInstExpr_polyfct), sn);
//
      SET<TYPE_AS_Name> res;
      res.Insert(expr.GetRecord(pos_AS_FctTypeInstExpr_polyfct));
      return res;
    }
    case TAG_TYPE_AS_BracketedExpr: {
      return getFuncAppExpr(expr.GetRecord(pos_AS_BracketedExpr_expr), sn);
    }
//    case TAG_TYPE_AS_ExplFnDef: {
//      return getFuncAppFnDef(expr, sn);
//    }
#ifdef VDMPP
    case TAG_TYPE_AS_NewExpr: {
      const SEQ<TYPE_AS_Expr> & arg_l (expr.GetSequence(pos_AS_NewExpr_args));
      SET<TYPE_AS_Name> res;
      size_t len_arg_l = arg_l.Length();
      for (size_t idx = 1; idx <= len_arg_l; idx++)
      {
        res.ImpUnion(getFuncAppExpr(arg_l[idx], sn));
      }
      return res;
    }
    case TAG_TYPE_AS_ThreadIdExpr: {
      return eset;
    }
    case TAG_TYPE_AS_SelfExpr: {
      return eset;
    }
    case TAG_TYPE_AS_SameClassExpr:
    case TAG_TYPE_AS_SameBaseClassExpr:
    case TAG_TYPE_AS_IsOfClassExpr:
    case TAG_TYPE_AS_IsOfBaseClassExpr:
    case TAG_TYPE_AS_ActExpr:
    case TAG_TYPE_AS_FinExpr:
    case TAG_TYPE_AS_ActiveExpr:
    case TAG_TYPE_AS_WaitingExpr:
    case TAG_TYPE_AS_ReqExpr: {
      return eset;
    }
#ifdef VICE
    case TAG_TYPE_AS_CurTimeExpr: {
      return eset;
    }
#endif // VICE
#endif // VDMPP
    default: {
      return eset;
    }
  }
}

// getFuncAppDefExpr
// expr : AS`DefExpr
// sn : set of AS`Name
// ==> set of AS`Name
SET<TYPE_AS_Name> SSREC::getFuncAppDefExpr(const TYPE_AS_DefExpr & expr, const SET<TYPE_AS_Name> & sn)
{
  type_dd2PL def_l (expr.GetSequence(pos_AS_DefExpr_Def));  // seq of (PatternBind * Expr)

  SET<TYPE_AS_Name> ids;
  SET<TYPE_AS_Name> remids (sn);
  Generic elm;
  for (bool bb = def_l.First(elm); bb; bb = def_l.Next(elm))
  {
    Tuple t (elm); // (PatternBind * Expr)
    SET<TYPE_AS_Name> expids (getFuncAppExpr(t.GetRecord(2), remids));
    SET<TYPE_AS_Name> patids (getFuncAppPatternBind(t.GetRecord(1)));
    ids.ImpUnion(expids);
    remids.ImpUnion(patids);
  }
  return getFuncAppExpr(expr.GetRecord(pos_AS_DefExpr_In), remids);
}

// getFuncAppPatternBind
// pb : AS`PatternBind
// -> set of AS`Name
SET<TYPE_AS_Name> SSREC::getFuncAppPatternBind(const TYPE_AS_PatternBind & pb)
{
  switch(pb.GetTag()) {
    case TAG_TYPE_AS_SetBind: {
      return GetStatSem().ExtractPatternName(pb.GetRecord(pos_AS_SetBind_pat)).Dom(); 
    }
    case TAG_TYPE_AS_TypeBind: {
      return GetStatSem().ExtractPatternName(pb.GetRecord(pos_AS_TypeBind_pat)).Dom(); 
    }
    default: {
      return GetStatSem().ExtractPatternName(pb).Dom(); 
    }
  }
}

// getFuncAppLetExpr
// expr : AS`LetExpr
// sn : set of AS`Name
// -> set of AS`Name
SET<TYPE_AS_Name> SSREC::getFuncAppLetExpr(const TYPE_AS_LetExpr & expr, const SET<TYPE_AS_Name> & sn)
{
  Tuple infer (getFuncAppLocalDef(expr.GetSequence(pos_AS_LetExpr_localdef), sn));
  SET<TYPE_AS_Name> res (infer.GetSet(1));
  res.ImpUnion(getFuncAppExpr(expr.GetRecord(pos_AS_LetExpr_body), infer.GetSet(2)));
  return res;
}

// getFuncAppLocalDef
// vd : seq of AS`LocalDef
// sn : set of AS`Name
// ==> set of AS`Name * set of AS`Name
Tuple SSREC::getFuncAppLocalDef(const SEQ<TYPE_AS_LocalDef> & vd, const SET<TYPE_AS_Name> & sn)
{
  SET<TYPE_AS_Name> ids;
  SET<TYPE_AS_Name> remids (sn);
  size_t len_vd = vd.Length();
  for (size_t idx = 1; idx <= len_vd; idx++)
  {
    const TYPE_AS_LocalDef & ldef (vd[idx]);
    switch(ldef.GetTag()) {
      case TAG_TYPE_AS_ValueDef: {
        SET<TYPE_AS_Name> expids (getFuncAppExpr(ldef.GetRecord(pos_AS_ValueDef_val), remids));
        SET<TYPE_AS_Name> patids (getFuncAppPatternBind(ldef.GetRecord(pos_AS_ValueDef_pat)));
        ids.ImpUnion(expids);
        remids.ImpUnion(patids);
        break;
      }
      case TAG_TYPE_AS_ExplFnDef: {
        ids.ImpUnion(getFuncAppFnDef(ldef, sn));
        break;
      }
      default: {
        break;
      }
    }
  }
  return mk_(ids, remids);
}

// getFuncAppApplyExpr
// expr : AS`ApplyExpr
// sn : set of AS`Name
// -> set of AS`Name
SET<TYPE_AS_Name> SSREC::getFuncAppApplyExpr(const TYPE_AS_ApplyExpr & expr, const SET<TYPE_AS_Name> & sn)
{
  const SEQ<TYPE_AS_Expr> & arg_l (expr.GetSequence(pos_AS_ApplyExpr_arg));

  SET<TYPE_AS_Name> res (getFuncAppExpr(expr.GetRecord(pos_AS_ApplyExpr_fct), sn));
  size_t len_arg_l = arg_l.Length();
  for (size_t idx = 1; idx <= len_arg_l; idx++)
  {
    res.ImpUnion(getFuncAppExpr(arg_l[idx], sn));
  }
  return res;
}

// getFuncAppIfExpr
// expr : AS`IfExpr
// sn : set of AS`Name
// -> set of AS`Name
SET<TYPE_AS_Name> SSREC::getFuncAppIfExpr(const TYPE_AS_IfExpr & expr, const SET<TYPE_AS_Name> & sn)
{
  const SEQ<TYPE_AS_ElseifExpr> & elif_l (expr.GetSequence(pos_AS_IfExpr_elsif));

  SET<TYPE_AS_Name> res (getFuncAppExpr(expr.GetRecord(pos_AS_IfExpr_test), sn));
  res.ImpUnion(getFuncAppExpr(expr.GetRecord(pos_AS_IfExpr_cons), sn));
  size_t len_elif_l = elif_l.Length();
  for (size_t idx = 1; idx <= len_elif_l; idx++)
  {
    res.ImpUnion(getFuncAppElseifExpr(elif_l[idx], sn));
  }
  res.ImpUnion(getFuncAppExpr(expr.GetRecord(pos_AS_IfExpr_altn), sn));
  return res;
}

// getFuncAppElseifExpr
// expr : AS`ElseifExpr
// sn : set of AS`Name
// -> set of AS`Name
SET<TYPE_AS_Name> SSREC::getFuncAppElseifExpr(const TYPE_AS_ElseifExpr & expr, const SET<TYPE_AS_Name> & sn)
{
  SET<TYPE_AS_Name> res (getFuncAppExpr(expr.GetRecord(pos_AS_ElseifExpr_test), sn));
  res.ImpUnion(getFuncAppExpr(expr.GetRecord(pos_AS_ElseifExpr_cons), sn));
  return res;
}

// getFuncAppBinaryExpr
// expr : AS`BinaryExpr
//sn : set of AS`Name
// -> set of AS`Name
SET<TYPE_AS_Name> SSREC::getFuncAppBinaryExpr(const TYPE_AS_BinaryExpr & expr, const SET<TYPE_AS_Name> & sn)
{
  SET<TYPE_AS_Name> res (getFuncAppExpr(expr.GetRecord(pos_AS_BinaryExpr_left), sn));
  res.ImpUnion(getFuncAppExpr(expr.GetRecord(pos_AS_BinaryExpr_right), sn));
  return res;
}

// getFuncAppTupleConstructorExpr
// expr : AS`TupleConstructorExpr
// sn : set of AS`Name
// -> set of AS`Name
SET<TYPE_AS_Name> SSREC::getFuncAppTupleConstructorExpr(const TYPE_AS_TupleConstructorExpr & expr,
                                                        const SET<TYPE_AS_Name> & sn)
{
  const SEQ<TYPE_AS_Expr> & fields_le (expr.GetSequence(pos_AS_TupleConstructorExpr_fields));

  SET<TYPE_AS_Name> res;
  size_t len_fields_le = fields_le.Length();
  for (size_t idx = 1; idx <= len_fields_le; idx++)
  {
    res.ImpUnion(getFuncAppExpr(fields_le[idx], sn));
  }
  return res;
}

// getFuncAppPrefixExpr
// expr : AS`PrefixExpr
// sn : set of AS`Name
// -> set of AS`Name
SET<TYPE_AS_Name> SSREC::getFuncAppPrefixExpr(const TYPE_AS_PrefixExpr & expr, const SET<TYPE_AS_Name> & sn)
{
  return getFuncAppExpr(expr.GetRecord(pos_AS_PrefixExpr_arg), sn);
}

// getFuncAppCasesExpr
// expr : AS`CasesExpr
// sn : set of AS`Name
// -> set of AS`Name
SET<TYPE_AS_Name> SSREC::getFuncAppCasesExpr(const TYPE_AS_CasesExpr & expr, const SET<TYPE_AS_Name> & sn)
{
  const TYPE_AS_Expr & sel_e (expr.GetRecord(pos_AS_CasesExpr_sel));
  const SEQ<TYPE_AS_CaseAltn> & altns_l (expr.GetSequence(pos_AS_CasesExpr_altns));
  const Generic & others_e (expr.GetField(pos_AS_CasesExpr_Others));

  SET<TYPE_AS_Name> res (getFuncAppExpr(sel_e, sn));
  size_t len_altns_l = altns_l.Length();
  for (size_t idx = 1; idx <= len_altns_l; idx++)
  {
    res.ImpUnion(getFuncAppCaseAltn(altns_l[idx], sn));
  }
  if (!others_e.IsNil())
    res.ImpUnion(getFuncAppExpr(others_e, sn));
  return res;
}

// getFuncAppCaseAltn
// expr : AS`CaseAltn
// sn : set of AS`Name
// -> set of AS`Name
SET<TYPE_AS_Name> SSREC::getFuncAppCaseAltn(const TYPE_AS_CaseAltn & expr, const SET<TYPE_AS_Name> & sn)
{
  const SEQ<TYPE_AS_Pattern> & pat_l (expr.GetSequence(pos_AS_CaseAltn_match));
  const TYPE_AS_Expr & body_e (expr.GetRecord(pos_AS_CaseAltn_body));

  SET<TYPE_AS_Name> res;
  size_t len_pat_l = pat_l.Length();
  for (size_t idx = 1; idx <= len_pat_l; idx++)
  {
    res.ImpUnion(getFuncAppPattern(pat_l[idx], sn));
  }
  res.ImpUnion(getFuncAppExpr(body_e, sn));
  return res;
}

// getFuncAppLetBeSTExpr
// expr : AS`LetBeSTExpr
// sn : set of AS`Name
// -> set of AS`Name
SET<TYPE_AS_Name> SSREC::getFuncAppLetBeSTExpr(const TYPE_AS_LetBeSTExpr & expr, const SET<TYPE_AS_Name> & sn)
{
  //const TYPE_AS_Bind & lhs (expr.GetRecord(pos_AS_LetBeSTExpr_lhs));
  const SEQ<TYPE_AS_MultBind> & lhs (expr.GetSequence(pos_AS_LetBeSTExpr_lhs));
  const Generic & st_e (expr.GetField(pos_AS_LetBeSTExpr_St));
  const TYPE_AS_Expr & in_e (expr.GetRecord(pos_AS_LetBeSTExpr_In));

  //SET<TYPE_AS_Name> res (getFuncAppBind(lhs, sn));
  SET<TYPE_AS_Name> res;
  size_t len_lhs = lhs.Length();
  for (size_t idx = 1; idx <= len_lhs; idx++)
  {
    res.ImpUnion(getFuncAppMultBind(lhs[idx], sn));
  }
  if (!st_e.IsNil())
    res.ImpUnion(getFuncAppExpr(st_e, sn));
  res.ImpUnion(getFuncAppExpr(in_e, sn));
  return res;
}

// getFuncAppMaplet
// expr : AS`Maplet
// sn : set of AS`Name
// -> set of AS`Name
SET<TYPE_AS_Name> SSREC::getFuncAppMaplet(const TYPE_AS_Maplet & expr, const SET<TYPE_AS_Name> & sn)
{
  SET<TYPE_AS_Name> res (getFuncAppExpr(expr.GetRecord(pos_AS_Maplet_mapdom), sn));
  res.ImpUnion(getFuncAppExpr(expr.GetRecord(pos_AS_Maplet_maprng), sn));
  return res;
}

// getFuncAppMapCampExpr
// expr : AS`MapComprehensionExpr
// sn : set of AS`Name
// -> set of AS`Name
SET<TYPE_AS_Name> SSREC::getFuncAppMapCampExpr(const TYPE_AS_MapComprehensionExpr & expr,
                                               const SET<TYPE_AS_Name> & sn)
{
  const TYPE_AS_Maplet & elem_e (expr.GetRecord(pos_AS_MapComprehensionExpr_elem));
  const SEQ<TYPE_AS_MultBind> & bind_l (expr.GetSequence(pos_AS_MapComprehensionExpr_bind));
  const Generic & pred_e (expr.GetField(pos_AS_MapComprehensionExpr_pred));

  SET<TYPE_AS_Name> res (getFuncAppMaplet(elem_e, sn));
  size_t len_bind_l = bind_l.Length();
  for (size_t idx = 1; idx <= len_bind_l; idx++)
  {
    res.ImpUnion(getFuncAppMultBind(bind_l[idx], sn));
  }
  if (!pred_e.IsNil())
    res.ImpUnion(getFuncAppExpr(pred_e, sn));
  return res;
}

// getFuncAppRecordModification
// m : AS`RecordModification
// sn : set of AS`Name
// -> set of AS`Name
SET<TYPE_AS_Name> SSREC::getFuncAppRecordModification(const TYPE_AS_RecordModification & m,
                                                      const SET<TYPE_AS_Name> & sn)
{
  return getFuncAppExpr(m.GetRecord(pos_AS_RecordModification_newexpr), sn);
}

// getFuncAppBind
// bind : AS`Bind
// sn : set of AS`Name
// -> set of AS`Name
SET<TYPE_AS_Name> SSREC::getFuncAppBind(const TYPE_AS_Bind & bind, const SET<TYPE_AS_Name> & sn)
{
  switch(bind.GetTag()){
    case TAG_TYPE_AS_SetBind:  { return getFuncAppSetBind(bind, sn); }
    case TAG_TYPE_AS_TypeBind: { return getFuncAppTypeBind(bind, sn); }
    default: { return eset; }
  }
}

// getFuncAppSetBind
// bind : AS`SetBind
// sn : set of AS`Name
// -> set of AS`Name
SET<TYPE_AS_Name> SSREC::getFuncAppSetBind(const TYPE_AS_SetBind & bind, const SET<TYPE_AS_Name> & sn)
{
  SET<TYPE_AS_Name> res (getFuncAppPattern(bind.GetRecord(pos_AS_SetBind_pat), sn));
  res.ImpUnion(getFuncAppExpr(bind.GetRecord(pos_AS_SetBind_Set), sn));
  return res;
} 

// getFuncAppTypeBind
// bind : AS`TypeBind
// sn : set of AS`Name
// -> set of AS`Name
SET<TYPE_AS_Name> SSREC::getFuncAppTypeBind(const TYPE_AS_TypeBind & bind, const SET<TYPE_AS_Name> & sn)
{
  return getFuncAppPattern(bind.GetRecord(pos_AS_TypeBind_pat), sn);
} 

// getFuncAppSeqBind
// bind : AS`SeqBind
// sn : set of AS`Name
// -> set of AS`Name
SET<TYPE_AS_Name> SSREC::getFuncAppSeqBind(const TYPE_AS_SeqBind & bind, const SET<TYPE_AS_Name> & sn)
{
  SET<TYPE_AS_Name> res (getFuncAppPattern(bind.GetRecord(pos_AS_SeqBind_pat), sn));
  res.ImpUnion(getFuncAppExpr(bind.GetRecord(pos_AS_SeqBind_Seq), sn));
  return res;
} 

// getFuncAppMultBind
// bind : AS`MultBind
// sn : set of AS`Name
// -> set of AS`Name
SET<TYPE_AS_Name> SSREC::getFuncAppMultBind(const TYPE_AS_MultBind & bind, const SET<TYPE_AS_Name> & sn)
{
  switch(bind.GetTag()) {
    case TAG_TYPE_AS_MultSetBind: {
      SET<TYPE_AS_Name> res;
      const SEQ<TYPE_AS_Pattern> & pat_l (bind.GetSequence(pos_AS_MultSetBind_pat));
      size_t len = pat_l.Length();
      for (size_t i = 1; i <= len; i++)
        res.ImpUnion (getFuncAppPattern(pat_l[i], sn));
      res.ImpUnion(getFuncAppExpr(bind.GetRecord(pos_AS_MultSetBind_Set), sn));
      return res;
    }
    case TAG_TYPE_AS_MultTypeBind: {
      const SEQ<TYPE_AS_Pattern> & pat_l (bind.GetSequence(pos_AS_MultTypeBind_pat));
      SET<TYPE_AS_Name> res;
      size_t len = pat_l.Length();
      for (size_t i = 1; i <= len; i++)
        res.ImpUnion (getFuncAppPattern(pat_l[i], sn));
      return res;
    }
    default: {
      return eset;
    }
  }
}

// getFuncAppPattern
// pat : AS`Pattern
// sn : set of AS`Name
// -> set of AS`Name
SET<TYPE_AS_Name> SSREC::getFuncAppPattern(const TYPE_AS_Pattern & pat, const SET<TYPE_AS_Name> & sn)
{
  switch(pat.GetTag()) {
    case TAG_TYPE_AS_PatternName: {
      return eset;
    }
    case TAG_TYPE_AS_MatchVal: {
      return getFuncAppExpr(pat.GetRecord(pos_AS_MatchVal_val), sn);
    }
    case TAG_TYPE_AS_SetEnumPattern: {
      const SEQ<TYPE_AS_Pattern> & elems (pat.GetSequence(pos_AS_SetEnumPattern_Elems));
      SET<TYPE_AS_Name> res;
      size_t len = elems.Length();
      for (size_t i = 1; i <= len; i++)
        res.ImpUnion (getFuncAppPattern(elems[i], sn));
      return res;
    }
    case TAG_TYPE_AS_SetUnionPattern: {
      SET<TYPE_AS_Name> res_set (getFuncAppPattern(pat.GetRecord(pos_AS_SetUnionPattern_lp), sn));
      res_set.ImpUnion (getFuncAppPattern(pat.GetRecord(pos_AS_SetUnionPattern_rp), sn));
      return (res_set);
    }
    case TAG_TYPE_AS_SeqEnumPattern: {
      const SEQ<TYPE_AS_Pattern> & elems (pat.GetSequence(pos_AS_SeqEnumPattern_els));
      SET<TYPE_AS_Name> res_set;
      size_t len = elems.Length();
      for (size_t i = 1; i <= len; i++ )
        res_set.ImpUnion (getFuncAppPattern(elems[i], sn));
      return (res_set);
    }
    case TAG_TYPE_AS_SeqConcPattern: {
      SET<TYPE_AS_Name> res_set (getFuncAppPattern(pat.GetRecord(pos_AS_SeqConcPattern_lp), sn));
      res_set.ImpUnion (getFuncAppPattern(pat.GetRecord(pos_AS_SeqConcPattern_rp), sn));
      return (res_set);
    }
    case TAG_TYPE_AS_MapletPattern: {
      SET<TYPE_AS_Name> res_set (getFuncAppPattern(pat.GetRecord(pos_AS_MapletPattern_dp), sn));
      res_set.ImpUnion (getFuncAppPattern(pat.GetRecord(pos_AS_MapletPattern_rp), sn));
      return (res_set);
    }
    case TAG_TYPE_AS_MapEnumPattern: {
      const SEQ<TYPE_AS_MapletPattern> & mls (pat.GetSequence(pos_AS_MapEnumPattern_mls));
      SET<TYPE_AS_Name> res_set;
      size_t len = mls.Length();
      for (size_t i = 1; i <= len; i++ )
        res_set.ImpUnion (getFuncAppPattern(mls[i], sn));
      return (res_set);
    }
    case TAG_TYPE_AS_MapMergePattern: {
      SET<TYPE_AS_Name> res_set (getFuncAppPattern(pat.GetRecord(pos_AS_MapMergePattern_lp), sn));
      res_set.ImpUnion (getFuncAppPattern(pat.GetRecord(pos_AS_MapMergePattern_rp), sn));
      return (res_set);
    }
    case TAG_TYPE_AS_TuplePattern: {
      const SEQ<TYPE_AS_Pattern> & elems (pat.GetSequence(pos_AS_TuplePattern_fields));
      SET<TYPE_AS_Name> res_set;
      size_t len = elems.Length();
      for (size_t i = 1; i <= len; i++ )
        res_set.ImpUnion (getFuncAppPattern(elems[i], sn));
      return (res_set);
    }
    case TAG_TYPE_AS_RecordPattern: {
      const SEQ<TYPE_AS_Pattern> & elems (pat.GetSequence(pos_AS_RecordPattern_fields));
      SET<TYPE_AS_Name> res_set;
      size_t len = elems.Length();
      for (size_t i = 1; i <= len; i++ )
        res_set.ImpUnion (getFuncAppPattern(elems[i], sn));
      return (res_set);
    }
#ifdef VDMPP
    case TAG_TYPE_AS_ObjectPattern: {
      const SEQ<TYPE_AS_FieldPattern> & elems (pat.GetSequence(pos_AS_ObjectPattern_fields));
      SET<TYPE_AS_Name> res_set;
      size_t len = elems.Length();
      for (size_t i = 1; i <= len; i++ )
        res_set.ImpUnion (getFuncAppPattern(elems[i], sn));
      return (res_set);
    }
    case TAG_TYPE_AS_FieldPattern: {
      return getFuncAppPattern(pat.GetRecord(pos_AS_FieldPattern_pat), sn);
    }
#endif // VDMPP
    default: {
      return eset;
    }
  }
}

// OtherClassApp
// clnm : AS`Name
// fnm : AS`Name
// -> bool
bool SSREC::OtherClassApp(const TYPE_AS_Name & clnm, const TYPE_AS_Name & fnm)
{
  if (fnm.GetSequence(pos_AS_Name_ids).Length() > 1)
  {
    if (fnm.GetSequence(pos_AS_Name_ids)[1] == clnm.GetSequence(pos_AS_Name_ids)[1])
      return false;
    else
      return true;
  }
  else
    return false;
}

// removeNonRec
// recmap : map AS`Name to ENV`FunctionInfo
// ==> map AS`Name to ENV`FunctionInfo
Map SSREC::removeNonRec(const Map & recmap)
{
  Generic x;
  Set dom_recmap (recmap.Dom());
  Map recmap_q;
  for (bool bb = dom_recmap.First(x); bb; bb = dom_recmap.Next(x))
  {
    TYPE_SSENV_FunctionInfo fi (recmap[x]);
    if (!fi.get_rec().IsEmpty())
      recmap_q.ImpModify(x, fi);
  }
  Map recmap_qq;
  for (bool cc = recmap_q.First(x); cc; cc = recmap_q.Next(x))
  {
    TYPE_SSENV_FunctionInfo fi (recmap[x]);
    SET<TYPE_AS_Name> rec (fi.get_rec());
    SET<TYPE_AS_Name> newrec;
    Generic y;
    for (bool dd = rec.First(y); dd; dd = rec.Next(y))
    {
      if (recmap_q.DomExists(y))
        newrec.Insert(y);
    }
    fi.set_rec(newrec);
    recmap_qq.ImpModify(x, fi);
  }

  if (recmap_qq == recmap)
    return recmap;
  else
    return removeNonRec(recmap_qq);
}

// BuildRecMap
// clnm : AS`Name
// fnm : map AS`Name to AS`FnDef
// ==> map AS`Name to ENV`FunctionInfo
Map SSREC::BuildRecMap(const TYPE_AS_Name & clnm, const Map & fnm)
{
  Map recMap; // map AS`Name to ENV`FunctionInfo
  Set nm_s (fnm.Dom());
  Generic nm;
  for (bool bb = nm_s.First(nm); bb; bb = nm_s.Next(nm))
  {
    SET<TYPE_AS_Name> allfns_q;
#ifdef VDMSL
    SET<TYPE_SSENV_ExpSigRep> esr_s (GetStatSem().GetModuleEnv().Rng());
    Generic g;
    for (bool cc = esr_s.First(g); cc; cc = esr_s.Next(g))
    {
      TYPE_SSENV_ExpSigRep esr (g);
      allfns_q.ImpUnion(esr.get_fcts().Dom());
      allfns_q.ImpUnion(esr.get_polys().Dom()); // 20080827
    }
#endif // VDMSL
#ifdef VDMPP
    SET<TYPE_SSENV_ParseTypeInfo> pti_s (GetStatSem().GetParseEnv().Rng());
    Generic g;
    for (bool cc = pti_s.First(g); cc; cc = pti_s.Next(g))
    {
      TYPE_SSENV_ParseTypeInfo pti (g);
      //allfns_q.ImpUnion(pti.get_fndefs().Dom());
      allfns_q.ImpUnion(pti.get_fcts().Dom());
      allfns_q.ImpUnion(pti.get_polys().Dom()); // 20080827
    }
#endif // VDMPP
    SET<TYPE_AS_Name> allapps_q (getFuncAppFnDef(fnm[nm], eset));

    SET<TYPE_AS_Name> allfns;
    Generic x;
    for (bool dd = allfns_q.First(x); dd; dd = allfns_q.Next(x))
    {
      allfns.Insert(DestroyCid(x));
    }

    SET<TYPE_AS_Name> allapps;
    for (bool dd = allapps_q.First(x); dd; dd = allapps_q.Next(x))
    {
      allapps.Insert(DestroyCid(x));
    }

//wcout << L"allfns: " << allfns << endl;
//wcout << L"allapps: " << allapps << endl;

    SET<TYPE_AS_Name> normout;
    for (bool ee = allapps.First(x); ee; ee = allapps.Next(x))
    {
      if (OtherClassApp(clnm, x) && allfns.InSet(x))
        normout.Insert(x);
    }

    SET<TYPE_AS_Name> inapps (allapps);
    inapps.ImpDiff(normout);

//wcout << L"inapps: " << inapps << endl;

    SET<TYPE_AS_Name> normin;
    for (bool ff = inapps.First(x); ff; ff = inapps.Next(x))
    {
      if (allfns.InSet(x))
        normin.Insert(NormalizeName(clnm, x));
      else if (allfns.InSet(NormalizeName(clnm, x)))
        normin.Insert(NormalizeName(clnm, x));
/*
      else
      {
        TYPE_AS_Name nm (x);
        if (nm.get_ids().Length() == 2)
        {
          TYPE_AS_Name classnm (ASTAUX::GetFirstName(nm));
          TYPE_AS_Name funcnnm (ASTAUX::GetSecondName(nm));
          if ((ASTAUX::GetFirstName(nm) == clnm) &&
               allfns.InSet(ASTAUX::GetSecondName(nm)))
            normin.Insert(nm);
        }
      }
*/
    }

//wcout << L"normin: " << normin << endl;

    SET<TYPE_AS_Name> norm (normin);
    norm.ImpUnion(normout);
    recMap.ImpModify(GetStatSem().ExtName(clnm, nm),
                     TYPE_SSENV_FunctionInfo().Init(norm,
                                            ASTAUX::GetFnMeasu(fnm[nm]),
                                            Bool(false)));
  }

//wcout << L"recMap: " << recMap << endl;

  SET<TYPE_AS_Name> toVisit;
  SET<TYPE_SSENV_FunctionInfo> fi_s (recMap.Rng());
  Generic x;
  for (bool gg = fi_s.First(x); gg; gg = fi_s.Next(x))
  {
    TYPE_SSENV_FunctionInfo fi (x);
    toVisit.ImpUnion(fi.get_rec());
  }
  SET<TYPE_AS_Name> visited (recMap.Dom());
  toVisit.ImpDiff(visited);

//wcout << L"toVisit: " << toVisit << endl;

  Map recMap2 (recConstructMap(toVisit, visited, recMap));
  Map recMap3 (removeNonRec(recMap2));
  Map recMap4 (main_RecMap(recMap3));
  Map recMap_res (removeNonRec(recMap4));

  return recMap_res;
}

// recConstructMap
// toVisit : set of AS`Name
// visited : set of AS`Name
// recMap : map AS`Name to ENV`FunctionInfo
// ==> map AS`Name to ENV`FunctionInfo
Map SSREC::recConstructMap(const SET<TYPE_AS_Name> & toVisit, const SET<TYPE_AS_Name> & visited, const Map & recMap)
{
  if (toVisit.IsEmpty())
    return recMap;

  TYPE_AS_Name x (toVisit.GetElem());
  Tuple xapps (GetAppsFn(x));
//wcout << L"xapps: " << xapps << endl;
  SET<TYPE_AS_Name> newvis (visited);
  newvis.Insert(x);
  SET<TYPE_AS_Name> newtov (toVisit);
  newtov.ImpUnion(xapps.GetSet(1));
  newtov.ImpDiff(newvis);
  Map newrm (recMap);
  newrm.ImpModify(x, TYPE_SSENV_FunctionInfo().Init(xapps.GetSet(1), xapps.GetField(2), Bool(false)));
//wcout << L"newtov: " << newtov << endl;
  return recConstructMap(newtov, newvis, newrm);
}

// NormalizeName
// clnm : AS`Name
// fnm : AS`Name
// ==> AS`Name
TYPE_AS_Name SSREC::NormalizeName(const TYPE_AS_Name & clnm, const TYPE_AS_Name & fnm)
{
  return GetStatSem().ExtName(clnm, fnm);
}

// DestroyCid
// nm : AS`Name
// ==> AS`Name
TYPE_AS_Name SSREC::DestroyCid(const TYPE_AS_Name & nm)
{
  TYPE_AS_Name res (nm);
  res.SetField(pos_AS_Name_cid, NilContextId);
  return res;
}

// GetAppsFn
// nm : AS`Name
// ==> (set of AS`Name) * [AS`Name]
Tuple SSREC::GetAppsFn(const TYPE_AS_Name & nm)
{
  Generic f (GetStatSem().GetFuncDefWOCtxt(nm));
//wcout << L"f: " << f << endl;
  if (!f.IsNil() && f.Is(TAG_TYPE_AS_ExplFnDef))
    return mk_(getFuncAppFnDef(f, eset), ASTAUX::GetFnMeasu(f));
  else
    return mk_(eset, nil);
}
