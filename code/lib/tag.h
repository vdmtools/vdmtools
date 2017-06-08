/***
*  * WHAT
*  *    Integer Record Tag definitions.
*  *    Implemenation of
*  *       common_as.vdm r1.33
*  *       rep.vdm r1.16
*  *       global.vdm r1.9
*  *
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/lib/tag.h,v $
*  * VERSION
*  *    $Revision: 1.44 $
*  * DATE
*  *    $Date: 2006/03/15 07:47:45 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    VDM-SL Toolbox.
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

/* VAL tags defined separately in val_tag.h as the must also */

/* be used in metaiv.cc for ConstructVAL() and val2generic() */

#ifndef INCL_TAG_H
#define INCL_TAG_H

#include "main_tag.h"
#include "AS.h"
#include "REP.h"
#include "position.h"

/* Record tags for dynamic semantics and abstract syntax */

#define Module TAG_TYPE_AS_Module
//#define Interface TAG_TYPE_AS_Interface

#define Definitions TAG_TYPE_AS_Definitions
#define TypeDef TAG_TYPE_AS_TypeDef
#define UnitType TAG_TYPE_AS_UnitType
#define RetType TAG_TYPE_AS_RetType
#define BasicType TAG_TYPE_AS_BasicType
#define QuoteType TAG_TYPE_AS_QuoteType
#define CompositeType TAG_TYPE_AS_CompositeType
//#define Field TAG_TYPE_AS_Field
#define UnionType TAG_TYPE_AS_UnionType
#define ProductType TAG_TYPE_AS_ProductType
#define OptionalType TAG_TYPE_AS_OptionalType
#define SetType TAG_TYPE_AS_SetType
#define AllType TAG_TYPE_AS_AllType

#define TypeName TAG_TYPE_AS_TypeName
#define StateDef TAG_TYPE_AS_StateDef
#define Invariant TAG_TYPE_AS_Invariant
#define StateInit TAG_TYPE_AS_StateInit


#define DefStmt TAG_TYPE_AS_DefStmt
#define LetStmt TAG_TYPE_AS_LetStmt
#define LetBeSTStmt TAG_TYPE_AS_LetBeSTStmt
#define AssignStmt TAG_TYPE_AS_AssignStmt
#define AtomicAssignStmt TAG_TYPE_AS_AtomicAssignStmt

#define SetForLoopStmt TAG_TYPE_AS_SetForLoopStmt
#define IndexForLoopStmt TAG_TYPE_AS_IndexForLoopStmt
#define WhileLoopStmt TAG_TYPE_AS_WhileLoopStmt
#define CallStmt TAG_TYPE_AS_CallStmt
#define ReturnStmt TAG_TYPE_AS_ReturnStmt
#define IfStmt TAG_TYPE_AS_IfStmt                          
#define CasesStmt TAG_TYPE_AS_CasesStmt
#define CasesStmtAltn TAG_TYPE_AS_CasesStmtAltn
#define ErrorStmt TAG_TYPE_AS_ErrorStmt
#define DefExpr TAG_TYPE_AS_DefExpr
#define LetExpr TAG_TYPE_AS_LetExpr
#define LetBeSTExpr TAG_TYPE_AS_LetBeSTExpr
#define IfExpr TAG_TYPE_AS_IfExpr
#define CasesExpr TAG_TYPE_AS_CasesExpr
#define CaseAltn TAG_TYPE_AS_CaseAltn
#define BinaryExpr TAG_TYPE_AS_BinaryExpr
#define AllOrExistsExpr TAG_TYPE_AS_AllOrExistsExpr
#define ExistsUniqueExpr TAG_TYPE_AS_ExistsUniqueExpr
#define SetEnumerationExpr TAG_TYPE_AS_SetEnumerationExpr
#define SetComprehensionExpr TAG_TYPE_AS_SetComprehensionExpr
#define SetRangeExpr TAG_TYPE_AS_SetRangeExpr
#define SeqEnumerationExpr TAG_TYPE_AS_SeqEnumerationExpr
#define SeqComprehensionExpr TAG_TYPE_AS_SeqComprehensionExpr
#define SubSequenceExpr TAG_TYPE_AS_SubSequenceExpr
#define SeqModifyMapOverrideExpr TAG_TYPE_AS_SeqModifyMapOverrideExpr
#define MapEnumerationExpr TAG_TYPE_AS_MapEnumerationExpr
#define Maplet TAG_TYPE_AS_Maplet
#define MapComprehensionExpr TAG_TYPE_AS_MapComprehensionExpr
#define TupleConstructorExpr TAG_TYPE_AS_TupleConstructorExpr
#define TupleSelectExpr TAG_TYPE_AS_TupleSelectExpr
#define TypeJudgementExpr TAG_TYPE_AS_TypeJudgementExpr
#define PreConditionApplyExpr TAG_TYPE_AS_PreConditionApplyExpr
#define RecordConstructorExpr TAG_TYPE_AS_RecordConstructorExpr
#define RecordModifierExpr TAG_TYPE_AS_RecordModifierExpr
#define ApplyExpr TAG_TYPE_AS_ApplyExpr
#define FieldSelectExpr TAG_TYPE_AS_FieldSelectExpr
#define IsExpr TAG_TYPE_AS_IsExpr
#define BoolLit TAG_TYPE_AS_BoolLit
#define RealLit TAG_TYPE_AS_RealLit
#define CharLit TAG_TYPE_AS_CharLit 
#define TextLit TAG_TYPE_AS_TextLit
#define QuoteLit TAG_TYPE_AS_QuoteLit
#define NilLit TAG_TYPE_AS_NilLit
//#define Name TAG_TYPE_AS_Name
#define OldName TAG_TYPE_AS_OldName
#define UndefinedExpr TAG_TYPE_AS_UndefinedExpr
#define MapOrSeqRef TAG_TYPE_AS_MapOrSeqRef
#define FieldRef TAG_TYPE_AS_FieldRef
#define PatternId TAG_TYPE_AS_PatternId
#define MatchVal TAG_TYPE_AS_MatchVal
#define SetEnumPattern TAG_TYPE_AS_SetEnumPattern
#define SetUnionPattern TAG_TYPE_AS_SetUnionPattern
#define SeqEnumPattern TAG_TYPE_AS_SeqEnumPattern
#define SeqConcPattern TAG_TYPE_AS_SeqConcPattern
#define RecordPattern TAG_TYPE_AS_RecordPattern
#define TuplePattern TAG_TYPE_AS_TuplePattern
#define SetBind TAG_TYPE_AS_SetBind
#define MultSetBind TAG_TYPE_AS_MultSetBind


#define SeqForLoopStmt TAG_TYPE_AS_SeqForLoopStmt

/* Extensions to ISO/VDM-SL (full syntax) */

#define AlwaysStmt TAG_TYPE_AS_AlwaysStmt
#define TrapStmt TAG_TYPE_AS_TrapStmt
#define RecTrapStmt TAG_TYPE_AS_RecTrapStmt /* TixeStmt */
#define ExitStmt TAG_TYPE_AS_ExitStmt
#define IdentStmt TAG_TYPE_AS_IdentStmt
#define BlockStmt TAG_TYPE_AS_BlockStmt

#define TypeBind TAG_TYPE_AS_TypeBind
#define MultTypeBind TAG_TYPE_AS_MultTypeBind
#define MapInverseExpr TAG_TYPE_AS_MapInverseExpr
#define IotaExpr TAG_TYPE_AS_IotaExpr

#define Seq0Type TAG_TYPE_AS_Seq0Type
#define Seq1Type TAG_TYPE_AS_Seq1Type
#define GeneralMapType TAG_TYPE_AS_GeneralMapType
#define InjectiveMapType TAG_TYPE_AS_InjectiveMapType
#define PartialFnType TAG_TYPE_AS_PartialFnType
#define TotalFnType TAG_TYPE_AS_TotalFnType
#define TypeVar TAG_TYPE_AS_TypeVar

#define ExplFnDef TAG_TYPE_AS_ExplFnDef
#define ImplFnDef TAG_TYPE_AS_ImplFnDef
#define ExplOpDef TAG_TYPE_AS_ExplOpDef
#define ImplOpDef TAG_TYPE_AS_ImplOpDef
#define IdType TAG_TYPE_AS_IdType
#define PatTypePair TAG_TYPE_AS_PatTypePair
#define OpType TAG_TYPE_AS_OpType
#define ExtInf TAG_TYPE_AS_ExtInf
//#define Error TAG_TYPE_AS_Error

/* Record tags which should have neen in the group containing
   ExplFnDef and ExplOpDef */
#define ExtExplFnDef TAG_TYPE_AS_ExtExplFnDef
#define ExtExplOpDef TAG_TYPE_AS_ExtExplOpDef
   
#define BracketedExpr TAG_TYPE_AS_BracketedExpr
#define LambdaExpr TAG_TYPE_AS_LambdaExpr
#define FctTypeInstExpr TAG_TYPE_AS_FctTypeInstExpr
#define LocalFnDef TAG_TYPE_AS_LocalFnDef

#define NonDetStmt TAG_TYPE_AS_NonDetStmt
#define AssignDef TAG_TYPE_AS_AssignDef
#define InstAssignDef TAG_TYPE_AS_InstAssignDef
#define Trap TAG_TYPE_AS_Trap
#define ElseifStmt TAG_TYPE_AS_ElseifStmt

#define ElseifExpr TAG_TYPE_AS_ElseifExpr
#define RecordModification TAG_TYPE_AS_RecordModification


#define TokenConstructorExpr TAG_TYPE_AS_TokenConstructorExpr

#define PrefixExpr TAG_TYPE_AS_PrefixExpr


/* Record tags for SEM`GLOBAL */

//#define State TAG_TYPE_DYNSEM_GLOBAL_State
//#define StateMap TAG_TYPE_AS_StateMap

/* Additions for modules as specified by KDB. Implememted by MA. */

#define Document TAG_TYPE_AS_Document
#define ImportSig TAG_TYPE_AS_ImportSig
#define ExportSig TAG_TYPE_AS_ExportSig

//#define NameType TAG_TYPE_AS_NameType
#define PatternName TAG_TYPE_AS_PatternName

/* Pretty printer */

#define FnDef TAG_TYPE_PRETTY_PRINTER_FnDef
#define OpDef TAG_TYPE_PRETTY_PRINTER_OpDef
#define InstVarDef TAG_TYPE_PRETTY_PRINTER_InstVarDef
#define TimeVarDef TAG_TYPE_PRETTY_PRINTER_TimeVarDef
#define SyncDef TAG_TYPE_PRETTY_PRINTER_SyncDef
#define ThreadDef TAG_TYPE_PRETTY_PRINTER_ThreadDef

#define ValueDef TAG_TYPE_AS_ValueDef
#define FunctionDef TAG_TYPE_AS_FunctionDef
#define OperationDef TAG_TYPE_AS_OperationDef
#define BracketedType TAG_TYPE_AS_BracketedType


#define Texbreak TAG_TYPE_PRETTY_PRINTER_Texbreak
#define ModOrder TAG_TYPE_PRETTY_PRINTER_ModOrder
#define ClassOrder TAG_TYPE_PRETTY_PRINTER_ClassOrder

/* Record tags for dl modules. */


///This is how it should be:
//// HC ////#define DLModule TAG_TYPE_AS_DLModule   
//// HC ////#define DLInterface TAG_TYPE_AS_DLInterface
//// HC ////#define DLImportSig TAG_TYPE_AS_DLImportSig
//// HC ////#define DLExportSig TAG_TYPE_AS_DLExportSig
//// HC ////
//// however for some reason the impl uses DLModule and the spec
//// uses ImplModule (???) Therefore:

#define DLModule TAG_TYPE_AS_DLModule   
#define DLInterface TAG_TYPE_AS_DLInterface
#define DLImportSig TAG_TYPE_AS_DLImportSig
#define DLExportSig TAG_TYPE_AS_DLExportSig

// This define is necessary because ProofInfo is not defined in tag.h
//#define ProofInfo TAG_TYPE_AS_ProofInfo 

#define SigmaEXP TAG_TYPE_DYNSEM_GLOBAL_SigmaEXP
#define SigmaIMO TAG_TYPE_DYNSEM_GLOBAL_SigmaIMO

//#define ImplExportSig	TAG_TYPE_AS_ImplExportSig
//#define ImplImportSig	TAG_TYPE_AS_ImplImportSig
//#define ImplInterface	TAG_TYPE_AS_ImplInterface
//#define ImplModule	TAG_TYPE_AS_ImplModule

/*  Record tags for error reporting */

//#define SyntaxError  TAG_TYPE_PARSER_SyntaxError
//#define MSCall       TAG_TYPE_PARSER_MSCall
//#define ErrMsg       TAG_TYPE_PARSER_ErrMsg

/* Tag for result for last evaluated expression */
//#define LastRes      TAG_TYPE_PARSER_LastRes


/************************* VDM++ **************************************/
/*********************** AFRODITE *************************************/

#define InstanceInv TAG_TYPE_AS_InstanceInv
//#define IntInf TAG_TYPE_AS_IntInf
//#define SetEnumType TAG_TYPE_AS_SetEnumType
//#define DelayStmt TAG_TYPE_AS_DelayStmt
#define StartStmt TAG_TYPE_AS_StartStmt
#define SelfExpr TAG_TYPE_AS_SelfExpr
#define NewExpr TAG_TYPE_AS_NewExpr
#define IsOfClassExpr TAG_TYPE_AS_IsOfClassExpr
#define IsOfBaseClassExpr TAG_TYPE_AS_IsOfBaseClassExpr
//#define PrelimType TAG_TYPE_AS_PrelimType
#define SpecificationStmt TAG_TYPE_AS_SpecificationStmt
//#define MethodInvoke TAG_TYPE_AS_MethodInvoke
#define StartListStmt TAG_TYPE_AS_StartListStmt
//#define TopologyStmt TAG_TYPE_AS_TopologyStmt
//#define Trace TAG_TYPE_AS_Trace
//#define AlphabetExpr TAG_TYPE_AS_AlphabetExpr
//#define SelectStmt TAG_TYPE_AS_SelectStmt
//#define TraceExpr TAG_TYPE_AS_TraceExpr
//#define CTerm TAG_TYPE_AS_CTerm
//#define CTerm0 TAG_TYPE_AS_CTerm0
//#define CTerm1 TAG_TYPE_AS_CTerm1
//#define PrefTrace TAG_TYPE_AS_PrefTrace
//#define DelTrace TAG_TYPE_AS_DelTrace
//#define WeaveTrace TAG_TYPE_AS_WeaveTrace
//#define SyncTrace TAG_TYPE_AS_SyncTrace
//#define QsyncTrace TAG_TYPE_AS_QsyncTrace
#define PerObl TAG_TYPE_AS_PerObl
#define Permission TAG_TYPE_AS_Permission
//#define GuardedCommand TAG_TYPE_AS_GuardedCommand
//#define AnswerStmt TAG_TYPE_AS_AnswerStmt
//#define AlphaTrace TAG_TYPE_AS_AlphaTrace
#define ActExpr TAG_TYPE_AS_ActExpr
#define FinExpr TAG_TYPE_AS_FinExpr
#define ActiveExpr TAG_TYPE_AS_ActiveExpr
#define WaitingExpr TAG_TYPE_AS_WaitingExpr
#define ReqExpr TAG_TYPE_AS_ReqExpr
//#define ProjTrace TAG_TYPE_AS_ProjTrace

#define ThreadIdExpr TAG_TYPE_AS_ThreadIdExpr
#define GuardExpr TAG_TYPE_AS_GuardExpr
#define DeclarativeSync TAG_TYPE_AS_DeclarativeSync
//#define Mutex TAG_TYPE_AS_Mutex
#define NameList TAG_TYPE_AS_NameList

#define SameClassExpr TAG_TYPE_AS_SameClassExpr
#define SameBaseClassExpr TAG_TYPE_AS_SameBaseClassExpr

#define TimeVarDecl TAG_TYPE_AS_TimeVarDecl
#define Assumption TAG_TYPE_AS_Assumption
#define Effect TAG_TYPE_AS_Effect

#ifdef VICE
#define DurationStmt TAG_TYPE_AS_DurationStmt
#define CurTimeExpr TAG_TYPE_AS_CurTimeExpr
#define CycleStmt TAG_TYPE_AS_CycleStmt
#endif //VICE

/*  Record tags for static semantics */

#define InvTypeRep TAG_TYPE_REP_InvTypeRep
#define BasicTypeRep TAG_TYPE_REP_BasicTypeRep
#define QuoteTypeRep TAG_TYPE_REP_QuoteTypeRep
#define CompositeTypeRep TAG_TYPE_REP_CompositeTypeRep
#define FieldRep TAG_TYPE_REP_FieldRep
#define UnionTypeRep TAG_TYPE_REP_UnionTypeRep
#define ProductTypeRep TAG_TYPE_REP_ProductTypeRep
#define SetTypeRep TAG_TYPE_REP_SetTypeRep
#define SeqTypeRep TAG_TYPE_REP_SeqTypeRep
#define Seq0TypeRep TAG_TYPE_REP_Seq0TypeRep
#define Seq1TypeRep TAG_TYPE_REP_Seq1TypeRep
#define MapTypeRep TAG_TYPE_REP_MapTypeRep
#define GeneralMapTypeRep TAG_TYPE_REP_GeneralMapTypeRep
#define InjectiveMapTypeRep TAG_TYPE_REP_InjectiveMapTypeRep
#define TypeNameRep TAG_TYPE_REP_TypeNameRep
#define FnTypeRep TAG_TYPE_REP_FnTypeRep
#define PartialFnTypeRep TAG_TYPE_REP_PartialFnTypeRep
#define TotalFnTypeRep TAG_TYPE_REP_TotalFnTypeRep
#define DiscretionaryTypeRep TAG_TYPE_REP_DiscretionaryTypeRep
#define ObjRefTypeRep TAG_TYPE_REP_ObjRefTypeRep
#define AllTypeRep TAG_TYPE_REP_AllTypeRep
#define UnitTypeRep TAG_TYPE_REP_UnitTypeRep
#define RetTypeRep TAG_TYPE_REP_RetTypeRep
#define NilTypeRep TAG_TYPE_REP_NilTypeRep
#define EmptySetTypeRep TAG_TYPE_REP_EmptySetTypeRep
#define EmptySeqTypeRep TAG_TYPE_REP_EmptySeqTypeRep
#define EmptyMapTypeRep TAG_TYPE_REP_EmptyMapTypeRep
#define MethodTypeRep TAG_TYPE_REP_MethodTypeRep
#define PrelimMethodTypeRep TAG_TYPE_REP_PrelimMethodTypeRep
#define FullMethodTypeRep TAG_TYPE_REP_FullMethodTypeRep
#define OpTypeRep TAG_TYPE_REP_OpTypeRep
#define TypeVarRep TAG_TYPE_REP_TypeVarRep
#define TypeParRep TAG_TYPE_REP_TypeParRep
#define PolyTypeRep TAG_TYPE_REP_PolyTypeRep
#define TmpTypeRep TAG_TYPE_REP_TmpTypeRep
#define ExitTypeRep TAG_TYPE_REP_ExitTypeRep

//////////////////////////////////////////////
// The ENV module for the static semantic modules
//////////////////////////////////////////////

#define TypeRepElem             TAG_TYPE_SSENV_TypeRepElem
#define TagRepElem              TAG_TYPE_SSENV_TagRepElem
#define TagRepEnv               TAG_TYPE_SSENV_TagRepEnv
#define NameInfo                TAG_TYPE_SSENV_NameInfo
#define ParseTypeInfo           TAG_TYPE_SSENV_ParseTypeInfo
#define ModuleRep               TAG_TYPE_SSENV_ModuleRep
#define ParSigRep               TAG_TYPE_SSENV_ParSigRep
#define ExpSigRep               TAG_TYPE_SSENV_ExpSigRep
//#define PolyTypeRep             TAG_TYPE_SSENV_PolyTypeRep
#define PolyTypeRepElem         TAG_TYPE_SSENV_PolyTypeRepElem
//#define TmpTypeRep              TAG_TYPE_SSENV_TmpTypeRep
//#define ExitTypeRep             TAG_TYPE_SSENV_ExitTypeRep
#ifdef VDMPP
#define AccessTypeRep           TAG_TYPE_SSENV_AccessTypeRep      
#define AccessFnTypeRep         TAG_TYPE_SSENV_AccessFnTypeRep    
#define AccessPolyTypeRep       TAG_TYPE_SSENV_AccessPolyTypeRep  
#define AccessOpTypeRep         TAG_TYPE_SSENV_AccessOpTypeRep    
#define AccessFieldRep          TAG_TYPE_SSENV_AccessFieldRep     
#endif // VDMPP

#endif // INCL_TAG_H
