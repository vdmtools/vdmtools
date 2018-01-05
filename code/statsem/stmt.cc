/***
*  * ID
*  *    $Id: stmt.cc,v 1.78 2006/03/22 07:15:37 vdmtools Exp $
*  * SPECIFICATION VERSION
*  *    stmt.cc: Implementation of stmt.vdm 1.96
***/

#include "statsem.h"
#include "astaux.h"
#include "intconvquotes.h"
#include "settings.h"

//wcout << L"intertp: " << INT2Q::h2gAS(intertp) << endl;

#ifndef NOSS

// ExtractExpectedSDArgType
// sd_tp : REP`TypeRep
// -> REP`TypeRep
TYPE_REP_TypeRep StatSem::ExtractExpectedSDArgType (const TYPE_REP_TypeRep & sd_tp)
{
  Generic seqtp;
  switch(sd_tp.GetTag()) {
    case TAG_TYPE_REP_SeqTypeRep: {
      seqtp = btp_natone;
      break;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tps (sd_tp.GetField(pos_REP_UnionTypeRep_tps));
      seqtp = Nil();
      Generic tp;
      for (bool bb = tps.First(tp); bb; bb = tps.Next(tp)) {
        if (tp.Is(TAG_TYPE_REP_SeqTypeRep)) {
          seqtp = btp_natone;
          break;
        }
      }
      break;
    }
    default: {
      seqtp = Nil();
      break;
    }
  }

  bool hasMap = false;

  switch(sd_tp.GetTag()) {
    case TAG_TYPE_REP_GeneralMapTypeRep:
    case TAG_TYPE_REP_InjectiveMapTypeRep: {
      hasMap = true;
      break;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tps (sd_tp.GetField(pos_REP_UnionTypeRep_tps));
      Generic tp;
      for (bool bb = tps.First(tp); bb; bb = tps.Next(tp)) {
        if (tp.Is(TAG_TYPE_REP_GeneralMapTypeRep) || (tp.Is(TAG_TYPE_REP_InjectiveMapTypeRep))) {
          hasMap = true;
          break;
        }
      }
      break;
    }
  }

/*
  hasMap = (sd_tp.Is(TAG_TYPE_REP_GeneralMapTypeRep) ||
           (sd_tp.Is(TAG_TYPE_REP_InjectiveMapTypeRep)));
  if (!hasMap) {
    if (sd_tp.Is(TAG_TYPE_REP_UnionTypeRep)) {
      TYPE_REP_UnionTypeRep utr (sd_tp);
      SET<TYPE_REP_TypeRep> tps (utr.get_tps());
      Generic tp;
      for (bool bb = tps.First(tp); bb; bb = tps.Next(tp)) {
        if (tp.Is(TAG_TYPE_REP_GeneralMapTypeRep) || (tp.Is(TAG_TYPE_REP_InjectiveMapTypeRep))) {
          hasMap = true;
          break;
        }
      }
    }
  }
*/

  Generic maptp;
  if (hasMap) {
    Generic mtp (ExtractMapType(sd_tp)); // will not be Nil
    Tuple splitMap (SplitMapType(mtp));
    maptp = splitMap.GetField(1);
  }
  else
    maptp = Nil();

  if (seqtp.IsNil() && maptp.IsNil())
    return rep_alltp;
  else if (seqtp.IsNil())
    return maptp;
  else if (maptp.IsNil())
    return seqtp;
  else
    return MergeTypes(maptp, seqtp);
}

// wf_Stmt
// i : TYPE`Ind
// stmt : AS`Stmt
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_Stmt (const Int & i, const TYPE_AS_Stmt & stmt, const TYPE_REP_TypeRep & exptp)
{
//wcout << stmt.GetTag () << endl;
//wcout << INT2Q::h2gAS(stmt) << endl;
  switch (stmt.GetTag ()) {
    case TAG_TYPE_AS_DefStmt:           { return wf_DefStmt (i, stmt, exptp); }
    case TAG_TYPE_AS_LetStmt:           { return wf_LetStmt (i, stmt, exptp); }
    case TAG_TYPE_AS_LetBeSTStmt:       { return wf_LetBeSTStmt (i, stmt, exptp); }
    case TAG_TYPE_AS_AssignStmt:        { return wf_AssignStmt (i, stmt, exptp); }
    case TAG_TYPE_AS_AtomicAssignStmt:  { return wf_AtomicAssignStmt (i, stmt, exptp); }
    case TAG_TYPE_AS_SeqForLoopStmt:    { return wf_SeqForLoopStmt (i, stmt, exptp); }
    case TAG_TYPE_AS_SetForLoopStmt:    { return wf_SetForLoopStmt (i, stmt, exptp); }
    case TAG_TYPE_AS_IndexForLoopStmt:  { return wf_IndexForLoopStmt (i, stmt, exptp); }
    case TAG_TYPE_AS_WhileLoopStmt:     { return wf_WhileLoopStmt (i, stmt, exptp); }
    case TAG_TYPE_AS_CallStmt:          { return wf_CallStmt (i, stmt, exptp); }
    case TAG_TYPE_AS_ReturnStmt:        { return wf_ReturnStmt (i, stmt, exptp); }
    case TAG_TYPE_AS_IfStmt:            { return wf_IfStmt (i, stmt, exptp); }
    case TAG_TYPE_AS_CasesStmt:         { return wf_CasesStmt (i, stmt, exptp); }
    case TAG_TYPE_AS_ErrorStmt:         { return wf_ErrorStmt (i, stmt, exptp); }
    case TAG_TYPE_AS_AlwaysStmt:        { return wf_AlwaysStmt (i, stmt, exptp); }
    case TAG_TYPE_AS_ExitStmt:          { return wf_ExitStmt (i, stmt, exptp); }
    case TAG_TYPE_AS_TrapStmt:          { return wf_TrapStmt (i, stmt, exptp); }
    case TAG_TYPE_AS_RecTrapStmt:       { return wf_RecTrapStmt (i, stmt, exptp); }
    case TAG_TYPE_AS_BlockStmt:         { return wf_BlockStmt (i, stmt, exptp); }
    case TAG_TYPE_AS_NonDetStmt:        { return wf_NonDetStmt (i, stmt, exptp); }
    case TAG_TYPE_AS_SpecificationStmt: { return wf_SpecificationStmt (i, stmt, exptp); }
    case TAG_TYPE_AS_AssertStmt:        { return wf_AssertStmt (i, stmt, exptp); }
    case TAG_TYPE_AS_IdentStmt:         { return wf_IdentStmt (i, stmt, exptp); }
#ifdef VDMPP
    case TAG_TYPE_AS_StartStmt:         { return wf_StartStmt (i, stmt, exptp); }
    case TAG_TYPE_AS_StartListStmt:     { return wf_StartListStmt (i, stmt, exptp); }
    case TAG_TYPE_AS_StopStmt:          { return wf_StopStmt (i, stmt, exptp); }
    case TAG_TYPE_AS_StopListStmt:      { return wf_StopListStmt (i, stmt, exptp); }
#ifdef VICE
    case TAG_TYPE_AS_DurationStmt:      { return wf_DurationStmt (i, stmt, exptp); }
    case TAG_TYPE_AS_CycleStmt:         { return wf_CycleStmt (i, stmt, exptp); }
#endif // VICE
#endif // VDMPP
    default: {
      InternalError (L"wf_Stmt");
      return mk_(Bool (false), rep_alltp);
    }
  }
}

// wf_DefStmt
// i : TYPE`Ind
// vDefStmt : AS`DefStmt
// TYPE_REP_TypeRep : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_DefStmt (const Int & i, const TYPE_AS_DefStmt & vDefStmt, const TYPE_REP_TypeRep & exptp)
{
  const type_dd2PL & defs (vDefStmt.GetSequence(pos_AS_DefStmt_value)); // seq of (PatternBind * Expr)
  const TYPE_AS_Stmt & in_e (vDefStmt.GetRecord(pos_AS_DefStmt_In));

  EnterScope (MAP<TYPE_AS_Name,Tuple>());

  // EXTENDED POS CHECK:
  // Instead of REP`AllTypeRep one could look up the type of each patbind and use this.
//  TYPE_REP_TypeRep expargtp (rep_alltp);

  SET<TYPE_AS_Name> used;
  Bool reswf (true);

  size_t len_defs = defs.Length();
  for (size_t idx = 1; idx <= len_defs; idx++)
  {
    const Tuple & t (defs[idx]); // (PatternBind * Expr)
    const TYPE_AS_PatternBind & patbind (t.GetRecord (1));
    const TYPE_AS_Expr & expr (t.GetRecord (2));

// 20120424 -->
    TYPE_REP_TypeRep expargtp (PatternBind2TypeRep(patbind));
// <-- 20120424

    Tuple infer;
    if (expr.Is(TAG_TYPE_AS_ApplyExpr) &&
        expr.GetRecord(pos_AS_ApplyExpr_fct).Is(TAG_TYPE_AS_Name) &&
        CheckOperationName (expr.GetField(pos_AS_ApplyExpr_fct)))
      // be determined before this point, therefore the contextid of expr is given to call.
      infer = wf_Stmt (i, ApplyToCall (expr), expargtp);
    else
      infer = wf_Expr (i, expr, expargtp);

    const Bool & wf_expr (infer.GetBool(1));
    const TYPE_REP_TypeRep & tp (infer.GetRecord(2));

    Tuple infer2 (wf_PatternBind (i, patbind, tp)); // [bool] * map AS`Name to REP`TypeRep
    const Generic & wf_def (infer2.GetField(1)); // [bool]
    const MAP<TYPE_AS_Name,Tuple> & bd (infer2.GetMap(2));

    SET<TYPE_AS_Name> unused (UsedStateIds(bd.Dom ()));
    Generic stid;
    for (bool bb = unused.First (stid); bb; bb = unused.Next (stid))
    {
      //----------------------------------------------------
      // Error message #18
      // The scope of the state component L"%1" is now hidden
      //----------------------------------------------------
      GenErr(stid, WRN1, 18, mk_sequence(PrintName(stid)));
    }

    if (!(wf_def == Bool(true)))
    {
      //---------------------------------
      // Error message #302
      // Pattern in Def-Stmt cannot match
      //---------------------------------
      GenErr (expr, ERR, 302, Sequence());
      reswf = Bool(false);
    }

    if (tp.Is(TAG_TYPE_REP_UnitTypeRep) && (patbind.Is(TAG_TYPE_AS_PatternName) || patbind.Is(TAG_TYPE_AS_MatchVal)))
    {
      //-------------------------------------------------------
      // Error message #304
      // Rhs of equal definition must return a value to pattern
      //-------------------------------------------------------
      GenErr (expr, ERR, 304, Sequence());
      reswf = Bool(false);
    }

    SET<TYPE_AS_Name> overlap (bd.Dom());
    overlap.ImpIntersect (used);
    Generic n;
    for (bool cc = overlap.First(n); cc; cc = overlap.Next(n))
    {
      //------------------------------
      // Error message #303
      // L"%1" is redefined in Def-Stmt
      //------------------------------
      GenErr (n, WRN1, 303, mk_sequence(PrintName (n)));
    }

    reswf &= wf_expr;
    used.ImpUnion (bd.Dom ());
    UpdateScope (bd);
  }

  Tuple infer3 (wf_Stmt (i, in_e, exptp));
  Bool wf_e (infer3.GetBool(1));
  TYPE_REP_TypeRep stp (infer3.GetRecord(2));

  LeaveScope();

  GetCI().SetTypeInfo(vDefStmt.GetInt(pos_AS_DefStmt_cid), stp);
  return mk_(reswf && wf_e, stp);
}

// ApplyToCall
// arc : AS`ApplyExpr
// ==> AS`CallStmt
TYPE_AS_CallStmt StatSem::ApplyToCall (const TYPE_AS_ApplyExpr & arc)
{
  TYPE_AS_CallStmt res;
  res.Init(Nil(),
           arc.GetRecord(pos_AS_ApplyExpr_fct),
           arc.GetSequence(pos_AS_ApplyExpr_arg),
           arc.GetInt(pos_AS_ApplyExpr_cid));
  return res;
}

// wf_LetStmt
// i : TYPE`Ind
// vLetStmt : AS`LetStmt
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_LetStmt (const Int & i, const TYPE_AS_LetStmt & vLetStmt, const TYPE_REP_TypeRep & exptp)
{
  const SEQ<TYPE_AS_LocalDef> & defs (vLetStmt.GetSequence(pos_AS_LetStmt_localdef));
  const TYPE_AS_Stmt & in_e (vLetStmt.GetRecord(pos_AS_LetStmt_In));

  EnterScope (Map()); //

  Bool reswf (wf_LocalDefs(i, defs));

  Tuple infer (wf_Stmt (i, in_e,exptp ));
  const Bool & wf_e (infer.GetBool(1));
  const TYPE_REP_TypeRep & stp (infer.GetRecord(2));

  LeaveScope();

  GetCI().SetTypeInfo(vLetStmt.GetInt(pos_AS_LetStmt_cid), stp);
  return mk_(reswf && wf_e, stp);
}

// wf_LetBeSTStmt
// i : TYPE`Ind
// stmt : AS`LetBeSTStmt
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_LetBeSTStmt (const Int & i, const TYPE_AS_LetBeSTStmt & stmt, const TYPE_REP_TypeRep & exptp)
{
  const SEQ<TYPE_AS_MultBind> & bind (stmt.GetSequence(pos_AS_LetBeSTStmt_bind));
  const Generic & St (stmt.GetField(pos_AS_LetBeSTStmt_St));
  const TYPE_AS_Stmt & In (stmt.GetRecord(pos_AS_LetBeSTStmt_In));

  // bind
  Tuple infer (wf_MultiBindList (i, bind, true));
  const Generic & wf_sb (infer.GetField(1));
  const MAP<TYPE_AS_Name,Tuple> & bd (infer.GetMap(2));

  if (wf_sb != Bool(true))
  {
    //------------------------------------
    // Error message #305
    // Pattern in Let-Be-Stmt cannot match
    //------------------------------------
    GenErr (stmt, ERR, 305, Sequence());
  }

  Bool reswf (wf_sb == Bool(true));

  EnterScope(bd);

  SET<TYPE_AS_Name> unused (UsedStateIds(bd.Dom ()));
  Generic stid;
  for (bool bb = unused.First (stid); bb; bb = unused.Next (stid))
  {
    //----------------------------------------------------
    // Error message #18
    // The scope of the state component L"%1" is now hidden
    //----------------------------------------------------
    GenErr(stid, WRN1, 18, mk_sequence(PrintName(stid)));
  }

  // pred expr
  if (!St.IsNil()) {
    Tuple infer2 (wf_Expr (i, St, btp_bool));
    const Bool & wf_e (infer2.GetBool(1));
    const TYPE_REP_TypeRep & etp (infer2.GetRecord(2));

    bool stcomp (IsCompatible (i, etp, btp_bool));

    if (!stcomp)
    {
      //-----------------------------------------------
      // Error message #231
      // Predicate for L"%1" is not a boolean expression
      //-----------------------------------------------
      GenErrTp (St, ERR, 231, etp, btp_bool, mk_sequence(SEQ<Char>(L"Let-Be-Stmt")));
    }
    reswf = reswf && wf_e && stcomp;
  }

  // in stmt
  Tuple infer3 (wf_Stmt (i, In, exptp));
  const Bool & wf_st (infer3.GetBool(1));
  const TYPE_REP_TypeRep & sttp (infer3.GetRecord(2));
  reswf = reswf && wf_st;

  LeaveScope();

  GetCI().SetTypeInfo(stmt.GetInt(pos_AS_LetBeSTStmt_cid), sttp);
  return mk_(reswf, sttp);
}

// wf_BlockStmt
// i : TYPE`Ind
// stmt : AS`BlockStmt
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_BlockStmt (const Int & i, const TYPE_AS_BlockStmt & stmt, const TYPE_REP_TypeRep & exptp)
{
  const SEQ<TYPE_AS_AssignDef> & dcls (stmt.GetSequence(pos_AS_BlockStmt_dcls));
  const SEQ<TYPE_AS_Stmt> & stmts (stmt.GetSequence(pos_AS_BlockStmt_stmts));

  TYPE_REP_TypeRep exprettp (mk_REP_UnionTypeRep(mk_set(exptp, rep_unittp, rep_rettp)));

  EnterScope (MAP<TYPE_AS_Name,Tuple>());
  EnterLocalScope (MAP<TYPE_AS_Name,TYPE_REP_TypeRep>());

  Bool reswf (true);

// 20140328 -->
  SET<TYPE_AS_Name> used;
// <-- 20140328

  size_t len_dcls = dcls.Length();
  for (size_t idx = 1; idx <= len_dcls; idx++)
  {
    Tuple infer (wf_Dcl (i, dcls[idx]));
    const Bool & wf_dcl (infer.GetBool(1));
    const MAP<TYPE_AS_Name,Tuple> & dclbd (infer.GetMap(2));

    UpdateScope (dclbd);

    SET<TYPE_AS_Name> unused (UsedStateIds(dclbd.Dom ()));
    if (!unused.IsEmpty())
    {
      Generic stid;
      for (bool cc = unused.First (stid); cc; cc = unused.Next (stid))
      {
        //----------------------------------------------------
        // Error message #18
        // The scope of the state component L"%1" is now hidden
        //----------------------------------------------------
        GenErr(stid, WRN1, 18, mk_sequence(PrintName(stid)));
      }
    }
// 20140328 -->
    SET<TYPE_AS_Name> overlap (dclbd.Dom().ImpIntersect (used));
    if (!overlap.IsEmpty())
    {
      Generic n;
      for (bool dd = overlap.First(n); dd; dd = overlap.Next(n))
      {
        //------------------------------
        // Error message #451
        // L"%1" is redefined in Block-Stmt
        //------------------------------
        GenErr (n, WRN1, 451, mk_sequence(PrintName (n)));
      }
    }
// <-- 20140328
    UpdateLocalScope (dclbd);
    reswf &= wf_dcl;
// 20140328 -->
    used.ImpUnion(dclbd.Dom ());
// <-- 20140328
  }

  Generic earlytp = Nil (); // [REP`TypeRep]
  TYPE_REP_TypeRep restp (rep_unittp);

  size_t len_stmts = stmts.Length ();
  for (size_t index = 1; index < len_stmts; index ++)
  {
    Tuple infer2 (wf_Stmt (i, stmts[index], exprettp));
    const Bool & wf (infer2.GetBool(1));
    const TYPE_REP_TypeRep & tp (infer2.GetRecord(2));

    if (!IsCompatible (POS, rep_unittp, tp))
    {
      //----------------------------------------------------
      // Error message #306
      // Block-Stmt will be terminated before last statement
      //----------------------------------------------------
      GenErr (stmts[index], WRN1, 306, Sequence());

      if (earlytp.IsNil ())
      {
        Generic newres (RemoveUnitType (restp));
        earlytp = (newres.IsNil () ? tp : MergeTypes (newres, tp));
      }
    }

    if (earlytp.IsNil ())
      restp = MergeTypes (restp, tp);

    reswf &= wf;
  }

  Tuple infer3 (stmts.IsEmpty() ? mk_(Bool(true), rep_unittp) : wf_Stmt (i, stmts[len_stmts], exptp));
  const Bool & wf (infer3.GetBool(1));
  const TYPE_REP_TypeRep & tp (infer3.GetRecord(2));

  LeaveLocalScope ();
  LeaveScope ();

  GetCI().SetTypeInfo(stmt.GetInt(pos_AS_BlockStmt_cid), tp);
  if (earlytp.IsNil ())
  {
    Generic newres (RemoveUnitType (restp));
    return mk_(wf && reswf, (newres.IsNil () ? tp : MergeTypes (newres, tp)));
  }
  else
    return mk_(wf && reswf, earlytp);
}

// wf_Dcl
// i : TYPE`Ind
// dcl : AS`AssignDef
// ==> bool * map AS`Name to (REP`TypeRep * nat1)
Tuple StatSem::wf_Dcl (const Int & i, const TYPE_AS_AssignDef & dcl)
{
  const TYPE_AS_Name & var (dcl.GetRecord(pos_AS_AssignDef_var));
  const TYPE_AS_Type & tp  (dcl.GetRecord(pos_AS_AssignDef_tp));
  const Generic & dclinit  (dcl.GetField(pos_AS_AssignDef_dclinit));

  Bool typewf (wf_Type(i, tp));
  TYPE_REP_TypeRep tp_q (TransType(Nil(), tp));

  MAP<TYPE_AS_Name, Tuple> resbd;
  resbd.ImpModify (var, mk_(tp_q, Int(1)));

  Bool reswf (true);
  if (!dclinit.IsNil()) {
    Tuple infer;
    TYPE_AS_Expr dclinit_r (dclinit);
    if (dclinit_r.Is(TAG_TYPE_AS_ApplyExpr) &&
        dclinit_r.GetRecord(pos_AS_ApplyExpr_fct).Is(TAG_TYPE_AS_Name))
    {
      if (!CheckOperationName (dclinit_r.GetField(pos_AS_ApplyExpr_fct)))
        infer = wf_Expr (i, dclinit_r, tp_q);
      else
        infer = wf_Stmt (i, ApplyToCall (dclinit), tp_q);
    }
    else
      infer = wf_Expr (i, dclinit_r, tp_q);

    const Bool & wf (infer.GetBool (1));
    const TYPE_REP_TypeRep & itp (infer.GetRecord (2));

    reswf &= wf;

    Bool icomp (IsCompatible (i, itp, tp_q));
    if (! icomp)
    {
      //-----------------------------------------------------------------------
      // Error message #307
      // Initialization expression is not compatible with defining type of L"%1"
      //-----------------------------------------------------------------------
      GenErrTp (dclinit, ERR, 307, itp, tp_q, mk_sequence(PrintName (var)));
      reswf = Bool (false);
    }
  }
  return mk_(typewf && reswf, resbd);
}

// wf_AssignStmt
// i : TYPE`Ind
// stmt : AS`AssignStmt
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_AssignStmt (const Int & i, const TYPE_AS_AssignStmt & stmt, const TYPE_REP_TypeRep & /*exptp*/)
{
  if (!stmt.Is(TAG_TYPE_AS_AssignStmt))
  {
    //------------------------------------
    // Error message #308
    // State designator is not well-formed
    //------------------------------------
    GenErr (stmt, ERR, 308, Sequence());
    return mk_(Bool(false), rep_unittp);
  }

  const TYPE_AS_StateDesignator & lhs (stmt.GetRecord(pos_AS_AssignStmt_lhs));
  const TYPE_AS_Expr & rhs (stmt.GetRecord(pos_AS_AssignStmt_rhs));

  Tuple infer (wf_StateDesignator (i, lhs, Bool(false)));
  const Bool & wf_sd (infer.GetBool(1));
  const TYPE_REP_TypeRep & sd_tp (infer.GetRecord(2));

  Tuple infer2;
  if (rhs.Is(TAG_TYPE_AS_ApplyExpr) &&
      rhs.GetRecord(pos_AS_ApplyExpr_fct).Is(TAG_TYPE_AS_Name) &&
      CheckOperationName (rhs.GetField(pos_AS_ApplyExpr_fct)))
    infer2 = wf_Stmt (i, ApplyToCall (rhs), sd_tp);
  else
    infer2 = wf_Expr (i, rhs, sd_tp);

  const Bool & wf_s (infer2.GetBool(1));
  const TYPE_REP_TypeRep & s_tp (infer2.GetRecord(2));

#ifdef VDMPP
  if (rhs.Is(TAG_TYPE_AS_ApplyExpr) && rhs.GetRecord(pos_AS_ApplyExpr_fct).Is(TAG_TYPE_AS_FieldSelectExpr))
  {
    const TYPE_AS_FieldSelectExpr & rhs_fct (rhs.GetRecord(pos_AS_ApplyExpr_fct));
    if (rhs_fct.GetRecord(pos_AS_FieldSelectExpr_nm).Is(TAG_TYPE_AS_Name))
    {
      const TYPE_AS_Name & rhs_fct_nm (rhs_fct.GetRecord(pos_AS_FieldSelectExpr_nm));

      Tuple t (wf_Expr(i, rhs_fct.GetRecord(pos_AS_FieldSelectExpr_rec), rep_alltp));
      const Bool & wf (t.GetBool(1));
      const TYPE_REP_TypeRep & tp (t.GetRecord(2));

      if (wf)
      {
        Generic clstp (ExtractObjRefType(tp));
        if (clstp.Is(TAG_TYPE_REP_ObjRefTypeRep))
        {
          TYPE_REP_ObjRefTypeRep otr (clstp);
          Generic lookUp (LookUpInObject(otr.get_nm(), rhs_fct_nm, false, true));
          if (lookUp.Is(TAG_TYPE_SSENV_AccessOpTypeRep))
            InsertOpCall(rhs_fct_nm);
        }
      }
    }
  }
#endif //VDMPP

  Bool comp (true);
  if (!wf_sd)
  {
    //------------------------------------
    // Error message #308
    // State designator is not well-formed
    //------------------------------------
    GenErr (stmt, ERR, 308, Sequence());
  }
  else
  {
    comp = IsCompatible (i, s_tp, sd_tp);
    if (!comp)
    {
      //----------------------------------------------------------
      // Error message #309
      // Rhs not compatible with defining type of state designator
      //----------------------------------------------------------
      GenErrTp (rhs, ERR, 309, s_tp, sd_tp, Sequence());
    }
  }
  GetCI().SetTypeInfo(stmt.GetInt(pos_AS_AssignStmt_cid), rep_unittp);
  return mk_(wf_sd && wf_s && comp, rep_unittp);
}

// wf_AtomicAssignStmt
// i : TYPE`Ind
// stmt : AS`AtomicAssignStmt
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_AtomicAssignStmt (const Int & i,
                                    const TYPE_AS_AtomicAssignStmt & stmt,
                                    const TYPE_REP_TypeRep & exptp)
{
  const SEQ<TYPE_AS_AssignStmt> & stmt_l (stmt.GetSequence(pos_AS_AtomicAssignStmt_atm));

  Bool wf (true);

  size_t len_stmt_l = stmt_l.Length();
  for (size_t idx = 1; idx <= len_stmt_l; idx++)
  {
    Tuple pair (wf_AssignStmt (i, stmt_l[idx], exptp));
    wf &= pair.GetBool (1);
  }
  GetCI().SetTypeInfo(stmt.GetInt(pos_AS_AtomicAssignStmt_cid), rep_unittp);
  return mk_(wf, rep_unittp);
}

// wf_StateDesignator
// i : TYPE`Ind
// stmt : AS`StateDesignator
// use : bool
// ==> bool * REP`TypeRep
Tuple StatSem::wf_StateDesignator (const Int & i, const TYPE_AS_StateDesignator & sd, const Bool & use)
{
  switch (sd.GetTag()) {
    case TAG_TYPE_AS_MapOrSeqRef: {
      const TYPE_AS_StateDesignator & var (sd.GetRecord(pos_AS_MapOrSeqRef_var));
      const TYPE_AS_Expr & expr (sd.GetRecord(pos_AS_MapOrSeqRef_arg));

      Tuple infer (wf_StateDesignator (i, var, use));
      const Bool & wf_v (infer.GetBool (1));
      const TYPE_REP_TypeRep & v_tp (infer.GetRecord (2));

      TYPE_REP_TypeRep expargtp (ExtractExpectedSDArgType(v_tp));

      Tuple infer2 (wf_Expr (i, expr, expargtp));
      const Bool & wf_e (infer2.GetBool (1));
      const TYPE_REP_TypeRep & e_tp (infer2.GetRecord (2));

      SEQ<TYPE_REP_TypeRep> sq;
      sq.ImpAppend (e_tp);

      Tuple ca (CheckApply (i, v_tp, sq, sd, Bool(true)));
      const Bool & wf_comp (ca.GetBool (1));
      const TYPE_REP_TypeRep & res_tp (ca.GetRecord (2));

      GetCI().SetTypeInfo(sd.GetInt(pos_AS_MapOrSeqRef_cid), res_tp);
      return mk_(wf_e && wf_v && wf_comp, res_tp);
    }
    case TAG_TYPE_AS_FieldRef: {
      const TYPE_AS_StateDesignator & var (sd.GetRecord(pos_AS_FieldRef_var));
      const TYPE_AS_Name & sel (sd.GetRecord(pos_AS_FieldRef_sel));

      Tuple infer (wf_StateDesignator (i, var, use));
      const Bool & wf_v (infer.GetBool (1));
      const TYPE_REP_TypeRep & v_tp (infer.GetRecord (2));

      Tuple cfs (CheckFieldSelect (i, v_tp, sel, true));
      const Bool & wf_sel (cfs.GetBool (1));
      const Generic & tp (cfs.GetField (2));

      TYPE_REP_TypeRep res_tp;
      if (!tp.IsSet()) {
        res_tp = tp;
      }
      else {
        SET<TYPE_REP_TypeRep> tp_set (tp);
        if (tp_set.Card() == 1)
          res_tp = tp_set.GetElem();
        else {
          res_tp = mk_REP_UnionTypeRep(tp_set);
        }
      }
      Bool reswf (wf_v && wf_sel);
      GetCI().SetTypeInfo(sd.GetInt(pos_AS_FieldRef_cid), res_tp);
// 20141210 -->
      GetCI().SetTypeInfo(ASTAUX::GetCid(sel), res_tp);
// <-- 20141210
      return mk_(reswf, res_tp);
    }
    case TAG_TYPE_AS_Name: {
#ifdef VDMSL
      Generic tp (LookUpState (sd, use, CUR));
#endif // VDMSL
#ifdef VDMPP
      Generic tp_q (LookUpState (sd, use, CUR));
      Generic tp (CheckAccessCurClass (tp_q));
#endif // VDMPP
      if (tp.IsNil())
      {
        GetCI().SetTypeInfo(sd.GetInt(pos_AS_Name_cid), rep_alltp);
        return mk_(Bool(false), rep_alltp);
      }
      else
      {
        GetCI().SetTypeInfo (sd.GetInt(pos_AS_Name_cid), tp);
        return mk_(Bool(true), tp);
      }
    }
    case TAG_TYPE_AS_NarrowRef: {
// 20151023 -->
      TYPE_REP_TypeRep rep_tp (TransType(Nil(), sd.GetRecord(pos_AS_NarrowRef_type)));

      Tuple infer (wf_StateDesignator(i, sd.GetRecord(pos_AS_NarrowRef_var), use));
      const Bool & wf (infer.GetBool(1));
      const TYPE_REP_TypeRep & tp (infer.GetRecord(2));
#ifdef VDMSL
      Bool iscomp (IsCompatible(i, CurMod(ADD, GetCurMod(), rep_tp), tp));
#endif // VDMPP
#ifdef VDMPP
      Bool iscomp (IsCompatible(i, rep_tp, tp));
#endif // VDMPP
      if (!iscomp)
      {
        //-----------------------------
        // Error message #446
        // Narrow-Expr will always be fail
        //-----------------------------
        GenErrTp (sd, ERR, 446, rep_tp, tp, Sequence());
        return mk_(Bool(false), rep_alltp);
      }
      GetCI().SetTypeInfo (sd.GetInt(pos_AS_NarrowRef_cid), rep_tp);
      return mk_(wf,rep_tp);
// <-- 20151023
    }
#ifdef VDMPP
    case TAG_TYPE_AS_SelfExpr: {
      Tuple infer (wf_Expr (i, sd, rep_alltp));
      return infer;
    }
#endif // VDMPP
    default: {
      // Avoid error C2202: not all control path return a value.
      // with VC++
      return Tuple(2);
    }
  }
}

// wf_IfStmt
// i : TYPE`Ind
// stmt :  AS`IfStmt
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_IfStmt (const Int & i, const TYPE_AS_IfStmt & stmt, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & test (stmt.GetRecord(pos_AS_IfStmt_test));
  const TYPE_AS_Stmt & cons (stmt.GetRecord(pos_AS_IfStmt_cons));
  const SEQ<TYPE_AS_ElseifStmt> & elsif (stmt.GetSequence(pos_AS_IfStmt_elsif));
  const Generic & altn (stmt.GetField(pos_AS_IfStmt_altn));

  // if
  Tuple infer1 (wf_Expr (i,test, btp_bool));
  const Bool & wf_t (infer1.GetBool(1));
  const TYPE_REP_TypeRep & ttp (infer1.GetRecord(2));
  Bool tcomp (IsCompatible (i, ttp, btp_bool));
  if (! tcomp)
  {
    //-----------------------------------------------
    // Error message #115
    // Test expression in L"%1" is not of boolean type
    //-----------------------------------------------
    GenErrTp (test, ERR, 115, ttp, btp_bool, mk_sequence(SEQ<Char>(L"If-Stmt")));
  }

  Bool reswf (wf_t && tcomp);
  TYPE_REP_TypeRep restp;

  // then
  Tuple infer2 (wf_Stmt (i,cons, exptp));
  const Bool & wf_c (infer2.GetBool(1));
  const TYPE_REP_TypeRep & ctp (infer2.GetRecord(2));
  Bool ccomp (IsCompatible (POS, ctp, exptp));
  if (! ccomp)
  {
    //---------------------------------
    // Error message #116
    // then-part is not of correct type
    //---------------------------------
    GenErrTp (cons, ERR, 116, ctp, exptp, Sequence());
  }
  reswf = reswf && wf_c && ccomp;
  restp = ctp;

  // elseif
  if (!elsif.IsEmpty()) {
    Tuple infer3 (wf_ElseIfStmts (i, elsif, exptp));
    const Bool & wf_e (infer3.GetBool(1));
    const Generic & etp (infer3.GetField(2));
    reswf = reswf && wf_e;
    restp = MergeTypes (restp, etp);
  }

  // else
  if (altn.IsNil())
  {
    if (IsCompatible (i, rep_unittp, exptp))
    {
      restp = MergeTypes (restp, rep_unittp);
    }
    else
    {
      //------------------------------------------------------------------------
      // Error message #310
      // operation may return without a value because of the empty
      // else-statement
      //------------------------------------------------------------------------
      GenErrTp(stmt, ERR, 310, rep_unittp, exptp, Sequence());
      reswf = false;
    }
  }
  else
  {
    Tuple infer4 (wf_Stmt (i, altn, exptp));
    const Bool & wf_a (infer4.GetBool (1));
    const Generic & atp (infer4.GetRecord (2));
    Bool ecomp (IsCompatible (POS, atp, exptp));

    if( ! ecomp)
    {
      //---------------------------------
      // Error message #117
      // else-part is not of correct type
      //---------------------------------
      GenErrTp (altn, ERR, 117, atp, exptp, Sequence());
    }
    reswf = reswf && wf_a && ecomp;
    restp = MergeTypes (restp, atp);
  }

  GetCI().SetTypeInfo(stmt.GetInt(pos_AS_IfStmt_cid), restp);
  return mk_(reswf, restp);
}

// wf_ElseIfStmts
// i : TYPE`Ind
// else_l : seq1 of AS`ElseifStmt
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_ElseIfStmts (const Int & i,
                               const SEQ<TYPE_AS_ElseifStmt> & else_l,
                               const TYPE_REP_TypeRep & exptp)
{
  Tuple infer (wf_ElseIfStmt (i, else_l.Hd(), exptp));
  const Bool & wf_e (infer.GetBool (1));
  const TYPE_REP_TypeRep & etp (infer.GetRecord (2));

  if (else_l.Length () == 1)
    return infer;
  else
  {
    Tuple infer2 (wf_ElseIfStmts (i, else_l.Tl(), exptp));
    const Bool & wf_rest (infer2.GetBool (1));
    const TYPE_REP_TypeRep & resttp (infer2.GetRecord (2));
    return mk_(wf_e && wf_rest, MergeTypes (etp, resttp));
  }
}

// wf_ElseIfStmt
// i : TYPE`Ind
// stmt : AS`ElseifStmt
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_ElseIfStmt (const Int & i, const TYPE_AS_ElseifStmt & stmt, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & test (stmt.GetRecord(pos_AS_ElseifStmt_test));
  const TYPE_AS_Stmt & cons (stmt.GetRecord(pos_AS_ElseifStmt_cons));

  // check test
  Tuple infer1 (wf_Expr (i, test, btp_bool));
  const Bool & wf_t (infer1.GetBool(1));
  const TYPE_REP_TypeRep & ttp (infer1.GetRecord(2));

  // check cons
  Tuple infer2 (wf_Stmt (i, cons, exptp));
  const Bool & wf_c (infer2.GetBool(1));
  const TYPE_REP_TypeRep & ctp (infer2.GetRecord(2));

  Bool ecomp (IsCompatible (i, ttp, btp_bool));
  Bool ccomp (IsCompatible (POS, ctp, exptp));

  if (!ecomp)
  {
    //-----------------------------------------------
    // Error message #115
    // Test expression in L"%1" is not of boolean type
    //-----------------------------------------------
    GenErrTp (test, ERR, 115, ttp, btp_bool, mk_sequence(SEQ<Char>(L"Else-If-Stmt")));
  }

  if (!ccomp)
  {
    //-----------------------------------
    // Error message #118
    // elseif-part is not of correct type
    //-----------------------------------
    GenErrTp (cons, ERR, 118, ctp, exptp, Sequence());
  }

  return mk_(wf_t && wf_c && ecomp && ccomp, ctp);
}

// wf_CasesStmt
// i : TYPE`Ind
// stmt : AS`CasesStmt
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_CasesStmt (const Int & i, const TYPE_AS_CasesStmt & stmt, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & sel (stmt.GetRecord(pos_AS_CasesStmt_sel));
  const SEQ<TYPE_AS_CasesStmtAltn> & altns (stmt.GetSequence(pos_AS_CasesStmt_altns));
  const Generic & oth (stmt.GetField(pos_AS_CasesStmt_Others));

  // check sel
  Tuple infer (wf_Expr(i, sel, rep_alltp));
  const Bool & wf_sel (infer.GetBool(1));
  const TYPE_REP_TypeRep & seltp (infer.GetRecord(2));

  // check altns
  Bool reswf (true);
  Generic restp = Nil ();
  size_t len_altns = altns.Length();
  for (size_t idx = 1; idx <= len_altns; idx++)
  {
    const TYPE_AS_CasesStmtAltn & naltn (altns[idx]);
    const SEQ<TYPE_AS_Pattern> & m (naltn.GetSequence(pos_AS_CasesStmtAltn_match));
    const TYPE_AS_Stmt & b (naltn.GetRecord(pos_AS_CasesStmtAltn_body));

    Tuple infer2 (wf_CasesStmtAltn (i, m, b, seltp, exptp));
    const Bool & wf (infer2.GetBool(1));
    const Generic & tp (infer2.GetField(2));

    reswf &= wf;

    if (!tp.IsNil())
      restp = MergeTypes (restp, tp);
  }

  // check others
  if (!oth.IsNil ())
  {
    Tuple infer3 (wf_Stmt (i, oth, exptp));
    const Bool & wf (infer3.GetBool(1));
    const TYPE_REP_TypeRep & tp (infer3.GetRecord(2));

    Bool ocomp (IsCompatible(POS, tp, exptp));
    if (! ocomp)
    {
      //-------------------------------------
      // Error message #119
      // others branch is not of correct type
      //-------------------------------------
      GenErrTp (oth, ERR, 119, tp, exptp, Sequence());
      reswf = false;
    }

    reswf &= wf;
    restp = MergeTypes (restp, tp);
  }
  else
  {
    if (i == POS)
      restp = MergeTypes (restp, rep_unittp);
  }

  if (restp.IsNil ())
    restp = rep_unittp;

  GetCI().SetTypeInfo(stmt.GetInt(pos_AS_CasesStmt_cid), restp);
  return mk_(wf_sel && reswf, restp);
}

// wf_CasesStmtAltn
// i : TYPE`Ind
// m_l : seq1 of AS`Pattern
// b : AS`Stmt
// tp : REP`TypeRep
// exptp : REP`TypeRep
// ==> bool * [REP`TypeRep]
Tuple StatSem::wf_CasesStmtAltn (const Int & i,
                                 const SEQ<TYPE_AS_Pattern> & m_l,
                                 const TYPE_AS_Stmt & b,
                                 const TYPE_REP_TypeRep & tp,
                                 const TYPE_REP_TypeRep & exptp)
{
  MAP<TYPE_AS_Name,Tuple> resbd;
  Bool reswf (true), onewf (false);
  Bool bo (true);

  size_t len_m_l = m_l.Length();
  for (size_t idx = 1; idx <= len_m_l; idx++)
  {
    Tuple infer (wf_Pattern (i, m_l[idx], tp));
    const Generic & wf_p (infer.GetField(1)); // [bool]
    const MAP<TYPE_AS_Name,Tuple> & bd (infer.GetMap(2));

    bool j = (wf_p == bo);
    onewf |= j;
    reswf &= j;

    Set tmp_s (resbd.Dom ().Intersect (bd.Dom ()));
    MAP<TYPE_AS_Name, Tuple> tmp_m;
    Generic id;
    for (bool cont = tmp_s.First (id); cont; cont = tmp_s.Next (id))
      tmp_m.Insert (id, mk_(MergeTypes (resbd[id].GetRecord(1), bd[id].GetRecord(1)),
                            resbd[id].GetInt(2) + bd[id].GetInt(2)));

    resbd.ImpOverride (bd);
    resbd.ImpOverride (tmp_m);

// 20130730 -->

    EnterScope(bd);
    Tuple infer2 (wf_Stmt (i, b, exptp));
    const Bool & wf_s (infer2.GetBool(1));
    reswf = reswf && wf_s;
    LeaveScope();

// <-- 20130730
  }

  if (onewf)
  {
    EnterScope(resbd);

    SET<TYPE_AS_Name> unused (UsedStateIds(resbd.Dom ()));
    Generic stid;
    for (bool bb = unused.First (stid); bb; bb = unused.Next (stid))
    {
      //----------------------------------------------------
      // Error message #18
      // The scope of the state component L"%1" is now hidden
      //----------------------------------------------------
      GenErr(stid, WRN1, 18, mk_sequence(PrintName(stid)));
    }

    Tuple infer2 (wf_Stmt (i, b, exptp));
    const Bool & wf_s (infer2.GetBool(1));
    const TYPE_REP_TypeRep & stp (infer2.GetRecord(2));

    // TODO: the case of mk_(a,a) -> ...
    LeaveScope();

    bool ccomp (IsCompatible(POS, stp, exptp));
    if (!ccomp)
    {
      //----------------------------------------
      // Error message #120
      // Case alternative is not of correct type
      //----------------------------------------
      GenErrTp (b, ERR, 120, stp, exptp, Sequence());
      return mk_(Bool(false), exptp);
    }
    return mk_(reswf && wf_s, stp);
  }
  else
  {
    //------------------------
    // Error message #121
    // Pattern can never match
    //------------------------
    GenErr (m_l[1], ERR, 121, Sequence());
    return mk_(Bool(false), Nil());
  }
}

// wf_SeqForLoopStmt
// i : TYPE`Ind
// stmt : AS`SeqForLoopStmt
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_SeqForLoopStmt (const Int & i,
                                  const TYPE_AS_SeqForLoopStmt & stmt,
                                  const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_PatternBind & cv (stmt.GetRecord(pos_AS_SeqForLoopStmt_cv));
  const TYPE_AS_Expr & fseq      (stmt.GetRecord(pos_AS_SeqForLoopStmt_fseq));
  const TYPE_AS_Stmt & body      (stmt.GetRecord(pos_AS_SeqForLoopStmt_body));

  // check fseq
  Tuple infer (wf_Expr(i, fseq, seq_alltp));
  const Bool & wf_s (infer.GetBool(1));
  const TYPE_REP_TypeRep & stp (infer.GetRecord(2));

  bool cmp (IsCompatible (i, stp, seq_alltp));

  if (!cmp) {
    //---------------------------------------
    // Error message #311
    // Seq-For-Loop-Stmt not used on sequence
    //---------------------------------------
    GenErrTp (fseq, ERR, 311, stp, seq_alltp, Sequence());
  }

  Generic stp_ (cmp ? UnmaskSeqTypeForLoop (i, ExtractSeqType (stp)) : (Generic)rep_alltp);

  Bool reswf (wf_s && cmp);
  Generic match = Bool(false);
  MAP<TYPE_AS_Name,Tuple> bd;
  // Nil means empty sequence
  if (stp_.IsNil ()) {
    //---------------------------------------------------------------------
    // Error message #312
    // Pattern in Seq-For-Loop-Stmt cannot be matched to the empty sequence
    //---------------------------------------------------------------------
    GenErr (stmt, ERR, 312, Sequence());
    reswf = false;
  }
  else {
    Tuple infer2 (wf_PatternBind (i, cv, stp_));
    match = infer2.GetField(1); // [bool]
    bd.ImpOverride(infer2.GetMap(2));
    if (match == Bool(false))
    {
      //-----------------------------------------------
      // Error message #313
      // Pattern in Seq-For-Loop-Stmt cannot be matched
      //-----------------------------------------------
      GenErrTp (stmt, ERR, 313, stp, mk_REP_SeqTypeRep(PatternBind2TypeRep(cv)), Sequence());
    }
    reswf = reswf && !match.IsNil();
  }
  EnterScope (bd);

  SET<TYPE_AS_Name> unused (UsedStateIds(bd.Dom ()));
  if (!unused.IsEmpty()) {
    Generic stid;
    for (bool bb = unused.First (stid); bb; bb = unused.Next (stid))
    { 
      //----------------------------------------------------
      // Error message #18
      // The scope of the state component L"%1" is now hidden
      //----------------------------------------------------
      GenErr(stid, WRN1, 18, mk_sequence(PrintName(stid)));
    }
  }

  Tuple infer3 (wf_Stmt (i, body, exptp));
  const Bool & wf_stmt (infer3.GetBool(1));
  const TYPE_REP_TypeRep & stmttp (infer3.GetRecord(2));
  reswf = reswf && wf_stmt;

  LeaveScope (); // MIAN inserted this.
  GetCI().SetTypeInfo(stmt.GetInt(pos_AS_SeqForLoopStmt_cid), stmttp);
  return mk_(reswf, ((match == Bool(true)) ? stmttp : (match.IsNil() ? rep_alltp : rep_unittp)));
}

// wf_SetForLoopStmt
// i : TYPE`Ind
// stmt : AS`SetForLoopStmt
// sxptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_SetForLoopStmt (const Int & i,
                                  const TYPE_AS_SetForLoopStmt & stmt,
                                  const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Pattern & cv (stmt.GetRecord(pos_AS_SetForLoopStmt_cv));
  const TYPE_AS_Expr & s     (stmt.GetRecord(pos_AS_SetForLoopStmt_fset));
  const TYPE_AS_Stmt & body  (stmt.GetRecord(pos_AS_SetForLoopStmt_body));

  Tuple infer (wf_Expr(i, s, set_alltp));
  const Bool & wf_s (infer.GetBool(1));
  const TYPE_REP_TypeRep & stp (infer.GetRecord(2));
  bool cmp (IsCompatible (i, stp, set_alltp));

  if (!cmp) {
    //----------------------------------
    // Error message #314
    // Set-For-Loop-Stmt not used on set
    //----------------------------------
    GenErrTp (s, ERR, 314, stp, set_alltp, Sequence());
  }

  Generic stp_ (cmp ? UnmaskSetTypeForLoop (i, ExtractSetType (stp)) : (Generic)rep_alltp);

  Bool reswf (wf_s && cmp);
  Generic match = Bool(false);
  MAP<TYPE_AS_Name,Tuple> bd;

  if (stp_.IsNil ()) {
    //------------------------------------------------------------
    // Error message #315
    // Pattern in Set-For-Loop-Stmt cannot be matched to empty set
    //------------------------------------------------------------
    GenErr (stmt, ERR, 315, Sequence());
    reswf = false;
  }
  else {
    Tuple infer2 (wf_Pattern (i, cv, stp_));
    match = infer2.GetField(1); // [bool]
    bd.ImpOverride(infer2.GetField(2));
    if (match == Bool(false)) {
      //-----------------------------------------------
      // Error message #316
      // Pattern in Set-For-Loop-Stmt cannot be matched
      //-----------------------------------------------
      GenErrTp (stmt, ERR, 316, stp, mk_REP_SetTypeRep(Pattern2TypeRep(cv)), Sequence());
    }
    reswf = reswf && !match.IsNil();
  }
  EnterScope (bd);

  SET<TYPE_AS_Name> unused (UsedStateIds(bd.Dom ()));
  if (!unused.IsEmpty()) {
    Generic stid;
    for (bool bb = unused.First (stid); bb; bb = unused.Next (stid)) {
      //-----------------------------------------------------
      // Error message #18
      // The scope of the state component L"%1" is now hidden
      //-----------------------------------------------------
      GenErr(stid, WRN1, 18, mk_sequence(PrintName(stid)));
    }
  }
  Tuple infer3 (wf_Stmt (i, body, exptp));
  const Bool & wf_stmt (infer3.GetBool(1));
  const TYPE_REP_TypeRep & stmttp (infer3.GetRecord(2));
  reswf = reswf && wf_stmt;

  LeaveScope (); // MIAN inserted this.
  GetCI().SetTypeInfo(stmt.GetInt(pos_AS_SetForLoopStmt_cid), stmttp);
  return mk_(reswf, ((match == Bool(true)) ? stmttp : (match.IsNil() ? rep_alltp : rep_unittp)));
}

// wf_IndexForLoopStmt
// i : TYPE`Ind
// stmt : AS`IndexForLoopStmt
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_IndexForLoopStmt (const Int & i,
                                         const TYPE_AS_IndexForLoopStmt & stmt,
                                         const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Name & nm   (stmt.GetRecord(pos_AS_IndexForLoopStmt_cv));
  const TYPE_AS_Expr & lb   (stmt.GetRecord(pos_AS_IndexForLoopStmt_lb));
  const TYPE_AS_Expr & ub   (stmt.GetRecord(pos_AS_IndexForLoopStmt_ub));
  const Generic & By        (stmt.GetField (pos_AS_IndexForLoopStmt_By));
  const TYPE_AS_Stmt & body (stmt.GetRecord(pos_AS_IndexForLoopStmt_body));

  TYPE_REP_TypeRep numtp1 (btp_int);
  // check lb
  Tuple infer1 (wf_Expr (i,lb, btp_int));
  const Bool & wf_lb (infer1.GetBool(1));
  const TYPE_REP_TypeRep & lbtp (infer1.GetRecord(2));
  bool lbcomp (IsCompatible (i, lbtp, btp_int));
  if (!lbcomp) {
    //------------------------------------
    // Error message #317
    // Lower bound is not an integer value
    //------------------------------------
    GenErrTp (lb, ERR, 317, lbtp, btp_int, Sequence());
  }
  else {
    numtp1 = ExtractNumericType (lbtp);
  }
  Bool reswf (wf_lb && lbcomp);

  // check ub
  Tuple infer2 (wf_Expr (i,ub, btp_int));
  const Bool & wf_ub (infer2.GetBool(1));
  const TYPE_REP_TypeRep & ubtp (infer2.GetRecord(2));
  bool ubcomp (IsCompatible (i, ubtp, btp_int));
  if (!ubcomp) {
    //------------------------------------
    // Error message #318
    // Upper bound is not an integer value
    //------------------------------------
    GenErrTp (ub, ERR, 318, ubtp, btp_int, Sequence());
  }
  reswf = reswf && wf_ub && ubcomp;

  // check By
  TYPE_REP_TypeRep numtp2 (btp_int);
  if (!By.IsNil ()) {
    Tuple infer3 (wf_Expr (i, By, btp_natone));
    const Bool & wf_By (infer3.GetBool(1));
    const TYPE_REP_TypeRep & Bytp (infer3.GetRecord(2));
    bool Bycomp (IsCompatible (i, Bytp, btp_int));
    if (!Bycomp) {
      //----------------------------------------
      // Error message #319
      // Step expression is not an integer value
      //----------------------------------------
      GenErrTp (By, ERR, 319, Bytp, btp_int, Sequence());
    }
    else {
      numtp2 = ExtractNumericType (Bytp);
    }
    reswf = reswf && wf_By && Bycomp;
  }

  MAP<TYPE_AS_Name,Tuple> bd;
  bd.Insert (nm, mk_(MostGeneralNumericType (mk_set(numtp1, numtp2)), Int(1)));

  EnterScope (bd);

  SET<TYPE_AS_Name> unused (UsedStateIds(bd.Dom ()));
  Generic stid;
  for (bool bb = unused.First (stid); bb; bb = unused.Next (stid)) {
    //----------------------------------------------------
    // Error message #18
    // The scope of the state component L"%1" is now hidden
    //----------------------------------------------------
    GenErr(stid, WRN1, 18, mk_sequence(PrintName(stid)));
  }

  Tuple infer4 (wf_Stmt(i, body, exptp));
  const Bool & wf_s (infer4.GetBool(1));
  const TYPE_REP_TypeRep & stmttp (infer4.GetRecord(2));
  reswf = reswf && wf_s;

  LeaveScope();
  GetCI().SetTypeInfo(stmt.GetInt(pos_AS_IndexForLoopStmt_cid), stmttp);
  return mk_(reswf, stmttp);
}

// wf_WhileLoopStmt
// i : TYPE`Ind
// stmt : AS`WhileLoopStmt
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_WhileLoopStmt (const Int & i, const TYPE_AS_WhileLoopStmt & stmt, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & Expr (stmt.GetRecord(pos_AS_WhileLoopStmt_test));
  const TYPE_AS_Stmt & Stmt (stmt.GetRecord(pos_AS_WhileLoopStmt_body));

  // check test
  Tuple infer1 (wf_Expr(i,Expr, btp_bool));
  const Bool & wf_e (infer1.GetBool(1));
  const TYPE_REP_TypeRep & etp (infer1.GetRecord(2));

  // check body
  Tuple infer2 (wf_Stmt(i,Stmt, exptp));
  const Bool & wf_st (infer2.GetBool(1));
  const TYPE_REP_TypeRep & sttp (infer2.GetRecord(2));

  bool ecomp (IsCompatible (i, etp, btp_bool));

  if (!ecomp)
  {
    //-----------------------------------------------
    // Error message #115
    // Test expression in L"%1" is not of boolean type
    //-----------------------------------------------
    GenErrTp (Expr, ERR, 115, etp, btp_bool, mk_sequence(SEQ<Char>(L"While-Loop-Stmt")));
  }

  GetCI().SetTypeInfo(stmt.GetInt(pos_AS_WhileLoopStmt_cid), sttp);
  return mk_(wf_e && wf_st && ecomp, sttp);
}

// wf_NonDetStmt
// i : TYPE`Ind
// stmt : AS`NonDetStmt
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_NonDetStmt (const Int & i, const TYPE_AS_NonDetStmt & stmt, const TYPE_REP_TypeRep & exptp)
{
  const SEQ<TYPE_AS_Stmt> & stmt_l (stmt.GetSequence(pos_AS_NonDetStmt_stmts));

  Bool reswf (true);
  TYPE_REP_TypeRep restp (rep_unittp);

  size_t len_stmt_l = stmt_l.Length();
  for (size_t idx = 1; idx <= len_stmt_l; idx++)
  {
    Tuple infer (wf_Stmt (i, stmt_l[idx], exptp));
    reswf &= infer.GetBool (1);
    restp = MergeTypes (restp, infer.GetRecord (2));
  }
  GetCI().SetTypeInfo(stmt.GetInt(pos_AS_NonDetStmt_cid), restp);
  return mk_(reswf, restp);
}

#ifdef VDMSL
// wf_CallStmt
// i : TYPE`Ind
// stmt : AS`CallStmt
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_CallStmt (const Int & i, const TYPE_AS_CallStmt & stmt, const TYPE_REP_TypeRep & /*exptp*/)
{
  const TYPE_AS_Name & nm        (stmt.GetRecord(pos_AS_CallStmt_oprt));
  const SEQ<TYPE_AS_Expr> & args (stmt.GetSequence(pos_AS_CallStmt_args));
  const TYPE_CI_ContextId & cid  (stmt.GetInt(pos_AS_CallStmt_cid));

 // if (!this->fullcheck)
    InsertOpCall(nm);

  Bool wf (true), wf_e (true);

  SEQ<TYPE_REP_TypeRep> etp;
  size_t len_args = args.Length();
  for (size_t idx = 1; idx <= len_args; idx++)
  {
    Tuple infer (wf_Expr (i, args[idx], rep_alltp));
    wf_e &= infer.GetBool(1);
    etp.ImpAppend(infer.GetRecord(2));
  }
  wf &= wf_e;

  Generic nmtp (LookUp (nm, true));
  if (nmtp.IsNil())
    GetCI().SetTypeInfo(nm.GetInt(pos_AS_Name_cid), rep_alltp);
  else
    GetCI().SetTypeInfo(nm.GetInt(pos_AS_Name_cid), nmtp);

  if (nmtp.Is(TAG_TYPE_REP_OpTypeRep) ||
      nmtp.Is(TAG_TYPE_REP_TotalFnTypeRep) ||
      nmtp.Is(TAG_TYPE_REP_PartialFnTypeRep))
  {
    TYPE_REP_TypeRep tr (nmtp);
    SEQ<TYPE_REP_TypeRep> Dom;
    TYPE_REP_TypeRep Rng;
    switch(tr.GetTag()) {
      case TAG_TYPE_REP_OpTypeRep: {
        Dom.ImpConc(tr.GetSequence(pos_REP_OpTypeRep_Dom));
        Rng = tr.GetRecord(pos_REP_OpTypeRep_Rng);
        break;
      }
      case TAG_TYPE_REP_TotalFnTypeRep: {
        Dom.ImpConc(tr.GetSequence(pos_REP_TotalFnTypeRep_fndom));
        Rng = tr.GetRecord(pos_REP_TotalFnTypeRep_fnrng);
        break;
      }
      case TAG_TYPE_REP_PartialFnTypeRep: {
        Dom.ImpConc(tr.GetSequence(pos_REP_PartialFnTypeRep_fndom));
        Rng = tr.GetRecord(pos_REP_PartialFnTypeRep_fnrng);
        break;
      }
    }

    GetCI().SetTypeInfo (cid, Rng);
    Bool cmp (true);
    if (Dom.Length () == etp.Length ())
    {
      size_t len_Dom = Dom.Length();
      for (size_t j = 1; j <= len_Dom; j++)
        cmp &= IsCompatible (i, etp[j], Dom[j]);

      if (!cmp)
      {
        //-----------------------------------------------
        // Error message #320
        // Arguments do not match definition of operation
        //-----------------------------------------------
        GenErrTp (nm, ERR, 320, mk_REP_ProductTypeRep(etp), mk_REP_ProductTypeRep(Dom), Sequence());
        return mk_(Bool(false), Rng);
      }
      else
        return mk_(wf, Rng);
    }
    else
    {
      //-------------------------------------------------
      // Error message #275
      // Operation applied with wrong number of arguments
      //-------------------------------------------------
      GenErr (nm, ERR, 275, Sequence());
      return mk_(Bool(false), Rng);
    }
  }
  else
  {
    //------------------------------
    // Error message #28
    // Operation L"%1" is not defined
    //------------------------------
    GenErr (nm, ERR, 28, mk_sequence(PrintName (nm)));
    return mk_(Bool(false), rep_unittp);
  }
}
#endif //VDMSL

#ifdef VDMPP
// wf_CallStmt
// i : TYPE`Ind
// stmt : AS`CallStmt
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_CallStmt (const Int & i, const TYPE_AS_CallStmt & stmt, const TYPE_REP_TypeRep & /*exptp*/)
{
  const Generic & obj            (stmt.GetField   (pos_AS_CallStmt_obj));
  const TYPE_AS_Name & nm        (stmt.GetRecord  (pos_AS_CallStmt_oprt));
  const SEQ<TYPE_AS_Expr> & args (stmt.GetSequence(pos_AS_CallStmt_args));
  const TYPE_CI_ContextId & cid  (stmt.GetInt     (pos_AS_CallStmt_cid));

//  if (!this->fullcheck)
    InsertOpCall(nm);

  Bool reswf (true);
  SEQ<TYPE_REP_TypeRep> etp;
  Set classes;
  if (!obj.IsNil ())
  {
    Tuple cor (CheckObjectRef (i,obj));
    classes = cor.GetSet(1);
    reswf &= cor.GetBool (2);

#ifdef VICE
    if ((nm == ASTAUX::MkNameFromId(ASTAUX::MkId(L"setPriority"), NilContextId)) &&
        (classes.Card() == 1) &&
        classes.InSet(ASTAUX::MkNameFromId(ASTAUX::MkId(L"CPU"), NilContextId)))
      NotStaticOk();
#endif // VICE

    size_t len_args = args.Length();
    for (size_t idx = 1; idx <= len_args; idx++)
    {
      Tuple infer2 (wf_Expr (i, args[idx], rep_alltp));
      reswf &= infer2.GetBool (1);
// 20090413 -->
      etp.ImpAppend (infer2.GetRecord (2));
//      const Generic & tp (infer2.GetRecord (2));
//      if (tp.IsRecord()) // REP`TypeRep
//        etp.ImpAppend (tp);
//      else               // set of REP`TypeRep
//        etp.ImpAppend (mk_REP_UnionTypeRep(tp));
// <-- 20090413
    }
#ifdef VICE
    StaticRequired();
#endif // VICE
  }
  else
  { // obj.IsNil()
    size_t len_args = args.Length();
    for (size_t idx = 1; idx <= len_args; idx++)
    {
      Tuple infer2 (wf_Expr (i, args[idx], rep_alltp));
      reswf &= infer2.GetBool (1);
      etp.ImpAppend (infer2.GetRecord (2));
    }
    classes.Insert(GetCurClass());
  }

  Tuple infer (CheckOperationCall (i, classes, nm, obj, etp));

  reswf &= infer.GetBool (1);
  TYPE_REP_TypeRep nmtp (infer.GetField (2).IsNil () ? rep_alltp : infer.GetRecord (2));
  TYPE_REP_TypeRep restp (infer.GetRecord (3));

  GetCI().SetTypeInfo (cid, restp);
  GetCI().SetTypeInfo (nm.GetInt(pos_AS_Name_cid), nmtp);
  return mk_(reswf, restp);
}

// CheckObjectRef
// i : TYPE`Ind
// obj : AS`Expr
// ==> set of AS`Name  * bool
Tuple StatSem::CheckObjectRef (const Int & i, const TYPE_AS_Expr & obj)
{
  TYPE_REP_ObjRefTypeRep objtp (mk_REP_ObjRefTypeRep( ASTAUX::MkNameFromVoid ()));

  Tuple infer (wf_Expr (i,obj,objtp));
  const Bool & wf_obj (infer.GetBool(1));
  Generic gtp (ExtractObjRefType (infer.GetRecord (2)));

  if (gtp.IsNil()) {
    if (wf_obj == Bool(true))
      GenErr(obj, ERR, 372, Sequence());
    return mk_(Set(), Bool(false));
  }
  else {
    TYPE_REP_TypeRep tp (gtp);
    switch(tp.GetTag()) {
      case TAG_TYPE_REP_ObjRefTypeRep: {
        SET<TYPE_AS_Name> nm_s;
        nm_s.Insert (tp.GetField(pos_REP_ObjRefTypeRep_nm));
        return mk_(nm_s, wf_obj);
      }
      case TAG_TYPE_REP_UnionTypeRep: {
        SET<TYPE_REP_TypeRep> utps (tp.GetField(pos_REP_UnionTypeRep_tps));
        SET<TYPE_AS_Name> nm_s;
        Generic cls;
        for (bool bb = utps.First(cls); bb; bb = utps.Next (cls))
          nm_s.Insert (TYPE_REP_ObjRefTypeRep (cls).get_nm ());

        return mk_(nm_s, wf_obj);
      }
      default:
        return mk_(Set(), Bool(false)); // dummy
    }
  }
}

// CheckOperationCall
// i : TYPE`Ind
// classes : set of AS`Name
// nm :  AS`Name
// objnm : [AS`Expr]
// etp : seq of REP`TypeRep
// ==> bool * [REP`TypeRep] * REP`TypeRep
Tuple StatSem::CheckOperationCall (const Int & i,
                                        const SET<TYPE_AS_Name> & classes_,
                                        const TYPE_AS_Name & nm,
                                        const Generic & objnm,
                                        const SEQ<TYPE_REP_TypeRep> & etp)
{
  Generic nmtp = Nil();
  bool allwf = true;
  bool onewf = false;
  Generic restp = Nil();

  SEQ<TYPE_AS_Name> wng_parms;
  SEQ<TYPE_AS_Name> args_no_match;
  SEQ<TYPE_AS_Name> not_defined;

  SET<TYPE_AS_Name> classes(classes_);
  Generic cls;
  for (bool bb = classes.First(cls); bb; bb = classes.Next(cls))
  {
    Set optp_q (LookUpOperationName(cls, nm, objnm)); // set of ENV`AccessType

    switch(optp_q.Card()) {
      case 0: {
        allwf = false;
        if (nm.get_ids().Length() == 1)
          not_defined.ImpAppend(cls);
        break;
      }
      case 1: {
        Generic opacc (optp_q.GetElem());
        Generic optp (CheckAccessCurClass (opacc));

        if (optp.IsNil ())
        {
          //--------------------
          // Error #368
          // Access violation
          //--------------------
          SEQ<Char> pn;
          if (objnm.IsNil())
            pn.ImpConc(PrintName (nm));
          else
            pn.ImpConc(PrintName (cls)).ImpConc(SEQ<Char>(L"`")).ImpConc(PrintName (nm));

          GenErr (nm, ERR, 368, mk_sequence(pn));
          allwf = false;
        }
        else if (optp.Is (TAG_TYPE_REP_OpTypeRep) ||
                 optp.Is (TAG_TYPE_REP_PartialFnTypeRep) ||
                 optp.Is (TAG_TYPE_REP_TotalFnTypeRep))
        {
          TYPE_REP_TypeRep tr (optp);
          SEQ<TYPE_REP_TypeRep> Dom;
          switch(tr.GetTag()) {
            case TAG_TYPE_REP_OpTypeRep: {
              Dom.ImpConc(tr.GetSequence(pos_REP_OpTypeRep_Dom));
              break;
            }
            case TAG_TYPE_REP_TotalFnTypeRep: {
              Dom.ImpConc(tr.GetSequence(pos_REP_TotalFnTypeRep_fndom));
              break;
            }
            case TAG_TYPE_REP_PartialFnTypeRep: {
              Dom.ImpConc(tr.GetSequence(pos_REP_PartialFnTypeRep_fndom));
              break;
            }
          }

          Tuple mergedTps (MergeOpTypes(optp, nmtp, restp));
          nmtp = mergedTps.GetRecord(1);
          restp = mergedTps.GetRecord(2);

          bool cmp = true;
          if (Dom.Length () == etp.Length ()) {
            size_t len_Dom = Dom.Length();
            for (size_t index = 1; index <= len_Dom; index++) {
              cmp = cmp && IsCompatible (i, etp[index], Dom[index]);
            }

            if (cmp)
              onewf = true;
            else {
              allwf = false;
              args_no_match.ImpAppend(cls);
            }
          }
          else {
            allwf = false;
            wng_parms.ImpAppend(cls);
          }
        }
        break;
      }
      default: { // optp_q.Card() > 1
        Set reptps;
        Generic b_tp;
        for (bool cc = optp_q.First(b_tp); cc; cc = optp_q.Next(b_tp))
          reptps.Insert(Record(b_tp).GetField(1));

        Tuple t (CheckOverloadedApply(i, reptps, etp, nm, true, false)); // bool * REP`TypeRep

        if (t.GetBoolValue(1))
        {
          const TYPE_REP_TypeRep & opreptp (t.GetRecord(2));
          Record acctp;
          bool found = false;
          for (bool dd = optp_q.First(b_tp); dd && !found; dd = optp_q.Next(b_tp))
          {
            if (Record(b_tp).GetField(1) == opreptp)
            {
              acctp = b_tp;
              found = true;
            }
          }

          Generic access (CheckAccessCurClass(acctp));
          if (access.IsNil ()) {
            //--------------------
            // Error #368
            // Access violation
            //--------------------
            SEQ<Char> pn;
            if (objnm.IsNil())
              pn.ImpConc(PrintName (nm));
            else
              pn.ImpConc(PrintName (cls)).ImpConc(SEQ<Char>(L"`")).ImpConc(PrintName (nm));

            GenErr (nm, ERR, 368, mk_sequence(pn));
            allwf = false;
          }
          else {
            Tuple mergedTps (MergeOpTypes(TYPE_REP_OpTypeRep(opreptp), nmtp, restp));
            nmtp = mergedTps.GetField(1);
            restp = mergedTps.GetField(2);
          }
        }
        break;
      }
    }
  }

  bool reswf ((i == POS) ? onewf : allwf);
  int err_kind (reswf ? WRN1 : ERR);

  if (!args_no_match.IsEmpty())
  {
    size_t len_args_no_match = args_no_match.Length(); 
    for (size_t idx = 1; idx <= len_args_no_match; idx++)
    {
      //--------------------------------------------------------------------
      // Error message #325
      // Arguments do not match definition of operation L"%1" from class "%2"
      //--------------------------------------------------------------------
      GenErr (nm, err_kind, 325, mk_sequence(PrintName (nm), PrintName(args_no_match[idx])));
    }
  }

  if (!wng_parms.IsEmpty())
  {
    size_t len_wng_parms = wng_parms.Length();
    for (size_t idx = 1; idx <= len_wng_parms; idx++)
    {
      //-----------------------------------------------------------------------
      // Error message #326
      // Operation L"%1" from class "%2" applied with wrong number of arguments
      //-----------------------------------------------------------------------
      GenErr (nm, err_kind, 326, mk_sequence(PrintName (nm), PrintName(wng_parms[idx])));
    }
  }

  if (!not_defined.IsEmpty())
  {
    size_t len_not_defined = not_defined.Length();
    for (size_t idx = 1; idx <= len_not_defined; idx++)
    {
      //------------------------------------------------
      // Error message #327
      // The operation L"%1" is not defined in class "%2"
      //------------------------------------------------
      GenErr (nm, err_kind, 327, mk_sequence(PrintName (nm), PrintName(not_defined[idx])));
    }
  }

  if (restp.IsNil())
    return mk_(Bool(reswf), nmtp, rep_alltp);
  else
    return mk_(Bool(reswf), nmtp, restp);
}

// MergeOpTypes
// p_optp : (REP`OpTypeRep | REP`FnTypeRep)
// p_nmtp : [REP`TypeRep]
// p_restp :  [REP`TypeRep]
// -> REP`TypeRep * REP`TypeRep
Tuple StatSem::MergeOpTypes(const TYPE_REP_TypeRep & p_optp, const Generic & p_nmtp, const Generic & p_restp)
{
  TYPE_REP_TypeRep l_nmtp (p_nmtp.IsNil() ? p_optp : MergeTypes(p_nmtp, p_optp));
  switch(p_optp.GetTag()) {
    case TAG_TYPE_REP_OpTypeRep: {
      const TYPE_REP_TypeRep & oprng (p_optp.GetRecord(pos_REP_OpTypeRep_Rng));
      TYPE_REP_TypeRep l_restp (p_restp.IsNil() ? oprng : MergeTypes(p_restp, oprng));
      return mk_(l_nmtp, l_restp);
    }
    case TAG_TYPE_REP_PartialFnTypeRep: {
      const TYPE_REP_TypeRep & fnrng (p_optp.GetRecord(pos_REP_PartialFnTypeRep_fnrng));
      TYPE_REP_TypeRep l_restp (p_restp.IsNil() ? fnrng : MergeTypes(p_restp, fnrng));
      return mk_(l_nmtp, l_restp);
    }
    case TAG_TYPE_REP_TotalFnTypeRep: {
      const TYPE_REP_TypeRep & fnrng (p_optp.GetRecord(pos_REP_TotalFnTypeRep_fnrng));
      TYPE_REP_TypeRep l_restp (p_restp.IsNil() ? fnrng : MergeTypes(p_restp, fnrng));
      return mk_(l_nmtp, l_restp);
    }
    default: {
      return Tuple(2); // dummy
    }
  }
}

#endif // VDMPP

// wf_SpecificationStmt
// i : TYPE`Ind
// stmt : AS`SpecificationStmt
// ==> bool * REP`TypeRep
Tuple StatSem::wf_SpecificationStmt (const Int & i,
                                     const TYPE_AS_SpecificationStmt & stmt,
                                     const TYPE_REP_TypeRep &)
{
  const SEQ<TYPE_AS_ExtInf> & opext (stmt.GetSequence(pos_AS_SpecificationStmt_opext));
  const Generic & oppre (stmt.GetField(pos_AS_SpecificationStmt_oppre));
  const TYPE_AS_Expr & oppost (stmt.GetRecord(pos_AS_SpecificationStmt_oppost));
  const SEQ<TYPE_AS_Error> & excps (stmt.GetSequence(pos_AS_SpecificationStmt_excps));

  Bool wf_ext (SetExt (opext, Set ()));

  Bool wf (wf_ImplOpBody (i, opext, oppre, oppost, excps, MAP<TYPE_AS_Name,TYPE_REP_TypeRep>()));
  GetCI().SetTypeInfo(stmt.GetInt(pos_AS_SpecificationStmt_cid), rep_unittp);
  return mk_(wf_ext && wf, rep_unittp);
}

// wf_AssertStmt
// i : TYPE`Ind
// stmt : AS`AssertStmt
// ==> bool * REP`TypeRep
Tuple StatSem::wf_AssertStmt (const Int & i, const TYPE_AS_AssertStmt & stmt, const TYPE_REP_TypeRep &)
{
  const TYPE_AS_Expr & expr (stmt.GetRecord(pos_AS_AssertStmt_expr));

  Tuple infer (wf_Expr (i, expr, rep_alltp));
  const Bool & reswf (infer.GetBool (1));
  const TYPE_REP_TypeRep & tp (infer.GetRecord(2));

  if (!IsCompatible(i, tp, btp_bool)) {
    //----------------------------------------
    // Error message #336
    // Condition must be a boolean expression
    //----------------------------------------
    GenErrTp (stmt, ERR, 336, tp, btp_bool, Sequence());
    return mk_(Bool(false), rep_unittp);
  }
  return mk_(reswf, rep_unittp);
}

// wf_ImplOpBody
// i : TYPE`Ind
// opext : seq of AS`ExtInf
// oppre : [AS`Expr]
// oppost : AS`Expr
// excps : seq of AS`Error
// valtp_bd : map AS`Name to REP`TypeRep
// ==> bool
Bool StatSem::wf_ImplOpBody(const Int & i,
                            const SEQ<TYPE_AS_ExtInf> & opext,
                            const Generic & oppre,
                            const TYPE_AS_Expr & oppost,
                            const SEQ<TYPE_AS_Error> & excps,
                            const MAP<TYPE_AS_Name,Tuple> & valtp_bd)
{
  Bool wf (true);
  PushContext(Int(PRE));
  size_t len_excps = excps.Length();
  for (size_t idx = 1; idx <= len_excps; idx++)
  {
    const TYPE_AS_Error & err (excps[idx]);
    wf = wf_Pred (i, err.get_cond (), EXCEP).GetBool (1) && wf;
  }
  wf = wf_Pred (i, oppre, PRE).GetBool (1) && wf;
  PopContext();

  EnterScope (valtp_bd);

  PushContext(Int(POST));

  for (size_t id2 = 1; id2 <= len_excps; id2++)
  {
    const TYPE_AS_Error & err (excps[id2]);
    wf = wf_Pred (i, err.get_action (), EXCEP).GetBool (1) && wf;
  }
  wf = wf_Pred (i, oppost, POST).GetBool (1) && wf;

  PopContext();

  LeaveScope();
  return wf;
}

#ifdef VDMPP
// wf_StartStmt
// i : TYPE`Ind
// stmt : AS`StartStmt
// exptp :  REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_StartStmt (const Int & i, const TYPE_AS_StartStmt & stmt, const TYPE_REP_TypeRep & /* exptp */)
{
  const Generic & inst (stmt.GetField(pos_AS_StartStmt_expr));

  TYPE_REP_ObjRefTypeRep objtp (mk_REP_ObjRefTypeRep(ASTAUX::MkNameFromVoid ()));

  Tuple infer (wf_Expr (i,inst, objtp));
  bool reswf = Bool(infer.GetBool (1));
  const TYPE_REP_TypeRep & tp (infer.GetRecord(2));

  if (!IsCompatible(i, tp, objtp)) {
    //--------------------------------------------------------------
    // Error message #331
    // Expression in start statement must return an object reference
    //--------------------------------------------------------------
    GenErrTp (inst, ERR, 331, tp, objtp, Sequence());
    reswf = false;
  }

  Generic gotp (ExtractObjRefType(tp));
  if (gotp.IsRecord())
  {
    TYPE_REP_TypeRep otp (gotp);
    switch(otp.GetTag()) {
      case TAG_TYPE_REP_ObjRefTypeRep: {
        SET<TYPE_AS_Name> cls_s;
        cls_s.Insert(otp.GetRecord(pos_REP_ObjRefTypeRep_nm));
        reswf = CheckThread(i, cls_s) && reswf;
        break;
      }
      case TAG_TYPE_REP_UnionTypeRep: {
        SET<TYPE_REP_TypeRep> utps (otp.GetField(pos_REP_UnionTypeRep_tps));
        Generic g;
        SET<TYPE_AS_Name> cls_s;
        for (bool bb = utps.First(g); bb; bb = utps.Next(g))
          cls_s.Insert(TYPE_REP_ObjRefTypeRep(g).GetRecord(pos_REP_ObjRefTypeRep_nm));
        reswf = CheckThread(i, cls_s) && reswf;
        break;
      }
      default:
        break;
    }
  }
  GetCI().SetTypeInfo(stmt.GetInt(pos_AS_StartStmt_cid), rep_unittp);
  return mk_(Bool(reswf), rep_unittp);
}

// wf_StartListStmt
// i : TYPE`Ind
// stmt :  AS`StartListStmt
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_StartListStmt (const Int & i, const TYPE_AS_StartListStmt & stmt, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & inst (stmt.GetRecord(pos_AS_StartListStmt_expr));

  TYPE_REP_ObjRefTypeRep objtp (mk_REP_ObjRefTypeRep(ASTAUX::MkNameFromVoid ()));
  TYPE_REP_SetTypeRep ExpectedRhsType (mk_REP_SetTypeRep(objtp));

  Tuple infer (wf_Expr (i, inst, ExpectedRhsType));
  bool reswf (infer.GetBoolValue (1));
  const TYPE_REP_TypeRep & tp (infer.GetRecord(2));

  if (!IsCompatible(i, tp, ExpectedRhsType))
  {
    //-------------------------------------------------------------------------
    // Error message #332
    // Expression in startlist statement must return a set of object references
    //-------------------------------------------------------------------------
    GenErrTp (inst, ERR, 332, tp, ExpectedRhsType, Sequence());
    reswf = false;
  }

  Generic gotp (ExtractSetObjRefType(tp));
  if (gotp.IsRecord())
  {
    TYPE_REP_TypeRep otp (gotp);
    switch(otp.GetTag()) {
      case TAG_TYPE_REP_ObjRefTypeRep: {
        SET<TYPE_AS_Name> cls_s;
        cls_s.Insert(otp.GetRecord(pos_REP_ObjRefTypeRep_nm));
        reswf = CheckThread(i, cls_s) && reswf;
        break;
      }
      case TAG_TYPE_REP_UnionTypeRep: {
        SET<TYPE_REP_TypeRep> utps (otp.GetField(pos_REP_UnionTypeRep_tps));
        Generic g;
        SET<TYPE_AS_Name> cls_s;
        for (bool bb = utps.First(g); bb; bb = utps.Next(g))
          cls_s.Insert(TYPE_REP_ObjRefTypeRep(g).get_nm());
        reswf = CheckThread(i, cls_s) && reswf;
        break;
      }
    }
  }
  GetCI().SetTypeInfo(stmt.GetInt(pos_AS_StartListStmt_cid), rep_unittp);
  return mk_(Bool(reswf), rep_unittp);
}

// CheckThread
// i : TYPE`Ind
// classes : set of AS`Name
// ==> bool
bool StatSem::CheckThread(const Int & i, const SET<TYPE_AS_Name> & classes)
{
  bool onewf = false;
  bool allwf = true;

  SET<TYPE_AS_Name> classes_q (classes);
  SEQ<TYPE_AS_Name> ncls;
  Generic cls;
  for (bool bb = classes_q.First(cls); bb; bb = classes_q.Next(cls))
  {
    if (HasThread(cls))
      onewf = true;
    else {
      ncls.ImpAppend(cls);
      allwf = false;
    }
  }

  bool reswf = ((i == POS) ? onewf : allwf);
  if (!reswf)
  {
    for (bool cc = ncls.First(cls); cc; cc = ncls.Next(cls))
    {
      //------------------------------------
      // Error message #333
      // Class L"%1" has no thread definition
      //------------------------------------
      GenErr(cls, ERR, 333, mk_sequence(PrintName(cls)));
    }
  }

  return reswf;
}

// wf_StopStmt
// i : TYPE`Ind
// stmt : AS`StopStmt
// exptp :  REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_StopStmt (const Int & i, const TYPE_AS_StopStmt & stmt, const TYPE_REP_TypeRep & /* exptp */)
{
  const Generic & inst (stmt.GetField(pos_AS_StopStmt_expr));

  TYPE_REP_ObjRefTypeRep objtp (mk_REP_ObjRefTypeRep(ASTAUX::MkNameFromVoid ()));

  Tuple infer (wf_Expr (i,inst, objtp));
  bool reswf = Bool(infer.GetBool (1));
  const TYPE_REP_TypeRep & tp (infer.GetRecord(2));

  if (!IsCompatible(i, tp, objtp)) {
    //--------------------------------------------------------------
    // Error message #452
    // Expression in stop statement must return an object reference
    //--------------------------------------------------------------
    GenErrTp (inst, ERR, 452, tp, objtp, Sequence());
    reswf = false;
  }

  Generic gotp (ExtractObjRefType(tp));
  if (gotp.IsRecord())
  {
    TYPE_REP_TypeRep otp (gotp);
    switch(otp.GetTag()) {
      case TAG_TYPE_REP_ObjRefTypeRep: {
        SET<TYPE_AS_Name> cls_s;
        cls_s.Insert(otp.GetRecord(pos_REP_ObjRefTypeRep_nm));
        reswf = CheckThread(i, cls_s) && reswf;
        break;
      }
      case TAG_TYPE_REP_UnionTypeRep: {
        SET<TYPE_REP_TypeRep> utps (otp.GetField(pos_REP_UnionTypeRep_tps));
        Generic g;
        SET<TYPE_AS_Name> cls_s;
        for (bool bb = utps.First(g); bb; bb = utps.Next(g))
          cls_s.Insert(TYPE_REP_ObjRefTypeRep(g).GetRecord(pos_REP_ObjRefTypeRep_nm));
        reswf = CheckThread(i, cls_s) && reswf;
        break;
      }
      default:
        break;
    }
  }
  GetCI().SetTypeInfo(stmt.GetInt(pos_AS_StopStmt_cid), rep_unittp);
  return mk_(Bool(reswf), rep_unittp);
}

// wf_StopListStmt
// i : TYPE`Ind
// stmt :  AS`StopListStmt
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_StopListStmt (const Int & i, const TYPE_AS_StopListStmt & stmt, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & inst (stmt.GetRecord(pos_AS_StopListStmt_expr));

  TYPE_REP_ObjRefTypeRep objtp (mk_REP_ObjRefTypeRep(ASTAUX::MkNameFromVoid ()));
  TYPE_REP_SetTypeRep ExpectedRhsType (mk_REP_SetTypeRep(objtp));

  Tuple infer (wf_Expr (i, inst, ExpectedRhsType));
  bool reswf (infer.GetBoolValue (1));
  const TYPE_REP_TypeRep & tp (infer.GetRecord(2));

  if (!IsCompatible(i, tp, ExpectedRhsType))
  {
    //-------------------------------------------------------------------------
    // Error message #453
    // Expression in stoplist statement must return a set of object references
    //-------------------------------------------------------------------------
    GenErrTp (inst, ERR, 453, tp, ExpectedRhsType, Sequence());
    reswf = false;
  }

  Generic gotp (ExtractSetObjRefType(tp));
  if (gotp.IsRecord())
  {
    TYPE_REP_TypeRep otp (gotp);
    switch(otp.GetTag()) {
      case TAG_TYPE_REP_ObjRefTypeRep: {
        SET<TYPE_AS_Name> cls_s;
        cls_s.Insert(otp.GetRecord(pos_REP_ObjRefTypeRep_nm));
        reswf = CheckThread(i, cls_s) && reswf;
        break;
      }
      case TAG_TYPE_REP_UnionTypeRep: {
        SET<TYPE_REP_TypeRep> utps (otp.GetField(pos_REP_UnionTypeRep_tps));
        Generic g;
        SET<TYPE_AS_Name> cls_s;
        for (bool bb = utps.First(g); bb; bb = utps.Next(g))
          cls_s.Insert(TYPE_REP_ObjRefTypeRep(g).get_nm());
        reswf = CheckThread(i, cls_s) && reswf;
        break;
      }
    }
  }
  GetCI().SetTypeInfo(stmt.GetInt(pos_AS_StopListStmt_cid), rep_unittp);
  return mk_(Bool(reswf), rep_unittp);
}
#endif //VDMPP

#ifdef VICE
// wf_DurationStmt
// i : TYPE`Ind
// stmt :  AS`DurationStmt
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_DurationStmt(const Int & i, const TYPE_AS_DurationStmt & durstmt, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & dur (durstmt.GetRecord(pos_AS_DurationStmt_num));
  const TYPE_AS_Stmt & stmt (durstmt.GetRecord(pos_AS_DurationStmt_stmt));

  Tuple infer (wf_Expr(i, dur, btp_nat));
  bool wf_dur (infer.GetBoolValue(1));

  if( !(dur.Is(TAG_TYPE_AS_RealLit) || dur.Is(TAG_TYPE_AS_NumLit)) || !wf_dur)
  {
    //-----------------------------------------------------------------------
    // Error message #402
    // This must be a constant positive number
    //-----------------------------------------------------------------------
    GenErr(dur, ERR, 402, Sequence());
    return mk_(Bool(false), rep_unittp);
  }
//  else if (dur.GetReal(pos_AS_RealLit_val).GetValue() < 0)
//  {
//    //-----------------------------------------------------------------------
//    // Error message #371
//    // Duration is not a positive number
//    //-----------------------------------------------------------------------
//    GenErr(dur, ERR, 371, Sequence());
//    return mk_(Bool(false), rep_unittp);
//  }
  else
    return wf_Stmt(i, stmt, exptp);

/*
  TYPE_AS_Expr dur (durstmt.get_num());
  TYPE_AS_Stmt stmt (durstmt.get_stmt());

  Tuple wfnumt (wf_Expr (i, dur, btp_real));
  bool wfnum = Bool(wfnumt.GetField (1));
  Record tp = wfnumt.GetField(2);
  if (wfnum && !IsCompatible(i,tp,btp_real)) {
    //-------------------------------------------------------------------------
    // Error message #373
    // Argument of 'duration' is not a numeric type
    //-------------------------------------------------------------------------
    GenErrTp (durstmt, ERR,373,tp,btp_real, Sequence());
    wfnum = false;
  }

  Tuple wfstmtt = wf_Stmt(i, stmt, exptp);
  bool wfstmt = Bool(wfstmtt.GetField (1));
  bool wf = wfnum && wfstmt;

  return mk_(Bool(wf), (wf ? (Record)wfstmtt.GetField (2) : rep_unittp));
*/
}

Tuple StatSem::wf_CycleStmt(const Int & i, const TYPE_AS_CycleStmt & cystmt, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & num (cystmt.GetRecord(pos_AS_CycleStmt_num));
  const TYPE_AS_Stmt & stmt (cystmt.GetRecord(pos_AS_CycleStmt_stmt));

  Tuple infer (wf_Expr(i, num, btp_int));
  bool  wf_num (infer.GetBoolValue(1));

  if ( !(num.Is(TAG_TYPE_AS_RealLit) || num.Is(TAG_TYPE_AS_NumLit)) || !wf_num)
  {
    //-----------------------------------------------------------------------
    // Error message #402
    // This must be a constant positive number
    //-----------------------------------------------------------------------
    GenErr(num, ERR, 402, Sequence());
    return mk_(Bool(false), rep_unittp);
  }
//  else if (num.GetReal(pos_AS_RealLit_val).GetValue() < 0)
//  {
//    //-----------------------------------------------------------------------
//    // Error message #371
//    // Duration is not a positive number
//    //-----------------------------------------------------------------------
//    GenErr(num ,ERR, 371, Sequence());
//    return mk_(Bool(false), rep_unittp);
//  }
  else
//  {
//    Tuple infer2 (wf_Stmt(i, stmt, exptp));
//    const Bool & wf (infer2.GetBool(1));
//    const TYPE_REP_TypeRep & tp (infer2.GetRecord(2));
//    return mk_(wf && wf_num, tp);
//  }
    return wf_Stmt(i, stmt, exptp);
}

#endif //VICE

// wf_ReturnStmt
// i : TYPE`Ind
// stmt : AS`ReturnStmt
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_ReturnStmt (const Int & i, const TYPE_AS_ReturnStmt & stmt, const TYPE_REP_TypeRep & exptp)
{
  const Generic & val (stmt.GetField(pos_AS_ReturnStmt_val));

  if (val.IsNil())
    return mk_(Bool(true), rep_rettp);
  else
  {
    Tuple res (wf_Expr (i, val, exptp));
    GetCI().SetTypeInfo(stmt.GetInt(pos_AS_ReturnStmt_cid), res.GetRecord(2));
    return res;
  }
}

// wf_AlwaysStmt
// i : TYPE`Ind
// astmt : AS`AlwaysStmt
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_AlwaysStmt (const Int & i, const TYPE_AS_AlwaysStmt & astmt, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Stmt & Post (astmt.GetRecord(pos_AS_AlwaysStmt_Post));
  const TYPE_AS_Stmt & stmt (astmt.GetRecord(pos_AS_AlwaysStmt_body));

  Tuple infer (wf_Stmt(i, Post, exptp));
  const Bool & wfpost (infer.GetBool (1));
  const TYPE_REP_TypeRep & tppost (infer.GetRecord (2));

  InsertTrapId(stmt);

  Tuple infer2 (wf_Stmt(i, stmt, exptp));
  const Bool & wfstmt (infer2.GetBool (1));
  const TYPE_REP_TypeRep & tpstmt (infer2.GetRecord (2));

  LeaveTrapId(stmt);

  Bool reswf (true);
  // commented out by PGL 18-03-08 because this does not work
  // appropriately. One would need either to collect exit
  // information in the parser or do type checking across
  // modules/classes in one go for this to work
  //
  // if (this->fullcheck && ExtractRealExitTypeRep(tpstmt, stmt).IsNil ()) {
  //   //---------------------------------------
  //   // Error message #328
  //   // Body of `always statement' cannot exit
  //   //---------------------------------------
  //   GenErr(stmt, WRN1, 328, Sequence());
  //   reswf = Bool (true);
  // }

  GetCI().SetTypeInfo(ASTAUX::GetCid(Post), tppost);
  GetCI().SetTypeInfo(ASTAUX::GetCid(stmt), tpstmt);
  return mk_(wfpost && wfstmt && reswf, AlwaysType(tppost,tpstmt));
}

// wf_ExitStmt
// i : TYPE`Ind
// stmt :  AS`ExitStmt
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_ExitStmt (const Int & i, const TYPE_AS_ExitStmt & stmt, const TYPE_REP_TypeRep & exptp)
{
  if (GetContext() == PUREOP) {
    //-----------------------------------------
    // Error message #459
    // Pure operation cannot exit
    //-----------------------------------------
    GenErr(stmt, ERR, 459, Sequence());
    return mk_(Bool(false), rep_alltp);
  }

  const Generic & e (stmt.GetRecord(pos_AS_ExitStmt_expr));

  if (e.IsNil ()) {
    GetCI().SetTypeInfo(ASTAUX::GetCid(stmt), mk_REP_ExitTypeRep(rep_unittp));
    return mk_(Bool (true), mk_REP_ExitTypeRep(rep_unittp));
  }
  else {
    Tuple infer (wf_Expr(i, e, rep_alltp));
    const Bool & wf (infer.GetBool (1));
    const TYPE_REP_TypeRep & tp (infer.GetRecord (2));

    InsertExitType(tp);

    GetCI().SetTypeInfo(ASTAUX::GetCid(stmt), mk_REP_ExitTypeRep(tp));
    return mk_(wf, mk_REP_ExitTypeRep(tp));
  }
}

// wf_TrapStmt
// i : TYPE`Ind
// stmt :  AS`TrapStmt
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_TrapStmt (const Int & i, const TYPE_AS_TrapStmt & tstmt, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_PatternBind & patbind (tstmt.GetRecord(pos_AS_TrapStmt_pat));
  const TYPE_AS_Stmt & Post (tstmt.GetRecord(pos_AS_TrapStmt_Post));
  const TYPE_AS_Stmt & stmt (tstmt.GetRecord(pos_AS_TrapStmt_body));

  InsertTrapId(stmt);

  Tuple infer (wf_Stmt(i, stmt, exptp));
  const Bool & wfstmt (infer.GetBool (1));
  const TYPE_REP_TypeRep & tpstmt (infer.GetRecord (2));

  LeaveTrapId(stmt);

  if (!this->fullcheck)
  {
    Tuple infer2 (wf_PatternBind (POS, patbind, rep_alltp)); // [bool] * map AS`Name to 9REP`TypeRep * nat1)
    const Generic & match (infer2.GetField(1)); // [bool]
    const MAP<TYPE_AS_Name,Tuple> & bd (infer2.GetMap (2));
    if (match == Bool(false))
    {
      //-----------------------------------------
      // Error message #330
      // Pattern in `trap statement' cannot match
      //-----------------------------------------
      GenErr(stmt, ERR, 330, Sequence());
    }

    EnterScope(bd);
    
    SET<TYPE_AS_Name> unused (UsedStateIds(bd.Dom ()));
    Generic stid;
    for (bool bb = unused.First (stid); bb; bb = unused.Next (stid))
    {
      //----------------------------------------------------
      // Error message #18
      // The scope of the state component L"%1" is now hidden
      //----------------------------------------------------
      GenErr(stid, WRN1, 18, mk_sequence(PrintName(stid)));
    }

    Tuple infer3 (wf_Stmt(i, Post, exptp));
    const Bool & wfpost (infer3.GetBool (1));
    const TYPE_REP_TypeRep & tppost (infer3.GetRecord (2));

    if (!IsCompatible(i, tppost, exptp))
    {
      // -------------------------------
      // -- Error message #397
      // -- Value is not type-compatible
      // -------------------------------
      GenErrTp (Post, ERR, 397, tppost, exptp, Sequence());
    }

    LeaveScope();

    GetCI().SetTypeInfo(ASTAUX::GetCid(tstmt), MergeTypes (tppost, tpstmt));
    return mk_(wfstmt && wfpost, MergeTypes (tppost, tpstmt));
  }
  else
  {
    // full check
    Bool reswf (true);

// 20081006 -->
    //Generic etp (ExtractRealExitTypeRep(tpstmt, stmt));
    //if (etp.IsNil())
    //{
    //  //----------------------------------------------------
    //  // Error message #329
    //  // Body of `trap statement' cannot exit
    //  //----------------------------------------------------
    //  GenErr(stmt, WRN1, 329, Sequence());
    //  reswf = Bool(false);
    //}
// <-- 20081006

    Tuple infer2 (wf_PatternBind (POS, patbind, rep_alltp)); // [bool] * map AS`Name to REP`TypeRep
    const Generic & match (infer2.GetField (1)); // [bool]
    const MAP<TYPE_AS_Name,Tuple> & bd (infer2.GetMap (2));

    if (match == Bool(false))
    {
      //-----------------------------------------
      // Error message #330
      // Pattern in `trap statement' cannot match
      //-----------------------------------------
      GenErr(stmt, ERR, 330, Sequence());
    }

    EnterScope(bd);

    SET<TYPE_AS_Name> unused (UsedStateIds(bd.Dom ()));
    Generic stid;
    for (bool bb = unused.First (stid); bb; bb = unused.Next (stid))
    {
      //----------------------------------------------------
      // Error message #18
      // The scope of the state component L"%1" is now hidden
      //----------------------------------------------------
      GenErr(stid, WRN1, 18, mk_sequence(PrintName(stid)));
    }

    Tuple infer3 (wf_Stmt(i, Post, exptp));
    const Bool & wfpost (infer3.GetBool (1));
    const TYPE_REP_TypeRep & tppost (infer3.GetRecord (2));

    LeaveScope();
    GetCI().SetTypeInfo(ASTAUX::GetCid(tstmt), MergeTypes (tppost, tpstmt));
    return mk_(wfstmt && (match != Bool(false)) && wfpost && reswf, MergeTypes (tpstmt, tppost));
  }
}

// wf_RecTrapStmt
// i : TYPE`Ind
// rtstmt : AS`RecTrapStmt
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_RecTrapStmt (const Int & i, const TYPE_AS_RecTrapStmt & rtstmt, const TYPE_REP_TypeRep & exptp)
{
  const SEQ<TYPE_AS_Trap> & trap_l (rtstmt.GetSequence(pos_AS_RecTrapStmt_traps));
  const TYPE_AS_Stmt & stmt (rtstmt.GetRecord(pos_AS_RecTrapStmt_body));

  InsertTrapId(stmt);

  Tuple infer (wf_Stmt(i, stmt, exptp));
  const Bool & wfstmt (infer.GetBool (1));
  const TYPE_REP_TypeRep & tpstmt (infer.GetRecord (2));

  LeaveTrapId(stmt);

  Bool reswf (true);
  Generic etp = Nil ();
  // commented out by PGL 18-03-08 because this does not work
  // appropriately. One would need either to collect exit
  // information in the parser or do type checking across
  // modules/classes in one go for this to work
  //
  // if (this->fullcheck)
  // {
  //   etp = ExtractRealExitTypeRep (tpstmt, stmt);
  //   if (etp.IsNil ()) {
  //     //-------------------------------------
  //     // Error message #329
  //     // Body of `trap statement' cannot exit
  //     //-------------------------------------
  //     GenErr(stmt, WRN1, 329, Sequence());
  //     reswf = Bool (true);
  //     etp = rep_alltp;
  //   }
  // }
// 20100730 -->
//  if (this->fullcheck)
//    etp = ExtractRealExitTypeRep (tpstmt, stmt);
  if (etp.IsNil ())
    etp = rep_alltp;
// <-- 20100730

// 20100730 -->
//  TYPE_REP_TypeRep restp (rep_unittp);
  TYPE_REP_TypeRep restp (tpstmt);
// <-- 20100730

  size_t len_trap_l = trap_l.Length();
  for (size_t idx = 1; idx <= len_trap_l; idx++)
  {
    const TYPE_AS_Trap & trap (trap_l[idx]);
    const TYPE_AS_PatternBind & match (trap.GetRecord(pos_AS_Trap_match));
    const TYPE_AS_Stmt & trappost (trap.GetRecord(pos_AS_Trap_trappost));

//    Tuple infer2 (wf_TrapAltn(i, match, trappost, (etp.IsNil () ? (Generic)rep_alltp : etp), exptp));
    Tuple infer2 (wf_TrapAltn(i, match, trappost, etp, exptp));

    const Bool & wf (infer2.GetBool (1));
    const Generic & tp (infer2.GetField (2));

    if (!IsCompatible(i, tp, exptp))
    {
      // -------------------------------
      // -- Error message #397
      // -- Value is not type-compatible
      // -------------------------------
      GenErrTp (trappost, ERR, 397, tp, exptp, Sequence());
    }

    reswf &= wf;

    if (!tp.IsNil ())
      restp = MergeTypes (restp, tp);
  }

  GetCI().SetTypeInfo(ASTAUX::GetCid(rtstmt), restp);
  if (!restp.Is (TAG_TYPE_REP_UnitTypeRep))
    return mk_(wfstmt && reswf, RemoveUnitType (restp));
  else
    return mk_(wfstmt && reswf, restp);
}

// wf_TrapAltn
// i : TYPE`Ind
// patbind : AS`PatternBind
// body : AS`Stmt
// tp : REP`TypeRep
// exptp : REP`TypeRep
// ==> bool * [REP`TypeRep]
Tuple StatSem::wf_TrapAltn (const Int & i,
                                 const TYPE_AS_PatternBind & patbind,
                                 const TYPE_AS_Stmt & body,
                                 const TYPE_REP_TypeRep & tp,
                                 const TYPE_REP_TypeRep & exptp)
{
  Tuple infer (wf_PatternBind ((tp.Is (TAG_TYPE_REP_AllTypeRep) ? POS : i), patbind, tp));
  const Generic & wf_pb (infer.GetField (1)); // [bool]
  const MAP<TYPE_AS_Name,Tuple> & bd (infer.GetMap (2));

  if (wf_pb == Bool (true))
  {
    EnterScope (bd);

    SET<TYPE_AS_Name> unused (UsedStateIds(bd.Dom ()));
    Generic stid;
    for (bool bb = unused.First (stid); bb; bb = unused.Next (stid))
    {
      //----------------------------------------------------
      // Error message #18
      // The scope of the state component L"%1" is now hidden
      //----------------------------------------------------
      GenErr(stid, WRN1, 18, mk_sequence(PrintName(stid)));
    }

    Tuple infer2 (wf_Stmt (i, body, exptp));
    const Bool & wf_s (infer2.GetBool (1));
    const TYPE_REP_TypeRep & stp (infer2.GetRecord (2));

    LeaveScope();

    return mk_(wf_s, stp);
  }
  else {
    if (wf_pb == Bool (false))
    {
      //------------------------
      // Error message #121
      // Pattern can never match
      //------------------------
      GenErr (patbind, WRN1, 121, Sequence());
    }

    return mk_(Bool (false), Nil ());
  }
}

// SetOpContext
// rm : AS`Name
void StatSem::SetOpContext (const TYPE_AS_Name & nm)
{
  this->curop = nm;
  this->trapids = SET<TYPE_AS_Stmt>();
  this->traps = false;
}

// AnyTraps
// ==> bool
bool StatSem::AnyTraps () const
{
  return this->traps;
}

// FullOpTest
// b : bool
void StatSem::FullOpTest (bool b)
{
  this->fullcheck = b;
}

// InsertExitType
// tp : REP`TypeRep
void StatSem::InsertExitType (const TYPE_REP_TypeRep & tp)
{
  if (this->direxit.DomExists (this->curop))
    this->direxit.ImpModify (this->curop, MergeTypes (this->direxit [this->curop],tp));
  else
    this->direxit.Insert (this->curop, tp);
}

// InsertTrapId
// stmt : AS`Stmt
void StatSem::InsertTrapId (const TYPE_AS_Stmt & stmt)
{
  this->trapids.Insert (stmt);
  this->traps = true;
}

// LeaveTrapId
// stmt : AS`Stmt
void StatSem::LeaveTrapId (const TYPE_AS_Stmt & stmt)
{
  this->trapids.ImpDiff(mk_set(stmt));
}

// InsertOpCall
// opnm : AS`Name
void StatSem::InsertOpCall (const TYPE_AS_Name & opnm)
{
  SET<TYPE_AS_Name> s1 (mk_set(opnm));

  SET<TYPE_AS_Stmt> tids (this->trapids);
  Generic trapid;
  for (bool bb = tids.First (trapid); bb; bb = tids.Next (trapid))
  {
    Tuple t (mk_(this->curop, trapid));
    SET<TYPE_AS_Name> s (s1);
    if (this->trapopcalls.DomExists (t))
      s.ImpUnion (this->trapopcalls [t]);
    this->trapopcalls.ImpModify (t, s);
  }

  if (this->opcalls.DomExists (this->curop))
    s1.ImpUnion (this->opcalls [this->curop]);
  this->opcalls.ImpModify (this->curop, s1);
}

// ExtractRealExitTypeRep
// tp : REP`TypeRep
// stmt : AS`Stmt
// ==> [REP`TypeRep]
Generic StatSem::ExtractRealExitTypeRep(const TYPE_REP_TypeRep & tp, const TYPE_AS_Stmt & stmt)
{
  Generic etp (ExtractExitTypeRep (tp));
  Tuple t (mk_(this->curop, stmt));
  if (this->trapopcalls.DomExists (t)) {
    Generic etp_q (TransClosExit (this->trapopcalls [t], Set ()));
    if (etp.IsNil ())
      return etp_q;
    else if (etp_q.IsNil ())
      return etp;
    else
      return MergeTypes(etp_q, etp);
  }
  else if (this->opcalls.DomExists (this->curop)) {
    Generic etp_q (TransClosExit (this->opcalls [this->curop], Set ()));
    if (etp.IsNil ())
      return etp_q;
    else if (etp_q.IsNil ())
      return etp;
    else
      return MergeTypes(etp_q, etp);
  }
  else
    return etp;
}

// TransClosExit
// nms : set of AS`Name
// already : set of AS`Name
// ==> [REP`TypeRep]
Generic StatSem::TransClosExit(const SET<TYPE_AS_Name> & nms, const SET<TYPE_AS_Name> & already)
{
  // pre nms inter already = {}
  if (!nms.Intersect(already).IsEmpty ())
    InternalError (L"TransClosExit");

  if (nms.SubSet(already)) // if nms subset already
    return Nil ();
  else
  {
    TYPE_AS_Name nm (nms.GetElem());
    Generic tpnm = Nil();
    if (this->direxit.DomExists(nm))
      tpnm = this->direxit[nm];

    SET<TYPE_AS_Name> nms_q;
    if (this->opcalls.DomExists (nm))
      nms_q.ImpUnion(this->opcalls[nm].Diff(already));

    Generic resttp (TransClosExit (nms_q.ImpUnion(nms).ImpDiff(mk_set(nm)), already.Union(mk_set(nm))));

    if (tpnm.IsNil ())
      return resttp;
    else if (resttp.IsNil ())
      return tpnm;
    else
      return MergeTypes(resttp, tpnm);
  }
}

// wf_ErrorStmt
// i : TYPE`Ind
// stmt : AS`ErrorStmt
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_ErrorStmt (const Int  &, const TYPE_AS_ErrorStmt & stmt, const TYPE_REP_TypeRep & exptp)
{
#ifdef VDMSL
  if (Settings.VDMSLmode())
  {
    //-----------------------------------------------
    // Error message #356
    // Error-Stmt is not supported in standard VDM-SL
    //-----------------------------------------------
    GenErr (stmt, ERR, 356, Sequence());
    return mk_(Bool(false), rep_alltp);
  }
#endif // VDMSL

// 20110523 -->
  //return mk_(Bool(true), mk_REP_UnionTypeRep(mk_set(rep_unittp, rep_alltp)));
  return mk_(Bool(true), exptp);
// <-- 20110523
}

// wf_IdentStmt
// i : TYPE`Ind
// stmt : AS`IdentStmt
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_IdentStmt (const Int &, const TYPE_AS_IdentStmt & stmt, const TYPE_REP_TypeRep &)
{
  GetCI().SetTypeInfo(stmt.GetInt(pos_AS_IdentStmt_cid), rep_unittp);
  return mk_(Bool(true), rep_unittp);
}

#endif //! NOSS
