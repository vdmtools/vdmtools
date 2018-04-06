/***
*  * What
*  *  This version contains both:
*  *    mod_class.cc: Implementation of mod_class.vdm 1.93
*  *
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/mod_class.cc,v $
*  * VERSION
*  *    $Revision: 1.60 $
*  * DATE
*  *    $Date: 2006/04/14 08:14:40 $
*  * STATUS
*  *    Under development
*  * REFERENCES
*  *
*  * PROJECT
*  *    IDERES/AFRODITE
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "vdmcg.h"
#include "BC.h"
#include "astaux.h"
#include "statsem.h"
#include "tbutils.h"
#include "settings.h"
#include "intconvquotes.h"
#include "cgtag.h"

//wcout << L"intertp: " << INT2Q::h2gAS(intertp) << endl;

// CLASS functions and operations

// GenClasses
// cs_l : AS`Document
// cl_nms : set of AS`Name
// ==> CPP`Files
SET<TYPE_CPP_File> vdmcg::GenClasses(const SEQ<TYPE_AS_Class> & cs_l_,
                                     const SET<TYPE_AS_Name> & cl_nms,
                                     const Bool & s,
                                     const Bool & p,
                                     const Bool & t,
                                     const Bool & st,
                                     const Generic & package_name,
                                     const Bool & cop,
                                     const Bool & testcond)
{
  SEQ<TYPE_AS_Class> cs_l (Settings.VDM10() ? SEQ<TYPE_AS_Class>(ASTAUX::ChangeDocumentToStatic(cs_l_)) : cs_l_);

  if (vdm_CPP_isJAVA())
  {
    set_skeleton_option(s.GetValue());
    set_preandpost_option(p.GetValue());
    set_onlytypes_option(t.GetValue());
    set_smalltypes_option(st.GetValue());
    set_package_option(package_name);
    set_conc_option(cop.GetValue());
  }
  set_testpreandpost_option(testcond.GetValue());

//  set_verbose_mode(true); // 2090127

  size_t len_cs_l = cs_l.Length();
  for (size_t i = 1; i <= len_cs_l; i++)
    AddClassTypes(cs_l[i]);

  // Note that the preprocessing that is performed at the
  // specification level at this point, has already been performed by
  // the call to vdmcg::Reset in vdmcg.cc; this is for efficiency
  // reasons, since otherwise the entire preprocessing phase is
  // performed each time a class is code generated. This is not a
  // problem at the spec level, since cs_l is exactly all of the
  // classes.

  SET<TYPE_CPP_File> files;
  for (size_t idx = 1; idx <= len_cs_l; idx++)
  {
    // Ast version control removed
    const TYPE_AS_Class & cs (cs_l[idx]);
    const TYPE_AS_Name & nm (cs.GetRecord(pos_AS_Class_nm));

    InitState_TPGEN(nm);

    GetStatSem().ExpandClass (POS, cs);

    InitState_CGAUX(nm);
    vdm_BC_ResetVarno();

    InitState_FCTDEF();
    InitState_FVD();

    if (vdm_CPP_isCPP())
    {
      TYPE_CPP_File hfile (GenHFile(cs));
      TYPE_CPP_File ccfile (GenCCFile(cs));
      files.Insert(hfile);
      files.Insert(ccfile);
      files.ImpUnion(GenAnonymFiles(cs));
    }
    else // for Java
    {
// 20110622
      const TYPE_AS_Ids & ids (nm.GetSequence(pos_AS_Name_ids));
      if (
          (ids[1].SubSequence(1,4) != Sequence(L"JDK_")) &&
          (ids[1].SubSequence(1,5) != Sequence(L"STUB_")) &&
          (ids[1].SubSequence(1,11) != Sequence(L"JDKArrayOf_")))
      {
        int num_errors = TBUTILS::GetErrorsCount();
        TYPE_CPP_File rc (GenJavaFile(cs, isInterface(nm)));

        if (TBUTILS::GetErrorsCount() == num_errors)
          files.Insert(rc);

        if (!UsedQuotes().IsEmpty())
          files.ImpUnion(GenJavaQuoteFiles());

        if (ContainsImplicitFct() && !isInterface(nm))
          files.Insert(GenJavaExternalFile(cs));
      }
    }
  }

  if (vdm_CPP_isCPP())
    files.ImpUnion (GenCGBase(cl_nms)) ;

  return files;
}

// GetFnDefAccess (not in spec)
// fd : AS`FnDef
// -> AS`Access
Int vdmcg::GetFnDefAccess (const TYPE_AS_FnDef & fd) const
{
  Int acs;
  switch (fd.GetTag ()) {
    case TAG_TYPE_AS_ExplFnDef: {
      acs = fd.GetField(pos_AS_ExplFnDef_access);
      break;
    }
    case TAG_TYPE_AS_ImplFnDef: {
      acs = fd.GetField(pos_AS_ImplFnDef_access);
      break;
    }
    case TAG_TYPE_AS_ExtExplFnDef: {
      acs = fd.GetField(pos_AS_ExtExplFnDef_access);
      break;
    }
  }
  return acs;
}

// GetOpDefAccess
// fd : AS`OpDef
// -> AS`Access
Int vdmcg::GetOpDefAccess (const TYPE_AS_OpDef & fd) const
{
  Int acs;
  switch (fd.GetTag ()) {
    case TAG_TYPE_AS_ExplOpDef: {
      acs = fd.GetField(pos_AS_ExplOpDef_access);
      break;
    }
    case TAG_TYPE_AS_ImplOpDef: {
      acs = fd.GetField(pos_AS_ImplOpDef_access);
      break;
    }
    case TAG_TYPE_AS_ExtExplOpDef: {
      acs = fd.GetField(pos_AS_ExtExplOpDef_access);
      break;
    }
  }
  return acs;
}

// MergeMemberDecl
// cur : map AS`Access to seq of CPP`MemberDeclaration
// orig : map AS`Access to seq of CPP`MemberDeclaration
// ==> map AS`Access to seq of CPP`MemberDeclaration
Map vdmcg::MergeMemberDecl (const Map & cur, const Map & orig) const
{
  Map res (orig);
  Set dom_cur (cur.Dom());
  Generic a;
  for (bool bb = dom_cur.First (a); bb; bb = dom_cur.Next (a))
  {
    SEQ<TYPE_CPP_MemberDeclaration> old;
    if (orig.DomExists (a))
      old = orig[a];
    res.ImpModify (a, old.ImpConc (cur[a]));
  }
  return res;
}

// GenHFile
// cs : AS`Class
// ==> CPP`File
TYPE_CPP_File vdmcg::GenHFile(const TYPE_AS_Class& cs)
{
  const TYPE_AS_Name & nm       (cs.GetRecord(pos_AS_Class_nm) );
  const SEQ<TYPE_AS_Name> & inh (cs.GetSequence(pos_AS_Class_supercls));
  const Generic & defs          (cs.GetField(pos_AS_Class_defs));
  const Generic & useslib       (cs.GetField(pos_AS_Class_useslib));

  TYPE_CPP_CPPAS cl_l;

  TYPE_CPP_ClassHead head (GenHead(nm, inh, false));

  TYPE_CPP_Preprocessors pp_l;

  Map access_m; // map AS`Access to seq of CPP`MemberDeclaration
  SEQ<TYPE_CPP_MemberDeclaration> pro_l;
  SEQ<TYPE_CPP_MemberDeclaration> pub_l;

  bool needinit = false;
  if (!defs.IsNil ()) {
    TYPE_AS_Definitions defin (defs);
    const MAP<TYPE_AS_Name, TYPE_AS_TypeDef> & tps (defin.GetMap(pos_AS_Definitions_typem));
    const SEQ<TYPE_AS_ValueDef> & vals             (defin.GetSequence(pos_AS_Definitions_valuem));
    const MAP<TYPE_AS_Name, TYPE_AS_FnDef> & fcts  (defin.GetMap(pos_AS_Definitions_fnm));
    const MAP<TYPE_AS_Name, TYPE_AS_OpDef> & oper  (defin.GetMap(pos_AS_Definitions_opm));
    const SEQ<TYPE_AS_InstanceVarDef> & iVars      (defin.GetSequence(pos_AS_Definitions_instvars));

    pub_l.ImpConc (GenLibMethods(nm));

    // merge static instance variables to values
    SEQ<TYPE_AS_ValueDef> new_vals (vals);
    SEQ<TYPE_AS_InstanceVarDef> new_iVars;
    size_t len_iVars = iVars.Length();
    for (size_t idx = 1; idx <= len_iVars; idx++) {
      const TYPE_AS_InstanceVarDef & ivd (iVars[idx]);
      if (ivd.Is(TAG_TYPE_AS_InstAssignDef) &&
          ivd.GetBool(pos_AS_InstAssignDef_stat) &&
          !ivd.GetRecord(pos_AS_InstAssignDef_ad).GetField(pos_AS_AssignDef_dclinit).IsNil()) {
        const TYPE_AS_AssignDef & ad (ivd.GetRecord(pos_AS_InstAssignDef_ad));
        const TYPE_AS_Name & var (ad.GetRecord(pos_AS_AssignDef_var));
        TYPE_AS_PatternName pn;
        pn.SetField(pos_AS_PatternName_nm,  var);
        pn.SetField(pos_AS_PatternName_tp,  ad.GetRecord(pos_AS_AssignDef_tp));
        pn.SetField(pos_AS_PatternName_cid, var.GetInt(pos_AS_Name_cid));
        TYPE_AS_ValueDef vd;
        vd.SetField(pos_AS_ValueDef_pat,    pn);
        vd.SetField(pos_AS_ValueDef_tp,     ad.GetRecord(pos_AS_AssignDef_tp));
        vd.SetField(pos_AS_ValueDef_val,    ad.GetRecord(pos_AS_AssignDef_dclinit));
        vd.SetField(pos_AS_ValueDef_access, ivd.GetField(pos_AS_InstAssignDef_access));
        vd.SetField(pos_AS_ValueDef_stat, ivd.GetBool(pos_AS_InstAssignDef_stat));
        vd.SetField(pos_AS_ValueDef_cid,    ivd.GetInt(pos_AS_InstAssignDef_cid));
        new_vals.ImpAppend(vd);
      }
      else {
        new_iVars.ImpAppend(ivd);
      }
    }
    Map acs_vals_m (GenValues(nm, new_vals));
    access_m = MergeMemberDecl (acs_vals_m, access_m);
    access_m = MergeMemberDecl (GenInstVars(new_iVars), access_m);

    needinit = !new_vals.IsEmpty();
    SEQ<TYPE_AS_TypeDef> type_l;
    Set dom_tps (tps.Dom());
    Generic tp;
    for (bool bb = dom_tps.First (tp); bb; bb = dom_tps.Next (tp)) {
      type_l.ImpAppend (tps[tp]);
      const TYPE_AS_TypeDef & td (tps[tp]);
      needinit |= td.GetRecord(pos_AS_TypeDef_shape).Is(TAG_TYPE_AS_CompositeType);
    }

    Sequence typedefs (GenTypeDef_DS(nm, type_l)); // seq of CPP`MemberDeclaration
    if (!typedefs.IsEmpty()) {
      const TYPE_CPP_MemberDeclaration & md (typedefs[1]);
      if ( md.Is(TAG_TYPE_CPP_MemberSpecifier) ) {
        pub_l.ImpAppend(md);
        cl_l.ImpConc(typedefs.Tl());
      }
      else {
        cl_l.ImpConc(typedefs);
      }
    }

    pub_l.ImpConc(GenInvEqOrdDecl(vdm_BC_Rename(nm), type_l));

    Set dom_fcts (fcts.Dom());
    Generic fnm;
    for (bool cc = dom_fcts.First (fnm); cc; cc = dom_fcts.Next (fnm)) {
      TYPE_AS_FnDef fctas (fcts[fnm]);
      Map acs_fct_m;
      acs_fct_m.Insert (GetFnDefAccess (ExchangeDefAccess (fctas)),
                        GenFctOpDecl (fnm, fctas, nil, nil));
      access_m = MergeMemberDecl (acs_fct_m, access_m);
    }

    Set dom_oper (oper.Dom());
    Generic onm;
    for (bool dd = dom_oper.First (onm); dd; dd = dom_oper.Next (onm)) {
      TYPE_AS_OpDef opas (oper[onm]);
      Map acs_op_m;
      acs_op_m.Insert (GetOpDefAccess (ExchangeDefAccess (opas)), GenFctOpDecl (onm, opas, nil, nil));
      access_m = MergeMemberDecl (acs_op_m, access_m);
      if (ASTAUX::GetConstr(opas) && GetOpParms(opas).IsEmpty()) {
        SetHasDefaultConstr();
      }
    }

    Set uhn (GetUnhiddenNames(nm));
    Generic g;
    for (bool ee = uhn.First(g); ee; ee = uhn.Next(g)) {
      Tuple t (g);
      const TYPE_AS_Name & l_mn (t.GetRecord(1));
      TYPE_AS_Name thisNm (l_mn);
      TYPE_AS_Ids ids (l_mn.get_ids());
      thisNm.set_ids(TYPE_AS_Ids().ImpAppend(ids.Hd()));
      if (thisNm != nm) {
        Map acs_m_m;
        acs_m_m.Insert(t.GetField(2), GenHiddenFctDecl(l_mn, t.GetField(3)));
        access_m = MergeMemberDecl(acs_m_m, access_m);
      }
    }

    if (!GetHasDefaultConstr()) {
      pub_l.ImpConc(GenConstructorDef(nm));
    }
    TYPE_CPP_Identifier initid (vdm_BC_GivePrePostNm(nm, ASTAUX::MkId(L"init")));

    TYPE_CPP_FctDecl fdecl (vdm_BC_GenFctDecl(initid, type_dL()));

    pub_l.ImpAppend(vdm_BC_GenMemberSpec(mk_sequence(vdm_BC_GenTypeSpecifier(vdm_BC_GenVoid())), fdecl));

    bool exists = false;
    for (size_t idx2 = 1; idx2 <= len_iVars && !exists; idx2++) {
      exists = iVars[idx2].Is(TAG_TYPE_AS_InstanceInv);
    }
    if (exists) {
      TYPE_CPP_Identifier invid (vdm_BC_GivePrePostNm(nm, ASTAUX::MkId(L"inv")));
      TYPE_CPP_FctDecl invdecl (vdm_BC_GenFctDecl(invid, type_dL()));

      pub_l.ImpAppend(vdm_BC_GenMemberSpec(mk_sequence(GenBoolType()), invdecl));
    }
  }

  cl_l.ImpConc(GenModuleHPart());
  cl_l.ImpConc(GenObjRefClass(nm,inh.Elems ()));

  if (!useslib.IsNil()) {
    pp_l.ImpAppend(vdm_BC_GenInclusion(ASTAUX::MkId(L"dlclass.h" )));
  }
  pp_l.ImpConc(GenInclusions(inh.Elems()));
  pp_l.ImpConc(GenModuleHIncludes());
  pp_l.ImpConc(GenHIncludes(inh.Elems()));

  TYPE_AS_Id i (ASTAUX::MkId(L"init_").ImpConc(GiveLastName(nm)));

  SEQ<TYPE_CPP_MemberDeclaration> mem_l;
//
  if ( needinit ) {
    mem_l.ImpConc(vdm_BC_GenClassFriend (vdm_BC_GenIdentifier (i)));
  }
  mem_l.ImpConc(vdm_BC_GenPublic(pub_l));

  if (!useslib.IsNil()) {
    TYPE_CPP_Identifier dlClassName (vdm_BC_GenIdentifier(DlClass_Name));
    TYPE_CPP_IndirectionDecl indirect (vdm_BC_GenIndirection(GenDlMember()));
    TYPE_CPP_MemberSpecifier memberSpec (
       vdm_BC_GenMemberSpec(type_dL().ImpAppend(vdm_BC_GenTypeSpecifier(dlClassName)), indirect));
    mem_l.ImpConc(vdm_BC_GenCPPMemberList(nil, type_dL().ImpAppend(memberSpec)));
  }
  mem_l.ImpConc (vdm_BC_GenProtected(pro_l));
  //mem_l.ImpConc (vdm_BC_GenAccessMemberList ((Generic)access_m));
  mem_l.ImpConc (vdm_BC_GenAccessMemberList (INT2Q::h2gAS(access_m)));

  type_dL cs_l;
  cs_l.ImpAppend(vdm_BC_GenTypeSpecifier(vdm_BC_GenClassSpecifier(head, mem_l)));

  SEQ<TYPE_CPP_IdentDeclaration> cpp_l;
  cpp_l.ImpAppend(vdm_BC_GenIdentDeclaration(SEQ<TYPE_CPP_Annotation>(), cs_l, nil));

  // TODO: "MATH" must be renamed for Windows
  TYPE_AS_Id clnm (GiveLastName(nm));
#ifdef _MSC_VER
  if (clnm == ASTAUX::MkId(L"MATH")) {
      clnm = ASTAUX::MkId(L"vdm_MATH");
  }
#endif // _MSC_VER
  return vdm_BC_GenFile2(nil, clnm.ImpConc(ASTAUX::MkId(L".h")), pp_l, cl_l.ImpConc(cpp_l),
                          cs.GetInt(pos_AS_Class_cid));
}

// GenInclusions
// name_s : set of AS`Name
// ==> CPP`Preprocessors
SEQ<TYPE_CPP_Preprocessor> vdmcg::GenInclusions(const SET<TYPE_AS_Name> & name_s)
{
  SEQ<TYPE_CPP_Preprocessor> incl;
  if (name_s.IsEmpty()) {
    incl.ImpAppend(vdm_BC_GenSqIncl(ASTAUX::MkId(L"math.h")));
    incl.ImpAppend(vdm_BC_GenInclusion(GiveLibName()));
    incl.ImpAppend(vdm_BC_GenInclusion(ASTAUX::MkId(L"cg.h")));
    incl.ImpAppend(vdm_BC_GenInclusion(ASTAUX::MkId(L"cg_aux.h")));
    incl.ImpAppend(vdm_BC_GenInclusion(ASTAUX::MkId(L"CGBase.h")));
  }

  SET<TYPE_AS_Name> name_s_q (name_s);
  Generic nm;
  for (bool bb = name_s_q.First(nm); bb; bb = name_s_q.Next(nm)) {
    TYPE_AS_Id id (GiveLastName(nm));
    incl.ImpAppend(vdm_BC_GenInclusion(id.ImpConc(ASTAUX::MkId(L".h"))));
  }
  return incl;
}

// GenLibMethods
// nm : AS`Name
// ==> seq of CPP`MemberDeclaration
SEQ<TYPE_CPP_MemberDeclaration> vdmcg::GenLibMethods(const TYPE_AS_Name & nm)
{
  TYPE_CPP_Identifier rnm (vdm_BC_Rename (nm));
  TYPE_CPP_Identifier cls (vdm_BC_Rename (nm));
  TYPE_CPP_Identifier obj (vdm_BC_GenIdentifier (ASTAUX::MkId(L"ObjectRef")));
  TYPE_CPP_Identifier getprefix (vdm_BC_PrefixClassName (ASTAUX::MkId(L"Get"), cls));
  TYPE_CPP_Identifier clid (vdm_BC_PrefixName (ASTAUX::MkId(L"Get"), rnm));

  TYPE_CPP_IndirectionDecl decl1 (vdm_BC_GenIndirection (vdm_BC_GenFctDecl (getprefix,type_dL())));
  TYPE_CPP_FctDecl decl2 (vdm_BC_GenFctDecl (vdm_BC_GenIdentifier (ASTAUX::MkId(L"Self")), type_dL()));
  TYPE_CPP_FctDecl decl3 (vdm_BC_GenFctDecl (vdm_BC_GenIdentifier (ASTAUX::MkId(L"vdm_GetId")), type_dL()));
  SEQ<TYPE_CPP_Expr> s1;
  s1.ImpAppend (vdm_BC_GenFctCall (clid, SEQ<TYPE_CPP_Expr>()));
  TYPE_CPP_Stmt stmt1 (vdm_BC_GenReturnStmt (GenThis()));
  TYPE_CPP_Stmt stmt2 (vdm_BC_GenReturnStmt (vdm_BC_GenFctCall (obj, s1)));
  TYPE_CPP_Stmt stmt3 (vdm_BC_GenReturnStmt (vdm_BC_GiveMacroName (nm)));

  Sequence s11;
  s11.ImpAppend (cls);

  SEQ<TYPE_CPP_MemberDeclaration> this_v;
  this_v.ImpAppend (vdm_BC_GenFctDef (type_dL().ImpAppend(vdm_BC_GenTypeSpecifier(s11)),
                                      decl1,
                                      nil,
                                      stmt1));
  SEQ<TYPE_CPP_DeclSpecifier> s2;
  s2.ImpAppend (vdm_BC_GenTypeSpecifier (obj));

  SEQ<TYPE_CPP_FunctionDefinition> self_v;
  self_v.ImpAppend (vdm_BC_GenFctDef (s2, decl2, nil, stmt2));

  SEQ<TYPE_CPP_DeclSpecifier> s3;
  s3.ImpAppend (vdm_BC_GenTypeSpecifier(vdm_BC_GenInt ()));

  SEQ<TYPE_CPP_FunctionDefinition> getid;
  getid.ImpAppend (vdm_BC_GenFctDef (s3, decl3, nil, stmt3));

  return this_v.ImpConc (self_v).ImpConc (getid);
}

// GenCCFile
// cs : AS`Class
// ==> CPP`File
TYPE_CPP_File vdmcg::GenCCFile(const TYPE_AS_Class & cs)
{
  const TYPE_AS_Name & nm       (cs.GetRecord(pos_AS_Class_nm));
  const SEQ<TYPE_AS_Name> & inh (cs.GetSequence(pos_AS_Class_supercls));
  const Generic & defs          (cs.GetField(pos_AS_Class_defs));
  const Generic & useslib       (cs.GetField(pos_AS_Class_useslib));

  TYPE_AS_Id id (GiveLastName(nm));

  Sequence pp_l;
#ifdef _MSC_VER
  if (id == ASTAUX::MkId(L"MATH")) {
    pp_l.ImpAppend(vdm_BC_GenInclusion(ASTAUX::MkId(L"vdm_MATH.h")));
  }
  else {
    pp_l.ImpAppend(vdm_BC_GenInclusion(GiveLastName(nm).ImpConc(ASTAUX::MkId(L".h"))));
  }
#else
  pp_l.ImpAppend(vdm_BC_GenInclusion(GiveLastName(nm).ImpConc(ASTAUX::MkId(L".h"))));
#endif // _MSC_VER
  pp_l.ImpConc(GenModuleCCIncludes());

  TYPE_CPP_CPPAS cpp_l;
  if (!defs.IsNil ()) {
    TYPE_AS_Definitions defin (defs);
    const MAP<TYPE_AS_Name, TYPE_AS_TypeDef> & tps (defin.GetMap(pos_AS_Definitions_typem));
    const MAP<TYPE_AS_Name, TYPE_AS_FnDef> & fcts  (defin.GetMap(pos_AS_Definitions_fnm));
    const MAP<TYPE_AS_Name, TYPE_AS_OpDef> & oper  (defin.GetMap(pos_AS_Definitions_opm));
    const SEQ<TYPE_AS_InstanceVarDef> & iVars      (defin.GetSequence(pos_AS_Definitions_instvars));

    Set dom_tps (tps.Dom());
    SEQ<TYPE_AS_TypeDef> type_l;
    Generic tp;
    for (bool bb = dom_tps.First (tp); bb; bb = dom_tps.Next (tp)) {
      type_l.ImpAppend (tps[tp]);
    }

    cpp_l.ImpConc(GenStaticInstVarInit(iVars, nm));
    cpp_l.ImpConc(GenClassInit(nm, type_l));
    cpp_l.ImpConc(GenInvEqOrdDef(type_l));

    if (!GetHasDefaultConstr()) {
      cpp_l.ImpConc (GenConstructorDef_cc (nm, iVars, inh.IsEmpty(), !useslib.IsNil()));
    }
    SEQ<TYPE_CPP_Stmt> base_l;
    base_l.ImpConc(GenInitBaseVars(inh.IsEmpty()));
    base_l.ImpConc(GenVarInits(iVars));

    TYPE_CPP_Identifier initid (vdm_BC_GivePrePostNm(nm, ASTAUX::MkId(L"init")));
    TYPE_CPP_FctDecl fdecl (
        vdm_BC_GenFctDecl(vdm_BC_GenQualifiedName(vdm_BC_Rename(nm), initid), type_dL()));

    TYPE_CPP_FunctionDefinition fd (
        vdm_BC_GenFctDef (mk_sequence(vdm_BC_GenTypeSpecifier(vdm_BC_GenVoid())),
                          fdecl,
                          nil,
                          vdm_BC_GenBlock(base_l)));
    cpp_l.ImpAppend(fd);

    cpp_l.ImpConc(GenInstanceInv(nm, iVars));

    Set dom_fcts (fcts.Dom());
    Generic fnm;
    for (bool cc = dom_fcts.First (fnm); cc; cc = dom_fcts.Next (fnm)) {
      CurrentMethod(fnm);
      cpp_l.ImpConc(GenFctDef_FD(fcts[fnm], !useslib.IsNil()));
    }
    Set dom_oper (oper.Dom());
    Generic onm;
    for (bool dd = dom_oper.First (onm); dd; dd = dom_oper.Next (onm)) {
      CurrentMethod(onm);
      cpp_l.ImpConc(GenOpDef_FD(onm, oper[onm], Nil (), !useslib.IsNil()));
    }

// 20120530 -->
    //pp_l.ImpConc(GenClassIncls());
    Sequence cincls (GenClassIncls());
    size_t len_cincls = cincls.Length();
    for (size_t idx = 1; idx <= len_cincls; idx++) {
      //if (!pp_l.Elems().InSet(cincls[idx]))
      if ( 0 == pp_l.Find(cincls[idx])) {
        pp_l.ImpAppend(cincls[idx]);
      }
    }
// <-- 20120530
    pp_l.ImpConc(IncludeExternalCC());
  }
  cpp_l.ImpConc(GenModuleCCPart());

// 20120522 -->
//  return vdm_BC_GenFile(Nil(), GenFileExt(id), pp_l, cpp_l);
  return vdm_BC_GenFile2(Nil(), GenFileExt(id), pp_l, cpp_l, cs.GetInt(pos_AS_Class_cid));
// <-- 20120522
}

// GenJavaFile
// cs : AS`Class
// isInterface : bool
// ==> CPP`File
TYPE_CPP_File vdmcg::GenJavaFile(const TYPE_AS_Class& cs, bool isInterface)
{
  const TYPE_AS_Name & nm (cs.GetRecord(pos_AS_Class_nm));
  const SEQ<TYPE_AS_Name> & inh (cs.GetSequence(pos_AS_Class_supercls));
  const Generic & defs (cs.GetField(pos_AS_Class_defs));

  Sequence cpp_l;
  Sequence pr_l;
  Sequence prtype_l;

  SEQ<TYPE_CPP_MemberDeclaration> evpp;

//  ClearUncaught(); // 20061204 for clear static member list

  if (isInterface && !IsPossibleInterface(cs)) {
    CannotCG(L"Erroneous attempt to generate an interface for class ", nm);
  }
  if (get_conc_option()) {
    InitClassState( nm );
    if (!isInterface) {
      evpp = GenEvaluatePP( nm );
    }
  }

  if (!defs.IsNil ()) {
    TYPE_AS_Definitions defin (defs);
    const MAP<TYPE_AS_Name, TYPE_AS_TypeDef> & tps (defin.GetMap(pos_AS_Definitions_typem));
// 20120208 -->
//    const SEQ<TYPE_AS_ValueDef> & vals (defin.GetSequence(pos_AS_Definitions_valuem));
    const SEQ<TYPE_AS_ValueDef> & vals (GetJCGOption(defin.GetSequence(pos_AS_Definitions_valuem)));
// <-- 20120208
    const MAP<TYPE_AS_Name, TYPE_AS_FnDef> & fcts (defin.GetMap(pos_AS_Definitions_fnm));
    const MAP<TYPE_AS_Name, TYPE_AS_OpDef> & oper (defin.GetMap(pos_AS_Definitions_opm));
    const SEQ<TYPE_AS_InstanceVarDef> & iVars (defin.GetSequence(pos_AS_Definitions_instvars));

    // comparator only needed if TreeSet is used as jdk implementation  of sets
// 20110526 -->
    //if (!get_onlytypes_option() && !isInterface)
    //  pr_l.ImpAppend(GenLocalComparator(nm));
// <-- 20110526

    SEQ<TYPE_AS_TypeDef> type_l;
    Set dom_tps (tps.Dom());
    Generic tp;
    for (bool bb = dom_tps.First (tp); bb; bb = dom_tps.Next (tp)) {
      type_l.ImpAppend (tps[tp]);
    }

    if (! get_onlytypes_option()) {
      GenValues(nm, vals);
      if (isInterface) {
        cpp_l.ImpConc(GetIdeclL());
      }
      else {
        pr_l.ImpConc(GenInstVars(iVars));
        cpp_l.ImpConc(GenJavaClassInit());
      }
    }

    prtype_l = GenTypeDef_DS(nm,type_l);
    cpp_l.ImpConc(GenInvEqOrdDef(type_l));

    if (! get_onlytypes_option()) {
      if (!isInterface) {
        cpp_l.ImpAppend(GenInstanceVarInit(nm, iVars));
        cpp_l.ImpConc(GenInstanceInv(nm, iVars));
      }

      Set dom_fcts (fcts.Dom());
      Map cidtocpp;
      Generic fnm;
      for (bool cc = dom_fcts.First (fnm); cc; cc = dom_fcts.Next (fnm)) {
        TYPE_AS_FnDef fn (fcts[fnm]);
        TYPE_CI_ContextId fcid (ASTAUX::GetCid(fn));
        CurrentMethod(fnm);
        TYPE_CPP_CPPAS fctdefcpp (GenFctDef_FD(fn, false));
        if (!fctdefcpp.IsEmpty())
            cidtocpp.Insert(fcid, fctdefcpp);
      }

      Set dom_oper (oper.Dom());
      Generic onm;
      for (bool dd = dom_oper.First (onm); dd; dd = dom_oper.Next (onm)) {
        TYPE_AS_OpDef op (oper[onm]);
        TYPE_CI_ContextId fcid (ASTAUX::GetCid(op));
//        bool constr = ASTAUX::GetConstr(onm);
        CurrentMethod(onm);
        TYPE_CPP_CPPAS opdefcpp (GenOpDef_FD(onm, op, Nil(), false));

        if (!opdefcpp.IsEmpty()) {
            cidtocpp.Insert(fcid, opdefcpp);
        }
      }

      if ((!isInterface) && !GetHasDefaultConstr()) {
        cpp_l.ImpConc (GenJavaConstructorDef(nm, iVars));
      }

      while (!cidtocpp.Dom().IsEmpty()) {
        SET<TYPE_CI_ContextId> cidtocppdom (cidtocpp.Dom());
        TYPE_CI_ContextId cid (cidtocppdom.GetElem());
        Generic gg;
        for (bool ff = cidtocppdom.First (gg); ff; ff = cidtocppdom.Next (gg)) {
          TYPE_CI_ContextId tempcid (gg);
          Tuple gfp1 (GetCI().GetFilePos(cid));
          const TYPE_CI_TokenPos & tst (gfp1.GetRecord(3));
          int ast_start = tst.GetIntValue(pos_CI_TokenPos_abs_uline);
          Tuple gfp2 (GetCI().GetFilePos(tempcid));
          const TYPE_CI_TokenPos & tst2 (gfp2.GetRecord(3));
          int ast_startf = tst2.GetIntValue(pos_CI_TokenPos_abs_uline);
          if (ast_start > ast_startf) {
            cid = tempcid;
          }
        }
        cpp_l.ImpConc(cidtocpp[cid]);
        cidtocpp.RemElem(cid);
      }
    }
  }

  if (get_conc_option() && !isInterface) {
    pr_l.ImpConc(GenLocalSentinel( nm, defs ));
    pr_l.ImpConc(evpp);
    pr_l.ImpAppend(GenSetSentinelMethod( nm));

    Generic thread (((const TYPE_AS_Definitions &)defs).get_threaddef());
    if (!thread.IsNil()) {
      TYPE_AS_ThreadDef threadRec (thread);
      switch(threadRec.GetTag()) {
        case TAG_TYPE_AS_PerObl: {
          pr_l.ImpConc(GenPeriodicStartMethod());
          break;
        }
        default: { // AS`Stmt
          pr_l.ImpConc(GenProceduralThread(threadRec));
          break;
        }
      }
    }
  }

  SEQ<TYPE_CPP_ImportDeclaration> impdecls (GenImportDeclarations());

  Generic pack;
  if (get_package_option().IsNil()) {
    pack = nil;
  }
  else {
    pack = vdm_BC_GenPackageDeclaration(get_package_option());
  }
  TYPE_CPP_PackageAndImportDeclarations decl (vdm_BC_GenPackageAndImportDeclarations(pack, impdecls));

  if (ContainsImplicitFct() && !isInterface) {
    type_dL childArg;
    if (!ContainsStaticImplicit()) {
      childArg.ImpAppend(GenThis());
    }
    TYPE_CPP_Identifier cn (vdm_BC_GenIdentifier(ASTAUX::MkId(L"external_").ImpConc(GiveCurCName())));
    TYPE_CPP_Identifier obj (vdm_BC_GenIdentifier(ASTAUX::MkId(L"child")));
    TYPE_CPP_AsgnInit ai (vdm_BC_GenAsgnInit(vdm_BC_GenClassInstanceCreationExpr(cn, childArg)));
    TYPE_CPP_DeclarationStmt makeinst (vdm_BC_GenDecl(vdm_BC_GenTypeSpecifier(cn),obj,ai));
    TYPE_CPP_IdentDeclaration idcl (makeinst.get_decl());
    SEQ<TYPE_CPP_DeclSpecifier> ds_l;
    ds_l.ImpConc(idcl.get_ds());
    SEQ<TYPE_CPP_InitDeclarator> dl (idcl.get_dl());

    TYPE_CPP_Modifier pro (vdm_BC_GenModifier(quote_PROTECTED));
    ds_l.ImpPrepend(pro);
    if (ContainsStaticImplicit()) {
      ds_l.ImpPrepend(vdm_BC_GenModifier(quote_STATIC));
    }
    pr_l.ImpAppend(vdm_BC_GenIdentDeclaration(SEQ<TYPE_CPP_Annotation>(), ds_l, dl));
  }

  Record head (GenHead(nm, inh, isInterface)); // CPP`ClassHead | CPP`InterfaceHead

  type_dL classbody;
  classbody.ImpConc(prtype_l);
  classbody.ImpConc(pr_l);
  classbody.ImpConc(cpp_l);

  type_dL cs_l;
  if (isInterface) {
    cs_l.ImpAppend(vdm_BC_GenTypeInterfaceSpecifier((const TYPE_CPP_InterfaceHead &)head, classbody));
  }
  else {
    cs_l.ImpAppend(vdm_BC_GenTypeClassSpecifier((const TYPE_CPP_ClassHead &)head, classbody));
  }
  TYPE_CPP_CPPAS new_cpp_l;
  new_cpp_l.ImpAppend(vdm_BC_GenIdentDeclaration(SEQ<TYPE_CPP_Annotation>(), cs_l, nil));
  Generic package_name (get_package_option());

// 20120522 -->
//  return vdm_BC_GenFile(PackageToDir(package_name), GiveLastName(nm).ImpConc(ASTAUX::MkId(L".java")), decl,
//                        new_cpp_l);
  return vdm_BC_GenFile2(PackageToDir(package_name), GiveLastName(nm).ImpConc(ASTAUX::MkId(L".java")), decl,
                          new_cpp_l, cs.GetInt(pos_AS_Class_cid));
// <--20120522
}

// GenJavaExternalFile
// cls : AS`Class
// ==> CPP`File
TYPE_CPP_File vdmcg::GenJavaExternalFile(const TYPE_AS_Class & cls)
{
  const TYPE_AS_Name & nm (cls.GetRecord(pos_AS_Class_nm));
//  const SEQ<TYPE_AS_Name> & inh (cls.GetSequence(pos_AS_Class_supercls));
  const Generic defs (cls.GetField(pos_AS_Class_defs));

  SEQ<TYPE_CPP_ImportDeclaration> impdecls (GenImportDeclarations());

  Generic pack;
  if (get_package_option().IsNil())
    pack = nil;
  else
    pack = vdm_BC_GenPackageDeclaration(get_package_option());

  TYPE_CPP_PackageAndImportDeclarations decl (vdm_BC_GenPackageAndImportDeclarations(pack, impdecls));

  TYPE_CPP_TypeSpecifier cnm (vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(GiveCurCName())));
  TYPE_CPP_TypeSpecifier tempnm (vdm_BC_GenIdentifier(ASTAUX::MkId(L"parent")));
  TYPE_CPP_AsgnInit ai (vdm_BC_GenAsgnInit(GenNullExpr()));
  TYPE_CPP_DeclarationStmt dstmt (vdm_BC_GenDecl(cnm, tempnm, ai));
  TYPE_CPP_IdentDeclaration idcl (dstmt.get_decl());

  type_dL cpp_l;
  cpp_l.ImpAppend(idcl);
  cpp_l.ImpConc(GenConstructorExternalJavaDef());
  cpp_l.ImpConc(GetImplicitFcts());

  TYPE_CPP_ClassHead  head (GenExternalClassHead());

  SEQ<TYPE_CPP_DeclSpecifier> cs_l;
  cs_l.ImpAppend(vdm_BC_GenClassSpecifier(head, cpp_l));

  TYPE_CPP_CPPAS cpptemp_l;
  cpptemp_l.ImpAppend(vdm_BC_GenIdentDeclaration(SEQ<TYPE_CPP_Annotation>(), cs_l, nil));
  Generic package_name (get_package_option());

  return vdm_BC_GenFile(PackageToDir(package_name),
                        ASTAUX::MkId(L"external_").ImpConc(GiveLastName(nm)).ImpConc(ASTAUX::MkId(L".java")),
                        decl,
                        cpptemp_l);
}

// GenImportDeclarations
// ==> CPP`ImportDeclarations
SEQ<TYPE_CPP_ImportDeclaration> vdmcg::GenImportDeclarations()
{
  SEQ<TYPE_CPP_ImportDeclaration> impdecls;
// 20120208 -->
  Set inm_s (get_imports_names());
  Generic nm;
  for (bool bb = inm_s.First(nm); bb; bb = inm_s.Next(nm))
    //impdecls.ImpAppend(vdm_BC_GenSingleTypeImportDeclaration(vdm_BC_GenIdentifier(nm)));
    InsertImport(nm);
// <-- 20120208

  InsertImport(SEQ<Char>(L"jp.vdmtools.VDM.UTIL"));

  if (get_conc_option()) {
    InsertImport(SEQ<Char>(L"jp.vdmtools.VDM.EvaluatePP"));
    InsertImport(SEQ<Char>(L"jp.vdmtools.VDM.Sentinel"));
  }
  impdecls.ImpConc(GenTypeImportDeclarations());

  if (get_j2v_option()) {
    impdecls.ImpAppend(vdm_BC_GenImportOnDemandDeclaration(GenPackageName(TYPE_AS_Ids().ImpAppend(ASTAUX::MkId(L"jp"))
                                                                                       .ImpAppend(ASTAUX::MkId(L"vdmtools"))
                                                                                       .ImpAppend(ASTAUX::MkId(L"VDM"))
                                                                                       .ImpAppend(ASTAUX::MkId(L"jdk")))));
  }
  return impdecls;
}

// GenJavaClassInit
// ==> seq of CPP`MemberDeclaration
SEQ<TYPE_CPP_MemberDeclaration> vdmcg::GenJavaClassInit()
{
  TYPE_CPP_TypeSpecifier l_ts (vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Throwable"))));
  TYPE_CPP_ExceptionDeclaration l_ed (vdm_BC_GenExceptionDeclaration(
                                        SEQ<TYPE_CPP_TypeSpecifier>().ImpAppend(l_ts),
                                        vdm_BC_GenIdentifier(ASTAUX::MkId(L"e"))));
  TYPE_CPP_Expr l_fc1 (vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"e.getMessage")),
                                         SEQ<TYPE_CPP_Expr>()));
  TYPE_CPP_Expr l_fc2 (vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"System.out.println")),
                                           SEQ<TYPE_CPP_Expr>().ImpAppend(l_fc1)));
  TYPE_CPP_Expr l_fc3 (vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"e.printStackTrace")),
                                           SEQ<TYPE_CPP_Expr>().ImpAppend(vdm_BC_GenIdentifier(ASTAUX::MkId(L"System.out")))));

  TYPE_CPP_Stmt l_stmt (vdm_BC_GenBlock(type_dL().ImpAppend(vdm_BC_GenExpressionStmt(l_fc3))
                                                 .ImpAppend(vdm_BC_GenExpressionStmt(l_fc2))));

  TYPE_CPP_Handler l_handler (vdm_BC_GenHandler(l_ed, l_stmt));
  SEQ<TYPE_CPP_Handler> l_handlers;
  l_handlers.ImpAppend(l_handler);

  SEQ<TYPE_CPP_Stmt> l_uncaughtBody (GetUncaught());

  SEQ<TYPE_CPP_Stmt> l_init;
  if (!l_uncaughtBody.IsEmpty())
    l_init.ImpAppend(vdm_BC_GenTryBlock(l_uncaughtBody, l_handlers, Nil()));

  TYPE_CPP_Identifier cpp_id (vdm_BC_GenIdentifier(ASTAUX::MkId(L"static")));
  TYPE_CPP_FctDecl fdecl (vdm_BC_GenStaticInitializerDecl(cpp_id));

  SEQ<TYPE_CPP_MemberDeclaration> idecl_l (GetIdeclL());
  SEQ<TYPE_CPP_Stmt> l_caughtBody (GetValInit());

  SEQ<TYPE_CPP_Stmt> l_body;
  l_body.ImpConc(l_caughtBody);
  l_body.ImpConc(l_init);

  if (!l_body.IsEmpty())
    idecl_l.ImpAppend(vdm_BC_GenFctDef(SEQ<TYPE_CPP_DeclSpecifier>(),
                                       fdecl,
                                       nil,
                                       vdm_BC_GenBlock(l_body)));
  return idecl_l;
}

// GenClassInit
// nm : AS`Name
// tps : seq of AS`TypeDef
// ==> CPP`CPPAS
TYPE_CPP_CPPAS vdmcg::GenClassInit(const TYPE_AS_Name& nm,
                                   const SEQ<TYPE_AS_TypeDef> & tps)
{
  SEQ<TYPE_CPP_Stmt> body;
  size_t len_tps = tps.Length();
  for (size_t i = 1; i <= len_tps; i++)
  {
    TYPE_AS_TypeDef td (tps[i]);
    TYPE_AS_Type type (td.get_shape());
    Generic acc (td.get_access());
    if (type.Is(TAG_TYPE_AS_CompositeType))
    {
      TYPE_AS_CompositeType ct (type);
      TYPE_AS_Name name (ct.get_name());
      SEQ<TYPE_AS_Field> field_l (ct.get_fields());
      TYPE_CPP_Identifier tag (GenCompositeTypeTagId(name));
      int size (field_l.Length());

      body.ImpAppend(GenNewTag(tag, size));
      body.ImpAppend(GenSetSymTag(tag, name, nm));

      size_t len(field_l.Length());
      for (size_t no = 1; no <= len; no++ ) {
        if ( field_l[no].get_dc().GetValue() )
              body.ImpAppend(GenSetDontCare(tag, no));
      }
      body.ImpAppend(GenSetRecordTag(tag, name, nm));
    }
  }
  body.ImpConc(GetValInit());

  TYPE_CPP_Identifier cpp_id (vdm_BC_GenIdentifier(ASTAUX::MkId(L"init_").ImpConc(GiveLastName(nm))));
  TYPE_CPP_ClassHead head (vdm_BC_GenClassHead(cpp_id, SEQ<TYPE_CPP_BaseSpecifier>()));
  TYPE_CPP_FctDecl fdecl (vdm_BC_GenFctDecl(cpp_id, type_dL()));
  SEQ<TYPE_CPP_MemberList> members (vdm_BC_GenPublic(type_dL().ImpAppend(
                                      vdm_BC_GenFctDef(type_dL(),
                                                       fdecl,
                                                       nil,
                                                       vdm_BC_GenBlock(body)))));
  TYPE_CPP_ClassSpecifier cs (vdm_BC_GenClassSpecifier(head, (Generic)members));
  type_dL cl_declArgs;
  cl_declArgs.ImpAppend(vdm_BC_GenTypeSpecifier(cs));
  TYPE_CPP_IdentDeclaration cl_decl(vdm_BC_GenIdentDeclaration(SEQ<TYPE_CPP_Annotation>(), cl_declArgs, nil));

  TYPE_CPP_Identifier init_id(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Init_").ImpConc(GiveLastName(nm))));
  TYPE_CPP_TypeDefName tp_nm(vdm_BC_GenTypeDefName(cpp_id));
  SEQ<TYPE_CPP_DeclSpecifier> init_declArgs;
  init_declArgs.ImpAppend(vdm_BC_GenTypeSpecifier(tp_nm));
  TYPE_CPP_IdentDeclaration init_decl(vdm_BC_GenIdentDecl(SEQ<TYPE_CPP_Annotation>(),
                                                          init_declArgs, init_id, nil));
  TYPE_CPP_CPPAS res;
  if ( !body.IsEmpty()) {
    res.ImpConc(GetIdeclL());
    res.ImpAppend(cl_decl);
    res.ImpAppend(init_decl);
  }
  return res;
}

// GenStaticInstVarInit
// p_ivds : seq of AS`InstanceVarDef
// p_clnm : AS`Name
// ==> seq of CPP`Declaration
SEQ<TYPE_CPP_Declaration> vdmcg::GenStaticInstVarInit(const SEQ<TYPE_AS_InstanceVarDef> & p_ivds,
                                                      const TYPE_AS_Name & p_clnm)
{
  SEQ<TYPE_CPP_Declaration> l_result;
  size_t len_p_ivds = p_ivds.Length();
  for (size_t i = 1; i <= len_p_ivds; i++)
  {
    TYPE_AS_InstanceVarDef ivd (p_ivds[i]);
    if (ivd.Is(TAG_TYPE_AS_InstAssignDef) &&
        ivd.GetBoolValue(pos_AS_InstAssignDef_stat) &&
        ivd.GetRecord(pos_AS_InstAssignDef_ad).GetField(pos_AS_AssignDef_dclinit).IsNil()) // 20110525
    {
      const TYPE_AS_AssignDef & l_ad (ivd.GetRecord(pos_AS_InstAssignDef_ad));
      TYPE_REP_TypeRep l_rtp (FromAS2RepType(l_ad.GetRecord(pos_AS_AssignDef_tp)));
      TYPE_CPP_Name l_nm (vdm_BC_Rename(QualiName(p_clnm, l_ad.GetRecord(pos_AS_AssignDef_var))));
      TYPE_CGMAIN_VT l_vt (mk_CG_VT(l_nm, l_rtp));
      SEQ<TYPE_AS_Stmt> l_cppinit (CGExpr (l_ad.GetField(pos_AS_AssignDef_dclinit), l_vt));

      Generic l_initval = Nil();
      if (l_cppinit.Length() == 1) {
        TYPE_AS_Stmt stmt (l_cppinit.Hd());
        if (stmt.Is(TAG_TYPE_CPP_ExpressionStmt)) {
          TYPE_CPP_Expr r2 (TYPE_CPP_ExpressionStmt(stmt).get_expr());
          if (r2.Is(TAG_TYPE_CPP_AssignExpr))
            l_initval = vdm_BC_GenAsgnInit( TYPE_CPP_AssignExpr(r2).get_assignexpr());
        }
      }

      l_result.ImpAppend(vdm_BC_GenDecl(GenType(l_rtp), l_nm, l_initval).get_decl());
    }
  }
  return l_result;
}

// GenHead
// nm : AS`Name
// inherit_l : seq of AS`Name
// isInterface : bool
// -> CPP`ClassHead | CPP`InterfaceHead
Record vdmcg::GenHead(const TYPE_AS_Name& nm, const SEQ<TYPE_AS_Name> & inherit_l, bool isInterface)
{
  TYPE_CPP_Identifier cnm (vdm_BC_Rename(nm));
  SEQ<TYPE_CPP_BaseSpecifier> base_l (GenInheritance(inherit_l));

  if (vdm_CPP_isCPP())
    return vdm_BC_GenClassHead(cnm, base_l);
  else
  { // for Java
    SEQ<TYPE_CPP_Modifier> ds;
    ds.ImpAppend(vdm_BC_GenModifier(quote_PUBLIC));

    if (IsAbstract() && !isInterface)
      ds.ImpAppend(vdm_BC_GenModifier(quote_ABSTRACT));

    SEQ<TYPE_CPP_PackageName> concinterface_l;
    if (get_conc_option()) {
      concinterface_l.ImpConc(GenConcInterface(nm));
    }

    SEQ<TYPE_CPP_PackageName> interface_l (GenInterfaceInheritance(inherit_l));

    SEQ<TYPE_CPP_Annotation> annos;
    if (isInterface)
      return vdm_BC_GenJavaInterfaceHead(annos, ds, cnm, interface_l.ImpConc(concinterface_l), nil);
    else if (base_l.Length () <= 1)
      return vdm_BC_GenJavaClassHead(annos, ds, cnm, base_l, interface_l.ImpConc(concinterface_l), nil);
    else {
      CannotCG(L"Multiple inheritance in this form", nm);
      return Record();
    }
  }
}

// GenExternalClassHead
// -> CPP`ClassHead
TYPE_CPP_ClassHead vdmcg::GenExternalClassHead()
{
  TYPE_CPP_Identifier id (vdm_BC_GenIdentifier(ASTAUX::MkId(L"external_").ImpConc(GiveCurCName())));
  SEQ<TYPE_CPP_Modifier> ds;
  TYPE_CPP_Modifier pub (vdm_BC_GenModifier(quote_PUBLIC));
  ds.ImpPrepend(pub);

  SEQ<TYPE_CPP_Annotation> annos;
  return vdm_BC_GenJavaClassHead(annos, ds, id, SEQ<TYPE_CPP_BaseSpecifier>(), nil, nil);
}

// GenInterfaceInheritance
// nms : seq of AS`Name
// ==> seq of CPP`PackageName
SEQ<TYPE_CPP_PackageName> vdmcg::GenInterfaceInheritance(const SEQ<TYPE_AS_Name> & nms)
{
  SEQ<TYPE_CPP_PackageName> res;
  size_t len_nms = nms.Length();
  for (size_t i = 1; i <= len_nms; i++)
  {
    const TYPE_AS_Name & nm (nms[i]);
    if (isInterface(nm))
    {
      TYPE_CPP_Identifier cnm (vdm_BC_Rename(nm));
      res.ImpAppend(vdm_BC_GenClass2Package(cnm));
    }
  }
  return res;
}

// GenInheritance
// inh_l : seq of AS`Name
// ==> CPP`BaseSpec
SEQ<TYPE_CPP_BaseSpecifier> vdmcg::GenInheritance(const SEQ<TYPE_AS_Name> & inh_l)
{
  SEQ<TYPE_CPP_BaseSpecifier> bs;
  size_t len_inh_l = inh_l.Length();
  for (size_t i = 1; i <= len_inh_l; i++)
  {
    const TYPE_AS_Name & nm (inh_l[i]);
    if (vdm_CPP_isCPP())
    {
      TYPE_CPP_Identifier super (vdm_BC_Rename(nm));
      bs.ImpAppend(vdm_BC_GenAccVirBaseSpec(super));
    }
    else if (!isInterface(nm))
    {
      TYPE_CPP_Identifier cnm (vdm_BC_Rename(nm));
      bs.ImpAppend(vdm_BC_GenClass2Package(cnm));
    }
  }
  if (vdm_CPP_isCPP()) {
    if ( bs.IsEmpty() ) {
      bs.ImpAppend(vdm_BC_GenAccVirBaseSpec(vdm_BC_GiveBaseName()));
      UseAuxFct();
    }
  }
  return bs;
}

// GenConstructorDef
// nm : AS`Name
//  * bool
// ==> seq of (CPP`FunctionDefinition|CPP`MemberSpecifier)
SEQ<TYPE_CPP_MemberDeclaration> vdmcg::GenConstructorDef(const TYPE_AS_Name & nm)
{
  CurrentMethod(nm);
  TYPE_CPP_Identifier fnm (vdm_BC_Rename(nm));

  // constructor
  TYPE_CPP_FctDecl fdecl (vdm_BC_GenFctDecl(fnm, type_dL()));

  SEQ<TYPE_CPP_MemberDeclaration> res;
  res.ImpAppend(vdm_BC_GenMemberSpec(type_dL(), fdecl));

  // destructor
  TYPE_CPP_FctDecl destr (vdm_BC_GenFctDecl(vdm_BC_GenDest(fnm), type_dL()));
  TYPE_CPP_Stmt stmt (vdm_BC_GenBlock(SEQ<TYPE_CPP_Stmt>()));

  type_dL vir_l;
  vir_l.ImpAppend(vdm_BC_GenModifier(quote_VIRTUAL));
  res.ImpAppend(vdm_BC_GenFctDef(vir_l, destr, nil, stmt));

  return res;
}

// GenConstructorDef_cc
// nm : AS`Name
// iInit_l : seq of AS`InstanceVarDef
// isBaseClass : bool
// isDlClass : bool
// ==> seq of (CPP`FunctionDefinition|CPP`MemberSpecifier)
SEQ<TYPE_CPP_MemberDeclaration> vdmcg::GenConstructorDef_cc(const TYPE_AS_Name& nm,
                                                            const SEQ<TYPE_AS_InstanceVarDef> & iInit_l,
                                                            int isBaseClass,
                                                            bool isDlClass)
{
  TYPE_CPP_Name fnm (vdm_BC_Rename(nm));
  TYPE_CPP_Identifier initid (vdm_BC_GivePrePostNm(nm, ASTAUX::MkId(L"init")));
  SEQ<TYPE_CPP_Stmt> base_l;
  base_l.ImpAppend(vdm_BC_GenExpressionStmt(vdm_BC_GenFctCall(initid, SEQ<TYPE_CPP_Expr>())));
  if (isDlClass)
    base_l.ImpAppend(GenDlInit(nm));

  TYPE_CPP_Stmt body (vdm_BC_GenBlock(base_l));
  TYPE_CPP_QualifiedName qname (vdm_BC_GenQualifiedName (fnm, fnm));
  TYPE_CPP_FctDecl decl (vdm_BC_GenFctDecl (qname, type_dL()));

  TYPE_CPP_FunctionDefinition fd (vdm_BC_GenFctDef (type_dL(), decl, nil, body));
  SEQ<TYPE_CPP_MemberDeclaration> res;
  res.ImpAppend (fd);
  return res;
}

// GenJavaConstructorDef
// nm : AS`Name
// iInit_l : seq of AS`InstanceVarDef
// ==> seq of CPP`FunctionDefinition
SEQ<TYPE_CPP_FunctionDefinition> vdmcg::GenJavaConstructorDef(const TYPE_AS_Name& nm,
                                                              const SEQ<TYPE_AS_InstanceVarDef> & iInit_l_)
{
  TYPE_CPP_Name fnm (vdm_BC_Rename(nm));

  TYPE_CPP_Expr initfc (vdm_BC_GenFctCall(vdm_BC_GivePrePostNm(nm, ASTAUX::MkId(L"init")),
                                          SEQ<TYPE_CPP_Expr>()));
  TYPE_CPP_Stmt body (vdm_BC_GenBlock(type_dL().ImpAppend(vdm_BC_GenExpressionStmt(initfc))));

  TYPE_CPP_FctDecl decl (vdm_BC_GenFctDecl (fnm, type_dL()));

  SEQ<TYPE_CPP_Identifier> excs (GenExceptionsHdr());

  SEQ<TYPE_CPP_Modifier> modifiers;
  modifiers.ImpPrepend(vdm_BC_GenModifier(quote_PUBLIC));

  SEQ<TYPE_CPP_FunctionDefinition> res;
  res.ImpAppend (vdm_BC_GenJavaFctDef (SEQ<TYPE_CPP_Annotation>(),
                                   modifiers, type_dL(), decl, excs, body));

  return res;
}

// GenConstructorExternalJavaDef
// nm :
// ==> seq of CPP`FunctionDefinition
SEQ<TYPE_CPP_FunctionDefinition> vdmcg::GenConstructorExternalJavaDef()
{
  TYPE_CPP_Identifier id1 (vdm_BC_GenIdentifier(ASTAUX::MkId(L"parent")));
  TYPE_CPP_Identifier id2 (vdm_BC_GenIdentifier(ASTAUX::MkId(L"parent").ImpConc(GiveCurCName())));

  type_dL body1, body2;
  body1.ImpAppend(vdm_BC_GenAsgnStmt(id1,id2));

  TYPE_CPP_TypeSpecifier id3 (vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(GiveCurCName())));
  TYPE_CPP_ArgDecl arg1 (vdm_BC_GenArgDecl(SEQ<TYPE_CPP_DeclSpecifier>().ImpAppend(id3), id2));

  type_dL arg1_l;
  arg1_l.ImpAppend(arg1);
  TYPE_CPP_FctDecl decl1 (vdm_BC_GenFctDecl(vdm_BC_GenIdentifier(ASTAUX::MkId(L"external_").ImpConc(GiveCurCName())),
                                            arg1_l));
  TYPE_CPP_FctDecl decl2 (vdm_BC_GenFctDecl(vdm_BC_GenIdentifier(ASTAUX::MkId(L"external_").ImpConc(GiveCurCName())),
                                            type_dL()));

  type_dL modifiers;
  TYPE_CPP_Modifier pub (vdm_BC_GenModifier(quote_PUBLIC));
  modifiers.ImpPrepend(pub);

  TYPE_CPP_FunctionDefinition cons1 (vdm_BC_GenConstructorDef(SEQ<TYPE_CPP_Annotation>(),
                                                              modifiers,
                                                              decl1,
                                                              SEQ<TYPE_CPP_MemInitializer>(),
                                                              vdm_BC_GenBlock(body1)));
  TYPE_CPP_FunctionDefinition cons2 (vdm_BC_GenConstructorDef(SEQ<TYPE_CPP_Annotation>(),
                                                              modifiers,
                                                              decl2,
                                                              SEQ<TYPE_CPP_MemInitializer>(),
                                                              vdm_BC_GenBlock(body2)));

  SEQ<TYPE_CPP_FunctionDefinition> res;
  res.ImpAppend(cons1);
  res.ImpAppend(cons2);
  return res;
}

// GenInitBaseVars
// - : AS`Name
// isBaseClass : bool
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenInitBaseVars(bool isBaseClass)
{
  TYPE_CPP_Expr fcall_vgi (vdm_BC_GenFctCall (vdm_BC_GenIdentifier(ASTAUX::MkId(L"vdm_GetId")), SEQ<TYPE_CPP_Expr>()));
  TYPE_CPP_Identifier id (isBaseClass ? vdm_BC_GenIdentifier(ASTAUX::MkId(L"RegisterAsBase"))
                                      : vdm_BC_GenIdentifier(ASTAUX::MkId(L"RegisterAsDerived")));

  SEQ<TYPE_CPP_Stmt> stmt_l;
  stmt_l.ImpAppend(vdm_BC_GenExpressionStmt (vdm_BC_GenFctCall (id, mk_sequence(fcall_vgi))));
  return stmt_l;
}

// GenDlInit
// nm : AS`Name
// ==> CPP`Stmt
TYPE_CPP_Stmt vdmcg::GenDlInit(const TYPE_AS_Name& nm)
{
  TYPE_CPP_Identifier ident (vdm_BC_GenIdentifier(ASTAUX::MkId(L"DlClass_new")));

  SEQ<TYPE_CPP_Expr> args;
  args.ImpAppend(vdm_BC_GenStringLit(GiveLastName(nm)));

  TYPE_CPP_Expr fctCall (vdm_BC_GenFctCall(ident, args));
  return vdm_BC_GenAsgnStmt(GenDlMember(), fctCall);
}

// GenInstVars
// var_l : seq of AS`InstanceVarDef
// ==> AccessMemberDecl | seq of CPP`MemberDeclaration
Generic vdmcg::GenInstVars(const SEQ<TYPE_AS_InstanceVarDef> & var_l)
{
  if (vdm_CPP_isCPP())
  {
    SEQ<TYPE_AS_InstanceVarDef> var_l_q;
    size_t len_var_l = var_l.Length();
    for(size_t i = 1; i <= len_var_l; i++)
    {
      var_l_q.ImpAppend( ExchangeDefAccess (var_l[i]) );
    }

    SET<TYPE_AS_Access> acs_s;
    size_t len_var_l_q = var_l_q.Length();
    for (size_t j = 1; j <= len_var_l_q; j++)
    {
      const TYPE_AS_InstanceVarDef & ivd (var_l_q[j]);
      if (ivd.Is(TAG_TYPE_AS_InstAssignDef))
        acs_s.Insert (ivd.GetField(pos_AS_InstAssignDef_access));
    }

    Map res; // map AS`Access to seq of CPP`MemberDeclaration
    Generic a;
    for (bool dd = acs_s.First(a); dd; dd = acs_s.Next (a))
    {
      SEQ<TYPE_CPP_MemberDeclaration> giv_l;
      for (size_t k = 1; k <= len_var_l_q; k++)
      {
        const TYPE_AS_InstanceVarDef & ivd (var_l_q[k]);
        if (ivd.Is(TAG_TYPE_AS_InstAssignDef))
        {
          if (ivd.GetField(pos_AS_InstAssignDef_access) == a)
            giv_l.ImpAppend (GenInstVar (ivd));
        }
      }
      res.Insert (a, giv_l);
    }
    return res;
  }
  else // Java
  {
    Map cidtocpp; // map CI`ContextId to CPP`IdentDeclaration
    size_t len_var_l = var_l.Length();
    for(size_t i = 1; i <= len_var_l; i++)
    {
      const TYPE_AS_InstanceVarDef & ivd (var_l[i]);
      if (ivd.Is (TAG_TYPE_AS_InstAssignDef))
      {
        cidtocpp.Insert(ivd.GetInt(pos_AS_InstAssignDef_cid), GenInstVar(ivd));
      }
    }

    SEQ<TYPE_CPP_MemberDeclaration> instvars;
    while (!cidtocpp.Dom().IsEmpty())
    {
      SET<TYPE_CI_ContextId> cidtocppdom (cidtocpp.Dom());
      TYPE_CI_ContextId cid (cidtocppdom.GetElem());

      Generic gg;
      for (bool cc = cidtocppdom.First(gg); cc; cc = cidtocppdom.Next(gg))
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
      instvars.ImpAppend(cidtocpp[cid]);
      cidtocpp.RemElem(cid);
    }
    return instvars;
  }
}

// GenInstVar
// iad : AS`InstAssignDef
// ==> CPP`MemberDeclaration
TYPE_CPP_MemberDeclaration vdmcg::GenInstVar(const TYPE_AS_InstAssignDef & iad)
{
  const TYPE_AS_AssignDef & ad (iad.GetRecord(pos_AS_InstAssignDef_ad));
  const Int & acc (iad.GetInt(pos_AS_InstAssignDef_access));
  bool stat (iad.GetBoolValue(pos_AS_InstAssignDef_stat));
  const TYPE_CI_ContextId & cid (iad.GetInt(pos_AS_InstAssignDef_cid));

  const TYPE_AS_Name & nm (ad.GetRecord(pos_AS_AssignDef_var));
  const TYPE_AS_Type & tp (ad.GetRecord(pos_AS_AssignDef_tp));
  const Generic & ex (ad.GetField(pos_AS_AssignDef_dclinit));

  TYPE_REP_TypeRep rtp (FromAS2RepType(tp));

  TYPE_CPP_Name var;
  //  if (vdm_CPP_isCPP())
    var = vdm_BC_Rename(nm);
    //  else
    //    var = vdm_BC_Rename(InsertGlobalName(nm));

  if (vdm_CPP_isCPP())
  {
    SEQ<TYPE_CPP_Stmt> dstmts (GenDecl_DS(rtp, var, nil));
    TYPE_CPP_DeclarationStmt dstmt (dstmts[1]);
    TYPE_CPP_IdentDeclaration idcl (dstmt.get_decl());

    type_dL ds_l;
    ds_l.ImpConc( idcl.get_ds() );

    SEQ<TYPE_CPP_InitDeclarator> dl (idcl.get_dl());
    TYPE_CPP_InitDeclarator init (dl.Hd());

    if (stat)
      ds_l.ImpPrepend(vdm_BC_GenModifier(quote_STATIC));

    return vdm_BC_GenMemberSpec( ds_l, init.get_decl() );
  }
  else
  { // for Java
    bool inStaticInitializer = false;
//    ClearUncaught(); // 20060710 for clear static member list

    Generic arg = Nil();
    if (stat)
    {
      TYPE_AS_ValueDef vd;
      vd.Init(TYPE_AS_PatternName().Init(nm, Nil(), nm.get_cid()), tp, ex, acc, stat, cid);
      //      if (AreDefinedDirectly(Sequence().ImpAppend(vd)))
      //      {
        TYPE_REP_TypeRep l_rtp (FromAS2RepType(tp));
        TYPE_AS_Name l_nm (vdm_BC_Rename(QualiName(GiveCurCASName(), nm)));
        TYPE_CGMAIN_VT l_vt (mk_CG_VT(l_nm, l_rtp));
        SEQ<TYPE_CPP_Stmt> l_expr (CGExpr(ex, l_vt));

        if (AreDefinedDirectly(Sequence().ImpAppend(vd)) &&
            (!((l_expr.Length() != 1) ||
             !(l_expr.Hd().Is(TAG_TYPE_CPP_ExpressionStmt)))))
        {
          TYPE_CPP_AssignExpr ae (TYPE_CPP_ExpressionStmt(l_expr.Hd()).get_expr());
          arg = vdm_BC_GenAsgnInit(ae.get_assignexpr());
        }
        else
        {
          AddUncaughtValInit(l_expr);
          inStaticInitializer = true;
        }
        //      }
    }

    Generic newarg (arg);
    if (!newarg.IsNil() && ex.Is(TAG_TYPE_AS_ApplyExpr) && !inStaticInitializer)
    {
      TYPE_CPP_TypeSpecifier l_ts (vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Throwable"))));
      TYPE_CPP_ExceptionDeclaration l_ed (vdm_BC_GenExceptionDeclaration(
                                            SEQ<TYPE_CPP_TypeSpecifier>().ImpAppend(l_ts),
                                            vdm_BC_GenIdentifier(ASTAUX::MkId(L"e"))));
      TYPE_CPP_Expr l_fc1 (vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"e.getMessage")),
                                               SEQ<TYPE_CPP_Expr>()));
      TYPE_CPP_Expr l_fc2 (vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"System.out.println")),
                                               SEQ<TYPE_CPP_Expr>().ImpAppend(l_fc1)));
      TYPE_CPP_Stmt l_stmt (vdm_BC_GenExpressionStmt(l_fc2));
      TYPE_CPP_Handler l_handler (vdm_BC_GenHandler(l_ed, l_stmt));

      SEQ<TYPE_CPP_Handler> l_handlers;
      l_handlers.ImpAppend(l_handler);

      TYPE_CPP_TryBlock l_init (vdm_BC_GenTryBlock(GenDecl_DS(rtp, var, newarg), l_handlers, Nil()));
      AddUncaughtValInit(Sequence().ImpAppend(l_init));
      newarg = Nil();
    }

    SEQ<TYPE_CPP_Stmt> dstmts (GenDecl_DS(rtp, var, newarg));
    TYPE_CPP_DeclarationStmt dstmt (dstmts[1]);
    TYPE_CPP_IdentDeclaration idcl (dstmt.get_decl());
    SEQ<TYPE_CPP_DeclSpecifier> ds_l (idcl.get_ds());
    SEQ<TYPE_CPP_InitDeclarator> dl (idcl.get_dl());

    type_dL modifiers;
    TYPE_CPP_Modifier mod (JavaAccess(acc));
    modifiers.ImpAppend(mod);
    if (get_conc_option()) {
      TYPE_CPP_Modifier vo (vdm_BC_GenModifier(quote_VOLATILE));
      modifiers.ImpAppend(vo);
    }
    if (stat) {
      TYPE_CPP_Modifier vo (vdm_BC_GenModifier(quote_STATIC));
      modifiers.ImpAppend(vo);
    }
    // 20140718 -->
    // annotation for instance variable
    return vdm_BC_GenIdentDeclaration(SEQ<TYPE_CPP_Annotation>(), modifiers.ImpConc(ds_l),dl);
    //TYPE_CPP_Identifier id (vdm_BC_GenIdentifier(ASTAUX::MkId(L"Instvar")));
    //TYPE_CPP_Annotation anno (vdm_BC_GenAnnotation(id, ASTAUX::MkId(L"xxxr")));
    //return vdm_BC_GenIdentDeclaration(SEQ<TYPE_CPP_Annotation>().ImpAppend(anno), modifiers.ImpConc(ds_l),dl);
    // <-- 20140718
  }
}

// GenVarInits
// iInit_l : seq of AS`InstanceVarDef
// -> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenVarInits(const SEQ<TYPE_AS_InstanceVarDef> & iInit_l)
{
  SEQ<TYPE_CPP_Stmt> res;
  if (!iInit_l.IsEmpty() )
  {
    size_t len_iInit_l = iInit_l.Length();
    for (size_t i = 1; i <= len_iInit_l; i++)
    {
      const TYPE_AS_InstanceVarDef & ivd (iInit_l[i]);
      if (ivd.Is(TAG_TYPE_AS_InstAssignDef))
      {
        if (!ivd.GetRecord(pos_AS_InstAssignDef_ad).GetField(pos_AS_AssignDef_dclinit).IsNil () &&
            !ivd.GetBoolValue(pos_AS_InstAssignDef_stat)) // 20110510
          res.ImpConc (GenVarInit (ivd));
      }
    }
  }
  return res;
}

// GenVarInit
// ii : AS`InstAssignDef
// -> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenVarInit(const TYPE_AS_InstAssignDef & ii)
{
  const TYPE_AS_AssignDef & ad (ii.GetRecord(pos_AS_InstAssignDef_ad));

  const TYPE_AS_Name & var (ad.GetRecord(pos_AS_AssignDef_var));
  const TYPE_AS_Type & tp  (ad.GetRecord(pos_AS_AssignDef_tp));
  const Generic & dclinit (ad.GetField(pos_AS_AssignDef_dclinit));

// 20130313 -->
//  TYPE_CPP_Expr varname (vdm_CPP_isCPP() ? vdm_BC_Rename (var) : vdm_BC_Rename (InsertGlobalName(var)));
  TYPE_CPP_Expr varname (vdm_BC_Rename (InsertGlobalName(var)));
// <-- 20130313
  TYPE_REP_TypeRep tpe (FromAS2RepType (tp));
  TYPE_CGMAIN_VT vt (mk_CG_VT (varname, tpe));
// 20100706 -->
//  return CGExpr (dclinit, vt);
  if (vdm_CPP_isCPP())
    return CGExpr (dclinit, vt);
  else
  {
// 20121210 -->
    // hack for String 
    //return CGExpr (dclinit, vt);
    SEQ<TYPE_CPP_Stmt> stmts (CGExpr (dclinit, vt));
    if ((stmts.Length() == 1) && IsStringType(tpe)) {
      const TYPE_CPP_Stmt & stmt (stmts[1]);
      if (stmt.Is(TAG_TYPE_CPP_ExpressionStmt) &&
          stmt.GetField(pos_CPP_ExpressionStmt_expr).Is(TAG_TYPE_CPP_AssignExpr)) {
        const TYPE_CPP_AssignExpr & ae (stmt.GetRecord(pos_CPP_ExpressionStmt_expr));
        if ((ae.GetField(pos_CPP_AssignExpr_assignop) 
                   == TYPE_CPP_AssignOp().Init(Quote(L"ASEQUAL"), NilContextId)) &&
             ae.GetRecord(pos_CPP_AssignExpr_assignexpr).Is(TAG_TYPE_CPP_ClassInstanceCreationExpr)) {
          const TYPE_CPP_ClassInstanceCreationExpr & cice (ae.GetRecord(pos_CPP_AssignExpr_assignexpr));
          if (cice.GetRecord(pos_CPP_ClassInstanceCreationExpr_classtype) == GenStringType().get_tp()) {
            TYPE_CPP_AssignExpr new_ae (ae);
            new_ae.SetField(pos_CPP_AssignExpr_assignexpr, cice.GetSequence(pos_CPP_ClassInstanceCreationExpr_arg)[1]); 
            SEQ<TYPE_CPP_Stmt> rb;
            rb.ImpAppend(vdm_BC_GenExpressionStmt(new_ae));
            return rb;
          }
        }
      }
    }
    return stmts;
// <-- 20121210
  }
// <-- 20100706
}

// IsCandidateInterface
// cl : AS`Class
// ==> bool
bool vdmcg::IsCandidateInterface(const TYPE_AS_Class & cl)
{
  if (cl.get_defs().IsNil())
    return true;
  else {
    const TYPE_AS_Definitions & defs (cl.GetRecord(pos_AS_Class_defs));
    const MAP<TYPE_AS_Name, TYPE_AS_TypeDef> & typem (defs.GetMap(pos_AS_Definitions_typem));
    const SEQ<TYPE_AS_ValueDef> & valuem (defs.GetSequence(pos_AS_Definitions_valuem));
    const MAP<TYPE_AS_Name, TYPE_AS_FnDef> & fnm (defs.GetMap(pos_AS_Definitions_fnm));
    const MAP<TYPE_AS_Name, TYPE_AS_OpDef> & opm (defs.GetMap(pos_AS_Definitions_opm));

    InitState_CGAUX(cl.GetRecord(pos_AS_Class_nm));

// 20120904 -->
    size_t len_valuem = valuem.Length();
    SEQ<TYPE_AS_ValueDef> vals;
    for (size_t idx = 1; idx <= len_valuem; idx++)
    {
      const TYPE_AS_ValueDef & vd (valuem[idx]);
      const TYPE_AS_Pattern & p (vd.GetRecord(pos_AS_ValueDef_pat));
      if (p.Is(TAG_TYPE_AS_PatternName) && !p.GetField(pos_AS_PatternName_nm).IsNil())
      {
        const TYPE_AS_Name & nm (p.GetField(pos_AS_PatternName_nm));
        if (Sequence(nm.GetSequence(pos_AS_Name_ids)[1]).SubSequence(1,11) != Sequence(L"JCGControl_"))
          vals.ImpAppend(vd);
      }
    }

    //bool isCandidateInterface = AreDefinedDirectly(valuem) &&
    bool isCandidateInterface = AreDefinedDirectly(vals) &&
// <-- 20120904
                                defs.GetSequence(pos_AS_Definitions_instvars).IsEmpty() &&
                                defs.GetSequence(pos_AS_Definitions_syncs).IsEmpty() &&
                                defs.GetField(pos_AS_Definitions_threaddef).IsNil();

    SET<TYPE_AS_TypeDef> rng_typem (typem.Rng());
    Generic tpd;
    for (bool bb = rng_typem.First(tpd); bb && isCandidateInterface; bb = rng_typem.Next(tpd))
      isCandidateInterface = TYPE_AS_TypeDef(tpd).get_access() == Int(PUBLIC_AS);

    SET<TYPE_AS_FnDef> rng_fnm (fnm.Rng());
    Generic fd;
    for (bool cc = rng_fnm.First(fd); cc && isCandidateInterface; cc = rng_fnm.Next(fd))
      isCandidateInterface = IsAbstractFunction(fd);

    SET<TYPE_AS_OpDef> rng_opm (opm.Rng());
    Generic od;
    for (bool dd = rng_opm.First(od); dd && isCandidateInterface; dd = rng_opm.Next(od))
      isCandidateInterface = (IsAbstractOperation(od) || (get_j2v_option() && IsJ2VAbstractOp(od)));

    return isCandidateInterface;
  }
}

// IsJ2VAbstractOp
// opdef : AS`OpDef
// ==> bool
bool vdmcg::IsJ2VAbstractOp(const TYPE_AS_OpDef& opdef) const
{
  switch(opdef.GetTag()) {
    case TAG_TYPE_AS_ExplOpDef: {
      //return (opdef.GetRecord(pos_AS_ExplOpDef_body).GetField(pos_AS_OpBody_body) == Int(NOTYETSPEC)) &&
      return (opdef.GetRecord(pos_AS_ExplOpDef_body).GetField(pos_AS_OpBody_body) == Int(NOTYETSPEC)) ||
             (opdef.GetBoolValue(pos_AS_ExplOpDef_constr) &&
              opdef.GetSequence(pos_AS_ExplOpDef_parms).IsEmpty());
    }
    case TAG_TYPE_AS_ExtExplOpDef: {
      //return (opdef.GetRecord(pos_AS_ExtExplOpDef_body).GetField(pos_AS_OpBody_body) == Int(NOTYETSPEC)) &&
      return (opdef.GetRecord(pos_AS_ExtExplOpDef_body).GetField(pos_AS_OpBody_body) == Int(NOTYETSPEC)) ||
             (opdef.GetBoolValue(pos_AS_ExtExplOpDef_constr) &&
              opdef.GetSequence(pos_AS_ExtExplOpDef_partps).IsEmpty());
    }
    default:
      return false;
  }
}

// IsPossibleInterface
// cl : AS`Class
// -> bool
bool vdmcg::IsPossibleInterface(const TYPE_AS_Class & cl)
{
  const SEQ<TYPE_AS_Name> & supercl (cl.GetSequence(pos_AS_Class_supercls));

  bool forall = IsCandidateInterface(cl);
  size_t len_supercl = supercl.Length();
  for (size_t idx = 1; (idx <= len_supercl) && forall; idx++) {
    const TYPE_AS_Name & sc (supercl[idx]);
    forall = (isInterface(sc) || (get_j2v_option() && (sc == ASTAUX::MkName(L"Nullable"))));
  }
  return forall;
}

// GetPossibleInterfaces
// classes : set of AS`Class
// ==> set of AS`Name
SET<TYPE_AS_Name> vdmcg::GetPossibleInterfaces(const SEQ<TYPE_AS_Class> & classes)
{
  vdm_CPP_setmode(Quote(L"JAVA"));

  SET<TYPE_AS_Name> result;
  size_t len_classes = classes.Length();
  for (size_t idx = 1; idx <= len_classes; idx++)
  {
    const TYPE_AS_Class & cl (classes[idx]);
    const TYPE_AS_Name & nm (cl.GetRecord(pos_AS_Class_nm));
    InitState_TPGEN(nm);
    if (IsPossibleInterface(cl))
      result.Insert(nm);
  }
  return result;
}

// GenInstanceVarInit
// nm : AS`Name
// iVars : seq of AS`InstanceVarDef
// ==> CPP`FunctionDefinition
TYPE_CPP_FunctionDefinition vdmcg::GenInstanceVarInit(const TYPE_AS_Name& nm, const SEQ<TYPE_AS_InstanceVarDef> & iVars)
{
  type_dL Init_l;

  if (get_conc_option())
    Init_l.ImpConc(GenConstrExtensions(nm));

  Init_l.ImpConc(GenVarInits(iVars));

  Generic body = Nil();
  if (!Init_l.IsEmpty())
  {
// 20120213 -->
    bool allsimple = !get_conc_option();
    if (!iVars.IsEmpty() )
    {
      size_t len_iVars = iVars.Length();
      for (size_t i = 1; (i <= len_iVars) && allsimple; i++)
      {
        const TYPE_AS_InstanceVarDef & ivd (iVars[i]);
        if (ivd.Is(TAG_TYPE_AS_InstAssignDef))
        {
          if (!ivd.GetRecord(pos_AS_InstAssignDef_ad).GetField(pos_AS_AssignDef_dclinit).IsNil () &&
              !ivd.GetBoolValue(pos_AS_InstAssignDef_stat)) // 20110510
          {
            const TYPE_AS_Expr & dclinit (ivd.GetRecord(pos_AS_InstAssignDef_ad).GetField(pos_AS_AssignDef_dclinit));
            switch (dclinit.GetTag()) {
              case TAG_TYPE_AS_BoolLit:
              case TAG_TYPE_AS_CharLit:
              case TAG_TYPE_AS_TextLit:
              case TAG_TYPE_AS_QuoteLit:
              case TAG_TYPE_AS_RealLit:
              case TAG_TYPE_AS_NilLit: {
                break;
              }
              default: {
                allsimple = false;
              }
            }
          }
        }
      }
    }
    if (allsimple)
      body = vdm_BC_GenBlock(Init_l);
    else
    {
// <-- 20120213
      SEQ<TYPE_CPP_TypeSpecifier> res;
      res.ImpAppend(vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Exception"))));
      TYPE_CPP_ExceptionDeclaration expdecl (vdm_BC_GenExceptionDeclaration(
                                               res,
                                               vdm_BC_GenIdentifier(ASTAUX::MkId(L"e"))));
      TYPE_CPP_Expr fctcall1 (vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"e.getMessage")),
                                                SEQ<TYPE_CPP_Expr>()));
      TYPE_CPP_Expr fctcall2 (vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"System.out.println")),
                                                SEQ<TYPE_CPP_Expr>().ImpAppend(fctcall1)));
      TYPE_CPP_Expr fctcall3 (vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"e.printStackTrace")),
                                                SEQ<TYPE_CPP_Expr>().ImpAppend(
                                                  vdm_BC_GenIdentifier(ASTAUX::MkId(L"System.out")))));

      TYPE_CPP_Stmt stmt (vdm_BC_GenBlock(mk_sequence(vdm_BC_GenExpressionStmt(fctcall3),
                                                      vdm_BC_GenExpressionStmt(fctcall2))));

      SEQ<TYPE_CPP_Handler> handlers;
      handlers.ImpAppend(vdm_BC_GenHandler(expdecl, stmt));

      body = vdm_BC_GenTryBlock(Init_l, handlers, nil);
// 20120213 -->
    }
// <-- 20120213
  }
  else
  {
    body = vdm_BC_GenBlock(type_dL());
  }
  return vdm_BC_GenJavaFctDef(SEQ<TYPE_CPP_Annotation>(),
                          SEQ<TYPE_CPP_Modifier>().ImpAppend(vdm_BC_GenModifier(quote_PRIVATE)),
                          mk_sequence(vdm_BC_GenTypeSpecifier(vdm_BC_GenVoid())),
                          vdm_BC_GenFctDecl(vdm_BC_GivePrePostNm(nm, ASTAUX::MkId(L"init")), type_dL()),
// 20120213 -->
                          //GenExceptionsHdr(),
                          nil,
// <-- 20120213 
                          body);
}

// GenInstanceInv
// nm : AS`Name
// iVars : seq of AS`InstanceVarDef
// ==> seq of CPP`FunctionDefinition
SEQ<TYPE_CPP_FunctionDefinition> vdmcg::GenInstanceInv(const TYPE_AS_Name& nm,
                                                       const SEQ<TYPE_AS_InstanceVarDef> & iVars)
{
  SEQ<TYPE_CPP_FunctionDefinition> res;
  if (!iVars.IsEmpty() )
  {
    SEQ<TYPE_AS_Expr> expr_l;
    size_t len_iVars = iVars.Length();
    for (size_t i = 1; i <= len_iVars; i++)
    {
      const TYPE_AS_InstanceVarDef & ivd (iVars[i]);
      if (ivd.Is(TAG_TYPE_AS_InstanceInv))
      {
        expr_l.ImpAppend(ivd.GetRecord(pos_AS_InstanceInv_expr));
      }
    }
    if (!expr_l.IsEmpty())
    {
      size_t len_expr_l = expr_l.Length();
      TYPE_AS_Expr expr = expr_l[len_expr_l];
      for (size_t idx = len_expr_l - 1; idx > 0; idx--)
      {
        TYPE_AS_BinaryExpr e;
        e.Init(expr_l[idx], Int(AND), expr, ASTAUX::GetCid (expr_l[idx]));
        expr = e;
      }
      TYPE_AS_ReturnStmt rs;
      rs.Init(expr, ASTAUX::GetCid (expr_l[1]));
 
      CurrentRType(TYPE_REP_BooleanTypeRep());
      TYPE_CPP_Stmt body (vdm_BC_GenBlock(GenStmt(rs, true)));

      if (vdm_CPP_isCPP())
      {
        res.ImpAppend(vdm_BC_GenFctDef(
                        mk_sequence(GenBoolType()),
                        vdm_BC_GenFctDecl(vdm_BC_GenQualifiedName(vdm_BC_Rename(nm),
                                            vdm_BC_GivePrePostNm(nm, ASTAUX::MkId(L"inv"))),
                                            type_dL()),
                        nil,
                        body));
      }
      else
      {
        res.ImpAppend(vdm_BC_GenJavaFctDef(SEQ<TYPE_CPP_Annotation>(),
                                       SEQ<TYPE_CPP_Modifier>().ImpAppend(vdm_BC_GenModifier(quote_PUBLIC)),
                        mk_sequence(GenBoolType()),
                        vdm_BC_GenFctDecl(vdm_BC_GenIdentifier(ASTAUX::MkId(L"inv_").ImpConc(GiveLastName(nm))),
                                          type_dL()),
                        nil,
                        body));
      }
    }
    if (!res.IsEmpty())
      SetHasInv();
  }
  return res;
}

SEQ<TYPE_AS_ValueDef> vdmcg::GetJCGOption (const SEQ<TYPE_AS_ValueDef> & vals_)
{
  size_t len_vals = vals_.Length();
  SEQ<TYPE_AS_ValueDef> vals;
  for (size_t idx = 1; idx <= len_vals; idx++) {
    const TYPE_AS_ValueDef & vd (vals_[idx]);
    bool controll_found = false;
    const TYPE_AS_Pattern & p (vd.GetRecord(pos_AS_ValueDef_pat));
    if (p.Is(TAG_TYPE_AS_PatternName) && !p.GetField(pos_AS_PatternName_nm).IsNil()) {
      const TYPE_AS_Name & nm (p.GetField(pos_AS_PatternName_nm));
      if (nm == ASTAUX::MkName(L"JCGControl_package")) {
        const TYPE_AS_Expr & e (vd.GetRecord(pos_AS_ValueDef_val)); 
        if (e.Is(TAG_TYPE_AS_TextLit)) {
          if (get_package_option().IsNil()) {
            const SEQ<Char> & ps (e.GetSequence(pos_AS_TextLit_val));
            set_package_option(ps); 
          }
          controll_found = true;
        }
      }
      else if (nm == ASTAUX::MkName(L"JCGControl_imports")) {
        const TYPE_AS_Expr & e (vd.GetRecord(pos_AS_ValueDef_val)); 
        if (e.Is(TAG_TYPE_AS_SetEnumerationExpr)) {
          const SEQ<TYPE_AS_Expr> & e_l (e.GetSequence(pos_AS_SetEnumerationExpr_els));
          bool forall = true;
          Set is_s;
          size_t len_e_l = e_l.Length();
          for (size_t i = 1; (i <= len_e_l) && forall; i++) {
            if (e_l[i].Is(TAG_TYPE_AS_TextLit)) {
              is_s.Insert(e_l[i].GetSequence(pos_AS_TextLit_val));
            }
            else {
              forall = false;
            }
          }
          if (forall) {
            set_imports_names(is_s);
            controll_found = true;
          }
        }
      }
      else if (nm == ASTAUX::MkName(L"JCGControl_interfaces")) {
        const TYPE_AS_Expr & e (vd.GetRecord(pos_AS_ValueDef_val)); 
        if (e.Is(TAG_TYPE_AS_SetEnumerationExpr)) {
          const SEQ<TYPE_AS_Expr> & e_l (e.GetSequence(pos_AS_SetEnumerationExpr_els));
          bool forall = true;
          Set ifs_s;
          size_t len_e_l = e_l.Length();
          for (size_t i = 1; (i <= len_e_l) && forall; i++) {
            if (e_l[i].Is(TAG_TYPE_AS_TextLit)) {
              ifs_s.Insert(ASTAUX::MkNameFromId(e_l[i].GetSequence(pos_AS_TextLit_val), NilContextId));
            }
            else {
              forall = false;
            }
          }
          if (forall)
          {
            set_interfaces(ifs_s);
            controll_found = true;
          }
        }
      }
      else if (nm == ASTAUX::MkName(L"JCGControl_entries")) {
        const TYPE_AS_Expr & e (vd.GetRecord(pos_AS_ValueDef_val)); 
        if (e.Is(TAG_TYPE_AS_SetEnumerationExpr)) {
          const SEQ<TYPE_AS_Expr> & e_l (e.GetSequence(pos_AS_SetEnumerationExpr_els));
          bool forall = true;
          Set etrs_s;
          size_t len_e_l = e_l.Length();
          for (size_t i = 1; (i <= len_e_l) && forall; i++) {
            if (e_l[i].Is(TAG_TYPE_AS_TextLit)) {
              etrs_s.Insert(ASTAUX::MkNameFromId(e_l[i].GetSequence(pos_AS_TextLit_val), NilContextId));
            }
            else {
              forall = false;
            }
          }
          if (forall) {
            set_entries_names(etrs_s);
            controll_found = true;
          }
        }
      }
      else if (nm == ASTAUX::MkName(L"JCGControl_throws")) {
        const TYPE_AS_Expr & e (vd.GetRecord(pos_AS_ValueDef_val)); 
        if (e.Is(TAG_TYPE_AS_SetEnumerationExpr)) {
          const SEQ<TYPE_AS_Expr> & e_l (e.GetSequence(pos_AS_SetEnumerationExpr_els));
          bool forall = true;
          Set thrs_s;
          size_t len_e_l = e_l.Length();
          for (size_t i = 1; (i <= len_e_l) && forall; i++) {
            if (e_l[i].Is(TAG_TYPE_AS_TextLit)) {
              thrs_s.Insert(ASTAUX::MkNameFromId(e_l[i].GetSequence(pos_AS_TextLit_val), NilContextId));
            }
            else {
              forall = false;
            }
          }
          if (forall) {
            set_throws_names(thrs_s);
            controll_found = true;
          }
        }
      }
      else if (nm == ASTAUX::MkName(L"JCGControl_integer")) {
        const TYPE_AS_Expr & e (vd.GetRecord(pos_AS_ValueDef_val)); 
        if (e.Is(TAG_TYPE_AS_SetEnumerationExpr)) {
          const SEQ<TYPE_AS_Expr> & e_l (e.GetSequence(pos_AS_SetEnumerationExpr_els));
          bool forall = true;
          Set thrs_s;
          size_t len_e_l = e_l.Length();
          for (size_t i = 1; (i <= len_e_l) && forall; i++) {
            if (e_l[i].Is(TAG_TYPE_AS_TextLit)) {
              TYPE_AS_Name nm (ASTAUX::MkName(e_l[i].GetSequence(pos_AS_TextLit_val).GetString()));
              TYPE_AS_Ids ids (nm.GetSequence(pos_AS_Name_ids));
              TYPE_AS_Id cnm (ids.Length() == 1 ? ASTAUX::MkId(L"*") : ids[1]);
              TYPE_AS_Id mnm (ids.Length() == 1 ? ids[1] : ids[2]);
              AddJavaPrimitiveIntType(cnm, mnm);
            }
            else {
              forall = false;
            }
          }
          controll_found = forall;
        }
      }
      else if (nm == ASTAUX::MkName(L"JCGControl_boolean")) {
        const TYPE_AS_Expr & e (vd.GetRecord(pos_AS_ValueDef_val)); 
        if (e.Is(TAG_TYPE_AS_SetEnumerationExpr)) {
          const SEQ<TYPE_AS_Expr> & e_l (e.GetSequence(pos_AS_SetEnumerationExpr_els));
          bool forall = true;
          Set thrs_s;
          size_t len_e_l = e_l.Length();
          for (size_t i = 1; (i <= len_e_l) && forall; i++) {
            if (e_l[i].Is(TAG_TYPE_AS_TextLit)) {
              TYPE_AS_Name nm (ASTAUX::MkName(e_l[i].GetSequence(pos_AS_TextLit_val).GetString()));
              TYPE_AS_Ids ids (nm.GetSequence(pos_AS_Name_ids));
              TYPE_AS_Id cnm (ids.Length() == 1 ? ASTAUX::MkId(L"*") : ids[1]);
              TYPE_AS_Id mnm (ids.Length() == 1 ? ids[1] : ids[2]);
              AddJavaPrimitiveBoolType(cnm, mnm);
            }
            else {
              forall = false;
            }
          }
          controll_found = forall;
        }
      }
      else if (nm == ASTAUX::MkName(L"JCGControl_double")) {
        const TYPE_AS_Expr & e (vd.GetRecord(pos_AS_ValueDef_val)); 
        if (e.Is(TAG_TYPE_AS_SetEnumerationExpr)) {
          const SEQ<TYPE_AS_Expr> & e_l (e.GetSequence(pos_AS_SetEnumerationExpr_els));
          bool forall = true;
          Set thrs_s;
          size_t len_e_l = e_l.Length();
          for (size_t i = 1; (i <= len_e_l) && forall; i++) {
            if (e_l[i].Is(TAG_TYPE_AS_TextLit)) {
              TYPE_AS_Name nm (ASTAUX::MkName(e_l[i].GetSequence(pos_AS_TextLit_val).GetString()));
              TYPE_AS_Ids ids (nm.GetSequence(pos_AS_Name_ids));
              TYPE_AS_Id cnm (ids.Length() == 1 ? ASTAUX::MkId(L"*") : ids[1]);
              TYPE_AS_Id mnm (ids.Length() == 1 ? ids[1] : ids[2]);
              AddJavaPrimitiveRealType(cnm, mnm);
            }
            else {
              forall = false;
            }
          }
          controll_found = forall;
        }
      }
    }
    if (!controll_found) {
      vals.ImpAppend(vd);
    }
  }
  return vals;
}

// Set vdmcg::GenIndExtension(Set file_s)
// {
//   Record super;
//   Generic g;
//   for (int cont=icl.First(g); cont; cont=icl.Next(g)) {
//     super = g;
//     Tuple tmp( icl[super] );
//     Set icl_s( tmp.GetField(1) );
//     Record cnm( tmp.GetField(2) );
//     Token fnm( GiveLastName(cnm) );
//     Token nm( fnm.GetValue() + L".h" );
//     Record file( FindFile(nm, file_s) );
//     file_s.RemElem(file);
//     Sequence cpp( file.GetField(3) );
//     cpp.ImpConc( GenIndClass(super,icl_s) );
//     file.SetField(3, cpp );
//     file_s.Insert(file);
//   }
//   return file_s;
// }

// Map vdmcg::MakeInheritMap(Sequence inherit_l)
// {
//   Map inh_m;
//   Record inh;
//   Generic g;
//   for (int cont=inherit_l.First(g); cont; cont=inherit_l.Next(g)) {
//     inh = g;
//     Record nm( inh.GetField(1) );
//     Sequence lbs( inh.GetField(2) );
//     if ( inh_m.DomExists(nm) ) {
//       Sequence rng( inh_m[nm] );
//       lbs.ImpConc(rng);
//     }
//     inh_m.ImpModify(nm,lbs);
//   }
//   return inh_m;
// }

//  Sequence vdmcg::GenMConstructor(Record fnm)
//  {
//    Record dstr(GenDest(fnm));
//    Record cst(GenFctDecl_BC(fnm, eseq));
//    Record destr(GenFctDecl_BC(dstr, eseq));
//    Record stmt(GenBlock(eseq));

//    Sequence res;
//    res.ImpAppend(GenFctDef(eseq,eseq, cst,nil, stmt));
//    Sequence vir_l;
//    vir_l.ImpAppend(Int(VIRTUAL_CPP));
//    res.ImpAppend(GenFctDef(eseq,vir_l, destr,nil,nil, stmt));

//    return res;
//  }

// Sequence vdmcg::GenIndexedInherit(Record nm, Sequence labels)
// {
//   Record super( vdm_BC_Rename(nm) );
//   Sequence base_l;
//   Set cnm_s;
//   Record label;
//   Generic g;
//   for (int cont=labels.First(g); cont; cont=labels.Next(g)) {
//     label = g;
//     Sequence super_l( GiveIndClassName(nm,label) );
//     for (int cont=super_l.First(g); cont; cont=super_l.Next(g)) {
//       base_l.ImpAppend( GenAccBaseSpec(g) );
//       cnm_s.Insert(g);
//     }
//   }
//   AccIndexedInheritance(super,cnm_s,nm);
//   return base_l;
// }

// void vdmcg::AccIndexedInheritance(Record id, Set cnm_s, Record bnm)
// {
//   if ( icl.DomExists(id) ) {
//     Tuple tmp( icl[id] );
//     Set cnm2_s( tmp.GetField(1) );
//     cnm2_s.ImpUnion(cnm_s);
//     tmp.SetField(1, cnm2_s).SetField(2, bnm);
//     icl.ImpModify(id,tmp);
//   }
//   else {
//     Tuple tmp(2);
//     tmp.SetField(1, cnm_s).SetField(2, bnm);
//     icl.Insert(id,tmp);
//   }
// }

// Sequence vdmcg::GenIndClass(Record super, Set super2_s)
// {
//   Sequence cppas, prot;
//   Record head, super2;
//   Generic g;
//   for (int cont=super2_s.First(g); cont; cont=super2_s.Next(g)) {
//     super2 = g;
//     Sequence bs_l;
//     bs_l.ImpAppend( GenAccBaseSpec(super) );
//     head = GenClassHead(super2,bs_l);
//     prot = GenProtected(GenMConstructor(super2));
//     Sequence cs_l;
//     cs_l.ImpAppend(vdm_BC_GenClassSpecifier(head,prot));
//     cppas.ImpAppend( GenIdentDeclaration(cs_l, nil) );
//   }
//   return cppas;
// }

