// {{{ Header

/***
*  * WHAT
*  *    Implementation of module CPAT
*  * COPYRIGHT
*  *    (C) Kyushu University
*  * SPECIFICATION VERSION
*  *    cpat.cc: Implementation of cpat.vdm 1.8
***/

// }}}
// {{{ includes
#include "compile.h"
#include "INSTRTP.h"
#include "astaux.h"
#include "asquotes.h"
#include "pattern.h"
#include "rt_errmsg.h"
#include "evalaux.h"
#include "libman.h"
// }}}

// {{{ CompileMultBindL
// CompileBind
// bind : AS`Bind
// part : PAT`PARTITION
// +> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileBind(const TYPE_AS_Bind & bind,
                                                const TYPE_PAT_PARTITION & part)
{
  switch(bind.GetTag()) {
    case TAG_TYPE_AS_TypeBind: {
      const TYPE_AS_Pattern & pat (bind.GetRecord(pos_AS_TypeBind_pat));
      const TYPE_AS_Type & tp     (pat.GetRecord(pos_AS_TypeBind_tp));

      TYPE_STKM_SubProgram sp;
      sp.ImpAppend(TYPE_INSTRTP_TPTOSET().Init(tp));
      sp.ImpConc(P2I(pat));
      sp.ImpAppend(TYPE_INSTRTP_MULTBINDL().Init(Int(1), part));
      return sp;
    }
    case TAG_TYPE_AS_SetBind: {
      const TYPE_AS_Pattern & pat (bind.GetRecord(pos_AS_SetBind_pat));
      const TYPE_AS_Expr & set_e  (bind.GetRecord(pos_AS_SetBind_Set));

      TYPE_STKM_SubProgram sp;
      sp.ImpConc(E2I(set_e));
      sp.ImpConc(P2I(pat));
      sp.ImpAppend(TYPE_INSTRTP_MULTBINDL().Init(Int(1), part));
      return sp;
    }
    case TAG_TYPE_AS_SeqBind: {
      const TYPE_AS_Pattern & pat (bind.GetRecord(pos_AS_SeqBind_pat));
      const TYPE_AS_Expr & seq_e  (bind.GetRecord(pos_AS_SeqBind_Seq));

      TYPE_STKM_SubProgram sp;
      sp.ImpConc(E2I(seq_e));
      sp.ImpConc(P2I(pat));
      sp.ImpAppend(TYPE_INSTRTP_SEQBIND());
      return sp;
    }
    default: {
      return TYPE_STKM_SubProgram();
    }
  }
}

// CompileMultBindL
// bind_l : AS`BindList
// part : PAT`PARTITION
// +> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileMultBindL(const SEQ<TYPE_AS_MultBind> & bind_l,
                                                     const TYPE_PAT_PARTITION & part)
{
  if (1 == bind_l.Length()) {
    const TYPE_AS_MultBind & mb (bind_l[1]);
    switch(mb.GetTag()) {
      case TAG_TYPE_AS_MultTypeBind: {
        const SEQ<TYPE_AS_Pattern> & pat_l (mb.GetSequence(pos_AS_MultTypeBind_pat));
        const TYPE_AS_Type & tp (mb.GetRecord(pos_AS_MultTypeBind_tp));

        size_t len_pat_l = pat_l.Length();
        if (1 == len_pat_l) {
          TYPE_STKM_SubProgram sp;
          sp.ImpAppend(TYPE_INSTRTP_TPTOSET().Init(tp));
          sp.ImpConc(P2I(pat_l[1]));
          sp.ImpAppend(TYPE_INSTRTP_MULTBINDL().Init(Int(1), part));
          return sp;
        }
        else {
          TYPE_STKM_SubProgram sp;
          TYPE_STKM_SubProgram sp_pat;
          sp.ImpAppend(TYPE_INSTRTP_TPTOSET().Init(tp));
          for (size_t j = 1; j <= len_pat_l; j++) {
            sp_pat.ImpConc(P2I(pat_l[j]));
            if (j > 1) {
              sp.ImpAppend(TYPE_INSTRTP_COPYVAL());
            }
          }
          sp.ImpConc(sp_pat);
          sp.ImpAppend(TYPE_INSTRTP_MULTBINDL().Init(Int(len_pat_l), part));
          return sp;
        }
      }
      case TAG_TYPE_AS_MultSetBind: {
        const SEQ<TYPE_AS_Pattern> & pat_l (mb.GetSequence(pos_AS_MultSetBind_pat));
        const TYPE_AS_Expr & set_e (mb.GetRecord(pos_AS_MultSetBind_Set));

        size_t len_pat_l = pat_l.Length();
        if (1 == len_pat_l) {
          TYPE_STKM_SubProgram sp;
          sp.ImpConc(E2I(set_e));
          sp.ImpConc(P2I(pat_l[1]));
          sp.ImpAppend(TYPE_INSTRTP_MULTBINDL().Init(Int(1), part));
          return sp;
        }
        else {
          TYPE_STKM_SubProgram sp;
          TYPE_STKM_SubProgram sp_pat;
          sp.ImpConc(E2I(set_e));
          for (size_t j = 1; j <= len_pat_l; j++) {
            sp_pat.ImpConc(P2I(pat_l[j]));
            if (j > 1) {
              sp.ImpAppend(TYPE_INSTRTP_COPYVAL());
            }
          }
          sp.ImpConc(sp_pat);
          sp.ImpAppend(TYPE_INSTRTP_MULTBINDL().Init(Int(len_pat_l), part));
          return sp;
        }
      }
      case TAG_TYPE_AS_MultSeqBind: {
        const SEQ<TYPE_AS_Pattern> & pat_l (mb.GetSequence(pos_AS_MultSeqBind_pat));

        TYPE_AS_Expr set_e (TYPE_AS_PrefixExpr().Init (Int(SEQELEMS),
                                                       mb.GetRecord(pos_AS_MultSeqBind_Seq),
                                                       mb.GetInt(pos_AS_MultSeqBind_cid)));
  
        size_t len_pat_l = pat_l.Length();
        if (1 == len_pat_l) {
          TYPE_STKM_SubProgram sp;
          sp.ImpConc(E2I(set_e));
          sp.ImpConc(P2I(pat_l[1]));
          sp.ImpAppend(TYPE_INSTRTP_MULTBINDL().Init(Int(1), part));
          return sp;
        }
        else {
          TYPE_STKM_SubProgram sp;
          TYPE_STKM_SubProgram sp_pat;
          sp.ImpConc(E2I(set_e));
          for (size_t j = 1; j <= len_pat_l; j++) {
            sp_pat.ImpConc(P2I(pat_l[j]));
            if (j > 1) {
              sp.ImpAppend(TYPE_INSTRTP_COPYVAL());
            }
          }
          sp.ImpConc(sp_pat);
          sp.ImpAppend(TYPE_INSTRTP_MULTBINDL().Init(Int(len_pat_l), part));
          return sp;
        }
      }
      default: {
        return TYPE_STKM_SubProgram(); // dummy
      }
    }
  }
  else {
    // Check if one of the bindings is a type bind
    size_t length = 0;

    TYPE_STKM_SubProgram sp_val;
    TYPE_STKM_SubProgram sp_pat;

    size_t len_bind_l = bind_l.Length();
    for (size_t i = 1; i <= len_bind_l; i++) {
      const TYPE_AS_MultBind & mb (bind_l[i]);
      switch(mb.GetTag()) {
        case TAG_TYPE_AS_MultTypeBind: {
          const SEQ<TYPE_AS_Pattern> & pat_l (mb.GetSequence(pos_AS_MultTypeBind_pat));
          const TYPE_AS_Type & tp (mb.GetRecord(pos_AS_MultTypeBind_tp));

          size_t len_pat_l = pat_l.Length();
          length += len_pat_l;
          sp_val.ImpAppend(TYPE_INSTRTP_TPTOSET().Init(tp));
          for (size_t j = 1; j <= len_pat_l; j++) {
            sp_pat.ImpConc(P2I(pat_l[j]));
            if (j > 1) {
              sp_val.ImpAppend(TYPE_INSTRTP_COPYVAL());
            }
          }
          break;
        }
        case TAG_TYPE_AS_MultSetBind: {
          const SEQ<TYPE_AS_Pattern> & pat_l (mb.GetSequence(pos_AS_MultSetBind_pat));
          const TYPE_AS_Expr & set_e (mb.GetRecord(pos_AS_MultSetBind_Set));

          size_t len_pat_l = pat_l.Length();
          length += len_pat_l;
          sp_val.ImpConc(E2I(set_e));
          for (size_t j = 1; j <= len_pat_l; j++) {
            sp_pat.ImpConc(P2I(pat_l[j]));
            if (j > 1) {
              sp_val.ImpAppend(TYPE_INSTRTP_COPYVAL());
            }
          }
          break;
        }
        case TAG_TYPE_AS_MultSeqBind: {
          const SEQ<TYPE_AS_Pattern> & pat_l (mb.GetSequence(pos_AS_MultSeqBind_pat));
          TYPE_AS_Expr set_e (TYPE_AS_PrefixExpr().Init (Int(SEQELEMS),
                                                         mb.GetRecord(pos_AS_MultSeqBind_Seq),
                                                         mb.GetInt(pos_AS_MultSeqBind_cid)));

          size_t len_pat_l = pat_l.Length();
          length += len_pat_l;
          sp_val.ImpConc(E2I(set_e));
          for (size_t j = 1; j <= len_pat_l; j++) {
            sp_pat.ImpConc(P2I(pat_l[j]));
            if (j > 1) {
              sp_val.ImpAppend(TYPE_INSTRTP_COPYVAL());
            }
          }
          break;
        }
      }
    }
    TYPE_STKM_SubProgram sp;
    sp.ImpConc(sp_val);
    sp.ImpConc(sp_pat);
    sp.ImpAppend(TYPE_INSTRTP_MULTBINDL().Init(Int(length), part));
    return sp;
  }
}
// }}}

// {{{ PB2I, P2I, PStack2I, PDirect2I
// PB2I
// pb : AS`PatternBind
// +> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::PB2I(const TYPE_AS_PatternBind & pb)
{
  switch(pb.GetTag()) {
    case TAG_TYPE_AS_TypeBind: {
      TYPE_STKM_SubProgram sp;
      sp.ImpAppend(TYPE_INSTRTP_DTC().Init(pb.GetRecord(pos_AS_TypeBind_tp)))
        .ImpConc(P2I(pb.GetRecord(pos_AS_TypeBind_pat)));
      return sp;
    }
    case TAG_TYPE_AS_SetBind: {
      TYPE_STKM_SubProgram sp;
      sp.ImpConc(E2I(pb.GetRecord(pos_AS_SetBind_Set)))
        .ImpAppend(TYPE_INSTRTP_DTCSET())
        .ImpConc(P2I(pb.GetRecord(pos_AS_SetBind_pat)));
      return sp;
    }
    case TAG_TYPE_AS_SeqBind: {
      TYPE_STKM_SubProgram sp;
      sp.ImpConc(E2I(pb.GetRecord(pos_AS_SeqBind_Seq)))
        .ImpAppend(TYPE_INSTRTP_DTCSEQ())
        .ImpConc(P2I(pb.GetRecord(pos_AS_SeqBind_pat)));
      return sp;
    }
    default: {
      return P2I(pb);
    }
  }
}

// P2I
// pat : AS`Pattern
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::P2I(const TYPE_AS_Pattern & pat)
{
  return (AnyMatchVals(pat) ? PStack2I(pat) : PDirect2I(pat));
}

// PStack2I
// pat : AS`Pattern
// +> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::PStack2I(const TYPE_AS_Pattern & pat)
{
  switch (pat.GetTag()) {
    case TAG_TYPE_AS_PatternName: {
      TYPE_STKM_PatternName p;
      p.Init(pat.GetField(pos_AS_PatternName_nm),
             pat.GetField(pos_AS_PatternName_tp));
      TYPE_STKM_SubProgram sp;
      sp.ImpAppend(TYPE_INSTRTP_PUSH().Init(p));
      return sp;
    }
    case TAG_TYPE_AS_MatchVal: {
      TYPE_STKM_SubProgram sp;
      sp.ImpConc(E2I(pat.GetRecord(pos_AS_MatchVal_val)))
        .ImpAppend(TYPE_INSTRTP_MATCHVAL());
      return sp;
    }
    case TAG_TYPE_AS_SetEnumPattern: {
      TYPE_STKM_SubProgram sp;
      const SEQ<TYPE_AS_Pattern> & elems (pat.GetSequence(pos_AS_SetEnumPattern_Elems));
      size_t len_elems = elems.Length();
      for (size_t idx = 1; idx <= len_elems; idx++) {
        sp.ImpConc(P2I(elems[idx]));
      }
      sp.ImpAppend(TYPE_INSTRTP_SETENUM().Init(Int(len_elems)));
      return sp;
    }
    case TAG_TYPE_AS_SeqEnumPattern: {
      TYPE_STKM_SubProgram sp;
      const SEQ<TYPE_AS_Pattern> & els (pat.GetSequence(pos_AS_SeqEnumPattern_els));
      size_t len_els = els.Length();
      for (size_t idx = 1; idx <= len_els; idx++) {
        sp.ImpConc(P2I(els[idx]));
      }
      sp.ImpAppend(TYPE_INSTRTP_SEQENUM().Init(Int(len_els)));
      return sp;
    }
    case TAG_TYPE_AS_MapEnumPattern: {
      TYPE_STKM_SubProgram sp;
      const SEQ<TYPE_AS_MapletPattern> & mls (pat.GetSequence(pos_AS_MapEnumPattern_mls));
      size_t len_mls = mls.Length();
      for (size_t idx = 1; idx <= len_mls; idx++) {
        const TYPE_AS_MapletPattern & mp (mls[idx]);
        sp.ImpConc(P2I(mp.GetRecord(pos_AS_MapletPattern_dp)))
          .ImpConc(P2I(mp.GetRecord(pos_AS_MapletPattern_rp)))
          .ImpAppend(TYPE_INSTRTP_MAPLET());
      }
      sp.ImpAppend(TYPE_INSTRTP_MAPENUM().Init(Int(len_mls)));
      return sp;
    }
    case TAG_TYPE_AS_SetUnionPattern: {
      TYPE_STKM_SubProgram sp;
      sp.ImpConc(P2I(pat.GetRecord(pos_AS_SetUnionPattern_lp)))
          .ImpConc(P2I(pat.GetRecord(pos_AS_SetUnionPattern_rp)))
          .ImpAppend(TYPE_INSTRTP_SETUNION());
      return sp;
    }
    case TAG_TYPE_AS_SeqConcPattern: {
      TYPE_STKM_SubProgram sp;
      sp.ImpConc(P2I(pat.GetRecord(pos_AS_SeqConcPattern_lp)))
        .ImpConc(P2I(pat.GetRecord(pos_AS_SeqConcPattern_rp)))
        .ImpAppend(TYPE_INSTRTP_SEQCONC());
      return sp;
    }
    case TAG_TYPE_AS_MapMergePattern: {
      TYPE_STKM_SubProgram sp;
      sp.ImpConc(P2I(pat.GetRecord(pos_AS_MapMergePattern_lp)))
        .ImpConc(P2I(pat.GetRecord(pos_AS_MapMergePattern_rp)))
        .ImpAppend(TYPE_INSTRTP_MAPMERGE());
      return sp;
    }
    case TAG_TYPE_AS_TuplePattern: {
      TYPE_STKM_SubProgram sp;
      const SEQ<TYPE_AS_Pattern> & fields (pat.GetSequence(pos_AS_TuplePattern_fields));
      size_t len_fields = fields.Length();
      for (size_t idx = 1; idx <= len_fields; idx++) {
        sp.ImpConc(P2I(fields[idx]));
      }
      sp.ImpAppend(TYPE_INSTRTP_TUPPATCONS().Init(Int(len_fields)));
      return sp;
    }
    case TAG_TYPE_AS_RecordPattern: {
      TYPE_STKM_SubProgram sp;
      const SEQ<TYPE_AS_Pattern> & fields (pat.GetSequence(pos_AS_RecordPattern_fields));
      size_t len_fields = fields.Length();
      for (size_t idx = 1; idx <= len_fields; idx++) {
        sp.ImpConc(P2I(fields[idx]));
      }
      sp.ImpAppend(TYPE_INSTRTP_RECPATCONS().Init(pat.GetRecord(pos_AS_RecordPattern_nm),
                                                  Int(len_fields)));
      return sp;
    }
#ifdef VDMPP
    case TAG_TYPE_AS_ObjectPattern: {
      TYPE_STKM_SubProgram sp;
      const SEQ<TYPE_AS_FieldPattern> & fields (pat.GetSequence(pos_AS_ObjectPattern_fields));
      size_t len_fields = fields.Length();
      for (size_t idx = 1; idx <= len_fields; idx++) {
        const TYPE_AS_FieldPattern & fp (fields[idx]);
        sp.ImpConc(P2I(fp.GetRecord(pos_AS_FieldPattern_pat)))
          .ImpAppend(TYPE_INSTRTP_FIELDPAT().Init(fp.GetRecord(pos_AS_FieldPattern_nm)));
      }
      sp.ImpAppend(TYPE_INSTRTP_OBJECTPAT().Init(pat.GetRecord(pos_AS_ObjectPattern_cls),
                                                 Int(len_fields)));
      return sp;
    }
#endif // VDMPP
    default: {
      // should never happen
      return TYPE_STKM_SubProgram();
    }
  }
}

// PDirect2I
// pat : AS`Pattern
// +> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::PDirect2I(const TYPE_AS_Pattern & pat)
{
  TYPE_STKM_SubProgram sp;
  sp.ImpAppend(TYPE_INSTRTP_PUSH().Init(P2P(pat)));
  return sp;
}
// }}}

// {{{ PL2PL, P2P
// PL2PL
// pl : seq of AS`Pattern
// +> seq of STKM`Pattern
SEQ<TYPE_STKM_Pattern> StackCompiler::PL2PL(const SEQ<TYPE_AS_Pattern> & pl)
{ 
  SEQ<TYPE_STKM_Pattern> res;
  size_t len_pl = pl.Length();
  for (size_t idx = 1; idx <= len_pl; idx++) {
    res.ImpAppend(P2P(pl[idx]));
  }
  return res;
}

// P2P
// pat : AS`Pattern
// +> STKM`Pattern
TYPE_STKM_Pattern StackCompiler::P2P(const TYPE_AS_Pattern & pat)
{
  switch (pat.GetTag()) {
    case TAG_TYPE_AS_PatternName: {
      return TYPE_STKM_PatternName().Init(pat.GetField(pos_AS_PatternName_nm),
                                          pat.GetField(pos_AS_PatternName_tp));
    }

    case TAG_TYPE_AS_MatchVal: {
      // TODO: if pat is expression, always match
      const TYPE_AS_Expr & val (pat.GetRecord(pos_AS_MatchVal_val));
      switch (val.GetTag()) {
        case TAG_TYPE_AS_BoolLit: {
          return TYPE_STKM_MatchVal().Init(val.GetBoolValue(pos_AS_BoolLit_val) ? sem_true : sem_false);
        }
        case TAG_TYPE_AS_NilLit: {
          return TYPE_STKM_MatchVal().Init(sem_nil);
        }
        case TAG_TYPE_AS_RealLit: {
          return TYPE_STKM_MatchVal().Init(TYPE_SEM_NUM().Init(val.GetReal(pos_AS_RealLit_val)));
        }
        case TAG_TYPE_AS_NumLit: {
          return TYPE_STKM_MatchVal().Init(TYPE_SEM_NUM().Init(val.GetReal(pos_AS_NumLit_val)));
        }
        case TAG_TYPE_AS_CharLit: {
          return TYPE_STKM_MatchVal().Init(TYPE_SEM_CHAR().Init(val.GetChar(pos_AS_CharLit_val)));
        }
        case TAG_TYPE_AS_TextLit: {
          const SEQ<Char> & elms (val.GetSequence(pos_AS_TextLit_val));
          size_t len_elms = elms.Length();
          SEQ<TYPE_SEM_VAL> res_elems;
          for (size_t i = 1; i <= len_elms; i++) {
            res_elems.ImpAppend(TYPE_SEM_CHAR().Init(elms[i]));
          }
          return TYPE_STKM_MatchVal().Init(TYPE_SEM_SEQ().Init(res_elems));
        }
        case TAG_TYPE_AS_QuoteLit: {
          return TYPE_STKM_MatchVal().Init(TYPE_SEM_QUOTE().Init(val.GetSequence(pos_AS_QuoteLit_val)));
        }
        default: {
          // TODO: PGL (06.07) This is not correct but a major update is needed to fix it.
          // match any value
          // return TYPE_STKM_PatternName().Init(Nil(), Nil(), cid);
          TYPE_AS_Name clmodName (GetClMod());
          TYPE_SEM_BlkEnv blkenv (AUX::MkEmptyBlkEnv(sem_read_only));
          TYPE_AS_TotalFnType lft;
          lft.Init(Sequence(), TYPE_AS_AllType().Init(NilContextId), NilContextId);

          TYPE_STKM_SubProgramId id (InsertProgram(clmodName, E2I(val)));

          TYPE_SEM_ExplFN efn;
          efn.Init(lft,
                   SEQ<type_dL>(),
                   id,
                   blkenv,
                   MAP<TYPE_AS_TypeVar,TYPE_AS_Type>(),
                   ASTAUX::MkNameFromVoid(),
                   clmodName,
                   Bool(false),
                   Nil(),
                   Int(PRIVATE_AS));
          return TYPE_STKM_MatchVal().Init(SemRec::CompFN(efn));
        }
      }
    }

    case TAG_TYPE_AS_SetEnumPattern: {
      return TYPE_STKM_SetEnumPattern().Init(PL2PL(pat.GetSequence(pos_AS_SetEnumPattern_Elems)));
    }

    case TAG_TYPE_AS_SetUnionPattern: {
      TYPE_STKM_Pattern sulp (P2P(pat.GetRecord(pos_AS_SetUnionPattern_lp)));
      TYPE_STKM_Pattern surp (P2P(pat.GetRecord(pos_AS_SetUnionPattern_rp)));
      if (surp.Is(TAG_TYPE_STKM_SetEnumPattern)) {
        switch (sulp.GetTag()) {
          case TAG_TYPE_STKM_SetEnumPattern: {
            SEQ<TYPE_STKM_Pattern> els (sulp.GetSequence(pos_STKM_SetEnumPattern_els));
            els.ImpConc(surp.GetSequence(pos_STKM_SetEnumPattern_els));
            return TYPE_STKM_SetEnumPattern().Init(els);
          }
          case TAG_TYPE_STKM_SetUnionPattern: {
            const TYPE_STKM_Pattern & suplp (sulp.GetRecord(pos_STKM_SetUnionPattern_lp));
            const TYPE_STKM_Pattern & suprp (sulp.GetRecord(pos_STKM_SetUnionPattern_rp));
            if (suprp.Is(TAG_TYPE_STKM_SetEnumPattern)) {
              SEQ<TYPE_STKM_Pattern> els (suprp.GetSequence(pos_STKM_SetEnumPattern_els));
              els.ImpConc(surp.GetSequence(pos_STKM_SetEnumPattern_els));
              return TYPE_STKM_SetUnionPattern ().Init(suplp, TYPE_STKM_SetEnumPattern().Init(els));
            }
            else if (suplp.Is(TAG_TYPE_STKM_SetEnumPattern)) {
              SEQ<TYPE_STKM_Pattern> els (suplp.GetSequence(pos_STKM_SetEnumPattern_els));
              els.ImpConc(surp.GetSequence(pos_STKM_SetEnumPattern_els));
              return TYPE_STKM_SetUnionPattern ().Init(suprp, TYPE_STKM_SetEnumPattern().Init(els));
            }
            else {
              return TYPE_STKM_SetUnionPattern().Init(sulp,surp);
            }
          }
          default: {
            return TYPE_STKM_SetUnionPattern().Init(sulp,surp);
          }
        }
      }
      else {
        return TYPE_STKM_SetUnionPattern().Init(sulp,surp);
      }
    }

    case TAG_TYPE_AS_SeqEnumPattern: {
      return TYPE_STKM_SeqEnumPattern().Init(PL2PL(pat.GetSequence(pos_AS_SeqEnumPattern_els)));
    }

    case TAG_TYPE_AS_SeqConcPattern: {
      TYPE_STKM_Pattern sclp (P2P(pat.GetRecord(pos_AS_SeqConcPattern_lp)));
      TYPE_STKM_Pattern scrp (P2P(pat.GetRecord(pos_AS_SeqConcPattern_rp)));
      if (scrp.Is(TAG_TYPE_STKM_SeqEnumPattern)) {
        switch (sclp.GetTag()) {
          case TAG_TYPE_STKM_SeqEnumPattern: {
            SEQ<TYPE_STKM_Pattern> els (sclp.GetSequence(pos_STKM_SeqEnumPattern_els));
            els.ImpConc(scrp.GetSequence(pos_AS_SeqEnumPattern_els));
            return TYPE_STKM_SeqEnumPattern().Init(els);
          }
          case TAG_TYPE_STKM_SeqConcPattern: {
            const TYPE_STKM_Pattern & scprp (sclp.GetRecord(pos_STKM_SeqConcPattern_rp));
            if (scprp.Is(TAG_TYPE_STKM_SeqEnumPattern)) {
              SEQ<TYPE_STKM_Pattern> els (scprp.GetSequence(pos_STKM_SeqEnumPattern_els));
              els.ImpConc(scrp.GetSequence(pos_AS_SeqEnumPattern_els));
              return TYPE_STKM_SeqConcPattern ().Init(sclp.GetRecord(pos_STKM_SeqConcPattern_lp),
                                                      TYPE_STKM_SeqEnumPattern().Init(els));
            }
            else {
              return TYPE_STKM_SeqConcPattern().Init(sclp,scrp);
            }
          }
          default: {
            return TYPE_STKM_SeqConcPattern().Init(sclp,scrp);
          }
        }
      }
      else {
        return TYPE_STKM_SeqConcPattern().Init(sclp,scrp);
      }
    }

    case TAG_TYPE_AS_MapEnumPattern: {
      const SEQ<TYPE_AS_MapletPattern> & mls (pat.GetSequence(pos_AS_MapEnumPattern_mls));
      SEQ<TYPE_STKM_Pattern> els;
      size_t len_mls = mls.Length();
      for (size_t i = 1; i <= len_mls; i++) {
        const TYPE_AS_MapletPattern & mp(mls[i]);
        TYPE_STKM_Pattern p (TYPE_STKM_MapletPattern().Init(P2P(mp.GetRecord(pos_AS_MapletPattern_dp)),
                                                            P2P(mp.GetRecord(pos_AS_MapletPattern_rp))));
        els.ImpAppend(p);
      }
      return TYPE_STKM_MapEnumPattern().Init(els);
    }

    case TAG_TYPE_AS_MapMergePattern: {
      TYPE_STKM_Pattern mmlp (P2P(pat.GetRecord(pos_AS_MapMergePattern_lp)));
      TYPE_STKM_Pattern mmrp (P2P(pat.GetRecord(pos_AS_MapMergePattern_rp)));
      if (mmrp.Is(TAG_TYPE_STKM_MapEnumPattern)) {
        switch (mmlp.GetTag()) {
          case TAG_TYPE_STKM_MapEnumPattern: {
            SEQ<TYPE_STKM_Pattern> mls (mmlp.GetSequence(pos_STKM_MapEnumPattern_mls));
            mls.ImpConc(mmrp.GetSequence(pos_STKM_MapEnumPattern_mls));
            return TYPE_STKM_MapEnumPattern().Init(mls);
          }
          case TAG_TYPE_STKM_MapMergePattern: {
            const TYPE_STKM_Pattern & mmplp (mmlp.GetRecord(pos_STKM_MapMergePattern_lp));
            const TYPE_STKM_Pattern & mmprp (mmlp.GetRecord(pos_STKM_MapMergePattern_rp));
            if (mmprp.Is(TAG_TYPE_STKM_MapEnumPattern)) {
              SEQ<TYPE_STKM_Pattern> mls (mmprp.GetSequence(pos_STKM_MapEnumPattern_mls));
              mls.ImpConc(mmrp.GetSequence(pos_STKM_MapEnumPattern_mls));
              return TYPE_STKM_MapMergePattern ().Init(mmplp, TYPE_STKM_MapEnumPattern().Init(mls));
            }
            else if (mmplp.Is(TAG_TYPE_STKM_MapEnumPattern)) {
              SEQ<TYPE_STKM_Pattern> mls (mmplp.GetSequence(pos_STKM_MapEnumPattern_mls));
              mls.ImpConc(mmrp.GetSequence(pos_STKM_MapEnumPattern_mls));
              return TYPE_STKM_MapMergePattern ().Init(mmprp, TYPE_STKM_MapEnumPattern().Init(mls));
            }
            else {
              return TYPE_STKM_MapMergePattern().Init(mmlp,mmrp);
            }
          }
          default: {
            return TYPE_STKM_MapMergePattern().Init(mmlp,mmrp);
          }
        }
      }
      else {
        return TYPE_STKM_MapMergePattern().Init(mmlp,mmrp);
      }
    }

    case TAG_TYPE_AS_TuplePattern: {
      return TYPE_STKM_TuplePattern().Init(PL2PL(pat.GetSequence(pos_AS_TuplePattern_fields)));
    }

    case TAG_TYPE_AS_RecordPattern: {
      return TYPE_STKM_RecordPattern().Init(pat.GetRecord(pos_AS_RecordPattern_nm),
                                            PL2PL(pat.GetSequence(pos_AS_RecordPattern_fields)));
    }
#ifdef VDMPP
    case TAG_TYPE_AS_ObjectPattern: {
      const SEQ<TYPE_AS_FieldPattern> & fields (pat.GetSequence(pos_AS_ObjectPattern_fields));
      SEQ<TYPE_STKM_FieldPattern> els;
      size_t len_fields = fields.Length();
      for (size_t i = 1; i <= len_fields; i++) {
        const TYPE_AS_FieldPattern & field (fields[i]);
        els.ImpAppend(TYPE_STKM_FieldPattern().Init(field.GetRecord(pos_AS_FieldPattern_nm),
                                                    P2P(field.GetRecord(pos_AS_FieldPattern_pat))));
      }
      return TYPE_STKM_ObjectPattern().Init(pat.GetRecord(pos_AS_ObjectPattern_cls), els);
    }
#endif // VDMPP
    default: {
      // should never happen
      return TYPE_STKM_Pattern();
    }
  }
}
// }}}

// {{{ AnyMatchVals, IsLiteral
// AnyMatchVals
// pat : AS`Pattern
// +> bool
bool StackCompiler::AnyMatchVals(const TYPE_AS_Pattern & pat)
{
  switch (pat.GetTag()) {
    case TAG_TYPE_AS_PatternName: {
      return false;
    }

    case TAG_TYPE_AS_MatchVal: {
      return !IsLiteral(pat.GetRecord(pos_AS_MatchVal_val));
    }

    case TAG_TYPE_AS_SetEnumPattern: {
      const SEQ<TYPE_AS_Pattern> & elems (pat.GetSequence(pos_AS_SetEnumPattern_Elems));
      // exists p in set elems pat.Elems & AnyMatchVals(p)
      bool exists (false);
      size_t len_elems = elems.Length();
      for (size_t idx = 1; (idx <= len_elems) && !exists; idx++) {
        exists = AnyMatchVals(elems[idx]);
      }
      return exists;
    }

    case TAG_TYPE_AS_SetUnionPattern: {
      return AnyMatchVals(pat.GetRecord(pos_AS_SetUnionPattern_lp)) ||
             AnyMatchVals(pat.GetRecord(pos_AS_SetUnionPattern_rp));
    }

    case TAG_TYPE_AS_SeqEnumPattern: {
      const SEQ<TYPE_AS_Pattern> & els (pat.GetSequence(pos_AS_SeqEnumPattern_els));
      // exists p in set elems pat.els & AnyMatchVals(p)
      bool exists (false);
      size_t len_els = els.Length();
      for (size_t idx = 1; (idx <= len_els) && !exists; idx++) {
        exists = AnyMatchVals(els[idx]);
      }
      return exists;
    }

    case TAG_TYPE_AS_SeqConcPattern: {
      return AnyMatchVals(pat.GetRecord(pos_AS_SeqConcPattern_lp)) ||
             AnyMatchVals(pat.GetRecord(pos_AS_SeqConcPattern_rp));
    }

    case TAG_TYPE_AS_MapEnumPattern: {
      const SEQ<TYPE_AS_MapletPattern> & mls (pat.GetSequence(pos_AS_MapEnumPattern_mls));
      bool exists (false);
      size_t len_mls = mls.Length();
      for (size_t idx = 1; (idx <= len_mls) && !exists; idx++) {
        exists = AnyMatchVals(mls[idx].GetRecord(pos_AS_MapletPattern_dp)) ||
                 AnyMatchVals(mls[idx].GetRecord(pos_AS_MapletPattern_rp));
      }
      return exists;
    }

    case TAG_TYPE_AS_MapMergePattern: {
      return AnyMatchVals(pat.GetRecord(pos_AS_MapMergePattern_lp)) ||
             AnyMatchVals(pat.GetRecord(pos_AS_MapMergePattern_rp));
    }

    case TAG_TYPE_AS_TuplePattern: {
      const SEQ<TYPE_AS_Pattern> & fields (pat.GetSequence(pos_AS_TuplePattern_fields));
      // exists p in set elems pat.fields & AnyMatchVals(p)
      bool exists (false);
      size_t len_fields = fields.Length();
      for (size_t idx = 1; (idx <= len_fields) && !exists; idx++) {
        exists = AnyMatchVals(fields[idx]);
      }
      return exists;
    }

    case TAG_TYPE_AS_RecordPattern: {
      const SEQ<TYPE_AS_Pattern> & fields (pat.GetSequence(pos_AS_RecordPattern_fields));
      // exists p in set elems pat.fields & AnyMatchVals(p)
      bool exists (false);
      size_t len_fields = fields.Length();
      for (size_t idx = 1; (idx <= len_fields) && !exists; idx++) {
        exists = AnyMatchVals(fields[idx]);
      }
      return exists;
    }
#ifdef VDMPP
    case TAG_TYPE_AS_ObjectPattern: {
      const SEQ<TYPE_AS_FieldPattern> & fields (pat.GetSequence(pos_AS_ObjectPattern_fields));
      bool exists (false);
      size_t len_fields = fields.Length();
      for (size_t idx = 1; (idx <= len_fields) && !exists; idx++) {
        exists = AnyMatchVals(fields[idx].GetRecord(pos_AS_FieldPattern_pat));
      }
      return exists;
    }
#endif // VDMPP
    default: {
      // should never happen
      return false;
    }
  }
}

// IsLiteral
// expr : AS`Expr
// +> bool
bool StackCompiler::IsLiteral(const TYPE_AS_Expr & expr)
{
  switch(expr.GetTag()) {
    case TAG_TYPE_AS_BoolLit:
    case TAG_TYPE_AS_NilLit:
    case TAG_TYPE_AS_RealLit:
    case TAG_TYPE_AS_NumLit:
    case TAG_TYPE_AS_CharLit:
    case TAG_TYPE_AS_TextLit:
    case TAG_TYPE_AS_QuoteLit: {
      return true;
    }
    default: {
      return false;
    }
  }
}
// }}}

// {{{ SD2I, SDStack2I, SDDirect2I, SD2SD, AnyExprs
// SD2I
// AS`StateDesignator (AS`Name | AS`FieldRef | AS`MapOrSeqRef)
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::SD2I(const TYPE_AS_StateDesignator & sd)
{
  return (AnyExprs(sd) ? SDStack2I(sd) : SDDirect2I(sd));
}

// SDStack2I
// sd : AS`StateDesignator
// +> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::SDStack2I(const TYPE_AS_StateDesignator & sd)
{
  switch (sd.GetTag()) {
#ifdef VDMPP
    case TAG_TYPE_AS_SelfExpr:
#endif // VDMPP
    case TAG_TYPE_AS_Name: {
      return TYPE_STKM_SubProgram().ImpAppend(TYPE_INSTRTP_PUSH().Init(sd));
    }
    case TAG_TYPE_AS_FieldRef: {
      TYPE_STKM_SubProgram sp;
      sp.ImpConc(SD2I(sd.GetRecord(pos_AS_FieldRef_var)))
        .ImpAppend(TYPE_INSTRTP_FREF().Init(sd.GetRecord(pos_AS_FieldRef_sel)));
      return sp;
    }
    case TAG_TYPE_AS_MapOrSeqRef: {
      TYPE_STKM_SubProgram sp;
      sp.ImpConc(SD2I(sd.GetRecord(pos_AS_MapOrSeqRef_var)))
        .ImpConc(E2I(sd.GetRecord(pos_AS_MapOrSeqRef_arg)))
        .ImpAppend(TYPE_INSTRTP_MOSREF());
      return sp;
    }
    case TAG_TYPE_AS_NarrowRef: {
      TYPE_STKM_SubProgram sp;
      sp.ImpConc(SD2I(sd.GetRecord(pos_AS_NarrowRef_var)));
      return sp;
    }
    default: {
      // should never happen
      return TYPE_STKM_SubProgram();
    }
  }
}

// SDDirect2I
// sd : AS`StateDesignator
// +> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::SDDirect2I(const TYPE_AS_StateDesignator& sd)
{
  return TYPE_STKM_SubProgram().ImpAppend(TYPE_INSTRTP_PUSH().Init(SD2SD(sd)));
}

// SD2SD
// sd : AS`StateDesignator
// +> STKM`StateDesignator
TYPE_STKM_StateDesignator StackCompiler::SD2SD(const TYPE_AS_StateDesignator& sd)
{
  switch (sd.GetTag()) {
#ifdef VDMPP
    case TAG_TYPE_AS_SelfExpr:
#endif // VDMPP
    case TAG_TYPE_AS_Name: {
      return sd;
    }
    case TAG_TYPE_AS_FieldRef: {
      return TYPE_STKM_FieldRef().Init(SD2SD(sd.GetRecord(pos_AS_FieldRef_var)),
                                       sd.GetRecord(pos_AS_FieldRef_sel));
    }
    case TAG_TYPE_AS_MapOrSeqRef: {
      TYPE_STKM_StateDesignator var (SD2SD(sd.GetRecord(pos_AS_MapOrSeqRef_var)));
      const TYPE_AS_Expr & arg (sd.GetRecord(pos_AS_MapOrSeqRef_arg));
      TYPE_SEM_VAL arg_v;
      switch (arg.GetTag()) {
        case TAG_TYPE_AS_BoolLit: {
          return TYPE_STKM_MapOrSeqRef().Init(var, arg.GetBoolValue(pos_AS_BoolLit_val) ? sem_true : sem_false);
        }
        case TAG_TYPE_AS_NilLit: {
          return TYPE_STKM_MapOrSeqRef().Init(var, sem_nil);
        }
        case TAG_TYPE_AS_RealLit: {
          return TYPE_STKM_MapOrSeqRef().Init(var, TYPE_SEM_NUM().Init(arg.GetReal(pos_AS_RealLit_val)));
        }
        case TAG_TYPE_AS_NumLit: {
          return TYPE_STKM_MapOrSeqRef().Init(var, TYPE_SEM_NUM().Init(arg.GetReal(pos_AS_NumLit_val)));
        }
        case TAG_TYPE_AS_CharLit: {
          return TYPE_STKM_MapOrSeqRef().Init(var, TYPE_SEM_CHAR().Init(arg.GetChar(pos_AS_CharLit_val)));
        }
        case TAG_TYPE_AS_TextLit: {
          const SEQ<Char> & elms (arg.GetSequence(pos_AS_TextLit_val));
          size_t len_elms = elms.Length();
          SEQ<TYPE_SEM_VAL> res_elems;
          for (size_t i = 1; i <= len_elms; i++) {
            res_elems.ImpAppend(TYPE_SEM_CHAR().Init(elms[i]));
          }
          return TYPE_STKM_MapOrSeqRef().Init(var, TYPE_SEM_SEQ().Init(res_elems));
        }
        case TAG_TYPE_AS_QuoteLit: {
          return TYPE_STKM_MapOrSeqRef().Init(var, TYPE_SEM_QUOTE().Init(arg.GetSequence(pos_AS_QuoteLit_val)));
        }
        default: {
          // never happen
          return TYPE_STKM_StateDesignator();
        }
      }
    }
    case TAG_TYPE_AS_NarrowRef:  {
      return SD2SD(sd.GetRecord(pos_AS_NarrowRef_var));
    }
    default: {
      // should never happen
      return TYPE_STKM_StateDesignator();
    }
  }
}

// AnyExprs
// sd : AS`StateDesignator
// +> bool
bool StackCompiler::AnyExprs(const TYPE_AS_StateDesignator & sd)
{
  switch (sd.GetTag()) {
#ifdef VDMPP
    case TAG_TYPE_AS_SelfExpr:
#endif // VDMPP
    case TAG_TYPE_AS_Name:        { return false; }
    case TAG_TYPE_AS_FieldRef:    { return AnyExprs(sd.GetRecord(pos_AS_FieldRef_var)); }
    case TAG_TYPE_AS_MapOrSeqRef: { return AnyExprs(sd.GetRecord(pos_AS_MapOrSeqRef_var)) ||
                                           !IsLiteral(sd.GetRecord(pos_AS_MapOrSeqRef_arg)); }
    case TAG_TYPE_AS_NarrowRef:   { return AnyExprs(sd.GetRecord(pos_AS_NarrowRef_var)); }
    default:                      { return false; } // should never happen
  }
}
// }}}

TYPE_AS_Expr StackCompiler::P2E (const TYPE_AS_Pattern & pat)
{
  switch(pat.GetTag()) {
    case TAG_TYPE_AS_PatternName: {
      const Generic & nmg (pat.GetField(pos_AS_PatternName_nm));
      if (!nmg.IsNil()) {
        return nmg;
      }
      wcout << L"Don't care patterns are not accepted at P2E" << endl;
      return TYPE_AS_UndefinedExpr().Init(NilContextId);
    }
    case TAG_TYPE_AS_MatchVal: {
      return pat.GetRecord(pos_AS_MatchVal_val);
    }
    case TAG_TYPE_AS_SetEnumPattern: {
      const SEQ<TYPE_AS_Pattern> & p_l (pat.GetSequence(pos_AS_SetEnumPattern_Elems));
      const TYPE_CI_ContextId & cid (pat.GetInt(pos_AS_SetEnumPattern_cid));
      int len_p_l = p_l.Length();
      SEQ<TYPE_AS_Expr> els;
      for (int index = 1; index <= len_p_l; index++) {
        els.ImpAppend(P2E(p_l[index]));
      }
      return TYPE_AS_SetEnumerationExpr().Init(els, cid);
    }
    case TAG_TYPE_AS_SetUnionPattern: {
      const TYPE_AS_Pattern & lp (pat.GetRecord(pos_AS_SetUnionPattern_lp));
      const TYPE_AS_Pattern & rp (pat.GetRecord(pos_AS_SetUnionPattern_rp));
      const TYPE_CI_ContextId & cid (pat.GetInt(pos_AS_SetUnionPattern_cid));
      return TYPE_AS_BinaryExpr().Init(P2E(lp), Int(SETUNION), P2E(rp), cid);
    }
    case TAG_TYPE_AS_SeqEnumPattern: {
      const SEQ<TYPE_AS_Pattern> & p_l (pat.GetSequence(pos_AS_SeqEnumPattern_els));
      const TYPE_CI_ContextId & cid (pat.GetInt(pos_AS_SeqEnumPattern_cid));
      int len_p_l = p_l.Length();
      SEQ<TYPE_AS_Expr> els;
      for (int index = 1; index <= len_p_l; index++) {
        els.ImpAppend(P2E(p_l[index]));
      }
      return TYPE_AS_SeqEnumerationExpr().Init(els, cid);
    }
    case TAG_TYPE_AS_SeqConcPattern: {
      const TYPE_AS_Pattern & lp (pat.GetRecord(pos_AS_SeqConcPattern_lp));
      const TYPE_AS_Pattern & rp (pat.GetRecord(pos_AS_SeqConcPattern_rp));
      const TYPE_CI_ContextId & cid (pat.GetInt(pos_AS_SeqConcPattern_cid));
      return TYPE_AS_BinaryExpr().Init(P2E(lp), Int(SEQCONC), P2E(rp), cid);
    }
    case TAG_TYPE_AS_MapEnumPattern: {
      const SEQ<TYPE_AS_MapletPattern> & p_l (pat.GetSequence(pos_AS_MapEnumPattern_mls));
      const TYPE_CI_ContextId & cid (pat.GetInt(pos_AS_MapEnumPattern_cid));
      int len_p_l = p_l.Length();
      SEQ<TYPE_AS_Maplet> mls;
      for (int index = 1; index <= len_p_l; index++) {
        const TYPE_AS_MapletPattern & mp(p_l[index]);
        const TYPE_AS_Pattern & dp (mp.GetRecord(pos_AS_MapletPattern_dp));
        const TYPE_AS_Pattern & rp (mp.GetRecord(pos_AS_MapletPattern_rp));
        const TYPE_CI_ContextId & mcid (pat.GetInt(pos_AS_MapletPattern_cid));
        mls.ImpAppend(TYPE_AS_Maplet().Init(P2E(dp), P2E(rp), mcid));
      }
      return TYPE_AS_MapEnumerationExpr().Init(mls, cid);
    }
    case TAG_TYPE_AS_MapMergePattern: {
      const TYPE_AS_Pattern & lp (pat.GetRecord(pos_AS_MapMergePattern_lp));
      const TYPE_AS_Pattern & rp (pat.GetRecord(pos_AS_MapMergePattern_rp));
      const TYPE_CI_ContextId & cid (pat.GetInt(pos_AS_MapMergePattern_cid));
      return TYPE_AS_BinaryExpr().Init(P2E(lp), Int(MAPMERGE), P2E(rp), cid);
    }
    case TAG_TYPE_AS_RecordPattern: {
      const TYPE_AS_Name nm (pat.GetRecord(pos_AS_RecordPattern_nm));
      const SEQ<TYPE_AS_Pattern> & p_l (pat.GetSequence(pos_AS_RecordPattern_fields));
      const TYPE_CI_ContextId & cid (pat.GetInt(pos_AS_RecordPattern_cid));
      int len_p_l = p_l.Length();
      SEQ<TYPE_AS_Expr> fields;
      for (int index = 1; index <= len_p_l; index++) {
        fields.ImpAppend(P2E(p_l[index]));
      }
      return TYPE_AS_RecordConstructorExpr().Init(nm, fields, cid);
    }
    case TAG_TYPE_AS_TuplePattern: {
      const SEQ<TYPE_AS_Pattern> & p_l (pat.GetSequence(pos_AS_TuplePattern_fields));
      const TYPE_CI_ContextId & cid (pat.GetInt(pos_AS_TuplePattern_cid));
      int len_p_l = p_l.Length();
      SEQ<TYPE_AS_Expr> fields;
      for (int index = 1; index <= len_p_l; index++) {
        fields.ImpAppend(P2E(p_l[index]));
      }
      return TYPE_AS_TupleConstructorExpr().Init(fields, cid);
    }
#ifdef VDMPP
    case TAG_TYPE_AS_ObjectPattern: {
      wcout << L"Object patterns are not accepted at P2E" << endl;
      return TYPE_AS_UndefinedExpr().Init(NilContextId);
    }
#endif // VDMPP
    default: // must not occur
      return TYPE_AS_UndefinedExpr().Init(NilContextId);
  }
}
