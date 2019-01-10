// {{{ Header
/***
*  * WHAT
*  *    Implementation of module CSTMT
*  * ID
*  *    $Id: cstmt.cc,v 1.52 2006/09/23 10:58:18 vdmtools Exp $
*  * SPECIFICATION VERSION
*  *    cstmt.cc: Implementation of cstmt.vdm 1.22
***/

// }}}
// {{{ includes

#include "compile.h"
#include "INSTRTP.h"
#include "astaux.h"
#include "asquotes.h"
#include "pattern.h"
#include "semvalues.h"
#include "contextinfo.h"
#include "rt_errmsg.h"
#include "libman.h"
#include "settings.h"

void StackCompiler::Init_CSTMT()
{
  init_cstmt_lit();

  init_TRAP(); 
}

void StackCompiler::init_TRAP()
{
  this->trapno = 0;
}

void StackCompiler::init_cstmt_lit()
{
  nDefStmt = Sequence(L"DefStmt");
  nLetStmt = Sequence(L"LetStmt");
  nLetBeSTStmt = Sequence(L"LetBeSTStmt");
  nAssignStmt = Sequence(L"AssignStmt");
  nAtomicAssignStmt = Sequence(L"AtomicAssignStmt");
  nSeqForLoopStmt = Sequence(L"SeqForLoopStmt");
  nSetForLoopStmt = Sequence(L"SetForLoopStmt");
  nIndexForLoopStmt = Sequence(L"IndexForLoopStmt");
  nWhileLoopStmt = Sequence(L"WhileLoopStmt");
  nCallStmt = Sequence(L"CallStmt");
  nReturnStmt = Sequence(L"ReturnStmt");
  nIfStmt = Sequence(L"IfStmt");
  nCasesStmt = Sequence(L"CasesStmt");
  nErrorStmt = Sequence(L"ErrorStmt");
  nExitStmt = Sequence(L"ExitStmt");
  nAlwaysStmt = Sequence(L"AlwaysStmt");
  nTrapStmt = Sequence(L"TrapStmt");
  nRecTrapStmt = Sequence(L"RecTrapStmt");
  nBlockStmt = Sequence(L"BlockStmt");
  nNonDetStmt = Sequence(L"NonDetStmt");
  nAssertStmt = Sequence(L"AssertStmt");
  nIdent = Sequence(L"IdentStmt");
  nSpecificationStmt = Sequence(L"SpecificationStmt");
#ifdef VDMPP
  nStartStmt = Sequence(L"StartStmt");
  nStartListStmt = Sequence(L"StartListStmt");
  nStopStmt = Sequence(L"StopStmt");
  nStopListStmt = Sequence(L"StopListStmt");
#ifdef VICE
  nDurationStmt = Sequence(L"DurationStmt");
  nCycleStmt = Sequence(L"CycleStmt");
#endif // VICE
#endif // VDMPP
}

// }}}
// {{{ S2I
// S2I
// stmt : AS`Stmt
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::S2I(const TYPE_AS_Stmt& stmt)
{
  TYPE_STKM_SubProgram sp (SetContext(ASTAUX::GetCid(stmt), true));
  switch (stmt.GetTag()) {
    case TAG_TYPE_AS_DefStmt: {
      sp.ImpConc(IStart(nDefStmt, stmt.GetInt(pos_AS_DefStmt_cid)))
        .ImpConc(CompileDefStmt(stmt))
        .ImpConc(IEnd(nDefStmt));
      break;
    }
    case TAG_TYPE_AS_LetStmt: {
      sp.ImpConc(IStart(nLetStmt, stmt.GetInt(pos_AS_LetStmt_cid)))
        .ImpConc(CompileLetStmt(stmt))
        .ImpConc(IEnd(nLetStmt));
      break;
    }
    case TAG_TYPE_AS_LetBeSTStmt: {
      sp.ImpConc(IStart(nLetBeSTStmt, stmt.GetInt(pos_AS_LetBeSTStmt_cid)))
        .ImpConc(CompileLetBeSTStmt(stmt))
        .ImpConc(IEnd(nLetBeSTStmt));
      break;
    }
    case TAG_TYPE_AS_AssignStmt: {
      sp.ImpConc(IStart(nAssignStmt, stmt.GetInt(pos_AS_AssignStmt_cid)))
        .ImpConc(CompileAssignStmt(stmt))
        .ImpConc(IEnd(nAssignStmt));
      break;
    }
    case TAG_TYPE_AS_AtomicAssignStmt: {
      sp.ImpConc(IStart(nAtomicAssignStmt, stmt.GetInt(pos_AS_AtomicAssignStmt_cid)))
        .ImpConc(CompileAtomicAssignStmt(stmt))
        .ImpConc(IEnd(nAtomicAssignStmt));
      break;
    }
    case TAG_TYPE_AS_SeqForLoopStmt: {
      sp.ImpConc(IStart(nSeqForLoopStmt, stmt.GetInt(pos_AS_SeqForLoopStmt_cid)))
        .ImpConc(CompileSeqForLoopStmt(stmt))
        .ImpConc(IEnd(nSeqForLoopStmt));
      break;
    }
    case TAG_TYPE_AS_SetForLoopStmt: {
      sp.ImpConc(IStart(nSetForLoopStmt, stmt.GetInt(pos_AS_SetForLoopStmt_cid)))
        .ImpConc(CompileSetForLoopStmt(stmt))
        .ImpConc(IEnd(nSetForLoopStmt));
      break;
    }
    case TAG_TYPE_AS_IndexForLoopStmt: {
      sp.ImpConc(IStart(nIndexForLoopStmt, stmt.GetInt(pos_AS_IndexForLoopStmt_cid)))
        .ImpConc(CompileIndexForLoopStmt(stmt))
        .ImpConc(IEnd(nIndexForLoopStmt));
      break;
    }
    case TAG_TYPE_AS_WhileLoopStmt: {
      sp.ImpConc(IStart(nWhileLoopStmt, stmt.GetInt(pos_AS_WhileLoopStmt_cid)))
        .ImpConc(CompileWhileLoopStmt(stmt))
        .ImpConc(IEnd(nWhileLoopStmt));
      break;
    }
    case TAG_TYPE_AS_CallStmt: {
      sp.ImpConc(IStart(nCallStmt, stmt.GetInt(pos_AS_CallStmt_cid)))
        .ImpConc(CompileCallStmt(stmt))
        .ImpConc(IEnd(nCallStmt));
      break;
    }
    case TAG_TYPE_AS_ReturnStmt: {
      sp.ImpConc(IStart(nReturnStmt, stmt.GetInt(pos_AS_ReturnStmt_cid)))
        .ImpConc(CompileReturnStmt(stmt))
        .ImpConc(IEnd(nReturnStmt));
      break;
    }
    case TAG_TYPE_AS_IfStmt: {
      sp.ImpConc(IStart(nIfStmt, stmt.GetInt(pos_AS_IfStmt_cid)))
        .ImpConc(CompileIfStmt(stmt))
        .ImpConc(IEnd(nIfStmt));
      break;
    }
    case TAG_TYPE_AS_CasesStmt: {
      sp.ImpConc(IStart(nCasesStmt, stmt.GetInt(pos_AS_CasesStmt_cid)))
        .ImpConc(CompileCasesStmt(stmt))
        .ImpConc(IEnd(nCasesStmt));
      break;
    }
    case TAG_TYPE_AS_ErrorStmt: {
      sp.ImpConc(IStart(nErrorStmt, stmt.GetInt(pos_AS_ErrorStmt_cid)))
        .ImpConc(CompileErrorStmt(stmt))
        .ImpConc(IEnd(nErrorStmt));
      break;
    }
    case TAG_TYPE_AS_ExitStmt: {
      sp.ImpConc(IStart(nExitStmt, stmt.GetInt(pos_AS_ExitStmt_cid)))
        .ImpConc(CompileExitStmt(stmt))
        .ImpConc(IEnd(nExitStmt));
      break;
    }
    case TAG_TYPE_AS_AlwaysStmt: {
      sp.ImpConc(IStart(nAlwaysStmt, stmt.GetInt(pos_AS_AlwaysStmt_cid)))
        .ImpConc(CompileAlwaysStmt(stmt))
        .ImpConc(IEnd(nAlwaysStmt));
      break;
    }
    case TAG_TYPE_AS_TrapStmt: {
      sp.ImpConc(IStart(nTrapStmt, stmt.GetInt(pos_AS_TrapStmt_cid)))
        .ImpConc(CompileTrapStmt(stmt))
        .ImpConc(IEnd(nTrapStmt));
      break;
    }
    case TAG_TYPE_AS_RecTrapStmt: {
      sp.ImpConc(IStart(nRecTrapStmt, stmt.GetInt(pos_AS_RecTrapStmt_cid)))
        .ImpConc(CompileRecTrapStmt(stmt))
        .ImpConc(IEnd(nRecTrapStmt));
      break;
    }
#ifdef VDMPP
    case TAG_TYPE_AS_StartStmt: {
      sp.ImpConc(IStart(nStartStmt, stmt.GetInt(pos_AS_StartStmt_cid)))
        .ImpConc(CompileStartStmt(stmt))
        .ImpConc(IEnd(nStartStmt));
      break;
    }
    case TAG_TYPE_AS_StartListStmt: {
      sp.ImpConc(IStart(nStartListStmt, stmt.GetInt(pos_AS_StartListStmt_cid)))
        .ImpConc(CompileStartListStmt(stmt))
        .ImpConc(IEnd(nStartListStmt));
      break;
    }
    case TAG_TYPE_AS_StopStmt: {
      sp.ImpConc(IStart(nStopStmt, stmt.GetInt(pos_AS_StopStmt_cid)))
        .ImpConc(CompileStopStmt(stmt))
        .ImpConc(IEnd(nStopStmt));
      break;
    }
    case TAG_TYPE_AS_StopListStmt: {
      sp.ImpConc(IStart(nStopListStmt, stmt.GetInt(pos_AS_StopListStmt_cid)))
        .ImpConc(CompileStopListStmt(stmt))
        .ImpConc(IEnd(nStopListStmt));
      break;
    }
#ifdef VICE
    case TAG_TYPE_AS_DurationStmt: {
      sp.ImpConc(IStart(nDurationStmt, stmt.GetInt(pos_AS_DurationStmt_cid)))
        .ImpConc(CompileDurationStmt(stmt))
        .ImpConc(IEnd(nDurationStmt));
      break;
    }
    case TAG_TYPE_AS_CycleStmt: {
      sp.ImpConc(IStart(nCycleStmt, stmt.GetInt(pos_AS_CycleStmt_cid)))
        .ImpConc(CompileCycleStmt(stmt))
        .ImpConc(IEnd(nCycleStmt));
      break;
    }
#endif // VICE
#endif // VDMPP
    case TAG_TYPE_AS_BlockStmt: {
      sp.ImpConc(IStart(nBlockStmt, stmt.GetInt(pos_AS_BlockStmt_cid)))
        .ImpConc(CompileBlockStmt(stmt))
        .ImpConc(IEnd(nBlockStmt));
      break;
    }
    case TAG_TYPE_AS_NonDetStmt: {
      sp.ImpConc(IStart(nNonDetStmt, stmt.GetInt(pos_AS_NonDetStmt_cid)))
        .ImpConc(CompileNonDetStmt(stmt))
        .ImpConc(IEnd(nNonDetStmt));
      break;
    }
    case TAG_TYPE_AS_AssertStmt: {
      sp.ImpConc(IStart(nAssertStmt, stmt.GetInt(pos_AS_AssertStmt_cid)))
        .ImpConc(CompileAssertStmt(stmt))
        .ImpConc(IEnd(nAssertStmt));
      break;
    }
    case TAG_TYPE_AS_IdentStmt: {
      sp.ImpConc(IStart(nIdent, stmt.GetInt(pos_AS_IdentStmt_cid)))
        .ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_cont))
        .ImpConc(IEnd(nIdent));
      break;
    }
    case TAG_TYPE_AS_SpecificationStmt: {
      sp.ImpConc(IStart(nSpecificationStmt, stmt.GetInt(pos_AS_SpecificationStmt_cid)))
        .ImpAppend(TYPE_INSTRTP_ERRINST().Init(TYPE_RTERR_ERR(RTERR_SPEC_STMT)))
        .ImpConc(IEnd(nSpecificationStmt));
      break;
    }
  }
#ifdef VICE
  if (GetCompilingTime())
    sp.ImpConc(S2Time(stmt));
#endif //VICE
  return sp;
}

// }}}
// {{{ Def, Let, LetBeST

// CompileDefStmt
// sIn : AS`DefStmt
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileDefStmt(const TYPE_AS_DefStmt& sIn)
{
  // Unpack sIn
  const type_dd2PL & def_l (sIn.GetSequence(pos_AS_DefStmt_value));
  const TYPE_AS_Stmt & In  (sIn.GetRecord  (pos_AS_DefStmt_In));

  TYPE_STKM_SubProgram def_sp;
  // for loop
  size_t len_def_l = def_l.Length();
  for (size_t index = 1; index <= len_def_l; index++) {
    const Tuple & t (def_l[index]); // AS`PatternBind * AS`Expr
    def_sp.ImpConc(E2I(t.GetRecord(2)));  // AS`Expr
    const TYPE_AS_PatternBind & pb (t.GetRecord(1));
    if (pb.Is(TAG_TYPE_AS_PatternName)) {
      const Generic & nm (pb.GetField(pos_AS_PatternName_nm));
      if (nm.IsNil()) {
        def_sp.ImpAppend(TYPE_INSTRTP_POP().Init(Int(1))); 
      }
      else {
        def_sp.ImpAppend(TYPE_INSTRTP_APPENDBLKENV().Init(nm, Nil()));
      }
    }
    else {
      def_sp.ImpConc(PB2I(pb))  // AS`PatternBind
            .ImpAppend(TYPE_INSTRTP_MATCHANDBIND());
    }
  }

  TYPE_STKM_SubProgram sp;
  sp.ImpAppend(TYPE_INSTRTP_EMPTYBLKENV().Init(sem_read_only))
    .ImpConc(def_sp)
    .ImpConc(S2I(In))
    .ImpAppend(TYPE_INSTRTP_POPBLKENV());
  return sp;
}

// CompileLetStmt
// sIn : AS`LetStmt
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileLetStmt(const TYPE_AS_LetStmt& sIn)
{
  // Unpack sIn
  const SEQ<TYPE_AS_LocalDef> & localdef (sIn.GetSequence(pos_AS_LetStmt_localdef));
  const TYPE_AS_Stmt & In                (sIn.GetRecord  (pos_AS_LetStmt_In));

  TYPE_STKM_SubProgram def_sp;
  // for loop
  size_t len_localdef = localdef.Length();
  for (size_t idx = 1; idx <= len_localdef; idx++) {
    const TYPE_AS_LocalDef & ldef (localdef[idx]);
    switch(ldef.GetTag()) {
      case TAG_TYPE_AS_ValueDef: {
        const Generic & tp (ldef.GetField(pos_AS_ValueDef_tp));
        const TYPE_AS_Pattern & pat (ldef.GetRecord(pos_AS_ValueDef_pat));

        def_sp.ImpConc(E2I(ldef.GetRecord(pos_AS_ValueDef_val)));
        if (!tp.IsNil()) {
          def_sp.ImpAppend(TYPE_INSTRTP_DTC().Init(tp));
        }
        if (pat.Is(TAG_TYPE_AS_PatternName) && tp.IsNil()) {
          const Generic & nm (pat.GetField(pos_AS_PatternName_nm));
          if (nm.IsNil()) {
            def_sp.ImpAppend(TYPE_INSTRTP_POP().Init(Int(1)));
          }
          else {
            def_sp.ImpAppend(TYPE_INSTRTP_APPENDBLKENV().Init(nm, Nil()));
          }
        }
        else {
          def_sp.ImpConc(P2I(pat))
                .ImpAppend(TYPE_INSTRTP_MATCHANDBIND());
        }
        break;
      }
      //case TAG_TYPE_AS_ExplFnDef:
      //case TAG_TYPE_AS_ImplFnDef:
      //case TAG_TYPE_AS_ExtExplFnDef:
      default: {
        Tuple t1 (PAT::ConstructFN(GetClMod(), ldef));
        def_sp.ImpAppend(TYPE_INSTRTP_CLOSENV().Init(t1.GetRecord(1), t1.GetMap(2)));
        break;
      }
    }
  }

  TYPE_STKM_SubProgram sp;
  sp.ImpAppend(TYPE_INSTRTP_EMPTYBLKENV().Init(sem_read_only))
    .ImpConc(def_sp)
    .ImpConc(S2I(In))
    .ImpAppend(TYPE_INSTRTP_POPBLKENV());
  return sp;
}

// CompileLetBeSTStmt
// sIn : AS`LetBeSTStmt
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileLetBeSTStmt(const TYPE_AS_LetBeSTStmt & sIn)
{
  // Unpack sIn
  const SEQ<TYPE_AS_MultBind> & lhs (sIn.GetSequence(pos_AS_LetBeSTStmt_bind));
  const Generic & St       (sIn.GetField (pos_AS_LetBeSTStmt_St));
  const TYPE_AS_Expr & In  (sIn.GetRecord(pos_AS_LetBeSTStmt_In));

  // prep_instr
  TYPE_STKM_SubProgram prep_instr;
  prep_instr.ImpConc(CompileMultBindL(lhs, Int(DONT_PARTITION)));

  TYPE_STKM_SubProgram body_instr;
  if (St.IsNil()) {
    TYPE_STKM_SubProgram s1, s2, s3;
    s1.ImpAppend(TYPE_INSTRTP_ISEMPTYSET().Init(Int(1)));
    s2.ImpAppend(TYPE_INSTRTP_ERRINST().Init(TYPE_RTERR_ERR(RTERR_EMPTY_ENV_S)));
    s3.ImpAppend(TYPE_INSTRTP_SELBLKENV().Init(Int(1)))
      .ImpConc(S2I(In))
      .ImpAppend(TYPE_INSTRTP_POPBLKENV());
    body_instr.ImpConc(ConcIfThenElse(s1, s2, s3));
  }
  else {
    // St_instr
    TYPE_STKM_SubProgram St_instr;
    St_instr.ImpAppend(TYPE_INSTRTP_SELBLKENV().Init(Int(1)))
            .ImpConc(E2I(St));

    // St_succ
    TYPE_STKM_SubProgram St_succ;
    St_succ.ImpConc(S2I(In))
           .ImpAppend(TYPE_INSTRTP_POPBLKENV())
           .ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_true));

    // St_failed
    TYPE_STKM_SubProgram St_failed;
    St_failed.ImpAppend(TYPE_INSTRTP_POPBLKENV())
             .ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_false));

    TYPE_STKM_SubProgram s1, s2;
    s1.ImpAppend(TYPE_INSTRTP_ISEMPTYSET().Init(Int(1)));
    s2.ImpAppend(TYPE_INSTRTP_ERRINST().Init(TYPE_RTERR_ERR(RTERR_EMPTY_ENV_S)));

    TYPE_STKM_SubProgram loop_instr;
    loop_instr.ImpConc(ConcIfThenElse(s1, s2, ConcIfThenElse(St_instr, St_succ, St_failed)));

    body_instr.ImpConc(CombRepeatUntil(loop_instr));
  }

  // postt_instr
  TYPE_STKM_SubProgram postt_instr;
  postt_instr.ImpAppend(TYPE_INSTRTP_REMSTACKELEM().Init(Int(2))); // remove set of env

  // return
  TYPE_STKM_SubProgram sp;
  sp.ImpConc(prep_instr)
    .ImpConc(body_instr)
    .ImpConc(postt_instr);

  return sp;
}

// }}}
// {{{ Loop statements

// CompileSeqForLoopStmt
// sIn : AS`SeqForLoopStmt
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileSeqForLoopStmt(const TYPE_AS_SeqForLoopStmt& sIn)
{
  // Unpack sIn
  const TYPE_AS_PatternBind & cv (sIn.GetRecord(pos_AS_SeqForLoopStmt_cv));
  TYPE_AS_Expr fseq              (sIn.GetRecord(pos_AS_SeqForLoopStmt_fseq));
  const TYPE_AS_Stmt & body      (sIn.GetRecord(pos_AS_SeqForLoopStmt_body));

  if (Settings.OldReverse() && fseq.Is(TAG_TYPE_AS_BinaryExpr)) {
    if (Int(SEQCONC) == fseq.GetField(pos_AS_BinaryExpr_opr)) {
      const TYPE_AS_Expr & left (fseq.GetRecord(pos_AS_BinaryExpr_left));
      if (left.Is(TAG_TYPE_AS_PrefixExpr)) {
        if (Int(SEQREVERSE) == left.GetField(pos_AS_PrefixExpr_opr)) {
          TYPE_AS_BinaryExpr be (fseq);
          TYPE_AS_PrefixExpr pe (left); 
          be.SetField(pos_AS_BinaryExpr_left, left.GetRecord(pos_AS_PrefixExpr_arg));
          pe.SetField(pos_AS_PrefixExpr_arg, be);
          fseq = pe;
        }
      } 
    }
  }

  // prep_instr is written directly to prog
  TYPE_STKM_SubProgram prep_instr;
  prep_instr.ImpConc(E2I(fseq))
            .ImpAppend(TYPE_INSTRTP_SEQFOR())
            .ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_cont));

  // bind_pat
  TYPE_STKM_SubProgram bind_pat;
  bind_pat.ImpAppend(TYPE_INSTRTP_SELSEQELEM())
          .ImpConc(PB2I(cv))
          .ImpAppend(TYPE_INSTRTP_MATCHANDBIND());
#ifdef VICE
  bind_pat.ImpConc(MkLoopBind(1));
#endif //VICE

  // remove_last_res
  TYPE_STKM_SubProgram remove_last_res;
  remove_last_res.ImpAppend(TYPE_INSTRTP_POP().Init(Int(1)));

  // loop_body
  TYPE_STKM_SubProgram loop_body;
  loop_body.ImpConc(remove_last_res)
           .ImpAppend(TYPE_INSTRTP_EMPTYBLKENV().Init(sem_read_only))
           .ImpConc(bind_pat)
           .ImpConc(S2I(body))
           .ImpAppend(TYPE_INSTRTP_POPBLKENV());
#ifdef VICE
  loop_body.ImpConc(MkCbr());
#endif //VICE

  // condition
  TYPE_STKM_SubProgram c1, c2, c3, condition;
  c1.ImpAppend(TYPE_INSTRTP_ISCONT());
  c2.ImpAppend(TYPE_INSTRTP_ISNOTEMPTYSEQ().Init(Int(2)));
  c3.ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_false));
  condition.ImpConc(ConcIfThenElse(c1, c2, c3));

  // clean_up
  TYPE_STKM_SubProgram clean_up;
  clean_up.ImpAppend(TYPE_INSTRTP_REMSTACKELEM().Init(Int(2)));
  
  // return
  TYPE_STKM_SubProgram sp;
  sp.ImpConc(prep_instr)
    .ImpConc(CombWhileLoopWithCond(condition, loop_body))
    .ImpConc(clean_up);
  return sp;
}

// CompileSetForLoopStmt
// sIn : AS`SetForLoopStmt
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileSetForLoopStmt(const TYPE_AS_SetForLoopStmt& sIn)
{
  // Unpack sIn
  const TYPE_AS_Pattern & cv   (sIn.GetRecord(pos_AS_SetForLoopStmt_cv));
  const TYPE_AS_Expr & fset    (sIn.GetRecord(pos_AS_SetForLoopStmt_fset));
  const TYPE_AS_Stmt & body    (sIn.GetRecord(pos_AS_SetForLoopStmt_body));
  const TYPE_CI_ContextId & ci (sIn.GetInt   (pos_AS_SetForLoopStmt_cid));

  // bind_l
  SEQ<TYPE_AS_MultBind> bind_l;
  TYPE_AS_Pattern new_cv (PAT::DoCarePattern(cv, ASTAUX::MkNameFromId(SEQ<Char>(L"1"), NilContextId)));
  bind_l.ImpAppend(TYPE_AS_MultSetBind().Init(SEQ<TYPE_AS_Pattern>().ImpAppend(new_cv), fset, ci));

  // prep_instr
  TYPE_STKM_SubProgram prep_instr;
  prep_instr.ImpConc(CompileMultBindL(bind_l, Int(DO_PARTITION)))
            .ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_cont));
#ifdef VICE
  prep_instr.ImpConc(MkLoopBind(1));
#endif //VICE

  // remove_last_res
  TYPE_STKM_SubProgram remove_last_res;
  remove_last_res.ImpAppend(TYPE_INSTRTP_POP().Init(Int(1)));

  // loop_body
  TYPE_STKM_SubProgram loop_body;
  loop_body.ImpConc(remove_last_res)
           .ImpAppend(TYPE_INSTRTP_SELBLKENV().Init(Int(1)))
           .ImpConc(S2I(body))
           .ImpAppend(TYPE_INSTRTP_POPBLKENV());
#ifdef VICE
  loop_body.ImpConc(MkCbr());
#endif //VICE

  // condition
  TYPE_STKM_SubProgram c1, c2, c3, condition;
  c1.ImpAppend(TYPE_INSTRTP_ISCONT());
  c2.ImpAppend(TYPE_INSTRTP_ISNOTEMPTYSET().Init(Int(2)));
  c3.ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_false));
  condition.ImpConc(ConcIfThenElse(c1, c2, c3));

  // clean_up
  TYPE_STKM_SubProgram clean_up;
  clean_up.ImpAppend(TYPE_INSTRTP_REMSTACKELEM().Init(Int(2)));
  
  // return
  TYPE_STKM_SubProgram sp;
  sp.ImpConc(prep_instr)
    .ImpConc(CombWhileLoopWithCond(condition, loop_body))
    .ImpConc(clean_up);
  return sp;
}

// CompileIndexForLoopStmt
// sIn : AS`IndexForLoopStmt
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileIndexForLoopStmt(const TYPE_AS_IndexForLoopStmt& sIn)
{
  // Unpack sIn
  const TYPE_AS_Name & cv   (sIn.GetRecord(pos_AS_IndexForLoopStmt_cv));
  const TYPE_AS_Expr & lb_e (sIn.GetRecord(pos_AS_IndexForLoopStmt_lb));
  const TYPE_AS_Expr & ub_e (sIn.GetRecord(pos_AS_IndexForLoopStmt_ub));
  const Generic & by_e      (sIn.GetField (pos_AS_IndexForLoopStmt_By));
  const TYPE_AS_Stmt & body (sIn.GetRecord(pos_AS_IndexForLoopStmt_body));

  // step_e
  TYPE_STKM_SubProgram step_e;
  if (by_e.IsNil()) {
    step_e.ImpAppend(TYPE_INSTRTP_PUSH().Init(TYPE_SEM_NUM().Init(Real(1))));
  }
  else {
    step_e.ImpConc(E2I(by_e));
  }
  // prep_instr
  TYPE_STKM_SubProgram prep_instr;
  prep_instr.ImpConc(step_e)     // step
            .ImpConc(E2I(ub_e))  // last
            .ImpConc(E2I(lb_e))  // step
            .ImpAppend(TYPE_INSTRTP_VERIFYINDEXARGS())
            .ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_cont)); // val

  // bindArgs
  TYPE_STKM_SubProgram bindArgs;
  //bindArgs.ImpAppend(TYPE_INSTRTP_COPYVAL())
  //        .ImpAppend(TYPE_INSTRTP_PUSH().Init(TYPE_STKM_PatternName().Init(cv, Nil(), cv.GetInt(pos_AS_Name_cid))))
  //        .ImpAppend(TYPE_INSTRTP_MATCHANDBIND());
  bindArgs.ImpAppend(TYPE_INSTRTP_COPYVAL())
          .ImpAppend(TYPE_INSTRTP_APPENDBLKENV().Init(cv, Nil()));
#ifdef VICE
  bindArgs.ImpConc(MkLoopBind(1));
#endif //VICE

  // remove_last_res
  TYPE_STKM_SubProgram remove_last_res;
  remove_last_res.ImpAppend(TYPE_INSTRTP_POP().Init(Int(1)));

  // loop
  TYPE_STKM_SubProgram loop_body;
  loop_body.ImpConc(remove_last_res)
           .ImpAppend(TYPE_INSTRTP_EMPTYBLKENV().Init(sem_read_only))
           .ImpConc(bindArgs)
           .ImpConc(S2I(body))
           .ImpAppend(TYPE_INSTRTP_POPBLKENV())
           .ImpAppend(TYPE_INSTRTP_INCRCOUNTER());
#ifdef VICE
  loop_body.ImpConc(MkCbr());
#endif //VICE

  // condition
  TYPE_STKM_SubProgram c1, c2, c3, condition;
  c1.ImpAppend(TYPE_INSTRTP_ISCONT());
  c2.ImpAppend(TYPE_INSTRTP_TESTCOUNTER());
  c3.ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_false));
  condition.ImpConc(ConcIfThenElse(c1, c2, c3));

  // cleanUp
  TYPE_STKM_SubProgram clean_up;
  clean_up.ImpAppend(TYPE_INSTRTP_REMSTACKELEM().Init(Int(2)))    // lb
          .ImpAppend(TYPE_INSTRTP_REMSTACKELEM().Init(Int(2)))    // ub
          .ImpAppend(TYPE_INSTRTP_REMSTACKELEM().Init(Int(2)));   // step

  // return
  TYPE_STKM_SubProgram sp;
  sp.ImpConc(prep_instr)
    .ImpConc(CombWhileLoopWithCond(condition, loop_body))
    .ImpConc(clean_up);
  return sp;
}

// CompileWhileLoopStmt
// sIn : AS`WhileLoopStmt
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileWhileLoopStmt(const TYPE_AS_WhileLoopStmt& sIn)
{
  // Unpack sIn
  const TYPE_AS_Expr & test (sIn.GetRecord(pos_AS_WhileLoopStmt_test));
  const TYPE_AS_Stmt & body (sIn.GetRecord(pos_AS_WhileLoopStmt_body));

  // prep_instr
  TYPE_STKM_SubProgram prep_instr;
  prep_instr.ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_cont));

  // remove_last_res
  TYPE_STKM_SubProgram remove_last_res;
  remove_last_res.ImpAppend(TYPE_INSTRTP_POP().Init(Int(1)));

  // loop_body
  TYPE_STKM_SubProgram loop_body;
  loop_body.ImpConc(remove_last_res)
           .ImpConc(S2I(body));
#ifdef VICE
  loop_body.ImpConc(MkCbr());
#endif //VICE

  // condition
  TYPE_STKM_SubProgram condition, c1, c2, c3, err, check;
  c1.ImpAppend(TYPE_INSTRTP_ISCONT());
  err.ImpAppend(TYPE_INSTRTP_ERRINST().Init(TYPE_RTERR_ERR(RTERR_LOOP_EXPR_NOT_AN_EXPR)));
  check.ImpAppend(TYPE_INSTRTP_COPYVAL())
       .ImpAppend(TYPE_INSTRTP_ISCHECK().Init(TYPE_AS_BooleanType().Init(NilContextId)));
  c2.ImpConc(E2I(test))
    .ImpConc(ConcIfThenElse(check, TYPE_STKM_SubProgram(), err));
  c3.ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_false));
  condition.ImpConc(ConcIfThenElse(c1, c2, c3));

  // return
  TYPE_STKM_SubProgram sp;
  sp.ImpConc(prep_instr)
    .ImpConc(CombWhileLoopWithCond(condition, loop_body));
  return sp;
}

// }}}
// {{{ Call, Return

#ifdef VDMSL
// CompileCallStmt
// sIn : AS`CallStmt
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileCallStmt(const TYPE_AS_CallStmt& sIn)
{
  // Unpack sIn
  const TYPE_AS_Name & oprt      (sIn.GetRecord  (pos_AS_CallStmt_oprt));
  const SEQ<TYPE_AS_Expr> & args (sIn.GetSequence(pos_AS_CallStmt_args));

  // Body of function
  TYPE_STKM_SubProgram sp;
  sp.ImpConc(E2I(oprt));

  // for loop
  size_t len = args.Length();
  for (size_t i = 1; i <= len; i++) {
    sp.ImpConc(E2I(args[i]));
  }
  sp.ImpAppend(TYPE_INSTRTP_PUSHLIST().Init(Int(len)));
    
  sp.ImpConc(SetContext(oprt.GetInt(pos_AS_Name_cid), false))
    .ImpAppend(TYPE_INSTRTP_APPLY());

  return sp;
}
#endif //VDMSL

#ifdef VDMPP
// CompileCallStmt
// sIn : AS`CallStmt
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileCallStmt(const TYPE_AS_CallStmt& sIn)
{
  // Unpack sIn
  const Generic & obj            (sIn.GetField   (pos_AS_CallStmt_obj));
  const TYPE_AS_Name & oprt      (sIn.GetRecord  (pos_AS_CallStmt_oprt));
  const SEQ<TYPE_AS_Expr> & args (sIn.GetSequence(pos_AS_CallStmt_args));

  // Body of function
  TYPE_STKM_SubProgram sp;
  if (!obj.IsNil()) {
    sp.ImpConc(E2I(obj));
  }
  // for loop
  size_t len = args.Length();
  for (size_t i = 1; i <= len; i++) {
    sp.ImpConc(E2I(args[i]));
  }
  sp.ImpAppend(TYPE_INSTRTP_PUSHLIST().Init(Int(len)));
    
  sp.ImpAppend(TYPE_INSTRTP_CALLGUARD().Init(Bool(!obj.IsNil()), oprt))
    .ImpConc(SetContext(oprt.GetInt(pos_AS_Name_cid), false))
    .ImpAppend(TYPE_INSTRTP_PPCALL());
  return sp;
}
#endif //VDMPP

// CompileReturnStmt
// sIn : AS`ReturnStmt
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileReturnStmt(const TYPE_AS_ReturnStmt& sIn)
{
  // Unpack sIn
  const Generic & expr (sIn.GetField(pos_AS_ReturnStmt_val));

  // body of function
  if (expr.IsNil()) {
    TYPE_STKM_SubProgram sp;
    sp.ImpAppend(TYPE_INSTRTP_PUSH().Init(TYPE_SEM_RETURN()));
    return sp;
  }
  else {
    TYPE_STKM_SubProgram sp;
    sp.ImpConc(E2I(expr))
      .ImpConc(SetContext(sIn.GetInt(pos_AS_ReturnStmt_cid), true));
    return sp;
  }
}

// }}}
// {{{ If, Cases

// CompileIfStmt
// sIn : AS`IfStmt
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileIfStmt(const TYPE_AS_IfStmt& sIn)
{
  // Unpack sIn
  const TYPE_AS_Expr & cond_e            (sIn.GetRecord  (pos_AS_IfStmt_test));
  const TYPE_AS_Stmt & cons_s            (sIn.GetRecord  (pos_AS_IfStmt_cons));
  const SEQ<TYPE_AS_ElseifStmt> & elif_l (sIn.GetSequence(pos_AS_IfStmt_elsif));
  const Generic & altn_s                 (sIn.GetField   (pos_AS_IfExpr_altn));

  // cond_l
  TYPE_STKM_SubProgram cond_l, s1;
  s1.ImpAppend(TYPE_INSTRTP_ERRINST().Init(TYPE_RTERR_ERR(RTERR_TEST_EXPR_NOT_AN_EXPR)));
  cond_l.ImpConc(E2I(cond_e))
        .ImpAppend(TYPE_INSTRTP_COPYVAL())
        .ImpAppend(TYPE_INSTRTP_ISCHECK().Init(TYPE_AS_BooleanType().Init(NilContextId)))
        .ImpConc(ConcIfThenElse(TYPE_STKM_SubProgram(), TYPE_STKM_SubProgram(), s1));

  // cons_l
  TYPE_STKM_SubProgram cons_l (S2I(cons_s));
#ifdef VICE
  cons_l.ImpConc(MkCbr());
#endif //VICE

  if (elif_l.IsEmpty()) {
    TYPE_STKM_SubProgram altn_l;
    if (!altn_s.IsNil()) {
      altn_l.ImpConc(S2I(altn_s));
#ifdef VICE
      altn_l.ImpConc(MkBr());
#endif //VICE
    }
    else {
      altn_l.ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_cont));
    }
    return ConcIfThenElse(cond_l, cons_l, altn_l);
  }
  else {
    // Unpack the first element of elif_l
    TYPE_AS_ElseifStmt elm (elif_l.Hd());
    const TYPE_AS_Expr & elif_cond     (elm.GetRecord(pos_AS_ElseifStmt_test));
    const TYPE_AS_Stmt & elif_s        (elm.GetRecord(pos_AS_ElseifStmt_cons));
    const TYPE_CI_ContextId & elif_cid (elm.GetInt   (pos_AS_ElseifExpr_cid));

    // altn_l
    TYPE_STKM_SubProgram altn_l;
    altn_l.ImpConc(CompileIfStmt(TYPE_AS_IfStmt().Init(elif_cond, elif_s, elif_l.Tl(), altn_s, elif_cid)));
#ifdef VICE
    altn_l.ImpConc(MkCbr());
#endif //VICE
    return ConcIfThenElse(cond_l, cons_l, altn_l);
  }
}

// CompileCasesStmt
// sIn : AS`CasesStmt
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileCasesStmt(const TYPE_AS_CasesStmt& sIn)
{
  // Unpack sIn
  const TYPE_AS_Expr & sel                 (sIn.GetRecord  (pos_AS_CasesStmt_sel));
  const SEQ<TYPE_AS_CasesStmtAltn> & altns (sIn.GetSequence(pos_AS_CasesStmt_altns));
  const Generic & others_s                 (sIn.GetField   (pos_AS_CasesExpr_Others));

  TYPE_STKM_SubProgram sp;
  if (others_s.IsNil()) {
    sp.ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_cont));
  }
  else {
    sp.ImpConc(S2I(others_s));
#ifdef VICE
    sp.ImpConc(MkBr());
#endif //VICE
  }

//#define MATCHUNIQUE
#ifdef MATCHUNIQUE
  TYPE_STKM_SubProgram tsp;
  tsp.ImpAppend(TYPE_INSTRTP_SWAP());
  TYPE_STKM_SubProgram err_sp;
  err_sp.ImpAppend(TYPE_INSTRTP_ERRINST().Init(TYPE_RTERR_ERR(RTERR_MULTIPLE_PATTERN)));
#endif // MATCHUNIQUE

  // run though altns in revers order
  for (size_t i = altns.Length(); i > 0; i--) {
    // Unpack the element
    TYPE_AS_CasesStmtAltn elm (altns[i]);
    const SEQ<TYPE_AS_Pattern> & match_lp (elm.GetSequence(pos_AS_CasesStmtAltn_match));
    const TYPE_AS_Stmt & body_s (elm.GetRecord(pos_AS_CasesStmtAltn_body));

    // body_instr
    TYPE_STKM_SubProgram body_instr;
    body_instr.ImpAppend(TYPE_INSTRTP_SELBLKENV().Init(Int(1)))
              .ImpAppend(TYPE_INSTRTP_POP().Init(Int(1)))
              .ImpConc(S2I(body_s))
              .ImpAppend(TYPE_INSTRTP_POPBLKENV());
#ifdef VICE
    body_instr.ImpConc(MkCbr());
#endif //VICE

    bool onep = (match_lp.Length() == 1);
    TYPE_STKM_SubProgram lsp;
    if (onep) {
      lsp.ImpConc(sp);
    }
    else {
      lsp.ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_false));
    }

    // run though match_lp in reverse order
    for (size_t j = match_lp.Length(); j > 0; j--) {
      const TYPE_AS_Pattern & pat (match_lp[j]);

      // test_instr
      TYPE_STKM_SubProgram test_instr;
      test_instr.ImpAppend(TYPE_INSTRTP_COPYVAL())
                .ImpConc(P2I(pat))
                .ImpAppend(TYPE_INSTRTP_TRYANYMATCH());
#ifdef VICE
      test_instr.ImpConc(MkMatchPattern());
      lsp.ImpConc(MkBr());
#endif //VICE

#ifdef MATCHUNIQUE
      TYPE_STKM_SubProgram body_instr2, check_multi_match;
      check_multi_match.ImpAppend(TYPE_INSTRTP_SWAP())
                       .ImpConc(tsp);
      tsp = ConcIfThenElse(test_instr, err_sp, tsp);

      body_instr2.ImpConc(ConcIfThenElse(TYPE_STKM_SubProgram().ImpAppend(TYPE_INSTRTP_CASES()),
                                         check_multi_match, TYPE_STKM_SubProgram()))
                 .ImpConc(SetContext(ASTAUX::GetCid(pat), true));

      if (onep) {
        body_instr2.ImpConc(body_instr);
      }
      else {
        body_instr2.ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_true));
      }
      lsp = ConcIfThenElse(test_instr, body_instr2, lsp);
#else
      if (onep) {
        lsp = ConcIfThenElse(test_instr, body_instr, lsp);
      }
      else {
        lsp = ConcIfThenElse(test_instr, mk_sequence(TYPE_INSTRTP_PUSH().Init(sem_true)), lsp);
      }
#endif // MATCHUNIQUE
    }
    if (!onep) {
      lsp.ImpConc(ConcIfThenElse(TYPE_STKM_SubProgram(), body_instr, sp));
    }
    sp = lsp;
  }

  // return
  TYPE_STKM_SubProgram prog;
  prog.ImpConc(E2I(sel))
      .ImpConc(sp)
      .ImpAppend(TYPE_INSTRTP_REMSTACKELEM().Init(Int(2)));  // remove sel
  return prog;
}

// }}}
// {{{ Exception Handling
// CompileErrorStmt
// sIn : AS`ErrorStmt
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileErrorStmt(const TYPE_AS_ErrorStmt& sIn) const
{
  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(TYPE_INSTRTP_ERRINST().Init(TYPE_RTERR_ERR(RTERR_ERROR_STATEMENT)));
  return prog;
}

// CompileExitStmt
// sIn : AS`ExitStmt
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileExitStmt(const TYPE_AS_ExitStmt& sIn)
{
  // Unpack sIn
  const Generic & expr (sIn.GetField(pos_AS_ExitStmt_expr));
  const TYPE_CI_ContextId & cid (sIn.GetInt(pos_AS_ExitStmt_cid));

  // Body of program
  TYPE_STKM_SubProgram prog;
  if (expr.IsNil()) {
    prog.ImpAppend(TYPE_INSTRTP_MKEXIT().Init(Bool(true), cid))
        .ImpAppend(TYPE_INSTRTP_EXITVAL());
  }
  else {
    prog.ImpConc(E2I(expr))
        .ImpAppend(TYPE_INSTRTP_MKEXIT().Init(Bool(false), cid))
        .ImpAppend(TYPE_INSTRTP_EXITVAL());
  }
  return prog;
}

// CompileAlwaysStmt
// sIn : AS`AlwaysStmt
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileAlwaysStmt(const TYPE_AS_AlwaysStmt & sIn)
{
  // Unpack sIn;
  const TYPE_AS_Stmt & Post (sIn.GetRecord(pos_AS_AlwaysStmt_Post));
  const TYPE_AS_Stmt & body (sIn.GetRecord(pos_AS_AlwaysStmt_body));

  // b_instr
  TYPE_STKM_SubProgram b_instr (S2I(body));

  // p_instr
  TYPE_STKM_SubProgram p_instr (S2I(Post));

  // ret_instr
  TYPE_STKM_SubProgram prog;
  prog.ImpConc(AddTrap(b_instr, TYPE_STKM_SubProgram()))
      .ImpConc(p_instr)
      .ImpConc(ConcIfThenElse(TYPE_STKM_SubProgram().ImpAppend(TYPE_INSTRTP_ISCONT()),
                              TYPE_STKM_SubProgram().ImpAppend(TYPE_INSTRTP_POP().Init(Int(1))),
                              TYPE_STKM_SubProgram()))
      .ImpConc(ConcIfThenElse(TYPE_STKM_SubProgram().ImpAppend(TYPE_INSTRTP_ISEXIT()),
                              TYPE_STKM_SubProgram().ImpAppend(TYPE_INSTRTP_EXITVAL()),
                              TYPE_STKM_SubProgram()));
  return prog;
}

// CompileTrapStmt
// sIn : AS`TrapStmt
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileTrapStmt(const TYPE_AS_TrapStmt & sIn)
{
  // Unpack sIn
  const TYPE_AS_PatternBind & pb (sIn.GetRecord(pos_AS_TrapStmt_pat));
  const TYPE_AS_Stmt & Post      (sIn.GetRecord(pos_AS_TrapStmt_Post));
  const TYPE_AS_Stmt & body      (sIn.GetRecord(pos_AS_TrapStmt_body));

  // b_instr
  TYPE_STKM_SubProgram b_instr (S2I(body));

  // p_instr
  TYPE_STKM_SubProgram p_instr;
  p_instr.ImpAppend(TYPE_INSTRTP_SELBLKENV().Init(Int(1)))
         .ImpAppend(TYPE_INSTRTP_POP().Init(Int(2)))  // remove sem val & set of env
         .ImpConc(S2I(Post))
         .ImpAppend(TYPE_INSTRTP_POPBLKENV());

  // bind_succ, bind_fail
  TYPE_STKM_SubProgram bind_succ, bind_fail;
  bind_succ.ImpAppend(TYPE_INSTRTP_TRYANYMATCH());
  bind_fail.ImpAppend(TYPE_INSTRTP_POP().Init(Int(2)))
           .ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_false));

  // pat_instr
  TYPE_STKM_SubProgram pat_instr;
  pat_instr.ImpAppend(TYPE_INSTRTP_REMEXITVAL());
  switch(pb.GetTag()) {
    case TAG_TYPE_AS_TypeBind: {
      pat_instr.ImpAppend(TYPE_INSTRTP_COPYVAL())
               .ImpConc(P2I(pb.GetRecord(pos_AS_TypeBind_pat)))
               .ImpAppend(TYPE_INSTRTP_SWAP())
               .ImpAppend(TYPE_INSTRTP_TYPEJUDGE().Init(pb.GetRecord(pos_AS_TypeBind_tp)));
      break;
    }
    case TAG_TYPE_AS_SetBind: {
      pat_instr.ImpAppend(TYPE_INSTRTP_COPYVAL())
               .ImpConc(P2I(pb.GetRecord(pos_AS_SetBind_pat)))
               .ImpAppend(TYPE_INSTRTP_SWAP())
               .ImpConc(E2I(pb.GetRecord(pos_AS_SetBind_Set)))
               .ImpAppend(TYPE_INSTRTP_BINOP().Init(Int(INSET)));
      break;
    }
    case TAG_TYPE_AS_SeqBind: {
      TYPE_AS_Expr set_e (TYPE_AS_PrefixExpr().Init (Int(SEQELEMS),
                                                     pb.GetRecord(pos_AS_MultSeqBind_Seq),
                                                     pb.GetInt(pos_AS_MultSeqBind_cid)));

      pat_instr.ImpAppend(TYPE_INSTRTP_COPYVAL())
               .ImpConc(P2I(pb.GetRecord(pos_AS_SeqBind_pat)))
               .ImpAppend(TYPE_INSTRTP_SWAP())
               .ImpConc(E2I(set_e))
               .ImpAppend(TYPE_INSTRTP_BINOP().Init(Int(INSET)));
      break;
    }
    default: {
      pat_instr.ImpConc(P2I(pb))
               .ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_true));
      break;
    }
  }

  // sp_exit
  TYPE_STKM_SubProgram sp_exit;
  sp_exit.ImpAppend(TYPE_INSTRTP_EXITVAL());

  // test_instr
  TYPE_STKM_SubProgram test_instr;
  test_instr//.ImpAppend(TYPE_INSTRTP_REMEXITVAL())
            .ImpConc(ConcIfThenElse(pat_instr, bind_succ, bind_fail));

  // han_instr
  TYPE_STKM_SubProgram han_instr;
  han_instr.ImpConc(ConcIfThenElse(test_instr, p_instr, sp_exit));

  // use_handler
  TYPE_STKM_SubProgram use_handler, s3;
  s3.ImpAppend(TYPE_INSTRTP_ISNEEXIT());
  use_handler.ImpConc(ConcIfThenElse(s3, han_instr, sp_exit));

  // part of return
  return AddTrap(b_instr, use_handler);
}

// CompileRecTrapStmt
// sIn : AS`RecTrapStmt
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileRecTrapStmt(const TYPE_AS_RecTrapStmt & sIn)
{
  // Unpack sIn
  const SEQ<TYPE_AS_Trap> & traps (sIn.GetSequence(pos_AS_RecTrapStmt_traps));
  const TYPE_AS_Stmt & body       (sIn.GetRecord  (pos_AS_RecTrapStmt_body));

  // b_instr
  TYPE_STKM_SubProgram b_instr;
  b_instr.ImpConc(S2I(body));

  // bind_succ, bind_fail
  TYPE_STKM_SubProgram bind_succ, bind_fail;
  bind_succ.ImpAppend(TYPE_INSTRTP_TRYANYMATCH());
  bind_fail.ImpAppend(TYPE_INSTRTP_POP().Init(Int(2)))
           .ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_false));

  // init of sp
  TYPE_STKM_SubProgram sp;
  sp.ImpAppend(TYPE_INSTRTP_POPTH())
    .ImpAppend(TYPE_INSTRTP_EXITVAL());

  // run through traps in reverse order
  size_t len_traps = traps.Length();
  for (int j = len_traps; j >= 1; j--) {
    TYPE_AS_Trap trap (traps[j]);                  // AS`Trap
    const TYPE_AS_PatternBind & pb (trap.GetRecord(pos_AS_Trap_match));
    const TYPE_AS_Stmt & Post      (trap.GetRecord(pos_AS_Trap_trappost));

    // p_instr
    TYPE_STKM_SubProgram p_instr;
    p_instr.ImpAppend(TYPE_INSTRTP_SELBLKENV().Init(Int(1)))
           .ImpAppend(TYPE_INSTRTP_POP().Init(Int(2)))
           .ImpConc(S2I(Post))
           .ImpAppend(TYPE_INSTRTP_POPBLKENV());

    // pat_instr
    TYPE_STKM_SubProgram pat_instr;
    pat_instr.ImpAppend(TYPE_INSTRTP_REMEXITVAL());
    switch(pb.GetTag()) {
      case TAG_TYPE_AS_TypeBind: {
        pat_instr.ImpAppend(TYPE_INSTRTP_COPYVAL())
                 .ImpConc(P2I(pb.GetRecord(pos_AS_TypeBind_pat)))
                 .ImpAppend(TYPE_INSTRTP_SWAP())
                 .ImpAppend(TYPE_INSTRTP_TYPEJUDGE().Init(pb.GetRecord(pos_AS_TypeBind_tp)));
        break;
      }
      case TAG_TYPE_AS_SetBind: {
        pat_instr.ImpAppend(TYPE_INSTRTP_COPYVAL())
                 .ImpConc(P2I(pb.GetRecord(pos_AS_SetBind_pat)))
                 .ImpAppend(TYPE_INSTRTP_SWAP())
                 .ImpConc(E2I(pb.GetRecord(pos_AS_SetBind_Set)))
                 .ImpAppend(TYPE_INSTRTP_BINOP().Init(Int(INSET)));
        break;
      }
      case TAG_TYPE_AS_SeqBind: {
        TYPE_AS_Expr set_e (TYPE_AS_PrefixExpr().Init (Int(SEQELEMS),
                                                       pb.GetRecord(pos_AS_MultSeqBind_Seq),
                                                       pb.GetInt(pos_AS_MultSeqBind_cid)));
        pat_instr.ImpAppend(TYPE_INSTRTP_COPYVAL())
                 .ImpConc(P2I(pb.GetRecord(pos_AS_SeqBind_pat)))
                 .ImpAppend(TYPE_INSTRTP_SWAP())
                 .ImpConc(E2I(set_e))
                 .ImpAppend(TYPE_INSTRTP_BINOP().Init(Int(INSET)));
        break;
      }
      default: {
        pat_instr.ImpConc(P2I(pb))
                 .ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_true));
        break;
      }      
    }

    // test_instr
    TYPE_STKM_SubProgram test_instr;
    test_instr.ImpConc(ConcIfThenElse(pat_instr, bind_succ, bind_fail));

    // update sp
    sp = ConcIfThenElse(test_instr, p_instr, sp);
  }

  // loop_instr
  TYPE_STKM_SubProgram s1, s2;
  s1.ImpAppend(TYPE_INSTRTP_ISNEEXIT());
  s2.ImpAppend(TYPE_INSTRTP_POPTH())
    .ImpAppend(TYPE_INSTRTP_EXITVAL());
  TYPE_STKM_SubProgram loop_instr (ConcIfThenElse(s1, sp, s2));

  // hand_no
  Int hand_no (GetNewTrapNo());

  // part of return
  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(TYPE_INSTRTP_PUSHTH().Init(hand_no))
      .ImpConc(b_instr)
      .ImpAppend(TYPE_INSTRTP_BR().Init(Int(loop_instr.Length() + 1)))
      .ImpAppend(TYPE_INSTRTP_HANDID().Init(hand_no))
      .ImpConc(loop_instr)
      .ImpAppend(TYPE_INSTRTP_POPTH());
  return prog;
}

// GetNewTrapNo
// ==> nat1
Int StackCompiler::GetNewTrapNo()
{
  this->trapno.Incr();
  return this->trapno;
}

TYPE_STKM_SubProgram StackCompiler::AddTrap(const TYPE_STKM_SubProgram & b_instr,
                                            const TYPE_STKM_SubProgram & handler)
{
  // hand_no
  Int hand_no (GetNewTrapNo());

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(TYPE_INSTRTP_PUSHTH().Init(hand_no))
      .ImpConc(b_instr)
      .ImpAppend(TYPE_INSTRTP_POPTH())
      .ImpAppend(TYPE_INSTRTP_BR().Init(Int(handler.Length() + 2)))
      .ImpAppend(TYPE_INSTRTP_HANDID().Init(hand_no))
      .ImpAppend(TYPE_INSTRTP_POPTH())
      .ImpConc(handler);
  return prog;
}

// {{{ Start, StartList

#ifdef VDMPP
// CompileStartStmt
// sIn : AS`StartStmt
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileStartStmt(const TYPE_AS_StartStmt & sIn)
{
  const TYPE_AS_Expr & expr (sIn.GetRecord(pos_AS_StartStmt_expr));

  TYPE_STKM_SubProgram sp;
  sp.ImpConc(E2I(expr))
    .ImpAppend(TYPE_INSTRTP_STARTLIST().Init(Bool(false)))
    .ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_cont));
  return sp;
}

// CompileStartListStmt
// sIn : AS`StartListStmt
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileStartListStmt(const TYPE_AS_StartListStmt& sIn)
{
  const TYPE_AS_Expr & expr (sIn.GetRecord(pos_AS_StartListStmt_expr));

  TYPE_STKM_SubProgram sp;
  sp.ImpConc(E2I(expr))
#ifdef VICE
    .ImpConc(MkRuntimeStartList())
#endif //VICE
    .ImpAppend(TYPE_INSTRTP_STARTLIST().Init(Bool(true)))
    .ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_cont));
  return sp;
}

// CompileStopStmt
// sIn : AS`StopStmt
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileStopStmt(const TYPE_AS_StopStmt & sIn)
{
  const TYPE_AS_Expr & expr (sIn.GetRecord(pos_AS_StopStmt_expr));

  TYPE_STKM_SubProgram sp;
  sp.ImpConc(E2I(expr))
    .ImpAppend(TYPE_INSTRTP_STOPLIST().Init(Bool(false)))
    .ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_cont));
  return sp;
}

// CompileStopListStmt
// sIn : AS`StopListStmt
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileStopListStmt(const TYPE_AS_StopListStmt& sIn)
{
  const TYPE_AS_Expr & expr (sIn.GetRecord(pos_AS_StopListStmt_expr));

  TYPE_STKM_SubProgram sp;
  sp.ImpConc(E2I(expr))
#ifdef VICE
    .ImpConc(MkRuntimeStopList())
#endif //VICE
    .ImpAppend(TYPE_INSTRTP_STOPLIST().Init(Bool(true)))
    .ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_cont));
  return sp;
}
#endif //VDMPP
// }}}

// {{{ Duration
#ifdef VICE
// CompileDurationStmt
// sIn : AS`DurationStmt
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileDurationStmt(const TYPE_AS_DurationStmt & sIn)
{
  const TYPE_AS_Expr & expr (sIn.GetRecord(pos_AS_DurationStmt_num));
  const TYPE_AS_Stmt & stmt (sIn.GetRecord(pos_AS_DurationStmt_stmt));

  TYPE_STKM_SubProgram sp;
  sp.ImpAppend(TYPE_INSTRTP_PUSHDURATION())
    .ImpConc(S2I(stmt));
  if (expr.Is(TAG_TYPE_AS_RealLit)) {
    const Real & num (expr.GetReal(pos_AS_RealLit_val));
    sp.ImpAppend(TYPE_INSTRTP_POPDURATION().Init(Int(num)));
  }
  else if (expr.Is(TAG_TYPE_AS_NumLit)) {
    const Real & num (expr.GetReal(pos_AS_NumLit_val));
    sp.ImpAppend(TYPE_INSTRTP_POPDURATION().Init(Int(num)));
  }
  else {
    sp.ImpConc(E2I(expr))
      .ImpAppend(TYPE_INSTRTP_POPDURATION().Init(Nil()));
  }
  return sp;
}

// CompileCycleStmt
// sIn : AS`CycleStmt
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileCycleStmt(const TYPE_AS_CycleStmt & sIn)
{
  const TYPE_AS_Expr & expr (sIn.GetRecord(pos_AS_CycleStmt_num));
  const TYPE_AS_Stmt & stmt (sIn.GetRecord(pos_AS_CycleStmt_stmt));

  TYPE_STKM_SubProgram sp;
  sp.ImpAppend(TYPE_INSTRTP_PUSHCYCLE())
    .ImpConc(S2I(stmt));
  if (expr.Is(TAG_TYPE_AS_RealLit)) {
    const Real & num (expr.GetReal(pos_AS_RealLit_val));
    sp.ImpAppend(TYPE_INSTRTP_POPCYCLE().Init(num));
  }
  else if (expr.Is(TAG_TYPE_AS_NumLit)) {
    const Real & num (expr.GetReal(pos_AS_NumLit_val));
    sp.ImpAppend(TYPE_INSTRTP_POPCYCLE().Init(num));
  }
  else {
    sp.ImpConc(E2I(expr))
      .ImpAppend(TYPE_INSTRTP_POPCYCLE().Init(Nil()));
  }
  return sp;
}
#endif //VICE
// }}}

// {{{ AtomicAssign, Assign

// CompileAtomicAssignStmt
// sIn : AS`AtomicAssignStmt
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileAtomicAssignStmt(const TYPE_AS_AtomicAssignStmt & sIn)
{
  const SEQ<TYPE_AS_AssignStmt> & assstmtl (sIn.GetSequence(pos_AS_AtomicAssignStmt_atm));

  TYPE_STKM_SubProgram sp;
  // TODO: into atomic mode ?? 
  size_t len_assstmtl = assstmtl.Length();
  for (size_t i = 1; i <= len_assstmtl; i++) {
    const TYPE_AS_AssignStmt & as (assstmtl[i]);      // AS`AssignStmt
    const TYPE_AS_StateDesignator & lhs (as.GetRecord(pos_AS_AssignStmt_lhs));
    const TYPE_AS_Expr & rhs            (as.GetRecord(pos_AS_AssignStmt_rhs));
    sp.ImpConc(SetContext(as.GetInt(pos_AS_AssignStmt_cid), true))
      .ImpConc(E2I(rhs))
      .ImpConc(SetContext(ASTAUX::GetCid(lhs), false)) //
      .ImpConc(SD2I(lhs));
  }

  sp.ImpAppend(TYPE_INSTRTP_ATOMIC().Init(Int(assstmtl.Length())));
  return sp;
}

// CompileAssignStmt
// sIn : AS`AssignStmt
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileAssignStmt(const TYPE_AS_AssignStmt& sIn)
{
  // Unpack sIn
  const TYPE_AS_StateDesignator & lhs (sIn.GetRecord(pos_AS_AssignStmt_lhs));
  const TYPE_AS_Expr & rhs            (sIn.GetRecord(pos_AS_AssignStmt_rhs));

  // rhs_instr
  TYPE_STKM_SubProgram rhs_instr (E2I(rhs));

  // lhs_instr
  TYPE_STKM_SubProgram lhs_instr (SetContext(ASTAUX::GetCid(lhs), false));
  lhs_instr.ImpConc(SD2I(lhs));

  // cont_check
  TYPE_STKM_SubProgram s1, s2, s3;
  s1.ImpAppend(TYPE_INSTRTP_ISCONT());
  s2.ImpAppend(TYPE_INSTRTP_ERRINST().Init(TYPE_RTERR_ERR(RTERR_OP_RETURNED_CONT)));
  s3.ImpConc(lhs_instr)
    .ImpAppend(TYPE_INSTRTP_ASSIGNSD());

  // return
  TYPE_STKM_SubProgram sp;
  sp.ImpConc(rhs_instr)
    .ImpConc(ConcIfThenElse(s1, s2, s3));
  return sp;
}

// }}}
// {{{ Block, NonDet

// CompileBlockStmt
// sIn : AS`BlockStmt
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileBlockStmt(const TYPE_AS_BlockStmt& sIn)
{
  // Unpack sIn
  const SEQ<TYPE_AS_AssignDef> & dcl_l (sIn.GetSequence(pos_AS_BlockStmt_dcls));
  const SEQ<TYPE_AS_Stmt> & stmt_l     (sIn.GetSequence(pos_AS_BlockStmt_stmts));

  // dcl 
  TYPE_STKM_SubProgram sp_dcl;
  // Run through dcl_l in reverse order
  size_t len_dcl_l = dcl_l.Length();
  for (size_t j = 1; j <= len_dcl_l; j++)
  {
    const TYPE_AS_AssignDef & elm (dcl_l[j]);
    const TYPE_AS_Name & id (elm.GetRecord(pos_AS_AssignDef_var));
    const TYPE_AS_Type & tp (elm.GetRecord(pos_AS_AssignDef_tp));
    const Generic & Iinit (elm.GetField(pos_AS_AssignDef_dclinit)); // AS`Expr

    if (Iinit.IsNil()) {
      sp_dcl.ImpAppend(TYPE_INSTRTP_PUSH().Init(TYPE_SEM_UNDEF()));
    }
    else {
      sp_dcl.ImpConc(E2I(Iinit))
            .ImpAppend(TYPE_INSTRTP_DTC().Init(tp));
    }
    sp_dcl.ImpAppend(TYPE_INSTRTP_APPENDBLKENV().Init(id, tp));
  }

  // Body of function
  TYPE_STKM_SubProgram sp;

  if (stmt_l.IsEmpty()) {
    sp.ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_cont));
  }
  else {
    sp.ImpConc(S2I(stmt_l[stmt_l.Length()]));

    // Run through stmt_l in reverse order
    for (size_t i = (stmt_l.Length() - 1); i >= 1; i--) {
      TYPE_STKM_SubProgram s1, s2;
      s1.ImpConc(S2I(stmt_l[i]))
        .ImpAppend(TYPE_INSTRTP_ISCONT());
      s2.ImpAppend(TYPE_INSTRTP_POP().Init(Int(1))) // remove SEM`CONT
        .ImpConc(sp);                               // execute next
      sp = ConcIfThenElse(s1, s2, TYPE_STKM_SubProgram());
/*
      wstring mes (L"Block Statement terminated at end of ");
      switch (i) {
        case 1:  { mes += L"first" ; break; }
        case 2:  { mes += L"second"; break; }
        case 3:  { mes += L"third"; break; }
        default: { mes += Int(i).ascii() + L"th"; break; }
      }
      mes += L" statement.";
      TYPE_STKM_SubProgram comment;
      comment.ImpAppend(TYPE_INSTRTP_COMMENT().Init(SEQ<Char>(mes), ASTAUX::GetCid(stmt_l[i])));
      sp = ConcIfThenElse(s1, s2, comment);
*/
    }
  }

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(TYPE_INSTRTP_EMPTYBLKENV().Init(sem_read_write))  // R/W
      .ImpConc(sp_dcl)
      .ImpConc(sp)
      .ImpAppend(TYPE_INSTRTP_POPBLKENV());

  return prog;
}

// CompileNonDetStmt
// sIn : AS`NonDetStmt
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileNonDetStmt(const TYPE_AS_NonDetStmt& sIn)
{
  // Unpack sIn
  const SEQ<TYPE_AS_Stmt> & stmts (sIn.GetSequence(pos_AS_NonDetStmt_stmts));

  // sp_l
  SEQ<TYPE_STKM_SubProgram> sp_l;
  size_t len_stmts = stmts.Length();
  for (size_t i = 1; i <= len_stmts; i++) {
    sp_l.ImpAppend(S2I(stmts[i]));
  }

  SEQ<Int> reljumps;
  reljumps.ImpAppend(Int(0));
  reljumps.ImpConc(RelJumpLengths(0, sp_l));

  // return
  Int lastRelJump (reljumps[reljumps.Length()]);
  SEQ<Int> newreljumps(reljumps);
  newreljumps.RemElem(reljumps.Length());

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(TYPE_INSTRTP_PUSH().Init(newreljumps))
      .ImpAppend(TYPE_INSTRTP_RANDOM())
      .ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_cont))
      .ImpAppend(TYPE_INSTRTP_NONDETSTMT())
      .ImpAppend(TYPE_INSTRTP_BR().Init(lastRelJump));

  // append sp_l
  size_t len_sp_l = sp_l.Length();
  for (size_t j = 1; j <= len_sp_l; j++) {
    prog.ImpConc(sp_l[j]);
    prog.ImpAppend(TYPE_INSTRTP_BR().Init(Int(-2) - reljumps[j+1]));
  }
  return prog;
}
// CompileAssertStmt
// sIn : AS`AssertStmt
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileAssertStmt(const TYPE_AS_AssertStmt& sIn)
{
  // Unpack sIn
  const TYPE_AS_Expr & expr (sIn.GetSequence(pos_AS_AssertStmt_expr));

  // errMsg
  TYPE_STKM_SubProgram errMsg;
  errMsg.ImpConc(CompileRunTime(TYPE_RTERR_ERR(RTERR_EVAL_ASSERT_GIVES_FALSE), ASTAUX::GetCid(expr)));

  TYPE_STKM_SubProgram prog;
  prog.ImpConc(ConcIfThenElse(mk_sequence(TYPE_INSTRTP_ASSERT()),
                              ConcIfThenElse(E2I(expr), TYPE_STKM_SubProgram(), errMsg),
                              TYPE_STKM_SubProgram()))
      .ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_cont));
  return prog;
}

// RelJumpLengths
// length : nat
// sp_l : seq of STKM`SubProgram
// ==> seq of nat
SEQ<Int> StackCompiler::RelJumpLengths( int length, const SEQ<TYPE_STKM_SubProgram>& sp_l)
{
  SEQ<Int> res;
  if (!sp_l.IsEmpty()) {
    size_t newlength = 1 + length + sp_l[1].Length();
    res.ImpAppend(Int(newlength));
    res.ImpConc(RelJumpLengths(newlength, sp_l.Tl()));
  }
  return res;
}
// }}}
