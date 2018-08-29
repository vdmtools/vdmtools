#include "traces.h"
#include "evalaux.h"
#include "contextinfo.h"
#include "semvalues.h"
#include "asquotes.h"
#include "libman.h"
#include "pattern.h"
#include "tb_exceptions.h"
#include "rt_errmsg.h"
#include "astaux.h"
#include "intconvquotes.h"

//wcout << L"intertp: " << INT2Q::h2gAS(intertp) << endl;

int EXPANDED::zeroOrMoreMax = 3;
int EXPANDED::oneOrMoreMax = 3;
SEQ<TYPE_AS_Id> EXPANDED::seqOfNames;
Generic EXPANDED::curcl = Nil();
#ifdef VDMPP
Generic EXPANDED::curobj = Nil();
#endif // VDMPP
Sequence EXPANDED::cxt_s_stack; // seq of set of SEM`BlkEnv

// ExpandSpecTraces
// spec : AS`Document
// clmod_s : set of AS`Name
// ==> map AS`Name to set of seq of AS`Expr
Map EXPANDED::ExpandSpecTraces(const TYPE_AS_Document & spec, const SET<TYPE_AS_Name> & clmod_s, wostream & wos)
{
  Map res_m;
  size_t len_spec = spec.Length();
  for (size_t idx = 1; idx <= len_spec; idx++) {
#ifdef VDMSL
    if (spec[idx].Is(TAG_TYPE_AS_Module)) {
      if (clmod_s.InSet(spec[idx].GetRecord(pos_AS_Module_nm))) {
        res_m.ImpOverride(ExpandModuleTraces(spec[idx], wos));
      }
    }
#endif // VDMLL
#ifdef VDMPP
    if (spec[idx].Is(TAG_TYPE_AS_Class)) {
      if (clmod_s.InSet(spec[idx].GetRecord(pos_AS_Class_nm))) {
        res_m.ImpOverride(ExpandClassTraces(spec[idx], wos));
      }
    }
#endif // VDMPP
  }
  return res_m;
}

#ifdef VDMSL
// ExpandModuleTraces
// mod : AS`Module
// ==> map AS`Name to set of seq of AS`Expr
Map EXPANDED::ExpandModuleTraces(const TYPE_AS_Module & mod, wostream & wos)
{
  const TYPE_AS_Name & nm (mod.GetRecord(pos_AS_Module_nm));
  const Generic & defsg (mod.GetField(pos_AS_Module_defs));
  if (!defsg.IsNil()) {
    TYPE_AS_Definitions defs (defsg);
    const Map & tracem (defs.GetMap(pos_AS_Definitions_tracem));
    if (!tracem.IsEmpty()) {
      EXPANDED::curcl = nm;
      //Map res (ExpandTraceDefs(nm, tracem));
      Map res (ConvertTestCases(ExpandTraceDefs(nm, tracem, wos), wos));
      EXPANDED::curcl = Nil();
      return res;
    }
  }
  return Map();
}
#endif // VDMSL
#ifdef VDMPP
// ExpandModuleTraces
// cl : AS`Class
// ==> map AS`Name to set of seq of AS`Expr
Map EXPANDED::ExpandClassTraces(const TYPE_AS_Class & cl, wostream & wos)
{
  const TYPE_AS_Name & nm (cl.GetRecord(pos_AS_Class_nm));
  const Generic & defsg (cl.GetField(pos_AS_Class_defs));
  if (!defsg.IsNil()) {
    TYPE_AS_Definitions defs (defsg);
    const Map & tracem (defs.GetMap(pos_AS_Definitions_tracem));
    if (!tracem.IsEmpty()) {
      EXPANDED::curcl = nm;
      TYPE_AS_NewExpr ne;
      ne.Init(nm, SEQ<TYPE_AS_Expr>(), NilContextId);

      Tuple crres (theStackMachine().EvalPrint(ne, Sequence(L"Create Pushed Object")));

      if (!crres.GetRecord(1).Is(TAG_TYPE_STKM_Success)) {
        RTERR::Error(L"ExpandClassTraces", RTERR_INTERNAL_ERROR, Nil(), Nil(), Sequence());
      }
      EXPANDED::curobj = crres.GetRecord(2);
      theStackMachine().SetLastRes(sem_undef);

      //Map res (ExpandTraceDefs(nm, tracem));
      Map res (ConvertTestCases(ExpandTraceDefs(nm, tracem, wos), wos));
      EXPANDED::curcl = Nil();
      EXPANDED::curobj = Nil();
      return res;
    }
  }
  return Map();
}
#endif // VDMPP

// ExpandTraceDefs
// clid : AS`Name
// def_m : map AS`NameList to AS`TraceDefList
// ==> map AS`Name to set of seq of AS`Expr
Map EXPANDED::ExpandTraceDefs(const TYPE_AS_Name & clid, const MAP<TYPE_AS_NameList,
                              const TYPE_AS_TraceDefList> & def_m, wostream & wos)
{
  Set dom_def_m (def_m.Dom());
  Map res;
  Generic nm_l;
  for (bool bb = dom_def_m.First(nm_l); bb; bb = dom_def_m.Next(nm_l)) {
    TYPE_AS_NameList nml (nm_l);
    TYPE_AS_Name nm (AUX::ConstructDoubleName(clid, nml[1]));
    wos << L"Expanding " << ASTAUX::ASName2String(nm) << L" ... ";
    Set e_l_s (ExpandTraceDefList(def_m[nm_l], mk_set(AUX::MkEmptyBlkEnv(sem_read_only)))); 
    res.ImpModify(nm, e_l_s);
    wos << L"done." << endl << flush;
  } 
  return res;
}

// ExpandTraceDefList
// tdef_l : AS`TraceDefList
// ctx_s : set of SEM`BlkEnv
// ==> set of seq of AS`Expr
Set EXPANDED::ExpandTraceDefList(const TYPE_AS_TraceDefList & tdef_l, const SET<TYPE_SEM_BlkEnv> & ctx_s)
{
  Set res_s (mk_set(Sequence()));
  size_t len_tdef_l = tdef_l.Length();
  for (size_t idx = 1; idx <= len_tdef_l; idx++) {
    Set e_l_s (ExpandTraceDef(tdef_l[idx], ctx_s));
    res_s = CombineTraces(res_s, e_l_s);
  }
  return res_s;
}

// ExpandTraceDef
// tdef : AS`TraceDefTerm
// ctx_s : set of SEM`BlkEnv
// ==> set of seq of AS`Expr
Set EXPANDED::ExpandTraceDef(const TYPE_AS_TraceDefTerm & tdef, const SET<TYPE_SEM_BlkEnv> & ctx_s)
{
  switch(tdef.GetTag()) {
    case TAG_TYPE_AS_TraceApplyExpr:       { return ExpandTraceApplyExpr(tdef, ctx_s); }
    case TAG_TYPE_AS_TraceBracketedExpr:   { return ExpandTraceBracketedExpr(tdef, ctx_s); }
    case TAG_TYPE_AS_TracePermuteExpr:     { return ExpandTracePermuteExpr(tdef, ctx_s); }
    case TAG_TYPE_AS_QualifiedTrace:       { return ExpandQualifiedTrace(tdef, ctx_s); }
    case TAG_TYPE_AS_RepeatTrace:          { return ExpandRepeatTrace(tdef, ctx_s); }
    case TAG_TYPE_AS_QualifiedRepeatTrace: { return ExpandQualifiedRepeatTrace(tdef, ctx_s); }
    case TAG_TYPE_AS_TraceDefAltn:         { return ExpandTraceDefAltn(tdef, ctx_s); }
    default: { // error
      return Set();
    }
  }
}

// ExpandTraceApplyExpr
// tdef : AS`TraceApplyExpr
// ctx_s : set of SEM`BlkEnv
// -> set of seq of AS`Expr
Set EXPANDED::ExpandTraceApplyExpr(const TYPE_AS_TraceApplyExpr & tdef, const SET<TYPE_SEM_BlkEnv> & ctx_s)
{
#ifdef VDMPP
  const Generic & obj (tdef.GetField(pos_AS_TraceApplyExpr_obj));
#endif // VDMPP

  const TYPE_AS_Name & op (tdef.GetRecord(pos_AS_TraceApplyExpr_op));
  const SEQ<TYPE_AS_Expr> & args (tdef.GetSequence(pos_AS_TraceApplyExpr_args));

#ifdef VDMSL
  TYPE_AS_ApplyExpr expr;
  expr.Init(op, args, NilContextId);
#endif // VDMSL

#ifdef VDMPP
  Record fct;
  if (obj.IsNil()) {
    fct = op;
  }
  else {
    fct = TYPE_AS_FieldSelectExpr().Init(obj, op, NilContextId);
  }
  TYPE_AS_ApplyExpr expr;
  expr.Init(fct, args, NilContextId);
#endif // VDMPP

  return AddContextToExpr(expr, ctx_s);
}

// ExpandTraceBracketedExpr
// tdef : AS`TraceBracketedExpr
// ctx_s : set of SEM`BlkEnv
// -> set of seq of AS`Expr
Set EXPANDED::ExpandTraceBracketedExpr(const TYPE_AS_TraceBracketedExpr & tdef, const SET<TYPE_SEM_BlkEnv> & ctx_s)
{
  PushCxt(ctx_s);
  Set e_l_s (ExpandTraceDefList(tdef.GetSequence(pos_AS_TraceBracketedExpr_list), 
                                mk_set(AUX::MkEmptyBlkEnv(sem_read_only))));

  Set res;
  Generic e_l;
  for (bool bb = e_l_s.First(e_l); bb; bb = e_l_s.Next(e_l)) {
    res.ImpUnion(AddContextToExprList(e_l, EXPANDED::cxt_s_stack[1]));
  }
  PopCxt();
  return res;
}

// ExpandTracePermuteExpr
// tdef : AS`TracePermuteExpr
// ctx_s : set of SEM`BlkEnv
// -> set of seq of AS`Expr
Set EXPANDED::ExpandTracePermuteExpr(const TYPE_AS_TracePermuteExpr & tdef, const SET<TYPE_SEM_BlkEnv> & ctx_s)
{
  const TYPE_AS_TraceDefList & list (tdef.GetSequence(pos_AS_TracePermuteExpr_list));
  PushCxt(ctx_s);
  Set ps (list.Permute());
  Set e_l_s;
  Generic td_l;
  for (bool bb = ps.First(td_l); bb; bb = ps.Next(td_l)) {
    e_l_s.ImpUnion(ExpandTraceDefList(td_l, mk_set(AUX::MkEmptyBlkEnv(sem_read_only))));
  }

  Set res;
  Generic e_l;
  for (bool cc = e_l_s.First(e_l); cc; cc = e_l_s.Next(e_l)) {
    res.ImpUnion(AddContextToExprList(e_l, EXPANDED::cxt_s_stack[1]));
  }
  PopCxt();
  return res;
}

// ExpandTraceBracketedExpr
// tdef : AS`QualifiedTrace
// ctx_s : set of SEM`BlkEnv
// -> set of seq of AS`Expr
Set EXPANDED::ExpandQualifiedTrace(const TYPE_AS_QualifiedTrace & tdef, const SET<TYPE_SEM_BlkEnv> & ctx_s)
{
  SET<TYPE_SEM_BlkEnv> new_ctx_s (ExpandTraceBind(tdef.GetSequence(pos_AS_QualifiedTrace_binds), ctx_s));
  return ExpandTraceDef(tdef.GetRecord(pos_AS_QualifiedTrace_tdef), new_ctx_s);
}

// ExpandRepeatTrace
// tdef : AS`RepeatTrace
// ctx_s : set of SEM`BlkEnv
// -> set of seq of AS`Expr
Set EXPANDED::ExpandRepeatTrace(const TYPE_AS_RepeatTrace & tdef, const SET<TYPE_SEM_BlkEnv> & ctx_s)
{
  Set expr_l_s (ExpandTraceDef(tdef.GetRecord(pos_AS_RepeatTrace_tdef), ctx_s));
  return ExpandTraceRepeatPat(tdef.GetField(pos_AS_RepeatTrace_pat), expr_l_s);
}

// ExpandQualifiedRepeatTrace
// tdef : AS`QualifiedRepeatTrace
// ctx_s : set of SEM`BlkEnv
// -> set of seq of AS`Expr
Set EXPANDED::ExpandQualifiedRepeatTrace(const TYPE_AS_QualifiedRepeatTrace & tdef, const SET<TYPE_SEM_BlkEnv> & ctx_s)
{
  SET<TYPE_SEM_BlkEnv> new_ctx_s (ExpandTraceBind(tdef.GetSequence(pos_AS_QualifiedTrace_binds), ctx_s));

  PushCxt(new_ctx_s);
  Set expr_l_s (ExpandTraceDef(tdef.GetRecord(pos_AS_QualifiedRepeatTrace_tdef),
                            mk_set(AUX::MkEmptyBlkEnv(sem_read_only))));
  Set e_l_s (ExpandTraceRepeatPat(tdef.GetField(pos_AS_QualifiedRepeatTrace_pat), expr_l_s));

  Set res;
  Generic e_l;
  for (bool bb = e_l_s.First(e_l); bb; bb = e_l_s.Next(e_l)) {
    res.ImpUnion(AddContextToExprList(e_l, EXPANDED::cxt_s_stack[1]));
  }
  PopCxt();
  return res;
}

// ExpandTraceDefAltn
// tdef : AS`TraceDefAltn
// ctx_s : set of SEM`BlkEnv
// -> set of seq of AS`Expr
Set EXPANDED::ExpandTraceDefAltn(const TYPE_AS_TraceDefAltn & tdef, const SET<TYPE_SEM_BlkEnv> & ctx_s)
{
  return ExpandTraceDef(tdef.GetRecord(pos_AS_TraceDefAltn_term), ctx_s).Union(
         ExpandTraceDef(tdef.GetRecord(pos_AS_TraceDefAltn_tdef), ctx_s));
}

// ExpandTraceBind
// bind_l : seq of AS`TraceBind
// ctx_s : set of SEM`BlkEnv
// ==> set of SEM`BlkEnv
SET<TYPE_SEM_BlkEnv> EXPANDED::ExpandTraceBind(const SEQ<TYPE_AS_TraceBind> & bind_l,
                                               const SET<TYPE_SEM_BlkEnv> & ctx_s)
{
  SET<TYPE_SEM_BlkEnv> c_s (ctx_s);
  size_t len_bind_l = bind_l.Length();
  for (size_t idx = 1; idx <= len_bind_l; idx++) {
    const TYPE_AS_TraceBind & bind (bind_l[idx]);
    switch(bind.GetTag()) {
      case TAG_TYPE_AS_LocalTraceBind: { c_s = CombineContext(c_s, ExpandLocalBinding(bind, c_s)); break; }
      case TAG_TYPE_AS_LetTraceBind:   { c_s = CombineContext(c_s, ExpandLetBinding(bind, c_s));   break; }
      case TAG_TYPE_AS_LetBeTraceBind: { c_s = CombineContext(c_s, ExpandLetBeBinding(bind, c_s)); break; }
    }
  }
  return c_s;
}

// ExpandTraceRepeatPat
// regexpr : AS`TraceRepeatPattern
// expr_l_s : set of seq of AS`Expr
// ==> set of seq of AS`Expr
Set EXPANDED::ExpandTraceRepeatPat(const TYPE_AS_TraceRepeatPattern & regexpr, const Set & expr_l_s)
{
  if (regexpr == Int(ANY)) {
    return mk_set(Sequence()).Union(RepeatCombine(expr_l_s, 1, EXPANDED::oneOrMoreMax));
  }
  else if (regexpr == Int(ATLEASTONE)) {
    return RepeatCombine(expr_l_s, 1, EXPANDED::oneOrMoreMax);
  }
  else if (regexpr == Int(POSSIBLY)) {
    return mk_set(Sequence()).Union(expr_l_s);
  }
  else if (regexpr.Is(TAG_TYPE_AS_RepeatFixed)) {
    return ExpandRepeatFixed(regexpr, expr_l_s);
  }
  else if (regexpr.Is(TAG_TYPE_AS_RepeatInterval)) {
    return ExpandRepeatInterval(regexpr, expr_l_s);
  }
  return Set(); // err
}

// ExpandRepeatFixed
// regexpr : AS`RepeatFixed
// expr_l_s : set of seq of AS`Expr
// ==> set of seq of AS`Expr
Set EXPANDED::ExpandRepeatFixed(const TYPE_AS_RepeatFixed & regexpr, const Set & expr_l_s)
{
  int num = (int)(regexpr.GetRecord(pos_AS_RepeatFixed_num).GetRealValue(pos_AS_NumLit_val));
  if (num == 0) {
    return mk_set(Sequence());
  }
  else {
    return RepeatCombine(expr_l_s, num, num);
  }
}

// ExpandRepeatInterval
// regexpr : AS`RepeatInterval
// expr_l_s : set of seq of AS`Expr
// ==> set of seq of AS`Expr
Set EXPANDED::ExpandRepeatInterval(const TYPE_AS_RepeatInterval & regexpr, const Set & expr_l_s)
{
  int low = (int)(regexpr.GetRecord(pos_AS_RepeatInterval_low).GetRealValue(pos_AS_NumLit_val));
  int high = (int)(regexpr.GetRecord(pos_AS_RepeatInterval_high).GetRealValue(pos_AS_NumLit_val));

  int l = ((low == 0) ? 1 : low);
  Set no ((low == 0) ? mk_set(Sequence()) : Set());

  return no.Union(RepeatCombine(expr_l_s, l, high));
}

// ExpandLocalBinding
// bind : AS`LocalTraceBind
// ctx_s : set of SEM`BlkEnv
// ==> set of SEM`BlkEnv
SET<TYPE_SEM_BlkEnv> EXPANDED::ExpandLocalBinding(const TYPE_AS_LocalTraceBind & bind, 
                                                  const SET<TYPE_SEM_BlkEnv> & ctx_s)
{
  const SEQ<TYPE_AS_LocalDef> & def_l (bind.GetSequence(pos_AS_LocalTraceBind_localdefs));
  SET<TYPE_SEM_BlkEnv> c_s (ctx_s);

  size_t len_def_l = def_l.Length();
  for (size_t i = 1; i <= len_def_l; i++) {
    const TYPE_AS_ValueDef & vd (def_l[i]);
    const TYPE_AS_Pattern & pat (vd.GetRecord(pos_AS_ValueDef_pat));
    const TYPE_AS_Expr & expr (vd.GetRecord(pos_AS_ValueDef_val));
    TYPE_STKM_Pattern p (theCompiler().P2P(pat));
    SET<TYPE_SEM_BlkEnv> newc_s;
    Generic cxt;
    for (bool bb = c_s.First(cxt); bb; bb = c_s.Next(cxt)) {
      TYPE_SEM_VAL val (evaluateExpression(expr, cxt));
      newc_s.ImpUnion(PAT::PatternMatch(p, val));
    }
    c_s = CombineContext(c_s, newc_s);
  }
  return c_s;
}

// RepeatCombine
// expr_l_s : set of seq of AS`Expr
// low : nat1
// high : nat1
// ==> set of seq of AS`Expr
Set EXPANDED::RepeatCombine(const Set & expr_l_s, int low, int high)
{
  Set acc_e_l_s;                      // set of seq of AS`Expr
  Set ith_e_l_s (mk_set(Sequence())); // set of seq of AS`Expr
  Set expr_l_s_q (expr_l_s);
  for(int i = 1; i <= high; i++) {
    Set oldith (ith_e_l_s);
    ith_e_l_s.Clear();
    Generic e_l1;
    for (bool bb = oldith.First(e_l1); bb; bb = oldith.Next(e_l1)) {
      Sequence l1 (e_l1);
      Generic e_l2;
      for (bool cc = expr_l_s_q.First(e_l2); cc; cc = expr_l_s_q.Next(e_l2)) {
        ith_e_l_s.Insert(l1.Conc(e_l2));
      }
    }
    if (i >= low) {
      acc_e_l_s.ImpUnion(ith_e_l_s);
    }
  }
  return acc_e_l_s;
}

// ExpandLetBinding
// lbbind : AS`LetTraceBind
// ctx_s : set of SEM`BlkEnv
// -> set of SEM`BlkEnv
SET<TYPE_SEM_BlkEnv> EXPANDED::ExpandLetBinding(const TYPE_AS_LetTraceBind & lbbind,
                                                const SET<TYPE_SEM_BlkEnv> & ctx_s)
{
  const SEQ<TYPE_AS_MultBind> & bind_l_q (lbbind.GetSequence(pos_AS_LetTraceBind_bind));

  Tuple t (TypeBindToSetBind(bind_l_q));
  if (t.GetBoolValue(1)) {
    const SEQ<TYPE_AS_MultBind> & bind_l (t.GetSequence(2));
    size_t len_bind_l = bind_l.Length();
    SET<TYPE_SEM_BlkEnv> res;
    SET<TYPE_SEM_BlkEnv> ctx_s_q (ctx_s); 
    Generic cxt;
    for (bool bb = ctx_s_q.First(cxt); bb; bb = ctx_s_q.Next(cxt)) {
      SEQ<TYPE_STKM_Pattern> pat_l;
      SEQ<TYPE_SEM_VAL> val_l;
      for (size_t i = 1; i <= len_bind_l; i++) {
        const TYPE_AS_MultSetBind & msb (bind_l[i]);
        const SEQ<TYPE_AS_Pattern> & p_l (msb.GetSequence(pos_AS_MultSetBind_pat));
        const TYPE_AS_Expr & expr (msb.GetRecord(pos_AS_MultSetBind_Set));
        TYPE_SEM_VAL val (evaluateExpression(expr, cxt));
        if (val.Is(TAG_TYPE_SEM_SET)) {
          size_t len_p_l = p_l.Length();
          for (size_t j = 1; j <= len_p_l; j++) {
            pat_l.ImpAppend(theCompiler().P2P(p_l[j]));
            val_l.ImpAppend(val);
          }
        }
      }
      Set c_s (PAT::EvalMultBindSeq(pat_l, val_l, Int(DO_PARTITION))); // set of (set of SEM`BlkEnv)
      if (!c_s.IsEmpty()) {
        Generic c2;
        for (bool cc = c_s.First(c2); cc; cc = c_s.Next(c2)) {
          res.Insert(AUX::CombineBlkEnv(c2, cxt)); 
        }
      }
    }
    return res;
  }
  else {
    // error
    RTERR::InitError(L"ExpandLetBinding", RTERR_TYPE_BIND_EVAL, Nil(), Nil(), ASTAUX::GetCid(lbbind), Sequence());
    return Set();
  }
}

// ExpandLetBeBinding
// lbbind : AS`LetBeTraceBind
// ctx_s : set of SEM`BlkEnv
// -> set of SEM`BlkEnv
SET<TYPE_SEM_BlkEnv> EXPANDED::ExpandLetBeBinding(const TYPE_AS_LetBeTraceBind & lbbind,
                                                  const SET<TYPE_SEM_BlkEnv> & ctx_s)
{
  const SEQ<TYPE_AS_MultBind> & bind_l_q (lbbind.GetSequence(pos_AS_LetBeTraceBind_bind));
  const Generic & pred (lbbind.GetField(pos_AS_LetBeTraceBind_stexpr));

  Tuple t (TypeBindToSetBind(bind_l_q));
  if (t.GetBoolValue(1)) {
    const SEQ<TYPE_AS_MultBind> & bind_l (t.GetSequence(2));
    size_t len_bind_l = bind_l.Length();
    SET<TYPE_SEM_BlkEnv> res;
    SET<TYPE_SEM_BlkEnv> ctx_s_q (ctx_s); 
    Generic cxt;
    for (bool bb = ctx_s_q.First(cxt); bb; bb = ctx_s_q.Next(cxt)) {
      SEQ<TYPE_STKM_Pattern> pat_l;
      SEQ<TYPE_SEM_VAL> val_l;
      for (size_t i = 1; i <= len_bind_l; i++) {
        const TYPE_AS_MultSetBind & msb (bind_l[i]);
        const SEQ<TYPE_AS_Pattern> & p_l (msb.GetSequence(pos_AS_MultSetBind_pat));
        const TYPE_AS_Expr & expr (msb.GetRecord(pos_AS_MultSetBind_Set));
        TYPE_SEM_VAL val (evaluateExpression(expr, cxt));
        if (val.Is(TAG_TYPE_SEM_SET)) {
          size_t len_p_l = p_l.Length();
          for (size_t j = 1; j <= len_p_l; j++) {
            pat_l.ImpAppend(theCompiler().P2P(p_l[j]));
            val_l.ImpAppend(val);
          }
        }
      }
      Set c_s (PAT::EvalMultBindSeq(pat_l, val_l, Int(DO_PARTITION))); // set of (set of SEM`BlkEnv)
      if (!c_s.IsEmpty()) {
        if (pred.IsNil()) {
          res.ImpUnion(c_s);
        }
        else {
          Generic c2;
          for (bool cc = c_s.First(c2); cc; cc = c_s.Next(c2)) {
            TYPE_SEM_VAL v (evaluateExpression(pred, AUX::CombineBlkEnv(c2, cxt)));
            if (v.Is(TAG_TYPE_SEM_BOOL))
              if (v.GetBoolValue(pos_SEM_BOOL_v))
                res.Insert(AUX::CombineBlkEnv(c2, cxt)); 
          }
        }
      }
    }
    return res;
  }
  else {
    // error
    RTERR::InitError(L"ExpandLetBinding", RTERR_TYPE_BIND_EVAL, Nil(), Nil(), ASTAUX::GetCid(lbbind), Sequence());
    return Set();
  }
}

// evaluateExpression
// expr : AS`Expr
// cxt : SEM`BlkEnv
TYPE_SEM_VAL EXPANDED::evaluateExpression(const TYPE_AS_Expr & expr, const TYPE_SEM_BlkEnv & cxt)
{
#ifdef VDMSL
  TYPE_SEM_VAL val (theStackMachine().EvaluateExpression(EXPANDED::curcl, expr, cxt));
#endif // VDMSL
#ifdef VDMPP
  TYPE_SEM_VAL val (theStackMachine().EvaluateExpression(EXPANDED::curcl, EXPANDED::curobj, expr, cxt));
#endif // VDMPP
  return val; 
}

// MergeContextSet
// c_s : set of SEM`BlkEnv
// -> SEM`BlkEnv
TYPE_SEM_BlkEnv EXPANDED::MergeContextSet(const SET<TYPE_SEM_BlkEnv> & c_s)
{
  switch(c_s.Card()) {
    case 0: {
      // error
      return TYPE_SEM_BlkEnv();
    }
    case 1: {
      return c_s.GetElem();
    }
    default: {
      return AUX::DistribCombineBlkEnv(c_s).GetElem(); 
      //return AUX::DistribCombineBlkEnv(c_s); 
    }
  } 
  // TODO
  return TYPE_SEM_BlkEnv();
}

// CombineContext 
// c_s : set of SEM`BlkEnv
// c_s2 : set of SEM`BlkEnv
// -> set of SEM`BlkEnv
SET<TYPE_SEM_BlkEnv> EXPANDED::CombineContext(const SET<TYPE_SEM_BlkEnv> & c_s, const SET<TYPE_SEM_BlkEnv> & c_s2)
{
  if (c_s.IsEmpty()) {
    return c_s2;
  }
  else if (c_s2.IsEmpty()) {
    return c_s;
  }
  else {
    Set res;
    Set s1 (c_s);
    Set s2 (c_s2);
    Generic c1;
    for (bool bb = s1.First(c1); bb; bb = s1.Next(c1)) {
      Generic c2;
      for (bool cc = s2.First(c2); cc; cc = s2.Next(c2)) {
        res.Insert(AUX::CombineBlkEnv(c1, c2));
      }
    }
    return res;
  }
}

// AddContextToExpr
// expr : AS`Expr
// cxt_s : set of SEM`BlkEnv
// -> set of seq of AS`Expr
Set EXPANDED::AddContextToExpr(const TYPE_AS_Expr & expr, const SET<TYPE_SEM_BlkEnv> & cxt_s)
{
  if (cxt_s.IsEmpty()) {
    return mk_set(mk_sequence(expr));
  }
  else {
    Set res;
    Set cxt_s_q (cxt_s);
    Generic cxt;
    for (bool bb = cxt_s_q.First(cxt); bb; bb = cxt_s_q.Next(cxt)) {
     TYPE_SEM_BlkEnv env (cxt);
      SEQ<TYPE_AS_ValueDef> def_l (Context2ValShapeL(env.GetMap(pos_SEM_BlkEnv_id_um)));
      if (def_l.IsEmpty()) {
        res.Insert(mk_sequence(expr));
      }
      else {
        res.Insert(mk_sequence(TYPE_AS_LetExpr().Init(def_l, expr, NilContextId)));
      }
    }
    return res;
  }
}

// AddContextToExprList
// e_l : seq of AS`Expr
// cxt_s : set of SEM`BlkEnv
// -> set of seq of AS`Expr
Set EXPANDED::AddContextToExprList(const SEQ<TYPE_AS_Expr> & e_l, const SET<TYPE_SEM_BlkEnv> & cxt_s)
{
  if (cxt_s.IsEmpty()) {
    return mk_set(e_l);
  }
  else {
    Set res;
    Set cxt_s_q (cxt_s);
    Generic cxt;
    for (bool bb = cxt_s_q.First(cxt); bb; bb = cxt_s_q.Next(cxt)) {
      TYPE_SEM_BlkEnv env (cxt);
      SEQ<TYPE_AS_ValueDef> def_l (Context2ValShapeL(env.GetMap(pos_SEM_BlkEnv_id_um)));
      if (def_l.IsEmpty()) {
        res.Insert(e_l);
      }
      else {
        SEQ<TYPE_AS_Expr> expr_l;
        size_t len_e_l = e_l.Length();
        for (size_t i = 1; i <= len_e_l; i++) {
          expr_l.ImpAppend(TYPE_AS_LetExpr().Init(def_l, e_l[i], NilContextId));
        }
        res.Insert(expr_l);
      }
    }
    return res;
  }
}

// CombineTraces
// e_l_s1 : set of seq of AS`Expr
// e_l_s2 : set of seq of AS`Expr
// -> set of seq of AS`Expr
Set EXPANDED::CombineTraces(const Set & e_l_s1, const Set & e_l_s2)
{
  Set res;
  Set s1 (e_l_s1);
  Set s2 (e_l_s2);
  Generic e_l1;
  for (bool bb = s1.First(e_l1); bb; bb = s1.Next(e_l1)) {
    Sequence l1 (e_l1);
    Generic e_l2;
    for (bool cc = s2.First(e_l2); cc; cc = s2.Next(e_l2)) {
      res.Insert(l1.Conc(e_l2));  
    }
  }
  return res;
}

// Context2ValShapeL
// id_m : map AS`Name to SEM`ValTp
// -> seq of AS`ValueDef
SEQ<TYPE_AS_ValueDef> EXPANDED::Context2ValShapeL(const MAP<TYPE_AS_Name, TYPE_SEM_ValTp> & id_m)
{
  SET<TYPE_AS_Name> d (id_m.Dom());
  SEQ<TYPE_AS_ValueDef> res;
  Generic id;
  for (bool bb = d.First(id); bb; bb = d.Next(id)) {
    const TYPE_SEM_ValTp & vt (id_m[id]);
    const TYPE_SEM_VAL & val (vt.GetRecord(pos_SEM_ValTp_val));
    const Generic & tp (vt.GetField(pos_SEM_ValTp_tp));
    TYPE_AS_PatternName pat;
    pat.Init(id, tp, NilContextId);
    TYPE_AS_ValueDef vd;
    vd.Init(pat, tp, SemRec::VAL2Expr(val), Int(NOT_INITIALISED_AS), Bool(true), NilContextId);
    res.ImpAppend(vd);
  }
  return res;
}

// PushCxt
// cxt_s : set of SEM`BlkEnv
// ==> ()
void EXPANDED::PushCxt(const SET<TYPE_SEM_BlkEnv> & cxt_s)
{
  EXPANDED::cxt_s_stack.ImpPrepend(cxt_s);
}

// PopCxt
// ==> ()
void EXPANDED::PopCxt()
{
  EXPANDED::cxt_s_stack.ImpTl();
}

// GetCurClass
// ==> [AS`Name]
Generic EXPANDED::GetCurClass()
{
  return EXPANDED::curcl;
}

// TypeBindToSetBind
// tp : seq of AS'MultBind
// ==> bool * [seq of AS'MultBind]
Tuple EXPANDED::TypeBindToSetBind(const SEQ<TYPE_AS_MultBind> & bind_l)
{
  SEQ<TYPE_AS_MultBind> new_bind_l;
  bool forall = true;
  size_t len_bind_l = bind_l.Length();
  for (size_t idx = 1; (idx <= len_bind_l) && forall; idx++) {
    const TYPE_AS_MultBind & mb (bind_l[idx]);
    switch(mb.GetTag()) {
      case TAG_TYPE_AS_MultTypeBind: {
        Tuple t (TypeToSet(mb.GetRecord(pos_AS_MultTypeBind_tp)));

        if (t.GetBoolValue(1)) {
          SET<TYPE_AS_Expr> e_s (t.GetSet(2));
          SEQ<TYPE_AS_Expr> e_l;
          Generic e;
          for (bool bb = e_s.First(e); bb; bb = e_s.Next(e)) {
            e_l.ImpAppend(e);
          }
          new_bind_l.ImpAppend(TYPE_AS_MultSetBind().Init(mb.GetSequence(pos_AS_MultTypeBind_pat),
                                                          TYPE_AS_SetEnumerationExpr().Init(e_l, NilContextId),
                                                          NilContextId));
        }
        else {
          forall = false;
        }
        break;
      }
      case TAG_TYPE_AS_MultSetBind: {
        new_bind_l.ImpAppend(mb);
        break;
      }
      default: {
        // not occur
        break;
      }
    }
  }
//  if (forall) {
//    return mk_(Bool(true), new_bind_l);
//  }
//  else {
//    return mk_(Bool(false), Nil());
//  }
  return (forall ? mk_(Bool(true), new_bind_l) : mk_(Bool(false), Nil()));
}

// TypeToSet
// tp : AS`Type
// ==> bool * [set of AS`Expr]
Tuple EXPANDED::TypeToSet(const TYPE_AS_Type & tp)
{ 
  switch(tp.GetTag()) {
    case TAG_TYPE_AS_BooleanType: {
      return mk_(Bool(true), mk_set(TYPE_AS_BoolLit().Init(Bool(true), NilContextId),
                                    TYPE_AS_BoolLit().Init(Bool(false), NilContextId)));
    }
    case TAG_TYPE_AS_QuoteType: {
      return mk_(Bool(true), mk_set(tp.GetRecord(pos_AS_QuoteType_lit)));
    }
    case TAG_TYPE_AS_UnionType: {
      const SEQ<TYPE_AS_Type> & tp_l (tp.GetSequence(pos_AS_UnionType_tps));
      bool forall = true;
      SET<TYPE_AS_Expr> e_s;
      size_t len_tp_l = tp_l.Length();
      for (size_t idx = 1; idx <= len_tp_l; idx++) {
        Tuple t (TypeToSet(tp_l[idx]));
        if (t.GetBoolValue(1)) {
          e_s.ImpUnion(t.GetSet(2));
        }
        else {
          forall = false;
        }
      }
      //if (forall)
      //  return mk_(Bool(true), e_s);
      //else
      //  return mk_(Bool(false), Nil());
      return (forall ? mk_(Bool(true), e_s) : mk_(Bool(false), Nil()));
    }
#ifdef VDMSL
    case TAG_TYPE_AS_TypeName: {
      Tuple itd (AUX::IsTypeDef(tp.GetRecord(pos_AS_TypeName_name)));
      if (itd.GetBoolValue(1)) {
        Tuple t (TypeToSet(itd.GetRecord(2)));
        // TODO: inv is not considerd
        return t;
      }
      else {
        return mk_(Bool(false), Nil());
      }
    }
#endif // VDMSL
#ifdef VDMPP
    case TAG_TYPE_AS_TypeName: {
      Tuple itd (AUX::IsTypeDef(tp));
      if (itd.GetBoolValue(1)) {
        Tuple t (TypeToSet(itd.GetField(2)));
        // TODO: inv is not considerd
        return t;
      }
      else {
        return mk_(Bool(false), Nil());
      }
    }
#endif // VDMPP
    default: {
      break;
    }
  }
  return mk_(Bool(false), Nil());
}

// ConvertTestCases
// cases : map AS`Name to set of seq of AS`Expr
// ==> map AS`Name to set of seq of AS`Expr
Map EXPANDED::ConvertTestCases(const Map & cases, wostream & wos)
{
  Map res_m;
  Set dom_cases (cases.Dom());
  Generic nm;
  for (bool bb = dom_cases.First(nm); bb; bb = dom_cases.Next(nm)) {
//    if (trnm.IsNil() || (nm == tpnm))
//    {
      res_m.ImpModify(nm, ConvertTestExprs(cases[nm]));
//    }
  }
  return res_m;
}

// ConvertTestExprs
// expr_l_s : set of seq of AS`Expr
// ==> set of seq of AS`Expr
Set EXPANDED::ConvertTestExprs(const Set & expr_l_s)
{
  TYPE_SEM_BlkEnv emptyenv (AUX::MkEmptyBlkEnv(sem_read_only));
  Set new_e_l_s;
  Set e_l_s (expr_l_s);

  Map cache_ex, cache_ma, cache_vl;

  Generic e_l;
  for (bool cc = e_l_s.First(e_l); cc; cc = e_l_s.Next(e_l)) {
    SEQ<TYPE_AS_Expr> new_expl_l;
    SEQ<TYPE_AS_Expr> expl_l (e_l);
    size_t len_expl_l = expl_l.Length();
    for (size_t idx = 1; idx <= len_expl_l; idx++) {
      TYPE_SEM_BlkEnv env (emptyenv);
      TYPE_AS_Expr e (expl_l[idx]);
      while (e.Is(TAG_TYPE_AS_LetExpr)) {
        const SEQ<TYPE_AS_LocalDef> & def_l (e.GetSequence(pos_AS_LetExpr_localdef));
        size_t len_def_l = def_l.Length();
        for (size_t i = 1; i <= len_def_l; i++) {
          const TYPE_AS_ValueDef & vd (def_l[i]);
          const TYPE_AS_Pattern & pat (vd.GetRecord(pos_AS_ValueDef_pat));
          const TYPE_AS_Expr & expr (vd.GetRecord(pos_AS_ValueDef_val));
          //TYPE_SEM_VAL val (evaluateExpression(expr, env));
          Tuple key (mk_(expr, env));
          if (!cache_ex.DomExists(key)) {
            cache_ex.ImpModify(key, evaluateExpression(expr, env)); 
          }
          const TYPE_SEM_VAL & val (cache_ex[key]);
          //SET<TYPE_SEM_BlkEnv> newc_s (PAT::PatternMatch(p, val));
          //env = AUX::CombineBlkEnv(env, newc_s.GetElem());
          Tuple key2 (mk_(pat, val));
          if (!cache_ma.DomExists(key2)) {
            TYPE_STKM_Pattern p (theCompiler().P2P(pat));
            cache_ma.ImpModify(key2, PAT::PatternMatch(p, val).GetElem());
          }
          env = AUX::CombineBlkEnv(env, cache_ma[key2]);
        }
        e = e.GetRecord(pos_AS_LetExpr_body);
      }
      if (e.Is(TAG_TYPE_AS_ApplyExpr)) {
        const SEQ<TYPE_AS_Expr> & arg (e.GetSequence(pos_AS_ApplyExpr_arg));
        SEQ<TYPE_AS_Expr> arg2;
        size_t len_arg = arg.Length();
        for (size_t i = 1; i <= len_arg; i++) {
          //TYPE_SEM_VAL val (evaluateExpression(arg[i], env));
          //arg2.ImpAppend(SemRec::VAL2Expr(val));
          Tuple key (mk_(arg[i], env));
          if (!cache_vl.DomExists(key)) {
            TYPE_SEM_VAL val (evaluateExpression(arg[i], env));
            cache_vl.ImpModify(key, SemRec::VAL2Expr(val));
          }
          arg2.ImpAppend(cache_vl[key]);
        }
        TYPE_AS_ApplyExpr ae2 (e);
        ae2.SetField(pos_AS_ApplyExpr_arg, arg2);
        new_expl_l.ImpAppend(ae2);
      }
      else {
        // error
        new_expl_l.ImpAppend(e);
      }
    }
    new_e_l_s.Insert(new_expl_l);
  }
  return new_e_l_s;
}
