/***
*  * WHAT
*  *    Header file with class definition for VDM-SL latex generation
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/eval/genlatex.h,v $
*  * PROJECT
*  *    Afrodite - ESPRIT III programme, project no. 6500.
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __genlatex_h__
#define __genlatex_h__

#include "metaiv.h"       // include IPTES MetaIV class prototype
#include "AS.h"           /* For AS_TYPE_Name */
#include "contextinfo.h"
#include <string>
#include <fstream>
#include "vdmpars.h"

//extern Sequence orderly;
//extern Sequence modorder;

//////////////////////////////////////
// THE PRETTY PRINTER CLASS
//////////////////////////////////////

class GenLatexOutput {

public:
  static std::wstring ConvertStr (const std::wstring &);  

  GenLatexOutput() {};
  GenLatexOutput (const std::wstring &,
                  const Set &,
                  int,
                  const Sequence &,
                  const std::wstring &,
                  ContextInfo&);
                                        // transform abstract syntax 
                                        // to latex and print it in a file
  ~GenLatexOutput ();                   // close output file

private:
//////////////////////////////////
// CATEGORIES IN LATEX INDEX
//////////////////////////////////
  enum {
    fct_index = 100,      // definition of function (FuncDef)
    fctocc_index,         // function call (FuncOcc)
    typedef_index,        // definition of type (TypeDef)
    typeocc_index,        // use of type (TypeOcc)
    method_index,         // definition of method (MethodDef)
    methodocc_index,      // invokation/tracespec of method (MethodOcc)
    class_index,          // definition of class (ClassDef)
    classocc_index,       // use of class in import (ClassOcc)
    instvar_index,        // definition of instance variable (InstVarDef)
    mod_index,            // definition of module (ModDef)
    state_index           // definition of state (StateDef)
  };

/* module part */

#ifdef VDMSL
  void GenModule (const TYPE_AS_Module &, const Record &);
  void GenDLModule (const TYPE_AS_DLModule &, const Record &);
  void GenInterface (const TYPE_AS_Interface & intf);  
  void GenDLInterface (const TYPE_AS_DLInterface &);  
  void GenImportSig (const TYPE_AS_Name &, const Generic &, bool);
  void GenDLImportSig (const TYPE_AS_Name &, const Generic &, bool);
  void GenExportSig (const Generic &);
  void GenDLExportSig (const Generic &);
#endif // VDMSL
#ifdef VDMPP
  void GenClass (const TYPE_AS_Class &, const Record &);
#endif // VDMPP

/** Definitions part ********************************************************/
/** Reference : Doc. iptes-ifad-12-v1.0  ************************************/
/****************************************************************************/

  void GenDefinitions (const Generic & modnm, const TYPE_AS_Definitions & defs, const Sequence &, bool);

/** Type Definitions part ***************************************************/
/** Reference : Doc. iptes-ifad-12-v1.0  ************************************/
/****************************************************************************/

  void GenTypeDef (const Generic & modnm, const TYPE_AS_TypeDef &, bool);
  void GenType (const TYPE_AS_Type &);
  void GenInv (const Generic & modnm, const TYPE_AS_Invariant &, bool);
  void GenEqual (const Generic & modnm, const TYPE_AS_Equal &, bool);
  void GenOrder (const Generic & modnm, const TYPE_AS_Order &, bool);
  void GenBooleanType (const TYPE_AS_BooleanType &);
  void GenNumericType (const TYPE_AS_NumericType &);
  void GenTokenType (const TYPE_AS_TokenType &);
  void GenCharType (const TYPE_AS_CharType &);
  void GenVoidType (const TYPE_AS_VoidType &);
  void GenQuoteLit (const TYPE_AS_QuoteLit &, std::ofstream & ofs);
  void GenCompositeType (const TYPE_AS_CompositeType &, bool);
  void GenRecordType (const TYPE_AS_CompositeType &, bool);  
  void GenUnionType (const TYPE_AS_UnionType &);
  void GenProductType (const TYPE_AS_ProductType &);
  void GenOptionalType (const TYPE_AS_OptionalType &);
  void GenSet0Type (const TYPE_AS_Set0Type &);
  void GenSet1Type (const TYPE_AS_Set1Type &);
  void GenSeq0Type (const TYPE_AS_Seq0Type &);
  void GenSeq1Type (const TYPE_AS_Seq1Type &);
  void GenGeneralMap0Type (const TYPE_AS_GeneralMap0Type &);
  void GenGeneralMap1Type (const TYPE_AS_GeneralMap1Type &);
  void GenInjectiveMap0Type (const TYPE_AS_InjectiveMap0Type &);
  void GenInjectiveMap1Type (const TYPE_AS_InjectiveMap1Type &);
  void GenTypeName (const TYPE_AS_TypeName &);
  void GenFnType (const TYPE_AS_FnType &);  
  void GenTotalFnType (const TYPE_AS_TotalFnType &);
  void GenPartialFnType (const TYPE_AS_PartialFnType &);
  void GenOpType (const TYPE_AS_OpType &);
  void GenTypeVar (const TYPE_AS_TypeVar &);
  void GenBracketedType (const TYPE_AS_BracketedType &);
  void GenAccess(const Int&, const Record &, bool, bool);
  void GenStatic(const Bool&, const Record &, bool, bool);
  void GenAsync(const Bool&, const Record &, bool, bool);

/** State Definitions part **************************************************/
/** Reference : Doc. iptes-ifad-12-v1.0  ************************************/
/****************************************************************************/

#ifdef VDMSL
  void GenStateDef (const Generic & modnm, const TYPE_AS_StateDef &);
  void GenInit (const Generic & modnm, const TYPE_AS_Pattern &, const TYPE_AS_Expr &);
#endif //VDMSL
  void GenInvariant (const Generic & modnm, const TYPE_AS_Invariant &);

/** Value part **************************************************************/
/** Reference : Doc. iptes-ifad-12-v1.0  ************************************/
/****************************************************************************/

  void GenValueDef (const Generic & modnm, const TYPE_AS_ValueDef &, bool);

/** Function Definitions part ***********************************************/
/** Reference : Doc. iptes-ifad-12-v1.0  ************************************/
/****************************************************************************/

  void GenFunctionDef (const Generic & modnm, const TYPE_AS_FnDef &, bool);
  void GenExplFnDef (const Generic & modnm, const TYPE_AS_ExplFnDef &, bool);
  void GenExtExplFnDef (const Generic & modnm, const TYPE_AS_ExtExplFnDef &, bool);
  void GenImplFnDef (const Generic & modnm, const TYPE_AS_ImplFnDef &, bool);
  void GenPre (const Generic & modnm, const TYPE_AS_Expr &);
  void GenPost (const Generic & modnm, const TYPE_AS_Expr &);
  void GenFnBody (const Generic & modnm, const TYPE_AS_FnBody &);

/** Operation Definitions part **********************************************/
/** Reference : Doc. iptes-ifad-12-v1.0  ************************************/
/****************************************************************************/

  void GenOperationDef (const Generic & modnm, const TYPE_AS_OpDef &, bool);
  void GenExplOpDef (const Generic & modnm, const TYPE_AS_ExplOpDef &, bool);
  void GenExtExplOpDef (const Generic & modnm, const TYPE_AS_ExtExplOpDef &, bool);
  void GenImplOpDef (const Generic & modnm, const TYPE_AS_ImplOpDef &, bool);
  void GenExternals (const SEQ<TYPE_AS_ExtInf> &);
  void GenExceptions (const Generic & modnm, const SEQ<TYPE_AS_Error> &);
  void GenOpBody (const Generic & modnm, const TYPE_AS_OpBody &);

/** VDM++ Defs *************************************************************/
/****************************************************************************/

#ifdef VDMPP
  void GenInstanceInv (const Generic & modnm, const TYPE_AS_InstanceInv&);
  void GenInstanceVarDef (const Generic & modnm, const TYPE_AS_InstanceVarDef&, bool);
  void GenInstAssignDef(const Generic & modnm, const TYPE_AS_InstAssignDef&);
  void GenSyncDef (const Generic & modnm, const TYPE_AS_SyncDef&, bool);
  void GenPermission(const Generic & modnm, const TYPE_AS_Permission &, bool);
  void GenMutex(const Generic & modnm, const TYPE_AS_Mutex &, bool);
  void GenNameList(const TYPE_AS_NameList &, std::ofstream & ofs);
  void GenThreadDef (const Generic & modnm, const TYPE_AS_ThreadDef&, bool);
  void GenPerObl (const Generic & modnm, const TYPE_AS_PerObl &);
  void GenSpoObl (const Generic & modnm, const TYPE_AS_SpoObl &);
#endif // VDMPP
  void GenTraces(const Generic & modnm,
                 const SEQ<TYPE_AS_Name> & name_l,
                 const SEQ<TYPE_AS_TraceDefTerm> & tdt_l,
                 bool);
  void GenTraceDefList(const Generic & modnm, const SEQ<TYPE_AS_TraceDefTerm> & tdt_l);
  void GenTraceDefTerm(const Generic & modnm, const TYPE_AS_TraceDefTerm & tdt);
  void GenTraceApplyExpr(const Generic & modnm, const TYPE_AS_TraceApplyExpr & te);
  void GenTraceBracketedExpr(const Generic & modnm, const TYPE_AS_TraceBracketedExpr & tbe);
  void GenTraceConcurrentExpr(const Generic & modnm, const TYPE_AS_TraceConcurrentExpr & tbe);
  void GenQualifiedTrace(const Generic & modnm, const TYPE_AS_QualifiedTrace & qt);
  void GenRepeatTrace(const Generic & modnm, const TYPE_AS_RepeatTrace & rt);
  void GenQualifiedRepeatTrace(const Generic & modnm, const TYPE_AS_QualifiedRepeatTrace & qrt);
  void GenTraceDefAltn(const Generic & modnm, const TYPE_AS_TraceDefAltn & tda);
  void GenTraceRepeatPattern(const Generic & modnm, const TYPE_AS_TraceRepeatPattern & trp);
  void GenTraceBind(const Generic & modnm, const TYPE_AS_TraceBind & tb);
  void GenLocalTraceBind(const Generic & modnm, const TYPE_AS_LocalTraceBind & ltb);
  void GenLetTraceBind(const Generic & modnm, const TYPE_AS_LetTraceBind & ltb);
  void GenLetBeTraceBind(const Generic & modnm, const TYPE_AS_LetBeTraceBind & lbtb);

/** Statement part **********************************************************/
/** Reference : Doc. iptes-ifad-12-v1.0  ************************************/
/****************************************************************************/

  void GenStmt(const Generic & modnm, const TYPE_AS_Stmt &);
  void GenDefStmt(const Generic & modnm, const TYPE_AS_DefStmt &);
  void GenLetStmt(const Generic & modnm, const TYPE_AS_LetStmt &);
  void GenLetBeSTStmt(const Generic & modnm, const TYPE_AS_LetBeSTStmt &);
  void GenAssignStmt(const Generic & modnm, const TYPE_AS_AssignStmt &);
  void GenAtomicAssignStmt(const Generic & modnm, const TYPE_AS_AtomicAssignStmt &);
  void GenStateDesignator(const Generic & modnm, const TYPE_AS_StateDesignator &);
  void GenMapOrSeqRef(const Generic & modnm, const TYPE_AS_MapOrSeqRef &);
  void GenFieldRef(const Generic & modnm, const TYPE_AS_FieldRef &);
  void GenSeqForLoopStmt (const Generic & modnm, const TYPE_AS_SeqForLoopStmt &);
  void GenSetForLoopStmt (const Generic & modnm, const TYPE_AS_SetForLoopStmt &);
  void GenIndexForLoopStmt (const Generic & modnm, const TYPE_AS_IndexForLoopStmt &);
  void GenWhileLoopStmt (const Generic & modnm, const TYPE_AS_WhileLoopStmt &);
  void GenCallStmt (const Generic & modnm, const TYPE_AS_CallStmt &);
  void GenReturnStmt (const Generic & modnm, const TYPE_AS_ReturnStmt &);
  void GenIfStmt (const Generic & modnm, const TYPE_AS_IfStmt &);
  void GenCasesStmt (const Generic & modnm, const TYPE_AS_CasesStmt &);
  void GenErrorStmt (void);
  void GenAlwaysStmt (const Generic & modnm, const TYPE_AS_AlwaysStmt &);
  void GenExitStmt (const Generic & modnm, const TYPE_AS_ExitStmt &);
  void GenTrapStmt (const Generic & modnm, const TYPE_AS_TrapStmt &);
  void GenRecTrapStmt (const Generic & modnm, const TYPE_AS_RecTrapStmt &);
  void GenAssignDef (const Generic & modnm, const TYPE_AS_AssignDef &);
  void GenBlockStmt (const Generic & modnm, const TYPE_AS_BlockStmt &);
  void GenNonDetStmt (const Generic & modnm, const TYPE_AS_NonDetStmt &);
  void GenIdentStmt (void);
  void GenSpecificationStmt(const Generic & modnm, const TYPE_AS_SpecificationStmt &);
#ifdef VDMPP
  void GenStartStmt(const Generic & modnm, const TYPE_AS_StartStmt &);
  void GenStartListStmt(const Generic & modnm, const TYPE_AS_StartListStmt &);
  void GenStopStmt(const Generic & modnm, const TYPE_AS_StopStmt &);
  void GenStopListStmt(const Generic & modnm, const TYPE_AS_StopListStmt &);
#ifdef VICE
  void GenDurationStmt(const Generic & modnm, const TYPE_AS_DurationStmt &);
  void GenCycleStmt(const Generic & modnm, const TYPE_AS_CycleStmt &);
#endif // VICE
#endif // VDMPP

/** Expressions part ********************************************************/
/** Reference : Doc. iptes-ifad-12-v1.0  ************************************/
/****************************************************************************/

  void GenExpr(const Generic & modnm, const TYPE_AS_Expr &);
  void GenDefExpr (const Generic & modnm, const TYPE_AS_DefExpr &);
  void GenLetExpr (const Generic & modnm, const TYPE_AS_LetExpr &);
  void GenLetBeSTExpr (const Generic & modnm, const TYPE_AS_LetBeSTExpr &);
  void GenIfExpr (const Generic & modnm, const TYPE_AS_IfExpr &);
  void GenCasesExpr (const Generic & modnm, const TYPE_AS_CasesExpr &);
  void GenPrefixExpr (const Generic & modnm, const TYPE_AS_PrefixExpr &);
  void GenBinaryExpr (const Generic & modnm, const TYPE_AS_BinaryExpr &);
  void GenAllOrExistsExpr (const Generic & modnm, const TYPE_AS_AllOrExistsExpr &);
  void GenExistsUniqueExpr (const Generic & modnm, const TYPE_AS_ExistsUniqueExpr &);
  void GenSetEnumerationExpr (const Generic & modnm, const TYPE_AS_SetEnumerationExpr &);
  void GenSetComprehensionExpr (const Generic & modnm, const TYPE_AS_SetComprehensionExpr &);
  void GenSetRangeExpr (const Generic & modnm, const TYPE_AS_SetRangeExpr &);
  void GenSeqEnumerationExpr (const Generic & modnm, const TYPE_AS_SeqEnumerationExpr &);
  void GenSeqComprehensionExpr (const Generic & modnm, const TYPE_AS_SeqComprehensionExpr &);
  void GenSubSequenceExpr (const Generic & modnm, const TYPE_AS_SubSequenceExpr &);
  void GenSeqModifyMapOverrideExpr (const Generic & modnm, const TYPE_AS_SeqModifyMapOverrideExpr &);
  void GenMaplet (const Generic & modnm, const TYPE_AS_Expr &, const TYPE_AS_Expr &);
  void GenMapEnumerationExpr (const Generic & modnm, const TYPE_AS_MapEnumerationExpr &);
  void GenMapComprehensionExpr (const Generic & modnm, const TYPE_AS_MapComprehensionExpr &);
  void GenTupleConstructorExpr (const Generic & modnm, const TYPE_AS_TupleConstructorExpr &);
  void GenTokenConstructorExpr (const Generic & modnm, const TYPE_AS_TokenConstructorExpr &);
  void GenRecordConstructorExpr (const Generic & modnm, const TYPE_AS_RecordConstructorExpr &);
  void GenRecordModifierExpr (const Generic & modnm, const TYPE_AS_RecordModifierExpr &);
  void GenApplyExpr (const Generic & modnm, const TYPE_AS_ApplyExpr &);
  void GenFieldSelectExpr (const Generic & modnm, const TYPE_AS_FieldSelectExpr &);
  void GenLambdaExpr (const Generic & modnm, const TYPE_AS_LambdaExpr &);
  void GenIsExpr (const Generic & modnm, const TYPE_AS_IsExpr &);
  void GenLiteral (const TYPE_AS_Literal &, std::ofstream & ofs);
  void GenLabelName (const TYPE_AS_Name &, std::ofstream & ofs);
  void GenName (const TYPE_AS_Name &, std::ofstream & ofs);
  void GenOldName (const TYPE_AS_OldName &, std::ofstream & ofs);
  void GenUndefinedExpr (void);
  void GenTupleSelectExpr(const Generic & modnm, const TYPE_AS_TupleSelectExpr &);
  void GenTypeJudgementExpr(const Generic & modnm, const TYPE_AS_TypeJudgementExpr &);
  void GenNarrowExpr(const Generic & modnm, const TYPE_AS_NarrowExpr &);
  void GenMacro(const Generic & modnm, const TYPE_AS_Macro &);
  void GenPreConditionApplyExpr(const Generic & modnm, const TYPE_AS_PreConditionApplyExpr &);

  void GenIotaExpr (const Generic & modnm, const TYPE_AS_IotaExpr &);
  void GenFctTypeInstExpr (const Generic & modnm, const TYPE_AS_FctTypeInstExpr &);
  void GenBracketedExpr (const Generic & modnm, const TYPE_AS_BracketedExpr &);

#ifdef VDMPP
  void GenNewExpr(const Generic & modnm, const TYPE_AS_NewExpr &, std::ofstream & ofs);
  void GenSelfExpr(std::ofstream & ofs);
  void GenIsOfClassExpr(const Generic & modnm, const TYPE_AS_IsOfClassExpr &, std::ofstream & ofs);
  void GenIsOfBaseClassExpr(const Generic & modnm, const TYPE_AS_IsOfBaseClassExpr &, std::ofstream & ofs);
  void GenSameClassExpr(const Generic & modnm, const TYPE_AS_SameClassExpr &, std::ofstream & ofs);
  void GenSameBaseClassExpr(const Generic & modnm, const TYPE_AS_SameBaseClassExpr &, std::ofstream & ofs);
  void GenActExpr(const Generic & modnm, const TYPE_AS_ActExpr &, std::ofstream & ofs);
  void GenFinExpr(const Generic & modnm, const TYPE_AS_FinExpr &, std::ofstream & ofs);
  void GenActiveExpr(const Generic & modnm, const TYPE_AS_ActiveExpr &, std::ofstream & ofs);
  void GenWaitingExpr(const Generic & modnm, const TYPE_AS_WaitingExpr &, std::ofstream & ofs);
  void GenReqExpr(const Generic & modnm, const TYPE_AS_ReqExpr &, std::ofstream & ofs);
  void GenThreadIdExpr(std::ofstream & ofs);
#ifdef VICE
  void GenCurTimeExpr(std::ofstream & ofs);
#endif // VICE
#endif // VDMPP

  std::wstring InsertGreekLetters(const std::wstring & str,
                                  const std::wstring & prefix,
                                  const std::wstring & suffix) const; 
  std::wstring InsertGreekLettersInIndex (const std::wstring & str) const;
  std::wstring InsertGreekLettersInMathSection (const std::wstring & str) const;
  std::wstring InsertGreekLettersInLabelSection (const std::wstring & str) const;

  void GenRealId (const TYPE_AS_Id &, std::ofstream & ofs);
  void GenRealLabelId (const TYPE_AS_Id &, std::ofstream & ofs);
  std::wstring ConvertIndexStr (const std::wstring &) const;

/** Pattern part ************************************************************/
/** Reference : Doc. iptes-ifad-12-v1.0  ************************************/
/****************************************************************************/

  int PatLine (const TYPE_AS_Pattern &);
  int PatBindLine (const TYPE_AS_PatternBind &);

  void GenPattern(const Generic & modnm, const TYPE_AS_Pattern &);
  void GenPatternBind (const Generic & modnm, const Record &);
  void GenPatternName(const TYPE_AS_PatternName &);
  void GenMatchVal(const Generic & modnm, const TYPE_AS_MatchVal &);
  void GenSetEnumPattern(const Generic & modnm, const TYPE_AS_SetEnumPattern &);
  void GenSetUnionPattern(const Generic & modnm, const TYPE_AS_SetUnionPattern &);
  void GenSeqEnumPattern(const Generic & modnm, const TYPE_AS_SeqEnumPattern &);
  void GenSeqConcPattern(const Generic & modnm, const TYPE_AS_SeqConcPattern &);
  void GenMapEnumPattern(const Generic & modnm, const TYPE_AS_MapEnumPattern &);
  void GenMapMergePattern(const Generic & modnm, const TYPE_AS_MapMergePattern &);
  void GenRecordPattern(const Generic & modnm, const TYPE_AS_RecordPattern &);
  void GenTuplePattern(const Generic & modnm, const TYPE_AS_TuplePattern &);
#ifdef VDMPP
  void GenObjectPattern(const Generic & modnm, const TYPE_AS_ObjectPattern &);
#endif // VDMPP

  int BindLine (const TYPE_AS_Bind &);

  void GenBind(const Generic & modnm, const TYPE_AS_Bind &);
  void GenSetBind(const Generic & modnm, const TYPE_AS_SetBind &);
  void GenTypeBind (const Generic & modnm, const TYPE_AS_TypeBind &);
  void GenSeqBind(const Generic & modnm, const TYPE_AS_SeqBind &);
  void GenMultBind(const Generic & modnm, const TYPE_AS_MultBind &);
  void GenMultSetBind (const Generic & modnm, const TYPE_AS_MultSetBind &);
  void GenMultSeqBind (const Generic & modnm, const TYPE_AS_MultSeqBind &);
  void GenMultTypeBind (const Generic & modnm, const TYPE_AS_MultTypeBind &);

/** Auxilary part ***********************************************************/
/** Reference : Doc. iptes-ifad-12-v1.0  ************************************/
/****************************************************************************/

  void ErrMsg1 (const Record &, const std::wstring &); // error msg handler of this class
  void StartDef (int, int *);
  Map InverseMap (const Map &);
  void GenRenaming (const Record &, const Map &);
  string LineFeed (void);
  std::wstring Name2Str (const TYPE_AS_Name &);

  void GenIndex (const TYPE_AS_Name &, int);
  bool IsPrePostInv (const TYPE_AS_Name &);
  int GetLine (const Generic & rc);

private:
  ContextInfo *contextinfo;              // The contextinfo table to pretty print from
  
  bool linefeed;
  Set Fns;

  // Latex output stream
  std::ofstream LatexOutput;

  //
  int INDEX;

  // the current piece of abstract syntax that is being used ???
  //TYPE_AS_Definitions current_ast;

  // the current module or Nil
  //Generic current_module;

  // flag to indicate whether stmt/expr has been covered
  bool not_covered_area;

  bool post_condition_scope;
};

#endif // __genlatex_h__
