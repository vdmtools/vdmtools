/***
*  * WHAT
*  *    Implementation of mod_stmt.vdm and ppstmt.vdm
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/mod_stmt.h,v $
*  * VERSION
*  *    $Revision: 1.19 $
*  * DATE
*  *    $Date: 2006/04/14 08:14:40 $
*  * STATUS
*  *    Under development
*  * REFERENCES
*  *    
*  * PROJECT
*  *    AFRODITE
*  * AUTHOR
*  *    Henrik Voss + $Author: vdmtools $
*  *
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __mod_stmt_h__
#define __mod_stmt_h__

SEQ<TYPE_CPP_Stmt> GenStmt(const TYPE_AS_Stmt &, bool);
SEQ<TYPE_CPP_Stmt> GenNotSupported(wstring, const TYPE_AS_Stmt &, bool);
SEQ<TYPE_CPP_Stmt> CGAssignStmt(const TYPE_AS_AssignStmt &);
SEQ<TYPE_CPP_Stmt> CGAtomicAssignStmt(const TYPE_AS_AtomicAssignStmt &, bool);
SEQ<TYPE_CPP_Stmt> CGStateDesignator(const TYPE_AS_StateDesignator &, const TYPE_CPP_Expr &);
Tuple  CGSDUpperLevel(const TYPE_AS_StateDesignator &, const TYPE_CPP_Name &);
Tuple  CGSDMapOrSeqRef_Name(const TYPE_AS_Name &, const TYPE_AS_Expr &, const TYPE_CPP_Name &);
Tuple  CGSDMapOrSeqRef_SD(const TYPE_AS_StateDesignator &, const TYPE_AS_Expr &,
                          const TYPE_CPP_Name &, const TYPE_CPP_Name &);
Tuple  CGSDFieldRef_Name(const TYPE_AS_Name &, const TYPE_AS_Expr &, const TYPE_CPP_Name &);
Tuple  CGSDFieldRef_SD(const TYPE_AS_StateDesignator &, const TYPE_AS_Expr &,
                       const TYPE_CPP_Name &, const TYPE_CPP_Name &);
SEQ<TYPE_CPP_Stmt> GenRecImpModify(const TYPE_AS_Name &, const TYPE_CGMAIN_VT &, const TYPE_CPP_Expr & );
Tuple  GenCommonAsgnStmt(const TYPE_AS_StateDesignator &, const TYPE_AS_Expr &);
Tuple  CGCommonMapOverrideExpr(const TYPE_AS_StateDesignator &, const TYPE_AS_SeqModifyMapOverrideExpr &);
Tuple  CGCommonAsgnSeqConc(const TYPE_AS_StateDesignator &, const TYPE_AS_Expr &);
Tuple  CGCommonAsgnSetUnion(const TYPE_AS_StateDesignator &, const TYPE_AS_Expr &);
SEQ<TYPE_CPP_Stmt> CGReturnStmt(const TYPE_AS_ReturnStmt &);
SEQ<TYPE_CPP_Stmt> CGBlockStmt(const TYPE_AS_BlockStmt &, bool);
SEQ<TYPE_CPP_Stmt> CGLetStmt(const TYPE_AS_LetStmt &, bool);
SEQ<TYPE_CPP_Stmt> CGLetBeSTStmt(const TYPE_AS_LetBeSTStmt &, bool);
SEQ<TYPE_CPP_Stmt> CGDefStmt(const TYPE_AS_DefStmt &, bool);
SEQ<TYPE_CPP_Stmt> CGWhileLoopStmt(const TYPE_AS_WhileLoopStmt &, bool);
SEQ<TYPE_CPP_Stmt> CGSeqForLoopStmt(const TYPE_AS_SeqForLoopStmt &, bool);
Tuple DeclareVars(const TYPE_AS_PatternBind &);
SEQ<TYPE_CPP_Stmt> CGSetForLoopStmt(const TYPE_AS_SetForLoopStmt &, bool);
SEQ<TYPE_CPP_Stmt> CGIndexedForLoopStmt(const TYPE_AS_IndexForLoopStmt &, bool);
Tuple GenBoundAndValue(const SEQ<Char> &, const TYPE_AS_Expr &);
Tuple GenByPart(const Generic &, const TYPE_CPP_Name &, const TYPE_CPP_Expr &);
SEQ<TYPE_CPP_Stmt> CGCasesStmt(const TYPE_AS_CasesStmt &, bool);
SEQ<TYPE_CPP_Stmt> CGCasesStmtAltn(const SEQ<TYPE_AS_CasesStmtAltn> &,
                                   const TYPE_CGMAIN_VT &,
                                   const TYPE_CPP_Name &,
                                   const Generic &, bool);
TYPE_CPP_Stmt CGAltnStmt(const TYPE_AS_CasesStmtAltn &, const TYPE_CGMAIN_VT &,
                         const TYPE_CPP_Name &, const Generic &, bool);
SEQ<TYPE_CPP_Stmt> CGIdentStmt();
SEQ<TYPE_CPP_Stmt> CGIfStmt(const TYPE_AS_IfStmt &, bool);
TYPE_CPP_Stmt GenIfConsCPPStmt(const SEQ<TYPE_CPP_Stmt> &);
bool IsReturnStmtAtLast (const TYPE_CPP_Stmt & stmt);
SEQ<TYPE_CPP_Stmt> CGNonDetStmt(const TYPE_AS_NonDetStmt &, bool);
SEQ<TYPE_CPP_Stmt> CGCallStmt(const TYPE_AS_CallStmt &);
SEQ<TYPE_CPP_Stmt> CGErrorStmt();
SEQ<TYPE_CPP_Stmt> GenSpecTopStmt(const TYPE_AS_SpecificationStmt &);
SEQ<TYPE_CPP_Stmt> CGAlwaysStmt(const TYPE_AS_AlwaysStmt&, bool);
SEQ<TYPE_CPP_Stmt> CGExitStmt(const TYPE_AS_ExitStmt&);
Tuple ConvertBind(const TYPE_AS_PatternBind & match, const TYPE_AS_Name & name, const TYPE_CPP_Expr & excValNm);
SEQ<TYPE_CPP_Stmt> CGRecTrapStmt(const TYPE_AS_RecTrapStmt&, bool);
bool FlowBrokenSwitchLabels(const Generic&);
bool FlowBroken(const TYPE_CPP_Stmt &);
SEQ<TYPE_CPP_Stmt> GenRecTrapTrap(const TYPE_AS_Stmt&, bool);
TYPE_CPP_Handler GenRecTrapHandler(const TYPE_CPP_Identifier&, const TYPE_CPP_Identifier&);
TYPE_CPP_Stmt GenRecTrapFinally(const TYPE_CPP_Identifier&,
                                const TYPE_CPP_Identifier&,
                                const TYPE_CPP_Identifier&,
                                bool);
Tuple ConvertHandler(const TYPE_AS_Bind & bind, const TYPE_AS_Stmt & stmt);
SEQ<TYPE_CPP_Stmt> CGTrapStmt(const TYPE_AS_TrapStmt&, bool);
#ifdef VDMPP
TYPE_CPP_Stmt GenInvCall(const TYPE_AS_Name & nm, bool stat);
#endif // VDMPP
#endif // __mod_stmt_h__
