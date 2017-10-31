/***
*  * WHAT
*  *   Record tags and quote definitions for module CPP
*  *   and record tags for modules CG and AUX.
*  *   Quotes for module MD.
*  * Implementation of
*  *   mod_aux.vdm    r1.48
*  *   mod_md.vdm     r1.13
*  *   mod_cppast.vdm r1.27
*  * 
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/cgtag.h,v $
*  * VERSION
*  *    $Revision: 1.15 $
*  * DATE
*  *    $Date: 2001/07/13 15:04:48 $
*  * STATUS
*  *    $State: Exp $
*  * REFERENCES
*  *    
*  * PROJECT
*  *    IDERS/AFRODITE
*  * AUTHOR
*  *    Henrik Voss + $Author: paulm $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __cgtag_h__
#define __cgtag_h__

#include "main_tag.h"
#include "CPP.h"
#include "CGAUX.h"
#include "CGMAIN.h"

// Record tags for abstract syntax for C++ (module CPP)

enum {
  AccCompleteClassSpecifier_CPP = TAG_TYPE_CPP_AccCompleteClassSpecifier,
  All_CPP = TAG_TYPE_CPP_All,
  AllocationNewTypeExpr_CPP = TAG_TYPE_CPP_AllocationNewTypeExpr,
  AllocationTypeExpr_CPP = TAG_TYPE_CPP_AllocationTypeExpr,
  ArgAbsDecl_CPP = TAG_TYPE_CPP_ArgAbsDecl,
  ArgDecl_CPP = TAG_TYPE_CPP_ArgDecl,
  ArgumentDeclarationList_CPP = TAG_TYPE_CPP_ArgumentDeclarationList,
  ArrayAbsDecl_CPP = TAG_TYPE_CPP_ArrayAbsDecl,
  ArrayApply_CPP = TAG_TYPE_CPP_ArrayApply,
  ArrayDecl_CPP = TAG_TYPE_CPP_ArrayDecl,
  ArrayNewDecl_CPP = TAG_TYPE_CPP_ArrayNewDecl,
  AsgnInit_CPP = TAG_TYPE_CPP_AsgnInit,
  AssignExpr_CPP = TAG_TYPE_CPP_AssignExpr,
  AssignOp_CPP = TAG_TYPE_CPP_AssignOp,
  BinExpr_CPP = TAG_TYPE_CPP_BinExpr,
  BinOp_CPP = TAG_TYPE_CPP_BinOp,
  BitExpr_CPP = TAG_TYPE_CPP_BitExpr,
  BitOp_CPP = TAG_TYPE_CPP_BitOp,
  Bool_CPP = TAG_TYPE_CPP_Bool,
  BoolLit_CPP = TAG_TYPE_CPP_BoolLit,
  BracketedAbsDecl_CPP = TAG_TYPE_CPP_BracketedAbsDecl,
  BracketedDecl_CPP = TAG_TYPE_CPP_BracketedDecl,
  BracketedExpr_CPP = TAG_TYPE_CPP_BracketedExpr,
  Break_CPP = TAG_TYPE_CPP_Break,
  CaseStmt_CPP = TAG_TYPE_CPP_CaseStmt,
  CastExpr_CPP = TAG_TYPE_CPP_CastExpr,
  CharacterLit_CPP = TAG_TYPE_CPP_CharacterLit,
  Char_CPP = TAG_TYPE_CPP_Char,
  ClassHead_CPP = TAG_TYPE_CPP_ClassHead,
  ClassInstanceCreationExpr_CPP = TAG_TYPE_CPP_ClassInstanceCreationExpr,
  ClassNewDecl_CPP = TAG_TYPE_CPP_ClassNewDecl,
//  ClassResScopeQualifiedClassName_CPP = TAG_TYPE_CPP_ClassResScopeQualifiedClassName,
  ClassSpecifier_CPP = TAG_TYPE_CPP_ClassSpecifier,
  CompoundStmt_CPP = TAG_TYPE_CPP_CompoundStmt,
  ConditionalExpr_CPP = TAG_TYPE_CPP_ConditionalExpr,
  Continue_CPP = TAG_TYPE_CPP_Continue,
#ifdef VDMPP
  ConversionFunctionName_CPP = TAG_TYPE_CPP_ConversionFunctionName,
  ConversionTypeName_CPP = TAG_TYPE_CPP_ConversionTypeName,
#endif // VDMPP
  CVNewDeclarator_CPP = TAG_TYPE_CPP_CVNewDeclarator,
  DeallocationArrayExpr_CPP = TAG_TYPE_CPP_DeallocationArrayExpr,
  DeallocationExpr_CPP = TAG_TYPE_CPP_DeallocationExpr,
  DeclarationStmt_CPP = TAG_TYPE_CPP_DeclarationStmt,
  DeclPureSpec_CPP = TAG_TYPE_CPP_DeclPureSpec,
  DefaultArgAbsDecl_CPP = TAG_TYPE_CPP_DefaultArgAbsDecl,
  DefaultArgDecl_CPP = TAG_TYPE_CPP_DefaultArgDecl,
  DefaultStmt_CPP = TAG_TYPE_CPP_DefaultStmt,
  Destructor_CPP = TAG_TYPE_CPP_Destructor,
  Double_CPP = TAG_TYPE_CPP_Double,
  DoWhileStmt_CPP = TAG_TYPE_CPP_DoWhileStmt,
  ElaboratedTypeSpecifier_CPP = TAG_TYPE_CPP_ElaboratedTypeSpecifier,
  EmptyStmt_CPP = TAG_TYPE_CPP_EmptyStmt,
  EnumDefaultAssgn_CPP = TAG_TYPE_CPP_EnumDefaultAssgn,
  EnumInit_CPP = TAG_TYPE_CPP_EnumInit,
  EnumName_CPP = TAG_TYPE_CPP_EnumName,
  EnumSpecifier_CPP = TAG_TYPE_CPP_EnumSpecifier,
  EqOp_CPP = TAG_TYPE_CPP_EqOp,
  EqualityExpr_CPP = TAG_TYPE_CPP_EqualityExpr,
  ExceptionDeclaration_CPP = TAG_TYPE_CPP_ExceptionDeclaration,
  ExplTypeConv_CPP = TAG_TYPE_CPP_ExplTypeConv,
  ExpressionStmt_CPP = TAG_TYPE_CPP_ExpressionStmt,
  FctAbsDecl_CPP = TAG_TYPE_CPP_FctAbsDecl,
  FctBody_CPP = TAG_TYPE_CPP_FctBody,
  FctCall_CPP = TAG_TYPE_CPP_FctCall,
  FctDecl_CPP = TAG_TYPE_CPP_FctDecl,
  FctMacroDef_CPP = TAG_TYPE_CPP_FctMacroDef,
  FctSpecifier_CPP = TAG_TYPE_CPP_FctSpecifier,
  File_CPP = TAG_TYPE_CPP_File,
  Float_CPP = TAG_TYPE_CPP_Float,
  FloatingLit_CPP = TAG_TYPE_CPP_FloatingLit,
  ForStmt_CPP = TAG_TYPE_CPP_ForStmt,
  Friend_CPP = TAG_TYPE_CPP_Friend,
  FunctionDefinition_CPP = TAG_TYPE_CPP_FunctionDefinition,
  Goto_CPP = TAG_TYPE_CPP_Goto,
  Handler_CPP = TAG_TYPE_CPP_Handler,
  IdentDeclaration_CPP = TAG_TYPE_CPP_IdentDeclaration,
  Identifier_CPP = TAG_TYPE_CPP_Identifier,
  IdMacroDef_CPP = TAG_TYPE_CPP_IdMacroDef,
  IfStmt_CPP = TAG_TYPE_CPP_IfStmt,
  IncludeStmt_CPP = TAG_TYPE_CPP_IncludeStmt,
  IndirectionAbsDecl_CPP = TAG_TYPE_CPP_IndirectionAbsDecl,
  IndirectionDecl_CPP = TAG_TYPE_CPP_IndirectionDecl,
  InitDeclarator_CPP = TAG_TYPE_CPP_InitDeclarator,
  InitializerList_CPP = TAG_TYPE_CPP_InitializerList,
  Int_CPP = TAG_TYPE_CPP_Int,
  IntegerLit_CPP = TAG_TYPE_CPP_IntegerLit,
  InterfaceHead_CPP = TAG_TYPE_CPP_InterfaceHead,
  InterfaceSpecifier_CPP = TAG_TYPE_CPP_InterfaceSpecifier,
  LabelStmt_CPP = TAG_TYPE_CPP_LabelStmt,
  LogicalExpr_CPP = TAG_TYPE_CPP_LogicalExpr,
  LogOp_CPP = TAG_TYPE_CPP_LogOp,
  Long_CPP = TAG_TYPE_CPP_Long,
  MemberList_CPP = TAG_TYPE_CPP_MemberList,
  MemberSpecifier_CPP = TAG_TYPE_CPP_MemberSpecifier,
  MemInitializer_CPP = TAG_TYPE_CPP_MemInitializer,
  Modifier_CPP = TAG_TYPE_CPP_Modifier,
  NewInitializer_CPP = TAG_TYPE_CPP_NewInitializer,
  NewTypeName_CPP = TAG_TYPE_CPP_NewTypeName,
  NullLit_CPP = TAG_TYPE_CPP_NullLit,
  ObjectInit_CPP = TAG_TYPE_CPP_ObjectInit,
  ObjectMemberAccess_CPP = TAG_TYPE_CPP_ObjectMemberAccess,
#ifdef VDMPP
  Operator_CPP = TAG_TYPE_CPP_Operator,
  OperatorFunctionName_CPP = TAG_TYPE_CPP_OperatorFunctionName,
#endif // VDMPP
  PackageAndImportDeclarations_CPP = TAG_TYPE_CPP_PackageAndImportDeclarations,
  PackageDeclaration_CPP = TAG_TYPE_CPP_PackageDeclaration,
  PmExpr_CPP = TAG_TYPE_CPP_PmExpr,
  PmOp_CPP = TAG_TYPE_CPP_PmOp,
#ifdef VDMPP
  PointerDecl_CPP = TAG_TYPE_CPP_PointerDecl,
#endif // VDMPP
  PointerToMemberAbs_CPP = TAG_TYPE_CPP_PointerToMemberAbs,
  PointerToMember_CPP = TAG_TYPE_CPP_PointerToMember,
#ifdef VDMPP
  PointerToMemberDecl_CPP = TAG_TYPE_CPP_PointerToMemberDecl,
#endif // VDMPP
  PointerToObjectMemberAccess_CPP = TAG_TYPE_CPP_PointerToObjectMemberAccess,
  PostFixInDecrementExpr_CPP = TAG_TYPE_CPP_PostFixInDecrementExpr,
  PreDefine_CPP = TAG_TYPE_CPP_PreDefine,
  PreElse_CPP = TAG_TYPE_CPP_PreElse,
  PreEndIf_CPP = TAG_TYPE_CPP_PreEndIf,
  PreIf_CPP = TAG_TYPE_CPP_PreIf,
  PreIfDef_CPP = TAG_TYPE_CPP_PreIfDef,
  PreIfNotDef_CPP = TAG_TYPE_CPP_PreIfNotDef,
  PreInDecrementExpr_CPP = TAG_TYPE_CPP_PreInDecrementExpr,
  PreMacro_CPP = TAG_TYPE_CPP_PreMacro,
  QualifiedName_CPP = TAG_TYPE_CPP_QualifiedName,
  QualifiedPackageName_CPP = TAG_TYPE_CPP_QualifiedPackageName,
  QuoteIncl_CPP = TAG_TYPE_CPP_QuoteIncl,
  RefAbsDecl_CPP = TAG_TYPE_CPP_RefAbsDecl,
  RefDecl_CPP = TAG_TYPE_CPP_RefDecl,
#ifdef VDMPP
  RefTypeDecl_CPP = TAG_TYPE_CPP_RefTypeDecl,
#endif // VDMPP
  RelationalExpr_CPP = TAG_TYPE_CPP_RelationalExpr,
  RelOp_CPP = TAG_TYPE_CPP_RelOp,
  ResScopeQualifiedClassName_CPP = TAG_TYPE_CPP_ResScopeQualifiedClassName,
  Return_CPP = TAG_TYPE_CPP_Return,
//  ScopeResIdentifier_CPP = TAG_TYPE_CPP_ScopeResIdentifier,
//  ScopeResOpFctName_CPP = TAG_TYPE_CPP_ScopeResOpFctName,
//  ScopeResQualifiedName_CPP = TAG_TYPE_CPP_ScopeResQualifiedName,
  ShiftExpr_CPP = TAG_TYPE_CPP_ShiftExpr,
  ShOp_CPP = TAG_TYPE_CPP_ShOp,
  Short_CPP = TAG_TYPE_CPP_Short,
  Signed_CPP = TAG_TYPE_CPP_Signed,
  SimplePackageName_CPP = TAG_TYPE_CPP_SimplePackageName,
  SingleTypeImportDeclaration_CPP = TAG_TYPE_CPP_SingleTypeImportDeclaration,
  SizeOfExpr_CPP = TAG_TYPE_CPP_SizeOfExpr,
  SizeOfType_CPP = TAG_TYPE_CPP_SizeOfType,
  SquareIncl_CPP = TAG_TYPE_CPP_SquareIncl,
  StaticInitializer_CPP = TAG_TYPE_CPP_StaticInitializer,
  StorageClassSpecifier_CPP = TAG_TYPE_CPP_StorageClassSpecifier,
  StringLit_CPP = TAG_TYPE_CPP_StringLit,
#ifdef VDMPP
  Super_CPP = TAG_TYPE_CPP_Super,
#endif // VDMPP
  SwitchBlock_CPP = TAG_TYPE_CPP_SwitchBlock,
  SwitchBlockStmtGrp_CPP = TAG_TYPE_CPP_SwitchBlockStmtGrp,
  SwitchLabels_CPP = TAG_TYPE_CPP_SwitchLabels,
  SwitchStmt_CPP = TAG_TYPE_CPP_SwitchStmt,
  SynchronizedStmt_CPP = TAG_TYPE_CPP_SynchronizedStmt,
  TcOp_CPP = TAG_TYPE_CPP_TcOp,
  TemplateClassName_CPP = TAG_TYPE_CPP_TemplateClassName,
  TemplateName_CPP = TAG_TYPE_CPP_TemplateName,
  This_CPP = TAG_TYPE_CPP_This,
  ThrowExpression_CPP = TAG_TYPE_CPP_ThrowExpression,
  TryBlock_CPP = TAG_TYPE_CPP_TryBlock,
  TypeCompExpr_CPP = TAG_TYPE_CPP_TypeCompExpr,
  TypeDef_CPP = TAG_TYPE_CPP_TypeDef,
  TypeDefName_CPP = TAG_TYPE_CPP_TypeDefName,
  TypeImportOnDemandDeclaration_CPP = TAG_TYPE_CPP_TypeImportOnDemandDeclaration,
  TypeName_CPP = TAG_TYPE_CPP_TypeName,
  TypeSpecifier_CPP = TAG_TYPE_CPP_TypeSpecifier,
  UnaryOp_CPP = TAG_TYPE_CPP_UnaryOp,
  UnaryOpExpr_CPP = TAG_TYPE_CPP_UnaryOpExpr,
  Unsigned_CPP = TAG_TYPE_CPP_Unsigned,
  VirCompleteClassSpecifier_CPP = TAG_TYPE_CPP_VirCompleteClassSpecifier,
  Void_CPP = TAG_TYPE_CPP_Void,
  WCharacterLit_CPP = TAG_TYPE_CPP_WCharacterLit,
  WhileStmt_CPP = TAG_TYPE_CPP_WhileStmt,
  WStringLit_CPP = TAG_TYPE_CPP_WStringLit,

  BlockComments_CPP = TAG_TYPE_CPP_BlockComments,
  SingleLineComments_CPP = TAG_TYPE_CPP_SingleLineComments,
  TrailingComments_CPP = TAG_TYPE_CPP_TrailingComments,
  EndOfLineComments_CPP = TAG_TYPE_CPP_EndOfLineComments,
  DocComments_CPP = TAG_TYPE_CPP_DocComments,

// Quote definitions

// AssignOp

  ASEQUAL_CPP = TAG_quote_ASEQUAL,
  ASMULT_CPP = TAG_quote_ASMULT,
  ASDIV_CPP = TAG_quote_ASDIV,
  ASMOD_CPP = TAG_quote_ASMOD,
  ASPLUS_CPP = TAG_quote_ASPLUS,
  ASMINUS_CPP = TAG_quote_ASMINUS,
  ASRIGHTSHIFT_CPP = TAG_quote_ASRIGHTSHIFT,
  ASLEFTSHIFT_CPP = TAG_quote_ASLEFTSHIFT,
  ASBITWISEAND_CPP = TAG_quote_ASBITWISEAND,
  ASBITWISEEXOR_CPP = TAG_quote_ASBITWISEEXOR,
  ASBITWISEINCLOR_CPP = TAG_quote_ASBITWISEINCLOR,

// LogOp

  OR_CPP = TAG_quote_OR,
  AND_CPP = TAG_quote_AND,

// BitOp

  EXCLOR_CPP = TAG_quote_EXCLOR,
  BITAND_CPP = TAG_quote_BITAND,
  BITOR_CPP = TAG_quote_BITOR,

// EqOp

  EQ_CPP = TAG_quote_EQ,
  NEQ_CPP = TAG_quote_NEQ,

// RelOp

  LT_CPP = TAG_quote_LT,
  GT_CPP = TAG_quote_GT,
  LEQ_CPP = TAG_quote_LEQ,
  GEQ_CPP = TAG_quote_GEQ,

//TypeComparison in Java

  ISINSTANCEOF_CPP = TAG_quote_ISINSTANCEOF,


// ShOp

  LEFTSHIFT_CPP = TAG_quote_LEFTSHIFT,
  RIGHTSHIFT_CPP = TAG_quote_RIGHTSHIFT,

// BinOp

  MULT_CPP = TAG_quote_MULT,
  MOD_CPP = TAG_quote_MOD,

// PmOp

  DOTSTAR_CPP = TAG_quote_DOTSTAR,

// IncDecOp

  PPLUS_CPP = TAG_quote_PPLUS,
  PMINUS_CPP = TAG_quote_PMINUS,

// UnaryOp

  INDIRECTION_CPP = TAG_quote_INDIRECTION,
  ADDROP_CPP = TAG_quote_ADDROP,
  NEG_CPP = TAG_quote_NEG,
  COMPL_CPP = TAG_quote_COMPL,
  REFERENCE_CPP = TAG_quote_REFERENCE,

// Operator

#ifdef VDMPP
  NEW_CPP = TAG_quote_NEW,
  DELETE_CPP = TAG_quote_DELETE,
  PLUS_CPP = TAG_quote_PLUS,
  MINUS_CPP = TAG_quote_MINUS,
  STAR_CPP = TAG_quote_STAR,
  DIV_CPP = TAG_quote_DIV,
  PRECENTAGEMARK_CPP = TAG_quote_PRECENTAGEMARK,
  CIRCUMFEX_CPP = TAG_quote_CIRCUMFEX,
  AMPERSAND_CPP = TAG_quote_AMPERSAND,
  VERTICALLINE_CPP = TAG_quote_VERTICALLINE,
  TILDE_CPP = TAG_quote_TILDE,
  EXCLAMATIONMARK_CPP = TAG_quote_EXCLAMATIONMARK,
  OPEQUAL_CPP = TAG_quote_OPEQUAL,
  LESS_CPP = TAG_quote_LESS,
  GREATER_CPP = TAG_quote_GREATER,
  PLUSEQUAL_CPP = TAG_quote_PLUSEQUAL,
  MINUSEQUAL_CPP = TAG_quote_MINUSEQUAL,
  STAREQUAL_CPP = TAG_quote_STAREQUAL,
  BACKSLASHEQUAL_CPP = TAG_quote_BACKSLASHEQUAL,
  PERCENTAGEMARKEQUAL_CPP = TAG_quote_PERCENTAGEMARKEQUAL,
  CIRCUMFLEXEQUAL_CPP = TAG_quote_CIRCUMFLEXEQUAL,
  ANDEQUAL_CPP = TAG_quote_ANDEQUAL,
  VERTICALLINEEQUAL_CPP = TAG_quote_VERTICALLINEEQUAL,
  DOUBLELESS_CPP = TAG_quote_DOUBLELESS,
  DOUBLEGREATER_CPP = TAG_quote_DOUBLEGREATER,
  DOUBLELESSEQUAL_CPP = TAG_quote_DOUBLELESSEQUAL,
  DOUBLEGREATEREQUAL_CPP = TAG_quote_DOUBLEGREATEREQUAL,
  DOUBLEEQUAL_CPP = TAG_quote_DOUBLEEQUAL,
  EXCLAMATIONMARKEQUAL_CPP = TAG_quote_EXCLAMATIONMARKEQUAL,
  LESSEQUAL_CPP = TAG_quote_LESSEQUAL,
  GREATEREQUAL_CPP = TAG_quote_GREATEREQUAL,
  DOUBLEAND_CPP = TAG_quote_DOUBLEAND,
  DOUBLEVERTICALLINE_CPP = TAG_quote_DOUBLEVERTICALLINE,
  DOUBLEPLUS_CPP = TAG_quote_DOUBLEPLUS,
  DOUBLEMINUS_CPP = TAG_quote_DOUBLEMINUS,
  COMMA_CPP = TAG_quote_COMMA,
  ARROWSTAR_CPP = TAG_quote_ARROWSTAR,
  ARROW_CPP = TAG_quote_ARROW,
  BRACKETS_CPP = TAG_quote_BRACKETS,
  SQUAREBRACKETS_CPP = TAG_quote_SQUAREBRACKETS,
#endif // VDMPP
  JAVA_CPP = TAG_quote_JAVA,
  CPP_CPP = TAG_quote_CPP,
// cv-qualitier

  CONST_CPP = TAG_quote_CONST,
  VOLATILE_CPP = TAG_quote_VOLATILE,
// ellipsis

  ELLIPSIS_CPP = TAG_quote_ELLIPSIS,

  CLASS_CPP = TAG_quote_CLASS,
  STRUCT_CPP = TAG_quote_STRUCT,
  UNION_CPP = TAG_quote_UNION,
  PRIVATE_CPP = TAG_quote_PRIVATE,
  PROTECTED_CPP = TAG_quote_PROTECTED,
  PUBLIC_CPP = TAG_quote_PUBLIC,
  ABSTRACT_CPP = TAG_quote_ABSTRACT,
  FINAL_CPP = TAG_quote_FINAL,

  INLINE_CPP = TAG_quote_INLINE,
  VIRTUAL_CPP = TAG_quote_VIRTUAL,
  AUTO_CPP = TAG_quote_AUTO,
  REGISTER_CPP = TAG_quote_REGISTER,
  STATIC_CPP = TAG_quote_STATIC,
  EXTERN_CPP = TAG_quote_EXTERN,

// Record tags for module CG

  VT_CG = TAG_TYPE_CGMAIN_VT,
//  VT_CG = TAG_CGMAIN + 1,

// Record tags for module AUX

  SortFct_AUX = TAG_TYPE_CGAUX_SortFct,
  AuxFct_AUX = TAG_TYPE_CGAUX_AuxFct
//  SortFct_AUX = TAG_CGAUX + 1,
//  AuxFct_AUX = TAG_CGAUX + 2,
};

#endif // __cgtag_h_ _
