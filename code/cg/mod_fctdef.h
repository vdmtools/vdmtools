/***
*  * WHAT
*  *    Implementation of mod_fctdef.vdm r1.55
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/mod_fctdef.h,v $
*  * VERSION
*  *    $Revision: 1.22 $
*  * DATE
*  *    $Date: 2006/04/04 00:11:16 $
*  * STATUS
*  *    Under development $State: Exp $
*  * PROJECT
*  *    AFRODITE
*  * AUTHOR
*  *    Poul Boegh Lassen (PBL) + $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __mod_fctdef_h__
#define __mod_fctdef_h__

private:
  Sequence fcttab;     // Env
  bool impl;
  bool staticImpl;
  Map implcpp; // map int to CPP`CPPAS

private:
void InitState_FCTDEF();
TYPE_CPP_CPPAS GenFctDef_FD(const TYPE_AS_FnDef &, bool);
Generic GenerateDlCall(const TYPE_AS_Name &, const SEQ<TYPE_CPP_ArgDecl> &, const bool&, const bool&);
bool IsNotyetspecified (const Record &, bool, bool);
#ifdef VDMPP
bool IsSubresp (const Record &);
SEQ<TYPE_CPP_Identifier> GenExceptionsHdr();
#endif // VDMPP
TYPE_CPP_CPPAS GenExplFctDef (const TYPE_AS_ExplFnDef &, bool, bool);
TYPE_CPP_FctDecl GenMethodDecl(const TYPE_AS_Name &, const SEQ<TYPE_CPP_ArgDecl> &);
Tuple GenExplFnBody(const TYPE_AS_ExplFnDef &, bool, bool, bool,
		    const SEQ<TYPE_CPP_ArgDecl> &,
                    const SEQ<TYPE_CPP_DeclSpecifier> &);
TYPE_CPP_Expr FindVariableinDecl(const TYPE_CPP_Declarator &);
SEQ<TYPE_CPP_Expr> FindVariables(const SEQ<TYPE_CPP_ArgDecl> &);
Tuple MakeArgDecl (const TYPE_AS_Type &, const SEQ<TYPE_AS_Pattern> &, const Map &);
SEQ<TYPE_CPP_ArgDecl> DeclPatArg(const Map &, const SEQ<TYPE_AS_Pattern> &, const SEQ<TYPE_AS_Type> &);
//Sequence IncludeImplicit();
TYPE_CPP_CPPAS IncludeExternalCC();
Tuple FindDomRngTypes(const Record &);
#ifdef VDMSL
Tuple FindExpDomRngTypes(const Record &, const Map &);
Tuple FindExpImplDomType(const SEQ<TYPE_AS_PatTypePair> &, const Map &);
#endif // VDMSL
Tuple FindImplDomType(const SEQ<TYPE_AS_PatTypePair> &);
#ifdef VDMSL
TYPE_AS_Type RenameType(const TYPE_AS_Type & t, const Map & m);
SEQ<TYPE_AS_Type> RenameTypeList(const SEQ<TYPE_AS_Type> & tlist, const Map & m);
#endif // VDMSL

TYPE_CPP_CPPAS GenOpDef_FD(const TYPE_AS_Name &, const TYPE_AS_OpDef &, const Generic &, bool);
SEQ<TYPE_AS_ExplFnDef> ExtFn2Fn(const TYPE_AS_ExtExplFnDef & extfd);
Tuple ExtOp2Op(const TYPE_AS_ExtExplOpDef & extop, const Generic & sd);
Tuple Impl2ExplSignature(const SEQ<TYPE_AS_PatTypePair> & partps, const SEQ<TYPE_AS_NameType> & resnmtps);
Tuple InitMethod(const SEQ<TYPE_AS_Pattern> &, const TYPE_AS_OpType&);
TYPE_CPP_CPPAS GenExplOpDef(const TYPE_AS_Name&, const TYPE_AS_ExplOpDef&, const Generic&, bool, bool);
SEQ<TYPE_CPP_Stmt> GenDefaultConstructorHdr(const SEQ<TYPE_CPP_Stmt> &);
TYPE_CPP_CPPAS GenPrePost(const TYPE_AS_ExplOpDef&, const Generic&);
SEQ<TYPE_CPP_Stmt> GenArgPatternMatch(const Map &, const TYPE_AS_Name &, const TYPE_AS_Parameters &,
                                      bool, const SEQ<TYPE_CGMAIN_VT> &);
SEQ<TYPE_CPP_Stmt> GenPatternBody(const Map&,
                                  const TYPE_AS_ExplOpDef &,
                                  bool,
                                  const SEQ<TYPE_CPP_ArgDecl> &,
                                  const SEQ<TYPE_CGMAIN_VT> &);
Tuple GenInlineBody(const TYPE_AS_ExplOpDef &,
                    const TYPE_REP_TypeRep &,
		    bool,
                    const SEQ<TYPE_CPP_ArgDecl> &,
                    bool, bool);

TYPE_CPP_Stmt GenMethPreCall(const TYPE_AS_Name &, const SEQ<TYPE_CPP_Expr> &, bool);
TYPE_CPP_Stmt GenFnPostCall(const TYPE_AS_Name &, const TYPE_CPP_Expr &, const SEQ<TYPE_CPP_Expr> &, bool);
TYPE_CPP_CPPAS GenImplDef(const Record &, const Generic &);
Sequence GenFctOpDecl(const TYPE_AS_Name&, const Record &, const Generic &, const Generic &);
#ifdef VDMSL
Sequence GenExpFctOpDecl(const TYPE_AS_Name&, const Record &, const Generic &, const Generic &, const TYPE_AS_Module &);
TYPE_CPP_TypeSpecifier FindStateType(const Generic &);
#endif // VDMSL

void PushEnv();
void PopEnv();
bool LookUpLocFct(const TYPE_AS_Expr &);
void InsertLocFct(const TYPE_AS_Expr &);
void GenLocalFcts(const SEQ<TYPE_AS_FnDef> &);
bool ContainsImplicitFct() const;
bool ContainsStaticImplicit() const;
TYPE_CPP_CPPAS GetImplicitFcts();
void InsPosFct(const Map &);

#ifdef VDMPP
TYPE_CPP_CPPAS GenHiddenFctDecl(const TYPE_AS_Name&, const Generic&);
#endif //VDMPP

Tuple IsSuperFirst(const SEQ<TYPE_CPP_Stmt> & stmts);
#ifdef VDMPP
Tuple IsMemInitFirst(const TYPE_AS_Name & name, const SEQ<TYPE_CPP_Stmt> & stmts);
#endif //VDMPP
bool IsSafeReturn(const TYPE_AS_OpBody & body, const TYPE_REP_TypeRep & rngtype);
bool HasUnitTypeRep(const TYPE_REP_TypeRep & type);
     
#endif // __mod_fctdef_h__

