// {{{ Header
/***
*  * WHAT
*  *    Functions for compiling VDM(SL|++) ASTs into stack machine
*  *    instructions.
*  * COPYRIGHT
*  *    (C) Kyushu University
*  * SPECIFICATION VERSION
*  *    implementation of compile.vdm
***/
// }}}

// {{{ Includes
#ifndef _COMPILE_H
#define _COMPILE_H

#include "metaiv.h"
#include "AS.h"
#include "CI.h"
#include "PAT.h"
#include <string>
#include <fstream>
#include "stackeval.h"
#include "main_tag.h"

#ifdef VICE
#include "TIMEMAP.h"
#endif //VICE

#define TAG_TYPE_DYNSEM_COMPILE_ModuleProgramTable (TAG_TYPE_DYNSEM_COMPILE +0)

// }}}
// {{{ ProgramTable

// {{{ Implementation description

// In the specification, this is specified in the following way:
//
//    types
//      ProgramTable = map AS`Name to ModuleProgramTable;
//
//      ModuleProgramTable :: tbl : seq of STKM`SubProgram
//                            old_id : STKM`SubProgramId;
//
// This is implemented as one class, and all references to
// ModuleProgramTable are converted to method calls in the class ProgramTable

// }}}
// {{{ Implementation

class ModuleProgramTable : public Record {
public:

  ModuleProgramTable () : Record(TAG_TYPE_DYNSEM_COMPILE_ModuleProgramTable, 2)
  { this->SetField(1, SEQ<TYPE_STKM_SubProgram>());
    this->SetField(2, TYPE_STKM_SubProgramId(0)); }

  ModuleProgramTable ( const SEQ<TYPE_STKM_SubProgram> & p1, const TYPE_STKM_SubProgramId & p2)
      : Record(TAG_TYPE_DYNSEM_COMPILE_ModuleProgramTable, 2)
  { this->SetField(1,p1); this->SetField(2,p2); }

  ModuleProgramTable(const Generic &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_DYNSEM_COMPILE_ModuleProgramTable";
  }

  SEQ<TYPE_STKM_SubProgram> get_tbl () const { return GetField(1); }
  void set_tbl (const SEQ<TYPE_STKM_SubProgram> &p) { SetField(1, p ); }
  TYPE_STKM_SubProgramId get_old_id () const { return GetField(2); }
  void set_old_id (const TYPE_STKM_SubProgramId &p) { SetField(2, p ); }
};

class ProgramTable{

private:
  // Instance vbariables
  MAP<TYPE_AS_Name, ModuleProgramTable> program_table;

public:
  // Constructors
  ProgramTable() : program_table(MAP<TYPE_AS_Name, ModuleProgramTable>()) {};
  ~ProgramTable() { this->program_table.Clear(); };

  // Methods
  void ResetProgramTable(const TYPE_AS_Name & modnm);
  TYPE_STKM_SubProgramId InsertProgram(const TYPE_AS_Name & modnm,
                                       const TYPE_STKM_SubProgram & instr);
  const TYPE_STKM_SubProgram& GetProgram(const TYPE_AS_Name& modnm,
                                         const TYPE_STKM_SubProgramId& id) const;
  void CopyProgram(const TYPE_AS_Name& oldnm, const TYPE_AS_Name& newnm);
  void Clear() { this->program_table.Clear(); };
  SEQ<TYPE_STKM_SubProgram> DumpProgram(const TYPE_AS_Name& modnm) const;
};
// }}}

// }}}
// {{{ StackCompiler

////////////////////////////////////////////////////////////////
//
// The StackCompiler class is the main interface to the stack
// compiler. It holds the global program table, and consequently
// only one instance of this class should exist.
//
////////////////////////////////////////////////////////////////

class StackCompiler
{
  // {{{ Instance variables

private:

  // CMPL module
  bool DebugInfo;
  TYPE_AS_Name curr_cl_mod;
  ProgramTable program_table;
#ifdef VICE
  TYPE_TIMEMAP_Timemap timem;
#endif //VICE

  // CEXPR module
#ifdef VICE
  Generic opnm;   // [AS`Name]
  Generic nms;    // <ALL> | set of AS`Name
  Set histnms;    // set of AS`Name
  Set opnms;      // set of AS`Name
#endif //VICE

  // CSTMT module
  //int trapno;
  Int trapno;

  // TIME module
#ifdef VICE
  bool compilingTime;
#endif //VICE

  // }}}

public:
  // Constructors
  StackCompiler();
  ~StackCompiler();

  SEQ<TYPE_STKM_SubProgram> DumpProgram(const TYPE_AS_Name & nm) const;

  //
  void GetEmptyCompEnv();
  void SetEmptyCompEnv();

  // {{{ Methods in compile.cc

  TYPE_STKM_SubProgramId CompileFnOpDef(const Record &);
  TYPE_STKM_SubProgramId CompilePrePostExpr(const TYPE_AS_Expr &);
  TYPE_STKM_SubProgramId CompileMeasureExpr(const TYPE_AS_Expr &);
  TYPE_STKM_SubProgram Mease2I(const TYPE_AS_FnDef &);
  bool MeasureIsId(const Generic & measu, const TYPE_AS_FnDef &);
  SET<TYPE_AS_Name> NamesInPatternList(const SEQ<TYPE_AS_Pattern> & pat_l);
  SET<TYPE_AS_Name> NamesInPattern(const TYPE_AS_Pattern & pat);
  TYPE_STKM_SubProgramId CompileLambdaBody(const TYPE_AS_Expr &);
  TYPE_STKM_SubProgram FnDef2I(const Generic &,
                               const Generic &,
                               const TYPE_AS_FnBody &,
                               const SEQ<TYPE_AS_NameType> &,
                               const TYPE_AS_Name &,
                               const Sequence &);
  TYPE_STKM_SubProgram PrePost2I(const Generic &, bool);
  TYPE_STKM_SubProgram ImplFnDef2I(const TYPE_CI_ContextId &);
  TYPE_STKM_SubProgram OpDef2I(const Generic &,
                               const Generic &,
                               const TYPE_AS_OpBody &,
                               const SEQ<TYPE_AS_NameType> &,
                               const TYPE_AS_Name &,
                               const Sequence &,
                               const Bool &,
                               const Bool &
                               );
  TYPE_STKM_SubProgram ImplOpDef2I(const TYPE_CI_ContextId &);
  TYPE_STKM_SubProgram IStart(const SEQ<Char> & name, const TYPE_CI_ContextId & cid) const;
  TYPE_STKM_SubProgram IEnd(const SEQ<Char> & name) const;
  TYPE_STKM_SubProgram SetContext(const TYPE_CI_ContextId &, bool);
  void SetDebugInfo(bool);
  void SetClMod(const TYPE_AS_Name &);
  const TYPE_AS_Name& GetClMod() const;

  void ResetProgramTable(const TYPE_AS_Name &);
  TYPE_STKM_SubProgramId InsertProgram(const TYPE_AS_Name &, const TYPE_STKM_SubProgram &);
  const TYPE_STKM_SubProgram& GetProgram(const TYPE_AS_Name & nm, const TYPE_STKM_SubProgramId & id) const;
  void CopyProgram(const TYPE_AS_Name&, const TYPE_AS_Name &);

  TYPE_STKM_SubProgram CompileRunTime(const TYPE_RTERR_ERR &, const TYPE_CI_ContextId &);

#ifdef VICE
  void SetTM(const TYPE_TIMEMAP_Timemap & newtm);
  TYPE_TIMEMAP_Timemap GetTM() const;
#endif //VICE

  // }}}
  // {{{ Methods in cexpr.cc
public:
  void Init_CEXPR();
#ifdef VICE
  void init_Depend();
#endif //VICE
  TYPE_STKM_SubProgram E2I(const TYPE_AS_Expr &);

private:
  void init_cexpr_lit();
  TYPE_STKM_SubProgram CompileDefExpr(const TYPE_AS_DefExpr &);
  TYPE_STKM_SubProgram CompileLetExpr(const TYPE_AS_LetExpr &);
  TYPE_STKM_SubProgram CompileLetBeSTExpr(const TYPE_AS_LetBeSTExpr &);
  TYPE_STKM_SubProgram CompileAllOrExistsExpr(const TYPE_AS_AllOrExistsExpr &);
  TYPE_STKM_SubProgram CompileEUandICommon(const TYPE_AS_Bind &, const TYPE_AS_Expr &);
  TYPE_STKM_SubProgram CompileExistsUniqueExpr(const TYPE_AS_ExistsUniqueExpr &);
  TYPE_STKM_SubProgram CompileIotaExpr(const TYPE_AS_IotaExpr &);
  TYPE_STKM_SubProgram CompileApplyExpr(const TYPE_AS_ApplyExpr &);
  TYPE_STKM_SubProgram CompileFieldSelectExpr(const TYPE_AS_FieldSelectExpr &);
  TYPE_STKM_SubProgram CompileIfExpr(const TYPE_AS_IfExpr &);
  TYPE_STKM_SubProgram CompileCasesExpr(const TYPE_AS_CasesExpr &);
  TYPE_STKM_SubProgram CompileUnaryExpr(const TYPE_AS_PrefixExpr &);
  TYPE_STKM_SubProgram CompileBinaryExpr(const TYPE_AS_BinaryExpr &);
  TYPE_STKM_SubProgram CompileLogBinaryExpr(const TYPE_AS_Expr &, const Int &, const TYPE_AS_Expr &);
  TYPE_STKM_SubProgram CompileOrdinaryBinaryExpr(const TYPE_AS_Expr &,
                                                 const TYPE_AS_BinaryOp &,
                                                 const TYPE_AS_Expr &,
                                                 const TYPE_CI_ContextId &);

  TYPE_STKM_SubProgram CompileSetRangeExpr(const TYPE_AS_SetRangeExpr &);
  TYPE_STKM_SubProgram CompileSubSequenceExpr(const TYPE_AS_SubSequenceExpr &);
  TYPE_STKM_SubProgram CompileSetEnumExpr(const TYPE_AS_SetEnumerationExpr &);
  TYPE_STKM_SubProgram CompileSeqEnumExpr(const TYPE_AS_SeqEnumerationExpr &);
  TYPE_STKM_SubProgram CompileMapEnumExpr(const TYPE_AS_MapEnumerationExpr &);
  TYPE_STKM_SubProgram CompileSetComprehensionExpr(const TYPE_AS_SetComprehensionExpr &);
  TYPE_STKM_SubProgram CompileSeqComprehensionExpr(const TYPE_AS_SeqComprehensionExpr &);
  TYPE_STKM_SubProgram CompileMapComprehensionExpr(const TYPE_AS_MapComprehensionExpr &);
  TYPE_STKM_SubProgram CompileTupleConstructorExpr(const TYPE_AS_TupleConstructorExpr &);
  TYPE_STKM_SubProgram CompileRecordConstructorExpr(const TYPE_AS_RecordConstructorExpr &);
  TYPE_STKM_SubProgram CompileRecordModifierExpr(const TYPE_AS_RecordModifierExpr &);
  TYPE_STKM_SubProgram CompileSeqModifyMapOverrideExpr(const TYPE_AS_SeqModifyMapOverrideExpr &);
  TYPE_STKM_SubProgram CompileTupleSelectExpr(const TYPE_AS_TupleSelectExpr & eIn);
  TYPE_STKM_SubProgram CompileTypeJudgementExpr(const TYPE_AS_TypeJudgementExpr & eIn);
  TYPE_STKM_SubProgram CompilePreConditionApplyExpr(const TYPE_AS_PreConditionApplyExpr & eIn);
  TYPE_STKM_SubProgram CompileLambdaExpr(const TYPE_AS_LambdaExpr &);
  TYPE_STKM_SubProgram CompileFctTypeInstExpr(const TYPE_AS_FctTypeInstExpr &);
  TYPE_STKM_SubProgram CompileIsExpr(const TYPE_AS_IsExpr &);
  TYPE_STKM_SubProgram CompileNarrowExpr(const TYPE_AS_NarrowExpr &);
  TYPE_STKM_SubProgram CompileTokenConstructorExpr(const TYPE_AS_TokenConstructorExpr &);
  TYPE_STKM_SubProgram CompileName(const TYPE_AS_Name & sIn);

public:
  TYPE_STKM_SubProgram ConcIfThenElse(const TYPE_STKM_SubProgram &,
                                      const TYPE_STKM_SubProgram &,
                                      const TYPE_STKM_SubProgram &) const;
private:
  TYPE_STKM_SubProgram CombRepeatUntil(const TYPE_STKM_SubProgram &) const;
  TYPE_STKM_SubProgram CombWhileLoop(const TYPE_STKM_SubProgram &) const;
  TYPE_STKM_SubProgram CombWhileLoopWithCond(const TYPE_STKM_SubProgram &, const TYPE_STKM_SubProgram &) const;

#ifdef VDMPP
  TYPE_STKM_SubProgram CompileSelfExpr(const TYPE_AS_SelfExpr &);
public:
  TYPE_STKM_SubProgram CompileNewExpr(const TYPE_AS_NewExpr &, const Generic & dlobject);
private:
  TYPE_STKM_SubProgram CompileIsOfClassExpr(const TYPE_AS_IsOfClassExpr &);
  TYPE_STKM_SubProgram CompileIsOfBaseClassExpr(const TYPE_AS_IsOfBaseClassExpr &);
  TYPE_STKM_SubProgram CompileSameBaseClassExpr(const TYPE_AS_SameBaseClassExpr &);
  TYPE_STKM_SubProgram CompileSameClassExpr(const TYPE_AS_SameClassExpr &);
  TYPE_STKM_SubProgram CompileThreadIdExpr(const TYPE_AS_ThreadIdExpr & eIn);
  TYPE_STKM_SubProgram CompileHistoryExpr(const SEQ<TYPE_AS_Name> & mthds, const TYPE_INSTRTP_HistoryKind & tp);
#ifdef VICE
public:
  void AddDepIds(const TYPE_AS_Name &, const SET<TYPE_AS_Name> & );
  Tuple StopDepIds();
#endif // VICE
#endif //VDMPP

  // }}}
  // {{{ Methods in cstmt.cc
public:
  void Init_CSTMT();
  void init_TRAP();
  TYPE_STKM_SubProgram S2I(const TYPE_AS_Stmt & stmt);
private:
  void init_cstmt_lit();
  TYPE_STKM_SubProgram CompileDefStmt(const TYPE_AS_DefStmt & sIn);
  TYPE_STKM_SubProgram CompileLetStmt(const TYPE_AS_LetStmt & sIn);
  TYPE_STKM_SubProgram CompileLetBeSTStmt(const TYPE_AS_LetBeSTStmt & sIn);
  TYPE_STKM_SubProgram CompileAssignStmt(const TYPE_AS_AssignStmt & sIn);
  TYPE_STKM_SubProgram CompileSeqForLoopStmt(const TYPE_AS_SeqForLoopStmt & sIn);
  TYPE_STKM_SubProgram CompileSetForLoopStmt(const TYPE_AS_SetForLoopStmt & sIn);
  TYPE_STKM_SubProgram CompileIndexForLoopStmt(const TYPE_AS_IndexForLoopStmt & sIn);
  TYPE_STKM_SubProgram CompileWhileLoopStmt(const TYPE_AS_WhileLoopStmt & sIn);
  TYPE_STKM_SubProgram CompileCallStmt(const TYPE_AS_CallStmt & sIn);
  TYPE_STKM_SubProgram CompileReturnStmt(const TYPE_AS_ReturnStmt & sIn);
  TYPE_STKM_SubProgram CompileIfStmt(const TYPE_AS_IfStmt & sIn);
  TYPE_STKM_SubProgram CompileCasesStmt(const TYPE_AS_CasesStmt & sIn);
  TYPE_STKM_SubProgram CompileErrorStmt(const TYPE_AS_ErrorStmt & sIn) const;
  TYPE_STKM_SubProgram CompileExitStmt(const TYPE_AS_ExitStmt & sIn);
  TYPE_STKM_SubProgram CompileAlwaysStmt(const TYPE_AS_AlwaysStmt & sIn);
  TYPE_STKM_SubProgram CompileTrapStmt(const TYPE_AS_TrapStmt & sIn);
  TYPE_STKM_SubProgram CompileRecTrapStmt(const TYPE_AS_RecTrapStmt & sIn);
  TYPE_STKM_SubProgram CompileAtomicAssignStmt(const TYPE_AS_AtomicAssignStmt & sIn);
  TYPE_STKM_SubProgram CompileBlockStmt(const TYPE_AS_BlockStmt & sIn);
  TYPE_STKM_SubProgram CompileNonDetStmt(const TYPE_AS_NonDetStmt & sIn);
  TYPE_STKM_SubProgram CompileAssertStmt(const TYPE_AS_AssertStmt & sIn);
  SEQ<Int> RelJumpLengths(int length, const SEQ<TYPE_STKM_SubProgram> & sp_l);
public:
  Int GetNewTrapNo();
  TYPE_STKM_SubProgram AddTrap(const TYPE_STKM_SubProgram & b_instr, const TYPE_STKM_SubProgram & handler);
private:
#ifdef VDMPP
  TYPE_STKM_SubProgram CompileStartStmt(const TYPE_AS_StartStmt & sIn);
  TYPE_STKM_SubProgram CompileStartListStmt(const TYPE_AS_StartListStmt & sIn);
  TYPE_STKM_SubProgram CompileStopStmt(const TYPE_AS_StopStmt & sIn);
  TYPE_STKM_SubProgram CompileStopListStmt(const TYPE_AS_StopListStmt & sIn);
#ifdef VICE
  TYPE_STKM_SubProgram CompileDurationStmt(const TYPE_AS_DurationStmt & sIn);
  TYPE_STKM_SubProgram CompileCycleStmt(const TYPE_AS_CycleStmt & sIn);
#endif // VICE
#endif // VDMPP

  // }}}
  // {{{ Methods in cpat.cc
private:
  TYPE_STKM_SubProgram CompileMultBindL(const SEQ<TYPE_AS_MultBind> &, const TYPE_PAT_PARTITION &);
  TYPE_STKM_SubProgram PB2I(const TYPE_AS_PatternBind &);
  TYPE_STKM_SubProgram P2I(const TYPE_AS_Pattern & pat);
  TYPE_STKM_SubProgram PStack2I(const TYPE_AS_Pattern & pat);
  TYPE_STKM_SubProgram PDirect2I(const TYPE_AS_Pattern & pat);
public:
  SEQ<TYPE_STKM_Pattern> PL2PL(const SEQ<TYPE_AS_Pattern> & pl);
  TYPE_STKM_Pattern P2P(const TYPE_AS_Pattern & pat);

  static TYPE_AS_Expr P2E (const TYPE_AS_Pattern & pat);
private:
  static bool AnyMatchVals(const TYPE_AS_Pattern & pat);
  static bool IsLiteral(const TYPE_AS_Expr & expr);
  TYPE_STKM_SubProgram SD2I(const TYPE_AS_StateDesignator & sd);
  TYPE_STKM_SubProgram SDStack2I(const TYPE_AS_StateDesignator & sd);
  static TYPE_STKM_SubProgram SDDirect2I(const TYPE_AS_StateDesignator & sd);
  static TYPE_STKM_StateDesignator SD2SD(const TYPE_AS_StateDesignator & sd);
  static bool AnyExprs(const TYPE_AS_StateDesignator & sd);

#ifdef VICE
  // {{{ Methods in time.cc
public:
  void Init_TS();
  void SetCompilingTime(bool newCompilingTime);
  bool GetCompilingTime();
  TYPE_STKM_SubProgram E2Time(const TYPE_AS_Expr &);
private:
  TYPE_STKM_SubProgram TimeDefExpr(const TYPE_AS_DefExpr &);
  TYPE_STKM_SubProgram TimeLetExpr(const TYPE_AS_LetExpr &);
  TYPE_STKM_SubProgram TimeLetBeSTExpr();
  TYPE_STKM_SubProgram TimeAllOrExistsExpr(const TYPE_AS_AllOrExistsExpr &);
  TYPE_STKM_SubProgram TimeExistsUniqueExpr();
  TYPE_STKM_SubProgram TimeIotaExpr();
  TYPE_STKM_SubProgram TimeApplyExpr();
  TYPE_STKM_SubProgram TimeFieldSelectExpr();
  TYPE_STKM_SubProgram TimePrefixExpr(const TYPE_AS_PrefixExpr &);
  TYPE_STKM_SubProgram TimeBinaryExpr(const TYPE_AS_BinaryExpr &);
  TYPE_STKM_SubProgram TimeSetEnumExpr(const TYPE_AS_SetEnumerationExpr &);
  TYPE_STKM_SubProgram TimeSeqEnumExpr(const TYPE_AS_SeqEnumerationExpr &);
  TYPE_STKM_SubProgram TimeMapEnumExpr(const TYPE_AS_MapEnumerationExpr &);
  TYPE_STKM_SubProgram TimeSetComprehensionExpr(const TYPE_AS_SetComprehensionExpr &);
  TYPE_STKM_SubProgram TimeSeqComprehensionExpr();
  TYPE_STKM_SubProgram TimeMapComprehensionExpr(const TYPE_AS_MapComprehensionExpr &);
  TYPE_STKM_SubProgram TimeTupleConstructorExpr(const TYPE_AS_TupleConstructorExpr &);
  TYPE_STKM_SubProgram TimeRecordConstructorExpr(const TYPE_AS_RecordConstructorExpr &);
  TYPE_STKM_SubProgram TimeRecordModifierExpr(const TYPE_AS_RecordModifierExpr &);
  TYPE_STKM_SubProgram TimeLambdaExpr(const TYPE_AS_LambdaExpr &);
  TYPE_STKM_SubProgram TimeFctTypeInstExpr(const TYPE_AS_FctTypeInstExpr &);
  TYPE_STKM_SubProgram TimeIsExpr(const TYPE_AS_IsExpr &);
  TYPE_STKM_SubProgram TimeNarrowExpr(const TYPE_AS_NarrowExpr &);
  TYPE_STKM_SubProgram TimeTupleSelectExpr();
  TYPE_STKM_SubProgram TimeTypeJudgementExpr(const TYPE_AS_TypeJudgementExpr &);
  TYPE_STKM_SubProgram TimeNameLookUp();
  TYPE_STKM_SubProgram TimeSelfExpr();
  TYPE_STKM_SubProgram TimeNewExpr(const TYPE_AS_NewExpr &);
  TYPE_STKM_SubProgram TimeIsOfClassExpr();
  TYPE_STKM_SubProgram TimeIsOfBaseClassExpr();
  TYPE_STKM_SubProgram TimeSameBaseClassExpr();
  TYPE_STKM_SubProgram TimeSameClassExpr();
  TYPE_STKM_SubProgram TimeThreadIdExpr();
  TYPE_STKM_SubProgram TimeHistoryExpr();

  Int TimeLookup(const TYPE_TIMEPARSER_Instruction &, const Generic &);
  Generic GetType(const TYPE_AS_Expr & e);

  TYPE_STKM_SubProgram TimeDefStmt(const TYPE_AS_DefStmt & stmt);
  TYPE_STKM_SubProgram TimeLetStmt(const TYPE_AS_LetStmt & stmt);
  TYPE_STKM_SubProgram TimeLetBeSTStmt();
  TYPE_STKM_SubProgram TimeAssignStmt(const TYPE_AS_AssignStmt & stmt);
  TYPE_STKM_SubProgram TimeAtomicAssignStmt(const TYPE_AS_AtomicAssignStmt & stmt);
  TYPE_STKM_SubProgram TimeLoopStmt();
  TYPE_STKM_SubProgram TimeCallStmt();
  TYPE_STKM_SubProgram TimeReturnStmt();
  TYPE_STKM_SubProgram TimeExitStmt();
  TYPE_STKM_SubProgram TimeAlwaysStmt();
  TYPE_STKM_SubProgram TimeTrapStmt();
  TYPE_STKM_SubProgram TimeRecTrapStmt();
  TYPE_STKM_SubProgram TimeStartStmt();
  TYPE_STKM_SubProgram TimeStopStmt();
  TYPE_STKM_SubProgram TimeBlockStmt(const TYPE_AS_BlockStmt & stmt);
  TYPE_STKM_SubProgram TimeNonDetStmt();

  bool IsRuntimeBinaryOp(const TYPE_AS_BinaryOp &);
  bool IsRuntimePrefixOp(const TYPE_AS_UnaryOp &);
  TYPE_STKM_SubProgram MkBr();
  TYPE_STKM_SubProgram MkCbr();
  TYPE_STKM_SubProgram MkLoopBind(int);
  TYPE_STKM_SubProgram MkMapCompInsert();
  TYPE_STKM_SubProgram MkMatchPattern();
  TYPE_STKM_SubProgram MkRuntimeBinaryOp(const TYPE_AS_BinaryOp &);
  TYPE_STKM_SubProgram MkRuntimePrefixOp(const TYPE_AS_UnaryOp &);
  TYPE_STKM_SubProgram MkRuntimeSetSeqMap(const Record &);
  TYPE_STKM_SubProgram MkRuntimeStartList();
  TYPE_STKM_SubProgram MkRuntimeStopList();
  TYPE_STKM_SubProgram MkSeqCompInsert();
  TYPE_STKM_SubProgram MkSetCompInsert();
  TYPE_STKM_SubProgram S2Time(const TYPE_AS_Stmt &);
  // }}}
#endif //VICE

private:
  Sequence nDefStmt;
  Sequence nLetStmt;
  Sequence nLetBeSTStmt;
  Sequence nAssignStmt;
  Sequence nAtomicAssignStmt;
  Sequence nSeqForLoopStmt;
  Sequence nSetForLoopStmt;
  Sequence nIndexForLoopStmt;
  Sequence nWhileLoopStmt;
  Sequence nCallStmt;
  Sequence nReturnStmt;
  Sequence nIfStmt;
  Sequence nCasesStmt;
  Sequence nErrorStmt;
  Sequence nExitStmt;
  Sequence nAlwaysStmt;
  Sequence nTrapStmt;
  Sequence nRecTrapStmt;
#ifdef VDMPP
  Sequence nStartStmt;
  Sequence nStartListStmt;
  Sequence nStopStmt;
  Sequence nStopListStmt;
#ifdef VICE
  Sequence nDurationStmt;
  Sequence nCycleStmt;
#endif // VICE
#endif // VDMPP
  Sequence nBlockStmt;
  Sequence nNonDetStmt;
  Sequence nIdent;
  Sequence nSpecificationStmt;
  Sequence nAssertStmt;

  Sequence nBracketedExpr;
  Sequence nDefExpr;
  Sequence nLetExpr;
  Sequence nLetBeSTExpr;
  Sequence nAllOrExistsExpr;
  Sequence nExistsUniqueExpr;
  Sequence nIotaExpr;
  Sequence nApplyExpr;
  Sequence nFieldSelectExpr;
  Sequence nIfExpr;
  Sequence nCasesExpr;
  Sequence nPrefixExpr;
  Sequence nBinaryExpr;
  Sequence nSetRangeExpr;
  Sequence nSubSequenceExpr;
  Sequence nSetEnumerationExpr;
  Sequence nSeqEnumerationExpr;
  Sequence nMapEnumerationExpr;
  Sequence nSetComprehensionExpr;
  Sequence nSeqComprehensionExpr;
  Sequence nMapComprehensionExpr;
  Sequence nTupleConstructorExpr;
  Sequence nRecordConstructorExpr;
  Sequence nRecordModifierExpr;
  Sequence nSeqModifyMapOverrideExpr;
  Sequence nLambdaExpr;
  Sequence nFctTypeInstExpr;
  Sequence nIsExpr;
  Sequence nNarrowExpr;
  Sequence nTokenConstructorExpr;
  Sequence nTupleSelectExpr;
  Sequence nTypeJudgementExpr;
  Sequence nPreConditionApplyExpr;
  Sequence nName;
  Sequence nOldName;
  Sequence nBoolLit;
  Sequence nNilLit;
  Sequence nRealLit;
  Sequence nNumLit;
  Sequence nTextLit;
  Sequence nCharLit;
  Sequence nQuoteLit;
  Sequence nUndefinedExpr;
  Sequence nLastRes;
  Sequence nMacro;
#ifdef VDMPP
  Sequence nSelfExpr;
  Sequence nNewExpr;
  Sequence nIsOfClassExpr;
  Sequence nIsOfBaseClassExpr;
  Sequence nSameBaseClassExpr;
  Sequence nSameClassExpr;
  Sequence nThreadIdExpr;
  Sequence nActExpr;
  Sequence nFinExpr;
  Sequence nActiveExpr;
  Sequence nWaitingExpr;
  Sequence nReqExpr;
#ifdef VICE
  Sequence nCurTimeExpr;
#endif // VICE
#endif // VDMPP
};

// }}}
#endif //_COMPILE_H
// }}}
