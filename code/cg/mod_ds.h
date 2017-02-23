/***
*  * WHAT
*  *    Implementation of mod_vdm_ds.vdm r1.145
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/mod_ds.h,v $
*  * VERSION
*  *    $Revision: 1.33 $
*  * DATE
*  *    $Date: 2006/04/04 00:11:16 $
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

#ifndef __mod_ds_h__
#define __mod_ds_h__

private:
  Map pid_m_ds; // map AS`Name to set of REP`TypeRep

private:
  TYPE_AS_Id GiveLibName() const;
  TYPE_CPP_TypeSpecifier GenType(const TYPE_REP_TypeRep & type);
  SEQ<TYPE_CPP_Stmt> GenDecl_DS(const Generic &, const TYPE_CPP_Name &, const Generic &);
  SEQ<TYPE_CPP_Stmt> GenDeclInit_DS(const Generic &, const TYPE_CPP_Name &, const TYPE_CPP_Expr &);
  SEQ<TYPE_CPP_Stmt> GenConstDeclInit(const Generic &, const TYPE_CPP_Name &, const TYPE_CPP_Expr &);
  SEQ<TYPE_CPP_Stmt> GenValDecl(const TYPE_REP_TypeRep &,const TYPE_AS_Name&);
  TYPE_CPP_Expr GenIsType(const TYPE_CPP_Expr &, const TYPE_REP_TypeRep &);
  TYPE_CPP_Expr GenIsUnionType(const TYPE_CPP_Expr &, const TYPE_REP_UnionTypeRep &);
  TYPE_CPP_Expr GenAuxType(const TYPE_CPP_Expr &,const TYPE_REP_TypeRep &);
  TYPE_CPP_Expr GenGetValue(const TYPE_CPP_Expr &, const Generic &);
  TYPE_CPP_Expr GenValue(const TYPE_AS_Literal &, const TYPE_CGMAIN_VT &);

  Sequence GenTypeDef_DS(const TYPE_AS_Name &, const SEQ<TYPE_AS_TypeDef> &);
  TYPE_CPP_Expr GenCastType(const TYPE_REP_TypeRep &, const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenGenericExpr();
  TYPE_CPP_Expr GenBoolExpr(const Generic & expr);
  TYPE_CPP_Expr GenIntExpr(const Generic & expr);
  TYPE_CPP_Expr GenRealExpr(const Generic & expr);
  TYPE_CPP_Expr GenCharExpr(const Generic & expr);
  TYPE_CPP_Expr GenTokenExpr(const Generic & expr);
  TYPE_CPP_Expr GenQuoteExpr(const Generic & nm);
  TYPE_CPP_Expr GenProductExpr(const Generic & expr);
  TYPE_CPP_Expr GenRecordExpr(const Generic & tag, const Generic & expr);
  void RememberPid_m(const Map &);
  void DeletePid_m();
  Generic DeclaredAs(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GetIntVal(const TYPE_CPP_Expr&);
  TYPE_CPP_Expr GetRealVal(const TYPE_CPP_Expr&);
  TYPE_CPP_Expr GetStringVal(const TYPE_CPP_Expr&);
  TYPE_CPP_Expr GetListVal(const TYPE_CPP_Expr&);
  TYPE_CPP_Expr GenExplicitCast(const TYPE_REP_TypeRep &, const TYPE_CPP_Expr &, const Generic &);
  Generic GenEmptyValue(const TYPE_REP_TypeRep &);
  TYPE_CPP_TypeSpecifier GenSmallBoolType();
  TYPE_CPP_TypeSpecifier GenBoolType();
  SEQ<TYPE_CPP_Stmt> GenBoolDecl(const TYPE_CPP_Name &, const Generic &);
  TYPE_CPP_Expr GenBoolLit_DS(const Bool &);
  TYPE_CPP_Expr GenIsBool(const TYPE_CPP_Expr &);
  TYPE_CPP_TypeSpecifier GenNatType();
  TYPE_CPP_TypeSpecifier GenImplNatType();
  TYPE_CPP_Expr GenIsNatOne(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenIsNat_DS(const TYPE_CPP_Expr &);
  SEQ<TYPE_CPP_Stmt> GenNatOneDecl(const TYPE_CPP_Name &, const Generic &);
  TYPE_CPP_TypeSpecifier GenSmallIntType();
  TYPE_CPP_TypeSpecifier GenSmallNumType();
  TYPE_CPP_TypeSpecifier GenIntType();
  TYPE_CPP_TypeSpecifier GenImplIntType();
  TYPE_CPP_Expr GenIsInt(const TYPE_CPP_Expr &);
  SEQ<TYPE_CPP_Stmt> GenIntDecl(const TYPE_CPP_Name &, const Generic &);
  TYPE_CPP_Expr GenNumLit(const Int &);
  TYPE_CPP_TypeSpecifier GenNumType();
  TYPE_CPP_TypeSpecifier GenRatType();
  TYPE_CPP_TypeSpecifier GenImplRatType();
  TYPE_CPP_Expr GenRealLit(const Real &);
  SEQ<TYPE_CPP_Stmt> GenRealDecl(const TYPE_CPP_Name &, const Generic &);
  TYPE_CPP_TypeSpecifier GenSmallRealType();
  TYPE_CPP_TypeSpecifier GenRealType();
  TYPE_CPP_TypeSpecifier GenImplRealType();
  TYPE_CPP_Expr GenIsReal(const TYPE_CPP_Expr &);
  TYPE_CPP_TypeSpecifier GenTokenType();
  SEQ<TYPE_CPP_Stmt> GenTokenDecl(const TYPE_CPP_Name &, const Generic &);
  TYPE_CPP_Expr GenIsToken(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenSetTokValExpr(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &);
  TYPE_CPP_TypeSpecifier GenCharType();
  TYPE_CPP_Expr GenCharLit_DS(const Char &);
  SEQ<TYPE_CPP_Stmt> GenCharDecl(const TYPE_CPP_Name &, const Generic &);
  TYPE_CPP_Expr GenIsChar(const TYPE_CPP_Expr &);
  TYPE_CPP_TypeSpecifier GenQuoteType(const Generic &);
  TYPE_CPP_Expr GenQuoteLit(const TYPE_AS_Id &);
  TYPE_CPP_Expr GenIsQuote(const TYPE_CPP_Expr &, const TYPE_REP_QuoteTypeRep &);
  TYPE_CPP_Expr GenTextLit(const TYPE_AS_TextLit &);
  TYPE_CPP_TypeSpecifier GenRecordType(const Generic &);
  SEQ<TYPE_CPP_Stmt> GenRecordDecl(const Generic &, const TYPE_CPP_Name &, const Generic &);
  TYPE_CPP_Expr GenCastRecord(const TYPE_CPP_Expr &, const Generic &);
  TYPE_CPP_Expr GenCastRecordTypeForModify(const TYPE_CPP_Expr &, const Generic &);
  TYPE_CPP_Expr GenEqTag(const TYPE_AS_Name &, const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenRecGetField(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &, const TYPE_REP_TypeRep &);
  TYPE_CPP_Expr GenRecGetFieldNm(const TYPE_CPP_Expr &, const TYPE_REP_CompositeTypeRep &, const TYPE_AS_Name &);
  TYPE_CPP_Expr GenRecGetFieldNo(const TYPE_REP_CompositeTypeRep &, const TYPE_AS_Name &);
  TYPE_AS_Id GenGetMethod(const TYPE_REP_TypeRep & tp);
  TYPE_CPP_Expr GenIsThisRecord(const TYPE_REP_CompositeTypeRep &, const TYPE_CPP_Expr &);
  TYPE_CPP_Stmt GenRecSetField(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &, const TYPE_CPP_Expr &);
  TYPE_CPP_Stmt GenRecSetFieldonThis(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenRecSetFieldExpr(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &, const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenIsRecord(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenGetTag(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenRecLength(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenEmptyRecordExpr(const Generic &);
  SEQ<TYPE_CPP_Stmt> GenGenericDecl(const TYPE_CPP_Name &, const Generic &);
  TYPE_CPP_TypeSpecifier GenProductType();
  TYPE_CPP_Expr GenCastProduct(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenCastProductTypeForModify(const TYPE_CPP_Expr &);
  SEQ<TYPE_CPP_Stmt> GenProductDecl_DS(const TYPE_CPP_Name &, const Int &, const Generic &);
  TYPE_CPP_Expr GenTupleLength(const TYPE_CPP_Expr &);
  TYPE_CPP_Stmt GenTupSetField(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &, const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenTupSetFieldExpr(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &, const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenTupGetField(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &,
                               const TYPE_REP_TypeRep &, const TYPE_REP_TypeRep &);
  TYPE_CPP_Expr GenIsTuple(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenNilLit();
  TYPE_CPP_Expr GenIsNil(const TYPE_CPP_Expr &);
#ifdef VDMPP
  SEQ<TYPE_CPP_MemberDeclaration> GenLocalComparator(const TYPE_AS_Name &);
#endif // VDMPP
  TYPE_CPP_TypeSpecifier GenSetType();
  TYPE_CPP_TypeSpecifier GenImplSetType();
  TYPE_CPP_TypeSpecifier GenStringType();
  SEQ<TYPE_CPP_Stmt> GenDeclEmptySet(const TYPE_CPP_Name &);
  SEQ<TYPE_CPP_Stmt> GenDeclSet(const TYPE_CPP_Name &, const Generic &);
  SEQ<TYPE_CPP_Stmt> GenDeclCollection(const TYPE_CPP_Name&, const Generic&);
  TYPE_CPP_Stmt GenSetUnion(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenSetUnionExpr(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenCastSetType(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenCastSetTypeForModify(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenIsSet(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenSetIsEmpty(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenCard(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenCard_int(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenDUnion(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenDInter(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenPower(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenUnion(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenInter(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenDiff(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenPSubSet_Bool(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenInterSect(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &);
  TYPE_CPP_Stmt GenInterSectStmt(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenSetMinus(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &);
  TYPE_CPP_Stmt GenSetMinusStmt(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenSubSet(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenSubSet_Bool(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenEmptySetExpr();
  TYPE_CPP_Expr GenSetExpr(const Generic & ie);
  TYPE_CPP_Expr GenSetGetElemExpr(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenInSet(const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &);
  TYPE_CPP_Expr GenNotInSet(const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &);
  TYPE_CPP_Expr GenInSet_bool(const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &);
  TYPE_CPP_Stmt GenSetInsert(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenSetInsertExpr(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &);
  SEQ<TYPE_CPP_Stmt> GenSetDecl_DS(const TYPE_CPP_Name &, const Generic &);
  SEQ<TYPE_CPP_Stmt> GenIterSet(const TYPE_CGMAIN_VT &,
                                const Generic &,
                                const TYPE_CGMAIN_VT &,
                                const SEQ<TYPE_CPP_Stmt> &);
  TYPE_CPP_TypeSpecifier GenSeq0Type();
  TYPE_CPP_TypeSpecifier GenImplSeq0Type();
  TYPE_CPP_Expr GenSeqApply(const Generic &, const TYPE_CPP_Expr &, const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenSeqApplyString(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenHd(const TYPE_REP_TypeRep &, const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenHdString(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenTl(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenTlString(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenReverse(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenReverseString(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenInds(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenDConc(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenConc(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenConcString(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenLen(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenLenString(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenLen_int(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenLenString_int(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenCastSeq(const TYPE_CPP_Expr &, const Generic &);
  TYPE_CPP_Expr GenCastSeqTypeForModify(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenCastString(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenSubSequence(const TYPE_CPP_Expr & e, const Generic & t,
                               const TYPE_CPP_Expr & from, const TYPE_CPP_Expr & to);
  TYPE_CPP_Expr GenPermute(const TYPE_CPP_Expr & e);
  TYPE_CPP_Stmt GenImpAppend(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenImpAppendExpr(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &);
  TYPE_CPP_Stmt GenSeqModify(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &, const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenSeqModifyExpr(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &, const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenModify(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenIsSeq(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenIsString(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenSeqIsEmpty(const TYPE_CPP_Expr &);
  SEQ<TYPE_CPP_Stmt> GenDeclEmptySeq(const TYPE_CPP_Name &);
  SEQ<TYPE_CPP_Stmt> GenDeclSeq(const TYPE_CPP_Name &, const Generic &);
  SEQ<TYPE_CPP_Stmt> GenSeqDecl_DS(const TYPE_CPP_Name &, const Generic &);
  SEQ<TYPE_CPP_Stmt> GenStringDecl(const TYPE_CPP_Name &, const Generic &);
  TYPE_CPP_Expr GenSeqConc(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &);
  TYPE_CPP_Stmt GenSeqConcStmt(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenSeqElems(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenEmptySeqExpr();
  TYPE_CPP_Expr GenSeqExpr(const Generic & ie);
  TYPE_CPP_Expr GenEmptyStringExpr();
  TYPE_CPP_Expr GenStringExpr(const Generic & ie);
  SEQ<TYPE_CPP_Stmt> GenIterSeq(const TYPE_CGMAIN_VT &,
                                const Generic &,
                                const TYPE_CGMAIN_VT &,
                                const SEQ<TYPE_CPP_Stmt> &);
  TYPE_CPP_TypeSpecifier GenMapType();
  TYPE_CPP_TypeSpecifier GenImplMapType();
  SEQ<TYPE_CPP_Stmt> GenMapDecl_DS(const TYPE_CPP_Name &, const Generic &);
  SEQ<TYPE_CPP_Stmt> GenDeclMap(const TYPE_CPP_Name &, const Generic &);
  TYPE_CPP_Expr GenDom(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenDMerge(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenRng(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenIsMap(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenMapIsEmpty(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenSize_int(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenSize(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenCastMap(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenCastMapTypeForModify(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenMapApply(const TYPE_CGMAIN_VT &, const TYPE_CPP_Expr &);
  TYPE_CPP_Stmt GenMapInsert(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &, const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenMapInsertExpr(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &, const TYPE_CPP_Expr &);
  TYPE_CPP_Stmt GenMapImpModify(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &, const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenMapImpModifyExpr(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &, const TYPE_CPP_Expr &);
  TYPE_CPP_Stmt GenMapOverride(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenOverride(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenInverse(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenDomExists(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenRngExists(const TYPE_CPP_Expr & m, const TYPE_CPP_Expr & e);
  SEQ<TYPE_CPP_Stmt> GenDeclEmptyMap(const Generic &, const TYPE_CPP_Name &);
  TYPE_CPP_Expr GenEmptyMapExpr();
  TYPE_CPP_Expr GenMapExpr(const Generic & ie);
  SEQ<TYPE_CPP_Stmt> GenMapMerge(const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &);
  Generic GenMapRestToBy(const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &, int);
  TYPE_CPP_Stmt GenMapRestrictStmt(const TYPE_CPP_Expr &,
                                   const TYPE_CPP_Expr &,
                                   const TYPE_CPP_Expr &,
                                   const TYPE_CPP_Expr &, int);
  SEQ<TYPE_CPP_Stmt> GenComposeExpr(const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &);
  SEQ<TYPE_CPP_Stmt> GenMapIteration(const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &);
  SEQ<TYPE_CPP_Stmt> GenMapIterIfPart(const TYPE_CPP_Name &, const TYPE_CPP_Name &, const Generic &);
  SEQ<TYPE_CPP_Stmt> GenIterMap(const TYPE_CGMAIN_VT &,
                                const Generic &,
                                const Generic &,
                                const Generic &,
                                const TYPE_CPP_Stmt &);
  Tuple FindMapDomRng(const TYPE_REP_TypeRep &);
  TYPE_CPP_Stmt GenSeqOrMapImpModify(const TYPE_CGMAIN_VT &,
                                     const TYPE_CPP_Expr &,
                                     const TYPE_REP_TypeRep &,
                                     const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenIntResult(const TYPE_CGMAIN_VT &);
  TYPE_CPP_Expr GenPlus_DS(const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &, const Generic &);
  TYPE_CPP_Expr GenMinus_DS(const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &, const Generic &);
  TYPE_CPP_Expr GenMult_DS(const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &, const Generic &);
  TYPE_CPP_Expr GenNumDiv(const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &, const Generic &);
  TYPE_CPP_Expr GenIntVal(const TYPE_CGMAIN_VT &);
  TYPE_CPP_Expr GenNumRem(const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &);
  TYPE_CPP_Expr GenNumMod(const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &);
  TYPE_CPP_Expr GenIntDiv(const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &);
  TYPE_CPP_Expr GenNumLT(const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &);
  TYPE_CPP_Expr GenNumLE(const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &);
  TYPE_CPP_Expr GenNumGT(const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &);
  TYPE_CPP_Expr GenNumGE(const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &);
  TYPE_CPP_Expr GenNumOp(const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &, int);
  TYPE_CPP_Stmt GenExp(const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &);
  TYPE_CPP_Expr GenEq_DS(const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &);
  TYPE_CPP_Expr GenNeq_DS(const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &);
  TYPE_CPP_Expr UnaryPlus(const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &);
  TYPE_CPP_Expr UnaryMinus(const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &);
  SEQ<TYPE_CPP_Stmt> UnaryAbs(const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &);
  SEQ<TYPE_CPP_Stmt> UnaryFloor(const TYPE_CGMAIN_VT &, const TYPE_CGMAIN_VT &);

  TYPE_CPP_Stmt GenNewTag(const TYPE_CPP_Expr &, int);
  TYPE_CPP_Stmt GenSetDontCare(const TYPE_CPP_Expr &, int);
  TYPE_CPP_Stmt GenSetSymTag(const TYPE_CPP_Expr &, const TYPE_AS_Name &, const TYPE_AS_Name &);
  TYPE_CPP_Stmt GenSetRecordTag(const TYPE_CPP_Expr&, const TYPE_AS_Name&, const TYPE_AS_Name&);
  TYPE_CPP_Expr GenNullExpr();

  TYPE_CPP_Identifier GenSuper();
  TYPE_CPP_Identifier GenThis();

  TYPE_CPP_Expr GenEqVal(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenNeqVal(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &);
  
#ifdef VDMPP
  TYPE_CPP_Expr GenIsClass(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenIsThisClass(const TYPE_AS_Name &, const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenIsClasses(const SET<TYPE_AS_Name> &, const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenMyClass(const TYPE_CPP_Expr &);
  TYPE_CPP_Expr GenMyBaseClass(const TYPE_CPP_Expr &);
  TYPE_CPP_TypeSpecifier GenObjRefType_DS(const Generic &);
  SEQ<TYPE_CPP_Stmt> GenObjRefDecl(const Generic &, const TYPE_CPP_Name &, const Generic &);
  TYPE_CPP_Expr CallDefaultConstructor(const TYPE_AS_Name&);
  TYPE_CPP_Expr CallConstructor(const TYPE_AS_Name &, const SEQ<TYPE_CPP_Expr> &);

  TYPE_CPP_Expr CastToClassPtr(const Generic &, const TYPE_CPP_Expr &);
  TYPE_CPP_Stmt GenIsOfClass(const TYPE_AS_Name &, const TYPE_CPP_Expr &, const TYPE_CPP_Name &);
  TYPE_CPP_Expr GenIsOfClassExpr(const TYPE_AS_Name &, const TYPE_CPP_Expr &);
  TYPE_CPP_Stmt GenIsOfBaseClass(const TYPE_AS_Name &, const TYPE_CPP_Expr &, const TYPE_CPP_Name &);
  TYPE_CPP_Expr GenIsOfBaseClassExpr(const TYPE_AS_Name &, const TYPE_CPP_Expr &);
  TYPE_CPP_Stmt GenSameBaseClass(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &, const TYPE_CPP_Name &, const Bool &);
  TYPE_CPP_Expr GenSameBaseClassExpr(const TYPE_CPP_Expr &, const TYPE_CPP_Expr &, const Bool &);
  TYPE_CPP_Expr GenInvokeExpr(const TYPE_REP_ObjRefTypeRep &,
                              const TYPE_CPP_Expr &,
                              const TYPE_CPP_Name &,
                              const SEQ<TYPE_CPP_Expr> &);
  SET<TYPE_CPP_File> GenCGBase(const SET<TYPE_AS_Name> &);
  Tuple GenGet (const TYPE_AS_Name &);
#endif // VDMPP
  TYPE_AS_Id GenCppCharType();
  TYPE_CPP_Expr RemoveIntCast(const TYPE_CPP_Expr & e);

#endif // __mod_ds_h__
