/***
*  * WHAT
*  *  This version contains:
*  *    Implementation of mod_conc.vdm  
*  *
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/mod_conc.h,v $
*  * VERSION
*  *    $Revision: 1.6 $
*  * DATE
*  *    $Date: 2006/04/13 03:13:11 $
*  * STATUS
*  *    Under development
*  * REFERENCES
*  *    
*  * PROJECT
*  *    INFORMA
*  * AUTHOR
*  *    jojo
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __mod_conc_h__
#define __mod_conc_h__

void InitSpecState ();
void InitClassState (const TYPE_AS_Name &);
void SetDefaultCast (const TYPE_AS_Name &);
void AssignOperConstants(int, const MAP<TYPE_AS_Name, TYPE_AS_OpDef> &);
Int GetOperConst(const TYPE_AS_Name &);
void ExtractGlobalInformation(const SEQ<TYPE_AS_Class> &, const SET<TYPE_AS_Name> &);
void ScanInheritance(const SET<TYPE_AS_Name> &);
void ScanSyncDefs(const SET<TYPE_AS_Name> &);
MAP<TYPE_AS_Name,Sequence> MergePermPreds(const MAP<TYPE_AS_Name,Sequence> &,
                                          const MAP<TYPE_AS_Name,Sequence> &);
MAP<TYPE_AS_Name,Set> CreateMutexTable(const SET<TYPE_AS_Mutex> &);
MAP<TYPE_AS_Name,Set> MergeMutexTable(const MAP<TYPE_AS_Name,Set> &,
                                      const MAP<TYPE_AS_Name,Set> &);
TYPE_AS_Id ClassDefiningOper(const TYPE_AS_Name &);
SEQ<TYPE_AS_Name> AllOperations(const TYPE_AS_Name &);
TYPE_AS_Name QualifyName(const TYPE_AS_Name &);
SEQ<TYPE_AS_Name> QualifyNameSeq(const SEQ<TYPE_AS_Name> &);
SEQ<TYPE_AS_Name> SetToSeq(const SET<TYPE_AS_Name> &);
SEQ<TYPE_CPP_Stmt> GenConcOper(const TYPE_AS_Name &, const SEQ<TYPE_CPP_Stmt> &);
TYPE_CPP_Expr GenGuardExpr(const TYPE_AS_Expr &);
TYPE_CPP_Expr BuildSumCPPExpr(const SEQ<TYPE_CPP_Expr> &);
TYPE_CPP_Expr GenCastedSentinel(const TYPE_AS_Name &);
SEQ<TYPE_CPP_MemberDeclaration> GenEvaluatePP(const TYPE_AS_Name &);
TYPE_CPP_Expr BuildConjGuardExpr(const SEQ<TYPE_AS_Expr> &);
SEQ<TYPE_CPP_MemberDeclaration> GenLocalSentinel(const TYPE_AS_Name &, const TYPE_AS_Definitions &);
TYPE_AS_Name MakeNameJavaCompliant(const TYPE_AS_Name &);
TYPE_AS_Id MakeStringJavaCompliant(const TYPE_AS_Id &);
TYPE_AS_Id MakeCharJavaCompliant(const Char &);
SEQ<TYPE_CPP_PackageName> GenConcInterface(const TYPE_AS_Name &);
TYPE_CPP_MemberDeclaration GenSetSentinelMethod(const TYPE_AS_Name &);
SEQ<TYPE_CPP_FunctionDefinition> GenProceduralThread(const TYPE_AS_ThreadDef &);
SEQ<TYPE_CPP_Stmt> GenConstrExtensions(const TYPE_AS_Name &);
SEQ<TYPE_CPP_Stmt> GenPeriodicThreads();
SEQ<TYPE_CPP_FunctionDefinition> GenPeriodicStartMethod();
SEQ<TYPE_CPP_Stmt> GenStartStmt(const TYPE_AS_StartStmt &, bool);
SEQ<TYPE_CPP_Stmt> GenStartlistStmt(const TYPE_AS_StartListStmt &, bool);
SEQ<TYPE_CPP_Stmt> GenStopStmt(const TYPE_AS_StopStmt &, bool);
SEQ<TYPE_CPP_Stmt> GenStoplistStmt(const TYPE_AS_StopListStmt &, bool);
TYPE_CPP_Expr GenThreadId();

#endif // __mod_conc_h__
