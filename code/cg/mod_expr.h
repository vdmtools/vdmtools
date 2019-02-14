/***
*  * WHAT
*  *    Implementation of mod_expr.vdm r 1.124
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/mod_expr.h,v $
*  * VERSION
*  *    $Revision: 1.19 $
*  * DATE
*  *    $Date: 2001/11/09 08:23:20 $
*  * STATUS
*  *    Under development $State: Exp $
*  * PROJECT
*  *    AFRODITE
*  * AUTHOR
*  *    Poul Boegh Lassen (PBL) + $Author: paulm $
*  * COPYRIHT
*  *    (C) Kyushu University
***/

#ifndef __mod_expr_h__
#define __mod_expr_h__

SEQ<TYPE_CPP_Stmt> CGExpr(const TYPE_AS_Expr &, const TYPE_CGMAIN_VT &);
Generic  CGExpr_q(const TYPE_AS_Expr &, const TYPE_CGMAIN_VT &);
Tuple    CGExprExcl(const TYPE_AS_Expr &, const Generic &, const Generic &);
SEQ<TYPE_CPP_Stmt> CGCasesExpr(const TYPE_AS_CasesExpr &, const TYPE_CGMAIN_VT &);
SET<TYPE_AS_Name> FindAllPatIdInCasesAlt(const SEQ<Record> &);
SEQ<TYPE_CPP_Stmt> CGCasesExprAltn(const SEQ<TYPE_AS_CaseAltn> &,
                                   const TYPE_CGMAIN_VT &,
                                   const TYPE_CGMAIN_VT &,
                                   const TYPE_CPP_Name &);
TYPE_CPP_Stmt CGAltn(const TYPE_AS_CaseAltn &, const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &, const TYPE_CPP_Name &);
Tuple    SamePatternIds(const SEQ<TYPE_AS_Pattern> &);
Map      FindPatternId(const TYPE_AS_Pattern &);
Map MergePidM(const Map &, const Map &);
SEQ<TYPE_CPP_Stmt> DeclarePatterns(const Map &);
Generic CGSetEnumerationExpr(const TYPE_AS_SetEnumerationExpr &, const TYPE_CGMAIN_VT &);
Generic CGSeqEnumerationExpr(const TYPE_AS_SeqEnumerationExpr &, const TYPE_CGMAIN_VT &);
Generic CGMapEnumerationExpr(const TYPE_AS_MapEnumerationExpr &, const TYPE_CGMAIN_VT &);
SEQ<TYPE_CPP_Stmt> CGSetComprehensionExpr(const TYPE_AS_SetComprehensionExpr &, const TYPE_CGMAIN_VT &);
SEQ<TYPE_CPP_Stmt> CGMapComprehensionExpr(const TYPE_AS_MapComprehensionExpr &, const TYPE_CGMAIN_VT &);
SEQ<TYPE_CPP_Stmt> CGSeqComprehensionExpr(const TYPE_AS_SeqComprehensionExpr &, const TYPE_CGMAIN_VT &);
SEQ<TYPE_CPP_Stmt> CGSeqComprehensionIndexLoop(const TYPE_AS_SeqComprehensionExpr &,
                                               const TYPE_CGMAIN_VT &);
SEQ<TYPE_CPP_Stmt> CGSeqComprehensionSetBind(const TYPE_AS_SeqComprehensionExpr &,
                                             const TYPE_CGMAIN_VT &);
SEQ<TYPE_CPP_Stmt> CGSeqComprehensionSeqBind(const TYPE_AS_SeqComprehensionExpr &,
                                             const TYPE_CGMAIN_VT &);
SEQ<TYPE_CPP_Stmt> CGSetRangeExpr(const TYPE_AS_SetRangeExpr &, const TYPE_CGMAIN_VT &);
SEQ<TYPE_CPP_Stmt> CGSubSeqExpr(const TYPE_AS_SubSequenceExpr &, const TYPE_CGMAIN_VT &);
Generic CGTupleConstructorExpr(const TYPE_AS_TupleConstructorExpr &, const TYPE_CGMAIN_VT &);
Generic CGRecordConstructorExpr(const TYPE_AS_RecordConstructorExpr &, const TYPE_CGMAIN_VT &);
Generic CGTokenConstExpr(const TYPE_AS_TokenConstructorExpr &, const TYPE_CGMAIN_VT &);
Generic CGTypeJudgementExpr(const TYPE_AS_TypeJudgementExpr &, const TYPE_CGMAIN_VT &);
Generic ConvertTypeJudgementExprAS(const TYPE_AS_Expr & expr, const TYPE_AS_Type & tp);
Generic CGNarrowExpr(const TYPE_AS_NarrowExpr &, const TYPE_CGMAIN_VT &);
Generic CGMacro(const TYPE_AS_Macro &, const TYPE_CGMAIN_VT &);
Generic CGTupleSelectExpr(const TYPE_AS_TupleSelectExpr &, const TYPE_CGMAIN_VT &);
Generic CGApplyExpr(const TYPE_AS_ApplyExpr &, const TYPE_CGMAIN_VT &, const Generic &, const Generic &, bool);
Generic CGMapApplyExpr(const TYPE_AS_ApplyExpr &, const TYPE_CGMAIN_VT &, const Generic &, const Generic &, bool);
Generic CGSeqApplyExpr(const TYPE_AS_ApplyExpr &, const TYPE_CGMAIN_VT &, const Generic &, const Generic &);
TYPE_CPP_Expr GenIsNat(const TYPE_CPP_Name &);
Generic CGFctApplyExpr(const TYPE_AS_ApplyExpr &, const TYPE_CGMAIN_VT &);
SEQ<TYPE_CPP_Stmt> CGUnionTypeApply (const TYPE_AS_ApplyExpr &,
                                     const TYPE_CGMAIN_VT &,
                                     const Generic &,
                                     const Generic &, bool);
TYPE_CPP_Stmt GenUTMapApply(const Generic &,
                            const TYPE_CPP_Name &,
                            const TYPE_CPP_Name &,
                            const TYPE_CPP_Expr &,
                            const TYPE_CPP_Stmt &, bool,
                            const TYPE_CPP_Name &);
Tuple    FindPosMapSeqComb(const TYPE_REP_UnionTypeRep &);
SEQ<TYPE_CPP_Stmt> CGLetExpr(const TYPE_AS_LetExpr &, const TYPE_CGMAIN_VT &);
SEQ<TYPE_CPP_Stmt> CGLetBeSTExpr(const TYPE_AS_LetBeSTExpr &, const TYPE_CGMAIN_VT &);
SEQ<TYPE_CPP_Stmt> GenBindVariables(const TYPE_AS_MultSetBind &, const Generic &);
SEQ<TYPE_CPP_Stmt> GenBindListVariables(const SEQ<TYPE_AS_MultBind> &, const Generic &);
SEQ<TYPE_CPP_Stmt> CGDefExpr(const TYPE_AS_DefExpr &, const TYPE_CGMAIN_VT &);
Generic  CGBracketedExpr(const TYPE_AS_BracketedExpr &, const TYPE_CGMAIN_VT &);
Generic CGIfExpr(const TYPE_AS_IfExpr &, const TYPE_CGMAIN_VT &);
SEQ<TYPE_CPP_Stmt> CGSeqModifyMapOverrideExpr(const TYPE_AS_SeqModifyMapOverrideExpr &, const TYPE_CGMAIN_VT &);
TYPE_REP_TypeRep FindPosApplyTypes(const TYPE_REP_TypeRep &);
SEQ<TYPE_CPP_Stmt> GenSeqOvwr(const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &, bool);
SEQ<TYPE_CPP_Stmt> GenMapOvwr(const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &, bool);
SEQ<TYPE_CPP_Stmt> GenSeqMapDynOvwr(const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &);
SEQ<TYPE_CPP_Stmt> CGFieldSelectExpr(const TYPE_AS_FieldSelectExpr &,
                                     const TYPE_CGMAIN_VT &,
                                     const Generic &,
                                     const Generic &);
SET<TYPE_REP_CompositeTypeRep> FindAllRecTypes(const TYPE_REP_TypeRep &);
#ifdef VDMPP
Generic GetOverFNOP(const TYPE_REP_OverTypeRep & tp, const SEQ<TYPE_AS_Expr> & par_l);
SET<TYPE_REP_ObjRefTypeRep> FindAllObjRefTypes (const TYPE_REP_TypeRep &);
SET<TYPE_REP_ObjRefTypeRep> FindPossibleObjRefTypes (const SET<TYPE_REP_ObjRefTypeRep> & tps,
                                                      const TYPE_AS_Name & fsnm);
#endif // VDMPP
SEQ<TYPE_CPP_Stmt> FindFieldSelApply(const TYPE_AS_Name &, const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &);
SET<TYPE_REP_CompositeTypeRep> FindPossibleRecTypes(const SET<TYPE_REP_CompositeTypeRep> &, const TYPE_AS_Name &);
SEQ<TYPE_CPP_Stmt> CGRecordModifierExpr(const TYPE_AS_RecordModifierExpr &, const TYPE_CGMAIN_VT &);
SEQ<TYPE_CPP_Stmt> GenModifyFields(const TYPE_CPP_Name &,
                                   const TYPE_REP_TypeRep &,
                                   const SEQ<TYPE_AS_RecordModification> &,
                                   const TYPE_CGMAIN_VT &);
SEQ<TYPE_CPP_Stmt> GenFieldInds(const TYPE_CPP_Name &, const TYPE_REP_TypeRep &, const SEQ<TYPE_AS_RecordModification> &);
int FindField(const SEQ<TYPE_REP_FieldRep> &, const TYPE_AS_Name &);
SEQ<TYPE_CPP_Stmt> CGAllOrExistsExpr(const TYPE_AS_QuantExpr &, const TYPE_CGMAIN_VT &);
SEQ<TYPE_CPP_Stmt> CGIotaExpr(const TYPE_AS_IotaExpr &, const TYPE_CGMAIN_VT &);
Generic CGIsExpr(const TYPE_AS_IsExpr &, const TYPE_CGMAIN_VT &);
Generic CGPrefixExpr(const TYPE_AS_PrefixExpr &, const TYPE_CGMAIN_VT &);
Generic CGUnaryNum(int, const TYPE_AS_Expr &, const TYPE_CGMAIN_VT &);
Generic CGUnaryNot(const TYPE_AS_Expr &, const TYPE_CGMAIN_VT &);
Generic CGUnarySet(int, const TYPE_AS_Expr &, const TYPE_CGMAIN_VT &);
TYPE_CPP_Expr CGSetCard(const TYPE_CPP_Expr &, const TYPE_REP_TypeRep &, const TYPE_CGMAIN_VT &);
Generic CGSetDistrUnion(const TYPE_CPP_Expr &, const TYPE_REP_TypeRep &, const TYPE_CGMAIN_VT &);
Generic CGSetDistrInterSect(const TYPE_CPP_Expr &, const TYPE_REP_TypeRep &, const TYPE_CGMAIN_VT &);
Generic CGUnarySetPower(const TYPE_CPP_Expr &, const TYPE_REP_TypeRep &, const TYPE_CGMAIN_VT &);
Generic CGUnarySeq(int, const TYPE_AS_Expr &, const TYPE_CGMAIN_VT &);
Generic CGSeqDistrConc(const TYPE_CPP_Expr &, const TYPE_REP_TypeRep &, const TYPE_CGMAIN_VT &);
Generic CGSeqElems(const TYPE_CPP_Expr &, const TYPE_REP_TypeRep &, const TYPE_CGMAIN_VT &);
Generic CGSeqIndices(const TYPE_CPP_Expr &, const TYPE_REP_TypeRep &, const TYPE_CGMAIN_VT &);
Generic CGSeqLen(const TYPE_CPP_Expr &, const TYPE_REP_TypeRep &, const TYPE_CGMAIN_VT &);
Generic CGSeqTail(const TYPE_CPP_Expr &, const TYPE_REP_TypeRep &, const TYPE_CGMAIN_VT &);
Generic CGSeqHead(const TYPE_CPP_Expr &, const TYPE_REP_TypeRep &, const TYPE_CGMAIN_VT &);
Generic CGSeqReverse(const TYPE_CPP_Expr &, const TYPE_REP_TypeRep &, const TYPE_CGMAIN_VT &);
Generic CGUnaryMap(int, const TYPE_AS_Expr &, const TYPE_CGMAIN_VT &);
Generic CGMapDom(const TYPE_CPP_Expr &, const TYPE_REP_TypeRep &, const TYPE_CGMAIN_VT &);
Generic CGMapRng(const TYPE_CPP_Expr &, const TYPE_REP_TypeRep &, const TYPE_CGMAIN_VT &);
Generic CGMapDistrMerge(const TYPE_CPP_Expr &, const TYPE_REP_TypeRep &, const TYPE_CGMAIN_VT &);
SEQ<TYPE_CPP_Stmt> GenMapDistrMergeInner(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &, const TYPE_REP_TypeRep &);
Generic CGMapInverse(const TYPE_CPP_Expr &, const TYPE_REP_TypeRep &, const TYPE_CGMAIN_VT &);
Tuple GenDynCheckOnSetsValues(const TYPE_CPP_Expr &,
                              const TYPE_CPP_Expr &,
                              const TYPE_REP_TypeRep &,
                              const TYPE_REP_TypeRep &);
bool     DoSetConsistsOnlyOfMaps(const TYPE_REP_TypeRep &);
TYPE_REP_TypeRep FindSetTypeInMapDistrMerge(const TYPE_REP_TypeRep &);
TYPE_REP_TypeRep FindMapTypeInMapDistrMerge(const TYPE_REP_TypeRep &);
Generic  CGName(const TYPE_AS_Name &);
Generic  CGOldName(const TYPE_AS_OldName &);
TYPE_CPP_Expr CGLiteral(const TYPE_AS_Literal &, const TYPE_CGMAIN_VT &);
SEQ<TYPE_CPP_Stmt> CGUndefinedExpr();
Generic CGBinaryExpr(const TYPE_AS_BinaryExpr &, const TYPE_CGMAIN_VT &);
Generic CGLogBinaryExpr(const TYPE_AS_Expr &, int, const TYPE_AS_Expr &, const TYPE_CGMAIN_VT &);
Generic CGEqualityBinaryExpr(const TYPE_AS_Expr &, int, const TYPE_AS_Expr &, const TYPE_CGMAIN_VT &);
Generic CGNumBinaryExpr(const TYPE_AS_Expr &, int, const TYPE_AS_Expr &, const TYPE_CGMAIN_VT &);
Generic CGSetBinaryExpr(const TYPE_AS_Expr &, int, const TYPE_AS_Expr &, const TYPE_CGMAIN_VT &);
Generic CGInSet(const TYPE_AS_Expr &, const TYPE_AS_Expr &, const TYPE_CGMAIN_VT &);
Generic CGNotInSet(const TYPE_AS_Expr &, const TYPE_AS_Expr &, const TYPE_CGMAIN_VT &);
Generic CGProperSubset(const TYPE_AS_Expr &, const TYPE_AS_Expr &, const TYPE_CGMAIN_VT &);
Generic CGSubset(const TYPE_AS_Expr &, const TYPE_AS_Expr &, const TYPE_CGMAIN_VT &);
Generic CGSetMinus(const TYPE_AS_Expr &, const TYPE_AS_Expr &, const TYPE_CGMAIN_VT &);
Generic CGSetInterSect(const TYPE_AS_Expr &, const TYPE_AS_Expr &, const TYPE_CGMAIN_VT &);
Generic CGSetUnion(const TYPE_AS_Expr &, const TYPE_AS_Expr &, const TYPE_CGMAIN_VT &);
Generic CGSeqConc(const TYPE_AS_Expr &, const TYPE_AS_Expr &, const TYPE_CGMAIN_VT &);
Generic CGMapBinaryExpr(const TYPE_AS_Expr &, int, const TYPE_AS_Expr &, const TYPE_CGMAIN_VT &);
Generic CGComposeExpr(const TYPE_AS_Expr &, const TYPE_AS_Expr &, const TYPE_CGMAIN_VT &);
Generic OptimizeMapExpr(const TYPE_AS_Expr& le, const TYPE_AS_PrefixExpr& re, const TYPE_CGMAIN_VT& vt);
Generic CGIterateExpr(const TYPE_AS_Expr &, const TYPE_AS_Expr &, const TYPE_CGMAIN_VT &);
#ifdef VDMPP
SEQ<TYPE_CPP_Stmt> CGApplyFieldSel (const TYPE_AS_FieldSelectExpr &,
                                    const SEQ<TYPE_CPP_Expr> &,
                                    const TYPE_CGMAIN_VT &);
Generic CGObjRef (const TYPE_AS_FieldSelectExpr &, const SEQ<TYPE_CPP_Expr> &, const TYPE_CGMAIN_VT &);
SEQ<TYPE_CPP_Stmt> GenMethodInvoke(const Generic &,
                                   const Generic &,
                                   const Bool &,
                                   const Tuple &,
                                   const Bool &);
SEQ<TYPE_CPP_Stmt> GenSimpleInvoke(const Generic &,
                                   const Generic &,
                                   const Bool &,
                                   const Tuple &,
                                   const Bool &);
SEQ<TYPE_CPP_Stmt> GenUnionInvoke(const TYPE_CGMAIN_VT &,
                                  const Generic &,
                                  const Bool &,
                                  const Tuple &,
                                  const Bool &);
Tuple GenParSequence(const SEQ<TYPE_CPP_Expr> &, const Generic &);
SEQ<TYPE_CPP_Stmt> GenInvoke(const Generic &, const Bool &, const TYPE_CPP_Expr &, const Bool &);
TYPE_CPP_Expr GenSelfExpr(const TYPE_CGMAIN_VT &);
Generic CGIsOfClassExpr(const TYPE_AS_IsOfClassExpr &, const TYPE_CGMAIN_VT &);
Generic CGIsOfBaseClassExpr(const TYPE_AS_IsOfBaseClassExpr &, const TYPE_CGMAIN_VT &);
Generic CGSameClassExpr(const TYPE_AS_SameClassExpr &, const TYPE_CGMAIN_VT &);
Generic CGSameBaseClassExpr(const TYPE_AS_SameBaseClassExpr &, const TYPE_CGMAIN_VT &);
Tuple GenClassOrBase(const TYPE_AS_Expr &, const TYPE_AS_Expr &, bool);
Generic CGNewExpr(const TYPE_AS_NewExpr &, const TYPE_CGMAIN_VT &);
#endif // VDMPP
SEQ<TYPE_CPP_Stmt> CGComprehension(const SEQ<TYPE_AS_MultBind> &,
                                   const Generic &,
                                   const SEQ<TYPE_CPP_Stmt> &,
                                   const Generic &,
                                   bool, const Map &, bool);
SEQ<TYPE_CPP_Stmt> CGComprehensionSimple(const TYPE_AS_MultBind &,
                                         const Generic &,
                                         const SEQ<TYPE_CPP_Stmt> &,
                                         const Generic &,
                                         bool, bool,
                                         const SEQ<TYPE_CPP_Stmt> &,
                                         const Tuple &);
SEQ<TYPE_CPP_Stmt> GenPredicateStmt(const Generic &, const SEQ<TYPE_CPP_Stmt> &, bool);
SEQ<TYPE_AS_Stmt> DeclarePatVars(const TYPE_AS_Pattern &);
SEQ<TYPE_AS_Stmt> DeclPatVars(const MAP<TYPE_AS_Name, TYPE_REP_TypeRep> &);
Tuple FindMultBind(const SEQ<TYPE_AS_MultBind> &);
Sequence FindPatternIdInBindList(const SEQ<TYPE_AS_MultBind> &);
SEQ<TYPE_AS_MultBind> MergeMultBind(const SEQ<TYPE_AS_MultBind> &);
SEQ<TYPE_AS_MultBind> ConvertBindList(const SEQ<TYPE_AS_MultBind> &);

#endif // __mod_expr_h__

