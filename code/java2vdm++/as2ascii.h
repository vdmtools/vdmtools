//
// AS backend - generates textual version of an AS value - header file
// $Id: as2ascii.h,v 1.6 2005/10/25 05:00:25 vdmtools Exp $
//

#ifndef __as2ascii_h__
#define __as2ascii_h__

#include "AS.h"
#include "CI.h"
#include <iostream>

class AS2ASCII {

 public:
  AS2ASCII() :indent(0){}
#ifdef VDMPP
  void Class2ASCII(const TYPE_AS_Class &, wostream &, const TYPE_CI_TokenContextInfo &);
#endif // VDMPP
  void BeginDocument(wostream&);
  void EndDocument(wostream&);

 private:
  // attributes
  int indent;
  const TYPE_CI_TokenContextInfo* pTokenci;

  // operations

  wstring GetComments(const TYPE_CI_ContextId & cid);

  void IncIndent(){ indent += 2;}
  void DecIndent(){ indent -= 2;}

  void Definitions2ASCII(const TYPE_AS_Definitions&, wostream &);
  void Newline(wostream&, int i=1);
  void MakeKeyword(const wstring &, wostream &);
  void MakeSpace(wostream &);
  wstring ExtractName(const TYPE_AS_Name & nm);
  void MakeNameDef(const wstring & nm, wostream & result);
  void MakeNameDef(const wstring & url, const wstring & nm,  wostream & result);
  void MakeNameRef(const wstring & nm, wostream & result);
  void Access2ASCII(const TYPE_AS_Access&, wostream&);
  void Invariant2ASCII(const TYPE_AS_Invariant &, wostream &);

  void TypeDefs2ASCII(const MAP<TYPE_AS_Name, TYPE_AS_TypeDef> &, wostream &);
  void TypeDef2ASCII(const TYPE_AS_TypeDef & td, wostream & result);

  void ValueDefs2ASCII(const SEQ<TYPE_AS_ValueDef> &, wostream &);
  void ValueDef2ASCII(const TYPE_AS_ValueDef &, wostream &);

  void FuncDefs2ASCII(const MAP<TYPE_AS_Name, TYPE_AS_FnDef> &, wostream &);

  void ExplFuncDef2ASCII(const TYPE_AS_ExplFnDef & fd, wostream & result);
  void ImplFuncDef2ASCII(const TYPE_AS_ImplFnDef & fd, wostream & result);
  void ExtExplFuncDef2ASCII(const TYPE_AS_ExtExplFnDef & fd, wostream & result);
  void OpDefs2ASCII(const MAP<TYPE_AS_Name, TYPE_AS_OpDef> &, wostream &);
#ifdef VDMPP
  void InstVars2ASCII(const SEQ<TYPE_AS_InstanceVarDef> &, wostream &);
  void InstVar2ASCII(const TYPE_AS_InstanceVarDef &, wostream &);

  void Syncs2ASCII(const SEQ<TYPE_AS_SyncDef> &, wostream &);
  void SyncDef2ASCII(const TYPE_AS_SyncDef &, wostream &);

  void Thread2ASCII(const TYPE_AS_ThreadDef &, wostream &);
#endif // VDMPP

  void FuncDef2ASCII(const TYPE_AS_FnDef &, wostream &);
  void FnBody2ASCII(const TYPE_AS_FnBody &, wostream &);

  void OpDef2ASCII(const TYPE_AS_OpDef &, wostream &);
  void ExplOpDef2ASCII(const TYPE_AS_ExplOpDef &, wostream &);
  void ImplOpDef2ASCII(const TYPE_AS_ImplOpDef &, wostream &);
  void ExtInf2ASCII(const TYPE_AS_ExtInf& ei, wostream& result);
  void NameType2ASCII(const TYPE_AS_NameType& nt, wostream& result);
  void ExtExplOpDef2ASCII(const TYPE_AS_ExtExplOpDef&, wostream&);
  void OpBody2ASCII(const TYPE_AS_OpBody&, wostream&);

  void Static2ASCII(const Bool& isStatic, wostream& result);

  // Types
public:
  void Type2ASCII(const TYPE_AS_Type&, wostream&);
private:
  void BooleanType2ASCII(const TYPE_AS_BooleanType& tp, wostream& result);
  void NumericType2ASCII(const TYPE_AS_NumericType& tp, wostream& result);
  void TokenType2ASCII(const TYPE_AS_TokenType& tp, wostream& result);
  void CharType2ASCII(const TYPE_AS_CharType& tp, wostream& result);
  void VoidType2ASCII(const TYPE_AS_VoidType& tp, wostream& result);
  void QuoteType2ASCII(const TYPE_AS_QuoteType& tp, wostream& result);
  void CompositeType2ASCII(const TYPE_AS_CompositeType& tp, wostream& result);
  void UnionType2ASCII(const TYPE_AS_UnionType& tp, wostream& result);
  void ProductType2ASCII(const TYPE_AS_ProductType& tp, wostream& result);
  void OptionalType2ASCII(const TYPE_AS_OptionalType& tp, wostream& result);
  void Set0Type2ASCII(const TYPE_AS_Set0Type& tp, wostream& result);
  void Set1Type2ASCII(const TYPE_AS_Set1Type& tp, wostream& result);
  void Seq0Type2ASCII(const TYPE_AS_Seq0Type& tp, wostream& result);
  void Seq1Type2ASCII(const TYPE_AS_Seq1Type& tp, wostream& result);
  void GeneralMap0Type2ASCII(const TYPE_AS_GeneralMap0Type& tp, wostream& result);
  void GeneralMap1Type2ASCII(const TYPE_AS_GeneralMap1Type& tp, wostream& result);
  void InjectiveMap0Type2ASCII(const TYPE_AS_InjectiveMap0Type& tp, wostream& result);
  void InjectiveMap1Type2ASCII(const TYPE_AS_InjectiveMap1Type& tp, wostream& result);
  void TypeName2ASCII(const TYPE_AS_TypeName& tp, wostream& result);
  void PartialFnType2ASCII(const TYPE_AS_PartialFnType& tp, wostream& result);
  void TotalFnType2ASCII(const TYPE_AS_TotalFnType& tp, wostream& result);
  void BracketedType2ASCII(const TYPE_AS_BracketedType& tp, wostream& result);
  void OpType2ASCII(const TYPE_AS_OpType& tp, wostream& result);
  void TypeVar2ASCII(const TYPE_AS_TypeVar& tp, wostream& result);
  void TypeSequence2ASCII(const SEQ<TYPE_AS_Type> & tps, wchar_t sep, wostream & result);
  void TypeVarList2ASCII(const TYPE_AS_TypeVarList&, wostream&);

  // Expressions
public:
  void Expr2ASCII(const TYPE_AS_Expr &, wostream &);
private:
  void ExprSequence2ASCII(const SEQ<TYPE_AS_Expr> & exprs, wstring sep, wostream &);
  void LocalDef2ASCII(const TYPE_AS_LocalDef&, wostream&);
  
  void DefExpr2ASCII(const TYPE_AS_DefExpr&, wostream&);
  void DefBinds2ASCII(const Sequence &, wostream&);
  void LetExpr2ASCII(const TYPE_AS_LetExpr &, wostream&);
  void LocalDefs2ASCII(const SEQ<TYPE_AS_LocalDef> &, wostream&);
  void LetBeSTExpr2ASCII(const TYPE_AS_LetBeSTExpr&, wostream&);
  void AllOrExistsExpr2ASCII(const TYPE_AS_AllOrExistsExpr&, wostream&);
  void ExistsUniqueExpr2ASCII(const TYPE_AS_ExistsUniqueExpr&, wostream&);
  void IotaExpr2ASCII(const TYPE_AS_IotaExpr&, wostream&);
  void ApplyExpr2ASCII(const TYPE_AS_ApplyExpr&, wostream&);
  void FieldSelectExpr2ASCII(const TYPE_AS_FieldSelectExpr&, wostream&);
  void IfExpr2ASCII(const TYPE_AS_IfExpr&, wostream&);
  void ElseifExpr2ASCII(const TYPE_AS_ElseifExpr&, wostream&);
  void CasesExpr2ASCII(const TYPE_AS_CasesExpr&, wostream&);
  void CaseAltn2ASCII(const TYPE_AS_CaseAltn&, wostream&);
  void PrefixExpr2ASCII(const TYPE_AS_PrefixExpr&, wostream&);
  void BinaryExpr2ASCII(const TYPE_AS_BinaryExpr&, wostream&);
  void SetRangeExpr2ASCII(const TYPE_AS_SetRangeExpr&, wostream&);
  void SubSequenceExpr2ASCII(const TYPE_AS_SubSequenceExpr&, wostream&);
  void SetEnumExpr2ASCII(const TYPE_AS_SetEnumerationExpr&, wostream&);
  void SeqEnumExpr2ASCII(const TYPE_AS_SeqEnumerationExpr&, wostream&);
  void MapEnumExpr2ASCII(const TYPE_AS_MapEnumerationExpr&, wostream&);
  void SetComprehensionExpr2ASCII(const TYPE_AS_SetComprehensionExpr&, wostream&);
  void SeqComprehensionExpr2ASCII(const TYPE_AS_SeqComprehensionExpr&, wostream&);
  void MapComprehensionExpr2ASCII(const TYPE_AS_MapComprehensionExpr&, wostream&);
  void Maplet2ASCII(const TYPE_AS_Maplet& maplet, wostream& result);
  void TupleConstructorExpr2ASCII(const TYPE_AS_TupleConstructorExpr&, wostream&);
  void RecordConstructorExpr2ASCII(const TYPE_AS_RecordConstructorExpr&, wostream&);
  void RecordModifierExpr2ASCII(const TYPE_AS_RecordModifierExpr&, wostream&);
  void RecordModification2ASCII(const TYPE_AS_RecordModification&, wostream&);
  void SeqModifyMapOverrideExpr2ASCII(const TYPE_AS_SeqModifyMapOverrideExpr&, wostream&);
  void LambdaExpr2ASCII(const TYPE_AS_LambdaExpr&, wostream&);
  void FctTypeInstExpr2ASCII(const TYPE_AS_FctTypeInstExpr&, wostream&);
  void IsExpr2ASCII(const TYPE_AS_IsExpr&, wostream&);
  void TokenConstructorExpr2ASCII(const TYPE_AS_TokenConstructorExpr&, wostream&);
  void TupleSelectExpr2ASCII(const TYPE_AS_TupleSelectExpr&, wostream&);
  void TypeJudgementExpr2ASCII(const TYPE_AS_TypeJudgementExpr&, wostream&);
  void NarrowExpr2ASCII(const TYPE_AS_NarrowExpr&, wostream&);
  void PreConditionApplyExpr2ASCII(const TYPE_AS_PreConditionApplyExpr&, wostream&);
  void NameExpr2ASCII(const TYPE_AS_Name&, wostream&);
  void OldNameExpr2ASCII(const TYPE_AS_OldName&, wostream&);
#ifdef VDMPP
  void SelfExpr2ASCII(const TYPE_AS_SelfExpr&, wostream&);
  void NewExpr2ASCII(const TYPE_AS_NewExpr&, wostream&);
  void IsOfClassExpr2ASCII(const TYPE_AS_IsOfClassExpr&, wostream&);
  void IsOfBaseClassExpr2ASCII(const TYPE_AS_IsOfBaseClassExpr&, wostream&);
  void SameBaseClassExpr2ASCII(const TYPE_AS_SameBaseClassExpr&, wostream&);
  void SameClassExpr2ASCII(const TYPE_AS_SameClassExpr&, wostream&);
  void ThreadIdExpr2ASCII(const TYPE_AS_ThreadIdExpr&, wostream&);
#ifdef VICE
  void CurTimeExpr2ASCII(const TYPE_AS_CurTimeExpr&, wostream&);
#endif // VICE
  void HistoryExpr2ASCII(const TYPE_AS_Expr&, wostream&);
#endif // VDMPP
  void Macro2ASCII(const TYPE_AS_Macro&, wostream&);

  // statements
public:
  void Stmt2ASCII(const TYPE_AS_Stmt&, wostream& result);

private:
  void DefStmt2ASCII(const TYPE_AS_DefStmt&, wostream&);
  void LetStmt2ASCII(const TYPE_AS_LetStmt&, wostream&);
  void LetBeSTStmt2ASCII(const TYPE_AS_LetBeSTStmt&, wostream&);
  void AssignDef2ASCII(const TYPE_AS_AssignDef&, wostream&);
  void AssignStmt2ASCII(const TYPE_AS_AssignStmt&, wostream&);
  void AtomicAssignStmt2ASCII(const TYPE_AS_AtomicAssignStmt&, wostream&);
  void SeqForLoopStmt2ASCII(const TYPE_AS_SeqForLoopStmt&, wostream&);
  void SetForLoopStmt2ASCII(const TYPE_AS_SetForLoopStmt&, wostream&);
  void IndexForLoopStmt2ASCII(const TYPE_AS_IndexForLoopStmt&, wostream&);
  void WhileLoopStmt2ASCII(const TYPE_AS_WhileLoopStmt&, wostream&);
  void CallStmt2ASCII(const TYPE_AS_CallStmt&, wostream&);
  void ReturnStmt2ASCII(const TYPE_AS_ReturnStmt&, wostream&);
  void IfStmt2ASCII(const TYPE_AS_IfStmt&, wostream&);
  void ElseifStmt2ASCII(const TYPE_AS_ElseifStmt&, wostream&);
  void CasesStmt2ASCII(const TYPE_AS_CasesStmt&, wostream&);
  void CasesStmtAltn2ASCII(const TYPE_AS_CasesStmtAltn&, wostream&);
  void ErrorStmt2ASCII(const TYPE_AS_ErrorStmt&, wostream&);
  void ExitStmt2ASCII(const TYPE_AS_ExitStmt&, wostream&);
  void AlwaysStmt2ASCII(const TYPE_AS_AlwaysStmt&, wostream&);
  void TrapStmt2ASCII(const TYPE_AS_TrapStmt&, wostream&);
  void RecTrapStmt2ASCII(const TYPE_AS_RecTrapStmt&, wostream&);
  void Trap2ASCII(const TYPE_AS_Trap& trap, wostream& result);
#ifdef VDMPP
  void StartStmt2ASCII(const TYPE_AS_StartStmt&, wostream&);
  void StartListStmt2ASCII(const TYPE_AS_StartListStmt&, wostream&);
  void StopStmt2ASCII(const TYPE_AS_StopStmt&, wostream&);
  void StopListStmt2ASCII(const TYPE_AS_StopListStmt&, wostream&);
#endif // VDMPP
  void BlockStmt2ASCII(const TYPE_AS_BlockStmt&, wostream&);
  void NonDetStmt2ASCII(const TYPE_AS_NonDetStmt&, wostream&);
  void SpecStmt2ASCII(const TYPE_AS_SpecificationStmt&, wostream&);

public:
  void StateDesignator2ASCII(const TYPE_AS_StateDesignator&, wostream&);

private:
  void StmtSequence2ASCII(const SEQ<TYPE_AS_Stmt> & stmts, wostream& result);

  // Patterns
public:
  void Pattern2ASCII(const TYPE_AS_Pattern& pat, wostream&);

private:
  void PatternName2ASCII(const TYPE_AS_PatternName&, wostream&);
  void MatchVal2ASCII(const TYPE_AS_MatchVal&, wostream&);
  void SetPattern2ASCII(const TYPE_AS_SetPattern&, wostream&);
  void SeqPattern2ASCII(const TYPE_AS_SeqPattern&, wostream&);    
  void TuplePattern2ASCII(const TYPE_AS_TuplePattern&, wostream&);
  void RecordPattern2ASCII(const TYPE_AS_RecordPattern&, wostream&);
  void PatternSequence2ASCII(const SEQ<TYPE_AS_Pattern> & pats, wostream & result);
  void PatternBind2ASCII(const TYPE_AS_PatternBind&, wostream&);
  void Bind2ASCII(const TYPE_AS_Bind& bind, wostream& result);
  void SetBind2ASCII(const TYPE_AS_SetBind& bind, wostream& result);
  void TypeBind2ASCII(const TYPE_AS_TypeBind& bind, wostream& result);
  void SeqBind2ASCII(const TYPE_AS_SeqBind& bind, wostream& result);
  void BindList2ASCII(const TYPE_AS_BindList&, wostream&);
  void MultBind2ASCII(const TYPE_AS_MultBind& mb, wostream& result);
  void ParameterTypes2ASCII(const TYPE_AS_ParameterTypes&, wostream&);

  // Literals
  void QuoteLit2ASCII(const TYPE_AS_QuoteLit&, wostream&);
  void BoolLit2ASCII(const TYPE_AS_BoolLit&, wostream&);
  void NilLit2ASCII(const TYPE_AS_NilLit&, wostream&);
  void RealLit2ASCII(const TYPE_AS_RealLit&, wostream&);
  void NumLit2ASCII(const TYPE_AS_NumLit&, wostream&);
  void TextLit2ASCII(const TYPE_AS_TextLit&, wostream&);
  void CharLit2ASCII(const TYPE_AS_CharLit&, wostream&);

};

#endif //__as2ascii_h__
