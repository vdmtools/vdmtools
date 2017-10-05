/***
*  * WHAT
*  *    Implementation of module CEXPR
*  * ID
*  *    $Id: cexpr.cc,v 1.71 2006/09/23 10:58:18 vdmtools Exp $
*  * SPECIFICATION VERSION
*  *    cexpr.cc: Implementation of cexpr.vdm 1.32
***/


// {{{ includes

#include "compile.h"
#include "INSTRTP.h"
#include "astaux.h"
#include "rt_errmsg.h"
#include "asquotes.h"
#include "pattern.h"
#include "freevars.h"
#include "evalaux.h"
#include "libman.h"
#include "intconvquotes.h"

//wcout << L"intertp: " << INT2Q::h2gAS(intertp) << endl;

// }}}
// {{{ E2I

void StackCompiler::Init_CEXPR()
{
  init_cexpr_lit();

#ifdef VICE
  init_Depend();
#endif // VICE
}

#ifdef VICE
void StackCompiler::init_Depend()
{
  this->opnm = Nil();
  this->nms = Set();
  this->histnms = Set();
  this->opnms = Set();
}
#endif // VICE

void StackCompiler::init_cexpr_lit()
{
  nBracketedExpr = Sequence(L"BracketedExpr");
  nDefExpr = Sequence(L"DefExpr");
  nLetExpr = Sequence(L"LetExpr");
  nLetBeSTExpr = Sequence(L"LetBeSTExpr");
  nAllOrExistsExpr = Sequence(L"AllOrExistsExpr");
  nExistsUniqueExpr = Sequence(L"ExistsUniqueExpr");
  nIotaExpr = Sequence(L"IotaExpr");
  nApplyExpr = Sequence(L"ApplyExpr");
  nFieldSelectExpr = Sequence(L"FieldSelectExpr");
  nIfExpr = Sequence(L"IfExpr");
  nCasesExpr = Sequence(L"CasesExpr");
  nPrefixExpr = Sequence(L"PrefixExpr");
  nBinaryExpr = Sequence(L"BinaryExpr");
  nSetRangeExpr = Sequence(L"SetRangeExpr");
  nSubSequenceExpr = Sequence(L"SubSequenceExpr");
  nSetEnumerationExpr = Sequence(L"SetEnumerationExpr");
  nSeqEnumerationExpr = Sequence(L"SeqEnumerationExpr");
  nMapEnumerationExpr = Sequence(L"MapEnumerationExpr");
  nSetComprehensionExpr = Sequence(L"SetComprehensionExpr");
  nSeqComprehensionExpr = Sequence(L"SeqComprehensionExpr");
  nMapComprehensionExpr = Sequence(L"MapComprehensionExpr");
  nTupleConstructorExpr = Sequence(L"TupleConstructorExpr");
  nRecordConstructorExpr = Sequence(L"RecordConstructorExpr");
  nRecordModifierExpr = Sequence(L"RecordModifierExpr");
  nSeqModifyMapOverrideExpr = Sequence(L"SeqModifyMapOverrideExpr");
  nLambdaExpr = Sequence(L"LambdaExpr");
  nFctTypeInstExpr = Sequence(L"FctTypeInstExpr");
  nIsExpr = Sequence(L"IsExpr");
  nNarrowExpr = Sequence(L"NarrowExpr");
  nTokenConstructorExpr = Sequence(L"TokenConstructorExpr");
  nTupleSelectExpr = Sequence(L"TupleSelectExpr");
  nTypeJudgementExpr = Sequence(L"TypeJudgementExpr");
  nPreConditionApplyExpr = Sequence(L"PreConditionApplyExpr");
  nName = Sequence(L"Name");
  nOldName = Sequence(L"OldName");
  nBoolLit = Sequence(L"BoolLit");
  nNilLit = Sequence(L"NilLit");
  nRealLit = Sequence(L"RealLit");
  nNumLit = Sequence(L"NumLit");
  nTextLit = Sequence(L"TextLit");
  nCharLit = Sequence(L"CharLit");
  nQuoteLit = Sequence(L"QuoteLit");
  nUndefinedExpr = Sequence(L"UndefinedExpr");
  nLastRes = Sequence(L"LastRes");
  nMacro = Sequence(L"Macro");
#ifdef VDMPP
  nSelfExpr = Sequence(L"SelfExpr");
  nNewExpr = Sequence(L"NewExpr");
  nIsOfClassExpr = Sequence(L"IsOfClassExpr");
  nIsOfBaseClassExpr = Sequence(L"IsOfBaseClassExpr");
  nSameBaseClassExpr = Sequence(L"SameBaseClassExpr");
  nSameClassExpr = Sequence(L"SameClassExpr");
  nThreadIdExpr = Sequence(L"ThreadIdExpr");
  nActExpr = Sequence(L"ActExpr");
  nFinExpr = Sequence(L"FinExpr");
  nActiveExpr = Sequence(L"ActiveExpr");
  nWaitingExpr = Sequence(L"WaitingExpr");
  nReqExpr = Sequence(L"ReqExpr");
#ifdef VICE
  nCurTimeExpr = Sequence(L"CurTimeExpr");
#endif // VICE
#endif // VDMPP
}

// E2I
// e : AS`Expr
// +> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::E2I(const TYPE_AS_Expr & e)
{
  TYPE_STKM_SubProgram prog (SetContext(ASTAUX::GetCid(e), false));

  switch (e.GetTag()) {
    case TAG_TYPE_AS_BracketedExpr: {
      prog.ImpConc(IStart(nBracketedExpr, e.GetInt(pos_AS_BracketedExpr_cid)))
          .ImpConc(E2I(e.GetRecord(pos_AS_BracketedExpr_expr)))
          .ImpConc(IEnd(nBracketedExpr));
      break;
    }
    case TAG_TYPE_AS_DefExpr: {
      prog.ImpConc(IStart(nDefExpr, e.GetInt(pos_AS_DefExpr_cid)))
          .ImpConc(CompileDefExpr(e))
          .ImpConc(IEnd(nDefExpr));
      break;
    }
    case TAG_TYPE_AS_LetExpr: {
      prog.ImpConc(IStart(nLetExpr, e.GetInt(pos_AS_LetExpr_cid)))
          .ImpConc(CompileLetExpr(e))
          .ImpConc(IEnd(nLetExpr));
      break;
    }
    case TAG_TYPE_AS_LetBeSTExpr: {
      prog.ImpConc(IStart(nLetBeSTExpr, e.GetInt(pos_AS_LetBeSTExpr_cid)))
          .ImpConc(CompileLetBeSTExpr(e))
          .ImpConc(IEnd(nLetBeSTExpr));
      break;
    }
    case TAG_TYPE_AS_AllOrExistsExpr: {
      prog.ImpConc(IStart(nAllOrExistsExpr, e.GetInt(pos_AS_AllOrExistsExpr_cid)))
          .ImpConc(CompileAllOrExistsExpr(e))
          .ImpConc(IEnd(nAllOrExistsExpr));
      break;
    }
    case TAG_TYPE_AS_ExistsUniqueExpr: {
      prog.ImpConc(IStart(nExistsUniqueExpr, e.GetInt(pos_AS_ExistsUniqueExpr_cid)))
          .ImpConc(CompileExistsUniqueExpr(e))
          .ImpConc(IEnd(nExistsUniqueExpr));
      break;
    }
    case TAG_TYPE_AS_IotaExpr: {
      prog.ImpConc(IStart(nIotaExpr, e.GetInt(pos_AS_IotaExpr_cid)))
          .ImpConc(CompileIotaExpr(e))
          .ImpConc(IEnd(nIotaExpr));
      break;
    }
    case TAG_TYPE_AS_ApplyExpr: {
      prog.ImpConc(IStart(nApplyExpr, e.GetInt(pos_AS_ApplyExpr_cid)))
          .ImpConc(CompileApplyExpr(e))
          .ImpConc(IEnd(nApplyExpr));
      break;
    }
    case TAG_TYPE_AS_FieldSelectExpr: {
      prog.ImpConc(IStart(nFieldSelectExpr, e.GetInt(pos_AS_FieldSelectExpr_cid)))
          .ImpConc(CompileFieldSelectExpr(e))
          .ImpConc(IEnd(nFieldSelectExpr));
      break;
    }
    case TAG_TYPE_AS_IfExpr: {
      prog.ImpConc(IStart(nIfExpr, e.GetInt(pos_AS_IfExpr_cid)))
          .ImpConc(CompileIfExpr(e))
          .ImpConc(IEnd(nIfExpr));
      break;
    }
    case TAG_TYPE_AS_CasesExpr: {
      prog.ImpConc(IStart(nCasesExpr, e.GetInt(pos_AS_CasesExpr_cid)))
          .ImpConc(CompileCasesExpr(e))
          .ImpConc(IEnd(nCasesExpr));
      break;
    }
    case TAG_TYPE_AS_PrefixExpr: {
      prog.ImpConc(IStart(nPrefixExpr, e.GetInt(pos_AS_PrefixExpr_cid)))
          .ImpConc(CompileUnaryExpr(e))
          .ImpConc(IEnd(nPrefixExpr));
      break;
    }
    case TAG_TYPE_AS_BinaryExpr: {
      prog.ImpConc(IStart(nBinaryExpr, e.GetInt(pos_AS_BinaryExpr_cid)))
          .ImpConc(CompileBinaryExpr(e))
          .ImpConc(IEnd(nBinaryExpr));
      break;
    }
    case TAG_TYPE_AS_SetRangeExpr: {
      prog.ImpConc(IStart(nSetRangeExpr, e.GetInt(pos_AS_SetRangeExpr_cid)))
          .ImpConc(CompileSetRangeExpr(e))
          .ImpConc(IEnd(nSetRangeExpr));
      break;
    }
    case TAG_TYPE_AS_SubSequenceExpr: {
      prog.ImpConc(IStart(nSubSequenceExpr, e.GetInt(pos_AS_SubSequenceExpr_cid)))
          .ImpConc(CompileSubSequenceExpr(e))
          .ImpConc(IEnd(nSubSequenceExpr));
      break;
    }
    case TAG_TYPE_AS_SetEnumerationExpr: {
      prog.ImpConc(IStart(nSetEnumerationExpr, e.GetInt(pos_AS_SetEnumerationExpr_cid)))
          .ImpConc(CompileSetEnumExpr(e))
          .ImpConc(IEnd(nSetEnumerationExpr));
      break;
    }
    case TAG_TYPE_AS_SeqEnumerationExpr: {
      prog.ImpConc(IStart(nSeqEnumerationExpr, e.GetInt(pos_AS_SeqEnumerationExpr_cid)))
          .ImpConc(CompileSeqEnumExpr(e))
          .ImpConc(IEnd(nSeqEnumerationExpr));
      break;
    }
    case TAG_TYPE_AS_MapEnumerationExpr: {
      prog.ImpConc(IStart(nMapEnumerationExpr, e.GetInt(pos_AS_MapEnumerationExpr_cid)))
          .ImpConc(CompileMapEnumExpr(e))
          .ImpConc(IEnd(nMapEnumerationExpr));
      break;
    }
    case TAG_TYPE_AS_SetComprehensionExpr: {
      prog.ImpConc(IStart(nSetComprehensionExpr, e.GetInt(pos_AS_SetComprehensionExpr_cid)))
          .ImpConc(CompileSetComprehensionExpr(e))
          .ImpConc(IEnd(nSetComprehensionExpr));
      break;
    }
    case TAG_TYPE_AS_SeqComprehensionExpr: {
      prog.ImpConc(IStart(nSeqComprehensionExpr, e.GetInt(pos_AS_SeqComprehensionExpr_cid)))
          .ImpConc(CompileSeqComprehensionExpr(e))
          .ImpConc(IEnd(nSeqComprehensionExpr));
      break;
    }
    case TAG_TYPE_AS_MapComprehensionExpr: {
      prog.ImpConc(IStart(nMapComprehensionExpr, e.GetInt(pos_AS_MapComprehensionExpr_cid)))
          .ImpConc(CompileMapComprehensionExpr(e))
          .ImpConc(IEnd(nMapComprehensionExpr));
      break;
    }
    case TAG_TYPE_AS_TupleConstructorExpr: {
      prog.ImpConc(IStart(nTupleConstructorExpr, e.GetInt(pos_AS_TupleConstructorExpr_cid)))
          .ImpConc(CompileTupleConstructorExpr(e))
          .ImpConc(IEnd(nTupleConstructorExpr));
      break;
    }
    case TAG_TYPE_AS_RecordConstructorExpr: {
      prog.ImpConc(IStart(nRecordConstructorExpr, e.GetInt(pos_AS_RecordConstructorExpr_cid)))
          .ImpConc(CompileRecordConstructorExpr(e))
          .ImpConc(IEnd(nRecordConstructorExpr));
      break;
    }
    case TAG_TYPE_AS_RecordModifierExpr: {
      prog.ImpConc(IStart(nRecordModifierExpr, e.GetInt(pos_AS_RecordModifierExpr_cid)))
          .ImpConc(CompileRecordModifierExpr(e))
          .ImpConc(IEnd(nRecordModifierExpr));
      break;
    }
    case TAG_TYPE_AS_SeqModifyMapOverrideExpr: {
      prog.ImpConc(IStart(nSeqModifyMapOverrideExpr, e.GetInt(pos_AS_SeqModifyMapOverrideExpr_cid)))
          .ImpConc(CompileSeqModifyMapOverrideExpr(e))
          .ImpConc(IEnd(nSeqModifyMapOverrideExpr));
      break;
    }
    case TAG_TYPE_AS_LambdaExpr: {
      prog.ImpConc(IStart(nLambdaExpr, e.GetInt(pos_AS_LambdaExpr_cid)))
          .ImpConc(CompileLambdaExpr(e))
          .ImpConc(IEnd(nLambdaExpr));
      break;
    }
    case TAG_TYPE_AS_FctTypeInstExpr: {
      prog.ImpConc(IStart(nFctTypeInstExpr, e.GetInt(pos_AS_FctTypeInstExpr_cid)))
          .ImpConc(CompileFctTypeInstExpr(e))
          .ImpConc(IEnd(nFctTypeInstExpr));
      break;
    }
    case TAG_TYPE_AS_IsExpr: {
      prog.ImpConc(IStart(nIsExpr, e.GetInt(pos_AS_IsExpr_cid)))
          .ImpConc(CompileIsExpr(e))
          .ImpConc(IEnd(nIsExpr));
      break;
    }
    case TAG_TYPE_AS_NarrowExpr: {
      prog.ImpConc(IStart(nNarrowExpr, e.GetInt(pos_AS_NarrowExpr_cid)))
          .ImpConc(CompileNarrowExpr(e))
          .ImpConc(IEnd(nNarrowExpr));
      break;
    }
    case TAG_TYPE_AS_TokenConstructorExpr: {
      prog.ImpConc(IStart(nTokenConstructorExpr, e.GetInt(pos_AS_TokenConstructorExpr_cid)))
          .ImpConc(CompileTokenConstructorExpr(e))
          .ImpConc(IEnd(nTokenConstructorExpr));
      break;
    }
    case TAG_TYPE_AS_TupleSelectExpr: {
      prog.ImpConc(IStart(nTupleSelectExpr, e.GetInt(pos_AS_TupleSelectExpr_cid)))
          .ImpConc(CompileTupleSelectExpr(e))
          .ImpConc(IEnd(nTupleSelectExpr));
      break;
    }
    case TAG_TYPE_AS_TypeJudgementExpr: {
      prog.ImpConc(IStart(nTypeJudgementExpr, e.GetInt(pos_AS_TypeJudgementExpr_cid)))
          .ImpConc(CompileTypeJudgementExpr(e))
          .ImpConc(IEnd(nTypeJudgementExpr));
      break;
    }
    case TAG_TYPE_AS_PreConditionApplyExpr: {
      prog.ImpConc(IStart(nPreConditionApplyExpr, e.GetInt(pos_AS_PreConditionApplyExpr_cid)))
          .ImpConc(CompilePreConditionApplyExpr(e))
          .ImpConc(IEnd(nPreConditionApplyExpr));
      break;
    }
    case TAG_TYPE_AS_Name: {
      prog.ImpConc(IStart(nName, e.GetInt(pos_AS_Name_cid)))
          .ImpConc(CompileName(e))
          .ImpConc(IEnd(nName));
      break;
    }
    case TAG_TYPE_AS_OldName: {
// 20091211 -->
      theStackMachine().SetUsesOldName(true);
// <-- 20091211
      prog.ImpConc(IStart(nOldName, e.GetInt(pos_AS_OldName_cid)))
          .ImpAppend(TYPE_INSTRTP_LOOKUP().Init(e))
          .ImpConc(IEnd(nOldName));
      break;
    }
    case TAG_TYPE_AS_BoolLit: {
      prog.ImpConc(IStart(nBoolLit, e.GetInt(pos_AS_BoolLit_cid)))
          .ImpAppend(TYPE_INSTRTP_PUSH().Init(e.GetBool(pos_AS_BoolLit_val) ? sem_true : sem_false))
          .ImpConc(IEnd(nBoolLit));
      break;
    }
    case TAG_TYPE_AS_NilLit: {
      prog.ImpConc(IStart(nNilLit, e.GetInt(pos_AS_NilLit_cid)))
          .ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_nil))
          .ImpConc(IEnd(nNilLit));
      break;
    }
    case TAG_TYPE_AS_RealLit: {
      prog.ImpConc(IStart(nRealLit, e.GetInt(pos_AS_RealLit_cid)))
          .ImpAppend(TYPE_INSTRTP_PUSH().Init(mk_SEM_NUM(e.GetReal(pos_AS_RealLit_val))))
          .ImpConc(IEnd(nRealLit));
      break;
    }
    case TAG_TYPE_AS_NumLit: {
      prog.ImpConc(IStart(nNumLit, e.GetInt(pos_AS_NumLit_cid)))
          .ImpAppend(TYPE_INSTRTP_PUSH().Init(mk_SEM_NUM(e.GetReal(pos_AS_NumLit_val))))
          .ImpConc(IEnd(nNumLit));
      break;
    }
    case TAG_TYPE_AS_TextLit: {
      const type_cL & val (e.GetSequence(pos_AS_TextLit_val));

      size_t len_val = val.Length();
      SEQ<TYPE_SEM_VAL> chars;
      for (size_t i = 1; i <= len_val; i++)
        chars.ImpAppend(mk_SEM_CHAR(val[i]));

      prog.ImpConc(IStart(nTextLit, e.GetInt(pos_AS_TextLit_cid)))
          .ImpAppend(TYPE_INSTRTP_PUSH().Init(mk_SEM_SEQ(chars)))
          .ImpConc(IEnd(nTextLit));
      break;
    }
    case TAG_TYPE_AS_CharLit: {
      prog.ImpConc(IStart(nCharLit, e.GetInt(pos_AS_CharLit_cid)))
          .ImpAppend(TYPE_INSTRTP_PUSH().Init(mk_SEM_CHAR(e.GetChar(pos_AS_CharLit_val))))
          .ImpConc(IEnd(nCharLit));
      break;
    }
    case TAG_TYPE_AS_QuoteLit: {
      prog.ImpConc(IStart(nQuoteLit, e.GetInt(pos_AS_QuoteLit_cid)))
          .ImpAppend(TYPE_INSTRTP_PUSH().Init(mk_SEM_QUOTE(e.GetSequence(pos_AS_QuoteLit_val))))
          .ImpConc(IEnd(nQuoteLit));
      break;
    }
    case TAG_TYPE_AS_UndefinedExpr: {
      prog.ImpConc(IStart(nUndefinedExpr, e.GetInt(pos_AS_UndefinedExpr_cid)))
          .ImpAppend(TYPE_INSTRTP_ERRINST().Init(TYPE_RTERR_ERR(RTERR_UNDEFINED_EXPRESSION)))
          .ImpConc(IEnd(nUndefinedExpr));
      break;
    }
    case TAG_TYPE_AS_LastRes: {
      prog.ImpConc(IStart(nLastRes, NilContextId))
          .ImpAppend(TYPE_INSTRTP_LASTRES())
          .ImpConc(IEnd(nLastRes));
      break;
    }
    case TAG_TYPE_AS_Macro: {
      const TYPE_CI_ContextId & cid (e.GetInt(pos_AS_Macro_cid));
      prog.ImpConc(IStart(nMacro, cid))
          .ImpAppend(TYPE_INSTRTP_MACRO().Init(e.GetInt(pos_AS_Macro_tp),cid))
          .ImpConc(IEnd(nMacro));
      break;
    }

#ifdef VDMPP
    case TAG_TYPE_AS_SelfExpr: {
      prog.ImpConc(IStart(nSelfExpr, e.GetInt(pos_AS_SelfExpr_cid)))
          .ImpConc(CompileSelfExpr(e))
          .ImpConc(IEnd(nSelfExpr));
      break;
    }
    case TAG_TYPE_AS_NewExpr: {
      prog.ImpConc(IStart(nNewExpr, e.GetInt(pos_AS_NewExpr_cid)))
          .ImpConc(CompileNewExpr(e, Generic(Nil())))
          .ImpConc(IEnd(nNewExpr));
      break;
    }
    case TAG_TYPE_AS_IsOfClassExpr: {
      prog.ImpConc(IStart(nIsOfClassExpr, e.GetInt(pos_AS_IsOfClassExpr_cid)))
          .ImpConc(CompileIsOfClassExpr(e))
          .ImpConc(IEnd(nIsOfClassExpr));
      break;
    }
    case TAG_TYPE_AS_IsOfBaseClassExpr: {
      prog.ImpConc(IStart(nIsOfBaseClassExpr, e.GetInt(pos_AS_IsOfBaseClassExpr_cid)))
          .ImpConc(CompileIsOfBaseClassExpr(e))
          .ImpConc(IEnd(nIsOfBaseClassExpr));
      break;
    }
    case TAG_TYPE_AS_SameBaseClassExpr: {
      prog.ImpConc(IStart(nSameBaseClassExpr, e.GetInt(pos_AS_SameBaseClassExpr_cid)))
          .ImpConc(CompileSameBaseClassExpr(e))
          .ImpConc(IEnd(nSameBaseClassExpr));
      break;
    }
    case TAG_TYPE_AS_SameClassExpr: {
      prog.ImpConc(IStart(nSameClassExpr, e.GetInt(pos_AS_SameClassExpr_cid)))
          .ImpConc(CompileSameClassExpr(e))
          .ImpConc(IEnd(nSameClassExpr));
      break;
    }
    case TAG_TYPE_AS_ThreadIdExpr: {
      prog.ImpConc(IStart(nThreadIdExpr, e.GetInt(pos_AS_ThreadIdExpr_cid)))
          .ImpConc(CompileThreadIdExpr(e))
          .ImpConc(IEnd(nThreadIdExpr));
      break;
    }
    case TAG_TYPE_AS_ActExpr: {
      prog.ImpConc(IStart(nActExpr, e.GetInt(pos_AS_ActExpr_cid)))
          .ImpConc(CompileHistoryExpr(e.GetSequence(pos_AS_ActExpr_mthd),
                                      //TYPE_INSTRTP_act()))
                                      EvalState::historyKind_act))
          .ImpConc(IEnd(nActExpr));
      break;
    }
    case TAG_TYPE_AS_FinExpr: {
      prog.ImpConc(IStart(nFinExpr, e.GetInt(pos_AS_FinExpr_cid)))
          .ImpConc(CompileHistoryExpr(e.GetSequence(pos_AS_FinExpr_mthd),
                                      //TYPE_INSTRTP_fin()))
                                      EvalState::historyKind_fin))
          .ImpConc(IEnd(nFinExpr));
      break;
    }
    case TAG_TYPE_AS_ActiveExpr: {
      prog.ImpConc(IStart(nActiveExpr, e.GetInt(pos_AS_ActiveExpr_cid)))
          .ImpConc(CompileHistoryExpr(e.GetSequence(pos_AS_ActiveExpr_mthd),
                                      //TYPE_INSTRTP_active()))
                                      EvalState::historyKind_active))
          .ImpConc(IEnd(nActiveExpr));
      break;
    }
    case TAG_TYPE_AS_WaitingExpr: {
      prog.ImpConc(IStart(nWaitingExpr, e.GetInt(pos_AS_WaitingExpr_cid)))
          .ImpConc(CompileHistoryExpr(e.GetSequence(pos_AS_WaitingExpr_mthd),
                                      //TYPE_INSTRTP_waiting()))
                                      EvalState::historyKind_waiting))
          .ImpConc(IEnd(nWaitingExpr));
      break;
    }
    case TAG_TYPE_AS_ReqExpr: {
      prog.ImpConc(IStart(nReqExpr, e.GetInt(pos_AS_ReqExpr_cid)))
          .ImpConc(CompileHistoryExpr(e.GetSequence(pos_AS_ReqExpr_mthd),
                                      //TYPE_INSTRTP_req()))
                                      EvalState::historyKind_req))
          .ImpConc(IEnd(nReqExpr));
      break;
    }
#ifdef VICE
    case TAG_TYPE_AS_CurTimeExpr: {
      prog.ImpConc(IStart(nCurTimeExpr, e.GetInt(pos_AS_CurTimeExpr_cid)))
          .ImpAppend(TYPE_INSTRTP_CURTIME())
          .ImpConc(IEnd(nCurTimeExpr));
      break;
    }
#endif // VICE
#endif //VDMPP
    default: {
      RTERR::Error(L"StackCompile::E2I", RTERR_UNKNOWN_AS_TYPE, Nil(), Nil(), Sequence());
      break;
    }
  }
#ifdef VICE
  if (GetCompilingTime()) {
    prog.ImpConc(E2Time(e));
  }
#endif //VICE

  return prog;
}

// }}}
// {{{ Def, Let, LetBeST

// CompileDefExpr
// eIn : AS`DefExpr
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileDefExpr(const TYPE_AS_DefExpr & eIn)
{
  const type_dd2PL & def_l  (eIn.GetSequence(pos_AS_DefExpr_Def));
  const TYPE_AS_Expr & in_e (eIn.GetRecord  (pos_AS_DefExpr_In));

  TYPE_STKM_SubProgram def_sp;
  size_t len_def_l = def_l.Length();
  for (size_t index = 1; index <= len_def_l; index++) {
    const Tuple & t (def_l[index]); // (AS`PatternBind * AS`Expr)
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
    .ImpConc(E2I(in_e))
    .ImpAppend(TYPE_INSTRTP_POPBLKENV());
  return sp;
}

// CompileName
// nm : AS`Name
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileName(const TYPE_AS_Name & nm)
{
#ifdef VICE
  if( !this->opnm.IsNil() ) {
    if( nms.IsQuote() || Set(opnms).InSet(nm) ) {
      nms = Quote(L"ALL");
    }
    else {
      Set s (nms);
      s.Insert(nm);
      nms = s;
    }
  }
#endif // VICE

  TYPE_STKM_SubProgram sp;
  sp.ImpAppend(TYPE_INSTRTP_LOOKUP().Init(nm));
  return sp;
}

// CompileLetExpr
// eIn : AS`LetExpr
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileLetExpr(const TYPE_AS_LetExpr & eIn)
{
  const SEQ<TYPE_AS_LocalDef> & localdef (eIn.GetSequence(pos_AS_LetExpr_localdef));
  const TYPE_AS_Expr & In (eIn.GetRecord(pos_AS_LetExpr_body));

  TYPE_STKM_SubProgram def_sp;
  size_t len_localdef = localdef.Length();
  for (size_t i = 1; i <= len_localdef; i++) {
    const TYPE_AS_LocalDef & ldef (localdef[i]);
    switch(ldef.GetTag()) {
      case TAG_TYPE_AS_ValueDef: {
        const Generic & tp (ldef.GetField(pos_AS_ValueDef_tp));
        // TODO: if tp is AS`CompositeType, local type definition needed
        def_sp.ImpConc(E2I(ldef.GetRecord(pos_AS_ValueDef_val)));
        if (!tp.IsNil()) {
          def_sp.ImpAppend(TYPE_INSTRTP_DTC().Init(tp));
        }

        const TYPE_AS_Pattern & pat (ldef.GetRecord(pos_AS_ValueDef_pat));
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
      //case TAG_TYPE_AS_ImplFnDef:
      //case TAG_TYPE_AS_ExplFnDef:
      //case TAG_TYPE_AS_ExtExplFnDef:
      default: {
        // SEM`BlkEnv * map AS`Name to (AS`Expr | <NOTYETSPEC> | <SUBRESP>)
        Tuple t (PAT::ConstructFN(GetClMod(), ldef));
        def_sp.ImpAppend(TYPE_INSTRTP_CLOSENV().Init(t.GetRecord(1), t.GetMap(2)));
        break;
      }
    }
  }

  TYPE_STKM_SubProgram sp;
  sp.ImpAppend(TYPE_INSTRTP_EMPTYBLKENV().Init(sem_read_only))
    .ImpConc(def_sp)
    .ImpConc(E2I(In))
    .ImpAppend(TYPE_INSTRTP_POPBLKENV());
  return sp;
}
    
// CompileLetBeSTExpr
// eIn : AS`LetBeSTExpr
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileLetBeSTExpr(const TYPE_AS_LetBeSTExpr & eIn)
{
  const SEQ<TYPE_AS_MultBind> & lhs (eIn.GetSequence(pos_AS_LetBeSTExpr_lhs));
  const Generic & St       (eIn.GetField (pos_AS_LetBeSTExpr_St));
  const TYPE_AS_Expr & In  (eIn.GetRecord(pos_AS_LetBeSTExpr_In));

  // prep_instr
  TYPE_STKM_SubProgram prep_instr;
  prep_instr.ImpConc(CompileMultBindL(lhs, Int(DONT_PARTITION)));

  TYPE_STKM_SubProgram body_instr;
  if (St.IsNil()) {
    TYPE_STKM_SubProgram s1, s2, s3;
    s1.ImpAppend(TYPE_INSTRTP_ISEMPTYSET().Init(Int(1)));
    s2.ImpAppend(TYPE_INSTRTP_ERRINST().Init(TYPE_RTERR_ERR(RTERR_EMPTY_ENV_S)));
    s3.ImpAppend(TYPE_INSTRTP_SELBLKENV().Init(Int(1)))
      .ImpConc(E2I(In))
      .ImpAppend(TYPE_INSTRTP_POPBLKENV());
    body_instr.ImpConc(ConcIfThenElse(s1, s2, s3));
  }
  else {
    TYPE_STKM_SubProgram s1, s2;
    s1.ImpAppend(TYPE_INSTRTP_ISEMPTYSET().Init(Int(1)));
    s2.ImpAppend(TYPE_INSTRTP_ERRINST().Init(TYPE_RTERR_ERR(RTERR_EMPTY_ENV_S)));

    // St_instr
    TYPE_STKM_SubProgram St_instr;
    St_instr.ImpAppend(TYPE_INSTRTP_SELBLKENV().Init(Int(1)))
            .ImpConc(E2I(St));

    // St_succ
    TYPE_STKM_SubProgram St_succ;
    St_succ.ImpConc(E2I(In))
           .ImpAppend(TYPE_INSTRTP_POPBLKENV())
           .ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_true));

    // St_failed
    TYPE_STKM_SubProgram St_failed;
    St_failed.ImpAppend(TYPE_INSTRTP_POPBLKENV())
             .ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_false));

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
// {{{ All, Exists, ExistsUnique, Iota

// CompileAllOrExistsExpr
// eIn : AS`AllOrExistsExpr
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileAllOrExistsExpr(const TYPE_AS_AllOrExistsExpr & eIn)
{
  // Extract information from eIn.
  const TYPE_AS_AllOrExistsQuantifier & quant (eIn.GetRecord(pos_AS_AllOrExistsExpr_quant));
  const SEQ<TYPE_AS_MultBind> & bind_l (eIn.GetSequence(pos_AS_AllOrExistsExpr_bind));
  const TYPE_AS_Expr & pred (eIn.GetRecord(pos_AS_AllOrExistsExpr_pred));

  // prep_instr
  TYPE_STKM_SubProgram prep_instr (CompileMultBindL(bind_l, Int(DONT_PARTITION)));

  // q_i, succ_instr, fail_instr
  TYPE_STKM_SubProgram succ_instr, fail_instr;
  succ_instr.ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_true));
  fail_instr.ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_false));
  TYPE_STKM_SubProgram q_i (quant == Int(ALL) ? succ_instr : fail_instr);
  
  // body_instr
  TYPE_STKM_SubProgram body_instr;
  body_instr.ImpAppend(TYPE_INSTRTP_SELBLKENV().Init(Int(1)))
            .ImpConc(E2I(pred))
            .ImpAppend(TYPE_INSTRTP_POPBLKENV());

  if (quant == Int(ALL)) {
    TYPE_STKM_SubProgram s1, s2, s3;
    s2.ImpConc(fail_instr);
    s3.ImpConc(fail_instr).ImpConc(succ_instr);
    body_instr.ImpConc(ConcIfThenElse(s1, s2, s3));
  }
  else {
    TYPE_STKM_SubProgram s1, s2, s3;
    s2.ImpConc(succ_instr).ImpConc(succ_instr);
    s3.ImpConc(fail_instr);
    body_instr.ImpConc(ConcIfThenElse(s1, s2, s3));
  }

  // loop_instr
  TYPE_STKM_SubProgram loop_instr, s4, s5;
  s4.ImpAppend(TYPE_INSTRTP_ISEMPTYSET().Init(Int(1)));
  s5.ImpConc(q_i);
  s5.ImpConc(succ_instr);
  loop_instr.ImpConc(ConcIfThenElse(s4, s5, body_instr));
  
  // loop
  TYPE_STKM_SubProgram sp;
  sp.ImpConc(prep_instr)
    .ImpConc(CombRepeatUntil(loop_instr))
    .ImpAppend(TYPE_INSTRTP_REMSTACKELEM().Init(Int(2)));

  return sp;
}

// CompileEUandICommon
// bind : AS`Bind
// expr : AS`Expr
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileEUandICommon(const TYPE_AS_Bind & bind, const TYPE_AS_Expr & expr)
{
  // bind_l
  SEQ<TYPE_AS_MultBind> bind_l (ASTAUX::BindToBindList(bind));
  
  // prep_instr
  TYPE_STKM_SubProgram prep_instr;
  prep_instr.ImpConc(CompileMultBindL(bind_l, Int(DO_PARTITION)))
            .ImpAppend(TYPE_INSTRTP_PUSH().Init(Set()));

  // pred_instr
  TYPE_STKM_SubProgram pred_instr (E2I(expr));

  // succ_instr, fail_instr
  TYPE_STKM_SubProgram succ_instr, fail_instr;
  succ_instr.ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_true));
  fail_instr.ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_false));
  
  // more_than_one_pred
  TYPE_STKM_SubProgram more_than_one_pred;
  more_than_one_pred.ImpAppend(TYPE_INSTRTP_ADDTOBLKENV())
                    .ImpAppend(TYPE_INSTRTP_SIZE().Init(Int(2)));
  
  // loop_body
  TYPE_STKM_SubProgram loop_body;
  loop_body.ImpAppend(TYPE_INSTRTP_SELBLKENV().Init(Int(2)))
           .ImpConc(ConcIfThenElse(pred_instr, more_than_one_pred, fail_instr))
           .ImpAppend(TYPE_INSTRTP_POPBLKENV());

  // loop_instr
  TYPE_STKM_SubProgram loop_instr, s2;
  s2.ImpAppend(TYPE_INSTRTP_ISEMPTYSET().Init(Int(2)));
  loop_instr.ImpConc(ConcIfThenElse(s2, succ_instr, loop_body));
  
  // loop
  TYPE_STKM_SubProgram sp;
  sp.ImpConc(prep_instr)
    .ImpConc(CombRepeatUntil(loop_instr))
    .ImpAppend(TYPE_INSTRTP_REMSTACKELEM().Init(Int(2)));

  return sp;
}

// CompileExistsUniqueExpr
// eIn : AS`ExistsUniqueExpr
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileExistsUniqueExpr(const TYPE_AS_ExistsUniqueExpr & eIn)
{
  // Unpack eIn
  const TYPE_AS_Bind & bind   (eIn.GetRecord(pos_AS_ExistsUniqueExpr_bind));
  const TYPE_AS_Expr & pred_e (eIn.GetRecord(pos_AS_ExistsUniqueExpr_pred));

  TYPE_AS_PatternBind new_bind (PAT::DoCarePattern(bind, ASTAUX::MkNameFromId(SEQ<Char>(L"1"), NilContextId)));
  
  // common
  TYPE_STKM_SubProgram common (CompileEUandICommon(new_bind, pred_e));

  // return
  TYPE_STKM_SubProgram sp;
  sp.ImpConc(common)
    .ImpAppend(TYPE_INSTRTP_SIZE().Init(Int(1)))
    .ImpAppend(TYPE_INSTRTP_REMSTACKELEM().Init(Int(2)));

  return sp; 
}

// CompileIotaExpr
// eIn : AS`IotaExpr
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileIotaExpr(const TYPE_AS_IotaExpr & eIn)
{
  // Unpack eIn
  const TYPE_AS_Bind & bind   (eIn.GetRecord(pos_AS_IotaExpr_bind));
  const TYPE_AS_Expr & pred_e (eIn.GetRecord(pos_AS_IotaExpr_pred));
  
  TYPE_AS_PatternBind new_bind (PAT::DoCarePattern(bind, ASTAUX::MkNameFromId(SEQ<Char>(L"1"), NilContextId)));
  
  // common
  TYPE_STKM_SubProgram common (CompileEUandICommon(new_bind, pred_e));
  
  // one_found
  TYPE_STKM_SubProgram one_found;
  one_found.ImpAppend(TYPE_INSTRTP_SELBLKENV().Init(Int(1)))
           .ImpConc(E2I(PAT::GetExpr(PAT::SelPattern(new_bind))))
           .ImpAppend(TYPE_INSTRTP_POPBLKENV())
           .ImpAppend(TYPE_INSTRTP_REMSTACKELEM().Init(Int(2)));

  // ConcIfThenElse
  TYPE_STKM_SubProgram s1, s2;
  s1.ImpAppend(TYPE_INSTRTP_SIZE().Init(Int(1)));
  s2.ImpConc(SetContext(ASTAUX::GetCid(eIn), false))
    .ImpAppend(TYPE_INSTRTP_ERRINST().Init(TYPE_RTERR_ERR(RTERR_NO_UNIQ_ELEM)));

  TYPE_STKM_SubProgram sp;
  sp.ImpConc(common)
    .ImpConc(ConcIfThenElse(s1, one_found, s2));

  return sp;
}

// }}}
// {{{ Apply, FieldSelect

// CompileApplyExpr
// eIn : AS`ApplyExpr
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileApplyExpr(const TYPE_AS_ApplyExpr & eIn)
{
  // Unpack eIn
  const TYPE_AS_Expr & fct      (eIn.GetRecord  (pos_AS_ApplyExpr_fct));
  const SEQ<TYPE_AS_Expr> & arg (eIn.GetSequence(pos_AS_ApplyExpr_arg));

  // Body of the function
  TYPE_STKM_SubProgram sp (E2I(fct));

  size_t len_arg = arg.Length();
  for (size_t i = 1; i <= len_arg; i++) {
    sp.ImpConc(E2I(arg[i]));
  }
  sp.ImpAppend(TYPE_INSTRTP_PUSHLIST().Init(Int(len_arg)));

#ifdef VDMPP
  sp.ImpAppend(TYPE_INSTRTP_GUARD().Init(Bool(true)));
#endif //VDMPP
  sp.ImpConc(SetContext(ASTAUX::GetCid(fct), false))
    .ImpAppend(TYPE_INSTRTP_APPLY());
  return sp;
}

// CompileFieldSelectExpr
// eIn : AS`FieldSelectExpr
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileFieldSelectExpr(const TYPE_AS_FieldSelectExpr & eIn)
{
  // Unpack eIn
  const TYPE_AS_Expr & rec   (eIn.GetRecord(pos_AS_FieldSelectExpr_rec));
  const TYPE_AS_Expr & field (eIn.GetRecord(pos_AS_FieldSelectExpr_nm)); // AS`Name | AS`FctTypeInstExpr

  // Body of the function
  TYPE_STKM_SubProgram sp (E2I(rec));
// 20140821 -->
  //sp.ImpConc(SetContext(ASTAUX::GetCid(field), false))
  //  .ImpAppend(TYPE_INSTRTP_FIELDSEL().Init(field));
  switch (field.GetTag()) {
    case TAG_TYPE_AS_Name: {
      sp.ImpConc(SetContext(ASTAUX::GetCid(field), false))
        .ImpAppend(TYPE_INSTRTP_FIELDSEL().Init(field));
      break;
    }
    case TAG_TYPE_AS_FctTypeInstExpr: {
      const TYPE_AS_Name & polyfct (field.GetRecord(pos_AS_FctTypeInstExpr_polyfct));
      const SEQ<TYPE_AS_Type> & inst (field.GetSequence(pos_AS_FctTypeInstExpr_inst));
      sp.ImpConc(SetContext(ASTAUX::GetCid(field), false))
        .ImpAppend(TYPE_INSTRTP_FIELDSEL().Init(polyfct))
        .ImpAppend(TYPE_INSTRTP_POLYINST().Init(inst, GetClMod()));
      break;
    }
  }
  return sp;
}

// }}}
// {{{ If, Cases

// CompileIfExpr
// eIn : AS`IfExpr
// +> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileIfExpr(const TYPE_AS_IfExpr & eIn)
{
  // Unpack eIn
  const TYPE_AS_Expr & cond_e            (eIn.GetRecord  (pos_AS_IfExpr_test));
  const TYPE_AS_Expr & expr1_e           (eIn.GetRecord  (pos_AS_IfExpr_cons));
  const SEQ<TYPE_AS_ElseifExpr> & elif_l (eIn.GetSequence(pos_AS_IfExpr_elsif));
  const TYPE_AS_Expr & altn_e            (eIn.GetRecord  (pos_AS_IfExpr_altn));

  // cond_l, expr1_l
  TYPE_STKM_SubProgram cond_l (E2I(cond_e));
  TYPE_STKM_SubProgram expr1_l (E2I(expr1_e));
#ifdef VICE
  expr1_l.ImpConc(MkCbr());
#endif //VICE
  
  if (elif_l.IsEmpty()) {
    TYPE_STKM_SubProgram altn_l (E2I(altn_e));
#ifdef VICE
    altn_l.ImpConc(MkBr());
#endif //VICE

    return ConcIfThenElse(cond_l, expr1_l, altn_l);
  }
  else {
    // unpack elif_l
    TYPE_AS_ElseifExpr elm (elif_l.Hd());
    const TYPE_AS_Expr & elif_cond     (elm.GetRecord(pos_AS_ElseifExpr_test));
    const TYPE_AS_Expr & elif_expr     (elm.GetRecord(pos_AS_ElseifExpr_cons));
    const TYPE_CI_ContextId & elif_cid (elm.GetInt   (pos_AS_ElseifExpr_cid));

    // altn_l
    TYPE_STKM_SubProgram altn_l;
    altn_l.ImpConc(CompileIfExpr(TYPE_AS_IfExpr().Init(elif_cond, elif_expr, elif_l.Tl(), altn_e,elif_cid)));
#ifdef VICE
    altn_l.ImpConc(MkCbr());
#endif //VICE

    return ConcIfThenElse(cond_l, expr1_l, altn_l);
  }
}

// CompileCasesExpr
// eIn : AS`CasesExpr
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileCasesExpr(const TYPE_AS_CasesExpr & eIn)
{
  // Unpack eIn
  const TYPE_AS_Expr & sel            (eIn.GetRecord  (pos_AS_CasesExpr_sel));
  const SEQ<TYPE_AS_CaseAltn> & altns (eIn.GetSequence(pos_AS_CasesExpr_altns));
  const Generic & others_e            (eIn.GetField   (pos_AS_CasesExpr_Others));

  TYPE_STKM_SubProgram sp;
  // Test if an others expression exists.
  if (others_e.IsNil()) {
    sp.ImpAppend(TYPE_INSTRTP_ERRINST().Init(TYPE_RTERR_ERR(RTERR_NO_OTHERS_EXPR)));
  }
  else {
    sp.ImpConc(E2I(others_e));
#ifdef VICE
    sp.ImpConc(MkBr());
#endif //VICE
  }

// 20140407 -->
//  TYPE_STKM_SubProgram err_sp;
//  err_sp.ImpAppend(TYPE_INSTRTP_ERRINST().Init(TYPE_RTERR_ERR(RTERR_MULTIPLE_PATTERN)));
// <-- 20140407

// 20140407 -->
//  TYPE_STKM_SubProgram tsp;
//  tsp.ImpAppend(TYPE_INSTRTP_SWAP());
// <-- 20140407

  for (size_t i = altns.Length(); i > 0; i--) {
    // Unpack altn
    const TYPE_AS_CaseAltn & altn (altns[i]);
    const SEQ<TYPE_AS_Pattern> & match_lp (altn.GetSequence(pos_AS_CaseAltn_match));
    const TYPE_AS_Expr & body_e (altn.GetRecord(pos_AS_CaseAltn_body));
    
    // body_instr
    TYPE_STKM_SubProgram body_instr;
    //body_instr.ImpAppend(TYPE_INSTRTP_SELBLKENV().Init(Int(1)))
    //          .ImpAppend(TYPE_INSTRTP_POP().Init(Int(1)))  // remove set of env
    //          .ImpConc(E2I(body_e))
    //          .ImpAppend(TYPE_INSTRTP_POPBLKENV());
    body_instr.ImpAppend(TYPE_INSTRTP_SELBLKENV().Init(Int(1)))
              .ImpConc(E2I(body_e))
              .ImpAppend(TYPE_INSTRTP_POPBLKENV())
              .ImpAppend(TYPE_INSTRTP_REMSTACKELEM().Init(Int(2)));  // remove sel of env
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

    for (size_t j = match_lp.Length(); j > 0; j--) {
      const TYPE_AS_Pattern & pat (match_lp[j]);
      
      // test_instr
      TYPE_STKM_SubProgram test_instr;
      test_instr.ImpAppend(TYPE_INSTRTP_COPYVAL())
                .ImpConc(P2I(pat))
                .ImpAppend(TYPE_INSTRTP_TRYANYMATCH());
#ifdef VICE
      test_instr.ImpConc(MkMatchPattern());
#endif //VICE

#ifdef VICE
      lsp.ImpConc(MkBr());
#endif //VICE

// 20140407 -->
/*
      TYPE_STKM_SubProgram body_instr2, check_multi_match;
      check_multi_match.ImpAppend(TYPE_INSTRTP_SWAP())
                       .ImpConc(tsp);
      tsp = ConcIfThenElse(test_instr, err_sp, tsp);

      body_instr2.ImpConc(ConcIfThenElse(TYPE_STKM_SubProgram().ImpAppend(TYPE_INSTRTP_CASES()),
                                         check_multi_match, TYPE_STKM_SubProgram()))
                 .ImpConc(SetContext(ASTAUX::GetCid(pat), false)); // 20130610
      if (onep)
        body_instr2.ImpConc(body_instr);
      else
        body_instr2.ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_true));
      lsp = ConcIfThenElse(test_instr, body_instr2, lsp);
*/
      if (onep) {
        lsp = ConcIfThenElse(test_instr, body_instr, lsp);
      }
      else {
        lsp = ConcIfThenElse(test_instr, mk_sequence(TYPE_INSTRTP_PUSH().Init(sem_true)), lsp);
      }
// <-- 20140407
    }
    if (!onep) {
      lsp.ImpConc(ConcIfThenElse(TYPE_STKM_SubProgram(), body_instr, sp));
    }
    sp = lsp;
  }
  TYPE_STKM_SubProgram prog;
  prog.ImpConc(E2I(sel))
      .ImpConc(sp)
      .ImpAppend(TYPE_INSTRTP_REMSTACKELEM().Init(Int(2))); // remove sel

  return prog;
}

// }}}
// {{{ Unary, Binary

// CompileUnaryExpr
// eIn : AS`PrefixExpr
// +> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileUnaryExpr(const TYPE_AS_PrefixExpr & eIn)
{
  // Unpack eIn
  const TYPE_AS_UnaryOp & opr (eIn.GetField (pos_AS_PrefixExpr_opr));
  const TYPE_AS_Expr & arg_e  (eIn.GetRecord(pos_AS_PrefixExpr_arg));
  
  // Body of function
  TYPE_STKM_SubProgram sp (E2I(arg_e));
#ifdef VICE
  if(IsRuntimePrefixOp(opr)) {
    sp.ImpConc(MkRuntimePrefixOp(opr));
  }
#endif //VICE
  sp.ImpAppend(TYPE_INSTRTP_UNOP().Init(opr));

  return sp;
}

// CompileBinaryExpr
// eIn : AS`BinaryExpr
// +> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileBinaryExpr(const TYPE_AS_BinaryExpr & eIn)
{
  // Unpack eIn
  const TYPE_AS_Expr & left_e   (eIn.GetRecord(pos_AS_BinaryExpr_left));
  const Int & opr               (eIn.GetInt   (pos_AS_BinaryExpr_opr));
  const TYPE_AS_Expr & right_e  (eIn.GetRecord(pos_AS_BinaryExpr_right));

  switch (opr.GetValue()) {
    case AND:
    case OR:
    case IMPLY: {
      return CompileLogBinaryExpr(left_e, opr, right_e);
      break;
    }
    case EQ:
    case NE:
    case EQUIV:
    case NUMPLUS:
    case NUMMINUS:
    case NUMMULT:
    case NUMDIV:
    case NUMREM:
    case INTDIV:
    case NUMLT:
    case NUMLE:
    case NUMGT:
    case NUMGE:
    case NUMMOD:
    case INSET:
    case NOTINSET:
    case SETUNION:
    case SETINTERSECT:
    case SETMINUS:
    case SUBSET:
    case PROPERSUBSET:
    case SEQCONC:
    case MAPMERGE:
    case MAPDOMRESTTO:
    case MAPDOMRESTBY:
    case MAPRNGRESTTO:
    case MAPRNGRESTBY:
    case COMPOSE:
    case NUMEXP: {
      const TYPE_CI_ContextId & cid (eIn.GetInt(pos_AS_BinaryExpr_cid));
      return CompileOrdinaryBinaryExpr(left_e, opr, right_e, cid);
      break;
    }
    default: {
      RTERR::Error(L"StackCompiler::CompileBinaryExpr", RTERR_UNKNOWN_OPERATOR, Nil(), Nil(), Sequence());
      return TYPE_STKM_SubProgram(); // Dummy
    }
  }
}

// CompileLogBinaryExpr
// left_e : AS`Expr
// op : AS`BinaryOp
// right_e : AS`Expr
// +> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileLogBinaryExpr(const TYPE_AS_Expr & left_e,
                                                         const Int & op,
                                                         const TYPE_AS_Expr & right_e)
{
  TYPE_STKM_SubProgram sp_left (E2I(left_e));

  TYPE_STKM_SubProgram def_true, def_false;
  def_true.ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_true));
  def_false.ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_false));
  
  TYPE_STKM_SubProgram sp_right;
  sp_right.ImpConc(ConcIfThenElse(E2I(right_e), def_true, def_false));

  switch (op.GetValue()) {
    case AND:   { return ConcIfThenElse(sp_left, sp_right, def_false); break; }
    case OR:    { return ConcIfThenElse(sp_left, def_true, sp_right); break; }
    case IMPLY: { return ConcIfThenElse(sp_left, sp_right, def_true); break; }
  }
  return TYPE_STKM_SubProgram(); // Dummy
}

// CompileOrdinaryBinaryExpr
// left_e : AS`Expr
// opr : AS`BinaryOp
// right_e : AS`Expr
// - : CI`ContextId
// +> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileOrdinaryBinaryExpr(const TYPE_AS_Expr & left_e,
                                                              const TYPE_AS_BinaryOp & opr,
                                                              const TYPE_AS_Expr & right_e,
                                                              const TYPE_CI_ContextId &) 
{
  TYPE_STKM_SubProgram sp;
  sp.ImpConc(E2I(left_e))
    .ImpConc(E2I(right_e));
#ifdef VICE
  if (IsRuntimeBinaryOp(opr)) {
    sp.ImpConc(MkRuntimeBinaryOp(opr));
  }
#endif //VICE
  sp.ImpAppend(TYPE_INSTRTP_BINOP().Init(opr));

  return sp;
} 

// }}}
// {{{ SetRange, SubSequence

// CompileSetRangeExpr
// eIn : AS`SetRangeExpr
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileSetRangeExpr(const TYPE_AS_SetRangeExpr & eIn)
{
  // Body of function
  TYPE_STKM_SubProgram sp;
  sp.ImpConc(E2I(eIn.GetRecord(pos_AS_SetRangeExpr_lb)))
    .ImpConc(E2I(eIn.GetRecord(pos_AS_SetRangeExpr_ub)))
#ifdef VICE
    .ImpConc(MkRuntimeSetSeqMap(eIn))
#endif //VICE
    .ImpAppend(TYPE_INSTRTP_SETRNG());

  return sp;
}

// CompileSubSequenceExpr
// eIn : AS`SubSequenceExpr
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileSubSequenceExpr(const TYPE_AS_SubSequenceExpr & eIn)
{
  // Body of function
  TYPE_STKM_SubProgram sp;
  sp.ImpConc(E2I(eIn.GetRecord(pos_AS_SubSequenceExpr_sequence)))
    .ImpConc(E2I(eIn.GetRecord(pos_AS_SubSequenceExpr_frompos)))
    .ImpConc(E2I(eIn.GetRecord(pos_AS_SubSequenceExpr_topos)))
#ifdef VICE
    .ImpConc(MkRuntimeSetSeqMap(eIn))
#endif //VICE
    .ImpAppend(TYPE_INSTRTP_SUBSEQ());

  return sp;
}

// }}}
// {{{ SetEnum, SeqEnum, MapEnum

// CompileSetEnumExpr
// eIn : AS`SetEnumerationExpr
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileSetEnumExpr(const TYPE_AS_SetEnumerationExpr & eIn)
{
  // Unpack eIn
  const SEQ<TYPE_AS_Expr> & els_l (eIn.GetSequence(pos_AS_SetEnumerationExpr_els));

  // Body of function
  TYPE_STKM_SubProgram sp;

  if (!els_l.IsEmpty()) {
    size_t len_els_l = els_l.Length();
    for (size_t index = 1; index <= len_els_l; index++) {
      sp.ImpConc(E2I(els_l[index]));
    }
    sp.ImpAppend(TYPE_INSTRTP_SETCONS().Init(Int(len_els_l)));
  }
  else {
    sp.ImpAppend(TYPE_INSTRTP_PUSH().Init(mk_SEM_SET(SET<TYPE_SEM_VAL>())));
  }
  return sp;
}

// CompileSeqEnumExpr
// eIn : AS`SeqEnumerationExpr
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileSeqEnumExpr(const TYPE_AS_SeqEnumerationExpr & eIn)
{
  // Unpack eIn
  const SEQ<TYPE_AS_Expr> & els_l (eIn.GetSequence(pos_AS_SeqEnumerationExpr_els));
  
  // Body of function
  TYPE_STKM_SubProgram sp;
  
  if (!els_l.IsEmpty()) {
    size_t len_els_l = els_l.Length();
    for (size_t i = 1; i <= len_els_l; i++) {
      sp.ImpConc(E2I(els_l[i]));
    }
    sp.ImpAppend(TYPE_INSTRTP_SEQCONS().Init(Int(len_els_l)));
  }
  else {
    sp.ImpAppend(TYPE_INSTRTP_PUSH().Init(mk_SEM_SEQ(SEQ<TYPE_SEM_VAL>())));
  }
  return sp;
}

// CompileMapEnumExpr
// eIn : AS`MapEnumerationExpr
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileMapEnumExpr(const TYPE_AS_MapEnumerationExpr & eIn)
{
  // Unpack eIn
  const SEQ<TYPE_AS_Maplet> & els_l (eIn.GetSequence(pos_AS_MapEnumerationExpr_els));

  // Body of function
  TYPE_STKM_SubProgram sp;

  if (!els_l.IsEmpty()) {
    size_t len_els_l = els_l.Length();
    for (size_t i = 1; i <= len_els_l; i++) {
      // Unpack elem
      const TYPE_AS_Maplet & ml (els_l[i]);
      sp.ImpConc(E2I(ml.GetRecord(pos_AS_Maplet_mapdom)))
        .ImpConc(E2I(ml.GetRecord(pos_AS_Maplet_maprng)));
    }
    sp.ImpAppend(TYPE_INSTRTP_MAPCONS().Init(Int(len_els_l)));
  }
  else {
    sp.ImpAppend(TYPE_INSTRTP_PUSH().Init(mk_SEM_MAP(Map())));
  }
  return sp;
}

// }}}
// {{{ SetComprehension, SeqComprehension, MapComprehension

// CompileSetComprehensionExpr
// eIn : AS`SetComprehensionExpr
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileSetComprehensionExpr(const TYPE_AS_SetComprehensionExpr & eIn)
{
  // Unpack eIn
  const TYPE_AS_Expr & elem (eIn.GetRecord(pos_AS_SetComprehensionExpr_elem));
  const SEQ<TYPE_AS_MultBind> & bind (eIn.GetSequence(pos_AS_SetComprehensionExpr_bind));
  const Generic & pred (eIn.GetField(pos_AS_SetComprehensionExpr_pred));

  // prep_instr
  TYPE_STKM_SubProgram prep_instr;
  prep_instr.ImpConc(CompileMultBindL(bind, Int(DO_PARTITION)));
  prep_instr.ImpAppend(TYPE_INSTRTP_PUSH().Init(mk_SEM_SET(SET<TYPE_SEM_VAL>())));
  
  // ConcIfThenElse
  TYPE_STKM_SubProgram then_instr;
  then_instr.ImpConc(E2I(elem));
  then_instr.ImpAppend(TYPE_INSTRTP_ADDSET());
#ifdef VICE
  then_instr.ImpConc(MkSetCompInsert());
#endif //VICE

  // loop_instr
  TYPE_STKM_SubProgram loop_instr;
  loop_instr.ImpAppend(TYPE_INSTRTP_SELBLKENV().Init(Int(2)));
  if (pred.IsNil()) {
    loop_instr.ImpConc(then_instr);
  }
  else {
    // pred_instr
    TYPE_STKM_SubProgram pred_instr (E2I(pred));
    loop_instr.ImpConc(ConcIfThenElse(pred_instr, then_instr, TYPE_STKM_SubProgram()));
  }
  loop_instr.ImpAppend(TYPE_INSTRTP_POPBLKENV());

  TYPE_STKM_SubProgram condition;
  condition.ImpAppend(TYPE_INSTRTP_ISNOTEMPTYSET().Init(Int(2)));

  TYPE_STKM_SubProgram clean_stack;
  clean_stack.ImpAppend(TYPE_INSTRTP_REMSTACKELEM().Init(Int(2)));

  TYPE_STKM_SubProgram sp;
  sp.ImpConc(prep_instr)
    .ImpConc(CombWhileLoopWithCond(condition, loop_instr))
    .ImpConc(clean_stack);

  return sp;
}

// CompileSeqComprehensionExpr
// eIn : AS`SeqComprehensionExpr
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileSeqComprehensionExpr(const TYPE_AS_SeqComprehensionExpr & eIn)
{
  // Unpack eIn
  const TYPE_AS_Expr & elem (eIn.GetRecord(pos_AS_SeqComprehensionExpr_elem));
  const Record & bind (eIn.GetRecord(pos_AS_SeqComprehensionExpr_bind));
  const Generic & pred (eIn.GetField(pos_AS_SeqComprehensionExpr_pred));

  // prep_instr
  TYPE_STKM_SubProgram prep_instr;
  switch (bind.GetTag()) {
    case TAG_TYPE_AS_SetBind: {
      // Unpack bind
      const TYPE_AS_Pattern & pat (bind.GetRecord(pos_AS_SetBind_pat));
      const TYPE_AS_Expr & set_e (bind.GetRecord(pos_AS_SetBind_Set));

      if (!pat.Is(TAG_TYPE_AS_PatternName) && elem.Is(TAG_TYPE_AS_Name)) {
        // experimental !!
        SEQ<TYPE_AS_Pattern> pat_l;
        pat_l.ImpAppend(pat);
        TYPE_AS_MultSetBind msb;
        msb.SetField(pos_AS_MultSetBind_pat, pat_l);
        msb.SetField(pos_AS_MultSetBind_Set, set_e);
        msb.SetField(pos_AS_MultSetBind_cid, bind.GetInt(pos_AS_SetBind_cid));
        SEQ<TYPE_AS_MultBind> mb_l;
        mb_l.ImpAppend(msb);

        TYPE_AS_SetComprehensionExpr sce;
        sce.SetField(pos_AS_SetComprehensionExpr_elem, elem);
        sce.SetField(pos_AS_SetComprehensionExpr_bind, mb_l);
        sce.SetField(pos_AS_SetComprehensionExpr_pred, pred);
        sce.SetField(pos_AS_SetComprehensionExpr_cid, bind.GetInt(pos_AS_SetBind_cid));

        // Note : same element will be igored.
        prep_instr.ImpConc(P2I(TYPE_AS_PatternName().Init(elem, Nil(), elem.GetInt(pos_AS_Name_cid))))
                  .ImpConc(E2I(sce))
                  .ImpAppend(TYPE_INSTRTP_SEQCOMPBIND().Init(Bool(true)))
                  .ImpAppend(TYPE_INSTRTP_PUSH().Init(mk_SEM_SEQ(SEQ<TYPE_SEM_VAL>())));
      }
      else {
        prep_instr.ImpConc(P2I(pat))
                  .ImpConc(E2I(set_e))
                  .ImpAppend(TYPE_INSTRTP_SEQCOMPBIND().Init(Bool(true)))
                  .ImpAppend(TYPE_INSTRTP_PUSH().Init(mk_SEM_SEQ(SEQ<TYPE_SEM_VAL>())));
      }
      break;
    }
    case TAG_TYPE_AS_SeqBind: {
      // Unpack bind
      const TYPE_AS_Pattern & pat (bind.GetRecord(pos_AS_SeqBind_pat));
      const TYPE_AS_Expr & seq_e (bind.GetRecord(pos_AS_SeqBind_Seq));
        prep_instr.ImpConc(P2I(pat))
                  .ImpConc(E2I(seq_e))
                  .ImpAppend(TYPE_INSTRTP_SEQCOMPBIND().Init(Bool(false)))
                  .ImpAppend(TYPE_INSTRTP_PUSH().Init(mk_SEM_SEQ(SEQ<TYPE_SEM_VAL>())));
      break;
    }
  }
    
  TYPE_STKM_SubProgram condition;
  condition.ImpAppend(TYPE_INSTRTP_ISNOTEMPTYSEQ().Init(Int(2)));

  TYPE_STKM_SubProgram then_branch;
  then_branch.ImpConc(E2I(elem))
             .ImpAppend(TYPE_INSTRTP_APPENDSEQ());
#ifdef VICE
  then_branch.ImpConc(MkSeqCompInsert());
#endif //VICE

  TYPE_STKM_SubProgram loop_instr;
  loop_instr.ImpAppend(TYPE_INSTRTP_EMPTYBLKENV().Init(sem_read_only))
            .ImpAppend(TYPE_INSTRTP_SEQELEMMATCH().Init(Int(3)));

  if (pred.IsNil()) {
    loop_instr.ImpConc(then_branch);
  }
  else {
    // pred_instr
    TYPE_STKM_SubProgram pred_instr (E2I(pred));
    loop_instr.ImpConc(ConcIfThenElse(pred_instr, then_branch, TYPE_STKM_SubProgram()));
  }
  loop_instr.ImpAppend(TYPE_INSTRTP_POPBLKENV());

  TYPE_STKM_SubProgram clean_stack;
  clean_stack.ImpAppend(TYPE_INSTRTP_REMSTACKELEM().Init(Int(2)))
             .ImpAppend(TYPE_INSTRTP_REMSTACKELEM().Init(Int(2)));

  TYPE_STKM_SubProgram sp;
  sp.ImpConc(prep_instr)
    .ImpConc(CombWhileLoopWithCond(condition, loop_instr))
    .ImpConc(clean_stack);

  return sp;
}

// CompileMapComprehensionExpr
// eIn : AS`MapComprehensionExpr
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileMapComprehensionExpr(const TYPE_AS_MapComprehensionExpr & eIn)
{
  const TYPE_AS_Maplet & elem (eIn.GetRecord(pos_AS_MapComprehensionExpr_elem));
  const SEQ<TYPE_AS_MultBind> & bind (eIn.GetSequence(pos_AS_MapComprehensionExpr_bind));
  const Generic & pred (eIn.GetField(pos_AS_MapComprehensionExpr_pred));

  // prep_instr
  TYPE_STKM_SubProgram prep_instr;
  prep_instr.ImpConc(CompileMultBindL(bind, Int(DO_PARTITION)))
            .ImpAppend(TYPE_INSTRTP_PUSH().Init(mk_SEM_MAP(Map())));

  TYPE_STKM_SubProgram then_instr;
  then_instr.ImpConc(E2I(elem.GetRecord(pos_AS_Maplet_mapdom)));
  then_instr.ImpConc(E2I(elem.GetRecord(pos_AS_Maplet_maprng)));
  then_instr.ImpAppend(TYPE_INSTRTP_APPENDMAP());
#ifdef VICE
  then_instr.ImpConc(MkMapCompInsert());
#endif //VICE

  TYPE_STKM_SubProgram loop_instr;
  loop_instr.ImpAppend(TYPE_INSTRTP_SELBLKENV().Init(Int(2)));
  if (pred.IsNil()) {
    loop_instr.ImpConc(then_instr);
  }
  else {
    // pred_instr
    TYPE_STKM_SubProgram pred_instr (E2I(pred));
    loop_instr.ImpConc(ConcIfThenElse(pred_instr, then_instr, TYPE_STKM_SubProgram()));
  }
  loop_instr.ImpAppend(TYPE_INSTRTP_POPBLKENV());

  TYPE_STKM_SubProgram condition;
  condition.ImpAppend(TYPE_INSTRTP_ISNOTEMPTYSET().Init(Int(2)));

  TYPE_STKM_SubProgram clean_stack;
  clean_stack.ImpAppend(TYPE_INSTRTP_REMSTACKELEM().Init(Int(2)));

  // rest of prog
  TYPE_STKM_SubProgram sp;
  sp.ImpConc(prep_instr)
    .ImpConc(CombWhileLoopWithCond(condition, loop_instr))
    .ImpConc(clean_stack);

  return sp;
}

// }}}
// {{{ TupleConstructor, RecordConstructor

// CompileTupleConstructorExpr
// eIn : AS`TupleConstructorExpr
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileTupleConstructorExpr(const TYPE_AS_TupleConstructorExpr & eIn)
{
  // Unpack eIn
  const SEQ<TYPE_AS_Expr> & fields (eIn.GetSequence(pos_AS_TupleConstructorExpr_fields));

  // Body of teh function
  TYPE_STKM_SubProgram sp;

  size_t len_fields = fields.Length();
  for (size_t i = 1; i <= len_fields; i++) {
    sp.ImpConc(E2I(fields[i]));
  }
  sp.ImpAppend(TYPE_INSTRTP_TUPCONS().Init(Int(len_fields)));

  return sp;
}

// CompileRecordConstructorExpr
// eIn : AS`RecordConstructorExpr
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileRecordConstructorExpr(const TYPE_AS_RecordConstructorExpr & eIn)
{
  // Unpack eIn
  const TYPE_AS_Name & tag         (eIn.GetRecord(pos_AS_RecordConstructorExpr_tag));
  const SEQ<TYPE_AS_Expr> & fields (eIn.GetSequence(pos_AS_RecordConstructorExpr_fields));

  // body of program
  TYPE_STKM_SubProgram sp;

  size_t len_fields = fields.Length();
  for (size_t i = 1; i <= len_fields; i++) {
    sp.ImpConc(E2I(fields[i]));
  }
  sp.ImpAppend(TYPE_INSTRTP_RECCONS().Init(tag, Int(fields.Length())));

  return sp;
}

// }}}
// {{{ RecordModufier, SeqModifyer

// CompileRecordModifierExpr
// eIn : AS`RecordModifierExpr
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileRecordModifierExpr(const TYPE_AS_RecordModifierExpr & eIn)
{
  // Unpack eIn
  const TYPE_AS_Expr & rec (eIn.GetRecord(pos_AS_RecordModifierExpr_rec));
  const SEQ<TYPE_AS_RecordModification> & modifiers (eIn.GetSequence(pos_AS_RecordModifierExpr_modifiers));

  // Body of function
  TYPE_STKM_SubProgram sp_val;
  SEQ<TYPE_AS_Name> fid_l;
  size_t len_modifiers = modifiers.Length();
  for (size_t i = 1; i <= len_modifiers; i++) {
    const TYPE_AS_RecordModification & rm (modifiers[i]);
    fid_l.ImpAppend(rm.GetRecord(pos_AS_RecordModification_field));
    sp_val.ImpConc(E2I(rm.GetRecord(pos_AS_RecordModification_newexpr)));
  }

  TYPE_STKM_SubProgram sp (E2I(rec));
  sp.ImpConc(sp_val);
  sp.ImpAppend(TYPE_INSTRTP_RECMOD().Init(fid_l));
  return sp;
}

// CompileSeqModifyMapOverrideExpr
// eIn : AS`SeqModifyMapOverrideExpr
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileSeqModifyMapOverrideExpr(const TYPE_AS_SeqModifyMapOverrideExpr & eIn)
{
  // Unpack eIn
  const TYPE_AS_Expr & sm_e (eIn.GetRecord(pos_AS_SeqModifyMapOverrideExpr_seqmap));
  const TYPE_AS_Expr & m_e (eIn.GetRecord(pos_AS_SeqModifyMapOverrideExpr_mapexp));

  // Body of function
  TYPE_STKM_SubProgram sp;
  sp.ImpConc(E2I(sm_e))
    .ImpConc(E2I(m_e))
#ifdef VICE
    .ImpConc(MkRuntimeSetSeqMap(eIn))
#endif //VICE
    .ImpAppend(TYPE_INSTRTP_SEQMAPOVER());

  return sp;
}

// }}}
// {{{ Tuple Select, Type Judgement, Pre condition Application

// CompileTupleSelectExpr
// eIn : AS`TupleSelectExpr
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileTupleSelectExpr(const TYPE_AS_TupleSelectExpr & eIn)
{
  // Unpack eIn
  const TYPE_AS_Expr & tuple (eIn.GetRecord(pos_AS_TupleSelectExpr_tuple));
  const TYPE_AS_Expr & no (eIn.GetRecord(pos_AS_TupleSelectExpr_no));
  
  TYPE_STKM_SubProgram sp (E2I(tuple));
  if (no.Is(TAG_TYPE_AS_NumLit)) {
    sp.ImpAppend(TYPE_INSTRTP_TUPSEL().Init(no.GetReal(pos_AS_NumLit_val)));
  }
  else {
    sp.ImpAppend(TYPE_INSTRTP_TUPSEL().Init(no.GetReal(pos_AS_RealLit_val)));
  }
  return sp;
}

// CompileTypeJudgementExpr
// eIn : AS`TypeJudgementExpr
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileTypeJudgementExpr(const TYPE_AS_TypeJudgementExpr & eIn)
{
  // Unpack eIn
  const TYPE_AS_Expr & ex (eIn.GetRecord(pos_AS_TypeJudgementExpr_expr));
  const TYPE_AS_Type & tp (eIn.GetRecord(pos_AS_TypeJudgementExpr_type));

  TYPE_STKM_SubProgram sp (E2I(ex));
  sp.ImpAppend(TYPE_INSTRTP_TYPEJUDGE().Init(tp));

  return sp;
}

// CompilePreConditionApplyExpr
// eIn : AS`PreConditionApplyExpr
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompilePreConditionApplyExpr(const TYPE_AS_PreConditionApplyExpr & eIn)
{
  const TYPE_AS_Expr fct (eIn.GetRecord(pos_AS_PreConditionApplyExpr_fct));
  const SEQ<TYPE_AS_Expr> & arg_l (eIn.GetSequence(pos_AS_PreConditionApplyExpr_arg));

  TYPE_STKM_SubProgram thensp;

  size_t len_arg_l = arg_l.Length();
  for (size_t i = 1; i <= len_arg_l; i++) {
    thensp.ImpConc(E2I(arg_l[i]));
  }
  thensp.ImpAppend(TYPE_INSTRTP_PUSHLIST().Init(Int(len_arg_l)));

  thensp.ImpConc(SetContext(ASTAUX::GetCid(fct), false))
        .ImpAppend(TYPE_INSTRTP_APPLY());

  TYPE_STKM_SubProgram elsesp;
  elsesp.ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_true));

  TYPE_STKM_SubProgram getpre;
  getpre.ImpAppend(TYPE_INSTRTP_GETPRE());

  TYPE_STKM_SubProgram sp;
  sp.ImpConc(E2I(fct))
    .ImpConc(ConcIfThenElse(getpre, thensp, elsesp));
  return sp;
}

// }}}
// {{{ Lambda, FctTypeInst

// CompileLambdaExpr
// eIn : AS`LambdaExpr
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileLambdaExpr(const TYPE_AS_LambdaExpr & eIn)
{
  // Unpack eIn
  SEQ<type_dL> type_l_l;
  type_l_l.ImpAppend(eIn.GetSequence(pos_AS_LambdaExpr_parm));
  TYPE_AS_Expr body (eIn.GetRecord(pos_AS_LambdaExpr_body));
  Generic rngtp = eIn.GetField(pos_AS_LambdaExpr_type);
  while (body.Is(TAG_TYPE_AS_LambdaExpr)) {
    type_l_l.ImpPrepend(body.GetSequence(pos_AS_LambdaExpr_parm));
    rngtp = body.GetSequence(pos_AS_LambdaExpr_type);
    body = body.GetRecord(pos_AS_LambdaExpr_body);
  }

  TYPE_AS_Type lft (rngtp.IsNil() ? (TYPE_AS_Type)TYPE_AS_AllType().Init(NilContextId)
                                  : (TYPE_AS_Type)rngtp);
    
  SEQ<type_dL> pi_l_l;
  size_t len_type_l_l = type_l_l.Length();
  for (size_t idx = 1; idx <= len_type_l_l; idx++ ) {
    const SEQ<TYPE_AS_TypeBind> & type_l (type_l_l[idx]);
    SEQ<TYPE_AS_Pattern> pat_l;
    SEQ<TYPE_AS_Type> tp_l;

    size_t len_type_l = type_l.Length();
    for (size_t i = 1; i <= len_type_l; i++) {
      const TYPE_AS_TypeBind & tb (type_l[i]);

      pat_l.ImpAppend(tb.GetRecord(pos_AS_TypeBind_pat));
      tp_l.ImpAppend(tb.GetRecord(pos_AS_TypeBind_tp));
    }

    // TODO: A pattern such as '(1 + 2)' becomes '-'(any) in P2P
    pi_l_l.ImpPrepend(PL2PL(pat_l));
    
    // TODO: TypeName must have Class/Module name
    lft = TYPE_AS_TotalFnType().Init(tp_l, lft, NilContextId);
  }

  TYPE_AS_Name clmodName (GetClMod());
  TYPE_STKM_SubProgramId body_prog (CompileLambdaBody(body));
  
  // This is equal to mk_SEM`BlkEnv({|->},<READ_ONLY>)
  TYPE_SEM_BlkEnv blkenv (AUX::MkEmptyBlkEnv(sem_read_only));

  TYPE_SEM_ExplFN efn;
  efn.Init(lft,
           pi_l_l,
           body_prog,
           blkenv,
           MAP<TYPE_AS_TypeVar,TYPE_AS_Type>(),
           Nil(), // no name means Lambda function
           clmodName,
           Bool(false),
           Nil(),
           Int(PRIVATE_AS));

  TYPE_STKM_SubProgram sp;
  sp.ImpAppend(TYPE_INSTRTP_PUSH().Init(SemRec::CompFN(efn)));
  sp.ImpAppend(TYPE_INSTRTP_UPDATECLOSENV().Init(body));
  return sp;
}

// CompileFctTypeInstExpr
// eIn : AS`FctTypeInstExpr
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileFctTypeInstExpr(const TYPE_AS_FctTypeInstExpr & eIn)
{
  // Unpack eIn
  const TYPE_AS_Name & polyfct (eIn.GetRecord(pos_AS_FctTypeInstExpr_polyfct));
  const SEQ<TYPE_AS_Type> & inst (eIn.GetSequence(pos_AS_FctTypeInstExpr_inst));

  // Body of function
  TYPE_STKM_SubProgram sp;
  sp.ImpAppend(TYPE_INSTRTP_LOOKUP().Init(polyfct))
    .ImpAppend(TYPE_INSTRTP_POLYINST().Init(inst, GetClMod()));
  return sp;
}

// }}}
// {{{ IsExpr, TokenConstructor

// CompileIsExpr
// eIn : AS`IsExpr
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileIsExpr(const TYPE_AS_IsExpr & eIn)
{
  // Unpack eIn
  const Record & Type (eIn.GetRecord(pos_AS_IsExpr_type)); // AS`BasicType | AS`Name
  const TYPE_AS_Expr & arg_e (eIn.GetRecord(pos_AS_IsExpr_arg));

  TYPE_STKM_SubProgram sp (E2I(arg_e));
  sp.ImpAppend(TYPE_INSTRTP_ISCHECK().Init(Type));

  return sp;
}

// CompileNarrowExpr
// eIn : AS`NarrowExpr
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileNarrowExpr(const TYPE_AS_NarrowExpr & eIn)
{
  // Unpack eIn
  const TYPE_AS_Type & type (eIn.GetRecord(pos_AS_NarrowExpr_type));
  const TYPE_AS_Expr & expr (eIn.GetRecord(pos_AS_NarrowExpr_expr));

  TYPE_STKM_SubProgram sp (E2I(expr));
  sp.ImpAppend(TYPE_INSTRTP_DTC().Init(type));

  return sp;
}

// CompileTokenConstructorExpr
// eIn : AS`TokenConstructorExpr
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileTokenConstructorExpr(const TYPE_AS_TokenConstructorExpr & eIn)
{
  // Unpack eIn
  const TYPE_AS_Expr & expr (eIn.GetRecord(pos_AS_TokenConstructorExpr_field));
  
  // Body of function
  TYPE_STKM_SubProgram sp (E2I(expr));
  sp.ImpAppend(TYPE_INSTRTP_TOKENVAL());

  return sp;
}

// }}}

#ifdef VDMPP
// {{{ Self, New

// CompileSelfExpr
// eIn : AS`SelfExpr
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileSelfExpr(const TYPE_AS_SelfExpr & eIn)
{
  TYPE_STKM_SubProgram sp;
  sp.ImpAppend(TYPE_INSTRTP_SELFEXPR());

  return sp;
}

// CompileNewExpr
// eIn : AS`NewExpr
// dlobject : [token]
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileNewExpr(const TYPE_AS_NewExpr & eIn, 
                                                   const Generic & dlobject)
{
  const TYPE_AS_Name & nm         (eIn.GetRecord  (pos_AS_NewExpr_cls));
  const SEQ<TYPE_AS_Expr> & exprs (eIn.GetSequence(pos_AS_NewExpr_args));

  TYPE_STKM_SubProgram sp;
  sp.ImpConc(SetContext(eIn.GetInt(pos_AS_NewExpr_cid), false)); // 20070109

  size_t len_exprs = exprs.Length();
  for (size_t i = 1; i <= len_exprs; i++) {
    sp.ImpConc(E2I(exprs[i]));
  }
  sp.ImpConc(SetContext(nm.GetInt(pos_AS_Name_cid), false))
    .ImpAppend(TYPE_INSTRTP_NEWOBJ().Init(nm, dlobject))
    .ImpAppend(TYPE_INSTRTP_INITCLASS().Init(nm, Int(exprs.Length())))
    .ImpAppend(TYPE_INSTRTP_NEWCOMPL().Init(Bool(true)));
  if (!exprs.IsEmpty()) {
    sp.ImpAppend(TYPE_INSTRTP_POP().Init(Int(1)));
  }
  return sp;
}

// }}}
// {{{ IsOfClass, IsOfBaseClass 

// CompileIsOfClassExpr
// eIn : AS`IsOfClassExpr
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileIsOfClassExpr(const TYPE_AS_IsOfClassExpr & eIn)
{
  // Unpack eIn
  const TYPE_AS_Name & clnm (eIn.GetRecord(pos_AS_IsOfClassExpr_cls));
  const TYPE_AS_Expr & arg  (eIn.GetRecord(pos_AS_IsOfClassExpr_arg));

  // Body of function
  TYPE_STKM_SubProgram sp (E2I(arg));
  sp.ImpConc(SetContext(clnm.GetInt(pos_AS_Name_cid), false)) // 20090126
    .ImpAppend(TYPE_INSTRTP_ISOFCLASS().Init(clnm));

  return sp;
}

// CompileIsOfBaseClassExpr
// eIn : AS`IsOfBaseClassExpr
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileIsOfBaseClassExpr(const TYPE_AS_IsOfBaseClassExpr & eIn)
{
   // Unpack eIn
  const TYPE_AS_Name & clnm (eIn.GetRecord(pos_AS_IsOfBaseClassExpr_cls));
  const TYPE_AS_Expr & arg  (eIn.GetRecord(pos_AS_IsOfBaseClassExpr_arg));

  // Body of function
  TYPE_STKM_SubProgram sp (E2I(arg));
  sp.ImpConc(SetContext(clnm.GetInt(pos_AS_Name_cid), false)) // 20090126
    .ImpAppend(TYPE_INSTRTP_ISOFBASECLASS().Init(clnm));

  return sp; 
}

// }}}
// {{{ SameBaseClass, SameClass

// CompileSameBaseClassExpr
// eIn : AS`SameBaseClassExpr
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileSameBaseClassExpr(const TYPE_AS_SameBaseClassExpr & eIn)
{
  const TYPE_AS_Expr & expr1 (eIn.GetRecord(pos_AS_SameBaseClassExpr_expr1));
  const TYPE_AS_Expr & expr2 (eIn.GetRecord(pos_AS_SameBaseClassExpr_expr2));

  TYPE_STKM_SubProgram sp (E2I(expr1));
  sp.ImpConc(E2I(expr2))
    .ImpAppend(TYPE_INSTRTP_SAMEBASECLASS());

  return sp;
}

// CompileSameClassExpr
// eIn : AS`SameClassExpr
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileSameClassExpr(const TYPE_AS_SameClassExpr & eIn)
{
  const TYPE_AS_Expr & expr1 (eIn.GetRecord(pos_AS_SameClassExpr_expr1));
  const TYPE_AS_Expr & expr2 (eIn.GetRecord(pos_AS_SameClassExpr_expr2));

  TYPE_STKM_SubProgram sp (E2I(expr1));
  sp.ImpConc(E2I(expr2))
    .ImpAppend(TYPE_INSTRTP_SAMECLASS());

  return sp;
}

// }}}
// {{{ ThreadId, Act, Fin, Active, Waiting, Req

// CompileThreadIdExpr
// eIn : AS`ThreadIdExpr
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileThreadIdExpr(const TYPE_AS_ThreadIdExpr & eIn)
{
  TYPE_STKM_SubProgram sp;
  sp.ImpAppend(TYPE_INSTRTP_THREADID());

  return sp;
}

// CompileHistoryExpr
// methd : seq of AS`Name
// tp : INSTRTP`HistoryKind
// ==> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CompileHistoryExpr(const SEQ<TYPE_AS_Name> & mthds,
                                                       const TYPE_INSTRTP_HistoryKind & tp)
{
  SEQ<TYPE_AS_Name> qualified_names;
  size_t len_mthds = mthds.Length();
  for (size_t i = 1; i <= len_mthds; i++) {
    const TYPE_AS_Name & mthd (mthds[i]);
    if (mthd.GetSequence(pos_AS_Name_ids).Length() == 2) {
      qualified_names.ImpAppend(mthd);
    }
    else {
      qualified_names.ImpAppend(AUX::ConstructDoubleName(GetClMod(), mthd));
    }
  }
#ifdef VICE
  if( !this->opnm.IsNil() ) {
    this->histnms.ImpUnion(mthds.Elems());
  }
#endif // VICE
  TYPE_STKM_SubProgram sp;
  sp.ImpAppend(TYPE_INSTRTP_HISTORY().Init(tp, qualified_names));

  return sp;
}

// }}}
#endif //VDMPP

// {{{ ConcIfThenElse, CombRepeatUntil, CombWhileLoop

// ConcIfThenElse
// cond_l : STKM`SubProgram
// expl_l : STKM`SubProgram
// altn_l : STKM`SubProgram
// +> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::ConcIfThenElse(const TYPE_STKM_SubProgram & cond_l, 
                                                   const TYPE_STKM_SubProgram & exp1_l,
                                                   const TYPE_STKM_SubProgram & altn_l) const
{
  if (exp1_l.IsEmpty()) {
    TYPE_STKM_SubProgram sp;
    sp.ImpConc(cond_l)
      .ImpAppend(TYPE_INSTRTP_CBR().Init(Int(altn_l.Length())))
      .ImpConc(altn_l);
    return sp;
  }
  else if (altn_l.IsEmpty()) {
    TYPE_STKM_SubProgram sp;
    sp.ImpConc(cond_l)
      .ImpAppend(TYPE_INSTRTP_CNBR().Init(Int(exp1_l.Length())))
      .ImpConc(exp1_l);
    return sp;
  }
  else {
    TYPE_STKM_SubProgram sp;
    sp.ImpConc(cond_l)
      .ImpAppend(TYPE_INSTRTP_CBR().Init(Int(altn_l.Length()+1)))
      .ImpConc(altn_l)
      .ImpAppend(TYPE_INSTRTP_BR().Init(Int(exp1_l.Length())))
      .ImpConc(exp1_l);
    return sp;
  }
}

// CombRepeatUntil
// loop_instr : STKM`SubProgram
// +> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CombRepeatUntil(const TYPE_STKM_SubProgram & loop_instr) const
{
  TYPE_STKM_SubProgram sp;
  sp.ImpConc(loop_instr)
    .ImpAppend(TYPE_INSTRTP_CNBR().Init(Int(-loop_instr.Length()-1)));
  return sp;
}

// CombWhileLoop
// loop_instr : STKM`SubProgram
// -> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CombWhileLoop(const TYPE_STKM_SubProgram & loop_instr) const
{
  return 
    ConcIfThenElse(TYPE_STKM_SubProgram(),
                   TYPE_STKM_SubProgram(loop_instr).ImpAppend(TYPE_INSTRTP_CBR().Init(Int(-loop_instr.Length()-1))),
                   TYPE_STKM_SubProgram());
}

// CombWhileLoopWithCond
// condition : STKM`SubProgram
// body : STKM`SubProgram
// -> STKM`SubProgram
TYPE_STKM_SubProgram StackCompiler::CombWhileLoopWithCond(const TYPE_STKM_SubProgram & condition,
                                                          const TYPE_STKM_SubProgram & loop_instr) const
{
/*
  TYPE_STKM_SubProgram while_body;
  while_body.ImpConc(condition)
            .ImpAppend(TYPE_INSTRTP_CNBR().Init(Int(loop_instr.Length()+1)))
            .ImpConc(loop_instr);
  TYPE_STKM_SubProgram sp;
  sp.ImpConc(while_body)
    .ImpAppend(TYPE_INSTRTP_BR().Init(Int(-while_body.Length()-1)));
  return sp;
*/
  TYPE_STKM_SubProgram sp;
  sp.ImpAppend(TYPE_INSTRTP_BR().Init(Int(loop_instr.Length())))
    .ImpConc(loop_instr)
    .ImpConc(condition)
    .ImpAppend(TYPE_INSTRTP_CBR().Init(Int(-loop_instr.Length()-condition.Length()-1)));
  return sp;
}

#ifdef VICE
// AddDepIds
// nm : AS`Name
// ops : set of AS`Name
// ==> ()
void StackCompiler::AddDepIds(const TYPE_AS_Name & nm, const SET<TYPE_AS_Name> & ops)
{
  this->opnm = nm;
  this->opnms = ops;
  this->nms = Set();
  this->histnms = Set();
}

// StopDepIds
// ==> set of AS`Name * set of AS`Name
Tuple StackCompiler::StopDepIds()
{
  return mk_(this->nms, this->histnms);
}
#endif // VICE

// }}}
