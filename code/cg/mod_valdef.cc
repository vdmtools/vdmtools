/***
*  * WHAT
*  *    VDM Code Generator.
*  *    mod_valdef.cc: Implementation of mod_valdef.vdm 1.55
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/mod_valdef.cc,v $
*  * VERSION
*  *    $Revision: 1.42 $
*  * DATE
*  *    $Date: 2001/12/17 09:44:35 $
*  * STATUS
*  *    Under development
*  * PROJECT
*  *    Toolbox
*  * AUTHOR
*  *    Poul Boegh Lassen (PBL) + $Author: paulm $ + jojo
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "vdmcg.h"
#include "astaux.h"
#include "contextinfo.h"
#include "BC.h"
#include "cgtag.h"
#include "statsem.h"
#include "intconvquotes.h"

//wcout << INT2Q::h2gAS(expr) << endl << flush;

// GenValDef
#ifdef VDMSL
// vd_l : seq of AS`ValueDef
// ==> (seq of (CPP`DeclSpecifier * CPP`Name)) * (seq of CPP`Stmt);
Tuple vdmcg::GenValDef (const SEQ<TYPE_AS_ValueDef> & vd_l)
#endif //VDMSL
#ifdef VDMPP
// classname : AS`Name
// vd_l : seq of AS`ValueDef
// ==> map AS`Access to seq of CPP`MemberDeclaration * seq of CPP`Stmt * seq of CPP`IdentDeclaration
Tuple vdmcg::GenValDef (const TYPE_AS_Name & classname, const SEQ<TYPE_AS_ValueDef> & vd_l)
#endif //VDMPP
{
  SEQ<TYPE_AS_Pattern> p_l;
  size_t len_vd_l = vd_l.Length();
  for (size_t idx = 1; idx <= len_vd_l; idx++) {
    p_l.ImpAppend (vd_l[idx].GetRecord(pos_AS_ValueDef_pat));
  }

  Map pid_m (SamePatternIds (p_l).GetField (2));

  TYPE_CPP_Expr succ_v (vdm_BC_GiveName (ASTAUX::MkId(L"succ")));
  TYPE_CPP_Stmt rti (RunTime(L"Pattern match did not succeed in value definition"));

#ifdef VDMSL
  Sequence decl (DeclValId (pid_m));
#endif // VDMSL
#ifdef VDMPP
  Map acs_pid_m (GenValDefAccessMap (vd_l));
  SEQ<TYPE_CPP_IdentDeclaration> decl;
  Map mem_decl;
  if (vdm_CPP_isCPP()) {
    Set dom_acs_pid_m (acs_pid_m.Dom());
    Generic acs;
    for (bool cc = dom_acs_pid_m.First (acs); cc; cc = dom_acs_pid_m.Next (acs)) {
      Tuple dvi (DeclValId (classname, acs_pid_m[acs]));
      mem_decl.Insert (acs, dvi.GetField (1));
    }
    Set s (acs_pid_m.Rng ());
    Map rng_m;
    Generic m;
    for (bool dd = s.First (m); dd; dd = s.Next (m)) {
      rng_m.ImpOverride (Map (m));
    }
    Tuple dvi (DeclValId(classname, rng_m));
    decl = dvi.GetField(2);
  }
  else {
    decl = DeclValJava(classname, pid_m, vd_l);  // for Java
  }
#endif //VDMPP

  bool need_succ(false);
  SEQ<TYPE_CPP_Stmt> inits;
  for (size_t index = 1; index <= len_vd_l; index++) {
    const TYPE_AS_ValueDef & vd (vd_l[index]);
#ifdef VDMSL
    const TYPE_AS_Pattern & pat (vd.GetRecord(pos_AS_ValueDef_pat));
    const TYPE_AS_Expr & val (vd.GetRecord(pos_AS_ValueDef_val));

    Map tmppid_m (FindPatternId(pat));
#endif // VDMSL
#ifdef VDMPP
    const TYPE_AS_Pattern & pat_q (vd.GetRecord(pos_AS_ValueDef_pat));
    const TYPE_AS_Expr & val_q (vd.GetRecord(pos_AS_ValueDef_val));

    Map tmppid_m (FindPatternId(pat_q));
    TYPE_AS_Pattern pat (vdm_CPP_isCPP() ? QuotePMExprWithClNm(pat_q, classname, Set())
                                         : QuotePMExprWithTemp(pat_q, classname, Set()));
    TYPE_AS_Expr val (vdm_CPP_isCPP() ? QuotePMExprWithClNm(val_q, classname, Set())
                                      : (Generic)val_q);
#endif //VDMPP

#ifdef VDMPP
    if (!(vdm_CPP_isJAVA().GetValue() && (tmppid_m.Dom()).SubSet(this->directdefinedValues)) ||
        (pat.Is(TAG_TYPE_AS_PatternName) && pat.GetField(pos_AS_PatternName_nm).IsNil()))
#endif //VDMPP
    {
      Generic valType (FindType (val));

      TYPE_CPP_Expr tmpVal_v (vdm_BC_GiveName (ASTAUX::MkId(L"tmpVal")));

      Tuple cgee (CGExprExcl(val, ASTAUX::MkId(L"tmpVal"), valType));
      const TYPE_CPP_Expr & expr (cgee.GetRecord(1));
      const SEQ<TYPE_CPP_Stmt> & stmts (cgee.GetSequence(2));

      TYPE_CGMAIN_VT vt (stmts.IsEmpty() ? mk_CG_VT(expr, valType) : mk_CG_VT(tmpVal_v, valType));

      Tuple cgpme (CGPatternMatchExcl(pat, vt, Set(), succ_v, Map(), Nil(), false));
      const SEQ<TYPE_CPP_Stmt> & pm (cgpme.GetSequence(1));
      bool Is_Excl (cgpme.GetBoolValue(2)); // false : need to check pattern match failed

      need_succ = (!Is_Excl) || need_succ;

      SEQ<TYPE_CPP_Stmt> pm1;
      pm1.ImpConc(pm);
      if (!Is_Excl) {
        pm1.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenNot(succ_v), vdm_BC_GenBlock(mk_sequence(rti)), nil));
      }

      SEQ<TYPE_CPP_Stmt> rb;
      if (!stmts.IsEmpty() ||
          (pat.Is(TAG_TYPE_AS_PatternName) && pat.GetField(pos_AS_PatternName_nm).IsNil())) {
        rb.ImpConc(stmts);
        rb.ImpConc(GenDeclInit_DS(valType, tmpVal_v, expr));
      }
      rb.ImpConc(pm1);

#ifdef VDMPP
      if (vdm_CPP_isJAVA()) {
        SEQ<TYPE_CPP_TypeSpecifier> res;
        res.ImpAppend(vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Throwable"))));
        TYPE_CPP_ExceptionDeclaration expdecl (
                      vdm_BC_GenExceptionDeclaration(res, vdm_BC_GenIdentifier(ASTAUX::MkId(L"e"))));
        TYPE_CPP_Expr fctcall1(vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"e.getMessage")),
                                                 SEQ<TYPE_CPP_Expr>()));
        TYPE_CPP_Expr fctcall2(vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"System.out.println")),
                                                 SEQ<TYPE_CPP_Expr>().ImpAppend(fctcall1)));
        TYPE_CPP_Stmt stmt (vdm_BC_GenExpressionStmt(fctcall2));

        SEQ<TYPE_CPP_Handler> handlers;
        handlers.ImpAppend(vdm_BC_GenHandler(expdecl,stmt));

        inits.ImpAppend(vdm_BC_GenTryBlock(rb, handlers, nil));
        inits.ImpConc(GenAssignTemp(tmppid_m.Dom()));
      }
      else
#endif // VDMPP
      { // C++
        if (rb.Length() <= 1) {
          inits.ImpConc(rb);
        }
        else {
          inits.ImpAppend (vdm_BC_GenBlock (rb));
        }
      }

#ifdef VDMPP
      if (valType.Is(TAG_TYPE_REP_CompositeTypeRep)) {
        TYPE_AS_Name nm (Record(valType).GetRecord(pos_REP_CompositeTypeRep_nm));
        TYPE_AS_Ids ids (nm.get_ids());
        if (ids.Length() > 1) {
          if (ids.Hd() != GiveCurCName()) {
            AddInclH(GiveFirstName(nm));
          }
        }
      }
#endif // VDMPP
    }
  } // end of for loop

  if (need_succ) {
    inits.ImpPrepend(vdm_BC_GenDecl(GenSmallBoolType(), succ_v, vdm_BC_GenAsgnInit(vdm_BC_GenBoolLit(false))));
  }

#ifdef VDMSL
  return mk_(decl, inits);
#endif // VDMSL
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    SEQ<TYPE_CPP_Stmt> tempinits;
    size_t len_decltemp = this->decltemp.Length();
    for (size_t idx = 1; idx <= len_decltemp; idx++) {
      tempinits.ImpAppend(vdm_BC_GenDeclStmt(this->decltemp[idx]));
    }

    tempinits.ImpConc(inits);
    inits = tempinits;
  }
  return mk_(mem_decl, inits, decl);
#endif //VDMPP
}

// DeclValId
#ifdef VDMSL
// pid_m : map AS`Name to set of REP`TypeRep
// ==> seq of (CPP`DeclSpecifier * CPP`Name )
Sequence vdmcg::DeclValId (const Map & pid_m)
#endif // VDMSL
#ifdef VDMPP
// classname : AS`Name
// pid_m : map AS`Name to set of REP`TypeRep
// ==> seq of CPP`MemberDeclaration * seq of CPP`IdentDeclaration
Tuple vdmcg::DeclValId(const TYPE_AS_Name & classname, const Map & pid_m)
#endif //VDMPP
{
  SEQ<TYPE_CPP_Stmt> rb;  // seq of CPP`Stmt

  Set dom_pid_m (pid_m.Dom());
  Generic g;
  // pid_m : map AS`Name to set of REP`TypeRep
  for (bool bb = dom_pid_m.First(g); bb; bb = dom_pid_m.Next(g)) {
    TYPE_REP_TypeRep type;
    SET<TYPE_REP_TypeRep> tp_s (pid_m[g]);      // set of REP`TypeRep
    if (tp_s.Card () == 1) {
      type = tp_s.GetElem();
    }
    else {
      TYPE_REP_UnionTypeRep utr;
      utr.set_tps(tp_s);
      type = utr;
    }

    rb.ImpConc (GenValDecl (type, g)); // seq of CPP`Stmt

    if (PossibleFnType(type)) {
      InsertLocFct(g);
    }
  }
#ifdef VDMSL
  Sequence res;
#endif // VDMSL
#ifdef VDMPP
  SEQ<TYPE_CPP_MemberDeclaration> mem_res;
  SEQ<TYPE_CPP_IdentDeclaration> def_res;
#endif //VDMPP

  // rb : seq of CPP`Stmt
  size_t len_rb = rb.Length();
  for (size_t idx = 1; idx <= len_rb; idx++) {
    const TYPE_CPP_DeclarationStmt & dstmt (rb[idx]);  // CPP`DeclarationStmt
    const TYPE_CPP_IdentDeclaration & idecl (dstmt.GetRecord(pos_CPP_DeclarationStmt_decl)); // CPP`IdentDeclaration
    const SEQ<TYPE_CPP_DeclSpecifier> & ds (idecl.GetSequence(pos_CPP_IdentDeclaration_ds));  // seq of DeclSpecifier
    const SEQ<TYPE_CPP_InitDeclarator> & nms (idecl.GetSequence(pos_CPP_IdentDeclaration_dl)); // CPP`DeclaratorList =
                                                       // seq of InitDeclarator
    TYPE_CPP_Declarator name (nms[1].GetRecord(pos_CPP_InitDeclarator_decl));

#ifdef VDMSL
    res.ImpAppend (mk_(ds.Hd(), name));
#endif // VDMSL
#ifdef VDMPP
    mem_res.ImpAppend(vdm_BC_GenStaticMemberSpec(ds, name));
    TYPE_CPP_QualifiedName quotedname (vdm_BC_GenQualifiedName(vdm_BC_Rename(classname), name));

    if (vdm_CPP_isCPP()) {
      def_res.ImpAppend(vdm_BC_GenIdentDecl(SEQ<TYPE_CPP_Annotation>(), ds, quotedname, Nil()));
    }
    else { // for Java
      TYPE_CPP_Modifier pub (vdm_BC_GenModifier(quote_PUBLIC));
      TYPE_CPP_Modifier sta (vdm_BC_GenModifier(quote_STATIC));
      SEQ<TYPE_CPP_DeclSpecifier> ds_q (ds);
      def_res.ImpAppend(vdm_BC_GenIdentDecl(SEQ<TYPE_CPP_Annotation>(),
                                            ds_q.ImpPrepend(sta).ImpPrepend(pub), name, Nil()));
    }
#endif // VDMPP
  }

#ifdef VDMSL
  return res;
#endif // VDMSL
#ifdef VDMPP
  return mk_(mem_res, def_res);
#endif //VDMPP
}

#ifdef VDMPP
// AreDefinedDirectly
// vd_l :seq of AS`ValueDef
// ==> bool
bool vdmcg::AreDefinedDirectly(const SEQ<TYPE_AS_ValueDef> & vd_l)
{
  SEQ<TYPE_AS_Pattern> p_l;
  size_t len_vd_l = vd_l.Length();
  for (size_t idx = 1; idx <= len_vd_l; idx++) {
    p_l.ImpAppend(vd_l[idx].GetRecord(pos_AS_ValueDef_pat));
  }

  Tuple t (SamePatternIds(p_l)); // bool * map AS`Name to set of REP`TypeRep
  const Map & pid_m (t.GetMap(2));     // map AS`Name to set of REP`TypeRep

  bool forall = true;
  for (size_t idx2 = 1; (idx2 <= len_vd_l) && forall; idx2++) {
    forall = IsDefinedDirectly(vd_l[idx2], pid_m);
  }
  return forall;
}

// IsDefinedDirectly
// vdef : AS`ValueDef
// pid_m : map AS`Name to set of REP`TypeRep
// ==> bool
bool vdmcg::IsDefinedDirectly(const TYPE_AS_ValueDef & vdef, const Map & pid_m)
{
  const TYPE_AS_Pattern & pat (vdef.GetRecord(pos_AS_ValueDef_pat));
  const TYPE_AS_Expr & val (vdef.GetRecord(pos_AS_ValueDef_val));

  Map tmp_m (FindPatternId(pat));
  if (tmp_m.Size() == 1) {
    //Generic tmp, tmp2;
    //tmp_m.First(tmp, tmp2);
    Generic tmp (tmp_m.Dom().GetElem());
    SetException(false);
    InitVariables();
    Generic pp_v (CGExpr_q(val, mk_CG_VT(vdm_BC_Rename(tmp), FindType(val))));
    bool res = IsSeqOfCPPStmt(pp_v);
    res = res || ThrowsException();
    res = res || VariablesinValDef().ImpIntersect(pid_m.Dom()).Card() > 0;
    return !res;
  }
  else {
    return false;
  }
}

// DeclValJava
// classname AS`Name
// pid_m : map AS`Name to set of REP`TypeRep
// vd_l : seq of AS`ValueDef
//// ==> map AS`Access to seq of CPP`MemberDeclaration *
////     seq of CPP`IdentDeclaration
// ==> seq of CPP`IdentDeclaration
SEQ<TYPE_CPP_IdentDeclaration> vdmcg::DeclValJava(const TYPE_AS_Name & classname,
                                                  const Map & pid_m,
                                                  const SEQ<TYPE_AS_ValueDef> & vd_l)
{
  Map rb = Map();        // map int to (AS`Access * seq of CPP`Stmt)
  Map rbdirect = Map();  // map int to (AS`Access * seq of CPP`Stmt)
  Map rbtemp = Map();    // map int to seq of CPP`Stmt
  Map cidtocpp = Map();  // map int to seq of CPP`IdentDeclaration
  Map cidtocpp2 = Map(); // map int to seq of CPP`IdentDeclaration

  Generic vd_g;
  this->directdefinedValues = SET<TYPE_AS_Name>();
  // vd_l : seq of AS`ValueDef
  size_t len_vd_l = vd_l.Length();
  for (size_t idx = 1; idx <= len_vd_l; idx++) {
    const TYPE_AS_ValueDef & vdef (vd_l[idx]);
    const TYPE_AS_Pattern & pat (vdef.GetRecord(pos_AS_ValueDef_pat));
    const TYPE_AS_Expr & val (vdef.GetRecord(pos_AS_ValueDef_val));
    const Int & acc (vdef.GetInt(pos_AS_ValueDef_access));
    const TYPE_CI_ContextId & cid (vdef.GetInt(pos_AS_ValueDef_cid));

    Map tmp_m (FindPatternId(pat)); // map AS`Name to set of REP`TypeRep
    if ((tmp_m.Dom()).Card() == 1) {
      SET<TYPE_AS_Name> dom_tmp_m (tmp_m.Dom());
      Generic tmp;
      // tmp_m : map AS`Name to set of REP`TypeRep
      for (bool cc = dom_tmp_m.First(tmp); cc; cc = dom_tmp_m.Next(tmp)) {
        TYPE_REP_TypeRep type;
        SET<TYPE_REP_TypeRep> tp_s (tmp_m[tmp]);
        if (tp_s.Card () == 1)
          type = tp_s.GetElem();
        else {
          TYPE_REP_UnionTypeRep utr;
          utr.set_tps(tp_s);
          type = utr;
        }

        if (!IsDefinedDirectly(vdef, pid_m)) {
          rb.Insert(cid, mk_(acc, GenValDecl(type, tmp)));
          TYPE_AS_Id newid (ASTAUX::GetFirstId(tmp).ImpConc(ASTAUX::MkId(L"temp")));
          TYPE_AS_Name tmpnm (tmp);
          tmpnm.set_ids(TYPE_AS_Ids().ImpAppend(newid));
          rbtemp.Insert(cid, GenDecl_DS(type,vdm_BC_Rename(tmpnm), nil));
        }
        else {
          Generic pp_v (CGExpr_q(val, mk_CG_VT(vdm_BC_Rename(tmp), FindType(val))));
          TYPE_CPP_Expr cast;
          if (val.Is(TAG_TYPE_AS_NilLit)) {
            cast = GenNilLit();
          }
          else {
            cast = GenExplicitCast(type,pp_v,FindType(val));
          }
          rbdirect.Insert(cid, mk_(acc, GenDecl_DS(type, vdm_BC_Rename(tmp), vdm_BC_GenAsgnInit(cast))));
          this->directdefinedValues.ImpUnion(Set().Insert(tmp));
        }

        if (PossibleFnType(type)) {
          InsertLocFct(tmp);
        }
      }
    }
    else {
      SET<TYPE_AS_Name> dom_tmp_m (tmp_m.Dom());
      Generic tmp;
      // tmp_m : map AS`Name to set of REP`TypeRep
      for (bool dd = dom_tmp_m.First(tmp); dd; dd = dom_tmp_m.Next(tmp)) {
        TYPE_REP_TypeRep type;
        SET<TYPE_REP_TypeRep> tp_s (tmp_m[tmp]);
        if (tp_s.Card () == 1) {
          type = tp_s.GetElem();
        }
        else {
          TYPE_REP_UnionTypeRep utr;
          utr.set_tps(tp_s);
          type = utr;
        }
        if (rb.DomExists(cid)) {
          Tuple t (rb[cid]);
          Sequence rn (t.GetField(2));
          rb.ImpModify(cid, mk_(acc, rn.ImpConc(GenValDecl(type, tmp))));
        }
        else {
          rb.Insert(cid, mk_(acc, GenValDecl(type, tmp)));
        }
        TYPE_AS_Id newid (ASTAUX::GetFirstId(tmp).ImpConc(ASTAUX::MkId(L"temp")));
        TYPE_AS_Name tmpnm (tmp);
        tmpnm.set_ids(TYPE_AS_Ids().ImpAppend(newid));
        
        if (rbtemp.DomExists(cid)) {
          Sequence rn (rbtemp[cid]);
          rbtemp.ImpModify(cid, rn.ImpConc(GenDecl_DS(type, vdm_BC_Rename(tmpnm), nil)));
        }
        else {
          rbtemp.Insert(cid, Sequence().ImpConc(GenDecl_DS(type, vdm_BC_Rename(tmpnm), nil)));
        }

        if (PossibleFnType(type)) {
          InsertLocFct(tmp);
        }
      }
    }
  }

  SET<Int> dom_rb (rb.Dom());
  Generic cid;
  // rb : map int to (AS`Access * seq of CPP`Stmt)
  for (bool ee = dom_rb.First(cid); ee; ee = dom_rb.Next(cid)) {
    Tuple t (rb[cid]);
    const Int & acc (t.GetInt(1));           // AS`Access
    SEQ<TYPE_CPP_Stmt> stmt_l (t.GetSequence(2));   // seq of CPP`Stmt
    while (!stmt_l.IsEmpty()) {
      const TYPE_CPP_DeclarationStmt & dstmt (stmt_l[1]);
      const TYPE_CPP_IdentDeclaration & idecl (dstmt.GetRecord(pos_CPP_DeclarationStmt_decl));
      SEQ<TYPE_CPP_DeclSpecifier> ds (idecl.GetSequence(pos_CPP_IdentDeclaration_ds));
      const SEQ<TYPE_CPP_Annotation> & annos (idecl.GetSequence(pos_CPP_IdentDeclaration_annos));
      const SEQ<TYPE_CPP_InitDeclarator> & nms (idecl.GetSequence(pos_CPP_IdentDeclaration_dl));
      TYPE_CPP_Declarator name (nms[1].GetRecord(pos_CPP_InitDeclarator_decl));
      TYPE_CPP_Modifier sta (vdm_BC_GenModifier(quote_STATIC));
      TYPE_CPP_Modifier fin (vdm_BC_GenModifier(quote_FINAL));
      ds.ImpPrepend(fin).ImpPrepend(sta).ImpPrepend(JavaAccess(acc));
      if (cidtocpp.DomExists(cid)) {
        SEQ<TYPE_CPP_IdentDeclaration> rn (cidtocpp[cid]);
        cidtocpp.ImpModify(cid, rn.ImpAppend(vdm_BC_GenIdentDecl(annos, ds, name, Nil())));
      }
      else {
        cidtocpp.Insert(cid,Sequence().ImpAppend(vdm_BC_GenIdentDecl(annos, ds, name, Nil())));
      }

      stmt_l = stmt_l.Tl();
    }
  }

  // rbdirect : map int to (AS`Access * seq of CPP`Stmt)
 
  SET<Int> dom_rbdirect (rbdirect.Dom());
  for (bool ff = dom_rbdirect.First(cid); ff; ff = dom_rbdirect.Next(cid)) {
    Tuple t (rbdirect[cid]);                      // (AS`Access * seq of CPP`Stmt)
    const Int & acc (t.GetInt(1));          // AS`Access
    SEQ<TYPE_CPP_Stmt> stmt_l (t.GetField(2));  // seq of CPP`Stmt
    TYPE_CPP_DeclarationStmt dstmt (stmt_l.Hd()); // CPP`DeclarationStmt
    TYPE_CPP_IdentDeclaration idecl (dstmt.get_decl()); // CPP`IdentDeclaration
    SEQ<TYPE_CPP_DeclSpecifier> ds (idecl.get_ds()); // seq of DeclSpecifier
    SEQ<TYPE_CPP_InitDeclarator> nms (idecl.get_dl()); // CPP`DeclaratorList =
                                      // seq of InitDeclarator
    const SEQ<TYPE_CPP_Annotation> & annos (idecl.GetSequence(pos_CPP_IdentDeclaration_annos));
    TYPE_CPP_InitDeclarator idec (nms.Hd());
    TYPE_CPP_Declarator name (idec.get_decl()); // CPP`Declarator
    Generic initExpr (idec.get_i());  // [ CPP`Initializer ]
    TYPE_CPP_Modifier sta (vdm_BC_GenModifier(quote_STATIC));
    TYPE_CPP_Modifier fin (vdm_BC_GenModifier(quote_FINAL));
    ds.ImpPrepend(fin).ImpPrepend(sta).ImpPrepend(JavaAccess(acc));
    cidtocpp.Insert(cid,vdm_BC_GenIdentDecl(annos, ds, name, initExpr));
  }

  this->decltemp = Sequence();

  // rbtemp : map int to seq of CPP`Stmt
  SET<Int> dom_rbtemp (rbtemp.Dom());
  for (bool gg = dom_rbtemp.First(cid); gg; gg = dom_rbtemp.Next(cid)) {
    SEQ<TYPE_CPP_Stmt> stmt_l (rbtemp[cid]);                  // seq of CPP`Stmt
    while (!stmt_l.IsEmpty()) {
      TYPE_CPP_DeclarationStmt dstmt (stmt_l.Hd()); // CPP`DeclarationStmt
      TYPE_CPP_IdentDeclaration idecl (dstmt.get_decl());
      if (cidtocpp2.DomExists(cid)) {
        SEQ<TYPE_CPP_IdentDeclaration> rn (cidtocpp2[cid]);
        cidtocpp2.ImpModify(cid,rn.ImpAppend(idecl));
      }
      else {
        cidtocpp2.Insert(cid,Sequence().ImpAppend(idecl));
      }

      stmt_l = stmt_l.Tl();
    }
  }

  Sequence def_res;    // seq of CPP`IdentDeclaration
  // cidtocpp : map int to seq of CPP`IdentDeclaration
  while (!cidtocpp.Dom().IsEmpty()) {
    SET<TYPE_CI_ContextId> cidtocppdom (cidtocpp.Dom());
    TYPE_CI_ContextId cid (cidtocppdom.GetElem());
    Generic gg;
    for (bool hh = cidtocppdom.First (gg); hh; hh = cidtocppdom.Next (gg)) {
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
    def_res.ImpAppend(cidtocpp[cid]);
    cidtocpp.RemElem(cid);
  }
  // cidtocpp2 : map int to seq of CPP`IdentDeclaration
  while (!cidtocpp2.Dom().IsEmpty()) {
    SET<TYPE_CI_ContextId> cidtocpp2dom (cidtocpp2.Dom());
    TYPE_CI_ContextId cid (cidtocpp2dom.GetElem());
    Generic gg;
    for (bool ii = cidtocpp2dom.First (gg); ii; ii = cidtocpp2dom.Next (gg)) {
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
    this->decltemp.ImpConc(cidtocpp2[cid]);
    cidtocpp2.RemElem(cid);
  }
  return def_res;
}

// GenValDefAccessMap
// valdef_l : seq of AS`ValueDef
// ==> map AS`Access to (map AS`Name to set of REP`TypeRep)
Map vdmcg::GenValDefAccessMap (const SEQ<TYPE_AS_ValueDef> & valdef_l)
{
  SEQ<TYPE_AS_ValueDef> vd_l;
  size_t len_valdef_l = valdef_l.Length();
  for (size_t i = 1; i <= len_valdef_l; i++) {
    vd_l.ImpAppend (ExchangeDefAccess (valdef_l[i]));
  }

  SET<TYPE_AS_Access> acs_s;
  size_t len_vd_l = vd_l.Length();
  for (size_t j = 1; j <= len_vd_l; j++) {
    acs_s.Insert (vd_l[j].GetField(pos_AS_ValueDef_access));
  }

  Map acs_pid_m; // map AS`Access to (map AS`Name to set of REP`TypeRep)
  Generic a;
  for (bool bb = acs_s.First (a); bb; bb = acs_s.Next (a)) {
    Set m_s; // set of map AS`Name to set of REP`TypeRep
    for (size_t k = 1; k <= len_vd_l; k++) {
      const TYPE_AS_ValueDef & vd (vd_l[k]);
      if (vd.GetField(pos_AS_ValueDef_access) == a) {
        m_s.Insert (FindPatternId (vd.GetRecord(pos_AS_ValueDef_pat)));
      }
    }
    Map m_m; // map AS`Name to set of REP`TypeRep
    Generic m;
    for (bool ff = m_s.First (m); ff; ff = m_s.Next (m)) {
      m_m.ImpOverride (m);
    }
    acs_pid_m.Insert (a, m_m);
  }
  return acs_pid_m;
}

// GenAssignTemp
// nms : set of AS`Name
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenAssignTemp(const SET<TYPE_AS_Name> & nms)
{
  SET<TYPE_AS_Name> nms_q (nms);
  SEQ<TYPE_CPP_Stmt> stmts;
  Generic nm;
  for (bool bb = nms_q.First(nm); bb; bb = nms_q.Next(nm)) {
    if (!this->directdefinedValues.InSet(nm)) {
      TYPE_CPP_Identifier newNm (vdm_BC_Rename(nm));
      stmts.ImpAppend(vdm_BC_GenAsgnStmt(newNm, vdm_BC_AppendName(newNm, ASTAUX::MkId(L"temp"))));
    }
  }
  return stmts;
}
#endif // VDMPP

// GenLocalValDef
// vd_l : seq of AS`ValueDef
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenLocalValDef(const SEQ<TYPE_AS_ValueDef> & vd_l)
{
  TYPE_CPP_Identifier succ (vdm_BC_GiveName(ASTAUX::MkId(L"succ")));
  TYPE_CPP_Stmt rti (RunTime(L"Pattern match did not succeed in value definition"));

  SEQ<TYPE_CPP_Stmt> rb_l;
  Set local_s;
  bool declSuc = false;
  size_t len_vd_l = vd_l.Length();
  for (size_t idx = 1; idx <= len_vd_l; idx++) {
    const TYPE_AS_ValueDef & vd (vd_l[idx]);
    const TYPE_AS_Pattern & pat (vd.GetRecord(pos_AS_ValueDef_pat));
//    const Generic & pattp (vd.GetField(pos_AS_ValueDef_tp));
    const TYPE_AS_Expr & val (vd.GetRecord(pos_AS_ValueDef_val));

    TYPE_REP_TypeRep vtp (FindType(val));

    //if ( pat.Is(TAG_TYPE_AS_PatternName) && !pat.GetField(pos_AS_PatternName_nm).IsNil() && InsertLoc( pat ))
    if ( pat.Is(TAG_TYPE_AS_PatternName) && !pat.GetField(pos_AS_PatternName_nm).IsNil()) {
      // let x = ... (the name of x is't already defined
      const TYPE_AS_Name & nm (pat.GetRecord(pos_AS_PatternName_nm) );
      Generic ptp (FindType(pat));

      if (FindScope(nm) >= 0) {
        // let x = ... (the name of x is already defined
        Tuple cgee (CGExprExcl(val, ASTAUX::MkId(L"tmpVal"), vtp));
        const TYPE_CPP_Expr & pat_v (cgee.GetRecord(1));
        const SEQ<TYPE_CPP_Stmt> & pat_stmt (cgee.GetSequence(2));

        TYPE_CPP_Name nm_v (vdm_BC_Rename(InsertName_CGAUX(nm)));

        rb_l.ImpConc(pat_stmt);
        rb_l.ImpConc(GenConstDeclInit(ptp, nm_v, pat_v));
      }
      else {
        // let x = ... (the name of x is't already defined
        TYPE_CPP_Identifier id (vdm_BC_Rename(InsertName_CGAUX(nm)));

        Tuple cgee (CGExprExcl(val, id.GetSequence(pos_CPP_Identifier_id), ptp));
        const TYPE_CPP_Expr & pat_v (cgee.GetRecord(1));
        const SEQ<TYPE_CPP_Stmt> & pat_stmt (cgee.GetSequence(2));

        rb_l.ImpConc(pat_stmt);
        TYPE_CGMAIN_VT vt (mk_CG_VT(pat_v, vtp));
        SEQ<TYPE_AS_Stmt> decls (DeclarePatVars(pat)); // must be before CGPatternMatchExcl
        Tuple cgpme (CGPatternMatchExcl(pat, vt, Set(), succ, Map(), Nil(), false));
        const SEQ<TYPE_CPP_Stmt> & pm (cgpme.GetSequence(1));
        bool Is_Excl (cgpme.GetBoolValue(2)); // false : need to check pattern match failed
        rb_l.ImpConc(MergeStmts( decls, pm )); // experimental
      }
      local_s.Insert(nm);
      if (PossibleFnType(ptp)) {
        InsertLocFct(nm);
      }
    }
    else if (pat.Is(TAG_TYPE_AS_PatternName) && pat.GetField(pos_AS_PatternName_nm).IsNil()) {
      Tuple cgee (CGExprExcl(val, ASTAUX::MkId(L"tmpVal"), nil));
      const TYPE_CPP_Expr & expr (cgee.GetRecord(1));
      const SEQ<TYPE_CPP_Stmt> & stmts (cgee.GetSequence(2));

      switch (val.GetTag()) {
#ifdef VDMPP
        case TAG_TYPE_AS_ApplyExpr: {
          rb_l.ImpConc(stmts);
          if (vdm_CPP_isCPP()) {
            rb_l.ImpAppend(vdm_BC_GenExpressionStmt(expr));
          }
          else {
            TYPE_CPP_Expr e (StripBracketedAndCastExpr(expr));
            rb_l.ImpAppend(vdm_BC_GenExpressionStmt(e));
          }
          break;
        }
#endif //VDMPP
        case TAG_TYPE_AS_BoolLit:
        case TAG_TYPE_AS_NilLit:
        case TAG_TYPE_AS_RealLit:
        case TAG_TYPE_AS_NumLit:
        case TAG_TYPE_AS_CharLit:
        case TAG_TYPE_AS_TextLit:
        case TAG_TYPE_AS_QuoteLit:
        case TAG_TYPE_AS_Name: {
          break;
        }
        default: {
          TYPE_CPP_Name tmpVal (vdm_BC_GiveName (ASTAUX::MkId(L"tmpVal")));
          rb_l.ImpConc(stmts);
          rb_l.ImpConc(GenConstDeclInit(vtp, tmpVal, expr));
          break;
        }
      }
    }
    else {
      Tuple cgee (CGExprExcl(val, ASTAUX::MkId(L"tmpVal"), vtp));
      const TYPE_CPP_Expr & expr (cgee.GetRecord(1));
      const SEQ<TYPE_CPP_Stmt> & stmts (cgee.GetSequence(2));

      TYPE_CGMAIN_VT vt (mk_CG_VT(expr, vtp));

      SEQ<TYPE_AS_Stmt> decls (DeclarePatVars(pat)); // must be before CGPatternMatchExcl

      Tuple cgpme (CGPatternMatchExcl(pat, vt, Set(), succ, Map(), Nil(), false));
      const SEQ<TYPE_CPP_Stmt> & pm (cgpme.GetSequence(1));
      bool Is_Excl (cgpme.GetBoolValue(2)); // false : need to check pattern match failed

      SEQ<TYPE_CPP_Stmt> pm1;
      pm1.ImpConc(pm);

      if (!Is_Excl) {
        pm1.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenNot(succ), vdm_BC_GenBlock(mk_sequence(rti)), nil));
      }

      rb_l.ImpConc(stmts);
      rb_l.ImpConc(MergeStmts( decls, pm1 )); // experimental

      declSuc = (declSuc || (!Is_Excl));
    }
  }
  if (declSuc) {
    rb_l.ImpPrepend(vdm_BC_GenDecl(GenSmallBoolType(), succ, vdm_BC_GenAsgnInit(vdm_BC_GenBoolLit(false))));
  }

  PushLoc( local_s );
  return rb_l;
}

// InsertLoc
// pn : AS`PatternName
// ==> bool
bool vdmcg::InsertLoc( const TYPE_AS_PatternName & pn )
{
  TYPE_AS_Expr n (pn.get_nm());
  if ( this->loc_s.InSet( n ) ) {
    return false;
  }
  else {
    this->loc_s.Insert( n );
    return true;
  }
}

// PushLoc
// n_s : set of (AS`Name | AS`OldName)
// ==> ()
void vdmcg::PushLoc( const Set & n_s )
{
  this->sl_l.ImpPrepend( n_s );
}

// DeleteLoc
// ==> ()
void vdmcg::DeleteLoc()
{
  if (!this->sl_l.IsEmpty()) {
    this->loc_s.ImpDiff( this->sl_l.Hd() );
  }
  this->sl_l.ImpTl();
}

// PushFctLoc
// n_s : set of (AS`Name | AS`OldName)
// ==> ()
void vdmcg::PushFctLoc( const Set & n_s )
{
  this->loc_s.ImpUnion( n_s );
  PushLoc( n_s );
}
