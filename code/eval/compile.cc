/***
*  * WHAT
*  *    Functions for compiling VDM(SL|++) ASTs into stack machine
*  *    instructions. 
*  * ID
*  *    $Id: compile.cc,v 1.51 2006/09/22 11:14:03 vdmtools Exp $
*  * SPECIFICATION VERSION
*  *    compile.cc: Implementation of compile.vdm 1.18
***/

// {{{ includes
#include "compile.h"
#include "rt_errmsg.h"
#include "projectval.h"
#include "asquotes.h"
#include "astaux.h"
#include "evalexpr.h"
#include "INSTRTP.h"
#include "RTERR.h"
#include "asquotes.h"
#include "contextinfo.h"
#include "stackeval.h"
#include "settings.h"
#include "evaldef.h"
#ifdef VDMPP
#include "libman.h"
#endif // VDMPP

// }}}
// {{{ ProgramTable

// ResetProgramTable
// modnm : AS`Name
// ==> ()
void ProgramTable::ResetProgramTable(const TYPE_AS_Name & modnm) 
{
  this->program_table.ImpModify( modnm, ModuleProgramTable() );
}

// InsertProgram
// modnm : AS`Name
// instr : STKM`SubProgram
// ==> STKM`SubProgramId
TYPE_STKM_SubProgramId ProgramTable::InsertProgram(const TYPE_AS_Name & modnm, const TYPE_STKM_SubProgram & instr)
{
  // 20060216
  if( !this->program_table.DomExists( modnm ) ) {
    this->ResetProgramTable( modnm );
  }
  ModuleProgramTable mpt (this->program_table[modnm]);
  SEQ<TYPE_STKM_SubProgram> tbl (mpt.get_tbl());  // seq of STKM`SubProgram
  TYPE_STKM_SubProgramId id (mpt.get_old_id());     // STKM`SubProgramId
  tbl.ImpAppend(instr);

  TYPE_STKM_SubProgramId newid (id.GetValue() + 1);
  this->program_table.ImpModify(modnm, ModuleProgramTable( tbl, newid ));
  return newid;
}

// GetProgram
// modnm : AS`Name
// id : STKM`SubProgramId
// ==> STKM`SubProgram
const TYPE_STKM_SubProgram& ProgramTable::GetProgram(const TYPE_AS_Name & modnm,
                                                     const TYPE_STKM_SubProgramId & id) const
{
  ModuleProgramTable mpt (this->program_table[modnm]);
  const SEQ<TYPE_STKM_SubProgram> & tbl (mpt.GetSequence(1));

  if (id > (int)tbl.Length()) {
    RTERR::Error(L"ProgramTable::GetProgram", RTERR_ID_NOT_IN_LIST, Nil(), Nil(), Sequence());
  }
  return tbl[id];
}
 
// CopyProgram
// oldnm : AS`Name
// newnm : AS`Name
// ==> ()
void ProgramTable::CopyProgram(const TYPE_AS_Name & oldnm, const TYPE_AS_Name & newnm)
{
  ModuleProgramTable mpt (this->program_table[oldnm]);
  this->program_table.ImpModify(newnm, mpt);
}

// DumpProgram
// modnm : AS`Name
SEQ<TYPE_STKM_SubProgram> ProgramTable::DumpProgram(const TYPE_AS_Name & modnm) const
{
  if (this->program_table.DomExists(modnm)) {
    ModuleProgramTable mpt (this->program_table[modnm]);
    return mpt.get_tbl();
  }
  return SEQ<TYPE_STKM_SubProgram>();
}
// }}}

// {{{ StackCompiler
//  
StackCompiler::StackCompiler()
{
  Init_CSTMT();
  Init_CEXPR();

#ifdef VICE
  init_TIMEMAP();
  Init_TS();
#endif // VICE

  GetEmptyCompEnv();
}

StackCompiler::~StackCompiler()
{
}

SEQ<TYPE_STKM_SubProgram> StackCompiler::DumpProgram(const TYPE_AS_Name & nm) const
{
  return this->program_table.DumpProgram(nm);
}

// GetEmptyCompEnv
void StackCompiler::GetEmptyCompEnv()
{
  this->DebugInfo = true;
  this->curr_cl_mod = ASTAUX::MkNameFromVoid();
  this->program_table = ProgramTable();
#ifdef VICE
  this->timem = vdm_TIMEMAP_defaultTimemap();
#endif // VICE
//
//  this->trapno = 0;
}

// SetEmptyCompEnv
void StackCompiler::SetEmptyCompEnv()
{
  this->DebugInfo = true;
}

// {{{ Misc
// CompileFnOpDef
// fndef : AS`FnDef | AS`OpDef
// +> STKM`SubProgramId
TYPE_STKM_SubProgramId StackCompiler::CompileFnOpDef(const Record & fndef)
{
  TYPE_STKM_SubProgram instr;
  switch(fndef.GetTag()) {
    // Various types of functions:
    case TAG_TYPE_AS_ExplFnDef: {
      const TYPE_AS_ParametersList & parmss (fndef.GetSequence(pos_AS_ExplFnDef_parms));
      TYPE_AS_Parameters parms;
      if (!parmss.IsEmpty()) {
        parms.ImpConc(parmss.Hd());
      }
      instr.ImpConc(Mease2I(fndef));
      instr.ImpConc(FnDef2I(fndef.GetField(pos_AS_ExplFnDef_fnpre),
                            fndef.GetField(pos_AS_ExplFnDef_fnpost),
                            fndef.GetField(pos_AS_ExplFnDef_body),
                            SEQ<TYPE_AS_NameType>(),
                            fndef.GetField(pos_AS_ExplFnDef_nm),
                            parms));
      break;
    }
    case TAG_TYPE_AS_ExtExplFnDef: {
      const TYPE_AS_ParameterTypes & partps(fndef.GetSequence(pos_AS_ExtExplFnDef_partps));
      TYPE_AS_Parameters parms;
      size_t len_partps = partps.Length();
      for (size_t i = 1; i <= len_partps; i++) {
        const TYPE_AS_PatTypePair & ptp (partps[i]);
        parms.ImpConc(ptp.GetSequence(pos_AS_PatTypePair_pats));
      }
      instr.ImpConc(Mease2I(fndef));
      instr.ImpConc(FnDef2I(fndef.GetField(pos_AS_ExtExplFnDef_fnpre),
                            fndef.GetField(pos_AS_ExtExplFnDef_fnpost),
                            fndef.GetField(pos_AS_ExtExplFnDef_body),
                            fndef.GetSequence(pos_AS_ExtExplFnDef_resnmtps),
                            fndef.GetField(pos_AS_ExtExplFnDef_nm),
                            parms));
      break;
    }
    case TAG_TYPE_AS_ImplFnDef: {
      instr.ImpConc(ImplFnDef2I(fndef.GetInt(pos_AS_ImplFnDef_cid)));
      break;
    }

  // Various types of operations:
    case TAG_TYPE_AS_ExplOpDef: {
      instr.ImpConc(OpDef2I(fndef.GetField(pos_AS_ExplOpDef_oppre),
                            fndef.GetField(pos_AS_ExplOpDef_oppost),
                            fndef.GetField(pos_AS_ExplOpDef_body),
                            SEQ<TYPE_AS_NameType>(),
                            fndef.GetField(pos_AS_ExplOpDef_nm),
                            fndef.GetSequence(pos_AS_ExplOpDef_parms),
                            fndef.GetBool(pos_AS_ExplOpDef_constr),
                            fndef.GetBool(pos_AS_ExplOpDef_opsync)
                            ));
      break;
    }
    case TAG_TYPE_AS_ExtExplOpDef: {
      const TYPE_AS_ParameterTypes & partps(fndef.GetSequence(pos_AS_ExtExplOpDef_partps));
      TYPE_AS_Parameters parms;
      size_t len_partps = partps.Length();
      for (size_t i = 1; i <= len_partps; i++) {
        const TYPE_AS_PatTypePair & ptp (partps[i]);
        parms.ImpConc(ptp.GetSequence(pos_AS_PatTypePair_pats));
      }
      instr.ImpConc(OpDef2I(fndef.GetField(pos_AS_ExtExplOpDef_oppre),
                            fndef.GetField(pos_AS_ExtExplOpDef_oppost),
                            fndef.GetField(pos_AS_ExtExplOpDef_body),
                            fndef.GetSequence(pos_AS_ExtExplOpDef_resnmtps),
                            fndef.GetField(pos_AS_ExtExplOpDef_nm),
                            parms,
                            fndef.GetBool(pos_AS_ExtExplOpDef_constr),
                            fndef.GetBool(pos_AS_ExtExplOpDef_opsync)
                           ));
      break;
    }
    case TAG_TYPE_AS_ImplOpDef: {
      instr.ImpConc(ImplOpDef2I(fndef.GetInt(pos_AS_ImplOpDef_cid)));
      break;
    }
    default: {
      RTERR::Error(L"StackCompiler::CompileFnOpDef", RTERR_UNKNOWN_RECORD_TYPE, Nil(), Nil(), Sequence());
      break;
    }
  }  
  // Insert the generated instruction sequence into the program table
  // and return the index of this new entry:

  TYPE_STKM_SubProgram prog;
#ifdef VDMSL
  prog.ImpConc(SetContext(ASTAUX::GetCid(fndef), false));
#endif // VDMSL
#ifdef VDMPP
  if (!ASTAUX::IsSubrespFnOp(fndef)) {
    prog.ImpConc(SetContext(ASTAUX::GetCid(fndef), false));
  }
#endif // VDMPP

  prog.ImpConc(instr);
  prog.ImpAppend(TYPE_INSTRTP_RETURN());

  return program_table.InsertProgram(GetClMod(), prog);
}

// CompilePrePostExpr
// e : AS`Expr
// ==> STKM`SubProgramId
TYPE_STKM_SubProgramId StackCompiler::CompilePrePostExpr(const TYPE_AS_Expr& e)
{
  TYPE_STKM_SubProgram sp (E2I(e));
  sp.ImpAppend(TYPE_INSTRTP_RETURN());
  return program_table.InsertProgram(GetClMod(), sp);
}

// CompileMeasureExpr
// e : AS`Expr
// ==> STKM`SubProgramId
TYPE_STKM_SubProgramId StackCompiler::CompileMeasureExpr(const TYPE_AS_Expr& e)
{
  TYPE_STKM_SubProgram sp (E2I(e));
  sp.ImpAppend(TYPE_INSTRTP_DTCMEASURE());
  sp.ImpAppend(TYPE_INSTRTP_RETURN());
  return program_table.InsertProgram(GetClMod(), sp);
}

// Mease2I
// fndef : AS`FnDef
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::Mease2I(const TYPE_AS_FnDef& fndef)
{
  const Generic & measu (ASTAUX::GetFnMeasu(fndef));
  const Generic & fnbody (ASTAUX::GetFnBody(fndef));
  TYPE_STKM_SubProgram sp;
  if (!measu.IsNil() && !fnbody.IsInt()) {
    if (measu != Int(NOTYETSPEC)) {
      TYPE_STKM_SubProgram tsp;
      tsp.ImpConc(E2I(measu)); 
      
      if (MeasureIsId(measu, fndef)) {
        int count = 0;
    
        if (fndef.Is(TAG_TYPE_AS_ExplFnDef)) {
          if (!fndef.GetSequence(pos_AS_ExplFnDef_tpparms).IsEmpty()) {
            tsp.ImpAppend(TYPE_INSTRTP_MEASURETPINST().Init(fndef.GetSequence(pos_AS_ExplFnDef_tpparms)));
          }
          tsp.ImpAppend(TYPE_INSTRTP_COPYVAL());

          const SEQ< SEQ<TYPE_AS_Pattern> > & pat_l_l (fndef.GetSequence(pos_AS_ExplFnDef_parms));
          size_t len_pat_l_l = pat_l_l.Length();
          for (size_t idx1 = 1; idx1 <= len_pat_l_l; idx1++) {
            SEQ<TYPE_AS_Pattern> pat_l (pat_l_l[idx1]);
            if (!pat_l.IsEmpty()) {
              size_t len_pat_l = pat_l.Length();
              for (size_t idx2 = 1; idx2 <= len_pat_l; idx2++) {
                tsp.ImpConc(E2I(P2E(pat_l[idx2])));
              }
              count += len_pat_l;
            }
          }
        }
        else if (fndef.Is(TAG_TYPE_AS_ExtExplFnDef)) {
          if (!fndef.GetSequence(pos_AS_ExtExplFnDef_params).IsEmpty()) {
            tsp.ImpAppend(TYPE_INSTRTP_MEASURETPINST().Init(fndef.GetSequence(pos_AS_ExtExplFnDef_params)));
          }
          tsp.ImpAppend(TYPE_INSTRTP_COPYVAL());

          const SEQ<TYPE_AS_PatTypePair> & parml (fndef.GetSequence(pos_AS_ExtExplFnDef_partps));
          size_t len_parml = parml.Length(); 
          for (size_t idx1 = 1; idx1 <= len_parml; idx1++) {
            SEQ<TYPE_AS_Pattern> pat_l (parml[idx1].GetSequence(pos_AS_PatTypePair_pats));
            if (!pat_l.IsEmpty()) {
              size_t len_pat_l = pat_l.Length();
              for (size_t idx2 = 1; idx2 <= len_pat_l; idx2++) {
                tsp.ImpConc(E2I(P2E(pat_l[idx2])));
              }
              count += len_pat_l;
            }
          }
        }
        tsp.ImpAppend(TYPE_INSTRTP_PUSHLIST().Init(Int(count)));
#ifdef VDMPP
        tsp.ImpAppend(TYPE_INSTRTP_GUARD().Init(Bool(true)));
#endif //VDMPP
        tsp.ImpConc(SetContext(ASTAUX::GetCid(measu), false))
           .ImpAppend(TYPE_INSTRTP_APPLY())
           .ImpAppend(TYPE_INSTRTP_REMSTACKELEM().Init(Int(2)));
      }
      tsp.ImpAppend(TYPE_INSTRTP_MEASURECHECK());
      sp.ImpConc(ConcIfThenElse(TYPE_STKM_SubProgram().ImpAppend(TYPE_INSTRTP_MEASURE()),
                                tsp, TYPE_STKM_SubProgram()));
    }
  }
  return sp;
}

// MeasureIsId
// measu : [AS`Expr |  <NOTYETSPEC>]
// fndef : AS`FnDef
// -> bool
bool StackCompiler::MeasureIsId(const Generic & measu, const TYPE_AS_FnDef & fndef)
{
  if (measu.IsNil()) {
    return false;
  }
  else if (measu == Int(NOTYETSPEC)) {
    return false;
  }
  else if (measu.Is(TAG_TYPE_AS_BracketedExpr)) {
    const TYPE_AS_Expr & expr (Record(measu).GetRecord(pos_AS_BracketedExpr_expr));
    return MeasureIsId(expr, fndef);
  }
  else if (measu.Is(TAG_TYPE_AS_Name)) {
    if (fndef.Is(TAG_TYPE_AS_ExplFnDef)) {
      const SEQ< SEQ<TYPE_AS_Pattern> > & parms (fndef.GetSequence(pos_AS_ExplFnDef_parms));
      size_t len_parms = parms.Length();
      SET<TYPE_AS_Name> nm_s;
      for (size_t i = 1; i <= len_parms; i++) {
        nm_s.ImpUnion(NamesInPatternList(parms[i]));
      }
      return !nm_s.InSet(measu);
    }
    else if (fndef.Is(TAG_TYPE_AS_ExtExplFnDef)) {
      const SEQ<TYPE_AS_PatTypePair> & partps (fndef.GetSequence(pos_AS_ExtExplFnDef_partps));
      size_t len_partps = partps.Length(); 
      SET<TYPE_AS_Name> nm_s;
      for (size_t i = 1; i <= len_partps; i++) {
        nm_s.ImpUnion(NamesInPatternList(partps[i].GetSequence(pos_AS_PatTypePair_pats)));
      }
      return !nm_s.InSet(measu);
    }
    else {
      return false; // not occur
    }
  }
  else {
    return false;
  }
}

SET<TYPE_AS_Name> StackCompiler::NamesInPatternList(const SEQ<TYPE_AS_Pattern> & pat_l)
{
  size_t len_pat_l = pat_l.Length(); 
  SET<TYPE_AS_Name> nm_s;
  for (size_t i = 1; i <= len_pat_l; i++) {
    nm_s.ImpUnion(NamesInPattern(pat_l[i]));
  }
  return nm_s;
}

SET<TYPE_AS_Name> StackCompiler::NamesInPattern(const TYPE_AS_Pattern & pat)
{
  switch(pat.GetTag()) {
    case TAG_TYPE_AS_PatternName: {
      const Generic & nm (pat.GetField(pos_AS_PatternName_nm));
      SET<TYPE_AS_Name> nm_s;
      if (!nm.IsNil()) {
        nm_s.Insert(nm);
      }
      return nm_s;
    }
    case TAG_TYPE_AS_MatchVal: {
      return SET<TYPE_AS_Name>();
    }
    case TAG_TYPE_AS_SetEnumPattern: {
      return NamesInPatternList(pat.GetSequence(pos_AS_SetEnumPattern_Elems));
    }
    case TAG_TYPE_AS_SetUnionPattern: {
      SET<TYPE_AS_Name> nm_s;
      nm_s.ImpUnion(NamesInPattern(pat.GetRecord(pos_AS_SetUnionPattern_lp)));
      nm_s.ImpUnion(NamesInPattern(pat.GetRecord(pos_AS_SetUnionPattern_rp)));
      return nm_s;
    }
    case TAG_TYPE_AS_SeqEnumPattern: {
      return NamesInPatternList(pat.GetSequence(pos_AS_SeqEnumPattern_els));
    }
    case TAG_TYPE_AS_SeqConcPattern: {
      SET<TYPE_AS_Name> nm_s;
      nm_s.ImpUnion(NamesInPattern(pat.GetRecord(pos_AS_SeqConcPattern_lp)));
      nm_s.ImpUnion(NamesInPattern(pat.GetRecord(pos_AS_SeqConcPattern_rp)));
      return nm_s;
    }
    case TAG_TYPE_AS_MapEnumPattern: {
      const SEQ<TYPE_AS_MapletPattern> & mls (pat.GetSequence(pos_AS_MapEnumPattern_mls));
      int len_mls = mls.Length();
      SET<TYPE_AS_Name> nm_s;
      for (int i = 1; i <= len_mls; i++) {
        nm_s.ImpUnion(NamesInPattern(mls[i].GetRecord(pos_AS_MapletPattern_dp)));
        nm_s.ImpUnion(NamesInPattern(mls[i].GetRecord(pos_AS_MapletPattern_rp)));
      }
      return nm_s;
    }
    case TAG_TYPE_AS_MapMergePattern: {
      SET<TYPE_AS_Name> nm_s;
      nm_s.ImpUnion(NamesInPattern(pat.GetRecord(pos_AS_MapMergePattern_lp)));
      nm_s.ImpUnion(NamesInPattern(pat.GetRecord(pos_AS_MapMergePattern_rp)));
      return nm_s;
    }
    case TAG_TYPE_AS_RecordPattern: {
      return NamesInPatternList(pat.GetSequence(pos_AS_RecordPattern_fields));
    }
    case TAG_TYPE_AS_TuplePattern: {
      return NamesInPatternList(pat.GetSequence(pos_AS_TuplePattern_fields));
    }
    default: {
      return SET<TYPE_AS_Name>();
    }
  }
}

// CompileLambdaBody
// e : AS`Expr
// +> STKM`SubProgramId
TYPE_STKM_SubProgramId StackCompiler::CompileLambdaBody(const TYPE_AS_Expr & e)
{
  TYPE_STKM_SubProgram sp (E2I(e));
  sp.ImpAppend(TYPE_INSTRTP_RETURN());
  return program_table.InsertProgram(GetClMod(), sp);
}

// FnDef2I
// fnpre : [AS`Expr]
// fnpost : [AS`Expr]
// body : AS`FnBody
// resnmtps : seq of AS`NameType
// fnname : AS`Name
// parms : (AS`ParametersList | AS`ParameterTypes)
// ==>  STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::FnDef2I(const Generic & fnpre, 
                                            const Generic & fnpost, 
                                            const TYPE_AS_FnBody & body,
                                            const SEQ<TYPE_AS_NameType> & resnmtps,
                                            const TYPE_AS_Name & fnname,
                                            const Sequence & parms)
{
  const Generic & body_body (body.GetField(pos_AS_FnBody_body));

  TYPE_STKM_SubProgram i_fnpre (PrePost2I(fnpre, true));

  TYPE_STKM_SubProgram i_body;
  if (body_body.IsInt()) {
    // <NOTYETSPEC> or <SUBRESP>
    switch (Int(body_body).GetValue()) {
      case NOTYETSPEC: {
#ifdef VDMPP
        if (theState().IsDLClass(GetClMod())) {
          i_body.ImpAppend(TYPE_INSTRTP_DLCALL().Init(GetClMod(), fnname));
        }
        else 
#endif // VDMPP
          i_body.ImpAppend(TYPE_INSTRTP_NOBODY().Init(TYPE_RTERR_ERR(RTERR_NOTYETSPECFCT),
                                                      GetClMod(), fnname, parms));
        break;
      }
#ifdef VDMPP    
      case SUBRESP: {
        theState().AddAbstract(GetClMod());
        i_body.ImpAppend(TYPE_INSTRTP_NOBODY().Init(TYPE_RTERR_ERR(RTERR_SUBRESP),
                                                    GetClMod(), fnname, parms));
        break;
      }
#endif // VDMPP
    }
  }
  else {
    i_body.ImpConc(E2I(body_body));
  }

  TYPE_STKM_SubProgram i_fnpost;
  if (!fnpost.IsNil()) {
    i_fnpost.ImpAppend(TYPE_INSTRTP_POSTENV().Init(resnmtps, ASTAUX::GetCid(fnpost)))
            .ImpConc(PrePost2I(fnpost,false))
            .ImpAppend(TYPE_INSTRTP_POPBLKENV());
  }
  TYPE_STKM_SubProgram prog;
  prog.ImpConc(i_fnpre);
  prog.ImpConc(i_body);
  prog.ImpConc(i_fnpost);
  return prog;
}

// PrePost2I
// cond : [AS`Expr]
// precond : bool
// -> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::PrePost2I(const Generic & cond, bool precond)
{
  if(cond.IsNil()) {
    return TYPE_STKM_SubProgram();
  }
  else {
    // icond 
    TYPE_STKM_SubProgram i_cond (E2I(cond)); // pre/post expr

    // prepost
    TYPE_STKM_SubProgram prepost;
    if (precond) {
      prepost.ImpAppend(TYPE_INSTRTP_PRE());
    }
    else {
      prepost.ImpAppend(TYPE_INSTRTP_POST());
    }
    // errMsg
    TYPE_STKM_SubProgram errMsg;
    if (precond) {
      errMsg.ImpConc(CompileRunTime(TYPE_RTERR_ERR(RTERR_EVAL_PRE_GIVES_FALSE), ASTAUX::GetCid(cond)));
    }
    else {
      errMsg.ImpConc(CompileRunTime(TYPE_RTERR_ERR(RTERR_EVAL_POST_GIVES_FALSE), ASTAUX::GetCid(cond)));
    }
    TYPE_STKM_SubProgram sp;
#ifdef VICE
    sp.ImpAppend(TYPE_INSTRTP_PUSHDURATION());
#endif // VICE

    sp.ImpConc(ConcIfThenElse(prepost,
                              ConcIfThenElse(i_cond, TYPE_STKM_SubProgram(), errMsg),
                              TYPE_STKM_SubProgram()));
#ifdef VICE
    sp.ImpAppend(TYPE_INSTRTP_POPDURATION().Init(Int(0)));
#endif // VICE

    return sp;
  }
}

// ImplFnDef2I
// cid : CI`ContextId
// -> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::ImplFnDef2I(const TYPE_CI_ContextId& cid)
{
  return CompileRunTime(TYPE_RTERR_ERR(RTERR_IMPL_FN_APPLY), cid);
}

// OpDef2I
// fnpre : [AS`Expr]
// fnpost : [AS`Expr]
// body : AS`OpBody
// resnmtps seq of AS`NameType
// fnname : AS`Name
// parms : AS`Parameters
// constr :  bool
// sync : bool
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::OpDef2I(const Generic & fnpre, 
                                            const Generic & fnpost, 
                                            const TYPE_AS_OpBody & body,
                                            const SEQ<TYPE_AS_NameType> & resnmtps,
                                            const TYPE_AS_Name & fnname,
                                            const Sequence & parms, 
                                            const Bool & constr,
                                            const Bool & sync)
{
  const Generic & body_body (body.GetField(pos_AS_OpBody_body));

#ifdef VDMPP
  if (body_body == Int(SUBRESP)) {
    theState().AddAbstract(GetClMod());
  }
#endif // VDMPP

  TYPE_STKM_SubProgram prog (PrePost2I(fnpre, true));

  if (body_body.IsInt()) {
    // <NOTYETSPEC> or <SUBRESP>
    switch(Int(body_body).GetValue()) {
      case NOTYETSPEC: {
#ifdef VDMPP
        if (theState().IsDLClass(GetClMod())) {
          prog.ImpAppend(TYPE_INSTRTP_DLCALL().Init(GetClMod(), fnname));
        }
        else 
#endif //VDMPP
          prog.ImpAppend(TYPE_INSTRTP_NOBODY().Init(TYPE_RTERR_ERR(RTERR_NOTYETSPECOP),
                                                    GetClMod(), fnname, parms));
        break;
      }
#ifdef VDMPP
      case SUBRESP: {
        prog.ImpAppend(TYPE_INSTRTP_NOBODY().Init(TYPE_RTERR_ERR(RTERR_SUBRESP),
                                                  GetClMod(), fnname, parms));
        break;
      }
#endif // VDMPP
    }
  }
  else {
    prog.ImpConc(S2I(body_body));

#ifdef VDMPP
    if (constr.GetValue()) {
      prog.ImpAppend(TYPE_INSTRTP_POP().Init(Int(1)))
          .ImpAppend(TYPE_INSTRTP_SELFEXPR());
    }
#endif //VDMPP
  }

  if (!fnpost.IsNil()) {
    prog.ImpAppend(TYPE_INSTRTP_POSTENV().Init(resnmtps, ASTAUX::GetCid(fnpost)))
        .ImpConc(PrePost2I(fnpost,false))
        .ImpAppend(TYPE_INSTRTP_POPBLKENV());
  }
  return prog;
}

// ImplOpDef2I
// cid : CI`ContextId
// -> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::ImplOpDef2I(const TYPE_CI_ContextId & cid)
{ 
  return CompileRunTime(TYPE_RTERR_ERR(RTERR_IMPL_OP_CALL), cid);
}

// SetContext
// cid : CI`ContextId
// isStmt : bool
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::SetContext(const TYPE_CI_ContextId & cid, bool isStmt)
{
  if (cid != NilContextId) {
    GetCI().SetBreakable(cid);
  }

  TYPE_STKM_SubProgram sp;
  if (this->DebugInfo) {
    sp.ImpAppend(TYPE_INSTRTP_CONTEXT().Init(cid, Bool(isStmt)));
  }
  return sp;
}

// SetDebugInfo
// b : bool
// ==> ()
void StackCompiler::SetDebugInfo(bool b) 
{
  this->DebugInfo = b;
}

// SetClMod
// nm : AS`Name
// ==> ()
void StackCompiler::SetClMod(const TYPE_AS_Name & nm) 
{
  this->curr_cl_mod = nm;
}

// GetClMod
// ==> AS`Name
const TYPE_AS_Name& StackCompiler::GetClMod() const
{
  return this->curr_cl_mod;
}

// }}}
// {{{ program_table modifying functions

// ResetProgramTable
// modnm : AS`Name
// ==> ()
void StackCompiler::ResetProgramTable(const TYPE_AS_Name & modnm) 
{
  program_table.ResetProgramTable(modnm);
}

// InsertProgram
// modnm : AS`Name
// instr : STKM`SubProgram
// ==> STKM`SubProgramId
TYPE_STKM_SubProgramId StackCompiler::InsertProgram(const TYPE_AS_Name & modnm, const TYPE_STKM_SubProgram & instr)
{
  return program_table.InsertProgram(modnm, instr);
}

// GetProgram
// modnm : AS`Name
// id : STKM`SubProgramId
// ==> STKM`SubProgram
const TYPE_STKM_SubProgram & StackCompiler::GetProgram(const TYPE_AS_Name & modnm,
                                                       const TYPE_STKM_SubProgramId & id) const
{
  return program_table.GetProgram(modnm, id);
}

// CopyProgram
// old : AS`Name
// newnm : AS`Name
// ==> ()
void StackCompiler::CopyProgram(const TYPE_AS_Name & old, const TYPE_AS_Name & newnm)
{
  program_table.CopyProgram(old, newnm);
}

// IStart
// name : seq of char
// cid : CI`ContextId
// -> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::IStart(const SEQ<Char> & name, const TYPE_CI_ContextId & cid) const
{
  TYPE_STKM_SubProgram prog;
#ifdef VICE
  prog.ImpAppend(TYPE_INSTRTP_ISTART().Init(name, cid));
#endif // VICE
  return prog;
}

// IEnd
// name : seq of char
// -> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::IEnd(const SEQ<Char> & name) const
{
  TYPE_STKM_SubProgram prog;
#ifdef VICE
  prog.ImpAppend(TYPE_INSTRTP_IEND().Init(name));
#endif // VICE
  return prog;
}
// }}}
// {{{ Run Time Error functions
// CompileRunTime
// str : RTERR`ERR
// cid : CI`ContextId
// -> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileRunTime(const TYPE_RTERR_ERR & rterr, const TYPE_CI_ContextId & cid)
{
  TYPE_STKM_SubProgram sp (SetContext(cid, false));
  sp.ImpAppend(TYPE_INSTRTP_ERRINST().Init(rterr));
  return sp;
}

// }}}

#ifdef VICE
//{{{ VICE Time map
// SetTM
// newtm : TIMEMAP`Timemap
// ==> ()
void StackCompiler::SetTM(const TYPE_TIMEMAP_Timemap & newtm)
{
  this->timem = newtm;
}

// GetTM
// ==> TIMEMAP`Timemap
TYPE_TIMEMAP_Timemap StackCompiler::GetTM() const
{
  return this->timem;
}
//}}}
#endif //VICE
// {{{ Old unused stuff from Ole's implementation

// SubProgram 
// StackCompiler::CompileCommandLine(const Generic &es)
// {
//   // In due time this function should decide whether es represents an
//   // expression or a statement and then call E2I or S2I accordingly.
// 
//   return E2I(es);
// }
// 
// void 
// StackCompiler::ConcSubPrograms(SubProgram &dest, const SubProgram &src)
//   // This function concatenates two sub programs. The sub program
//   // contained in src is concatenated to the sub program of dest. 
//   // The function is used only during compilation.
//   //
// {
//   copy(src.begin(), src.end(), back_insert_iterator<SubProgram>(dest));
// }
// 
// 
// Instruction 
// StackCompiler::GenRunTime(wchar_t * err)
//   // Generates a run time error instruction 
// {
//   return Instruction(INSTR::ERRINST, Sequence(wstring(err) ));
// }
// 
// 
// 
// 
// 
// 
// 
// SubProgram 
// StackCompiler::E2I(const TYPE_AS_Expr & r)
// {
//   SubProgram p;
// 
//   // First we add a CONTEXT instruction:
//   PushContext(p, r);
// 
//   // Next we recursively generate instructions depending on the type
//   // of r:
//   switch(r.GetTag()){
//   case TAG_TYPE_AS_BracketedExpr:
//     {
//       TYPE_AS_BracketedExpr e(r);
//       ConcSubPrograms(p, E2I(e.get_expr()));
//       break;
//     }
//   case TAG_TYPE_AS_ApplyExpr:
//     {
//       ConcSubPrograms(p, CompileApplyExpr(TYPE_AS_ApplyExpr(r)));
//       break;
//     }
//   case TAG_TYPE_AS_IfExpr :
//     {
//       ConcSubPrograms(p, CompileIfExpr(TYPE_AS_IfExpr(r)));
//       break;
//     }
//   case TAG_TYPE_AS_PrefixExpr :
//     {
//       ConcSubPrograms(p, CompileUnaryExpr(TYPE_AS_PrefixExpr(r)));
//       break;
//     }
//   case TAG_TYPE_AS_BinaryExpr :
//     {
//       ConcSubPrograms(p, CompileBinaryExpr(TYPE_AS_BinaryExpr(r)));
//       break;
//     }
//   case TAG_TYPE_AS_MapComprehensionExpr :
//     {
//       ConcSubPrograms(p, CompileMapComprehensionExpr(TYPE_AS_MapComprehensionExpr(r)));
//       break;
//     }
//   case TAG_TYPE_AS_Name :
//     {
//       p.push_back(Instruction(INSTR::PUSH, r));
//       p.push_back(Instruction(INSTR::LOOKUP));
//       break;
//     }
//   case TAG_TYPE_AS_BoolLit :
//   case TAG_TYPE_AS_NilLit :
//   case TAG_TYPE_AS_RealLit :
//   case TAG_TYPE_AS_TextLit :
//   case TAG_TYPE_AS_CharLit :
//   case TAG_TYPE_AS_QuoteLit :
//     {
//       // Here we use EvalLiteral from eval.h to convert AS literals to
//       // semantic values
//       p.push_back( Instruction(INSTR::PUSH, EvalLiteral((Generic)r) ) );
//       break;
//     }
// 
//   default:
//     Error(L"StackCompiler::E2I", "Unknown record type", 
//                  Generic());    
//   }
//   
//   // - and finally append a POPCONTEXT instruction:
//   PopContext(p, r);
// 
//   return p;
// }
// 
// 
// 
// 
// SubProgram 
// StackCompiler::CompileApplyExpr(const TYPE_AS_ApplyExpr &e)
// { 
//   SubProgram p;
//   p.push_back(Instruction(INSTR::EMPTYLIST));
// 
//   // Generate instructions for each argument:
//   type_dL arg_l = e.get_arg();
//   Generic gc;
//   for(bool bb = arg_l.First(gc); bb; bb = arg_l.Next(gc)){
//     ConcSubPrograms(p, E2I(gc));
//     p.push_back(Instruction(INSTR::APPENDESTCK));
//   }
//   p.push_back(Instruction(INSTR::APPLY));
// 
//   return p;
// }
// 
// SubProgram 
// StackCompiler::ConcIfThenElse(const SubProgram& cond_l,
//                               const SubProgram& exp1_l,
//                               const SubProgram& altn_l)
//   // Constructs the right instruction sequence for a simple if
//   // expression, that is an if expression without any elseif branches.
// {
//   SubProgram p = cond_l;
//   p.push_back(Instruction(INSTR::CBR, Int(altn_l.size() + 1) ));
//   ConcSubPrograms(p, altn_l);
//   p.push_back(Instruction(INSTR::BR, Int(exp1_l.size()) ));
//   ConcSubPrograms(p, exp1_l);
//   return p;
// }
// 
// SubProgram 
// StackCompiler::CompileIfExpr(const TYPE_AS_IfExpr &e)
// {
//   SubProgram p,
//     cond_l = E2I(e.get_test()),
//     expr1_l = E2I(e.get_cons());
//   if(e.get_elsif().Length() == 0){
//     // No elseif branches in this if expression
//     return ConcIfThenElse(cond_l, expr1_l, E2I(e.get_altn()));
//   }
//   else{
//     // Take the head of the sequence of elseif nodes and call E2I
//     // recursively:
//     Sequence elif_l = e.get_elsif();
//     TYPE_AS_ElseifExpr elif_e = elif_l.Hd();
//     TYPE_AS_IfExpr if_e;
// 
//     // Construct a 'virtual' if-node containing the 'tail' of the
//     // sequence of elseif branches:
//     if_e.set_test(elif_e.get_test());
//     if_e.set_cons(elif_e.get_cons());
//     if_e.set_elsif((Generic) elif_l.Tl());
//     if_e.set_altn(e.get_altn());
//     // Remember the context field. The context id of the first (Hd())
//     // elseif expression is used:
//     if_e.set_cid(elif_e.get_cid());
// 
//     // Now make the recursive call:
//     return ConcIfThenElse(cond_l, expr1_l, E2I(if_e));
//   }
// }
// 
// 
// SubProgram 
// StackCompiler::CompileUnaryExpr(const TYPE_AS_PrefixExpr &)
// {return SubProgram();}
// 
// 
// SubProgram 
// StackCompiler::CompileBinaryExpr(const TYPE_AS_BinaryExpr &e)
// {
//   Int q = e.get_opr();
//   switch(q.GetValue()) {
//   case AND:
//   case OR:
//   case IMPLY:
//   case EQUIV: 
//     return CompileLogBinaryExpr(e);
//   case COMPOSE:
//     return CompileComposeExpr(e);
//   case NUMEXP:
//     return CompileIterateExpr(e);
// 
//   case EQ:
//   case NE:
//   case NUMPLUS:   
//   case NUMMINUS:  
//   case NUMMULT:   
//   case NUMDIV:    
//   case NUMREM:    
//   case INTDIV:    
//   case NUMLT:     
//   case NUMLE:     
//   case NUMGT:     
//   case NUMGE:     
//   case NUMMOD:    
//   case INSET:     
//   case NOTINSET:  
//   case SETUNION:  
//   case SETINTERSECT:
//   case SETMINUS:  
//   case SUBSET:    
//   case PROPERSUBSET:
//   case SEQCONC:   
//   case MAPMERGE:  
//   case MAPDOMRESTTO:
//   case MAPDOMRESTBY:
//   case MAPRNGRESTTO:
//   case MAPRNGRESTBY: 
//     return CompileOrdinaryBinaryExpr(e);
//   }
//   return SubProgram(); // To keep VC++ happy.
// }
// 
// 
// SubProgram 
// StackCompiler::CompileLogBinaryExpr(const TYPE_AS_BinaryExpr &e)
// {return SubProgram();}
// 
// SubProgram 
// StackCompiler::CompileComposeExpr(const TYPE_AS_BinaryExpr &)
// {return SubProgram();}
// 
// SubProgram 
// StackCompiler::CompileIterateExpr(const TYPE_AS_BinaryExpr &)
// {return SubProgram();}
// 
// 
// SubProgram 
// StackCompiler::CompileOrdinaryBinaryExpr(const TYPE_AS_BinaryExpr &e)
// {
//   SubProgram p = E2I(e.get_left());
//   ConcSubPrograms(p, E2I(e.get_right()));
//   p.push_back(Instruction(INSTR::BINOP, e.get_opr()));
//   return p;
// }
// 
// 
// SubProgram 
// StackCompiler::CompileMapComprehensionExpr(const TYPE_AS_MapComprehensionExpr &)
// {return SubProgram();}
// }}}

// }}}
