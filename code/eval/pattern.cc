/***
*  * WHAT
*  *    Pattern matching evaluation functions in interpreter.
*  * FILE
*  *    $Source: /mnt/cvsroot/toolbox/code/eval/pattern.cc,v $
*  * VERSION
*  *    $Revision: 1.77 $
*  * DATE
*  *    $Date: 2006/09/23 10:58:19 $
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
*  *    pattern.cc: Implementation of eval_pat.vdm 1.28
***/

#include "metaiv.h"
#include "AS.h"
#include "astaux.h"
#include "pattern.h"
#include "settings.h"
#include "rt_errmsg.h"
#include "compile.h"
#include "evaldef.h"
#include "evalaux.h"
#include "intconvquotes.h"
#include "libman.h"
#include "tbutils.h"
#include "val2x.h"
#include "statsem.h"
#include "evalexpr.h"

Nil PAT::nil;
SET<TYPE_SEM_BlkEnv> PAT::eset;

static Map cache;
/** PatternMatch part *******************************************************/
/** Reference : Doc. iptes-ifad-12-v1.0  ************************************/
/****************************************************************************/

// PatternMatch
// pat_p : STKM`Pattern
// val_v : SEM`VAL
// ==> set of SEM`BlkEnv
SET<TYPE_SEM_BlkEnv> PAT::PatternMatch (const TYPE_STKM_Pattern & pat_p, const TYPE_SEM_VAL & val_v)
{
  switch (pat_p.GetTag()) {
    case TAG_TYPE_STKM_PatternName:     { return MatchPatternName (pat_p, val_v); }
    case TAG_TYPE_STKM_MatchVal:        { return MatchMatchVal (pat_p, val_v); }
    case TAG_TYPE_STKM_SetEnumPattern:  { return MatchSetEnumPattern (pat_p, val_v); }
    case TAG_TYPE_STKM_SetUnionPattern: { return MatchSetUnionPattern (pat_p, val_v); }
    case TAG_TYPE_STKM_SeqEnumPattern:  { return MatchSeqEnumPattern (pat_p, val_v); }
    case TAG_TYPE_STKM_SeqConcPattern:  { return MatchSeqConcPattern (pat_p, val_v); }
    case TAG_TYPE_STKM_MapEnumPattern:  { return MatchMapEnumPattern (pat_p, val_v); }
    case TAG_TYPE_STKM_MapMergePattern: { return MatchMapMergePattern (pat_p, val_v); }
    case TAG_TYPE_STKM_MapletPattern:   { return MatchMapletPattern (pat_p, val_v); }
    case TAG_TYPE_STKM_TuplePattern:    { return MatchTuplePattern (pat_p, val_v); }
    case TAG_TYPE_STKM_RecordPattern:   { return MatchRecordPattern (pat_p, val_v); }

#ifdef VDMPP
    case TAG_TYPE_STKM_ObjectPattern:   { return MatchObjectPattern (pat_p, val_v); }
#endif // VDMPP
    default: {
      RTERR::Error (L"PatternMatch", RTERR_PATTERN_UNKNOWN, Nil(), Nil(), Sequence());
      return eset; // Dummy return.
    }
  }
}

// MatchPatternName
// PAT : STKM`PatternName
// val_v : SEM`VAL
// ==> set of SEM`BlkEnv
SET<TYPE_SEM_BlkEnv> PAT::MatchPatternName(const TYPE_STKM_PatternName & pat, const TYPE_SEM_VAL & val_v)
{
  const Generic & tp (pat.GetField (pos_STKM_PatternName_tp));
  if (!tp.IsNil() && !theState().RealSubType(val_v, tp, true)) {
    return eset;
  }
  const Generic & id (pat.GetField (pos_STKM_PatternName_nm));
  return SET<TYPE_SEM_BlkEnv>().Insert (
    id.IsNil () ? AUX::MkEmptyBlkEnv (sem_read_only) : AUX::MkBlkEnv (id, val_v, tp, sem_read_only));
}

// MatchMatchVal
// PAT : STKM`MatchVal
// val_v : SEM`VAL
// ==> set of SEM`BlkEnv
SET<TYPE_SEM_BlkEnv> PAT::MatchMatchVal(const TYPE_STKM_MatchVal & pat, const TYPE_SEM_VAL & val_v)
{
  const TYPE_SEM_VAL & val (pat.GetRecord(pos_STKM_MatchVal_val));
  if (val.Is(TAG_TYPE_SEM_CompExplFN)) {
    const SEQ<TYPE_SEM_ExplFN> & fl = val.GetSequence(pos_SEM_CompExplFN_fl);
    if (fl.Length() == 1 ) {
      const TYPE_SEM_ExplFN & fn (fl[1]);
      if (fn.GetField(pos_SEM_ExplFN_fnName) == ASTAUX::MkNameFromVoid ()) {
        const TYPE_AS_Name & modName (fn.GetRecord(pos_SEM_ExplFN_modName));
        const Generic & instr (fn.GetField(pos_SEM_ExplFN_instr));
        const TYPE_SEM_BlkEnv & env (fn.GetRecord(pos_SEM_ExplFN_env));
        const TYPE_STKM_SubProgram & prog (theCompiler().GetProgram(modName, instr));
#ifdef VDMPP
        theStackMachine().PushClNmCurObj(modName, modName);
#endif // VDMPP
        theStackMachine().PushBlkEnv(env);
        Tuple res (theStackMachine().EvalAuxProgram(prog, SEQ<Char>(L"Evaluate Matchval"), false));
        theStackMachine().PopBlkEnv();
#ifdef VDMPP
        theStackMachine().PopClNmCurObj();
#endif // VDMPP
        const TYPE_STKM_EvaluationState & eval_state (res.GetRecord(1));
        if (eval_state.Is(TAG_TYPE_STKM_Success)) {
          if (res.GetRecord(2) == val_v) {
            return SET<TYPE_SEM_BlkEnv>().Insert(AUX::MkEmptyBlkEnv(sem_read_only));
          }
        }
        return eset;
      }
    }
  }
  return (val == val_v ? SET<TYPE_SEM_BlkEnv>().Insert(AUX::MkEmptyBlkEnv(sem_read_only)) : eset);
}

// MatchSetEnumPattern
// PAT : STKM`SetEnumPattern
// val_v : SEM`VAL
// ==> set of SEM`BlkEnv
SET<TYPE_SEM_BlkEnv> PAT::MatchSetEnumPattern (const TYPE_STKM_SetEnumPattern & pat,
                                               const TYPE_SEM_VAL & val_v)
{
  if (val_v.Is(TAG_TYPE_SEM_SET)) {
    const SEQ<TYPE_STKM_Pattern> & elems_lp_ (pat.GetSequence (pos_STKM_SetEnumPattern_els));
    const Set & val_sv_ (val_v.GetSet(pos_SEM_SET_v));
    if (val_sv_.Card () == elems_lp_.Length ()) {
// 20140301 -->
      SEQ<TYPE_STKM_Pattern> elems_lp;
      Set val_sv (val_sv_);
      size_t len_elems_lp_ = elems_lp_.Length();
      for (size_t idx = 1; idx <= len_elems_lp_; idx++) {
        const TYPE_STKM_Pattern & p (elems_lp_[idx]);
        if (p.Is(TAG_TYPE_STKM_MatchVal) &&
            p.GetRecord(pos_STKM_MatchVal_val).Is(TAG_TYPE_SEM_CompExplFN)) {
          const TYPE_SEM_VAL & val (p.GetRecord(pos_STKM_MatchVal_val));
          if (val_sv.InSet(val)) {
            val_sv.RemElem(val);
          }
          else {
            if (val.Is(TAG_TYPE_SEM_ExplFN) && (val.GetField(pos_SEM_ExplFN_fnName) == ASTAUX::MkNameFromVoid ())) {
              elems_lp.ImpAppend(p);
            }
            else {
              return eset;
            }
          }
        }
        else {
          elems_lp.ImpAppend(p);
        }
      }

// <-- 20140301
      switch(val_sv.Card()) {
        case 0: {
          //return eset;
          return mk_set(AUX::MkEmptyBlkEnv (sem_read_only));
        }
        case 1: {
          return PatternMatch(elems_lp[1], val_sv.GetElem());
        }
        default: {
//          SET<Sequence> perm_slv (AUX::AUX_Permute (AUX::ValSetToSeq (val_sv)));
          SET<Sequence> perm_slv (val_sv.ToSequence().Permute());
          SET<TYPE_SEM_BlkEnv> res_s;
          Generic tmp_lv;
          for (bool bb = perm_slv.First (tmp_lv); bb; bb = perm_slv.Next (tmp_lv))
            res_s.ImpUnion (MatchLists (elems_lp, tmp_lv));
          return res_s;
        }
      }
    }
    else {
      return eset;
    }
  }
  else {
    return eset;
  }
}

#define POWERLIMIT 15 

// MatchSetUnionPattern
// PAT : STKM`SetUnionPattern
// val_v : SEM`VAL
// ==> set of SEM`BlkEnv
SET<TYPE_SEM_BlkEnv> PAT::MatchSetUnionPattern (const TYPE_STKM_SetUnionPattern & pat, const TYPE_SEM_VAL & val_v)
{
  if (val_v.Is(TAG_TYPE_SEM_SET)) {
// 20140227 -->
    //const TYPE_STKM_Pattern & lp_p (pat.GetRecord(pos_STKM_SetUnionPattern_lp));
    //const TYPE_STKM_Pattern & rp_p (pat.GetRecord(pos_STKM_SetUnionPattern_rp));
    
    bool swap_pat (!pat.GetRecord(pos_STKM_SetUnionPattern_lp).Is(TAG_TYPE_STKM_SetEnumPattern) ||
                   pat.GetRecord(pos_STKM_SetUnionPattern_rp).Is(TAG_TYPE_STKM_SetEnumPattern));

    const TYPE_STKM_Pattern & lp_p (swap_pat ? pat.GetRecord(pos_STKM_SetUnionPattern_rp)
                                             : pat.GetRecord(pos_STKM_SetUnionPattern_lp));
    const TYPE_STKM_Pattern & rp_p (swap_pat ? pat.GetRecord(pos_STKM_SetUnionPattern_lp)
                                             : pat.GetRecord(pos_STKM_SetUnionPattern_rp));
// <-- 20140227

    switch (lp_p.GetTag()) {
      case TAG_TYPE_STKM_SetEnumPattern:
      case TAG_TYPE_STKM_SetUnionPattern:
      case TAG_TYPE_STKM_MatchVal:
      case TAG_TYPE_STKM_PatternName: {
        switch (rp_p.GetTag()) {
          case TAG_TYPE_STKM_SetEnumPattern:
          case TAG_TYPE_STKM_SetUnionPattern:
          case TAG_TYPE_STKM_MatchVal:
          case TAG_TYPE_STKM_PatternName: { break; }
          default: { return eset; break; }
        }
        break;
      }
      default: { return eset; break; }
    }

    //const Set & val_sv (val_v.GetSet(pos_SEM_SET_v));
    Set val_sv (val_v.GetSet(pos_SEM_SET_v));

    bool lp_is_set_enum_pattern = lp_p.Is(TAG_TYPE_STKM_SetEnumPattern);
    bool rp_is_set_enum_pattern = rp_p.Is(TAG_TYPE_STKM_SetEnumPattern);
    bool empty_not_allowed = (( val_sv.Card() > 1 ) && !lp_is_set_enum_pattern && !rp_is_set_enum_pattern);
    int lp_pat_length = (lp_is_set_enum_pattern ? lp_p.GetSequence(pos_STKM_SetEnumPattern_els).Length() : 0); 
    int rp_pat_length = (rp_is_set_enum_pattern ? rp_p.GetSequence(pos_STKM_SetEnumPattern_els).Length() : 0); 

    SET<TYPE_SEM_BlkEnv> envres_sl;
    if (!empty_not_allowed) {
      SET<TYPE_SEM_BlkEnv> envl_s (PatternMatch (lp_p, mk_SEM_SET(Set())));
      if (!envl_s.IsEmpty()) {
        SET<TYPE_SEM_BlkEnv> envr_s (PatternMatch (rp_p, mk_SEM_SET(val_sv)));
        if (!envr_s.IsEmpty()) {
          SET<Set> envlr_s (envl_s.DirectProduct(envr_s));
          Generic envlr;
          for (bool cc = envlr_s.First(envlr); cc; cc = envlr_s.Next(envlr)) {
            envres_sl.ImpUnion (AUX::DistribCombineBlkEnv(envlr));
          }
        }
      }
    }
    Set already; // power set
    already.Insert(SET<TYPE_SEM_VAL>()); // power {} -> {{}}
    Generic e;
    for (bool bb = val_sv.First(e); bb; bb = val_sv.Next(e)) {
      Set s (already); // part of power set
      Generic es;
      for (bool cc = s.First(es); cc; cc = s.Next(es)) {
        SET<TYPE_SEM_VAL> setl_sv (Set(es).Insert(e));
        if (!(lp_is_set_enum_pattern && (setl_sv.Card() >= lp_pat_length))) {
          already.Insert(setl_sv);
        }

        if (!lp_is_set_enum_pattern || (setl_sv.Card() == lp_pat_length)) {

          SET<TYPE_SEM_VAL> setr_sv (val_sv.Diff(setl_sv));

          if ((!empty_not_allowed || !setr_sv.IsEmpty()) &&
              (!rp_is_set_enum_pattern || (rp_pat_length == setr_sv.Card()))) {

            SET<TYPE_SEM_BlkEnv> envl_s (PatternMatch (lp_p, mk_SEM_SET(setl_sv)));
            if (!envl_s.IsEmpty()) {
              SET<TYPE_SEM_BlkEnv> envr_s (PatternMatch (rp_p, mk_SEM_SET(setr_sv)));
              if (!envr_s.IsEmpty()) {
                SET<Set> envlr_s (envl_s.DirectProduct(envr_s));
                Generic envlr;
                for (bool cc = envlr_s.First(envlr); cc; cc = envlr_s.Next(envlr)) {
                  envres_sl.ImpUnion (AUX::DistribCombineBlkEnv(envlr));
                }
              }
            }
          }
        }
      }
    }
    return envres_sl;
  }
  else {
    return eset;
  }
}

// MatchSeqEnumPattern
// PAT : STKM`SeqEnumPattern
// val_v : SEM`VAL
// ==> set of SEM`BlkEnv
SET<TYPE_SEM_BlkEnv> PAT::MatchSeqEnumPattern (const TYPE_STKM_SeqEnumPattern & pat, const TYPE_SEM_VAL & val_v)
{
  if (val_v.Is(TAG_TYPE_SEM_SEQ)) {
    return MatchLists (pat.GetSequence (pos_STKM_SeqEnumPattern_els), val_v.GetSequence(pos_SEM_SEQ_v));
  }
  else {
    return eset;
  }
}

// MatchSeqConcPattern
// PAT : STKM`SeqConcPattern
// val_v : SEM`VAL
// ==> set of SEM`BlkEnv
SET<TYPE_SEM_BlkEnv> PAT::MatchSeqConcPattern (const TYPE_STKM_SeqConcPattern & pat, const TYPE_SEM_VAL & val_v)
{
  if (val_v.Is(TAG_TYPE_SEM_SEQ)) {
    const TYPE_STKM_Pattern & lp_p (pat.GetRecord(pos_STKM_SeqConcPattern_lp));
    const TYPE_STKM_Pattern & rp_p (pat.GetRecord(pos_STKM_SeqConcPattern_rp));

    switch (lp_p.GetTag()) {
      case TAG_TYPE_STKM_SeqEnumPattern:
      case TAG_TYPE_STKM_SeqConcPattern:
      case TAG_TYPE_STKM_MatchVal:
      case TAG_TYPE_STKM_PatternName: {
        switch (rp_p.GetTag()) {
          case TAG_TYPE_STKM_SeqEnumPattern:
          case TAG_TYPE_STKM_SeqConcPattern:
          case TAG_TYPE_STKM_MatchVal:
          case TAG_TYPE_STKM_PatternName: { break; }
          default: { return eset; break; }
        }
        break;
      }
      default: { return eset; break; }
    }

    bool lp_is_seq_enum_pattern = lp_p.Is(TAG_TYPE_STKM_SeqEnumPattern);
    bool rp_is_seq_enum_pattern = rp_p.Is(TAG_TYPE_STKM_SeqEnumPattern);
    bool lp_is_match_val_pattern = lp_p.Is(TAG_TYPE_STKM_MatchVal) &&
                                   lp_p.GetRecord(pos_STKM_MatchVal_val).Is(TAG_TYPE_SEM_SEQ);
    bool rp_is_match_val_pattern = rp_p.Is(TAG_TYPE_STKM_MatchVal) &&
                                   rp_p.GetRecord(pos_STKM_MatchVal_val).Is(TAG_TYPE_SEM_SEQ);

    const Sequence & val_lv (val_v.GetSequence(pos_SEM_SEQ_v));

    int len_val_lv = val_lv.Length();
    int from = 1;
    int to = len_val_lv;
    if(lp_is_seq_enum_pattern) {
      int len_lp_p = lp_p.GetSequence(pos_STKM_SeqEnumPattern_els).Length(); 
      // [x,y,z]^?
      if (len_val_lv < len_lp_p) {
        return eset;
      }
      if (rp_p.Is(TAG_TYPE_STKM_SeqEnumPattern)) {
        if (len_val_lv != (len_lp_p + rp_p.GetSequence(pos_STKM_SeqEnumPattern_els).Length())) {
          return eset;
        }
      }
      from = len_lp_p;
      to = from;
    }
    else if(rp_is_seq_enum_pattern) {
      int len_rp_p = rp_p.GetSequence(pos_STKM_SeqEnumPattern_els).Length(); 
      // ?^[x,y,z]
      if (len_val_lv < len_rp_p) {
        return eset;
      }
      from = len_val_lv - len_rp_p;
      to = from;
    }
    else if (lp_is_match_val_pattern) {
      int len_lp_p = lp_p.GetRecord(pos_STKM_MatchVal_val).GetSequence(pos_SEM_SEQ_v).Length(); 
      if (len_val_lv < len_lp_p) {
        return eset;
      }
      from = len_lp_p;
      to = from;
    }
    else if (rp_is_match_val_pattern) {
      int len_rp_p = rp_p.GetRecord(pos_STKM_MatchVal_val).GetSequence(pos_SEM_SEQ_v).Length(); 
      if (len_val_lv < len_rp_p) {
        return eset;
      }
      from = len_val_lv - len_rp_p;
      to = from;
    }
    else
    {
      // ?^?
      // !lp_p.Is(TAG_TYPE_STKM_SeqEnumPattern) && !rp_p.Is(TAG_TYPE_STKM_SeqEnumPattern)
      if (len_val_lv > 1 )
      {
//        from = 1; // default
        to = len_val_lv - 1;
      }
//      else // len_val_lv <= 1
//      {
//        from = 1; // default
//        to = len_val_lv;  // 0 or 1 // default
//      }
    }

    SET<TYPE_SEM_BlkEnv> envres_sl;
    if (rp_is_seq_enum_pattern || rp_is_match_val_pattern) {
      for (int i = from; i <= to; i++) {
        SET<TYPE_SEM_BlkEnv> envr_sl (PatternMatch (rp_p, mk_SEM_SEQ(val_lv.SubSequence(i + 1, len_val_lv ))));
        if (!envr_sl.IsEmpty()) {
          SET<TYPE_SEM_BlkEnv> envl_sl (PatternMatch (lp_p, mk_SEM_SEQ(val_lv.SubSequence(1, i))));
          if (!envl_sl.IsEmpty()) {
            SET<Set> envlr_s (envl_sl.DirectProduct(envr_sl));
            Generic envlr;
            for (bool cc = envlr_s.First(envlr); cc; cc = envlr_s.Next(envlr)) {
              envres_sl.ImpUnion (AUX::DistribCombineBlkEnv(envlr));
            }
          }
        }
      }
    }
    else {
      if ( len_val_lv == 0 ) {
        SET<TYPE_SEM_BlkEnv> envl_sl (PatternMatch (lp_p, mk_SEM_SEQ(Sequence())));
        SET<TYPE_SEM_BlkEnv> envr_sl (PatternMatch (rp_p, mk_SEM_SEQ(Sequence())));
        SET<Set> envlr_s (envl_sl.DirectProduct(envr_sl));
        Generic envlr;
        for (bool cc = envlr_s.First(envlr); cc; cc = envlr_s.Next(envlr)) {
          envres_sl.ImpUnion (AUX::DistribCombineBlkEnv(envlr));
        }
      }
      else {
        for (int i = from; i <= to; i++) {
          SET<TYPE_SEM_BlkEnv> envl_sl (PatternMatch (lp_p, mk_SEM_SEQ(val_lv.SubSequence(1, i))));
          if (!envl_sl.IsEmpty()) {
            SET<TYPE_SEM_BlkEnv> envr_sl (PatternMatch (rp_p, mk_SEM_SEQ(val_lv.SubSequence(i + 1, len_val_lv ))));
            if (!envr_sl.IsEmpty()) {
              SET<Set> envlr_s (envl_sl.DirectProduct(envr_sl));
              Generic envlr;
              for (bool cc = envlr_s.First(envlr); cc; cc = envlr_s.Next(envlr)) {
                envres_sl.ImpUnion (AUX::DistribCombineBlkEnv(envlr));
              }
            }
          }
        }
      }
    }
    return envres_sl;
  }
  else {
    return eset;
  }
}

// MatchMapEnumPattern
// PAT : STKM`MapEnumPattern
// val_v : SEM`VAL
// ==> set of SEM`BlkEnv
SET<TYPE_SEM_BlkEnv> PAT::MatchMapEnumPattern (const TYPE_STKM_MapEnumPattern & pat, const TYPE_SEM_VAL & val_v)
{
  if (val_v.Is(TAG_TYPE_SEM_MAP)) {
    const SEQ<TYPE_STKM_MapletPattern> & elems_lp (pat.GetSequence (pos_STKM_MapEnumPattern_mls));
    const Map & val_mv (val_v.GetMap(pos_SEM_MAP_v));
    if (val_mv.Size () == elems_lp.Length ()) {
      switch(val_mv.Size()) {
        case 0: {
          //return eset;
          return mk_set(AUX::MkEmptyBlkEnv (sem_read_only));
        }
        case 1: {
          //return PatternMatch(elems_lp[1], mk_SEM_MAP(val_mv));
          return PatternMatch(elems_lp[1], val_v);
        }
        default: {
//          SET<Sequence> perm_slv (AUX::AUX_Permute (AUX::ValSetToSeq (val_sv)));
          Set dom_val_mv (val_mv.Dom());
          SET<Sequence> perm_slv (dom_val_mv.ToSequence().Permute());
          SET<TYPE_SEM_BlkEnv> res_s;
          Generic tmp_lv;
          for (bool bb = perm_slv.First (tmp_lv); bb; bb = perm_slv.Next (tmp_lv)) {
            Sequence l (tmp_lv);
            SEQ<TYPE_SEM_MAP> tmp_ml;
            size_t len_l = l.Length();
            for (size_t i = 1; i<= len_l; i++) {
              tmp_ml.ImpAppend(mk_SEM_MAP(Map().Insert(l[i], val_mv[l[i]])));
            }
            res_s.ImpUnion (MatchLists (elems_lp, tmp_ml));
          }
          return (res_s);
        }
      }
    }
    else {
      return eset;
    }
  }
  else {
    return eset;
  }
}

// MatchMapletPattern
// PAT : STKM`MapltPattern
// val_v : SEM`VAL
// ==> set of SEM`BlkEnv
SET<TYPE_SEM_BlkEnv> PAT::MatchMapletPattern (const TYPE_STKM_MapletPattern & pat, const TYPE_SEM_VAL & val_v)
{
  if (val_v.Is(TAG_TYPE_SEM_MAP)) {
    const Map & val_mv (val_v.GetMap(pos_SEM_MAP_v));
    switch (val_mv.Size()) {
      case 0: { return eset; }
      case 1: {
        TYPE_SEM_VAL e (val_mv.Dom().GetElem());
        return MatchLists(mk_sequence(pat.GetRecord(pos_AS_MapletPattern_dp),
                                      pat.GetRecord(pos_AS_MapletPattern_rp)),
                          mk_sequence(e, val_mv[e]));
      }
      default: { return eset; }
    } 
  }
  else {
    return eset;
  }
}

// MatchMapMergePattern
// PAT : STKM`MapMergePattern
// val_v : SEM`VAL
// ==> set of SEM`BlkEnv
SET<TYPE_SEM_BlkEnv> PAT::MatchMapMergePattern (const TYPE_STKM_MapMergePattern & pat, const TYPE_SEM_VAL & val_v)
{
  if (val_v.Is(TAG_TYPE_SEM_MAP)) {
// 20140227 -->
    //const TYPE_STKM_Pattern & lp_p (pat.GetRecord(pos_STKM_MapMergePattern_lp));
    //const TYPE_STKM_Pattern & rp_p (pat.GetRecord(pos_STKM_MapMergePattern_rp));
    bool swap_pat (!pat.GetRecord(pos_STKM_MapMergePattern_lp).Is(TAG_TYPE_STKM_MapEnumPattern) ||
                   pat.GetRecord(pos_STKM_MapMergePattern_rp).Is(TAG_TYPE_STKM_MapEnumPattern));

    const TYPE_STKM_Pattern & lp_p (swap_pat ? pat.GetRecord(pos_STKM_MapMergePattern_rp)
                                             : pat.GetRecord(pos_STKM_MapMergePattern_lp));
    const TYPE_STKM_Pattern & rp_p (swap_pat ? pat.GetRecord(pos_STKM_MapMergePattern_lp)
                                             : pat.GetRecord(pos_STKM_MapMergePattern_rp));
// <-- 20140227

    switch (lp_p.GetTag()) {
      case TAG_TYPE_STKM_MapEnumPattern:
      case TAG_TYPE_STKM_MapMergePattern:
      case TAG_TYPE_STKM_MatchVal:
      case TAG_TYPE_STKM_PatternName: {
        switch (rp_p.GetTag()) {
          case TAG_TYPE_STKM_MapEnumPattern:
          case TAG_TYPE_STKM_MapMergePattern:
          case TAG_TYPE_STKM_MatchVal:
          case TAG_TYPE_STKM_PatternName: { break; }
          default: { return eset; break; }
        }
        break;
      }
      default: { return eset; break; }
    }

    const Map & val_mv (val_v.GetMap(pos_SEM_MAP_v));

    bool lp_is_map_enum_pattern = lp_p.Is(TAG_TYPE_STKM_MapEnumPattern);
    bool rp_is_map_enum_pattern = rp_p.Is(TAG_TYPE_STKM_MapEnumPattern);
    bool empty_not_allowed = (( val_mv.Size() > 1 ) && !lp_is_map_enum_pattern && !rp_is_map_enum_pattern);
    int lp_pat_length = (lp_is_map_enum_pattern ? lp_p.GetSequence(pos_STKM_MapEnumPattern_mls).Length() : 0); 
    int rp_pat_length = (rp_is_map_enum_pattern ? rp_p.GetSequence(pos_STKM_MapEnumPattern_mls).Length() : 0); 

    Set val_sv (val_mv.Dom());
    SET<TYPE_SEM_BlkEnv> envres_sl;
    if (!empty_not_allowed) {
      SET<TYPE_SEM_BlkEnv> envl_s (PatternMatch (lp_p, mk_SEM_MAP(Map())));
      if (!envl_s.IsEmpty()) {
        SET<TYPE_SEM_BlkEnv> envr_s (PatternMatch (rp_p, mk_SEM_MAP(val_mv)));
        if (!envr_s.IsEmpty()) {
          SET<Set> envlr_s (envl_s.DirectProduct(envr_s));
          Generic envlr;
          for (bool cc = envlr_s.First(envlr); cc; cc = envlr_s.Next(envlr)) {
            envres_sl.ImpUnion (AUX::DistribCombineBlkEnv(envlr));
          }
        }
      }
    }
    Set already;
    already.Insert(SET<TYPE_SEM_VAL>());
    Generic e;
    for (bool bb = val_sv.First(e); bb; bb = val_sv.Next(e)) {
      Set s (already); //  part of power set
      Generic es;
      for (bool cc = s.First(es); cc; cc = s.Next(es)) {
        SET<TYPE_SEM_VAL> setl_sv (Set(es).Insert(e));
        if (!(lp_is_map_enum_pattern && (setl_sv.Card() >= lp_pat_length))) {
          already.Insert(setl_sv);
        }

        if (!lp_is_map_enum_pattern || (setl_sv.Card() == lp_pat_length)) {

          SET<TYPE_SEM_VAL> setr_sv (val_sv.Diff(setl_sv));
  
          if ((!empty_not_allowed || !setr_sv.IsEmpty())  &&
              (!rp_is_map_enum_pattern || (rp_pat_length == setr_sv.Card()))) {

            SET<TYPE_SEM_BlkEnv> envl_s (PatternMatch (lp_p, mk_SEM_MAP(val_mv.DomRestrictedTo(setl_sv))));
            if (!envl_s.IsEmpty()) {
              SET<TYPE_SEM_BlkEnv> envr_s (PatternMatch (rp_p, mk_SEM_MAP(val_mv.DomRestrictedTo(setr_sv))));
              if (!envr_s.IsEmpty()) {
                SET<Set> envlr_s (envl_s.DirectProduct(envr_s));
                Generic envlr;
                for (bool cc = envlr_s.First(envlr); cc; cc = envlr_s.Next(envlr)) {
                  envres_sl.ImpUnion (AUX::DistribCombineBlkEnv(envlr));
                }
              }
            }
          }
        }
      }
    }
    return envres_sl;
  }
  else {
    return eset;
  }
}
// MatchTuplePattern
// PAT : STKM`TuplePattern
// val_v : SEM`VAL
// ==> set of SEM`BlkEnv
SET<TYPE_SEM_BlkEnv> PAT::MatchTuplePattern (const TYPE_STKM_TuplePattern & pat, const TYPE_SEM_VAL & val_v)
{
  if (val_v.Is(TAG_TYPE_SEM_TUPLE)) {
    const SEQ<TYPE_STKM_Pattern> & fields_lp (pat.GetSequence (pos_STKM_TuplePattern_fields));
    const SEQ<TYPE_SEM_VAL> & val_lv (val_v.GetSequence(pos_SEM_TUPLE_v));
    if ( fields_lp.Length() == val_lv.Length() ) {
      return MatchLists (fields_lp,val_lv);
    }
    else  {
      return eset;
    }
  }
  else {
    return eset;
  }
}

// MatchRecordPattern
// PAT : STKM`RecordPattern
// val_v : SEM`VAL
// ==> set of SEM`BlkEnv
SET<TYPE_SEM_BlkEnv> PAT::MatchRecordPattern (const TYPE_STKM_RecordPattern & pat, const TYPE_SEM_VAL & val_v)
{
  if (val_v.Is(TAG_TYPE_DYNSEM_SEM_REC)) {
    const TYPE_AS_Name & ptag (pat.GetRecord  (pos_STKM_RecordPattern_nm));
    const TYPE_AS_Name & vtag (val_v.GetRecord(pos_DYNSEM_SEM_SemRecord_tag));
//    Tuple etn_v (AUX::ExtractTagName(vtag));
    Tuple etn_v (theState().ExtractTagNameCached(vtag));
//    Tuple etn_p (AUX::ExtractTagName(ptag));
    Tuple etn_p (theState().ExtractTagNameCached(ptag));

    if ( ! (etn_v.GetBool(2) && etn_p.GetBool(2))) { // if not (v_isit and p_isiif)
      RTERR::Error(L"MatchRecordPattern", RTERR_TAG_UNKNOWN, Nil(), Nil(), Sequence());
      return eset;
    }

    if (etn_v.GetRecord(1) == etn_p.GetRecord(1)) { // if vtagname = ptagname
      const SEQ<TYPE_STKM_Pattern> & fields_lp (pat.GetSequence (pos_STKM_RecordPattern_fields));

      if (fields_lp.IsEmpty()) {
        return SET<TYPE_SEM_BlkEnv>().Insert(AUX::MkEmptyBlkEnv(sem_read_only));
      }
      else {
        // NOTE: SEM`REC implimentation is different from spec.
        const Record & value (val_v.GetRecord(pos_DYNSEM_SEM_SemRecord_value));
        SEQ<TYPE_SEM_VAL> val_lv (value.GetFields());
        size_t len_fields_lp = fields_lp.Length();
        for (size_t i = 1; i <= len_fields_lp; i++) {
          const TYPE_STKM_Pattern & p (fields_lp[i]);
          if (value.IsDontCare(i) && !p.Is(TAG_TYPE_STKM_PatternName)) {
            RTERR::Error(L"MatchRecordPattern", RTERR_DC_NOT_PATTERN_NAME, Nil(), Nil(), Sequence());
            return eset;
          }
        }
        return MatchLists (fields_lp,val_lv);
      }
    }
    else {
      return eset;
    }
  }
  else {
    return eset;
  }
}

#ifdef VDMPP
// MatchObjectPattern
// PAT : STKM`ObjectPattern
// val_v : SEM`VAL
// ==> set of SEM`BlkEnv
SET<TYPE_SEM_BlkEnv> PAT::MatchObjectPattern (const TYPE_STKM_ObjectPattern & pat, const TYPE_SEM_VAL & val_v)
{
  const TYPE_AS_Name & cls (pat.GetRecord(pos_STKM_ObjectPattern_cls));
  if (val_v.Is(TAG_TYPE_SEM_OBJ_uRef) && theState().IsAClass(cls)) {
    const TYPE_AS_Name & objnm (val_v.GetRecord(pos_SEM_OBJ_uRef_tp));
    if ((objnm == cls) || theState().IsSubClass(objnm, cls)) {
      const SEQ<TYPE_STKM_Pattern> & fields_lp (pat.GetSequence (pos_STKM_ObjectPattern_fields));
      SEQ<TYPE_STKM_Pattern> pat_lp;
      SEQ<TYPE_SEM_VAL> val_lv;
   
      size_t flen = fields_lp.Length();
      for (size_t i = 1; i <= flen; i++) {
        const TYPE_STKM_FieldPattern & fp(fields_lp[i]);
        pat_lp.ImpAppend(fp.GetRecord(pos_STKM_FieldPattern_pat));
        val_lv.ImpAppend(EXPR::EvalFieldOBJRefSelect(val_v, fp.GetRecord(pos_STKM_FieldPattern_nm)));
      }
      return MatchLists (pat_lp, val_lv);
    }
  }
  return eset;
}
#endif // VDMPP

// MatchLists
// els_lp : seq of STKM`Pattern
// val_lv : seq of SEM`VAL
// ==> set of SEM`BlkEnv
SET<TYPE_SEM_BlkEnv> PAT::MatchLists (const SEQ<TYPE_STKM_Pattern> & els_lp, const SEQ<TYPE_SEM_VAL> & val_lv)
{
  if (els_lp.Length () == val_lv.Length ()) {
    SET<Set> env_ss;
    size_t len_els_lp = els_lp.Length();
    for (size_t i = 1; i <= len_els_lp; i++) {
      const TYPE_STKM_Pattern & pat (els_lp[i]);
      if (!pat.Is(TAG_TYPE_STKM_PatternName) || !pat.GetField(pos_STKM_PatternName_nm).IsNil()) {
        SET<TYPE_SEM_BlkEnv> env_s (PatternMatch (pat, val_lv[i]));
        if (env_s.IsEmpty ()) {
          return eset;
        }
        else {
          env_ss.Insert (env_s);
        }
      }
    }
    switch (env_ss.Card()) {
      case 0: {
        return SET<TYPE_SEM_BlkEnv>().Insert(AUX::MkEmptyBlkEnv(sem_read_only));
      }
      case 1: {
        return env_ss.GetElem();
      }
      default: {
        SET<Set> perm_s (env_ss.DistrDirectProduct()); // set of set of SEM`BlkEnv
        switch(perm_s.Card()) {
          case 0: {
            return eset;
          }
          case 1: {
            return AUX::DistribCombineBlkEnv(perm_s.GetElem()); // set of SEM`BlkEnv
          }
          default: {
            SET<TYPE_SEM_BlkEnv> res_s;
            Generic l;
            for (bool bb = perm_s.First (l); bb; bb = perm_s.Next (l)) {
              res_s.ImpUnion(AUX::DistribCombineBlkEnv(l));
            }
            return res_s;
          }
        }
      }
    }
  }
  else {
    return eset;
  }
}

// EvalMultBindSeq
// pat_lp : seq of STKM`Pattern
// seq_lv : seq of SEM`VAL
// partition : PARTITION
// ==> set of SEM`BlkEnv
Set PAT::EvalMultBindSeq (const SEQ<TYPE_STKM_Pattern> & pat_lp,
                          const SEQ<TYPE_SEM_VAL> & seq_lv,
                          const Int & partition)
{
  Set s (EvalMultBindSeqAll(pat_lp, seq_lv, partition));
  if (DO_PARTITION == partition.GetValue()) {
    return s.GetElem(); // set of SEM`BlkEnv
  }
  else {
    return s;  // set of SEM`BlkEnv
  }
}

// PatternMatchSetVal
// pat : STKM`Pattern
// val : SEM`VAL
// ==> set of set of SEM`BlkEnv
Set PAT::PatternMatchSetVal(const TYPE_STKM_Pattern & pat, const TYPE_SEM_VAL & val)
{
  if (val.Is(TAG_TYPE_SEM_SET)) {
    SET<TYPE_SEM_VAL> val_s (val.GetSet(pos_SEM_SET_v));
    SET<Set> env_ss; // set of set of SEM`BlkEnv
    Generic v;
    for (bool bb = val_s.First(v); bb; bb = val_s.Next(v)) {
      Set env_s (PatternMatch(pat, v));
      if (!env_s.IsEmpty()) {
        env_ss.Insert (env_s);
      }
    }
    return env_ss;
  }
  else {
    RTERR::Error (L"PatternMatchSetVal", RTERR_SET_EXPECTED, val, Nil(), Sequence());
  }
  return eset; // Dummy return.
}

// EvalMultBindSeqAll
// pat_lp : seq of STKM`Pattern
// seq_lv : seq of SEM`VAL
// partition : PARTITION
// ==> set of SEM`BlkEnv | set of set of SEM`BlkEnv
Set PAT::EvalMultBindSeqAll (const SEQ<TYPE_STKM_Pattern> & pat_lp,
                             const SEQ<TYPE_SEM_VAL> & seq_lv,
                             const Int & partition)
{
  switch (seq_lv.Length()) {
    case 0: {
      if (DO_PARTITION == partition.GetValue()) {
        return mk_set(eset); // set of set of SEM`BlkEnv
      }
      else {
        return eset;  // set of SEM`BlkEnv
      }
    }
    case 1: {
      SET<Set> env_ss (PatternMatchSetVal(pat_lp[1], seq_lv[1])); // set of set of SEM`BlkEnv
      if (DO_PARTITION == partition.GetValue()) {
        return Partition (env_ss); // set of set of SEM`BlkEnv
      }
      else {
        return Collapse (env_ss);  // set of SEM`BlkEnv
      }
    }
    default: {
      SET<Set> env_ss (PatternMatchSetVal(pat_lp[1], seq_lv[1])); // set of set of SEM`BlkEnv
      size_t len_seq_lv = seq_lv.Length();
      for (size_t idx = 2; (idx <= len_seq_lv) && !env_ss.IsEmpty(); idx++) {
        SET<Set> next_env_ss (PatternMatchSetVal(pat_lp[idx], seq_lv[idx])); // set of set of SEM`BlkEnv
        SET<Set> env_pair_ss (env_ss.DirectProduct(next_env_ss)); // set of set of set of SEM`BlkEnv
        env_ss.Clear();
        Generic pair;
        for (bool bb = env_pair_ss.First(pair); bb; bb = env_pair_ss.Next(pair)) {
          SET<Set> ss = ((Set)pair).DistrDirectProduct();// set of set of SEM`BlkEnv
          Set env;
          Generic env_s;
          for (bool cc = ss.First(env_s); cc; cc = ss.Next(env_s)) {
            env.ImpUnion(AUX::DistribCombineBlkEnv(env_s));
          }
          if (!env.IsEmpty()) {
            env_ss.Insert(env);
          }
        }
      }
      if (DO_PARTITION == partition.GetValue()) {
        return Partition (env_ss); // set of set of SEM`BlkEnv
      }
      else {
        return Collapse (env_ss);  // set of SEM`BlkEnv
      }
    }
  }
}

// ConstructFN
// clmodName : AS`Name
// fndef : AS`FnDef
// ==> SEM`BlkEnv * map AS`Name to (AS`Expr | AS`NoBody)
//Tuple PAT::ConstructFN (const TYPE_AS_FnDef & fndef)
Tuple PAT::ConstructFN (const TYPE_AS_Name & clmodName, const TYPE_AS_FnDef & fndef)
{
  switch(fndef.GetTag()) {
    case TAG_TYPE_AS_ExplFnDef:    { return ConstructExplFN(clmodName, fndef);    break; }
    case TAG_TYPE_AS_ExtExplFnDef: { return ConstructExtExplFN(clmodName, fndef); break; }
    case TAG_TYPE_AS_ImplFnDef:    { return ConstructImplFN(clmodName, fndef);    break; }
    default: { return Tuple(); } // dummy
  }
}

// ConstructExplFN
// clmodName : AS`Name
// fndef : AS`ExplFnDef
// ==> SEM`BlkEnv * map AS`Name to (AS`Expr | AS`NoBody)
Tuple PAT::ConstructExplFN (const TYPE_AS_Name & clmodName, const TYPE_AS_FnDef & fndef)
{
  const TYPE_AS_Name & nm (fndef.GetRecord(pos_AS_ExplFnDef_nm));
  const SEQ<TYPE_AS_TypeVar> & tpparms (fndef.GetSequence(pos_AS_ExplFnDef_tpparms));
  const TYPE_AS_FnBody & body (fndef.GetRecord(pos_AS_ExplFnDef_body));
  const Generic & fnpre (fndef.GetField(pos_AS_ExplFnDef_fnpre));   // [Expr]
  const Generic & fnpost (fndef.GetField(pos_AS_ExplFnDef_fnpost)); // [Expr]

  TYPE_SEM_BlkEnv benv (AUX::MkBlkEnv(nm, DEF::TransFN(clmodName, fndef), nil, sem_read_only));

  // map AS`name to (AS`Expr | AS`NoBody)
  Map b_m;
  b_m.Insert(nm, body.GetField(pos_AS_FnBody_body)); // AS`Expr | AS`NoBody

  if (!fnpre.IsNil())
    b_m.Insert(AUX::PreName(nm), fnpre);

  if (!fnpost.IsNil())
    b_m.Insert(AUX::PostName(nm), fnpost);

  // map AS`Name to ((SEM`CompExplFN | SEM`ExplPOLY) * AS`Access)
  Map mp (tpparms.IsEmpty () ? DEF::CreatePrePostFns (clmodName, fndef, nm)
                             : DEF::CreatePolyPrePostFns (clmodName, fndef, nm));

  SET<TYPE_SEM_BlkEnv> benv_s (mk_set(benv));
  Set dom_mp (mp.Dom());
  Generic name;
  for (bool bb = dom_mp.First(name); bb; bb = dom_mp.Next(name)) {
    benv_s.Insert(AUX::MkBlkEnv (name, mp[name], nil, sem_read_only));
  }
  return mk_(AUX::DistribCombineBlkEnv(benv_s).GetElem(), b_m);
  //return mk_(AUX::DistribCombineBlkEnv(benv_s), b_m);
}

// ConstructExtExplFN
// clmodName : AS`Name
// fndef : AS`ExplFnDef
// ==> SEM`BlkEnv * map AS`Name to (AS`Expr | AS`NoBody)
Tuple PAT::ConstructExtExplFN (const TYPE_AS_Name & clmodName, const TYPE_AS_FnDef & fndef)
{
  const TYPE_AS_Name & nm (fndef.GetRecord(pos_AS_ExtExplFnDef_nm));
  const SEQ<TYPE_AS_TypeVar> & tpparms (fndef.GetSequence(pos_AS_ExtExplFnDef_params));
  const TYPE_AS_FnBody & body (fndef.GetRecord(pos_AS_ExtExplFnDef_body));
  const Generic & fnpre (fndef.GetField(pos_AS_ExtExplFnDef_fnpre));
  const Generic & fnpost (fndef.GetField(pos_AS_ExtExplFnDef_fnpost));

  TYPE_SEM_BlkEnv benv (AUX::MkBlkEnv(nm, DEF::TransFN(clmodName, fndef), nil, sem_read_only));

  Map b_m;
  b_m.Insert(nm, body.GetField(pos_AS_FnBody_body));
  
  if (!fnpre.IsNil())
    b_m.Insert(AUX::PreName(nm), fnpre);
  
  if (!fnpost.IsNil())
    b_m.Insert(AUX::PostName(nm), fnpost);

  Map mp (tpparms.IsEmpty() ? DEF::CreatePrePostFns(clmodName, fndef, nm)
                            : DEF::CreatePolyPrePostFns(clmodName, fndef, nm));

  SET<TYPE_SEM_BlkEnv> benv_s (mk_set(benv));
  Set dom_mp (mp.Dom());
  Generic name;
  for (bool bb = dom_mp.First(name); bb; bb = dom_mp.Next(name)) {
    benv_s.Insert(AUX::MkBlkEnv (name, mp[name], nil, sem_read_only));
  }
  return mk_(AUX::DistribCombineBlkEnv(benv_s).GetElem(), b_m);
}

// ConstructImplFN
// clmodName : AS`Name
// fndef : AS`ExplFnDef
// ==> SEM`BlkEnv * map AS`Name to (AS`Expr | AS`NoBody)
Tuple PAT::ConstructImplFN (const TYPE_AS_Name & clmodName, const TYPE_AS_FnDef & fndef)
{
  const TYPE_AS_Name & nm (fndef.GetRecord(pos_AS_ImplFnDef_nm));
  const SEQ<TYPE_AS_TypeVar> & tpparms (fndef.GetSequence(pos_AS_ImplFnDef_params));
  const Generic & fnpre (fndef.GetField(pos_AS_ImplFnDef_fnpre));
  const Generic & fnpost (fndef.GetField(pos_AS_ImplFnDef_fnpost));
  
  TYPE_SEM_BlkEnv benv (AUX::MkBlkEnv(nm, DEF::TransFN(clmodName, fndef), nil, sem_read_only));
  
  Map b_m;
  b_m.Insert(nm, TYPE_AS_UndefinedExpr().Init(NilContextId));
  
  if (!fnpre.IsNil())
    b_m.Insert(AUX::PreName(nm), fnpre);
  b_m.Insert(AUX::PostName(nm), fnpost);

  // map AS`Name to ((SEM`CompExplFN * AS`Access) | (SEM`ExplPOLY * AS`Access))
  Map mp (tpparms.IsEmpty() ? DEF::CreatePrePostFns (clmodName, fndef, nm)
                            : DEF::CreatePolyPrePostFns (clmodName, fndef, nm));
  
  SET<TYPE_SEM_BlkEnv> benv_s (mk_set(benv));
  Set dom_mp (mp.Dom());
  Generic name;
  for (bool bb = dom_mp.First(name); bb; bb = dom_mp.Next(name)) {
    benv_s.Insert(AUX::MkBlkEnv (name, mp[name], nil, sem_read_only));
  }
  return mk_(AUX::DistribCombineBlkEnv(benv_s).GetElem(), b_m);
}

// SelPattern
// bind : AS`Bind
// ==> AS`Pattern
TYPE_AS_Pattern PAT::SelPattern (const TYPE_AS_Bind & bind)
{
  switch(bind.GetTag()) {
    case TAG_TYPE_AS_SetBind:  { return bind.GetRecord(pos_AS_SetBind_pat); }
    case TAG_TYPE_AS_TypeBind: { return bind.GetRecord(pos_AS_TypeBind_pat); }
    default:                   { return TYPE_AS_Pattern(); } // dummy
  }
}

// GetExpr
// pat_p : AS`Pattern
// ==> AS`Expr
TYPE_AS_Expr PAT::GetExpr (const TYPE_AS_Pattern & pat_p)
{
  switch(pat_p.GetTag()) {
    case TAG_TYPE_AS_PatternName: {
      if ( pat_p.GetField(pos_AS_PatternName_nm).IsNil () )
        return GetExpr(DoCarePattern (pat_p, ASTAUX::MkNameFromId(SEQ<Char>(L"1"), NilContextId)));
      else {
        return pat_p.GetRecord(pos_AS_PatternName_nm);
      }
    }
    case TAG_TYPE_AS_MatchVal: {
      return pat_p.GetRecord(pos_AS_MatchVal_val);
    }
    case TAG_TYPE_AS_SetEnumPattern: {
      const SEQ<TYPE_AS_Pattern> & els (pat_p.GetSequence(pos_AS_SetEnumPattern_Elems));
      SEQ<TYPE_AS_Expr> e_l;
      size_t len = els.Length();
      for (size_t i = 1; i <= len; i++)
        e_l.ImpAppend (GetExpr (els[i]));

      return TYPE_AS_SetEnumerationExpr().Init(e_l, pat_p.GetInt(pos_AS_SetEnumPattern_cid));
    }
    case TAG_TYPE_AS_SetUnionPattern: {
      return TYPE_AS_BinaryExpr().Init(GetExpr (pat_p.GetRecord(pos_AS_SetUnionPattern_lp)),
                                       Int(SETUNION),
                                       GetExpr (pat_p.GetRecord(pos_AS_SetUnionPattern_rp)),
                                       pat_p.GetInt(pos_AS_SetUnionPattern_cid));
    }
    case TAG_TYPE_AS_SeqEnumPattern: {
      const SEQ<TYPE_AS_Pattern> & els (pat_p.GetSequence(pos_AS_SeqEnumPattern_els));
      SEQ<TYPE_AS_Expr> e_l;
      size_t len = els.Length();
      for (size_t i = 1; i <= len; i++)
        e_l.ImpAppend (GetExpr (els[i]));

      return TYPE_AS_SeqEnumerationExpr().Init(e_l, pat_p.GetInt(pos_AS_SeqEnumPattern_cid));
    }
    case TAG_TYPE_AS_SeqConcPattern: {
      return TYPE_AS_BinaryExpr().Init(GetExpr (pat_p.GetRecord(pos_AS_SeqConcPattern_lp)),
                                       Int(SEQCONC),
                                       GetExpr (pat_p.GetRecord(pos_AS_SeqConcPattern_rp)),
                                       pat_p.GetInt(pos_AS_SeqConcPattern_cid));
    }
    case TAG_TYPE_AS_TuplePattern: {
      const SEQ<TYPE_AS_Pattern> & fields (pat_p.GetSequence(pos_AS_TuplePattern_fields));
      SEQ<TYPE_AS_Expr> e_l;
      size_t len_fields = fields.Length();
      for (size_t i = 1; i <= len_fields; i++)
        e_l.ImpAppend (GetExpr (fields[i]));

      return TYPE_AS_TupleConstructorExpr().Init(e_l, pat_p.GetInt(pos_AS_TuplePattern_cid));
    }
    case TAG_TYPE_AS_RecordPattern: {
      const SEQ<TYPE_AS_Pattern> & fields (pat_p.GetSequence(pos_AS_RecordPattern_fields));
      SEQ<TYPE_AS_Expr> e_l;
      size_t len_fields = fields.Length();
      for (size_t i = 1; i <= len_fields; i++)
        e_l.ImpAppend (GetExpr (fields[i]));

      return TYPE_AS_RecordConstructorExpr().Init(pat_p.GetField(pos_AS_RecordPattern_nm),
                                                  e_l,
                                                  pat_p.GetInt(pos_AS_RecordPattern_cid));
    }
    default: {
      RTERR::Error (L"PatternMatch", RTERR_PATTERN_UNKNOWN, Nil(), Nil(), Sequence());
      return Record(); // Dummy return.
    }
  }
}

// Partition
// env_ls : set of (set of SEM`BlkEnv)
// ==> set of (set of SEM`BlkEnv)
SET<Set> PAT::Partition (const SET<Set> & env_ls)
{
  // { {e1,e2,e3}, {e4,e5,e6} } -> { {e1,e4}, {e1,e5}, {e1,e6}, {e2,e4}, {e2,e5}, {e2,e6}, {e3,e4}, {e3,e5}, {e3,e6} }
  SET<Set> env_ss (env_ls.Diff(mk_set(eset)));
  if (!env_ss.IsEmpty())
    return env_ss.DistrDirectProduct();
  else
    return SET<Set>().Insert (eset);
}

// Collapse
// env_ls : set of (set of SEM`BlkEnv)
// ==> set of SEM`BlkEnv
SET<TYPE_SEM_BlkEnv> PAT::Collapse (const SET<Set> & env_ls)
{
  // { {e1,e2,e3}, {e4,e5,e6} } -> { e1,e2,e3,e4,e5,e6 }
  return env_ls.DUnion();
}

// NewBase
// id : AS`Name
// n : nat
// -> AS`Name
TYPE_AS_Name PAT::NewBase (const TYPE_AS_Name & nm, int n)
{
  SEQ<Char> str (n <= 0 ? SEQ<Char>(L"0") : SEQ<Char>(Int(n).ascii()));
  return ASTAUX::MkNameFromId(ASTAUX::GetFirstId(nm).ImpConc(str).ImpConc(SEQ<Char>(L"_")),
                              nm.GetInt(pos_AS_Name_cid) );
}

// DoCarePattern
// pat_p : AS`PatternBind
// id_base : AS`Name
// ==> AS`PatternBind
TYPE_AS_PatternBind PAT::DoCarePattern (const TYPE_AS_PatternBind & pat_p, const TYPE_AS_Name & id_base)
{
  switch(pat_p.GetTag()) {
    case TAG_TYPE_AS_PatternName: {
      TYPE_AS_PatternName res (pat_p);
      if (res.GetField(pos_AS_PatternName_nm).IsNil ()) {
        res.SetField(pos_AS_PatternName_nm, id_base);
      }
      return res;
    }
    case TAG_TYPE_AS_MatchVal: {
      return pat_p;
    }
    case TAG_TYPE_AS_SetEnumPattern: {
      const SEQ<TYPE_AS_Pattern> & els (pat_p.GetSequence(pos_AS_SetEnumPattern_Elems));
      SEQ<TYPE_AS_Pattern> e_l;
      size_t len_els = els.Length();
      for (size_t i = 1; i <= len_els; i++)
        e_l.ImpAppend (DoCarePattern (els[i], NewBase (id_base, i)));

      TYPE_AS_SetEnumPattern res_v (pat_p);
      res_v.set_Elems(e_l);
      return res_v;
    }
    case TAG_TYPE_AS_SetUnionPattern: {
      TYPE_AS_SetUnionPattern res_v (pat_p);
      res_v.set_lp(DoCarePattern (pat_p.GetRecord(pos_AS_SetUnionPattern_lp), NewBase (id_base, 1)));
      res_v.set_rp(DoCarePattern (pat_p.GetRecord(pos_AS_SetUnionPattern_rp), NewBase (id_base, 2)));
      return res_v;
    }
    case TAG_TYPE_AS_SeqEnumPattern: {
      const SEQ<TYPE_AS_Pattern> & els (pat_p.GetSequence(pos_AS_SeqEnumPattern_els));
      SEQ<TYPE_AS_Pattern> e_l;
      size_t len_els = els.Length();
      for (size_t i = 1; i <= len_els; i++)
        e_l.ImpAppend (DoCarePattern (els[i], NewBase (id_base, i)));

      TYPE_AS_SeqEnumPattern res_v (pat_p);
      res_v.set_els(e_l);
      return res_v;
    }
    case TAG_TYPE_AS_SeqConcPattern: {
      TYPE_AS_SeqConcPattern res_v (pat_p);
      res_v.set_lp(DoCarePattern (pat_p.GetRecord(pos_AS_SeqConcPattern_lp), NewBase (id_base, 1)));
      res_v.set_rp(DoCarePattern (pat_p.GetRecord(pos_AS_SeqConcPattern_rp), NewBase (id_base, 2)));
      return res_v;
    }
    case TAG_TYPE_AS_MapEnumPattern: {
      const SEQ<TYPE_AS_MapletPattern> & mls (pat_p.GetSequence(pos_AS_MapEnumPattern_mls));
      SEQ<TYPE_AS_MapletPattern> m_l;
      size_t len_mls = mls.Length();
      for (size_t i = 1; i <= len_mls; i++)
        m_l.ImpAppend (DoCarePattern (mls[i], NewBase (id_base, i)));

      TYPE_AS_MapEnumPattern res_v (pat_p);
      res_v.SetField(pos_AS_MapEnumPattern_mls, m_l);
      return res_v;
    }
    case TAG_TYPE_AS_MapletPattern: {
      TYPE_AS_MapletPattern res_v (pat_p);
      res_v.set_dp(DoCarePattern (pat_p.GetRecord(pos_AS_MapletPattern_dp), NewBase (id_base, 1)));
      res_v.set_rp(DoCarePattern (pat_p.GetRecord(pos_AS_MapletPattern_rp), NewBase (id_base, 2)));
      return res_v;
    }
    case TAG_TYPE_AS_MapMergePattern: {
      TYPE_AS_MapMergePattern res_v (pat_p);
      res_v.set_lp(DoCarePattern (pat_p.GetRecord(pos_AS_MapMergePattern_lp), NewBase (id_base, 1)));
      res_v.set_rp(DoCarePattern (pat_p.GetRecord(pos_AS_MapMergePattern_rp), NewBase (id_base, 2)));
      return res_v;
    }
    case TAG_TYPE_AS_TuplePattern: {
      const SEQ<TYPE_AS_Pattern> & p_l (pat_p.GetSequence(pos_AS_TuplePattern_fields));
      SEQ<TYPE_AS_Pattern> e_l;
      size_t len_p_l = p_l.Length();
      for (size_t i = 1; i <= len_p_l; i++)
        e_l.ImpAppend (DoCarePattern (p_l[i], NewBase (id_base, i)));

      TYPE_AS_TuplePattern res_v (pat_p);
      res_v.SetField(pos_AS_TuplePattern_fields, e_l);
      return res_v;
    }
    case TAG_TYPE_AS_RecordPattern: {
      const SEQ<TYPE_AS_Pattern> & p_l (pat_p.GetSequence(pos_AS_RecordPattern_fields));
      SEQ<TYPE_AS_Pattern> e_l;
      size_t len_p_l = p_l.Length();
      for (size_t i = 1; i <= len_p_l; i++)
        e_l.ImpAppend (DoCarePattern (p_l[i], NewBase (id_base, i)));

      TYPE_AS_RecordPattern res_v (pat_p);
      res_v.SetField(pos_AS_RecordPattern_fields, e_l);
      return res_v;
    }
#ifdef VDMPP
    case TAG_TYPE_AS_ObjectPattern: {
      const SEQ<TYPE_AS_FieldPattern> & fields (pat_p.GetSequence(pos_AS_ObjectPattern_fields));
      SEQ<TYPE_AS_FieldPattern> new_fields;
      size_t len_fields = fields.Length();
      for (size_t i = 1; i <= len_fields; i++)
        new_fields.ImpAppend (DoCarePattern (fields[i], NewBase (id_base, i)));

      TYPE_AS_ObjectPattern res_v (pat_p);
      res_v.SetField(pos_AS_ObjectPattern_fields, new_fields);
      return res_v;
    }
    case TAG_TYPE_AS_FieldPattern: {
      TYPE_AS_FieldPattern res_v (pat_p);
      res_v.set_pat(DoCarePattern (pat_p.GetRecord(pos_AS_FieldPattern_pat), NewBase (id_base, 1)));
      return res_v;
    }
#endif // VDMPP
    case TAG_TYPE_AS_SetBind: {
      TYPE_AS_SetBind res_v (pat_p);
      res_v.set_pat(DoCarePattern (pat_p.GetRecord(pos_AS_SetBind_pat), NewBase (id_base, 1)));
      return res_v;
    }
    case TAG_TYPE_AS_TypeBind: {
      TYPE_AS_TypeBind res_v (pat_p);
      res_v.set_pat(DoCarePattern (pat_p.GetRecord(pos_AS_TypeBind_pat), NewBase (id_base, 1)));
      return res_v;
    }
    default: {
      RTERR::Error (L"PatternMatch", RTERR_PATTERN_UNKNOWN, Nil(), Nil(), Sequence());
      return TYPE_AS_PatternBind(); // Dummy return.
    }
  }
}

#ifdef VDMSL
// StripPatternTypeInfo
Record PAT::StripPatternTypeInfo (const Record & pat)
{
  switch(pat.GetTag()) {
    case TAG_TYPE_AS_Name: {
      TYPE_AS_Name res (pat);
      res.SetField (pos_AS_Name_cid, NilContextId); // strip type info
      return res;
    }
    case TAG_TYPE_AS_OldName: {
      TYPE_AS_OldName res (pat);
      res.SetField (pos_AS_OldName_cid, NilContextId); // strip type info
      return res;
    }
    case TAG_TYPE_AS_TypeName: {
      TYPE_AS_TypeName res (pat);
      res.SetField(pos_AS_TypeName_name, StripPatternTypeInfo(pat.GetRecord(pos_AS_TypeName_name)));
      res.SetField(pos_AS_TypeName_cid, NilContextId);
      return res;
    }
    case TAG_TYPE_AS_TuplePattern: {
      const SEQ<TYPE_AS_Pattern> & p_l (pat.GetSequence(pos_AS_TuplePattern_fields));
      SEQ<TYPE_AS_Pattern> fields;
      size_t len_p_l = p_l.Length();
      for (size_t i = 1; i <= len_p_l; i++)
        fields.ImpAppend (StripPatternTypeInfo(p_l[i]));

      TYPE_AS_TuplePattern res (pat);
      res.SetField (pos_AS_TuplePattern_fields, fields);
      res.SetField (pos_AS_TuplePattern_cid, NilContextId); // strip type info
      return res;
    }
    case TAG_TYPE_AS_SeqEnumPattern: {
      const SEQ<TYPE_AS_Pattern> & p_l (pat.GetSequence(pos_AS_SeqEnumPattern_els));
      SEQ<TYPE_AS_Pattern> els;
      size_t len_p_l = p_l.Length();
      for (size_t i = 1; i <= len_p_l; i++)
        els.ImpAppend (StripPatternTypeInfo(p_l[i]));

      TYPE_AS_SeqEnumPattern res (pat);
      res.SetField (pos_AS_SeqEnumPattern_els, els);
      res.SetField (pos_AS_SeqEnumPattern_cid, NilContextId); // strip type info
      return res;
    }
    case TAG_TYPE_AS_SetEnumPattern: {
      const SEQ<TYPE_AS_Pattern> & p_l (pat.GetSequence(pos_AS_SetEnumPattern_Elems));
      SEQ<TYPE_AS_Pattern> elems;
      size_t len_p_l = p_l.Length();
      for (size_t i = 1; i <= len_p_l; i++)
        elems.ImpAppend (StripPatternTypeInfo(p_l[i]));

      TYPE_AS_SetEnumPattern res (pat);
      res.SetField (pos_AS_SetEnumPattern_Elems, elems);
      res.SetField (pos_AS_SetEnumPattern_cid, NilContextId); // strip type info
      return res;
    }
    case TAG_TYPE_AS_PatternName: {
      TYPE_AS_PatternName res (pat);
      if (!pat.GetField (pos_AS_PatternName_nm) .IsNil())
        res.SetField (pos_AS_PatternName_nm, StripPatternTypeInfo (pat.GetField (pos_AS_PatternName_nm)));
      res.SetField (pos_AS_PatternName_cid, NilContextId); // strip type info
      return res;
    }
    case TAG_TYPE_AS_RecordPattern: {
      const SEQ<TYPE_AS_Pattern> & p_l (pat.GetSequence(pos_AS_RecordPattern_fields));
      SEQ<TYPE_AS_Pattern> fields;
      size_t len_p_l = p_l.Length();
      for (size_t i = 1; i <= len_p_l; i++)
        fields.ImpAppend (StripPatternTypeInfo(p_l[i]));
      
      TYPE_AS_RecordPattern res (pat);
      res.SetField (pos_AS_RecordPattern_nm, StripPatternTypeInfo (pat.GetRecord(pos_AS_RecordPattern_nm)));
      res.SetField (pos_AS_RecordPattern_fields, fields);
      res.SetField (pos_AS_RecordPattern_cid, NilContextId); // strip type info
      return res;
    }
    case TAG_TYPE_AS_SetUnionPattern: {
      TYPE_AS_SetUnionPattern res (pat);
      res.SetField (pos_AS_SetUnionPattern_lp, StripPatternTypeInfo (pat.GetRecord(pos_AS_SetUnionPattern_lp)));
      res.SetField (pos_AS_SetUnionPattern_rp, StripPatternTypeInfo (pat.GetRecord(pos_AS_SetUnionPattern_rp)));
      res.SetField (pos_AS_SetUnionPattern_cid, NilContextId);
      return res;
    }
    case TAG_TYPE_AS_SeqConcPattern: {
      TYPE_AS_SeqConcPattern res (pat);
      res.SetField (pos_AS_SeqConcPattern_lp, StripPatternTypeInfo (pat.GetRecord(pos_AS_SeqConcPattern_lp)));
      res.SetField (pos_AS_SeqConcPattern_rp, StripPatternTypeInfo (pat.GetRecord(pos_AS_SeqConcPattern_rp)));
      res.SetField (pos_AS_SeqConcPattern_cid, NilContextId);
      return res;
    }
    default: {
      return pat;
    }
  }
}
#endif // VDMSL
