/***
*  * WHAT
*  *    VDM Code Generator.
*  *    mod_fctdef.cc: Implementation of mod_fctdef.vdm 1.81
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/mod_fctdef.cc,v $
*  * VERSION
*  *    $Revision: 1.69 $
*  * DATE
*  *    $Date: 2006/07/03 05:21:15 $
*  * STATUS
*  *    $State: Exp $
*  * PROJECT
*  *    Afrodite - ESPRIT programme, project no. 6500.
*  * AUTHOR
*  *    Poul Boegh Lassen (PBL) + $Author: vdmtools $ + jojo
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "vdmcg.h"
#include "astaux.h"
#include "contextinfo.h"
#include "asquotes.h"
#include "projectval.h"
#ifdef VDMPP
#include "MANGLE.h"
#endif // VDMPP
#include "statsem.h"
#include "BC.h"

void vdmcg::InitState_FCTDEF()
{
  this->fcttab.Clear();
  this->impl = false;
  this->staticImpl = false;
  this->implcpp.Clear();
}

// GenFctDef_FD (GenFctDef)
// fn : AS`FnDef
// isDlClass : bool
// -> CPP`CPPAS
TYPE_CPP_CPPAS vdmcg::GenFctDef_FD(const TYPE_AS_FnDef & fn, bool isDlClass)
{
  switch (fn.GetTag())
  {
    case TAG_TYPE_AS_ExplFnDef:
      return GenExplFctDef(fn, false, isDlClass);
    case TAG_TYPE_AS_ExtExplFnDef: {
      SEQ<TYPE_AS_ExplFnDef> fn_l (ExtFn2Fn(fn));
      TYPE_CPP_CPPAS res;
      size_t len_fn_l = fn_l.Length();
      for (size_t idx = 1; idx <= len_fn_l; idx++)
        res.ImpConc(GenExplFctDef(fn_l[idx], false, isDlClass));
      return res;
    }
    case TAG_TYPE_AS_ImplFnDef:
      return GenImplDef(fn, nil);
  }
  return TYPE_CPP_CPPAS(); // To avoid warnings
}

// GenOpDef
// opnm : AS`Name
// opdef : AS`OpDef
// sd : GenericStateDef
// isDLClass : bool
// -> CPP`CPPAS
TYPE_CPP_CPPAS vdmcg::GenOpDef_FD(const TYPE_AS_Name & opnm,
                                  const TYPE_AS_OpDef & opdef,
                                  const Generic & sd,
                                  bool isDlClass)
{
  switch (opdef.GetTag())
  {
    case TAG_TYPE_AS_ExplOpDef:
      return GenExplOpDef(opnm, opdef, sd, false, isDlClass);
    case TAG_TYPE_AS_ExtExplOpDef: {
      // ==> AS`ExplOpDef * seq of AS`ExplFnDef
      Tuple eo2o (ExtOp2Op(opdef, sd));
      TYPE_CPP_CPPAS res (GenExplOpDef(opnm, eo2o.GetRecord(1), sd, false, isDlClass));
      // pre and post functions
      const SEQ<TYPE_AS_ExplFnDef> & fn_l (eo2o.GetSequence(2)); // seq of AS`ExplFnDef
      size_t len_fn_l = fn_l.Length();
      for (size_t idx = 1; idx <= len_fn_l; idx++)
        res.ImpConc(GenExplFctDef(fn_l[idx], false, false));
      return res;
    }
    case TAG_TYPE_AS_ImplOpDef:
      return GenImplDef(opdef, sd);
  }
  return TYPE_CPP_CPPAS(); // To avoid warnings
}

// ExtFn2Fn
// extfd : AS`ExtExplFnDef
// ==> seq1 of AS`ExplFnDef
SEQ<TYPE_AS_ExplFnDef> vdmcg::ExtFn2Fn(const TYPE_AS_ExtExplFnDef & extfd)
{
  const TYPE_AS_Name & nm (extfd.GetRecord(pos_AS_ExtExplFnDef_nm));
  const SEQ<TYPE_AS_TypeVar> & params (extfd.GetSequence(pos_AS_ExtExplFnDef_params));
  const SEQ<TYPE_AS_PatTypePair> & partps (extfd.GetSequence(pos_AS_ExtExplFnDef_partps));
  const SEQ<TYPE_AS_NameType> & resnmtp (extfd.GetSequence(pos_AS_ExtExplFnDef_resnmtps));
  const TYPE_AS_FnBody & body (extfd.GetRecord(pos_AS_ExtExplFnDef_body));
  const Generic & fnpre (extfd.GetField(pos_AS_ExtExplFnDef_fnpre));
  const Generic & fnpost (extfd.GetField(pos_AS_ExtExplFnDef_fnpost));
  const TYPE_AS_Access & acc (extfd.GetField(pos_AS_ExtExplFnDef_access));
  const Bool & stat (extfd.GetBool(pos_AS_ExtExplFnDef_stat));
  const TYPE_CI_ContextId & cid (extfd.GetInt(pos_AS_ExtExplFnDef_cid));

  Tuple i2es (Impl2ExplSignature(partps, resnmtp));
  const SEQ<TYPE_AS_Pattern> & parms (i2es.GetSequence(1)); // AS`Parameters
  const SEQ<TYPE_AS_Type> & fndom (i2es.GetSequence(2));    // seq of AS`Type
  const Generic & fnrng (i2es.GetField(3));                 // [AS`Type]

  TYPE_AS_PartialFnType fntp;
  fntp.Init(fndom, fnrng, NilContextId);

  TYPE_AS_ExplFnDef fd;
  fd.Init(nm, params, fntp, SEQ<TYPE_AS_Parameters>().ImpAppend(parms), body, Nil(), Nil(), acc, stat, Nil(), cid);

  SEQ<TYPE_AS_ExplFnDef> res_l;
  res_l.ImpAppend(fd);

#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    if (get_preandpost_option()) {
      if (!fnpre.IsNil()) {
        res_l.ImpAppend(MakePreFct(extfd, nil));
      }
      if (!fnpost.IsNil()) {
        res_l.ImpAppend(MakePostFct(extfd, nil));
      }
    }
  }
  else
#endif // VDMPP
  { // C++
    if (!fnpre.IsNil()) {
      res_l.ImpAppend(MakePreFct(extfd, nil));
    }
    if (!fnpost.IsNil()) {
      res_l.ImpAppend(MakePostFct(extfd, nil));
    }
  }
  return res_l;
}

// ExtOp2Op
// extop : AS`ExtExplOpDef
// sd : GenericStateDefi
// ==> AS`ExplOpDef * seq of AS`ExplFnDef
Tuple vdmcg::ExtOp2Op(const TYPE_AS_ExtExplOpDef & extop, const Generic & sd)
{
  const TYPE_AS_Name & nm (extop.GetRecord(pos_AS_ExtExplOpDef_nm));
  const Bool & pu (extop.GetBool(pos_AS_ExtExplOpDef_oppure));
  const Bool & sync (extop.GetBool(pos_AS_ExtExplOpDef_opsync));
  const SEQ<TYPE_AS_PatTypePair> & partps (extop.GetSequence(pos_AS_ExtExplOpDef_partps));
  const SEQ<TYPE_AS_NameType> & resnmtp (extop.GetSequence(pos_AS_ExtExplOpDef_resnmtps));
  const TYPE_AS_OpBody & body (extop.GetRecord(pos_AS_ExtExplOpDef_body));
  const Generic & oppre (extop.GetField(pos_AS_ExtExplOpDef_oppre));
#ifdef VDMSL
  const Generic & oppost (extop.GetField(pos_AS_ExtExplOpDef_oppost));
#endif // VDMSL
  const TYPE_AS_Access & acc (extop.GetField(pos_AS_ExtExplOpDef_access));
  const Bool & stat (extop.GetBool(pos_AS_ExtExplOpDef_stat));
  const Bool & constr (extop.GetBool(pos_AS_ExtExplOpDef_constr));
  const TYPE_CI_ContextId & cid (extop.GetInt(pos_AS_ExtExplOpDef_cid));

  Tuple i2es (Impl2ExplSignature(partps, resnmtp));
  const SEQ<TYPE_AS_Pattern> & parms (i2es.GetSequence(1)); // AS`Parameters
  const SEQ<TYPE_AS_Type> & opdom (i2es.GetSequence(2));    // seq of AS`Type
  const Generic & oprng (i2es.GetField(3));                 // [AS`Type]

  TYPE_AS_OpType optp;
  optp.Init(opdom, oprng, NilContextId);

  TYPE_AS_ExplOpDef opdef;
  opdef.Init(nm,pu,sync,optp,parms,body,Nil(),Nil(),acc,stat,constr,cid);

  SEQ<TYPE_AS_ExplFnDef> fd_l;
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    if (get_preandpost_option()) {
      if (!oppre.IsNil()) {
        fd_l.ImpAppend(MakePreFct(extop, sd));
      }
    }
  }
  else
#endif //VDMPP
  { // C++
    if (!oppre.IsNil()) {
      fd_l.ImpAppend(MakePreFct(extop, sd));
    }
#ifdef VDMSL
    if (!oppost.IsNil()) {
      fd_l.ImpAppend(MakePostFct(extop, sd));
    }
#endif //VDMSL
  }
  return mk_(opdef, fd_l);
}

// Impl2ExplSignature
// partps : AS`ParameterTypes
// resnmtps : seq of AS`NameType
// ==> AS`Parameters * seq of AS`Type * [AS`Type]
Tuple vdmcg::Impl2ExplSignature(const SEQ<TYPE_AS_PatTypePair> & partps, const SEQ<TYPE_AS_NameType> & resnmtps)
{
  SEQ<TYPE_AS_Pattern> parm_l;
  SEQ<TYPE_AS_Type> tp_l;
  Generic restp;

  size_t len_partps = partps.Length();
  for (size_t idx = 1; idx <= len_partps; idx++) {
    const TYPE_AS_PatTypePair & ptp (partps[idx]);
    const SEQ<TYPE_AS_Pattern> & pat_l (ptp.GetSequence(pos_AS_PatTypePair_pats));
    const TYPE_AS_Type & tp (ptp.GetRecord(pos_AS_PatTypePair_tp));

    parm_l.ImpConc(pat_l);
    size_t len_pat_l = pat_l.Length();
    for (size_t i = 1; i <= len_pat_l; i++) {
      tp_l.ImpAppend (tp);
    }
  }

  switch (resnmtps.Length()) {
    case 0: {
      restp = TYPE_AS_VoidType().Init(NilContextId);
      break;
    }
    case 1: {
      restp = resnmtps[1].GetRecord(pos_AS_NameType_tp);
      break;
    }
    default: {
      SEQ<TYPE_AS_Type> restp_l;
      size_t len_resnmtps = resnmtps.Length();
      for (size_t i = 1; i <= len_resnmtps; i++) {
        restp_l.ImpAppend(resnmtps[i].GetRecord(pos_AS_NameType_tp));
      }
      restp = TYPE_AS_ProductType().Init(restp_l, NilContextId);
      break;
    }
  }
  return mk_(parm_l, tp_l, restp);
}

#ifdef VDMPP
// IsSubresp
// bdy : AS`FnBody | AS`OpBody
// ==> bool
bool vdmcg::IsSubresp (const Record & bdy)
{
  Generic body;
  switch(bdy.GetTag()) {
    case TAG_TYPE_AS_FnBody: {
      body = bdy.GetField(pos_AS_FnBody_body);
      break;
    }
    case TAG_TYPE_AS_OpBody: {
      body = bdy.GetField(pos_AS_OpBody_body);
      break;
    }
  }

  if ( body.IsInt() && Int(body) == ((Int) SUBRESP) ) {
    DefineClassAsAbstract();
    return true;
  }
  return false;
}
#endif // VDMPP

// IsNotyetspecified
// bdy : (AS`FnBody | AS`OpBody)
// isDlClass : bool
// isStatic : bool
// ==> bool
bool vdmcg::IsNotyetspecified (const Record & bdy, bool isDlClass, bool isStatic)
{
  Generic body;
  switch(bdy.GetTag()) {
    case TAG_TYPE_AS_FnBody: {
      body = bdy.GetField(pos_AS_FnBody_body);
      break;
    }
    case TAG_TYPE_AS_OpBody: {
      body = bdy.GetField(pos_AS_OpBody_body);
      break;
    }
  }

  if ( body == Int(NOTYETSPEC) ) {
    this->impl = ! isDlClass || this->impl;
    this->staticImpl = (!isDlClass && isStatic) || this->staticImpl;
    return true;
  }
  return false;
}

//bool vdmcg::IsPrelim (Record bdy)
//{
//  if ( bdy.GetField(1).IsInt()  &&
//       bdy.GetField(1) == ((Int) NOTYETSPEC) ) {
//    this->impl = true;
//  }
//  return !(bdy.GetField (1)).IsRecord ();
//}

#ifdef VDMPP
// GenExceptionsHdr
// -> seq of CPP`Identifier
SEQ<TYPE_CPP_Identifier> vdmcg::GenExceptionsHdr()
{
  InsertImport(SEQ<Char>(L"jp.vdmtools.VDM.CGException"));
  return SEQ<TYPE_CPP_Identifier>().ImpAppend(vdm_BC_GenIdentifier(ASTAUX::MkId(L"CGException")));
}
#endif // VDMPP

// GenExplFctDef
// efd : AS`ExplFnDef
// isimpl : bool
// isDlClass : bool
// ==> CPP`CPPAS
TYPE_CPP_CPPAS vdmcg::GenExplFctDef (const TYPE_AS_ExplFnDef & efd, bool isimpl, bool isDlClass)
{
  const TYPE_AS_Name & id (efd.GetRecord(pos_AS_ExplFnDef_nm));

  if (get_verbose_mode()) {
    wcout << L"function: " << ASTAUX::ASName2String(id) << L" start" << endl;
  }

  const TYPE_AS_FnType & tp (efd.GetRecord(pos_AS_ExplFnDef_tp));
  const SEQ<TYPE_AS_Parameters> & parms_ll (efd.GetSequence(pos_AS_ExplFnDef_parms));
  const TYPE_AS_FnBody & body (efd.GetRecord(pos_AS_ExplFnDef_body));
  const Generic & prefn (efd.GetField(pos_AS_ExplFnDef_fnpre));
  const Generic & postfn (efd.GetField(pos_AS_ExplFnDef_fnpost));
  const TYPE_AS_Access & acc (efd.GetField(pos_AS_ExplFnDef_access));
  bool stat (efd.GetBool(pos_AS_ExplFnDef_stat).GetValue());
  const TYPE_CI_ContextId & fctcid (efd.GetInt(pos_AS_ExplFnDef_cid));

  bool isimplicit (isimpl);
  bool isabstract (false);

#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    if (IsSubresp(body)) {
      isabstract = true;
    }
    else if (IsNotyetspecified(body, isDlClass, stat)) {
      isimplicit = true;
    }
  }
  else
#endif // VDMPP
  {
#ifdef VDMPP
    if (IsSubresp(body)) {
      return TYPE_CPP_CPPAS();
    }
    else
#endif // VDMPP
    {
      if (IsNotyetspecified(body, isDlClass, stat) && !isDlClass) {
        return TYPE_CPP_CPPAS();
      }
    }
  }

  PushEnv();
  InitEnv_CGAUX();
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    SetException(false);
  }
#endif // VDMPP
  vdm_BC_ResetVarno();

  SEQ<TYPE_AS_Pattern> parms (parms_ll.Hd());
  if (parms_ll.Length() != 1) {
    PopEnv();
    return TYPE_CPP_CPPAS();
  }

  if (stat) {
    SetIsStatic();
  }
  // ==> bool * map AS`Name to set of REP`TypeRep
  Tuple spi (SamePatternIds (parms));
  const Map & pid_m (spi.GetMap (2)); // map AS`Name to set of REP`TypeRep

  InsertNamesinEnv_CGAUX(pid_m.Dom());

  TYPE_REP_TypeRep rngtype (FromAS2RepType (tp.GetField (2)));

//  TYPE_CPP_Identifier tid (vdm_BC_Rename(efd.get_nm()));
  TYPE_CPP_Identifier tid (vdm_BC_Rename(id));
  TYPE_AS_Id fnName (tid.get_id());

  PushFctLoc( pid_m.Dom() );
  InsPosFct( pid_m );

  SEQ<TYPE_CPP_DeclSpecifier> ds;
  ds.ImpAppend(Rep2CppTypeSpecifier(rngtype, tp.GetField(2)));

  Tuple mad (MakeArgDecl (tp, parms, pid_m ));
  const SEQ<TYPE_CPP_ArgDecl> & arg_l (mad.GetSequence (1));
  const SEQ<TYPE_CGMAIN_VT> & varlist (mad.GetSequence (2));
  bool inlineDecl = mad.GetBoolValue(3);;

  // -> [CPP`Expr] * seq of CPP`Stmt
  Tuple tupBody (GenExplFnBody(efd, isDlClass, isimplicit, isabstract, arg_l, ds));
  const TYPE_CPP_Expr & resVar_v (tupBody.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & b_stmt (tupBody.GetSequence(2));

  SEQ<TYPE_CPP_Stmt> fb;
#ifdef VDMPP
  if (vdm_CPP_isJAVA() ) {
    if (isimplicit && IsNotyetspecified(body, isDlClass, stat)) {
      TYPE_CPP_FctDecl exdecl (vdm_BC_GenFctDecl(vdm_BC_GenIdentifier(ASTAUX::MkId(L"impl_").ImpConc(GiveLastName(id))), arg_l));
      SEQ<TYPE_CPP_Stmt> return_stmt;
      return_stmt.ImpAppend(vdm_BC_GenReturnStmt(GenEmptyValue(RemoveNil(rngtype))));
  
      SEQ<TYPE_CPP_Stmt> implfb;
      if (IsNotyetspecified(body, isDlClass, stat))
        implfb.ImpAppend((RunTime(L"Preliminary Function " + ASTAUX::Id2String(fnName) + L" has been called")));
      else
        implfb.ImpAppend((RunTime(L"Implicit Function " + ASTAUX::Id2String(fnName) + L" has been called")));
      implfb.ImpConc(return_stmt);

      SEQ<TYPE_CPP_Identifier> excs (GenExceptionsHdr());

      TYPE_CPP_CPPAS implcppRVal;
      TYPE_CPP_Modifier pub (vdm_BC_GenModifier(quote_PUBLIC));
      implcppRVal.ImpAppend(vdm_BC_GenJavaFctDef(SEQ<TYPE_CPP_Annotation>(),
                                             SEQ<TYPE_CPP_Modifier>().ImpAppend(pub),
                                             ds, exdecl, excs, vdm_BC_GenBlock(implfb)));
      this->implcpp.Insert(fctcid, implcppRVal);
    }

    if (!parms.IsEmpty () && !inlineDecl
        && !(isimplicit || isabstract || get_skeleton_option())) {
      if (!prefn.IsNil() && get_testpreandpost_option()) {
        fb.ImpAppend(GenMethPreCall(id, FindVariables(arg_l), stat));
      }
      fb.ImpConc( GenArgPatternMatch(pid_m, efd, varlist) );
      fb.ImpConc( b_stmt );
      if (!postfn.IsNil() && get_testpreandpost_option()) {
        fb.ImpAppend(GenFnPostCall(id, resVar_v, FindVariables(arg_l),stat));
      }
      fb.ImpAppend( vdm_BC_GenReturnStmt(resVar_v) );
    }
    else {
      if (!prefn.IsNil() && get_testpreandpost_option()) {
        fb.ImpAppend(GenMethPreCall(id, FindVariables(arg_l), stat));
      }

      if (!isabstract && !get_skeleton_option()) {
        fb.ImpConc(b_stmt);
      }
      else if (isimplicit && IsNotyetspecified(body, isDlClass, stat)) {
        fb.ImpConc(b_stmt);
      }
      else if (isimplicit) {
        ;
      }
      else {
        fb = type_dL(); // Can this be correct??
      }

      if (!postfn.IsNil() && get_testpreandpost_option()) {
        fb.ImpAppend(GenFnPostCall(id, resVar_v, FindVariables(arg_l),stat));
      }

      if (isimplicit && IsNotyetspecified(body, isDlClass, stat)) {
        fb.ImpAppend(vdm_BC_GenReturnStmt(resVar_v));
      }
      else if (!isabstract && !get_skeleton_option()) {
        fb.ImpAppend(vdm_BC_GenReturnStmt(resVar_v));
      }
      else {
        SEQ<TYPE_CPP_Expr> exprlist (FindVariables(arg_l));
        TYPE_CPP_Identifier obj (vdm_BC_GenIdentifier(ASTAUX::MkId(L"child")));
        TYPE_CPP_Expr fctcall (vdm_BC_GenFctCallObjMemAcc(obj, ASTAUX::MkId(L"impl_").ImpConc(fnName), exprlist));
        fb.ImpAppend(vdm_BC_GenReturnStmt(fctcall));
      }
    }
  }
  else
#endif // VDMPP
  { // C++
    if (!prefn.IsNil() && get_testpreandpost_option()) {
      fb.ImpAppend(GenMethPreCall(id, FindVariables(arg_l), stat));
    }
    if (!parms.IsEmpty () && !inlineDecl) {
      fb.ImpConc( GenArgPatternMatch(pid_m, efd, varlist) );
    }
    fb.ImpConc(b_stmt);
    if (!postfn.IsNil() && get_testpreandpost_option()) {
      fb.ImpAppend(GenFnPostCall(id, resVar_v, FindVariables(arg_l),stat));
    }
    fb.ImpAppend(vdm_BC_GenReturnStmt(resVar_v));
  }

  // Add run-time file information
  if (rti) {
      fb.ImpPrepend(GenPushFile());
      fb.ImpAppend(GenPopFile());
  }

  TYPE_CPP_FctDecl decl (GenMethodDecl(id, arg_l));

  TYPE_CPP_CPPAS cpp;
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    SEQ<TYPE_CPP_Identifier> excs (GenExceptionsHdr());
    SEQ<TYPE_CPP_Modifier> mod;
    if (stat) {
      mod.ImpAppend(vdm_BC_GenModifier(quote_STATIC));
    }
    mod.ImpAppend(JavaAccess(acc));

    if (!isabstract && !get_skeleton_option()) {
      cpp.ImpAppend(vdm_BC_GenJavaFctDef(SEQ<TYPE_CPP_Annotation>(),
                                     mod, ds, decl, excs, vdm_BC_GenBlock(fb)));
    }
    else if (isabstract) {
      mod.ImpPrepend(vdm_BC_GenModifier(quote_ABSTRACT));
      cpp.ImpAppend(vdm_BC_GenJavaFctDef(SEQ<TYPE_CPP_Annotation>(), mod, ds, decl, excs, nil));
    }
    else {
      cpp.ImpAppend(vdm_BC_GenJavaFctDef(SEQ<TYPE_CPP_Annotation>(),
                                     mod, ds, decl, nil, vdm_BC_GenReturnStmt(GenEmptyValue(rngtype))));
    }
  }
  else
#endif // VDMPP
  { // C++
    TYPE_CPP_CPPAS fdSeq;
    fdSeq.ImpAppend(vdm_BC_GenFctDef(ds, decl, nil, vdm_BC_GenBlock(fb)));
    cpp.ImpConc (GenFctOpDefIfDef(id, fdSeq));
  }

  PopEnv();

#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    if (get_preandpost_option()) {
      if (!prefn.IsNil ()) {
        cpp.ImpConc (GenFctDef_FD (MakePreFct(efd, nil),false));
      }
      if (!postfn.IsNil ()) {
        cpp.ImpConc (GenFctDef_FD (MakePostFct(efd, nil),false));
      }
    }
  }
  else
#endif // VDMPP
  { // C++
    if (!prefn.IsNil ()) {
      cpp.ImpConc (GenFctDef_FD (MakePreFct(efd, nil),false));
    }
    if (!postfn.IsNil ()) {
      cpp.ImpConc (GenFctDef_FD (MakePostFct(efd, nil),false));
    }
  }

  DeleteLoc();
  ResetIsStatic();

  if (get_verbose_mode()) {
    wcout << L"function: " << ASTAUX::ASName2String(id) << L" end" << endl;
  }

  return cpp;
}

// GenMethodDecl
// p_nm : AS`Name
// p_arg_l : seq of CPP`ArgDecl
// -> CPP`FctDecl
TYPE_CPP_FctDecl vdmcg::GenMethodDecl(const TYPE_AS_Name & p_nm, const SEQ<TYPE_CPP_ArgDecl> & p_arg_l)
{
#ifdef VDMSL
    return vdm_BC_GenFctDecl (vdm_BC_Rename (p_nm), p_arg_l);
#endif //VDMSL
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenFctDecl (vdm_BC_Rename(InsertGlobalName(p_nm)), p_arg_l);
  }
  else { // C++
    TYPE_AS_Name cnm (GiveCurCASName());
    TYPE_CPP_Expr cid (vdm_BC_Rename (cnm));
    TYPE_CPP_Expr fnm (vdm_BC_Rename (p_nm));
    return vdm_BC_GenFctDecl (vdm_BC_GenQualifiedName (cid, fnm), p_arg_l);
  }
#endif //VDMPP
}

// GenExplFnBody
// p_fndef : AS`ExplFnDef
// p_isDlClass : bool
// p_isImplicit : bool
// p_isAbstract : bool
// p_arg_l : seq of CPP`ArgDecl
// p_ds : CPP`DeclSpecifiers
// -> [CPP`Expr] * seq of CPP`Stmt
Tuple vdmcg::GenExplFnBody(const TYPE_AS_ExplFnDef & p_fndef,
                           bool p_isDlClass,
                           bool p_isImplicit,
                           bool p_isAbstract,
                           const SEQ<TYPE_CPP_ArgDecl> & p_arg_l,
                           const SEQ<TYPE_CPP_DeclSpecifier> & p_ds)
{
  const TYPE_AS_Name & nm     (p_fndef.GetRecord(pos_AS_ExplFnDef_nm));
  const TYPE_AS_FnType & tp   (p_fndef.GetRecord(pos_AS_ExplFnDef_tp));
  const TYPE_AS_FnBody & body (p_fndef.GetRecord(pos_AS_ExplFnDef_body));
  const Bool & stat           (p_fndef.GetBool(pos_AS_ExplFnDef_stat));
      
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    if (p_isImplicit) {
      TYPE_CPP_Identifier tid (vdm_BC_Rename(nm));
      const TYPE_AS_Id & id (tid.GetSequence(pos_CPP_Identifier_id));
      if(IsNotyetspecified(body, false, stat)) {
        SEQ<TYPE_CPP_Expr> exprlist (FindVariables(p_arg_l));
        TYPE_CPP_Expr l_resVar (vdm_BC_GiveName(ASTAUX::MkId(L"childResult")));
        TYPE_CPP_Identifier childid (vdm_BC_GenIdentifier(ASTAUX::MkId(L"child")));
        TYPE_CPP_Expr fctCall (vdm_BC_GenFctCallObjMemAcc(childid, ASTAUX::MkId(L"impl_").ImpConc(id), exprlist ));
        SEQ<TYPE_CPP_Stmt> l_stmt;
        l_stmt.ImpAppend(vdm_BC_GenDecl(p_ds.Hd(), l_resVar, vdm_BC_GenAsgnInit(fctCall)));
        return mk_(l_resVar, l_stmt);
      }
      else {
        SEQ<TYPE_CPP_Stmt> l_stmt;
        l_stmt.ImpAppend((RunTime(L"Implicit Function " + ASTAUX::Id2String(id) + L" has been called")));
        return mk_(Nil(), l_stmt);
      }
    }
    else if (p_isAbstract) { // mk_(nil, [])
      return mk_(Nil(), SEQ<TYPE_CPP_Stmt>());
    }
    else {
      return CGExprExcl(body.get_body(), ASTAUX::MkId(L"varRes"), FromAS2RepType(tp.GetRecord(2)));
    }
  }
  else
#endif // VDMPP
  { // C++
    if (IsNotyetspecified(body, p_isDlClass, stat) && p_isDlClass) {
      return GenerateDlCall(nm, p_arg_l, false, false);
    }
    else {
      //return CGExprExcl(body.get_body(), ASTAUX::MkId(L"varRes"), FromAS2RepType(tp.GetRecord(2)));
      const TYPE_AS_Expr & expr (body.GetRecord(pos_AS_FnBody_body));
      Tuple cgee (CGExprExcl(body.get_body(), ASTAUX::MkId(L"varRes"), FromAS2RepType(tp.GetRecord(2))));
      const SEQ<TYPE_CPP_Stmt> & stmts (cgee.GetSequence(2));
      if (!stmts.IsEmpty()) {
        bool forall = stmts[stmts.Length()].Is(TAG_TYPE_CPP_CompoundStmt);
        size_t len_stmts = stmts.Length();
        for (size_t i = 1; (i < len_stmts) && forall; i++) {
          forall = stmts[i].Is(TAG_TYPE_CPP_DeclarationStmt);
        }
        if (forall) { 
          SEQ<TYPE_CPP_Stmt> res (stmts.SubSequence(1,len_stmts -1));
          return mk_(cgee.GetField(1),
                     stmts.SubSequence(1,len_stmts -1).Conc(stmts[len_stmts].GetSequence(pos_CPP_CompoundStmt_stms)));
        }
      }
      return cgee;
    }
  }
}

// GenerateDlCall
// id : AS`Name
// arg_l : seq of CPP`ArgDecl
// isOp : bool
// isVoid : bool
// ==>  (CPP`Expr * seq of CPP`Stmt) | seq of CPP`Stmt
//
// (CPP`Expr * seq of CPP`Stmt) for function
// seq of CPP`Stmt for operation
Generic vdmcg::GenerateDlCall(const TYPE_AS_Name & id,
                              const SEQ<TYPE_CPP_ArgDecl> & arg_l,
                              const bool & isOp,
                              const bool & isVoid)
{
  TYPE_CPP_Identifier seqName (vdm_BC_GiveName(ASTAUX::MkId(L"parmSeq")));
  SEQ<TYPE_CPP_Expr> parms;
  size_t len_arg_l = arg_l.Length();
  for (size_t idx = 1; idx <= len_arg_l; idx++) {
    parms.ImpAppend(FindVariableinDecl(arg_l[idx]));
  }

  TYPE_CPP_Identifier succFlag (vdm_BC_GiveName(ASTAUX::MkId(L"success")));
  TYPE_CPP_Identifier resName (vdm_BC_GiveName(ASTAUX::MkId(L"dlCallResult")));
  TYPE_AS_Id fnm (GiveLastName(id));

  SEQ<TYPE_CPP_Stmt> body;
  body.ImpConc(GenSeqDecl_DS(seqName, Nil()));

  size_t len_parms = parms.Length();
  for (size_t i = 1; i <= len_parms; i++) {
    body.ImpAppend(vdm_BC_GenFctCallObjMemAccStmt(seqName,
                                                  ASTAUX::MkId(L"ImpAppend"),
                                                  SEQ<TYPE_CPP_Expr>().ImpAppend(parms[i])));
  }
  body.ImpAppend(vdm_BC_GenDecl(vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"int"))), succFlag, Nil()));
  SEQ<TYPE_CPP_Expr> dlArgs;
  dlArgs.ImpAppend(GenDlMember());
  dlArgs.ImpAppend(vdm_BC_GenStringLit(fnm));
  dlArgs.ImpAppend(seqName);
  dlArgs.ImpAppend(succFlag);
  TYPE_CPP_Expr dlcall (vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"DlClass_call")), dlArgs));
  if (isOp) { // operation
    if (isVoid) {
      body.ImpAppend(vdm_BC_GenExpressionStmt(dlcall));
    }
    else {
      body.ImpAppend(vdm_BC_GenReturnStmt(dlcall));
    }
    return body;
  }
  else { // function
    body.ImpConc(GenGenericDecl(resName, vdm_BC_GenAsgnInit(dlcall)));
    return mk_(resName, body);
  }
}

// FindVariableinDecl
// decl : CPP`Declarator
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::FindVariableinDecl(const TYPE_CPP_Declarator & decl)
{
  switch (decl.GetTag())
  {
    case TAG_TYPE_CPP_Identifier: {
      return decl;
    }
    case TAG_TYPE_CPP_ArgDecl: {
      TYPE_CPP_ArgDecl ad (decl);
      TYPE_CPP_Declarator declarator (ad.get_decl());
      return FindVariableinDecl(declarator);
    }
    case TAG_TYPE_CPP_RefDecl: {
      TYPE_CPP_RefDecl rd (decl);
      TYPE_CPP_Declarator declarator (rd.get_decl());
      return FindVariableinDecl(declarator);
    }
    default: {
      ReportError (L"FindVariableinDecl");
      return TYPE_CPP_Expr(); // dummy
    }
  }
}

// FindVariables
// arg_l : seq of CPP`ArgDecl
// ==> seq of CPP`Expr
SEQ<TYPE_CPP_Expr> vdmcg::FindVariables(const SEQ<TYPE_CPP_ArgDecl> & arg_l)
{
  SEQ<TYPE_CPP_Expr> exprs;
  size_t len_arg_l = arg_l.Length();
  for (size_t idx = 1; idx <= len_arg_l; idx++)
  {
    exprs.ImpAppend(FindVariableinDecl(arg_l[idx].get_decl()));
  }
  return exprs;
}

// MakeArgDecl
// fntype : AS`FnType | AS`OpType
// parms : seq of AS`Pattern
// pid_m : map AS`Name to set of REP`TypeRep
// ==> seq of CPP`ArgDecl * seq of CGMAIN`VT * bool
Tuple vdmcg::MakeArgDecl (const TYPE_AS_Type & fntype, 
                          const SEQ<TYPE_AS_Pattern> & parms,
                          const Map & pid_m)
{
  const SEQ<TYPE_AS_Type> & dtypes (fntype.GetSequence (1));

  SEQ<TYPE_CPP_ArgDecl> argdecl_l;
  SEQ<TYPE_CGMAIN_VT> varlist;
  size_t len_dtypes = dtypes.Length();
  for (size_t varno = 1; varno <= len_dtypes; varno++) {
    const TYPE_AS_Type & tmpType (dtypes[varno]);
    TYPE_CPP_Identifier varname (vdm_BC_GiveName(ASTAUX::MkId(L"var_").ImpConc(StringNumber(varno))));

    TYPE_CPP_Expr refvarname;
#ifdef VDMPP
    if (vdm_CPP_isJAVA()) {
      refvarname = varname;
    }
    else
#endif // VDMPP
    {
      refvarname = vdm_BC_GenRef(varname);
    }

    TYPE_REP_TypeRep tmpTypeRep (FromAS2RepType (tmpType));
    varlist.ImpAppend (mk_CG_VT (varname, tmpTypeRep));

    SEQ<TYPE_CPP_DeclSpecifier> const_decl;
    const_decl.ImpAppend(vdm_BC_GenTypeSpecifier(quote_CONST));
    const_decl.ImpAppend(Rep2CppTypeSpecifier(tmpTypeRep, tmpType));
    argdecl_l.ImpAppend (vdm_BC_GenArgDecl(const_decl, refvarname));
  }

  bool inlineDecl = true;
  int count = 0;
  size_t len_parms = parms.Length();
  for (size_t idx = 1; (idx <= len_parms) && inlineDecl; idx++) {
    if ((inlineDecl = parms[idx].Is(TAG_TYPE_AS_PatternName))) {
      if (parms[idx].GetField(pos_AS_PatternName_nm).IsNil()) {
         count++;
      }
    }
  }
  inlineDecl = inlineDecl && ((pid_m.Dom()).Card() == (parms.Length() - count));

  SEQ<TYPE_CPP_ArgumentDeclaration> arg_l (inlineDecl ? DeclPatArg (pid_m, parms, dtypes) : argdecl_l);
  return mk_(arg_l, varlist, Bool(inlineDecl));
}

// DeclPatArg
// - : map AS`Name to set of REP`TypeRep
// parm_l : seq of AS`Pattern // must be AS`PatternName
// argtypes : AS`DiscretionaryType
// ==> seq of CPP`ArgDecl
SEQ<TYPE_CPP_ArgDecl> vdmcg::DeclPatArg(const Map & /*pid_m*/,
                                        const SEQ<TYPE_AS_Pattern> & parm_l,
                                        const SEQ<TYPE_AS_Type> & argtypes )
{
  SEQ<TYPE_CPP_ArgDecl> rb;
  size_t len_parm_l = parm_l.Length();
  for (size_t count = 1; count <= len_parm_l; count++)
  {
    Generic name (parm_l[count].GetField(pos_AS_PatternName_nm));

    TYPE_REP_TypeRep type (FromAS2RepType(argtypes[count]));

    if (!name.IsNil())
      InsertName_CGAUX(name);

    TYPE_CPP_Identifier varname (name.IsNil()
                   ? vdm_BC_GiveName(ASTAUX::MkId(L"unused_").ImpConc(StringNumber(count)))
                   : vdm_BC_Rename(name));

    TYPE_CPP_Expr refvarname;
#ifdef VDMPP
    if (vdm_CPP_isJAVA()) {
      refvarname = varname;
    }
    else
#endif // VDMPP
    {
      refvarname = vdm_BC_GenRef(varname);
    }

    SEQ<TYPE_CPP_DeclSpecifier> const_decl;
    const_decl.ImpAppend(vdm_BC_GenTypeSpecifier(quote_CONST));
    const_decl.ImpAppend(Rep2CppTypeSpecifier(type, argtypes[count]));
    rb.ImpAppend( vdm_BC_GenArgDecl( const_decl, refvarname ) );
  }
  return rb;
}

// InitMethod
// parm_l : AS`Parameters
// tp : AS`OpType
// ==> (map AS`Name to set of REP`TypeRep) *
//     seq of CGMAIN`VT * REP`TypeRep * bool * seq of CPP`ArgDecl
Tuple vdmcg::InitMethod(const SEQ<TYPE_AS_Pattern> & parm_l, const TYPE_AS_OpType & tp)
{
  PushEnv();
  InitEnv_CGAUX();
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    SetException(false);
  }
#endif // VDMPP
  vdm_BC_ResetVarno();

  Tuple spi (SamePatternIds (parm_l));
  Map pid_m (spi.GetMap (2));

  InsertNamesinEnv_CGAUX(pid_m.Dom());

  TYPE_REP_TypeRep rngtype (FromAS2RepType(tp.GetRecord(pos_AS_OpType_oprng)));

  PushFctLoc( pid_m.Dom() );
  InsPosFct( pid_m );
  CurrentRType(rngtype);

  Tuple mad (MakeArgDecl (tp, parm_l, pid_m));
  const SEQ<TYPE_CPP_ArgDecl> & arg_l (mad.GetSequence (1));
  const SEQ<TYPE_CGMAIN_VT> & varlist (mad.GetSequence (2));
  bool inlineDecl = (mad.GetBoolValue(3));

  return mk_(pid_m, varlist, rngtype, Bool(inlineDecl), arg_l);
}

// GenExplOpDef
// opnm : AS`Name
// opdef : AS`ExplOpDef
// sd : GenericStateDef
// isimpl : bool
// isDLClass : bool
// ==> CPP`CPPAS
TYPE_CPP_CPPAS vdmcg::GenExplOpDef(const TYPE_AS_Name & opnm,
                                   const TYPE_AS_ExplOpDef & opdef,
                                   const Generic & sd,
                                   bool isimpl,
                                   bool isDlClass)
{
  const TYPE_AS_Name & nm (opdef.GetRecord(pos_AS_ExplOpDef_nm));

  if (get_verbose_mode()) {
    wcout << L"operation: " << ASTAUX::ASName2String(nm) << L" start" << endl;
  }

  const TYPE_AS_OpType & tp (opdef.GetRecord(pos_AS_ExplOpDef_tp));
  const SEQ<TYPE_AS_Pattern> & parm_l (opdef.GetSequence(pos_AS_ExplOpDef_parms));
  const TYPE_AS_OpBody & body (opdef.GetRecord(pos_AS_ExplOpDef_body));
#ifdef VDMPP
  const Generic & oppre (opdef.GetField(pos_AS_ExplOpDef_oppre));
//  const Generic & oppost (opdef.GetField(pos_AS_ExplOpDef_oppost));
#endif // VDMPP
  const TYPE_AS_Access & acc (opdef.GetField(pos_AS_ExplOpDef_access));
  bool stat (opdef.GetBoolValue(pos_AS_ExplOpDef_stat));
  bool constr (opdef.GetBoolValue(pos_AS_ExplOpDef_constr));
  const TYPE_CI_ContextId & opcid (opdef.GetInt(pos_AS_ExplOpDef_cid));

#ifdef VDMPP
  bool isimplicit (isimpl);
  bool isabstract (false);

  if (vdm_CPP_isJAVA()) {
    if (IsSubresp(body) || (get_j2v_option() &&
                            isInterface(GiveCurCASName()) &&
                            body.get_body() == Int(NOTYETSPEC))) {
      isabstract = true;
    }
    else if (IsNotyetspecified(body, false, stat)) {
      isimplicit = true;
    }
  }
  else
#endif // VDMPP
  { // C++
#ifdef VDMPP
    if (IsSubresp(body)) {
      return TYPE_CPP_CPPAS();
    }
    else
#endif // VDMPP
    {
      if (IsNotyetspecified(body, isDlClass, stat)) {
        if (!isDlClass) {
          return TYPE_CPP_CPPAS();
        }
      }
    }
  }

  if (stat) {
    SetIsStatic();
  }

#ifdef VDMPP
  if (constr && parm_l.IsEmpty()) {
    SetHasDefaultConstr();
  }

  ResetUsesSuper();
  ResetUsesThis();
#endif // VDMPP

  Tuple initTuple (InitMethod(parm_l, tp));
  const Map & pid_m (initTuple.GetMap(1)); // (map AS`Name to set of REP`TypeRep)
  const SEQ<TYPE_CGMAIN_VT> & varlist (initTuple.GetSequence(2)); // seq of CGMAIN`VT
  const TYPE_REP_TypeRep & rngtype (initTuple.GetRecord(3)); // REP`TypeRep
  bool inlineDecl (initTuple.GetBoolValue(4)); // bool
  const SEQ<TYPE_CPP_ArgDecl> & arg_l (initTuple.GetSequence(5)); // seq of CPP`ArgDecl

  type_dL ds;
  if (!constr) {
    ds.ImpAppend(Rep2CppTypeSpecifier(rngtype, tp.GetRecord(pos_AS_OpType_oprng)));
  }

  TYPE_CPP_FctDecl decl (GenMethodDecl(nm, arg_l));

  SEQ<TYPE_CPP_Stmt> fb;
  SEQ<TYPE_CPP_Stmt> implfb;

  const Generic & st (body.GetField(pos_AS_OpBody_body));

  if (!st.Is(TAG_TYPE_AS_IdentStmt)) {
    if (!parm_l.IsEmpty () && !inlineDecl
#ifdef VDMPP
         && !(vdm_CPP_isJAVA() && (isimplicit || isabstract || get_skeleton_option()))
#endif // VDMPP
       ) {
      fb.ImpConc(GenPatternBody(pid_m, opdef, isDlClass, arg_l, varlist));
    }
    else {
#ifdef VDMSL
      Tuple tupBody (GenInlineBody(opdef, rngtype, isDlClass, arg_l, false, false));
      fb.ImpConc(tupBody.GetSequence(2));
#endif // VDMSL
#ifdef VDMPP
      Tuple tupBody (GenInlineBody(opdef, rngtype, isDlClass, arg_l, isimplicit, isabstract));
      implfb.ImpConc(tupBody.GetSequence(1));
      fb.ImpConc(tupBody.GetSequence(2));
#endif // VDMPP
    }
  }

  TYPE_CPP_CPPAS cpp;
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    SEQ<TYPE_CPP_Stmt> pre_fb;
    if (!oppre.IsNil() && get_testpreandpost_option()) {
      pre_fb.ImpAppend(GenMethPreCall(nm, FindVariables(arg_l), stat));
    }

    if (constr) {
      // for constructor
      TYPE_CPP_Stmt initfc (vdm_BC_GenExpressionStmt(
                              vdm_BC_GenFctCall(
                                vdm_BC_GivePrePostNm(GiveCurCASName(), ASTAUX::MkId(L"init")),
                                SEQ<TYPE_CPP_Expr>())));

      
      SEQ<TYPE_CPP_Stmt> new_fb;
      Tuple isf (IsSuperFirst(fb));
      if (isf.GetBoolValue(1)) {
        new_fb.ImpAppend(isf.GetRecord(2));
        new_fb.ImpAppend(initfc);
        new_fb.ImpConc(pre_fb);
        new_fb.ImpConc(isf.GetSequence(3));
      }
      else {
        new_fb.ImpAppend(initfc);
        new_fb.ImpConc(pre_fb);
        new_fb.ImpConc(fb);
      }
      fb = new_fb;
    }
    else {
      SEQ<TYPE_CPP_Stmt> new_fb;
      new_fb.ImpConc(pre_fb);
      new_fb.ImpConc(fb);
      fb = new_fb;
    }

    if (get_conc_option()) {
      if (!constr && !stat) {
        fb = GenConcOper(opnm, fb);
      }
      else if (GetHasDefaultConstr() && GetOrderedSupers(GiveCurCASName()).IsEmpty()) {
        fb = GenDefaultConstructorHdr(fb);
      }
    }

    // TODO: calling vdm_init_XXX must be before pre function for Sentinel
    //if (!oppre.IsNil() && get_testpreandpost_option())
    //    fb.ImpPrepend(GenMethPreCall(nm, FindVariables(arg_l), stat));

    // Add run-time file information FIXME: why isn't this in the spec?
    if (rti) {
      fb.ImpPrepend(GenPushFile());
      fb.ImpAppend(GenPopFile());
    }

    SEQ<TYPE_CPP_Modifier> mods;
    if (isabstract) {
      mods.ImpAppend(vdm_BC_GenModifier(quote_ABSTRACT));
    }
    if (stat) {
      mods.ImpAppend(vdm_BC_GenModifier(quote_STATIC));
    }

    mods.ImpAppend(JavaAccess(acc));

    SEQ<TYPE_CPP_Identifier> excs (GenExceptionsHdr());

    if (isEntries(nm)) {
      excs.Clear();
    }

    if (isabstract) {
      cpp.ImpAppend(vdm_BC_GenJavaFctDef(SEQ<TYPE_CPP_Annotation>(), mods, ds, decl, excs, nil));
    }
    else if  (get_skeleton_option()) {
      cpp.ImpAppend(vdm_BC_GenJavaFctDef(SEQ<TYPE_CPP_Annotation>(),
                                     mods, ds, decl, nil, vdm_BC_GenReturnStmt(GenEmptyValue(rngtype))));
    }
    else {
      cpp.ImpAppend(vdm_BC_GenJavaFctDef(SEQ<TYPE_CPP_Annotation>(),
                                     mods, ds, decl, excs, vdm_BC_GenBlock(fb)));
    }

    if (isimplicit) {
      TYPE_CPP_FctDecl exdecl (vdm_BC_GenFctDecl(vdm_BC_GenIdentifier(ASTAUX::MkId(L"impl_").ImpConc(GiveLastName(nm))),
                                                 (Generic)arg_l));
      TYPE_CPP_Modifier pub (vdm_BC_GenModifier(quote_PUBLIC));

      this->implcpp.Insert(opcid, Sequence().ImpAppend(
            vdm_BC_GenJavaFctDef(SEQ<TYPE_CPP_Annotation>(),
                             SEQ<TYPE_CPP_Modifier>().ImpAppend(pub), ds, exdecl, excs, vdm_BC_GenBlock(implfb))));
    }

    if ((ASTAUX::ASName2String(nm) == L"toString") &&
         tp.GetSequence(pos_AS_OpType_opdom).IsEmpty() &
         IsStringType(FromAS2RepType(tp.GetRecord(pos_AS_OpType_oprng)))) {
      SEQ<TYPE_CPP_Modifier> mods;
      mods.ImpAppend(vdm_BC_GenModifier(quote_PUBLIC));
      type_dL ds;
      ds.ImpAppend(vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"String"))));
      Generic decl (vdm_BC_GenFctDecl (vdm_BC_GenIdentifier(ASTAUX::MkId(L"toString")), SEQ<TYPE_CPP_ArgDecl>()));
      SEQ<TYPE_CPP_Identifier> excs;
      SEQ<TYPE_CPP_Stmt> fb;
      fb.ImpAppend(vdm_BC_GenReturnStmt(vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"vdm_toString")),
                                                          SEQ<TYPE_CPP_Expr>())));
      TYPE_CPP_ExceptionDeclaration expdecl (vdm_BC_GenExceptionDeclaration(
                                               SEQ<TYPE_CPP_TypeSpecifier>().ImpAppend(
                                                 vdm_BC_GenTypeSpecifier(
                                                   vdm_BC_GenIdentifier(ASTAUX::MkId(L"CGException")))),
                                                 vdm_BC_GenIdentifier(ASTAUX::MkId(L"e"))));

//      TYPE_CPP_Stmt stmt (vdm_BC_GenReturnStmt(vdm_BC_GenFctCall(
//                                                 vdm_BC_GenIdentifier(ASTAUX::MkId(L"super.toString")),
//                                                 SEQ<TYPE_CPP_Expr>())));
      TYPE_CPP_Stmt stmt (vdm_BC_GenReturnStmt(
                            vdm_BC_GenFctCall(
                              vdm_BC_GenIdentifier(ASTAUX::MkId(L"getClass().getName() + '@' + Integer.toString")),
                                                 SEQ<TYPE_CPP_Expr>().ImpAppend(vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"hashCode")), SEQ<TYPE_CPP_Expr>())))));
      SEQ<TYPE_CPP_Handler> handlers;
      handlers.ImpAppend(vdm_BC_GenHandler(expdecl, stmt));
      cpp.ImpAppend(vdm_BC_GenJavaFctDef(SEQ<TYPE_CPP_Annotation>(),
                                     mods, ds, decl, excs, vdm_BC_GenTryBlock(fb, handlers, Nil())));
    }
  }
  else 
#endif // VDMPP
  { // C++
#ifdef VDMPP
    // initialization of super class
    // temporary simple hack 
    SEQ<TYPE_CPP_MemInitializer> mi_l;
    if (constr)
    {
      TYPE_CPP_Stmt initfc (vdm_BC_GenExpressionStmt(
                              vdm_BC_GenFctCall(vdm_BC_GivePrePostNm(nm, ASTAUX::MkId(L"init")),
                                                SEQ<TYPE_CPP_Expr>())));
      SEQ<TYPE_CPP_Stmt> new_fb;
      if (!GetOrderedSupers(nm).IsEmpty() && !fb.IsEmpty()) {
        Tuple imif (IsMemInitFirst(nm, fb));
        if (imif.GetBoolValue(1)) {
          mi_l.ImpAppend(imif.GetRecord(2));
          new_fb.ImpAppend(initfc);
          new_fb.ImpConc(imif.GetSequence(3));
        }
        else {
          new_fb.ImpAppend(initfc);
          new_fb.ImpConc(fb);
        }
      }
      else {
        new_fb.ImpAppend(initfc);
        new_fb.ImpConc(fb);
      }
      fb = new_fb;
    }
    // TODO: calling vdm_init_XXX must be before pre function for Sentinel
    if (!oppre.IsNil() && get_testpreandpost_option()) {
        fb.ImpPrepend(GenMethPreCall(nm, FindVariables(arg_l), stat));
    }
#endif // VDMPP

    // Add run-time file information FIXME: why isn't this in the spec?
    if (rti) {
      fb.ImpPrepend(GenPushFile());
      fb.ImpAppend(GenPopFile());
    }

#ifdef VDMPP
    TYPE_CPP_FunctionDefinition fd (constr 
            ? vdm_BC_GenConstructorDef(SEQ<TYPE_CPP_Annotation>(), ds, decl, mi_l, vdm_BC_GenBlock(fb))
            : vdm_BC_GenFctDef(ds, decl, nil, vdm_BC_GenBlock(fb)));
#endif // VDMPP
#ifdef VDMSL
    TYPE_CPP_FunctionDefinition fd (vdm_BC_GenFctDef(ds, decl, nil, vdm_BC_GenBlock(fb)));
#endif // VDMSL

    cpp.ImpConc(GenFctOpDefIfDef(nm, mk_sequence(fd)));
  }

  PopEnv();

  cpp.ImpConc(GenPrePost(opdef, sd));
  ResetIsStatic();

  if (get_verbose_mode()) {
    wcout << L"operation: " << ASTAUX::ASName2String(nm) << L" end"  << endl;
  }

  return cpp;
}

// GenDefaultConstructorHdr
// fb : seq of CPP`Stmt
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenDefaultConstructorHdr(const SEQ<TYPE_CPP_Stmt> & fb)
{
  TYPE_CPP_ExceptionDeclaration expdecl (vdm_BC_GenExceptionDeclaration(
                                           SEQ<TYPE_CPP_TypeSpecifier>().ImpAppend(
                                             vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Throwable")))),
                                           vdm_BC_GenIdentifier(ASTAUX::MkId(L"e"))));

  TYPE_CPP_Stmt stmt (vdm_BC_GenExpressionStmt(vdm_BC_GenFctCall(
                                                 vdm_BC_GenIdentifier(ASTAUX::MkId(L"System.out.println")),
                                                 SEQ<TYPE_CPP_Expr>().ImpAppend(
                                                   vdm_BC_GenFctCall(
                                                     vdm_BC_GenIdentifier(ASTAUX::MkId(L"e.getMessage")),
                                                     SEQ<TYPE_CPP_Expr>())))));

  SEQ<TYPE_CPP_Stmt> newfb (fb);
  newfb.ImpAppend(vdm_BC_GenExpressionStmt(vdm_BC_GenFctCall(
                                             vdm_BC_GenIdentifier(ASTAUX::MkId(L"setSentinel")),
                                             SEQ<TYPE_CPP_Expr>())));

  SEQ<TYPE_CPP_Handler> handlers;
  handlers.ImpAppend(vdm_BC_GenHandler(expdecl, stmt));
  return SEQ<TYPE_CPP_Stmt>().ImpAppend(vdm_BC_GenTryBlock(newfb, handlers, Nil()));
}

// GenPrePost
// opdef : AS`OpDef
// sd : GenericStateDef
// ==> CPP`CPPAS
TYPE_CPP_CPPAS vdmcg::GenPrePost(const TYPE_AS_ExplOpDef & opdef, const Generic & sd)
{
  TYPE_CPP_CPPAS cpp;
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    if (get_preandpost_option()) {
      if (! opdef.GetField(pos_AS_ExplOpDef_oppre).IsNil()) {
        cpp.ImpConc(GenFctDef_FD(MakePreFct(opdef, sd),false));
      }
    }
  }
  else
#endif // VDMPP
  { // C++
    if (! opdef.GetField(pos_AS_ExplOpDef_oppre).IsNil()) {
      cpp.ImpConc(GenFctDef_FD(MakePreFct(opdef, sd),false));
    }
#ifdef VDMSL
    if (! opdef.GetField(pos_AS_ExplOpDef_oppost).IsNil()) {
      cpp.ImpConc(GenFctDef_FD(MakePostFct(opdef, sd),false));
    }
#endif // VDMSL
  }
  return cpp;
}

// GenArgPatternMatch
// pid_m : map (AS`Name | AS`OldName) to set of REP`TypeRep
// opdef : (AS`ExplOpDef | AS`ExplFnDef)
// varlist : seq of CGMAIN`VT
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenArgPatternMatch(const Map & pid_m,
                                             const Record & opdef,
                                             const SEQ<TYPE_CGMAIN_VT> & varlist)
{
  TYPE_CPP_Identifier succ_v (vdm_BC_GiveName (ASTAUX::MkId(L"succ")));

  SEQ<TYPE_AS_Pattern> l_parms (opdef.Is(TAG_TYPE_AS_ExplFnDef)
                        ? (Sequence)(opdef.GetSequence(pos_AS_ExplFnDef_parms).Hd())
                        : opdef.GetSequence(pos_AS_ExplOpDef_parms) );

  SEQ<TYPE_CGMAIN_VT> var_l (varlist);
  Set p_s;
  SEQ<TYPE_CPP_Stmt> fb (DeclarePatterns(pid_m));

  bool need_succ = false;
  SEQ<TYPE_CPP_Stmt> pattern_fb;
  SEQ<TYPE_CPP_Stmt> tmp_fb;
  size_t len_l_parms = l_parms.Length();
  for (size_t idx = 1; idx <= len_l_parms; idx++)
  {
    const TYPE_AS_Pattern & pat (l_parms[idx]);
    if (! (pat.Is(TAG_TYPE_AS_PatternName) && pat.GetField(pos_AS_PatternName_nm).IsNil()))
    {
      Tuple cgpm (CGPatternMatchExcl(pat, var_l.Hd(), p_s, succ_v, Map(), Nil(), false));
      const SEQ<TYPE_CPP_Stmt> & match (cgpm.GetSequence(1));
      bool Is_Excl = cgpm.GetBoolValue(2);

      Set patId (FindPatternId (pat).Dom ());

      tmp_fb.ImpConc(match);

      if (!Is_Excl)
      {
        if (need_succ)
          pattern_fb.ImpAppend (vdm_BC_GenIfStmt(succ_v, vdm_BC_GenBlock(tmp_fb), nil));
        else
          pattern_fb.ImpConc(tmp_fb);
        tmp_fb.Clear();
      }
      p_s.ImpUnion (patId);

      need_succ = (!Is_Excl) || need_succ;
    }
    var_l.ImpTl ();
  }
  if (!tmp_fb.IsEmpty())
  {
    if (need_succ)
      pattern_fb.ImpAppend (vdm_BC_GenIfStmt(succ_v, vdm_BC_GenBlock(tmp_fb), nil));
    else
      pattern_fb.ImpConc(tmp_fb);
  }

  if (!pattern_fb.IsEmpty())
  {
    wstring errMess = L"Pattern match failed in ";
    if (opdef.Is(TAG_TYPE_AS_ExplOpDef)) {
      errMess = errMess + L"operation call in operation ";
    } else {
      errMess = errMess + L"function apply in function ";
    }
    errMess = errMess + ASTAUX::Id2String(GiveLastName(opdef.GetField(1)));

    TYPE_CPP_Stmt rtiM (vdm_BC_GenBlock(mk_sequence(RunTime(errMess))));

    if (need_succ) {
      pattern_fb.ImpPrepend (vdm_BC_GenDecl (GenSmallBoolType(), succ_v,
                                             vdm_BC_GenAsgnInit (vdm_BC_GenBoolLit(false))));
    }

    fb = MergeStmts(fb, pattern_fb);

    if (need_succ) {
      fb.ImpAppend (vdm_BC_GenIfStmt (vdm_BC_GenNot(succ_v), rtiM, nil));
    }
  }
  else {
    fb = pattern_fb; // FIXME: Mistake in spec
  }
  return fb;
}

// GenPatternBody
// pid_m : map AS`Name to set of REP`TypeRep
// opdef : AS`ExplOpDef
// isDlClass : bool
// arg_l : seq of CPP`ArgDecl
// varlist : seq of CGMAIN`VT
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenPatternBody(const Map & pid_m,
                                         const TYPE_AS_ExplOpDef & opdef,
                                         bool isDlClass,
                                         const SEQ<TYPE_CPP_ArgDecl> & arg_l,
                                         const SEQ<TYPE_CGMAIN_VT> & varlist)
{
  SEQ<TYPE_CPP_Stmt> fb (GenArgPatternMatch(pid_m, opdef, varlist));

  const TYPE_AS_OpBody & body (opdef.GetRecord(pos_AS_ExplOpDef_body));

  SEQ<TYPE_CPP_Stmt> cppast;
  if (vdm_CPP_isCPP() &&
      isDlClass &&
      IsNotyetspecified(body, isDlClass, opdef.get_stat().GetValue())) {
      cppast.ImpConc(Sequence(GenerateDlCall(opdef.get_nm(), arg_l, true,
                              opdef.get_tp().get_oprng().Is(TAG_TYPE_AS_VoidType))));
  }
#ifdef VDMPP
  else if (opdef.GetBoolValue(pos_AS_ExplOpDef_constr)) {
    SetConstr();
    cppast.ImpConc(GenStmt(body.GetField(pos_AS_OpBody_body), true));
    UnsetConstr();
  }
#endif //VDMPP
  else {
    cppast.ImpConc(GenStmt(body.GetField(pos_AS_OpBody_body), true));
  }

  if (Record (body.get_body ()).Is(TAG_TYPE_AS_BlockStmt) &&
      (cppast.Length() == 1) &&
      cppast[1].Is(TAG_TYPE_CPP_CompoundStmt)) {
    fb.ImpConc(cppast[1].GetSequence(pos_CPP_CompoundStmt_stms));
  }
  else {
    fb.ImpConc(cppast);
  }
  return fb;
}

// GenInlineBody
// opdef : AS`OpDef
// rngtype : REP`TypeRep
// isDlClass : bool
// arg_l : seq of CPP`ArgDecl
// isimplicit : bool
// isabstract : bool
// ==> seq of CPP`Stmt * seq of CPP`Stmt
Tuple vdmcg::GenInlineBody(const TYPE_AS_ExplOpDef & opdef,
                           const TYPE_REP_TypeRep & rngtype,
                           bool isDlClass,
                           const SEQ<TYPE_CPP_ArgDecl> & arg_l,
                           bool isimplicit,
                           bool isabstract)
{
  SEQ<TYPE_CPP_Stmt> return_stmt;
  bool has_return_type = false;
  TYPE_AS_Type oprng (opdef.get_tp().get_oprng());
  if (!oprng.Is(TAG_TYPE_AS_VoidType)) {
    has_return_type = true;
    return_stmt.ImpAppend(vdm_BC_GenReturnStmt(GenEmptyValue(rngtype)));
  }

  const TYPE_AS_OpBody & body (opdef.GetRecord(pos_AS_ExplOpDef_body));
  bool stat = opdef.GetBoolValue(pos_AS_ExplOpDef_stat);

  wstring prelimOrImpl;
  if (IsNotyetspecified(body, isDlClass, stat)) {
    prelimOrImpl = L"Preliminary Operation ";
  }
  else {
    prelimOrImpl = L"Implicit Operation ";
  }

  TYPE_CPP_Stmt fc (RunTime(prelimOrImpl + PTAUX::Seq2Str(TYPE_CPP_Identifier(vdm_BC_Rename(opdef.get_nm())).get_id())
                                         + L" has been called"));

  SEQ<TYPE_CPP_Stmt> implfb;
  implfb.ImpAppend(fc);
  implfb.ImpConc(return_stmt);

  SEQ<TYPE_CPP_Stmt> fb;

#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    if (isimplicit) {
      SEQ<TYPE_CPP_Expr> exprlist (FindVariables(arg_l));
      TYPE_CPP_Identifier obj (vdm_BC_GenIdentifier(ASTAUX::MkId(L"child")));
      TYPE_CPP_ExpressionStmt fctcall (vdm_BC_GenFctCallObjMemAccStmt(
                                         obj,
                                         ASTAUX::MkId(L"impl_").ImpConc(vdm_BC_Rename(opdef.get_nm()).GetField(1)),
                                         exprlist));
      // Note that the spec is ludicrous here!
      if (has_return_type) {
        fb.ImpAppend(vdm_BC_GenReturnStmt(fctcall.get_expr()));
      }
      else {
        fb.ImpAppend(fctcall);
      }
    }
    else { // !isimplicit
      if (opdef.GetBoolValue(pos_AS_ExplOpDef_constr)) {
        // for constructor of C++ and java
        SetConstr();
        fb.ImpConc(GenStmt(body.get_body(), true));
        UnsetConstr();
      }
      else if (!isabstract && !get_skeleton_option()) {
        fb.ImpConc(GenStmt(body.get_body (), true));
        IsSafeReturn(body, rngtype);
      }
    }
  }
  else 
#endif //VDMPP
  { // C++
    if (isDlClass && IsNotyetspecified(body, isDlClass, stat)) {
      // for dlclass of C++
      fb.ImpConc(Sequence(GenerateDlCall(opdef.get_nm(), arg_l,
                        true, opdef.get_tp().get_oprng().Is(TAG_TYPE_AS_VoidType))));
    }
    else {
#ifdef VDMPP
      if (opdef.GetBoolValue(pos_AS_ExplOpDef_constr)) {
        // for constructor of C++ and java
        SetConstr();
        fb.ImpConc(GenStmt(body.get_body(), true));
        UnsetConstr();
      }
      else
#endif //VDMPP
      {
        fb.ImpConc(GenStmt(body.get_body (), true));
      }
    }
  }
  return mk_(implfb, fb);
}

// GenMethPreCall
// nm : AS`Name
// args : seq of CPP`Expr
// stat : bool
// -> CPP`Stmt
TYPE_CPP_Stmt vdmcg::GenMethPreCall(const TYPE_AS_Name & nm, const SEQ<TYPE_CPP_Expr> & args, bool stat)
{
#ifdef VDMSL
  TYPE_CPP_Name prename (vdm_BC_GivePrePostNm(nm, ASTAUX::MkId(L"pre")));
  TYPE_CPP_Expr preref(vdm_BC_GenFctCall(prename, args));
#endif // VDMSL
#ifdef VDMPP
  TYPE_AS_Id preid (ASTAUX::MkId(L"pre_").ImpConc(ASTAUX::GetFirstId(nm)));
  TYPE_CPP_Name prename (vdm_BC_Rename(ASTAUX::MkNameFromId(preid, nm.get_cid())));

  TYPE_CPP_Expr preref;
  if (stat) {
    preref = vdm_BC_GenFctCall(prename, args);
  }
  else {
    if (vdm_CPP_isCPP()) {
      preref = vdm_BC_GenFctCallPtrToObjMemAcc(GenThis(), prename, args);
    } else { // java
      preref = vdm_BC_GenFctCall(vdm_BC_GenObjectMemberAccess(GenThis(), prename), args);
    }
  }
#endif // VDMPP

  TYPE_CPP_Expr cond (GenGetValue(preref, mk_REP_BooleanTypeRep()));
  return vdm_BC_GenIfStmt(vdm_BC_GenNot(cond),
         vdm_BC_GenBlock(mk_sequence(RunTime(L"Precondition failure in " + ASTAUX::ASName2String(nm)))), nil);
}

// GenFnPostCall
// nm : AS`Name
// res : CPP`Expr
// args : seq of CPP`Expr
// stat : bool
// -> CPP`Stmt
TYPE_CPP_Stmt vdmcg::GenFnPostCall(const TYPE_AS_Name & nm,
                                   const TYPE_CPP_Expr & res,
                                   const SEQ<TYPE_CPP_Expr> & args,
                                   bool stat)
{
  SEQ<TYPE_CPP_Expr> parms (args);
  parms.ImpAppend(res);

#ifdef VDMSL
  TYPE_CPP_Name postname (vdm_BC_GivePrePostNm(nm, ASTAUX::MkId(L"post")));
  TYPE_CPP_Expr postref (vdm_BC_GenFctCall(postname, parms));
#endif // VDMSL
#ifdef VDMPP
  TYPE_AS_Id postid (ASTAUX::MkId(L"post_").ImpConc(ASTAUX::GetFirstId(nm)));
  TYPE_CPP_Name postname (vdm_BC_Rename(ASTAUX::MkNameFromId(postid, nm.get_cid())));

  TYPE_CPP_Expr postref;
  if (stat) {
    postref = vdm_BC_GenFctCall(postname, parms);
  }
  else {
    if (vdm_CPP_isCPP()) {
      postref = vdm_BC_GenFctCallPtrToObjMemAcc(GenThis(), postname, parms);
    }
    else { // java
      postref = vdm_BC_GenFctCall(vdm_BC_GenObjectMemberAccess(GenThis(), postname), parms);
    }
  }
#endif // VDMPP
  TYPE_CPP_Expr cond (GenGetValue(postref, mk_REP_BooleanTypeRep()));
  return vdm_BC_GenIfStmt(vdm_BC_GenNot(cond),
        vdm_BC_GenBlock(mk_sequence(RunTime(L"Postcondition failure in " + ASTAUX::ASName2String(nm)))), nil);
}

// GenImplDef
// fndef : (AS`ImplFnDef | AS`ImplOpDef)
// sd : GenericStateDef
// ==> CPP`CPPAS
TYPE_CPP_CPPAS vdmcg::GenImplDef(const Record & fndef, const Generic & sd)
{
  TYPE_CPP_CPPAS cpp;
  vdm_BC_ResetVarno();
  this->impl = true;

#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    switch(fndef.GetTag()) {
      case TAG_TYPE_AS_ImplFnDef: {
        const TYPE_AS_Name & nm (fndef.GetRecord(pos_AS_ImplFnDef_nm));
        const SEQ<TYPE_AS_TypeVar> & tvars (fndef.GetSequence(pos_AS_ImplFnDef_params));
        const SEQ<TYPE_AS_PatTypePair> & partps (fndef.GetSequence(pos_AS_ImplFnDef_partps));
        const SEQ<TYPE_AS_NameType> & resnmtp (fndef.GetSequence(pos_AS_ImplFnDef_resnmtps));
        const TYPE_AS_Access & access (fndef.GetField(pos_AS_ImplFnDef_access));
        const Bool & stat (fndef.GetBool(pos_AS_ImplFnDef_stat));
        const TYPE_CI_ContextId & cid (fndef.GetInt(pos_AS_ImplFnDef_cid));

        // ==> AS`Parameters * seq of AS`Type * [AS`Type]
        Tuple t (Impl2ExplSignature(partps, resnmtp));
        const SEQ<TYPE_AS_Pattern> & parms (t.GetSequence(1));
        const SEQ<TYPE_AS_Type> & fndom (t.GetSequence(2)); // seq of AS`Type
        const Generic & fnrng (t.GetField(3));

        TYPE_AS_PartialFnType fntp;
        fntp.Init(fndom, fnrng, NilContextId);

        TYPE_AS_FnBody body;
        body.Init(Int(NOTYETSPEC), NilContextId);

        TYPE_AS_ExplFnDef fd;
        fd.Init(nm,
                tvars,
                fntp,
                SEQ<TYPE_AS_Parameters>().ImpAppend(parms),
                body,
                Nil(),
                Nil(),
                access,
                stat,
                Nil(),
                cid);
        cpp.ImpConc(GenExplFctDef(fd, true, false));
        break;
      }
      case TAG_TYPE_AS_ImplOpDef: {
        const TYPE_AS_Name & nm (fndef.GetRecord(pos_AS_ImplOpDef_nm));
        const Bool & pu (fndef.GetBool(pos_AS_ImplOpDef_oppure));
//        Bool sync (fndef.GetBool(pos_AS_ImplOpDef_opsync));
        const SEQ<TYPE_AS_PatTypePair> & partps (fndef.GetSequence(pos_AS_ImplOpDef_partps));
        const SEQ<TYPE_AS_NameType> & resnmtp (fndef.GetSequence(pos_AS_ImplOpDef_resnmtps));
        const TYPE_AS_Access & access (fndef.GetField(pos_AS_ImplOpDef_access));
        const Bool & stat (fndef.GetBool(pos_AS_ImplOpDef_stat));
        const Bool & constr (fndef.GetBool(pos_AS_ImplOpDef_constr));
        const TYPE_CI_ContextId & cid (fndef.GetInt(pos_AS_ImplOpDef_cid));

        // ==> AS`Parameters * seq of AS`Type * [AS`Type]
        Tuple t (Impl2ExplSignature(partps,resnmtp));
        const SEQ<TYPE_AS_Pattern> & parms (t.GetSequence(1));
        const SEQ<TYPE_AS_Type> & fndom (t.GetSequence(2));
        const Generic & fnrng (t.GetField(3));

        TYPE_AS_OpType optp;
        optp.Init(fndom, fnrng, NilContextId);

        TYPE_AS_OpBody body;
        body.Init(TYPE_AS_ErrorStmt().Init(NilContextId), NilContextId);

        TYPE_AS_ExplOpDef op;
        op.Init(nm,pu,Bool(true),optp,parms,body,nil,nil,access,stat,constr,cid);
        cpp.ImpConc(GenExplOpDef(nm, op, sd, true, false));
        break;
      }
    }
  }
#endif // VDMPP

  Generic fnpre;
  switch(fndef.GetTag()) {
    case TAG_TYPE_AS_ImplFnDef: {
      fnpre = fndef.GetField(pos_AS_ImplFnDef_fnpre);
      break;
    }
    case TAG_TYPE_AS_ImplOpDef: {
      fnpre = fndef.GetField(pos_AS_ImplOpDef_oppre);
      break;
    }
  }

  if (!fnpre.IsNil()) {
    cpp.ImpConc(GenFctDef_FD(MakePreFct(fndef, sd), false));
  }

#ifdef VDMPP
  if (fndef.Is(TAG_TYPE_AS_ImplFnDef))
#endif //VDMPP
  {
      cpp.ImpConc(GenFctDef_FD(MakePostFct(fndef, sd), false));
  }
  return cpp;
}

//Sequence vdmcg::IncludeImplicit()
//{
//if (this->impl) {
//  wstring id = L"vdm_" + GiveCurCName().GetValue();
//  this->impl = false;
// Sequence res;
//  res.ImpAppend(GenInclusion(GiveExtFileName(id, L"implicit")));
//  return res;
// }
//else
//  return eseq;
//}

// IncludeExternalCC
// ==> CPP`CPPAS
TYPE_CPP_CPPAS vdmcg::IncludeExternalCC()
{
  TYPE_AS_Id nm (GiveCurCName());
  TYPE_AS_Id tmp (nm);
  TYPE_CPP_IncludeStmt incl (vdm_BC_GenInclStmt(GenFileExt(tmp.ImpConc(Userimpl_Suffix))));
  TYPE_CPP_Identifier defexpr (vdm_BC_GenIdentifier(ASTAUX::MkId(L"DEF_").ImpConc(nm).ImpConc(Userimpl_Flag)));
  TYPE_CPP_PreIfDef preif (vdm_BC_GenPreIfDef(defexpr));
  TYPE_CPP_PreEndIf preend (vdm_BC_GenPreEndIf(defexpr));

  if (this->impl) {
    this->impl = false;
    return TYPE_CPP_CPPAS().ImpAppend(incl);
  }
  else {
    return TYPE_CPP_CPPAS().ImpAppend(preif).ImpAppend(incl).ImpAppend(preend);
  }
}

// GenFctOpDecl
// mnm : AS`Name
// df : (AS`FnDef | AS`OpDef)
// scs : ([CPP`StorageClassSpecifier] | [ CPP`Modifier])
// sd : GenericStateDef
// ==> CPP`CPPAS | seq of CPP`MemberDeclaration
Sequence vdmcg::GenFctOpDecl(const TYPE_AS_Name& mnm, const Record & df, const Generic & scs, const Generic & sd)
{
  Sequence cpp;

#ifdef VDMSL
  TYPE_AS_Name nm (mnm);
#endif // VDMSL
#ifdef VDMPP
  TYPE_AS_Name nm (MANGLE::IsMangled(mnm) ? MANGLE::GetUnmangledName(mnm) : mnm);
#endif // VDMPP

  if (df.Is(TAG_TYPE_AS_ExplFnDef)) {
    SEQ<TYPE_AS_Parameters> parms (df.GetSequence(pos_AS_ExplFnDef_parms));
    if (parms.Length() != 1) {
      wstring s (L"higher order function definition (" + ASTAUX::Id2String(nm.get_ids().Hd()) + L")");
      TYPE_CPP_Stmt notused (NotSupported(s, nm));
      notused = Record(); // To awoid warnings
      return Sequence();
    }
  }

  // ==> seq of REP`TypeRep * REP`TypeRep * seq of AS`Type * [AS`Type]
  Tuple fdrt (FindDomRngTypes(df));
  const SEQ<TYPE_REP_TypeRep> & domtp_l (fdrt.GetSequence(1));
  const TYPE_REP_TypeRep & rngtp (fdrt.GetRecord(2));
  const SEQ<TYPE_AS_Type> & domatp_l (fdrt.GetSequence(3));
  const Generic & rngatp (fdrt.GetField(4));

  TYPE_CPP_Expr id (vdm_BC_Rename(nm));
  TYPE_CPP_RefAbsDecl ref (vdm_BC_GenRefAbsDecl(nil, nil));

  type_dL argdecl_l;
  size_t len_domtp_l = domtp_l.Length();
  for (size_t i = 1; i <= len_domtp_l; i++)
  {
    SEQ<TYPE_CPP_DeclSpecifier> decl;
    decl.ImpAppend( vdm_BC_GenTypeSpecifier(quote_CONST));
    decl.ImpAppend( Rep2CppTypeSpecifier(domtp_l[i], domatp_l[i]) );
    argdecl_l.ImpAppend(vdm_BC_GenArgAbs(decl, ref));
  }

  TYPE_CPP_TypeSpecifier ds (Rep2CppTypeSpecifier(rngtp, rngatp));

  TYPE_CPP_FctDecl dl (vdm_BC_GenFctDecl(id, argdecl_l));

  type_dL scs_l;
  if (! scs.IsNil()) {
    scs_l.ImpAppend(scs);
  }

  TYPE_CPP_TypeSpecifier rtp (GenBoolType());

  Sequence dsarg_l;
  if (!rngtp.Is(TAG_TYPE_REP_UnitTypeRep))
  {
    SEQ<TYPE_CPP_DeclSpecifier> decl;
    decl.ImpAppend(vdm_BC_GenTypeSpecifier(quote_CONST));
    decl.ImpAppend(ds);
    dsarg_l.ImpAppend(vdm_BC_GenArgAbs(decl, ref));
  }

#ifdef VDMSL
  type_dL ds_l (scs_l);
  ds_l.ImpAppend (ds);
  cpp.ImpAppend (vdm_BC_GenIdentDecl(SEQ<TYPE_CPP_Annotation>(), ds_l, dl, nil));

  ds_l = scs_l;
  ds_l.ImpAppend (rtp);
#endif // VDMSL

#ifdef VDMPP
  bool stat = false;
  type_dL ds_l (scs_l);
  ds_l.ImpAppend (vdm_BC_GenFctSpecifier(quote_VIRTUAL));
  ds_l.ImpAppend (ds);

  bool constr = false;
  bool pure = false;
  switch(df.GetTag()) {
    case TAG_TYPE_AS_ExplOpDef: {
      stat = df.GetBool(pos_AS_ExplOpDef_stat);
      pure = IsAbstractOperation(df);
      constr = df.GetBool(pos_AS_ExplOpDef_constr);
      break;
    }
    case TAG_TYPE_AS_ImplOpDef: {
      stat = df.GetBool(pos_AS_ImplOpDef_stat);
      pure = IsAbstractOperation(df);
      constr = df.GetBool(pos_AS_ImplOpDef_constr);
      break;
    }
    case TAG_TYPE_AS_ExtExplOpDef: {
      stat = df.GetBool(pos_AS_ExtExplOpDef_stat);
      pure = IsAbstractOperation(df);
      constr = df.GetBool(pos_AS_ExtExplOpDef_constr);
      break;
    }
    case TAG_TYPE_AS_ExplFnDef: {
      stat = df.GetBool(pos_AS_ExplFnDef_stat);
      pure = IsAbstractFunction(df);
      break;
    }
    case TAG_TYPE_AS_ImplFnDef: {
      stat = df.GetBool(pos_AS_ImplFnDef_stat);
      pure = IsAbstractFunction(df);
      break;
    }
    case TAG_TYPE_AS_ExtExplFnDef: {
      stat = df.GetBool(pos_AS_ExtExplFnDef_stat);
      pure = IsAbstractFunction(df);
      break;
    }
  }

  if (constr)
    cpp.ImpAppend (vdm_BC_GenMemberSpec(SEQ<TYPE_CPP_DeclSpecifier>(), dl));
  else if (stat)
    cpp.ImpAppend (vdm_BC_GenMemberSpec(type_dL().ImpConc(scs_l)
                                                 .ImpAppend(vdm_BC_GenStorageClassSpecifier(quote_STATIC))
                                                 .ImpAppend(ds),
                                        dl));
  else if (pure)
    cpp.ImpAppend (vdm_BC_GenPureMemberSpec(ds_l, dl));
  else
    cpp.ImpAppend (vdm_BC_GenMemberSpec(ds_l, dl));

  SEQ<TYPE_CPP_StorageClassSpecifier> statMod;
  if (stat)
    statMod.ImpAppend(vdm_BC_GenStorageClassSpecifier(quote_STATIC));

  ds_l = scs_l;
  ds_l.ImpConc(statMod);
  ds_l.ImpAppend (rtp);
#endif //VDMPP

  TYPE_CPP_Name prenm (vdm_BC_GivePrePostNm(nm, ASTAUX::MkId(L"pre")));
  TYPE_CPP_Name postnm (vdm_BC_GivePrePostNm(nm, ASTAUX::MkId(L"post")));

  switch (df.GetTag()) {
    case TAG_TYPE_AS_ExtExplFnDef:
    case TAG_TYPE_AS_ExplFnDef:
    case TAG_TYPE_AS_ImplFnDef: {
      Generic fnpre (ASTAUX::GetFnPrecond(df));
      Generic fnpost (ASTAUX::GetFnPostcond(df));
      if (! fnpre.IsNil())
#ifdef VDMSL
        cpp.ImpAppend(vdm_BC_GenIdentDecl(SEQ<TYPE_CPP_Annotation>(),
                                          ds_l, vdm_BC_GenFctDecl(prenm, argdecl_l), nil));
#endif //VDMSL
#ifdef VDMPP
        cpp.ImpAppend(vdm_BC_GenMemberSpec(ds_l, vdm_BC_GenFctDecl(prenm, argdecl_l)));
#endif //VDMPP
      if (! fnpost.IsNil()) {
        argdecl_l.ImpConc(dsarg_l);
#ifdef VDMSL
        cpp.ImpAppend(vdm_BC_GenIdentDecl(SEQ<TYPE_CPP_Annotation>(),
                                          ds_l, vdm_BC_GenFctDecl(postnm, argdecl_l), nil));
#endif //VDMSL
#ifdef VDMPP
        cpp.ImpAppend(vdm_BC_GenMemberSpec(ds_l, vdm_BC_GenFctDecl(postnm, argdecl_l)));
#endif //VDMPP
      }
      break;
    }
    case TAG_TYPE_AS_ExtExplOpDef:
    case TAG_TYPE_AS_ImplOpDef:
    case TAG_TYPE_AS_ExplOpDef: {
      Generic fnpre (ASTAUX::GetOpPrecond(df));
      Generic fnpost (ASTAUX::GetOpPostcond(df));
      Sequence sdtp_l;
#ifdef VDMSL
      if (! sd.IsNil()) {
        SEQ<TYPE_CPP_DeclSpecifier> decl;
        decl.ImpAppend(vdm_BC_GenTypeSpecifier(quote_CONST));
        decl.ImpAppend(FindStateType(sd));
        sdtp_l.ImpAppend(vdm_BC_GenArgAbs(decl, ref));
      }
#endif // VDMSL
      if (! fnpre.IsNil()) {
        type_dL arg_l(argdecl_l);
        arg_l.ImpConc(sdtp_l);
#ifdef VDMSL
        cpp.ImpAppend(vdm_BC_GenIdentDecl(SEQ<TYPE_CPP_Annotation>(),
                                          ds_l, vdm_BC_GenFctDecl(prenm, arg_l), nil));
#endif //VDMSL
#ifdef VDMPP
        cpp.ImpAppend(vdm_BC_GenMemberSpec(ds_l, vdm_BC_GenFctDecl(prenm, arg_l)));
#endif //VDMPP
      }
      if (! fnpost.IsNil()) {
        if (! rngtp.Is(TAG_TYPE_REP_UnitTypeRep))
          argdecl_l.ImpConc(dsarg_l);

        argdecl_l.ImpConc(sdtp_l).ImpConc(sdtp_l);
#ifdef VDMSL
        cpp.ImpAppend(vdm_BC_GenIdentDecl(SEQ<TYPE_CPP_Annotation>(),
                                          ds_l, vdm_BC_GenFctDecl(postnm, argdecl_l), nil));
#endif //VDMSL
#ifdef VDMPP
        cpp.ImpAppend(vdm_BC_GenMemberSpec(ds_l, vdm_BC_GenFctDecl(postnm, argdecl_l)));
#endif //VDMPP
      }
    }
  }
  return cpp;
}

#ifdef VDMSL
// GenExpFctOpDecl
// mnm : AS`Name
// df : (AS`FnDef | AS`OpDef)
// scs : ([CPP`StorageClassSpecifier] | [ CPP`Modifier])
// sd : GenericStateDef
// md : AS`Module
// ==> CPP`CPPAS | seq of CPP`MemberDeclaration
Sequence vdmcg::GenExpFctOpDecl(const TYPE_AS_Name& mnm, const Record & df, const Generic & scs,
                                const Generic & sd, const TYPE_AS_Module & md)
{
  Sequence cpp;
  if (df.Is(TAG_TYPE_AS_ExplFnDef))
    if (df.GetSequence(pos_AS_ExplFnDef_parms).Length() != 1)
      return cpp;

  TYPE_AS_Name nm (mnm);
  const TYPE_AS_Interface & intf (md.GetRecord(pos_AS_Module_intf)); 
  const Map & importMap (intf.GetMap(pos_AS_Interface_imp)); // map Name to [ImportSig]
  Set dom_importMap(importMap.Dom());
  Map renamingMap;
  Generic j;
  for (bool bb = dom_importMap.First(j); bb; bb = dom_importMap.Next(j))
  {
    Generic is_g (importMap[j]);
    if (!is_g.IsNil())
    {
      TYPE_AS_ImportSig is (is_g);
      const Map & ren (is.GetMap(pos_AS_ImportSig_ren)); // map Name to Name
      Set dom_ren (ren.Dom());
      Generic i;
      for (bool cc = dom_ren.First(i); cc; cc = dom_ren.Next(i))
      {
        renamingMap.ImpModify(i, ASTAUX::Combine2Names(j, ren[i]));
      }
    } 
  }
  Tuple fedrt (FindExpDomRngTypes(df, renamingMap)); // seq of REP`TypeRep * REP`TypeRep * seq of AS`Type * [AS`Type]
  const SEQ<TYPE_REP_TypeRep> & domtp_l (fedrt.GetSequence(1));
  const TYPE_REP_TypeRep & rngtp (fedrt.GetRecord(2));
  const SEQ<TYPE_AS_Type> & domatp_l (fedrt.GetSequence(3));
  const Generic & rngatp (fedrt.GetField(4));
  TYPE_CPP_Expr id (vdm_BC_Rename(nm));
  TYPE_CPP_RefAbsDecl ref (vdm_BC_GenRefAbsDecl(nil, nil));
  
  type_dL argdecl_l;
  size_t len_domtp_l = domtp_l.Length();
  for (size_t i = 1; i <= len_domtp_l; i++)
  {
    SEQ<TYPE_CPP_DeclSpecifier> decl;
    decl.ImpAppend( vdm_BC_GenTypeSpecifier(quote_CONST));
    decl.ImpAppend( Rep2CppTypeSpecifier(domtp_l[i], domatp_l[i]) );
    argdecl_l.ImpAppend(vdm_BC_GenArgAbs(decl, ref));
  }

  TYPE_CPP_TypeSpecifier ds (Rep2CppTypeSpecifier(rngtp, rngatp));

  TYPE_CPP_FctDecl dl (vdm_BC_GenFctDecl(id, argdecl_l));

  type_dL scs_l;
  if (! scs.IsNil())
    scs_l.ImpAppend(scs);

  TYPE_CPP_TypeSpecifier rtp (GenBoolType());

  Sequence dsarg_l;
  if (! rngtp.Is(TAG_TYPE_REP_UnitTypeRep))
  {
    SEQ<TYPE_CPP_DeclSpecifier> decl;
    decl.ImpAppend(vdm_BC_GenTypeSpecifier(quote_CONST));
    decl.ImpAppend(ds);
    dsarg_l.ImpAppend(vdm_BC_GenArgAbs(decl, ref));
  }

  TYPE_CPP_Name prenm(vdm_BC_GivePrePostNm(nm, ASTAUX::MkId(L"pre")));
  TYPE_CPP_Name postnm(vdm_BC_GivePrePostNm(nm, ASTAUX::MkId(L"post")));

  type_dL ds_l (scs_l);
  ds_l.ImpAppend (ds);
  cpp.ImpAppend (vdm_BC_GenIdentDecl(SEQ<TYPE_CPP_Annotation>(), ds_l, dl, nil));

  bool stat = false;
  switch (df.GetTag()) {
    case TAG_TYPE_AS_ExtExplFnDef: { stat = df.GetBool(pos_AS_ExtExplFnDef_stat); break; }
    case TAG_TYPE_AS_ExplFnDef:    { stat = df.GetBool(pos_AS_ExplFnDef_stat); break; }
    case TAG_TYPE_AS_ImplFnDef:    { stat = df.GetBool(pos_AS_ImplFnDef_stat); break; }
    case TAG_TYPE_AS_ExtExplOpDef: { stat = df.GetBool(pos_AS_ExtExplOpDef_stat); break; }
    case TAG_TYPE_AS_ImplOpDef:    { stat = df.GetBool(pos_AS_ImplOpDef_stat); break; }
    case TAG_TYPE_AS_ExplOpDef:    { stat = df.GetBool(pos_AS_ExplOpDef_stat); break; }
  }

  SEQ<TYPE_CPP_StorageClassSpecifier> statMod;
  if (stat)
    statMod.ImpAppend(vdm_BC_GenStorageClassSpecifier(quote_STATIC));

  ds_l = scs_l;
  ds_l.ImpConc(statMod);
  ds_l.ImpAppend (rtp);

  switch (df.GetTag()) {
    case TAG_TYPE_AS_ExtExplFnDef:
    case TAG_TYPE_AS_ExplFnDef:
    case TAG_TYPE_AS_ImplFnDef: {
      Generic fnpre (ASTAUX::GetFnPrecond(df));
      Generic fnpost (ASTAUX::GetFnPostcond(df));
      if (! fnpre.IsNil())
        cpp.ImpAppend(vdm_BC_GenIdentDecl(SEQ<TYPE_CPP_Annotation>(),
                                          ds_l, vdm_BC_GenFctDecl(prenm, argdecl_l), nil));
      if (! fnpost.IsNil()) {
        argdecl_l.ImpConc(dsarg_l);
        cpp.ImpAppend(vdm_BC_GenIdentDecl(SEQ<TYPE_CPP_Annotation>(),
                                          ds_l, vdm_BC_GenFctDecl(postnm, argdecl_l), nil));
      }
      break;
    }
    case TAG_TYPE_AS_ExtExplOpDef:
    case TAG_TYPE_AS_ImplOpDef:
    case TAG_TYPE_AS_ExplOpDef: {
      Generic fnpre (ASTAUX::GetOpPrecond(df));
      Generic fnpost (ASTAUX::GetOpPostcond(df));
      Sequence sdtp_l;
      if (! sd.IsNil()) {
        SEQ<TYPE_CPP_DeclSpecifier> decl;
        decl.ImpAppend(vdm_BC_GenTypeSpecifier(quote_CONST));
        decl.ImpAppend(FindStateType(sd));
        sdtp_l.ImpAppend(vdm_BC_GenArgAbs(decl, ref));
      }
      if (! fnpre.IsNil()) {
        type_dL arg_l(argdecl_l);
        arg_l.ImpConc(sdtp_l);
        cpp.ImpAppend(vdm_BC_GenIdentDecl(SEQ<TYPE_CPP_Annotation>(),
                                          ds_l, vdm_BC_GenFctDecl(prenm, arg_l), nil));
      }
      if (! fnpost.IsNil()) {
        if (! rngtp.Is(TAG_TYPE_REP_UnitTypeRep))
          argdecl_l.ImpConc(dsarg_l);

        argdecl_l.ImpConc(sdtp_l).ImpConc(sdtp_l);
        cpp.ImpAppend(vdm_BC_GenIdentDecl(SEQ<TYPE_CPP_Annotation>(),
                                          ds_l, vdm_BC_GenFctDecl(postnm, argdecl_l), nil));
      }
    }
  }
  return cpp;
}
#endif // VDMSL

#ifdef VDMPP
// GenHiddenFctDecl
// p_mangledNm : AS`Name
// p_retTp : [AS`Type]
// ==> CPP`CPPAS
TYPE_CPP_CPPAS vdmcg::GenHiddenFctDecl(const TYPE_AS_Name & p_mangledNm, const Generic & p_retTp)
{
  Tuple t (MANGLE::UnMangle(p_mangledNm));
  const TYPE_AS_Name & l_qualFctNm (t.GetRecord(1));
  const SEQ<TYPE_AS_Type> & l_fctTp (t.GetSequence(3));

  TYPE_REP_TypeRep l_rngtype;
  if (p_retTp.IsNil())
    l_rngtype = TYPE_REP_UnitTypeRep();
  else
    l_rngtype = FromAS2RepType(p_retTp);

  TYPE_AS_Name l_fctNm (l_qualFctNm);
  l_fctNm.set_ids(l_fctNm.get_ids().Tl());
  TYPE_AS_Name l_cnm (ASTAUX::MkNameFromId(GiveCurCName(), NilContextId));
  TYPE_CPP_Expr l_cid (vdm_BC_Rename(l_cnm));
  TYPE_CPP_Expr l_fnm (vdm_BC_Rename(l_fctNm));

  SEQ<TYPE_AS_Pattern> l_params;
  SEQ<TYPE_CPP_Expr> l_argVals;
  size_t len_l_fctTp = l_fctTp.Length();
  for (size_t i = 1; i <= len_l_fctTp; i++)
  {
    Int I(i);
    TYPE_AS_Name pNm (ASTAUX::MkNameFromId(ASTAUX::MkId(L"p_").ImpConc(ASTAUX::MkId(I.ascii())), NilContextId));
    l_params.ImpAppend(TYPE_AS_PatternName().Init(pNm, Nil(), NilContextId));
    l_argVals.ImpAppend(vdm_BC_Rename(pNm));
  }

  SEQ<TYPE_CPP_ArgumentDeclaration> l_argl (DeclPatArg(Map(), l_params, l_fctTp));

  SEQ<TYPE_CPP_DeclSpecifier> ds;
  ds.ImpAppend(Rep2CppTypeSpecifier(l_rngtype, p_retTp));

  TYPE_CPP_FctDecl decl (vdm_BC_GenFctDecl(l_fnm, l_argl));
  TYPE_CPP_Expr fcall (vdm_BC_GenGenericFctCall(vdm_BC_Rename(l_qualFctNm), l_argVals));
  TYPE_CPP_Stmt fb (vdm_BC_GenReturnStmt(fcall));
  return TYPE_CPP_CPPAS().ImpAppend(vdm_BC_GenFctDef(ds, decl,
                                                     nil,
                                                     vdm_BC_GenBlock(type_dL().ImpAppend(fb))));
}
#endif // VDMPP

#ifdef VDMSL
// FindStateType
// sd : GenericStateDef
// ==> CPP`TypeSpecifier
TYPE_CPP_TypeSpecifier vdmcg::FindStateType(const Generic & sd)
{
  TYPE_AS_StateDef sd_ (sd);
  const TYPE_AS_CompositeType & tp (sd_.GetRecord(pos_AS_StateDef_tp));
  return Rep2CppTypeSpecifier(FromAS2RepType(tp), tp);
}
#endif // VDMSL

// FindDomRngTypes
// df : AS`FnDef | AS`OpDef
// ==> seq of REP`TypeRep * REP`TypeRep * seq of AS`Type * [AS`Type]
Tuple vdmcg::FindDomRngTypes(const Record & df)
{
  switch (df.GetTag()) {
    case TAG_TYPE_AS_ExplFnDef: {
      const TYPE_AS_FnType & tp (df.GetRecord(pos_AS_ExplFnDef_tp));
      const SEQ<TYPE_AS_Type> & fndom (tp.GetSequence(1));
      const TYPE_AS_Type & fnrng (tp.GetRecord(2));
      SEQ<TYPE_REP_TypeRep> dt_l;
      size_t len_fndom = fndom.Length();
      for (size_t idx = 1; idx <= len_fndom; idx++)
        dt_l.ImpAppend(FromAS2RepType(fndom[idx]));
      return mk_(dt_l, FromAS2RepType(fnrng), fndom, fnrng);
    }
    case TAG_TYPE_AS_ExplOpDef: {
      const TYPE_AS_OpType & tp (df.GetRecord(pos_AS_ExplOpDef_tp));
      const SEQ<TYPE_AS_Type> & opdom (tp.GetSequence(pos_AS_OpType_opdom));
      const TYPE_AS_Type & oprng (tp.GetRecord(pos_AS_OpType_oprng));
      SEQ<TYPE_REP_TypeRep> dt_l;
      size_t len_opdom = opdom.Length();
      for (size_t idx = 1; idx <= len_opdom; idx++)
        dt_l.ImpAppend(FromAS2RepType(opdom[idx]));
      return mk_(dt_l, FromAS2RepType(oprng), opdom, oprng);
    }
    default: {
      SEQ<TYPE_AS_NameType> resnmtp_l; // seq of AS`NameType
      SEQ<TYPE_AS_PatTypePair> partps; // seq of AS`PatTypePair

      switch (df.GetTag()) {
        case TAG_TYPE_AS_ImplFnDef: {
          resnmtp_l.ImpConc(df.GetSequence(pos_AS_ImplFnDef_resnmtps));
          partps.ImpConc(df.GetSequence(pos_AS_ImplFnDef_partps));
          break;
        }
        case TAG_TYPE_AS_ExtExplFnDef: {
          resnmtp_l.ImpConc(df.GetSequence(pos_AS_ExtExplFnDef_resnmtps));
          partps.ImpConc(df.GetSequence(pos_AS_ExtExplFnDef_partps));
          break;
        }
        case TAG_TYPE_AS_ImplOpDef: {
          resnmtp_l.ImpConc(df.GetSequence(pos_AS_ImplOpDef_resnmtps));
          partps.ImpConc(df.GetSequence(pos_AS_ImplOpDef_partps));
          break;
        }
        case TAG_TYPE_AS_ExtExplOpDef: {
          resnmtp_l.ImpConc(df.GetSequence(pos_AS_ExtExplOpDef_resnmtps));
          partps.ImpConc(df.GetSequence(pos_AS_ExtExplOpDef_partps));
          break;
        }
      }

      TYPE_REP_TypeRep rt;
      Generic atp;
      switch(resnmtp_l.Length()) {
        case 0: {
          rt = TYPE_REP_UnitTypeRep();
          atp = nil;
          break;
        }
        case 1: {
          const TYPE_AS_Type & tp (resnmtp_l[1].GetRecord(pos_AS_NameType_tp));
          rt = FromAS2RepType(tp);
          atp = tp; // 20070824
          break;
        }
        default: {
          SEQ<TYPE_REP_TypeRep> tp_l;
          SEQ<TYPE_AS_Type> artpl;
          size_t len_resnmtp_l = resnmtp_l.Length();
          for (size_t idx = 1; idx <= len_resnmtp_l; idx++)
          {
            const TYPE_AS_Type & tp (resnmtp_l[idx].GetRecord(pos_AS_NameType_tp));
            tp_l.ImpAppend(FromAS2RepType(tp));
            artpl.ImpAppend(tp);
          }

          rt = TYPE_REP_ProductTypeRep().Init(tp_l);
          atp = TYPE_AS_ProductType().Init(artpl, NilContextId);
          break;
        }
      }

      Tuple fidt (FindImplDomType(partps)); // seq of REP`TypeRep * seq of AS`Type
      const SEQ<TYPE_REP_TypeRep> & rtp_l (fidt.GetSequence(1));
      const SEQ<TYPE_AS_Type> & atp_l (fidt.GetSequence(2));

      return mk_(rtp_l, rt, atp_l, atp);
    }
  }
  return Tuple(0); // To avoid warnings
}

#ifdef VDMSL
// FindExpDomRngTypes
// df : (AS`FnDef | AS`OpDef)
// m : map AS`Name to AS`Name
// ==> seq of REP`TypeRep * REP`TypeRep * seq of AS`Type * [AS`Type]
Tuple vdmcg::FindExpDomRngTypes(const Record & df, const Map & m)
{
  switch (df.GetTag()) {
    case TAG_TYPE_AS_ExplFnDef: {
      const TYPE_AS_FnType & tp (df.GetRecord(pos_AS_ExplFnDef_tp));
      const SEQ<TYPE_AS_Type> & fndom (tp.GetSequence(1));
      const TYPE_AS_Type & fnrng (tp.GetRecord(2));
      SEQ<TYPE_AS_Type> fndom_q (RenameTypeList(fndom, m));
      TYPE_AS_Type fnrng_q (RenameType(fnrng, m));
      SEQ<TYPE_REP_TypeRep> dt_l;
      size_t len_fndom_q = fndom_q.Length();
      for (size_t idx = 1; idx <= len_fndom_q; idx++)
        dt_l.ImpAppend(FromAS2RepType(fndom_q[idx]));
      return mk_(dt_l, FromAS2RepType(fnrng_q), fndom_q, fnrng_q);
    }
    case TAG_TYPE_AS_ExplOpDef: {
      const TYPE_AS_OpType & tp (df.GetRecord(pos_AS_ExplOpDef_tp));
      const SEQ<TYPE_AS_Type> & opdom (tp.GetSequence(pos_AS_OpType_opdom));
      SEQ<TYPE_AS_Type> opdom_q (RenameTypeList(opdom, m));
      const TYPE_AS_Type & oprng (tp.GetRecord(pos_AS_OpType_oprng));
      TYPE_AS_Type oprng_q (RenameType(oprng, m));
      SEQ<TYPE_REP_TypeRep> dt_l;
      size_t len_opdom_q = opdom_q.Length();
      for (size_t idx = 1; idx <= len_opdom_q; idx++)
        dt_l.ImpAppend(FromAS2RepType(opdom_q[idx]));
      return mk_(dt_l, FromAS2RepType(oprng_q), opdom_q, oprng_q);
    }
    default: {
      SEQ<TYPE_AS_NameType> resnmtp_l; // seq of AS`NameType
      SEQ<TYPE_AS_PatTypePair> partps; // seq of AS`PatTypePair

      switch (df.GetTag()) {
        case TAG_TYPE_AS_ImplFnDef: {
          resnmtp_l.ImpConc(df.GetSequence(pos_AS_ImplFnDef_resnmtps));
          partps.ImpConc(df.GetSequence(pos_AS_ImplFnDef_partps));
          break;
        }
        case TAG_TYPE_AS_ExtExplFnDef: {
          resnmtp_l.ImpConc(df.GetSequence(pos_AS_ExtExplFnDef_resnmtps));
          partps.ImpConc(df.GetSequence(pos_AS_ExtExplFnDef_partps));
          break;
        }
        case TAG_TYPE_AS_ImplOpDef: {
          resnmtp_l.ImpConc(df.GetSequence(pos_AS_ImplOpDef_resnmtps));
          partps.ImpConc(df.GetSequence(pos_AS_ImplOpDef_partps));
          break;
        }
        case TAG_TYPE_AS_ExtExplOpDef: {
          resnmtp_l.ImpConc(df.GetSequence(pos_AS_ExtExplOpDef_resnmtps));
          partps.ImpConc(df.GetSequence(pos_AS_ExtExplOpDef_partps));
          break;
        }
      }

      TYPE_REP_TypeRep rt;
      Generic atp;
      switch(resnmtp_l.Length()) {
        case 0: {
          rt = TYPE_REP_UnitTypeRep();
          atp = nil;
          break;
        }
        case 1: {
          const TYPE_AS_Type & tp (resnmtp_l[1].GetRecord(pos_AS_NameType_tp));
          rt = FromAS2RepType(RenameType(tp, m));
          atp = RenameType(tp, m); // 20070824
          break;
        }
        default: {
          SEQ<TYPE_REP_TypeRep> tp_l;
          SEQ<TYPE_AS_Type> artpl;
          size_t len_resnmtp_l = resnmtp_l.Length();
          for (size_t idx = 1; idx <= len_resnmtp_l; idx++)
          {
            const TYPE_AS_Type & tp (resnmtp_l[idx].GetRecord(pos_AS_NameType_tp));
            tp_l.ImpAppend(FromAS2RepType(RenameType(tp, m)));
            artpl.ImpAppend(RenameType(tp, m));
          }

          rt = TYPE_REP_ProductTypeRep().Init(tp_l);
          atp = TYPE_AS_ProductType().Init(artpl, NilContextId);
          break;
        }
      }

      Tuple fidt (FindExpImplDomType(partps, m)); // seq of REP`TypeRep * seq of AS`Type
      const SEQ<TYPE_REP_TypeRep> & rtp_l (fidt.GetSequence(1));
      const SEQ<TYPE_AS_Type> & atp_l (fidt.GetSequence(2));

      return mk_(rtp_l, rt, atp_l, atp);
    }
  }
  return Tuple(0); // To avoid warnings
}

// FindExpImplDomType
// partp_l : seq of AS`PatTypePair
// m : map AS`Name to AS`Name
// ==> seq of REP`TypeRep * seq of AS`Type
Tuple vdmcg::FindExpImplDomType(const SEQ<TYPE_AS_PatTypePair> & partp_l, const Map & m)
{
  SEQ<TYPE_REP_TypeRep> rtp_l;
  SEQ<TYPE_AS_Type> atp_l;
  size_t len_partp_l = partp_l.Length();
  for (size_t idx = 1; idx <= len_partp_l; idx++)
  {
    const TYPE_AS_PatTypePair & ptp (partp_l[idx]);
    const SEQ<TYPE_AS_Pattern> & pat_l (ptp.GetSequence(pos_AS_PatTypePair_pats));
    const TYPE_AS_Type & tp (ptp.GetRecord(pos_AS_PatTypePair_tp));
    TYPE_REP_TypeRep ti (FromAS2RepType(RenameType(tp, m)));
    size_t len_pat_l = pat_l.Length();
    for (size_t i = 1; i <= len_pat_l; i++) {
      rtp_l.ImpAppend(ti);
      atp_l.ImpAppend(tp);
    }
  }
  return mk_(rtp_l, atp_l);
}
#endif // VDMSL

// FindImplDomType
// partp_l : seq of AS`PatTypePair
// ==> seq of REP`TypeRep * seq of AS`Type
Tuple vdmcg::FindImplDomType(const SEQ<TYPE_AS_PatTypePair> & partp_l)
{
  SEQ<TYPE_REP_TypeRep> rtp_l;
  SEQ<TYPE_AS_Type> atp_l;
  size_t len_partp_l = partp_l.Length();
  for (size_t idx = 1; idx <= len_partp_l; idx++)
  {
    const TYPE_AS_PatTypePair & ptp (partp_l[idx]);
    const SEQ<TYPE_AS_Pattern> & pat_l (ptp.GetSequence(pos_AS_PatTypePair_pats));
    const TYPE_AS_Type & tp (ptp.GetRecord(pos_AS_PatTypePair_tp));
    TYPE_REP_TypeRep ti (FromAS2RepType(tp));
    size_t len_pat_l = pat_l.Length();
    for (size_t i = 1; i <= len_pat_l; i++) {
      rtp_l.ImpAppend(ti);
      atp_l.ImpAppend(tp);
    }
  }
  return mk_(rtp_l, atp_l);
}

#ifdef VDMSL
// RenameType
// t : AS`Type
// m : map AS`Name to AS`Name
// -> AS`Type
TYPE_AS_Type vdmcg::RenameType(const TYPE_AS_Type & t, const Map & m)
{
  switch(t.GetTag()) {
    case TAG_TYPE_AS_CompositeType: {
      const TYPE_AS_Name & name (t.GetRecord(pos_AS_CompositeType_name));
      TYPE_AS_CompositeType ct (t);
      if (m.Dom().InSet(name))
      {
        ct.SetField(pos_AS_CompositeType_name, m[name]);
      }
      return ct;
    }
    case TAG_TYPE_AS_UnionType: {
      return TYPE_AS_UnionType().Init(RenameTypeList(t.GetSequence(pos_AS_UnionType_tps), m), NilContextId);
    }
    case TAG_TYPE_AS_ProductType: {
      return TYPE_AS_ProductType().Init(RenameTypeList(t.GetSequence(pos_AS_ProductType_tps), m), NilContextId);
    }
    case TAG_TYPE_AS_OptionalType: {
      return TYPE_AS_OptionalType().Init(RenameType(t.GetRecord(pos_AS_OptionalType_tp), m), NilContextId);
    }
    case TAG_TYPE_AS_Set0Type: {
      return TYPE_AS_Set0Type().Init(RenameType(t.GetRecord(pos_AS_Set0Type_elemtp), m), NilContextId);
    }
    case TAG_TYPE_AS_Set1Type: {
      return TYPE_AS_Set1Type().Init(RenameType(t.GetRecord(pos_AS_Set1Type_elemtp), m), NilContextId);
    }
    case TAG_TYPE_AS_Seq0Type: {
      return TYPE_AS_Seq0Type().Init(RenameType(t.GetRecord(pos_AS_Seq0Type_elemtp), m), NilContextId);
    }
    case TAG_TYPE_AS_Seq1Type: {
      return TYPE_AS_Seq1Type().Init(RenameType(t.GetRecord(pos_AS_Seq1Type_elemtp), m), NilContextId);
    }
    case TAG_TYPE_AS_GeneralMap0Type: {
      return TYPE_AS_GeneralMap0Type().Init(RenameType(t.GetRecord(pos_AS_GeneralMap0Type_mapdom), m),
                                           RenameType(t.GetRecord(pos_AS_GeneralMap0Type_maprng), m), NilContextId);
    }
    case TAG_TYPE_AS_GeneralMap1Type: {
      return TYPE_AS_GeneralMap1Type().Init(RenameType(t.GetRecord(pos_AS_GeneralMap1Type_mapdom), m),
                                           RenameType(t.GetRecord(pos_AS_GeneralMap1Type_maprng), m), NilContextId);
    }
    case TAG_TYPE_AS_InjectiveMap0Type: {
      return TYPE_AS_InjectiveMap0Type().Init(RenameType(t.GetRecord(pos_AS_InjectiveMap0Type_mapdom), m),
                                             RenameType(t.GetRecord(pos_AS_InjectiveMap0Type_maprng), m), NilContextId);
    }
    case TAG_TYPE_AS_InjectiveMap1Type: {
      return TYPE_AS_InjectiveMap1Type().Init(RenameType(t.GetRecord(pos_AS_InjectiveMap1Type_mapdom), m),
                                             RenameType(t.GetRecord(pos_AS_InjectiveMap1Type_maprng), m), NilContextId);
    }
    case TAG_TYPE_AS_OpType: {
      return TYPE_AS_OpType().Init(RenameTypeList(t.GetSequence(pos_AS_OpType_opdom), m),
                                   RenameType(t.GetRecord(pos_AS_OpType_oprng), m), NilContextId);
    }
    case TAG_TYPE_AS_PartialFnType: {
      return TYPE_AS_PartialFnType().Init(RenameTypeList(t.GetSequence(pos_AS_PartialFnType_fndom), m),
                                          RenameType(t.GetRecord(pos_AS_PartialFnType_fnrng), m), NilContextId);
    }
    case TAG_TYPE_AS_TotalFnType: {
      return TYPE_AS_TotalFnType().Init(RenameTypeList(t.GetSequence(pos_AS_TotalFnType_fndom), m),
                                        RenameType(t.GetRecord(pos_AS_TotalFnType_fnrng), m), NilContextId);
    }
    case TAG_TYPE_AS_BracketedType: {
      return TYPE_AS_BracketedType().Init(RenameType(t.GetRecord(pos_AS_BracketedType_tp), m), NilContextId);
    }
    case TAG_TYPE_AS_TypeName: {
      const TYPE_AS_Name & name (t.GetRecord(pos_AS_TypeName_name));
      TYPE_AS_TypeName tn (t);
      if (m.Dom().InSet(name))
      {
        tn.SetField(pos_AS_TypeName_name, m[name]);
      }
      return tn;
    }
    default: {
      return t;
    }
  }
}

// RenameTypeList
// tlist : seq of AS`Type
// m : map AS`Name to AS`Name
// -> seq of AS`Type
SEQ<TYPE_AS_Type> vdmcg::RenameTypeList(const SEQ<TYPE_AS_Type> & tlist, const Map & m)
{
  SEQ<TYPE_AS_Type> res;
  size_t len_tlist = tlist.Length();
  for (size_t idx = 1; idx <= len_tlist; idx++)
  {
    res.ImpAppend(RenameType(tlist[idx], m));
  }
  return res;
}

#endif // VDMSL

// PushEnv
// ==> ()
void vdmcg::PushEnv()
{
  this->fcttab.ImpAppend(Set());
}

// PopEnv
// ==> ()
void vdmcg::PopEnv()
{
  this->fcttab.ImpTl();
}

// LookUpLocFct
// nm : ( AS`Name | AS`OldName )
// ==> bool
bool vdmcg::LookUpLocFct(const TYPE_AS_Expr & nm)
{
//  BlkEnv = set of AS`Id;
//  Env = seq of BlkEnv

  const TYPE_AS_Ids & id_l (nm.GetSequence(1));
  if (id_l.Length() == 1)
  {
    const TYPE_AS_Id & id (id_l[1]);
    bool exists = false;
    size_t len_fcttab = this->fcttab.Length();
    for (size_t idx = 1; (idx <= len_fcttab) && !exists; idx++)
    {
      const Set & b_val (this->fcttab[idx]); // set of AS`Id
      exists = (b_val.InSet(CleanIdentifier(id)));
    }
    return exists;
  }
  return false;
}

// InsertLocFct
// nm : ( AS`Name | AS`OldName )
// ==> ()
void vdmcg::InsertLocFct(const TYPE_AS_Expr & nm)
{
  Set blk (this->fcttab.Hd());
  this->fcttab.ImpTl();
  blk.Insert(GiveLastName(nm));
  this->fcttab.ImpPrepend(blk);
}

// InsPosFct
// pid_m : map ( AS`Name | AS`OldName ) to set of REP`TypeRep
// ==> ()
void vdmcg::InsPosFct(const Map & pid_m)
{
  Set dom_pid_m (pid_m.Dom());
  Generic nm;
  for (bool bb = dom_pid_m.First(nm); bb; bb = dom_pid_m.Next(nm))
  {
    SET<TYPE_REP_TypeRep> tmp_s (pid_m[nm]);
    bool exists = false;
    Generic typ;
    for (bool cc = tmp_s.First(typ); cc && !exists; cc = tmp_s.Next(typ))
//      exists = exists || PossibleFnType(typ);
      exists = PossibleFnType(typ);

    if (exists)
      InsertLocFct(nm);
  }
}

// GenLocalFcts
// def_l : seq of AS`FnDef
// ==> ()
void vdmcg::GenLocalFcts(const SEQ<TYPE_AS_FnDef> & def_l)
{
  size_t len_def_l = def_l.Length();
  for (size_t i = 1; i <= len_def_l; i++)
  {
    const TYPE_AS_FnDef & fd (def_l[i]);
    switch(fd.GetTag()) {
      case TAG_TYPE_AS_ExplFnDef: {
        const TYPE_AS_Name & fnm (fd.GetRecord(pos_AS_ExplFnDef_nm));
        InsertLocFct(fnm);
        if (!fd.GetField(pos_AS_ExplFnDef_fnpre).IsNil())
          InsertLocFct(vdm_BC_GivePrePostName(fnm, ASTAUX::MkId(L"pre")));
        if (!fd.GetField(pos_AS_ExplFnDef_fnpost).IsNil())
          InsertLocFct(vdm_BC_GivePrePostName(fnm, ASTAUX::MkId(L"post")));
        break;
      }
      case TAG_TYPE_AS_ExtExplFnDef: {
        const TYPE_AS_Name & fnm (fd.GetRecord(pos_AS_ExtExplFnDef_nm));
        InsertLocFct(fnm);
        if (!fd.GetField(pos_AS_ExtExplFnDef_fnpre).IsNil())
          InsertLocFct(vdm_BC_GivePrePostName(fnm, ASTAUX::MkId(L"pre")));
        if (!fd.GetField(pos_AS_ExtExplFnDef_fnpost).IsNil())
          InsertLocFct(vdm_BC_GivePrePostName(fnm, ASTAUX::MkId(L"post")));
        break;
      }
      case TAG_TYPE_AS_ImplFnDef: {
        const TYPE_AS_Name & fnm (fd.GetRecord(pos_AS_ImplFnDef_nm));
        InsertLocFct(fnm);
        if (!fd.GetField(pos_AS_ImplFnDef_fnpre).IsNil())
          InsertLocFct(vdm_BC_GivePrePostName(fnm, ASTAUX::MkId(L"pre")));
        InsertLocFct(vdm_BC_GivePrePostName(fnm, ASTAUX::MkId(L"post")));
        break;
      }
    }
  }
}

// ContainsImplicitFct
// ==> bool
bool vdmcg::ContainsImplicitFct() const
{
  return this->impl;
}

// ContainsStaticImplicit
// ==> bool
bool vdmcg::ContainsStaticImplicit() const
{
  return this->staticImpl;
}

// GetImplicitFcts
// ==> CPP`CPPAS
TYPE_CPP_CPPAS vdmcg::GetImplicitFcts()
{
  TYPE_CPP_CPPAS implfcts_cpp;
  while (!this->implcpp.Dom().IsEmpty())
  {
    SET<TYPE_CI_ContextId> implcppdom (this->implcpp.Dom());
    TYPE_CI_ContextId cid (implcppdom.GetElem());
    Generic gg;
    for (bool bb = implcppdom.First (gg); bb; bb = implcppdom.Next (gg))
    {
      TYPE_CI_ContextId tempcid (gg);
      Tuple gfp1 (GetCI().GetFilePos(cid));
      const TYPE_CI_TokenPos & tst (gfp1.GetRecord(3));
      int ast_start = tst.GetIntValue(pos_CI_TokenPos_abs_uline);
      Tuple gfp2 (GetCI().GetFilePos(tempcid));
      const TYPE_CI_TokenPos & tst2 (gfp2.GetRecord(3));
      int ast_startf = tst2.GetIntValue(pos_CI_TokenPos_abs_uline);
      if (ast_start > ast_startf)
        cid = tempcid;
    }
    implfcts_cpp.ImpConc(this->implcpp[cid]);
    this->implcpp.RemElem(cid);
  }
  this->impl = false;
  this->implcpp = Map();
  return implfcts_cpp;
}

// IsSuperFirst
// stmts : seq of CPP`Stmt
// -> bool * [CPP`Stmt] * [seq of CPP`Stmt]
Tuple vdmcg::IsSuperFirst(const SEQ<TYPE_CPP_Stmt> & stmts)
{
  if (stmts.IsEmpty())
    return mk_(Bool(false), Nil(), Nil());

  const TYPE_CPP_Stmt & stmt (stmts[1]);
  switch(stmt.GetTag()) {
    case TAG_TYPE_CPP_CompoundStmt: {
      const SEQ<TYPE_CPP_Stmt> & stmt_l (stmt.GetSequence(pos_CPP_CompoundStmt_stms));
      Tuple isf (IsSuperFirst(stmt_l));
      if (isf.GetBoolValue(1))
      {
        const SEQ<TYPE_CPP_Stmt> & l (isf.GetSequence(3));
        switch(l.Length()) {
          case 0:
          case 1: {
            return mk_(Bool(true), isf.GetRecord(2), l.Conc(stmts.Tl()));
          }
          default: {
            TYPE_CPP_CompoundStmt cstmt;
            cstmt.Init(l, stmt.GetInt(pos_CPP_CompoundStmt_cid));
            return mk_(Bool(true), isf.GetRecord(2), mk_sequence(cstmt).Conc(stmts.Tl()));
          }
        }
      }
      else
        return mk_(Bool(false), Nil(), Nil());
    }
    case TAG_TYPE_CPP_ExpressionStmt: {
      const Generic & expr (stmt.GetField(pos_CPP_ExpressionStmt_expr));
      if (expr.Is(TAG_TYPE_CPP_FctCall))
      {
        TYPE_CPP_Expr fct (Record(expr).GetRecord(pos_CPP_FctCall_fct));
        if (fct.Is(TAG_TYPE_CPP_Identifier))
        {
          SEQ<Char> id (fct.GetSequence(pos_CPP_Identifier_id));
          if (id == ASTAUX::MkId(L"super"))
          {
            return mk_(Bool(true), stmt, stmts.Tl());
          }
        }
      }
      return mk_(Bool(false), Nil(), Nil());
    }
  }
  return mk_(Bool(false), Nil(), Nil());
}

#ifdef VDMPP
// IsMemInitFirst
// name : AS`Name
// stmts : seq of CPP`Stmt
// -> bool * [CPP`MemInitializer] * [seq of CPP`Stmt]
Tuple vdmcg::IsMemInitFirst(const TYPE_AS_Name & nm, const SEQ<TYPE_CPP_Stmt> & stmts)
{
  if (stmts.IsEmpty())
    return mk_(Bool(false), Nil(), Nil());

  const TYPE_CPP_Stmt & stmt (stmts[1]);
  switch(stmt.GetTag()) {
    case TAG_TYPE_CPP_CompoundStmt: {
      const SEQ<TYPE_CPP_Stmt> & stmt_l (stmt.GetSequence(pos_CPP_CompoundStmt_stms));
      Tuple isf (IsMemInitFirst(nm, stmt_l));
      if (isf.GetBoolValue(1))
      {
        const SEQ<TYPE_CPP_Stmt> & l (isf.GetSequence(3));
        switch(l.Length()) {
          case 0:
          case 1: {
            return mk_(Bool(true), isf.GetRecord(2), l.Conc(stmts.Tl()));
          }
          default: {
            TYPE_CPP_CompoundStmt cstmt;
            cstmt.Init(l, stmt.GetInt(pos_CPP_CompoundStmt_cid));
            return mk_(Bool(true), isf.GetRecord(2), mk_sequence(cstmt).Conc(stmts.Tl()));
          }
        }
      }
      else
        return mk_(Bool(false), Nil(), Nil());
    }
    case TAG_TYPE_CPP_Return:
    case TAG_TYPE_CPP_ExpressionStmt: {
      Generic exprg = (stmt.Is(TAG_TYPE_CPP_Return) ? stmt.GetField(pos_CPP_Return_expr)
                                                    : stmt.GetField(pos_CPP_ExpressionStmt_expr));
      if (!exprg.IsNil()) {
        TYPE_CPP_Expr expr (StripBracketedAndCastExpr(exprg)); 
        if (expr.Is(TAG_TYPE_CPP_FctCall))
        {
          TYPE_CPP_Expr fct (expr.GetRecord(pos_CPP_FctCall_fct)); 
          SEQ<TYPE_AS_Name> cl_l (GetOrderedSupers(nm));
          size_t len_cl_l = cl_l.Length();
          bool exists = false;
          for (size_t idx = 1; idx <= len_cl_l && !exists; idx++)
             exists = (fct == vdm_BC_Rename(cl_l[idx]));

          if(exists)
          {
            TYPE_CPP_MemInitializer mi;
            mi.Init(expr.GetRecord(pos_CPP_FctCall_fct),
                    expr.GetSequence(pos_CPP_FctCall_arg),
                    expr.GetInt(pos_CPP_FctCall_cid));

            return mk_(Bool(true), mi, stmts.Tl());
          }
        }
      }
      return mk_(Bool(false), Nil(), Nil());
    }
  }
  return mk_(Bool(false), Nil(), Nil());
}
#endif // VDMPP

// IsSafeReturn (not in spec)
// body : AS`OpBody
// rngtype : REP`TypeRep
// ==> bool
bool vdmcg::IsSafeReturn(const TYPE_AS_OpBody & body, const TYPE_REP_TypeRep & rngtype)
{
  const TYPE_CI_ContextId & cid (body.GetInt(pos_AS_OpBody_cid));
  if (cid != NilContextId)
  {
    Generic ti (GetCI().GetTypeInfo(cid));
    if (!ti.IsNil())
    {
      if (rngtype.Is(TAG_TYPE_REP_UnitTypeRep) && !ti.Is(TAG_TYPE_REP_UnitTypeRep))
      {
        return false;
      }
      else if (!rngtype.Is(TAG_TYPE_REP_UnitTypeRep))
      {
        if (!rngtype.Is(TAG_TYPE_REP_AllTypeRep) && HasUnitTypeRep(ti))
        {
          return false;
        }
      }
    }
  }
  return true;
}

// HasUnitTypeRep
// type : REP`TypeRep
// +> bool
bool vdmcg::HasUnitTypeRep(const TYPE_REP_TypeRep & type)
{
  switch (type.GetTag()) {
    case TAG_TYPE_REP_UnitTypeRep:
      return true;
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tp_s (type.GetSet(pos_REP_UnionTypeRep_tps));
      bool exists = false;
      Generic tp;
      for (bool bb = tp_s.First(tp); bb && !exists; bb = tp_s.Next(tp))
        exists = HasUnitTypeRep(tp);
      return exists;
    }
    default:
      return false;
  }
}
