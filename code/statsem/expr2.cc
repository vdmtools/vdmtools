/***
*  * ID
*  *    $Id: expr2.cc,v 1.108 2006/04/03 23:50:30 vdmtools Exp $
*  * SPECIFICATION VERSION
*  *    expr2.cc: Implementation of expr.vdm 1.131
***/

#include "statsem.h"
#include "astaux.h"
#include "intconvquotes.h"
#include "settings.h"
#include "tbutils.h"

#include <math.h>

//wcout << L"intertp: " << INT2Q::h2gAS(intertp) << endl;

#ifndef NOSS

// wf_SeqModify
// i : TYPE`Ind
// s : REP`TypeRep
// m : REP`TypeRep
// SeqModMapOverride : AS`Expr
// ==> bool * REP`TypeRep
Tuple StatSem::wf_SeqModify (const Int & i,
                             const TYPE_REP_TypeRep & s,
                             const TYPE_REP_TypeRep & m,
                             const TYPE_AS_Expr & SeqModMapOverride)
{
  TYPE_REP_TypeRep stp (ExtractSeqType (s));
  Generic mtp_g (ExtractMapType (m));

  Generic Dom, Rng;
  if (mtp_g.IsNil () || mtp_g.Is(TAG_TYPE_REP_EmptyMapTypeRep)) {
    Dom = Nil();
    Rng = Nil();
  }
  else {
    TYPE_REP_TypeRep mtp (mtp_g);
    switch(mtp.GetTag()) {
      case TAG_TYPE_REP_GeneralMapTypeRep: {
        Dom = mtp.GetRecord (pos_REP_GeneralMapTypeRep_mapdom);
        Rng = mtp.GetRecord (pos_REP_GeneralMapTypeRep_maprng);
        break;
      }
      case TAG_TYPE_REP_InjectiveMapTypeRep: {
        Dom = mtp.GetRecord (pos_REP_InjectiveMapTypeRep_mapdom);
        Rng = mtp.GetRecord (pos_REP_InjectiveMapTypeRep_maprng);
        break;
      }
      case TAG_TYPE_REP_UnionTypeRep: {
        Set d_s, r_s, utps (mtp.GetSet(pos_REP_UnionTypeRep_tps));
        Generic g;
        for (bool bb = utps.First(g); bb; bb = utps.Next(g)) {
          TYPE_REP_TypeRep r (g);
          switch(r.GetTag()) {
            case TAG_TYPE_REP_GeneralMapTypeRep: {
              d_s.Insert (r.GetRecord (pos_REP_GeneralMapTypeRep_mapdom));
              r_s.Insert (r.GetRecord (pos_REP_GeneralMapTypeRep_maprng));
              break;
            }
            case TAG_TYPE_REP_InjectiveMapTypeRep: {
              d_s.Insert (r.GetRecord (pos_REP_InjectiveMapTypeRep_mapdom));
              r_s.Insert (r.GetRecord (pos_REP_InjectiveMapTypeRep_maprng));
              break;
            }
          }
        }
  
        SET<TYPE_REP_TypeRep> dtps (MergeNumericTypes (d_s));
        SET<TYPE_REP_TypeRep> rtps (MergeNumericTypes (r_s));
  
        switch(dtps.Card ()) {
          case 0: {
            InternalError (L"SeqModify1");
            Dom = Nil();
            break;
          }
          case 1: {
            Dom = dtps.GetElem ();
            break;
          }
          default: {
            Dom = mk_REP_UnionTypeRep(dtps);
            break;
          }
        }
  
        switch(rtps.Card ()) {
          case 0: {
            InternalError (L"SeqModify2");
            Rng = Nil();
            break;
          }
          case 1: {
            Rng = rtps.GetElem ();
            break;
          }
          default: {
            Rng = mk_REP_UnionTypeRep(rtps);
            break;
          }
        }
        break;
      }
      default: {
        InternalError (L"SeqModify3");
        Dom = Nil();
        Rng = Nil();
        break;
      }
    }
  }

  if (Dom.IsNil()) {
    //------------------------------
    // Error message #242
    // An empty modification is used
    //------------------------------
    GenErr (SeqModMapOverride, ERR,242, Sequence());
    return mk_(Bool (false), stp);
  }
  else if (stp.Is(TAG_TYPE_REP_EmptySeqTypeRep)) {
    //-------------------------------------
    // Error message #243
    // An empty sequence cannot be modified
    //-------------------------------------
    GenErr (SeqModMapOverride, ERR,243, Sequence());
    return mk_(Bool (false), stp);
  }
  else if (IsCompatible (i, Dom, btp_natone)) {
    if (i == DEF && (Settings.ErrorLevel() >= ERR2)) {
      //----------------------------------------------------
      // Error message #244
      // Modification of non-existing index may be attempted
      //----------------------------------------------------
      GenErr (SeqModMapOverride, ERR2, 244, Sequence());
      return mk_(Bool (false),  MergeTypes (stp, mk_REP_SeqTypeRep(Rng)));
    }
    else {
      return mk_(Bool (true),  MergeTypes (stp, mk_REP_SeqTypeRep(Rng)));
    }
  }
  else {
    //------------------------------------------------------------------------
    // Error message #246
    // A sequence can only be modified with indices which are positive natural
    // numbers
    //------------------------------------------------------------------------
    GenErrTp (SeqModMapOverride, ERR, 246, Dom, btp_natone, Sequence());
    return mk_(Bool (false),  MergeTypes (stp, mk_REP_SeqTypeRep(Rng)));
  }
}

// wf_MapOverride
// s : REP`TypeRep
// m : REP`TypeRep
// SeqModMapOverride : AS`Expr
// ==> bool * REP`TypeRep
Tuple StatSem::wf_MapOverride ( const TYPE_REP_TypeRep & s,
                                     const TYPE_REP_TypeRep & m,
                                     const TYPE_AS_Expr & SeqModMapOverride)
{
  Generic m1tp (ExtractMapType (s));
  Generic m2tp (ExtractMapType (m));

  if (m2tp.Is(TAG_TYPE_REP_EmptyMapTypeRep)) {
    //------------------------------
    // Error message #242
    // An empty modification is used
    //------------------------------
    GenErr (SeqModMapOverride, ERR, 242, Sequence());
    return mk_(Bool(false), m1tp);
  }
  else if (m2tp.IsNil () ) {
    return mk_(Bool(false), m1tp);
  }
  else {
    return mk_(Bool(true), MapTypeMerge (mk_set(m1tp, m2tp)));
  }
}

// wf_SeqModifyMapOverrideExpr
// i : TYPE`Ind
// SeqModMapOverride : AS`SeqModifyMapOverrideExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_SeqModifyMapOverrideExpr (const Int & i,
                                                 const TYPE_AS_SeqModifyMapOverrideExpr & SeqModMapOverride,
                                                 const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & s (SeqModMapOverride.GetRecord(pos_AS_SeqModifyMapOverrideExpr_seqmap));
  const TYPE_AS_Expr & m (SeqModMapOverride.GetRecord(pos_AS_SeqModifyMapOverrideExpr_mapexp));

  const TYPE_CI_ContextId & cid (SeqModMapOverride.GetInt(pos_AS_SeqModifyMapOverrideExpr_cid));

  Generic expelemtp (rep_alltp);
  if (IsCompatible(i, seq_alltp, exptp)) {
    expelemtp = UnmaskSeqType (POS, ExtractSeqType (exptp));
  }
  else if (IsCompatible(i, map_alltp, exptp)) {
    Tuple dr (SplitMapType(ExtractMapType (exptp)));
    expelemtp = dr.GetField(2);
  }

  TYPE_REP_TypeRep expmaptp (mk_REP_UnionTypeRep(mk_set(mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp),
                                                        mk_REP_GeneralMapTypeRep(btp_natone, expelemtp))));

  Tuple infer (wf_Expr(i, s, RemoveEmptyMapType(exptp)));
  const Bool & wf_seqmap (infer.GetBool (1));
  const TYPE_REP_TypeRep & seqmaptp (infer.GetRecord (2));

  TYPE_REP_TypeRep m1tp (mk_REP_UnionTypeRep(mk_set(mk_REP_GeneralMapTypeRep(rep_alltp, rep_alltp),
                                                    mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp))));

  TYPE_REP_TypeRep stp (seq_alltp);

  Generic ExpectedType (MergeTypes (stp, m1tp));

  bool seqcomp      (IsCompatible (i, seqmaptp, stp));
  bool mapcomp      (IsCompatible (i, seqmaptp, m1tp));
  bool seqormapcomp (IsCompatible (i, seqmaptp, ExpectedType));

  if (seqcomp && !mapcomp) {
    // modify sequence
    Tuple infer2 (wf_Expr(i, m, expmaptp));
    const Bool & wf_map (infer2.GetBool (1));
    const TYPE_REP_TypeRep & maptp (infer2.GetRecord (2));

    Tuple infer3 (wf_SeqModify (i, seqmaptp, maptp, SeqModMapOverride));
    const Bool & wf (infer3.GetBool (1));
    const TYPE_REP_TypeRep & restp (infer3.GetRecord(2));

    bool amap (IsCompatible(i, maptp, m1tp));
    if (!amap) {
      //---------------------------------
      // Error message #240
      // Rhs expression is not a map type
      //---------------------------------
      GenErrTp (SeqModMapOverride, ERR, 240, maptp, m1tp, Sequence());
    }

    GetCI().SetTypeInfo (cid, restp);
    return mk_(wf_seqmap && wf_map && wf && amap, restp);
  }
  else if (!seqcomp && mapcomp) {
    // override map
    Tuple infer2 (wf_Expr(i, m, RemoveEmptyMapType(exptp)));
    const Bool & wf_map (infer2.GetBool (1));
    const TYPE_REP_TypeRep & maptp (infer2.GetRecord (2));

    Tuple infer3 (wf_MapOverride (seqmaptp, maptp, SeqModMapOverride));
    const Bool & wf (infer3.GetBool (1));
    const TYPE_REP_TypeRep & restp (infer3.GetRecord(2));

    bool amap (IsCompatible(i, maptp, m1tp));
    if (!amap) {
      //---------------------------------
      // Error message #240
      // Rhs expression is not a map type
      //---------------------------------
      GenErrTp (SeqModMapOverride, ERR, 240, maptp, m1tp, Sequence());
    }

    GetCI().SetTypeInfo (cid, restp);
    return mk_(wf_seqmap && wf_map && wf && amap, restp);
  }
  else if (seqormapcomp) {
    TYPE_REP_TypeRep m_exptp (MergeTypes(exptp, expmaptp));

    Tuple infer2 (wf_Expr(i, m, m_exptp));
    const Bool & wf_map (infer2.GetBool (1));
    const TYPE_REP_TypeRep & maptp (infer2.GetRecord (2));

    bool amap (IsCompatible(i, maptp, m1tp));
    if (!amap) {
      //---------------------------------
      // Error message #240
      // Rhs expression is not a map type
      //---------------------------------
      GenErrTp (SeqModMapOverride, ERR, 240, maptp, m1tp, Sequence());
    }

    TYPE_REP_TypeRep resstp (ExtractSeqType (seqmaptp));
    Generic resmtp (ExtractMapType (seqmaptp));
    Generic resmap (ExtractMapType (maptp));
    TYPE_REP_TypeRep restp;
    if (!amap) {
      restp = MergeTypes (stp, m1tp);
    }
    else {
      if (resmap.IsNil () || resmap.Is(TAG_TYPE_REP_EmptyMapTypeRep)) {
        restp = MergeTypes (resstp, resmtp);
      }
      else {
        restp = SeqMapTypeModify (resstp, resmtp, resmap);
      }
    }

    GetCI().SetTypeInfo (cid, restp);

    if (resmap.Is(TAG_TYPE_REP_EmptyMapTypeRep) && !maptp.Is(TAG_TYPE_REP_AllTypeRep)) {
      //------------------------------
      // Error message #242
      // An empty modification is used
      //------------------------------
      GenErr (SeqModMapOverride, ERR, 242, Sequence());
      return mk_(Bool (false), restp);
    }
    else if (resstp.Is(TAG_TYPE_REP_EmptySeqTypeRep) && !seqmaptp.Is(TAG_TYPE_REP_AllTypeRep)) {
      //-------------------------------------
      // Error message #243
      // An empty sequence cannot be modified
      //-------------------------------------
      GenErr (SeqModMapOverride, ERR, 243, Sequence());
      return mk_(Bool(false), restp);
    }
    else if (i == DEF && (Settings.ErrorLevel() >= ERR2)) {
      //----------------------------------------------------
      // Error message #244
      // Modification of non-existing index may be attempted
      //----------------------------------------------------
      GenErr (SeqModMapOverride, ERR2, 244, Sequence());
      return mk_(Bool(false), restp);
    }
    else {
      return mk_(wf_seqmap && wf_map && amap, restp);
    }
  }
  else if (!seqcomp && !mapcomp && !seqormapcomp) {
    //------------------------------------------------
    // Error message #245
    // Expression is neither a sequence nor a map type
    //------------------------------------------------
    GenErrTp (SeqModMapOverride, ERR, 245, seqmaptp, ExpectedType, Sequence());
    GetCI().SetTypeInfo (cid, ExpectedType);
    return mk_(Bool(false), ExpectedType);
  }
  else {
    InternalError(L"SeqModMapOverride");
    GetCI().SetTypeInfo (cid, ExpectedType);
    return mk_(Bool(false), ExpectedType);
  }
}

// wf_SubSequenceExpr
// i : TYPE`Ind
// vSubSequence AS`SubSequenceExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_SubSequenceExpr (const Int & i,
                                   const TYPE_AS_SubSequenceExpr & vSubSequence,
                                   const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & s (vSubSequence.GetRecord(pos_AS_SubSequenceExpr_sequence));
  const TYPE_AS_Expr & f (vSubSequence.GetRecord(pos_AS_SubSequenceExpr_frompos));
  const TYPE_AS_Expr & t (vSubSequence.GetRecord(pos_AS_SubSequenceExpr_topos));

  const TYPE_CI_ContextId & cid (vSubSequence.GetInt(pos_AS_SubSequenceExpr_cid));

  TYPE_REP_TypeRep intertp (IntersectTypeReps (seq_alltp, exptp));

  Tuple infer (wf_Expr (i, s, intertp));
  const Bool & wf_s (infer.GetBool(1));
  const TYPE_REP_TypeRep & sqtp (infer.GetRecord(2));
  Bool scomp (IsCompatible (i, sqtp, seq_alltp));
  Bool reswf (wf_s && scomp);
  TYPE_REP_TypeRep restp (seq_alltp);
  if (!scomp) {
    //----------------------------------
    // Error message #237
    // Expression is not a sequence type
    //----------------------------------
    GenErrTp (s, ERR, 237, sqtp, seq_alltp , Sequence());
  }
  else {
    restp = ExtractSeqType(sqtp);
    if (!IsCompatible (POS, restp, mk_REP_EmptySeqTypeRep(rep_alltp))) {
      restp = MergeTypes (restp, mk_REP_EmptySeqTypeRep(rep_alltp));
    }
  }

  Tuple infer2 (wf_Expr (i, f, btp_natone));
  const Bool & wf_f (infer2.GetBool(1));
  const TYPE_REP_TypeRep & ftp (infer2.GetRecord(2));
  Bool fcomp (IsCompatible (i, ftp, btp_natone));
  reswf = reswf && wf_f && fcomp;
  if (!fcomp) {
    //---------------------------------------------
    // Error message #238
    // Lower index is not a positive natural number
    //---------------------------------------------
    GenErrTp (f, ERR, 238, ftp, btp_natone, Sequence());
  }

  Tuple infer3 (wf_Expr (i, t, btp_natone));
  const Bool & wf_t (infer3.GetBool(1));
  const TYPE_REP_TypeRep & ttp (infer3.GetRecord(2));
  Bool tcomp (IsCompatible (i, ttp, btp_natone));
  reswf = reswf && wf_t && tcomp;
  if (!tcomp) {
    //---------------------------------------------
    // Error message #239
    // Upper index is not a positive natural number
    //---------------------------------------------
    GenErrTp (t, ERR, 239, ttp, btp_natone, Sequence());
  }

  GetCI().SetTypeInfo (cid, restp);
  return mk_(reswf, restp);
}

// wf_Maplet
// i : TYPE`Ind
// maplet : AS`Maplet
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_Maplet (const Int & i, const TYPE_AS_Maplet & maplet, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & Dom (maplet.GetRecord(pos_AS_Maplet_mapdom));
  const TYPE_AS_Expr & Rng (maplet.GetRecord(pos_AS_Maplet_maprng));

  TYPE_REP_TypeRep maptype (mk_REP_UnionTypeRep(mk_set(mk_REP_GeneralMapTypeRep(rep_alltp, rep_alltp),
                                                       mk_REP_InjectiveMapTypeRep(rep_alltp, rep_alltp))));
  Tuple expmaptp (mk_(rep_alltp, rep_alltp));
  if (IsCompatible(POS, maptype, exptp) &&
      ! exptp.Is(TAG_TYPE_REP_AllTypeRep) &&
      ! exptp.Is(TAG_TYPE_REP_TypeVarRep)) {
    expmaptp = SplitMapType (ExtractMapType (exptp)); // [REP`TypeRep] * [REP`TypeRep]
  }
  Tuple inferlhs (wf_Expr(i, Dom, expmaptp.GetField(1)));
  const Bool & wf_Dom (inferlhs.GetBool(1));
  const TYPE_REP_TypeRep & Domtp (inferlhs.GetRecord(2));

  Tuple inferrhs (wf_Expr(i, Rng, expmaptp.GetField(2)));
  const Bool & wf_Rng (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & Rngtp (inferrhs.GetRecord(2));

  if (wf_Dom && wf_Rng) {
    return mk_(Bool(true), mk_REP_GeneralMapTypeRep(Domtp, Rngtp));
  }
  else if (wf_Dom) {
    return mk_(Bool(false), mk_REP_GeneralMapTypeRep(Domtp, rep_alltp));
  }
  else if (wf_Rng) {
    return mk_(Bool(false), mk_REP_GeneralMapTypeRep(rep_alltp, Rngtp));
  }
  else {
    return mk_(Bool(false), mk_REP_GeneralMapTypeRep(rep_alltp, rep_alltp));
  }
}

// wf_MapEnumerationExpr
// i : TYPE`Ind
// MapEnum : AS`MapEnumerationExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_MapEnumerationExpr (const Int & i,
                                           const TYPE_AS_MapEnumerationExpr & MapEnum,
                                           const TYPE_REP_TypeRep & exptp)
{
  const SEQ<TYPE_AS_Maplet> & els (MapEnum.GetSequence(pos_AS_MapEnumerationExpr_els));
  const TYPE_CI_ContextId & cid (MapEnum.GetInt(pos_AS_MapEnumerationExpr_cid));

  if (els.IsEmpty()) {
    GetCI().SetTypeInfo (cid, mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp));
    return mk_(Bool(true), mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp));
  }
  else {
    TYPE_REP_TypeRep realexptp;
    if(exptp.Is(TAG_TYPE_REP_UnionTypeRep)) {
      SET<TYPE_REP_TypeRep> exptp_tps (exptp.GetSet(pos_REP_UnionTypeRep_tps).Diff(mk_set(mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp))));
      switch (exptp_tps.Card()) {
        case 1: {
          realexptp = exptp_tps.GetElem();
          break;
        }
        default: {
          realexptp = mk_REP_UnionTypeRep(exptp_tps);
          break;
        }
      }
    }
    else {
      realexptp = exptp;
    }

    Bool reswf (true);
    Generic restp = Nil();
    size_t len_els = els.Length();
    for (size_t idx = 1; idx <= len_els; idx++) {
      Tuple infer (wf_Maplet(i, els[idx], realexptp));
      const Bool & wf (infer.GetBool(1));
      const TYPE_REP_TypeRep & tp (infer.GetRecord(2));
      reswf &= wf;
      restp = (restp.IsNil() ? tp : MergeTypes (restp, tp));
    }

    GetCI().SetTypeInfo (cid, restp);
    if ((els.Length() > 1) && (i == DEF) && (Settings.ErrorLevel() >= ERR2)) {
      //----------------------------------------------------
      // Error message #247
      // Same domain value may map to different range values
      //----------------------------------------------------
      GenErr (MapEnum, ERR2, 247, Sequence());
      return mk_(Bool(false), restp);
    }
    else {
      return mk_(reswf, restp);
    }
  }
}

// wf_MapComprehensionExpr
// i : TYPE`Ind
// vMapComprehension : AS`MapComprehensionExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_MapComprehensionExpr (const Int & i,
                                        const TYPE_AS_MapComprehensionExpr & vMapComprehension,
                                        const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Maplet & elem (vMapComprehension.GetRecord(pos_AS_MapComprehensionExpr_elem));
  const SEQ<TYPE_AS_MultBind> & bind (vMapComprehension.GetSequence(pos_AS_MapComprehensionExpr_bind));
  const Generic & pred (vMapComprehension.GetField(pos_AS_MapComprehensionExpr_pred));
  const TYPE_CI_ContextId & cid (vMapComprehension.GetInt(pos_AS_MapComprehensionExpr_cid));

  Tuple infer (wf_MultiBindList (i, bind, false));
  const Generic & wf_bd (infer.GetField(1));
  const MAP<TYPE_AS_Name,Tuple> & bd (infer.GetMap(2));
  Bool reswf (wf_bd == Bool(true)); 

  //GetCI().SetPidM (cid, bd);
  Generic nm;
  MAP<TYPE_AS_Name, TYPE_REP_TypeRep> bdm;
  SET<TYPE_AS_Name> dom_bd (bd.Dom());
  for (bool bb = dom_bd.First(nm); bb; bb = dom_bd.Next(nm)) {
    bdm.Insert(nm, bd[nm].GetRecord(1));
  }
  GetCI().SetPidM (cid, bdm);


  if (wf_bd.IsNil()) {
    //------------------------------
    // Error message #229
    // Binding for L"%1" is erroneous
    //------------------------------
    GenErr (vMapComprehension, ERR, 229, mk_sequence(SEQ<Char>(L"Map-Comprh-Expr")));
    TYPE_REP_TypeRep rc1 (mk_REP_GeneralMapTypeRep(rep_alltp, rep_alltp));
    GetCI().SetTypeInfo (cid, rc1);
    return mk_(Bool(false), rc1);
  }

  if (wf_bd == Bool(false)) {
    //---------------------------------
    // Error message #230
    // Not all bindings are well-formed
    //---------------------------------
    GenErr (vMapComprehension, ERR, 230, Sequence());
  }

  EnterScope (bd);

  if (!pred.IsNil()) {
    Tuple infer2 (wf_Expr(i, pred, btp_bool));
    const Bool & wf_pr (infer2.GetBool(1));
    const TYPE_REP_TypeRep & pr_tp (infer2.GetRecord(2));

    bool predcomp = IsCompatible (i, pr_tp, btp_bool);

    reswf = reswf && wf_pr && predcomp;
    if (!predcomp) {
      //-----------------------------------------------
      // Error message #231
      // Predicate for L"%1" is not a boolean expression
      //-----------------------------------------------
      GenErrTp (pred, ERR, 231, pr_tp, btp_bool, mk_sequence(SEQ<Char>(L"Map-Comprh-Expr")));
    }
  }

  Tuple infer3 (wf_Maplet(i, elem, exptp));
  const Bool & wf_e (infer3.GetBool(1));
  const TYPE_REP_TypeRep & e_tp (infer3.GetRecord(2));
  reswf = reswf && wf_e;
  LeaveScope();

  if (!wf_e) {
    GetCI().SetTypeInfo (cid, e_tp);
    return mk_(Bool(false), e_tp);
  }
  else {
    TYPE_REP_TypeRep rc1 (mk_REP_UnionTypeRep(mk_set(mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp), e_tp)));
    GetCI().SetTypeInfo (cid, rc1);
    return mk_(reswf, rc1);
  }
}

// wf_TupleConstructorExpr
// i : TYPE`Ind
// TupleConstr : AS`TupleConstructorExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_TupleConstructorExpr (const Int & i,
                                        const TYPE_AS_TupleConstructorExpr & TupleConstr,
                                        const TYPE_REP_TypeRep & exptp)
{
  const SEQ<TYPE_AS_Expr> & fields (TupleConstr.GetSequence(pos_AS_TupleConstructorExpr_fields));
  const TYPE_CI_ContextId & cid (TupleConstr.GetInt(pos_AS_TupleConstructorExpr_cid));
  size_t len_fields = fields.Length();

  Generic ptp (ExtractProductType (exptp, (int)len_fields, EQ));
  SEQ<TYPE_REP_TypeRep> fldtp;
  if (ptp.IsNil()) {
    for (size_t idx = 1; idx <= len_fields; idx++) {
      fldtp.ImpAppend(rep_alltp);
    }
  }
  else {
    fldtp.ImpConc(Record(ptp).GetSequence(pos_REP_ProductTypeRep_tps));
  }

  Bool reswf (true);

  SEQ<TYPE_REP_TypeRep> restp;
  //size_t len_fields = fields.Length();
  for (size_t idx = 1; idx <= len_fields; idx++) {
    Tuple infer (wf_Expr (i, fields[idx], rep_alltp));
    const Bool & wf (infer.GetBool(1));
    const TYPE_REP_TypeRep & tp (infer.GetRecord(2));

    reswf &= wf;
    restp.ImpAppend(tp);
  }

  TYPE_REP_TypeRep tmpres (mk_REP_ProductTypeRep(restp));
  GetCI().SetTypeInfo (cid, tmpres);
  return mk_(reswf, tmpres);
}

// wf_TupleSelectExpr
// i : TYPE`Ind
// tupleselect : AS`TupleSelectExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_TupleSelectExpr (const Int & i,
                                        const TYPE_AS_TupleSelectExpr & tupleselect,
                                        const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & expr (tupleselect.GetRecord(pos_AS_TupleSelectExpr_tuple));
  const TYPE_AS_RealLit & no (tupleselect.GetRecord(pos_AS_TupleSelectExpr_no));
  const TYPE_CI_ContextId & cid (tupleselect.GetInt(pos_AS_TupleSelectExpr_cid));

  Tuple infer_no (wf_Literal(DEF, no, btp_natone));
  const Bool & wf_no (infer_no.GetBool(1));

  Tuple infer (wf_Expr(i, expr, rep_alltp));
  const Bool & wf_arg (infer.GetBool(1));
  const TYPE_REP_TypeRep & argtp (infer.GetRecord(2));

  if (!wf_no) {
    return mk_(Bool(false), rep_alltp);
  }
  else {
    double no_int = no.GetRealValue(pos_AS_RealLit_val);

    Generic ptp (ExtractProductType (argtp, (int)no_int, NUMGE));
    if (ptp.IsNil()) {
      //---------------------
      // Error message #365
      // A tuple was expected
      //---------------------
      GenErr (expr, ERR, 365, Sequence());
      return mk_(Bool(false), rep_alltp);
    }

    const SEQ<TYPE_REP_TypeRep> & ptp_l (Record(ptp).GetSequence (pos_REP_ProductTypeRep_tps));

    if (ptp_l.Length () < 2 ) {
      //---------------------
      // Error message #365
      // A tuple was expected
      //---------------------
      GenErr (expr, ERR, 365, Sequence());
      return mk_(Bool(false), rep_alltp);
    }
    else if (ptp_l.Length () >= no_int) {
      GetCI().SetTypeInfo (cid, ptp_l[(int)no_int]);
      return mk_(wf_arg, ptp_l[(int)no_int]);
    }
    else {
      //-------------------------
      // Error message #364
      // Tuple selector too large
      //-------------------------
      GenErr (no, ERR, 364, Sequence());
      return mk_(Bool(false), rep_alltp);
    }
  }
}

// f_TypeJudgementExpr
// i : TYPE`Ind
// typejudge : AS`TypeJudgementExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_TypeJudgementExpr (const Int & i,
                                          const TYPE_AS_TypeJudgementExpr & typejudge,
                                          const TYPE_REP_TypeRep & exptp)
{
  return wf_IsExpr(i, typejudge, exptp);
}

// wf_PreConditionApplyExpr
// i : TYPE`Ind
// precondapp : AS`PreConditionApplyExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_PreConditionApplyExpr (const Int & i,
                                              const TYPE_AS_PreConditionApplyExpr & precondapp,
                                              const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & expr       (precondapp.GetRecord(pos_AS_PreConditionApplyExpr_fct));
  const SEQ<TYPE_AS_Expr> & arg_l (precondapp.GetSequence(pos_AS_PreConditionApplyExpr_arg));
  const TYPE_CI_ContextId & cid   (precondapp.GetInt(pos_AS_PreConditionApplyExpr_cid));

  TYPE_AS_ApplyExpr appexpr;
  appexpr.Init(expr, arg_l, cid);

  Tuple infer (wf_ApplyExpr (i, appexpr, exptp));

  GetCI().SetTypeInfo (cid,  btp_bool);
  return mk_(infer.GetBool(1), btp_bool);
}

// wf_TokenConstructorExpr
// i : TYPE`Ind
// TokenConstr AS`TokenConstructorExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_TokenConstructorExpr (const Int & i,
                                             const TYPE_AS_TokenConstructorExpr & TokenConstr,
                                             const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & expr     (TokenConstr.GetRecord(pos_AS_TokenConstructorExpr_field));
  const TYPE_CI_ContextId & cid (TokenConstr.GetInt(pos_AS_TokenConstructorExpr_cid));

  Tuple infer (wf_Expr (i, expr, rep_alltp));
  const Bool & wf (infer.GetBool (1));

  Bool reswf (true);
  if (!(IsCompatible(i, btp_token, exptp))) {
    //---------------------------------
    //-- Error message #398
    //-- Token value was not expected
    //---------------------------------
    GenErrTp (expr, ERR, 398, btp_token, exptp, Sequence());
    reswf = false;
  }

  GetCI().SetTypeInfo (cid, btp_token);
  return mk_(reswf && wf, btp_token);
}

// wf_RecordConstructorExpr
// i : TYPE`Ind
// RecordConstr : AS`RecordConstructorExpr
// evptp: REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_RecordConstructorExpr (const Int & i,
                                              const TYPE_AS_RecordConstructorExpr& RecordConstr,
                                              const TYPE_REP_TypeRep &)
{
  const TYPE_AS_Name & tag         (RecordConstr.GetRecord(pos_AS_RecordConstructorExpr_tag));
  const SEQ<TYPE_AS_Expr> & fields (RecordConstr.GetSequence(pos_AS_RecordConstructorExpr_fields));
  const TYPE_CI_ContextId & cid    (RecordConstr.GetInt(pos_AS_RecordConstructorExpr_cid));

  Bool reswf (true);
  SEQ<TYPE_REP_FieldRep> restp;

  size_t len_fields = fields.Length();
  for (size_t idx = 1; idx <= len_fields; idx++) {
    Tuple infer (wf_Expr (i, fields[idx], rep_alltp));
    const Bool & wf (infer.GetBool(1));
    const TYPE_REP_TypeRep & tp (infer.GetRecord(2));

    reswf &= wf;
    restp.ImpAppend(mk_REP_FieldRep(Nil(), tp, Bool(false)));
  }

  // sl : [AS`Name * seq of REP`FieldRep]
  // pp : [AS`Name * (AccessFieldRep|TagRepElem)]
  Generic lut_g (LookUpTag (i, tag, Set()));

  if (lut_g.IsNil ()) {
    //--------------------------------------------
    // Error message #254
    // Composite type L"%1" is not defined in "%2".
    //--------------------------------------------
    GenErr (RecordConstr, ERR, 254, mk_sequence(PrintName(tag), SEQ<Char>(L"Record-Constructor-Expr")));
    TYPE_REP_TypeRep ctr (mk_REP_CompositeTypeRep(tag, restp));
    GetCI().SetTypeInfo (cid, ctr);
    return mk_(Bool(false), ctr);
  }

  Tuple lut (lut_g);
#ifdef VDMSL
  // lut : AS`Name * seq of REP`FieldRep
  TYPE_AS_Name newtag (ExtName (GetCurMod(), lut.GetRecord (1)));
  SEQ<TYPE_REP_FieldRep> fl (lut.GetSequence (2));
#endif // VDMSL
#ifdef VDMPP
  // lut : AS`Name * (AccessFieldRep|TagRepElem)
  const TYPE_AS_Name & newtag (lut.GetRecord (1));
  const Generic & fl_q (lut.GetField (2));
  Generic fl_tmp_g (CheckAccessCurClass (fl_q));

  if (fl_tmp_g.IsNil ()) {
    //-------------------
    // Error message #368
    // Access violation
    //-------------------
    GenErr (tag, ERR, 368, mk_sequence(PrintName (tag)));
    return mk_(Bool (false), mk_REP_CompositeTypeRep(newtag, restp));
  }
  SEQ<TYPE_REP_FieldRep> fl (fl_tmp_g);
#endif // VDMPP

  TYPE_REP_TypeRep ctr (mk_REP_CompositeTypeRep(newtag, fl));
  GetCI().SetTypeInfo (cid, ctr);

  if (fl.Length () != restp.Length ()) {
    //--------------------------------
    // Error message #297
    // Wrong number of fields for L"%1"
    //--------------------------------
    GenErr (RecordConstr, ERR, 297, mk_sequence(SEQ<Char>(L"Record-Constructor-Expr")));
    return mk_(Bool(false), ctr);
  }
  else {
    size_t len_fl = fl.Length ();
    for(size_t idx = 1; idx <= len_fl; idx++) {
      if (! IsCompatible (i, restp[idx].get_tp(), fl[idx].get_tp())) {
        //---------------------------------------------------------------------
        // Error message #250
        // Type of field number %1 is not compatible with its defined type in
        // Record-Constructor-Expr
        //---------------------------------------------------------------------
        GenErrTp (fields[idx], ERR, 250, restp[idx].get_tp(), fl[idx].get_tp(), mk_sequence(SEQ<Char>(StringNumber(idx) )));
        reswf = Bool (false);
      }
    }
    return mk_(reswf, mk_REP_TypeNameRep(tag));
  }
}

// CheckModifications
// i : TYPE`Ind
// tp : (REP`CompositeTypeRep | REP`UnionTypeRep)
// mods : seq1 of AS`RecordModification *
// exptp : REP`TypeRep
// ==> bool
bool StatSem::CheckModifications (const Int & i,
                                  const TYPE_REP_TypeRep & tp,
                                  const SEQ<TYPE_AS_RecordModification> & mods,
                                  const TYPE_REP_TypeRep & exptp)
{
  bool reswf (true);

  switch (tp.GetTag ()) {
    case TAG_TYPE_REP_CompositeTypeRep: {
      const TYPE_AS_Name & nm (tp.GetRecord(pos_REP_CompositeTypeRep_nm));
      Generic lut_g (LookUpTag (i, nm, Set()));

      if (lut_g.IsNil ()) {
        // There is no need to report an error here
        // as it is already done, when the record was created.
        // 28 Oct. 1998 11:05 -- Jesper K. Pedersen
        // GenErr (nm, ERR, 248, mk_sequence(PrintName(nm)));
        return false;
      }
      Tuple lut (lut_g);
#ifdef VDMSL
      const SEQ<TYPE_REP_FieldRep> & fl (lut.GetSequence (2));
#endif // VDMSL
#ifdef VDMPP
      const Generic & fl_q (lut.GetField (2)); // (AccessFieldRep|TagRepElem)
      Generic fl_tmp (CheckAccessCurClass (fl_q));
      if (fl_tmp.IsNil ()) {
        //-------------------
        // Error message #368
        // Access violation
        //-------------------
        GenErr (nm, ERR, 368, mk_sequence(PrintName (nm)));
        return false;
      }
      SEQ<TYPE_REP_FieldRep> fl (fl_tmp);
#endif // VDMPP
      MAP<TYPE_AS_Name, TYPE_REP_TypeRep> ftp;
      size_t len_fl = fl.Length();
      for(size_t idx = 1; idx <= len_fl; idx++) {
        const TYPE_REP_FieldRep & fr (fl[idx]);
        const Generic & nm (fr.GetField(pos_REP_FieldRep_sel));
        if (!nm.IsNil () && !ftp.DomExists (nm))
          ftp.Insert (nm, fr.GetRecord(pos_REP_FieldRep_tp));
      }

      SET<TYPE_AS_Name> domm;
      size_t len_mods = mods.Length();
      for(size_t idm = 1; idm <= len_mods; idm++) {
        const TYPE_AS_RecordModification & ng (mods[idm]);
        const TYPE_AS_Name & mid (ng.GetRecord(pos_AS_RecordModification_field));
        const TYPE_AS_Expr & mid_e (ng.GetRecord(pos_AS_RecordModification_newexpr));
        if (ftp.DomExists (mid)) {
          Tuple infer (wf_Expr (i, mid_e, ftp[mid]));
          const Bool & wf_arg (infer.GetBool (1));
          const TYPE_REP_TypeRep & argtp (infer.GetRecord (2));

          reswf = reswf && (wf_arg.GetValue());

          if (!IsCompatible (i, argtp, ftp[mid])) {
            //-----------------------------------------------------------
            // Error message #251
            // Type of field L"%1" is not compatible with its defined type
            //-----------------------------------------------------------
            GenErrTp (mid_e, ERR, 251, argtp, ftp[mid], mk_sequence(PrintName(mid)));
            reswf = false;
          }

          if (domm.InSet (mid)) {
            //--------------------------------------
            // Error message #348
            // Field L"%1" is modified more than once
            //--------------------------------------
            GenErr (mid_e, WRN1, 348, mk_sequence(PrintName(mid)));
          }
          domm.Insert (mid);
        }
        else {
          //-----------------------------------------------
          // Error message #253
          // Field with id L"%1" is not defined in type "%2"
          //-----------------------------------------------
          GenErr (mid_e, ERR, 253, mk_sequence(PrintName(mid), PrintName(nm)));
          reswf = false;
        }
      }
      break;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> Utps (tp.GetSet(pos_REP_UnionTypeRep_tps));
      Bool onewf(false);
      SEQ<TYPE_AS_Name> nms;
      Generic g;
      for (bool bb = Utps.First (g); bb; bb = Utps.Next (g)) {
        TYPE_REP_CompositeTypeRep ctr (g);
        const TYPE_AS_Name & nm (ctr.GetRecord(pos_REP_CompositeTypeRep_nm));
        nms.ImpAppend (nm);
        Generic pair_g (LookUpTag(i, nm, Set()));
        if (! pair_g.IsNil ()) {
          Tuple pair (pair_g);
#ifdef VDMSL
          const SEQ<TYPE_REP_FieldRep> & fl (pair.GetSequence (2));
#endif // VDMSL
#ifdef VDMPP
          const Generic & fl_q (pair.GetField (2));
          Generic fl_tmp (CheckAccessCurClass (fl_q));
          if (fl_tmp.IsNil ()) {
            //-------------------
            // Error message #368
            // Access violation
            //-------------------
            GenErr (nm, ERR, 368, mk_sequence(PrintName (nm)));
            return false;
          }
          SEQ<TYPE_REP_FieldRep> fl (fl_tmp);
#endif // VDMPP
          MAP<TYPE_AS_Name, TYPE_REP_TypeRep> ftp;
          size_t len_fl = fl.Length();
          for (size_t idx = 1; idx <= len_fl; idx++) {
            const TYPE_REP_FieldRep & fr (fl[idx]);
            const Generic & fnm (fr.GetField(pos_REP_FieldRep_sel));
            if (! fnm.IsNil ()) {
              ftp.ImpModify (fnm, fr.GetRecord(pos_REP_FieldRep_tp));
            }
          }
          SET<TYPE_AS_Name> domm;
          size_t len_mods = mods.Length();
          for (size_t idm = 1; idm <= len_mods; idm++) {
            const TYPE_AS_RecordModification & ng (mods[idm]);
            const TYPE_AS_Name & mnm (ng.GetRecord(pos_AS_RecordModification_field));
            const TYPE_AS_Expr & new_e (ng.GetRecord(pos_AS_RecordModification_newexpr));

            if (ftp.DomExists (mnm)) {
              Tuple infer (wf_Expr (i, new_e, ftp[mnm]));
              const Bool & wf_arg (infer.GetBool (1));
              const TYPE_REP_TypeRep & argtp (infer.GetRecord (2));

              reswf = reswf && (wf_arg.GetValue());

              if (IsCompatible (i, argtp, ftp[mnm])) {
                onewf = Bool(true);
              }
              else {
                if (i == DEF) {
                  //-----------------------------------------------------------
                  // Error message #251
                  // Type of field L"%1" is not compatible with its defined type
                  //-----------------------------------------------------------
                  GenErrTp (mnm, ERR, 251, argtp, ftp[mnm], mk_sequence(PrintName(mnm)));
                  reswf = false;
                }
              }
              if (domm.InSet (mnm)) {
                //--------------------------------------
                // Error message #348
                // Field L"%1" is modified more than once
                //--------------------------------------
                GenErr(mnm, WRN1, 348, mk_sequence(PrintName(mnm)));
              }
              domm.Insert (mnm);
            }
            else {
              if (i == DEF) {
                //-----------------------------------------------
                // Error message #253
                // Field with id L"%1" is not defined in type "%2"
                //-----------------------------------------------
                GenErr(mnm, ERR, 253, mk_sequence(PrintName(mnm), PrintName(nm)));
                reswf = false;
              }
            }
          }
        }
        else {
          if (i == DEF) {
            // There is no need to report an error here
            // as it is already done, when the record was created.
            // 28 Oct. 1998 11:05 -- Jesper K. Pedersen
            // GenErr(nm,ERR,248,mk_sequence(PrintName(nm)));
            reswf = false;
          }
        }
      }
      if (! onewf.GetValue ()) {
        size_t len_nms = nms.Length();
        SEQ<Char> s;
        for (size_t idx = 1; idx <= len_nms; idx++) {
          if (idx > 1) {
            s.ImpConc(SEQ<Char>(L", "));
          }
          s.ImpConc(PrintName(nms[idx]));
        }
        //------------------------------------------------------------------------
        // Error message #256
        // None of the composite types L"%1" have fields which are compatible with
        // given field modifications
        //------------------------------------------------------------------------
        GenErr(mods.Hd(), ERR, 256, mk_sequence(s));
        reswf = false;
      }
      break;
    }
    default:
      // This must not happen!

      break;
  }
  return reswf;
}

// wf_RecordModifierExpr
// i : TYPE`Ind
// vRecordModifier : AS`RecordModifierExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_RecordModifierExpr (const Int & i,
                                           const TYPE_AS_RecordModifierExpr & vRecordModifier,
                                           const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & rec (vRecordModifier.GetRecord(pos_AS_RecordModifierExpr_rec));
  const SEQ<TYPE_AS_RecordModification> & mods (vRecordModifier.GetSequence(pos_AS_RecordModifierExpr_modifiers));
  const TYPE_CI_ContextId & cid (vRecordModifier.GetInt(pos_AS_RecordModifierExpr_cid));

  Bool reswf (true);

  Tuple infer (wf_Expr(i, rec, rep_alltp));
  const Bool & wf_arg (infer.GetBool(1));
  const TYPE_REP_TypeRep & argtp (infer.GetRecord(2));

  TYPE_REP_TypeRep cmptp (mk_REP_CompositeTypeRep(ASTAUX::MkNameFromVoid (), SEQ<TYPE_REP_FieldRep>()));

  if (IsCompatible(i, argtp, cmptp)) {
    TYPE_REP_TypeRep r1tp (ExtractCompositeType(argtp));
    if (cmptp == r1tp) {
      //---------------------------------
      // Error message #257
      // Modifications cannot be analyzed
      //---------------------------------
      GenErr (rec, WRN1, 257, Sequence());
      GetCI().SetTypeInfo (cid, rep_alltp);
      return mk_(Bool(false), rep_alltp);
    }
    else {
      UpdateLastPosition (vRecordModifier);
      reswf &= CheckModifications (i, r1tp, mods, exptp);
      GetCI().SetTypeInfo (cid, r1tp);
      return mk_(wf_arg && reswf, argtp);
    }
  }
  else {
    //----------------------------
    // Error message #258
    // Rhs is not a composite type
    //----------------------------
    GenErrTp (rec, ERR , 258, argtp, cmptp, Sequence());
    GetCI().SetTypeInfo (cid, rep_alltp);
    return mk_(Bool(false), rep_alltp);
  }
}

#ifdef VDMPP
// CheckOverloadedApply
// p_i : TYPE`Ind
// p_overloadedTps : (set of REP`FnTypeRep | set of REP`OpTypeRep)
// p_argTps : seq of REP`TypeRep
// p_expr : (AS`Expr | AS`StateDesignator)
// p_repErr : bool
// p_retRng : bool
// ==> bool * REP`TypeRep
Tuple StatSem::CheckOverloadedApply(const Int & p_i,
                                         const SET<TYPE_REP_TypeRep> & p_overloadedTps,
                                         const SEQ<TYPE_REP_TypeRep> & p_argTps,
                                         const TYPE_AS_Expr & p_expr,
                                         bool p_repErr,
                                         bool p_retRng)
{
  SET<TYPE_REP_TypeRep> p_overloadedTps_q (p_overloadedTps);
  SET<TYPE_REP_TypeRep> l_fnTps;
  SET<TYPE_REP_TypeRep> l_compatTps;
  Generic b_tp;
  for (bool bb = p_overloadedTps_q.First(b_tp); bb; bb = p_overloadedTps_q.Next(b_tp)) {
    SEQ<TYPE_REP_TypeRep> b_tpDom (GetMethDom(b_tp));
    if (b_tpDom.Length() == p_argTps.Length()) {
      l_fnTps.Insert(b_tp);

      bool forall = true;
      size_t len_b_tpDom = b_tpDom.Length();
      for (size_t b_i = 1; (b_i <= len_b_tpDom) && forall; b_i++) {
        forall = IsCompatible(p_i, p_argTps[b_i], b_tpDom[b_i]);
      }
      if (forall) {
        l_compatTps.Insert(b_tp);
      }
    }
  }

  if (l_fnTps.IsEmpty()) {
    // -------------------------------
    // -- Error message #385
    // -- No overloaded function or operation with this
    // -- number of arguments exists
    // -------------------------------
    if (p_repErr) {
      GenErr(p_expr, ERR, 385, Sequence());
    }
    return mk_(Bool(false), rep_alltp);
  }

  switch(l_compatTps.Card()) {
    case 0: {
      // -------------------------------
      // -- Error message #386
      // -- No overloaded function or operation matches
      // -- these arguments
      // -------------------------------
      if (p_repErr) {
        GenErr (p_expr, ERR, 386, Sequence());
      }
      return mk_(Bool(false), rep_alltp);
    }
    case 1: {
      TYPE_REP_TypeRep l_tp (l_compatTps.GetElem());
      switch(l_tp.GetTag()) {
        case TAG_TYPE_REP_OpTypeRep: {
          if (p_retRng) {
            return mk_(Bool(true), l_tp.GetField(pos_REP_OpTypeRep_Rng));
          }
          else {
            return mk_(Bool(true), l_tp);
          }
        }
        case TAG_TYPE_REP_PartialFnTypeRep: {
          return mk_(Bool(true), l_tp.GetField(pos_REP_PartialFnTypeRep_fnrng));
        }
        case TAG_TYPE_REP_TotalFnTypeRep: {
          return mk_(Bool(true), l_tp.GetField(pos_REP_TotalFnTypeRep_fnrng));
        }
        default: {
          return mk_(Bool(false), rep_alltp); // dummy
        }
      }
    }
    default: { // l_compatTps.Card() > 1)
      // -------------------------------
      // -- Error message #387
      // -- More than one overloaded function or
      // -- operation matches these arguments
      // -------------------------------
      if (p_repErr) {
       GenErr (p_expr, ERR, 387, Sequence());
      }
      return mk_(Bool(false), rep_alltp);
    }
  }
}
#endif //VDMPP

// CheckBasicApply
// i : TYPE`Ind
// tpl : REP`TypeRep
// tp_l : seq of REP`TypeRep
// vApply : (AS`ApplyExpr | AS`StateDesignator)
// callerr : bool
// ==> bool * REP`TypeRep
Tuple StatSem::CheckBasicApply (const Int & i,
                                const TYPE_REP_TypeRep & tp1,
                                const SEQ<TYPE_REP_TypeRep> & tp_l,
                                const Record & vApply,
                                const Bool & callerr)
{
  switch(tp1.GetTag()) {
    case TAG_TYPE_REP_SeqTypeRep: {
      const TYPE_REP_TypeRep & stp (tp1.GetRecord(pos_REP_SeqTypeRep_elemtp));
      if (tp_l.Length() == 1) {
        if (IsCompatible (i, tp_l.Hd(), btp_natone)) {
          if (i == DEF && (Settings.ErrorLevel() >= ERR2)) {
            //-------------------------------------------------
            // Error message #265
            // Given index may not exist for the given sequence
            //-------------------------------------------------
            if (callerr) {
              GenErr (vApply, ERR2, 265, Sequence());
             }
            return mk_(Bool(false), stp);
          }
          else {
            return mk_(Bool(true), stp);
          }
        }
        else {
          //---------------------------------------
          // Error message #268
          // Sequence index is not a natural number
          //---------------------------------------
          if (callerr) {
            GenErrTp (vApply, ERR, 268, tp_l.Hd(), btp_natone, Sequence());
          }
          return mk_(Bool(false), stp);
        }
      }
      else {
        //---------------------------------------------
        // Error message #266
        // Sequence is applied with more than one index
        //---------------------------------------------
        if (callerr) {
          GenErr (vApply, ERR, 266, Sequence());
        }
        return mk_(Bool(false), stp);
      }
    }
    case TAG_TYPE_REP_GeneralMapTypeRep:
    case TAG_TYPE_REP_InjectiveMapTypeRep: {
      const TYPE_REP_TypeRep & Dom (tp1.GetRecord(1));
      const TYPE_REP_TypeRep & Rng (tp1.GetRecord(2));
  
      if (tp_l.Length() == 1) {
        if (IsCompatible (i, tp_l.Hd(), Dom)) {
          if (i == DEF && (Settings.ErrorLevel() >= ERR2)) {
            //-----------------------------------------------------
            // Error message #269
            // Given domain element may not exist for the given map
            //-----------------------------------------------------
            if (callerr) {
              GenErr (vApply, ERR2, 269, Sequence());
            }
            return mk_(Bool(false), Rng);
          }
          else {
            return mk_(Bool(true), Rng);
          }
        }
        else {
          //----------------------------------
          // Error message #270
          // Map domain type of incorrect type
          //----------------------------------
          if (callerr) {
            GenErrTp (vApply, ERR, 270, tp_l.Hd(), Dom, Sequence());
          }
          return mk_(Bool(false), Rng);
        }
      }
      else {
        //----------------------------------------
        // Error message #271
        // Map is applied with more than one value
        //----------------------------------------
        if (callerr) {
          GenErr (vApply, ERR, 271, Sequence());
        }
        return mk_(Bool(false), Rng);
      }
    }
    case TAG_TYPE_REP_TotalFnTypeRep:
    case TAG_TYPE_REP_PartialFnTypeRep: {
      // TODO: When checking type invariant, dom types must not contain the checking type.
      const SEQ<TYPE_REP_TypeRep> & Dom (tp1.GetSequence(1));
      const TYPE_REP_TypeRep & Rng (tp1.GetRecord(2));

      if (Dom.Length() == tp_l.Length()) {
        size_t len_Dom = Dom.Length();
        bool reswf = true;
        for (size_t index = 1; index <= len_Dom; index++) {
          reswf &= IsCompatible (i, tp_l[index], Dom[index]);
        }
        if (reswf) {
          return mk_(Bool(true), Rng);
        }
        else {
          //------------------------------------------------------------
          // Error message #272
          // Function is not applied with parameters of the correct type
          //------------------------------------------------------------
          if (callerr) {
            GenErrTp (vApply, ERR, 272, mk_REP_ProductTypeRep(tp_l), mk_REP_ProductTypeRep(Dom), Sequence());
          }
          return mk_(Bool(false), Rng);
        }
      }
      else {
        //-------------------------------------------------
        // Error message #273
        // Function applied with wrong number of parameters
        //-------------------------------------------------
        if (callerr) {
          GenErr (vApply, ERR, 273, Sequence());
        }
        return mk_(Bool(false), Rng);
      }
    }
    case TAG_TYPE_REP_OpTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & Dom (tp1.GetSequence(pos_REP_OpTypeRep_Dom));
      const TYPE_REP_TypeRep & Rng (tp1.GetRecord(pos_REP_OpTypeRep_Rng));

#ifdef VDMSL
      if (Settings.VDMSLmode()) {
        //-----------------------------
        // Error message #349
        // Operation call in expression
        //-----------------------------
        if (callerr) {
          GenErr (vApply, ERR, 349, Sequence());
        }
        return mk_(Bool(false), Rng);
      }
      if (IsInVDContext()) {
        //-----------------------------
        // Error message #349
        // Operation call in expression
        //-----------------------------
        GenErr (vApply, WRN1, 349, Sequence());
      }
#endif // VDMSL

      if ((Settings.ErrorLevel() >= ERR2) && ((GetContext() == PRE) || (GetContext() == POST))) {
        //-----------------------------
        // Error message #448
        // Operation call in pre/post function
        //-----------------------------
        GenErr (vApply, WRN1, 448, Sequence());
      }

      if (Dom.Length() == tp_l.Length()) {
        size_t len_Dom = Dom.Length();
        bool reswf = true;
        for (size_t index = 1; index <= len_Dom; index++) {
          reswf &= IsCompatible (i, tp_l[index], Dom[index]);
        }
        if (reswf) {
          return mk_(Bool(true), Rng);
        }
        else {
          //-------------------------------------------------------------
          // Error message #274
          // Operation is not applied with parameters of the correct type
          //-------------------------------------------------------------
          if (callerr) {
            GenErrTp (vApply, ERR, 274, mk_REP_ProductTypeRep(tp_l), mk_REP_ProductTypeRep(Dom), Sequence());
          }
          return mk_(Bool(false), Rng);
        }
      }
      else {
        //-------------------------------------------------
        // Error message #275
        // Operation applied with wrong number of arguments
        //-------------------------------------------------
        if (callerr) {
          GenErr (vApply, ERR, 275, Sequence());
        }
        return mk_(Bool(false), Rng);
      }
    }
    default: {
      return Tuple(); //This is only to avoid warnings from the compiler
    }
  }
}

// CheckUnionApply
// i : TYPE`Ind
// tpl : REP`TypeRep
// tp_l : seq of REP`TypeRep
// vApply : (AS`Expr | AS`StateDesignator)
// ==> bool * REP`TypeRep
Tuple StatSem::CheckUnionApply (const Int & i,
                                     const TYPE_REP_TypeRep & tp1,
                                     const SEQ<TYPE_REP_TypeRep> & tp_l,
                                     const Record & vApply)
{
  int msgtp ((i == POS) ? WRN2 : ERR);

  switch(tp1.GetTag()) {
    case TAG_TYPE_REP_SeqTypeRep: {
      const TYPE_REP_TypeRep & stp (tp1.GetRecord(pos_REP_SeqTypeRep_elemtp));
      if (tp_l.Length() == 1) {
        if (IsCompatible (i, tp_l.Hd(), btp_natone)) {
          if (i == DEF && (Settings.ErrorLevel() >= ERR2)) {
            //-------------------------------------------------
            // Error message #265
            // Given index may not exist for the given sequence
            //-------------------------------------------------
            GenErr (vApply, ERR2, 265, Sequence());
            return mk_(Bool(false), stp);
          }
          else {
            return mk_(Bool(true), stp);
          }
        }
        else {
          //---------------------------------------
          // Error message #268
          // Sequence index is not a natural number
          //---------------------------------------
          GenErrTp (vApply, msgtp, 268, tp_l.Hd(), btp_natone, Sequence());
          return mk_(Bool(false), rep_alltp);
        }
      }
      else {
        //---------------------------------------------
        // Error message #266
        // Sequence is applied with more than one index
        //---------------------------------------------
        GenErr (vApply, msgtp, 266, Sequence());
        return mk_(Bool(false), rep_alltp);
      }
    }
    case TAG_TYPE_REP_GeneralMapTypeRep:
    case TAG_TYPE_REP_InjectiveMapTypeRep: {
      const TYPE_REP_TypeRep & Dom (tp1.GetRecord(1));
      const TYPE_REP_TypeRep & Rng (tp1.GetRecord(2));
      if (tp_l.Length() == 1) {
        if (IsCompatible (i, tp_l.Hd(), Dom)) {
          if (i == DEF && (Settings.ErrorLevel() >= ERR2)) {
            //-----------------------------------------------------
            // Error message #269
            // Given domain element may not exist for the given map
            //-----------------------------------------------------
            GenErr (vApply, ERR2, 269, Sequence());
            return mk_(Bool(false), Rng);
          }
          else {
            return mk_(Bool(true), Rng);
          }
        }
        else {
          //----------------------------------
          // Error message #270
          // Map domain type of incorrect type
          //----------------------------------
          GenErrTp (vApply, msgtp, 270, tp_l.Hd(), Dom, Sequence());
          return mk_(Bool(false), Rng);
        }
      }
      else {
        //----------------------------------------
        // Error message #271
        // Map is applied with more than one value
        //----------------------------------------
        GenErr (vApply, msgtp, 271, Sequence());
        return mk_(Bool(false), Rng);
      }
    }
    case TAG_TYPE_REP_TotalFnTypeRep:
    case TAG_TYPE_REP_PartialFnTypeRep: {
      const Generic & g (tp1.GetField(1));
      if (!g.IsRecord()) {
        const SEQ<TYPE_REP_TypeRep> & Dom (tp1.GetSequence(1));
        const TYPE_REP_TypeRep & Rng (tp1.GetRecord(2));

        size_t len_tp_l = tp_l.Length();
        if ((size_t)(Dom.Length()) == len_tp_l) {
          if ((len_tp_l == 1) &&
              (tp_l[1].Is (TAG_TYPE_REP_PartialFnTypeRep) || tp_l[1].Is (TAG_TYPE_REP_TotalFnTypeRep)) &&
              (Rng.Is (TAG_TYPE_REP_PartialFnTypeRep) || Rng.Is (TAG_TYPE_REP_TotalFnTypeRep))) {
            return CheckHigherOrder(i, tp_l[i], Dom, Rng, vApply);
          }
          size_t len_Dom = Dom.Length();
          bool reswf = true;
          for (size_t index = 1; index <= len_Dom; index++) {
            reswf = IsCompatible (i, tp_l[index], Dom[index]) && reswf;
          }
          if (reswf) {
            return mk_(Bool(true), Rng);
          }
          else {
            //------------------------------------------------------------
            // Error message #272
            // Function is not applied with parameters of the correct type
            //------------------------------------------------------------
            GenErrTp (vApply, msgtp, 272, mk_REP_ProductTypeRep(tp_l), mk_REP_ProductTypeRep(Dom), Sequence());
            return mk_(Bool(false), Rng);
          }
        }
        else {
          //-------------------------------------------------
          // Error message #273
          // Function applied with wrong number of parameters
          //-------------------------------------------------
          GenErr (vApply, msgtp, 273, Sequence());
          return mk_(Bool(false), Rng);
        }
      }
      else {
        return mk_(Bool(false), tp1.GetField(2));
      }
    }
    case TAG_TYPE_REP_OpTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & Dom (tp1.GetSequence(pos_REP_OpTypeRep_Dom));
      const TYPE_REP_TypeRep & Rng (tp1.GetRecord(pos_REP_OpTypeRep_Rng));

      if (Dom.Length() == tp_l.Length()) {
        size_t len_Dom = Dom.Length();
        bool reswf = true;
        for (size_t index = 1; index <= len_Dom; index++) {
          reswf = IsCompatible (i, tp_l[index], Dom[index]) && reswf;
        }
        if (reswf) {
          return mk_(Bool(true), Rng);
        }
        else {
          //-------------------------------------------------------------
          // Error message #274
          // Operation is not applied with parameters of the correct type
          //-------------------------------------------------------------
          GenErrTp (vApply, msgtp, 274, mk_REP_ProductTypeRep(tp_l), mk_REP_ProductTypeRep(Dom), Sequence());
          return mk_(Bool(false), Rng);
        }
      }
      else {
        //-------------------------------------------------
        // Error message #275
        // Operation applied with wrong number of arguments
        //-------------------------------------------------
        GenErr (vApply, msgtp, 275, Sequence());
        return mk_(Bool(false), Rng);
      }
    }
    default: {
      return Tuple(); //This is only to avoid warnings from the compiler
    }
  }
}

// CheckHigherOrder
// i : TYPE`Ind
// argtp : REP`TypeRep
// Dom : seq1 of REP`TypeRep
// Rng : REP`TypeRep
// vApply : (AS`Expr | AS`StateDesignator)
// ==> bool * REP`TypeRep
Tuple StatSem::CheckHigherOrder (const Int & i,
                                      const TYPE_REP_TypeRep & argtp,
                                      const SEQ<TYPE_REP_TypeRep> & Dom,
                                      const TYPE_REP_TypeRep & Rng,
                                      const Record & vApply)
{
  SEQ<Sequence> realdomtp; // seq1 of seq1 of REP`TypeRep
  realdomtp.ImpAppend (Dom);

  TYPE_REP_TypeRep restargtp (argtp);
  TYPE_REP_TypeRep restRng (Rng);

  bool stop = false;
  while(!stop) {
    stop = true;
    if ((restargtp.Is (TAG_TYPE_REP_PartialFnTypeRep) || restargtp.Is (TAG_TYPE_REP_TotalFnTypeRep)) &&
        (restRng.Is (TAG_TYPE_REP_PartialFnTypeRep) || restRng.Is (TAG_TYPE_REP_TotalFnTypeRep))) {
      TYPE_REP_TypeRep atp (realdomtp [realdomtp.Length ()].Hd());
      if (!(atp.Is (TAG_TYPE_REP_PartialFnTypeRep) || atp.Is (TAG_TYPE_REP_TotalFnTypeRep))) {
        switch(restRng.GetTag()) {
          case TAG_TYPE_REP_PartialFnTypeRep: {
            realdomtp.ImpAppend (restRng.GetSequence (pos_REP_PartialFnTypeRep_fndom));
            restRng = restRng.GetRecord (pos_REP_PartialFnTypeRep_fnrng);
            break;
          }
          case TAG_TYPE_REP_TotalFnTypeRep: {
            realdomtp.ImpAppend (restRng.GetSequence (pos_REP_TotalFnTypeRep_fndom));
            restRng = restRng.GetRecord (pos_REP_TotalFnTypeRep_fnrng);
            break;
          }
        }
        switch(restargtp.GetTag()) {
          case TAG_TYPE_REP_PartialFnTypeRep: {
            restargtp = restargtp.GetRecord (pos_REP_PartialFnTypeRep_fnrng);
            break;
          }
          case TAG_TYPE_REP_TotalFnTypeRep: {
            restargtp = restargtp.GetRecord (pos_REP_TotalFnTypeRep_fnrng);
            break;
          }
        }
        stop = false;
      }
    }
  }

  bool reswf ((realdomtp.Length () == 1) ? IsCompatible(i, argtp, realdomtp.Hd ().Hd ())
                                         : IsCompatible(i, argtp, CreateFnType(realdomtp)));

  if (reswf) {
    return mk_(Bool(reswf), restRng);
  }
  else {
    //------------------------------------------------------------
    // Error message #272
    // Function is not applied with parameters of the correct type
    //------------------------------------------------------------
    //GenErr(vApply, ERR, 272, Sequence());
    if (realdomtp.Length () == 1) {
      GenErrTp (vApply, ERR, 272, argtp, realdomtp.Hd ().Hd (), Sequence());
    }
    else {
      GenErrTp (vApply, ERR, 272, argtp, CreateFnType(realdomtp), Sequence());
    }
    return mk_(Bool (false), restRng);
  }
}

// CreateFnType
// tp_l_l : seq of seq1 of REP`TypeRep
// +> REP`PartialFnTypeRep
TYPE_REP_PartialFnTypeRep StatSem::CreateFnType (const Sequence & tp_l_l)
{
  if (tp_l_l.Length () > 2) {
    return mk_REP_PartialFnTypeRep(tp_l_l.Hd (), CreateFnType(tp_l_l.Tl ()));
  }
  else { // tp_l_l.Length () == 2
    SEQ<TYPE_REP_TypeRep> tp2_l (tp_l_l[2]);
    if (tp2_l.Length () == 1) {
      return mk_REP_PartialFnTypeRep(tp_l_l.Hd (), tp2_l.Hd ());
    }
    else {
      return mk_REP_PartialFnTypeRep(tp_l_l.Hd (), mk_REP_ProductTypeRep(tp2_l));
    }
  }
}

// wf_ApplyExpr
// i : TYPE`Ind
// vApply : AS`ApplyExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_ApplyExpr (const Int & i, const TYPE_AS_ApplyExpr & vApply, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & fct (vApply.GetRecord(pos_AS_ApplyExpr_fct));
  const SEQ<TYPE_AS_Expr> & arg (vApply.GetSequence(pos_AS_ApplyExpr_arg));
  const TYPE_CI_ContextId & cid (vApply.GetInt(pos_AS_ApplyExpr_cid));

  SET<TYPE_REP_TypeRep> st(mk_set(mk_REP_PartialFnTypeRep(rep_alltp, exptp),
                                  mk_REP_TotalFnTypeRep(rep_alltp, exptp),
                                  mk_REP_GeneralMapTypeRep(rep_alltp, exptp),
                                  mk_REP_InjectiveMapTypeRep(rep_alltp, exptp),
                                  mk_REP_SeqTypeRep(exptp)));

  TYPE_REP_TypeRep fnmapseqtp (mk_REP_UnionTypeRep(st));

#ifdef VDMSL
  Tuple infer (wf_Expr(i, fct, fnmapseqtp));
  const Bool & wf_fct (infer.GetBool(1));
  const TYPE_REP_TypeRep & fcttp (infer.GetRecord(2));
#endif // VDMSL

#ifdef VDMPP
  Bool wf_fct;
  TYPE_REP_TypeRep fcttp;
  if (fct.Is(TAG_TYPE_AS_Name)) {
    Generic gtp (LookUpInObject (GetCurClass (), fct, false, false));
    if (gtp.IsSet()) {
      wf_fct = Bool(true);
      fcttp = mk_REP_OverTypeRep(CheckAccessCurClass (gtp));
      GetCI().SetTypeInfo (fct.GetInt(pos_AS_Name_cid), fcttp);
    }
    else {
      Tuple infer (wf_Expr(i, fct, fnmapseqtp));
      wf_fct = (infer.GetBool(1));
      fcttp = (infer.GetRecord(2));
    }
  }
  else {
    Tuple infer (wf_Expr(i, fct, fnmapseqtp));
    wf_fct = (infer.GetBool(1));
    fcttp = (infer.GetRecord(2));
  }
#endif // VDMPP

  Bool wf_arg (true);
  SEQ<TYPE_REP_TypeRep> argtp_l;
  size_t len_arg = arg.Length();
  for (size_t idx = 1; idx <= len_arg; idx++) {
    Tuple infer2 (wf_Expr (i, arg[idx], rep_alltp));
    wf_arg = infer2.GetBool (1) && wf_arg;
    argtp_l.ImpAppend(infer2.GetRecord (2));
  }

  Tuple infer3 (CheckApply (i, fcttp, argtp_l, vApply, Bool(true)));
  const Bool & wf_apply (infer3.GetBool (1));
  const TYPE_REP_TypeRep & res_tp (infer3.GetRecord (2));

  if (res_tp.Is(TAG_TYPE_REP_UnitTypeRep)) {
    //-------------------------------
    // Error message #259
    // Apply-Expr must return a value
    //-------------------------------
    GenErr (vApply, ERR, 259, Sequence());
    GetCI().SetTypeInfo (cid, rep_alltp);
    return mk_(Bool (false), rep_alltp);
  }
  else {
    GetCI().SetTypeInfo (cid, res_tp);
    return mk_(wf_fct && wf_arg && wf_apply, res_tp);
  }
}

// CheckApply
// i : TYPE`Ind
// p_fcttp : REP`TypeRep
// argtp_l : seq of REP`TypeRep
// vApply : (AS`ApplyExpr | AS`StateDesignator)
// callerr : bool
// ==> bool * (REP`TypeRep | set of REP`TypeRep)
Tuple StatSem::CheckApply (const Int & i,
                           const TYPE_REP_TypeRep & p_fcttp,
                           const SEQ<TYPE_REP_TypeRep> & argtp_l,
                           const Record & vApply,
                           const Bool & callerr)
{
  Bool defwf (true);
  Bool poswf (true);
  Bool anyfct (false);
  Bool allfct (true);

  Generic realfcttp = p_fcttp;

  SET<TYPE_REP_TypeRep> st (mk_set(mk_REP_SeqTypeRep(rep_alltp),
                                   mk_REP_GeneralMapTypeRep(rep_alltp, rep_alltp),
                                   mk_REP_InjectiveMapTypeRep(rep_alltp, rep_alltp),
                                   mk_REP_PartialFnTypeRep(mk_sequence(rep_alltp), rep_alltp),
                                   mk_REP_TotalFnTypeRep(mk_sequence(rep_alltp), rep_alltp),
                                   mk_REP_OpTypeRep(mk_sequence(rep_alltp), rep_alltp)));

  TYPE_REP_TypeRep ExpectedTypeRep (mk_REP_UnionTypeRep(st));

  while (realfcttp.Is(TAG_TYPE_REP_TypeNameRep) && !IsUnionRecursive (realfcttp)) {
#ifdef VDMPP
    Generic acctp = Nil ();
    Generic realfcttp_q (LookUpTypeName (Record(realfcttp).GetRecord(pos_REP_TypeNameRep_nm), true));
    if (IsAccessType (realfcttp_q)) {
      realfcttp = StripAccessType(realfcttp_q);
      acctp = realfcttp_q;
    }
    else {
      realfcttp = realfcttp_q;
      acctp = Nil ();
    }

    if (!acctp.IsNil () && !IsAccessibleCurClass (acctp)) {
      const TYPE_AS_Name & nm (Record (realfcttp).GetRecord (pos_REP_TypeNameRep_nm));
      if (callerr) {
        //-------------------
        // Error message #368
        // Access violation
        //-------------------
        GenErr (nm, ERR, 368, mk_sequence(PrintName (nm)));
      }
      return mk_(Bool(false), rep_alltp);
    }
#endif // VDMPP
#ifdef VDMSL
    realfcttp = LookUpTypeName (Record(realfcttp).GetRecord(pos_REP_TypeNameRep_nm));
#endif // VDMSL
  }

  if (realfcttp.IsNil()) {
    return mk_(Bool(false), rep_alltp);
  } 
  else {
    TYPE_REP_TypeRep fcttp (realfcttp);
    switch (fcttp.GetTag()) {
      case TAG_TYPE_REP_AllTypeRep:
      case TAG_TYPE_REP_TypeNameRep: {
        return mk_(Bool(false), rep_alltp);
      }
      case TAG_TYPE_REP_InvTypeRep: {
        if (i == DEF && (Settings.ErrorLevel() >= ERR2)) {
          //----------------------------------
          // Error message #260
          // Invariant type used in Apply-Expr
          //----------------------------------
          if (callerr) {
            GenErr (vApply, ERR2, 260, Sequence());
          }
          Tuple infer (CheckApply (i, fcttp.GetRecord(pos_REP_InvTypeRep_shape), argtp_l, vApply, callerr));
          return mk_(Bool(false), infer.GetField (2));
        }
        else {
          return CheckApply (i, fcttp.GetRecord(pos_REP_InvTypeRep_shape), argtp_l, vApply, callerr);
        }
      }
      case TAG_TYPE_REP_SeqTypeRep:
      case TAG_TYPE_REP_GeneralMapTypeRep:
      case TAG_TYPE_REP_InjectiveMapTypeRep:
      case TAG_TYPE_REP_PartialFnTypeRep:
      case TAG_TYPE_REP_TotalFnTypeRep:
      case TAG_TYPE_REP_OpTypeRep: {
        return CheckBasicApply (i, fcttp, argtp_l, vApply, callerr);
      }
      case TAG_TYPE_REP_UnionTypeRep: {
        SET<TYPE_REP_TypeRep> utps (fcttp.GetSet(pos_REP_UnionTypeRep_tps));

        if (utps.InSet(mk_REP_EmptySeqTypeRep(rep_alltp)) && (utps.Card () == 2)) {
          SET<TYPE_REP_TypeRep> tmps (utps.Diff(mk_set(mk_REP_EmptySeqTypeRep(rep_alltp))));
          TYPE_REP_TypeRep rc (tmps.GetElem());
          if (rc.Is (TAG_TYPE_REP_SeqTypeRep)) {
            if (i == DEF && (Settings.ErrorLevel() >= ERR2)) {
              Tuple infer (CheckBasicApply (i, rc, argtp_l, vApply, callerr));
              //----------------------
              // Error message #261
              // Sequence may be empty
              //----------------------
              if (callerr) {
                GenErr (vApply, ERR2, 261, Sequence());
              }
              return mk_(Bool (false), infer.GetField(2));
            }
            else {
              return CheckBasicApply (i, rc, argtp_l, vApply, callerr);
            }
          }
        }

        if (utps.InSet(mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp)) && (utps.Card () == 2)) {
          SET<TYPE_REP_TypeRep> tmps (utps.Diff(mk_set(mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp))));
          TYPE_REP_TypeRep rc (tmps.GetElem());
          if (rc.Is (TAG_TYPE_REP_GeneralMapTypeRep)) {
            if (i == DEF && (Settings.ErrorLevel() >= ERR2)) {
              Tuple infer (CheckBasicApply (i, rc, argtp_l, vApply, callerr));
              //--------------------
              // Error message #262
              // Map may be empty
              //--------------------
              if (callerr) {
                GenErr (vApply, ERR2, 262, Sequence());
              }
              return mk_(Bool (false), infer.GetField(2));
            }
            else {
              return CheckBasicApply (i, rc, argtp_l, vApply, callerr);
            }
          }
          else if (rc.Is (TAG_TYPE_REP_InjectiveMapTypeRep)) {
            if (i == DEF && (Settings.ErrorLevel() >= ERR2)) {
              Tuple infer (CheckBasicApply (i, rc, argtp_l, vApply, callerr));
              //--------------------
              // Error message #262
              // Map may be empty
              //--------------------
              if (callerr) {
                GenErr (vApply, ERR2, 262, Sequence());
              }
              return mk_(Bool (false), infer.GetField(2));
            }
            else {
              return CheckBasicApply (i, rc, argtp_l, vApply, callerr);
            }
          }
        }

        Generic restp = Nil();
        Generic gutp;
        for (bool bb = utps.First(gutp); bb; bb = utps.Next(gutp)) {
          TYPE_REP_TypeRep utp (gutp);
          if (utp.Is(TAG_TYPE_REP_SeqTypeRep) ||
              utp.Is(TAG_TYPE_REP_GeneralMapTypeRep) ||
              utp.Is(TAG_TYPE_REP_InjectiveMapTypeRep) ||
              utp.Is(TAG_TYPE_REP_PartialFnTypeRep) ||
              utp.Is(TAG_TYPE_REP_TotalFnTypeRep) ||
              utp.Is(TAG_TYPE_REP_TypeNameRep) ||
              utp.Is(TAG_TYPE_REP_InvTypeRep) ||
              utp.Is(TAG_TYPE_REP_OpTypeRep)) {
            Tuple infer (2);
            if (utp.Is(TAG_TYPE_REP_InvTypeRep) ||
                utp.Is(TAG_TYPE_REP_TypeNameRep)) {
              Tuple infer1 (CanBeApplied (utp));
              const Bool & ok (infer1.GetBool (1));
              const Generic & t (infer1.GetField (2));

              if (ok) {
                infer = CheckApply (i, t, argtp_l, vApply, Bool(false));
              }
              else {
                infer = mk_(Bool(false), rep_alltp);
              }
            }
            else {
              infer = CheckUnionApply (i, utp, argtp_l, vApply);
            }
            const Bool & wf (infer.GetBool (1));
            const TYPE_REP_TypeRep & tp (infer.GetRecord (2));

#ifdef VDMSL
            if (Settings.VDMSLmode() && utp.Is(TAG_TYPE_REP_OpTypeRep)) {
              //-----------------------------
              // Error message #349
              // Operation call in expression
              //-----------------------------
              if (callerr) {
                GenErr (vApply, ERR, 349, Sequence());
              }
              defwf = false;
              poswf = false;
            }
            else
#endif // VDMSL
            {
              defwf &= wf;
              poswf |= wf;
            }

            if (restp.IsNil()) {
              restp = tp;
            }
            else if (! tp.Is(TAG_TYPE_REP_AllTypeRep)) {
              if (! restp.Is(TAG_TYPE_REP_AllTypeRep)) {
                restp = MergeTypes (restp, tp);
              }
              else {
                restp = tp;
              }
            }
            anyfct |= wf; // anyfct = 1;
          }
          else if (!utp.Is(TAG_TYPE_REP_EmptyMapTypeRep) && !utp.Is(TAG_TYPE_REP_EmptySeqTypeRep)) {
            allfct = false;
          }
          else if (Settings.ErrorLevel() >= ERR2) {
            allfct = false;
          }
        } // end of loop

        if (restp.IsNil ()) {
          restp = rep_alltp;
        }
        if (!anyfct) {
          //-----------------------------
          // Error message #263
          // Expression cannot be applied
          //-----------------------------
          if (callerr) {
            GenErrTp (vApply, ERR, 263, fcttp, ExpectedTypeRep, Sequence());
          }
          return mk_(Bool(false), restp);
        }

        if ((i == DEF) && !allfct) {
          //------------------------------------------
          // Error message #264
          // Expression cannot be applied in all cases
          //------------------------------------------
          if (callerr) {
            GenErr (vApply, ERR, 264, Sequence());
          }
          return mk_(Bool(false), restp);
        }

        Bool reswf ((i == POS) ? poswf : defwf);
        if (!reswf && callerr) {
          //--------------------
          // Error message #267
          // Error in Apply-Expr
          //--------------------
          GenErr (vApply, ERR, 267, Sequence());
        }

        return mk_(reswf, restp);
      } // end of TAG_TYPE_REP_UnionTypeRep
#ifdef VDMPP
      case TAG_TYPE_REP_OverTypeRep: {
        return CheckOverloadedApply(i, fcttp.GetSet(pos_REP_OverTypeRep_tps), argtp_l, vApply, true, true);
      }
#endif // VDMPP
      default: {
        //-----------------------------
        // Error message #263
        // Expression cannot be applied
        //-----------------------------
        if (callerr) {
          GenErrTp (vApply, ERR, 263, fcttp, ExpectedTypeRep, Sequence());
        }
        return mk_(Bool(false), rep_alltp);
      }
    }
  }
}

// wf_FctTypeInstExpr
// i : TYPE`Ind
// FctTypeExpr : AS`FctTypeInstExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_FctTypeInstExpr (const Int & i,
                                   const TYPE_AS_FctTypeInstExpr& FctTypeExpr,
                                   const TYPE_REP_TypeRep &)
{
  const TYPE_AS_Name & polyfct (FctTypeExpr.GetRecord(pos_AS_FctTypeInstExpr_polyfct));
#ifdef VDMPP
  const SEQ<TYPE_AS_Type> & inst (FctTypeExpr.GetSequence(pos_AS_FctTypeInstExpr_inst));
#endif // VDMPP
//  const TYPE_CI_ContextId & cid (FctTypeExpr.GetInt(pos_AS_FctTypeInstExpr_cid));

#ifdef VDMSL
  Generic poly (LookUpPolyFn (polyfct));
#endif // VDMSL
#ifdef VDMPP
  Generic p (LookUpInObject (GetCurClass (), polyfct, false, true));
  Generic p_q (p);
  if (p.IsNil ()) {
    p_q = LookUp (polyfct, true);
  }
  Generic poly = Nil ();
  if (!p_q.IsNil ()) {
    // [ REP`TypeRep | TagRepElem | set of (REP`FnTypeRep | REP`OpTypeRep | REP`PolyTypeRep) ]
    Generic poly_q (CheckAccessCurClass (p_q));

    if (poly_q.IsNil ()) {
      // ----------------------
      //  -- Error message #368
      //  -- Access violation
      //  ---------------------
      GenErr (polyfct, ERR, 368, mk_sequence(PrintName (polyfct)));
      return mk_(Bool(false), rep_alltp);
    }
    else if (poly_q.IsRecord()) {
      // poly_q : REP`TypeRep | TagRepElem
      if (poly_q.Is (TAG_TYPE_REP_PolyTypeRep)) {
        TYPE_REP_PolyTypeRep pp (poly_q);
        poly = mk_(pp.get_vars (), pp.get_tp ());
      }
    }
    else if (poly_q.IsSet ()) {
      // poly_q : set of (REP`FnTypeRep | REP`OpTypeRep | REP`PolyTypeRep)
      SET<TYPE_REP_PolyTypeRep> poly_q_s (poly_q), poly_set;
      Generic g;
      for (bool bb = poly_q_s.First(g); bb ; bb = poly_q_s.Next(g)) {
        TYPE_REP_PolyTypeRep pp (g);
        if (pp.get_vars().Length() == inst.Length()) {
          poly_set.Insert(pp);
        }
      }

      if (!poly_set.IsEmpty()) {
        Set p_s;
        Generic e;
        for (bool cc = poly_set.First(e); cc; cc = poly_set.Next(e)) {
          TYPE_REP_PolyTypeRep pp (e);
          p_s.Insert(mk_(pp.get_vars (), pp.get_tp ()));
        }
        if (p_s.Card() == 1) {
          poly = p_s.GetElem();
        }
        else {
          poly = p_s;
        }
      }  
    }
  }
#endif // VDMPP
  return CheckFctTypeInst (i, poly, FctTypeExpr);
}

// CheckFctTypeInst
// i : TYPE`Ind
#ifdef VDMSL
// poly : [(seq of REP`TypeVarRep) * REP`FnTypeRep]
#endif // VDMSL
#ifdef VDMPP
// poly : [((seq of REP`TypeVarRep) * REP`FnTypeRep) | set of ((seq of REP`TypeVarRep) * REP`FnTypeRep) ]
#endif // VDMPP
// field : AS`FctTypeInstExpr
// ==> bool * REP`TypeRep
Tuple StatSem::CheckFctTypeInst (const Int & i,
                                 const Generic & poly,
                                 const TYPE_AS_FctTypeInstExpr & field)
{
  const TYPE_AS_Name & polyfct (field.GetRecord(pos_AS_FctTypeInstExpr_polyfct));
  const SEQ<TYPE_AS_Type> & inst (field.GetSequence(pos_AS_FctTypeInstExpr_inst));
  const TYPE_CI_ContextId & cid (field.GetInt(pos_AS_FctTypeInstExpr_cid));

  Bool wf_inst (wf_TypeList (i, inst));

  if (poly.IsNil ()) {
    //------------------------------------
    // Error message #366
    // Unknown polymorphic identifier L"%1"
    //------------------------------------
    GenErr (polyfct, ERR, 366, mk_sequence(PrintName (polyfct)));
    GetCI().SetTypeInfo (cid, rep_alltp);
    return mk_(Bool(false), rep_alltp);
  }
#ifdef VDMPP
  else if(poly.IsSet()) {
    Set poly_s (poly);
    SET<TYPE_REP_TypeRep> t_s;
    Generic e;
    for (bool bb = poly_s.First(e); bb; bb = poly_s.Next(e)) {
      Tuple poly_t (e); // (seq of REP`TypeVarRep) * REP`FnTypeRep
      const SEQ<TYPE_REP_TypeVarRep> & tv_l (poly_t.GetSequence (1));
      //const TYPE_REP_TypeRep & fntp (poly_t.GetRecord (2));
      if (inst.Length () == tv_l.Length ()) {
        t_s.Insert(CheckFctTypeInstAux (e, field));
      }
    }
    switch (t_s.Card()) {
      case 0: {
        //----------------------------------------
        // Error message #281
        // All type variables must be instantiated
        //----------------------------------------
        GenErr (field, ERR, 281, Sequence());
        return mk_(Bool(false), rep_alltp);
      }
      case 1: {
        return mk_(wf_inst, t_s.GetElem());
      }
      default: {
        return mk_(wf_inst, mk_REP_OverTypeRep(t_s));
      }
    }
  }
#endif // VDMPP
  else  { // poly.IsTuple()
    Tuple poly_t (poly); // (seq of REP`TypeVarRep) * REP`FnTypeRep
    const SEQ<TYPE_REP_TypeVarRep> & tv_l (poly_t.GetSequence (1));
    const TYPE_REP_TypeRep & fntp (poly_t.GetRecord (2));
    if (inst.Length () != tv_l.Length ()) {
      //----------------------------------------
      // Error message #281
      // All type variables must be instantiated
      //----------------------------------------
      GenErr (field, ERR, 281, Sequence());

      MAP<TYPE_AS_Name,TYPE_REP_TypeRep> tpbd;
      size_t len_tv_l = tv_l.Length();
      for (size_t j = 1; j <= len_tv_l; j++) {
        tpbd.ImpModify (tv_l[j].get_nm (), rep_alltp);
      }
      TYPE_REP_TypeRep restp (InstFn (VAR, fntp, tpbd));
      GetCI().SetTypeInfo (cid, restp);
      return mk_(Bool(false), restp);
    }
    else {
      const TYPE_CI_ContextId & pcid (polyfct.GetInt(pos_AS_Name_cid));
      MAP<TYPE_AS_Name,TYPE_REP_TypeRep> tpbd;
      size_t len_tv_l = tv_l.Length();
      for (size_t j = 1; j <= len_tv_l; j++) {
        tpbd.ImpModify (tv_l[j].get_nm (), rep_alltp);
      }
      TYPE_REP_TypeRep restp (InstFn (VAR, fntp, tpbd));
      GetCI().SetTypeInfo (pcid, restp);
      return mk_(wf_inst, CheckFctTypeInstAux (poly, field));
    }
  }
}

// CheckFctTypeInstAux
// poly : ((seq of REP`TypeVarRep) * REP`FnTypeRep)
// field : AS`FctTypeInstExpr
// ==> REP`TypeRep
TYPE_REP_TypeRep StatSem::CheckFctTypeInstAux (const Tuple & poly, const TYPE_AS_FctTypeInstExpr & field)
{
  const SEQ<TYPE_REP_TypeVarRep> & tv_l (poly.GetSequence (1));
  const TYPE_REP_TypeRep & fntp (poly.GetRecord (2));

  const SEQ<TYPE_AS_Type> & inst (field.GetSequence(pos_AS_FctTypeInstExpr_inst));
  const TYPE_CI_ContextId & cid (field.GetInt(pos_AS_FctTypeInstExpr_cid));

  SEQ<TYPE_REP_TypeRep> tp_l (TransTypeList (Nil (), inst));
  MAP<TYPE_AS_Name,TYPE_REP_TypeRep> tpbd;

  size_t len_tp_l = tp_l.Length();
  for (size_t j = 1; j <= len_tp_l; j++) {
    tpbd.ImpModify (tv_l[j].GetRecord(pos_REP_TypeVarRep_nm), tp_l[j]);
  }
  TYPE_REP_TypeRep restp (InstFn (VAR, fntp, tpbd));
  GetCI().SetTypeInfo (cid, restp);
  return restp;
}

// wf_BracketedExpr
// i : TYPE`Ind
// vBracket : AS`BracketedExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_BracketedExpr (const Int & i,
                                      const TYPE_AS_BracketedExpr & vBracket,
                                      const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & expr (vBracket.GetRecord(pos_AS_BracketedExpr_expr));
  const TYPE_CI_ContextId & cid (vBracket.GetInt(pos_AS_BracketedExpr_cid));

  Tuple infer (wf_Expr (i, expr, exptp));
  const Bool & wf_e (infer.GetBool (1));
  const TYPE_REP_TypeRep & etp (infer.GetRecord (2));

  GetCI().SetTypeInfo (cid, etp);
  return mk_(wf_e, etp);
}

// wf_LambdaExpr
// i : TYPE`Ind
// VLambdaExpr : AS`LambdaExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_LambdaExpr (const Int & i,
                                   const TYPE_AS_LambdaExpr & VLambdaExpr,
                                   const TYPE_REP_TypeRep & /*exptp*/)
{
  const SEQ<TYPE_AS_TypeBind> & parms (VLambdaExpr.GetSequence(pos_AS_LambdaExpr_parm));
  const TYPE_AS_Expr & body (VLambdaExpr.GetRecord(pos_AS_LambdaExpr_body));
  const TYPE_CI_ContextId & cid (VLambdaExpr.GetInt(pos_AS_LambdaExpr_cid));

  Tuple infer (wf_TypeBindList (i, parms));
  const Generic & wf_parms (infer.GetField (1));
  const MAP<TYPE_AS_Name,Tuple> & bd (infer.GetMap (2));
  const SEQ<TYPE_REP_TypeRep> & tp_l (infer.GetSequence (3));

  EnterScope(bd);

  Tuple infer2 (wf_Expr (i, body, rep_alltp));
  const Bool & wf_body (infer2.GetBool (1));
  const TYPE_REP_TypeRep & tp (infer2.GetRecord (2));

  LeaveScope();

  TYPE_REP_TypeRep fntp (mk_REP_PartialFnTypeRep(tp_l, tp));

  GetCI().SetTypeInfo (cid, fntp);

  if (wf_parms.IsNil ()) {
    return mk_(Bool(false), fntp);
  }
  else if (wf_parms == Bool(false)) {
    //---------------------------------------------
    // Error message #282
    // No binding is possible in the parameter list
    //---------------------------------------------
    GenErr (VLambdaExpr, ERR,282, Sequence());
    return mk_(Bool(false), fntp);
  }
  else {
    return mk_(wf_body, fntp);
  }
}

// IsAValidField
// fnm : AS`Name
// tp : REP`CompositeTypeRep
// ==> bool * REP`TypeRep * [AS`Name] * [AS`Name]
Tuple StatSem::IsAValidField (const TYPE_AS_Name & fnm, const TYPE_REP_CompositeTypeRep & tp)
{
  const TYPE_AS_Name & tag (tp.GetRecord(pos_REP_CompositeTypeRep_nm));
  const SEQ<TYPE_REP_FieldRep> & flds (tp.GetSequence(pos_REP_CompositeTypeRep_fields));

  const TYPE_AS_Ids & ids (tag.GetSequence(pos_AS_Name_ids));

  if (ids.IsEmpty() || ids == TYPE_AS_Ids().ImpAppend(TYPE_AS_Id())) {
    return mk_(Bool(true), rep_alltp, Nil(), Nil());
  }

  size_t len_flds = flds.Length();
  for(size_t i = 1; i <= len_flds; i++) {
    const TYPE_REP_FieldRep & field (flds[i]);
    if (field.GetField(pos_REP_FieldRep_sel) == fnm) {
      return mk_(Bool (true), field.GetRecord(pos_REP_FieldRep_tp), Nil(), Nil());
    }
  }
  return mk_(Bool(false), rep_alltp, fnm, tag);
}

// wf_FieldSelectExpr
// i : TYPE`Ind
// vFieldSelect : AS`FieldSelectExpr
// exptp : REP`TypeRep
// ==> bool * (REP`TypeRep | set of REP`TypeRep)
Tuple StatSem::wf_FieldSelectExpr (const Int & i,
                                        const TYPE_AS_FieldSelectExpr & vFieldSelect,
                                        const TYPE_REP_TypeRep &)
{
  const TYPE_AS_Expr & rec      (vFieldSelect.GetRecord(pos_AS_FieldSelectExpr_rec));
  const Generic & field         (vFieldSelect.GetField(pos_AS_FieldSelectExpr_nm));
  const TYPE_CI_ContextId & cid (vFieldSelect.GetInt(pos_AS_FieldSelectExpr_cid));

  Tuple infer (wf_Expr(i, rec, rep_alltp));
  const Bool & wf_arg (infer.GetBool(1));
  const TYPE_REP_TypeRep & argtp (infer.GetRecord(2));

#ifdef VDMPP
  TYPE_REP_TypeRep ctp (mk_REP_CompositeTypeRep(ASTAUX::MkNameFromVoid (), SEQ<TYPE_REP_FieldRep>()));
  TYPE_REP_TypeRep otp (mk_REP_ObjRefTypeRep(ASTAUX::MkNameFromVoid ()));

  bool objRefComp    = IsCompatible (i, argtp, otp);
  bool compositeComp = IsCompatible (i, argtp, ctp);

  if (!(compositeComp || objRefComp)) {
    TYPE_REP_TypeRep utp (mk_REP_UnionTypeRep(mk_set(ctp, otp)));

    //---------------------------------------------------------------
    // Error message #277
    // Expression is not a composite type or an object reference type
    //---------------------------------------------------------------
    GenErrTp (field, ERR, 277, argtp, utp, Sequence());
    return mk_(Bool(false), rep_alltp);
  }

  if (!CheckQualifiedField(i, field, objRefComp, argtp)) {
    return mk_(Bool(false), rep_alltp);
  }
#endif //VDMPP

#ifdef VDMPP
  Bool stat (GetStatic());
  if (objRefComp) {
    SetStatic(false);
  }
#endif // VDMPP

  Tuple infer2 (CheckFieldSelect(i, argtp, field, false));
  const Bool & wf_field (infer2.GetBool(1));
  const Generic & res_tp (infer2.GetField(2));

#ifdef VDMPP
  SetStatic(stat);
#endif // VDMPP

  TYPE_REP_TypeRep l_actResTp;
  if (res_tp.IsRecord()) {
    l_actResTp = res_tp;
  }
  else {
    SET<TYPE_REP_TypeRep> res_tps (res_tp);
    if (res_tps.Card() == 1) {
      l_actResTp = res_tps.GetElem();
    }
    else {
#ifdef VDMSL
      l_actResTp = mk_REP_UnionTypeRep(res_tps);
#endif // VDMSL
#ifdef VDMPP
      l_actResTp = mk_REP_OverTypeRep(res_tps);
#endif // VDMPP
    }
  }
  GetCI().SetTypeInfo (cid, l_actResTp);
  return mk_(wf_arg && wf_field, l_actResTp);
}


#ifdef VDMPP
// CheckQualifiedField
// i : TYPE`Ind
// field : (AS`Name | AS`FctTypeInstExpr)
// objRefComp : bool
// objtp : REP`TypeRep
// ==> bool
bool StatSem::CheckQualifiedField (const Int & i,
                                        const Record & field,
                                        bool objRefComp,
                                        const TYPE_REP_TypeRep & objtp)
{
  if (!field.Is(TAG_TYPE_AS_Name)) {
    return true;
  }
  else {
    if (field.GetSequence(pos_AS_Name_ids).Length() != 2) {
      return true;
    }
    else if (!objRefComp) {
      // -------------------------------------
      //  -- Error message #396
      //  -- Composite type can not have qualified field
      //  -------------------------------------
      GenErr (field, ERR, 396, Sequence());
      return false;
    }
    else {
      SET<TYPE_AS_Name> cls;

      TYPE_REP_TypeRep rtp (ExtractObjRefType (objtp));

      switch(rtp.GetTag()) {
        case TAG_TYPE_REP_ObjRefTypeRep: {
          cls.Insert(rtp.GetField(pos_REP_ObjRefTypeRep_nm));
          break;
        }
        case TAG_TYPE_REP_UnionTypeRep: {
          SET<TYPE_REP_TypeRep> utps (rtp.GetSet(pos_REP_UnionTypeRep_tps));
          Generic utp;
          for (bool bb = utps.First(utp); bb ; bb = utps.Next(utp))
            cls.Insert(Record(utp).GetRecord(pos_REP_ObjRefTypeRep_nm));
        }
        default:
          break;
      }

      TYPE_AS_Ids ids;
      ids.ImpAppend(field.GetSequence(pos_AS_Name_ids).Hd());
      TYPE_AS_Name qualif (ASTAUX::MkNameFromIds(ids, NilContextId));

      bool qualif_ok;
      if (i == POS) {
        bool exists = false;
        Generic cl;
        for (bool bb = cls.First(cl); bb && !exists; bb = cls.Next(cl)) {
          exists = ((cl == qualif) || IsSubClass(cl, qualif));
        }
        qualif_ok = exists;
      }
      else {
        bool forall = true;
        Generic cl;
        for (bool bb = cls.First(cl); bb && forall; bb = cls.Next(cl)) {
          forall = ((cl == qualif) || IsSubClass(cl, qualif));
        }
        qualif_ok = forall;
      }

      if (qualif_ok) {
        return true;
      }
      else {
        // -------------------------------------
        //  -- Error message #394
        //  -- Class qualifier "%1" must be a subclass of the object class
        //  -------------------------------------
        GenErr (field, ERR, 394, mk_sequence(PrintName(qualif)));
        return false;
      }
    }
  }
}
#endif //VDMPP

// CheckFieldSelect
// i : TYPE`Ind
// rectp : REP`TypeRep
// field : (AS`Name|AS`FctTypeInstExpr)
// writable : bool
// ==> bool * (REP`TypeRep | set of REP`TypeRep)
Tuple StatSem::CheckFieldSelect (const Int & i, const TYPE_REP_TypeRep & rectp, const Record & field, bool writable)
{
  Tuple infer (CheckRecordFieldSelect(i, rectp, field));
#ifdef VDMPP
  bool reswf = infer.GetBoolValue(1);
#endif // VDMPP
  Generic restp = infer.GetField(2);

#ifdef VDMSL
  if (!restp.IsNil()) {
    return infer;
  }
  else {
    TYPE_REP_TypeRep ctp (mk_REP_CompositeTypeRep(ASTAUX::MkNameFromVoid (), SEQ<TYPE_REP_FieldRep>()));

    //-----------------------------------
    // Error message #276
    // Expression is not a composite type
    //-----------------------------------
    GenErrTp (field, ERR, 276, rectp, ctp, Sequence());
    return mk_(Bool(false), rep_alltp);
  }
#endif //VDMSL

#ifdef VDMPP
  Tuple infer2 (CheckObjRefFieldSelect(i, rectp, field, writable));
  bool wf = infer2.GetBoolValue(1);
  const Generic & tp (infer2.GetField(2));

  reswf = reswf || wf;

  if (!restp.IsNil() && !tp.IsNil()) {
    restp = MergeTypes(restp, tp);
  }
  else if (wf) {
    restp = tp;
  }

  if (!restp.IsNil()) {
    return mk_(Bool(reswf), restp);
  }
  else {
    return mk_(Bool(false), rep_alltp);
  }
#endif //VDMPP
}

// CheckRecordFieldSelect
// i : TYPE`Ind
// rectp : REP`TypeRep
// field : (AS`Name|AS`FctTypeInstExpr)
// ==> bool * [REP`TypeRep]
Tuple StatSem::CheckRecordFieldSelect (const Int & i, const TYPE_REP_TypeRep & rectp, const Record & field)
{
  bool allwf (true);
  bool onewf (false);
  Generic restp;
  restp = Nil();

  TYPE_REP_TypeRep ctp (mk_REP_CompositeTypeRep(ASTAUX::MkNameFromVoid (), SEQ<TYPE_REP_FieldRep>()));

  if (IsCompatible (i, rectp, ctp)) {
#ifdef VDMPP
    if (field.Is (TAG_TYPE_AS_FctTypeInstExpr)) {
      //----------------------------------------------------------------
      // Error message #367
      // Cannot apply polymorphic function L"%1" to record field selector
      //---------------------------------------------------------------
      GenErr (field.GetField(pos_AS_FctTypeInstExpr_polyfct), ERR, 367,
              mk_sequence(PrintName (field.GetField(pos_AS_FctTypeInstExpr_polyfct))));
      return mk_(Bool (false), rep_alltp);
    }
#endif // VDMPP
    Generic rtp_ (ExtractCompositeType (rectp));

    if (rtp_.IsNil ()) {
      return mk_(Bool (false), rep_alltp);
    }

    TYPE_REP_TypeRep rtp (rtp_);
    switch(rtp.GetTag()) {
      case TAG_TYPE_REP_CompositeTypeRep: {
        Tuple infer (IsAValidField (field, rtp));
        const Bool & wf (infer.GetBool(1));
        const TYPE_REP_TypeRep & tp (infer.GetRecord(2));
        const Generic & nvf (infer.GetField(3));
        const Generic & tag (infer.GetField(4));
  
        if (wf) {
          return mk_(wf, tp);
        }
        else {
          //-----------------------------
          // Error message #278
          // An invalid field is selected
          //-----------------------------
          GenErr (field, ERR, 278, Sequence());

          if (!nvf.IsNil() && !tag.IsNil()) {
            //--------------------------------------------------------------
            // Error message #279
            // The field L"%1" is not defined for the record tagged with "%2"
            //--------------------------------------------------------------
            GenErr(nvf, WRN1, 279, mk_sequence(PrintName(nvf), PrintName(tag)));
          }
          return mk_(wf, rep_alltp);
        }
      }
      case TAG_TYPE_REP_UnionTypeRep: {
        Set nvfields;
        SET<TYPE_REP_TypeRep> utps (rtp.GetSet(pos_REP_UnionTypeRep_tps));
        Generic utp;
        for (bool bb = utps.First(utp); bb ; bb = utps.Next(utp)) {
          bool wf = false;
          TYPE_REP_TypeRep tp (rep_unittp);
          Generic nvf, tag;
          if (utp.Is(TAG_TYPE_REP_CompositeTypeRep)) {
            Tuple infer (IsAValidField(field, utp));
            wf = infer.GetBoolValue(1);
            tp = infer.GetRecord(2);
            nvf = infer.GetField(3);
            tag = infer.GetField(4);
          }
  
          allwf = allwf && wf;
          onewf = onewf || wf;
          if (restp.IsNil() && wf) {
            restp = tp;
          }
          else if (wf) {
            restp = MergeTypes (restp, tp);
          }
          if (!wf && !nvf.IsNil() && !tag.IsNil()) {
            nvfields.Insert (mk_(nvf, tag));
          }
        }
  
        if (restp.IsNil ()) {
          restp = rep_alltp;
        } 
        bool reswf;
        if (i == DEF) {
          reswf = allwf;
        }
        else {
          reswf = onewf;
        } 
        if (!reswf) {
          //-----------------------------
          // Error message #278
          // An invalid field is selected
          //-----------------------------
          GenErr (field, ERR, 278, Sequence());

          Generic nvf_tag_tup;
          for (bool cc = nvfields.First(nvf_tag_tup); cc; cc = nvfields.Next(nvf_tag_tup)) {
            TYPE_AS_Name nvf (((Tuple) (nvf_tag_tup)).GetRecord (1));
            TYPE_AS_Name tag (((Tuple) (nvf_tag_tup)).GetRecord (2));
            //--------------------------------------------------------------
            // Error message #279
            // The field L"%1" is not defined for the record tagged with "%2"
            //--------------------------------------------------------------
            GenErr(nvf, WRN1, 279, mk_sequence(PrintName(nvf), PrintName(tag)));
          }
        }
        return mk_(Bool(reswf), restp);
      }
      default: {
        InternalError(L"CheckFieldSelect");
        return mk_(Bool(false), rep_alltp);
      }
    } // end of switch
  }
  else {
    return mk_(Bool(false), Nil());
  }
}

#ifdef VDMPP
// CheckObjRefFieldSelect
// i : TYPE`Ind
// rectp : REP`TypeRep
// field :(AS`Name|AS`FctTypeInstExpr)
// writable : bool
// ==> bool * [REP`TypeRep | set of REP`TypeRep]
Tuple StatSem::CheckObjRefFieldSelect (const Int & i,
                                            const TYPE_REP_TypeRep & objtp,
                                            const Record & field,
                                            bool writable)
{
  bool allwf = true;
  bool onewf = false;
  Generic restp; // [REP`TypeRep | set of REP`TypeRep]
  restp = Nil();

  TYPE_REP_TypeRep otp (mk_REP_ObjRefTypeRep(ASTAUX::MkNameFromVoid ()));

  TYPE_AS_Name field_q;
  if (field.Is (TAG_TYPE_AS_FctTypeInstExpr)) {
    field_q = field.GetRecord(pos_AS_FctTypeInstExpr_polyfct);
  }
  else {
    field_q = TYPE_AS_Name(field);
  }
  if (IsCompatible (i, objtp, otp)) {
    Generic rtpg (ExtractObjRefType (objtp));

    if (rtpg.IsNil ()) {
      return mk_(Bool (false), rep_alltp);
    }
    TYPE_REP_TypeRep rtp (rtpg);
    switch(rtp.GetTag()) {
      case TAG_TYPE_REP_ObjRefTypeRep: {
        const TYPE_AS_Name & cls (rtp.GetRecord(pos_REP_ObjRefTypeRep_nm));
        Generic tp_q (LookUpInObject(cls, field_q, writable, true));
        if (!tp_q.IsNil()) {
          Generic tp (CheckAccess (cls, tp_q));
          if (tp.IsNil ()) {
            // ------------------
            // Error message #368
            // Access violation
            // ------------------
            GenErr (field_q, ERR, 368, mk_sequence(PrintName(ExtName(cls, field_q))));
            return mk_(Bool(false), rep_alltp);
          }
          if (field.Is (TAG_TYPE_AS_FctTypeInstExpr)) {
            Generic poly;
            if (tp.IsRecord()) {
              if (tp.Is (TAG_TYPE_REP_PolyTypeRep)) {
                TYPE_REP_PolyTypeRep ptr (tp);
                poly = mk_(ptr.get_vars (), ptr.get_tp ());
              }
              else {
                poly = Nil ();
              }
            }
            else if (tp.IsSet ()) {
              SET<TYPE_REP_TypeRep> tps (tp);
              SET<TYPE_REP_PolyTypeRep> poly_set;
              SEQ<TYPE_AS_Type> inst (field.GetSequence(pos_AS_FctTypeInstExpr_inst));
              Generic pp;
              for (bool bb = tps.First(pp); bb ; bb = tps.Next(pp)) {
                if (pp.Is (TAG_TYPE_REP_PolyTypeRep)) {
                  if (TYPE_REP_PolyTypeRep(pp).get_vars().Length() == inst.Length()) {
                    poly_set.Insert(pp);
                    break;
                  }
                }
              }
              if (poly_set.IsEmpty()) {
                poly = Nil ();
              }
              else {
                TYPE_REP_PolyTypeRep pp (poly_set.GetElem());
                poly = mk_(pp.get_vars (), pp.get_tp ());
              }
            }
            else {
              poly = Nil ();
            }
            if (poly.IsTuple()) {
              GetCI().SetTypeInfo (ASTAUX::GetCid(field_q), Tuple(poly).GetField(2));
            }
            return CheckFctTypeInst (i, poly, field);
          }
          else {
            return mk_(Bool(true), tp);
          }
        }
        else {
          //--------------------------------------------
          // Error message #280
          // The field L"%1" is not defined in class "%2"
          //--------------------------------------------
          GenErr (field_q, ERR, 280, mk_sequence(PrintName(field_q), PrintName(cls)));
          return mk_(Bool(false), rep_alltp);
        }
      }
      case TAG_TYPE_REP_UnionTypeRep: {
        Set nvfields; // set of (AS`Name * AS`Name)
        SET<TYPE_REP_TypeRep> insttp;  // set of REP`TypeRep
        bool all_acc_wf = true;
        bool one_acc_wf = false;
        SET<TYPE_REP_TypeRep> utps (rtp.GetSet(pos_REP_UnionTypeRep_tps));
        Generic utp;
        for (bool bb = utps.First(utp); bb ; bb = utps.Next(utp)) {
          TYPE_AS_Name cls (TYPE_REP_ObjRefTypeRep(utp).get_nm());
  
          Generic tp_q (LookUpInObject(cls, field_q, false, true));
          if (!tp_q.IsNil()) {
            TYPE_AS_Name cls_q (cls); // Copy class name
            cls_q.set_cid (field_q.get_cid ()); // Position info from field_q
            Generic tp (CheckAccess (cls_q, tp_q));
            if (tp.IsNil ()) {
              allwf = false;
              all_acc_wf = false;
            }
            else {
              one_acc_wf = true;
              if (field.Is (TAG_TYPE_AS_FctTypeInstExpr)) {
                if (tp.Is (TAG_TYPE_REP_PolyTypeRep)) {
                  TYPE_REP_PolyTypeRep ptr (tp);
                  if (ptr.get_vars().Length () == field.GetSequence(pos_AS_FctTypeInstExpr_inst).Length ()) {
                    onewf = true;
                    Tuple tp_t (mk_(ptr.get_vars(), ptr.get_tp()));
                    TYPE_REP_TypeRep tp_q (CheckFctTypeInstAux (tp_t, field));
                    restp = (restp.IsNil () ? tp_q : MergeTypes (restp, tp_q));
                  }
                  else { // len tp.vars <> len field.inst
                    insttp.Insert (tp);
                    allwf = false;
                  }
                }
                else if (tp.IsSet()) { // should really be is_(tp,set of REP`PolyTypeRep)
                  SET<TYPE_REP_TypeRep> tp_set (tp);
                  SET<TYPE_REP_PolyTypeRep> poly_set;
                  const SEQ<TYPE_AS_Type> & inst (field.GetSequence(pos_AS_FctTypeInstExpr_inst));
                  Generic pp;
                  for (bool bb = tp_set.First(pp); bb ; bb = tp_set.Next(pp)) {
                    if (pp.Is(TAG_TYPE_REP_PolyTypeRep)) {
                      if (TYPE_REP_PolyTypeRep(pp).get_vars().Length() == inst.Length()) {
                        poly_set.Insert(pp);
                        break;
                      }
                    }
                  }
  
                  if (poly_set.IsEmpty()) {
                    insttp.ImpUnion (tp_set);
                    allwf = false;
                  }
                  else {
                    TYPE_REP_PolyTypeRep pp (poly_set.GetElem());
                    onewf = true;
                    Tuple tp_t (mk_(pp.get_vars (), pp.get_tp ()));
                    TYPE_REP_TypeRep tp_q (CheckFctTypeInstAux (tp_t, field));
                    restp = (restp.IsNil () ? tp_q : MergeTypes (restp, tp_q));
                  }
                }
                else { // not PolyTypeRep
                  allwf = false;
                }
              }
              else { // not FctTypeInstExpr
                onewf = true;
                if (restp.IsNil()) {
                  restp = tp;
                }
                else {
                  restp = MergeTypes(restp, tp);
                }
              }
            }
          }
          else {
            allwf = false;
            nvfields.Insert (mk_(field_q, cls));
            // nvfields not implemented ???
          }
        }
  
        bool reswf;
        if (i == DEF) {
          reswf = allwf;
        }
        else {
          reswf = onewf;
        } 
        if (!reswf) {
          if (!nvfields.IsEmpty ()) {
            //-----------------------------
            // Error message #278
            // An invalid field is selected
            //-----------------------------
            GenErr (field_q, ERR, 278, Sequence());
            Generic nvf_tup;
            for (bool cc = nvfields.First(nvf_tup); cc; cc = nvfields.Next(nvf_tup)) {
              Tuple t (nvf_tup);
              const TYPE_AS_Name & nvf (t.GetRecord (1));
              const TYPE_AS_Name & cls (t.GetRecord (2));
              //--------------------------------------------
              // Error message #280
              // The field L"%1" is not defined in class "%2"
              //--------------------------------------------
              GenErr(nvf, WRN1, 280, mk_sequence(PrintName(nvf), PrintName(cls)));
            }
          }
          if (!insttp.IsEmpty ()) {
            Generic tp_g;
            for (bool dd = insttp.First(tp_g); dd; dd = insttp.Next (tp_g)) {
              TYPE_REP_PolyTypeRep t (tp_g);
              CheckFctTypeInst (i, mk_(t.get_vars (), t.get_tp ()), field);
            }
          }
          else {
            if (field.Is (TAG_TYPE_AS_FctTypeInstExpr)) {
              CheckFctTypeInst (i, Nil(), field);
            }
          }
          if (nvfields.IsEmpty() && ((i == DEF && !all_acc_wf) || (i == POS && !one_acc_wf))) {
            // ------------------
            // Error message #368
            // Access violation
            // ------------------
            GenErr(field_q, ERR, 368, mk_sequence(PrintName (field_q)));
          }
        }
  
        if (field.Is (TAG_TYPE_AS_FctTypeInstExpr)) {
          reswf = wf_TypeList (i, field.GetField(pos_AS_FctTypeInstExpr_inst)) && reswf;
        }
        return mk_(Bool(reswf), restp);
      }
      default: {
        InternalError(L"CheckFieldSelect");
        return mk_(Bool(false), rep_alltp);
      }
    } // end of switch
  }
  else {
    return mk_(Bool(false), Nil());
  }
}
#endif //VDMPP

// wf_IsExpr
// i : TYPE`Ind
// isexpr : (AS`IsExpr|AS`TypeJudgementExpr)
// : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_IsExpr (const Int & i, const Record & isexpr, const TYPE_REP_TypeRep &)
{
  Record tp;
  TYPE_AS_Expr arg;
  TYPE_CI_ContextId cid;

  if (isexpr.Is(TAG_TYPE_AS_IsExpr)) {
    tp = isexpr.GetRecord(pos_AS_IsExpr_type); // BasicType | Name
    arg = isexpr.GetRecord(pos_AS_IsExpr_arg); // Expr
    cid = isexpr.GetInt(pos_AS_IsExpr_cid);
  }
  else { // TypeJudgement
    tp = isexpr.GetRecord(pos_AS_TypeJudgementExpr_type);   // Type
    arg = isexpr.GetRecord(pos_AS_TypeJudgementExpr_expr);  // Expr
    cid = isexpr.GetInt(pos_AS_TypeJudgementExpr_cid);
  }

  TYPE_REP_TypeRep restp (btp_bool);
  GetCI().SetTypeInfo (cid, restp);

  if (tp.Is (TAG_TYPE_AS_BooleanType) || tp.Is (TAG_TYPE_AS_NumericType) ||
      tp.Is (TAG_TYPE_AS_TokenType) || tp.Is (TAG_TYPE_AS_CharType) ||
      isexpr.Is (TAG_TYPE_AS_TypeJudgementExpr)) {
    TYPE_REP_TypeRep rep_tp (TransType(Nil(), tp));

    Tuple infer (wf_Expr(i, arg, rep_alltp));
    const Bool & wf_arg (infer.GetBool(1));
    const TYPE_REP_TypeRep & argtp (infer.GetRecord(2));

#ifdef VDMSL
    Bool iscomp (IsCompatible(POS, argtp, CurMod(ADD, GetCurMod(), rep_tp)));
#endif // VDMSL
#ifdef VDMPP
    Bool iscomp (IsCompatible(POS, argtp, rep_tp));
#endif // VDMPP
    if (!(iscomp.GetValue())) {
      //-----------------------------
      // Error message #283
      // Is-Expr will always be false
      //-----------------------------
      //GenErrTp (isexpr, ERR, 283, argtp, rep_tp, Sequence());
      GenErrTp (arg, ERR, 283, argtp, rep_tp, Sequence());
    }
    return mk_(wf_arg && iscomp, restp);
  }
  else {
    Generic pair_g (LookUpTag (i, tp, Set()));

    if (pair_g.IsNil ()) {
      //--------------------------------------------
      // Error message #254
      // Composite type L"%1" is not defined in "%2".
      //--------------------------------------------
      GenErr (isexpr, ERR, 254, mk_sequence(PrintName(tp), SEQ<Char>(L"Is-Expr")));
      return mk_(Bool (false), restp);
    }
    else {
      Tuple pair (pair_g);
      const TYPE_AS_Name & newtag (pair.GetRecord(1));
#ifdef VDMSL
      const SEQ<TYPE_REP_FieldRep> & flds (pair.GetSequence(2));
#endif // VDMSL
#ifdef VDMPP
      const Generic & flds_q (pair.GetField(2));
      Generic flds (CheckAccessCurClass (flds_q));
      if (flds.IsNil ()) {
        //-------------------
        // Error message #368
        // Access violation
        //-------------------
        GenErr(newtag, ERR, 368, mk_sequence(PrintName (newtag)));
        return mk_(Bool (false), restp);
      }
#endif // VDMPP
      TYPE_REP_TypeRep rep_tp (mk_REP_CompositeTypeRep(newtag, flds));

      Tuple infer (wf_Expr(i, arg, rep_alltp));
      const Bool & wf_arg(infer.GetBool(1));
      const TYPE_REP_TypeRep & argtp (infer.GetRecord(2));

#ifdef VDMSL
      Bool iscomp (IsCompatible (POS, argtp, CurMod(ADD,GetCurMod(), rep_tp)));
#endif // VDMSL
#ifdef VDMPP
      Bool iscomp (IsCompatible (POS, argtp, rep_tp));
#endif // VDMPP
      if (!iscomp.GetValue()) {
        //-----------------------------
        // Error message #283
        // Is-Expr will always be false
        //-----------------------------
        GenErrTp (arg, ERR, 283, argtp, rep_tp, Sequence());
      }
      return mk_(wf_arg && iscomp, restp);
    }
  }
}

// wf_NarrowExpr
// i : TYPE`Ind
// narrowexpr : AS`NarrowExpr
// : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_NarrowExpr (const Int & i, const TYPE_AS_NarrowExpr & narrowexpr, const TYPE_REP_TypeRep &)
{
  const TYPE_AS_Expr & expr (narrowexpr.GetRecord(pos_AS_NarrowExpr_expr));
  const TYPE_AS_Type & tp (narrowexpr.GetRecord(pos_AS_NarrowExpr_type));
  const TYPE_CI_ContextId & cid (narrowexpr.GetInt(pos_AS_NarrowExpr_cid));

  TYPE_REP_TypeRep rep_tp (TransType(Nil(), tp));
  GetCI().SetTypeInfo (cid, rep_tp);

  Tuple infer (wf_Expr(i, expr, rep_alltp));
  const Bool & wf_arg (infer.GetBool(1));
  const TYPE_REP_TypeRep & argtp (infer.GetRecord(2));

#ifdef VDMSL
  Bool iscomp (IsCompatible(i, CurMod(ADD, GetCurMod(), rep_tp), argtp));
#endif // VDMPP
#ifdef VDMPP
  Bool iscomp (IsCompatible(i, rep_tp, argtp));
#endif // VDMPP
  if (!iscomp) {
    //-----------------------------
    // Error message #446
    // Narrow-Expr will always be fail
    //-----------------------------
    GenErrTp (narrowexpr, ERR, 446, rep_tp, argtp, Sequence());
    return mk_(Bool(false), rep_alltp);
  }
  return mk_(wf_arg && iscomp, rep_tp);
}

#ifdef VDMPP
// wf_SelfExpr
// i : TYPE`Ind
// selexp : AS`SelfExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_SelfExpr (const Int &, const TYPE_AS_SelfExpr & selexp, const TYPE_REP_TypeRep &)
{
  TYPE_REP_TypeRep rc (mk_REP_ObjRefTypeRep(GetCurClass()));
  if (GetStatic().GetValue()) {
    // --------------------------------------------
    // -- Error message #379
    // -- Can not use self expression in a static class member
    // --------------------------------------------
    GenErr(selexp, ERR, 379, Sequence());
    return mk_(Bool(false), rc);
  }
  else {
    const TYPE_CI_ContextId & cid (selexp.GetInt(pos_AS_SelfExpr_cid));
    GetCI().SetTypeInfo (cid, rc);
    return mk_(Bool(true), rc);
  }
}

#ifdef VICE
// CpuOrBusOutSideSys
// nm : AS`Name
// ==> bool
bool StatSem::CpuOrBusOutSideSys(const TYPE_AS_Name & nm)
{
  return (((nm == ASTAUX::MkName(L"CPU")) || (nm == ASTAUX::MkName(L"BUS"))) &&
           !IsSysClass(GetCurClass()));
}
#endif // VICE

// wf_ThreadIdExpr
// i : TYPE`Ind
// threadidexp : AS`ThreadIdExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_ThreadIdExpr (const Int &, const TYPE_AS_ThreadIdExpr & threadidexp, const TYPE_REP_TypeRep &)
{
  return mk_(Bool(true), btp_nat);
}

// wf_NewExpr
// i : TYPE`Ind
// newrc : AS`NewExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_NewExpr (const Int & i, const TYPE_AS_NewExpr & newrc, const TYPE_REP_TypeRep & exptp)
{
  TYPE_AS_Name nm (newrc.GetRecord(pos_AS_NewExpr_cls));
  const SEQ<TYPE_AS_Expr> & exprs (newrc.GetSequence(pos_AS_NewExpr_args));
  const TYPE_CI_ContextId & cid (newrc.GetInt(pos_AS_NewExpr_cid));

  Tuple t (ExpandClassName(nm, Set()));
  if (t.GetBoolValue(1)) {
    nm = t.GetRecord(2);
  }
  TYPE_REP_ObjRefTypeRep tprep (mk_REP_ObjRefTypeRep(nm));

  SET<TYPE_AS_Name> s;
  s.Insert(nm);

  if (CheckClassName (nm)) {
#ifdef VICE
    if (CpuOrBusOutSideSys(nm)) {
      if (nm == ASTAUX::MkName(L"CPU")) {
        //--------------------------------------------
        // -- Error message #408
        //-- Instances of CPU can only be made in a system class
        //--------------------------------------------
        GenErr(nm, ERR, 408, Sequence());
        return mk_(Bool(false), tprep);
      }
      else {
        //--------------------------------------------
        //-- Error message #409
        //-- Instances of BUS can only be made in a system class
        //--------------------------------------------
        GenErr(nm, ERR, 409, Sequence());
        return mk_(Bool(false), tprep);
      }
    }
    else if (IsSysClass(nm)) {
      //--------------------------------------------
      //-- Error message #410
      //-- Instances of system classes cannot be made: "%1"
      //--------------------------------------------
      GenErr(nm, ERR, 410, mk_sequence(PrintName(nm)));
      return mk_(Bool(false), tprep);
    }
    else
#endif // VICE

    if (!CheckSubRespIsImplemented(i, nm)) {
      //--------------------------------------------
      //-- Error message #395
      //-- Abstract class "%1" can not be instantiated
      //--------------------------------------------
      GenErr(nm, ERR, 395, mk_sequence(PrintName(nm)));
      return mk_(Bool(false), tprep);
    }

    if (exprs.IsEmpty()) {
      // default constructor
      if (!DefaultConstructorAccessible(nm)) {
        // --------------------------------------------
        // -- Error message #389
        // -- Default constructor not in scope
        // --------------------------------------------
        GenErr(nm, ERR, 389, mk_sequence(PrintName(nm)));
        return mk_(Bool(false), tprep);
      }
      else {
        TYPE_REP_OpTypeRep oprt (mk_REP_OpTypeRep(Sequence(), mk_REP_TypeNameRep(nm)));

        GetCI().SetTypeInfo (nm.get_cid(), oprt);
        GetCI().SetTypeInfo (cid, tprep);
         
        Bool iscomp (IsCompatible(POS, tprep, exptp));
        if ( !iscomp ) {
          // --------------------------------------------
          // -- Error message #397
          // -- Value is not type-compatible
          // --------------------------------------------
          //GenErr(nm, ERR, 397, Sequence());
          GenErrTp(nm, ERR, 397, tprep, exptp, Sequence());
        }
        return mk_(iscomp, tprep);
      }
    }
    else {
      // constructor has argument(s)
      bool l_wf_arg = true;
      SEQ<TYPE_REP_TypeRep> l_argtpL;
      size_t len_exprs = exprs.Length();
      for (size_t idx = 1; idx <= len_exprs; idx++) {
        Tuple t (wf_Expr(i, exprs[idx], rep_alltp));
        l_wf_arg = t.GetBoolValue(1) && l_wf_arg;
        l_argtpL.ImpAppend(t.GetRecord(2));
      }

      SetDefClass(nm);

      if (!ConstructorExists(i, nm, l_argtpL, nm, false)) {
        // -----------------------------------------------------------------------------------
        // -- Error message #388
        // -- No unique matching constructor could be found in this class or its super classes
        // -----------------------------------------------------------------------------------
        GenErr(nm, ERR, 388, Sequence());
        l_wf_arg = false;
      }
      else {
        if (!ConstructorExists(i, nm, l_argtpL, nm, true)) {
          // ------------------------------------------------------------
          // -- Error message #440
          // -- Matching constructor is not defined locally in class "%1"
          // ------------------------------------------------------------
          GenErr(nm, WRN1, 440, mk_sequence(PrintName(nm)));
        }

        TYPE_REP_OpTypeRep oprt (mk_REP_OpTypeRep(l_argtpL, mk_REP_TypeNameRep(nm)));
        GetCI().SetTypeInfo (nm.get_cid(), oprt);
        GetCI().SetTypeInfo (cid, tprep);
      }
      UnSetDefClass();
      return mk_(Bool(l_wf_arg), tprep);
    }
  }
  else {
    //------------------------------
    // Error message #284
    // L"%1" is not an existing class
    //------------------------------
    GenErr (nm, ERR, 284, mk_sequence(PrintName (nm)));
    return mk_(Bool(false), tprep);
  }
}

// wf_SameClassExpr
// i : TYPE`Ind
// sameexp : (AS`SameBaseClassExpr|AS`SameClassExpr)
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_SameClassExpr (const Int & i, const Record & sameexp, const TYPE_REP_TypeRep &)
{
  TYPE_REP_ObjRefTypeRep objtp;
  objtp.set_nm(ASTAUX::MkNameFromVoid());

  const TYPE_AS_Expr & exp1 (sameexp.Is(TAG_TYPE_AS_SameClassExpr)
                               ? sameexp.GetRecord (pos_AS_SameClassExpr_expr1)
                               : sameexp.GetRecord (pos_AS_SameBaseClassExpr_expr1));
  const TYPE_AS_Expr & exp2 (sameexp.Is(TAG_TYPE_AS_SameClassExpr) 
                               ? sameexp.GetRecord (pos_AS_SameClassExpr_expr2)
                               : sameexp.GetRecord (pos_AS_SameBaseClassExpr_expr2));
  const TYPE_CI_ContextId & cid (sameexp.Is(TAG_TYPE_AS_SameClassExpr) 
                                   ? sameexp.GetInt (pos_AS_SameClassExpr_cid)
                                   : sameexp.GetInt (pos_AS_SameBaseClassExpr_cid));
  Tuple infer (wf_Expr (i, exp1, objtp));
  const Bool & wf_exp1 (infer.GetBool (1));
  const TYPE_REP_TypeRep & tp_exp1 (infer.GetRecord (2));

  Tuple infer2 (wf_Expr (i, exp2, objtp));
  const Bool & wf_exp2 (infer2.GetBool (1));
  const TYPE_REP_TypeRep & tp_exp2 (infer2.GetRecord (2));

  bool iscomp1 = IsCompatible (i, tp_exp1,objtp);
  if (!iscomp1) {
    //-------------------------------------------
    // Error message #285
    // Expression must return an object reference
    //-------------------------------------------
    GenErrTp (exp1, ERR, 285, tp_exp1, objtp, Sequence());
  }

  bool iscomp2 = IsCompatible (i, tp_exp2, objtp);
  if (!iscomp2) {
    //-------------------------------------------
    // Error message #285
    // Expression must return an object reference
    //-------------------------------------------
    GenErrTp (exp2, ERR, 285, tp_exp2, objtp, Sequence());
  }

  GetCI().SetTypeInfo (cid, btp_bool);
  return mk_(Bool(wf_exp1 && wf_exp2 && iscomp1 && iscomp2), btp_bool);
}

// wf_IsOfClassExpr
// i : TYPE`Ind
// isofexp : (AS`IsOfClassExpr|AS`IsOfBaseClassExpr)
//  : REP`TypeRep
// return : bool * REP`TypeRep
Tuple StatSem::wf_IsOfClassExpr (const Int & i, const Record & isofexp, const TYPE_REP_TypeRep &)
{
  const TYPE_AS_Name & cls (isofexp.Is(TAG_TYPE_AS_IsOfClassExpr)
                              ? isofexp.GetRecord (pos_AS_IsOfClassExpr_cls)
                              : isofexp.GetRecord (pos_AS_IsOfBaseClassExpr_cls));
  const TYPE_AS_Expr & exp (isofexp.Is(TAG_TYPE_AS_IsOfClassExpr)
                              ? isofexp.GetRecord (pos_AS_IsOfClassExpr_arg)
                              : isofexp.GetRecord (pos_AS_IsOfBaseClassExpr_arg));
  const TYPE_CI_ContextId & cid (isofexp.Is(TAG_TYPE_AS_IsOfClassExpr)
                                   ? isofexp.GetInt (pos_AS_IsOfClassExpr_cid)
                                   : isofexp.GetInt (pos_AS_IsOfBaseClassExpr_cid));

  TYPE_REP_ObjRefTypeRep objtp;
  objtp.set_nm(cls);

  Tuple infer (wf_Expr (i, exp, objtp));
  const Bool & wf_e (infer.GetBool (1));
  const TYPE_REP_TypeRep & tp_e (infer.GetRecord (2));

  bool wf_class = CheckClassName (cls);

  bool iscomp = IsCompatible (POS, tp_e, objtp);
  if (!iscomp) {
    //-----------------------------------------------------------------
    // Error message #286
    // Expression is not compatible with object reference of class L"%1"
    //-----------------------------------------------------------------
    GenErrTp (isofexp, ERR, 286, tp_e, objtp, mk_sequence(PrintName(cls)));
  }

  if (!wf_class) {
    //------------------------
    // Error message #287
    // Class L"%1" is undefined
    //------------------------
    GenErr (cls, ERR, 287, mk_sequence(PrintName(cls)));
  }

  GetCI().SetTypeInfo (cid, btp_bool);
  return mk_(Bool(wf_e && wf_class && iscomp), btp_bool);
}

// wf_NameList
// name_l : seq1 of AS`Name
// ==> bool * REP`TypeRep
Tuple StatSem::wf_NameList (const SEQ<TYPE_AS_Name> & name_l)
{
  bool reswf = true;
  SET<TYPE_AS_Name> prev_nm;
  size_t len_name_l = name_l.Length();
  for (size_t idx = 1; idx <= len_name_l; idx++) {
    const TYPE_AS_Name & name (name_l[idx]);
    if (name == GetCurClass()) {
      //------------------------------ 
      // Error message #431
      // History counters cannot be used for constructors like "%1"
      //------------------------------
      GenErr (name, ERR, 431, mk_sequence(PrintName (name)));
      reswf = false;
    }
    else {
      Set mthtp (LookUpOperationName (Nil(), name, Nil())); // set of ENV`AccessType
      if (mthtp.IsEmpty ()) {
        //------------------------------
        // Error message #28
        // Operation L"%1" is not defined
        //------------------------------
        GenErr (name, ERR, 28, mk_sequence(PrintName (name)));
        reswf = false;
      }
      else {
        bool forall = true;
        Generic tp; // REP`TypeRep|ENV`AccessType
        for (bool cc = mthtp.First(tp); cc && forall; cc = mthtp.Next(tp)) {
          forall = tp.Is(TAG_TYPE_SSENV_AccessOpTypeRep) ||
                   tp.Is(TAG_TYPE_SSENV_AccessFnTypeRep);
        }
        if (forall && (mthtp.Card() > 1)) {
          //------------------------------
          // Warning message #423
          // Overloaded operation "%1" used in permission guard
          //------------------------------
          GenErr (name, WRN1, 423, mk_sequence(PrintName (name)));
        }

        bool forallFn = true;
        for (bool cc = mthtp.First(tp); cc && forallFn; cc = mthtp.Next(tp)) {
          forallFn = tp.Is(TAG_TYPE_SSENV_AccessFnTypeRep);
        }
        if (forallFn) {
          //------------------------------
          // Warning message #429
          // History counters cannot be used for functions like "%1"
          //------------------------------
          GenErr (name, ERR, 429, mk_sequence(PrintName (name)));
          reswf = false;
        }
        for (bool cc = mthtp.First(tp); cc; cc = mthtp.Next(tp)) {
          if (tp.Is(TAG_TYPE_SSENV_AccessOpTypeRep)) {
            TYPE_SSENV_AccessOpTypeRep aor (tp);
            if (aor.GetBoolValue(pos_SSENV_AccessOpTypeRep_oppure)) {
              //------------------------------
              // Warning message #457
              // History counters cannot be used for pure operation like "%1"
              //------------------------------
              GenErr (name, ERR, 457, mk_sequence(PrintName (name)));
              reswf = false;
            }
          }
        }
      }

      if (prev_nm.InSet (name)) {
        //------------------------------
        // Warning message #73
        // L"%1" multiple defined
        //------------------------------
        GenErr (name, WRN1, 73, mk_sequence(PrintName (name)));
      }
      else {
        prev_nm.Insert (name);
      }
    }
  }
  return mk_(Bool(reswf), btp_nat);
}

// wf_ActExpr
// i : TYPE`Ind
// nnm : AS`ActExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_ActExpr (const Int &, const TYPE_AS_ActExpr & nnm, const TYPE_REP_TypeRep&)
{
  if (GetContext () == PERM) {
    const SEQ<TYPE_AS_Name> & name_l (nnm.GetSequence(pos_AS_ActExpr_mthd));
    const TYPE_CI_ContextId & cid (nnm.GetInt(pos_AS_ActExpr_cid));

    GetCI().SetTypeInfo (cid, btp_nat);
    return wf_NameList (name_l);
  }
  else {
    //-----------------------------------------------
    // Error message #288
    // Act-Expr only allowed in permission predicates
    //-----------------------------------------------
    GenErr (nnm, ERR, 288, Sequence());
    return mk_(Bool(false), btp_nat);
  }
}

// wf_FinExpr
// i : TYPE`Ind
// nnm :  AS`FinExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_FinExpr (const Int &, const TYPE_AS_FinExpr & nnm, const TYPE_REP_TypeRep &)
{
  if (GetContext () == PERM) {
    const SEQ<TYPE_AS_Name> & name_l (nnm.GetSequence(pos_AS_FinExpr_mthd));
    const TYPE_CI_ContextId & cid (nnm.GetInt(pos_AS_FinExpr_cid));

    GetCI().SetTypeInfo (cid, btp_nat);
    return wf_NameList (name_l);
  }
  else {
    //-----------------------------------------------
    // Error message #289
    // Fin-Expr only allowed in permission predicates
    //-----------------------------------------------
    GenErr (nnm, ERR, 289, Sequence());
    return mk_(Bool(false), btp_nat);
  }
}

// wf_ActiveExpr
// i : TYPE`Ind
// nnm : AS`ActiveExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_ActiveExpr (const Int &, const TYPE_AS_ActiveExpr & nnm, const TYPE_REP_TypeRep &)
{
  if (GetContext () == PERM) {
    const SEQ<TYPE_AS_Name> & name_l (nnm.GetSequence(pos_AS_ActiveExpr_mthd));
    const TYPE_CI_ContextId & cid    (nnm.GetInt(pos_AS_ActiveExpr_cid));

    GetCI().SetTypeInfo (cid, btp_nat);
    return wf_NameList (name_l);
  }
  else {
    //-----------------------------------------------
    // Error message #411
    // Active-Expr only allowed in permission predicates
    //-----------------------------------------------
    GenErr (nnm, ERR, 411, Sequence());
    return mk_(Bool(false), btp_nat);
  }
}

// wf_WaitingExpr
// i : TYPE`Ind
// nnm : AS`WaitingExpr
// exptp: REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_WaitingExpr (const Int &, const TYPE_AS_WaitingExpr & nnm, const TYPE_REP_TypeRep &)
{
  if (GetContext () == PERM) {
    const SEQ<TYPE_AS_Name> & name_l (nnm.GetSequence(pos_AS_WaitingExpr_mthd));
    const TYPE_CI_ContextId & cid    (nnm.GetInt(pos_AS_WaitingExpr_cid));

    GetCI().SetTypeInfo (cid, btp_nat);
    return wf_NameList (name_l);
  }
  else {
    //---------------------------------------------------
    // Error message #290
    // Waiting-Expr only allowed in permission predicates
    //---------------------------------------------------
    GenErr (nnm, ERR, 290, Sequence());
    return mk_(Bool(false), btp_nat);
  }
}

// wf_ReqExpr
// i : TYPE`Ind
// nnm : AS`ReqExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_ReqExpr (const Int &, const TYPE_AS_ReqExpr & nnm, const TYPE_REP_TypeRep &)
{
  if (GetContext () == PERM) {
    const SEQ<TYPE_AS_Name> & name_l (nnm.GetSequence(pos_AS_ReqExpr_mthd));
    const TYPE_CI_ContextId & cid    (nnm.GetInt(pos_AS_ReqExpr_cid));

    GetCI().SetTypeInfo (cid, btp_nat);
    return wf_NameList (name_l);
  }
  else {
    //-----------------------------------------------
    // Error message #291
    // Req-Expr only allowed in permission predicates
    //-----------------------------------------------
    GenErr (nnm, ERR, 291, Sequence());
    return mk_(Bool(false), btp_nat);
  }
}
#endif //VDMPP

// wf_Name
// i : TYPE`Ind
// nm : AS`Name
// ==> bool * REP`TypeRep
Tuple StatSem::wf_Name (const Int &, const TYPE_AS_Name& nm)
{
  const TYPE_CI_ContextId & cid (nm.GetInt(pos_AS_Name_cid));
  Generic restp (LookUp(nm, true));

  if (restp.IsNil()) {
    GetCI().SetTypeInfo (cid, rep_alltp);
    return mk_(Bool(false), rep_alltp);
  }
  else {
#ifdef VDMSL
    GetCI().SetTypeInfo (cid, restp);
    return mk_(Bool(true), restp);
#endif // VDMSL
#ifdef VDMPP
    Generic reptp = CheckAccessCurClass(restp);
    if (reptp.IsNil ()) {
      //-------------------
      // Error message #368
      // Access violation
      //-------------------
      GenErr (nm, ERR, 368, mk_sequence(PrintName (nm)));
      GetCI().SetTypeInfo (cid, rep_alltp);
      return mk_(Bool (false), rep_alltp);
    }
    else {
      TYPE_REP_TypeRep l_actResTp;
      if (reptp.IsSet()) {
        SET<TYPE_REP_TypeRep> reptp_s (reptp);
        if (reptp_s.Card() == 1) {
          l_actResTp = reptp_s.GetElem();
        }
        else {
          l_actResTp = mk_REP_OverTypeRep(reptp);
        }
      }
      else {
        l_actResTp = reptp;
      }
      GetCI().SetTypeInfo (cid, l_actResTp);
      return mk_(Bool(true), l_actResTp);
    }
#endif // VDMPP
  }
}

// wf_OldName
// i : TYPE`Ind
// onm : AS`OldName
// ==> bool * REP`TypeRep
Tuple StatSem::wf_OldName (const Int &, const TYPE_AS_OldName & onm)
{
  const TYPE_CI_ContextId & cid (onm.GetInt(pos_AS_OldName_cid));

  TYPE_AS_Name nm (ASTAUX::MkNameFromIds (onm.get_ids (), cid));

  Generic restp;
  if (GetContext () != POST) {
    //-----------------------------------------
    // Error message #292
    // Old names cannot be used in this context
    //-----------------------------------------
    GenErr (onm, ERR, 292, Sequence());
    restp = Nil();
  }
  else {
    restp = LookUpState (nm, Bool (true), OLD);
  }
  if (restp.IsNil()) {
    GetCI().SetTypeInfo (cid, rep_alltp);
    return mk_(Bool(false), rep_alltp);
  }
  else {
#ifdef VDMPP
    if (IsAccessType(restp)) {
      Generic restp2 = StripAccessType(restp);
      GetCI().SetTypeInfo (cid, restp2);
      return mk_(Bool(true), restp2);
    }
#endif // VDMPP
    GetCI().SetTypeInfo (cid, restp);
    return mk_(Bool(true), restp);
  }
}

// wf_Literal
// i : TYPE`Ind
// lit : AS`Literal
// exptp REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_Literal (const Int & i, const TYPE_AS_Literal & lit, const TYPE_REP_TypeRep & exptp)
{
  TYPE_REP_TypeRep restp;
  TYPE_CI_ContextId cid = NilContextId;
  switch (lit.GetTag()) {
    case TAG_TYPE_AS_BoolLit: {
      restp = btp_bool;
      cid = lit.GetInt(pos_AS_BoolLit_cid);
      break;
    }
    case TAG_TYPE_AS_RealLit: {
/*
      double val = lit.GetRealValue(pos_AS_RealLit_val);
      //double intval = floor(val);
      //if (val == intval)
      if (val == floor(val))
      { // int, nat, nat1
        if (val > 0) {
          restp = btp_natone;
        }
        else if (val >= 0) {
          restp = btp_nat;
        }
        else {
          restp = btp_int;
        }
      }
      else {
        restp = btp_real;
      }
*/
      restp = btp_real;
      cid = lit.GetInt(pos_AS_RealLit_cid);
      break;
    }
    case TAG_TYPE_AS_NumLit: {
      double val = lit.GetRealValue(pos_AS_NumLit_val);
      // int, nat, nat1
      if (val > 0) {
        restp = btp_natone;
      }
      else if (val >= 0) {
        restp = btp_nat;
      }
      else {
        restp = btp_int;
      }
      cid = lit.GetInt(pos_AS_NumLit_cid);
      break;
    }
    case TAG_TYPE_AS_CharLit: {
      restp = btp_char;
      cid = lit.GetInt(pos_AS_CharLit_cid);
      break;
    }
    case TAG_TYPE_AS_TextLit: {
      if (lit.GetSequence(pos_AS_TextLit_val).IsEmpty()) {
        restp = mk_REP_EmptySeqTypeRep(btp_char);
      }
      else {
        restp = mk_REP_SeqTypeRep(btp_char);
      }
      cid = lit.GetInt(pos_AS_TextLit_cid);
      break;
    }
    case TAG_TYPE_AS_QuoteLit: {
      restp = mk_REP_QuoteTypeRep(ASTAUX::MkNameFromId (lit.GetRecord(pos_AS_QuoteLit_val), NilContextId));
      cid = lit.GetInt(pos_AS_QuoteLit_cid);
      break;
    }
    case TAG_TYPE_AS_NilLit:  {
      restp = rep_niltp;
      cid = lit.GetInt(pos_AS_NilLit_cid);
      break;
    }
  }

  GetCI().SetTypeInfo (cid, restp);

  if (IsCompatible (i, restp, exptp)) {
    return mk_(Bool(true), restp);
  }
  else {
    //----------------------------------------
    // Error message #397
    // Value is not type-compatible
    //----------------------------------------
    GenErrTp (lit, ERR, 397, restp, exptp, Sequence());

    return mk_(Bool(false), restp);
  }
}

// wf_PrefixExpr
// i : TYPE`Ind
// unexpr : AS`PrefixExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_PrefixExpr (const Int & i, const TYPE_AS_PrefixExpr & unexpr, const TYPE_REP_TypeRep & exptp)
{
  const Int & op (unexpr.GetInt(pos_AS_PrefixExpr_opr));
  const TYPE_CI_ContextId & cid (unexpr.GetInt(pos_AS_PrefixExpr_cid));

  Tuple tpl;
  switch(op.GetValue ()) {
    case NUMPLUS:           { tpl = wf_UNARYNUMPLUS (i, unexpr, exptp); break; }
    case NUMMINUS:          { tpl = wf_UNARYNUMMINUS (i, unexpr, exptp); break; }
    case NUMABS:            { tpl = wf_NUMABS (i, unexpr, exptp); break; }
    case FLOOR:             { tpl = wf_FLOOR (i, unexpr, exptp); break; }
    case NOT:               { tpl = wf_NOT (i, unexpr, exptp); break; }
    case SETCARD:           { tpl = wf_SETCARD (i, unexpr, exptp); break; }
    case SETDISTRUNION:     { tpl = wf_SETDISTRUNION (i, unexpr, exptp); break; }
    case SETDISTRINTERSECT: { tpl = wf_SETDISTRINTERSECT (i, unexpr, exptp); break; }
    case SETPOWER:          { tpl = wf_SETPOWER (i, unexpr, exptp); break; }
    case SEQLEN:            { tpl = wf_SEQLEN (i, unexpr, exptp); break; }
    case SEQDISTRCONC:      { tpl = wf_SEQDISTRCONC (i, unexpr, exptp); break; }
    case SEQELEMS:          { tpl = wf_SEQELEMS (i, unexpr, exptp); break; }
    case SEQINDICES:        { tpl = wf_SEQINDICES (i, unexpr, exptp); break; }
    case SEQHEAD:           { tpl = wf_SEQHEAD (i, unexpr, exptp); break; }
    case SEQTAIL:           { tpl = wf_SEQTAIL (i, unexpr, exptp); break; }
    case SEQREVERSE:        { tpl = wf_SEQREVERSE (i, unexpr, exptp); break; }
    case MAPDOM:            { tpl = wf_MAPDOM (i, unexpr, exptp); break; }
    case MAPRNG:            { tpl = wf_MAPRNG (i, unexpr, exptp); break; }
    case MAPDISTRMERGE:     { tpl = wf_MAPDISTRMERGE (i, unexpr, exptp); break; }
    case MAPINVERSE:        { tpl = wf_MAPINVERSE (i, unexpr, exptp); break; }
    default: {
      InternalError (L"wf_PrefixExpr");
      tpl = mk_(Bool(false), rep_alltp);
    }
    break;
  }
  const TYPE_REP_TypeRep & petp (tpl.GetRecord(2));
  GetCI().SetTypeInfo(cid, petp);

  return tpl;
}

// wf_UNARYNUMPLUS
// i : TYPE`Ind
// unexpr : AS`PrefixExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_UNARYNUMPLUS (const Int & i, const TYPE_AS_PrefixExpr & unexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & rhs (unexpr.GetRecord(pos_AS_PrefixExpr_arg));

  TYPE_REP_TypeRep ExpectedRhsType (btp_real);
  TYPE_REP_TypeRep intertp (IntersectTypeReps(ExpectedRhsType, exptp));

  Tuple inferrhs (wf_Expr(i, rhs, intertp));
  const Bool & wf_arg (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & argtp (inferrhs.GetRecord(2));

  if (IsCompatible (i, argtp, ExpectedRhsType)) {
    return mk_(wf_arg, ExtractNumericType(argtp));
  }
  else {
    //---------------------------------------
    // Error message #122
    // Rhs of unary '+' is not a numeric type
    //---------------------------------------
    GenErrTp (unexpr, ERR, 122, argtp, ExpectedRhsType, Sequence());
    return mk_(Bool(false), ExpectedRhsType);
  }
}

// wf_UNARYNUMMINUS
// i : TYPE`Ind
// unexpr : AS`PrefixExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_UNARYNUMMINUS (const Int & i, const TYPE_AS_PrefixExpr & unexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & rhs (unexpr.GetRecord(pos_AS_PrefixExpr_arg));

  TYPE_REP_TypeRep ExpectedRhsType (btp_real);
  TYPE_REP_TypeRep intertp (IntersectTypeReps(ExpectedRhsType, exptp));

  Tuple inferrhs (wf_Expr(i, rhs, intertp));
  const Bool & wf_arg (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & argtp (inferrhs.GetRecord(2));

  if (IsCompatible (i, argtp, ExpectedRhsType)) {
    TYPE_REP_TypeRep restp (ExtractNumericType(argtp));
    if (restp.Is(TAG_TYPE_REP_NumericTypeRep) &&
        ((restp.GetIntValue(pos_REP_NumericTypeRep_qtp) == NAT) ||
         (restp.GetIntValue(pos_REP_NumericTypeRep_qtp) == NATONE))) {
      return mk_(wf_arg, btp_int);
    }
    else {
      return mk_(wf_arg, restp);
    }
  }
  else {
    //---------------------------------------
    // Error message #123
    // Rhs of unary '-' is not a numeric type
    //---------------------------------------
    GenErrTp (unexpr, ERR, 123, argtp, ExpectedRhsType, Sequence());
    return mk_(Bool(false), ExpectedRhsType);
  }
}

// wf_NUMABS
// i : TYPE`Ind
// unexpr : AS`PrefixExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_NUMABS (const Int & i, const TYPE_AS_PrefixExpr & unexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & rhs (unexpr.GetRecord(pos_AS_PrefixExpr_arg));

  TYPE_REP_TypeRep ExpectedRhsType (btp_real);
  TYPE_REP_TypeRep intertp (IntersectTypeReps(ExpectedRhsType, exptp));

  Tuple inferrhs (wf_Expr(i, rhs, intertp));
  const Bool & wf_arg (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & argtp (inferrhs.GetRecord(2));

  if (IsCompatible (i, argtp, ExpectedRhsType)) {
    TYPE_REP_TypeRep restp (ExtractNumericType(argtp));
    if (restp.Is(TAG_TYPE_REP_NumericTypeRep) && (restp.GetIntValue(pos_REP_NumericTypeRep_qtp) == INTEGER)) {
      return mk_(wf_arg, btp_nat);
    }
    else {
      return mk_(wf_arg, restp);
    }
  }
  else {
    //----------------------------------------
    // Error message #124
    // Argument of 'abs' is not a numeric type
    //----------------------------------------
    GenErrTp (unexpr, ERR, 124, argtp, ExpectedRhsType, Sequence());
    return mk_(Bool(false), ExpectedRhsType);
  }
}

// wf_FLOOR
// i : TYPE`Ind
// unexpr : AS`PrefixExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_FLOOR (const Int & i, const TYPE_AS_PrefixExpr & unexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & rhs (unexpr.GetRecord(pos_AS_PrefixExpr_arg));

  TYPE_REP_TypeRep ExpectedRhsType (btp_real);
  TYPE_REP_TypeRep intertp (IntersectTypeReps(ExpectedRhsType, exptp));

  Tuple inferrhs (wf_Expr(i, rhs, intertp));
  const Bool & wf_arg (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & argtp (inferrhs.GetRecord(2));

  if (IsCompatible (i, argtp, ExpectedRhsType)) {
    TYPE_REP_TypeRep restp (ExtractNumericType(argtp));
    if (restp.Is(TAG_TYPE_REP_NumericTypeRep) &&
        ((restp.GetIntValue(pos_REP_NumericTypeRep_qtp) == NAT) ||
         (restp.GetIntValue(pos_REP_NumericTypeRep_qtp) == NATONE))) {
      return mk_(wf_arg, restp);
    }
    else {
      return mk_(wf_arg, btp_int);
    }
  }
  else {
    //------------------------------------------
    // Error message #125
    // Argument to 'floor' is not a numeric type
    //------------------------------------------
    GenErrTp (unexpr, ERR, 125, argtp, ExpectedRhsType, Sequence());
    return mk_(Bool(false), btp_int);
  }
}

// wf_NOT
// i : TYPE`Ind
// unexpr : AS`PrefixExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_NOT (const Int & i, const TYPE_AS_PrefixExpr & unexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & rhs (unexpr.GetRecord(pos_AS_PrefixExpr_arg));

  TYPE_REP_TypeRep ExpectedRhsType (btp_bool);
  TYPE_REP_TypeRep intertp (IntersectTypeReps (ExpectedRhsType, exptp));

  Tuple inferrhs (wf_Expr(i, rhs, intertp));
  const Bool & wf_arg (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & argtp (inferrhs.GetRecord(2));

  if (IsCompatible (i, argtp, ExpectedRhsType)) {
    return mk_(wf_arg, ExpectedRhsType);
  }
  else {
    //----------------------------------------
    // Error message #126
    // Argument to 'not' is not a boolean type
    //----------------------------------------
    GenErrTp (unexpr, ERR, 126, argtp, ExpectedRhsType, Sequence());
    return mk_(Bool(false), ExpectedRhsType);
  }
}

// wf_SETCARD
// i : TYPE`Ind
// unexpr : AS`PrefixExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_SETCARD (const Int & i, const TYPE_AS_PrefixExpr & unexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & rhs (unexpr.GetRecord(pos_AS_PrefixExpr_arg));

  TYPE_REP_TypeRep ExpectedRhsType (set_alltp);

  Tuple inferrhs (wf_Expr(i, rhs, ExpectedRhsType));
  const Bool & wf_arg (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & argtp (inferrhs.GetRecord(2));

  if (IsCompatible (i, argtp, ExpectedRhsType)) {
    if (IsCompatible (DEF, argtp, RemoveEmptySetType(ExpectedRhsType))) {
      return mk_(wf_arg, btp_natone);
    }
    return mk_(wf_arg, btp_nat);
  }
  else {
    //-------------------------------------
    // Error message #127
    // Argument to 'card' is not a set type
    //-------------------------------------
    GenErrTp (unexpr, ERR, 127, argtp, ExpectedRhsType, Sequence());
    return mk_(Bool(false), btp_nat);
  }
}

// wf_SETDISTRUNION
// i : TYPE`Ind
// unexpr : AS`PrefixExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_SETDISTRUNION (const Int & i, const TYPE_AS_PrefixExpr & unexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & rhs (unexpr.GetRecord(pos_AS_PrefixExpr_arg));

  TYPE_REP_TypeRep ExpRes (set_alltp);
  TYPE_REP_TypeRep ExpectedRhsType (mk_REP_UnionTypeRep(mk_set(mk_REP_EmptySetTypeRep(rep_alltp),
                                                               mk_REP_SetTypeRep(ExpRes))));

  TYPE_REP_TypeRep rhsexptp;
  if (IsCompatible (i, exptp, ExpRes)) {
    rhsexptp = mk_REP_UnionTypeRep(mk_set(mk_REP_SetTypeRep(mk_REP_UnionTypeRep(mk_set(ExtractSetType(exptp),
                                                                                mk_REP_EmptySetTypeRep(rep_alltp)))),
                                          mk_REP_EmptySetTypeRep(rep_alltp)));
  }
  else {
    rhsexptp = ExpectedRhsType;
  }
  TYPE_REP_TypeRep intertp (IntersectTypeReps (ExpectedRhsType, rhsexptp));

  Tuple inferrhs (wf_Expr(i, rhs, intertp));
  const Bool & wf_arg (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & argtp (inferrhs.GetRecord(2));

  if (IsCompatible (i, argtp, ExpectedRhsType)) {
    Generic elmtpg (UnmaskSetType (i, ExtractSetType (argtp)));

    if (elmtpg.IsNil()) {
      //-------------------------------------------
      // Error message #128
      // The 'dunion' will always give an empty set
      //-------------------------------------------
      GenErr (unexpr, WRN1, 128, Sequence());
      return mk_(wf_arg, mk_REP_EmptySetTypeRep(rep_alltp));
    }
    else {
      TYPE_REP_TypeRep elmtp (ExtractSetType (elmtpg));
      if (elmtp.Is(TAG_TYPE_REP_UnionTypeRep)) {
        return mk_(wf_arg, SetTypeUnion (elmtp.GetSet(pos_REP_UnionTypeRep_tps)));
      }
      else {
        return mk_(wf_arg, elmtp);
      }
    }
  }
  else {
    //----------------------------------------------
    // Error message #129
    // Argument of 'dunion' is not a set of set type
    //----------------------------------------------
    GenErrTp (unexpr, ERR, 129, argtp, ExpectedRhsType, Sequence());
    return mk_(Bool(false), ExpRes);
  }
}

// wf_SETDISTRINTERSECT
// i : TYPE`Ind
// unexpr : AS`PrefixExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_SETDISTRINTERSECT (const Int & i,
                                          const TYPE_AS_PrefixExpr & unexpr,
                                          const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & rhs (unexpr.GetRecord(pos_AS_PrefixExpr_arg));

  TYPE_REP_TypeRep ExpRes (set_alltp);
  TYPE_REP_TypeRep ExpectedRhsType (mk_REP_SetTypeRep(ExpRes)); // empty not allowed

  TYPE_REP_TypeRep rhsexptp;
  if (IsCompatible (i, exptp, ExpRes)) {
    rhsexptp = mk_REP_SetTypeRep(mk_REP_UnionTypeRep(mk_set(ExtractSetType(exptp),
                                                            mk_REP_EmptySetTypeRep(rep_alltp))));
  }
  else {
    rhsexptp = ExpectedRhsType;
  }
  TYPE_REP_TypeRep intertp (IntersectTypeReps (ExpectedRhsType, rhsexptp));

  Tuple inferrhs (wf_Expr(i, rhs, intertp));
  const Bool & wf_arg (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & argtp (inferrhs.GetRecord(2));

  if (argtp.Is (TAG_TYPE_REP_EmptySetTypeRep)) {
    //----------------------------------------------
    // Error message #130
    // An empty set is given as argument to 'dinter'
    //----------------------------------------------
    GenErr (unexpr, ERR, 130, Sequence());
    return mk_(Bool (false), ExpRes);
  }
  else if (IsCompatible (i, argtp, ExpectedRhsType)) {
    Generic elmtpg (UnmaskSetType (i, ExtractSetType (argtp)));
    if (elmtpg.IsNil()) {
      //----------------------------------------------
      // Error message #130
      // An empty set is given as argument to 'dinter'
      //----------------------------------------------
      GenErr (unexpr, ERR, 130, Sequence());
      return mk_(Bool(false), ExpRes);
    }
    else {
      TYPE_REP_TypeRep elmtp (ExtractSetType (elmtpg));
      if (elmtp.Is(TAG_TYPE_REP_UnionTypeRep) && (i == DEF)) {
        TYPE_REP_TypeRep restp (SetTypeInter (elmtp.GetSet(pos_REP_UnionTypeRep_tps)));

        if (restp.Is(TAG_TYPE_REP_EmptySetTypeRep)) {
          //---------------------------------------
          // Error message #131
          // Result of 'dinter' can be an empty set
          //---------------------------------------
          GenErr (unexpr, WRN2, 131, Sequence());
        }
        return mk_(wf_arg, restp);
      }
      else {
        return mk_(wf_arg, elmtp);
      }
    }
  }
  else {
    //----------------------------------------------
    // Error message #132
    // Argument of 'dinter' is not a set of set type
    //----------------------------------------------
    GenErrTp (unexpr, ERR, 132, argtp, ExpectedRhsType, Sequence());
    return mk_(Bool(false), ExpRes);
  }
}

// wf_SETPOWER
// i : TYPE`Ind
// unexpr : AS`PrefixExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_SETPOWER (const Int & i, const TYPE_AS_PrefixExpr & unexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & rhs (unexpr.GetRecord(pos_AS_PrefixExpr_arg));

  TYPE_REP_TypeRep ExpectedRhsType (set_alltp);
  TYPE_REP_SetTypeRep ExpectedType (mk_REP_SetTypeRep(rep_alltp));

  TYPE_REP_TypeRep rhsexptp;
  if (IsCompatible (i, exptp, ExpectedType)) {
    rhsexptp = UnmaskSetType(i, ExtractSetType(exptp));
  }
  else {
    rhsexptp = ExpectedRhsType;
  }
  TYPE_REP_TypeRep intertp (IntersectTypeReps (ExpectedRhsType, rhsexptp));

  Tuple inferrhs (wf_Expr(i, rhs, intertp));
  const Bool & wf_arg (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & argtp (inferrhs.GetRecord(2));

  if (IsCompatible (i, argtp, ExpectedRhsType)) {
    TYPE_REP_TypeRep r1tp (ExtractSetType(argtp));
    TYPE_REP_TypeRep restp;
    switch(r1tp.GetTag()) {
      case TAG_TYPE_REP_EmptySetTypeRep: {
        restp = r1tp;
        break;
      }
      case TAG_TYPE_REP_UnionTypeRep: {
        restp = mk_REP_UnionTypeRep(r1tp.GetSet(pos_REP_UnionTypeRep_tps).Union(mk_set(mk_REP_EmptySetTypeRep(rep_alltp))));
        break;
      }
      case TAG_TYPE_REP_SetTypeRep: {
        restp = mk_REP_UnionTypeRep(mk_set(r1tp, mk_REP_EmptySetTypeRep(rep_alltp)));
        break;
      }
      default:
        break;
    }
    return mk_(wf_arg, mk_REP_SetTypeRep(restp));
  }
  else {
    //--------------------------------------
    // Error message #133
    // Argument to 'power' is not a set type
    //--------------------------------------
    GenErrTp (unexpr, ERR, 133, argtp, ExpectedRhsType, Sequence());
    return mk_(Bool(false), mk_REP_SetTypeRep(mk_REP_SetTypeRep(rep_alltp)));
  }
}

// wf_SEQLEN
// i : TYPE`Ind
// unexpr : AS`PrefixExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_SEQLEN (const Int & i, const TYPE_AS_PrefixExpr & unexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & rhs (unexpr.GetRecord(pos_AS_PrefixExpr_arg));

  TYPE_REP_TypeRep ExpectedRhsType (seq_alltp);

  Tuple inferrhs (wf_Expr(i, rhs, ExpectedRhsType));
  const Bool & wf_arg (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & argtp (inferrhs.GetRecord(2));

  if (IsCompatible (i, argtp, ExpectedRhsType)) {
    if (IsCompatible (DEF, argtp, RemoveEmptySeqType(ExpectedRhsType))) {
      return mk_(wf_arg, btp_natone);
    }
    return mk_(wf_arg, btp_nat);
  }
  else {
    //-----------------------------------------
    // Error message #134
    // Argument of 'len' is not a sequence type
    //-----------------------------------------
    GenErrTp (unexpr, ERR, 134, argtp, ExpectedRhsType, Sequence());
    return mk_(Bool(false), btp_nat);
  }
}

// wf_SEQDISTRCONC
// i : TYPE`Ind
// unexpr : AS`PrefixExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_SEQDISTRCONC (const Int & i, const TYPE_AS_PrefixExpr & unexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & rhs (unexpr.GetRecord(pos_AS_PrefixExpr_arg));

  TYPE_REP_TypeRep ExpectedRhsType (mk_REP_UnionTypeRep(mk_set(mk_REP_EmptySeqTypeRep(rep_alltp),
                                                               mk_REP_SeqTypeRep(seq_alltp))));

  TYPE_REP_TypeRep ExpectedType (mk_REP_UnionTypeRep(mk_set(mk_REP_EmptySeqTypeRep(rep_alltp),
                                                            mk_REP_SeqTypeRep(exptp))));
  TYPE_REP_TypeRep intertp (IntersectTypeReps (ExpectedRhsType, ExpectedType));

  Tuple inferrhs (wf_Expr(i, rhs, intertp));
  const Bool & wf_arg (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & argtp (inferrhs.GetRecord(2));

  if (IsCompatible (i, argtp, ExpectedRhsType)) {
    Generic elmtpg (UnmaskSeqType (i, ExtractSeqType (argtp)));

    if (elmtpg.IsNil()) { //GenErr (unexpr, WRN1,
      //  L"'conc' will always give an empty sequence");
      return mk_(wf_arg, mk_REP_EmptySeqTypeRep(rep_alltp));
    }
    else {
      TYPE_REP_TypeRep elmtp (ExtractSeqType (elmtpg));
      if (elmtp.Is(TAG_TYPE_REP_UnionTypeRep)) {
        TYPE_REP_TypeRep restp (SeqTypeConc (elmtp.GetSet(pos_REP_UnionTypeRep_tps)));

        if (restp.Is(TAG_TYPE_REP_EmptySeqTypeRep) && (i == DEF)) {
          //------------------------------------------
          // Error message #135
          // Result of 'conc' can be an empty sequence
          //------------------------------------------
          GenErr (unexpr, WRN2, 135, Sequence());
        }

        if(restp.Is(TAG_TYPE_REP_SeqTypeRep)) {
          restp = mk_REP_UnionTypeRep(mk_set(mk_REP_EmptySeqTypeRep(rep_alltp), restp));
        }
        return mk_(wf_arg, restp);
      }
      else {
        return mk_(wf_arg, elmtp);
      }
    }
  }
  else {
    //--------------------------------------------------
    // Error message #136
    // Argument of 'conc' is not a sequence of sequences
    //--------------------------------------------------
    GenErrTp (unexpr, ERR, 136, argtp, ExpectedRhsType, Sequence());
    return mk_(Bool(false), mk_REP_SeqTypeRep(rep_alltp));
  }
}

// wf_SEQELEMS
// i : TYPE`Ind
// unexpr : AS`PrefixExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_SEQELEMS (const Int & i, const TYPE_AS_PrefixExpr & unexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & rhs (unexpr.GetRecord(pos_AS_PrefixExpr_arg));

  TYPE_REP_TypeRep ExpectedRhsType (seq_alltp);
  TYPE_REP_TypeRep ExpectedType (set_alltp);

  TYPE_REP_TypeRep elemtp (rep_alltp);
  if (IsCompatible (i, exptp, ExpectedType)) {
    elemtp = UnmaskSetType(i, ExtractSetType(exptp));
  }
  TYPE_REP_SeqTypeRep expseqtp (mk_REP_SeqTypeRep(elemtp));

  TYPE_REP_TypeRep intertp (IntersectTypeReps (ExpectedRhsType, expseqtp));

  Tuple inferrhs (wf_Expr(i, rhs, intertp));
  const Bool & wf_arg (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & argtp (inferrhs.GetRecord(2));

  if (IsCompatible (i, argtp, ExpectedRhsType)) {
    TYPE_REP_TypeRep r1tp (ExtractSeqType(argtp));
    switch(r1tp.GetTag()) {
      case TAG_TYPE_REP_EmptySeqTypeRep: {
        return mk_(wf_arg, mk_REP_EmptySetTypeRep(rep_alltp));
      }
      case TAG_TYPE_REP_SeqTypeRep: {
        return mk_(wf_arg, mk_REP_SetTypeRep(r1tp.GetField(pos_REP_SetTypeRep_elemtp)));
      }
      case TAG_TYPE_REP_UnionTypeRep: {
        SET<TYPE_REP_TypeRep> u (r1tp.GetSet(pos_REP_UnionTypeRep_tps)), restp;
        Generic utp;
        for (bool bb = u.First(utp); bb ; bb = u.Next(utp)) {
          TYPE_REP_TypeRep rc (utp);
          switch (rc.GetTag()) {
            case TAG_TYPE_REP_EmptySeqTypeRep: {
              restp.Insert (mk_REP_EmptySetTypeRep(rep_alltp));
              break;
            }
            case TAG_TYPE_REP_SeqTypeRep: {
              restp.Insert (mk_REP_SetTypeRep(rc.GetRecord(pos_REP_SeqTypeRep_elemtp)));
              break;
            }
          }
        }
        return mk_(wf_arg, mk_REP_UnionTypeRep(restp));
      }
      default: {
        return Tuple();
      }
    }
  }
  else {
    //-------------------------------------------
    // Error message #137
    // Argument of 'elems' is not a sequence type
    //-------------------------------------------
    GenErrTp (unexpr, ERR, 137, argtp, ExpectedRhsType, Sequence());
    return mk_(Bool(false), mk_REP_SetTypeRep(rep_alltp));
  }
}

// wf_SEQINDICES
// i : TYPE`Ind
// unexpr : AS`PrefixExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_SEQINDICES (const Int & i, const TYPE_AS_PrefixExpr & unexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & rhs (unexpr.GetRecord(pos_AS_PrefixExpr_arg));

  TYPE_REP_TypeRep ExpectedRhsType (seq_alltp);

  Tuple inferrhs (wf_Expr(i, rhs, ExpectedRhsType));
  const Bool & wf_arg (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & argtp (inferrhs.GetRecord(2));

  if (IsCompatible (i, argtp, ExpectedRhsType)) {
    if (argtp.Is(TAG_TYPE_REP_EmptySeqTypeRep)) {
      return mk_(wf_arg, mk_REP_EmptySetTypeRep(btp_natone));
    }
    else {
      return mk_(wf_arg, mk_REP_SetTypeRep(btp_natone));
    }
  }
  else {
    //------------------------------------------
    // Error message #138
    // Argument of 'inds' is not a sequence type
    //------------------------------------------
    GenErrTp (unexpr, ERR, 138, argtp, ExpectedRhsType, Sequence());
    return mk_(Bool(false), mk_REP_SetTypeRep(btp_natone));
  }
}

// wf_SEQHEAD
// i : TYPE`Ind
// unexpr : AS`PrefixExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_SEQHEAD (const Int & i, const TYPE_AS_PrefixExpr & unexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & rhs (unexpr.GetRecord(pos_AS_PrefixExpr_arg));

  TYPE_REP_TypeRep ExpectedRhsType (seq_alltp);

  TYPE_REP_SeqTypeRep expseqtp (mk_REP_SeqTypeRep(exptp));

  Tuple inferrhs (wf_Expr(i, rhs, expseqtp));
  const Bool & wf_arg (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & argtp (inferrhs.GetRecord(2));

  Bool reswf (true);
  if (IsCompatible (i, argtp, ExpectedRhsType)) {
    TYPE_REP_TypeRep restp (ExtractSeqType(argtp));
    switch(restp.GetTag()) {
      case TAG_TYPE_REP_EmptySeqTypeRep: {
        //--------------------------------------
        // Error message #139
        // Argument of 'hd' is an empty sequence
        //--------------------------------------
        GenErr (unexpr, ERR, 139, Sequence());
        return mk_(Bool(false), rep_alltp);
      }
      case TAG_TYPE_REP_SeqTypeRep: {
        return mk_(wf_arg, restp.GetRecord(pos_REP_SeqTypeRep_elemtp));
      }
      case TAG_TYPE_REP_UnionTypeRep: {
        SET<TYPE_REP_TypeRep> utps (restp.GetSet(pos_REP_UnionTypeRep_tps));
        if (utps.InSet(mk_REP_EmptySeqTypeRep(rep_alltp)) &&
            (i == DEF) && (Settings.ErrorLevel() >= ERR2)) {
          //------------------------------------------
          // Error message #140
          // Argument of 'hd' can be an empty sequence
          //------------------------------------------
          GenErr (unexpr, ERR2,140, Sequence());
          reswf = Bool(false);
        }
        SET<TYPE_REP_TypeRep> r1tp;
        Generic utp;
        for (bool bb = utps.First(utp); bb ; bb = utps.Next(utp)) {
          if (utp.Is(TAG_TYPE_REP_SeqTypeRep)) {
            r1tp.Insert(Record(utp).GetRecord(pos_REP_SeqTypeRep_elemtp));
          }
        }
        SET<TYPE_REP_TypeRep> r2tp (MergeNumericTypes(r1tp));
        switch(r2tp.Card()) {
          case 0: {
            InternalError(L"wf_SEQHEAD");
            break;
          }
          case 1: {
            restp = r2tp.GetElem();
            break;
          }
          default: {
            restp = mk_REP_UnionTypeRep(r2tp);
            break;
          }
        }
        return mk_(wf_arg && reswf, restp);
      }
      default: {
        return Tuple(); //This is only to avoid warnings from the compiler
      }
    }
  }
  else {
    //----------------------------------------
    // Error message #141
    // Argument of 'hd' is not a sequence type
    //----------------------------------------
    GenErrTp (unexpr, ERR, 141, argtp, ExpectedRhsType, Sequence());
    return mk_(Bool(false), rep_alltp);
  }
}

#ifdef VICE
// wf_CurTimeExpr
// i : TYPE`Ind
// ctime : AS`CurTimeExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_CurTimeExpr (const Int &, const TYPE_AS_CurTimeExpr & ctimeexp, const TYPE_REP_TypeRep &)
{
  return mk_(Bool(true), btp_real);
}
#endif // VICE

#endif //NOSS

