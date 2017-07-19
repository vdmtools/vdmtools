/***
*  * WHAT
*  *    mod_typedef.cc: Implementation of mod_typedef.vdm r1.34
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/mod_typedef.cc,v $
*  * VERSION
*  *    $Revision: 1.27 $
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
#include "asquotes.h"
#include "contextinfo.h"
#include "BC.h"

#ifdef VDMSL
// GenInvEqOrdDecl
// td_l : seq of AS`TypeDef
// ==> CPP`CPPAS
TYPE_CPP_CPPAS vdmcg::GenInvEqOrdDecl(const SEQ<TYPE_AS_TypeDef> & td_l)
#endif //VDMSL
#ifdef VDMPP
// GenInvEqOrdDecl
// cnm : CPP`Name
// td_l : seq of AS`TypeDef
// ==> seq of CPP`MemberDeclaration
SEQ<TYPE_CPP_MemberDeclaration> vdmcg::GenInvEqOrdDecl(const TYPE_CPP_Name & cnm,
                                                  const SEQ<TYPE_AS_TypeDef> & td_l)
#endif //VDMPP
{
#ifdef VDMSL
  TYPE_CPP_CPPAS cpp;
#endif // VDMSL
#ifdef VDMPP
  SEQ<TYPE_CPP_MemberDeclaration> cpp;
#endif // VDMPP

  size_t len_td_l = td_l.Length();
  for (size_t idx = 1; idx <= len_td_l; idx++) {
    const TYPE_AS_TypeDef & td (td_l[idx]);
    const Generic & Inv (td.GetField(pos_AS_TypeDef_Inv));
    const Generic & Eq (td.GetField(pos_AS_TypeDef_Eq));
    const Generic & Ord (td.GetField(pos_AS_TypeDef_Ord));
    if (!Inv.IsNil() || !Eq.IsNil() || !Ord.IsNil()) {

      SEQ<TYPE_CPP_DeclSpecifier> ds_l;
#ifdef VDMPP
      ds_l.ImpAppend(vdm_BC_GenStorageClassSpecifier(quote_STATIC));
#endif //VDMPP
      ds_l.ImpAppend(GenBoolType());

      TYPE_CPP_TypeSpecifier argtp (Rep2CppTypeSpecifier(FromAS2RepType(td.get_shape()), td.get_shape()));
      TYPE_CPP_RefAbsDecl ref (vdm_BC_GenRefAbsDecl(Nil(), Nil()));
      
      SEQ<TYPE_CPP_DeclSpecifier> decls;
      decls.ImpAppend(vdm_BC_GenTypeSpecifier(quote_CONST));
      decls.ImpAppend(argtp);

      type_dL arg_dcl;
      arg_dcl.ImpAppend(vdm_BC_GenArgAbs(decls, ref));

      type_dL arg2_dcl;
      arg2_dcl.ImpAppend(vdm_BC_GenArgAbs(decls, ref));
      arg2_dcl.ImpAppend(vdm_BC_GenArgAbs(decls, ref));

      if (!Inv.IsNil()) {
        TYPE_CPP_Name fnm (vdm_BC_GivePrePostNm(td.GetRecord(pos_AS_TypeDef_nm), ASTAUX::MkId(L"inv")));
        TYPE_CPP_FctDecl fndecl (vdm_BC_GenFctDecl(fnm, arg_dcl));

#ifdef VDMSL
        cpp.ImpAppend(vdm_BC_GenIdentDecl(SEQ<TYPE_CPP_Annotation>(), ds_l, fndecl, Nil()));
#endif //VDMSL
#ifdef VDMPP
        cpp.ImpAppend(vdm_BC_GenMemberSpec(ds_l, fndecl));
#endif //VDMPP
      }
      if (!Eq.IsNil()) {
        TYPE_CPP_Name fnm (vdm_BC_GivePrePostNm(td.GetRecord(pos_AS_TypeDef_nm), ASTAUX::MkId(L"eq")));
        TYPE_CPP_FctDecl fndecl (vdm_BC_GenFctDecl(fnm, arg2_dcl));

#ifdef VDMSL
        cpp.ImpAppend(vdm_BC_GenIdentDecl(SEQ<TYPE_CPP_Annotation>(), ds_l, fndecl, Nil()));
#endif //VDMSL
#ifdef VDMPP
        cpp.ImpAppend(vdm_BC_GenMemberSpec(ds_l, fndecl));
#endif //VDMPP
      }
      if (!Ord.IsNil()) {
        TYPE_CPP_Name fnm (vdm_BC_GivePrePostNm(td.GetRecord(pos_AS_TypeDef_nm), ASTAUX::MkId(L"ord")));
        TYPE_CPP_FctDecl fndecl (vdm_BC_GenFctDecl(fnm, arg2_dcl));

#ifdef VDMSL
        cpp.ImpAppend(vdm_BC_GenIdentDecl(SEQ<TYPE_CPP_Annotation>(), ds_l, fndecl, Nil()));
#endif //VDMSL
#ifdef VDMPP
        cpp.ImpAppend(vdm_BC_GenMemberSpec(ds_l, fndecl));
#endif //VDMPP
      }
    }
  }
  return cpp;
}

// GenInvEqOrdDef
// td_l : seq of AS`TypeDef
// ==> CPP`CPPAS | seq of CPP`MemberDeclaration
Sequence vdmcg::GenInvEqOrdDef(const SEQ<TYPE_AS_TypeDef> & td_l)
{
  TYPE_AS_BooleanType btp;
  btp.Init(NilContextId);

  Sequence cpp;
  Map cidtocpp; // map int to seq of CPP`Declaration
  size_t len_td_l = td_l.Length();
  for (size_t idx = 1; idx <= len_td_l; idx++) {
    const TYPE_AS_TypeDef & td (td_l[idx]);
    const Generic & Inv (td.GetField(pos_AS_TypeDef_Inv));
    const Generic & Eq (td.GetField(pos_AS_TypeDef_Eq));
    const Generic & Ord (td.GetField(pos_AS_TypeDef_Ord));
    if (! Inv.IsNil()) {
      TYPE_AS_Invariant inv (Inv);
      const TYPE_AS_Name & tnm (td.GetRecord(pos_AS_TypeDef_nm));
      const Generic & acc (td.GetField(pos_AS_TypeDef_access));

      TYPE_AS_Name fnm (vdm_BC_GivePrePostName(tnm, ASTAUX::MkId(L"inv")));

      SEQ<TYPE_AS_Type> shape_l;
      shape_l.ImpAppend(td.GetRecord(pos_AS_TypeDef_shape));
      TYPE_AS_TotalFnType ftp;
      ftp.Init(shape_l, btp, NilContextId);

      TYPE_AS_Expr expr (inv.GetRecord(pos_AS_Invariant_expr));
      TYPE_AS_FnBody fnbd;
      fnbd.Init(expr, NilContextId);

      SEQ<TYPE_AS_Pattern> pat_l;
      pat_l.ImpAppend(inv.GetRecord(pos_AS_Invariant_pat));

      SEQ<TYPE_AS_Parameters> pat_ll;
      pat_ll.ImpAppend(pat_l);

      TYPE_AS_ExplFnDef fdef;
      fdef.Init(fnm,
                SEQ<TYPE_AS_TypeVar>(),
                ftp,
                pat_ll,
                fnbd,
                Nil(),
                Nil(),
                acc,
                Bool(true),
                Nil(),
                NilContextId);

#ifdef VDMPP
      if (vdm_CPP_isJAVA()) {
        if (get_preandpost_option()) {
          //const TYPE_CI_ContextId & cid (td.GetInt(pos_AS_TypeDef_cid));
          const TYPE_CI_ContextId & cid (inv.GetInt(pos_AS_Invariant_cid));
          cidtocpp.Insert(cid, GenFctDef_FD(fdef, false));
        }
      }
      else 
#endif //VDMPP
      {
        cpp.ImpConc(GenFctDef_FD(fdef, false));
      }
    }
    if (! Eq.IsNil()) {
      TYPE_AS_Equal eq (Eq);
      const TYPE_AS_Name & tnm (td.GetRecord(pos_AS_TypeDef_nm));
      const Generic & acc (td.GetField(pos_AS_TypeDef_access));

      TYPE_AS_Name fnm (vdm_BC_GivePrePostName(tnm, ASTAUX::MkId(L"eq")));

      SEQ<TYPE_AS_Type> shape_l;
      shape_l.ImpAppend(td.GetRecord(pos_AS_TypeDef_shape));
      shape_l.ImpAppend(td.GetRecord(pos_AS_TypeDef_shape));
      TYPE_AS_TotalFnType ftp;
      ftp.Init(shape_l, btp, NilContextId);

      TYPE_AS_Expr expr (eq.GetRecord(pos_AS_Equal_expr));
      TYPE_AS_FnBody fnbd;
      fnbd.Init(expr, NilContextId);

      SEQ<TYPE_AS_Pattern> pat_l;
      pat_l.ImpAppend(eq.GetRecord(pos_AS_Equal_lhs));
      pat_l.ImpAppend(eq.GetRecord(pos_AS_Equal_rhs));

      SEQ<TYPE_AS_Parameters> pat_ll;
      pat_ll.ImpAppend(pat_l);

      TYPE_AS_ExplFnDef fdef;
      fdef.Init(fnm,
                SEQ<TYPE_AS_TypeVar>(),
                ftp,
                pat_ll,
                fnbd,
                Nil(),
                Nil(),
                acc,
                Bool(true),
                Nil(),
                NilContextId);

#ifdef VDMPP
      if (vdm_CPP_isJAVA()) {
        //const TYPE_CI_ContextId & cid (td.GetInt(pos_AS_TypeDef_cid));
        const TYPE_CI_ContextId & cid (eq.GetInt(pos_AS_Equal_cid));
        cidtocpp.Insert(cid, GenFctDef_FD(fdef, false));
      }
      else 
#endif //VDMPP
      {
        cpp.ImpConc(GenFctDef_FD(fdef, false));
      }
    }
    if (! Ord.IsNil()) {
      TYPE_AS_Invariant ord (Ord);
      const TYPE_AS_Name & tnm (td.GetRecord(pos_AS_TypeDef_nm));
      const Generic & acc (td.GetField(pos_AS_TypeDef_access));

      TYPE_AS_Name fnm (vdm_BC_GivePrePostName(tnm, ASTAUX::MkId(L"ord")));

      SEQ<TYPE_AS_Type> shape_l;
      shape_l.ImpAppend(td.GetRecord(pos_AS_TypeDef_shape));
      shape_l.ImpAppend(td.GetRecord(pos_AS_TypeDef_shape));
      TYPE_AS_TotalFnType ftp;
      ftp.Init(shape_l, btp, NilContextId);

      TYPE_AS_Expr expr (ord.GetRecord(pos_AS_Order_expr));
      TYPE_AS_FnBody fnbd;
      fnbd.Init(expr, NilContextId);

      SEQ<TYPE_AS_Pattern> pat_l;
      pat_l.ImpAppend(ord.GetRecord(pos_AS_Order_lhs));
      pat_l.ImpAppend(ord.GetRecord(pos_AS_Order_rhs));

      SEQ<TYPE_AS_Parameters> pat_ll;
      pat_ll.ImpAppend(pat_l);

      TYPE_AS_ExplFnDef fdef;
      fdef.Init(fnm,
                SEQ<TYPE_AS_TypeVar>(),
                ftp,
                pat_ll,
                fnbd,
                Nil(),
                Nil(),
                acc,
                Bool(true),
                Nil(),
                NilContextId);

#ifdef VDMPP
      if (vdm_CPP_isJAVA()) {
        //const TYPE_CI_ContextId & cid (td.GetInt(pos_AS_TypeDef_cid));
        const TYPE_CI_ContextId & cid (ord.GetInt(pos_AS_Order_cid));
        cidtocpp.Insert(cid, GenFctDef_FD(fdef, false));
      }
      else 
#endif //VDMPP
      {
        cpp.ImpConc(GenFctDef_FD(fdef, false));
      }
    }
  }

#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    while (!cidtocpp.Dom().IsEmpty()) {
      SET<TYPE_CI_ContextId> cidtocppdom (cidtocpp.Dom());
      TYPE_CI_ContextId cid (cidtocppdom.GetElem());
      Generic gg;
      for (bool bb = cidtocppdom.First (gg); bb; bb = cidtocppdom.Next (gg)) {
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
      cpp.ImpConc(cidtocpp[cid]);
      cidtocpp.RemElem(cid);
    }
    return cpp;
  }
  else // C++
#endif //VDMPP
  {
    return cpp;
  }
}

// Initialisation of VDM Library

#ifdef VDMSL
// GenVDMLibInit
// mnm : AS`Name
// type_m : map AS`Name to AS`TypeDef
// sd : [AS`StateDef]
// ==> seq of CPP`FunctionDefinition
SEQ<TYPE_CPP_FunctionDefinition> vdmcg::GenVDMLibInit(const TYPE_AS_Name & mnm,
                                                      const MAP<TYPE_AS_Name, TYPE_AS_TypeDef> & type_m,
                                                      const Generic & sd)
{
  type_dL ds_l;
  ds_l.ImpAppend(vdm_BC_GenTypeSpecifier(vdm_BC_GenVoid()));

  TYPE_CPP_Identifier fct_id (vdm_BC_GenIdentifier(ASTAUX::MkId(L"init_").ImpConc(GiveLastName(mnm))
                                                                 .ImpConc(ASTAUX::MkId(L"_VDMLib"))));
  TYPE_CPP_FctDecl fdecl (vdm_BC_GenFctDecl( fct_id, type_dL() ));

  SET<TYPE_AS_CompositeType> type_s;
  Set dom_type_m (type_m.Dom());
  Generic nm;
  for (bool bb = dom_type_m.First(nm); bb; bb = dom_type_m.Next(nm) ) {
    TYPE_AS_TypeDef td (type_m[nm]);
    const TYPE_AS_Type & type (td.GetRecord(pos_AS_TypeDef_shape));
    if (type.Is(TAG_TYPE_AS_CompositeType))
      type_s.Insert(type);
  }
  if ( !sd.IsNil() ) {
    type_s.Insert( Record(sd).GetRecord(pos_AS_StateDef_tp) );
  }

  SEQ<TYPE_CPP_Stmt> body;
  Generic tpg;
  for (bool cc = type_s.First(tpg); cc; cc = type_s.Next(tpg) )
  {
    TYPE_AS_CompositeType type(tpg);
    const TYPE_AS_Name & name (type.GetRecord(pos_AS_CompositeType_name));
    const SEQ<TYPE_AS_Field> & field_l (type.GetSequence(pos_AS_CompositeType_fields));

    TYPE_CPP_Identifier tag (GenCompositeTypeTagId(name));
    body.ImpAppend(GenNewTag(tag, field_l.Length()));
    body.ImpAppend(GenSetSymTag(tag, name, mnm));

    // [DS`GenSetDontCare(tag, no) | 
    //                    no in set inds field_l &
    //                    field_l(no).dc ]
    size_t len (field_l.Length());
    for (size_t no = 1; no <= len; no++ ) {
      if ( field_l[no].get_dc().GetValue() )
        body.ImpAppend(GenSetDontCare(tag, no));
    }
    body.ImpAppend(GenSetRecordTag(tag, name, mnm));
  }

  if ( body.IsEmpty() )
    return SEQ<TYPE_CPP_FunctionDefinition>();
  else
    return SEQ<TYPE_CPP_FunctionDefinition>().ImpAppend(vdm_BC_GenFctDef( ds_l, fdecl, Nil(),
                                                                         vdm_BC_GenBlock(body)));
}
#endif //VDMSL

