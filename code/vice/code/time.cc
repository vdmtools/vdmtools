// {{{ Header
/***
*  * WHAT
*  *    Implementation of module TIME (VICE)
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/vice/code/time.cc,v $
*  * VERSION
*  *    $Revision: 1.10 $
*  * DATE
*  *    $Date: 2005/10/25 05:00:26 $
*  * PROJECT
*  *    VICE
*  * STATUS
*  *    Under development.
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
*  * SPECIFICATION VERSION
*  *    implementation of time.vdm 1.6
*  *                      
***/

// }}}

// {{{ includes

#include "compile.h"
#include "stackeval.h"
#include "AS.h"
#include "asquotes.h"
#include "TIMEPARSER.h"
#include "TIMEMAP.h"
#include "libman.h"
#include "astaux.h"
#include "rt_errmsg.h"

// }}}

void StackCompiler::Init_TS()
{
  this->compilingTime = true;
}

// {{{ SetCompilingTime

void StackCompiler::SetCompilingTime(bool newCompilingTime)
{
  this->compilingTime = newCompilingTime;
}

// }}} SetCompilingTime


// {{{ GetCompilingTime

bool StackCompiler::GetCompilingTime()
{
  return this->compilingTime;
}

// }}} GetCompilingTime


// {{{ E2Time
TYPE_STKM_SubProgram StackCompiler::E2Time(const TYPE_AS_Expr& e)
{
  TYPE_STKM_SubProgram prog;
  int tag = e.GetTag();

  switch (tag) {
  case TAG_TYPE_AS_BracketedExpr: 
    {
      prog.ImpConc(E2Time(e.GetRecord(pos_AS_BracketedExpr_expr)));
      break;
    }
  case TAG_TYPE_AS_DefExpr: 
    {
      prog.ImpConc(TimeDefExpr(e));
      break;
    }
  case TAG_TYPE_AS_LetExpr: 
    {
      prog.ImpConc(TimeLetExpr(e));
      break;
    }
  case TAG_TYPE_AS_LetBeSTExpr: 
    {
      prog.ImpConc(TimeLetBeSTExpr());
      break;
    }
  case TAG_TYPE_AS_AllOrExistsExpr: 
    {
      prog.ImpConc(TimeAllOrExistsExpr(e));
      break;
    }
  case TAG_TYPE_AS_ExistsUniqueExpr: 
    {
      prog.ImpConc(TimeExistsUniqueExpr());
      break;
    }
  case TAG_TYPE_AS_IotaExpr: 
    {
      prog.ImpConc(TimeIotaExpr());
      break;
    }
  case TAG_TYPE_AS_ApplyExpr: 
    {
      prog.ImpConc(TimeApplyExpr());
      break;
    }
  case TAG_TYPE_AS_FieldSelectExpr: 
    {
      prog.ImpConc(TimeFieldSelectExpr());
      break;
    }
  case TAG_TYPE_AS_PrefixExpr: 
    {
      prog.ImpConc(TimePrefixExpr(e));
      break;
    }
  case TAG_TYPE_AS_BinaryExpr: 
    {
      prog.ImpConc(TimeBinaryExpr(e));
      break;
    }
  case TAG_TYPE_AS_SetEnumerationExpr: 
    {
      prog.ImpConc(TimeSetEnumExpr(e));
      break;
    }
  case TAG_TYPE_AS_SeqEnumerationExpr: 
    {
      prog.ImpConc(TimeSeqEnumExpr(e));
      break;
    }
  case TAG_TYPE_AS_MapEnumerationExpr: 
    {
      prog.ImpConc(TimeMapEnumExpr(e));
      break;
    }
  case TAG_TYPE_AS_SetComprehensionExpr: 
    {
      prog.ImpConc(TimeSetComprehensionExpr(e));
      break;
    }
  case TAG_TYPE_AS_SeqComprehensionExpr: 
    {
      prog.ImpConc(TimeSeqComprehensionExpr());
      break;
    }
  case TAG_TYPE_AS_MapComprehensionExpr: 
    {
      prog.ImpConc(TimeMapComprehensionExpr(e));
      break;
    }
  case TAG_TYPE_AS_TupleConstructorExpr: 
    {
      prog.ImpConc(TimeTupleConstructorExpr(e));
      break;
    }
  case TAG_TYPE_AS_RecordConstructorExpr: 
    {
      prog.ImpConc(TimeRecordConstructorExpr(e));
      break;
    }
  case TAG_TYPE_AS_RecordModifierExpr: 
    {
      prog.ImpConc(TimeRecordModifierExpr(e));
      break;
    }
  case TAG_TYPE_AS_LambdaExpr: 
    {
      prog.ImpConc(TimeLambdaExpr(e));
      break;
    }
  case TAG_TYPE_AS_FctTypeInstExpr: 
    {
      prog.ImpConc(TimeFctTypeInstExpr(e));
      break;
    }
  case TAG_TYPE_AS_IsExpr: 
    {
      prog.ImpConc(TimeIsExpr(e));
      break;
    }
  case TAG_TYPE_AS_NarrowExpr: 
    {
      prog.ImpConc(TimeNarrowExpr(e));
      break;
    }
  case TAG_TYPE_AS_TupleSelectExpr: 
    {
      prog.ImpConc(TimeTupleSelectExpr());
      break;
    }
  case TAG_TYPE_AS_TypeJudgementExpr: 
    {
      prog.ImpConc(TimeTypeJudgementExpr(e));
      break;
    }
  case TAG_TYPE_AS_Name: 
    {
      prog.ImpConc(TimeNameLookUp());
      break;
    }
  case TAG_TYPE_AS_OldName: 
    {
      prog.ImpConc(TimeNameLookUp());
      break;
    }
  case TAG_TYPE_AS_SelfExpr: 
    {
      prog.ImpConc(TimeSelfExpr());
      break;
    }
  case TAG_TYPE_AS_NewExpr: 
    {
      prog.ImpConc(TimeNewExpr(e));
      break;
    }
  case TAG_TYPE_AS_IsOfClassExpr: 
    {
      prog.ImpConc(TimeIsOfClassExpr());
      break;
    }
  case TAG_TYPE_AS_IsOfBaseClassExpr: 
    {
      prog.ImpConc(TimeIsOfBaseClassExpr());
      break;
    }
  case TAG_TYPE_AS_SameBaseClassExpr: 
    {
      prog.ImpConc(TimeSameBaseClassExpr());
      break;
    }
  case TAG_TYPE_AS_SameClassExpr: 
    {
      prog.ImpConc(TimeSameClassExpr());
      break;
    }
  case TAG_TYPE_AS_ThreadIdExpr: 
    {
      prog.ImpConc(TimeThreadIdExpr());
      break;
    }
  case TAG_TYPE_AS_ActExpr: 
    {
      prog.ImpConc(TimeHistoryExpr());
      break;
    }
  case TAG_TYPE_AS_FinExpr: 
    {
      prog.ImpConc(TimeHistoryExpr());
      break;
    }
  case TAG_TYPE_AS_ActiveExpr: 
    {
      prog.ImpConc(TimeHistoryExpr());
      break;
    }
  case TAG_TYPE_AS_WaitingExpr: 
    {
      prog.ImpConc(TimeHistoryExpr());
      break;
    }
  case TAG_TYPE_AS_ReqExpr: 
    {
      prog.ImpConc(TimeHistoryExpr());
      break;
    }
  default: 
    break;
  }
  return prog;
}
// }}}

// {{{ MkCbr
TYPE_STKM_SubProgram StackCompiler::MkCbr()
{
  TYPE_STKM_SubProgram prog;
  if (GetCompilingTime()) {
    Int oh (TimeLookup(TYPE_TIMEPARSER_Cbr(), Nil()));
    prog.ImpAppend(TYPE_INSTRTP_INCRTIME().Init(oh));
  }
  return prog;
}
// }}}

// {{{ MkBr
TYPE_STKM_SubProgram StackCompiler::MkBr()
{
  TYPE_STKM_SubProgram prog;
  if (GetCompilingTime()) {
    Int oh (TimeLookup(TYPE_TIMEPARSER_Br(), Nil()));
    prog.ImpAppend(TYPE_INSTRTP_INCRTIME().Init(oh));
  }
  return prog;
}
// }}}

// {{{ MkMatchPattern
TYPE_STKM_SubProgram StackCompiler::MkMatchPattern()
{
  TYPE_STKM_SubProgram prog;
  if (GetCompilingTime()) {
    Int oh (TimeLookup(TYPE_TIMEPARSER_MatchPattern(), Nil()));
    prog.ImpAppend(TYPE_INSTRTP_INCRTIME().Init(oh));
  }
  return prog;
}
// }}}

// {{{ MkSetCompInsert
TYPE_STKM_SubProgram StackCompiler::MkSetCompInsert()
{
  TYPE_STKM_SubProgram prog;
  if (GetCompilingTime()) {
    Int oh1 (TimeLookup(TYPE_TIMEPARSER_Addset(), Nil()));
    Int oh2 (TimeLookup(TYPE_TIMEPARSER_Cbr(), Nil()));
    prog.ImpAppend(TYPE_INSTRTP_INCRTIME().Init(oh1 + oh2));
  }
  return prog;
}
// }}}

// {{{ MkSeqCompInsert
TYPE_STKM_SubProgram StackCompiler::MkSeqCompInsert()
{
  TYPE_STKM_SubProgram prog;
  if (GetCompilingTime()) {
    Int oh1 (TimeLookup(TYPE_TIMEPARSER_Appendseq(), Nil()));
    Int oh2 (TimeLookup(TYPE_TIMEPARSER_Cbr(), Nil()));
    prog.ImpAppend(TYPE_INSTRTP_INCRTIME().Init(oh1 + oh2));
  }
  return prog;
}
// }}}

// {{{ MkMapCompInsert
TYPE_STKM_SubProgram StackCompiler::MkMapCompInsert()
{
  TYPE_STKM_SubProgram prog;
  if (GetCompilingTime()) {
    Int oh1 (TimeLookup(TYPE_TIMEPARSER_Appendmap(), Nil()));
    Int oh2 (TimeLookup(TYPE_TIMEPARSER_Cbr(), Nil()));
    prog.ImpAppend(TYPE_INSTRTP_INCRTIME().Init(oh1 + oh2));
  }
  return prog;
}
// }}}

// {{{ IsRuntimePrefixOp
bool StackCompiler::IsRuntimePrefixOp(const TYPE_AS_UnaryOp & op)
{
  int tag = ((Int) op).GetValue();
  switch(tag) {
    case SETDISTRUNION:
    case SETDISTRINTERSECT:
    case SETPOWER:
    case SEQDISTRCONC:
    case SEQELEMS:
    case SEQINDICES:
    case SEQTAIL:
    case SEQREVERSE:
    case MAPDOM:
    case MAPRNG:
    case MAPDISTRMERGE: 
    case MAPINVERSE: 
    {
      return true;
      break;
    }
    default: 
    {
      return false;
      break;
    }
  }
}
// }}}

// {{{ MkRuntimePrefixOp
TYPE_STKM_SubProgram StackCompiler::MkRuntimePrefixOp(const TYPE_AS_UnaryOp&opr)
{
  TYPE_STKM_SubProgram prog;
  if (GetCompilingTime()) {
    int tag = ((Int) opr).GetValue();
    Int oh (0);
    
    switch(tag) {
      case SETDISTRUNION: 
      case SETDISTRINTERSECT:
      case SEQELEMS:
      case SEQINDICES:
      case MAPDOM:
      case MAPRNG:
      case SETPOWER:
      {
        oh = TimeLookup(TYPE_TIMEPARSER_Addset(), Nil());
        break;
      }
      case SEQDISTRCONC:
      case SEQTAIL:
      case SEQREVERSE:
      {
        oh = TimeLookup(TYPE_TIMEPARSER_Appendseq(), Nil());
        break;
      }
      case MAPDISTRMERGE: 
      case MAPINVERSE: 
      {
        oh = TimeLookup(TYPE_TIMEPARSER_Appendmap(), Nil());
        break;
      }
      default: 
      {
        RTERR::Error(L"StackCompile::MkRuntimePrefixOp", RTERR_UNEXPECTED_PREFIX_OP, Nil(), Nil(), Sequence());
        break;
      }
    }
    TYPE_INSTRTP_RUNTIME_uINCRTIME_uPREF incpref;
    incpref.Init(opr,oh);
    prog.ImpAppend(incpref);
  }  
  return prog;
}
// }}}

// {{{ IsRuntimeBinaryOp  
bool StackCompiler::IsRuntimeBinaryOp(const TYPE_AS_BinaryOp& op)
{
  int tag = ((Int) op).GetValue();
  switch(tag) {
    case NUMEXP:
    case SETUNION:
    case SETINTERSECT:
    case SETMINUS:
    case SUBSET:
    case PROPERSUBSET:
    case INSET:
    case NOTINSET:
    case SEQCONC:
    case MAPMERGE:
    case MAPDOMRESTTO:
    case MAPDOMRESTBY:
    case MAPRNGRESTTO:
    case MAPRNGRESTBY:
    case COMPOSE: 
    {
      return true;
      break;
    }
    default: 
    {
      return false;
      break;
    }
  }
}
// }}}

// {{{ MkRuntimeBinaryOp
TYPE_STKM_SubProgram StackCompiler::MkRuntimeBinaryOp(const TYPE_AS_BinaryOp& opr)
{
  TYPE_STKM_SubProgram prog;
  if (GetCompilingTime()) {
    int tag = ((Int) opr).GetValue();
    Int oh1 (0);
    Generic oh2 = Nil();
    
    switch(tag){
      case NUMEXP:
      {
        oh1 = TimeLookup(TYPE_TIMEPARSER_Mult(), Nil());
        break;
      }
      case SETUNION:
      case SETINTERSECT:
      case SETMINUS:
      {
        oh1 = TimeLookup(TYPE_TIMEPARSER_Addset(), Nil());
        break;
      }
      case SUBSET:
      case PROPERSUBSET:
      case INSET:
      case NOTINSET:
      case SEQCONC:
      {
        oh1 = TimeLookup(TYPE_TIMEPARSER_Appendseq(), Nil());
        break;
      }
      case MAPMERGE:
      {
        oh1 = TimeLookup(TYPE_TIMEPARSER_Appendmap(), Nil());
        oh2 = TimeLookup(TYPE_TIMEPARSER_Equal(), Nil());
        break;
      }
      case MAPDOMRESTTO:
      {
        oh1 = TimeLookup(TYPE_TIMEPARSER_Appendmap(), Nil());
        break;
      }
      case MAPDOMRESTBY:
      case MAPRNGRESTTO:
      case MAPRNGRESTBY:
      {
        oh1 = TimeLookup(TYPE_TIMEPARSER_Appendmap(), Nil());
        oh2 = TimeLookup(TYPE_TIMEPARSER_Equal(), Nil());
        break;
      }
      case COMPOSE: 
      {
        oh1 = TimeLookup(TYPE_TIMEPARSER_Addset(), Nil());
        break;
      }
      default: 
      {
        RTERR::Error(L"StackCompile::MkRuntimeBinaryOp", RTERR_UNEXPECTED_BINARY_OP, Nil(), Nil(), Sequence());
        break;
      }
    }
    TYPE_INSTRTP_RUNTIME_uINCRTIME_uBIN incbin;
    incbin.Init(opr,oh1,oh2);
    prog.ImpAppend(incbin);
  }  
  return prog;
}
// }}}

// {{{ MkRuntimeSetSeqMap
TYPE_STKM_SubProgram StackCompiler::MkRuntimeSetSeqMap(const Record & e)
{
  TYPE_STKM_SubProgram prog;
  if (GetCompilingTime()) {
    int tag = e.GetTag();
    Int oh (0);
    
    switch(tag) {
      case TAG_TYPE_AS_SetRangeExpr: 
      {
        oh = TimeLookup(TYPE_TIMEPARSER_Addset(), Nil());
        break;
      }
      case TAG_TYPE_AS_SubSequenceExpr:
      {
        oh = TimeLookup(TYPE_TIMEPARSER_Subseq(), Nil());
        break;
      }
      case TAG_TYPE_AS_SeqModifyMapOverrideExpr:
      {
        oh = TimeLookup(TYPE_TIMEPARSER_Seqmapover(), Nil());
        break;
      }
      default: 
      {
        RTERR::Error(L"StackCompile::MkRuntimeSetSeqMap", RTERR_UNEXPECTED_EXPRESSION, Nil(), Nil(), Sequence());
        break;
      }
    }
  
    TYPE_INSTRTP_RUNTIME_uINCRTIME_uSETSEQMAP incssm;
    incssm.Init(oh);
    prog.ImpAppend(incssm);
  }  
  return prog;
}
// }}}

// {{{ MkLoopBind
TYPE_STKM_SubProgram StackCompiler::MkLoopBind(int n)
{
  TYPE_STKM_SubProgram prog;
  Int oh (TimeLookup(TYPE_TIMEPARSER_AddEnv(), Nil()));
  TYPE_INSTRTP_INCRTIME inct;
  inct.Init(oh * Int(n));
  prog.ImpAppend(inct);
  return prog;
}

// {{{ MkRuntimeStartList
TYPE_STKM_SubProgram StackCompiler::MkRuntimeStartList()
{
  TYPE_STKM_SubProgram prog;
  Int oh (TimeLookup(TYPE_TIMEPARSER_Start(), Nil()));
  TYPE_INSTRTP_RUNTIME_uINCRTIME_uSTARTLIST incssm;
  incssm.Init(oh);
  prog.ImpAppend(incssm);
  return prog;
}

// {{{ MkRuntimeStopList
TYPE_STKM_SubProgram StackCompiler::MkRuntimeStopList()
{
  TYPE_STKM_SubProgram prog;
  Int oh (TimeLookup(TYPE_TIMEPARSER_Stop(), Nil()));
  TYPE_INSTRTP_RUNTIME_uINCRTIME_uSTOPLIST incssm;
  incssm.Init(oh);
  prog.ImpAppend(incssm);
  return prog;
}

// {{{ S2Time
TYPE_STKM_SubProgram StackCompiler::S2Time(const TYPE_AS_Stmt& stmt)
{
  TYPE_STKM_SubProgram prog;
  int tag = stmt.GetTag();
  switch (tag) {
    case TAG_TYPE_AS_DefStmt: 
    {
      prog.ImpConc(TimeDefStmt(stmt));
      break;
    }
    case TAG_TYPE_AS_LetStmt: 
    {
      prog.ImpConc(TimeLetStmt(stmt));
      break;
    }
    case TAG_TYPE_AS_LetBeSTStmt: 
    {
      prog.ImpConc(TimeLetBeSTStmt());
      break;
    }
    case TAG_TYPE_AS_AssignStmt: 
    {
      prog.ImpConc(TimeAssignStmt(stmt));
      break;
    }
    case TAG_TYPE_AS_AtomicAssignStmt: 
    {
      prog.ImpConc(TimeAtomicAssignStmt(stmt));
      break;
    }
    case TAG_TYPE_AS_SeqForLoopStmt: 
    case TAG_TYPE_AS_SetForLoopStmt: 
    case TAG_TYPE_AS_IndexForLoopStmt: 
    case TAG_TYPE_AS_WhileLoopStmt: 
    {
      prog.ImpConc(TimeLoopStmt());
      break;
    }
    case TAG_TYPE_AS_CallStmt: 
    {
      prog.ImpConc(TimeCallStmt());
      break;
    }
    case TAG_TYPE_AS_ReturnStmt: 
    {
      prog.ImpConc(TimeReturnStmt());
      break;
    }
    case TAG_TYPE_AS_ExitStmt: 
    {
      prog.ImpConc(TimeExitStmt());
      break;
    }
    case TAG_TYPE_AS_AlwaysStmt: 
    {
      prog.ImpConc(TimeAlwaysStmt());
      break;
    }
    case TAG_TYPE_AS_TrapStmt: 
    {
      prog.ImpConc(TimeTrapStmt());
      break;
    }
    case TAG_TYPE_AS_RecTrapStmt: 
    {
      prog.ImpConc(TimeRecTrapStmt());
      break;
    }
    case TAG_TYPE_AS_StartStmt: 
    {
      prog.ImpConc(TimeStartStmt());
      break;
    }
    case TAG_TYPE_AS_StopStmt: 
    {
      prog.ImpConc(TimeStopStmt());
      break;
    }
    case TAG_TYPE_AS_BlockStmt: 
    {
      prog.ImpConc(TimeBlockStmt(stmt));
      break;
    }
    case TAG_TYPE_AS_NonDetStmt: 
    {
      prog.ImpConc(TimeNonDetStmt());
      break;
    }
  }
  return prog;
}
// From here onwards are internal functions

// {{{ TimeDefExpr
TYPE_STKM_SubProgram StackCompiler::TimeDefExpr(const TYPE_AS_DefExpr& e)
{
  Int oh (TimeLookup(TYPE_TIMEPARSER_AddEnv(), Nil()));
  TYPE_INSTRTP_INCRTIME inct;
  inct.Init(oh * Int(e.get_Def().Length()));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(inct);
  return prog;
}
// }}}

// {{{ TimeLetExpr  
TYPE_STKM_SubProgram StackCompiler::TimeLetExpr(const TYPE_AS_LetExpr& e)
{
  Int oh (TimeLookup(TYPE_TIMEPARSER_AddEnv(), Nil()));
  TYPE_INSTRTP_INCRTIME inct;
  inct.Init(oh * Int(e.get_localdef().Length()));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(inct);
  return prog;
}
// }}}

// {{{ TimeLetBeSTExpr
TYPE_STKM_SubProgram StackCompiler::TimeLetBeSTExpr()
{
  Int oh (TimeLookup(TYPE_TIMEPARSER_LetBeST(), Nil()));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(TYPE_INSTRTP_INCRTIME().Init(oh));
  return prog;
}
// }}}

// {{{ TimeAllOrExistsExpr
TYPE_STKM_SubProgram StackCompiler::TimeAllOrExistsExpr( 
                         const TYPE_AS_AllOrExistsExpr& e)
{
  int numbinds = 0;
  Generic g;
  Sequence bind = e.get_bind();
  for (bool bb = bind.First(g); bb; bb = bind.Next(g))
  {
    Record mult_bind(g); // MultBind mult_bind(g);
    numbinds += ((Sequence)(mult_bind.GetField(1))).Length();
  }
  Int oh1 (TimeLookup(TYPE_TIMEPARSER_BindList(), Nil()));
  Int oh2 (TimeLookup(TYPE_TIMEPARSER_AllorExists(), Nil()));
  TYPE_INSTRTP_INCRTIME inct;
  inct.Init((oh1 * Int(numbinds)) + oh2);

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(inct);
  return prog;
}
// }}}

// {{{ TimeExistsUniqueExpr
TYPE_STKM_SubProgram StackCompiler::TimeExistsUniqueExpr()
{
  Int oh1 (TimeLookup(TYPE_TIMEPARSER_AddEnv(), Nil()));
  Int oh2 (TimeLookup(TYPE_TIMEPARSER_AllorExists(), Nil()));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(TYPE_INSTRTP_INCRTIME().Init(oh1 + oh2));
  return prog;
}
// }}}

// {{{ TimeIotaExpr
TYPE_STKM_SubProgram StackCompiler::TimeIotaExpr()
{
  Int oh1 (TimeLookup(TYPE_TIMEPARSER_AddEnv(), Nil()));
  Int oh2 (TimeLookup(TYPE_TIMEPARSER_Iota(), Nil()));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(TYPE_INSTRTP_INCRTIME().Init(oh1 + oh2));
  return prog;
}
// }}}

// {{{ TimeApplyExpr
TYPE_STKM_SubProgram StackCompiler::TimeApplyExpr()
{
  Int oh (TimeLookup(TYPE_TIMEPARSER_Apply(), Nil()));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(TYPE_INSTRTP_INCRTIME().Init(oh));
  return prog;
}
// }}}

// {{{ TimeFieldSelectExpr
TYPE_STKM_SubProgram StackCompiler::TimeFieldSelectExpr()
{
  Int oh (TimeLookup(TYPE_TIMEPARSER_Fieldsel(), Nil()));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(TYPE_INSTRTP_INCRTIME().Init(oh));
  return prog;
}
// }}}

// {{{ TimePrefixExpr
TYPE_STKM_SubProgram StackCompiler::TimePrefixExpr(const TYPE_AS_PrefixExpr&e)
{
  int tag = ((Int) (e.get_opr())).GetValue();
  Int time (0);
  switch(tag) {
  case NUMPLUS: 
    { 
      time = 0; 
      break;
    }
  case NUMMINUS: 
    { 
      time = TimeLookup(TYPE_TIMEPARSER_Minus(), GetType(e));
      break;
    }
  case NUMABS: 
    {
      time = TimeLookup(TYPE_TIMEPARSER_Abs(), GetType(e));
      break;
    }
  case FLOOR: 
    {
      time = TimeLookup(TYPE_TIMEPARSER_Floor(), GetType(e));
      break;
    }
  case NOT: 
    {
      time = TimeLookup(TYPE_TIMEPARSER_Not(), Nil());
      break;
    }
  case SETCARD: 
    {
      time = TimeLookup(TYPE_TIMEPARSER_Setcard(), Nil());
      break;
    }
  case SEQLEN: 
    {
      time = TimeLookup(TYPE_TIMEPARSER_Seqlen(), Nil());
      break;
    }
  case MAPINVERSE: 
    {
      time = TimeLookup(TYPE_TIMEPARSER_MapInverse(), Nil());
      break;
    }
  case SEQHEAD: 
    {
      time = 0;
      break;
    }
  default: 
    {
      time = 0;
      break;
    }
  }

  TYPE_STKM_SubProgram prog;
  if (time.GetValue() != 0){
    TYPE_INSTRTP_INCRTIME inct;
    inct.Init(time);
    prog.ImpAppend(inct);
  }
  return prog;
}
// }}}

Generic StackCompiler::GetType(const TYPE_AS_Expr& e)
{
  // This is based on code in the code generator 
  // (FindType in mod_cgaux). It is a crude and incomplete attempt
  // to find the type of e. It relies on the specification already
  // having been type checked.
  TYPE_CI_ContextId cid (ASTAUX::GetCid (e));
  if (cid.GetValue() != NilContextId) {
    Generic citp (GetCI().GetTypeInfo(cid)); 
    return citp;
  }
  else
    return Nil();
}
// }}}

// {{{ TimeBinaryExpr
TYPE_STKM_SubProgram StackCompiler::TimeBinaryExpr(const TYPE_AS_BinaryExpr&e)
{
  int tag = ((Int) (e.get_opr())).GetValue();
  Int time (0);
  switch(tag){
  case NUMPLUS: 
    {
      time = TimeLookup(TYPE_TIMEPARSER_Abs(), GetType(e));
      break;
    }
  case NUMMINUS: 
    {
      time = TimeLookup(TYPE_TIMEPARSER_Minus(), GetType(e));
      break;
    }
  case NUMMULT: 
    {
      time = TimeLookup(TYPE_TIMEPARSER_Mult(), GetType(e));
      break;
    }
  case NUMDIV: 
    {
      time = TimeLookup(TYPE_TIMEPARSER_Div(), GetType(e));
      break;
    }
  case NUMREM: 
    {
      time = TimeLookup(TYPE_TIMEPARSER_Rem(), GetType(e));
      break;
    }
  case NUMMOD: 
    {
      time = TimeLookup(TYPE_TIMEPARSER_Mod(), GetType(e));
      break;
    }
  case INTDIV: 
    {
      time = TimeLookup(TYPE_TIMEPARSER_Intdiv(), GetType(e));
      break;
    }
  case NUMLT: 
    {
      time = TimeLookup(TYPE_TIMEPARSER_GreaterThan(), GetType(e));
      break;
    }
  case NUMLE: 
    {
      time = TimeLookup(TYPE_TIMEPARSER_GreaterThanOrEqual(), GetType(e));
      break;
    }
  case NUMGT: 
    {
      time = TimeLookup(TYPE_TIMEPARSER_GreaterThan(), GetType(e));
      break;
    }
  case NUMGE: 
    {
      time = TimeLookup(TYPE_TIMEPARSER_GreaterThanOrEqual(), GetType(e));
      break;
    }
  case AND: 
    {
      time = TimeLookup(TYPE_TIMEPARSER_And(), Nil());
      break;
    }
  case OR: 
    {
      time = TimeLookup(TYPE_TIMEPARSER_Or(), Nil());
      break;
    }
  case IMPLY: {
    time = TimeLookup(TYPE_TIMEPARSER_Or(), Nil()) + 
           TimeLookup(TYPE_TIMEPARSER_Not(), Nil());
    break;
    }
  case EQUIV: {
    time = TimeLookup(TYPE_TIMEPARSER_Equal(), GetType(e));
    break;
    }
  case EQ: {
    time = TimeLookup(TYPE_TIMEPARSER_Equal(), GetType(e));
    break;
    }
  case NE: {
    time = TimeLookup(TYPE_TIMEPARSER_Equal(), GetType(e));
    break;
    }
  default: {
    time = 0;
    break;
    }
  }
  TYPE_STKM_SubProgram prog;
  if (time.GetValue() != 0){
    TYPE_INSTRTP_INCRTIME inct;
    inct.Init(time);
    prog.ImpAppend(inct);
  }
  return prog;
}
// }}}

// {{{ TimeSetEnumExpr
TYPE_STKM_SubProgram StackCompiler::TimeSetEnumExpr(
                                  const TYPE_AS_SetEnumerationExpr& expr)
{
  Int oh (TimeLookup(TYPE_TIMEPARSER_Addset(), Nil()));
  Sequence e (expr.get_els());
  TYPE_INSTRTP_INCRTIME inct;
  inct.Init(oh * Int(e.Length()));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(inct);
  return prog;
}
// }}}

// {{{ TimeSeqEnumExpr
TYPE_STKM_SubProgram StackCompiler::TimeSeqEnumExpr(
                                    const TYPE_AS_SeqEnumerationExpr& expr)
{
  Int oh (TimeLookup(TYPE_TIMEPARSER_Appendseq(), Nil()));
  Sequence e (expr.get_els());
  TYPE_INSTRTP_INCRTIME inct;
  inct.Init(oh * Int(e.Length()));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(inct);
  return prog;
}
// }}}

// {{{ TimeMapEnumExpr
TYPE_STKM_SubProgram StackCompiler::TimeMapEnumExpr(
                                    const TYPE_AS_MapEnumerationExpr& expr)
{
  Int oh (TimeLookup(TYPE_TIMEPARSER_Appendmap(), Nil()));
  Sequence e (expr.get_els());
  TYPE_INSTRTP_INCRTIME inct;
  inct.Init(oh * Int(e.Length()));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(inct);
  return prog;
}
// }}}

// {{{ TimeSetComprehensionExpr
TYPE_STKM_SubProgram StackCompiler::TimeSetComprehensionExpr(
                              const TYPE_AS_SetComprehensionExpr& e)
{
  int numbinds = 0;
  Generic g;
  Sequence bind = e.get_bind();
  for (int ii=bind.First(g); ii; ii=bind.Next(g))
  {
    Record mult_bind(g); // MultBind mult_bind(g);
    numbinds += ((Sequence)(mult_bind.GetField(1))).Length();
  }
  Int oh (TimeLookup(TYPE_TIMEPARSER_BindList(), Nil()));
  TYPE_INSTRTP_INCRTIME inct;
  inct.Init(oh * Int(numbinds));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(inct);
  return prog;
}
// }}}

// {{{ TimeSeqComprehensionExpr
TYPE_STKM_SubProgram StackCompiler::TimeSeqComprehensionExpr()
{
  Int oh (TimeLookup(TYPE_TIMEPARSER_BindList(), Nil()));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(TYPE_INSTRTP_INCRTIME().Init(oh));
  return prog;
}
// }}}

// {{{ TimeMapComprehensionExpr
TYPE_STKM_SubProgram StackCompiler::TimeMapComprehensionExpr(
                              const TYPE_AS_MapComprehensionExpr& e)
{
  int numbinds = 0;
  Generic g;
  Sequence bind (e.get_bind());
  for (bool ii=bind.First(g); ii; ii=bind.Next(g))
  {
    Record mult_bind(g); // MultBind mult_bind(g);
    numbinds += ((Sequence)(mult_bind.GetField(1))).Length();
  }
  Int oh (TimeLookup(TYPE_TIMEPARSER_BindList(), Nil()));
  TYPE_INSTRTP_INCRTIME inct;
  inct.Init(oh * Int(numbinds));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(inct);
  return prog;
}
// }}}

// {{{ TimeTupleConstructorExpr
TYPE_STKM_SubProgram StackCompiler::TimeTupleConstructorExpr(
                              const TYPE_AS_TupleConstructorExpr& e)
{
  Int oh (TimeLookup(TYPE_TIMEPARSER_Appendtup(), Nil()));
  Sequence fields (e.get_fields());
  TYPE_INSTRTP_INCRTIME inct;
  inct.Init(oh * Int(fields.Length()));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(inct);
  return prog;
}
// }}}

// {{{ TimeRecordConstructorExpr
TYPE_STKM_SubProgram StackCompiler::TimeRecordConstructorExpr(
                               const TYPE_AS_RecordConstructorExpr& e)
{
  Int oh (TimeLookup(TYPE_TIMEPARSER_Reccons(), Nil()));
  Sequence fields (e.get_fields());
  TYPE_INSTRTP_INCRTIME inct;
  inct.Init(oh * Int(fields.Length()));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(inct);
  return prog;
}
// }}}

// {{{ TimeRecordModifierExpr
TYPE_STKM_SubProgram StackCompiler::TimeRecordModifierExpr(
                            const TYPE_AS_RecordModifierExpr& e)
{
  Int oh (TimeLookup(TYPE_TIMEPARSER_Recmod(), Nil()));
  Sequence mods (e.get_modifiers());
  TYPE_INSTRTP_INCRTIME inct;
  inct.Init(oh * Int(mods.Length()));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(inct);
  return prog;
}
// }}}

// {{{ TimeLambdaExpr
TYPE_STKM_SubProgram StackCompiler::TimeLambdaExpr(const TYPE_AS_LambdaExpr& e)
{
  Int oh1 (TimeLookup(TYPE_TIMEPARSER_AddEnv(), Nil()));
  Int oh2 (TimeLookup(TYPE_TIMEPARSER_LambdaExpr(), Nil()));
  Sequence parm (e.get_parm());
  TYPE_INSTRTP_INCRTIME inct;
  inct.Init(oh1 * Int(parm.Length()) + oh2);

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(inct);
  return prog;
}
// }}}

// {{{ TimeFctTypeInstExpr
TYPE_STKM_SubProgram StackCompiler::TimeFctTypeInstExpr(
                                    const TYPE_AS_FctTypeInstExpr& e)
{
  Int oh (TimeLookup(TYPE_TIMEPARSER_Polyinst(), Nil()));
  Sequence inst (e.get_inst());
  TYPE_INSTRTP_INCRTIME inct;
  inct.Init(oh * Int(inst.Length()));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(inct);
  return prog;
}
// }}}

// {{{ TimeIsExpr
TYPE_STKM_SubProgram StackCompiler::TimeIsExpr(const TYPE_AS_IsExpr& e)
{
  Int oh (TimeLookup(TYPE_TIMEPARSER_Isexpr(), GetType(e.get_arg())));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(TYPE_INSTRTP_INCRTIME().Init(oh));
  return prog;
}
// }}}

// {{{ TimeNarrowExpr
TYPE_STKM_SubProgram StackCompiler::TimeNarrowExpr(const TYPE_AS_NarrowExpr& e)
{
  Int oh (TimeLookup(TYPE_TIMEPARSER_Narrowexpr(), GetType(e.get_expr())));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(TYPE_INSTRTP_INCRTIME().Init(oh));
  return prog;
}
// }}}

// {{{ TimeTupleSelectExpr
TYPE_STKM_SubProgram StackCompiler::TimeTupleSelectExpr()
{
  Int oh (TimeLookup(TYPE_TIMEPARSER_Tupsel(), Nil()));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(TYPE_INSTRTP_INCRTIME().Init(oh));
  return prog;
}
// }}}

// {{{ TimeTypeJudgementExpr
TYPE_STKM_SubProgram StackCompiler::TimeTypeJudgementExpr(
                           const TYPE_AS_TypeJudgementExpr& expr)
{
  Int oh (TimeLookup(TYPE_TIMEPARSER_Isexpr(), GetType(expr.get_expr())));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(TYPE_INSTRTP_INCRTIME().Init(oh));
  return prog;
}
// }}}

// {{{ TimeNameLookUp
TYPE_STKM_SubProgram StackCompiler::TimeNameLookUp()
{
  Int oh (TimeLookup(TYPE_TIMEPARSER_Lookup(), Nil()));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(TYPE_INSTRTP_INCRTIME().Init(oh));
  return prog;
}
// }}}

// {{{ TimeSelfExpr
TYPE_STKM_SubProgram StackCompiler::TimeSelfExpr()
{
  Int oh (TimeLookup(TYPE_TIMEPARSER_Selfexpr(), Nil()));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(TYPE_INSTRTP_INCRTIME().Init(oh));
  return prog;
}
// }}}

// {{{ TimeIsOfClassExpr
TYPE_STKM_SubProgram StackCompiler::TimeIsOfClassExpr()
{
  Int oh (TimeLookup(TYPE_TIMEPARSER_Isofclass(), Nil()));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(TYPE_INSTRTP_INCRTIME().Init(oh));
  return prog;
}
// }}}

// {{{ TimeIsOfBaseClassExpr
TYPE_STKM_SubProgram StackCompiler::TimeIsOfBaseClassExpr()
{
  Int oh (TimeLookup(TYPE_TIMEPARSER_Isofbaseclass(), Nil()));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(TYPE_INSTRTP_INCRTIME().Init(oh));
  return prog;
}
// }}}

// {{{ TimeSameBaseClassExpr
TYPE_STKM_SubProgram StackCompiler::TimeSameBaseClassExpr()
{
  Int oh (TimeLookup(TYPE_TIMEPARSER_Samebaseclass(), Nil()));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(TYPE_INSTRTP_INCRTIME().Init(oh));
  return prog;
}
// }}}

// {{{ TimeSameClassExpr
TYPE_STKM_SubProgram StackCompiler::TimeSameClassExpr()
{
  Int oh (TimeLookup(TYPE_TIMEPARSER_Sameclass(), Nil()));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(TYPE_INSTRTP_INCRTIME().Init(oh));
  return prog;
}
// }}}

// {{{ TimeThreadIdExpr
TYPE_STKM_SubProgram StackCompiler::TimeThreadIdExpr()
{
  Int oh (TimeLookup(TYPE_TIMEPARSER_Threadid(), Nil()));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(TYPE_INSTRTP_INCRTIME().Init(oh));
  return prog;
}
// }}}

// {{{ TimeHistoryExpr
TYPE_STKM_SubProgram StackCompiler::TimeHistoryExpr()
{
  Int oh (TimeLookup(TYPE_TIMEPARSER_History(), Nil()));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(TYPE_INSTRTP_INCRTIME().Init(oh));
  return prog;
}
// }}}

// {{{ TimeNewExpr
TYPE_STKM_SubProgram StackCompiler::TimeNewExpr(const TYPE_AS_NewExpr& e)
{
  Int oh (TimeLookup(TYPE_TIMEPARSER_Newobj(), Nil()));
  TYPE_INSTRTP_RUNTIME_uINCRTIME_uNEW incnewt;
  incnewt.Init(oh, e.get_cls());

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(incnewt);
  return prog;
}
// }}}

// {{{ TimeLookup
Int StackCompiler::TimeLookup(const TYPE_TIMEPARSER_Instruction& ins, const Generic &g)
{
  Map m (theCompiler().GetTM());
  Generic g2; 
  if (m.DomExists(mk_(ins,g), g2))
    return Int(g2);
  else
    return Int(m[mk_(ins,Nil())]);
}
// }}}

// {{{ TimeDefStmt
TYPE_STKM_SubProgram StackCompiler::TimeDefStmt(const TYPE_AS_DefStmt & stmt)
{
  Int oh (TimeLookup(TYPE_TIMEPARSER_AddEnv(), Nil()));
  TYPE_INSTRTP_INCRTIME inct;
  inct.Init(oh * Int(stmt.get_value().Length()));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(inct);
  return prog;
}
// }}}

// {{{ TimeLetStmt
TYPE_STKM_SubProgram StackCompiler::TimeLetStmt(const TYPE_AS_LetStmt & stmt)
{
  Int oh (TimeLookup(TYPE_TIMEPARSER_AddEnv(), Nil()));
  TYPE_INSTRTP_INCRTIME inct;
  inct.Init(oh * Int(stmt.get_localdef().Length()));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(inct);
  return prog;
}
// }}}

// {{{ TimeLetBeSTStmt
TYPE_STKM_SubProgram StackCompiler::TimeLetBeSTStmt()
{
  Int oh (TimeLookup(TYPE_TIMEPARSER_LetBeST(), Nil()));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(TYPE_INSTRTP_INCRTIME().Init(oh));
  return prog;
}
// }}}

// {{{ TimeAssignStmt
TYPE_STKM_SubProgram StackCompiler::TimeAssignStmt(
                                    const TYPE_AS_AssignStmt & stmt)
{
  Int oh1 (TimeLookup(TYPE_TIMEPARSER_AddEnv(), Nil()));
  Int oh2 (TimeLookup(TYPE_TIMEPARSER_Update(), Nil()));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(TYPE_INSTRTP_INCRTIME().Init(oh1 + oh2));
  return prog;
}
// }}}

// {{{ TimeAtomicAssignStmt
TYPE_STKM_SubProgram StackCompiler::TimeAtomicAssignStmt( 
                          const TYPE_AS_AtomicAssignStmt & stmt)
{
  Int oh1 (TimeLookup(TYPE_TIMEPARSER_AddEnv(), Nil()));
  Int oh2 (TimeLookup(TYPE_TIMEPARSER_Update(), Nil()));
  Int oh ((oh1 + oh2) * Int(stmt.get_atm().Length()));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(TYPE_INSTRTP_INCRTIME().Init(oh));
  return prog;
}
// }}}

// {{{ TimeLoopStmt
TYPE_STKM_SubProgram StackCompiler::TimeLoopStmt()
{
  Int oh (TimeLookup(TYPE_TIMEPARSER_Loop(), Nil()));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(TYPE_INSTRTP_INCRTIME().Init(oh));
  return prog;
}
// }}}

// {{{ TimeCallStmt
TYPE_STKM_SubProgram StackCompiler::TimeCallStmt()
{
  Int oh (TimeLookup(TYPE_TIMEPARSER_Call(), Nil()));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(TYPE_INSTRTP_INCRTIME().Init(oh));
  return prog;
}
// }}}

// {{{ TimeReturnStmt
TYPE_STKM_SubProgram StackCompiler::TimeReturnStmt()
{
  Int oh (TimeLookup(TYPE_TIMEPARSER_Return(), Nil()));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(TYPE_INSTRTP_INCRTIME().Init(oh));
  return prog;
}
// }}}

// {{{ TimeExitStmt
TYPE_STKM_SubProgram StackCompiler::TimeExitStmt()
{
  Int oh (TimeLookup(TYPE_TIMEPARSER_Br(), Nil()));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(TYPE_INSTRTP_INCRTIME().Init(oh));
  return prog;
}
// }}}

// {{{ TimeAlwaysStmt
TYPE_STKM_SubProgram StackCompiler::TimeAlwaysStmt()
{
  Int oh (TimeLookup(TYPE_TIMEPARSER_Always(), Nil()));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(TYPE_INSTRTP_INCRTIME().Init(oh));
  return prog;
}
// }}}

// {{{ TimeTrapStmt
TYPE_STKM_SubProgram StackCompiler::TimeTrapStmt()
{
  Int oh (TimeLookup(TYPE_TIMEPARSER_Trap(), Nil()));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(TYPE_INSTRTP_INCRTIME().Init(oh));
  return prog;
}
// }}}

// {{{ TimeRecTrapStmt
TYPE_STKM_SubProgram StackCompiler::TimeRecTrapStmt()
{
  Int oh (TimeLookup(TYPE_TIMEPARSER_RecTrap(), Nil()));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(TYPE_INSTRTP_INCRTIME().Init(oh));
  return prog;
}
// }}}

// {{{ TimeStartStmt
TYPE_STKM_SubProgram StackCompiler::TimeStartStmt()
{
  Int oh (TimeLookup(TYPE_TIMEPARSER_Start(), Nil()));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(TYPE_INSTRTP_INCRTIME().Init(oh));
  return prog;
}
// }}}

// {{{ TimeStopStmt
TYPE_STKM_SubProgram StackCompiler::TimeStopStmt()
{
  Int oh (TimeLookup(TYPE_TIMEPARSER_Stop(), Nil()));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(TYPE_INSTRTP_INCRTIME().Init(oh));
  return prog;
}
// }}}

// {{{ TimeBlockStmt
TYPE_STKM_SubProgram StackCompiler::TimeBlockStmt(
                                    const TYPE_AS_BlockStmt & stmt)
{
  Int oh1 (TimeLookup(TYPE_TIMEPARSER_AddEnv(), Nil()));
  Int oh2 (TimeLookup(TYPE_TIMEPARSER_Update(), Nil()));
  Int oh3 (TimeLookup(TYPE_TIMEPARSER_BlockStmt(), Nil()));
  Int oh ((oh1 + oh2) * Int(stmt.get_dcls().Length()) + oh3);

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(TYPE_INSTRTP_INCRTIME().Init(oh));
  return prog;
}
// }}}

// {{{ TimeNonDetStmt
TYPE_STKM_SubProgram StackCompiler::TimeNonDetStmt()
{
  Int oh (TimeLookup(TYPE_TIMEPARSER_NonDet(), Nil()));

  TYPE_STKM_SubProgram prog;
  prog.ImpAppend(TYPE_INSTRTP_INCRTIME().Init(oh));
  return prog;
}
// }}}
