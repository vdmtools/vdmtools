/***
*  * ID
*  *    $Id: pat.cc,v 1.34 2006/03/22 07:15:37 vdmtools Exp $
*  * SPECIFICATION VERSION
*  *    pat.cc: Implementation of pat.vdm 1.44
***/

#include "statsem.h"
#include "astaux.h"
#include "intconvquotes.h"
#include "settings.h"

//wcout << L"intertp: " << INT2Q::h2gAS(intertp) << endl;

#ifndef NOSS

// wf_PatternBind
// i : TYPE`Ind * AS`PatternBind * REP`TypeRep
// ==> [bool] * map AS`Name to (REP`TypeRep * nat1)
//     true  : can match
//     false : never match
//     nil   : type error
Tuple StatSem::wf_PatternBind (const Int & i, const TYPE_AS_PatternBind & patbind, const TYPE_REP_TypeRep & tp)
{
  switch(patbind.GetTag()) {
    case TAG_TYPE_AS_SetBind: {
      const TYPE_AS_Pattern & p (patbind.GetRecord(pos_AS_SetBind_pat));
      const TYPE_AS_Expr & s (patbind.GetRecord(pos_AS_SetBind_Set));
  
      Tuple infer (wf_Expr (i, s, mk_REP_SetTypeRep(rep_alltp)));
      const Bool & wf_s (infer.GetBool (1));
      const TYPE_REP_TypeRep & stp (infer.GetRecord (2));
  
      if (IsCompatible (i, stp, mk_REP_SetTypeRep(rep_alltp)))
      {
        Generic elemtp (UnmaskSetType (i, ExtractSetType (stp)));
  
        if (elemtp.IsNil())
        {
          //-----------------------------------
          // Error message #293
          // An empty set is used in a set bind
          //-----------------------------------
          GenErr (s, WRN1, 293, Sequence());
          Tuple infer2 (wf_Pattern (i, p, tp)); // [bool] * map AS`Name to (REP`TypeRep * nat1)
          const Generic & wf_p (infer2.GetField (1)); // [bool]
          const MAP<TYPE_AS_Name,Tuple> & bd (infer2.GetMap (2));
  
          if (wf_p.IsNil())
            return mk_(Nil(), bd);
          else
            return mk_(wf_s && Bool (wf_p), bd);
        }
        else if (IsCompatible (i, tp, elemtp))
        {
          Tuple infer3 (wf_Pattern (i, p, tp)); // [bool] * map AS`Name to (REP`TypeRep * nat1)
          const Generic & wf_p (infer3.GetField (1)); // [bool]
          const MAP<TYPE_AS_Name,Tuple> & bd (infer3.GetMap (2));
  
          if (wf_p.IsNil())
            return mk_(Nil(), bd);
          else
            return mk_(Bool (wf_p) && wf_s, bd);
        }
        else
        {
          //-----------------------------------------------------------
          // Error message #294
          // Type of defining expression is not compatible with binding
          //-----------------------------------------------------------
          GenErrTp (s, WRN1, 294, tp, elemtp, Sequence());
          Tuple infer4 (wf_Pattern (i, p, elemtp)); // [bool] * map AS`Name to REP`TypeRep
          const Generic & wf_p (infer4.GetField (1)); // [bool]
          const MAP<TYPE_AS_Name,Tuple> & bd (infer4.GetMap (2));
  
          if (wf_p.IsNil())
            return mk_(Nil(), bd);
          else
            return mk_(Bool(false), bd);
        }
      }
      else
      {
        //-------------------------------------
        // Error message #295
        // Type of expression is not a set type
        //-------------------------------------
        GenErrTp (s, ERR, 295, stp, mk_REP_SetTypeRep(rep_alltp), Sequence());
        return mk_(Nil(), ExtractBindings (p));
      }
    }
    case TAG_TYPE_AS_TypeBind: {
      const TYPE_AS_Pattern & p (patbind.GetRecord(pos_AS_TypeBind_pat));
      const TYPE_AS_Type & t (patbind.GetRecord(pos_AS_TypeBind_tp));
#ifdef VDMPP
      if (t.Is (TAG_TYPE_AS_TypeName))
      {
        TYPE_AS_Name tname (t.GetRecord(pos_AS_TypeName_name));
        Generic tnm (LookUpTypeName (tname, true));
        Generic defcl (LookUpDefClassForTypeName (tname));
        TYPE_AS_Name tmpname (tname);
        if (!defcl.IsNil ())
          tmpname = defcl;
        Generic ca (CheckAccess (tmpname,tnm));
        if (ca.IsNil ()) {
          //-------------------
          // Error message #368
          // Access violation
          //-------------------
          GenErr (tname, ERR, 368, mk_sequence(PrintName (tname)));
          return mk_(Nil(), ExtractBindings (p));
        }
      }
#endif // VDMPP
      TYPE_REP_TypeRep ttp (TransType(Nil (), t));
  
      if (IsCompatible (i, tp, ttp))
      {
        Tuple tpl (wf_Pattern (i, p, ttp)); // [bool] * map AS`Name to REP`TypeRep
        return tpl;
      }
      else
      {
        //-----------------------------------------------------------
        // Error message #294
        // Type of defining expression is not compatible with binding
        //-----------------------------------------------------------
        GenErrTp (patbind, WRN1, 294, tp, ttp, Sequence());
  
        Tuple infer (wf_Pattern (i, p, ttp)); // [bool] * map AS`Name to REP`TypeRep
        const Generic & wf_p (infer.GetField (1)); // [bool]
        const MAP<TYPE_AS_Name,Tuple> & bd (infer.GetMap (2));
  
        if (wf_p.IsNil())
          return mk_(Nil(), bd);
        else
          return mk_(Bool(false), bd);
      }
    }
    default: {
      return wf_Pattern (i, patbind, tp); // [bool] * map AS`Name to REP`TypeRep
    }
  } 
}

// wf_Pattern
// i : TYPE`Ind
// pat : AS`Pattern
// tp : REP`TypeRep
// ==> [bool] * map AS`Name to (REP`TypeRep * nat1)
//     true  : can match
//     false : never match
//     nil   : type error
Tuple StatSem::wf_Pattern (const Int & i, const TYPE_AS_Pattern & pat, const TYPE_REP_TypeRep & tp)
{
  switch (pat.GetTag()) {
    case TAG_TYPE_AS_PatternName:     { return wf_PatternName (i, pat, tp); }
    case TAG_TYPE_AS_MatchVal:        { return wf_MatchVal (i, pat, tp); }
    case TAG_TYPE_AS_SetEnumPattern:  { return wf_SetEnumPattern (i, pat, tp); }
    case TAG_TYPE_AS_SetUnionPattern: { return wf_SetUnionPattern (i, pat, tp); }
    case TAG_TYPE_AS_SeqEnumPattern:  { return wf_SeqEnumPattern (i, pat, tp); }
    case TAG_TYPE_AS_SeqConcPattern:  { return wf_SeqConcPattern (i, pat, tp); }
    case TAG_TYPE_AS_MapletPattern:   { return wf_MapletPattern (i, pat, tp); }
    case TAG_TYPE_AS_MapEnumPattern:  { return wf_MapEnumPattern (i, pat, tp); }
    case TAG_TYPE_AS_MapMergePattern: { return wf_MapMergePattern (i, pat, tp); }
    case TAG_TYPE_AS_RecordPattern:   { return wf_RecordPattern (i, pat, tp); }
    case TAG_TYPE_AS_TuplePattern:    { return wf_TuplePattern (i, pat, tp); }
#ifdef VDMPP
    case TAG_TYPE_AS_ObjectPattern:   { return wf_ObjectPattern (i, pat, tp); }
#endif // VDMPP
  }
  return Tuple(); //This is only to avoid warnings from the compiler
}

// wf_PatternName
// i : TYPE`Ind
// pat : AS`PatternName
// tp : REP`TypeRep
// ==> [bool] * map AS`Name to (REP`TypeRep * nat1)
Tuple StatSem::wf_PatternName (const Int & /*i*/, const TYPE_AS_PatternName & pat, const TYPE_REP_TypeRep & tp)
{
  const Generic & nm (pat.GetField(pos_AS_PatternName_nm));
  const TYPE_CI_ContextId & cid (pat.GetInt(pos_AS_PatternName_cid));

  GetCI().SetTypeInfo (cid, tp);

  if (nm.IsNil())
    return mk_(Bool(true), Map());
  else
  {
    Map bd;
    bd.Insert(nm, mk_(tp, Int(1)));
    return mk_(Bool(true), bd);
  }
}

// wf_MatchVal
// i : TYPE`Ind
// pat : AS`MatchVal
// tp : REP`TypeRep
/// ==> [bool] * map AS`Name to (REP`TypeRep * nat1)
Tuple StatSem::wf_MatchVal (const Int & i, const TYPE_AS_MatchVal & pat, const TYPE_REP_TypeRep & tp)
{
  const TYPE_AS_Expr & val (pat.GetRecord(pos_AS_MatchVal_val));
  const TYPE_CI_ContextId & cid (pat.GetInt(pos_AS_MatchVal_cid));

  Tuple infer (wf_Expr (i, val, rep_alltp));
  const Bool & wf_e (infer.GetBool (1));
  const TYPE_REP_TypeRep & tp_e (infer.GetRecord (2));

  GetCI().SetTypeInfo (cid, tp_e);

  if (IsCompatible (i, tp_e, tp))
  {
    if (wf_e)
    {
      // TODO
// 20100804 -->
      //return mk_(Bool(i == POS) || IsOneValueType (i, tp), Map());
      return mk_(Bool(i == POS) || IsOneValueType (i, tp_e), Map());
// <-- 20100804
    }
    else
      return mk_(Nil(), Map());
  }
  else
  {
    return mk_(Bool(false), Map());
  }
}

// wf_SetEnumPattern
// i : TYPE`Ind
// pat : AS`SetEnumPattern
// tp : REP`TypeRep
// ==> [bool] * map AS`Name to (REP`TypeRep * nat1)
Tuple StatSem::wf_SetEnumPattern (const Int & i, const TYPE_AS_SetEnumPattern & pat, const TYPE_REP_TypeRep & tp)
{
  const SEQ<TYPE_AS_Pattern> & els (pat.GetSequence(pos_AS_SetEnumPattern_Elems));
  const TYPE_CI_ContextId & cid (pat.GetInt(pos_AS_SetEnumPattern_cid));

  TYPE_REP_TypeRep stp (ExtractSetType (tp));

  GetCI().SetTypeInfo (cid, stp);
  Generic elemtp (UnmaskSetType (i, stp));

  if (elemtp.IsNil())
  {
    if (els.IsEmpty())
    {
      return mk_(Bool(true), MAP<TYPE_AS_Name,Tuple>());
    }
    else
    {
      MAP<TYPE_AS_Name,TYPE_REP_TypeRep> bd;
      size_t len_els = els.Length();
      for (size_t idx = 1; idx <= len_els; idx++)
        bd.ImpOverride (ExtractBindings (els[idx]));
      return mk_(Bool(false), bd);
    }
  }
  else
  {
    Set wf;
    Sequence b_s;
    size_t len_els = els.Length();
    for (size_t idx = 1; idx <= len_els; idx++)
    {
      Tuple t (wf_Pattern (i, els[idx], elemtp)); // [bool] * map AS`Name to REP`TypeRep
      wf.Insert (t.GetField (1)); // [bool]
      b_s.ImpAppend (t.GetMap (2)); // map AS`Name to REP`TypeRep
    }

    Tuple infer (MergeBindings (i, b_s)); // bool * map AS`Name to REP`TypeRep
    const Bool & wf_b (infer.GetBool (1));
    const MAP<TYPE_AS_Name,Tuple> & bind (infer.GetMap (2));

    if (wf.InSet(Nil()))
      return mk_(Nil(), bind);
    else if (wf.InSet(Bool (false)))
      return mk_(Bool(false), bind);
    else
    {
      Bool tmpb (!(i == DEF) || (stp == tp));
      return mk_(wf_b && tmpb, bind);
    }
  }
}

// wf_SetUnionPattern
// i : TYPE`Ind
// pat : AS`SetUnionPattern
// tp : REP`TypeRep
// ==> [bool] * map AS`Name to (REP`TypeRep * nat1)
Tuple StatSem::wf_SetUnionPattern (const Int & i, const TYPE_AS_SetUnionPattern & pat, const TYPE_REP_TypeRep & tp)
{
  const TYPE_AS_Pattern & lp (pat.GetRecord(pos_AS_SetUnionPattern_lp));
  const TYPE_AS_Pattern & rp (pat.GetRecord(pos_AS_SetUnionPattern_rp));
  const TYPE_CI_ContextId & cid (pat.GetInt(pos_AS_SetUnionPattern_cid));

  TYPE_REP_TypeRep tp_stp (ExtractSetType(tp));
  GetCI().SetTypeInfo(cid, tp_stp);

  if (IsCompatible (i, tp, set_alltp))
  {
    Tuple infer (wf_Pattern (i, lp, tp));  // bool * map AS`Name to REP`TypeRep
    const Generic & wf_lp (infer.GetField (1)); // [bool]
    const MAP<TYPE_AS_Name,Tuple> & bd_l (infer.GetMap (2));

    Tuple infer2 (wf_Pattern (i, rp, tp));  // bool * map AS`Name to REP`TypeRep
    const Generic & wf_rp (infer2.GetField (1)); // [bool]
    const MAP<TYPE_AS_Name,Tuple> & bd_r (infer2.GetMap (2));

    Tuple infer3 (MergeBindings (i, mk_sequence(bd_l, bd_r))); // bool * map AS`Name to REP`TypeRep
    const Bool & wf_b (infer3.GetBool (1));
    const MAP<TYPE_AS_Name,Tuple> & bind (infer3.GetMap (2));

    if (wf_lp.IsNil() || wf_rp.IsNil())
      return mk_(Nil(), bind);
    else
      return mk_(wf_b && Bool(wf_lp) && Bool(wf_rp), bind);
  }
  else
  {
    MAP<TYPE_AS_Name,Tuple> bind (ExtractBindings (lp));
    bind.ImpOverride (ExtractBindings (rp));
    return mk_(Bool(false), bind);
  }
}

// wf_SeqEnumPattern
// i : TYPE`Ind
// pat : AS`SeqEnumPattern
// tp : REP`TypeRep
// ==> [bool] * map AS`Name to (REP`TypeRep * nat1)
Tuple StatSem::wf_SeqEnumPattern (const Int & i, const TYPE_AS_SeqEnumPattern & pat, const TYPE_REP_TypeRep & tp)
{
  const SEQ<TYPE_AS_Pattern> & els (pat.GetSequence(pos_AS_SeqEnumPattern_els));
  const TYPE_CI_ContextId & cid (pat.GetInt(pos_AS_SeqEnumPattern_cid));

  TYPE_REP_TypeRep stp (ExtractSeqType (tp));
  GetCI().SetTypeInfo (cid, stp);

  Generic elemtp (UnmaskSeqType (i, stp));

  if (elemtp.IsNil())
  {
    if (els.IsEmpty())
      return mk_(Bool(true), MAP<TYPE_AS_Name,Tuple>());
    else
    {
      MAP<TYPE_AS_Name,Tuple> bd;
      size_t len_els = els.Length();
      for (size_t idx = 1; idx <= len_els; idx++)
        bd.ImpOverride (ExtractBindings (els[idx]));
      return mk_(Bool(false), bd);
    }
  }
  else
  {
    Set wf;
    Sequence b_s;
    size_t len_els = els.Length();
    for (size_t idx = 1; idx <= len_els; idx++)
    {
      Tuple t (wf_Pattern (i, els[idx], elemtp)); // [bool] * map AS`Name to REP`TypeRep
      wf.Insert (t.GetField (1)); // [bool]
      b_s.ImpAppend (t.GetMap (2)); // map AS`Name to REP`TypeRep
    }

    Tuple infer (MergeBindings (i, b_s)); // bool * map AS`Name to REP`TypeRep
    const Bool & wf_b (infer.GetBool (1));
    const MAP<TYPE_AS_Name,Tuple> & bind (infer.GetMap (2));

    if (wf.InSet(Nil()))
      return mk_(Nil(), bind);
    else if (wf.InSet(Bool (false)))
      return mk_(Bool(false), bind);
    else
    {
      Bool tmpb (!(i == DEF) || (stp == tp));
      return mk_(wf_b && tmpb, bind);
    }
  }
}

// wf_SeqConcPattern
// i : TYPE`Ind
// pat : AS`SeqConcPattern
// tp : REP`TypeRep
// ==> [bool] * map AS`Name to (REP`TypeRep * nat1)
Tuple StatSem::wf_SeqConcPattern (const Int & i, const TYPE_AS_SeqConcPattern & pat, const TYPE_REP_TypeRep & tp)
{
  const TYPE_AS_Pattern & lp (pat.GetRecord(pos_AS_SeqConcPattern_lp));
  const TYPE_AS_Pattern & rp (pat.GetRecord(pos_AS_SeqConcPattern_rp));
  const TYPE_CI_ContextId & cid (pat.GetInt(pos_AS_SeqConcPattern_cid));

  if (IsCompatible (i, tp, seq_alltp))
  {
    Tuple infer (wf_Pattern (i, lp, tp)); // bool * map AS`Name to REP`TypeRep
    const Generic & wf_lp (infer.GetField (1)); // [bool]
    const MAP<TYPE_AS_Name,Tuple> & bd_l (infer.GetMap (2));

    Tuple infer2 (wf_Pattern (i, rp, tp)); // bool * map AS`Name to REP`TypeRep
    const Generic & wf_rp (infer2.GetField (1)); // [bool]
    const MAP<TYPE_AS_Name,Tuple> & bd_r (infer2.GetMap (2));

    Tuple infer3 (MergeBindings (i, mk_sequence(bd_l, bd_r))); // bool * map AS`Name to REP`TypeRep
    const Bool & wf_b (infer3.GetBool (1));
    const MAP<TYPE_AS_Name,Tuple> & bind (infer3.GetMap (2));

// 20101020 -->
    //TYPE_REP_TypeRep stp (ExtractSeqType(tp));
    TYPE_REP_TypeRep stp (RemoveEmptySeqType(ExtractSeqType(tp)));
// <-- 20101020
    GetCI().SetTypeInfo(cid, stp);

    if (wf_lp.IsNil() || wf_rp.IsNil())
      return mk_(Nil(), bind);
    else
      return mk_(wf_b && Bool(wf_lp) && Bool(wf_rp), bind);
  }
  else
  {
    MAP<TYPE_AS_Name,Tuple> bind (ExtractBindings (lp));
    bind.ImpOverride (ExtractBindings (rp));
    return mk_(Bool(false), bind);
  }
}

// wf_MapEnumPattern
// i : TYPE`Ind
// pat : AS`MapEnumPattern
// tp : REP`TypeRep
// ==> [bool] * map AS`Name to 9REP`TypeRep * nat1)
Tuple StatSem::wf_MapEnumPattern (const Int & i, const TYPE_AS_MapEnumPattern & pat, const TYPE_REP_TypeRep & tp)
{
  const SEQ<TYPE_AS_MapletPattern> & mls (pat.GetSequence(pos_AS_MapEnumPattern_mls));
  const TYPE_CI_ContextId & cid (pat.GetInt(pos_AS_MapEnumPattern_cid));

  TYPE_REP_TypeRep mtp (ExtractMapType (tp));

  GetCI().SetTypeInfo (cid, mtp);
  Generic elemtp = mtp;

  if (elemtp.IsNil())
  {
    if (mls.IsEmpty())
    {
      return mk_(Bool(true), MAP<TYPE_AS_Name,Tuple>());
    }
    else
    {
      MAP<TYPE_AS_Name,Tuple> dombd;
      MAP<TYPE_AS_Name,Tuple> rngbd;
      size_t len_mls = mls.Length();
      for (size_t idx = 1; idx <= len_mls; idx++)
      {
        dombd.ImpOverride (ExtractBindings (mls[idx].GetRecord(pos_AS_MapletPattern_dp)));
        rngbd.ImpOverride (ExtractBindings (mls[idx].GetRecord(pos_AS_MapletPattern_rp)));
      }
      Tuple t (MergeBindings(i, mk_sequence(dombd, rngbd)));
      return mk_(Bool(false), t.GetField(2));
    }
  }
  else
  {
    Set wf;
    Sequence b_s;
    size_t len_mls = mls.Length();
    for (size_t idx = 1; idx <= len_mls; idx++)
    {
      Tuple t (wf_MapletPattern (i, mls[idx], mtp)); // [bool] * map AS`Name to REP`TypeRep
      wf.Insert (t.GetField (1)); // [bool]
      b_s.ImpAppend (t.GetMap (2)); // map AS`Name to REP`TypeRep
    }

    Tuple infer (MergeBindings (i, b_s)); // bool * map AS`Name to REP`TypeRep
    const Bool & wf_b (infer.GetBool (1));
    const MAP<TYPE_AS_Name,Tuple> & bind (infer.GetMap (2));

    if (wf.InSet(Nil()))
      return mk_(Nil(), bind);
    else if (wf.InSet(Bool (false)))
      return mk_(Bool(false), bind);
    else
    {
      Bool tmpb (!(i == DEF) || (mtp == tp));
      return mk_(wf_b && tmpb, bind);
    }
  }
}

// wf_MapMergePattern
// i : TYPE`Ind
// pat : AS`MapMergePattern
// tp : REP`TypeRep
// ==> [bool] * map AS`Name to (REP`TypeRep * nat1)
Tuple StatSem::wf_MapMergePattern (const Int & i, const TYPE_AS_MapMergePattern & pat, const TYPE_REP_TypeRep & tp)
{
  const TYPE_AS_Pattern & lp (pat.GetRecord(pos_AS_MapMergePattern_lp));
  const TYPE_AS_Pattern & rp (pat.GetRecord(pos_AS_MapMergePattern_rp));
  const TYPE_CI_ContextId & cid (pat.GetInt(pos_AS_MapMergePattern_cid));

  TYPE_REP_TypeRep mtp (ExtractMapType(tp));
  GetCI().SetTypeInfo(cid, mtp);

  if (IsCompatible (i, tp, map_alltp))
  {
    Tuple infer (wf_Pattern (i, lp, tp));  // bool * map AS`Name to REP`TypeRep
    const Generic & wf_lp (infer.GetField (1)); // [bool]
    const MAP<TYPE_AS_Name,Tuple> & bd_l (infer.GetMap (2));

    Tuple infer2 (wf_Pattern (i, rp, tp));  // bool * map AS`Name to REP`TypeRep
    const Generic & wf_rp (infer2.GetField (1)); // [bool]
    const MAP<TYPE_AS_Name,Tuple> & bd_r (infer2.GetMap (2));

    Tuple infer3 (MergeBindings (i, mk_sequence(bd_l, bd_r))); // bool * map AS`Name to REP`TypeRep
    const Bool & wf_b (infer3.GetBool (1));
    const MAP<TYPE_AS_Name,Tuple> & bind (infer3.GetMap (2));

    if (wf_lp.IsNil() || wf_rp.IsNil())
      return mk_(Nil(), bind);
    else
      return mk_(wf_b && Bool(wf_lp) && Bool(wf_rp), bind);
  }
  else
  {
    MAP<TYPE_AS_Name,Tuple> bind (ExtractBindings (lp));
    bind.ImpOverride (ExtractBindings (rp));
    return mk_(Bool(false), bind);
  }
}

// wf_MapletPattern
// i : TYPE`Ind
// pat : AS`MapletPattern
// tp : REP`TypeRep
// ==> [bool] * map AS`Name to (REP`TypeRep * nat1)
Tuple StatSem::wf_MapletPattern(const Int & i, const TYPE_AS_MapletPattern & pat, const TYPE_REP_TypeRep & tp)
{
  const TYPE_AS_Pattern & dp (pat.GetRecord(pos_AS_MapletPattern_dp));
  const TYPE_AS_Pattern & rp (pat.GetRecord(pos_AS_MapletPattern_rp));
  const TYPE_CI_ContextId & cid (pat.GetInt(pos_AS_MapletPattern_cid));

  TYPE_REP_TypeRep mtp (ExtractMapType(tp));
  GetCI().SetTypeInfo(cid, mtp);

  if (IsCompatible (i, tp, map_alltp))
  {
    Generic domtp = UnmaskMapDomType(i, mtp);
    Generic rngtp = UnmaskMapRngType(i, mtp);

    Tuple infer (domtp.IsNil() ? wf_Pattern (i, dp, rep_alltp)
                               : wf_Pattern (i, dp, domtp));  // bool * map AS`Name to REP`TypeRep
    const Generic & wf_dp (infer.GetField (1)); // [bool]
    const MAP<TYPE_AS_Name,Tuple> & bd_d (infer.GetMap (2));

    Tuple infer2 (rngtp.IsNil() ? wf_Pattern (i, rp, rep_alltp)
                                : wf_Pattern (i, rp, rngtp));  // bool * map AS`Name to REP`TypeRep
    const Generic & wf_rp (infer2.GetField (1)); // [bool]
    const MAP<TYPE_AS_Name,Tuple> & bd_r (infer2.GetMap (2));

    Tuple infer3 (MergeBindings (i, mk_sequence(bd_d, bd_r))); // bool * map AS`Name to REP`TypeRep
    const Bool & wf_b (infer3.GetBool (1));
    const MAP<TYPE_AS_Name,Tuple> & bind (infer3.GetMap (2));

    if (wf_dp.IsNil() || wf_rp.IsNil())
      return mk_(Nil(), bind);
    else
      return mk_(wf_b && Bool(wf_dp) && Bool(wf_rp), bind);
  }
  else
  {
    MAP<TYPE_AS_Name,Tuple> bind (ExtractBindings (dp));
    bind.ImpOverride (ExtractBindings (rp));
    return mk_(Bool(false), bind);
  }
}

// FIXME
// wf_RecordPattern
// i : TYPE`Ind
// pat : AS`RecordPattern
// tp : REP`TypeRep
// ==> [bool] * map AS`Name to 9REP`TypeRep * nat1)
Tuple StatSem::wf_RecordPattern (const Int & i, const TYPE_AS_RecordPattern & pat, const TYPE_REP_TypeRep & tp)
{
  const TYPE_AS_Name & nm (pat.GetRecord(pos_AS_RecordPattern_nm));
  const SEQ<TYPE_AS_Pattern> & flds (pat.GetSequence(pos_AS_RecordPattern_fields));
  const TYPE_CI_ContextId & cid (pat.GetInt(pos_AS_RecordPattern_cid));

  Bool reswf(true);

  // for SL ==> [AS`Name * seq of REP`FieldRep]
  // for PP ==> [AS`Name * (AccessFieldRep|TagRepElem)]
  Generic pair (LookUpTag (i, nm, Set()));
  Generic comptp (ExtractCompositeType (tp));

  if (comptp.IsNil())
  {
    GetCI().SetTypeInfo (cid, rep_alltp);

    MAP<TYPE_AS_Name,Tuple> bind;
    size_t len_flds = flds.Length();
    for (size_t idx = 1; idx <= len_flds; idx++)
      bind.ImpOverride (ExtractBindings (flds[idx]));
    return mk_(Bool(false), bind);
  }

  if (pair.IsNil())
  {
    //-----------------------------------
    // Error message #296
    // There exists no type with tag L"%1"
    //-----------------------------------
    GenErr (nm, ERR, 296, mk_sequence(PrintName(nm)));

    GetCI().SetTypeInfo (cid, rep_alltp);

    MAP<TYPE_AS_Name,Tuple> bind;
    size_t len_flds = flds.Length();
    for (size_t idx = 1; idx <= len_flds; idx++)
      bind.ImpOverride (ExtractBindings (flds[idx]));
    return mk_(Nil (), bind);
  }
#ifdef VDMPP
//  else if (LookUpTypeInSuper((Tuple(pair)).GetRecord (1), GetCurClass()).IsNil())
//  {
//    //---------------------------------------------------
//    // Error message #418
//    // There exists no type with tag "%1" that is visible
//    //---------------------------------------------------
//    GenErr (nm, ERR, 418, mk_sequence(PrintName(nm)));
//
//    GetCI().SetTypeInfo (cid, rep_alltp);
//
//    MAP<TYPE_AS_Name,TYPE_REP_TypeRep> bind;
//    size_t len_flds = flds.Length();
//    for (size_t idx = 1; idx <= len_flds; idx++)
//      bind.ImpOverride (ExtractBindings (flds[idx]));
//    return mk_(Nil (), bind);
//  }
#endif // VDMPP

#ifdef VDMSL
  //TYPE_AS_Name newtag (ExtName(GetCurMod(), (Tuple(pair)).GetRecord (1)));
  TYPE_AS_Name newtag (Tuple(pair).GetRecord (1));
  SEQ<TYPE_REP_FieldRep> def_flds (Tuple(pair).GetSequence (2));
#endif // VDMSL
#ifdef VDMPP
  TYPE_AS_Name newtag (Tuple(pair).GetRecord (1));
  SEQ<TYPE_REP_FieldRep> def_flds (StripAccessType (Tuple(pair).GetSequence (2)));
#endif // VDMPP

  if (flds.Length() != def_flds.Length())
  {
    //--------------------------------
    // Error message #297
    // Wrong number of fields for L"%1"
    //--------------------------------
    GenErr (nm, ERR, 297, mk_sequence(PrintName(nm)));

    GetCI().SetTypeInfo (cid, rep_alltp);

    MAP<TYPE_AS_Name,Tuple> bind;
    size_t len_flds = flds.Length();
    for (size_t idx = 1; idx <= len_flds; idx++)
      bind.ImpOverride (ExtractBindings (flds[idx]));
    return mk_(Nil (), bind);
  }

  Generic realtp (GetRealType (tp));

  size_t len_def_flds = def_flds.Length();
  for (size_t index = 1; index <= len_def_flds; index++)
  {
    const TYPE_REP_FieldRep & field (def_flds[index]);
    if (field.GetBoolValue(pos_REP_FieldRep_dc))
    {
      const TYPE_AS_Pattern & pat (flds[index]);
      if (! pat.Is(TAG_TYPE_AS_PatternName))
      {
        const TYPE_AS_Name & sel_name (field.GetRecord(pos_REP_FieldRep_sel));
        //-----------------------------------------------------------------
        // Error message #298
        // Field L"%1" can only be matched with a name or don't care pattern
        //-----------------------------------------------------------------
        GenErr(newtag, ERR, 298, mk_sequence(PrintName(sel_name)));
        reswf = false;
      }
    }
  }

  Set wf;
  Sequence b_s;
  size_t len_flds = flds.Length();
  for (size_t idx = 1; idx <= len_flds; idx++)
  {
    Tuple t (wf_Pattern (i, flds[idx], def_flds[idx].GetRecord(pos_REP_FieldRep_tp)));
                                                   // [bool] * map AS`Name to REP`TypeRep
    wf.Insert (t.GetField (1)); // [bool]
    b_s.ImpAppend (t.GetMap (2)); // map AS`Name to REP`TypeRep
  }

  Tuple infer (MergeBindings (i, b_s)); // bool * map AS`Name to REP`TypeRep
  const Bool & wf_b (infer.GetBool (1));
  const MAP<TYPE_AS_Name,Tuple> & bind (infer.GetMap (2));

  TYPE_REP_CompositeTypeRep ctp (mk_REP_CompositeTypeRep(newtag, def_flds));

  Bool wf_tp;
  if (i == DEF)
  {
    if (realtp.IsNil ())
      wf_tp = Bool(false);
    else
    {
      TYPE_REP_TypeRep l_tp (realtp); // *****
      if (l_tp == ctp)
      {
        wf_tp = Bool(true);
      }
      else if (l_tp.Is(TAG_TYPE_REP_InvTypeRep))
      {
        TYPE_REP_TypeRep t (GetRealType (l_tp.GetRecord (pos_REP_InvTypeRep_shape)));
        if (t == ctp)
        {
          bool forall = true;
          for (size_t idx = 1; (idx <= len_flds) && forall; idx++)
            forall = SimplePatterns (flds[idx]);
          wf_tp = Bool(forall);
        }
        else
          wf_tp = Bool(false);
      }
      else
        wf_tp = Bool(true);
    }
  }
  else
  {
    wf_tp = IsCompatible (i, ctp, comptp);
  }

  if (i == DEF)
    GetCI().SetTypeInfo (cid, comptp);
  else
    GetCI().SetTypeInfo (cid, ctp);

  if (wf.InSet(Nil()))
    return mk_(Nil(), bind);
  else if (wf.InSet(Bool (false)))
    return mk_(Bool(false), bind);
  else
    return mk_(reswf && wf_b && wf_tp, bind);
}

// wf_TuplePattern
// TYPE`Ind
// pat : AS`TuplePattern
// tp : REP`TypeRep
// ==> [bool] * map AS`Name to (REP`TypeRep * nat1)
Tuple StatSem::wf_TuplePattern (const Int & i, const TYPE_AS_TuplePattern & pat, const TYPE_REP_TypeRep & tp)
{
  const SEQ<TYPE_AS_Pattern> & flds (pat.GetSequence(pos_AS_TuplePattern_fields));
  const TYPE_CI_ContextId & cid (pat.GetInt(pos_AS_TuplePattern_cid));

// 20131216 -->
  //TYPE_REP_ProductTypeRep ptp (ExtractProductType (tp, flds.Length(), EQ));
  //const SEQ<TYPE_REP_TypeRep> & tp_l (ptp.GetSequence (pos_REP_ProductTypeRep_tps));

  Generic ptp (ExtractProductType (tp, flds.Length(), EQ));
  if (ptp.IsNil())
  {
    MAP<TYPE_AS_Name,Tuple> bind;
    size_t len_flds = flds.Length();
    for (size_t idx = 1; idx <= len_flds; idx++)
      bind.ImpOverride (ExtractBindings (flds[idx]));

    SEQ<TYPE_REP_TypeRep> tp_l;
    for (size_t idx2 = 1; idx2 <= len_flds; idx2++)
    { 
      wf_Pattern(i, flds[idx2], rep_alltp);
      tp_l.ImpAppend(rep_alltp);
    }
    GetCI().SetTypeInfo (cid, rep_alltp);

    return mk_(Bool(false), bind);
  }

  const SEQ<TYPE_REP_TypeRep> & tp_l (Record(ptp).GetSequence (pos_REP_ProductTypeRep_tps));
// <-- 20131216


  if (tp_l.Length() != flds.Length())
  {
    MAP<TYPE_AS_Name,Tuple> bind;
    size_t len_flds = flds.Length();
    for (size_t idx = 1; idx <= len_flds; idx++)
      bind.ImpOverride (ExtractBindings (flds[idx]));

    SEQ<TYPE_REP_TypeRep> tp_l;
    for (size_t idx2 = 1; idx2 <= len_flds; idx2++)
    { 
      wf_Pattern(i, flds[idx2], rep_alltp);
      tp_l.ImpAppend(rep_alltp);
    }
    GetCI().SetTypeInfo (cid, mk_REP_ProductTypeRep(tp_l));

    return mk_(Bool(false), bind);
  }
  else
  {
    Generic realtp (GetRealType (tp));

    Set wf;
    Sequence b_s;
    size_t len_flds = flds.Length();
    for (size_t idx = 1; idx <= len_flds; idx++)
    {
      Tuple infer (wf_Pattern (i, flds[idx], tp_l[idx])); // [bool] * map AS`Name to REP`TypeRep
      wf.Insert (infer.GetField (1)); // [bool]
      b_s.ImpAppend (infer.GetMap (2)); // map AS`Name to REP`TypeRep
    }

    Tuple infer (MergeBindings (i, b_s)); // bool * map AS`Name to REP`TypeRep
    const Bool & wf_b (infer.GetBool (1));
    const MAP<TYPE_AS_Name,Tuple> & bind (infer.GetMap (2));

    Bool wf_tp (true);
    if (i == DEF)
    {
      if (realtp.IsNil ())
        wf_tp = Bool(false);
      else
      {
        if (realtp == ptp)
          wf_tp = Bool(true);
        else
        {
          TYPE_REP_TypeRep rtp (realtp);
          switch(rtp.GetTag()) {
            case TAG_TYPE_REP_UnionTypeRep: {
              SET<TYPE_REP_TypeRep> tps (rtp.GetSet(pos_REP_UnionTypeRep_tps));
              bool forall = true;
              Generic gt;
              for (bool bb = tps.First(gt); bb && forall; bb = tps.Next(gt))
              {
                TYPE_REP_TypeRep t (gt);
                if (t.Is(TAG_TYPE_REP_ProductTypeRep))
                {
                  const SEQ<TYPE_REP_TypeRep> & t_l (t.GetSequence(pos_REP_ProductTypeRep_tps));
                  forall = (t_l.Length() == tp_l.Length());
                }
                else
                  forall = false;
              }
              wf_tp = Bool(forall);
              break;
            }
            case TAG_TYPE_REP_InvTypeRep: {
              TYPE_REP_TypeRep t (GetRealType (rtp.GetRecord(pos_REP_InvTypeRep_shape)));
              if (t == ptp)
              {
                bool forall = true;
                for (size_t idx = 1; (idx <= len_flds) && forall; idx++)
                  forall = SimplePatterns (flds[idx]);
                wf_tp = Bool(forall);
              }
              else
                wf_tp = Bool(false);
              break;
            }
            default: {
              wf_tp = Bool(false);
              break;
            }
          }
        }
      }
    }
    else // (i == POS) 
    {
      wf_tp = IsCompatible (i, ptp, tp);
    }

    GetCI().SetTypeInfo (cid, ptp);

    if (wf.InSet(Nil()))
      return mk_(Nil(), bind);
    else if (wf.InSet(Bool (false)))
      return mk_(Bool(false), bind);
    else
      return mk_(wf_b && wf_tp, bind);
  }
}

#ifdef VDMPP
// wf_ObjectPattern
// TYPE`Ind
// pat : AS`ObjectPattern
// tp : REP`TypeRep
// ==> [bool] * map AS`Name to (REP`TypeRep * nat1)
Tuple StatSem::wf_ObjectPattern (const Int & i, const TYPE_AS_ObjectPattern & pat, const TYPE_REP_TypeRep & tp)
{
  const TYPE_AS_Name & cls (pat.GetRecord(pos_AS_ObjectPattern_cls));
  const SEQ<TYPE_AS_FieldPattern> & fields (pat.GetSequence(pos_AS_ObjectPattern_fields));

  if (CheckClassName (cls))
  {
    Set wf;
    Sequence b_s;
    size_t len_fields = fields.Length();
    for (size_t idx = 1; idx <= len_fields; idx++)
    {
      const TYPE_AS_FieldPattern & fp (fields[idx]);
      const TYPE_AS_Name & field (fp.GetRecord(pos_AS_FieldPattern_nm));
      const TYPE_AS_Pattern & p (fp.GetRecord(pos_AS_FieldPattern_pat));

      Generic gtp (LookUpInObject (cls, field, false, false));
      if (gtp.IsNil())
      {
        //--------------------------------------------
        // Error message #280
        // The field L"%1" is not defined in class "%2"
        //--------------------------------------------
        GenErr (field, ERR, 280, mk_sequence(PrintName(field), PrintName(cls)));
        wf.Insert (Nil());
      }
      else
      {
        Generic ca (CheckAccessCurClass(gtp));
        if (ca.IsNil ()) {
          //-------------------
          // Error message #368
          // Access violation
          //-------------------
          GenErr (field, ERR, 368, mk_sequence(PrintName (field)));
        }
        TYPE_REP_TypeRep ftp (StripAccessType(gtp));
        Tuple infer (wf_Pattern (i, p, ftp)); // [bool] * map AS`Name to REP`TypeRep
        wf.Insert (infer.GetField (1)); // [bool]
        b_s.ImpAppend (infer.GetMap (2)); // map AS`Name to REP`TypeRep
        GetCI().SetTypeInfo (ASTAUX::GetCid(p), ftp);
      }
    }
    Tuple infer (MergeBindings (i, b_s)); // bool * map AS`Name to REP`TypeRep
    const Bool & wf_b (infer.GetBool (1));
    const MAP<TYPE_AS_Name,Tuple> & bind (infer.GetMap (2));
    TYPE_REP_TypeRep ptp (mk_REP_ObjRefTypeRep(cls));
    bool wf_tp = IsCompatible (i, ptp, tp);
    GetCI().SetTypeInfo (pat.GetInt(pos_AS_ObjectPattern_cid), ptp);
    if (wf.InSet(Nil()))
      return mk_(Nil(), bind);
    else if (wf.InSet(Bool (false)))
      return mk_(Bool(false), bind);
    else
      return mk_(wf_b && wf_tp, bind);
  }
  else
  {
    //------------------------------
    // Error message #284
    // L"%1" is not an existing class
    //------------------------------
    GenErr (cls, ERR, 284, mk_sequence(PrintName (cls)));
    return mk_(Nil(), MAP<TYPE_AS_Name,Tuple>());
  }
}
#endif // VDMPP

// wf_PatternList
// i : TYPE`Ind
// p_l : seq of AS`Pattern
// tp_l : seq of REP`TypeRep
// Pos : AS`Name
// ==> bool * map AS`Name to (REP`TypeRep * nat1)
Tuple StatSem::wf_PatternList (const Int & i,
                               const SEQ<TYPE_AS_Pattern> & p_l,
                               const SEQ<TYPE_REP_TypeRep> & tp_l,
                               const TYPE_AS_Name & Pos)
{
  if (p_l.Length() != tp_l.Length())
  {
    //--------------------------------------------------
    // Error message #299
    // Number of patterns different than number of types
    //--------------------------------------------------
    GenErr (Pos, ERR, 299, Sequence());

    MAP<TYPE_AS_Name,Tuple> mp;
    size_t len_p_l = p_l.Length();
    for (size_t index = 1; index <= len_p_l; index++)
    {
      mp.ImpOverride (ExtractBindings (p_l[index]));
// 20100804 -->
      // for POG Error
      GetCI().SetTypeInfo (ASTAUX::GetCid(p_l[index]), rep_alltp);
// <-- 20100804
    }
    return mk_(Bool (false), mp);
  }

  Bool reswf (true);
  Sequence resbd;
  resbd.ImpAppend(Map());

  size_t len_p_l = p_l.Length();
  for (size_t idx = 1; idx <= len_p_l; idx++)
  {
    Tuple infer (wf_Pattern (i, p_l[idx], tp_l[idx])); // [bool] * map AS`Name to REP`TypeRep
    const Generic & wf_bd (infer.GetField (1)); // [bool]
    const MAP<TYPE_AS_Name,Tuple> & bd (infer.GetMap (2));

    if (wf_bd == Bool (false))
    {
      //---------------------
      // Error message #104
      // Pattern cannot match
      //---------------------
// 20100713 -->
      if (p_l[idx].Is(TAG_TYPE_AS_PatternName) && !p_l[idx].GetField(pos_AS_PatternName_tp).IsNil())
      {
        GenErrTp (p_l[idx], ERR, 104, p_l[idx].GetField(pos_AS_PatternName_tp), tp_l[idx], Sequence());
      }
      else
// <-- 20100713
      GenErr (p_l[idx], ERR, 104, Sequence());
    }

    reswf &= (wf_bd == Bool (true));
    resbd.ImpAppend(bd);
  }

  Tuple infer2 (MergeBindings (i, resbd)); // bool * map AS`Name to REP`TypeRep
  const Bool & wf_b (infer2.GetBool (1));
  const MAP<TYPE_AS_Name,Tuple> & bind (infer2.GetMap (2));

  return mk_(reswf && wf_b, bind);
}

// wf_Bind
// i : TYPE`Ind
// bind : AS`Bind
// ==> [bool] * map AS`Name to (REP`TypeRep * nat1)
Tuple StatSem::wf_Bind (const Int & i, const TYPE_AS_Bind & bind)
{
  switch(bind.GetTag()) {
    case TAG_TYPE_AS_SetBind:  { return wf_SetBind(i, bind); }
    case TAG_TYPE_AS_TypeBind: { return wf_TypeBind(i, bind); }
    default: { return Tuple(); } //This is only to avoid warnings from the compiler
  }
}

// wf_SetBind
// i : TYPE`Ind
// bind : AS`Bind
// ==> [bool] * map AS`Name to (REP`TypeRep * nat1)
Tuple StatSem::wf_SetBind (const Int & i, const TYPE_AS_SetBind & bind)
{
  const TYPE_AS_Pattern & p (bind.GetRecord(pos_AS_SetBind_pat));
  const TYPE_AS_Expr & s (bind.GetRecord(pos_AS_SetBind_Set));
  
// 20101025 -->
  //Tuple infer (wf_Expr (i, s, rep_alltp));
  Tuple infer (wf_Expr (i, s, mk_REP_SetTypeRep(rep_alltp)));
// <-- 20101025
  const Bool & wf_s (infer.GetBool (1));
  const TYPE_REP_TypeRep & stp (infer.GetRecord (2));
  
  if (IsCompatible (i, stp, set_alltp))
  {
    Generic elemtp (UnmaskSetType (i, ExtractSetType (stp)));

    if (elemtp.IsNil())
    {
      //-----------------------------------
      // Error message #293
      // An empty set is used in a set bind
      //-----------------------------------
      GenErr (s, WRN1, 293, Sequence());
      return mk_(Bool(false), ExtractBindings (p));
    }
    else
    {
      Tuple infer2 (wf_Pattern (i, p, elemtp)); // [bool] * map AS`Name to REP`TypeRep
      const Generic & wf_p (infer2.GetField (1)); // [bool]
      const MAP<TYPE_AS_Name,Tuple> & bd (infer2.GetMap (2));
  
      if (wf_p.IsNil())
        return mk_(Nil(), bd);
      else
        return mk_(wf_s && Bool (wf_p), bd);
    }
  }
  else
  {
    //-------------------------------------
    // Error message #295
    // Type of expression is not a set type
    //-------------------------------------
    GenErrTp (s, ERR, 295, stp, set_alltp, Sequence());
    return mk_(Nil(), ExtractBindings (p));
  }
}

// wf_TypeBind
// i : TYPE`Ind
// bind : AS`TypeBind
// ==> [bool] * map AS`Name to (REP`TypeRep * nat1)
Tuple StatSem::wf_TypeBind (const Int & i, const TYPE_AS_TypeBind & bind)
{
  const TYPE_AS_Pattern & p (bind.GetRecord(pos_AS_TypeBind_pat));
  const TYPE_AS_Type & t (bind.GetRecord(pos_AS_TypeBind_tp));

  return wf_Pattern (i, p, TransType(Nil (), t)); // [bool] * map AS`Name to REP`TypeRep
}

// wf_SeqBind
// i : TYPE`Ind
// bind : AS`SeqBind
// ==> [bool] * map AS`Name to (REP`TypeRep * nat1)
Tuple StatSem::wf_SeqBind (const Int & i, const TYPE_AS_SeqBind & bind)
{
  const TYPE_AS_Pattern & p (bind.GetRecord(pos_AS_SeqBind_pat));
  const TYPE_AS_Expr & s (bind.GetRecord(pos_AS_SeqBind_Seq));
  
  Tuple infer (wf_Expr (i, s, mk_REP_SeqTypeRep(rep_alltp)));
  const Bool & wf_s (infer.GetBool (1));
  const TYPE_REP_TypeRep & stp (infer.GetRecord (2));
  
  if (IsCompatible (i, stp, seq_alltp))
  {
    Generic elemtp (UnmaskSeqType (i, ExtractSeqType (stp)));

    if (elemtp.IsNil())
    {
      //-----------------------------------
      // Error message #462
      // An empty seq is used in a seq bind
      //-----------------------------------
      GenErr (s, WRN1, 462, Sequence());
      return mk_(Bool(false), ExtractBindings (p));
    }
    else
    {
      Tuple infer2 (wf_Pattern (i, p, elemtp)); // [bool] * map AS`Name to REP`TypeRep
      const Generic & wf_p (infer2.GetField (1)); // [bool]
      const MAP<TYPE_AS_Name,Tuple> & bd (infer2.GetMap (2));

      if (wf_p.IsNil())
        return mk_(Nil(), bd);
      else
        return mk_(wf_s && Bool (wf_p), bd);
    }
  }
  else
  {
    //-------------------------------------
    // Error message #237
    // Expression is not a sequence type
    //-------------------------------------
    GenErrTp (s, ERR, 237, stp, seq_alltp, Sequence());
    return mk_(Nil(), ExtractBindings (p));
  }
}

// wf_BindAndType
// i : TYPE`Ind
//  bind : AS`Bind
// ==> [bool] * (map AS`Name to (REP`TypeRep * nat1)) * REP`TypeRep
Tuple StatSem::wf_BindAndType (const Int & i, const TYPE_AS_Bind & bind)
{
  switch(bind.GetTag()) {
    case TAG_TYPE_AS_SetBind: {
      const TYPE_AS_Pattern & p (bind.GetRecord(pos_AS_SetBind_pat));
      const TYPE_AS_Expr & s (bind.GetRecord(pos_AS_SetBind_Set));
  
// 20101025 -->
      //Tuple infer (wf_Expr (i, s, rep_alltp));
      Tuple infer (wf_Expr (i, s, set_alltp));
// <-- 20101025
      const Bool & wf_s (infer.GetBool (1));
      const TYPE_REP_TypeRep & stp (infer.GetRecord (2));
  
      if (IsCompatible (i, stp, set_alltp))
      {
        Generic elemtp (UnmaskSetType (i, ExtractSetType (stp)));
  
        if (elemtp.IsNil())
        {
          //-----------------------------------
          // Error message #293
          // An empty set is used in a set bind
          //-----------------------------------
          GenErr (s, WRN1, 293, Sequence());
          return mk_(Bool(false), ExtractBindings (p), rep_alltp);
        }
        else
        {
          Tuple  infer2 (wf_Pattern (i, p, elemtp)); // [bool] * map AS`Name to REP`TypeRep
          const Generic & wf_p (infer2.GetField (1)); // [bool]
          const MAP<TYPE_AS_Name,Tuple> & bd (infer2.GetMap (2));
  
          if (wf_p.IsNil())
            return mk_(Nil(), bd, elemtp);
          else
            return mk_(wf_s && Bool (wf_p), bd, elemtp);
        }
      }
      else
      {
        //-------------------------------------
        // Error message #295
        // Type of expression is not a set type
        //-------------------------------------
        GenErrTp (s, ERR, 295, stp, set_alltp, Sequence());
        return mk_(Nil(), ExtractBindings (p), rep_alltp);
      }
    }
    case TAG_TYPE_AS_TypeBind: {
      const TYPE_AS_Pattern & p (bind.GetRecord(pos_AS_TypeBind_pat));
      const TYPE_AS_Type & t (bind.GetRecord(pos_AS_TypeBind_tp));

      Tuple infer (wf_Pattern (i, p, TransType(Nil (), t))); // [bool] * map AS`Name to REP`TypeRep
      return mk_(infer.GetField (1), // [bool]
                 infer.GetMap (2),
                 TransType(Nil (), t));
    }
    default: {
      //This is only to avoid warnings from the compiler
      return Tuple();
    }
  }
}

// wf_TypeBindList
// i : TYPE`Ind
// parms : seq of AS`TypeBind
// ==> [bool] * (map AS`Name to (REP`TypeRep * nat1)) * seq of REP`TypeRep
Tuple StatSem::wf_TypeBindList (const Int & i, const SEQ<TYPE_AS_TypeBind> & parms)
{
  Generic reswf = Bool(true);
  Sequence resbd;
  SET<TYPE_AS_Name> altbd;
  SEQ<TYPE_REP_TypeRep> restp;

  size_t len_parms = parms.Length();
  for (size_t idx = 1; idx <= len_parms; idx++)
  {
    const TYPE_AS_TypeBind & tb (parms[idx]);
    Tuple infer (wf_BindAndType (i, tb));
    const Generic & wf (infer.GetField (1)); // [bool]
    const MAP<TYPE_AS_Name,Tuple> & bd (infer.GetMap (2));
    const TYPE_REP_TypeRep & tp (infer.GetRecord (3));

    if (reswf.IsNil () || wf.IsNil ())
      reswf = Nil ();
    else
      reswf = Bool (reswf) && Bool (wf);

    resbd.ImpAppend (bd);
    restp.ImpAppend (tp);
//    if (bd.Dom().SubSet (altbd))
    Set dom_bd (bd.Dom());
    if (!altbd.Intersect(dom_bd).IsEmpty())
    {
      //----------------------------------------------------------------
      // Error message #300
      // Multiple use of pattern identifier in parameters to Lambda-Expr
      //----------------------------------------------------------------
      GenErr (tb, WRN1, 300, Sequence());
    }
    altbd.ImpUnion (dom_bd);
  }

  Bool ok (false);
  MAP<TYPE_AS_Name,Tuple> binds;
  
  if (reswf == Bool(true))
  {
    Tuple infer2 (MergeBindings (i, resbd)); // bool * map AS`Name to REP`TypeRep
    ok = infer2.GetBool (1);
    binds = infer2.GetField (2);
  }
  else
  {
    Generic nm;
    for (bool cc = altbd.First(nm); cc; cc = altbd.Next(nm))
      binds.Insert (nm, mk_(rep_alltp, Int(1)));
  }

  if (reswf.IsNil())
    return mk_(Nil(), binds, restp);
  else
    return mk_(ok && Bool (reswf), binds, restp);
}

// wf_MultiBindList
// i : TYPE`Ind
// mbs : seq of AS`MultBind
// ==> [bool] * map AS`Name to (REP`TypeRep * nat1)
Tuple StatSem::wf_MultiBindList (const Int & i, const SEQ<TYPE_AS_MultBind> & mbs, bool mustmatch)
{
  Generic reswf = Bool(true);
  Sequence resbd;
  SET<TYPE_AS_Name> altbd;

  size_t len_mbs = mbs.Length();
  for (size_t idx = 1; idx <= len_mbs; idx++)
  {
    const TYPE_AS_MultBind & mb (mbs[idx]);
    Tuple infer (mb.Is(TAG_TYPE_AS_MultSetBind) ? wf_MultiSetBind (i, mb, mustmatch) : wf_MultiTypeBind (i, mb));
    const Generic & wf (infer.GetField (1)); // [bool]
    const MAP<TYPE_AS_Name,Tuple> & bd (infer.GetMap (2));

    if (reswf.IsNil() || wf.IsNil())
      reswf = Nil();
    else
      reswf = Bool (reswf) && Bool (wf);

    resbd.ImpAppend (bd);
    altbd.ImpUnion (bd.Dom());
  }

  Bool ok (false);
  MAP<TYPE_AS_Name,Tuple> binds;

  if (reswf == Bool(true))
  {
    Tuple infer2 (MergeBindings (i, resbd)); // bool * map AS`Name to REP`TypeRep
    ok = infer2.GetBool (1);
    binds = infer2.GetField (2);
  }
  else
  {
    Generic nm;
    for (bool cc = altbd.First(nm); cc; cc = altbd.Next(nm))
      binds.Insert (nm, mk_(rep_alltp, Int(1)));
  }

  if (reswf.IsNil())
    return mk_(Nil(), binds);
  else
    return mk_(ok && Bool (reswf), binds);
}

// wf_MultiSetBind
// i : TYPE`Ind
// msb : AS`MultSetBind
// mustmatch : bool
// ==> [bool] * map AS`Name to (REP`TypeRep * nat1)
Tuple StatSem::wf_MultiSetBind (const Int & i, const TYPE_AS_MultSetBind & msb, bool mustmatch)
{
  const SEQ<TYPE_AS_Pattern> & pats (msb.GetSequence(pos_AS_MultSetBind_pat));
  const TYPE_AS_Expr & e (msb.GetRecord(pos_AS_MultSetBind_Set));

  MAP<TYPE_AS_Name,Tuple> altbinds;

  size_t len_pats = pats.Length();
  for (size_t idx = 1; idx <= len_pats; idx++)
    altbinds.ImpOverride (ExtractBindings (pats[idx]));

// 20101025 -->
  //Tuple infer (wf_Expr (i, e, rep_alltp));
  Tuple infer (mustmatch ? wf_Expr (i, e, mk_REP_SetTypeRep(rep_alltp)) : wf_Expr (i, e, set_alltp));
// <-- 20101025
  const Bool & wf_e (infer.GetBool(1));
  const TYPE_REP_TypeRep & etp (infer.GetRecord (2));

  if (IsCompatible (i, etp, set_alltp))
  {
    Generic reswf = Bool(true);
    Generic sttp (UnmaskSetType (POS, ExtractSetType (etp)));

// 2010-720 -->
    //if (sttp.IsNil()) //|| etp.Is(EmptySetTypeRep))
    if (sttp.IsNil() || (mustmatch && (i == DEF) && IsCompatible (POS, etp, mk_REP_EmptySetTypeRep(rep_alltp))))
// <-- 20100720
    {
      //-----------------------------------
      // Error message #293
      // An empty set is used in a set bind
      //-----------------------------------
      GenErr (e, WRN1, 293, Sequence());

      if (mustmatch)
        reswf = Nil();
    }

    Generic sttp_q (sttp);
    if (sttp.IsNil())
      sttp_q = rep_alltp;

    Sequence resbd;
    for (size_t idx = 1; idx <= len_pats; idx++)
    {
      Tuple infer2 (wf_Pattern (i, pats[idx], sttp_q)); // [bool] * map AS`Name to REP`TypeRep
      const Generic & match (infer2.GetField (1)); // [bool]
      const MAP<TYPE_AS_Name,Tuple> & bd (infer2.GetMap (2));

      if (reswf.IsNil() || match.IsNil())
        reswf = Nil();
      else
        reswf = Bool (reswf) && Bool (match);

      resbd.ImpAppend (bd);
    }

    Tuple infer3 ((reswf == Bool(true)) ? MergeBindings (i, resbd) : mk_(Bool(false), altbinds)); 
    const Bool & ok (infer3.GetBool(1));
    const MAP<TYPE_AS_Name,Tuple> & binds (infer3.GetMap(2));

    if (reswf.IsNil() || !wf_e)
      return mk_(Nil(), binds);
    else
      return mk_(ok && Bool (reswf), binds);
  }
  else
  {
    //-------------------------------------
    // Error message #295
    // Type of expression is not a set type
    //-------------------------------------
    if (mustmatch)
      GenErrTp (e, ERR, 295, etp, mk_REP_SetTypeRep(rep_alltp), Sequence());
    else
      GenErrTp (e, ERR, 295, etp, set_alltp, Sequence());
    return mk_(Nil(), altbinds);
  }
}

// wf_MultiTypeBind
// i : TYPE`Ind
// msb :  AS`MultTypeBind
// ==> [bool] * map AS`Name to (REP`TypeRep * nat1)
Tuple StatSem::wf_MultiTypeBind (const Int & i, const TYPE_AS_MultTypeBind & msb)
{
  const SEQ<TYPE_AS_Pattern> & pats (msb.GetSequence(pos_AS_MultTypeBind_pat));
  const TYPE_AS_Type & tp (msb.GetRecord(pos_AS_MultTypeBind_tp));

  MAP<TYPE_AS_Name,Tuple> altbinds;
  size_t len_pats = pats.Length();
  for (size_t idx = 1; idx <= len_pats; idx++)
    altbinds.ImpOverride (ExtractBindings (pats[idx]));

  Generic reswf = Bool(true);
  Sequence resbd;
  for (size_t idp = 1; idp <= len_pats; idp++)
  {
    Tuple infer (wf_Pattern (i, pats[idp], TransType(Nil (),tp))); // [bool] * map AS`Name to REP`TypeRep
    const Generic & match (infer.GetField (1)); // [bool]
    const MAP<TYPE_AS_Name,Tuple> & bd (infer.GetMap (2));

    if (reswf.IsNil() || match.IsNil())
      reswf = Nil();
    else
      reswf = Bool (reswf) && Bool (match);

    resbd.ImpAppend (bd);
  }

  Tuple infer2 ((reswf == Bool(true)) ? MergeBindings (i, resbd) // bool * map AS`Name to REP`TypeRep
                                      : mk_(Bool(false), altbinds));
  const Bool & ok (infer2.GetBool(1));
  const MAP<TYPE_AS_Name,Tuple> & binds (infer2.GetMap(2));

  if (reswf.IsNil())
    return mk_(Nil(), binds);
  else
    return mk_(ok && Bool (reswf), binds);
}

// MergeBindings
// i : TYPE`Ind
// l : seq of (map AS`Name to (REP`TypeRep * nat1))
// ==> bool * map AS`Name to (REP`TypeRep * nat1)
Tuple StatSem::MergeBindings (const Int & i, const Sequence & l)
{
  switch(l.Length()) {
    case 0:
      return mk_(Bool(true), MAP<TYPE_AS_Name,Tuple>());
    case 1:
      return mk_(Bool(true), l.Hd());
    default: {
      MAP<TYPE_AS_Name,Tuple> bind (l.Hd());
      Sequence l_s (l.Tl());
  
      Tuple infer (MergeBindings (i, l_s)); // bool * map AS`Name to REP`TypeRep
      const Bool & ok (infer.GetBool (1));
      const MAP<TYPE_AS_Name,Tuple> & bd (infer.GetMap (2));
  
      SET<TYPE_AS_Name> common (bind.Dom());
      common.ImpIntersect (bd.Dom());
  
      if (common.IsEmpty())
        return mk_(ok, bind.ImpOverride(bd));
      else
      {
        Bool all_ok (true);
        MAP<TYPE_AS_Name,Tuple> newbd;
  
        Generic el;
        for (bool bb = common.First(el); bb; bb = common.Next(el))
        {
          TYPE_REP_TypeRep t1 (bind[el].GetRecord(1)), t2 (bd[el].GetRecord(1));
          Int num (bind[el].GetInt(2) + bd[el].GetInt(2));
          if (!(t1 == t2))
          {
            if (IsCompatible (i, t1, t2))
            {
              Generic t (IntersectTypes(t1, t2));
              if (t.IsNil ()) // Safty check (HV)
                t = t1;
              newbd.ImpModify (el, mk_(t, num));
            }
            else if ((i == DEF) && IsCompatible (i, t2, t1))
              newbd.ImpModify (el, mk_(t2, num));
            else
            {
              //-----------------------------------------------------
              // Error message #301
              // L"%1" is bound multiple times with incompatible types
              //-----------------------------------------------------
              GenErrTp (el, ERR, 301, t1, t2, mk_sequence(PrintName (el)));
              newbd.ImpModify (el, mk_(MergeTypes (t1, t2), Int(1)));
              all_ok = Bool(false);
            }
          } else {
            newbd.ImpModify (el, mk_(t1, num));
          }
        }
        // TODO:
        //wcout << L"common: " << common << endl;
        newbd.ImpOverride(bind.DomRestrictedBy(common));
        newbd.ImpOverride(bd.DomRestrictedBy(common));
        return mk_(all_ok && ok, newbd);
      }
    }
  }
}

// ExtractPatternName
// pat : AS`Pattern
// -> map AS`Name to nat1
MAP<TYPE_AS_Name, Int> StatSem::ExtractPatternName (const TYPE_AS_Pattern & pat)
{
  switch (pat.GetTag()) {
    case TAG_TYPE_AS_PatternName : {
      const Generic & nm (pat.GetField(pos_AS_PatternName_nm));
      MAP<TYPE_AS_Name, Int> st;
      if (!nm.IsNil())
        st.Insert (nm, Int(1));
      return st;
    }
    case TAG_TYPE_AS_MatchVal: {
      return MAP<TYPE_AS_Name, Int>();
    }
    case TAG_TYPE_AS_SetEnumPattern: {
      const SEQ<TYPE_AS_Pattern> & pats (pat.GetSequence(pos_AS_SetEnumPattern_Elems));
      SEQ<Map> st;
      size_t len_pats = pats.Length();
      for (size_t idx = 1; idx <= len_pats; idx++)
        st.ImpAppend (ExtractPatternName (pats[idx]));
      return MergePatternName(st);
    }
    case TAG_TYPE_AS_SetUnionPattern: {
      SEQ<Map> st;
      st.ImpAppend (ExtractPatternName (pat.GetRecord(pos_AS_SetUnionPattern_lp)));
      st.ImpAppend (ExtractPatternName (pat.GetRecord(pos_AS_SetUnionPattern_rp)));
      return MergePatternName(st);
    }
    case TAG_TYPE_AS_SeqEnumPattern: {
      const SEQ<TYPE_AS_Pattern> & pats (pat.GetSequence(pos_AS_SeqEnumPattern_els));

      SEQ<Map> st;
      size_t len_pats = pats.Length();
      for (size_t idx = 1; idx <= len_pats; idx++)
        st.ImpAppend (ExtractPatternName (pats[idx]));
      return MergePatternName(st);
    }
    case TAG_TYPE_AS_SeqConcPattern: {
      SEQ<Map> st;
      st.ImpAppend (ExtractPatternName (pat.GetRecord(pos_AS_SeqConcPattern_lp)));
      st.ImpAppend (ExtractPatternName (pat.GetRecord(pos_AS_SeqConcPattern_rp)));
      return MergePatternName(st);
    }
    case TAG_TYPE_AS_MapletPattern: {
      SEQ<Map> st;
      st.ImpAppend (ExtractPatternName (pat.GetRecord(pos_AS_MapletPattern_dp)));
      st.ImpAppend (ExtractPatternName (pat.GetRecord(pos_AS_MapletPattern_rp)));
      return MergePatternName(st);
    }
    case TAG_TYPE_AS_MapEnumPattern: {
      const SEQ<TYPE_AS_MapletPattern> & pats (pat.GetSequence(pos_AS_MapEnumPattern_mls));

      SEQ<Map> st;
      size_t len_pats = pats.Length();
      for (size_t idx = 1; idx <= len_pats; idx++)
        st.ImpAppend (ExtractPatternName (pats[idx]));
      return MergePatternName(st);
    }
    case TAG_TYPE_AS_MapMergePattern: {
      SEQ<Map> st;
      st.ImpAppend (ExtractPatternName (pat.GetRecord(pos_AS_MapMergePattern_lp)));
      st.ImpAppend (ExtractPatternName (pat.GetRecord(pos_AS_MapMergePattern_rp)));
      return MergePatternName(st);
    }
    case TAG_TYPE_AS_RecordPattern: {
      const SEQ<TYPE_AS_Pattern> & pats (pat.GetSequence(pos_AS_RecordPattern_fields));
      SEQ<Map> st;
      size_t len_pats = pats.Length();
      for (size_t idx = 1; idx <= len_pats; idx++)
        st.ImpAppend (ExtractPatternName (pats[idx]));
      return MergePatternName(st);
    }
    case TAG_TYPE_AS_TuplePattern: {
      const SEQ<TYPE_AS_Pattern> & pats (pat.GetSequence(pos_AS_TuplePattern_fields));
      SEQ<Map> st;
      size_t len_pats = pats.Length();
      for (size_t idx = 1; idx <= len_pats; idx++)
        st.ImpAppend (ExtractPatternName (pats[idx]));
      return MergePatternName(st);
    }
#ifdef VDMPP
    case TAG_TYPE_AS_ObjectPattern: {
      const SEQ<TYPE_AS_FieldPattern> & pats (pat.GetSequence(pos_AS_ObjectPattern_fields));
      SEQ<Map> st;
      size_t len_pats = pats.Length();
      for (size_t idx = 1; idx <= len_pats; idx++)
        st.ImpAppend (ExtractPatternName (pats[idx]));
      return MergePatternName(st);
    }
    case TAG_TYPE_AS_FieldPattern: {
      return ExtractPatternName (pat.GetRecord(pos_AS_FieldPattern_pat));
    }
#endif // VDMPP
    default: {
      //This is only to avoid warnings from the compiler
      return MAP<TYPE_AS_Name, Int>();
    }
  }
}

// MergePatternName
// ptml : seq of map AS`Name to nat1
// -> map AS`Name to nat1
MAP<TYPE_AS_Name, Int> StatSem::MergePatternName (const SEQ<Map> & ptml)
{
  switch (ptml.Length()) {
    case 0: {
      return Map();
    }
    case 1: {
      return ptml[1];
    }
    default: {
      MAP<TYPE_AS_Name, Int> m1 (ptml.Hd());
      MAP<TYPE_AS_Name, Int> m2 (MergePatternName(ptml.Tl()));
      Set common (m1.Dom().Intersect(m2.Dom()));
      if (common.IsEmpty()) {
        m1.ImpOverride(m2);
        return m1;
      }
      MAP<TYPE_AS_Name, Int> res;
      res.ImpOverride(m1.DomRestrictedBy(common));
      res.ImpOverride(m2.DomRestrictedBy(common));
      Generic nm;
      for (bool bb = common.First(nm); bb; bb = common.Next(nm)) {
        Int n1 (m1[nm]), n2 (m2[nm]);
        Int sum (n1 + n2);
        res.Insert(nm, sum); 
      }
      return res;
    }
  }
}

// ExtractBindings
// pat : AS`Pattern
// -> map AS`Name to (REP`TypeRep * nat1)
MAP<TYPE_AS_Name,Tuple> StatSem::ExtractBindings (const TYPE_AS_Pattern & pat)
{
  MAP<TYPE_AS_Name,Tuple> res;

  SET<TYPE_AS_Name> pat_s (ExtractPatternName (pat).Dom());
  Generic nm;
  for (bool bb = pat_s.First(nm); bb; bb = pat_s.Next(nm))
    res.Insert (nm, mk_(rep_alltp, Int(1)));
  return res;
}

// SimplePatterns
// pat : AS`Pattern
// -> bool
bool StatSem::SimplePatterns (const TYPE_AS_Pattern & pat)
{
  switch (pat.GetTag()) {
    case TAG_TYPE_AS_PatternName: {
      return true;
    }
    case TAG_TYPE_AS_MatchVal:
    case TAG_TYPE_AS_SetEnumPattern:
    case TAG_TYPE_AS_SeqEnumPattern:
    case TAG_TYPE_AS_MapEnumPattern: {
      return false;
    }
    case TAG_TYPE_AS_SetUnionPattern: {
      return SimplePatterns (pat.GetRecord(pos_AS_SetUnionPattern_lp)) &&
             SimplePatterns (pat.GetRecord(pos_AS_SetUnionPattern_rp));
    }
    case TAG_TYPE_AS_SeqConcPattern: {
      return SimplePatterns (pat.GetRecord(pos_AS_SeqConcPattern_lp)) &&
             SimplePatterns (pat.GetRecord(pos_AS_SeqConcPattern_rp));
    }
    case TAG_TYPE_AS_MapMergePattern: {
      return SimplePatterns (pat.GetRecord(pos_AS_MapMergePattern_lp)) &&
             SimplePatterns (pat.GetRecord(pos_AS_MapMergePattern_rp));
    }
    case TAG_TYPE_AS_RecordPattern: {
      const SEQ<TYPE_AS_Pattern> & p_l (pat.GetSequence(pos_AS_RecordPattern_fields));
      bool forall = true;
      size_t len_p_l = p_l.Length();
      for (size_t idx = 1; (idx <= len_p_l) && forall; idx++)
        forall = SimplePatterns (p_l[idx]);
      return forall;
    }
    case TAG_TYPE_AS_TuplePattern: {
      const SEQ<TYPE_AS_Pattern> & p_l (pat.GetSequence(pos_AS_TuplePattern_fields));
      bool forall = true;
      size_t len_p_l = p_l.Length();
      for (size_t idx = 1; (idx <= len_p_l) && forall; idx++)
        forall = SimplePatterns (p_l[idx]);
      return forall;
    }
#ifdef VDMPP
    case TAG_TYPE_AS_ObjectPattern: {
      const SEQ<TYPE_AS_FieldPattern> & p_l (pat.GetSequence(pos_AS_ObjectPattern_fields));
      bool forall = true;
      size_t len_p_l = p_l.Length();
      for (size_t idx = 1; (idx <= len_p_l) && forall; idx++)
        forall = SimplePatterns (p_l[idx]);
      return forall;
    }
    case TAG_TYPE_AS_FieldPattern: {
      return SimplePatterns (pat.GetRecord(pos_AS_FieldPattern_pat));
    }
#endif // VDMPP
    default:
      return true;
  }
}

// GetRealType
// tp : REP`TypeRep
// ==> REP`TypeRep
TYPE_REP_TypeRep StatSem::GetRealType (const TYPE_REP_TypeRep & tp)
{
  TYPE_REP_TypeRep realtp (tp);

  while (realtp.Is(TAG_TYPE_REP_TypeNameRep) && !IsUnionRecursive (realtp))
  {
#ifdef VDMPP
    Record l_tp (LookUpTypeName (realtp.GetRecord(pos_REP_TypeNameRep_nm), true));
    if (IsAccessType(l_tp))
      realtp = StripAccessType(l_tp);
    else
      realtp = l_tp;
#endif //VDMPP
#ifdef VDMSL
    Record l_tp (LookUpTypeName (realtp.GetRecord(pos_REP_TypeNameRep_nm)));
    realtp = l_tp;
#endif // VDMSL
  }
  return realtp;
}

// PatternBind2TypeRep
// pat : AS`PatternBind
// -> REP`TypeRep
TYPE_REP_TypeRep StatSem::PatternBind2TypeRep (const TYPE_AS_PatternBind & patbind)
{
  switch(patbind.GetTag()) {
    case TAG_TYPE_AS_SetBind: {
      const TYPE_AS_Expr & s (patbind.GetRecord(pos_AS_SetBind_Set));
      Tuple infer (wf_Expr (POS, s, mk_REP_SetTypeRep(rep_alltp)));
      const TYPE_REP_TypeRep & stp (infer.GetRecord (2));
      if (stp.Is(TAG_TYPE_REP_SetTypeRep))
        return stp.GetRecord(pos_REP_SetTypeRep_elemtp);
      else
        return rep_alltp;
    }
    case TAG_TYPE_AS_TypeBind: {
      const TYPE_AS_Type & tp (patbind.GetRecord(pos_AS_TypeBind_tp));
      return TransType(Nil (), tp);
    }
    default: {
      return Pattern2TypeRep(patbind);
    }
  }
}

// Pattern2TypeRep
// pat : AS`Pattern
// -> REP`TypeRep
TYPE_REP_TypeRep StatSem::Pattern2TypeRep (const TYPE_AS_Pattern & pat)
{
  switch (pat.GetTag()) {
    case TAG_TYPE_AS_PatternName:
    case TAG_TYPE_AS_MatchVal: {
      return rep_alltp;
    }
    case TAG_TYPE_AS_SetEnumPattern: {
      const SEQ<TYPE_AS_Pattern> & Elems (pat.GetSequence(pos_AS_SetEnumPattern_Elems));
      SET<TYPE_REP_TypeRep> tp_s;
      size_t len_Elems = Elems.Length();
      for (size_t idx = 1; idx <= len_Elems; idx++)
        tp_s.Insert(Pattern2TypeRep(Elems[idx]));
      switch (tp_s.Card()) {
        case 0: {
          return mk_REP_SetTypeRep(rep_alltp);
        }
        case 1: {
          return mk_REP_SetTypeRep(tp_s.GetElem());
        }
        default: {
          return mk_REP_SetTypeRep(mk_REP_UnionTypeRep(tp_s));
        }
      }
    }
    case TAG_TYPE_AS_SeqEnumPattern: {
      const SEQ<TYPE_AS_Pattern> & els (pat.GetSequence(pos_AS_SeqEnumPattern_els));
      SET<TYPE_REP_TypeRep> tp_s;
      size_t len_els = els.Length();
      for (size_t idx = 1; idx <= len_els; idx++)
        tp_s.Insert(Pattern2TypeRep(els[idx]));
      switch (tp_s.Card()) {
        case 0: {
          return mk_REP_SeqTypeRep(rep_alltp);
        }
        case 1: {
          return mk_REP_SeqTypeRep(tp_s.GetElem());
        }
        default: {
          return mk_REP_SeqTypeRep(mk_REP_UnionTypeRep(tp_s));
        }
      }
    }
    case TAG_TYPE_AS_MapEnumPattern: {
      const SEQ<TYPE_AS_MapletPattern> & mls (pat.GetSequence(pos_AS_MapEnumPattern_mls));
      SET<TYPE_REP_TypeRep> dtp_s;
      SET<TYPE_REP_TypeRep> rtp_s;
      size_t len_mls = mls.Length();
      for (size_t idx = 1; idx <= len_mls; idx++)
      {
        dtp_s.Insert(Pattern2TypeRep(mls[idx].GetRecord(pos_AS_MapletPattern_dp)));
        rtp_s.Insert(Pattern2TypeRep(mls[idx].GetRecord(pos_AS_MapletPattern_rp)));
      }
      TYPE_REP_TypeRep dtp;
      switch (dtp_s.Card()) {
        case 0: {
          dtp = rep_alltp;
          break;
        }
        case 1: {
          dtp = dtp_s.GetElem();
          break;
        }
        default: {
          dtp = mk_REP_UnionTypeRep(dtp_s);
          break;
        }
      }
      TYPE_REP_TypeRep rtp;
      switch (rtp_s.Card()) {
        case 0: {
          rtp = rep_alltp;
          break;
        }
        case 1: {
          rtp = rtp_s.GetElem();
          break;
        }
        default: {
          rtp = mk_REP_UnionTypeRep(rtp_s);
          break;
        }
      }
      return mk_REP_GeneralMapTypeRep(dtp, rtp);
    }
    case TAG_TYPE_AS_SetUnionPattern: {
      TYPE_REP_TypeRep lp (Pattern2TypeRep(pat.GetRecord(pos_AS_SetUnionPattern_lp)));
      TYPE_REP_TypeRep rp (Pattern2TypeRep(pat.GetRecord(pos_AS_SetUnionPattern_rp)));
      SET<TYPE_REP_TypeRep> tp_s; 
      if (lp.Is(TAG_TYPE_REP_SetTypeRep))
      {
        TYPE_REP_TypeRep tp (lp.GetRecord(pos_REP_SetTypeRep_elemtp));
        if (tp.Is(TAG_TYPE_REP_UnionTypeRep))
          tp_s.ImpUnion(tp.GetSet(pos_REP_UnionTypeRep_tps));
        else
          tp_s.Insert(tp);
      }
      if (rp.Is(TAG_TYPE_REP_SetTypeRep))
      {
        TYPE_REP_TypeRep tp (rp.GetRecord(pos_REP_SetTypeRep_elemtp));
        if (tp.Is(TAG_TYPE_REP_UnionTypeRep))
          tp_s.ImpUnion(tp.GetSet(pos_REP_UnionTypeRep_tps));
        else
          tp_s.Insert(tp);
      }
      switch (tp_s.Card()) {
        case 0: {
          return mk_REP_SetTypeRep(rep_alltp);
        }
        case 1: {
          return mk_REP_SetTypeRep(tp_s.GetElem());
        }
        default: {
          return mk_REP_SetTypeRep(mk_REP_UnionTypeRep(tp_s));
        }
      }
    }
    case TAG_TYPE_AS_SeqConcPattern: {
      TYPE_REP_TypeRep ltp (Pattern2TypeRep(pat.GetRecord(pos_AS_SeqConcPattern_lp)));
      TYPE_REP_TypeRep rtp (Pattern2TypeRep(pat.GetRecord(pos_AS_SeqConcPattern_rp)));
      SET<TYPE_REP_TypeRep> tp_s; 
      if (ltp.Is(TAG_TYPE_REP_SeqTypeRep))
      {
        TYPE_REP_TypeRep tp (ltp.GetRecord(pos_REP_SeqTypeRep_elemtp));
        if (tp.Is(TAG_TYPE_REP_UnionTypeRep))
          tp_s.ImpUnion(tp.GetSet(pos_REP_UnionTypeRep_tps));
        else
          tp_s.Insert(tp);
      }
      if (rtp.Is(TAG_TYPE_REP_SeqTypeRep))
      {
        TYPE_REP_TypeRep tp (rtp.GetRecord(pos_REP_SeqTypeRep_elemtp));
        if (tp.Is(TAG_TYPE_REP_UnionTypeRep))
          tp_s.ImpUnion(tp.GetSet(pos_REP_UnionTypeRep_tps));
        else
          tp_s.Insert(tp);
      }
      switch (tp_s.Card()) {
        case 0: {
          return mk_REP_SeqTypeRep(rep_alltp);
        }
        case 1: {
          return mk_REP_SeqTypeRep(tp_s.GetElem());
        }
        default: {
          return mk_REP_SeqTypeRep(mk_REP_UnionTypeRep(tp_s));
        }
      }
    }
    case TAG_TYPE_AS_MapMergePattern: {
      TYPE_REP_TypeRep ltp (Pattern2TypeRep(pat.GetRecord(pos_AS_MapMergePattern_lp)));
      TYPE_REP_TypeRep rtp (Pattern2TypeRep(pat.GetRecord(pos_AS_MapMergePattern_rp)));
      SET<TYPE_REP_TypeRep> dtp_s; 
      SET<TYPE_REP_TypeRep> rtp_s; 
      if (ltp.Is(TAG_TYPE_REP_GeneralMapTypeRep))
      {
        TYPE_REP_TypeRep domtp (ltp.GetRecord(pos_REP_GeneralMapTypeRep_mapdom));
        TYPE_REP_TypeRep rngtp (ltp.GetRecord(pos_REP_GeneralMapTypeRep_maprng));
     
        if (domtp.Is(TAG_TYPE_REP_UnionTypeRep))
          dtp_s.ImpUnion(domtp.GetSet(pos_REP_UnionTypeRep_tps));
        else
          dtp_s.Insert(domtp);
        if (rngtp.Is(TAG_TYPE_REP_UnionTypeRep))
          rtp_s.ImpUnion(rngtp.GetSet(pos_REP_UnionTypeRep_tps));
        else
          rtp_s.Insert(rngtp);
      }
      if (rtp.Is(TAG_TYPE_REP_GeneralMapTypeRep))
      {
        TYPE_REP_TypeRep domtp (rtp.GetRecord(pos_REP_GeneralMapTypeRep_mapdom));
        TYPE_REP_TypeRep rngtp (rtp.GetRecord(pos_REP_GeneralMapTypeRep_maprng));
     
        if (domtp.Is(TAG_TYPE_REP_UnionTypeRep))
          dtp_s.ImpUnion(domtp.GetSet(pos_REP_UnionTypeRep_tps));
        else
          dtp_s.Insert(domtp);
        if (rngtp.Is(TAG_TYPE_REP_UnionTypeRep))
          rtp_s.ImpUnion(rngtp.GetSet(pos_REP_UnionTypeRep_tps));
        else
          rtp_s.Insert(rngtp);
      }
      TYPE_REP_TypeRep domtp;
      switch (dtp_s.Card()) {
        case 0: {
          domtp = rep_alltp;
          break;
        }
        case 1: {
          domtp = dtp_s.GetElem();
          break;
        }
        default: {
          domtp = mk_REP_UnionTypeRep(dtp_s);
          break;
        }
      }
      TYPE_REP_TypeRep rngtp;
      switch (rtp_s.Card()) {
        case 0: {
          rngtp = rep_alltp;
          break;
        }
        case 1: {
          rngtp = rtp_s.GetElem();
          break;
        }
        default: {
          rngtp = mk_REP_UnionTypeRep(rtp_s);
          break;
        }
      }
      return mk_REP_GeneralMapTypeRep(domtp, rngtp);
    }
    case TAG_TYPE_AS_RecordPattern: {
      const TYPE_AS_Name & tag (pat.GetRecord(pos_AS_RecordPattern_nm));
#ifdef VDMSL
      Generic tp (LookUpTypeName(tag));
#endif // VDMSL
#ifdef VDMPP
      Generic tp (LookUpTypeName(tag, false));
#endif // VDMPP
      if (tp.IsNil())
        return rep_alltp;
      else
        return tp;
    }
    case TAG_TYPE_AS_TuplePattern: {
      const SEQ<TYPE_AS_Pattern> & p_l (pat.GetSequence(pos_AS_TuplePattern_fields));
      SEQ<TYPE_REP_TypeRep> tp_l;
      size_t len_p_l = p_l.Length();
      for (size_t idx = 1; idx <= len_p_l; idx++)
        tp_l.ImpAppend(Pattern2TypeRep(p_l[idx]));
      return mk_REP_ProductTypeRep(tp_l);
    }
#ifdef VDMPP
    case TAG_TYPE_AS_ObjectPattern: {
      const TYPE_AS_Name & cls (pat.GetRecord(pos_AS_ObjectPattern_cls));
      return mk_REP_ObjRefTypeRep(cls);
    }
#endif // VDMPP
    default: {
      return rep_alltp;
    }
  }
}
#endif //!NOSS
