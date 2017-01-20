/***
*  * WHAT
*  *    VDM Code Generator.
*  *    mod_expr.cc: Implementation of mod_expr.vdm 1.128
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/mod_expr.cc,v $
*  * VERSION
*  *    $Revision: 1.82 $
*  * DATE
*  *    $Date: 2005/08/22 08:13:08 $
*  * STATUS
*  *    Under development
*  * PROJECT
*  *    Afrodite - ESPRIT programme, project no. 6500.
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "vdmcg.h"
#include "astaux.h"
#include "contextinfo.h"
#include "statsem.h"
#include "BC.h"
#include "position.h"
#include "intconvquotes.h"
#include "tbutils.h"
//#include "mini-p-print.h"

//wcout << L"intertp: " << INT2Q::h2gAS(intertp) << endl;

// EXPR functions and operationscg/mod_expr.cc

// CGExpr
// expr : AS`Expr
// resVar_v : CGMAIN`VT
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGExpr (const TYPE_AS_Expr & expr, const TYPE_CGMAIN_VT & resVar_v)
{
  Generic gpp_v (CGExpr_q(expr, resVar_v)); // (seq of CPP`Stmt) | CPP`Expr
  if (IsSeqOfCPPStmt( gpp_v ))
    return gpp_v;
  else
  {
    TYPE_CPP_Expr pp_v (gpp_v);
    const TYPE_CPP_Expr & res_v (resVar_v.GetRecord(pos_CGMAIN_VT_name));
    const Generic & type (resVar_v.GetField(pos_CGMAIN_VT_type));

    SEQ<TYPE_AS_Stmt> res;
    if (vdm_CPP_isCPP()) {
      SEQ<TYPE_AS_Stmt> res;
      res.ImpAppend(vdm_BC_GenAsgnStmt(res_v, pp_v));
      return res;
    }
    else
    { // for Java
      TYPE_REP_TypeRep tp (RemoveNil(RemoveInvType(FindType(expr))));
      TYPE_CPP_Expr cast;
      if (IsStringType(type) &&
          pp_v.Is(TAG_TYPE_CPP_ClassInstanceCreationExpr) &&
          (pp_v.GetRecord(pos_CPP_ClassInstanceCreationExpr_classtype) == GenStringType().get_tp()))
      {
        const SEQ<TYPE_CPP_Expr> & args (pp_v.GetSequence(pos_CPP_ClassInstanceCreationExpr_arg));
        if (args.IsEmpty())
          cast = vdm_BC_GenStringLit(ASTAUX::MkId(L""));
        else
          cast = args[1];
      }
      else if (IsSubType(tp, type))
        cast = pp_v;
      else
        cast = GenExplicitCast(type, pp_v, tp);
 
      SEQ<TYPE_AS_Stmt> res;
      res.ImpAppend(vdm_BC_GenAsgnStmt(res_v, cast));
      return res;
    }
  }
}

// CGExpr_q (CGExpr')
// expr : AS`Expr
// resVar_v : CGMAIN`VT
// ==> (seq of CPP`Stmt) | CPP`Expr
Generic vdmcg::CGExpr_q (const TYPE_AS_Expr & expr, const TYPE_CGMAIN_VT & resVar_v)
{
//wcout << INT2Q::h2gAS(expr) << endl << flush;
  Generic cpp;
  switch (expr.GetTag ()) {
    case TAG_TYPE_AS_CasesExpr:
      cpp = CGCasesExpr (expr, resVar_v);
      break;
    case TAG_TYPE_AS_BoolLit:
    case TAG_TYPE_AS_CharLit:
    case TAG_TYPE_AS_TextLit:
    case TAG_TYPE_AS_QuoteLit:
    case TAG_TYPE_AS_RealLit:
    case TAG_TYPE_AS_NumLit:
    case TAG_TYPE_AS_NilLit:
      cpp = CGLiteral (expr, resVar_v);
      break;
    case TAG_TYPE_AS_Name: {
#ifdef VDMSL
      Generic tp (GetStatSem().LookUpStateEnv(expr));
      if (tp.Is(TAG_TYPE_REP_CompositeTypeRep) &&
          (Record(tp).GetRecord(pos_REP_CompositeTypeRep_nm) == expr))
      {
        const SEQ<TYPE_REP_FieldRep> & fl (Record(tp).GetSequence(pos_REP_CompositeTypeRep_fields));
        SEQ<TYPE_AS_Expr> exprs;
        size_t len_fl = fl.Length();
        for (size_t i = 1; i <= len_fl; i++)
        {
          const TYPE_REP_FieldRep & fr (fl[i]);
          exprs.ImpAppend(fr.GetField(pos_REP_FieldRep_sel));
        }
        
        TYPE_CI_ContextId cid (GetCI().PushCGType(RemoveInvType(FindType(expr))));
        TYPE_AS_RecordConstructorExpr rce;
        rce.Init(Record(tp).GetRecord(pos_REP_CompositeTypeRep_nm), exprs, cid);
         cpp = CGRecordConstructorExpr(rce, resVar_v);
         break;
        }
        else
#endif // VDMSL
      cpp = CGName (expr);
      break;
    }
    case TAG_TYPE_AS_BinaryExpr:
      cpp = CGBinaryExpr (expr, resVar_v);
      break;
    case TAG_TYPE_AS_SetEnumerationExpr:
      cpp = CGSetEnumerationExpr (expr, resVar_v);
      break;
    case TAG_TYPE_AS_SeqEnumerationExpr:
      cpp = CGSeqEnumerationExpr (expr, resVar_v);
      break;
    case TAG_TYPE_AS_MapEnumerationExpr:
      cpp = CGMapEnumerationExpr (expr, resVar_v);
      break;
    case TAG_TYPE_AS_SetComprehensionExpr:
      cpp = CGSetComprehensionExpr (expr, resVar_v);
      break;
    case TAG_TYPE_AS_MapComprehensionExpr:
      cpp = CGMapComprehensionExpr (expr, resVar_v);
      break;
    case TAG_TYPE_AS_SeqComprehensionExpr:
      cpp = CGSeqComprehensionExpr (expr, resVar_v);
      break;
    case TAG_TYPE_AS_SetRangeExpr:
      cpp = CGSetRangeExpr (expr, resVar_v);
      break;
    case TAG_TYPE_AS_SubSequenceExpr:
      cpp = CGSubSeqExpr (expr, resVar_v);
      break;
    case TAG_TYPE_AS_TupleConstructorExpr:
      cpp = CGTupleConstructorExpr (expr, resVar_v);
      break;
    case TAG_TYPE_AS_ApplyExpr:
      cpp = CGApplyExpr (expr, resVar_v, Nil(), Nil(), false);
      break;
    case TAG_TYPE_AS_RecordConstructorExpr:
      cpp = CGRecordConstructorExpr (expr, resVar_v);
      break;
    case TAG_TYPE_AS_LetExpr:
      cpp = CGLetExpr (expr, resVar_v);
      break;
    case TAG_TYPE_AS_LetBeSTExpr:
      cpp = CGLetBeSTExpr(expr, resVar_v);
      break;
    case TAG_TYPE_AS_DefExpr:
      cpp = CGDefExpr (expr, resVar_v);
      break;
    case TAG_TYPE_AS_IfExpr:
      cpp = CGIfExpr (expr, resVar_v);
      break;
    case TAG_TYPE_AS_BracketedExpr:
      cpp = CGBracketedExpr (expr, resVar_v);
      break;
    case TAG_TYPE_AS_SeqModifyMapOverrideExpr:
      cpp = CGSeqModifyMapOverrideExpr (expr, resVar_v);
      break;
    case TAG_TYPE_AS_FieldSelectExpr:
      cpp = CGFieldSelectExpr (expr, resVar_v, Nil(), Nil());
      break;
    case TAG_TYPE_AS_ExistsUniqueExpr:
    case TAG_TYPE_AS_AllOrExistsExpr:
      cpp = CGAllOrExistsExpr (expr, resVar_v);
      break;
    case TAG_TYPE_AS_IotaExpr:
      cpp = CGIotaExpr(expr, resVar_v);
      break;
    case TAG_TYPE_AS_IsExpr:
      cpp = CGIsExpr (expr, resVar_v);
      break;
    case TAG_TYPE_AS_PrefixExpr:
      cpp = CGPrefixExpr (expr, resVar_v);
      break;
    case TAG_TYPE_AS_RecordModifierExpr:
      cpp = CGRecordModifierExpr(expr, resVar_v);
      break;
    case TAG_TYPE_AS_UndefinedExpr:
      cpp = CGUndefinedExpr();
      break;
    case TAG_TYPE_AS_OldName:
      cpp = CGOldName(expr);
      break;
    case TAG_TYPE_AS_TokenConstructorExpr: {
      cpp = CGTokenConstExpr(expr, resVar_v);
      break;
    } 
    case TAG_TYPE_AS_FctTypeInstExpr: {
      Sequence res;
      SetNotSupportedException(true);
      res.ImpAppend(NotSupported(L"function type instantiation expression", expr));
      cpp = res;
      break;
    }
    case TAG_TYPE_AS_LambdaExpr: {
      SetNotSupportedException(true);
      cpp = Sequence().ImpAppend(NotSupported(L"lambda expression", expr));
      break;
    }
    case TAG_TYPE_AS_TypeJudgementExpr: {
      cpp = CGTypeJudgementExpr(expr, resVar_v);
      break;
    }
    case TAG_TYPE_AS_PreConditionApplyExpr: {
      SetNotSupportedException(true);
      cpp = Sequence().ImpAppend(NotSupported(L"precondition expression", expr));
      break;
    }
    case TAG_TYPE_AS_TupleSelectExpr: {
      cpp = CGTupleSelectExpr(expr, resVar_v);
      break;
    }
    case TAG_TYPE_AS_NarrowExpr: {
      cpp = CGNarrowExpr(expr, resVar_v);
      break;
    }
    case TAG_TYPE_AS_Macro: {
      cpp = CGMacro(expr, resVar_v);
      break;
    }
#ifdef VDMPP
    case TAG_TYPE_AS_NewExpr: {
      cpp = CGNewExpr (expr, resVar_v);
      break;
    }
    case TAG_TYPE_AS_SelfExpr: {
      cpp = GenSelfExpr (resVar_v);
      break;
    }
    case TAG_TYPE_AS_IsOfClassExpr: {
      cpp = CGIsOfClassExpr(expr, resVar_v);
      break;
    }
    case TAG_TYPE_AS_IsOfBaseClassExpr: {
      cpp = CGIsOfBaseClassExpr(expr, resVar_v);
      break;
    }
    case TAG_TYPE_AS_SameClassExpr: {
      cpp = CGSameClassExpr(expr, resVar_v);
      break;
    }
    case TAG_TYPE_AS_SameBaseClassExpr: {
      cpp = CGSameBaseClassExpr(expr, resVar_v);
      break;
    }
    case TAG_TYPE_AS_GuardExpr: {
      if (vdm_CPP_isCPP()) {
        SetNotSupportedException(true);
        cpp = Sequence().ImpAppend(NotSupported(L"guard expression", expr));
      }
      else{
        const TYPE_AS_Expr & innerExpr (expr.GetRecord(pos_AS_GuardExpr_expr));
        const TYPE_AS_Name & origin    (expr.GetRecord(pos_AS_GuardExpr_name));
        SetDefaultCast(origin);
        cpp = CGExpr_q(innerExpr, resVar_v);
        break;
      }
      break;
    }
    case TAG_TYPE_AS_ActExpr: {
      if (vdm_CPP_isCPP()) {
        SetNotSupportedException(true);
        cpp = Sequence().ImpAppend(NotSupported(L"act. expression", expr));
      }
      else
        cpp = GenGuardExpr(expr);
      break;
    }
    case TAG_TYPE_AS_FinExpr: {
      if (vdm_CPP_isCPP())
      {
        SetNotSupportedException(true);
        cpp = Sequence().ImpAppend(NotSupported(L"fin. expression", expr));
      }
      else
        cpp = GenGuardExpr(expr);
      break;
    }
    case TAG_TYPE_AS_ActiveExpr: {
      if (vdm_CPP_isCPP())
      {
        SetNotSupportedException(true);
        cpp = Sequence().ImpAppend(NotSupported(L"active. expression", expr));
      }
      else
        cpp = GenGuardExpr(expr);
      break;
    }
    case TAG_TYPE_AS_WaitingExpr: {
      if (vdm_CPP_isCPP())
      {
        SetNotSupportedException(true);
        cpp = Sequence().ImpAppend(NotSupported(L"waiting expression", expr));
      }
      else
        cpp = GenGuardExpr(expr);
      break;
    }
    case TAG_TYPE_AS_ReqExpr: {
      if (vdm_CPP_isCPP())
      {
        SetNotSupportedException(true);
        cpp = Sequence().ImpAppend(NotSupported(L"req. expression", expr));
      }
      else
        cpp = GenGuardExpr(expr);
      break;
    }
    case TAG_TYPE_AS_ThreadIdExpr: {
      if (vdm_CPP_isCPP())
      {
        SetNotSupportedException(true);
        cpp = Sequence().ImpAppend(NotSupported(L"threadid expression", expr));
      }
      else // Java
        if (!(get_conc_option())) {
          SetNotSupportedException(true);
          cpp = Sequence().ImpAppend(NotSupported(L"threadId expression", expr));
        }
        else
          cpp = GenThreadId();
      break;
    }
#ifdef VICE
    case TAG_TYPE_AS_CurTimeExpr: { // 20070216
        SetNotSupportedException(true);
        cpp = Sequence().ImpAppend(NotSupported(L"time expression", expr));
      break;
    }
#endif // VICE
#endif // VDMPP
  
    default: {
      ReportError(L"CGExpr");
      return Record(0,0); // To avoid warnings
    }
  }

//wcout << L"end: " << expr.GetTag () << endl << flush;

  if (rti) {
    // Add run-time position information
    SEQ<TYPE_CPP_Stmt> pos_l (GenUpdatePosInfo(expr));
    if (IsSeqOfCPPStmt(cpp))
      pos_l.ImpConc(cpp);
    else {
      const TYPE_CPP_Expr & res_v (resVar_v.GetRecord(pos_CGMAIN_VT_name));
      pos_l.ImpAppend(vdm_BC_GenAsgnStmt(res_v, cpp));
    }
    pos_l.ImpAppend(GenPopPosInfo());
    return pos_l;
  }
  else
    return cpp;
}

// CGExprExcl
// expr : AS`Expr
// e_name : ([seq of char] | CPP`Name)
// etp : [REP`TypeRep]
// ==> CPP`Expr * seq of CPP`Stmt
Tuple vdmcg::CGExprExcl(const TYPE_AS_Expr & expr, const Generic & e_name, const Generic & etp)
{
  TYPE_CPP_Identifier etmpVar;
  if (e_name.IsNil())
    etmpVar = vdm_BC_GiveName(ASTAUX::MkId(L"e"));
  else if (e_name.Is(TAG_TYPE_CPP_Identifier))
    etmpVar = e_name;
  else
    etmpVar = vdm_BC_GiveName(e_name);

  TYPE_REP_TypeRep e_t;
  if (etp.IsNil())
    e_t = FindType(expr);
  else
    e_t = etp;

  Generic exl_v (CGExpr_q(expr, mk_CG_VT(etmpVar, e_t))); // (seq of CPP`Stmt) | CPP`Expr

  if (IsSeqOfCPPStmt(exl_v))
  {
    if (e_name.Is(TAG_TYPE_CPP_Identifier))
    {
      return mk_(etmpVar, exl_v);
    }
    else
    {
      TYPE_REP_TypeRep tp (RemoveInvType(e_t));
      SEQ<TYPE_CPP_Stmt> decl_stmt (GenDecl_DS(tp, etmpVar, Nil())); // must be here for include

      SEQ<TYPE_CPP_Stmt> stmts (exl_v);
      size_t len_exl_v = stmts.Length();
      TYPE_CPP_Stmt stmt (stmts[len_exl_v]);
      if (stmt.Is(TAG_TYPE_CPP_ExpressionStmt) &&
          stmt.GetField(pos_CPP_ExpressionStmt_expr).Is(TAG_TYPE_CPP_AssignExpr) &&
          (stmt.GetRecord(pos_CPP_ExpressionStmt_expr).GetRecord(pos_CPP_AssignExpr_unary) == etmpVar) &&
          (stmt.GetRecord(pos_CPP_ExpressionStmt_expr).GetRecord(pos_CPP_AssignExpr_assignop)
                                .GetField(pos_CPP_AssignOp_op) == Quote(L"ASEQUAL"))) 
      {
        const TYPE_CPP_Expr & assignexpr (stmt.GetRecord(pos_CPP_ExpressionStmt_expr)
                                              .GetRecord(pos_CPP_AssignExpr_assignexpr));
        TYPE_CPP_Expr retex;
        if (IsSubType(FindType(expr), tp))
        {
          switch (assignexpr.GetTag()) {
            case TAG_TYPE_CPP_Identifier:
            case TAG_TYPE_CPP_ClassInstanceCreationExpr:
            case TAG_TYPE_CPP_FctCall:
            case TAG_TYPE_CPP_BracketedExpr: { retex = assignexpr; break; }
            case TAG_TYPE_CPP_CastExpr: {
              retex = (vdm_CPP_isCPP() ? assignexpr : vdm_BC_GenBracketedExpr(assignexpr));
              break;
            }
            default: { retex = vdm_BC_GenBracketedExpr(assignexpr); break; }
          }
        }
        else
        {
          if (vdm_CPP_isCPP())
            retex = vdm_BC_GenCastExpr(GenType(tp), assignexpr);
          else
            retex = GenExplicitCast(tp, assignexpr, Nil());
        } 
        return mk_(retex, stmts.SubSequence(1, len_exl_v - 1));
      }
      else
      {
        TYPE_CPP_Stmt stmt (stmts[1]);
        if (stmt.Is(TAG_TYPE_CPP_ExpressionStmt) &&
            stmt.GetField(pos_CPP_ExpressionStmt_expr).Is(TAG_TYPE_CPP_AssignExpr) &&
            (stmt.GetRecord(pos_CPP_ExpressionStmt_expr).GetRecord(pos_CPP_AssignExpr_unary) == etmpVar) &&
            (stmt.GetRecord(pos_CPP_ExpressionStmt_expr).GetRecord(pos_CPP_AssignExpr_assignop)
                                .GetField(pos_CPP_AssignOp_op) == Quote(L"ASEQUAL"))) 
        {
          const TYPE_CPP_Expr & assignexpr (stmt.GetRecord(pos_CPP_ExpressionStmt_expr)
                                                .GetRecord(pos_CPP_AssignExpr_assignexpr));
          SEQ<TYPE_CPP_Stmt> eDecl;
          eDecl.ImpConc(GenDeclInit_DS(tp, etmpVar, assignexpr));
          eDecl.ImpConc(stmts.Tl());
          return mk_(etmpVar, eDecl);
        }
        else
        {
          SEQ<TYPE_CPP_Stmt> eDecl;
          eDecl.ImpConc(decl_stmt);
          eDecl.ImpConc(exl_v);
          return mk_(etmpVar, eDecl);
        }
      }
    }
  }
  else
  { // !IsSeqOfCPPStmt(exl_v)
    if (vdm_CPP_isCPP())
      return mk_(exl_v, SEQ<TYPE_CPP_Stmt>());
    else
    {
      Generic gtp (RemoveNil(FindType(expr)));
      TYPE_REP_TypeRep tp (etp.IsNil() ? gtp : etp);
      if (IsSubType(gtp, tp))
      {
        if ((IsIntType(tp) && !IsIntType(gtp)) || (!IsIntType(tp) && IsIntType(gtp)))
          return mk_(GenExplicitCast(tp, exl_v, Nil()), SEQ<TYPE_CPP_Stmt>());
        else
          return mk_(exl_v, SEQ<TYPE_CPP_Stmt>());
      }
      else if (((gtp.Is(TAG_TYPE_REP_EmptySeqTypeRep) && IsSeqType(tp)) ||
                (gtp.Is(TAG_TYPE_REP_EmptySetTypeRep) && IsSetType(tp)) ||
                (gtp.Is(TAG_TYPE_REP_EmptyMapTypeRep) && IsMapType(tp))) &&
               exl_v.Is(TAG_TYPE_CPP_ClassInstanceCreationExpr))
      {
        return mk_(exl_v, SEQ<TYPE_CPP_Stmt>());
      }
      else if (IsProductType(tp) && IsProductType(gtp))
      {
        return mk_(exl_v, SEQ<TYPE_CPP_Stmt>());
      }
      else
        return mk_(GenExplicitCast(tp, exl_v, Nil()), SEQ<TYPE_CPP_Stmt>());
    }
  }
}

// MergeStmts
// decl : seq of CPP`Stmt
// pm : seq of CPP`Stmt
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::MergeStmts (const SEQ<TYPE_CPP_Stmt> & decls, const SEQ<TYPE_CPP_Stmt> & pm) {
  Map id_m;
  size_t len_decls = decls.Length();
  for ( size_t i = 1; i <= len_decls; i++ ) {
    const TYPE_CPP_Stmt & stmt (decls[i]);
    if ( stmt.Is(TAG_TYPE_CPP_DeclarationStmt) &&
         stmt.GetRecord(pos_CPP_DeclarationStmt_decl).Is(TAG_TYPE_CPP_IdentDeclaration)) {
      const TYPE_CPP_IdentDeclaration & iDecl (stmt.GetRecord(pos_CPP_DeclarationStmt_decl));
      const SEQ<TYPE_CPP_InitDeclarator> & ids (iDecl.GetSequence(pos_CPP_IdentDeclaration_dl));
      if ( ids.Length() == 1 ) {
        const TYPE_CPP_Declarator & decl (ids[1].GetRecord(pos_CPP_InitDeclarator_decl));
        if ( decl.Is(TAG_TYPE_CPP_Identifier) ) {
          id_m.Insert( decl, stmt );
        }
      }
    }
  }
  SEQ<TYPE_CPP_Stmt> new_decls (decls);
  SEQ<TYPE_CPP_Stmt> stmts (pm);
  size_t len_pm = pm.Length();
  for  ( size_t index = 1; index <= len_pm; index++ ) {
    TYPE_CPP_Stmt stmt (pm[index]);
    if (stmt.Is(TAG_TYPE_CPP_ExpressionStmt) &&
        stmt.GetField(pos_CPP_ExpressionStmt_expr).Is(TAG_TYPE_CPP_AssignExpr) &&
        (stmt.GetRecord(pos_CPP_ExpressionStmt_expr).GetRecord(pos_CPP_AssignExpr_assignop)
                            .GetField(pos_CPP_AssignOp_op) == Quote(L"ASEQUAL"))) {
      const TYPE_CPP_Expr & unary (stmt.GetRecord(pos_CPP_ExpressionStmt_expr)
                                       .GetRecord(pos_CPP_AssignExpr_unary));
      if (id_m.DomExists( unary ) ) {
        const TYPE_CPP_Expr & assignexpr (stmt.GetRecord(pos_CPP_ExpressionStmt_expr)
                                              .GetRecord(pos_CPP_AssignExpr_assignexpr));
        TYPE_CPP_DeclarationStmt declstmt ( id_m[unary] ); 
        int64_t idx = new_decls.Find( declstmt );
        new_decls.RemElem( idx ); 

        TYPE_CPP_IdentDeclaration idecl (declstmt.GetRecord(pos_CPP_DeclarationStmt_decl));
        SEQ<TYPE_CPP_DeclSpecifier> ds (idecl.GetSequence(pos_CPP_IdentDeclaration_ds));
        TYPE_CPP_InitDeclarator inidec (idecl.GetSequence(pos_CPP_IdentDeclaration_dl)[1]);
        ds.ImpPrepend(vdm_BC_GenTypeSpecifier(quote_CONST));
        TYPE_CPP_Initializer initExpr;
// 20160622 -->
        if (vdm_CPP_isCPP()) {
          //initExpr = vdm_BC_GenObjectInit(mk_sequence(assignexpr));
          if (assignexpr.Is(TAG_TYPE_CPP_FctCall) &&
              assignexpr.GetRecord(pos_CPP_FctCall_fct).Is(TAG_TYPE_CPP_Identifier) &&
              assignexpr.GetSequence(pos_CPP_FctCall_arg).IsEmpty()) {
            const SEQ<Char> & id (assignexpr.GetRecord(pos_CPP_FctCall_fct).GetSequence(pos_CPP_Identifier_id));
            if ( id.StartsWith( SEQ<Char>(L"vdm_")) )
              initExpr = vdm_BC_GenObjectInit(mk_sequence(assignexpr));
            else
              initExpr = vdm_BC_GenAsgnInit(assignexpr);
          }
          else
            initExpr = vdm_BC_GenObjectInit(mk_sequence(assignexpr));
// <-- 20160622
        }
        else
          initExpr = vdm_BC_GenAsgnInit(assignexpr);
        inidec.SetField(pos_CPP_InitDeclarator_i, initExpr);
        idecl.SetField(pos_CPP_IdentDeclaration_ds, ds);
        idecl.SetField(pos_CPP_IdentDeclaration_dl, mk_sequence(inidec));
        declstmt.SetField(pos_CPP_DeclarationStmt_decl, idecl);
        stmts.ImpModify(index, declstmt);
      }
    }
  }
  return new_decls.ImpConc(stmts);
}

// CGCasesExpr
// rc1 : AS`CasesExpr
// vt : CGMAIN`VT
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGCasesExpr (const TYPE_AS_CasesExpr & rc1,
                                       const TYPE_CGMAIN_VT & vt)
{
  const TYPE_AS_Expr & sel            (rc1.GetRecord(  pos_AS_CasesExpr_sel));
  const SEQ<TYPE_AS_CaseAltn> & altns (rc1.GetSequence(pos_AS_CasesExpr_altns));
  const Generic & Others              (rc1.GetField(   pos_AS_CasesExpr_Others));

  TYPE_CPP_Identifier succ_v (vdm_BC_GiveName (ASTAUX::MkId(L"succ")));

  Tuple cgee (CGExprExcl(sel, ASTAUX::MkId(L"selRes"), nil));
  const TYPE_CPP_Expr & selRes1_v (cgee.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & selRes_stmt (cgee.GetSequence(2));

  SET<TYPE_AS_Name> all_pat_ids (FindAllPatIdInCasesAlt(altns));
  SET<TYPE_AS_Name> all_nm_in_expr(FindAllNamesInExpr(sel));
  bool pat_in_expr ( !((all_pat_ids.Intersect(all_nm_in_expr)).IsEmpty()));

  TYPE_CPP_Expr sel_v (vdm_BC_GiveName(ASTAUX::MkId(L"selRes")));
  TYPE_REP_TypeRep sel_type (FindType(sel));

  bool need_decl = (pat_in_expr && selRes_stmt.IsEmpty());

#ifdef VDMPP
  bool exists = false;
  Generic n;
  for (bool bb = all_nm_in_expr.First(n); bb && !exists; bb = all_nm_in_expr.Next(n))
  {
    exists = FindType(n).Is(TAG_TYPE_REP_ObjRefTypeRep);
  }
  need_decl = need_decl || exists;
#endif // VDMPP
  switch (sel.GetTag()) {
    case TAG_TYPE_AS_TupleConstructorExpr: 
    case TAG_TYPE_AS_RecordConstructorExpr:
    case TAG_TYPE_AS_SetEnumerationExpr:
    case TAG_TYPE_AS_SeqEnumerationExpr: {
      need_decl = true;
      break;
    }
  }

  TYPE_CPP_Expr selRes_v (need_decl ? sel_v : selRes1_v);

  SEQ<TYPE_CPP_Stmt> rb (selRes_stmt);

  if (need_decl)
    rb.ImpConc(GenConstDeclInit(sel_type, sel_v, selRes1_v));

  rb.ImpAppend (vdm_BC_GenDecl (GenSmallBoolType(), succ_v, vdm_BC_GenAsgnInit (vdm_BC_GenBoolLit(false))));

  rb.ImpConc (CGCasesExprAltn (altns, mk_CG_VT (selRes_v, sel_type), vt, succ_v));

  if (!Others.IsNil ())
  {
//#ifdef VDMPP
//    rb.ImpAppend(vdm_BC_GenSingleLineComments(SEQ<Char>(L"others")));
//#endif // VDMPP
    TYPE_CPP_Stmt alt1 (vdm_BC_GenBlock(CGExpr(Others, vt)));
    rb.ImpAppend (vdm_BC_GenIfStmt (vdm_BC_GenNot (succ_v), alt1, nil));
  }
  else
  {
    rb.ImpAppend (vdm_BC_GenIfStmt (vdm_BC_GenNot (succ_v),
                     vdm_BC_GenBlock(mk_sequence(RunTime (L"No 'others' branch in CasesExpr"))), nil));
  }
  return rb;
}

// FindAllPatIdInCasesAlt
// altns : seq of (AS`CaseAltn | AS`CasesStmtAltn)
// ==> set of AS`Name
SET<TYPE_AS_Name> vdmcg::FindAllPatIdInCasesAlt(const SEQ<Record> & altns)
{
  SET<TYPE_AS_Name> res;
  size_t len_altns = altns.Length();
  for (size_t idx = 1; idx <= len_altns; idx++)
  {
    const Record & r (altns[idx]);
    switch(r.GetTag()) {
      case TAG_TYPE_AS_CaseAltn: {
        const SEQ<TYPE_AS_Pattern> & match (r.GetSequence(pos_AS_CaseAltn_match));
        size_t len_match = match.Length();
        for (size_t i = 1; i <= len_match; i++)
        {
          res.ImpUnion(FindPatternId(match[i]).Dom());
        }
        break;
      }
      case TAG_TYPE_AS_CasesStmtAltn: {
        const SEQ<TYPE_AS_Pattern> & match (r.GetSequence(pos_AS_CasesStmtAltn_match));
        size_t len_match = match.Length();
        for (size_t i = 1; i <= len_match; i++)
        {
          res.ImpUnion(FindPatternId(match[i]).Dom());
        }
        break;
      }
    }
  }
  return res;
}

// CGCasesExprAltn
// altns : seq1 of AS`CaseAltn
// selRes_v : CGMAIN`VT
// resVar_v : CGMAIN`VT
// succ_v : CPP`Name
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGCasesExprAltn (const SEQ<TYPE_AS_CaseAltn> & altns,
                                           const TYPE_CGMAIN_VT & selRes_v,
                                           const TYPE_CGMAIN_VT & resVar_v,
                                           const TYPE_CPP_Name & succ_v)
{
  SEQ<TYPE_CPP_Stmt> rb;
  size_t len_altns = altns.Length();
  for (size_t i = 1; i <= len_altns; i++)
  {
    TYPE_CPP_Stmt stmt (CGAltn (altns[i], selRes_v, resVar_v, succ_v));
    if (i == 1)
      rb.ImpAppend (stmt);
    else
      rb.ImpAppend (vdm_BC_GenIfStmt (vdm_BC_GenNot (succ_v), stmt, nil));
  }
  return rb;
}

// CGAltn
// rc1 : AS`CaseAltn
// selRes_v : CGMAIN`VT
// resVar_v : CGMAIN`VT
// succ_v : CPP`Name
// ==> CPP`Stmt
TYPE_CPP_Stmt vdmcg::CGAltn(const TYPE_AS_CaseAltn & rc1,
                            const TYPE_CGMAIN_VT & selRes_v,
                            const TYPE_CGMAIN_VT & resVar_v,
                            const TYPE_CPP_Name & succ_v)
{
  PushEnv();
  PushEnv_CGAUX();

  const SEQ<TYPE_AS_Pattern> & p_l (rc1.GetSequence(pos_AS_CaseAltn_match));
  const TYPE_AS_Expr & e (rc1.GetRecord(pos_AS_CaseAltn_body));
  const TYPE_CI_ContextId & cid (rc1.GetInt(pos_AS_CaseAltn_cid));

  // ==> bool * map AS`Name to set of REP`TypeRep
  Tuple spi (SamePatternIds (p_l));
  const Bool & samepatternIds (spi.GetBool (1));
  const Map & pid_m (spi.GetMap (2)); // map AS`Name to set of REP`TypeRep

  SEQ<TYPE_CPP_Stmt> rb;
// 20100622 -->
//#ifdef VDMPP
//    rb.ImpAppend(vdm_BC_GenSingleLineComments(SEQ<Char>(MPP::MiniPP(INT2Q::h2gAS(rc1)))));
//#endif // VDMPP
// <-- 20100622
  if (samepatternIds.GetValue ())
  {
    SEQ<TYPE_CPP_Stmt> decl (DeclarePatterns (pid_m)); // must be first of CGPatternMatch
    if ( p_l.Length() > 1)
      rb.ImpConc( decl );

    size_t len_p_l = p_l.Length();
    for (size_t i = 1; i <= len_p_l; i++)
    {
      Tuple cgpm (CGPatternMatchExcl (p_l[i], selRes_v, eset, succ_v, Map(), Nil(), false));
      const SEQ<TYPE_CPP_Stmt> & pm (cgpm.GetSequence(1));
      bool Is_Excl (cgpm.GetBoolValue(2)); // false : need to check pattern match failed

      SEQ<TYPE_CPP_Stmt> pm1;
      if (Is_Excl)
        pm1.ImpAppend(vdm_BC_GenAsgnStmt(succ_v,vdm_BC_GenBoolLit(true)));
      pm1.ImpConc(pm);

      if (i == 1) {
        if ( len_p_l > 1 ) 
          rb.ImpConc(pm1);
        else
          rb.ImpConc(MergeStmts(decl,pm1));
      }
      else
        rb.ImpAppend(vdm_BC_GenIfStmt (vdm_BC_GenNot (succ_v), vdm_BC_GenBlock(pm1), nil));
    }
    SEQ<TYPE_CPP_Stmt> stmts;
//#ifdef VDMPP
//    stmts.ImpAppend(vdm_BC_GenSingleLineComments(SEQ<Char>(MPP::MiniPP(INT2Q::h2gAS(e)))));
//#endif // VDMPP
    stmts.ImpConc(CGExpr(e,resVar_v));
    rb.ImpAppend(vdm_BC_GenIfStmt (succ_v, vdm_BC_GenBlock(stmts), nil));
  }
  else
  {
    SEQ<TYPE_AS_CaseAltn> ca_l;
    size_t len_p_l = p_l.Length();
    for (size_t idx = 1; idx <= len_p_l; idx++)
    {
      SEQ<TYPE_AS_Pattern> sq;
      sq.ImpAppend (p_l[idx]);
      TYPE_AS_CaseAltn ca;
      ca.Init(sq, e, cid);
      ca_l.ImpAppend (ca);
    }
    rb.ImpConc(CGCasesExprAltn (ca_l, selRes_v, resVar_v, succ_v));
  }

  PopEnv();
  PopEnv_CGAUX();

  return vdm_BC_GenBlock (rb);
}

// SamePatternIds modified for Java and C++ (jojo, 11.12.98)

// SamePatternIds
// p_l : seq of AS`Pattern
// ==> bool * map (AS`Name | AS`OldName) to set of REP`TypeRep
Tuple vdmcg::SamePatternIds (const SEQ<TYPE_AS_Pattern> & p_l)
{
  if (!p_l.IsEmpty ())
  {
    Map pid_m (FindPatternId (p_l.Hd ())); // map AS`Name to set of REP`TypeRep
    bool same_pid = true;

    size_t len_p_l = p_l.Length();
    for (size_t idx = 2; idx <= len_p_l; idx++)
    {
      Map tmp_m (FindPatternId (p_l[idx])); // map AS`Name to set of REP`TypeRep
      same_pid = same_pid && tmp_m.Dom().SubSet(pid_m.Dom());
      if ( same_pid ) {
        pid_m = MergePidM(tmp_m, pid_m);
      }
      else {
        pid_m.ImpOverride(tmp_m);
      }
    }
    return mk_(Bool (same_pid), pid_m);
  }
  else
    return mk_(Bool (false), Map ());
}

// FindPatternId
// pat : AS`Pattern
// ==> map (AS`Name | AS`OldName) to set of REP`TypeRep
Map vdmcg::FindPatternId (const TYPE_AS_Pattern & pat)
{
  switch (pat.GetTag ()) {
    case TAG_TYPE_AS_PatternName: {
      Map res;
      if (!pat.GetField(pos_AS_PatternName_nm).IsNil ())
      {
        SET<TYPE_REP_TypeRep> st;
        Generic tp (FindType(pat));
        if (tp.IsNil())
          st.Insert (TYPE_REP_AllTypeRep());
        else
          st.Insert (tp);
        res.Insert (pat.GetRecord(pos_AS_PatternName_nm), st);
      }
      return res; 
      break;
    }
    case TAG_TYPE_AS_MatchVal:
      return Map ();
    case TAG_TYPE_AS_SetEnumPattern: {
      const SEQ<TYPE_AS_Pattern> & pat_l (pat.GetSequence (pos_AS_SetEnumPattern_Elems));
      Map pid_m; // map AS`Name to set of REP`TypeRep
      size_t len_pat_l = pat_l.Length();
      for (size_t idx = 1; idx <= len_pat_l; idx++)
        pid_m = MergePidM(FindPatternId (pat_l[idx]), pid_m);
      return pid_m; 
      break;
    }
    case TAG_TYPE_AS_SeqEnumPattern: {
      const SEQ<TYPE_AS_Pattern> & pat_l (pat.GetSequence (pos_AS_SeqEnumPattern_els));
      Map pid_m; // map AS`Name to set of REP`TypeRep
      size_t len_pat_l = pat_l.Length();
      for (size_t idx = 1; idx <= len_pat_l; idx++)
        pid_m = MergePidM(FindPatternId (pat_l[idx]), pid_m);
      return pid_m; 
      break;
    }
    case TAG_TYPE_AS_MapEnumPattern: {
      const SEQ<TYPE_AS_MapletPattern> & pat_l (pat.GetSequence (pos_AS_MapEnumPattern_mls));
      Map pid_m; // map AS`Name to set of REP`TypeRep
      size_t len_pat_l = pat_l.Length();
      for (size_t idx = 1; idx <= len_pat_l; idx++)
        pid_m = MergePidM(FindPatternId (pat_l[idx]), pid_m);
      return pid_m; 
      break;
    }
    case TAG_TYPE_AS_TuplePattern: {
      const SEQ<TYPE_AS_Pattern> & pat_l (pat.GetSequence (pos_AS_TuplePattern_fields));
      Map pid_m; // map AS`Name to set of REP`TypeRep
      size_t len_pat_l = pat_l.Length();
      for (size_t idx = 1; idx <= len_pat_l; idx++)
        pid_m = MergePidM(FindPatternId (pat_l[idx]), pid_m);
      return pid_m; 
      break;
    }
    case TAG_TYPE_AS_RecordPattern: {
      const SEQ<TYPE_AS_Pattern> & pat_l (pat.GetSequence(pos_AS_RecordPattern_fields));
      Map pid_m; // map AS`Name to set of REP`TypeRep
      size_t len_pat_l = pat_l.Length();
      for (size_t idx = 1; idx <= len_pat_l; idx++)
        pid_m = MergePidM(FindPatternId (pat_l[idx]), pid_m);
      return pid_m; 
      break;
    }
    case TAG_TYPE_AS_SeqConcPattern: {
      Map pid_m (FindPatternId(pat.GetRecord(pos_AS_SeqConcPattern_lp)));
      pid_m.ImpOverride(FindPatternId(pat.GetRecord(pos_AS_SeqConcPattern_rp)));
      return pid_m; 
      break;
    }
    case TAG_TYPE_AS_SetUnionPattern: {
      Map pid_m (FindPatternId(pat.GetRecord(pos_AS_SetUnionPattern_lp)));
      pid_m.ImpOverride(FindPatternId(pat.GetRecord(pos_AS_SetUnionPattern_rp)));
      return pid_m; 
      break;
    }
    case TAG_TYPE_AS_MapMergePattern: {
      Map pid_m (FindPatternId(pat.GetRecord(pos_AS_MapMergePattern_lp)));
      pid_m.ImpOverride(FindPatternId(pat.GetRecord(pos_AS_MapMergePattern_rp)));
      return pid_m; 
      break;
    }
    case TAG_TYPE_AS_MapletPattern: {
      Map pid_m (FindPatternId(pat.GetRecord(pos_AS_MapletPattern_dp)));
      pid_m.ImpOverride(FindPatternId(pat.GetRecord(pos_AS_MapletPattern_rp)));
      return pid_m; 
      break;
    }
#ifdef VDMPP
    case TAG_TYPE_AS_ObjectPattern: {
      const SEQ<TYPE_AS_FieldPattern> & pat_l (pat.GetSequence(pos_AS_ObjectPattern_fields));
      Map pid_m; // map AS`Name to set of REP`TypeRep
      size_t len_pat_l = pat_l.Length();
      for (size_t idx = 1; idx <= len_pat_l; idx++)
        pid_m = MergePidM(FindPatternId (pat_l[idx]), pid_m);
      return pid_m; 
      break;
    }
    case TAG_TYPE_AS_FieldPattern: {
      return FindPatternId(pat.GetRecord(pos_AS_FieldPattern_pat));
      break;
    }
#endif // VDMPP
    default:
      ReportError (L"FindPatternId");
      break;
  }
  return Map();
}

// MergePidM
// pid_m1 : map (AS`Name | AS`OldName) to set of REP`TypeRep
// pid_m2 : map (AS`Name | AS`OldName) to set of REP`TypeRep
// ==> map (AS`Name | AS`OldName) to set of REP`TypeRep
Map vdmcg::MergePidM(const Map & pid_m1, const Map & pid_m2)
{
  Map pid_m;
  Set dom_pid_m (pid_m1.Dom().ImpUnion(pid_m2.Dom()));
  Generic id;
  for (bool bb = dom_pid_m.First(id); bb; bb = dom_pid_m.Next(id))
  {
    SET<TYPE_REP_TypeRep> rng;
    if (pid_m1.DomExists(id))
      rng.ImpUnion(pid_m1[id]);
    if (pid_m2.DomExists(id))
      rng.ImpUnion(pid_m2[id]);
    pid_m.ImpModify(id, rng);
  }
  return pid_m;
}

// DeclarePatterns
// pid_m : map (AS`Name | AS`OldName) to set of REP`TypeRep
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::DeclarePatterns (const Map & pid_m)
{
  SEQ<TYPE_CPP_Stmt> rb;

  SET<TYPE_AS_Name> dom_pid_m (pid_m.Dom());
  Generic tmp;
  for (bool bb = dom_pid_m.First(tmp); bb; bb = dom_pid_m.Next(tmp))
  {
    const SET<TYPE_REP_TypeRep> & tp_s (pid_m[tmp]);

    TYPE_REP_TypeRep type;
    if (tp_s.Card() == 1)
      type = tp_s.GetElem();
    else {
      type = TYPE_REP_UnionTypeRep().Init(tp_s);
    }

    InsertName_CGAUX(tmp);
    rb.ImpConc (GenDecl_DS (type, vdm_BC_Rename(tmp), nil));

    if (PossibleFnType(type))
      InsertLocFct(tmp);
  }
  return rb;
}

// CGSetEnumerationExpr
// rc1 : AS`SetEnumerationExpr
// vt : CGMAIN`VT
// ==> seq of CPP`Stmt | CPP`Expr
Generic vdmcg::CGSetEnumerationExpr (const TYPE_AS_SetEnumerationExpr & rc1, const TYPE_CGMAIN_VT & vt)
{
  const SEQ<TYPE_AS_Expr> & expr (rc1.GetSequence(pos_AS_SetEnumerationExpr_els));
  const TYPE_CPP_Expr & resVar_v (vt.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & rType (vt.GetRecord(pos_CGMAIN_VT_type));

  if (expr.IsEmpty())
    return GenEmptySetExpr();
  else
  {
    bool casting = !( rType.Is(TAG_TYPE_REP_SetTypeRep) );

    TYPE_CPP_Expr resVar_vq (casting ? GenCastSetTypeForModify(resVar_v) : resVar_v);

    SEQ<TYPE_CPP_Stmt> rb;

    if (casting)
      rb.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, GenEmptySetExpr()));

    bool allsimple = true;
    SEQ<TYPE_CPP_Expr> args;
    size_t len_expr = expr.Length();
    for (size_t i = 1; i <= len_expr; i++)
    {
      const TYPE_AS_Expr & e (expr[i]);

// 20150703 -->
      //Tuple cgee (CGExprExcl(e, ASTAUX::MkId(L"enum_set"), nil));
// 20150714 -->
      //Generic etp (FindSetElemType(rType));
      Generic etp (vdm_CPP_isCPP() ? (Generic)nil : FindSetElemType(rType));
// <-- 20150714
      Tuple cgee (CGExprExcl(e, ASTAUX::MkId(L"enum_set"), etp));
// <-- 20150703
      const TYPE_CPP_Expr & e_v (cgee.GetRecord(1));
      const SEQ<TYPE_CPP_Stmt> & e_stmt (cgee.GetSequence(2));

      rb.ImpConc(e_stmt);
      rb.ImpAppend(GenSetInsert(resVar_vq, e_v));

      if (!e_stmt.IsEmpty() || e.Is(TAG_TYPE_AS_ApplyExpr))
        allsimple = false;
      args.ImpAppend(e_v);
    }

    if (rb.IsEmpty())
      return GenEmptySetExpr();
    else
      if (vdm_CPP_isCPP() && allsimple && (args.Length() <= 8))
        return vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"mk_set")), args);
      else
        return rb;
  }
}

// CGSeqEnumerationExpr
// rc1 : AS`SeqEnumerationExpr
// vt : CGMAIN`VT
// ==> seq of CPP`Stmt | CPP`Expr
Generic vdmcg::CGSeqEnumerationExpr (const TYPE_AS_SeqEnumerationExpr & rc1, const TYPE_CGMAIN_VT & vt)
{
  const SEQ<TYPE_AS_Expr> & expr (rc1.GetSequence(pos_AS_SeqEnumerationExpr_els));

  const TYPE_CPP_Expr & resVar_v (vt.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & rType (vt.GetRecord(pos_CGMAIN_VT_type));

  if (vdm_CPP_isCPP())
  {
    if (expr.IsEmpty())
      return GenEmptySeqExpr();
    else
    {
    bool casting = !( rType.Is(TAG_TYPE_REP_SeqTypeRep) );

    bool isstringexpr = IsSeqType(rType) && IsCharType(FindSeqElemType(rType));
    size_t len_expr = expr.Length();
    for (size_t i = 1; (i <= len_expr) && isstringexpr; i++)
      isstringexpr = expr[i].Is(TAG_TYPE_AS_CharLit);

    if (isstringexpr)
    {
      type_cL s;
      size_t len_expr = expr.Length();
      for (size_t i = 1; i <= len_expr; i++)
      {
        s.ImpAppend(expr[i].GetChar(pos_AS_CharLit_val));
      }
      return vdm_BC_GenFctCall(GenSeq0Type().get_tp(), mk_sequence(vdm_BC_GenStringLit(s)));
    }

    TYPE_CPP_Expr resVar_vq (casting ? GenCastSeqTypeForModify(resVar_v) : resVar_v);

    SEQ<TYPE_CPP_Stmt> rb;

    //if (!expr.IsEmpty() && casting)
    if (casting)
      rb.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, GenEmptySeqExpr()));

    bool allsimple = true;
    SEQ<TYPE_CPP_Expr> args;
    for (size_t i = 1; i <= len_expr; i++)
    {
      const TYPE_AS_Expr & e (expr[i]);

// 20150703 -->
      //Tuple cgee (CGExprExcl(e, ASTAUX::MkId(L"e_seq"), nil));
// 20150714 -->
      //Generic etp (FindSeqElemType(rType));
      Generic etp (vdm_CPP_isCPP() ? (Generic)nil : FindSeqElemType(rType));
// <-- 20150714
      Tuple cgee (CGExprExcl(e, ASTAUX::MkId(L"e_seq"), etp));
// <-- 20150703
      const TYPE_CPP_Expr & tmpExpr (cgee.GetRecord(1));
      const SEQ<TYPE_CPP_Stmt> & st_l (cgee.GetSequence(2));

      rb.ImpConc(st_l);
      rb.ImpAppend(GenImpAppend(resVar_vq, tmpExpr));

      if (!st_l.IsEmpty() || e.Is(TAG_TYPE_AS_ApplyExpr))
        allsimple = false;
      args.ImpAppend(tmpExpr);
    }

    if (rb.IsEmpty())
      return GenEmptySeqExpr();
    else
      if (allsimple && (args.Length() <= 8))
        return vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"mk_sequence")), args);
      else
        return rb;
   }
  }
  else
  { // Java
    bool iswstring = (IsStringType(rType));
    bool possiblestringtype = (IsPossibleStringType(rType));

    bool isstringexpr = true;
    size_t len_expr = expr.Length();
    for (size_t i = 1; (i <= len_expr) && isstringexpr; i++)
      isstringexpr = expr[i].Is(TAG_TYPE_AS_CharLit);

    TYPE_CPP_Expr seqCall ((iswstring || (possiblestringtype && isstringexpr))
      ? GenEmptyStringExpr() : GenEmptySeqExpr());

    if (expr.IsEmpty())
      return seqCall;
    else
    { 
      if (isstringexpr)
      {
        type_cL s;
        size_t len_expr = expr.Length();
        for (size_t i = 1; i <= len_expr; i++)
          s.ImpAppend(expr[i].GetChar(pos_AS_CharLit_val));
        return vdm_BC_GenStringLit(s);
      }
      else if (iswstring || (possiblestringtype && isstringexpr))
      { // string
        TYPE_CPP_Identifier tmpSeq (vdm_BC_GiveName(ASTAUX::MkId(L"tmpSeq")));
        SEQ<TYPE_CPP_Stmt> rb;
        rb.ImpAppend(vdm_BC_GenDecl(GenStringType(), tmpSeq, vdm_BC_GenAsgnInit(seqCall)));
        size_t len_expr = expr.Length();
        for (size_t i = 1; i <= len_expr; i++)
        {
          const TYPE_AS_Expr & e (expr[i]);

          Tuple cgee (CGExprExcl(e, ASTAUX::MkId(L"e_seq"), nil));
          const TYPE_CPP_Expr & tmpExpr (cgee.GetRecord(1));
          const SEQ<TYPE_CPP_Stmt> & st_l (cgee.GetSequence(2));

          rb.ImpConc(st_l);
          rb.ImpAppend(vdm_BC_GenAsgnStmt(tmpSeq, vdm_BC_GenPlus(tmpSeq, tmpExpr)));
        }
        rb.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, tmpSeq));
        return rb;
      }
      else
      {
// 20150807 -->
        bool allsimple = true;
        bool hasstring = false;
        SEQ<TYPE_CPP_Expr> args;
// <-- 20150807
        TYPE_CPP_Identifier tmpSeq (vdm_BC_GiveName(ASTAUX::MkId(L"tmpSeq")));
        SEQ<TYPE_CPP_Stmt> rb;
        rb.ImpAppend(vdm_BC_GenDecl(GenSeq0Type(), tmpSeq, vdm_BC_GenAsgnInit(seqCall)));
        size_t len_expr = expr.Length();
        for (size_t i = 1; i <= len_expr; i++)
        {
          const TYPE_AS_Expr & e (expr[i]);

          Tuple cgee (CGExprExcl(e, ASTAUX::MkId(L"e_seq"), nil));
          const TYPE_CPP_Expr & tmpExpr (cgee.GetRecord(1));
          const SEQ<TYPE_CPP_Stmt> & st_l (cgee.GetSequence(2));

          rb.ImpConc(st_l);
          rb.ImpAppend(GenImpAppend(tmpSeq, tmpExpr));
// 20150807 -->
          if (!st_l.IsEmpty() || e.Is(TAG_TYPE_AS_ApplyExpr))
            allsimple = false;
          args.ImpAppend(tmpExpr);
      
          Generic etp (FindType(e));
          hasstring = IsStringType(etp) || hasstring;
// <-- 20150807
        }
// 20150807 -->

        if (allsimple) {
          TYPE_CPP_ArrayInitializer ai (vdm_BC_GenArrayInitializer(args));
          Generic etp (FindSeqElemType(rType));
          TYPE_CPP_TypeSpecifier ts (IsSeqType(etp) && !IsStringType(etp) && hasstring
                  ? vdm_BC_GenGeneric() : GenType(etp));
          TYPE_CPP_Expr expr (vdm_BC_GenArrayCreationExpr(ts.get_tp(), nil, nil, ai));
            
          TYPE_CPP_Expr res (vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Arrays.asList")),
                                              mk_sequence(expr)));
          InsertImport(SEQ<Char>(L"java.util.Arrays"));
          return res;
        }

// <-- 20150807
        rb.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, tmpSeq));
        return rb;
      }
    }
  }
}

// CGMapEnumerationExpr
// rc1 : AS`MapEnumerationExpr
// vt : CGMAIN`VT
// ==> seq of CPP`Stmt | CPP`Expr
Generic vdmcg::CGMapEnumerationExpr (const TYPE_AS_MapEnumerationExpr & rc1, const TYPE_CGMAIN_VT & vt)
{
  const SEQ<TYPE_AS_Maplet> & ml (rc1.GetSequence(pos_AS_MapEnumerationExpr_els));
  const TYPE_CPP_Expr & resVar_v (vt.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & rType (vt.GetRecord(pos_CGMAIN_VT_type));

  if (ml.IsEmpty ())
    return GenEmptyMapExpr();
  else
  {
    int casting = !IsMapType(rType);

    TYPE_CPP_Expr resVar_vq (casting ? GenCastMapTypeForModify(resVar_v) : resVar_v);

    SEQ<TYPE_CPP_Stmt> rb;

    if (casting)
      rb.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, GenEmptyMapExpr()));

// 20150722 -->
    bool allsimple = true;
    TYPE_CPP_Expr expr (GenEmptyMapExpr());
// <-- 20150722
    size_t len_ml = ml.Length();
    for (size_t i = 1; i <= len_ml; i++)
    {
      const TYPE_AS_Maplet & mlet (ml[i]);
      const TYPE_AS_Expr & md (mlet.GetRecord(pos_AS_Maplet_mapdom));
      const TYPE_AS_Expr & mr (mlet.GetRecord(pos_AS_Maplet_maprng));

// 20150703 -->
      //Tuple cgee (CGExprExcl(md, ASTAUX::MkId(L"tmpVar1"), nil));
// 20150714 -->
      //Generic dtp (FindMapDomType(rType));
      Generic dtp (vdm_CPP_isCPP() ? (Generic)nil : FindMapDomType(rType));
// <-- 20150714
      Tuple cgee (CGExprExcl(md, ASTAUX::MkId(L"tmpVar1"), dtp));
// <-- 20150703
      const TYPE_CPP_Expr & var1_v (cgee.GetRecord(1));
      const SEQ<TYPE_CPP_Stmt> & tmpVar1_stmt (cgee.GetSequence(2));

// 20150703 -->
      //Tuple cgee2 (CGExprExcl(mr, ASTAUX::MkId(L"tmpVar2"), nil));
// 20150714 -->
      //Generic rtp (FindMapRngType(rType));
      Generic rtp (vdm_CPP_isCPP() ? (Generic)nil : FindMapRngType(rType));
// <-- 20150714
      Tuple cgee2 (CGExprExcl(mr, ASTAUX::MkId(L"tmpVar2"), rtp));
// <-- 20150703
      const TYPE_CPP_Expr & var2_v (cgee2.GetRecord(1));
      const SEQ<TYPE_CPP_Stmt> & tmpVar2_stmt (cgee2.GetSequence(2));

      rb.ImpConc(tmpVar1_stmt);
      rb.ImpConc(tmpVar2_stmt);
      rb.ImpAppend(GenMapInsert(resVar_vq, var1_v, var2_v));
// 20150722 -->
      allsimple = tmpVar1_stmt.IsEmpty() && tmpVar2_stmt.IsEmpty() && allsimple;
      if (vdm_CPP_isCPP() && allsimple) {
        expr = GenMapInsertExpr(expr, var1_v, var2_v);
      }
// <-- 20150722
    }
// 20150722 -->
    if (vdm_CPP_isCPP() && allsimple)
      return expr;
// <-- 20150722
    return rb;
  }
}

// CGSetComprehensionExpr
// expr : AS`SetComprehensionExpr
// vt : CGMAIN`VT
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGSetComprehensionExpr (const TYPE_AS_SetComprehensionExpr & expr,
                                                  const TYPE_CGMAIN_VT & vt)
{
  const TYPE_AS_Expr & elem          (expr.GetRecord  (pos_AS_SetComprehensionExpr_elem));
  const SEQ<TYPE_AS_MultBind> & bind (expr.GetSequence(pos_AS_SetComprehensionExpr_bind));
  const Generic & pred               (expr.GetField   (pos_AS_SetComprehensionExpr_pred));

//  TYPE_REP_TypeRep type (FindType(expr));
  MAP<TYPE_AS_Name, TYPE_REP_TypeRep> pid_m (FindPatIdMap(expr));

  PushEnv_CGAUX();
  InsertNamesinEnv_CGAUX(pid_m.Dom());
  PushFctLoc (pid_m.Dom());

// 20150827 -->
  Set seqapply_s;
  if (bind.Length() == 1) {
    const TYPE_AS_MultBind & mb (bind[1]);
    if (mb.Is(TAG_TYPE_AS_MultSetBind)) {
      const TYPE_AS_Name & setexpr (mb.GetRecord(pos_AS_MultSetBind_Set));
      if (IsSeqIndicesSet(setexpr)) {
        const SEQ<TYPE_AS_Pattern> & p_l (mb.GetSequence(pos_AS_MultSetBind_pat));
        size_t len_p_l = p_l.Length();
        for (size_t i = 1; i <= len_p_l; i++) {
          const TYPE_AS_Pattern & p (p_l[i]);
          if (p.Is(TAG_TYPE_AS_PatternName)) {
            const Generic & nm (p.GetField(pos_AS_PatternName_nm));
            if (!nm.IsNil()) {
              seqapply_s.Insert(mk_(vdm_BC_Rename(setexpr.GetRecord(pos_AS_PrefixExpr_arg)), vdm_BC_Rename(nm)));
            }
          }
        }
      }
    }
  }
  AddNoCheckSeqApply(seqapply_s);
// <-- 20150827

  TYPE_CPP_Identifier resS_v (vdm_BC_GiveName(ASTAUX::MkId(L"res_s")));

  Tuple cgee (CGExprExcl(elem, ASTAUX::MkId(L"res_s"), nil));
  const TYPE_CPP_Expr & elem_v (cgee.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & elem_stmt (cgee.GetSequence(2));

// 20150827 -->
  RemNoCheckSeqApply(seqapply_s);
// <-- 20150827

  SEQ<TYPE_CPP_Stmt> todo (Sequence(elem_stmt).ImpAppend(GenSetInsert(resS_v, elem_v)));

  SEQ<TYPE_CPP_Stmt> rb;
  rb.ImpConc (GenDeclEmptySet (resS_v));
  POSITION::SetPosition(expr);

  rb.ImpAppend (CGComprehension (bind, pred, todo, nil, nil, pid_m, true));
  rb.ImpAppend (vdm_BC_GenAsgnStmt (vt.GetRecord(pos_CGMAIN_VT_name), resS_v));

  DeleteLoc();
  PopEnv_CGAUX();

  return rb;
}

// CGMapComprehensionExpr
// expr : AS`MapComprehensionExpr
// vt : CGMAIN`VT
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGMapComprehensionExpr (const TYPE_AS_MapComprehensionExpr & expr,
                                                  const TYPE_CGMAIN_VT & vt)
{
  const TYPE_AS_Maplet & rc3         (expr.GetRecord(pos_AS_MapComprehensionExpr_elem));
  const SEQ<TYPE_AS_MultBind> & bind (expr.GetSequence(pos_AS_MapComprehensionExpr_bind));
  const Generic & pred               (expr.GetField(pos_AS_MapComprehensionExpr_pred));

  const TYPE_AS_Expr & md (rc3.GetRecord(pos_AS_Maplet_mapdom));
  const TYPE_AS_Expr & mr (rc3.GetRecord(pos_AS_Maplet_maprng));

  TYPE_REP_TypeRep type (FindType(expr));
  MAP<TYPE_AS_Name, TYPE_REP_TypeRep> pid_m (FindPatIdMap(expr));

  PushEnv_CGAUX();
  InsertNamesinEnv_CGAUX(pid_m.Dom());
  PushFctLoc(pid_m.Dom());

  TYPE_CPP_Identifier resM_v (vdm_BC_GiveName(ASTAUX::MkId(L"res_m")));

  Tuple cgee (CGExprExcl(md, ASTAUX::MkId(L"md"), nil));
  const TYPE_CPP_Expr & md_v (cgee.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & md_stmt(cgee.GetSequence(2));

  Tuple cgee2 (CGExprExcl(mr, ASTAUX::MkId(L"mr"), nil));
  const TYPE_CPP_Expr & mr_v (cgee2.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & mr_stmt (cgee2.GetSequence(2));

  SEQ<TYPE_CPP_Stmt> todo;
  todo.ImpConc(md_stmt);
  todo.ImpConc(mr_stmt);
  todo.ImpAppend(GenMapInsert(resM_v, md_v, mr_v));

  SEQ<TYPE_CPP_Stmt> rb;
  rb.ImpConc (GenDeclEmptyMap (type, resM_v));
  POSITION::SetPosition(expr);

  rb.ImpAppend (CGComprehension (bind, pred, todo, nil, nil, pid_m, true));
  rb.ImpAppend (vdm_BC_GenAsgnStmt (vt.GetRecord(pos_CGMAIN_VT_name), resM_v));

  DeleteLoc();
  PopEnv_CGAUX();

  return rb;
}

// CGSeqComprehensionExpr
// rc1 : AS`SeqComprehensionExpr
// vt : CGMAIN`VT
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGSeqComprehensionExpr (const TYPE_AS_SeqComprehensionExpr & rc1,
                                                  const TYPE_CGMAIN_VT & vt)
{
  const TYPE_AS_Bind & bind (rc1.GetRecord(pos_AS_SeqComprehensionExpr_bind));

  if (bind.Is(TAG_TYPE_AS_SeqBind)) {
    return CGSeqComprehensionSeqBind(rc1, vt);
  }
  else {
    const TYPE_AS_Pattern & pat (bind.GetRecord(pos_AS_SetBind_pat));
    const TYPE_AS_Expr & expr   (bind.GetRecord(pos_AS_SetBind_Set));

    if (pat.Is(TAG_TYPE_AS_PatternName) && !pat.GetField(pos_AS_PatternName_nm).IsNil() &&
      (IsSeqIndicesSet(expr) ||
       (expr.Is(TAG_TYPE_AS_SetRangeExpr) && IsIntType(FindType(expr.GetRecord(pos_AS_SetRangeExpr_lb)))
                                          && IsIntType(FindType(expr.GetRecord(pos_AS_SetRangeExpr_ub))))))
    {
      return CGSeqComprehensionIndexLoop(rc1, vt);
    } else {
      return CGSeqComprehensionSetBind(rc1, vt);
    }
  }
}

// CGSeqComprehensionExprIndexLoop
// rc1 : AS`SeqComprehensionExpr
// vt : CGMAIN`VT
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGSeqComprehensionIndexLoop (const TYPE_AS_SeqComprehensionExpr & rc1,
                                                       const TYPE_CGMAIN_VT & vt)
{
  const TYPE_AS_Expr & elem (rc1.GetRecord(pos_AS_SeqComprehensionExpr_elem));
  const TYPE_AS_SetBind & bind (rc1.GetRecord(pos_AS_SeqComprehensionExpr_bind));
  const Generic & pred      (rc1.GetField(pos_AS_SeqComprehensionExpr_pred));

//  MAP<TYPE_AS_Name, TYPE_REP_TypeRep> pid_m (FindPatIdMap(rc1)); //
  const TYPE_CPP_Expr & resVar_v (vt.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & rType (vt.GetRecord(pos_CGMAIN_VT_type));

  const TYPE_AS_Pattern & pat (bind.GetRecord(pos_AS_SetBind_pat));
  const TYPE_AS_Expr & expr   (bind.GetRecord(pos_AS_SetBind_Set));

  const TYPE_AS_Name & nm (pat.GetRecord(pos_AS_PatternName_nm));
    
  TYPE_CPP_Identifier tmpSeq_v (vdm_BC_GiveName(ASTAUX::MkId(L"tmpSeq")));
  TYPE_CPP_Identifier count (vdm_BC_GiveName(ASTAUX::MkId(L"count")));

  Set seqapply_s;
  SEQ<TYPE_CPP_Stmt> stmts;
  TYPE_CPP_Expr lb;
  TYPE_CPP_Expr ub;
  if (expr.Is(TAG_TYPE_AS_PrefixExpr))
  {
    // in set inds l
    const TYPE_AS_Expr & arg (expr.GetRecord(pos_AS_PrefixExpr_arg));

    Tuple bl_cgee (CGExprExcl(arg, ASTAUX::MkId(L"bind_l"), nil));
    const TYPE_CPP_Expr & bl_expr (bl_cgee.GetRecord(1));
    const SEQ<TYPE_CPP_Stmt> & bl_stmt (bl_cgee.GetSequence(2));

    stmts.ImpConc(bl_stmt);

    TYPE_REP_TypeRep argtype (FindType(arg));

    lb = vdm_BC_GenIntegerLit(1);

    TYPE_CPP_Expr bl_cast (IsSeqType(argtype) ? bl_expr
                                              : vdm_CPP_isCPP() ? GenCastSeq(bl_expr, Nil())
                                                                : IsStringType(argtype) ? GenCastString(bl_expr)
                                                                                        : GenCastSeq(bl_expr, Nil()));
    ub = (vdm_CPP_isCPP() ? GenLen_int(bl_cast)
                          : IsPossibleStringType(argtype) ? GenLenString_int(bl_cast)
                                                          : GenLen_int(bl_cast));

    seqapply_s.Insert(mk_(bl_expr, vdm_BC_Rename(nm)));
  }
  else
  {
    // in set {lb,...,ub}
    const TYPE_AS_Expr & srlb (expr.GetRecord(pos_AS_SetRangeExpr_lb));
    const TYPE_AS_Expr & srub (expr.GetRecord(pos_AS_SetRangeExpr_ub));

    if (srlb.Is(TAG_TYPE_AS_RealLit))
    {
      lb = vdm_BC_GenIntegerLit(srlb.GetReal(pos_AS_RealLit_val));
    }
    else if (srlb.Is(TAG_TYPE_AS_NumLit))
    {
      lb = vdm_BC_GenIntegerLit(srlb.GetReal(pos_AS_NumLit_val));
    }
    else
    {
      Tuple lb_cgee (CGExprExcl(srlb, ASTAUX::MkId(L"lb"), nil));
      const TYPE_CPP_Expr & lb_expr (lb_cgee.GetRecord(1));
      const SEQ<TYPE_CPP_Stmt> & lb_stmt (lb_cgee.GetSequence(2));
      stmts.ImpConc(lb_stmt);
      lb = GenGetValue(lb_expr, mk_REP_NumericTypeRep(Int(INTEGER)));
    }
   
    if (srub.Is(TAG_TYPE_AS_RealLit))
    {
      ub = vdm_BC_GenIntegerLit(srub.GetReal(pos_AS_RealLit_val));
    }
    else if (srub.Is(TAG_TYPE_AS_NumLit))
    {
      ub = vdm_BC_GenIntegerLit(srub.GetReal(pos_AS_NumLit_val));
    }
    else
    {
      Tuple ub_cgee (CGExprExcl(srub, ASTAUX::MkId(L"ub"), nil));
      const TYPE_CPP_Expr & ub_expr (ub_cgee.GetRecord(1));
      const SEQ<TYPE_CPP_Stmt> & ub_stmt (ub_cgee.GetSequence(2));
      stmts.ImpConc(ub_stmt);
      ub = GenGetValue(ub_expr, mk_REP_NumericTypeRep(Int(INTEGER)));
    }
  }
  AddNoCheckSeqApply(seqapply_s);

  PushEnv_CGAUX();
  InsertName_CGAUX(nm);

  Tuple cgee (CGExprExcl(elem, ASTAUX::MkId(L"reselem"), nil));
  const TYPE_CPP_Expr & elemExpr (cgee.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & e_stmt (cgee.GetSequence(2));

  SEQ<TYPE_CPP_Stmt> elemStmt (e_stmt);
  TYPE_CPP_Expr append (vdm_CPP_isCPP() ? GenImpAppend(tmpSeq_v, elemExpr)
                                        : IsPossibleStringType(rType)
                                             ? vdm_BC_GenAsgnStmt(tmpSeq_v, vdm_BC_GenPlus(tmpSeq_v, elemExpr))
                                             : GenImpAppend(tmpSeq_v, elemExpr));
  elemStmt.ImpAppend(append);

  SEQ<TYPE_CPP_Stmt> succbody;
  if (pred.IsNil() || (pred.Is(TAG_TYPE_AS_BoolLit) && ((Record) pred).GetBoolValue(pos_AS_BoolLit_val)))
  {
    succbody.ImpConc(elemStmt);
  }
  else
  {
    Generic predType (FindType (pred));
    Tuple cgee (CGExprExcl(pred, ASTAUX::MkId(L"pred"), nil));
    const TYPE_CPP_Expr & pred_v (cgee.GetRecord(1));
    const SEQ<TYPE_CPP_Stmt> & pred_stmt (cgee.GetSequence(2));

    TYPE_CPP_Expr cond (vdm_CPP_isCPP() ? GenGetValue(pred_v, predType)
                                        : GenGetValue(pred_v, mk_REP_BooleanTypeRep()));

    succbody.ImpConc(pred_stmt);
    succbody.ImpAppend(vdm_BC_GenIfStmt(cond, vdm_BC_GenBlock(elemStmt), nil));
  }

  RemNoCheckSeqApply(seqapply_s);

  TYPE_CPP_Stmt ini (vdm_BC_GenDecl(GenSmallNumType(), count, vdm_BC_GenAsgnInit(lb)));
  TYPE_CPP_Expr e1 (vdm_BC_GenLeq(count, ub));
  TYPE_CPP_Expr e2 (vdm_BC_GenPostPlusPlus(count));

  TYPE_CPP_Initializer initExpr;
  if (vdm_CPP_isCPP())
    initExpr = vdm_BC_GenObjectInit(mk_sequence(count));
  else
    initExpr = vdm_BC_GenAsgnInit(GenIntExpr(count));

  SEQ<TYPE_CPP_Stmt> inner_rb_l;
  inner_rb_l.ImpConc(GenDecl_DS(mk_REP_NumericTypeRep(Int(NAT)), vdm_BC_Rename(nm), initExpr));
  inner_rb_l.ImpConc(succbody);

  SEQ<TYPE_CPP_Stmt> decl;
  if (vdm_CPP_isCPP())
    decl.ImpConc(GenDeclEmptySeq (tmpSeq_v));
  else
  { // java
    if (IsPossibleStringType(rType))
      decl.ImpConc(GenStringDecl(tmpSeq_v, vdm_BC_GenAsgnInit(GenEmptyStringExpr())));
    else
      decl.ImpConc(GenDeclEmptySeq (tmpSeq_v));
  }

  SEQ<TYPE_CPP_Stmt> rb_l;
  rb_l.ImpConc(stmts);
  rb_l.ImpConc(decl);
  rb_l.ImpAppend(vdm_BC_GenForStmt(ini, e1, type_dL().ImpAppend(e2), vdm_BC_GenBlock(inner_rb_l)));
  rb_l.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, tmpSeq_v));

  PopEnv_CGAUX();
  return rb_l;
}

// CGSeqComprehensionSetBind
// rc1 : AS`SeqComprehensionExpr
// vt : CGMAIN`VT
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGSeqComprehensionSetBind (const TYPE_AS_SeqComprehensionExpr & rc1,
                                                     const TYPE_CGMAIN_VT & vt)
{
  const TYPE_AS_Expr & elem (rc1.GetRecord(pos_AS_SeqComprehensionExpr_elem));
  const TYPE_AS_SetBind & bind (rc1.GetRecord(pos_AS_SeqComprehensionExpr_bind));
  const Generic & pred      (rc1.GetField(pos_AS_SeqComprehensionExpr_pred));

  const TYPE_AS_Pattern & pat (bind.GetRecord(pos_AS_SetBind_pat));
  const TYPE_AS_Expr & expr   (bind.GetRecord(pos_AS_SetBind_Set));

  MAP<TYPE_AS_Name, TYPE_REP_TypeRep> pid_m (FindPatIdMap(rc1)); //
  const TYPE_CPP_Expr & resVar_v (vt.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & rType (vt.GetRecord(pos_CGMAIN_VT_type));

  PushEnv();
  PushFctLoc (pid_m.Dom());

  TYPE_REP_SetTypeRep exprType (FindType (expr)); //
  TYPE_CPP_Identifier resL_v (vdm_BC_GiveName(ASTAUX::MkId(L"res_l")));
  TYPE_CPP_Identifier bindL_v (vdm_BC_GiveName(ASTAUX::MkId(L"bind_l")));
  TYPE_REP_SeqTypeRep bindLType (mk_REP_SeqTypeRep (exprType.get_elemtp()));
  TYPE_REP_TypeRep resSType (exprType);
  TYPE_CPP_Identifier succ_v (vdm_BC_GiveName(ASTAUX::MkId(L"succ")));

  Generic predType;
  if (!pred.IsNil())
    predType = FindType (pred);

  TYPE_CPP_Identifier e_v (vdm_BC_GiveName(ASTAUX::MkId(L"e")));
  TYPE_REP_TypeRep eType (CleanFlatType(exprType.GetRecord (pos_REP_SetTypeRep_elemtp)));

  TYPE_CPP_Identifier sort_f (vdm_CPP_isCPP() ? vdm_BC_GenIdentifier (ASTAUX::MkId(L"CGUTIL::Sort"))
                                              : vdm_BC_GenIdentifier (ASTAUX::MkId(L"UTIL.Sort")));

  Tuple lb_cgee (CGExprExcl(expr, ASTAUX::MkId(L"resBind_s"), resSType));
  const TYPE_CPP_Expr & rb_expr (lb_cgee.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & rb_stmt (lb_cgee.GetSequence(2));

  SEQ<TYPE_CPP_Stmt> rb;

  rb.ImpConc(rb_stmt);

  TYPE_CPP_Expr resS_v = rb_expr;
  if (!rb_expr.Is(TAG_TYPE_CPP_Identifier))
  {
    resS_v = vdm_BC_GiveName(ASTAUX::MkId(L"resBind_s"));
    rb.ImpConc (GenDecl_DS (resSType, resS_v, vdm_BC_GenAsgnInit(rb_expr)));
  }

  PushEnv_CGAUX();
  InsertNamesinEnv_CGAUX(pid_m.Dom());

  Tuple cgee (CGExprExcl(elem, ASTAUX::MkId(L"reselem"), nil));
  const TYPE_CPP_Expr & elemExpr (cgee.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & e_stmt (cgee.GetSequence(2));

  TYPE_CPP_Stmt impappend (GenImpAppend(resL_v, elemExpr));
  SEQ<TYPE_CPP_Stmt> elemStmt (e_stmt);
  elemStmt.ImpAppend(impappend);

  SEQ<TYPE_CPP_Stmt> decls (DeclPatVars(pid_m));

  SEQ<TYPE_CPP_Stmt> succbody;
  if (pred.IsNil() || (pred.Is(TAG_TYPE_AS_BoolLit) && ((Record) pred).GetBoolValue(pos_AS_BoolLit_val)))
  {
    succbody.ImpConc(elemStmt);
  }
  else
  {
    Tuple cgee (CGExprExcl(pred, ASTAUX::MkId(L"pred"), nil));
    const TYPE_CPP_Expr & pred_v (cgee.GetRecord(1));
    const SEQ<TYPE_CPP_Stmt> & pred_stmt (cgee.GetSequence(2));

    TYPE_CPP_Expr cond (vdm_CPP_isCPP() ? GenGetValue(pred_v, predType)
                                        : GenGetValue(pred_v, mk_REP_BooleanTypeRep()));

    succbody.ImpConc(pred_stmt);
    succbody.ImpAppend(vdm_BC_GenIfStmt(cond, vdm_BC_GenBlock(elemStmt), nil));
  }

  Tuple cgpme (CGPatternMatchExcl(pat, mk_CG_VT(e_v, eType), eset, succ_v, pid_m, succbody, false));
  const SEQ<TYPE_CPP_Stmt> & pm (cgpme.GetSequence(1));
  bool Is_excl (cgpme.GetBoolValue(2)); // false : need to check pattern match failed

  SEQ<TYPE_CPP_Stmt> pm_q;
  if (! Is_excl) {
    pm_q.ImpAppend(vdm_BC_GenDecl(GenSmallBoolType(), succ_v,
                                  vdm_BC_GenAsgnInit( vdm_BC_GenBoolLit(false))));
  }
  pm_q.ImpConc(pm);

  SEQ<TYPE_CPP_Stmt> todo;
  if (!IsIntType (eType))
    todo.ImpAppend (vdm_BC_GenIfStmt (vdm_BC_GenNot (GenIsInt (e_v)),
                vdm_BC_GenBlock(mk_sequence(RunTime (L"The binding set must only contain integers"))),
                                      nil));
  todo.ImpConc(pm_q);

  TYPE_CPP_Expr cast;
  if (vdm_CPP_isCPP())
    cast = resL_v;
  else
  {
    if (IsSubType(bindLType, rType))
      cast = resL_v;
    else
      cast = GenExplicitCast(rType, resL_v, bindLType);
  }

  SEQ<TYPE_CPP_Stmt> rb_inner;
  rb_inner.ImpConc (GenDecl_DS (bindLType, bindL_v,
                                  vdm_BC_GenAsgnInit(vdm_BC_GenFctCall (sort_f, mk_sequence(resS_v)))));
  rb_inner.ImpConc(decls); // 
  rb_inner.ImpConc(GenIterSeq(mk_CG_VT(bindL_v, bindLType), nil, mk_CG_VT(e_v, eType), todo));

  rb.ImpConc (GenDeclEmptySeq (resL_v));
  rb.ImpAppend (vdm_BC_GenIfStmt (vdm_BC_GenNot(GenSetIsEmpty(resS_v)), vdm_BC_GenBlock(rb_inner), nil));

  rb.ImpAppend (vdm_BC_GenAsgnStmt(resVar_v, cast));

  PopEnv();
  DeleteLoc();
  PopEnv_CGAUX();

  SEQ<TYPE_CPP_Stmt> res_s;
  res_s.ImpAppend(vdm_BC_GenBlock(rb));
  return res_s;
}

// CGSeqComprehensionSeqBind
// rc1 : AS`SeqComprehensionExpr
// vt : CGMAIN`VT
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGSeqComprehensionSeqBind (const TYPE_AS_SeqComprehensionExpr & rc1,
                                                     const TYPE_CGMAIN_VT & vt)
{
  const TYPE_AS_Expr & elem (rc1.GetRecord(pos_AS_SeqComprehensionExpr_elem));
  const TYPE_AS_SeqBind & bind (rc1.GetRecord(pos_AS_SeqComprehensionExpr_bind));
  const Generic & pred      (rc1.GetField(pos_AS_SeqComprehensionExpr_pred));

  const TYPE_AS_Pattern & pat (bind.GetRecord(pos_AS_SeqBind_pat));
  const TYPE_AS_Expr & expr   (bind.GetRecord(pos_AS_SeqBind_Seq));

  MAP<TYPE_AS_Name, TYPE_REP_TypeRep> pid_m (FindPatIdMap(rc1)); //
  const TYPE_CPP_Expr & resVar_v (vt.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & rType (vt.GetRecord(pos_CGMAIN_VT_type));

  PushEnv();
  PushFctLoc (pid_m.Dom());

  TYPE_REP_SetTypeRep exprType (FindType (expr)); //
  TYPE_CPP_Identifier resL_v (vdm_BC_GiveName(ASTAUX::MkId(L"res_l")));
  TYPE_CPP_Identifier bindL_v (vdm_BC_GiveName(ASTAUX::MkId(L"bind_l")));
  TYPE_REP_SeqTypeRep bindLType (exprType);
  TYPE_REP_TypeRep resSType (exprType);
  TYPE_CPP_Identifier succ_v (vdm_BC_GiveName(ASTAUX::MkId(L"succ")));

  Generic predType;
  if (!pred.IsNil())
    predType = FindType (pred);

  TYPE_CPP_Identifier e_v (vdm_BC_GiveName(ASTAUX::MkId(L"e")));
  TYPE_REP_TypeRep eType (CleanFlatType(exprType.GetRecord (pos_REP_SetTypeRep_elemtp)));

  Tuple lb_cgee (CGExprExcl(expr, ASTAUX::MkId(L"resBind_s"), resSType));
  const TYPE_CPP_Expr & rb_expr (lb_cgee.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & rb_stmt (lb_cgee.GetSequence(2));

  SEQ<TYPE_CPP_Stmt> rb;

  rb.ImpConc(rb_stmt);

  TYPE_CPP_Expr resS_v = rb_expr;
  if (!rb_expr.Is(TAG_TYPE_CPP_Identifier))
  {
    resS_v = vdm_BC_GiveName(ASTAUX::MkId(L"resBind_s"));
    rb.ImpConc (GenDecl_DS (resSType, resS_v, vdm_BC_GenAsgnInit(rb_expr)));
  }

  PushEnv_CGAUX();
  InsertNamesinEnv_CGAUX(pid_m.Dom());

  Tuple cgee (CGExprExcl(elem, ASTAUX::MkId(L"reselem"), nil));
  const TYPE_CPP_Expr & elemExpr (cgee.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & e_stmt (cgee.GetSequence(2));

  TYPE_CPP_Stmt impappend (GenImpAppend(resL_v, elemExpr));
  SEQ<TYPE_CPP_Stmt> elemStmt (e_stmt);
  elemStmt.ImpAppend(impappend);

  SEQ<TYPE_CPP_Stmt> decls (DeclPatVars(pid_m));

  SEQ<TYPE_CPP_Stmt> succbody;
  if (pred.IsNil() || (pred.Is(TAG_TYPE_AS_BoolLit) && ((Record) pred).GetBoolValue(pos_AS_BoolLit_val)))
  {
    succbody.ImpConc(elemStmt);
  }
  else
  {
    Tuple cgee (CGExprExcl(pred, ASTAUX::MkId(L"pred"), nil));
    const TYPE_CPP_Expr & pred_v (cgee.GetRecord(1));
    const SEQ<TYPE_CPP_Stmt> & pred_stmt (cgee.GetSequence(2));

    TYPE_CPP_Expr cond (vdm_CPP_isCPP() ? GenGetValue(pred_v, predType)
                                        : GenGetValue(pred_v, mk_REP_BooleanTypeRep()));

    succbody.ImpConc(pred_stmt);
    succbody.ImpAppend(vdm_BC_GenIfStmt(cond, vdm_BC_GenBlock(elemStmt), nil));
  }

  Tuple cgpme (CGPatternMatchExcl(pat, mk_CG_VT(e_v, eType), eset, succ_v, pid_m, succbody, false));
  const SEQ<TYPE_CPP_Stmt> & pm (cgpme.GetSequence(1));
  bool Is_excl (cgpme.GetBoolValue(2)); // false : need to check pattern match failed

  SEQ<TYPE_CPP_Stmt> pm_q;
  if (! Is_excl) {
    pm_q.ImpAppend(vdm_BC_GenDecl(GenSmallBoolType(), succ_v,
                                  vdm_BC_GenAsgnInit( vdm_BC_GenBoolLit(false))));
  }
  pm_q.ImpConc(pm);

  SEQ<TYPE_CPP_Stmt> todo;
  todo.ImpConc(MergeStmts( decls, pm_q));

  TYPE_CPP_Expr cast;
  if (vdm_CPP_isCPP())
    cast = resL_v;
  else
  {
    if (IsSubType(bindLType, rType))
      cast = resL_v;
    else
      cast = GenExplicitCast(rType, resL_v, bindLType);
  }

  SEQ<TYPE_CPP_Stmt> rb_inner;
  rb_inner.ImpConc (GenDecl_DS (bindLType, bindL_v, vdm_BC_GenAsgnInit(resS_v)));
  rb_inner.ImpConc(GenIterSeq(mk_CG_VT(bindL_v, bindLType), nil, mk_CG_VT(e_v, eType), todo));
  rb.ImpConc (GenDeclEmptySeq (resL_v));
  rb.ImpAppend (vdm_BC_GenIfStmt (vdm_BC_GenNot(GenSeqIsEmpty(resS_v)), vdm_BC_GenBlock(rb_inner), nil));
  rb.ImpAppend (vdm_BC_GenAsgnStmt(resVar_v, cast));

  PopEnv();
  DeleteLoc();
  PopEnv_CGAUX();

  SEQ<TYPE_CPP_Stmt> res_s;
  res_s.ImpAppend(vdm_BC_GenBlock(rb));
  return res_s;
}

// CGSetRangeExpr
// expr : AS`SetRangeExpr
// vt : CGMAIN`VT
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGSetRangeExpr(const TYPE_AS_SetRangeExpr & expr, const TYPE_CGMAIN_VT & vt)
{
  const TYPE_AS_Expr & lb (expr.GetRecord(pos_AS_SetRangeExpr_lb));
  const TYPE_AS_Expr & ub (expr.GetRecord(pos_AS_SetRangeExpr_ub));

  const TYPE_CPP_Expr & resVar_v (vt.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & rType (vt.GetRecord(pos_CGMAIN_VT_type));

  int casting = ! IsSetType( rType );

  Tuple cgee (CGExprExcl(lb, ASTAUX::MkId(L"lb"), nil));
  const TYPE_CPP_Expr & lb_v (cgee.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & lb_stmt(cgee.GetSequence(2));

  Tuple cgee2 (CGExprExcl(ub, ASTAUX::MkId(L"ub"), nil));
  const TYPE_CPP_Expr & ub_v (cgee2.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & ub_stmt(cgee2.GetSequence(2));

  Generic lbt (FindType(lb));
  Generic ubt (FindType(ub));

  TYPE_CPP_Identifier count (vdm_BC_GiveName(ASTAUX::MkId(L"count")));

  TYPE_REP_TypeRep irep (mk_REP_NumericTypeRep(Int(INTEGER)));
  TYPE_CPP_TypeSpecifier its (GenIntType());
  TYPE_CPP_TypeSpecifier sint (GenSmallNumType());

  SEQ<TYPE_CPP_Stmt> rb_l;
  rb_l.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, GenEmptySetExpr()));
  rb_l.ImpConc(lb_stmt);
  rb_l.ImpConc(ub_stmt);

  TYPE_CPP_Expr lbi_v (vdm_BC_GiveName(ASTAUX::MkId(L"lbi")));
  if (! IsIntType(lbt))
  {
    TYPE_CPP_Expr lb_vq (vdm_BC_GiveName(ASTAUX::MkId(L"lb")));
    if (lb_v.Is(TAG_TYPE_CPP_Identifier) || IsIntExpr(lb_v))
      lb_vq = lb_v;
    else
      rb_l.ImpConc(GenConstDeclInit(lbt, lb_vq, lb_v));

    TYPE_CPP_Expr cond (vdm_BC_GenNot(GenIsInt(lb_vq)));
    TYPE_CPP_Stmt rti (RunTime(L"Lower bound was not an integer in set range expression"));
    //rb_l.ImpAppend(vdm_BC_GenIfStmt(cond, rti, nil));
    rb_l.ImpAppend(vdm_BC_GenIfStmt(cond, vdm_BC_GenBlock(mk_sequence(rti)), nil));

    TYPE_CPP_Expr lbval (vdm_CPP_isCPP() ? GenGetValue(vdm_BC_GenCastExpr(its, lb_vq), irep)
                                         : GenGetValue(GenExplicitCast(irep, lb_vq, lbt), irep));

    rb_l.ImpAppend(vdm_BC_GenDecl(sint, lbi_v, vdm_BC_GenAsgnInit(lbval)));
  }
  else
  {
    TYPE_CPP_Expr lb_vq (GenGetValue(lb_v, irep));
    if (lb_vq.Is(TAG_TYPE_CPP_IntegerLit))
      lbi_v = lb_vq;
    else
      rb_l.ImpAppend(vdm_BC_GenDecl(sint, lbi_v, vdm_BC_GenAsgnInit(lb_vq)));
  }

  TYPE_CPP_Expr ubi_v (vdm_BC_GiveName(ASTAUX::MkId(L"ubi")));
  if (! IsIntType(ubt))
  {
    TYPE_CPP_Expr ub_vq (vdm_BC_GiveName(ASTAUX::MkId(L"ub")));
    if (ub_v.Is(TAG_TYPE_CPP_Identifier) || IsIntExpr(ub_v))
      ub_vq = ub_v;
    else
      rb_l.ImpConc(GenConstDeclInit(ubt, ub_vq, ub_v));

    TYPE_CPP_Expr cond (vdm_BC_GenNot(GenIsInt(ub_vq)));
    TYPE_CPP_Stmt rti (RunTime(L"Upper bound was not an integer in set range expression"));
    //rb_l.ImpAppend(vdm_BC_GenIfStmt(cond, rti, nil));
    rb_l.ImpAppend(vdm_BC_GenIfStmt(cond, vdm_BC_GenBlock(mk_sequence(rti)), nil));

    TYPE_CPP_Expr ubval (vdm_CPP_isCPP() ? GenGetValue(vdm_BC_GenCastExpr(its, ub_vq), irep)
                                         : GenGetValue(GenExplicitCast(irep, ub_vq, ubt), irep));

    rb_l.ImpAppend(vdm_BC_GenDecl(sint, ubi_v, vdm_BC_GenAsgnInit(ubval)));
  }
  else
  {
    TYPE_CPP_Expr ub_vq (GenGetValue(ub_v, irep));
    if (ub_vq.Is(TAG_TYPE_CPP_IntegerLit))
      ubi_v = ub_vq;
    else
      rb_l.ImpAppend(vdm_BC_GenDecl(sint, ubi_v, vdm_BC_GenAsgnInit(ub_vq)));
  }

  TYPE_CPP_Stmt ini (vdm_BC_GenDecl(sint, count, vdm_BC_GenAsgnInit(lbi_v)));
  TYPE_CPP_Expr e1 (vdm_BC_GenLeq(count, ubi_v));
  TYPE_CPP_Expr e2 (vdm_BC_GenPostPlusPlus(count));
  TYPE_CPP_Expr cast (GenIntExpr(count));

  TYPE_CPP_Stmt stmt (casting
        ? (vdm_CPP_isCPP() ? vdm_BC_GenAsgnStmt(resVar_v, GenSetInsertExpr(GenSetExpr(resVar_v), cast))
                           : vdm_BC_GenExpressionStmt(GenSetInsertExpr(GenCastSetType(resVar_v), cast)))
        : GenSetInsert(resVar_v, cast));

  //rb_l.ImpAppend(vdm_BC_GenForStmt(ini, e1, type_dL().ImpAppend(e2), stmt));
  rb_l.ImpAppend(vdm_BC_GenForStmt(ini, e1, type_dL().ImpAppend(e2), vdm_BC_GenBlock(mk_sequence(stmt))));
  return rb_l;
}

// CGSubSeqExpr
// expr : AS`SubSequenceExpr
// vt : CGMAIN`VT
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGSubSeqExpr(const TYPE_AS_SubSequenceExpr & expr, const TYPE_CGMAIN_VT & vt)
{
  const TYPE_AS_Expr & sq (expr.GetRecord(pos_AS_SubSequenceExpr_sequence));
  const TYPE_AS_Expr & n1 (expr.GetRecord(pos_AS_SubSequenceExpr_frompos));
  const TYPE_AS_Expr & n2 (expr.GetRecord(pos_AS_SubSequenceExpr_topos));

  const TYPE_CPP_Expr & resVar_v (vt.GetRecord(pos_CGMAIN_VT_name));

  TYPE_CPP_Identifier resSeq (vdm_BC_GiveName(ASTAUX::MkId(L"resSeq")));
  TYPE_CPP_Expr n1_v (vdm_BC_GiveName(ASTAUX::MkId(L"n1")));
  TYPE_CPP_Expr n2_v (vdm_BC_GiveName(ASTAUX::MkId(L"n2")));
  TYPE_CPP_Identifier From (vdm_BC_GiveName(ASTAUX::MkId(L"from")));
  TYPE_CPP_Identifier To (vdm_BC_GiveName(ASTAUX::MkId(L"to")));
  TYPE_CPP_Identifier Max (vdm_BC_GiveName(ASTAUX::MkId(L"max")));
  TYPE_CPP_Identifier i (vdm_BC_GiveName(ASTAUX::MkId(L"i")));
  Generic sqt (FindType(sq));
  TYPE_REP_TypeRep rrep (mk_REP_NumericTypeRep(Int(REAL)));
  TYPE_REP_TypeRep irep (mk_REP_NumericTypeRep(Int(INTEGER)));
  TYPE_CPP_TypeSpecifier sint (GenSmallIntType());

  Tuple cgee (CGExprExcl(sq, ASTAUX::MkId(L"tmpSeq"), nil));
  const TYPE_CPP_Expr & tmpSeq1(cgee.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & sq_stmt(cgee.GetSequence(2));

  TYPE_CPP_Expr tmpSeq (tmpSeq1);

  SEQ<TYPE_CPP_Stmt> rb_l;
  rb_l.ImpConc(sq_stmt);

  if (! IsSeqType(sqt)) {
    TYPE_CPP_Identifier tmpGSeq (vdm_BC_GiveName(ASTAUX::MkId(L"tmpGSeq")));
    rb_l.ImpConc(GenDecl_DS(sqt, tmpGSeq, vdm_BC_GenAsgnInit(tmpSeq1)));
    tmpSeq = tmpGSeq;
    TYPE_CPP_Expr cond (vdm_BC_GenNot(GenIsSeq(tmpSeq)));
    TYPE_CPP_Stmt rti(vdm_BC_GenBlock(mk_sequence(RunTime(L"A sequence was expected in subsequence expression"))));
    rb_l.ImpAppend(vdm_BC_GenIfStmt(cond, rti, nil));
    tmpSeq = GenCastSeq(tmpSeq, sqt);
  }

  Tuple cgee2 (CGExprExcl(n1, ASTAUX::MkId(L"n1"), nil));
  const TYPE_CPP_Expr & n1_expr_v (cgee2.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & n1_stmt (cgee2.GetSequence(2));

  Tuple cgee3 (CGExprExcl(n2, ASTAUX::MkId(L"n2"), nil));
  const TYPE_CPP_Expr & n2_expr_v (cgee3.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & n2_stmt (cgee3.GetSequence(2));

  rb_l.ImpConc(n1_stmt).ImpConc(n2_stmt);

  n1_v = n1_expr_v;
  n2_v = n2_expr_v;

  if (vdm_CPP_isJAVA())
  {
    bool isStringType = IsStringType(sqt) || IsPossibleStringType(sqt);
    Generic n1tp (FindType(n1));
    Generic n2tp (FindType(n2));

    TYPE_CPP_Expr n1value;
    if (IsNumType(n1tp)) {
      if (IsIntExpr(n1_v))
        n1value = GenGetValue(n1_v, irep);
      else
        n1value = GenGetValue(n1_v, rrep);
    }
    else {
      TYPE_CPP_Identifier From2 (vdm_BC_GiveName(ASTAUX::MkId(L"from")));
      rb_l.ImpConc(GenDecl_DS(n1tp, From2, vdm_BC_GenAsgnInit(n1_v)));
      rb_l.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenNot(GenIsReal(From2)),
                           vdm_BC_GenBlock(mk_sequence(RunTime (L"A number was expected"))), nil));
      n1_v = From2;
      n1value = GenGetValue(GenExplicitCast(rrep, n1_v, n1tp), rrep);
    }

    TYPE_CPP_Expr n1intvalue;
    if(IsIntType(n1tp)) {
      n1intvalue = n1value;
    }
    else {
      n1intvalue = vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Math.ceil")), mk_sequence(n1value));
    }

    TYPE_CPP_Expr n2value;
    if (IsNumType(n2tp)) {
      if (IsIntExpr(n2_v))
        n2value = GenGetValue(n2_v, irep);
      else
        n2value = GenGetValue(n2_v, rrep);
    }
    else {
      TYPE_CPP_Identifier To2 (vdm_BC_GiveName(ASTAUX::MkId(L"to")));
      rb_l.ImpConc(GenDecl_DS(n2tp, To2, vdm_BC_GenAsgnInit(n2_v)));
      rb_l.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenNot(GenIsReal(To2)),
                             vdm_BC_GenBlock(mk_sequence(RunTime (L"A number was expected"))), nil));
      n2_v = To2;
      n2value = GenGetValue(GenExplicitCast(rrep, n2_v, n2tp), rrep);
    }

    TYPE_CPP_Expr n2intvalue;
    if (IsIntType(n2tp))
      n2intvalue = n2value;
    else
      n2intvalue = vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Math.floor")), mk_sequence(n2value));

    SEQ<TYPE_CPP_Expr> seq1;
// 20120306 -->
    if (n1intvalue.Is(TAG_TYPE_CPP_IntegerLit))
    {
      Int n (n1intvalue.GetInt(pos_CPP_IntegerLit_val));
      n1intvalue.SetField(pos_CPP_IntegerLit_val, Int(n.GetValue() - 1));
      seq1.ImpAppend(n1intvalue);
    }
    else
      seq1.ImpAppend(vdm_BC_GenMinus(n1intvalue, vdm_BC_GenIntegerLit(1)));
// <-- 20120306
    seq1.ImpAppend(vdm_BC_GenIntegerLit(0));
    TYPE_CPP_Expr ai1max (vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Math.max")), seq1));

    TYPE_AS_Id sizelen (ASTAUX::MkId(isStringType ? L"length" : L"size"));
    TYPE_CPP_Stmt ai2final (vdm_BC_GenFctCallObjMemAcc(tmpSeq, sizelen, SEQ<TYPE_CPP_Expr>()));

    SEQ<TYPE_CPP_Expr> seq2;
    seq2.ImpAppend(n2intvalue);
    seq2.ImpAppend(ai2final);

    TYPE_CPP_Expr ai2min (vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Math.min")), seq2));

    TYPE_CPP_AsgnInit ai1 (vdm_BC_GenAsgnInit(vdm_BC_GenCastExpr(
                                                vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"int"))),
                                              ai1max)));
    TYPE_CPP_AsgnInit ai2 (vdm_BC_GenAsgnInit(vdm_BC_GenCastExpr(
                                                vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"int"))),
                                              ai2min)));

    rb_l.ImpAppend(vdm_BC_GenDecl(sint, From, ai1));
    rb_l.ImpAppend(vdm_BC_GenDecl(sint, To, ai2));

    SEQ<TYPE_CPP_Expr> seq3;
    seq3.ImpAppend(From);
    seq3.ImpAppend(To);
    TYPE_AS_Id substrlist (ASTAUX::MkId(isStringType ? L"substring" : L"subList"));
    TYPE_CPP_Expr fcall (vdm_BC_GenFctCallObjMemAcc(tmpSeq, substrlist, seq3));
    TYPE_CPP_Expr classcr (isStringType ? fcall : GenSeqExpr(fcall));
    TYPE_CPP_Expr emptySeq;
    if(isStringType)
      emptySeq = vdm_BC_GenStringLit(ASTAUX::MkId(L""));
    else
      emptySeq = GenEmptySeqExpr();
    // This should be inside GenEmptySeq !!!

    TYPE_CPP_Stmt thenstmt (vdm_BC_GenAsgnStmt(resVar_v, emptySeq));
    TYPE_CPP_Stmt elsestmt (vdm_BC_GenAsgnStmt(resVar_v, classcr));
    TYPE_CPP_Expr cond (vdm_BC_GenGt(From, To));
    rb_l.ImpAppend(vdm_BC_GenIfStmt(cond, thenstmt, elsestmt));
    return rb_l;
  }
  else
  { // vdm_CPP_isCPP()
    Generic n1tp (FindType(n1));
    Generic n2tp (FindType(n2));

    TYPE_CPP_Expr n1value;
    if (IsNumType(n1tp))
    {
      if (IsIntType(n1tp))
      {
        n1value = GenGetValue(n1_v , rrep);
// 20100107 -->
        if (vdm_CPP_isCPP())
        {
          if (n1_v.Is(TAG_TYPE_CPP_FctCall) &&
              (n1_v.GetRecord(pos_CPP_FctCall_fct) == GenIntType().get_tp()) &&
              !(n1_v.GetSequence(pos_CPP_FctCall_arg).IsEmpty()) &&
              (n1_v.GetSequence(pos_CPP_FctCall_arg).Hd()).Is(TAG_TYPE_CPP_IntegerLit))
          {
            //ai1 = vdm_BC_GenAsgnInit(n1_v.GetSequence(pos_CPP_FctCall_arg).Hd());
            n1value = n1_v.GetSequence(pos_CPP_FctCall_arg).Hd();
          }
        }
// <-- 20100107
      }
      else
        n1value = GenGetValue(vdm_BC_GenFctCallObjMemAcc(n1_v, ASTAUX::MkId(L"Ceil"), SEQ<TYPE_CPP_Expr>()), rrep);
    }
    else
    {
      TYPE_CPP_Identifier From2 (vdm_BC_GiveName(ASTAUX::MkId(L"from")));
      rb_l.ImpConc(GenDecl_DS(n1tp, From2, vdm_BC_GenAsgnInit(n1_v)));
      rb_l.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenNot(GenIsReal(From2)),
                                vdm_BC_GenBlock(mk_sequence(RunTime (L"A number was expected"))), nil));
      n1_v = From2;
      if (vdm_CPP_isCPP())
        n1value = GenGetValue(vdm_BC_GenCastExpr(GenRealType(), n1_v), rrep );
      else
        n1value = GenGetValue(GenExplicitCast(rrep, n1_v, n1tp), rrep);
    }

    TYPE_CPP_Expr n2value;
    if (IsNumType(n2tp))
    {
      if (IsIntType(n2tp))
      {
        n2value = GenGetValue(n2_v, rrep);
// 20100107 -->
        if (vdm_CPP_isCPP())
        {
          if (n2_v.Is(TAG_TYPE_CPP_FctCall) &&
              (n2_v.GetRecord(pos_CPP_FctCall_fct) == GenIntType().get_tp()) &&
              !(n2_v.GetSequence(pos_CPP_FctCall_arg).IsEmpty()) &&
              (n2_v.GetSequence(pos_CPP_FctCall_arg).Hd()).Is(TAG_TYPE_CPP_IntegerLit))
          {
            //ai2 = vdm_BC_GenAsgnInit(n2_v.GetSequence(pos_CPP_FctCall_arg).Hd());
            n2value = n2_v.GetSequence(pos_CPP_FctCall_arg).Hd();
          }
          else if (n2_v.Is(TAG_TYPE_CPP_CastExpr) &&
                   n2_v.GetRecord(pos_CPP_CastExpr_expr).Is(TAG_TYPE_CPP_FctCall))
          {
            const TYPE_CPP_FctCall & fc (n2_v.GetRecord(pos_CPP_CastExpr_expr));
            if (fc.GetRecord(pos_CPP_FctCall_fct).Is(TAG_TYPE_CPP_ObjectMemberAccess))
            {
               const TYPE_CPP_ObjectMemberAccess & oma (fc.GetRecord(pos_CPP_FctCall_fct));
               if ((oma.GetRecord(pos_CPP_ObjectMemberAccess_object) == tmpSeq) &&
                   (oma.GetRecord(pos_CPP_ObjectMemberAccess_name) == vdm_BC_GenIdentifier(ASTAUX::MkId(L"Length"))) &&
                   (fc.GetSequence(pos_CPP_FctCall_arg).IsEmpty()))
               {
                 //ai2 = vdm_BC_GenAsgnInit(fc);
                 n2value = fc;
               }
            }
          }
        }
// <-- 20100107
      }
      else
        n2value = GenGetValue(vdm_BC_GenFctCallObjMemAcc(n2_v, ASTAUX::MkId(L"Floor"), SEQ<TYPE_CPP_Expr>()), rrep);
    }
    else
    {
      TYPE_CPP_Identifier To2 (vdm_BC_GiveName(ASTAUX::MkId(L"to")));
      rb_l.ImpConc(GenDecl_DS(n2tp, To2, vdm_BC_GenAsgnInit(n2_v)));
      rb_l.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenNot(GenIsReal(To2)),
                               vdm_BC_GenBlock(mk_sequence(RunTime (L"A number was expected"))), nil));
      n2_v = To2;
      if (vdm_CPP_isCPP())
        n2value = GenGetValue(vdm_BC_GenCastExpr(GenRealType(), n2_v), rrep);
      else
        n2value = GenGetValue(GenExplicitCast(rrep, n2_v, n2tp), rrep);
    }

    if (vdm_CPP_isCPP())
    {
      rb_l.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, GenSubSequence(tmpSeq, Nil(), n1value, n2value)));
      return rb_l;
    }
    else
    {
      TYPE_CPP_AsgnInit ai1 (vdm_BC_GenAsgnInit(n1value));
      TYPE_CPP_AsgnInit ai2 (vdm_BC_GenAsgnInit(n2value));

      TYPE_CPP_AsgnInit ai3 (vdm_BC_GenAsgnInit(GenLen_int(tmpSeq)));

      rb_l.ImpAppend(vdm_BC_GenDecl(sint, From, ai1));
      rb_l.ImpAppend(vdm_BC_GenDecl(sint, To, ai2));
      rb_l.ImpAppend(vdm_BC_GenDecl(sint, Max, ai3));;
      TYPE_CPP_Stmt ini (vdm_BC_GenDecl(sint, i, vdm_BC_GenAsgnInit(vdm_BC_GenIntegerLit(1))));
      TYPE_CPP_Expr e1 (vdm_BC_GenLeq(i, Max));
      TYPE_CPP_Expr e2 (vdm_BC_GenPostPlusPlus(i));
      TYPE_CPP_Expr c1 (vdm_BC_GenLeq(From, i));
      TYPE_CPP_Expr c2 (vdm_BC_GenLeq(i, To));
      TYPE_CPP_Stmt stmt (GenImpAppend(resSeq, GenSeqApply(nil, tmpSeq, i)));
      TYPE_CPP_Stmt ifs (vdm_BC_GenIfStmt(vdm_BC_GenLogAnd(c1,c2), stmt, nil));
      rb_l.ImpAppend(vdm_BC_GenForStmt(ini, e1, type_dL().ImpAppend(e2), ifs));
      rb_l.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, resSeq));
      return rb_l;
    }
  }
}

// CGTupleConstructorExpr
// rc1 : AS`TupleConstructorExpr
// vt : CGMAIN`VT
// ==> seq of CPP`Stmt | CPP`Expr
Generic vdmcg::CGTupleConstructorExpr (const TYPE_AS_TupleConstructorExpr & rc1,
                                       const TYPE_CGMAIN_VT & vt)
{
  const SEQ<TYPE_AS_Expr> & fields (rc1.GetSequence(pos_AS_TupleConstructorExpr_fields));
  const TYPE_CPP_Expr & resVar_v (vt.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & rType (vt.GetRecord(pos_CGMAIN_VT_type));

  TYPE_CPP_Expr resVar_vq (IsProductType(rType) ? resVar_v : GenCastProductTypeForModify(resVar_v));

  TYPE_CPP_Expr tc (GenProductExpr(vdm_BC_GenIntegerLit(fields.Length())));

  SEQ<TYPE_CPP_Stmt> rb;
  rb.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, tc));

  bool allsimple = true;
  SEQ<TYPE_CPP_Expr> args;
  size_t len_fields = fields.Length();

  SEQ<TYPE_REP_TypeRep> etp_l (FindProductElemType(rType, len_fields));
  for (size_t i = 1; i <= len_fields; i++)
  {
    const TYPE_CPP_Expr & e (fields[i]);
    Generic etp (vdm_CPP_isCPP() ? (Generic)nil : (Generic)(etp_l[i]));
    Tuple cgee (CGExprExcl(e, ASTAUX::MkId(L"tmpVar"), etp));
    const TYPE_CPP_Expr & tmpExpr (cgee.GetRecord(1));
    const SEQ<TYPE_CPP_Stmt> & e_stmt (cgee.GetSequence(2));
    TYPE_CPP_IntegerLit field (vdm_BC_GenIntegerLit(i));
    TYPE_CPP_Stmt setField (GenTupSetField(resVar_vq, field, tmpExpr));

    rb.ImpConc(e_stmt);
    rb.ImpAppend(setField);

    if (!e_stmt.IsEmpty() || e.Is(TAG_TYPE_AS_ApplyExpr))
      allsimple = false;
    args.ImpAppend(tmpExpr);
  }
   
  if ((len_fields <= 8) && allsimple)
  {
    TYPE_CPP_Expr fnm (vdm_CPP_isCPP() ? vdm_BC_GenIdentifier(ASTAUX::MkId(L"mk_"))
                                       : vdm_BC_GenIdentifier(ASTAUX::MkId(L"Tuple.mk_")));
    return vdm_BC_GenFctCall(fnm, args);
  }
  else
    return rb;
}

// CGRecordConstructorExpr
// rc1 : AS`RecordConstructorExpr
// vt : CGMAIN`VT
//  ==> seq of CPP`Stmt | CPP`Expr
Generic vdmcg::CGRecordConstructorExpr (const TYPE_AS_RecordConstructorExpr & rc1, const TYPE_CGMAIN_VT & vt)
{
//  const TYPE_AS_Name & tag         (rc1.GetRecord(pos_AS_RecordConstructorExpr_tag));
  const SEQ<TYPE_AS_Expr> & fields (rc1.GetSequence(pos_AS_RecordConstructorExpr_fields));
  const TYPE_CPP_Expr & resVar_v (vt.GetRecord(pos_CGMAIN_VT_name));
  //const TYPE_REP_TypeRep & rType (vt.GetRecord(pos_CGMAIN_VT_type));

  TYPE_REP_CompositeTypeRep ctp (FindType(rc1));
  const TYPE_AS_Name & tag (ctp.GetRecord(pos_REP_CompositeTypeRep_nm));

  if (vdm_CPP_isCPP())
  {
    TYPE_CPP_Expr rc (vdm_BC_GenFctCall(Name2CppTpId(tag), Sequence()));

    if (fields.IsEmpty())
      return rc;

    SEQ<TYPE_CPP_Stmt> rb;
    SEQ<TYPE_CPP_Expr> args;

    size_t len_fields = fields.Length();
    for (size_t i = 1; i <= len_fields; i++)
    {
      const TYPE_CPP_Expr & expr (fields[i]);
      Tuple cgee (CGExprExcl(expr, ASTAUX::MkId(L"tmpVar"), nil));
      const TYPE_CPP_Expr & tmpExpr (cgee.GetRecord(1));
      const SEQ<TYPE_CPP_Stmt> & e_stmt (cgee.GetSequence(2));

      rb.ImpConc(e_stmt);
      TYPE_CPP_Expr tmpExpr_q (tmpExpr);
      if (expr.Is(TAG_TYPE_AS_ApplyExpr) && tmpExpr.Is(TAG_TYPE_CPP_CastExpr)) {
        tmpExpr_q = tmpExpr.GetRecord(pos_CPP_CastExpr_expr);
      }
      args.ImpAppend(tmpExpr_q);
    }
    rb.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, vdm_BC_GenFctCallObjMemAcc(rc, ASTAUX::MkId(L"Init"), args)));
    return rb;
  }
  else // for Java
  {
    TYPE_CPP_Expr rc (GenEmptyRecordExpr(tag));

    if (fields.IsEmpty())
      return rc;

    TYPE_REP_CompositeTypeRep ft (FindType(rc1));
    const SEQ<TYPE_REP_FieldRep> & rcfields (ft.GetSequence(pos_REP_CompositeTypeRep_fields));

    SEQ<TYPE_CPP_Stmt> rb;
    SEQ<TYPE_CPP_Expr> par;
    size_t len_fields = fields.Length();
    for (size_t i = 1; i <= len_fields; i++)
    {
      const TYPE_AS_Field & fld (fields[i]);
      const TYPE_REP_FieldRep & frep (rcfields[i]);
      const TYPE_REP_TypeRep & type (frep.GetRecord(pos_REP_FieldRep_tp));

      Tuple cgee (CGExprExcl(fld, ASTAUX::MkId(L"tmpVar"), nil));
      const TYPE_CPP_Expr & tmpExpr (cgee.GetRecord(1));
      const SEQ<TYPE_CPP_Stmt> & e_stmt (cgee.GetSequence(2));

      TYPE_REP_TypeRep cft (CleanFlatType(type));
      TYPE_REP_TypeRep casttp;
#ifdef VDMPP
      if ( cft.Is(TAG_TYPE_REP_ObjRefTypeRep) )
        casttp = UnQClassType (cft);
      else
#endif // VDMPP
        casttp = type;

      TYPE_CPP_Expr castexpr (GenExplicitCast(casttp, tmpExpr, FindType(fld)));

      rb.ImpConc(e_stmt);
      par.ImpAppend(castexpr);
    }
    rb.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, GenRecordExpr(tag, par)));
    return rb;
  }
}

// CGTokenConstExpr
// expr : AS`TokenConstructorExpr
// vt : CGMAIN`VT
// ==> seq of CPP`Stmt | CPP`Expr
Generic vdmcg::CGTokenConstExpr(const TYPE_AS_TokenConstructorExpr & expr, const TYPE_CGMAIN_VT & vt)
{
  const TYPE_AS_Expr & elem (expr.GetRecord(pos_AS_TokenConstructorExpr_field));
  const TYPE_CPP_Expr & resVar (vt.GetRecord(pos_CGMAIN_VT_name));

  Tuple cgee (CGExprExcl(elem, ASTAUX::MkId(L"tmpvar"), nil));
  const TYPE_CPP_Expr & tmpvar (cgee.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & e_stmt (cgee.GetSequence(2));

  TYPE_CPP_Expr tokenconst (vdm_CPP_isCPP()
                 ? vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"mk_token")), mk_sequence(tmpvar))
                 : GenTokenExpr(tmpvar));

  if (e_stmt.IsEmpty())
    return tokenconst;
  else
  {
    SEQ<TYPE_CPP_Stmt> rb (e_stmt);
    rb.ImpAppend(vdm_BC_GenAsgnStmt(resVar, tokenconst));
    return rb;
  }
}

// CGTypeJudgementExpr
// tj : AS`TypeJudgementExpr
// vt : CGMAIN`VT
// ==> (seq of CPP`Stmt) | CPP`Expr
Generic vdmcg::CGTypeJudgementExpr(const TYPE_AS_TypeJudgementExpr & tj, const TYPE_CGMAIN_VT & vt)
{
  const TYPE_AS_Expr & expr (tj.GetRecord(pos_AS_TypeJudgementExpr_expr));
  const TYPE_AS_Type & type (tj.GetRecord(pos_AS_TypeJudgementExpr_type));
  const TYPE_CI_ContextId & cid (tj.GetInt(pos_AS_TypeJudgementExpr_cid));

  if (!expr.Is(TAG_TYPE_AS_Name))
  {
    TYPE_CI_ContextId ecid (GetCI().PushCGType(FindType(expr)));
    TYPE_CPP_Identifier tjTempId (vdm_BC_GiveName(ASTAUX::MkId(L"tjTemp")));
    TYPE_AS_Name tjTemp (ASTAUX::MkNameFromId(tjTempId.GetSequence(pos_CPP_Identifier_id), ecid));
    TYPE_AS_PatternName pn;
    pn.Init(tjTemp, Nil(), ecid);
    TYPE_AS_TypeJudgementExpr ntj (tj);
    ntj.SetField(pos_AS_TypeJudgementExpr_expr, tjTemp);
    return CGExpr_q(TYPE_AS_DefExpr().Init(mk_sequence(mk_(pn, expr)), ntj, cid), vt);
  }
  else
  {
    Generic e = ConvertTypeJudgementExprAS(expr, type);
    if (!e.IsNil()) {
      return CGExpr_q(e, vt);
    }
    else {
      switch (type.GetTag()) {
        case TAG_TYPE_AS_TypeVar: {
          SetNotSupportedException(true);
          SEQ<TYPE_CPP_Stmt> rb_l;
          rb_l.ImpAppend(NotSupported(L"type judgement expression for type variable", tj));
          return rb_l;
        }
        default: {
          SetNotSupportedException(true);
          SEQ<TYPE_CPP_Stmt> rb_l;
          rb_l.ImpAppend(NotSupported(L"type judgement expression", tj));
          return rb_l;
        }
      }
    }
  }
}

// ConvertTypeJudgementExprAS
// expr : AS`Expr
// tp : AS`Type
// ==> [AS`Expr]
Generic vdmcg::ConvertTypeJudgementExprAS(const TYPE_AS_Expr & expr, const TYPE_AS_Type & tp)
{
  TYPE_REP_BooleanTypeRep bt;
  TYPE_CI_ContextId cid (GetCI().PushCGType(bt));

  switch (tp.GetTag()) {
    case TAG_TYPE_AS_BooleanType:
    case TAG_TYPE_AS_NumericType:
    case TAG_TYPE_AS_TokenType:
    case TAG_TYPE_AS_CharType: {
      return TYPE_AS_IsExpr().Init(tp, expr, cid);
    }
    case TAG_TYPE_AS_CompositeType: {
      return TYPE_AS_IsExpr().Init(tp.GetRecord(pos_AS_CompositeType_name), expr, cid);
    }
    case TAG_TYPE_AS_QuoteType: {
      const TYPE_AS_Name & lit (tp.GetRecord(pos_AS_QuoteType_lit));
      return TYPE_AS_BinaryExpr().Init(expr, Int(EQ), lit, cid);
    }
    case TAG_TYPE_AS_BracketedType: {
      return ConvertTypeJudgementExprAS(expr, tp.GetRecord(pos_AS_BracketedType_tp));
    }
    case TAG_TYPE_AS_AllType: {
      return TYPE_AS_BoolLit().Init(Bool(true), cid);
    }
    case TAG_TYPE_AS_UnionType: {
      // is_(expr, T1 | ... | Tn) == is_(expr, T1) or ... or is_(expr, Tn)
      const SEQ<TYPE_AS_Type> & tps (tp.GetSequence(pos_AS_UnionType_tps));
      bool forall = true;
      SEQ<TYPE_AS_Expr> e_l;
      size_t len_tps = tps.Length();
      for (size_t idx = 1; (idx <= len_tps) && forall; idx++) {
        Generic e (ConvertTypeJudgementExprAS(expr, tps[idx]));
        if (!e.IsNil()) {
          e_l.ImpAppend(e);
        }
        else {
          forall = false;
        }
      }
      if (forall && !e_l.IsEmpty()) {
        size_t len_e_l = e_l.Length();
        TYPE_AS_Expr e (e_l[len_e_l]);
        for (size_t idx = len_e_l - 1; idx >= 1; idx--) {
          TYPE_AS_BinaryExpr be;
          be.Init(e_l[idx], Int(OR), e, cid);
          e = be;
        }
        return e;
      }
      return Nil();
    }
    case TAG_TYPE_AS_ProductType: {
      // is_(expr, T1 * ... * Tn) ==
      //   cases expr:
      //     mk_(-, ..., -) -> is_(expe.#1, T1) and ... and is_(expe.#n, Tn),
      //     others -> false
      //   end
      const SEQ<TYPE_AS_Type> & tp_l (tp.GetSequence(pos_AS_ProductType_tps));
      TYPE_CI_ContextId ncid (GetCI().PushCGType(mk_REP_NumericTypeRep(Int(NATONE))));

      size_t len_tp_l = tp_l.Length();
      Generic fpet (FindProductElemType (FindType(expr), len_tp_l));
      if (fpet.IsNil()) return Nil();
      SEQ<TYPE_REP_TypeRep> ptp_l (fpet);
      TYPE_CI_ContextId pcid (GetCI().PushCGType(mk_REP_ProductTypeRep(ptp_l)));
      bool forall = true;
      SEQ<TYPE_AS_Expr> e_l;
      for (size_t idx = 1; (idx <= len_tp_l) && forall; idx++)
      {
        TYPE_CI_ContextId ecid (GetCI().PushCGType(ptp_l[idx]));
        TYPE_AS_TupleSelectExpr tse;
        tse.Init(expr, TYPE_AS_NumLit().Init(Real(idx), ncid), ecid);
        Generic e (ConvertTypeJudgementExprAS(tse, tp_l[idx]));
        if (!e.IsNil())
          e_l.ImpAppend(e);
        else
          forall = false;
      }
      if (forall)
      {
        size_t len_e_l = e_l.Length();
        TYPE_AS_Expr e (e_l[len_e_l]);
        for (size_t idx = len_e_l - 1; idx >= 1; idx--)
        {
          TYPE_AS_BinaryExpr be;
          be.Init(e_l[idx], Int(AND), e, cid);
          e = be;
        }
        SEQ<TYPE_AS_Pattern> p_l;
        for (size_t i = 1; i <= len_e_l; i++)
        {
          TYPE_CI_ContextId tcid (GetCI().PushCGType(ptp_l[i]));
          p_l.ImpAppend(TYPE_AS_PatternName().Init(Nil(), Nil(), tcid));
        }
        TYPE_AS_CaseAltn ca1;
        ca1.Init(mk_sequence(TYPE_AS_TuplePattern().Init(p_l, pcid)), e, cid);
        return TYPE_AS_CasesExpr().Init(expr, mk_sequence(ca1), TYPE_AS_BoolLit().Init(Bool(false), cid), cid);
      }
      return Nil();
    }
    case TAG_TYPE_AS_Seq0Type:
    case TAG_TYPE_AS_Seq1Type: {
      // is_(expr, seq of T) ==
      //   cases expr:
      //     - ^ - -> forall elem in set elems expr & is_(elem, T),  -- for Seq0Type
      //     [-] ^ - -> forall elem in set elems expr & is_(elem, T),-- for Seq1Type
      //     others -> false
      //   end
      const TYPE_AS_Type & elemtp (tp.GetRecord(pos_AS_Seq0Type_elemtp));
      TYPE_CI_ContextId bcid (GetCI().PushCGType(mk_REP_BooleanTypeRep()));
      TYPE_REP_TypeRep seqtp (FindSeqElemType(FindType(expr)));
      TYPE_CI_ContextId ecid (GetCI().PushCGType(seqtp));
      TYPE_CPP_Identifier elemId (vdm_BC_GiveName(ASTAUX::MkId(L"elem")));
      TYPE_AS_Name elem (ASTAUX::MkNameFromId(elemId.GetSequence(pos_CPP_Identifier_id), ecid));
      Map pid_m;
      pid_m.ImpModify(elem, seqtp);
      GetCI().SetPidM(bcid, pid_m);
      Generic pred (ConvertTypeJudgementExprAS(elem, elemtp));
      if (!pred.IsNil())
      {
        TYPE_CI_ContextId scid (GetCI().PushCGType(mk_REP_SetTypeRep(seqtp)));
        TYPE_AS_AllOrExistsExpr aoee;
        aoee.Init(Int(ALL),
                  mk_sequence(TYPE_AS_MultSetBind().Init(mk_sequence(TYPE_AS_PatternName().Init(elem, Nil(), ecid)),
                                                         TYPE_AS_PrefixExpr().Init(Int(SEQELEMS), expr, scid),
                                                         NilContextId)),
                  pred,
                  bcid); 
        TYPE_AS_Pattern p (tp.Is(TAG_TYPE_AS_Seq0Type)
                             ? (TYPE_AS_Pattern)TYPE_AS_PatternName().Init(Nil(), Nil(), NilContextId)
                             : (TYPE_AS_Pattern)TYPE_AS_SeqEnumPattern().Init(
                                     mk_sequence(TYPE_AS_PatternName().Init(Nil(), Nil(), NilContextId)),
                                     NilContextId));
        TYPE_AS_CaseAltn ca;
        ca.Init(mk_sequence(TYPE_AS_SeqConcPattern().Init(
                                 p,
                                 TYPE_AS_PatternName().Init(Nil(), Nil(), NilContextId), NilContextId)),
                 aoee, cid);
        return TYPE_AS_CasesExpr().Init(expr, mk_sequence(ca), TYPE_AS_BoolLit().Init(Bool(false), cid), cid);
      }
      return Nil();
    }
    case TAG_TYPE_AS_Set0Type:
    case TAG_TYPE_AS_Set1Type: {
      // is_(expr, set of T) ==
      //   cases expr:
      //     - union - -> forall elem in set expr & is_(elem, T),   -- for Set0Type
      //     {-} union - -> forall elem in set expr & is_(elem, T), -- for Set1Type
      //     others -> false
      //   end
      const TYPE_AS_Type & elemtp (tp.GetRecord(pos_AS_Set0Type_elemtp));
      TYPE_CI_ContextId bcid (GetCI().PushCGType(mk_REP_BooleanTypeRep()));
      TYPE_REP_TypeRep settp (FindSetElemType(FindType(expr)));
      TYPE_CI_ContextId ecid (GetCI().PushCGType(settp));
      TYPE_CPP_Identifier elemId (vdm_BC_GiveName(ASTAUX::MkId(L"elem")));
      TYPE_AS_Name elem (ASTAUX::MkNameFromId(elemId.GetSequence(pos_CPP_Identifier_id), ecid));
      Generic pred (ConvertTypeJudgementExprAS(elem, elemtp));
      if (!pred.IsNil())
      {
        Map pid_m;
        pid_m.ImpModify(elem, settp);
        GetCI().SetPidM(bcid, pid_m);

        TYPE_AS_AllOrExistsExpr aoee;
        aoee.Init(Int(ALL),
                  mk_sequence(TYPE_AS_MultSetBind().Init(mk_sequence(TYPE_AS_PatternName().Init(elem, Nil(), ecid)),
                                                         expr,
                                                         NilContextId)),
                  pred,
                  bcid); 
        TYPE_AS_Pattern p (tp.Is(TAG_TYPE_AS_Set0Type)
                             ? (TYPE_AS_Pattern)TYPE_AS_PatternName().Init(Nil(), Nil(), NilContextId)
                             : (TYPE_AS_Pattern)TYPE_AS_SetEnumPattern().Init(
                                     mk_sequence(TYPE_AS_PatternName().Init(Nil(), Nil(), NilContextId)),
                                     NilContextId));
        TYPE_AS_CaseAltn ca;
        ca.Init(mk_sequence(TYPE_AS_SetUnionPattern().Init(
                                 p,
                                 TYPE_AS_PatternName().Init(Nil(), Nil(), NilContextId), NilContextId)),
                 aoee, cid);
        return TYPE_AS_CasesExpr().Init(expr, mk_sequence(ca), TYPE_AS_BoolLit().Init(Bool(false), cid), cid);
      }
      return Nil();
    }
    case TAG_TYPE_AS_GeneralMap0Type:
    case TAG_TYPE_AS_GeneralMap1Type:
    case TAG_TYPE_AS_InjectiveMap0Type:
    case TAG_TYPE_AS_InjectiveMap1Type: {
      // is_(expr, map Td to Tr) ==
      //   cases expr:
      //     - munion -         -- for GeneralMap0Type and InjectiveMap0Type
      //     {-|->-} munion -   -- for GeneralMap1Type and InjectiveMap1Type
      //           -> forall e in set dom expr & is_(e, Td) and is_(narrow_(expr, map ? to ?)(e), Tr),
      //                       -- for GeneralMap0Type and GeneralMap1Type
      //           -> (forall e in set dom expr & is_(e, Td) and is_(narrow_(expr, map ? to ?)(e), Tr))
      //                     and card dom expr = car rng expr,
      //                       -- for InjectiveMap0Type and InjectiveMap1Type
      //             
      //     others -> false
      //   end
      const TYPE_AS_Type & mapdomtp (tp.GetRecord(1));
      const TYPE_AS_Type & maprngtp (tp.GetRecord(2));
      TYPE_CI_ContextId bcid (GetCI().PushCGType(mk_REP_BooleanTypeRep()));
      Generic getp (FindType(expr));
      TYPE_REP_TypeRep domtp (FindMapDomType(getp));
      TYPE_REP_TypeRep rngtp (FindMapRngType(getp));
      TYPE_CI_ContextId domcid (GetCI().PushCGType(domtp));
      TYPE_CI_ContextId rngcid (GetCI().PushCGType(rngtp));
      TYPE_CI_ContextId mapcid (GetCI().PushCGType(mk_REP_GeneralMapTypeRep(domtp, rngtp)));
      TYPE_CPP_Identifier elemId (vdm_BC_GiveName(ASTAUX::MkId(L"elem")));
      TYPE_AS_Name elem (ASTAUX::MkNameFromId(elemId.GetSequence(pos_CPP_Identifier_id), domcid));
      Map pid_m;
      pid_m.ImpModify(elem, domtp);
      GetCI().SetPidM(bcid, pid_m);
      TYPE_AS_ApplyExpr ae;
      if (getp.Is(TAG_TYPE_REP_AllTypeRep)) {
        TYPE_AS_AllType atp;
        atp.Init(NilContextId);
        TYPE_AS_GeneralMap0Type gmt;
        gmt.Init(atp, atp, NilContextId);
        TYPE_AS_NarrowExpr ne;
        ne.Init(expr, gmt, mapcid); 
        ae.Init(ne, mk_sequence(elem), rngcid);
      }
      else
        ae.Init(expr, mk_sequence(elem), rngcid);
      Generic pred1 (ConvertTypeJudgementExprAS(elem, mapdomtp));
      Generic pred2 (ConvertTypeJudgementExprAS(ae, maprngtp));
      if (!pred1.IsNil() && !pred2.IsNil())
      {
        TYPE_CI_ContextId scid (GetCI().PushCGType(mk_REP_SetTypeRep(domtp)));
        TYPE_CPP_Expr mdom (TYPE_AS_PrefixExpr().Init(Int(MAPDOM), expr, scid));
        TYPE_CI_ContextId rcid (GetCI().PushCGType(mk_REP_SetTypeRep(rngtp)));
        TYPE_CPP_Expr mrng (TYPE_AS_PrefixExpr().Init(Int(MAPRNG), expr, rcid));
        TYPE_CI_ContextId ncid (GetCI().PushCGType(mk_REP_NumericTypeRep(Int(NAT))));
        TYPE_AS_AllOrExistsExpr aoee;
        aoee.Init(Int(ALL),
                  mk_sequence(TYPE_AS_MultSetBind().Init(mk_sequence(TYPE_AS_PatternName().Init(elem, Nil(), domcid)),
                                                         mdom, NilContextId)),
                  TYPE_AS_BinaryExpr().Init(pred1, Int(AND), pred2, bcid),
                  bcid); 
        TYPE_AS_Expr cond (tp.Is(TAG_TYPE_AS_GeneralMap0Type) || tp.Is(TAG_TYPE_AS_GeneralMap1Type)
                             ? (TYPE_CPP_Expr)aoee
                             : (TYPE_CPP_Expr)TYPE_AS_BinaryExpr().Init(aoee, Int(AND),
                                        TYPE_AS_BinaryExpr().Init(
                                          TYPE_AS_PrefixExpr().Init(Int(SETCARD), mdom, ncid),
                                          Int(EQ),
                                          TYPE_AS_PrefixExpr().Init(Int(SETCARD), mrng, ncid),
                                          bcid),bcid));
        TYPE_AS_Pattern p (tp.Is(TAG_TYPE_AS_GeneralMap0Type) || tp.Is(TAG_TYPE_AS_InjectiveMap0Type)
                             ? (TYPE_AS_Pattern)TYPE_AS_PatternName().Init(Nil(), Nil(), NilContextId)
                             : (TYPE_AS_Pattern)TYPE_AS_MapEnumPattern().Init(
                                     mk_sequence(
                     TYPE_AS_MapletPattern().Init(
                       TYPE_AS_PatternName().Init(Nil(), Nil(), NilContextId),
                       TYPE_AS_PatternName().Init(Nil(), Nil(), NilContextId),
                                     NilContextId)), NilContextId));
        TYPE_AS_CaseAltn ca;
        ca.Init(mk_sequence(TYPE_AS_MapMergePattern().Init(
                                 p,
                                 TYPE_AS_PatternName().Init(Nil(), Nil(), NilContextId), NilContextId)),
                 cond, cid);
        return TYPE_AS_CasesExpr().Init(expr, mk_sequence(ca), TYPE_AS_BoolLit().Init(Bool(false), cid), cid);
      }
      return Nil();
    }
    case TAG_TYPE_AS_TypeName: {
      const TYPE_AS_Name & name (tp.GetRecord(pos_AS_TypeName_name));
#ifdef VDMPP
      if (IsClass(name)) {
        return TYPE_AS_IsOfClassExpr().Init(name, expr, cid);
      }
#endif // VDPP
      Generic tdef (GetTypeDef(name));
      if (!tdef.IsNil()) {
        const TYPE_AS_Type & shape (Record(tdef).GetRecord(pos_AS_TypeDef_shape));
        const Generic & inv (Record(tdef).GetField(pos_AS_TypeDef_Inv));
        Generic e (ConvertTypeJudgementExprAS(expr, shape));
        if (!inv.IsNil() && !e.IsNil()) {
          //const TYPE_AS_Name & name (tp.GetRecord(pos_REP_InvTypeRep_name));
          TYPE_AS_Ids ids (name.GetSequence(pos_AS_Name_ids)); 
          size_t len_ids = ids.Length();
          ids.ImpModify(len_ids, ASTAUX::MkId(L"inv_").ImpConc(ids[len_ids]));
          TYPE_REP_TotalFnTypeRep tftr;
          tftr.Init(mk_sequence(FromAS2RepType(tp)), mk_REP_BooleanTypeRep());
          TYPE_CI_ContextId fncid (GetCI().PushCGType(tftr));
          TYPE_AS_Name invfn (ASTAUX::MkNameFromIds(ids, fncid));
          TYPE_AS_TypeName tn;
          tn.Init(name, name.GetInt(pos_AS_Name_cid));
          TYPE_REP_TypeNameRep tnr;
          tnr.Init(name);
          TYPE_AS_NarrowExpr ne;
          ne.Init(expr, tn, GetCI().PushCGType(tnr));

          TYPE_AS_ApplyExpr inve;
          inve.Init(invfn, mk_sequence(ne), cid);
          return TYPE_AS_BinaryExpr().Init(e, Int(AND), inve, cid);
        }
        return e;
      }
      return Nil();
    }
    default: {
      return Nil();
    }
  }
}

// CGNarrowExpr
// tj : AS`NarrowExpr
// vt : CGMAIN`VT
// ==> (seq of CPP`Stmt) | CPP`Expr
Generic vdmcg::CGNarrowExpr(const TYPE_AS_NarrowExpr & ne, const TYPE_CGMAIN_VT & vt)
{
  const TYPE_AS_Expr & expr (ne.GetRecord(pos_AS_NarrowExpr_expr));
  const TYPE_AS_Expr & type (ne.GetRecord(pos_AS_NarrowExpr_type));

  Generic rtp (FindType(expr));
  TYPE_REP_TypeRep tp (RemoveInvType(FromAS2RepType(type)));

  if (!rtp.IsNil())
  {
    const TYPE_CPP_Expr & resvar (vt.GetRecord(pos_CGMAIN_VT_name));

    Tuple cgee (CGExprExcl(expr, ASTAUX::MkId(L"tmpVar"), nil));
    const TYPE_CPP_Expr & tmpVar_v (cgee.GetRecord(1));
    const SEQ<TYPE_CPP_Stmt> & stmts (cgee.GetSequence(2));

    TYPE_CPP_Expr cast (IsSubType(rtp, tp) ? tmpVar_v
                          : vdm_CPP_isCPP() ? GenCastType(tp, tmpVar_v) : GenExplicitCast(tp, tmpVar_v, rtp));
    if (stmts.IsEmpty())
      return cast;
    else
    {
      SEQ<TYPE_CPP_Stmt> rb (stmts);
      rb.ImpAppend(vdm_BC_GenAsgnStmt(resvar, cast));
      return rb;
    }
  }
  else
    return CGExpr_q(expr, vt);
}

// CGMacro
// tj : AS`Macro
// vt : CGMAIN`VT
// ==> (seq of CPP`Stmt) | CPP`Expr
Generic vdmcg::CGMacro(const TYPE_AS_Macro & me, const TYPE_CGMAIN_VT & vt)
{
  const Int & tp (me.GetInt(pos_AS_Macro_tp));
  switch (tp.GetValue()) {
    case MACRO_FILE: {
      if (vdm_CPP_isCPP())
      {
        // __FILE__ : SEQ<Char>(string2wstring(__FILE__))
#ifdef _MSC_VER
        TYPE_CPP_Expr ds (vdm_BC_GenIdentifier(ASTAUX::MkId(L"__FILEW__")));
#else
        TYPE_CPP_Expr ds (vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"string2wstring")),
                                            mk_sequence(vdm_BC_GenIdentifier(ASTAUX::MkId(L"__FILE__")))));
#endif // _MSC_VER
        return vdm_BC_GenFctCall(GenStringType().get_tp(), mk_sequence(ds));
      }
      else
      {
        // __FILE__ : (new Throwable()).getStackTrace()[0].getFileName()
        TYPE_CPP_Expr th (vdm_BC_GenClassInstanceCreationExpr(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Throwable")),
                                                              Sequence()));
        TYPE_CPP_Expr st (vdm_BC_GenFctCallObjMemAcc (vdm_BC_GenBracketedExpr(th),
                                                      ASTAUX::MkId(L"getStackTrace"),
                                                      Sequence()));
        TYPE_CPP_Expr aa (vdm_BC_GenArrayApply(st, vdm_BC_GenIntegerLit (0)));
        return vdm_BC_GenFctCallObjMemAcc(aa, ASTAUX::MkId(L"getFileName"), Sequence());
      }
    }
    case MACRO_LINE: {
      if (vdm_CPP_isCPP())
      {
        // __LINE__ : Int(__LINE__)
        return GenIntExpr(vdm_BC_GenIdentifier(ASTAUX::MkId(L"__LINE__")));
      }
      else
      {
        // __LINE__ : (new Throwable()).getStackTrace()[0].getLineNumber()
        TYPE_CPP_Expr th (vdm_BC_GenClassInstanceCreationExpr(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Throwable")),
                                                              Sequence()));
        TYPE_CPP_Expr st (vdm_BC_GenFctCallObjMemAcc (vdm_BC_GenBracketedExpr(th),
                                                      ASTAUX::MkId(L"getStackTrace"),
                                                      Sequence()));
        TYPE_CPP_Expr aa (vdm_BC_GenArrayApply(st, vdm_BC_GenIntegerLit (0)));
        return vdm_BC_GenFctCallObjMemAcc(aa, ASTAUX::MkId(L"getLineNumber"), Sequence());
      }
    }
    case MACRO_COLUMN: {
      TYPE_CPP_Expr colno (vdm_BC_GenIntegerLit (0));
      return GenIntExpr(colno);
    }
    case MACRO_CLMOD: {
      if (vdm_CPP_isCPP())
      {
#ifdef VDMSL
        TYPE_AS_Name clmod (GiveCurMASName());
#endif // VDMSL
#ifdef VDMPP
        TYPE_AS_Name clmod (GiveCurCASName());
#endif // VDMPP
        TYPE_CPP_Identifier id (vdm_BC_Rename(clmod));
        const TYPE_AS_Id & ch_l (id.GetSequence(pos_CPP_Identifier_id));
        return vdm_BC_GenFctCall(GenStringType().get_tp(), mk_sequence(vdm_BC_GenStringLit(ch_l)));
      }
      else
      {
        // __CLMOD__ : (new Throwable()).getStackTrace()[0].getClassName()
        TYPE_CPP_Expr th (vdm_BC_GenClassInstanceCreationExpr(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Throwable")),
                                                              Sequence()));
        TYPE_CPP_Expr st (vdm_BC_GenFctCallObjMemAcc (vdm_BC_GenBracketedExpr(th),
                                                      ASTAUX::MkId(L"getStackTrace"),
                                                      Sequence()));
        TYPE_CPP_Expr aa (vdm_BC_GenArrayApply(st, vdm_BC_GenIntegerLit (0)));
        return vdm_BC_GenFctCallObjMemAcc(aa, ASTAUX::MkId(L"getClassName"), Sequence());
      }
    }
    case MACRO_FNOP: {
      if (vdm_CPP_isCPP())
      {
        // __FNOP__ : SEQ<Char>(string2wstring(__func__))
#ifdef _MSC_VER
        TYPE_CPP_Expr ds (vdm_BC_GenIdentifier(ASTAUX::MkId(L"__FUNCTIONW__")));
#else
        TYPE_CPP_Expr ds (vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"string2wstring")),
                                            mk_sequence(vdm_BC_GenIdentifier(ASTAUX::MkId(L"__func__")))));
#endif // _MSC_VER
        return vdm_BC_GenFctCall(GenStringType().get_tp(), mk_sequence(ds));
      }
      else
      {
        // __FNOP__ : (new Throwable()).getStackTrace()[0].getMethodName()
        TYPE_CPP_Expr th (vdm_BC_GenClassInstanceCreationExpr(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Throwable")),
                                                              Sequence()));
        TYPE_CPP_Expr st (vdm_BC_GenFctCallObjMemAcc (vdm_BC_GenBracketedExpr(th),
                                                      ASTAUX::MkId(L"getStackTrace"),
                                                      Sequence()));
        TYPE_CPP_Expr aa (vdm_BC_GenArrayApply(st, vdm_BC_GenIntegerLit (0)));
        return vdm_BC_GenFctCallObjMemAcc(aa, ASTAUX::MkId(L"getMethodName"), Sequence());
      }
    }
  }
  return SEQ<TYPE_CPP_Stmt>(); // dummy
}

// CGTupleSelectExpr
// ts : AS`TupleSelectExpr
// vtv : CGMAIN`VT
// ==> (seq of CPP`Stmt) | CPP`Expr
Generic vdmcg::CGTupleSelectExpr(const TYPE_AS_TupleSelectExpr & ts, const TYPE_CGMAIN_VT & vtv)
{
  const TYPE_AS_Expr & tuple (ts.GetRecord(pos_AS_TupleSelectExpr_tuple));
  const TYPE_AS_Expr & no (ts.GetRecord(pos_AS_TupleSelectExpr_no));
  const TYPE_CPP_Expr & resvar (vtv.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & resTp (vtv.GetRecord(pos_CGMAIN_VT_type));

  TYPE_REP_TypeRep tupleTp (FindType(tuple));

  Tuple cgee (CGExprExcl(tuple, ASTAUX::MkId(L"tuple"), tupleTp));
  const TYPE_CPP_Expr & tupleName (cgee.GetRecord(1));
  SEQ<TYPE_CPP_Stmt> tupleCode (cgee.GetSequence(2));

  TYPE_CPP_Expr tupleVal;
  if (IsProductType(tupleTp))
    tupleVal = tupleName;
  else
  {
    // TODO: 
    //tupleVal = GenCastProduct(tupleName);
    tupleVal = (vdm_CPP_isCPP() ? vdm_BC_GenFctCall(GenProductType().get_tp(), mk_sequence(tupleName))
                                : GenCastProduct(tupleName));
  }

  TYPE_CPP_Identifier selector (vdm_BC_GiveName(ASTAUX::MkId(L"fieldsel")));
  TYPE_CPP_Expr selObj (CGLiteral(no, mk_CG_VT(selector, mk_REP_NumericTypeRep(Int(NATONE)))));
  TYPE_CPP_Expr selVal (GenGetValue(selObj, mk_REP_NumericTypeRep(Int(NATONE))));
  if (vdm_CPP_isJAVA() && get_longs_option())
    selVal = vdm_BC_GenCastExpr(GenSmallIntType(), selVal);

  TYPE_CPP_Expr tupleCall (vdm_BC_GenFctCallObjMemAcc(tupleVal, ASTAUX::MkId(L"GetField"), mk_sequence(selVal)));

  TYPE_CPP_Expr expr (GenCastType(resTp, tupleCall));
  if (tupleCode.IsEmpty()) 
    return expr;
  else {
    tupleCode.ImpAppend(vdm_BC_GenAsgnStmt(resvar, expr));
    return tupleCode;
  }
}

// CGApplyExpr
// rc1 : AS`ApplyExpr
// vt : CGMAIN`VT
// expr1 : [CPP`Name]
// type1 : [REP`TypeRep | set of REP`TypeRep]
// sd : bool
// ==> seq of CPP`Stmt | CPP`Expr
Generic vdmcg::CGApplyExpr (const TYPE_AS_ApplyExpr & rc1,
                            const TYPE_CGMAIN_VT & vt,
                            const Generic & expr1,
                            const Generic & type1,
                            bool sd)
{
  Generic ti = (expr1.IsNil() ? FindType (rc1.get_fct()) : type1);
// 20110626 -->
  if (ti.IsRecord())
  {
    if (ti.Is(TAG_TYPE_REP_InvTypeRep)) 
      ti = Record(ti).GetRecord(pos_REP_InvTypeRep_shape);
  }
// <-- 20110626

  if (ti.IsRecord())
  {
    switch ( Record(ti).GetTag() )
    {
      case TAG_TYPE_REP_GeneralMapTypeRep:
      case TAG_TYPE_REP_InjectiveMapTypeRep: {
        return CGMapApplyExpr (rc1, vt, expr1, type1, sd);
      }
      case TAG_TYPE_REP_SeqTypeRep: {
        return CGSeqApplyExpr (rc1, vt, expr1, type1);
      }
      case TAG_TYPE_REP_OpTypeRep:
      case TAG_TYPE_REP_PartialFnTypeRep:
      case TAG_TYPE_REP_TotalFnTypeRep: {
        return CGFctApplyExpr (rc1, vt);
      }
      case TAG_TYPE_REP_UnionTypeRep: {
        return CGUnionTypeApply (rc1, vt, expr1, type1, sd);
      }
#ifdef VDMPP
      case TAG_TYPE_REP_OverTypeRep: {
        return CGFctApplyExpr (rc1, vt);
      }
#endif // VDMPP
      case TAG_TYPE_REP_AllTypeRep: {
        return Sequence().ImpAppend(NotSupported(L"lambda expression", rc1));
      }
      default: {
        ReportError (L"CGApplyExpr");
      }
    }
  }
  else if (ti.IsSet()) // for overload
    return CGUnionTypeApply(rc1, vt, expr1, mk_REP_UnionTypeRep(ti), sd);

  return Record (); //to avoid warnings from g++
}

// CGMapApplyExpr
// rc1 : AS`ApplyExpr
// vt : CGMAIN`VT
// expr1 : [CPP`Name]
// type1 : [REP`TypeRep]
// sd : bool
// ==> seq of CPP`Stmt | CPP`Expr
Generic vdmcg::CGMapApplyExpr (const TYPE_AS_ApplyExpr & rc1,
                               const TYPE_CGMAIN_VT & vt,
                               const Generic & expr1,
                               const Generic & type1,
                               bool sd)
{
  const TYPE_AS_Expr & expr (rc1.GetRecord(pos_AS_ApplyExpr_fct));
  const TYPE_AS_Expr & par (rc1.GetSequence(pos_AS_ApplyExpr_arg).Hd());

  const TYPE_CPP_Expr & resVar_v (vt.GetRecord(pos_CGMAIN_VT_name));

  TYPE_CPP_Expr tmpmap_v;
  SEQ<TYPE_CPP_Stmt> e_stmt;
  TYPE_REP_TypeRep tmpmapType;
  if (expr1.IsNil())
  {
    Tuple tmp1 (CGExprExcl(expr, ASTAUX::MkId(L"tmp_m"), nil) );
    tmpmap_v = tmp1.GetRecord(1);
    e_stmt.ImpConc(tmp1.GetSequence(2));
    tmpmapType = FindType(expr);
  }
  else
  {
    tmpmap_v = expr1;
    tmpmapType = type1;
  }

  Generic rngType (FindMapDomRng(tmpmapType).GetField(2));

  Tuple cgee (CGExprExcl(par, ASTAUX::MkId(L"tmppar"), nil));
  const TYPE_CPP_Expr & tmppar_v (cgee.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & par_stmt (cgee.GetSequence(2));

  TYPE_CPP_Expr cond (GenDomExists(tmpmap_v, tmppar_v));

// 20090805 -->
//  TYPE_CPP_Expr alt1 (GenMapApply(mk_CG_VT(tmpmap_v, tmpmapType), tmppar_v));
  TYPE_CPP_Expr alt1;
  if (vdm_CPP_isCPP())
  {
    if (type1.IsNil())
    {
//      FindType(par)
      alt1 = GenMapApply(mk_CG_VT(tmpmap_v, nil), tmppar_v);
      if (!rngType.IsNil())
        alt1 = GenCastType(rngType, alt1);
    }
    else
    {
      alt1 = GenMapApply(mk_CG_VT(tmpmap_v, tmpmapType), tmppar_v);
    }
  }
  else
  {
    alt1 = GenMapApply(mk_CG_VT(tmpmap_v, tmpmapType), tmppar_v);
  }
// <-- 20090805

  SEQ<TYPE_CPP_Stmt> rb;

  if ( IsMapType(rngType) ||
       IsSeqType(rngType) ||
       rngType.Is(TAG_TYPE_REP_CompositeTypeRep) ||
       ! sd)
  {
    TYPE_CPP_Expr mapapply (alt1);
    if (sd)
    {
      TYPE_CPP_Expr alt2;
      if (IsMapType(rngType))
        alt2 = GenEmptyMapExpr();
      else if (IsSeqType(rngType))
        alt2 = GenEmptySeqExpr();
      else if (rngType.Is(TAG_TYPE_REP_CompositeTypeRep))
        alt2 = GenEmptyRecordExpr(Record(rngType).GetRecord(pos_REP_CompositeTypeRep_nm));
      mapapply = vdm_BC_GenBracketedExpr(vdm_BC_GenCondExpr(cond, alt1, alt2));
    }

    rb.ImpConc(e_stmt).ImpConc(par_stmt);

    if ( rb.IsEmpty() )
      return mapapply;
    else
      rb.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, mapapply));
  }
  else
  {
    TYPE_CPP_Expr alt2 (GenEmptyMapExpr());
    TYPE_CPP_Stmt asgn1 (vdm_BC_GenAsgnStmt(resVar_v, alt1));
    TYPE_CPP_Stmt asgn2 (vdm_BC_GenAsgnStmt(resVar_v, alt2));

    rb.ImpConc(e_stmt).ImpConc(par_stmt);
    rb.ImpAppend(vdm_BC_GenIfStmt(cond, asgn1, asgn2));
  }
  return rb;
}

// CGSeqApplyExpr
// rc1 : AS`ApplyExpr
// vt : CGMAIN`VT
// expr1 : [CPP`Name]
// type1 : [REP`TypeRep]
// ==> seq of CPP`Stmt | CPP`Expr
Generic vdmcg::CGSeqApplyExpr (const TYPE_AS_ApplyExpr & rc1,
                               const TYPE_CGMAIN_VT & vt,
                               const Generic & expr1,
                               const Generic & type1)
{
  const TYPE_AS_Expr & expr (rc1.GetRecord(pos_AS_ApplyExpr_fct));
  const TYPE_AS_Expr & par (rc1.GetSequence(pos_AS_ApplyExpr_arg).Hd());

  const TYPE_CPP_Expr & resVar_v (vt.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & restp (vt.GetRecord(pos_CGMAIN_VT_type));

  TYPE_CPP_Expr tmpseq_v;
  SEQ<TYPE_CPP_Stmt> e_stmt;
  TYPE_REP_TypeRep tmpseqType;
  if (expr1.IsNil())
  {
    Tuple tmp1 (CGExprExcl(expr, ASTAUX::MkId(L"tmp_l"), nil) );
    tmpseq_v = tmp1.GetRecord(1);
    e_stmt.ImpConc(tmp1.GetSequence(2));

    if (!tmpseq_v.Is(TAG_TYPE_CPP_Identifier))
    {
      TYPE_CPP_Expr argTmp (vdm_BC_GiveName(ASTAUX::MkId(L"argTmp")));
      e_stmt.ImpConc (GenDeclSeq (argTmp, tmpseq_v));
      tmpseq_v = argTmp;
    }
    tmpseqType = FindType(expr);
  }
  else
  {
    tmpseq_v = expr1;
    tmpseqType = type1;
  }

  TYPE_REP_TypeRep tmpparType (FindType(par));

  Tuple cgee (CGExprExcl(par, ASTAUX::MkId(L"tmppar"),nil));
  const TYPE_CPP_Expr & tmppar_v (cgee.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & par_stmt (cgee.GetSequence(2));

  SEQ<TYPE_CPP_Stmt> rb;
  rb.ImpConc(e_stmt).ImpConc(par_stmt);

  TYPE_CPP_Expr seqexpr;
  if (vdm_CPP_isCPP())
  {
    seqexpr = GenCastSeq (tmpseq_v, tmpseqType);
  }
  else
  { // Java
    if (IsSeqType(tmpseqType) || IsStringType(tmpseqType))
      seqexpr = tmpseq_v;
    else
      if (IsPossibleStringType(tmpseqType))
        seqexpr = GenCastString(tmpseq_v);
      else
        seqexpr = GenCastSeq (tmpseq_v, nil);
  }

  TYPE_REP_TypeRep nattype (mk_REP_NumericTypeRep (Int(NAT)));

  TYPE_CPP_Expr casting (IsIntType(tmpparType) ? tmppar_v
                                               : (vdm_CPP_isCPP() ? GenCastType(nattype, tmppar_v)
                                                                  : GetIntVal(tmppar_v)));
  if (IsIntType (tmpparType))
  {
    TYPE_CPP_Expr parapply (GenGetValue(casting, tmpparType));

    TYPE_CPP_Expr seqapply;
    if (vdm_CPP_isCPP()) {
      seqapply = GenSeqApply(restp, seqexpr, parapply) ;
    } else { // Java
      if (IsPossibleStringType(tmpseqType))
        seqapply = GenSeqApplyString(seqexpr, parapply) ;
      else
        seqapply = GenSeqApply(restp, seqexpr, parapply) ;
    }

    if (CheckSeqApply(mk_(seqexpr, casting)))
    {
      TYPE_CPP_Expr l_check;
      if (vdm_CPP_isCPP()) {
        l_check = vdm_BC_GenFctCallObjMemAcc(seqexpr, ASTAUX::MkId(L"OutOfRange"), mk_sequence(parapply));
      }
      else
      { // java
        TYPE_CPP_Expr l_lcheck (vdm_BC_GenGt(vdm_BC_GenIntegerLit(1), parapply));
        TYPE_CPP_Expr l_rcheck (vdm_BC_GenGt(parapply, IsPossibleStringType(tmpseqType) ? GenLenString_int(seqexpr)
                                                                           : GenLen_int(seqexpr)));
        l_check = vdm_BC_GenLogOr(vdm_BC_GenBracketedExpr(l_lcheck), vdm_BC_GenBracketedExpr(l_rcheck));
      }
      TYPE_CPP_Stmt l_then (vdm_BC_GenBlock(mk_sequence(RunTime(L"Illegal index"))));
      rb.ImpAppend(vdm_BC_GenIfStmt(l_check, l_then, nil));
    }

    rb.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, seqapply));
  }
  else
  {
    TYPE_CPP_Expr parapply (GenGetValue(casting, nattype));
    TYPE_CPP_Expr seqapply;
    if (vdm_CPP_isCPP())
      seqapply = GenSeqApply(restp, seqexpr, parapply) ;
    else
    { // java
      if (IsPossibleStringType(tmpseqType))
        seqapply = GenSeqApplyString(seqexpr, parapply) ;
      else
        seqapply = GenSeqApply(restp, seqexpr, parapply) ;
    }

    TYPE_CPP_Expr isnat (GenIsNat (tmppar_v));
    TYPE_CPP_Stmt rre (RunTime(L"An integer was expected in apply expression"));
    rb.ImpAppend (vdm_BC_GenIfStmt (isnat,
                                    vdm_BC_GenBlock(mk_sequence(vdm_BC_GenAsgnStmt (resVar_v, seqapply))),
                                    vdm_BC_GenBlock(mk_sequence(rre))));
  }
  return rb;
}

// GenIsNat
// name : CPP`Name
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenIsNat (const TYPE_CPP_Name & name)
{
  TYPE_REP_TypeRep ntype (mk_REP_NumericTypeRep (Int(NAT)));
  TYPE_CPP_Expr istype (GenIsType(name, ntype));
  TYPE_REP_BooleanTypeRep btype;
  return GenGetValue(istype, btype);
}

// CGFctApplyExpr
// ae : AS`ApplyExpr
// vt : CGMAIN`VT
// ==> seq of CPP`Stmt | CPP`Expr
Generic vdmcg::CGFctApplyExpr (const TYPE_AS_ApplyExpr & ae, const TYPE_CGMAIN_VT & vt)
{
  TYPE_AS_Expr expr               (ae.GetRecord(pos_AS_ApplyExpr_fct));
  const SEQ<TYPE_AS_Expr> & par_l (ae.GetSequence(pos_AS_ApplyExpr_arg));
  const TYPE_CI_ContextId & cid   (ae.GetInt(pos_AS_ApplyExpr_cid));

  switch (expr.GetTag()) {
    case TAG_TYPE_AS_FctTypeInstExpr: {
      expr = expr.GetRecord(pos_AS_FctTypeInstExpr_polyfct);
      // enter to case TAG_TYPE_AS_Name
    }
    case TAG_TYPE_AS_Name: {

      if (LookUpLocFct(expr))
      {
        SetNotSupportedException(true);
        return (Sequence().ImpAppend(NotSupported(L"function value or local function definitions", expr)));
      }
      else if (IsFctType( GetCI().GetTypeInfo(cid)))
      {
        SetNotSupportedException(true);
        return (SEQ<TYPE_CPP_Stmt>().ImpAppend(NotSupported(L"Higher order functions", expr)));
      }
      // Following ifdef'ed for VDMPP since it uses obj ref types
#ifdef VDMPP
      else if (vdm_CPP_isJAVA() && (expr.GetSequence(pos_AS_Name_ids).Length() > 1))
      {
        const TYPE_AS_Ids & ids         (expr.GetSequence(pos_AS_Name_ids));
        const TYPE_CI_ContextId & cidnm (expr.GetInt(pos_AS_Name_cid));

        TYPE_CPP_Expr obj;
        if (ids.Hd() == GiveCurCName())
          obj = GenThis();
        else if (IsSuperClass(GiveCurCASName(), ASTAUX::MkNameFromId(ids.Hd(), cidnm)))
          obj = GenSuper();
        else
          obj = vdm_BC_GenIdentifier(CleanIdentifier(ids.Hd()));

        TYPE_AS_Name mthdname (ASTAUX::MkNameFromIds(ids.Tl(), cidnm));
        Generic vtp;
        if (vt.GetField(pos_CGMAIN_VT_type).IsNil() ||
            (vt.GetRecord(pos_CGMAIN_VT_type).Is(TAG_TYPE_REP_UnitTypeRep)))
          vtp = Nil();
        else
          vtp = vt;

        TYPE_AS_Ids mids;
        mids.ImpAppend(ids.Hd());
        TYPE_REP_ObjRefTypeRep ti (mk_REP_ObjRefTypeRep(ASTAUX::MkNameFromIds(mids, cidnm)));
        TYPE_CGMAIN_VT objvt (mk_CG_VT(obj, ti));
        Tuple t (mk_(mthdname, par_l));
        return GenMethodInvoke(objvt, vtp, false, t, false);
      }
#endif //VDMPP
      else
      {
#ifdef VDMSL
        TYPE_CPP_Expr fctname (vdm_BC_Rename(expr));
#endif // VDMSL
#ifdef VDMPP
        TYPE_CPP_Expr fctname;
        if (vdm_CPP_isJAVA())
        {
          if (IsSuperClass(GiveCurCASName(), expr))
            fctname = GenSuper();
          else if (GiveCurCASName() == expr)
            fctname = GenThis();
          else
            fctname = vdm_BC_Rename(expr);
        }
        else
        { // C++
          fctname = vdm_BC_Rename(expr);
        }
#endif //VDMPP

        if (par_l.IsEmpty())
          return vdm_BC_GenFctCall(fctname, SEQ<TYPE_CPP_Expr>());
        else
        { // has parameters
          Generic tmptp (FindType(expr));
          Generic exprtp (tmptp.IsNil() ? tmptp : (Generic)RemoveInvType(tmptp));
#ifdef VDMPP
          if (exprtp.Is(TAG_TYPE_REP_OverTypeRep))
            exprtp = GetOverFNOP(exprtp, par_l);
#endif // VDMPP

          Generic fntype = nil; // [REP`RepType]
          Sequence fndom;       // seq of REP`RepType
          if (exprtp.IsRecord())
          {
            TYPE_REP_TypeRep etp (exprtp);
            switch (etp.GetTag ()) {
              case TAG_TYPE_REP_PartialFnTypeRep: {
                fntype = etp.GetRecord(pos_REP_PartialFnTypeRep_fnrng);
                fndom = etp.GetSequence(pos_REP_PartialFnTypeRep_fndom);
                break;
              }
              case TAG_TYPE_REP_TotalFnTypeRep: {
                fntype = etp.GetRecord(pos_REP_TotalFnTypeRep_fnrng);
                fndom = etp.GetSequence(pos_REP_TotalFnTypeRep_fndom);
                break;
              }
              case TAG_TYPE_REP_OpTypeRep: {
                fntype = etp.GetRecord(pos_REP_OpTypeRep_Rng);
                fndom = etp.GetSequence(pos_REP_OpTypeRep_Dom);
                break;
              }
            }
          }

          TYPE_CPP_TypeSpecifier gtype (vdm_BC_GenGeneric());
          SEQ<TYPE_CPP_Expr> var_l;
          SEQ<TYPE_CPP_Stmt> rb_l;
          size_t len_par_l = par_l.Length();
          for(size_t index = 1; index <= len_par_l; index++) 
          {
            const TYPE_AS_Expr & par (par_l[index]);
            Tuple cgee (CGExprExcl(par, ASTAUX::MkId(L"par"), nil));
            const TYPE_CPP_Expr & evalx (cgee.GetRecord(1));
            const SEQ<TYPE_CPP_Stmt> & eval_stmt (cgee.GetSequence(2));

            TYPE_CPP_Expr eval (evalx);
            if (vdm_CPP_isCPP()) {
              if (par.Is(TAG_TYPE_AS_ApplyExpr) && evalx.Is(TAG_TYPE_CPP_CastExpr)) {
                eval = evalx.GetRecord(pos_CPP_CastExpr_expr);
              }
              if (exprtp.IsRecord()) {
                switch (Record(exprtp).GetTag ()) {
                  case TAG_TYPE_REP_PartialFnTypeRep:
                  case TAG_TYPE_REP_TotalFnTypeRep:
                  case TAG_TYPE_REP_OpTypeRep: {
                    Generic gtp (FindType(par));
                    if (IsSubType(gtp, fndom[index])
#ifdef VDMPP
                        && !IsSuperClass(GiveCurCASName(), expr)
#endif // VDMPP
                       )
                    {
                      rb_l.ImpConc(eval_stmt);
                      var_l.ImpAppend(eval);
                    }
                    else if (GenType(gtp) == gtype) {
                      rb_l.ImpConc(eval_stmt);
                      var_l.ImpAppend(eval);
                    }
                    else
                    {
                      rb_l.ImpConc(eval_stmt);
                      Generic lgtp (LOT(gtp));
                      Generic ldomtp (LOT(fndom[index]));
                      if (IsSubType(lgtp, ldomtp) ||
                          (IsCompositeType(lgtp) && IsCompositeType(ldomtp)) ||
                          (IsSeqType(lgtp) && IsSeqType(ldomtp)) ||
                          (IsSetType(lgtp) && IsSetType(ldomtp)) ||
                          (IsIntType(lgtp) && IsIntType(ldomtp))
                         )
                        var_l.ImpAppend(eval);
                      else {
                        var_l.ImpAppend(vdm_BC_GenCastExpr(gtype, eval));
                      }
                    }
                    break;
                  }
                  default: {
                    rb_l.ImpConc(eval_stmt);
                    var_l.ImpAppend(vdm_BC_GenCastExpr(gtype, eval));
                    break;
                  }
                }
              }
              else {
                rb_l.ImpConc(eval_stmt);
                var_l.ImpAppend(vdm_BC_GenCastExpr(gtype, eval));
              }
            }
            else
            { // java
              if (exprtp.IsRecord()) {
                switch (Record(exprtp).GetTag ()) {
                  case TAG_TYPE_REP_PartialFnTypeRep:
                  case TAG_TYPE_REP_TotalFnTypeRep:
                  case TAG_TYPE_REP_OpTypeRep: {
                    TYPE_REP_TypeRep tp1 (fndom[index]);
                    TYPE_REP_TypeRep tp2 (RemoveNil(FindType(par)));
                    rb_l.ImpConc(eval_stmt);
                    var_l.ImpAppend(GenExplicitCast(tp1, eval, tp2));
                    break;
                  }
                  default: {
                    rb_l.ImpConc(eval_stmt);
                    var_l.ImpAppend(eval);
                    break;
                  }
                }
              }
              else {
                rb_l.ImpConc(eval_stmt);
                var_l.ImpAppend(eval);
              }
            }
          }
          TYPE_CPP_Expr fcCall (vdm_BC_GenFctCall(fctname, var_l));
          if (vdm_CPP_isCPP()) {
            if (rb_l.IsEmpty())
              return fcCall;
            else {
              const TYPE_CPP_Expr & resVar_v (vt.GetRecord(pos_CGMAIN_VT_name));
              rb_l.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, fcCall));
              return rb_l;
            }
          }
          else
          {
            const TYPE_REP_TypeRep & restype (vt.GetRecord(pos_CGMAIN_VT_type));
            TYPE_CPP_Expr cast (restype.Is(TAG_TYPE_REP_UnitTypeRep)
                                   ? fcCall
                                   : GenExplicitCast(restype, fcCall, fntype));
            if (rb_l.IsEmpty())
              return cast;
            else {
              const TYPE_CPP_Expr & resVar_v (vt.GetRecord(pos_CGMAIN_VT_name));
              rb_l.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, cast));
              return rb_l;
            }
          }
        }
      }
    } // end of case TAG_TYPE_AS_Name:
#ifdef VDMPP
    case TAG_TYPE_AS_FieldSelectExpr: {
      return CGApplyFieldSel (expr, par_l, vt);
    }
#endif // VDMPP
    default: {
      return CGExpr(expr, vt);
    }
  }
}

#ifdef VDMPP
// GetOverFNOP
// ovtp : REP`OverTypeRep
// par_l : seq of AS`Expr
// ==> [REP`PartialFnTypeRep | REP`TotalFnTypeRep | REP`OpTypeRep]
Generic vdmcg::GetOverFNOP(const TYPE_REP_OverTypeRep & ovtp, const SEQ<TYPE_AS_Expr> & par_l)
{
  SET<TYPE_REP_TypeRep> tp_s (ovtp.GetSet(pos_REP_OverTypeRep_tps));
  int len_par_l = par_l.Length();
  SET<TYPE_REP_TypeRep> tps;
  Generic tp_;
  for (bool bb = tp_s.First(tp_); bb; bb = tp_s.Next(tp_))
  {
    TYPE_REP_TypeRep tp (tp_);
    Sequence fdom;
    switch (tp.GetTag ()) {
      case TAG_TYPE_REP_PartialFnTypeRep: {
        if (tp.GetSequence(pos_REP_PartialFnTypeRep_fndom).Length() == len_par_l)
          tps.Insert(tp);
        break;
      }
      case TAG_TYPE_REP_TotalFnTypeRep: {
        if (tp.GetSequence(pos_REP_TotalFnTypeRep_fndom).Length() == len_par_l)
          tps.Insert(tp);
        break;
      }
      case TAG_TYPE_REP_OpTypeRep: {
        if (tp.GetSequence(pos_REP_OpTypeRep_Dom).Length() == len_par_l)
          tps.Insert(tp);
        break;
      }
    }
  }
  switch (tps.Card()) {
    case 0: {
      return Nil();
    }
    case 1: {
      return tps.GetElem();
    }
    default: {
      Generic tp_;
      for (bool bb = tps.First(tp_); bb; bb = tps.Next(tp_))
      {
        TYPE_REP_TypeRep tp (tp_);
        Sequence fdom;
        switch (tp.GetTag ()) {
          case TAG_TYPE_REP_PartialFnTypeRep: {
            fdom = tp.GetSequence(pos_REP_PartialFnTypeRep_fndom);
            break;
          }
          case TAG_TYPE_REP_TotalFnTypeRep: {
            fdom = tp.GetSequence(pos_REP_TotalFnTypeRep_fndom);
            break;
          }
          case TAG_TYPE_REP_OpTypeRep: {
            fdom = tp.GetSequence(pos_REP_OpTypeRep_Dom);
            break;
          }
        }
        bool forall = true;
        size_t len_fdom = fdom.Length();
        for (size_t i = 1; (i <= len_fdom) && forall; i++) {
          forall = IsSubType(FindType(par_l[i]), fdom[i]) ||
                   (IsIntType(FindType(par_l[i])) && IsIntType(fdom[i]));
        }
        if (forall)
          return tp;
      }
      return Nil();
    }
  }
}

// CGApplyFieldSel
// fexpr : AS`FieldSelectExpr
// par_l : seq of AS`Expr
// vt : CGMAIN`VT
// ==> seq of CPP`Stmt | CPP`Expr
SEQ<TYPE_CPP_Stmt> vdmcg::CGApplyFieldSel (const TYPE_AS_FieldSelectExpr & fexpr,
                                           const SEQ<TYPE_CPP_Expr> & par_l,
                                           const TYPE_CGMAIN_VT & vt)
{
  const TYPE_REP_TypeRep & tp (FindType (fexpr.GetRecord(pos_AS_FieldSelectExpr_rec)));

  switch (tp.GetTag ()) {
    case TAG_TYPE_REP_ObjRefTypeRep:
    case TAG_TYPE_REP_UnionTypeRep: {
      return CGObjRef (fexpr, par_l, vt);
    }
    default: {
      if (IsObjRefType (tp))
        return CGObjRef (fexpr, par_l, vt);
      else
        ReportError(L"CGApplyFieldSel");
      return SEQ<TYPE_CPP_Stmt>();  // To keep VC++ happy.
    }
  }
}

// CGObjRef
// fexpr : AS`FieldSelectExpr
// par_l : seq of AS`Expr
// vt : CGMAIN`VT
// ==> seq of CPP`Stmt | CPP`Expr
Generic vdmcg::CGObjRef (const TYPE_AS_FieldSelectExpr & fexpr,
                         const SEQ<TYPE_CPP_Expr> & par_l,
                         const TYPE_CGMAIN_VT & vt)
{
  const Generic & vt_type (vt.GetField(pos_CGMAIN_VT_type));

  Generic vt_q (vt);
  if (vt_type.IsNil () || (vt_type.Is (TAG_TYPE_REP_UnitTypeRep)))
    vt_q = Nil ();

  const TYPE_AS_Expr & rec (fexpr.GetRecord(pos_AS_FieldSelectExpr_rec));
  const Record & nm (fexpr.GetRecord(pos_AS_FieldSelectExpr_nm)); // Name | FctTypeInstExpr

  TYPE_AS_Name fexpr_nm (nm.Is(TAG_TYPE_AS_FctTypeInstExpr) ? nm.GetRecord(pos_AS_FctTypeInstExpr_polyfct) : nm);
  TYPE_CI_ContextId fexpr_cid (nm.Is(TAG_TYPE_AS_FctTypeInstExpr) ? fexpr_nm.get_cid() : fexpr.get_cid());

  switch (rec.GetTag ()) {
    case TAG_TYPE_AS_Name: {
      TYPE_CPP_Expr obj (vdm_BC_Rename (rec));
      TYPE_REP_TypeRep ti (FindType (rec));
      if (get_j2v_option() &&
          ti.Is(TAG_TYPE_REP_ObjRefTypeRep) &&
          ( IsAbstractClass(ti.GetRecord(pos_REP_ObjRefTypeRep_nm)) ||
            isInterface(ti.GetRecord(pos_REP_ObjRefTypeRep_nm))) &&
          fexpr_nm == ASTAUX::MkNameFromId(ASTAUX::MkId(L"IsNil"), NilContextId) &&
          par_l.IsEmpty())
      {
        TYPE_CPP_Expr eqExpr (vdm_BC_GenEq(obj, GenNullExpr()));
        TYPE_CPP_Expr isNilExpr (GenInvokeExpr(TYPE_REP_ObjRefTypeRep(ti),
                                               obj,
                                               vdm_BC_Rename(fexpr_nm),
                                               SEQ<TYPE_CPP_Expr>()));
        TYPE_CPP_Expr getValExpr (GenGetValue(isNilExpr, mk_REP_BooleanTypeRep()));
        TYPE_CPP_Expr orExpr (vdm_BC_GenLogOr(eqExpr, getValExpr));
        return GenBoolExpr(orExpr);
      }

      TYPE_CGMAIN_VT objvt (mk_CG_VT (obj,ti));

      TYPE_AS_Name mthdnm;
      if (vdm_CPP_isCPP())
        mthdnm = ASTAUX::MkNameFromIds(fexpr_nm.get_ids(), fexpr_cid);
      else
      { // java
        if (fexpr_nm.get_ids().Length() > 1)
        {
          mthdnm = ASTAUX::MkNameFromIds(fexpr_nm.get_ids().Tl(), fexpr_cid);

          GetStatSem().GenErr(fexpr_nm, StatSem::WRN1, 363, mk_sequence(SEQ<Char>(L"Quoted method call")));
        }
        else
          mthdnm = ASTAUX::MkNameFromIds(fexpr_nm.get_ids(), fexpr_cid);
      }
      return GenMethodInvoke (objvt, vt_q, false, mk_(mthdnm, par_l), false);
    } // end case TAG_TYPE_AS_Name
    case TAG_TYPE_AS_SelfExpr: {
      if (vdm_CPP_isCPP())
      {
        TYPE_AS_Name mthdnm (ASTAUX::MkNameFromIds(fexpr_nm.get_ids(), fexpr_cid));
        return GenMethodInvoke (Nil (), vt_q, false, mk_(mthdnm, par_l), false);
      }
      else
      { // for Java
        TYPE_AS_Ids ids (fexpr_nm.get_ids());
        TYPE_CPP_Identifier obj;
        if (ids.Length() > 1)
        {
          if (ids.Hd() == GiveCurCName())
            obj = GenThis();
          else {
            obj = GenSuper();
            GetStatSem().GenErr(fexpr_nm, StatSem::WRN1, 362, mk_sequence(SEQ<Char>(L"Quoted method call")));}
        }
        else
          obj = GenThis();

        TYPE_AS_Name mthdnm;
        if (fexpr_nm.get_ids().Length() > 1)
          mthdnm = ASTAUX::MkNameFromIds(fexpr_nm.get_ids().Tl(), fexpr_cid);
        else
          mthdnm = ASTAUX::MkNameFromIds(fexpr_nm.get_ids(), fexpr_cid);

        TYPE_REP_TypeRep ti (FindType (rec));
        TYPE_CGMAIN_VT objvt (mk_CG_VT (obj, ti));
        return GenMethodInvoke (objvt, vt_q, false, mk_(mthdnm, par_l), false);
      }
    } // end case TAG_TYPE_AS_SelfExpr
    default: {
      TYPE_AS_Name mthdnm (ASTAUX::MkNameFromIds(fexpr_nm.get_ids(), fexpr_cid));

      TYPE_REP_TypeRep ti (FindType (rec));
      Tuple cgres (CGExprExcl (rec, ASTAUX::MkId(L"tmpObj"), ti));
      const TYPE_CPP_Expr & tmpobj (cgres.GetRecord (1));
      const SEQ<TYPE_CPP_Stmt> & cgexpr (cgres.GetSequence (2));

      if (vdm_CPP_isJAVA() && rec.Is(TAG_TYPE_AS_NewExpr))
      {
        TYPE_CGMAIN_VT objvt (mk_CG_VT (tmpobj, ti));

        SEQ<TYPE_CPP_Stmt> dos;
        dos.ImpConc (cgexpr);
        dos.ImpConc (GenMethodInvoke (objvt, vt_q, false, mk_(mthdnm, par_l), false));
        return dos;
      }
      else
      {
        TYPE_CPP_Identifier obj (vdm_BC_GiveName(ASTAUX::MkId(L"obj")));
// 20140801 -->
        //SEQ<TYPE_CPP_Stmt> declObj (GenDecl_DS (ti, obj, vdm_BC_GenAsgnInit(tmpobj)));
        SEQ<TYPE_CPP_Stmt> declObj (GenDeclInit_DS (ti, obj, tmpobj));
        //TYPE_CPP_DeclarationStmt declObj (vdm_CPP_isCPP()
        //             ? GenDecl_DS (ti, obj, vdm_BC_GenObjectInit(mk_sequence(tmpobj)))
        //             : GenDecl_DS (ti, obj, vdm_BC_GenAsgnInit(tmpobj)));
// <-- 20140801
        TYPE_CGMAIN_VT objvt (mk_CG_VT (obj, ti));

        SEQ<TYPE_CPP_Stmt> dos;
        dos.ImpConc (cgexpr);
        dos.ImpConc (declObj);
        dos.ImpConc (GenMethodInvoke (objvt, vt_q, false, mk_(mthdnm, par_l), false));
        return dos;
      }
    }
  }
}

// GenMethodInvoke
// obj : [CGMAIN`VT]
// res : [CGMAIN`VT]
// res_decl : bool
// mi : (AS`Name * seq of AS`Expr)
// last : bool
// -> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenMethodInvoke(const Generic & obj,
                                          const Generic & res,
                                          const Bool & res_decl,
                                          const Tuple & mi,
                                          const Bool & last)
{
//  bool res_decl_b(res_decl.GetValue());
  if( obj.IsNil() )
    return GenSimpleInvoke(obj, res, res_decl, mi, last);
  else {
    TYPE_CGMAIN_VT o (obj);
    const TYPE_REP_TypeRep & oti ( o.GetRecord(pos_CGMAIN_VT_type) );
    if ( oti.Is(TAG_TYPE_REP_ObjRefTypeRep) )
      return GenSimpleInvoke(o, res, res_decl, mi, last);
    else if (vdm_CPP_isJAVA() && RemoveNil(oti).Is(TAG_TYPE_REP_ObjRefTypeRep))
    {
      TYPE_CGMAIN_VT cg_vt (mk_CG_VT(o.GetRecord(pos_CGMAIN_VT_name), RemoveNil(oti)));
      return GenSimpleInvoke(cg_vt, res, res_decl, mi, last);
    }
    else
      return GenUnionInvoke(o, res, res_decl, mi, last);
  }
}

// GenSimpleInvoke
// obj : [CGMAIN`VT]
// res : [CGMAIN`VT]
// res_decl : bool
// mi : (AS`Name * seq of AS`Expr)
// last : bool
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenSimpleInvoke(const Generic & obj,
                                          const Generic & res,
                                          const Bool & res_decl,
                                          const Tuple & mi,
                                          const Bool & last)
{
  const TYPE_AS_Name & mthd (mi.GetRecord(1)); // AS`Name
  Tuple tmp (GenParSequence(mi.GetField (2), FindType(mthd)));
  SEQ<TYPE_CPP_Stmt> pdecl_l (tmp.GetSequence(1));
  const SEQ<TYPE_CPP_Expr> & id_l (tmp.GetSequence(2));

  SEQ<TYPE_CPP_Stmt> check;
  TYPE_CPP_Expr fcall;
  if (! obj.IsNil() )
  {
    TYPE_CGMAIN_VT vt (obj);
    const TYPE_CPP_Expr & id (vt.GetRecord(pos_CGMAIN_VT_name));
    const TYPE_REP_TypeRep & ti (vt.GetRecord(pos_CGMAIN_VT_type));

    TYPE_CPP_Expr id_q (IsObjRefType(ti) ? id : vdm_BC_GenCastExpr(GenObjRefType_DS(nil), id));

// 20120216 -->
    //fcall = GenInvokeExpr(TYPE_REP_ObjRefTypeRep(ti), id_q, vdm_BC_Rename (mthd), id_l);
    TYPE_CPP_Identifier mthdid (vdm_BC_Rename (mthd));
    if (vdm_CPP_isJAVA() && ti.Is(TAG_TYPE_REP_ObjRefTypeRep))
    {
      TYPE_AS_Id clnm (ASTAUX::GetFirstId(ti.GetRecord(pos_REP_ObjRefTypeRep_nm)));
      if (clnm.SubSequence(1,4) == SEQ<Char>(L"JDK_"))
      {
        TYPE_AS_Id mid (CleanIdentifier(ASTAUX::GetFirstId(mthd)));
        if (mid.SubSequence(1,4) == SEQ<Char>(L"vdm_"))
        {
          mthdid = vdm_BC_GenIdentifier(mid.SubSequence(5, mid.Length()));
        }
// 20121106 -->
        SEQ<TYPE_CPP_Expr> id_l_q (id_l);
        if (clnm == SEQ<Char>(L"JDK_Calendar"))
        {
          if ((mid == SEQ<Char>(L"add")) ||
              (mid == SEQ<Char>(L"set")) ||
              (mid == SEQ<Char>(L"get")))
          {
            size_t len_id_l = id_l.Length();
            for (size_t idx = 1; idx <= len_id_l; idx++)
            {
              TYPE_CPP_Expr expr (GenGetValue(id_l[idx], mk_REP_NumericTypeRep(Int(INTEGER))));
              id_l_q.ImpModify(idx, expr);
            }
          }
        }
        fcall = GenInvokeExpr(ti, id_q, mthdid, id_l_q);
// <-- 20121106
      }
      else
      {
        fcall = GenInvokeExpr(ti, id_q, mthdid, id_l);
      }
    }
    else
    {
      fcall = GenInvokeExpr(ti, id_q, mthdid, id_l);
    }
// <-- 20120216

    if (vdm_CPP_isCPP())
    {
      Generic exprtp (FindType(mthd));
      if (exprtp.Is(TAG_TYPE_REP_OverTypeRep))
        exprtp = GetOverFNOP(exprtp, mi.GetSequence(2));
      Sequence fndom;       // seq of REP`RepType
      if (exprtp.IsRecord())
      {
        TYPE_REP_TypeRep etp (exprtp);
        switch (etp.GetTag ()) {
          case TAG_TYPE_REP_PartialFnTypeRep: {
            fndom = etp.GetSequence(pos_REP_PartialFnTypeRep_fndom);
            break;
          }
          case TAG_TYPE_REP_TotalFnTypeRep: {
            fndom = etp.GetSequence(pos_REP_TotalFnTypeRep_fndom);
            break;
          }
          case TAG_TYPE_REP_OpTypeRep: {
            fndom = etp.GetSequence(pos_REP_OpTypeRep_Dom);
            break;
          }
        }
      }
      if(!fndom.IsEmpty() &&
        (fndom.Length() == mi.GetSequence(2).Length()) &&
        (fndom.Length() == fcall.GetSequence(pos_CPP_FctCall_arg).Length()))
      {
        const SEQ<TYPE_AS_Expr> e_l (mi.GetSequence(2));
        const SEQ<TYPE_CPP_Expr> & arg (fcall.GetSequence(pos_CPP_FctCall_arg));
        SEQ<TYPE_CPP_Expr> new_arg;
        size_t len_arg = arg.Length();
        for (size_t idx = 1; idx <= len_arg; idx++)
        {
          const TYPE_CPP_Expr & e (arg[idx]);
          Generic ti (FindType(e_l[idx]));
          if (e.Is(TAG_TYPE_CPP_CastExpr) && ti.IsRecord() && IsSubType(ti, fndom[idx]))
            new_arg.ImpAppend(e.GetRecord(pos_CPP_CastExpr_expr));
          else
            new_arg.ImpAppend(e);
        }
        fcall.SetField(pos_CPP_FctCall_arg, new_arg);
      }
    }
  }
  else {
    fcall = vdm_BC_GenFctCall(vdm_BC_Rename (mthd), id_l);
  }

  if (vdm_CPP_isJAVA())
  {
    if (!res.IsNil())
    {
      TYPE_CGMAIN_VT res_v (res);
      const Generic & tp (res_v.GetField(pos_CGMAIN_VT_type));
      Generic exprtp (FindType(mthd));
      if (exprtp.Is(TAG_TYPE_REP_OverTypeRep))
        exprtp = GetOverFNOP(exprtp, mi.GetSequence(2));
      Generic fnrng = Nil();
      if (exprtp.IsRecord())
      {
        TYPE_REP_TypeRep etp (exprtp);
        switch (etp.GetTag ()) {
          case TAG_TYPE_REP_PartialFnTypeRep: {
            fnrng = RemoveNil(CleanFlatType(etp.GetSequence(pos_REP_PartialFnTypeRep_fnrng)));
            break;
          }
          case TAG_TYPE_REP_TotalFnTypeRep: {
            fnrng = RemoveNil(CleanFlatType(etp.GetSequence(pos_REP_TotalFnTypeRep_fnrng)));
            break;
          }
          case TAG_TYPE_REP_OpTypeRep: {
            fnrng = RemoveNil(CleanFlatType(etp.GetSequence(pos_REP_OpTypeRep_Rng)));
            break;
          }
        }
      }
      fcall = GenExplicitCast(tp, fcall, fnrng);

      // hack TODO: to check object is JDK_ class
      SEQ<Char> mnm (mi.GetRecord(1).GetSequence(pos_AS_Name_ids)[1]);
// 20121219 -->
      if (! obj.IsNil() )
      {
        TYPE_CGMAIN_VT vt (obj);
        const TYPE_REP_TypeRep & ti (vt.GetRecord(pos_CGMAIN_VT_type));
        if (ti.Is(TAG_TYPE_REP_ObjRefTypeRep))
        {
          TYPE_AS_Id clnm (ASTAUX::GetFirstId(ti.GetRecord(pos_REP_ObjRefTypeRep_nm)));
          fcall = CheckJavaPrimitiveType(fcall, clnm, mnm);
        }
      }
      else
      { // TODO: 
        TYPE_AS_Id clnm (ASTAUX::GetFirstId(GiveCurCASName()));
        fcall = CheckJavaPrimitiveType(fcall, clnm, mnm);
      }
// <-- 20121219
    }
  }
// 20120313 -->
  else
  {
    Generic exprtp (FindType(mthd));
    if (exprtp.IsRecord())
    {
      Generic fnrng = Nil();
      TYPE_REP_TypeRep etp (exprtp);
// 20120718 -->
      if (etp.Is(TAG_TYPE_REP_OverTypeRep))
        etp = GetOverFNOP(etp, mi.GetSequence(2));
// <-- 20120718
      switch (etp.GetTag ()) {
        case TAG_TYPE_REP_PartialFnTypeRep: {
          fnrng = etp.GetSequence(pos_REP_PartialFnTypeRep_fnrng);
          break;
        }
        case TAG_TYPE_REP_TotalFnTypeRep: {
          fnrng = etp.GetSequence(pos_REP_TotalFnTypeRep_fnrng);
           break;
        }
        case TAG_TYPE_REP_OpTypeRep: {
          fnrng = etp.GetSequence(pos_REP_OpTypeRep_Rng);
          break;
        }
      }
      // fix for Map
      if (!fnrng.IsNil())
      {
        if (IsMapType(fnrng))
        {
          //fcall = GenCastMap(fcall);
          fcall = vdm_BC_GenFctCall(GenMapType().get_tp(), mk_sequence(fcall));
        }
      }
    }
  }
// <-- 20120313

  SEQ<TYPE_CPP_Stmt> stmts( GenInvoke(res, res_decl, fcall, last) );

  if (!id_l.IsEmpty()) {
    pdecl_l.ImpConc(check);
    pdecl_l.ImpConc(stmts);
    return pdecl_l;
  }
  else {
    SEQ<TYPE_CPP_Stmt> rb_l;
    rb_l.ImpConc(check);
    rb_l.ImpConc(stmts);
    return rb_l;
  }
}

// GenUnionInvoke
// vt : CGMAIN`VT
// res : [CGMAIN`VT]
// res_decl : bool
// mi : (AS`Name * seq of AS`Expr)
// last : bool
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenUnionInvoke(const TYPE_CGMAIN_VT & vt,
                                         const Generic & res,
                                         const Bool & res_decl,
                                         const Tuple & mi,
                                         const Bool & last)
{
  const TYPE_CPP_Expr & oid (vt.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_UnionTypeRep & oti (vt.GetRecord(pos_CGMAIN_VT_type));
  const TYPE_AS_Name & mthd (mi.GetRecord(1));

  Tuple tmp (GenParSequence(mi.GetSequence (2), FindType(mthd)));
  SEQ<TYPE_CPP_Stmt> pdecl_l (tmp.GetSequence(1)); // modified
  const SEQ<TYPE_CPP_Expr> & id_l (tmp.GetSequence(2));

  SET<TYPE_REP_TypeRep> t_s (oti.GetSet(pos_REP_UnionTypeRep_tps));

  SEQ<TYPE_CPP_Stmt> rv_d;
  if (!res.IsNil() && res_decl.GetValue())
  {
    TYPE_CGMAIN_VT res_v (res);
    const TYPE_CPP_Expr & resVar (res_v.GetRecord(pos_CGMAIN_VT_name));
    const Generic & resType (res_v.GetField(pos_CGMAIN_VT_type));
    rv_d.ImpConc(GenDecl_DS(resType, resVar, nil));
  }

  SET<TYPE_REP_ObjRefTypeRep> ti_s;
  SET<TYPE_AS_Name> nm_s;
  //SET<TYPE_AS_Name> tinm_s;
  Generic ti;
  for (bool bb = t_s.First(ti); bb; bb = t_s.Next(ti))
  {
    if (ti.Is(TAG_TYPE_REP_ObjRefTypeRep))
    {
      TYPE_REP_ObjRefTypeRep ortr (ti);
      ti_s.Insert(ortr);

      if (vdm_CPP_isJAVA())
        ortr = UnQClassType(ortr);

      const TYPE_AS_Name & nm (ortr.GetRecord(pos_REP_ObjRefTypeRep_nm));
      //tinm_s.Insert(nm);
      Generic luofn (GetStatSem().LookUpOpOrFnName(nm, mthd));
                // [REP_OpTypeRep || REP`TotalFnTypeRep || REP`PartialFnTypeRep]
      if ( !luofn.IsNil() && IsInClassNamespace(nm, mthd))
      {
// 20100104 -->
// TODO: arguments must be checked 
//        nm_s.Insert(nm);
        if (luofn.IsRecord())
        {
          Sequence fndom;
          TYPE_REP_TypeRep etp (luofn);
          switch (etp.GetTag ()) {
            case TAG_TYPE_REP_PartialFnTypeRep: {
              fndom = etp.GetSequence(pos_REP_PartialFnTypeRep_fndom);
              break;
            }
            case TAG_TYPE_REP_TotalFnTypeRep: {
              fndom = etp.GetSequence(pos_REP_TotalFnTypeRep_fndom);
              break;
            }
            case TAG_TYPE_REP_OpTypeRep: {
              fndom = etp.GetSequence(pos_REP_OpTypeRep_Dom);
              break;
            }
          }
          const SEQ<TYPE_AS_Expr> & arg_l (mi.GetSequence(2));
          if (fndom.Length() == arg_l.Length())
          {
            size_t len_arg_l = arg_l.Length();
            bool forall = true;
            for (size_t idx = 1; (idx <= len_arg_l) && forall; idx++) {
              forall = GetStatSem().IsOverlapping(FindType(arg_l[idx]), fndom[idx], Set());
            }
            if (forall)
              nm_s.Insert(nm);
          }
        }
        else if (luofn.IsSet())
        {
          // TODO: for overload
          nm_s.Insert(nm);
        }
// <-- 20100104
      }
    }
  }

  if (nm_s.IsEmpty())
  {
    return SEQ<TYPE_CPP_Stmt>().ImpAppend(RunTime(L"Cannot apply or invoke operation"));
  }

  TYPE_CPP_Expr oid_ (oid);
  if (!IsObjRefType(oti))
  {
// 20131202 -->
    //if (vdm_CPP_isCPP())
    if (vdm_CPP_isCPP() && (GenType(oti) != vdm_BC_GenGeneric()))
// <-- 20131202
      oid_ = vdm_BC_GenCastExpr(GenObjRefType_DS(nil), oid);
    else
      oid_ = oid;
  }

  TYPE_CPP_Expr mthd2 (vdm_BC_Rename(mthd));

  Sequence alts_l;  // seq of (CPP`Expr, CPP`Stmt)
  // `for all nm in set nm_s do' part
  Generic nm;
  for (bool cc = nm_s.First(nm); cc; cc = nm_s.Next(nm))
  {
    TYPE_CPP_Expr cast (CastToClassPtr(nm, oid_));

    //  // In addition to the specification:
    //  Record df( ss->LookUpDefClassForMethod(nm,mthd) );
    //  Record mthd2( vdm_BC_GenQualifiedName(vdm_BC_Rename(df), vdm_BC_Rename(mthd)) );

    if (vdm_CPP_isCPP())
    {
// 20120605 -->
//      TYPE_CPP_Expr fcall (vdm_BC_GenGenericFctCallPtrToObjMemAcc(cast, mthd2, id_l));
      TYPE_CPP_Expr fcall (vdm_BC_GenFctCallPtrToObjMemAcc(cast, mthd2, id_l));
// <-- 20120605
      SEQ<TYPE_CPP_Stmt> estmt (GenInvoke(res, false, fcall, last));
// 20120525 -->
      //alts_l.ImpAppend( mk_(GenAuxType(oid, mk_REP_ObjRefTypeRep(nm)), estmt) );
      if (IsObjRefType(oti))
        alts_l.ImpAppend( mk_(GenIsOfClassExpr(nm, oid), estmt) );
      else
        alts_l.ImpAppend( mk_(GenAuxType(oid, mk_REP_ObjRefTypeRep(nm)), estmt) );
// <-- 20120525
    }
    else
    {
      TYPE_CPP_Expr isofclass (GenIsThisClass(nm, oid_));
      TYPE_CPP_Expr fcalltemp (vdm_BC_GenFctCall(vdm_BC_GenObjectMemberAccess(cast, mthd2), id_l));

      TYPE_CPP_Expr fcall;
      if (!res.IsNil()) {
        TYPE_CGMAIN_VT res_v (res);
        const Generic & tp (res_v.GetField(pos_CGMAIN_VT_type));
        fcall = GenExplicitCast(tp, fcalltemp, FindType(mthd));
      }
      else
        fcall = fcalltemp;

      SEQ<TYPE_CPP_Stmt> estmt (GenInvoke(res, false, fcall, last));
      alts_l.ImpAppend( mk_(isofclass, estmt) );
    }
  }

// 20120721 -->
  Set t_s_q (t_s.Diff(mk_set(mk_REP_NilTypeRep())));
// <-- 20120721
  SEQ<TYPE_CPP_Stmt> body;
  int64_t card = ti_s.Card();
// 20120721 -->
//  if ((card == 1) && (t_s.Card() == 1)) {
  if ((card == 1) && (t_s_q.Card() == 1)) {
// <-- 20120721
    Tuple alts (alts_l.Hd());
    //body = SEQ<TYPE_CPP_Stmt>().ImpAppend(tmp2.GetField(2));
    body.ImpConc(alts.GetSequence(2));
  }
  else
  {
    TYPE_CPP_Stmt err (IsPosCompositeType (oti) ? RunTime(L"Cannot apply or invoke operation")
                                                : RunTime(L"Cannot invoke operation in a VDM value"));

    Generic body1 = nil;
// 20120509 -->
// 20120721 -->
//    if (t_s.Card() != card)
    if (t_s_q.Card() != card)
// <-- 20120721
    {
      body1 = err;
    }
// <--20120509
    if (card != nm_s.Card())
    {
      wstring m (ASTAUX::Id2String(GiveLastName(mthd)));
      if (vdm_CPP_isCPP())
        body1 = RunTime(L"No member method vdm_" + m);
      else
        body1 = RunTime(L"No member method " + m);
    }
    size_t len_alts_l = alts_l.Length();
// 20120625 -->
    //for(size_t idx = 1; idx <= len_alts_l; idx++)
    for(size_t idx = len_alts_l; idx >= 1; idx--)
// <-- 20120625
    {
      const Tuple & alts (alts_l[idx]);
      const TYPE_CPP_Expr & cond (alts.GetRecord(1));
      const SEQ<TYPE_CPP_Stmt> & stmts (alts.GetSequence(2));

      TYPE_CPP_Stmt stmt (stmts.Length() == 1 ? stmts[1] : vdm_BC_GenBlock (stmts));
// 20120625 -->
      if ((idx == len_alts_l) && body1.IsNil())
        body1 = stmt;
      else
// <-- 20120625
      body1 = vdm_BC_GenIfStmt(cond, stmt, body1);
    }
    body = SEQ<TYPE_CPP_Stmt>().ImpAppend(body1);
  }

// 20120509 -->
/*
  if (t_s.Card() != card)
  {
    TYPE_CPP_Expr cond (vdm_CPP_isCPP() ? GenIsClass(oid) : GenIsClasses(tinm_s, oid));

    TYPE_CPP_Stmt err (IsPosCompositeType (oti) ? RunTime(L"Cannot apply or invoke operation")
                                                : RunTime(L"Cannot invoke operation in a VDM value"));

    TYPE_CPP_Stmt blbd ((body.Length () == 1) ? body.Hd () : vdm_BC_GenBlock (body));

    body = SEQ<TYPE_CPP_Stmt>().ImpAppend(vdm_BC_GenIfStmt(cond, blbd, err));
  }
*/
// <-- 20120509

  pdecl_l.ImpConc(rv_d);
  pdecl_l.ImpConc(body);
  return pdecl_l;
}

// GenParSequence
// arg_l : seq of AS`Expr
// tp : [REP`TypeRep]
// ==> seq of CPP`Stmt * seq of CPP`Expr
Tuple vdmcg::GenParSequence(const SEQ<TYPE_CPP_Expr> & arg_l, const Generic & tp)
{
  if ( !arg_l.IsEmpty() )
  {
    SEQ<TYPE_CPP_Stmt> stmt_l;
    SEQ<TYPE_CPP_Expr> id_l;
    size_t len_arg_l = arg_l.Length();
    for(size_t idx = 1; idx <= len_arg_l; idx++)
    {
      const TYPE_CPP_Expr & expr (arg_l[idx]);
      Tuple cgee (CGExprExcl(expr, ASTAUX::MkId(L"par"), nil));
      const TYPE_CPP_Expr & par_v (cgee.GetRecord(1));
      const SEQ<TYPE_CPP_Stmt> & par_stmt (cgee.GetSequence(2));

      if (vdm_CPP_isCPP())
      {
        stmt_l.ImpConc(par_stmt);
        id_l.ImpAppend(par_v);
      }
      else // for Java
      {
        if (tp.Is(TAG_TYPE_REP_OpTypeRep))
        {
          TYPE_REP_OpTypeRep otr (tp);
          const SEQ<TYPE_REP_TypeRep> & Dom (otr.GetSequence(pos_REP_OpTypeRep_Dom));
          //const TYPE_REP_TypeRep & Rng (otr.GetRecord(pos_REP_OpTypeRep_Rng));
          TYPE_REP_TypeRep cft (CleanFlatType(Dom[idx]));
          TYPE_REP_TypeRep tp1;
          if ( cft.Is(TAG_TYPE_REP_ObjRefTypeRep) )
            tp1 = UnQClassType (cft);
          else
            if (Dom[idx].Is(TAG_TYPE_REP_TypeNameRep) && cft.Is(TAG_TYPE_REP_UnionTypeRep) && IsCompositeType(cft))
              tp1 = Dom[idx];
            else
              tp1 = cft;

          TYPE_REP_TypeRep tp2 (FindType(expr));
          stmt_l.ImpConc(par_stmt);
// 20120904 -->
          //id_l.ImpAppend(GenExplicitCast(tp1, par_v, tp2));
          id_l.ImpAppend(GenExplicitCast(RemoveNil(tp1), par_v, RemoveNil(tp2)));
// <-- 20120904
        }
        else
        {
          stmt_l.ImpConc(par_stmt);
          id_l.ImpAppend(par_v);
        }
      }
    }
    return mk_(stmt_l, id_l);
  }
  else
    return mk_(SEQ<TYPE_CPP_Stmt>(), SEQ<TYPE_CPP_Expr>());
}

// GenInvoke
// res : [CGMAIN`VT]
// res_decl : bool
// fcall : CPP`Expr
// last : bool
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenInvoke(const Generic & res,
                               const Bool & res_decl,
                               const TYPE_CPP_Expr & fcall,
                               const Bool & last)
{
  if ( res.IsNil() ) {
    //return vdm_BC_GenExpressionStmt(fcall);
    SEQ<TYPE_CPP_Stmt> rb_l;
    rb_l.ImpAppend(vdm_BC_GenExpressionStmt(fcall));
    return rb_l;
  }
  else if (last)
  {
    if (rti) {
      SEQ<TYPE_CPP_Stmt> rb_l;
      rb_l.ImpAppend(GenPopFile());
      rb_l.ImpAppend(GenPopPosInfo());

      if (vdm_CPP_isCPP())
        rb_l.ImpAppend(vdm_BC_GenGenericReturnStmt(fcall));
      else
        rb_l.ImpAppend(vdm_BC_GenReturnStmt(fcall));

      //return vdm_BC_GenBlock(rb_l);
      return rb_l;
    }
    else {
      SEQ<TYPE_CPP_Stmt> rb_l;
      rb_l.ImpAppend(vdm_BC_GenGenericReturnStmt(fcall));
      return rb_l;
    }
  }
  else
  {
    TYPE_CGMAIN_VT r (res);
    const TYPE_CPP_Expr & id (r.GetRecord(pos_CGMAIN_VT_name));
    const TYPE_REP_TypeRep & idtype (r.GetRecord(pos_CGMAIN_VT_type));

    SEQ<TYPE_CPP_Stmt> rb_l;
    if (res_decl)
      rb_l.ImpConc(GenDeclInit_DS(idtype, id, fcall));
    else
      rb_l.ImpAppend(vdm_BC_GenAsgnStmt(id, fcall));
    return rb_l;
  }
}
#endif // VDMPP

// CGUnionTypeApply
// applexpr : AS`ApplyExpr
// rv : CGMAIN`VT
// expr1 : [CPP`Name]
// type1 : [REP`TypeRep]
// sd : bool
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGUnionTypeApply (const TYPE_AS_ApplyExpr & applexpr,
                                            const TYPE_CGMAIN_VT & rv,
                                            const Generic & expr1,
                                            const Generic & type1,
                                            bool sd)
{
  const TYPE_AS_Expr & expr (applexpr.GetRecord(pos_AS_ApplyExpr_fct));
  const SEQ<TYPE_AS_Expr> & par_l (applexpr.GetSequence(pos_AS_ApplyExpr_arg));
  const TYPE_CPP_Expr & resVar_v (rv.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & restp (rv.GetRecord(pos_CGMAIN_VT_type));

  TYPE_CPP_Expr tmpGen_v;
  SEQ<TYPE_CPP_Stmt> e_stmt;
  TYPE_REP_TypeRep tmpGenType;
  if( expr1.IsNil() )
  {
    Tuple tmp1 (CGExprExcl(expr, ASTAUX::MkId(L"tmpGen"),nil));
    TYPE_CPP_Expr e (tmp1.GetRecord(1));
    e_stmt.ImpConc(tmp1.GetSequence(2));
    tmpGenType = FindType(expr);
    if (!e.Is(TAG_TYPE_CPP_Identifier))
    {
      TYPE_CPP_Identifier temp (vdm_BC_GenIdentifier(ASTAUX::MkId(L"temp")));
      e_stmt.ImpAppend(vdm_BC_GenDecl(GenType(tmpGenType), temp, vdm_BC_GenAsgnInit(e)));
      e = temp;
    }
    tmpGen_v = e;
  }
  else
  {
    tmpGen_v = expr1;
    tmpGenType = type1;
  }

#ifdef VDMPP
  size_t lngt = e_stmt.Length ();
  if (expr.Is (TAG_TYPE_AS_FieldSelectExpr) && IsUnionFunctionType (tmpGenType) && (lngt >= 1))
    e_stmt.RemElem (lngt);
#endif // VDMPP

  SEQ<TYPE_CPP_Stmt> rb;
  rb.ImpConc(e_stmt);

  TYPE_CPP_Identifier tmpRes_v (vdm_BC_GiveName(ASTAUX::MkId(L"tmpRes")));

  Tuple fpmsc (tmpGenType.Is(TAG_TYPE_REP_UnionTypeRep) ? FindPosMapSeqComb (tmpGenType)
                                                        : mk_(Nil(), Nil(), Nil(), Nil()));
  const Generic & mapgens (fpmsc.GetField (1)); // [ REP`GeneralMapTypeRep ]
  const Generic & mapinjs (fpmsc.GetField (2)); // [ REP`InjectiveMapTypeRep]
  const Generic & seqs (fpmsc.GetField (3));    // [ REP`SeqTypeRep ]
  const Generic & strings (fpmsc.GetField (4)); // [ REP`SeqTypeRep ]

  TYPE_CPP_Stmt alt2 (vdm_BC_GenBlock(mk_sequence(RunTime (L"The argument could not be applied"))));

  if (par_l.IsEmpty())
  {
    Generic apex (CGFctApplyExpr(applexpr, rv)); // seq of CPP`Stmt | CPP`Expr
    if ( apex.IsSequence() )
    {
      // seq of CPP`Stmt
      SEQ<TYPE_CPP_Stmt> stmts (apex);
      if ( stmts.Length() == 1 )
        alt2 = stmts.Hd();
      else
        alt2 = vdm_BC_GenBlock(stmts);
    }
    else // apex.IsRecord()
    { // CPP`Expr
        alt2 = (restp.Is(TAG_TYPE_REP_UnitTypeRep) ? vdm_BC_GenExpressionStmt(apex)
                                                   : vdm_BC_GenAsgnStmt(resVar_v, apex));
    }
  }

  // FIXME: Change from spec
  if (!par_l.IsEmpty())
  {
    Generic parType (FindType (par_l.Hd ()));

    Tuple cgee (CGExprExcl(par_l.Hd (), ASTAUX::MkId(L"par"), nil));
    const TYPE_CPP_Expr & par_v (cgee.GetRecord(1));
    const SEQ<TYPE_CPP_Stmt> & par_stmt(cgee.GetSequence(2));

    rb.ImpConc(par_stmt);
#ifdef VDMPP
    if (IsUnionFunctionType (tmpGenType))
    {
      Generic apex (CGFctApplyExpr (applexpr, rv));
      if ( apex.IsSequence() ) // 20070220
      {
        // seq of CPP`Stmt
        SEQ<TYPE_CPP_Stmt> stmts (apex);
        if ( stmts.Length() == 1 )
          alt2 = stmts.Hd ();
        else
          alt2 = vdm_BC_GenBlock (apex);
      }
      else // apex/IsRecord()
      { // CPP`Expr
        alt2 = (restp.Is(TAG_TYPE_REP_UnitTypeRep) ? vdm_BC_GenExpressionStmt(apex)
                                                   : vdm_BC_GenAsgnStmt(resVar_v, apex));
      }
    }
#endif // VDMPP

    if (!mapgens.IsNil ())
      alt2 = GenUTMapApply(mapgens, tmpRes_v, tmpGen_v, par_v, alt2, sd, resVar_v);

    if (!mapinjs.IsNil ())
      alt2 = GenUTMapApply(mapinjs, tmpRes_v, tmpGen_v, par_v, alt2, sd, resVar_v);

    //    if (!seqs.IsNil ()){   If not JAVA and "strings" is not nil,
    //                           we should proceed as if we have seq of char.
    //                           ML 22.07.2002
// 20111221 -->
    //if (!seqs.IsNil () || (vdm_CPP_isCPP() && !strings.IsNil ()))
    if ((!seqs.IsNil () || (vdm_CPP_isCPP() && !strings.IsNil ())) && IsPosNumType(parType))
// <-- 20111221
    {
      SEQ<TYPE_CPP_Stmt> sq;
      if (vdm_CPP_isCPP())
        sq.ImpConc (GenDecl_DS (seqs, tmpRes_v, vdm_BC_GenAsgnInit (tmpGen_v)));
      else
        sq.ImpConc (GenDecl_DS (seqs, tmpRes_v, vdm_BC_GenAsgnInit(GenCastSeq(tmpGen_v,nil))));

      TYPE_REP_TypeRep nattype (mk_REP_NumericTypeRep (Int(NAT)));
      Generic parapply (IsNumType (parType) ? GenGetValue (par_v, parType)
                                            : GenGetValue (GenCastType(nattype, par_v), nattype));

      if (vdm_CPP_isCPP())
        sq.ImpAppend (vdm_BC_GenAsgnStmt (resVar_v,
                                          GenSeqApply(nil, GenCastType(mk_REP_EmptySeqTypeRep(mk_REP_AllTypeRep()),
                                                                       tmpRes_v),
                                                      parapply)));
      else
        sq.ImpAppend (vdm_BC_GenAsgnStmt (resVar_v, GenCastType(restp,GenSeqApply(nil, tmpRes_v, parapply))));

      alt2 = vdm_BC_GenIfStmt (GenIsSeq(tmpGen_v), vdm_BC_GenBlock (sq), alt2);
    }

    if (!strings.IsNil ())
    {
      if (vdm_CPP_isJAVA())
      {
        SEQ<TYPE_CPP_Stmt> sq;
        sq.ImpConc(GenDecl_DS(strings, tmpRes_v, vdm_BC_GenAsgnInit(GenCastString(tmpGen_v))));

        TYPE_REP_TypeRep nattype (mk_REP_NumericTypeRep (Int(NAT)));
        TYPE_CPP_Expr parapply (IsNumType (parType) ? GenGetValue (par_v, parType)
                                                    : GenGetValue (GenCastType(nattype, par_v), nattype));

        sq.ImpAppend (vdm_BC_GenAsgnStmt (resVar_v, GenSeqApplyString(tmpRes_v, parapply)));
        alt2 = vdm_BC_GenIfStmt (GenIsString(tmpGen_v), vdm_BC_GenBlock (sq), alt2);
      } else {
      }
    }
  }
  rb.ImpAppend (alt2);
  return rb;
}

// GenUTMapApply
// mapgens : REP`TypeRep
// tmpRes_v : CPP`Name
// tmpGen_v : CPP`Name
// par_v : CPP`Expr
// alt2 : [CPP`Stmt]
// sd : bool
// resVar_v : CPP`Name
// ==> CPP`Stmt
TYPE_CPP_Stmt vdmcg::GenUTMapApply(const Generic & mapgens,
                                   const TYPE_CPP_Name & tmpRes_v,
                                   const TYPE_CPP_Name & tmpGen_v,
                                   const TYPE_CPP_Expr & par_v,
                                   const TYPE_CPP_Stmt & alt2,
                                   bool sd,
                                   const TYPE_CPP_Name & resVar_v)
{
  Tuple fmdr (FindMapDomRng(mapgens));
  Generic rngType (fmdr.GetField(2));

  SEQ<TYPE_CPP_Stmt> decl;
  if (vdm_CPP_isCPP())
    decl.ImpConc(GenDecl_DS(mapgens, tmpRes_v, vdm_BC_GenAsgnInit(tmpGen_v)));
  else
    decl.ImpConc(GenDecl_DS(mapgens, tmpRes_v, vdm_BC_GenAsgnInit(GenCastMap(tmpGen_v))));

  TYPE_CGMAIN_VT cg_vt (mk_CG_VT(tmpRes_v, mapgens));
  TYPE_CPP_Expr mapapply (GenMapApply(cg_vt, par_v));
  TYPE_CPP_Expr cond (GenDomExists(tmpRes_v, par_v));

  TYPE_CPP_Stmt asgn;
  if (IsMapType(rngType) || IsSeqType(rngType) || rngType.Is(TAG_TYPE_REP_CompositeTypeRep) || ! sd)
  {
    if (sd)
    {
      TYPE_CPP_Expr emptyalt;
      if (IsMapType(rngType))
        emptyalt = GenEmptyMapExpr();
      else if (IsSeqType(rngType))
        emptyalt = GenEmptySeqExpr();
      else if (rngType.Is(TAG_TYPE_REP_CompositeTypeRep))
        emptyalt = GenEmptyRecordExpr(Record(rngType).GetRecord(pos_REP_CompositeTypeRep_nm));

      TYPE_CPP_Expr condexpr (vdm_BC_GenBracketedExpr(vdm_BC_GenCondExpr(cond, mapapply, emptyalt)));
      asgn = vdm_BC_GenAsgnStmt(resVar_v, condexpr);
    }
    else
      asgn = vdm_BC_GenAsgnStmt(resVar_v, mapapply);
  }
  else {
    TYPE_CPP_Expr emptymap (GenEmptyMapExpr());
    TYPE_CPP_Stmt asgn1 (vdm_BC_GenAsgnStmt(resVar_v, mapapply));
    TYPE_CPP_Stmt asgn2 (vdm_BC_GenAsgnStmt(resVar_v, emptymap));

    asgn = vdm_BC_GenIfStmt(cond, asgn1, asgn2);
  }

  SEQ<TYPE_CPP_Stmt> decl_asgn;
  decl_asgn.ImpConc(decl).ImpAppend(asgn);
  TYPE_CPP_Stmt res_stmt (vdm_BC_GenIfStmt(GenIsMap(tmpGen_v), vdm_BC_GenBlock(decl_asgn), alt2));
  return res_stmt;
}

// FindPosMapSeqComb
// rc1 : REP`UnionTypeRep
// ==> [ REP`GeneralMapTypeRep ] *
//     [ REP`InjectiveMapTypeRep] *
//     [ REP`SeqTypeRep ] *
//     [ REP`SeqTypeRep ]
Tuple vdmcg::FindPosMapSeqComb (const TYPE_REP_UnionTypeRep & rc1)
{
  SET<TYPE_REP_TypeRep> flattype_s (GiveFlatUnionType (rc1.get_tps()));

  Generic gen = nil;
  Generic inj = nil;
  Generic seqs = nil;
  Generic strings = nil;

  Generic flattype;
  for (bool bb = flattype_s.First (flattype); bb; bb = flattype_s.Next (flattype))
  {
    TYPE_REP_TypeRep type (CleanFlatType(flattype));
    switch (type.GetTag ())
    {
      case TAG_TYPE_REP_GeneralMapTypeRep: {
        gen = mk_REP_GeneralMapTypeRep (type.GetRecord(pos_REP_GeneralMapTypeRep_mapdom),
                                        type.GetRecord(pos_REP_GeneralMapTypeRep_maprng));
        break;
      }
      case TAG_TYPE_REP_InjectiveMapTypeRep: {
        inj = mk_REP_InjectiveMapTypeRep (type.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom),
                                          type.GetRecord(pos_REP_InjectiveMapTypeRep_maprng));
        break;
      }
      case TAG_TYPE_REP_SeqTypeRep: {
        if (IsPossibleStringType(type))
          strings = mk_REP_SeqTypeRep (mk_REP_CharTypeRep());
        else
          seqs = mk_REP_SeqTypeRep (type.GetRecord(pos_REP_SeqTypeRep_elemtp));
        break;
      }
    }
  }
  return mk_(gen, inj, seqs, strings);
}

// CGLetExpr
// rc1 : AS`LetExpr
// resVar_V : CGMAIN`VT
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGLetExpr (const TYPE_AS_LetExpr & rc1, const TYPE_CGMAIN_VT & resVar_v)
{
  const SEQ<TYPE_AS_LocalDef> & ld_l (rc1.GetSequence(pos_AS_LetExpr_localdef));
  const TYPE_AS_Expr & body (rc1.GetRecord(pos_AS_LetExpr_body));

  PushEnv();
  PushEnv_CGAUX();

  SEQ<TYPE_AS_ValueDef> lvd_l;
  SEQ<TYPE_AS_FnDef> lfd_l;
  size_t len_ld_l = ld_l.Length();
  for (size_t idx = 1; idx <= len_ld_l; idx++)
  {
    const TYPE_AS_LocalDef & ld (ld_l[idx]);
    switch(ld.GetTag()) {
      case TAG_TYPE_AS_ValueDef: {
        lvd_l.ImpAppend(ld);
        break;
      }
      case TAG_TYPE_AS_ExplFnDef:
      case TAG_TYPE_AS_ImplFnDef:
      case TAG_TYPE_AS_ExtExplFnDef: {
        lfd_l.ImpAppend(ld);
        break;
      }
      default:
        break;
     }
  }

  GenLocalFcts(lfd_l);

  SEQ<TYPE_CPP_Stmt> rb;
  rb.ImpConc(GenLocalValDef(lvd_l));
  rb.ImpConc(CGExpr( body, resVar_v));

  PopEnv();
  PopEnv_CGAUX();
  DeleteLoc();

  SEQ<TYPE_CPP_Stmt> sq;
  sq.ImpAppend(vdm_BC_GenBlock(rb));;
  return sq;
}

// CGLetBeSTExpr
// expr : AS`LetBeSTExpr
// vt : CGMAIN`VT
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGLetBeSTExpr(const TYPE_AS_LetBeSTExpr & expr, const TYPE_CGMAIN_VT & vt)
{
  const SEQ<TYPE_AS_MultBind> & bind_l (expr.GetSequence(pos_AS_LetBeSTExpr_lhs));
  
  bool exists = false;
  size_t len_bind_l = bind_l.Length();
  for (size_t idx = 1; (idx <= len_bind_l) && !exists; idx++)
  {
    exists = bind_l[idx].Is(TAG_TYPE_AS_MultTypeBind);
  }

  if (exists)
  {
    SetNotSupportedException(true);
    return SEQ<TYPE_CPP_Stmt>().ImpAppend(NotSupported(L"type bind", expr));
  }
  else
  {
    PushEnv_CGAUX();

    SEQ<TYPE_CPP_Stmt> rb_l;
    rb_l.ImpConc(GenBindListVariables(bind_l, expr.GetField(pos_AS_LetBeSTExpr_St)));
    rb_l.ImpConc(CGExpr(expr.GetRecord(pos_AS_LetBeSTExpr_In), vt));

    PopEnv_CGAUX();

    return SEQ<TYPE_CPP_Stmt>().ImpAppend(vdm_BC_GenBlock(rb_l));
  }
}

// GenBindVariables
// MSB : AS`MultSetBind
// St : [AS`Expr]
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenBindVariables(const TYPE_AS_MultSetBind & MSB, const Generic & St)
{
  const SEQ<TYPE_AS_Pattern> & pat_l (MSB.GetSequence(pos_AS_MultSetBind_pat));
  const TYPE_AS_Expr & set (MSB.GetRecord(pos_AS_MultSetBind_Set));

  TYPE_CPP_Identifier tmpElem (vdm_BC_GiveName(ASTAUX::MkId(L"tmpElem")));

  Generic setType (FindType(set));
  TYPE_REP_TypeRep settp (FindSetElemType(setType));
  TYPE_CGMAIN_VT elemVT (mk_CG_VT(tmpElem, settp));

  Tuple cgee (CGExprExcl(set, ASTAUX::MkId(L"tmpSet"), nil)); // -- first -- must be before DeclarePatterns
  const TYPE_CPP_Expr & s_res (cgee.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & s_st_stmt (cgee.GetSequence(2));

  TYPE_CPP_Expr tmpSet (s_res); 
  SEQ<TYPE_CPP_Stmt> rb_l;
  if (!IsSetType(setType))
  {
    TYPE_CPP_Expr tmpS (s_res); 
    if (!s_res.Is(TAG_TYPE_CPP_Identifier))
    {
      tmpSet = vdm_BC_GiveName(ASTAUX::MkId(L"tmpS"));
      rb_l.ImpAppend(vdm_BC_GenDecl(GenType(setType), tmpS, vdm_BC_GenAsgnInit(s_res)));
    }
    rb_l.ImpAppend (vdm_BC_GenIfStmt (vdm_BC_GenNot (GenIsSet(tmpS)),
                               vdm_BC_GenBlock(mk_sequence(RunTime(L"A set was expected"))), nil));
    tmpSet = GenCastSetType(tmpS);
  }
  rb_l.ImpConc(s_st_stmt);

  TYPE_CGMAIN_VT setVT (mk_CG_VT(tmpSet, mk_REP_SetTypeRep(settp)));

  TYPE_CPP_Identifier succ (vdm_BC_GiveName(ASTAUX::MkId(L"succ")));

  Map pid_m (SamePatternIds(pat_l).GetMap(2)); // bool * map AS`Name to set of REP`TypeRep

  rb_l.ImpConc(DeclarePatterns(pid_m));   // -- second -- must be before CGExprExcl and CGPatternMatchExcl 

  SEQ<TYPE_CPP_Stmt> inner_stmts;
  if (!St.IsNil())
  {
    Tuple cgee (CGExprExcl(St, ASTAUX::MkId(L"tmpB"), nil));
    const TYPE_CPP_Expr & res (cgee.GetRecord(1));
    const SEQ<TYPE_CPP_Stmt> & st_stmt (cgee.GetSequence(2));

    inner_stmts.ImpConc(st_stmt); //

    TYPE_REP_BooleanTypeRep bt;
    if (IsBoolType(FindType(St))) // TYPE_REP_BasicTypeRep
    {
      inner_stmts.ImpAppend(vdm_BC_GenAsgnStmt(succ, GenGetValue(res, bt)));
    }
    else
    {
      TYPE_CPP_Expr getval (GenGetValue(vdm_BC_GenCastExpr(GenBoolType(), res), bt));

      inner_stmts.ImpAppend(vdm_BC_GenIfStmt(GenIsBool(res),
                                            vdm_BC_GenAsgnStmt(succ, getval),
                               vdm_BC_GenBlock(mk_sequence(RunTime(L"A boolean was expected")))));
    }
  }

  bool need_decl = false;
  size_t len_pat_l = pat_l.Length();
  for (size_t i = len_pat_l; i >= 1; i--)
  {
    Set p_s;
    for (size_t j = 1; j < i; j++)
      p_s.ImpUnion (FindPatternId (pat_l[j]).Dom ());
    Generic p = Nil();
    if (!inner_stmts.IsEmpty())
      p = inner_stmts;
    Tuple cgpme (CGPatternMatchExcl(pat_l[i], elemVT, p_s, succ, Map(), p, false));
    const SEQ<TYPE_CPP_Stmt> & pm (cgpme.GetSequence(1));
    bool Is_Excl (cgpme.GetBoolValue(2)); // false : need to check pattern match failed
    inner_stmts = pm;
    need_decl = need_decl || !Is_Excl;
  }

  if (!need_decl && St.IsNil())
    inner_stmts.ImpAppend(vdm_BC_GenAsgnStmt(succ, vdm_BC_GenBoolLit(true)));

  rb_l.ImpAppend(vdm_BC_GenDecl(GenSmallBoolType(), succ, vdm_BC_GenAsgnInit(vdm_BC_GenBoolLit(false))));
  rb_l.ImpConc(GenIterSet(setVT, vdm_BC_GenNot(succ), elemVT, inner_stmts));
  rb_l.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenNot(succ),
                  vdm_BC_GenBlock(mk_sequence(RunTime(L"The binding environment was empty"))), nil));
  return rb_l;
}

// GenBindListVariables
// SB : BindList
// St : [AS`Expr]
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenBindListVariables(const SEQ<TYPE_AS_MultBind> & bind_l, const Generic & St)
{
  if ((bind_l.Length() == 1) &&
      bind_l[1].Is(TAG_TYPE_AS_MultSetBind) &&
      (bind_l[1].GetSequence(pos_AS_MultSetBind_pat).Length() == 1))
    return GenBindVariables(bind_l[1], St);

  SEQ<TYPE_AS_Pattern> p_l;
  size_t len_bind_l = bind_l.Length();
  for (size_t idx = 1; idx <= len_bind_l; idx++)
  {
    const TYPE_AS_MultBind & mb (bind_l[idx]);
    switch(mb.GetTag()) {
      case TAG_TYPE_AS_MultSetBind: {
        p_l.ImpConc(mb.GetSequence(pos_AS_MultSetBind_pat));
        break;
      }
      case TAG_TYPE_AS_MultTypeBind: {
        // do nothing
        break;
      }
    }
  }
  Map m (SamePatternIds(p_l).GetMap(2));

  MAP<TYPE_AS_Name, TYPE_REP_TypeRep> pid_m;
  if (!m.IsEmpty())
  {
    Set dom_m (m.Dom());
    Generic nm;
    for (bool bb = dom_m.First(nm); bb; bb = dom_m.Next(nm))
    {
      SET<TYPE_REP_TypeRep> tp_s (m[nm]);
      if (tp_s.Card() == 1)
        pid_m.ImpModify(nm, tp_s.GetElem());
      else
        pid_m.ImpModify(nm, TYPE_REP_UnionTypeRep().Init(tp_s));
    }
  }

  TYPE_CPP_Identifier succ_bind_v (vdm_BC_GiveName(ASTAUX::MkId(L"succ_bind")));
  TYPE_CPP_Stmt stmt (vdm_BC_GenAsgnStmt(succ_bind_v, vdm_BC_GenBoolLit(true)));
  TYPE_CPP_Expr contexpr (vdm_BC_GenNot(succ_bind_v));
  TYPE_CPP_Stmt cstmt (CGComprehension(bind_l, St, mk_sequence(stmt), contexpr, nil, pid_m, false));

  SEQ<TYPE_CPP_Stmt> rb_l;
  rb_l.ImpAppend(vdm_BC_GenDecl(GenSmallBoolType(), succ_bind_v, vdm_BC_GenAsgnInit(vdm_BC_GenBoolLit(false))));
  rb_l.ImpConc(StripCompoundStmt(cstmt));
  rb_l.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenNot(succ_bind_v),
                   vdm_BC_GenBlock(mk_sequence(RunTime(L"The binding environment was empty"))), nil));

  return rb_l;
}

// CGDefExpr
// DE : AS`DefExpr
// resVar_v : CGMAIN`VT
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGDefExpr(const TYPE_AS_DefExpr & DE, const TYPE_CGMAIN_VT & resVar_v)
{
  PushEnv();
  PushEnv_CGAUX();

  const type_dd2PL & def_l (DE.GetSequence(pos_AS_DefExpr_Def));
  const TYPE_AS_Expr & body(DE.GetRecord(pos_AS_DefExpr_In));

  SEQ<TYPE_AS_ValueDef> lvd_l;
  size_t len_def_l = def_l.Length();
  for (size_t idx = 1; idx <= len_def_l; idx++)
  {
    const Tuple & t (def_l[idx]); 
    const TYPE_AS_PatternBind & patb (t.GetRecord(1));
    const TYPE_AS_Expr & expr (t.GetRecord(2));

    switch (patb.GetTag()) {
      case TAG_TYPE_AS_TypeBind: {
        TYPE_AS_ValueDef vd;
        vd.Init(patb.GetRecord(pos_AS_TypeBind_pat),
                patb.GetRecord(pos_AS_TypeBind_tp),
                expr, Int (NOT_INITIALISED_AS), Bool(false), ASTAUX::GetCid(patb));
        lvd_l.ImpAppend(vd);
        break;
      }
      case TAG_TYPE_AS_SetBind: {

//        pat = patb.GetRecord(pos_AS_SetBind_pat);
        const TYPE_AS_Pattern & pat (patb.GetRecord(pos_AS_SetBind_pat));
        const TYPE_AS_Expr & Set (patb.GetRecord(pos_AS_SetBind_Set));
        TYPE_CI_ContextId pcid (GetCI().PushCGType(mk_REP_SetTypeRep(FindType(pat))));
        TYPE_CI_ContextId ecid (GetCI().PushCGType(mk_REP_SetTypeRep(FindType(expr))));
        TYPE_REP_TypeRep tp (mk_REP_UnionTypeRep(mk_set(FindType(Set), mk_REP_SetTypeRep(FindType(expr)))));
        TYPE_CI_ContextId iscid (GetCI().PushCGType(tp));

        TYPE_AS_BinaryExpr iset;
        iset.Init(Set, Int(SETINTERSECT), TYPE_AS_SetEnumerationExpr().Init(mk_sequence(expr), ecid), iscid);
        TYPE_AS_SetEnumPattern sep;
        sep.Init(mk_sequence(pat), pcid);

        TYPE_AS_ValueDef vd;
        vd.Init(sep, nil, iset, Int (NOT_INITIALISED_AS), Bool(false), ASTAUX::GetCid(patb));
        lvd_l.ImpAppend(vd);
        break;
      }
      default: {
        TYPE_AS_ValueDef vd;
        vd.Init(patb, nil, expr, Int (NOT_INITIALISED_AS), Bool(false), ASTAUX::GetCid(patb));
        lvd_l.ImpAppend(vd);
        break;
      }
    }
  }

  SEQ<TYPE_CPP_Stmt> rb_l;
  rb_l.ImpConc(GenLocalValDef(lvd_l));
  rb_l.ImpConc(CGExpr(body, resVar_v));

  PopEnv();
  PopEnv_CGAUX();
  DeleteLoc();

  SEQ<TYPE_CPP_Stmt> sq;
  sq.ImpAppend(vdm_BC_GenBlock(rb_l));
  return sq;
}

// CGBracketedExpr
// rc1 : AS`BracketedExpr
// resVar_v : CGMAIN`VT
// ==> seq of CPP`Stmt | CPP`Expr
Generic vdmcg::CGBracketedExpr (const TYPE_AS_BracketedExpr & rc1, const TYPE_CGMAIN_VT & resVar_v)
{
  return CGExpr_q (rc1.GetRecord(pos_AS_BracketedExpr_expr), resVar_v);
}

// CGIfExpr
// rc1 : AS`IfExpr
// vt : CGMAIN`VT
// ==> CPP`Expr | seq of CPP`Stmt
Generic vdmcg::CGIfExpr (const TYPE_AS_IfExpr & rc1, const TYPE_CGMAIN_VT & vt)
{
  const TYPE_AS_Expr & test (rc1.GetRecord(pos_AS_IfExpr_test));
  const TYPE_AS_Expr & cons (rc1.GetRecord(pos_AS_IfExpr_cons));
  const SEQ<TYPE_AS_ElseifExpr> & elif (rc1.GetSequence(pos_AS_IfExpr_elsif));
  const TYPE_AS_Expr & altn (rc1.GetRecord(pos_AS_IfExpr_altn));

  const TYPE_CPP_Expr & resVar_v (vt.GetRecord(pos_CGMAIN_VT_name));
  const Generic & resVarType (vt.GetField(pos_CGMAIN_VT_type));

  TYPE_REP_TypeRep condType (FindType(test));

  Tuple cgee (CGExprExcl(test, ASTAUX::MkId(L"cond"),nil));
  const TYPE_CPP_Expr & res1 (cgee.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & cond_stmt (cgee.GetSequence(2));

  Tuple cgee_cons (CGExprExcl(cons, ASTAUX::MkId(L"cons"),resVarType));
  const TYPE_CPP_Expr & cons_expr (cgee_cons.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & cons_stmt (cgee_cons.GetSequence(2));

  Tuple cgee_altn (CGExprExcl(altn, ASTAUX::MkId(L"altn"),resVarType));
  const TYPE_CPP_Expr & altn_expr (cgee_altn.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & altn_stmt (cgee_altn.GetSequence(2));

  TYPE_CPP_Expr getval (IsBoolType(condType) ? res1 : vdm_BC_GenCastExpr(GenBoolType(), res1));
  TYPE_CPP_Expr cond (vdm_CPP_isCPP() ? GenGetValue(getval, condType)
                                      : GenGetValue(getval, mk_REP_BooleanTypeRep()));

// 20150807 -->
  if (elif.IsEmpty() && cons_stmt.IsEmpty() && altn_stmt.IsEmpty()) {
    Generic cons_type (FindType(cons));
    Generic altn_type (FindType(altn));
    if (!cons_type.IsNil() && !altn_type.IsNil()) {
      if ((cons_type == altn_type) ||
          (IsIntType(cons_type) && IsIntType(altn_type)) ||
          (vdm_CPP_isCPP() && cons.Is(TAG_TYPE_AS_QuoteLit) && altn.Is(TAG_TYPE_AS_QuoteLit)) ||
          (vdm_CPP_isJAVA() && IsQuoteType(cons_type) && IsQuoteType(altn_type))) {
        TYPE_CPP_Expr expr (vdm_BC_GenCondExpr(cond, cons_expr, altn_expr));
        //return cond_stmt.Conc(mk_sequence(vdm_BC_GenAsgnStmt(resVar_v, vdm_BC_GenBracketedExpr(expr))));
        if (cond_stmt.IsEmpty())
          return vdm_BC_GenBracketedExpr(expr);
        else
          return cond_stmt.Conc(mk_sequence(vdm_BC_GenAsgnStmt(resVar_v, vdm_BC_GenBracketedExpr(expr))));
      }
      else if (vdm_CPP_isJAVA()) {
        TYPE_CPP_Expr cast_cons (cons_type == resVarType ? cons_expr
                         : vdm_BC_GenCastExpr(GenType(resVarType), cons_expr));
        TYPE_CPP_Expr cast_altn (altn_type == resVarType ? altn_expr
                         : vdm_BC_GenCastExpr(GenType(resVarType), altn_expr));
        TYPE_CPP_Expr expr (vdm_BC_GenCondExpr(cond, cast_cons, cast_altn));
        //return cond_stmt.Conc(mk_sequence(vdm_BC_GenAsgnStmt(resVar_v, vdm_BC_GenBracketedExpr(expr))));
        if (cond_stmt.IsEmpty())
          return vdm_BC_GenBracketedExpr(expr);
        else
          return cond_stmt.Conc(mk_sequence(vdm_BC_GenAsgnStmt(resVar_v, vdm_BC_GenBracketedExpr(expr))));
      }
    }
  }
// <-- 20150807

  SEQ<TYPE_CPP_Stmt> tmpb (altn_stmt);
  tmpb.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, altn_expr));

  for (size_t i = elif.Length (); i >= 1; i--)
  {
    const TYPE_AS_ElseifExpr & tpl (elif[i]);
    const TYPE_AS_Expr & elif_test (tpl.GetRecord(pos_AS_ElseifExpr_test));
    const TYPE_AS_Expr & elif_cons (tpl.GetRecord(pos_AS_ElseifExpr_cons));

    Tuple cgee (CGExprExcl(elif_test, ASTAUX::MkId(L"cond"), nil));
    const TYPE_CPP_Expr & res2 (cgee.GetRecord(1));
    const SEQ<TYPE_CPP_Stmt> & res2_stmt (cgee.GetSequence(2));

    TYPE_CPP_Stmt elif_alt2 (GenCPPStmt(tmpb));
    TYPE_CPP_Stmt elif_alt1 (GenCPPStmt(CGExpr(elif_cons, mk_CG_VT(resVar_v, resVarType))));

    TYPE_REP_TypeRep testType (FindType(elif_test));
    TYPE_CPP_Expr nres2 (IsBoolType(testType) ? res2 : vdm_BC_GenCastExpr(GenBoolType(), res2));

    TYPE_CPP_Expr elif_cond (vdm_CPP_isCPP() ? GenGetValue(nres2, testType)
                                             : GenGetValue(nres2, mk_REP_BooleanTypeRep()));
    SEQ<TYPE_CPP_Stmt> body (res2_stmt);
    body.ImpAppend(vdm_BC_GenIfStmt(elif_cond, elif_alt1, elif_alt2));
    tmpb = body;
  }

  TYPE_CPP_Stmt alt1 (GenCPPStmt(cons_stmt.Conc(mk_sequence(vdm_BC_GenAsgnStmt(resVar_v, cons_expr)))));
  TYPE_CPP_Stmt alt2 (GenCPPStmt(tmpb));

  SEQ<TYPE_CPP_Stmt> rb (cond_stmt);
  rb.ImpAppend (vdm_BC_GenIfStmt (cond, alt1, alt2));
  return rb;
}

// CGSeqModifyMapOverrideExpr
// rc1 : AS`SeqModifyMapOverrideExpr
// vt : CGMAIN`VT
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGSeqModifyMapOverrideExpr (const TYPE_AS_SeqModifyMapOverrideExpr & rc1,
                                                      const TYPE_CGMAIN_VT & vt)
{
  const TYPE_AS_Expr & seqmap (rc1.GetRecord(pos_AS_SeqModifyMapOverrideExpr_seqmap));
  const TYPE_AS_Expr & mapexp (rc1.GetRecord(pos_AS_SeqModifyMapOverrideExpr_mapexp));
  const TYPE_CPP_Expr & resVar_v (vt.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & restp (vt.GetRecord(pos_CGMAIN_VT_type));

  TYPE_REP_TypeRep seqmapType1 (FindType(seqmap));
  TYPE_CPP_Expr modmap_v (vdm_BC_GiveName(ASTAUX::MkId(L"modmap")));

  TYPE_REP_TypeRep modmapType (FindType (mapexp));
  TYPE_REP_TypeRep tmpmapType (FindPossibleMapType (modmapType));

  Tuple cgee (CGExprExcl(seqmap, ASTAUX::MkId(L"seqmap"), nil));
  const TYPE_CPP_Expr & seqmap_v (cgee.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & seqmap_stmt (cgee.GetSequence(2));

  SEQ<TYPE_CPP_Stmt> rb (seqmap_stmt);

  if (mapexp.Is(TAG_TYPE_AS_Name))
    modmap_v = vdm_BC_Rename(mapexp);
  else
  {
    if (IsMapType (modmapType))
    {
      Tuple smcgee (CGExprExcl(mapexp, ASTAUX::MkId(L"modmap"), modmapType));
      rb.ImpConc(smcgee.GetSequence(2));
      rb.ImpConc (GenDeclInit_DS (modmapType, modmap_v, smcgee.GetRecord(1)));
    }
    else
    {
      TYPE_CPP_Identifier tmpmap_v (vdm_BC_GiveName(ASTAUX::MkId(L"tmpmap")));
      Tuple smcgee (CGExprExcl(mapexp, ASTAUX::MkId(L"tmpmap"), modmapType));
      rb.ImpConc(smcgee.GetSequence(2));
      rb.ImpConc (GenDeclInit_DS (modmapType, tmpmap_v, smcgee.GetRecord(1)));
      rb.ImpAppend (vdm_BC_GenIfStmt (vdm_BC_GenNot (GenIsMap (tmpmap_v)), RunTime (L"A map was expected"), nil));
      rb.ImpConc (GenDeclInit_DS (tmpmapType, modmap_v, GenCastMap(tmpmap_v)));
    }
  }
  TYPE_CPP_Stmt rre (RunTime(L"A map or sequence was expected in sequence or map override"));

  if (IsPosMapType(seqmapType1) && !IsPosSeqType(seqmapType1))
  {
    SEQ<TYPE_CPP_Stmt> mapover (GenMapOvwr (mk_CG_VT (seqmap_v, seqmapType1),
                                mk_CG_VT (modmap_v, modmapType),
                                mk_CG_VT (resVar_v, restp),
                                !IsMapType(seqmapType1) || !IsMapType(restp)));
    if (IsMapType(seqmapType1))
      rb.ImpConc (mapover);
    else
      rb.ImpAppend(vdm_BC_GenIfStmt(GenIsMap(seqmap_v), vdm_BC_GenBlock(mapover), rre));
  }
  else if (IsPosSeqType(seqmapType1) && !IsPosMapType(seqmapType1))
  {
    SEQ<TYPE_CPP_Stmt> seqover (GenSeqOvwr (mk_CG_VT (seqmap_v, seqmapType1),
                                mk_CG_VT (modmap_v, modmapType),
                                mk_CG_VT (resVar_v, restp),
                                !IsSeqType(seqmapType1) || !IsSeqType(restp)));
    if (IsSeqType(seqmapType1))
      rb.ImpConc (seqover);
    else
    {
      if (vdm_CPP_isCPP())
        rb.ImpAppend(vdm_BC_GenIfStmt(GenIsSeq(seqmap_v), vdm_BC_GenBlock(seqover), rre));
      else
      {
        if (IsPossibleStringType(seqmapType1))
          rb.ImpAppend(vdm_BC_GenIfStmt(GenIsString(seqmap_v), vdm_BC_GenBlock(seqover), rre));
        else
          rb.ImpAppend(vdm_BC_GenIfStmt(GenIsSeq(seqmap_v), vdm_BC_GenBlock(seqover), rre));
      }
    }
  }
  else
  {
    SEQ<TYPE_CPP_Stmt> mapover (GenMapOvwr (mk_CG_VT (seqmap_v, seqmapType1),
                                mk_CG_VT (modmap_v, modmapType),
                                mk_CG_VT (resVar_v, restp),
                                true));
    SEQ<TYPE_CPP_Stmt> seqover (GenSeqOvwr (mk_CG_VT (seqmap_v, seqmapType1),
                                mk_CG_VT (modmap_v, modmapType),
                                mk_CG_VT (resVar_v, restp),
                                true));
    if (vdm_CPP_isCPP())
      rb.ImpAppend(vdm_BC_GenIfStmt(GenIsMap(seqmap_v),
                                    vdm_BC_GenBlock(mapover),
                                    vdm_BC_GenIfStmt(GenIsSeq(seqmap_v), vdm_BC_GenBlock(seqover), rre)));
    else
    {
      if (IsPossibleStringType(seqmapType1))
        rb.ImpAppend(vdm_BC_GenIfStmt(GenIsMap(seqmap_v),
                                      vdm_BC_GenBlock(mapover),
                                      vdm_BC_GenIfStmt(GenIsString(seqmap_v), vdm_BC_GenBlock(seqover), rre)));
      else
        rb.ImpAppend(vdm_BC_GenIfStmt(GenIsMap(seqmap_v),
                                      vdm_BC_GenBlock(mapover),
                                      vdm_BC_GenIfStmt(GenIsSeq(seqmap_v), vdm_BC_GenBlock(seqover), rre)));
    }
  }
// <-- 20120322

  return rb;
}

// FindPosApplyTypes
// tp : REP`TypeRep
// ==> REP`TypeRep
TYPE_REP_TypeRep vdmcg::FindPosApplyTypes(const TYPE_REP_TypeRep & tp)
{
  switch (tp.GetTag()) {
    case TAG_TYPE_REP_SeqTypeRep:
    case TAG_TYPE_REP_GeneralMapTypeRep:
    case TAG_TYPE_REP_InjectiveMapTypeRep:
      return tp;
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tps (tp.GetSet(pos_REP_UnionTypeRep_tps));
      SET<TYPE_REP_TypeRep> tps_res;
      Generic t_g;
      for (bool bb = tps.First(t_g); bb; bb = tps.Next(t_g)) {
        TYPE_REP_TypeRep t (t_g);
        if (t.Is(TAG_TYPE_REP_SeqTypeRep) ||
            (IsMapType(t) && ! t.Is(TAG_TYPE_REP_EmptyMapTypeRep) ))
          tps_res.Insert(FindPosApplyTypes(t));
      }
      return mk_REP_UnionTypeRep(tps_res);
    }
    default:
      ReportError(L"FindPosApplyTypes");
  }
  return Record(); // To avoid warnings
}

// GenSeqOvwr
// rc1 : CGMAIN`VT
// rc2 : CGMAIN`VT
// rc3 : CGMAIN`VT
// cast : bool
// ==> seq of CPP`Stmt | CPP`Expr
SEQ<TYPE_CPP_Stmt> vdmcg::GenSeqOvwr (const TYPE_CGMAIN_VT & rc1,
                                      const TYPE_CGMAIN_VT & rc2,
                                      const TYPE_CGMAIN_VT & rc3,
                                      bool cast)
{
  const TYPE_CPP_Expr & seq_v          (rc1.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & seqtype     (rc1.GetRecord(pos_CGMAIN_VT_type));
  const TYPE_CPP_Expr & map_v          (rc2.GetRecord(pos_CGMAIN_VT_name));
  const Generic & mapType              (rc2.GetField(pos_CGMAIN_VT_type));
  const TYPE_CPP_Identifier & resVar_v (rc3.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & restp       (rc3.GetRecord(pos_CGMAIN_VT_type));

  TYPE_CPP_Identifier s_v (cast ? vdm_BC_GiveName(ASTAUX::MkId(L"s_v")) : resVar_v);
  TYPE_CPP_Identifier edom (vdm_BC_GiveName(ASTAUX::MkId(L"edom")));
  TYPE_CPP_Identifier erng (vdm_BC_GiveName(ASTAUX::MkId(L"erng")));
  TYPE_REP_TypeRep nattp (mk_REP_NumericTypeRep(Int(NAT)));

  Tuple fmdr (FindMapDomRng(mapType)); // ([REP`TypeRep] * [REP`TypeRep])
  const Generic & domtype (fmdr.GetField(1));

  TYPE_CPP_Identifier e_v (edom);
  SEQ<TYPE_CPP_Stmt> isint_part;
  if (!IsIntType(domtype))
  {
    e_v = vdm_BC_GiveName(ASTAUX::MkId(L"e_v"));
    TYPE_CPP_Stmt rre (vdm_BC_GenBlock(mk_sequence(RunTime(L"An integer was expected in domain of map in sequence modifier"))));
    TYPE_CPP_Expr int_cond (vdm_BC_GenNot(GenIsInt(edom)));
    TYPE_CPP_Stmt ifisint (vdm_BC_GenIfStmt(int_cond, rre, nil));
    SEQ<TYPE_CPP_Stmt> e_decl (vdm_CPP_isCPP()
                               ? GenDeclInit_DS(nattp, e_v, edom)
                               : GenDeclInit_DS(nattp, e_v, vdm_BC_GenCastExpr(GenNatType(),edom)));
    isint_part.ImpAppend(ifisint).ImpConc(e_decl);
  }

  TYPE_CPP_Expr e_int (GenGetValue(e_v, nattp));
  TYPE_CPP_Expr seq_len (vdm_CPP_isCPP() ? GenLen(s_v) : GenLen_int(s_v));
  TYPE_CPP_Expr cond (vdm_BC_GenLogOr(vdm_BC_GenGt(e_int, seq_len), vdm_BC_GenLt(e_int, vdm_BC_GenIntegerLit(1))));
  TYPE_CPP_Stmt rre (vdm_BC_GenBlock(mk_sequence(RunTime(L"Illegal index in sequence modifier"))));
  TYPE_CPP_Stmt alt2 (GenSeqModify(s_v, e_int, erng));
  TYPE_CPP_Stmt if_body (vdm_CPP_isCPP() ? alt2 : vdm_BC_GenIfStmt(cond, rre, alt2));
  TYPE_CPP_Stmt body (GenCPPStmt(isint_part.ImpAppend(if_body)));
  TYPE_REP_TypeRep seqtp (mk_REP_SeqTypeRep(FindSeqElemType(seqtype)));

  SEQ<TYPE_CPP_Stmt> rb;
  if (vdm_CPP_isCPP())
  {
    if (cast)
      rb.ImpConc(GenDeclInit_DS(seqtp, s_v, seq_v));
    else
      rb.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, seq_v));
  }
  else
  { // java
    if (IsStringType(seqtype) || IsPossibleStringType(seqtype))
    {
      TYPE_CPP_Expr caste (GenExplicitCast(mk_REP_SeqTypeRep(nattp), seq_v, nil));
      rb.ImpConc(GenDeclInit_DS(seqtp, s_v, caste));
    }
    else
    {
      TYPE_CPP_Expr seq_v_q (cast ? GenCastSeq(seq_v, nil) : seq_v);
      TYPE_CPP_Expr seq_v_c (GenSeqExpr(seq_v_q));
      if (cast)
        rb.ImpConc(GenDeclInit_DS(seqtp, s_v, seq_v_c));
      else
        rb.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, seq_v_c));
    }
  }

  rb.ImpConc(GenIterMap(mk_CG_VT(map_v, mapType), nil, edom, erng, body));

  if (vdm_CPP_isCPP())
  {
    if (cast)
      rb.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, s_v));
  }
  else
  {
    if (IsStringType(restp) || IsPossibleStringType(restp))
      rb.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v,
                                      GenExplicitCast(mk_REP_SeqTypeRep(mk_REP_CharTypeRep()),
                                                      s_v,
                                                      nil)));
    else
      if (cast)
        rb.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, s_v));
  }
  return rb;
}

// GenMapOvwr
// rc1 : CGMAIN`VT
// rc2 : CGMAIN`VT
// rc3 : CGMAIN`VT
// cast : bool
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenMapOvwr (const TYPE_CGMAIN_VT & rc1,
                                      const TYPE_CGMAIN_VT & rc2,
                                      const TYPE_CGMAIN_VT & rc3,
                                      bool cast)
{
  const TYPE_CPP_Expr & seqmap_v      (rc1.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & seqmapType (rc1.GetRecord(pos_CGMAIN_VT_type));
  const TYPE_CPP_Expr & modmap_v      (rc2.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & modmapType (rc2.GetRecord(pos_CGMAIN_VT_type));
  const TYPE_CPP_Expr & resVar_v      (rc3.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & resType    (rc3.GetRecord(pos_CGMAIN_VT_type));

  if (vdm_CPP_isCPP())
  {
    if (cast)
    {
      TYPE_CPP_Identifier tmpMap_v (vdm_BC_GiveName(ASTAUX::MkId(L"tmpMap")));
      SEQ<TYPE_CPP_Stmt> rb;
      rb.ImpConc (GenDecl_DS (FindPossibleMapType (seqmapType), tmpMap_v, vdm_BC_GenAsgnInit (seqmap_v)));
      rb.ImpAppend (GenMapOverride (tmpMap_v, modmap_v));
      rb.ImpAppend (vdm_BC_GenAsgnStmt (resVar_v, tmpMap_v));
      return rb;
    }
    else {
      SEQ<TYPE_CPP_Stmt> rb;
      rb.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, seqmap_v));
      rb.ImpAppend(GenMapOverride(resVar_v, modmap_v));
      return rb;
    }
  }
  else
  { // java
    TYPE_CPP_Expr castmap1 (IsMapType(seqmapType) ? seqmap_v : GenCastMap(seqmap_v));
    TYPE_CPP_Expr castmap2 (IsMapType(modmapType) ? modmap_v : GenCastMap(modmap_v));
    TYPE_CPP_Expr resmap (IsMapType(resType) ? resVar_v : GenCastMap(resVar_v));

    SEQ<TYPE_CPP_Stmt> rb;
    rb.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, GenMapExpr(castmap1)));
    rb.ImpAppend(GenMapOverride(resmap, castmap2));
    return rb;
  }
}

// GenSeqMapDynOvwr
// rc1 : CGMAIN`VT
// rc2 : CGMAIN`VT
// rc3 : CGMAIN`VT
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenSeqMapDynOvwr (const TYPE_CGMAIN_VT & rc1,
                                            const TYPE_CGMAIN_VT & rc2,
                                            const TYPE_CGMAIN_VT & rc3)
{
  const TYPE_CPP_Expr & seqmap_v      (rc1.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & seqmapType (rc1.GetRecord(pos_CGMAIN_VT_type));
  const TYPE_CPP_Expr & modmap_v      (rc2.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & modmapType (rc2.GetRecord(pos_CGMAIN_VT_type));
  const TYPE_CPP_Expr & resVar_v      (rc3.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & restp      (rc3.GetRecord(pos_CGMAIN_VT_type));

  TYPE_CPP_Expr condseq (GenIsSeq(seqmap_v));
  TYPE_CPP_Expr condmap (GenIsMap(seqmap_v));
  TYPE_CPP_Expr condwstring (GenIsString(seqmap_v));
  SEQ<TYPE_CPP_Stmt> altseq1 (GenSeqOvwr(mk_CG_VT(seqmap_v, seqmapType),
                                         mk_CG_VT(modmap_v, modmapType),
                                         mk_CG_VT(resVar_v, restp), true));
  SEQ<TYPE_CPP_Stmt> altmap1 (GenMapOvwr(mk_CG_VT(seqmap_v, seqmapType),
                                         mk_CG_VT(modmap_v, modmapType),
                                         mk_CG_VT(resVar_v, restp), true));

  TYPE_CPP_Stmt rerr;
  if (vdm_CPP_isCPP())
    rerr = (RunTime(L"A map or sequence was expected in sequence or map override"));
  else
    rerr = (RunTime(L"Wrong arguments for '++'"));

  TYPE_CPP_Stmt altseq (vdm_BC_GenBlock(altseq1));
  TYPE_CPP_Stmt altmap (vdm_BC_GenBlock(altmap1));

  SEQ<TYPE_CPP_Stmt> rb;
  if (seqmapType.Is(TAG_TYPE_REP_UnionTypeRep) && (seqmapType.GetSet(pos_REP_UnionTypeRep_tps).Card() == 1) )
  {
    TYPE_REP_TypeRep tp (seqmapType.GetSet(pos_REP_UnionTypeRep_tps).GetElem());
    if (tp.Is(TAG_TYPE_REP_SeqTypeRep))
    {
      if (vdm_CPP_isCPP())
        rb.ImpAppend(vdm_BC_GenIfStmt(condseq, altseq, rerr));
      else
        rb.ImpConc(altseq1);
    }
    else
    {
      if (vdm_CPP_isCPP())
        rb.ImpAppend(vdm_BC_GenIfStmt(condmap, altmap, rerr));
      else
        rb.ImpConc(altmap1);
    }
  }
  else
  {
    if (vdm_CPP_isCPP())
    {
      rb.ImpAppend(vdm_BC_GenIfStmt(condmap, altmap, vdm_BC_GenIfStmt(condseq, altseq, rerr)));
    }
    else
    { // java
      if (IsPossibleStringType(seqmapType))
        rb.ImpAppend(vdm_BC_GenIfStmt(condmap, altmap, vdm_BC_GenIfStmt(condwstring, altseq, rerr)));
      else
        rb.ImpAppend(vdm_BC_GenIfStmt(condmap, altmap, vdm_BC_GenIfStmt(condseq, altseq, rerr)));
    }
  }
  return rb;
}

// CGFieldSelectExpr
// rc1 : AS`FieldSelectExpr
// vt : CGMAIN`VT
// expr1 : [CPP`Name]
// type1 : [REP`TypeRep]
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGFieldSelectExpr (const TYPE_AS_FieldSelectExpr & rc1,
                                             const TYPE_CGMAIN_VT & vt,
                                             const Generic & expr1,
                                             const Generic & type1)
{
  const TYPE_AS_Expr & rec (rc1.GetRecord(pos_AS_FieldSelectExpr_rec));
  const TYPE_AS_Name & nm  (rc1.GetRecord(pos_AS_FieldSelectExpr_nm));

// 20140318 -->
#ifdef VDMSL
  if (rec.Is(TAG_TYPE_AS_Name))
  {
    Generic tp (GetStatSem().LookUpStateEnv(rec));
    if (tp.Is(TAG_TYPE_REP_CompositeTypeRep))
      //if (Record(tp).GetRecord(pos_REP_CompositeTypeRep_nm) == ASTAUX::Combine2Names(GiveCurCASName(), rec))
      if (Record(tp).GetRecord(pos_REP_CompositeTypeRep_nm) == rec)
        return CGExpr(nm, vt);
  }
#endif // VDMSL
// <-- 20140318
#ifdef VDMPP
  TYPE_REP_TypeRep ti (FindType (rc1));
  if (ti.Is (TAG_TYPE_REP_PartialFnTypeRep) ||
      ti.Is (TAG_TYPE_REP_TotalFnTypeRep) ||
      ti.Is (TAG_TYPE_REP_OpTypeRep))
  {
    SetNotSupportedException(true);
    return SEQ<TYPE_CPP_Stmt>().ImpAppend (NotSupported (L"Function value",rc1));
  }
#endif // VDMPP

  TYPE_CPP_Expr tmpRec_v;
  SEQ<TYPE_CPP_Stmt> rec_stmt;
  TYPE_REP_TypeRep tmpRecType;
  if ( expr1.IsNil() )
  {
    Tuple cgee(CGExprExcl(rec, ASTAUX::MkId(L"tmpRec"),nil));
    tmpRec_v = cgee.GetRecord(1);
    rec_stmt.ImpConc(cgee.GetSequence(2));
    tmpRecType = RemoveInvType(FindType(rec));
  }
  else
  {
    tmpRec_v = expr1;
    tmpRecType = type1;
  }


  SEQ<TYPE_CPP_Stmt> rb (rec_stmt);

  SET<TYPE_REP_CompositeTypeRep> rectypes (FindAllRecTypes(tmpRecType));
  SET<TYPE_REP_CompositeTypeRep> posTypes (FindPossibleRecTypes(rectypes, nm));
#ifdef VDMPP
  SET<TYPE_REP_ObjRefTypeRep> oreftypes (FindAllObjRefTypes(tmpRecType));
  posTypes.ImpUnion(FindPossibleObjRefTypes(oreftypes, nm));
#endif // VDMPP


  // TODO: error case
  // types
  // T :: m : map nat to nat;
  //
  // class C
  // operations
  // public m : nat ==> nat
  // ...
  //
  // let x : T | C = new C()
  // in x.m(3) <-- can't generate correct code
  // 
  if (!posTypes.IsEmpty())
    rb.ImpConc(FindFieldSelApply (nm, vt, mk_CG_VT (tmpRec_v, tmpRecType)));

#ifdef VDMPP
    if (IsUnionFunctionType (ti))
    {
      SetNotSupportedException(true); //20120924
      if (vdm_CPP_isCPP())
      {
        if (!ti.Is(TAG_TYPE_REP_OverTypeRep))
          rb.ImpAppend (vdm_BC_GenIfStmt (GenIsClass (tmpRec_v), NotSupported (L"Function value", rc1), Nil ()));
      }
      else
      { // java
        SET<TYPE_REP_ObjRefTypeRep> ti_s;
        SET<TYPE_AS_Name> nm_s;
        SET<TYPE_REP_TypeRep> t_s (tmpRecType.GetSet(1));
        Generic g;
        for (bool bb = t_s.First(g); bb; bb = t_s.Next(g))
        {
          TYPE_REP_TypeRep ti2 (g);
          if (ti2.Is(TAG_TYPE_REP_ObjRefTypeRep))
          {
            TYPE_REP_ObjRefTypeRep ortr (ti2);
            ti_s.Insert(ortr);
            ortr = UnQClassType(ortr);
            TYPE_AS_Name onm (ortr.get_nm());
            if ( !GetStatSem().LookUpOpOrFnName(onm, nm).IsNil() ) {
              nm_s.Insert(onm);
            }
          }
        }
        rb.ImpAppend(vdm_BC_GenIfStmt (GenIsClasses (nm_s,tmpRec_v), NotSupported (L"Function value", rc1), Nil ()));
      }
    }
#endif // VDMPP
  return rb;
}

// FindAllRecTypes
// type : REP`TypeRep
// ==> set of REP`CompositeTypeRep
SET<TYPE_REP_CompositeTypeRep> vdmcg::FindAllRecTypes (const TYPE_REP_TypeRep & type)
{
  SET<TYPE_REP_CompositeTypeRep> st;
  switch(type.GetTag()) {
    case TAG_TYPE_REP_TypeNameRep: {
      st.ImpUnion(FindAllRecTypes(ExpandTypeRep(type, Set())));
      break;
    }
    case TAG_TYPE_REP_CompositeTypeRep: {
      st.Insert (type);
      break;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> st2 (type.GetSet(pos_REP_UnionTypeRep_tps));
      Generic g;
      for (bool bb = st2.First(g); bb; bb = st2.Next(g))
        st.ImpUnion (FindAllRecTypes (g));
      break;
    }
    case TAG_TYPE_REP_InvTypeRep: {
      st.ImpUnion(FindAllRecTypes(type.GetRecord(pos_REP_InvTypeRep_shape)));
    }
    default:
      break;
  }
  return st;
}

#ifdef VDMPP
// FindAllObjRefTypes
// type : REP`TypeRep
// ==> set of REP`ObjRefTypeRep
SET<TYPE_REP_ObjRefTypeRep>  vdmcg::FindAllObjRefTypes (const TYPE_REP_TypeRep & type)
{
  SET<TYPE_REP_ObjRefTypeRep> st;
  switch(type.GetTag()) {
    case TAG_TYPE_REP_ObjRefTypeRep: {
      st.Insert (type);
      break;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> st2 (type.GetSet(pos_REP_UnionTypeRep_tps));
      Generic g;
      for (bool bb = st2.First(g); bb; bb = st2.Next(g))
        st.ImpUnion (FindAllObjRefTypes (g));
      break;
    }
    default:
      break;
  }
  return st;
}

// 20120920 -->
// FindPossibleObjRefTypes
// tps : set of REP`ObjRefTypeRep
// fsnm : AS`Name
// ==> set of REP`ObjRefTypeRep
SET<TYPE_REP_ObjRefTypeRep> vdmcg::FindPossibleObjRefTypes (const SET<TYPE_REP_ObjRefTypeRep> & tps_,
                                                            const TYPE_AS_Name & fsnm)
{
  SET<TYPE_REP_ObjRefTypeRep> tps (tps_);
  SET<TYPE_REP_ObjRefTypeRep> res;
  Generic t;
  for (bool bb = tps.First(t); bb; bb = tps.Next(t))
  {
    TYPE_REP_ObjRefTypeRep otr (t);
    const TYPE_AS_Name & clnm (otr.GetRecord(pos_REP_ObjRefTypeRep_nm));
    TYPE_AS_Name nm (ASTAUX::Combine2Names(clnm, fsnm));
    if (GetStatSem().IsInstanceVar(nm))
      res.Insert(t);
// 20130212 -->
//    else if (!GetStatSem().LookUpOpOrFnName(clnm, fsnm).IsNil())
//      res.Insert(t);
// <-- 20130212
  } 
  return res;
}
// <-- 20120920

#endif // VDMPP

// FindFieldSelApply
// fsnm : AS`Name
// rcl : CGMAIN`VT
// rc2 : CGMAIN`VT
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::FindFieldSelApply (const TYPE_AS_Name & fsnm,
                                             const TYPE_CGMAIN_VT & rc1,
                                             const TYPE_CGMAIN_VT & rc2)
{
  const TYPE_CPP_Expr & tmpRec        (rc2.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & tmpRecType_q (rc2.GetRecord(pos_CGMAIN_VT_type));
  const TYPE_CPP_Expr & resVar_v      (rc1.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & resTp      (rc1.GetRecord(pos_CGMAIN_VT_type));

  TYPE_REP_TypeRep tmpRecType (RemoveNil(tmpRecType_q));

  SET<TYPE_REP_CompositeTypeRep> rectypes (FindAllRecTypes(tmpRecType));
  SET<TYPE_REP_CompositeTypeRep> posRecTypes (FindPossibleRecTypes (rectypes, fsnm));
#ifdef VDMPP
  SET<TYPE_REP_ObjRefTypeRep> oreftypes (FindAllObjRefTypes(tmpRecType));
  SET<TYPE_REP_ObjRefTypeRep> posORefTypes (FindPossibleObjRefTypes(oreftypes, fsnm));
#endif //VDMPP

#ifdef VDMSL
  TYPE_CPP_Stmt rti (vdm_BC_GenBlock(mk_sequence((RunTime(L"A record was expected")))));
#endif // VDMSL
#ifdef VDMPP
  TYPE_CPP_Stmt rti (vdm_BC_GenBlock(mk_sequence((RunTime(L"An object or record was expected")))));
#endif // VDMPP

  //TYPE_CPP_Stmt alt (rti);
  Generic alt = rti;
  if (tmpRecType.Is(TAG_TYPE_REP_UnionTypeRep))
  {
    const SET<TYPE_REP_TypeRep> & tps (tmpRecType.GetSet(pos_REP_UnionTypeRep_tps));
#ifdef VDMSL
    if (tps == posRecTypes)
#endif // VDMSL
#ifdef VDMPP
    if (tps == posRecTypes.Union(posORefTypes))
#endif // VDMPP
      alt = Nil();
  }

  if (vdm_CPP_isCPP())
  {
    if (!posRecTypes.IsEmpty())
    {
      if (tmpRecType.Is(TAG_TYPE_REP_CompositeTypeRep) && (posRecTypes.Card() == 1))
      {
        TYPE_CPP_Expr getfield (GenRecGetFieldNm (tmpRec, posRecTypes.GetElem(), fsnm));
        return mk_sequence(vdm_BC_GenAsgnStmt (resVar_v, getfield));
      }
      else
      {
        Generic t;
        for (bool bb = posRecTypes.First(t); bb; bb = posRecTypes.Next(t))
        {
          TYPE_CPP_Expr castrec (IsCompositeType(tmpRecType) ? tmpRec : GenCastType(t, tmpRec));
          TYPE_CPP_Expr getfield (GenRecGetFieldNm (castrec, t, fsnm));
          TYPE_CPP_Stmt asgn (vdm_BC_GenBlock(mk_sequence(vdm_BC_GenAsgnStmt (resVar_v, getfield))));
          TYPE_CPP_Expr isRecord (GenIsThisRecord(t, tmpRec));
          if (alt.IsNil())
            alt = asgn;
          else
            alt = vdm_BC_GenIfStmt(isRecord, asgn, alt);
        }
      }
    }
#ifdef VDMPP
    if (!posORefTypes.IsEmpty())
    {
      if (tmpRecType.Is(TAG_TYPE_REP_ObjRefTypeRep) && (posORefTypes.Card() == 1))
      {
        TYPE_REP_ObjRefTypeRep otr (posORefTypes.GetElem());
        const TYPE_AS_Name & clnm (otr.GetRecord(pos_REP_ObjRefTypeRep_nm));
        TYPE_CPP_Expr objptr (CastToClassPtr(clnm, tmpRec));
        TYPE_CPP_Expr getfield (vdm_BC_GenPointerToObjectMemberAccess(objptr, vdm_BC_Rename(fsnm)));
        return mk_sequence(vdm_BC_GenAsgnStmt(resVar_v, getfield));
      }
      else
      {
        Generic t;
        for (bool bb = posORefTypes.First(t); bb; bb = posORefTypes.Next(t))
        {
          TYPE_REP_ObjRefTypeRep otr (t);
          const TYPE_AS_Name & clnm (otr.GetRecord(pos_REP_ObjRefTypeRep_nm));
          TYPE_CPP_Expr objptr (CastToClassPtr(clnm, tmpRec));
          TYPE_CPP_Expr getfield (vdm_BC_GenPointerToObjectMemberAccess(objptr, vdm_BC_Rename(fsnm)));
          TYPE_CPP_Stmt l_asgn (vdm_BC_GenBlock(mk_sequence(vdm_BC_GenAsgnStmt(resVar_v, getfield))));
          TYPE_CPP_Expr if_cond (GenAuxType(tmpRec, otr));
          if (alt.IsNil())
            alt = l_asgn;
          else
            alt = vdm_BC_GenIfStmt(if_cond, l_asgn, alt);
        }
      }
    }
#endif // VDMPP
  }
#ifdef VDMPP
  else // java
  {
    if (tmpRecType.Is(TAG_TYPE_REP_CompositeTypeRep) && (posRecTypes.Card() == 1))
    {
      const SEQ<TYPE_REP_FieldRep> & fields (tmpRecType.GetSequence(pos_REP_CompositeTypeRep_fields));
      int i (FindField(fields, fsnm));
      const TYPE_REP_FieldRep & fi (fields[i]);
      const TYPE_REP_TypeRep & type (fi.GetRecord(pos_REP_FieldRep_tp));
      TYPE_CPP_Expr rec (tmpRec.Is(TAG_TYPE_CPP_CastExpr) ? vdm_BC_GenBracketedExpr(tmpRec) : tmpRec);
      TYPE_CPP_QualifiedName getfield (vdm_BC_GenQualifiedName(rec, vdm_BC_Rename2(fsnm)));
      TYPE_CPP_Expr cast (IsSubType(type, resTp) ? getfield : GenExplicitCast(resTp, getfield, type));
      return mk_sequence(vdm_BC_GenAsgnStmt(resVar_v, cast));
    }
    else
    {
      Generic t;
      for (bool bb = posRecTypes.First(t); bb; bb = posRecTypes.Next(t))
      {
        TYPE_CPP_Expr rec (vdm_BC_GenBracketedExpr(GenCastType(t, tmpRec)));

        TYPE_REP_CompositeTypeRep ctr (t);
        const SEQ<TYPE_REP_FieldRep> & fields (ctr.GetSequence(pos_REP_CompositeTypeRep_fields));

        int i (FindField(fields, fsnm));
        const TYPE_REP_FieldRep & fi (fields[i]);
        const TYPE_REP_TypeRep & type (fi.GetRecord(pos_REP_FieldRep_tp));

        TYPE_CPP_QualifiedName getfield (vdm_BC_GenQualifiedName(rec, vdm_BC_Rename2(fsnm)));
        TYPE_CPP_Expr cast (IsSubType(type, resTp) ? getfield : GenExplicitCast(resTp, getfield, type));
        TYPE_CPP_Stmt l_asgn (vdm_BC_GenBlock(mk_sequence(vdm_BC_GenAsgnStmt(resVar_v, cast))));
        if (alt.IsNil())
          alt = l_asgn;
        else
          alt = vdm_BC_GenIfStmt(GenIsThisRecord(ctr, tmpRec), l_asgn, alt);
      }
      if (!posORefTypes.IsEmpty())
      {
        if (tmpRecType.Is(TAG_TYPE_REP_ObjRefTypeRep) && (posORefTypes.Card() == 1))
        {
          TYPE_CPP_Expr rec (tmpRec.Is(TAG_TYPE_CPP_CastExpr) ? vdm_BC_GenBracketedExpr(tmpRec) : tmpRec);
          TYPE_CPP_QualifiedName getfield (vdm_BC_GenQualifiedName(rec, vdm_BC_Rename2(fsnm)));
          return mk_sequence(vdm_BC_GenAsgnStmt(resVar_v, getfield));
        }
        else
        {
          Generic t;
          for (bool bb = posORefTypes.First(t); bb; bb = posORefTypes.Next(t))
          {
            TYPE_REP_ObjRefTypeRep otr (t);
            TYPE_CPP_Expr rec (vdm_BC_GenBracketedExpr(GenCastType(otr, tmpRec)));
            const TYPE_AS_Name & clnm (otr.GetRecord(pos_REP_ObjRefTypeRep_nm));
            TYPE_CPP_QualifiedName getfield (vdm_BC_GenQualifiedName(rec, vdm_BC_Rename2(fsnm)));
            TYPE_CPP_Stmt l_asgn (vdm_BC_GenBlock(mk_sequence(vdm_BC_GenAsgnStmt(resVar_v, getfield))));
            TYPE_CPP_Expr if_cond (vdm_BC_GenTypeComp(vdm_BC_Rename(clnm), tmpRec));
            if (alt.IsNil())
              alt = l_asgn;
            else
              alt = vdm_BC_GenIfStmt(if_cond, l_asgn, alt);
          }
        }
      }
    }
  }
#endif // VDMPP
  return StripCompoundStmt(alt);
}

// FindPossibleRecTypes
// tps : set of REP`CompositeTypeRep
// fsnm : AS`Name
// ==> set of REP`CompositeTypeRep
SET<TYPE_REP_CompositeTypeRep> vdmcg::FindPossibleRecTypes (const SET<TYPE_REP_CompositeTypeRep> & tps_,
                                                            const TYPE_AS_Name & fsnm)
{
  Set tps (tps_); // for safe
  SET<TYPE_REP_CompositeTypeRep> restps;
  Generic tp;
  for (bool bb = tps.First(tp); bb; bb = tps.Next(tp))
  {
    const SEQ<TYPE_REP_FieldRep> & fields (Record(tp).GetSequence(pos_REP_CompositeTypeRep_fields));

    bool exists = false;
    size_t len_fields = fields.Length();
    for (size_t idx = 1; (idx <= len_fields) && !exists; idx++)
      exists = (fields[idx].get_sel () == fsnm);

    if (exists)
      restps.Insert (tp);
  }
  return restps;
}

// CGRecordModifierExpr
// RME : AS`RecordModifierExpr
// vt : CGMAIN`VT
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGRecordModifierExpr(const TYPE_AS_RecordModifierExpr & RME,
                                               const TYPE_CGMAIN_VT & vt)
{
  const TYPE_AS_Expr & rec (RME.GetRecord(pos_AS_RecordModifierExpr_rec));
  const SEQ<TYPE_AS_RecordModification> & mu_l (RME.GetSequence(pos_AS_RecordModifierExpr_modifiers));

  SEQ<TYPE_CPP_Stmt> rb_l;
  Generic tmpTp (FindType(rec));

  Tuple cgee (CGExprExcl(rec, ASTAUX::MkId(L"tmpRE"), nil));
  const TYPE_CPP_Expr & tmpRE (cgee.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & rec_stmt (cgee.GetSequence(2));

  TYPE_CPP_Expr REVar;
  if (rec_stmt.IsEmpty())
  {
    REVar = vdm_BC_GiveName(ASTAUX::MkId(L"tmpRE"));
    if (vdm_CPP_isCPP())
      rb_l.ImpConc(GenDeclInit_DS(tmpTp, REVar, tmpRE));
    else
    { // java
// 20120510 -->
//      if (IsCompositeType(tmpTp))
      if (IsCompositeType(tmpTp) && !tmpRE.Is(TAG_TYPE_CPP_ClassInstanceCreationExpr))
// <-- 20120510
        rb_l.ImpConc(GenDeclInit_DS(tmpTp,
                                      REVar,
                                      GenExplicitCast(tmpTp,
                                                      vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"UTIL.clone")),
                                                                        SEQ<TYPE_CPP_Expr>().ImpAppend(tmpRE)),
                                                      nil)));
      else
        rb_l.ImpConc(GenDeclInit_DS(tmpTp, REVar, tmpRE));
    }
  }
  else
  {
    REVar = tmpRE;
    rb_l = rec_stmt;
  }

  if (IsCompositeType(tmpTp)) {
    rb_l.ImpConc(GenModifyFields(REVar, tmpTp, mu_l, vt));
  } else {
    TYPE_CPP_Expr cond (GenIsRecord(tmpRE));
    type_dL stmt_l(GenModifyFields(REVar, tmpTp, mu_l, vt));
    TYPE_CPP_Stmt rti (vdm_BC_GenBlock(mk_sequence(RunTime(L"A record was expected in record modifier expression"))));
    rb_l.ImpAppend(vdm_BC_GenIfStmt(cond, vdm_BC_GenBlock(stmt_l), rti));
  }
  // TODO:invariant check for REVar ??
  return rb_l;
}

// GenModifyFields
// rec : CPP`Name
// recTp : REP`TypeRep
// mu_l : seq1 of AS`RecordModification
// vt : CGMAIN`VT
// ==> seq1 of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenModifyFields(const TYPE_CPP_Name & rec,
                                          const TYPE_REP_TypeRep & recTp,
                                          const SEQ<TYPE_AS_RecordModification> & mu_l,
                                          const TYPE_CGMAIN_VT & vt)
{
  const TYPE_CPP_Expr & resVar (vt.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & resTp (vt.GetRecord(pos_CGMAIN_VT_type));

  TYPE_CPP_Name tmpRec (rec);
  SEQ<TYPE_CPP_Stmt> rb_l;
  if (!IsCompositeType(recTp))
  {
    tmpRec = vdm_BC_GiveName(ASTAUX::MkId(L"tmpRec"));
    if (vdm_CPP_isCPP())
      rb_l.ImpAppend(vdm_BC_GenDecl(GenRecordType(nil), tmpRec, vdm_BC_GenAsgnInit(rec)));
    else
    { // Java
      TYPE_CPP_Expr clonefct (vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"UTIL.clone")), mk_sequence(rec)));
      rb_l.ImpAppend(vdm_BC_GenDecl(GenRecordType(nil), tmpRec, vdm_BC_GenAsgnInit(GenCastRecord(clonefct, nil))));
    }
  }

  TYPE_CPP_Stmt fi (GenFieldInds(tmpRec, recTp, mu_l));
  rb_l.ImpConc(StripCompoundStmt(fi));

  if (vdm_CPP_isCPP())
    rb_l.ImpAppend(vdm_BC_GenAsgnStmt(resVar, tmpRec));
  else
    rb_l.ImpAppend(vdm_BC_GenAsgnStmt(resVar, GenExplicitCast(resTp, tmpRec, recTp)));
  
  return rb_l;
}

// GenFieldInds
// rec : CPP`Name
// recTp : REP`TypeRep
// mu_l : seq1 of AS`RecordModification
// ==> CPP`Stmt
TYPE_CPP_Stmt vdmcg::GenFieldInds(const TYPE_CPP_Name & rec,
                                  const TYPE_REP_TypeRep & recTp,
                                  const SEQ<TYPE_AS_RecordModification> & mu_l)
{
  SET<TYPE_REP_CompositeTypeRep> rectp_s (FindAllRecTypes(recTp));
  SET<TYPE_REP_CompositeTypeRep> postp_s (rectp_s);

  size_t len_mu_l = mu_l.Length();
  for (size_t idx = 1; idx <= len_mu_l; idx++)
  {
    const TYPE_AS_RecordModification & rm (mu_l[idx]);
    postp_s.ImpIntersect(FindPossibleRecTypes(rectp_s, rm.GetRecord(pos_AS_RecordModification_field)));
  }

  TYPE_CPP_Stmt errstmt (RunTime(L"Unknown record field selector in record modifier expression"));

  switch (postp_s.Card()) {
    case 0: {
      return errstmt;
    }
    case 1: {
      TYPE_REP_CompositeTypeRep t (postp_s.GetElem());
      if (vdm_CPP_isCPP())
      {
        SEQ<TYPE_CPP_Stmt> stmt_l;
        for (size_t idx = 1; idx <= len_mu_l; idx++)
        {
          const TYPE_AS_RecordModification & rm (mu_l[idx]);
          TYPE_CPP_Expr fieldno (GenRecGetFieldNo(t, rm.GetRecord(pos_AS_RecordModification_field)));
// 20120223 -->
          const TYPE_AS_Expr & newval (rm.GetRecord(pos_AS_RecordModification_newexpr));
          Tuple tpl (CGExprExcl(newval, ASTAUX::MkId(L"val"), nil));
          const TYPE_CPP_Expr & val (tpl.GetRecord(1));
          const SEQ<TYPE_CPP_Stmt> & val_stmt(tpl.GetSequence(2));

          stmt_l.ImpConc(val_stmt);
          stmt_l.ImpAppend(GenRecSetField(rec, fieldno, val));
// <-- 20120223
        }
        return GenCPPStmt(stmt_l);
      }
      else
      {
        const TYPE_AS_Name & tagnm (t.GetRecord(pos_REP_CompositeTypeRep_nm));
        SEQ<TYPE_CPP_Stmt> stmt_l;
        const SEQ<TYPE_REP_FieldRep> & fields (t.GetSequence(pos_REP_CompositeTypeRep_fields));
        for (size_t idx = 1; idx <= len_mu_l; idx++)
        {
          const TYPE_AS_RecordModification & rm (mu_l[idx]);
          const TYPE_AS_Name & nm (rm.GetRecord(pos_AS_RecordModification_field));
          const TYPE_AS_Expr & newval (rm.GetRecord(pos_AS_RecordModification_newexpr));
          int j = FindField(fields, nm);
          const TYPE_REP_FieldRep & fi (fields[j]);
          const TYPE_REP_TypeRep & type (fi.GetRecord(pos_REP_FieldRep_tp));

          Tuple tpl (CGExprExcl(newval, ASTAUX::MkId(L"val"), nil));
          const TYPE_CPP_Expr & val (tpl.GetRecord(1));
          const SEQ<TYPE_CPP_Stmt> & val_stmt(tpl.GetSequence(2));

          TYPE_CPP_Expr cast (IsCompositeType(recTp) ? rec : vdm_BC_GenBracketedExpr(GenCastRecord(rec, tagnm)));
          stmt_l.ImpConc(val_stmt);
          stmt_l.ImpAppend(vdm_BC_GenAsgnStmt(vdm_BC_GenQualifiedName(cast, vdm_BC_Rename2(nm)),
                                              GenExplicitCast(type, val, FindType(newval))));
        }
        return GenCPPStmt(stmt_l);
      }
    }
    default: {
      if (vdm_CPP_isCPP())
      {
        SEQ<TYPE_CPP_CaseStmt> case_l;
        Generic ctr_g;
        for (bool b1 = postp_s.First(ctr_g); b1; b1 = postp_s.Next(ctr_g))
        {
          TYPE_REP_CompositeTypeRep t (ctr_g);
          SEQ<TYPE_CPP_Stmt> stmt_l;
          for (size_t idx = 1; idx <= len_mu_l; idx++)
          {
            const TYPE_AS_RecordModification & rm (mu_l[idx]);
            TYPE_CPP_Expr fieldno (GenRecGetFieldNo(t, rm.GetRecord(pos_AS_RecordModification_field)));
// 20120223 -->
            const TYPE_AS_Expr & newval (rm.GetRecord(pos_AS_RecordModification_newexpr));
            Tuple tpl (CGExprExcl(newval, ASTAUX::MkId(L"val"), nil));
            const TYPE_CPP_Expr & val (tpl.GetRecord(1));
            const SEQ<TYPE_CPP_Stmt> & val_stmt(tpl.GetSequence(2));

            stmt_l.ImpConc(val_stmt);
            stmt_l.ImpAppend(GenRecSetField(rec, fieldno, val));
// <-- 20120213
          }
          case_l.ImpAppend(vdm_BC_GenCaseStmt(vdm_BC_Rename(t.GetRecord(pos_REP_CompositeTypeRep_nm)),
                                              vdm_BC_GenBlock(SEQ<TYPE_CPP_Stmt>()
                                                                .ImpConc(stmt_l)
                                                                .ImpAppend(vdm_BC_GenBreakStmt(Nil())))));
        }
        case_l.ImpAppend(vdm_BC_GenDefaultStmt( vdm_BC_GenBlock(mk_sequence(errstmt, vdm_BC_GenBreakStmt(Nil())))));

//        TYPE_CPP_Expr tmpRec (IsCompositeType(recTp) ? rec : vdm_BC_GenCastExpr(GenRecordType(Nil()), rec));
                        
        return vdm_BC_GenSwitchStmt(GenGetTag(rec), case_l);
      }
      else
      { // java
        TYPE_CPP_Stmt ifs (errstmt);
        Generic ctr_g;
        for (bool b1 = postp_s.First(ctr_g); b1; b1 = postp_s.Next(ctr_g))
        {
          TYPE_REP_CompositeTypeRep t (ctr_g);
          TYPE_CPP_Expr isRecord (GenIsThisRecord(t, rec));
          SEQ<TYPE_CPP_Stmt> stmt_l;
          const TYPE_AS_Name & tagnm (t.GetRecord(pos_REP_CompositeTypeRep_nm));
          const SEQ<TYPE_REP_FieldRep> & fields (t.GetSequence(pos_REP_CompositeTypeRep_fields));
          for (size_t idx = 1; idx <= len_mu_l; idx++)
          {
            const TYPE_AS_RecordModification & rm (mu_l[idx]);
            const TYPE_AS_Name & nm (rm.GetRecord(pos_AS_RecordModification_field));
            const TYPE_AS_Expr & newval (rm.GetRecord(pos_AS_RecordModification_newexpr));
            int j = FindField(fields, nm);
            const TYPE_REP_FieldRep & fi (fields[j]);
            const TYPE_REP_TypeRep & type (fi.GetRecord(pos_REP_FieldRep_tp));

            Tuple tpl (CGExprExcl(newval, ASTAUX::MkId(L"val"), nil));
            const TYPE_CPP_Expr & val (tpl.GetRecord(1));
            const SEQ<TYPE_CPP_Stmt> & val_stmt(tpl.GetSequence(2));

            TYPE_CPP_Expr cast (vdm_BC_GenBracketedExpr(GenCastRecord(rec, tagnm)));
            stmt_l.ImpConc(val_stmt);
            stmt_l.ImpAppend(vdm_BC_GenAsgnStmt(vdm_BC_GenQualifiedName(cast, vdm_BC_Rename2(nm)),
                                                GenExplicitCast(type, val, FindType(newval))));
          }
          ifs = vdm_BC_GenIfStmt(isRecord, GenCPPStmt(stmt_l), ifs);
        }
        return ifs;
      }
    }
  }
}

// FindField
// fieldreps : seq of REP`FieldRep
// nm : AS`Name
// ==> nat
int vdmcg::FindField(const SEQ<TYPE_REP_FieldRep> & fieldreps, const TYPE_AS_Name & nm)
{
  size_t len_fieldreps = fieldreps.Length();
  for (size_t idx = 1; idx <= len_fieldreps; idx++)
  {
    const TYPE_REP_FieldRep & fr (fieldreps[idx]);
    const Generic & sel (fr.GetField(pos_REP_FieldRep_sel));
    // const TYPE_REP_TypeRep & tp (fr.get_tp(pos_REP_FieldRep_tp));
    if (!sel.IsNil())
      if (nm == sel) return idx;
  }
  return fieldreps.Length() + 1;
}

// CGAllOrExistsExpr
// quant : AS`QuantExpr
// vt : CGMAIN`VT
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGAllOrExistsExpr (const TYPE_AS_QuantExpr & quant, const TYPE_CGMAIN_VT & vt)
{
  const TYPE_CPP_Expr & resVar_v (vt.GetRecord(pos_CGMAIN_VT_name));
  TYPE_CPP_Identifier tmpQuant_v (vdm_BC_GiveName(ASTAUX::MkId(L"tmpQuant")));

  MAP<TYPE_AS_Name, TYPE_REP_TypeRep> pid_m (FindPatIdMap(quant));

  PushEnv_CGAUX();
  InsertNamesinEnv_CGAUX(pid_m.Dom());

  POSITION::SetPosition(quant);

  SEQ<TYPE_CPP_Stmt> rb;
  TYPE_CPP_Expr res;
  switch (quant.GetTag()) {
    case TAG_TYPE_AS_AllOrExistsExpr: {
      int kind = quant.GetIntValue(pos_AS_AllOrExistsExpr_quant);
      const SEQ<TYPE_AS_MultBind> & bind_l (quant.GetSequence(pos_AS_AllOrExistsExpr_bind));
      const TYPE_AS_Expr & pred (quant.GetRecord(pos_AS_AllOrExistsExpr_pred));
  
      switch(kind) {
        case ALL: {
          TYPE_CPP_AsgnInit qinit (vdm_BC_GenAsgnInit(vdm_BC_GenBoolLit(true)));
          //TYPE_CPP_Stmt np (vdm_BC_GenAsgnStmt(tmpQuant_v, vdm_BC_GenBoolLit(false)));
          TYPE_CPP_Stmt np (vdm_BC_GenBlock(mk_sequence(vdm_BC_GenAsgnStmt(tmpQuant_v, vdm_BC_GenBoolLit(false)))));
          rb.ImpAppend(vdm_BC_GenDecl(GenSmallBoolType(), tmpQuant_v, qinit));
          rb.ImpAppend(CGComprehension(bind_l, pred, Sequence(), tmpQuant_v, np, pid_m, true));
          res = tmpQuant_v;
          break;
        }
        case EXISTS: {
          TYPE_CPP_AsgnInit qinit (vdm_BC_GenAsgnInit(vdm_BC_GenBoolLit(false)));
          TYPE_CPP_Stmt stmt (vdm_BC_GenAsgnStmt(tmpQuant_v, vdm_BC_GenBoolLit(true)));
          TYPE_CPP_Expr contexpr (vdm_BC_GenNot(tmpQuant_v));
          rb.ImpAppend(vdm_BC_GenDecl(GenSmallBoolType(), tmpQuant_v, qinit));
          rb.ImpAppend(CGComprehension(bind_l, pred, mk_sequence(stmt), contexpr, nil, pid_m, true));
          res = tmpQuant_v;
          break;
        }
      }
      break;
    }
    case TAG_TYPE_AS_ExistsUniqueExpr: {
      const TYPE_AS_Bind & bind (quant.GetRecord(pos_AS_ExistsUniqueExpr_bind));
  
      switch(bind.GetTag()) {
        case TAG_TYPE_AS_SetBind: {
          const TYPE_AS_Expr & pred (quant.GetRecord(pos_AS_ExistsUniqueExpr_pred));
          TYPE_CPP_AsgnInit qinit (vdm_BC_GenAsgnInit(vdm_BC_GenIntegerLit(0)));
          rb.ImpAppend(vdm_BC_GenDecl(GenSmallIntType(), tmpQuant_v, qinit));
          SEQ<TYPE_AS_Pattern> pat_l;
          pat_l.ImpAppend(bind.GetRecord(pos_AS_SetBind_pat));

          TYPE_AS_MultSetBind msb;
          msb.Init(pat_l, bind.GetRecord(pos_AS_SetBind_Set), bind.GetInt(pos_AS_SetBind_cid));

          SEQ<TYPE_AS_MultBind> bind_l;
          bind_l.ImpAppend(msb);
          TYPE_CPP_Stmt stmt (vdm_BC_GenExpressionStmt(vdm_BC_GenPostPlusPlus(tmpQuant_v)));
          //TYPE_CPP_Expr contexpr (vdm_BC_GenBracketedExpr(vdm_BC_GenLeq(tmpQuant_v, vdm_BC_GenIntegerLit(2))));
          TYPE_CPP_Expr contexpr (vdm_BC_GenBracketedExpr(vdm_BC_GenLt(tmpQuant_v, vdm_BC_GenIntegerLit(2))));
          rb.ImpAppend(CGComprehension(bind_l, pred, mk_sequence(stmt), contexpr, nil, pid_m, true));
          res = vdm_BC_GenEq(tmpQuant_v, vdm_BC_GenIntegerLit(1));
          break;
        }
        case TAG_TYPE_AS_TypeBind: {
          SetNotSupportedException(true);
          return (Sequence().ImpAppend(NotSupported(L"type bind", quant)));
          break;
        }
      }
      break;
    }
    default: {
      PopEnv_CGAUX();
      SetNotSupportedException(true);
      return Sequence().ImpAppend(NotSupported(L"type bind", quant));
    }
  }
  rb.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, GenBoolExpr(res)));
  PopEnv_CGAUX();
  return rb;
}

// CGIotaExpr
// rc1 : AS`IotaExpr
// vt : CGMAIN`VT
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGIotaExpr(const TYPE_AS_IotaExpr & rc1, const TYPE_CGMAIN_VT & vt)
{
  const TYPE_AS_Bind & bind (rc1.GetRecord(pos_AS_IotaExpr_bind));

  switch(bind.GetTag()) {
    case TAG_TYPE_AS_TypeBind: {
      SetNotSupportedException(true);
      return (Sequence().ImpAppend(NotSupported(L"type bind", rc1)));
    }
    case TAG_TYPE_AS_SetBind: {
      const TYPE_AS_Expr & pred (rc1.GetRecord(pos_AS_IotaExpr_pred));
      const TYPE_CPP_Expr & resVar (vt.GetRecord(pos_CGMAIN_VT_name));
      const TYPE_AS_Pattern & pat (bind.GetRecord(pos_AS_SetBind_pat));
      const TYPE_AS_Expr & expr (bind.GetRecord(pos_AS_SetBind_Set));

      TYPE_CPP_Identifier count (vdm_BC_GiveName(ASTAUX::MkId(L"count")));
      TYPE_CPP_Identifier succ (vdm_BC_GiveName(ASTAUX::MkId(L"succ")));
      TYPE_CPP_Identifier tmpSet (vdm_BC_GiveName(ASTAUX::MkId(L"tmpSet")));

      Generic setType (FindType(expr));

      SET<TYPE_AS_Name> pid_pat_s (FindPatternId(pat).Dom());
      SET<TYPE_AS_Name> pid_expr_s (FindAllNamesInExpr(expr));

      Tuple set_cgee (CGExprExcl(expr, ASTAUX::MkId(L"tmpSet"), nil));
      const TYPE_CPP_Expr & tmpSet1 (set_cgee.GetRecord(1));
      const SEQ<TYPE_CPP_Stmt> & set_stmt (set_cgee.GetSequence(2));

      SEQ<TYPE_CPP_Stmt> decl(DeclarePatVars(pat)); // must be after CGExpr and before CGExprExcl and CGPatternMatchExcl

      SEQ<TYPE_CPP_Stmt> rb_l (set_stmt);
      if ((!(pid_pat_s.ImpIntersect(pid_expr_s)).IsEmpty()) || !expr.Is(TAG_TYPE_AS_Name))
      {
        rb_l.ImpConc(GenConstDeclInit(setType, tmpSet, tmpSet1));
      }
      else
      {
        tmpSet = tmpSet1;
      }

      if (!IsSetType(setType))
      {
        TYPE_CPP_Identifier tmpSet_q (vdm_BC_GiveName(ASTAUX::MkId(L"tmpSet")));
        TYPE_CPP_Expr cond (GenIsSet(tmpSet));

        TYPE_CPP_Stmt rti(vdm_BC_GenBlock(mk_sequence(RunTime(L"A set was expected"))));
        rb_l.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenNot(cond), rti, nil));
        if (vdm_CPP_isCPP())
          rb_l.ImpAppend(vdm_BC_GenDecl(GenSetType(), tmpSet_q, vdm_BC_GenAsgnInit(tmpSet)));
        else
          rb_l.ImpAppend(vdm_BC_GenDecl(GenSetType(), tmpSet_q, vdm_BC_GenAsgnInit(GenCastSetType(tmpSet))));
        tmpSet = tmpSet_q;
      }

//      SEQ<TYPE_CPP_Stmt> decl(DeclarePatVars(pat)); // must be after CGExpr and before CGExprExcl and CGPatternMatchExcl

      TYPE_CPP_Identifier tmpElem (vdm_BC_GiveName(ASTAUX::MkId(L"tmpElem")));
      TYPE_REP_TypeRep settp (FindSetElemType(setType));
      TYPE_CGMAIN_VT elemVT (mk_CG_VT(tmpElem, settp));
      TYPE_REP_BooleanTypeRep bt;

      // pred
      Tuple cgee (CGExprExcl(pred, ASTAUX::MkId(L"tmpB"), nil));
      const TYPE_CPP_Expr & tmpB (cgee.GetRecord(1));
      const SEQ<TYPE_CPP_Stmt> & pr_stmt (cgee.GetSequence(2));

      TYPE_REP_TypeRep pred_type (FindType(pred));

      SEQ<TYPE_CPP_Stmt> if_then;
      if_then.ImpAppend(vdm_BC_GenExpressionStmt(vdm_BC_GenPostPlusPlus(count)));
      //
      //if_then.ImpAppend(vdm_BC_GenAsgnStmt(resVar, tmpElem));
      if (vdm_CPP_isCPP()) {
        if_then.ImpAppend(vdm_BC_GenAsgnStmt(resVar, tmpElem));
      }
      else {
        const TYPE_REP_TypeRep & resTp (vt.GetRecord(pos_CGMAIN_VT_type));
        TYPE_CPP_Expr cast (IsSubType(settp, resTp ) ? tmpElem : GenCastType(resTp, tmpElem));
        if_then.ImpAppend(vdm_BC_GenAsgnStmt(resVar, cast));
      }

      SEQ<TYPE_CPP_Stmt> stmts (pr_stmt);

      TYPE_CPP_Expr pred_v (tmpB);
      if (IsBoolType(pred_type))
      {
        stmts.ImpAppend(vdm_BC_GenIfStmt(GenGetValue(pred_v, bt), vdm_BC_GenBlock(if_then), nil));
      }
      else
      {
        if (!tmpB.Is(TAG_TYPE_CPP_Identifier))
        {
          pred_v = vdm_BC_GiveName(ASTAUX::MkId(L"pred"));
          stmts.ImpConc (GenDecl_DS (pred_type, pred_v, vdm_BC_GenAsgnInit(tmpB)));
        }
        stmts.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenNot(GenIsBool(pred_v)),
                   vdm_BC_GenBlock(mk_sequence(RunTime(L"A boolean was expected"))), nil));
        stmts.ImpAppend(vdm_BC_GenIfStmt(GenGetValue(vdm_BC_GenCastExpr(GenBoolType(), pred_v), bt),
                                         vdm_BC_GenBlock(if_then), nil));
      }

      Tuple cgpme (CGPatternMatchExcl(pat, elemVT, eset, succ, Map(), stmts, false));
      const SEQ<TYPE_CPP_Stmt> & pm (cgpme.GetSequence(1));
      bool Is_Excl (cgpme.GetBoolValue(2)); // false : need to check pattern match failed

      SEQ<TYPE_CPP_Stmt> pm1;
      if (!Is_Excl)
        pm1.ImpAppend(vdm_BC_GenDecl(GenSmallBoolType(), succ, vdm_BC_GenAsgnInit(vdm_BC_GenBoolLit(false))));
      pm1.ImpConc(pm);
     
      SEQ<TYPE_CPP_Stmt> body_l(MergeStmts( decl, pm1 ));

      TYPE_CPP_Expr cexpr (vdm_BC_GenBracketedExpr(vdm_BC_GenLt(count, vdm_BC_GenIntegerLit(2))));
      TYPE_CGMAIN_VT setVT (mk_CG_VT(tmpSet, mk_REP_SetTypeRep(settp)));

      rb_l.ImpAppend(vdm_BC_GenDecl(GenSmallIntType(), count, vdm_BC_GenAsgnInit(vdm_BC_GenIntegerLit(0))));
      rb_l.ImpConc(GenIterSet(setVT, cexpr, elemVT, body_l));
      rb_l.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenNeq(count, vdm_BC_GenIntegerLit(1)),
                  vdm_BC_GenBlock(mk_sequence(RunTime(L"No unique element in 'iota'"))), nil));

      SEQ<TYPE_CPP_Stmt> sq;
      return sq.ImpAppend(vdm_BC_GenBlock(rb_l));
    }
    default:
      return SEQ<TYPE_CPP_Stmt>(); // dummy
  }
}

// CGIsExpr
// rc1 : AS`IsExpr
// vt : CGMAIN`VT
// ==> seq of CPP`Stmt | CPP`Expr
Generic vdmcg::CGIsExpr (const TYPE_AS_IsExpr & rc1, const TYPE_CGMAIN_VT & vt)
{
  const Record & type      (rc1.GetRecord(pos_AS_IsExpr_type)); // BasicType | Name
  const TYPE_AS_Expr & arg (rc1.GetRecord(pos_AS_IsExpr_arg));
  const TYPE_CPP_Expr & resVar_v (vt.GetRecord(pos_CGMAIN_VT_name));

  TYPE_REP_TypeRep argTmpType (FindType (arg));

  Tuple cgee (CGExprExcl(arg, ASTAUX::MkId(L"argTmp"), nil));
  const TYPE_CPP_Expr & argTmp_v (cgee.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & arg_stmt (cgee.GetSequence(2));

  SEQ<TYPE_CPP_Stmt> rb (arg_stmt);

  if (type.Is(TAG_TYPE_AS_BooleanType) || type.Is(TAG_TYPE_AS_NumericType) ||
      type.Is(TAG_TYPE_AS_TokenType) || type.Is(TAG_TYPE_AS_CharType))
  {
    TYPE_REP_TypeRep typerep (FromAS2RepType(type));
    if (vdm_CPP_isCPP())
    {
      if (rb.IsEmpty())
        return GenIsType(argTmp_v, typerep);
      else
        rb.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, GenIsType(argTmp_v, typerep)));
    }
    else
    { // java
      TYPE_CPP_Expr argTmpCast (IsSubType(argTmpType, typerep) ||
                                GenType(argTmpType).get_tp() == vdm_BC_GenIdentifier(ASTAUX::MkId(L"Object"))
                                   ? argTmp_v
                                   : vdm_BC_GenCastExpr(
                                       vdm_BC_GenTypeSpecifier(
                                         vdm_BC_GenIdentifier(ASTAUX::MkId(L"Object"))), argTmp_v));
      if (rb.IsEmpty())
        return GenIsType(argTmpCast, typerep);
      else
        rb.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, GenIsType(argTmpCast, typerep)));
    }
    return rb;
  }
  else
  { // type : AS`Name
    if (vdm_CPP_isCPP())
    {
      TYPE_CPP_Expr expr_c (GenAuxType(argTmp_v, LOT(TYPE_REP_TypeNameRep().Init(type))));
      rb.ImpAppend (vdm_BC_GenAsgnStmt (resVar_v, vdm_BC_GenCastExpr(GenBoolType(), expr_c)));
    }
    else // Java
    {
      TYPE_CPP_Expr argTmpCast (IsCompositeType(argTmpType)
                                   ? argTmp_v
                                   : vdm_BC_GenCastExpr(
                                       vdm_BC_GenTypeSpecifier(
                                         vdm_BC_GenIdentifier(ASTAUX::MkId(L"Object"))), argTmp_v));

      if (IsCompositeType(argTmpType))
      {
        rb.ImpAppend (vdm_BC_GenAsgnStmt (resVar_v, GenBoolExpr(GenEqTag((const TYPE_AS_Name &)type, argTmpCast))));
      }
      else
      {
        rb.ImpAppend (
          vdm_BC_GenIfStmt (
            GenIsRecord (argTmpCast),
            vdm_BC_GenAsgnStmt (resVar_v, GenBoolExpr(GenEqTag((const TYPE_AS_Name &)type, argTmpCast))),
            vdm_BC_GenAsgnStmt (resVar_v, GenBoolExpr(vdm_BC_GenBoolLit(false)))));
      }
    }
    return rb;
  }
}

// CGPrefixExpr
// pe : AS`PrefixExpr
// vt : CGMAIN`VT
// ==> CPP`Expr | seq of CPP`Stmt
Generic vdmcg::CGPrefixExpr (const TYPE_AS_PrefixExpr & pe, const TYPE_CGMAIN_VT & vt)
{
  const Int & opr (pe.GetInt(pos_AS_PrefixExpr_opr));
  const TYPE_AS_Expr & arg (pe.GetRecord(pos_AS_PrefixExpr_arg));

  switch (opr.GetValue()) {
    case NUMMINUS:
    case NUMPLUS:
    case NUMABS:
    case FLOOR: { return CGUnaryNum (opr, arg, vt); break; }

    case NOT: { return CGUnaryNot (arg, vt); break; }

    case SETCARD:
    case SETDISTRUNION:
    case SETDISTRINTERSECT:
    case SETPOWER: { POSITION::SetPosition(pe);
                     return CGUnarySet (opr, arg, vt); break; }
    case SEQLEN:
    case SEQDISTRCONC:
    case SEQELEMS:
    case SEQINDICES:
    case SEQTAIL:
    case SEQHEAD:
    case SEQREVERSE: { return CGUnarySeq (opr, arg, vt); break; }

    case MAPDOM:
    case MAPRNG:
    case MAPINVERSE:
    case MAPDISTRMERGE: { return CGUnaryMap (opr, arg, vt); break; }
    default: { ReportError (L"CGPrefixExpr"); break; }
  }
  return SEQ<TYPE_CPP_Stmt>(); // dummy
}

// CGUnaryNum
// opr : AS`UnaryOp
// arg : AS`Expr
// vt : CGMAIN`VT
// ==> seq of CPP`Stmt | CPP`Expr
Generic vdmcg::CGUnaryNum (int opr, const TYPE_AS_Expr & arg, const TYPE_CGMAIN_VT & vt)
{
  TYPE_REP_TypeRep argTmpType (FindType (arg));

  Tuple cgee (CGExprExcl(arg, ASTAUX::MkId(L"unArg"), nil));
  const TYPE_CPP_Expr & argTmp (cgee.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & arg_stmt (cgee.GetSequence(2));

  TYPE_CPP_Expr argTmp_q (argTmp);
  if (NUMMINUS == opr) {
    if (arg.Is(TAG_TYPE_AS_BracketedExpr) && arg_stmt.IsEmpty())
      argTmp_q = vdm_BC_GenBracketedExpr(argTmp);
    else
      argTmp_q = argTmp;
  }

  TYPE_CGMAIN_VT rc1 (mk_CG_VT (argTmp_q, argTmpType));

  SEQ<TYPE_CPP_Stmt> rb_l (arg_stmt);
  //if (!IsNumType(argTmpType)) {
  if (argTmpType.Is(TAG_TYPE_REP_UnionTypeRep) && !IsNumType(argTmpType)) {
    rb_l.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenNot(GenIsReal(argTmp)),
                           vdm_BC_GenBlock(mk_sequence(RunTime (L"A number was expected"))), nil));
  }

  Generic alt2;
  switch (opr) {
    case NUMPLUS:  alt2 = UnaryPlus  (vt, rc1); break;
    case NUMMINUS: alt2 = UnaryMinus (vt, rc1); break;
    case NUMABS:   alt2 = UnaryAbs   (vt, rc1); break;
    case FLOOR:    alt2 = UnaryFloor (vt, rc1); break;
    default:       ReportError (L"CGUnaryNum");  break;
  }

  if (IsSeqOfCPPStmt(alt2)) {
    rb_l.ImpConc(alt2);
    return rb_l;
  } 
  else {
    if (rb_l.IsEmpty())
      return alt2;
    else {
      const TYPE_CPP_Expr & resVar_v (vt.GetRecord(pos_CGMAIN_VT_name));
      rb_l.ImpAppend(vdm_BC_GenAsgnStmt( resVar_v, alt2 ));
      return rb_l;
    }
  }
}

// CGUnaryNot
// arg : AS`Expr
// vt : CGMAIN`VT
// ==> CPP`Expr | seq of CPP`Stmt
Generic vdmcg::CGUnaryNot (const TYPE_AS_Expr & arg, const TYPE_CGMAIN_VT & vt)
{
  TYPE_REP_TypeRep argTmpType (FindType (arg));

  Tuple cgee (CGExprExcl(arg, ASTAUX::MkId(L"unArg"), nil));
  const TYPE_CPP_Expr & argTmp (cgee.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & arg_stmt (cgee.GetSequence(2));

  SEQ<TYPE_CPP_Stmt> rb_l (arg_stmt);
  if (!IsBoolType( argTmpType ) ) {
    TYPE_CPP_Stmt rti (vdm_BC_GenBlock(mk_sequence(RunTime (L"A boolean was expected"))));
    rb_l.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenNot(GenIsBool(argTmp)), rti, nil));
  }

  TYPE_REP_BooleanTypeRep bt;
  TYPE_CPP_Expr cast (IsBoolType( argTmpType ) ? argTmp : GenCastType(bt, argTmp));
  TYPE_CPP_Expr expr;
  if (vdm_CPP_isCPP()) {
    if (IsBoolType( argTmpType ) )
      expr = vdm_BC_GenNot (vdm_BC_GenBracketedExpr(cast));
    else
      expr = vdm_BC_GenNot(cast);
  }
  else {
    TYPE_CPP_Expr bv (vdm_BC_GenNot(GenGetValue(cast, bt)));
    expr = GenBoolExpr(bv);
  }
  if (rb_l.IsEmpty())
    return expr;
  else {
    const TYPE_CPP_Expr & resVar_v (vt.GetRecord(pos_CGMAIN_VT_name));
    rb_l.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, expr));
    return rb_l;
  }
}

// CGUnarySet
// opr : AS`UnaryOp
// arg : AS`Expr
// vt : CGMAIN`VT
// ==> seq of CPP`Stmt | CPP`Expr
Generic vdmcg::CGUnarySet (int opr, const TYPE_AS_Expr & arg, const TYPE_CGMAIN_VT & vt)
{
  TYPE_REP_TypeRep argTmpType (FindType (arg));

  Tuple cgee (CGExprExcl(arg, ASTAUX::MkId(L"unArg"), nil));
  const TYPE_CPP_Expr & argTmp (cgee.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & arg_stmt (cgee.GetSequence(2));

  SEQ<TYPE_CPP_Stmt> rb_l (arg_stmt);
  if (!IsSetType(argTmpType)) {
    rb_l.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenNot (GenIsSet (argTmp)),
                                    vdm_BC_GenBlock(mk_sequence(RunTime (L"A set was expected"))),
                                    nil));
  }

  Generic alt2;
  switch (opr) {
    case SETCARD:           { alt2 = CGSetCard (argTmp, argTmpType, vt); break; }
    case SETDISTRUNION:     { alt2 = CGSetDistrUnion (argTmp, argTmpType, vt); break; }
    case SETDISTRINTERSECT: { alt2 = CGSetDistrInterSect (argTmp, argTmpType, vt); break; }
    case SETPOWER:          { alt2 = CGUnarySetPower (argTmp, argTmpType, vt); break; }
    default:                { ReportError (L"CGUnarySet"); break; }
  }

  if (IsSeqOfCPPStmt(alt2)) {
    rb_l.ImpConc(alt2);
    return rb_l;
  } 
  else {
    if (rb_l.IsEmpty())
      return alt2;
    else {
      const TYPE_CPP_Expr & resVar_v (vt.GetRecord(pos_CGMAIN_VT_name));
      rb_l.ImpAppend(vdm_BC_GenAsgnStmt( resVar_v, alt2 ));
      return rb_l;
    }
  }
}

// CGSetCard
// argTmp : CPP`Expr
// argtype : REP`TypeRep
// - : CGMAIN`VT
// ==> CPP`Expr | seq of CPP`Stmt
TYPE_CPP_Expr vdmcg::CGSetCard (const TYPE_CPP_Expr & argTmp,
                                const TYPE_REP_TypeRep & argTmpType,
                                const TYPE_CGMAIN_VT &)
{
  TYPE_CPP_Expr argexpr (IsSetType (argTmpType) ? argTmp : GenCastSetType (argTmp));
  return GenCard (argexpr);
}

// CGSetDistrUnion
// argexpr : CPP`Expr
// argtype : REP`TypeRep
// vt : CGMAIN`VT
// ==> CPP`Expr | seq of CPP`Stmt
Generic vdmcg::CGSetDistrUnion (const TYPE_CPP_Expr & argexpr,
                                const TYPE_REP_TypeRep & argtype,
                                const TYPE_CGMAIN_VT & vt)
{
  if (vdm_CPP_isJAVA())
  {
    const TYPE_CPP_Expr & resVar_v (vt.GetRecord(pos_CGMAIN_VT_name));

    TYPE_CPP_Identifier rduset_v (vdm_BC_GiveName(ASTAUX::MkId(L"rduset")));
    TYPE_REP_TypeRep rdusetType (FindPosResSetTypeInSetDistrUnion (argtype));

    TYPE_CPP_Expr castargexpr (IsSetType(argtype) ? argexpr : GenCastSetType(argexpr));

    TYPE_CPP_Identifier e_v (vdm_BC_GiveName(ASTAUX::MkId(L"e")));

    TYPE_CPP_Stmt todo (IsSetType(rdusetType) ? GenSetUnion(rduset_v, e_v)
                                              : GenSetUnion(rduset_v, GenCastSetType(e_v)));

    SEQ<TYPE_CPP_Stmt> rb;
    // kun Java???
    rb.ImpConc(GenDeclEmptySet(rduset_v));

    TYPE_CPP_Expr argS (argexpr);
    if (! IsSetType (argtype) || ! argexpr.Is(TAG_TYPE_CPP_Identifier))
    {
      argS = vdm_BC_GiveName(ASTAUX::MkId(L"argS"));
      rb.ImpConc (GenDeclSet (argS, castargexpr));
    }

    rb.ImpConc (GenIterSet (mk_CG_VT (argS, mk_REP_SetTypeRep(rdusetType)), nil,
                            mk_CG_VT (e_v, rdusetType), mk_sequence(todo)));
    rb.ImpAppend (vdm_BC_GenAsgnStmt (resVar_v, rduset_v));

    return rb;
  }
  else
  { // C++
    TYPE_CPP_Expr help (IsSetType(argtype) ? argexpr : GenCastSetType(argexpr));
    return GenDUnion(help);
  }
}

// CGSetDistrInterSect
// argexpr : CPP`Expr
// argtype : REP`TypeRep
// vt : CGMAIN`VT
// ==> CPP`Expr | seq of CPP`Stmt
Generic vdmcg::CGSetDistrInterSect(const TYPE_CPP_Expr & argexpr,
                                   const TYPE_REP_TypeRep & argtype,
                                   const TYPE_CGMAIN_VT & vt)
{
  if (vdm_CPP_isJAVA())
  {
    const TYPE_CPP_Expr & resVar_v (vt.GetRecord(pos_CGMAIN_VT_name));

    TYPE_CPP_Identifier rdiset_v (vdm_BC_GiveName(ASTAUX::MkId(L"rdiset")));
    TYPE_REP_TypeRep rdisetType (FindPosResSetTypeInSetDistrUnion (argtype));

    TYPE_CPP_Expr castargexpr (IsSetType(argtype) ? argexpr : GenCastSetType(argexpr));

    TYPE_CPP_Identifier e_v (vdm_BC_GiveName(ASTAUX::MkId(L"e")));

    TYPE_CPP_Stmt todo;
    if (IsSetType(rdisetType))
      todo = GenInterSectStmt(rdiset_v, e_v);
    else
      todo = GenInterSectStmt(rdiset_v, GenCastSetType(e_v));

    SEQ<TYPE_CPP_Stmt> rb;
    TYPE_CPP_Expr argS (argexpr);
    if (! IsSetType (argtype) || ! argexpr.Is(TAG_TYPE_CPP_Identifier) )
    {
      argS = vdm_BC_GiveName(ASTAUX::MkId(L"argS"));
      rb.ImpConc (GenDeclSet (argS, castargexpr));
    }
    TYPE_CPP_Expr rdisetInit (GenSetGetElemExpr (argS));

    //rb.ImpConc(GenDeclSet(rdiset_v, GenCastType(rdisetType, rdisetInit)));
    rb.ImpConc(GenDeclSet(rdiset_v, GenCastSetType(rdisetInit)));
    rb.ImpConc (GenIterSet (mk_CG_VT (argS, mk_REP_SetTypeRep (rdisetType)), nil,
                            mk_CG_VT (e_v, rdisetType), mk_sequence(todo)));
    rb.ImpAppend (vdm_BC_GenAsgnStmt (resVar_v, rdiset_v));
    return rb;
  }
  else
  { // C++
    TYPE_CPP_Expr help (IsSetType(argtype) ? argexpr : GenCastSetType(argexpr));
    return GenDInter(help);
  }
}

// CGUnarySetPower
// argexpr : CPP`Expr
// - : REP`TypeRep
// vt : CGMAIN`VT
// ==> CPP`Expr | seq of CPP`Stmt
Generic vdmcg::CGUnarySetPower (const TYPE_CPP_Expr & argTmp, const TYPE_REP_TypeRep & argTp,
                                const TYPE_CGMAIN_VT & vt)
{
  TYPE_CPP_Expr argexpr (IsSetType (argTp) ? argTmp : GenCastSetType (argTmp));

  if (vdm_CPP_isCPP())
  {
    const TYPE_CPP_Expr & resVar_v (vt.GetRecord(pos_CGMAIN_VT_name));

    TYPE_CPP_Identifier argexprTmp(argexpr);
    if (!argexpr.Is(TAG_TYPE_CPP_Identifier))
      argexprTmp = vdm_BC_GiveName(ASTAUX::MkId(L"argS"));
    TYPE_CPP_Stmt max (vdm_BC_GenIfStmt(vdm_BC_GenGt(GenCard_int(argexprTmp), vdm_BC_GenIntegerLit(25)),
                  vdm_BC_GenBlock(mk_sequence(RunTime(L"Set too big for 'power' - limit is 25"))), nil ));
    SEQ<TYPE_CPP_Stmt> rb;
    if (!argexpr.Is(TAG_TYPE_CPP_Identifier))
      rb.ImpConc (GenDeclSet (argexprTmp, argexpr));
    rb.ImpAppend(max);
    rb.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, GenPower(argexprTmp)));
    return rb;
  }
  else
  { // java
      const TYPE_CPP_Expr & resVar_v (vt.GetRecord(pos_CGMAIN_VT_name));

      TYPE_CPP_Identifier rpowset_v(vdm_BC_GiveName(ASTAUX::MkId(L"rpowset")));
      TYPE_CPP_Identifier elem_v (vdm_BC_GiveName(ASTAUX::MkId(L"elem")));
      TYPE_CPP_Identifier elemSet_v (vdm_BC_GiveName(ASTAUX::MkId(L"elemSet")));
      TYPE_CPP_Identifier tmpSet_v (vdm_BC_GiveName(ASTAUX::MkId(L"tmpSet")));
 
      TYPE_REP_TypeRep e_t (FindSetElemType(argTp));
      TYPE_CPP_Stmt max (vdm_BC_GenIfStmt(vdm_BC_GenGt(GenCard_int(argexpr), vdm_BC_GenIntegerLit(25)),
                  vdm_BC_GenBlock(mk_sequence(RunTime(L"Set too big for 'power' - limit is 25"))), nil ));

      SEQ<TYPE_CPP_Stmt> inner_inner_rb;
      inner_inner_rb.ImpConc(GenDeclSet(tmpSet_v, elemSet_v));
      inner_inner_rb.ImpAppend(GenSetInsert(tmpSet_v, elem_v));
      inner_inner_rb.ImpAppend(GenSetInsert(rpowset_v, tmpSet_v));

      SEQ<TYPE_CPP_Stmt> inner_rb;
      inner_rb.ImpConc(GenIterSet(mk_CG_VT(rpowset_v, mk_REP_SetTypeRep(argTp)), nil,
                                  mk_CG_VT(elemSet_v, argTp), inner_inner_rb) );
      
      SEQ<TYPE_CPP_Stmt> rb;
      rb.ImpAppend(max);
      rb.ImpConc(GenDeclSet(rpowset_v, nil));
      rb.ImpAppend(GenSetInsert(rpowset_v, GenEmptySetExpr()));
      rb.ImpConc(GenIterSet(mk_CG_VT(argexpr, argTp), nil, mk_CG_VT(elem_v, e_t), inner_rb));
// 20130315 -->
      //rb.ImpAppend(vdm_BC_GenExpressionStmt(vdm_BC_GenAsgnExpr(resVar_v, rpowset_v)));
      rb.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, rpowset_v));
// <-- 20130315

      return rb;
  }
}

// CGUnarySeq
// opr : AS`UnaryOp
// arg : AS`Expr
// vt : CGMAIN`VT
// ==> seq of CPP`Stmt | CPP`Expr
Generic vdmcg::CGUnarySeq (int opr, const TYPE_AS_Expr & arg, const TYPE_CGMAIN_VT & vt)
{
  TYPE_REP_TypeRep argTmpType (FindType (arg));

  Tuple cgee (CGExprExcl(arg, ASTAUX::MkId(L"unArg"), nil));
  const TYPE_CPP_Expr & argTmp (cgee.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & arg_stmt (cgee.GetSequence(2));

  SEQ<TYPE_CPP_Stmt> rb_l (arg_stmt);
  if (!IsSeqType(argTmpType)) {
    TYPE_CPP_Expr cond;
    if (vdm_CPP_isCPP())
      cond = GenIsSeq (argTmp);
    else
      cond = vdm_BC_GenLogOr(GenIsSeq (argTmp), GenIsString(argTmp));
     
    rb_l.ImpAppend(vdm_BC_GenIfStmt (vdm_BC_GenNot (cond),
                                     vdm_BC_GenBlock(mk_sequence(RunTime (L"A sequence was expected"))), nil));
  }

  Generic alt2;
  switch (opr) {
    case SEQLEN:       { alt2 = CGSeqLen (argTmp, argTmpType, vt); break; }
    case SEQDISTRCONC: { alt2 = CGSeqDistrConc (argTmp, argTmpType, vt); break; }
    case SEQELEMS:     { alt2 = CGSeqElems (argTmp, argTmpType, vt); break; }
    case SEQINDICES:   { alt2 = CGSeqIndices (argTmp, argTmpType, vt); break; }
    case SEQTAIL:      { alt2 = CGSeqTail (argTmp, argTmpType, vt); break; }
    case SEQHEAD:      { alt2 = CGSeqHead (argTmp, argTmpType, vt); break; }
    case SEQREVERSE:   { alt2 = CGSeqReverse (argTmp, argTmpType, vt); break; }
    default:           { ReportError(L"CGUnarySeq"); break; }
  }

  if (IsSeqOfCPPStmt(alt2)) {
    rb_l.ImpConc(alt2);
    return rb_l;
  }
  else {
    if (rb_l.IsEmpty())
      return alt2;
    else {
      const TYPE_CPP_Expr & resVar_v (vt.GetRecord(pos_CGMAIN_VT_name));
      rb_l.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, alt2));
      return rb_l;
    }
  }
}

// CGSeqDistrConc
// argexpr : CPP`Expr
// argtype : REP`TypeRep
// vt : CGMAIN`VT
// ==> seq of CPP`Stmt | CPP`Expr
Generic vdmcg::CGSeqDistrConc (const TYPE_CPP_Expr & argexpr,
                               const TYPE_REP_TypeRep & argtype,
                               const TYPE_CGMAIN_VT & vt)
{
  const TYPE_CPP_Expr & resVar_v (vt.GetRecord(pos_CGMAIN_VT_name));

  TYPE_CPP_Identifier rdcseq_v (vdm_BC_GiveName(ASTAUX::MkId(L"rdcseq")));
  TYPE_REP_TypeRep rdcseqType (FindPosResSeqTypeInSeqDistr (argtype));
  TYPE_CPP_Identifier e_v (vdm_BC_GiveName(ASTAUX::MkId(L"e")));

  SEQ<TYPE_CPP_Stmt> todo;
  if (vdm_CPP_isCPP())
    todo.ImpAppend(GenSeqConcStmt(rdcseq_v, e_v));
  else // Java
    if (IsSeqType(rdcseqType))
    {
      if (IsPossibleStringType(rdcseqType))
        todo.ImpAppend(vdm_BC_GenAsgnStmt(rdcseq_v, GenConcString(rdcseq_v, e_v)));
      else
        todo.ImpAppend(GenSeqConcStmt(rdcseq_v, e_v));
    }
    else
      todo.ImpAppend(GenSeqConcStmt(rdcseq_v, GenCastSeq(e_v,nil)));

  SEQ<TYPE_CPP_Stmt> rb;
  if (vdm_CPP_isCPP())
    rb.ImpConc (GenDeclEmptySeq (rdcseq_v));
  else
  { // java
    if (IsPossibleStringType(rdcseqType))
      rb.ImpAppend(vdm_BC_GenDecl(GenStringType(), rdcseq_v, vdm_BC_GenAsgnInit(GenEmptyStringExpr())));
    else
      rb.ImpConc (GenDeclEmptySeq (rdcseq_v));
  }

  TYPE_CPP_Expr argS (argexpr);
  if (!IsSeqType (argtype) || !argexpr.Is(TAG_TYPE_CPP_Identifier))
  {
    argS = vdm_BC_GiveName(ASTAUX::MkId(L"argS"));
    rb.ImpConc (GenDeclSeq (argS, GenCastSeq(argexpr, nil)));
  }

  rb.ImpConc (GenIterSeq (mk_CG_VT (argS, mk_REP_SeqTypeRep (rdcseqType)), nil,
                          mk_CG_VT (e_v, rdcseqType), todo));
  rb.ImpAppend (vdm_BC_GenAsgnStmt (resVar_v, rdcseq_v));
  return rb;
}

// CGSeqElems
// argTmp : CPP`Expr
// argTmpType : REP`TypeRep
// vt : CGMAIN`VT
// ==> seq of CPP`Stmt | CPP`Expr
Generic vdmcg::CGSeqElems (const TYPE_CPP_Expr & argTmp,
                           const TYPE_REP_TypeRep & argTmpType,
                           const TYPE_CGMAIN_VT & vt)
{
  if (vdm_CPP_isCPP())
  {
    return GenSeqElems(IsSeqType(argTmpType) ? argTmp : GenCastSeq (argTmp, nil));
  }
  else // for Java
  {
    const TYPE_CPP_Expr & resVar_v (vt.GetRecord(pos_CGMAIN_VT_name));
    
    TYPE_CPP_Expr argexpr;
    if (IsStringType(argTmpType) || IsPossibleStringType(argTmpType))
      argexpr = GenExplicitCast(mk_REP_SeqTypeRep(mk_REP_NumericTypeRep(Int(NAT))),
                                argTmp,
                                mk_REP_SeqTypeRep(mk_REP_CharTypeRep()));
    else if (IsSeqType(argTmpType))
      argexpr = argTmp;
    else
      argexpr =vdm_BC_GenCondExpr(GenIsString(argTmp), GetListVal(argTmp), GenCastSeq(argTmp, nil));

    return GenSetExpr(argexpr);
  }
}

// CGSeqIndices
// argTmp : CPP`Expr
// argTmpType : REP`TypeRep
// vt : CGMAIN`VT
// ==> seq of CPP`Stmt | CPP`Expr
Generic vdmcg::CGSeqIndices (const TYPE_CPP_Expr & argTmp,
                             const TYPE_REP_TypeRep & argTmpType,
                             const TYPE_CGMAIN_VT & vt)
{
  if (vdm_CPP_isCPP())
  {
    return GenInds(GenCastSeq(argTmp, argTmpType));
  }
  else
  { // java
    TYPE_CPP_Expr argexpr (GenCastSeq (argTmp, argTmpType));

    TYPE_CPP_Identifier riseq_v (vdm_BC_GiveName(ASTAUX::MkId(L"riseq")));
    TYPE_CPP_Identifier max_v (vdm_BC_GiveName(ASTAUX::MkId(L"max")));
    TYPE_CPP_Identifier i_v (vdm_BC_GiveName(ASTAUX::MkId(L"i")));

    TYPE_CPP_Expr length ((IsStringType(argTmpType) || IsPossibleStringType(argTmpType))
                       ? GenLenString (argexpr) : GenLen_int (argexpr));

    TYPE_CPP_Stmt insert (vdm_BC_GenBlock(mk_sequence(GenSetInsert (riseq_v, GenIntExpr(i_v)))));

    SEQ<TYPE_CPP_Stmt> rb;
    rb.ImpConc (GenDeclEmptySet (riseq_v));
    rb.ImpAppend (vdm_BC_GenForStmt (vdm_BC_GenDecl (GenSmallIntType (),
                                                     i_v,
                                                     vdm_BC_GenAsgnInit (vdm_BC_GenIntegerLit (1))),
                                     vdm_BC_GenLeq (i_v, length),
                                     mk_sequence(vdm_BC_GenPostPlusPlus(i_v)),
                                     insert));
    rb.ImpAppend (vdm_BC_GenAsgnStmt (vt.GetRecord (pos_CGMAIN_VT_name), riseq_v));
    return rb;
  }
}

// CGSeqLen
// argTmp : CPP`Expr
// argTmpType : REP`TypeRep
// vt : CGMAIN`VT
// ==> seq of CPP`Stmt | CPP`Expr
Generic vdmcg::CGSeqLen (const TYPE_CPP_Expr & argTmp,
                         const TYPE_REP_TypeRep & argTmpType,
                         const TYPE_CGMAIN_VT & vt)
{
  TYPE_CPP_Expr argexpr (IsSeqType(argTmpType) ? argTmp : GenCastSeq (argTmp, argTmpType));

  if (vdm_CPP_isCPP())
    return GenLen(argexpr);
  else {// java
    return (IsStringType(argTmpType) ? GenLenString (argexpr)
                                     : IsSeqType(argTmpType) ? GenLen (argexpr)
               : vdm_BC_GenCondExpr(GenIsString(argTmp), GenLenString (GenCastString(argTmp)),
                                                           GenLen (GenCastSeq (argTmp, nil))));
  }
}

// CGSeqTail
// argTmp : CPP`Expr
// argTmpType : REP`TypeRep
// vt : CGMAIN`VT
// ==> seq of CPP`Stmt | CPP`Expr
Generic vdmcg::CGSeqTail (const TYPE_CPP_Expr & argTmp,
                          const TYPE_REP_TypeRep & argTmpType,
                          const TYPE_CGMAIN_VT & vt)
{
  TYPE_CPP_Expr argexpr (IsSeqType(argTmpType) ? argTmp : GenCastSeq (argTmp, argTmpType));

  if (vdm_CPP_isCPP())
    return GenTl(argexpr);
  else // java
    return (IsStringType(argTmpType) || IsPossibleStringType(argTmpType)
              ? GenTlString (argexpr) : GenTl (argexpr));
}

// CGSeqHead
// argTmp : CPP`Expr
// argTmpType : REP`TypeRep
// vt : CGMAIN`VT
// ==> seq of CPP`Stmt | CPP`Expr
Generic vdmcg::CGSeqHead (const TYPE_CPP_Expr & argTmp,
                          const TYPE_REP_TypeRep & argTmpType,
                          const TYPE_CGMAIN_VT & vt)
{
  TYPE_CPP_Expr argexpr (IsSeqType(argTmpType) ? argTmp : GenCastSeq (argTmp, argTmpType));

  if (vdm_CPP_isCPP())
    return GenHd(vt.GetRecord(pos_CGMAIN_VT_type), argexpr);
  else // java
    return (IsStringType(argTmpType) || IsPossibleStringType(argTmpType)
              ? GenHdString(argexpr) : GenHd(vt.GetRecord(pos_CGMAIN_VT_type), argexpr));
}

// CGSeqReverse
// argTmp : CPP`Expr
// argTmpType : REP`TypeRep
// vt : CGMAIN`VT
// ==> seq of CPP`Stmt | CPP`Expr
Generic vdmcg::CGSeqReverse (const TYPE_CPP_Expr & argTmp,
                             const TYPE_REP_TypeRep & argTmpType,
                             const TYPE_CGMAIN_VT & vt)
{
  TYPE_CPP_Expr argexpr (IsSeqType(argTmpType) ? argTmp : GenCastSeq (argTmp, argTmpType));

  if (vdm_CPP_isCPP())
    return GenReverse(argexpr);
  else // java
    return (IsStringType(argTmpType) || IsPossibleStringType(argTmpType)
              ? GenReverseString (argexpr) : GenReverse(argexpr));
}

// CGUnaryMap
// opr : AS`UnaryOp
// arg : AS`Expr
// vt : CGMAIN`VT
// ==> seq of CPP`Stmt | CPP`Expr
Generic vdmcg::CGUnaryMap (int opr, const TYPE_AS_Expr & arg, const TYPE_CGMAIN_VT & vt)
{
  TYPE_REP_TypeRep argTmpType (FindType (arg));

  Tuple cgee (CGExprExcl(arg, ASTAUX::MkId(L"unArg"), nil));
  const TYPE_CPP_Expr & argTmp (cgee.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & arg_stmt (cgee.GetSequence(2));

  SEQ<TYPE_CPP_Stmt> rb_l (arg_stmt);

  switch (opr) {
    case MAPDOM:
    case MAPRNG:
    case MAPINVERSE: {
      if (!IsMapType(argTmpType)) {
        TYPE_CPP_Stmt rtm (vdm_BC_GenBlock(mk_sequence(RunTime(L"A map was expected"))));
        rb_l.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenNot(GenIsMap(argTmp)), rtm, nil));
      }
      break;
    }
    case MAPDISTRMERGE: {
      if (!IsSetType(argTmpType)) {
        TYPE_CPP_Stmt rts (vdm_BC_GenBlock(mk_sequence(RunTime(L"A set was expected"))));
        rb_l.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenNot(GenIsSet(argTmp)), rts, nil));
      }
      break;
    }
  }

  TYPE_CPP_Expr argexprmap (IsMapType(argTmpType) ? argTmp : GenCastMap(argTmp));

  Generic alt2; // CPP`Expr | seq of CPP`Stmt
  switch ( opr ) {
    case MAPDOM:        { alt2 = CGMapDom(argexprmap, argTmpType, vt); break; }
    case MAPRNG:        { alt2 = CGMapRng(argexprmap, argTmpType, vt); break; }
    case MAPDISTRMERGE: { alt2 = CGMapDistrMerge(argTmp, argTmpType, vt); break; }
    case MAPINVERSE:    { alt2 = CGMapInverse(argexprmap, argTmpType, vt); break; }
    default: { ReportUndefined(L"CGUnaryMap"); break; }
  }

  if (IsSeqOfCPPStmt(alt2)) {
    rb_l.ImpConc(alt2);
    return rb_l;
  }
  else {
    if (rb_l.IsEmpty())
      return alt2;
    else {
      const TYPE_CPP_Expr & resVar_v (vt.GetRecord(pos_CGMAIN_VT_name));
      rb_l.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, alt2));
      return rb_l;
    }
  }
}

// CGMapDom
// argexpr : CPP`Expr
// - : REP`TypeRep
// res : CGMAIN`VT
// ==> CPP`Expr | seq of CPP`Stmt
Generic vdmcg::CGMapDom(const TYPE_CPP_Expr & argexpr, const TYPE_REP_TypeRep &,
                        const TYPE_CGMAIN_VT & res)
{
  if (vdm_CPP_isJAVA())
    return GenSetExpr(GenDom(argexpr));
  else // C++
    return GenDom(argexpr);
}

// CGMapRng
// argexpr : CPP`Expr
// - : REP`TypeRep
// res : CGMAIN`VT
// ==> CPP`Expr | seq of CPP`Stmt
Generic vdmcg::CGMapRng(const TYPE_CPP_Expr & argexpr, const TYPE_REP_TypeRep &,
                        const TYPE_CGMAIN_VT & res)
{
  if (vdm_CPP_isJAVA())
    return GenSetExpr(GenRng(argexpr));
  else // C++
    return GenRng(argexpr);
}

// CGMapDistrMerge
// argtmp : CPP`Expr
// argType : REP`TypeRep
// rc1 : CGMAIN`VT
// ==> CPP`Expr | seq of CPP`Stmt
Generic vdmcg::CGMapDistrMerge(const TYPE_CPP_Expr & argtmp,
                               const TYPE_REP_TypeRep & argType,
                               const TYPE_CGMAIN_VT & rc1)
{
  const TYPE_CPP_Expr & resVar_v (rc1.GetRecord(pos_CGMAIN_VT_name));

  TYPE_CPP_Identifier rdmmap_v ( vdm_BC_GiveName(ASTAUX::MkId(L"rdmmap")) );
  TYPE_CPP_Identifier e_v ( vdm_BC_GiveName(ASTAUX::MkId(L"e")) );
  TYPE_CPP_Identifier tmp_v (vdm_BC_GiveName(ASTAUX::MkId(L"tmp")));
  TYPE_REP_TypeRep maptype (FindMapTypeInMapDistrMerge(argType));
  TYPE_REP_SetTypeRep stp (FindSetTypeInMapDistrMerge(argType));
  TYPE_REP_TypeRep elemtype (CleanFlatType(stp.get_elemtp()));

  Tuple tmp (GenDynCheckOnSetsValues(tmp_v, e_v, argType, maptype));
  SEQ<TYPE_CPP_Stmt> dyncheck (tmp.GetSequence(1));
  const TYPE_CPP_Expr e (tmp.GetRecord(2));

  SEQ<TYPE_CPP_Stmt> todo_inner (GenMapDistrMergeInner(e, rdmmap_v, maptype));

  SEQ<TYPE_CPP_Stmt> rb;
  rb.ImpConc( GenDeclEmptyMap(maptype, rdmmap_v) );

  TYPE_CPP_Expr argS (argtmp);
  if (! IsSetType (argType) || ! argtmp.Is(TAG_TYPE_CPP_Identifier) )
  {
    argS = vdm_BC_GiveName(ASTAUX::MkId(L"argS"));
    rb.ImpConc (GenDeclSet (argS, GenCastSetType(argtmp)));
  }
  dyncheck.ImpConc(todo_inner);

  rb.ImpConc( GenIterSet(mk_CG_VT(argS, mk_REP_SetTypeRep(elemtype)), nil, mk_CG_VT(e_v, elemtype), dyncheck) );
  rb.ImpAppend( vdm_BC_GenAsgnStmt(resVar_v, rdmmap_v) );
  return rb;
}

// GenMapDistrMergeInner
// e : CPP`Expr
// rdmmap_v : CPP`Expr
// maptype : REP`TypeRep
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenMapDistrMergeInner(const TYPE_CPP_Expr & e,
                                                const TYPE_CPP_Expr & rdmmap_v,
                                                const TYPE_REP_TypeRep & maptype)
{
  TYPE_CPP_Stmt rt (RunTime(L"Duplicate entries for \'merge\' had different values"));

  if (vdm_CPP_isCPP())
  {
    TYPE_CPP_Expr eq (vdm_BC_GenFctCallObjMemAcc(rdmmap_v, ASTAUX::MkId(L"IsCompatible"),
                                                 mk_sequence(e)));
    SEQ<TYPE_CPP_Stmt> rb;
    rb.ImpAppend( vdm_BC_GenIfStmt(vdm_BC_GenNot(eq), rt, nil) );
    rb.ImpAppend( GenMapOverride(rdmmap_v, e) );
    return rb;
  }
  else
  { // java
    TYPE_CPP_Identifier eqDom_v( vdm_BC_GiveName(ASTAUX::MkId(L"eq_dom")) );
    TYPE_CPP_Identifier allDupEq_v( vdm_BC_GiveName(ASTAUX::MkId(L"all_dup_eq")) );
    TYPE_CPP_Identifier d_v( vdm_BC_GiveName(ASTAUX::MkId(L"d")) );

    TYPE_REP_TypeRep mdType;
    switch(maptype.GetTag()) {
      case TAG_TYPE_REP_GeneralMapTypeRep: {
        mdType = CleanFlatType(maptype.GetRecord(pos_REP_GeneralMapTypeRep_mapdom));
        break;
      }
      case TAG_TYPE_REP_InjectiveMapTypeRep: {
        mdType = CleanFlatType(maptype.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom));
        break;
      }
    }

    TYPE_CPP_Expr eqtest (GenEqVal(GenMapApply(mk_CG_VT(rdmmap_v, maptype), d_v),
                                   GenMapApply(mk_CG_VT(e, maptype), d_v)));
    SEQ<TYPE_CPP_Stmt> inner;
    inner.ImpAppend( vdm_BC_GenAsgnStmt(allDupEq_v, eqtest) );

    SEQ<TYPE_CPP_Stmt> rb;
    rb.ImpConc( GenDeclSet(eqDom_v, GenDom(rdmmap_v)) );
    rb.ImpAppend( GenInterSectStmt(eqDom_v, GenDom(e)) );
    rb.ImpAppend( vdm_BC_GenDecl(GenSmallBoolType(), allDupEq_v, vdm_BC_GenAsgnInit(vdm_BC_GenBoolLit(true))) );
    rb.ImpConc( GenIterSet(mk_CG_VT(eqDom_v, mk_REP_SetTypeRep(mdType)), allDupEq_v, mk_CG_VT(d_v, mdType), inner) );
    rb.ImpAppend( vdm_BC_GenIfStmt(vdm_BC_GenNot(allDupEq_v), rt, nil) );
    rb.ImpAppend( GenMapOverride(rdmmap_v, e) );
    return rb;
  }
}

// GenDynCheckOnSetsValues
// tmp_v : CPP`Expr
// e_v : CPP`Expr
// type : REP`TypeRep
// maptype : REP`TypeRep
// ==> seq of CPP`Stmt * CPP`Expr
Tuple vdmcg::GenDynCheckOnSetsValues(const TYPE_CPP_Expr & tmp_v,
                                     const TYPE_CPP_Expr & e_v,
                                     const TYPE_REP_TypeRep & type,
                                     const TYPE_REP_TypeRep & maptype)
{
  if ( DoSetConsistsOnlyOfMaps(type) )
    return mk_(SEQ<TYPE_CPP_Stmt>(), e_v);
  else {
    TYPE_CPP_Expr cond (vdm_BC_GenNot(GenIsMap(e_v)));
    TYPE_CPP_Stmt rt (RunTime(L"A map was expected"));
    TYPE_CPP_Stmt ifs (vdm_BC_GenIfStmt(cond, rt, nil));
    SEQ<TYPE_CPP_Stmt> decl (vdm_CPP_isCPP() ? GenDecl_DS(maptype, tmp_v, vdm_BC_GenAsgnInit(e_v)) 
                                             : GenDecl_DS(maptype, tmp_v, vdm_BC_GenAsgnInit(GenCastMap(e_v))));

    SEQ<TYPE_CPP_Stmt> s;
    s.ImpAppend(ifs);
    s.ImpConc(decl);
    return mk_(s, tmp_v);
  }
}

// DoSetConsistsOnlyOfMaps
// type : REP`TypeRep
// ==> bool
bool vdmcg::DoSetConsistsOnlyOfMaps(const TYPE_REP_TypeRep & type)
{
  switch (type.GetTag() ) {
    case TAG_TYPE_REP_SetTypeRep:
      return IsMapType(CleanFlatType(type.GetRecord(pos_REP_SetTypeRep_elemtp)));
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tps (type.GetSet(pos_REP_UnionTypeRep_tps));
      Generic g;
      for (bool bb = tps.First(g); bb; bb = tps.Next(g))
      {
        TYPE_REP_TypeRep tp (LOT (g));
        if ( tp.Is(TAG_TYPE_REP_SetTypeRep) )
          return DoSetConsistsOnlyOfMaps(tp);
      }
      ReportError (L"DoSetConsistsOnlyOfMaps");
    }
    default:
      ReportError(L"DoSetConsistsOnlyOfMaps");
  }
  return false; // To avoid warnings;
}

// FindSetTypeInMapDistrMerge
// argtype : REP`TypeRep
// ==> REP`TypeRep
TYPE_REP_TypeRep vdmcg::FindSetTypeInMapDistrMerge(const TYPE_REP_TypeRep & argtype)
{
  switch (argtype.GetTag() ) {
    case TAG_TYPE_REP_SetTypeRep:
      return argtype;
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tps (argtype.GetSet(pos_REP_UnionTypeRep_tps));
      Generic g;
      for (bool bb = tps.First(g); bb; bb = tps.Next(g)) {
        TYPE_REP_TypeRep tp (LOT (g));
        if ( tp.Is(TAG_TYPE_REP_SetTypeRep) )
          return tp;
      }
      ReportError(L"FindSetTypeInMapDistrMerge");
    }
    default:
      ReportError(L"FindSetTypeInMapDistrMerge");
  }

  return TYPE_REP_TypeRep(); // To avoid warnings from g++
}

// FindMapTypeInMapDistrMerge
// argtype : REP`TypeRep
// ==> REP`TypeRep
TYPE_REP_TypeRep vdmcg::FindMapTypeInMapDistrMerge(const TYPE_REP_TypeRep & argtype)
{
  switch (argtype.GetTag()) {
    case TAG_TYPE_REP_SetTypeRep: {
      TYPE_REP_TypeRep tp (CleanFlatType(argtype).GetRecord(pos_REP_SetTypeRep_elemtp));
      if ( IsMapType(tp) )
        return tp;
      else if (tp.Is(TAG_TYPE_REP_UnionTypeRep)) {
        SET<TYPE_REP_TypeRep> tps (tp.GetSet(pos_REP_UnionTypeRep_tps));
        Generic g;
        for (bool bb = tps.First(g); bb; bb = tps.Next(g))
          if ( IsMapType(g) )
            return g;
        ReportError(L"FindMapTypeInMapDistrMerge");
      }
      else
        ReportError(L"FindMapTypeInMapDistrMerge");
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tps (argtype.GetSet(pos_REP_UnionTypeRep_tps));
      Generic g;
      for (bool bb = tps.First(g); bb; bb = tps.Next(g)) {
        TYPE_REP_TypeRep tp (LOT (g));
        if ( tp.Is(TAG_TYPE_REP_SetTypeRep) )
          return FindMapTypeInMapDistrMerge(tp);
      }
      ReportError(L"FindMapTypeInMapDistrMerge");
    }
    default:
      ReportError(L"FindMapTypeInMapDistrMerge");
  }
  return TYPE_REP_TypeRep(); // To avoid warnings from g++
}

// CGMapInverse
// argexpr : CPP`Expr
// argType : REP`TypeRep
// vt : CGMAIN`VT
// ==> CPP`Expr | seq of CPP`Stmt
Generic vdmcg::CGMapInverse(const TYPE_CPP_Expr & argexpr, const TYPE_REP_TypeRep & argType, const TYPE_CGMAIN_VT & vt)
{
  const TYPE_CPP_Expr & resVar_v (vt.GetRecord(pos_CGMAIN_VT_name));

  TYPE_CPP_Identifier id_r (vdm_BC_GiveName(ASTAUX::MkId(L"id_r")));
  TYPE_CPP_Identifier id_d (vdm_BC_GiveName(ASTAUX::MkId(L"id_d")));
  TYPE_CGMAIN_VT maptmp_cg_vt (mk_CG_VT(argexpr, argType));

  TYPE_CPP_Expr cond_expr (GenDomExists(resVar_v, id_r));
  TYPE_CPP_Stmt alt1 (RunTime(L"Map is not injective - 'inverse' fails"));
  TYPE_CPP_Stmt alt2 (GenMapInsert(resVar_v, id_r, id_d));
  TYPE_CPP_Stmt body (vdm_BC_GenIfStmt(cond_expr, vdm_BC_GenBlock(mk_sequence(alt1)),
                                                  vdm_BC_GenBlock(mk_sequence(alt2))));

  SEQ<TYPE_CPP_Stmt> itermap (GenIterMap(maptmp_cg_vt, nil, id_d, id_r, body));

  SEQ<TYPE_CPP_Stmt> rb_l;

  if (vdm_CPP_isJAVA())
    rb_l.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, GenEmptyMapExpr()));

  rb_l.ImpConc(itermap);

  return rb_l;
}

// CGName
// asname : AS`Name
// ==> seq of CPP`Stmt | CPP`Expr
Generic vdmcg::CGName(const TYPE_AS_Name & asname)
{
  Generic ti (FindType(asname));
  if (ti.Is(TAG_TYPE_REP_PartialFnTypeRep) || ti.Is(TAG_TYPE_REP_TotalFnTypeRep))
  {
    SetNotSupportedException(true);
    return SEQ<TYPE_CPP_Stmt>().ImpAppend(NotSupported(L"function value", asname));
  }
// 2011510 -->
  else if (ti.Is(TAG_TYPE_REP_OpTypeRep))
  {
    SetNotSupportedException(true);
    return SEQ<TYPE_CPP_Stmt>().ImpAppend(NotSupported(L"operation value", asname));
  }
// <-- 20110510

  TYPE_AS_Name asname_q (asname);
  if (asname.get_ids().Length() > 1)
  {
    TYPE_AS_Name argname (asname);
    argname.set_ids(TYPE_AS_Ids().ImpAppend(argname.get_ids().Hd()));
    
// 20120905 -->  
#ifdef VDMPP
    Tuple t (ExpandClassName(argname, GiveCurCASName(), Set()));
    if (t.GetBoolValue(1))
    {
      argname = t.GetRecord(2);
      asname_q = ASTAUX::Combine2Names(argname, asname);
    }
// <-- 20120905
    IncludeClass(argname);
#endif // VDMPP
  }

  InsertVariable(asname_q);
// 20121219 -->
#ifdef VDMSL
  return vdm_BC_Rename(asname_q);
#endif // VDMSL
#ifdef VDMPP
  TYPE_CPP_Expr e (vdm_BC_Rename(asname_q));
  if (vdm_CPP_isJAVA())
  {
    if (e.Is(TAG_TYPE_CPP_QualifiedName) &&
        e.GetField(pos_CPP_QualifiedName_qcn).Is(TAG_TYPE_CPP_Identifier) &&
        e.GetField(pos_CPP_QualifiedName_id).Is(TAG_TYPE_CPP_Identifier))
    {
      SEQ<Char> cnm (e.GetRecord(pos_CPP_QualifiedName_qcn).GetSequence(pos_CPP_Identifier_id));
      SEQ<Char> mnm (e.GetRecord(pos_CPP_QualifiedName_id).GetSequence(pos_CPP_Identifier_id));
      return CheckJavaPrimitiveType(e, cnm, mnm);
    }
  }
  return e;
#endif // VDMPP
// <-- 20121219
}

// CGOldName
// oldnm : AS`OldName
// ==> seq of CPP`Stmt | CPP`Expr
Generic vdmcg::CGOldName(const TYPE_AS_OldName & oldnm)
{
  Generic ti (FindType(oldnm));
  if (ti.Is(TAG_TYPE_REP_PartialFnTypeRep) || ti.Is(TAG_TYPE_REP_TotalFnTypeRep))
  {
    SetNotSupportedException(true);
    return SEQ<TYPE_CPP_Stmt>().ImpAppend(NotSupported(L"function value", oldnm));
  }
// 20110510 -->
  else if (ti.Is(TAG_TYPE_REP_OpTypeRep))
  {
    SetNotSupportedException(true);
    return SEQ<TYPE_CPP_Stmt>().ImpAppend(NotSupported(L"operation value", oldnm));
  }
// <-- 20110510

  return vdm_BC_Rename(oldnm);
}

// CGLiteral
// lit : AS`Literal
// resVar : CGMAIN`VT
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::CGLiteral(const TYPE_AS_Literal & lit, const TYPE_CGMAIN_VT & resVar)
{
  return GenValue(lit, resVar);
}

// CGUndefinedExpr
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGUndefinedExpr()
{
  return SEQ<TYPE_CPP_Stmt>().ImpAppend(RunTime(L"Can not evaluate 'undefined' expression"));
}

// CGBinaryExpr
// be : AS`BinaryExpr
// vt : CGMAIN`VT
// ==> CPP`Expr | seq of CPP`Stmt
Generic vdmcg::CGBinaryExpr(const TYPE_AS_BinaryExpr & be, const TYPE_CGMAIN_VT & vt)
{
  const TYPE_AS_Expr & le (be.GetRecord(pos_AS_BinaryExpr_left));
  const Int & opr         (be.GetInt   (pos_AS_BinaryExpr_opr));
  const TYPE_AS_Expr & re (be.GetRecord(pos_AS_BinaryExpr_right));

  if (vdm_CPP_isJAVA() &&
      (opr == Int(INSET)) &&
      re.Is(TAG_TYPE_AS_PrefixExpr) &&
      (re.GetInt(pos_AS_PrefixExpr_opr) == Int(MAPDOM) ||
       re.GetInt(pos_AS_PrefixExpr_opr) == Int(MAPRNG)))
    return OptimizeMapExpr(le, re, vt);

  int op = opr.GetValue();
  switch ( op ) {
    case AND:
    case OR:
    case IMPLY:
    case EQUIV:
    case EQ:
    case NE:           { return CGLogBinaryExpr(le, op, re, vt); break; }

    case NUMMINUS:
    case NUMPLUS:
    case NUMMULT:
    case NUMDIV:
    case NUMREM:
    case INTDIV:
    case NUMLT:
    case NUMLE:
    case NUMGT:
    case NUMGE:
    case NUMMOD:       { return CGNumBinaryExpr(le, op, re, vt); break; }

    case INSET:
    case NOTINSET:
    case SETUNION:
    case SETINTERSECT:
    case SETMINUS:
    case PROPERSUBSET:
    case SUBSET:       { return CGSetBinaryExpr(le, op, re, vt); break; }

    case SEQCONC:      { return CGSeqConc(le, re, vt); break; }

    case MAPMERGE:
    case MAPDOMRESTTO:
    case MAPDOMRESTBY:
    case MAPRNGRESTTO:
    case MAPRNGRESTBY: { return CGMapBinaryExpr(le, op, re, vt); break; } 

    case COMPOSE:      { return CGComposeExpr(le, re, vt); break; } 

    case NUMEXP:       { return CGIterateExpr(le, re, vt); break; }
    default:           { ReportError(L"CGBinaryExpr"); break; }
  }
  return SEQ<TYPE_CPP_Stmt>(); // dummy
}

// CGLogBinaryExpr
// le : AS`Expr
// opr : AS`BinaryOp
// re : AS`Expr
// vt : CGMAIN`VT
// ==> CPP`Expr | seq of CPP`Stmt
Generic vdmcg::CGLogBinaryExpr(const TYPE_AS_Expr & le,
                               int opr,
                               const TYPE_AS_Expr & re,
                               const TYPE_CGMAIN_VT & vt)
{
  const TYPE_CPP_Expr & resVar_v (vt.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & rType  (vt.GetRecord(pos_CGMAIN_VT_type));

  TYPE_REP_TypeRep type1 (FindType(le));
  TYPE_REP_TypeRep type2 (FindType(re));

  switch(opr) {
    case EQUIV:
    case EQ:
    case NE: {
      Tuple cgeel (CGExprExcl(le, ASTAUX::MkId(L"var1"), type1));
      const TYPE_CPP_Expr & var1 (cgeel.GetRecord(1));
      const SEQ<TYPE_CPP_Stmt> & stmt1 (cgeel.GetSequence(2));

      Tuple cgeer (CGExprExcl(re, ASTAUX::MkId(L"var2"), type2));
      const TYPE_CPP_Expr & var2 (cgeer.GetRecord(1));
      const SEQ<TYPE_CPP_Stmt> & stmt2 (cgeer.GetSequence(2));

      TYPE_CGMAIN_VT vt1 (mk_CG_VT(var1, type1));
      TYPE_CGMAIN_VT vt2 (mk_CG_VT(var2, type2));

      TYPE_CPP_Expr expr (NE == opr ? GenNeq_DS(vt1, vt2) : GenEq_DS(vt1, vt2));
      if (stmt1.IsEmpty() && stmt2.IsEmpty())
        return expr;
      else {
        SEQ<TYPE_CPP_Stmt> rb_l (stmt1);
        rb_l.ImpConc(stmt2);
        rb_l.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, expr));
        return rb_l;
      }
      break;
    }
    default: {
      Tuple cgeel (CGExprExcl(le, ASTAUX::MkId(L"var1"), type1));
      const TYPE_CPP_Expr & var1 (cgeel.GetRecord(1));
      const SEQ<TYPE_CPP_Stmt> & stmt1 (cgeel.GetSequence(2));
    
      Tuple cgeer (CGExprExcl(re, ASTAUX::MkId(L"var2"), type2));
      const TYPE_CPP_Expr & var2 (cgeer.GetRecord(1));
      const SEQ<TYPE_CPP_Stmt> & stmt2 (cgeer.GetSequence(2));
    
      TYPE_CPP_Stmt errmess (RunTime(L"A boolean was expected"));

      SEQ<TYPE_CPP_Stmt> rb1new (stmt1);
      SEQ<TYPE_CPP_Stmt> rb2new (stmt2);
      TYPE_CPP_Expr expr1 (var1);
      TYPE_CPP_Expr expr2 (var2);
    
      if (!IsBoolType(type1)) {
        if (!var1.Is(TAG_TYPE_CPP_Identifier)) {
          TYPE_CPP_Identifier v1 (vdm_BC_GiveName(ASTAUX::MkId(L"ver1")));
          rb1new.ImpConc(GenDecl_DS(type1, v1, vdm_BC_GenAsgnInit(var1)));
          expr1 = v1; 
        }
        rb1new.ImpAppend( vdm_BC_GenIfStmt(vdm_BC_GenNot(GenIsBool(expr1)), errmess, nil) );
        expr1 = GenCastType(mk_REP_BooleanTypeRep(), expr1);
      }
    
      if (!IsBoolType(type2)) {
        if (!var2.Is(TAG_TYPE_CPP_Identifier)) {
          TYPE_CPP_Identifier v2 (vdm_BC_GiveName(ASTAUX::MkId(L"ver2")));
          rb1new.ImpConc(GenDecl_DS(type1, v2, vdm_BC_GenAsgnInit(var2)));
          expr2 = v2; 
        }
        rb2new.ImpAppend( vdm_BC_GenIfStmt(vdm_BC_GenNot(GenIsBool(expr2)), errmess, nil) );
        expr2 = GenCastType(mk_REP_BooleanTypeRep(), expr2);
      }
    
      if (!rb2new.IsEmpty()) {
        TYPE_CPP_Expr cond (GenGetValue(expr1, mk_REP_BooleanTypeRep()));
    
        TYPE_CPP_Stmt asgnstmt (vdm_BC_GenAsgnStmt(resVar_v, expr2));
        TYPE_CPP_Stmt alt1 (vdm_BC_GenBlock(rb2new.ImpAppend(asgnstmt)));

        switch (opr) {
          case AND: {
            rb1new.ImpAppend(vdm_BC_GenIfStmt(cond, alt1,
                         vdm_BC_GenBlock(mk_sequence(vdm_BC_GenAsgnStmt(resVar_v,
                                     GenBoolLit_DS(Bool (false)))))));
            return rb1new;
          }
          case OR: {
            rb1new.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenNot(cond), alt1,
                         vdm_BC_GenBlock(mk_sequence(vdm_BC_GenAsgnStmt(resVar_v,
                                     GenBoolLit_DS(Bool (true)))))));
            return rb1new;
          }
          case IMPLY: {
            rb1new.ImpAppend(vdm_BC_GenIfStmt(cond, alt1,
                         vdm_BC_GenBlock(mk_sequence(vdm_BC_GenAsgnStmt(resVar_v,
                                     GenBoolLit_DS(Bool (true)))))));
            return rb1new;
          }
          default: {
            rb1new.ImpConc(stmt2);
            return rb1new;
          }
        }
      } else {
        TYPE_CPP_Expr cond (GenGetValue(expr1, mk_REP_BooleanTypeRep()));
        TYPE_CPP_Expr alt1 (GenGetValue(expr2, mk_REP_BooleanTypeRep()));
        switch (opr) {
          case AND: {
            TYPE_CPP_Expr e (GenBoolExpr(vdm_BC_GenCondExpr(cond, alt1, vdm_BC_GenBoolLit(false))));
            if (rb1new.IsEmpty())
              return e;
            else {
              rb1new.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, e));
              return rb1new;
            }
          }
          case OR: {
            TYPE_CPP_Expr e (GenBoolExpr(vdm_BC_GenCondExpr(cond, vdm_BC_GenBoolLit(true), alt1)));
            if (rb1new.IsEmpty())
              return e;
            else {
              rb1new.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, e));
              return rb1new;
            }
          }
          case IMPLY: {
            TYPE_CPP_Expr e (GenBoolExpr(vdm_BC_GenCondExpr(cond, alt1, vdm_BC_GenBoolLit(true))));
            if (rb1new.IsEmpty())
              return e;
            else {
              rb1new.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, e));
              return rb1new;
            }
          }
          default: {
            return rb1new;
          }
        }
      }
    }
  }
}

// CGNumBinaryExpr
// vt1 : AS`Expr
// opr : AS`BinaryOp
// re : AS`Expr
// vtres : CGMAIN`VT
// ==> CPP`Expr | seq of CPP`Stmt
Generic vdmcg::CGNumBinaryExpr(const TYPE_AS_Expr & le, int opr, const TYPE_AS_Expr & re, const TYPE_CGMAIN_VT & vtres)
{
  const TYPE_CPP_Expr & resVar_v (vtres.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & type  (vtres.GetRecord(pos_CGMAIN_VT_type));

  TYPE_REP_TypeRep type1 (FindType(le));
  TYPE_REP_TypeRep type2 (FindType(re));

  Tuple cgeel (CGExprExcl(le, ASTAUX::MkId(L"var1"), type1));
  const TYPE_CPP_Expr & var1_v (cgeel.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & stmt1 (cgeel.GetSequence(2));

  Tuple cgeer (CGExprExcl(re, ASTAUX::MkId(L"var2"), type2));
  const TYPE_CPP_Expr & var2_v (cgeer.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & stmt2 (cgeer.GetSequence(2));

  TYPE_CPP_Expr var2_v_q;
  if (re.Is(TAG_TYPE_AS_BracketedExpr) && stmt2.IsEmpty())
    var2_v_q = vdm_BC_GenBracketedExpr (var2_v);
  else
    var2_v_q = var2_v;

  TYPE_CGMAIN_VT vt1 (mk_CG_VT(var1_v, type1));
  TYPE_CGMAIN_VT vt2 (mk_CG_VT(var2_v_q, type2));

  SEQ<TYPE_CPP_Stmt> rb_l (stmt1);
  rb_l.ImpConc(stmt2);

  switch( opr ) {
    case NUMPLUS:
    case NUMMINUS:
    case NUMMULT:
    case NUMDIV:
    case NUMLT:
    case NUMLE:
    case NUMGT:
    case NUMGE: {
      if (!IsNumType(type1) || !IsNumType(type2))
      {
        TYPE_CPP_Expr v1_v (var1_v);
        TYPE_CPP_Expr v2_v (var2_v);
        if (!var1_v.Is(TAG_TYPE_CPP_Identifier)) {
          TYPE_CPP_Identifier e1_v (vdm_BC_GiveName(ASTAUX::MkId(L"e1"))); 
          v1_v = e1_v;
          rb_l.ImpConc(GenConstDeclInit(type1, e1_v, var1_v));
        }
        TYPE_CPP_Identifier e2_v;
        if (!var2_v.Is(TAG_TYPE_CPP_Identifier)) {
          TYPE_CPP_Identifier e2_v (vdm_BC_GiveName(ASTAUX::MkId(L"e2"))); 
          v2_v = e2_v;
          rb_l.ImpConc(GenConstDeclInit(type2, e2_v, var2_v));
        }
        TYPE_CPP_Expr cond;
        if (!IsNumType(type1) && !IsNumType(type2))
          cond = vdm_BC_GenLogOr(vdm_BC_GenNot(GenIsReal(v1_v)), vdm_BC_GenNot(GenIsReal(v2_v)));
        else if (!IsNumType(type1))
          cond = vdm_BC_GenNot(GenIsReal(v1_v));
        else
          cond = vdm_BC_GenNot(GenIsReal(v2_v));

        rb_l.ImpAppend(vdm_BC_GenIfStmt(cond,
                 vdm_BC_GenBlock(mk_sequence(RunTime (L"A number was expected"))), nil));
      }
      break;
    }
    case NUMREM:
    case NUMMOD:
    case INTDIV: {
      if (!IsIntType(type1) || !IsIntType(type2))
      {
        TYPE_CPP_Expr v1_v (var1_v);
        TYPE_CPP_Expr v2_v (var2_v);
        if (!var1_v.Is(TAG_TYPE_CPP_Identifier)) {
          TYPE_CPP_Identifier e1_v (vdm_BC_GiveName(ASTAUX::MkId(L"e1"))); 
          v1_v = e1_v;
          rb_l.ImpConc(GenConstDeclInit(type1, e1_v, var1_v));
        }
        if (!var2_v.Is(TAG_TYPE_CPP_Identifier)) {
          TYPE_CPP_Identifier e2_v (vdm_BC_GiveName(ASTAUX::MkId(L"e2"))); 
          v2_v = e2_v;
          rb_l.ImpConc(GenConstDeclInit(type2, e2_v, var2_v));
        }
        TYPE_CPP_Expr cond;
        if (!IsIntType(type1) && !IsIntType(type2))
          cond = vdm_BC_GenLogOr(vdm_BC_GenNot(GenIsInt(v1_v)), vdm_BC_GenNot(GenIsInt(v2_v)));
        else if (!IsIntType(type1))
          cond = vdm_BC_GenNot(GenIsInt(v1_v));
        else
          cond = vdm_BC_GenNot(GenIsInt(v2_v));

        rb_l.ImpAppend(vdm_BC_GenIfStmt(cond, RunTime (L"A integer was expected"), nil));
      }
      break;
    }
    default:       { ReportError(L"CGNumBinaryExpr"); }
  }

  TYPE_CPP_Expr expr;
  switch( opr ) {
    case NUMPLUS:  { expr = GenPlus_DS(vt1, vt2, vtres); break; }
    case NUMMINUS: { expr = GenMinus_DS(vt1, vt2, vtres); break; }
    case NUMMULT:  { expr = GenMult_DS(vt1, vt2, vtres); break; }
    case NUMDIV:   { expr = GenNumDiv(vt1, vt2, vtres); break; }
    case NUMREM:   { expr = GenNumRem(vt1, vt2); break; }
    case NUMMOD:   { expr = GenNumMod(vt1, vt2, vtres); break; }
    case INTDIV:   { expr = GenIntDiv(vt1, vt2, vtres); break; }
    case NUMLT:    { expr = GenNumLT(vt1, vt2); break; }
    case NUMLE:    { expr = GenNumLE(vt1, vt2); break; }
    case NUMGT:    { expr = GenNumGT(vt1, vt2); break; }
    case NUMGE:    { expr = GenNumGE(vt1, vt2); break; }
    default:       { ReportError(L"CGNumBinaryExpr"); }
  }

  if (rb_l.IsEmpty())
    return expr;
  else
    return rb_l.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, expr));
}

// CGSetBinaryExpr
// le : AS`Expr
// opr : AS`BinaryOp
// re : AS`Expr
// vt : CGMAIN`VT
// ==> CPP`Expr | seq of CPP`Stmt
Generic vdmcg::CGSetBinaryExpr(const TYPE_AS_Expr & le, int opr, const TYPE_AS_Expr & re,
                               const TYPE_CGMAIN_VT & vt)
{
  switch( opr ) {
    case INSET:        { return CGInSet(le, re, vt); break; }
    case NOTINSET:     { return CGNotInSet(le, re, vt); break; }
    case SETUNION:     { return CGSetUnion(le, re, vt); break; }
    case SETINTERSECT: { return CGSetInterSect(le, re, vt); break; }
    case SETMINUS:     { return CGSetMinus(le, re, vt); break; }
    case SUBSET:       { return CGSubset(le, re, vt); break; }
    case PROPERSUBSET: { return CGProperSubset(le, re, vt); break; }
    default:           { ReportError(L"CGSetBinaryExpr"); break; }
  }
  return SEQ<TYPE_CPP_Stmt>(); // dummy
}

// CGInSet
// le : AS`Expr
// re : AS`Expr
// vt : CGMAIN`VT
// ==> CPP`Expr | seq of CPP`Stmt
Generic vdmcg::CGInSet(const TYPE_AS_Expr & le, const TYPE_AS_Expr & re,
                       const TYPE_CGMAIN_VT & vt)
{
  TYPE_REP_TypeRep type1 (FindType(le));
  TYPE_REP_TypeRep type2 (FindType(re));

  Tuple cgeel (CGExprExcl(le, ASTAUX::MkId(L"var1"), type1));
  const TYPE_CPP_Expr & var1_v (cgeel.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & stmt1 (cgeel.GetSequence(2));

  Tuple cgeer (CGExprExcl(re, ASTAUX::MkId(L"var2"), type2));
  const TYPE_CPP_Expr & var2_v (cgeer.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & stmt2 (cgeer.GetSequence(2));

  TYPE_CGMAIN_VT vt1 (mk_CG_VT(var1_v, type1));

  SEQ<TYPE_CPP_Stmt> rb_l (stmt1);
  rb_l.ImpConc(stmt2);
  
  TYPE_CPP_Expr var2_q (IsSetType(type2) ? var2_v : GenCastSetType(var2_v));

  if (!IsSetType(type2))
    rb_l.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenNot(GenIsSet(var2_v)),
                                    vdm_BC_GenBlock(mk_sequence(RunTime(L"A set was expected"))), nil));

  TYPE_CPP_Expr expr (GenInSet(vt1, mk_CG_VT(var2_q, type2)));
  if (rb_l.IsEmpty())
    return expr;
  else {
    const TYPE_CPP_Expr & resVar_v (vt.GetRecord(pos_CGMAIN_VT_name));
    rb_l.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, expr));
    return rb_l;
  }
}

// CGNotInSet
// le : AS`Expr
// re : AS`Expr
// vt : CGMAIN`VT
// ==> seq of CPP`Stmt
Generic vdmcg::CGNotInSet(const TYPE_AS_Expr & le, const TYPE_AS_Expr & re,
                          const TYPE_CGMAIN_VT & vt)
{
  TYPE_REP_TypeRep type1 (FindType(le));
  TYPE_REP_TypeRep type2 (FindType(re));

  Tuple cgeel (CGExprExcl(le, ASTAUX::MkId(L"var1"), type1));
  const TYPE_CPP_Expr & var1_v (cgeel.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & stmt1 (cgeel.GetSequence(2));

  Tuple cgeer (CGExprExcl(re, ASTAUX::MkId(L"var2"), type2));
  const TYPE_CPP_Expr & var2_v (cgeer.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & stmt2 (cgeer.GetSequence(2));

  TYPE_CGMAIN_VT vt1 (mk_CG_VT(var1_v, type1));

  SEQ<TYPE_CPP_Stmt> rb_l (stmt1);
  rb_l.ImpConc(stmt2);

  TYPE_CPP_Expr var2_q (IsSetType(type2) ? var2_v : GenCastSetType(var2_v));

  TYPE_CPP_Expr expr (GenNotInSet(vt1, mk_CG_VT(var2_q, type2)));
  if (rb_l.IsEmpty())
    return expr;
  else {
    const TYPE_CPP_Expr & resVar_v (vt.GetRecord(pos_CGMAIN_VT_name));
    rb_l.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, expr));
    return rb_l;
  }
}

// CGProperSubset
// le : AS`Expr
// re : AS`Expr
// vt : CGMAIN`VT
// ==> seq of CPP`Stmt
Generic vdmcg::CGProperSubset(const TYPE_AS_Expr & le, const TYPE_AS_Expr & re,
                              const TYPE_CGMAIN_VT & vt)
{
  TYPE_REP_TypeRep type1 (FindType(le));
  TYPE_REP_TypeRep type2 (FindType(re));

  Tuple cgeel (CGExprExcl(le, ASTAUX::MkId(L"var1"), type1));
  const TYPE_CPP_Expr & var1_v (cgeel.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & stmt1 (cgeel.GetSequence(2));

  Tuple cgeer (CGExprExcl(re, ASTAUX::MkId(L"var2"), type2));
  const TYPE_CPP_Expr & var2_v (cgeer.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & stmt2 (cgeer.GetSequence(2));

  SEQ<TYPE_CPP_Stmt> rb_l (stmt1);
  rb_l.ImpConc(stmt2);

  const TYPE_CPP_Expr & resVar_v (vt.GetRecord(pos_CGMAIN_VT_name));

  TYPE_CPP_Expr var1q (IsSetType(type1) ? var1_v : GenCastSetType(var1_v));
  TYPE_CPP_Expr var2q (IsSetType(type2) ? var2_v : GenCastSetType(var2_v));
  TYPE_CPP_Expr subs (GenSubSet(var1q, var2q));
  TYPE_CPP_Expr cond1 (vdm_BC_GenNot(GenIsSet(var1_v)));
  TYPE_CPP_Expr cond2 (vdm_BC_GenNot(GenIsSet(var2_v)));
  TYPE_CPP_Stmt errmess (vdm_BC_GenBlock(mk_sequence(RunTime(L"A set was expected"))));

  if (IsSetType(type1) && !IsSetType(type2))
  {
    rb_l.ImpAppend( vdm_BC_GenIfStmt(cond2, errmess, nil) );
  }
  else if (!IsSetType(type1) && IsSetType(type2))
  {
    rb_l.ImpAppend( vdm_BC_GenIfStmt(cond1, errmess, nil) );
  }
  else if (!IsSetType(type1) && !IsSetType(type2))
  {
    rb_l.ImpAppend( vdm_BC_GenIfStmt(vdm_BC_GenLogOr(cond1, cond2), errmess, nil) );
  }

  if (vdm_CPP_isCPP())
  {
    TYPE_CPP_Expr neq (GenNeqVal(GenCard_int(var1q),GenCard_int(var2q)));
    TYPE_CPP_Expr expr (vdm_BC_GenCastExpr(GenBoolType(), vdm_BC_GenLogAnd(subs, vdm_BC_GenBracketedExpr(neq))));
    if (rb_l.IsEmpty())
      return expr;
    else {
      rb_l.ImpAppend( vdm_BC_GenAsgnStmt(resVar_v, expr) );
      return rb_l;
    }
  }
  else
  { // java
    TYPE_CPP_Expr neq (GenNeqVal(GenCard(var1q), GenCard(var2q)));
    TYPE_CPP_Expr resval (GenBoolExpr(vdm_BC_GenLogAnd(subs, neq)));
    if (rb_l.IsEmpty())
      return resval;
    else {
      rb_l.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, resval));
      return rb_l;
    }
  }
}

// CGSubset
// le : AS`Expr
// re : AS`Expr
// vt : CGMAIN`VT
// ==> seq of CPP`Stmt
Generic vdmcg::CGSubset(const TYPE_AS_Expr & le, const TYPE_AS_Expr & re,
                        const TYPE_CGMAIN_VT & vt)
{
  TYPE_REP_TypeRep type1 (FindType(le));
  TYPE_REP_TypeRep type2 (FindType(re));

  Tuple cgeel (CGExprExcl(le, ASTAUX::MkId(L"var1"), type1));
  const TYPE_CPP_Expr & var1_v (cgeel.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & stmt1 (cgeel.GetSequence(2));

  Tuple cgeer (CGExprExcl(re, ASTAUX::MkId(L"var2"), type2));
  const TYPE_CPP_Expr & var2_v (cgeer.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & stmt2 (cgeer.GetSequence(2));

  SEQ<TYPE_CPP_Stmt> rb_l (stmt1);
  rb_l.ImpConc(stmt2);

  const TYPE_CPP_Expr & resVar_v (vt.GetRecord(pos_CGMAIN_VT_name));

  TYPE_CPP_Expr var1q (IsSetType(type1) ? var1_v : GenCastSetType(var1_v));
  TYPE_CPP_Expr var2q (IsSetType(type2) ? var2_v : GenCastSetType(var2_v));
  TYPE_CPP_Expr cond1 (vdm_BC_GenNot(GenIsSet(var1_v)));
  TYPE_CPP_Expr cond2 (vdm_BC_GenNot(GenIsSet(var2_v)));
  TYPE_CPP_Stmt errmess (vdm_BC_GenBlock(mk_sequence(RunTime(L"A set was expected"))));

  if (IsSetType(type1) && !IsSetType(type2))
  {
    rb_l.ImpAppend( vdm_BC_GenIfStmt(cond2, errmess, nil) );
  }
  else if (!IsSetType(type1) && IsSetType(type2))
  {
    rb_l.ImpAppend( vdm_BC_GenIfStmt(cond1, errmess, nil) );
  }
  else if (!IsSetType(type1) && !IsSetType(type2))
  {
    rb_l.ImpAppend( vdm_BC_GenIfStmt(vdm_BC_GenLogOr(cond1, cond2), errmess, nil) );
  }

  TYPE_CPP_Expr expr (GenSubSet_Bool(var1q, var2q));
  if (rb_l.IsEmpty())
    return expr;
  else {
    rb_l.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, expr));
    return rb_l;
  }
}

// CGSetMinus
// le : AS`Expr
// re : AS`Expr
// vt : CGMAIN`VT
// ==> seq of CPP`Stmt
Generic vdmcg::CGSetMinus(const TYPE_AS_Expr & le, const TYPE_AS_Expr & re,
                          const TYPE_CGMAIN_VT & vt)
{
  TYPE_REP_TypeRep type1 (FindType(le));
  TYPE_REP_TypeRep type2 (FindType(re));

  Tuple cgeel (CGExprExcl(le, ASTAUX::MkId(L"var1"), type1));
  const TYPE_CPP_Expr & var1_v (cgeel.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & stmt1 (cgeel.GetSequence(2));

  Tuple cgeer (CGExprExcl(re, ASTAUX::MkId(L"var2"), type2));
  const TYPE_CPP_Expr & var2_v (cgeer.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & stmt2 (cgeer.GetSequence(2));

  TYPE_CGMAIN_VT vt1 (mk_CG_VT(var1_v, type1));
  TYPE_CGMAIN_VT vt2 (mk_CG_VT(var2_v, type2));

  const TYPE_CPP_Expr &    resVar_v (vt.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & rType    (vt.GetRecord(pos_CGMAIN_VT_type));

  TYPE_CPP_Expr var1q (IsSetType(type1) ? var1_v : GenCastSetType(var1_v));
  TYPE_CPP_Expr var2q (IsSetType(type2) ? var2_v : GenCastSetType(var2_v));

  SEQ<TYPE_CPP_Stmt> rb_l (stmt1);
  rb_l.ImpConc(stmt2);

  if (vdm_CPP_isCPP())
  {
// 20151217 -->
    if ( IsSetType(rType) && rb_l.IsEmpty() ) {
      return vdm_BC_GenFctCallObjMemAcc(var1q, ASTAUX::MkId(L"Diff"), mk_sequence(var2q));
    }
// <-- 20151217
    TYPE_CPP_Expr resVar_vq (IsSetType(rType) ? resVar_v : GenCastSetTypeForModify(resVar_v));

    rb_l.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, var1q));
    rb_l.ImpAppend(GenSetMinusStmt(resVar_vq, var2q));
    return rb_l;
  }
  else
  { // java
    TYPE_CPP_Expr resVar_vq (IsSetType(rType) ? resVar_v : GenCastSetType(resVar_v));

    rb_l.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, GenSetExpr(var1q)));
// <-- 20150908
    return rb_l.ImpAppend(GenSetMinusStmt(resVar_vq, var2q));
  }
}

// CGSetInterSect
// le : AS`Expr
// re : AS`Expr
// vt : CGMAIN`VT
// ==> seq of CPP`Stmt
Generic vdmcg::CGSetInterSect(const TYPE_AS_Expr & le, const TYPE_AS_Expr & re,
                              const TYPE_CGMAIN_VT & vt)
{
  TYPE_REP_TypeRep type1 (FindType(le));
  TYPE_REP_TypeRep type2 (FindType(re));

  Tuple cgeel (CGExprExcl(le, ASTAUX::MkId(L"var1"), type1));
  const TYPE_CPP_Expr & var1_v (cgeel.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & stmt1 (cgeel.GetSequence(2));

  Tuple cgeer (CGExprExcl(re, ASTAUX::MkId(L"var2"), type2));
  const TYPE_CPP_Expr & var2_v (cgeer.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & stmt2 (cgeer.GetSequence(2));

  const TYPE_CPP_Expr & resVar_v (vt.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & rType (vt.GetRecord(pos_CGMAIN_VT_type));

  TYPE_CPP_Expr var1q (IsSetType(type1) ? var1_v : GenCastSetType(var1_v));
  TYPE_CPP_Expr var2q (IsSetType(type2) ? var2_v : GenCastSetType(var2_v));

  SEQ<TYPE_CPP_Stmt> rb_l (stmt1);
  rb_l.ImpConc(stmt2);

  if (vdm_CPP_isCPP())
  {
// 20151217 -->
    if ( IsSetType(rType) && rb_l.IsEmpty() ) {
      return vdm_BC_GenFctCallObjMemAcc(var1q, ASTAUX::MkId(L"Intersect"), mk_sequence(var2q));
    }
// <-- 20151217
    TYPE_CPP_Expr resVar_vq (IsSetType(rType) ? resVar_v : GenCastSetTypeForModify(resVar_v));

    rb_l.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, var1q));
    rb_l.ImpAppend(GenInterSectStmt(resVar_vq, var2q));
    return rb_l;
  }
  else
  { // java
    TYPE_CPP_Expr resVar_vq (IsSetType(rType) ? resVar_v : GenCastSetType(resVar_v));

    rb_l.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, GenSetExpr(var1q)));
    return rb_l.ImpAppend(GenInterSectStmt(resVar_vq, var2q));
  }
}

// CGSetUnion
// le : AS`Expr
// re : AS`Expr
// vt : CGMAIN`VT
// ==> CPP`Expr | seq of CPP`Stmt
Generic vdmcg::CGSetUnion(const TYPE_AS_Expr & le, const TYPE_AS_Expr & re,
                          const TYPE_CGMAIN_VT & vt)
{
  TYPE_REP_TypeRep type1 (FindType(le));
  TYPE_REP_TypeRep type2 (FindType(re));

  Tuple cgeel (CGExprExcl(le, ASTAUX::MkId(L"var1"), type1));
  const TYPE_CPP_Expr & var1_v (cgeel.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & stmt1 (cgeel.GetSequence(2));

  Tuple cgeer (CGExprExcl(re, ASTAUX::MkId(L"var2"), type2));
  const TYPE_CPP_Expr & var2_v (cgeer.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & stmt2 (cgeer.GetSequence(2));

  const TYPE_CPP_Expr & resVar_v (vt.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & rType (vt.GetRecord(pos_CGMAIN_VT_type));

  TYPE_CPP_Expr var1q (IsSetType(type1) ? var1_v : GenCastSetType(var1_v));
  TYPE_CPP_Expr var2q (IsSetType(type2) ? var2_v : GenCastSetType(var2_v));

  SEQ<TYPE_CPP_Stmt> rb_l (stmt1);
  rb_l.ImpConc(stmt2);

  if (vdm_CPP_isCPP())
  {
// 20151217 --> 
    if ( IsSetType(rType) && rb_l.IsEmpty() ) {
      return vdm_BC_GenFctCallObjMemAcc(var1q, ASTAUX::MkId(L"Union"), mk_sequence(var2q));
    }
// <-- 20151217
    TYPE_CPP_Expr resVar_vq (IsSetType(rType) ? resVar_v : GenCastSetTypeForModify(resVar_v));

    rb_l.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, var1q));
    rb_l.ImpAppend(GenSetUnion(resVar_vq, var2q));
    return rb_l;
  }
  else
  { // java
    TYPE_CPP_Expr resVar_vq (IsSetType(rType) ? resVar_v : GenCastSetType(resVar_v));

    rb_l.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, GenSetExpr(var1q)));
    return rb_l.ImpAppend(GenSetUnion(resVar_vq, var2q));
  }
}

// CGSeqConc
// le ; AS`Expr
// re ; AS`Expr
// vt : CGMAIN`VT
// ==> CPP`Expr | seq of CPP`Stmt
Generic vdmcg::CGSeqConc(const TYPE_AS_Expr & le, const TYPE_AS_Expr & re,
                         const TYPE_CGMAIN_VT & vt)
{
  const TYPE_CPP_Expr & resVar_v (vt.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & rType  (vt.GetRecord(pos_CGMAIN_VT_type));

  TYPE_REP_TypeRep type1 (FindType(le));
  TYPE_REP_TypeRep type2 (FindType(re));

  Tuple cgeel (CGExprExcl(le, ASTAUX::MkId(L"var1"), type1));
  const TYPE_CPP_Expr & var1 (cgeel.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & stmt1 (cgeel.GetSequence(2));

  Tuple cgeer (CGExprExcl(re, ASTAUX::MkId(L"var2"), type2));
  const TYPE_CPP_Expr & var2 (cgeer.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & stmt2 (cgeer.GetSequence(2));

  SEQ<TYPE_CPP_Stmt> rb_l (stmt1);
  rb_l.ImpConc(stmt2);

  if (vdm_CPP_isCPP())
  {
    TYPE_CPP_Expr var1_q (IsSeqType(type1) ? var1 : GenCastSeq(var1, Nil()));
    TYPE_CPP_Expr expr (vdm_BC_GenFctCallObjMemAcc(var1_q, ASTAUX::MkId(L"Conc"), mk_sequence(var2)));
    if (rb_l.IsEmpty())
      return expr;
    else {
      rb_l.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, expr));
      return rb_l;
    }
  }
  else
  { // Java
    if (!rType.IsNil() && IsStringType(rType))
    {
      TYPE_REP_TypeRep l_charrep (mk_REP_SeqTypeRep(mk_REP_CharTypeRep()));
      TYPE_CPP_Expr var1q (IsStringType(RemoveNil(type1)) ? var1 : GenExplicitCast(l_charrep, var1, Nil()));
      TYPE_CPP_Expr var2q (IsStringType(RemoveNil(type2)) ? var2 : GenExplicitCast(l_charrep, var2, Nil()));
      TYPE_CPP_Expr cast (GenExplicitCast(rType, GenConcString(var1q, var2q), l_charrep));
      if (rb_l.IsEmpty())
        return cast;
      else {
        rb_l.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, cast));
        return rb_l;
      }
    }
    else
    {
      TYPE_REP_TypeRep l_natrep (mk_REP_SeqTypeRep(mk_REP_NumericTypeRep(Int(NAT))));
      TYPE_REP_TypeRep l_charrep (mk_REP_SeqTypeRep(mk_REP_CharTypeRep()));

      TYPE_CPP_Expr var1q (IsPossibleStringType(RemoveNil(type1))
                                 ? GenExplicitCast(l_natrep, var1, l_charrep)
                                 : GenCastSeq(var1, type1));

      TYPE_CPP_Expr var2q (IsPossibleStringType(RemoveNil(type2))
                                 ? GenExplicitCast(l_natrep, var2, l_charrep)
                                 : GenCastSeq(var2, type2));

      TYPE_CPP_Expr resVar_vq (IsSeqType(rType) ? resVar_v : GenCastSeq(resVar_v, nil));
      TYPE_CPP_Expr l_clone (GenSeqExpr(var1q));
      rb_l.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, l_clone));
      return rb_l.ImpAppend(vdm_BC_GenExpressionStmt(GenSeqConc(resVar_vq, var2q)));
    }
  }
}

// CGMapBinaryExpr
// le : AS`Expr
// opr : AS`BinaryOp
// re : AS`Expr
// vt : CGMAIN`VT
// ==> CPP`Expr | seq of CPP`Stmt
Generic vdmcg::CGMapBinaryExpr(const TYPE_AS_Expr & le, int opr, const TYPE_AS_Expr & re,
                               const TYPE_CGMAIN_VT & resVT)
{
  TYPE_REP_TypeRep type1 (FindType(le));
  TYPE_REP_TypeRep type2 (FindType(re));

  Tuple cgeel (CGExprExcl(le, ASTAUX::MkId(L"var1"), type1));
  const TYPE_CPP_Expr & var1_v (cgeel.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & stmt1 (cgeel.GetSequence(2));

  Tuple cgeer (CGExprExcl(re, ASTAUX::MkId(L"var2"), type2));
  const TYPE_CPP_Expr & var2_v (cgeer.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & stmt2 (cgeer.GetSequence(2));

  TYPE_CGMAIN_VT vt1 (mk_CG_VT(var1_v, type1));
  TYPE_CGMAIN_VT vt2 (mk_CG_VT(var2_v, type2));

  SEQ<TYPE_CPP_Stmt> rb_l (stmt1);
  rb_l.ImpConc(stmt2);

  switch( opr ) {
    case MAPMERGE:     {
      rb_l.ImpConc(GenMapMerge(vt1, vt2, resVT));
      return rb_l;
      break;
    }
    case MAPDOMRESTTO:
    case MAPDOMRESTBY: {
      Generic res_v (GenMapRestToBy(vt1, vt2, resVT, opr));
      if (IsSeqOfCPPStmt(res_v)) {
        rb_l.ImpConc(res_v);
        return rb_l;
      }
      else {
        if (rb_l.IsEmpty())
          return res_v;
        else {
          const TYPE_CPP_Expr & resVar_v (resVT.GetRecord(pos_CGMAIN_VT_name));
          rb_l.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, res_v));
          return rb_l;
        }
      }
      break;
    }
    case MAPRNGRESTTO:
    case MAPRNGRESTBY: {
      Generic res_v (GenMapRestToBy(vt2, vt1, resVT, opr));
      if (IsSeqOfCPPStmt(res_v)) {
        rb_l.ImpConc(res_v);
        return rb_l;
      }
      else {
        if (rb_l.IsEmpty())
          return res_v;
        else {
          const TYPE_CPP_Expr & resVar_v (resVT.GetRecord(pos_CGMAIN_VT_name));
          rb_l.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, res_v));
          return rb_l;
        }
      }
      break;
    }
  }
  return rb_l; // dummy
}

// OptimizeMapExpr
// le : AS`Expr
// re : AS`PrefixExpr
// vt : CGMAIN`VT
// ==> CPP`Expr | seq of CPP`Stmt
Generic vdmcg::OptimizeMapExpr(const TYPE_AS_Expr & le, const TYPE_AS_PrefixExpr & re,
                               const TYPE_CGMAIN_VT & vt)
{
  const TYPE_CPP_Expr & resVar_v (vt.GetRecord(pos_CGMAIN_VT_name));

  Tuple t1 (CGExprExcl(le, ASTAUX::MkId(L"var1"), Nil()));
  const TYPE_CPP_Expr & var1_v (t1.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & stmt1 (t1.GetSequence(2));

  Tuple t2 (CGExprExcl(re.get_arg(), ASTAUX::MkId(L"var2"), Nil()));
  const TYPE_CPP_Expr & var2_v (t2.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & stmt2 (t2.GetSequence(2));

  TYPE_CPP_Expr mapCall ((re.GetIntValue(pos_AS_PrefixExpr_opr) == MAPDOM)
                            ? GenDomExists(var2_v, var1_v)
                            : GenRngExists(var2_v, var1_v));

  TYPE_CPP_Expr objCreate (GenBoolExpr(mapCall));

  if (stmt1.IsEmpty() && stmt2.IsEmpty()) 
    return objCreate;
  else {
    SEQ<TYPE_CPP_Stmt> rb_l (stmt1);
    rb_l.ImpConc(stmt2);
    rb_l.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, objCreate));
    return rb_l;
  }
}

// CGComposeExpr
// le : AS`Expr
// re : AS`Expr
// resVT : CGMAIN`VT
// ==> CPP`Expr | seq of CPP`Stmt
Generic vdmcg::CGComposeExpr(const TYPE_AS_Expr & le, const TYPE_AS_Expr & re,
                             const TYPE_CGMAIN_VT & resVT)
{
  const TYPE_CPP_Expr & resVar_v (resVT.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & type  (resVT.GetRecord(pos_CGMAIN_VT_type));

  TYPE_REP_TypeRep type1 (FindType(le));
  TYPE_REP_TypeRep type2 (FindType(re));

  Tuple cgeel (CGExprExcl(le, ASTAUX::MkId(L"var1"), type1));
  const TYPE_CPP_Expr & var1_v (cgeel.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & stmt1 (cgeel.GetSequence(2));

  Tuple cgeer (CGExprExcl(re, ASTAUX::MkId(L"var2"), type2));
  const TYPE_CPP_Expr & var2_v (cgeer.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & stmt2 (cgeer.GetSequence(2));

  SEQ<TYPE_CPP_Stmt> rb1_l (stmt1);
  SEQ<TYPE_CPP_Stmt> rb2_l (stmt2);

  TYPE_CPP_Expr v1 = var1_v;
  TYPE_CPP_Expr v2 = var2_v;

  if (stmt1.IsEmpty()) {
    if (vdm_CPP_isCPP() || !le.Is(TAG_TYPE_AS_Name)) {
      v1 = vdm_BC_GiveName(ASTAUX::MkId(L"v1"));
      rb1_l.ImpConc(GenDeclInit_DS(type1, v1, var1_v));
    }
  }

  if (stmt2.IsEmpty()) {
    if (vdm_CPP_isCPP() || !re.Is(TAG_TYPE_AS_Name)) {
      v2 = vdm_BC_GiveName(ASTAUX::MkId(L"v2"));
      rb2_l.ImpConc(GenDeclInit_DS(type2, v2, var2_v));
    }
  }
  
  rb1_l.ImpConc(rb2_l);

  if (type1.Is(TAG_TYPE_REP_PartialFnTypeRep) || type1.Is(TAG_TYPE_REP_TotalFnTypeRep) ||
      type2.Is(TAG_TYPE_REP_PartialFnTypeRep) || type2.Is(TAG_TYPE_REP_TotalFnTypeRep))
  {
    // composition of function is not supported
    return rb1_l;
  }
  else
  {
    TYPE_CGMAIN_VT vt1 (mk_CG_VT(var1_v, type1));
    TYPE_CGMAIN_VT vt2 (mk_CG_VT(var2_v, type2));
    rb1_l.ImpConc(GenComposeExpr(vt1, vt2, resVT));
    return rb1_l;
  }
}

// CGIterateExpr
// le : AS`Expr
// re : AS`Expr
// resVT : CGMAIN`VT
// ==> CPP`Expr | seq of CPP`Stmt
Generic vdmcg::CGIterateExpr(const TYPE_AS_Expr & le, const TYPE_AS_Expr & re,
                             const TYPE_CGMAIN_VT & resVT)
{
  const TYPE_CPP_Expr & resVar_v (resVT.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & rtype  (resVT.GetRecord(pos_CGMAIN_VT_type));

  TYPE_REP_TypeRep type1 (FindType(le));
  TYPE_REP_TypeRep type2 (FindType(re));

  Tuple cgeel (CGExprExcl(le, ASTAUX::MkId(L"var1"), type1));
  const TYPE_CPP_Expr & var1 (cgeel.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & stmt1 (cgeel.GetSequence(2));

  Tuple cgeer (CGExprExcl(re, ASTAUX::MkId(L"var2"), type2));
  const TYPE_CPP_Expr & var2 (cgeer.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & stmt2 (cgeer.GetSequence(2));

  TYPE_CGMAIN_VT vt1 (mk_CG_VT(var1, type1));
  TYPE_CGMAIN_VT vt2 (mk_CG_VT(var2, type2));

  SEQ<TYPE_CPP_Stmt> rb_l (stmt1);
  rb_l.ImpConc(stmt2);

  if ( IsNumType(type1) && IsNumType(type2) ) {
    rb_l.ImpAppend(GenExp(vt1, vt2, resVT));
    return rb_l;
  }
  else if (IsMapType(type1) || (vdm_CPP_isJAVA() && IsMapType(rtype)))
  {
    rb_l.ImpConc(GenMapIteration(vt1, vt2, resVT));
    return rb_l;
  }
  else if (IsFctType(type1))
  {
    SetNotSupportedException(true);
    return SEQ<TYPE_CPP_Stmt>().ImpAppend(NotSupported(L"function iteration expression", le));
  }
  else
  {
    TYPE_CPP_Expr cond;
    if (vdm_CPP_isCPP())
      cond = (vdm_BC_GenLogOr(GenIsReal(var1), GenIsReal(var2)));
    else
      cond = GenIsReal(var1);

    TYPE_CPP_Stmt th (GenExp(vt1, vt2, resVT));
    TYPE_CPP_Stmt altn (RunTime(L"Wrong arguments for '**'"));
    TYPE_CPP_Stmt stmt (vdm_BC_GenIfStmt(cond, th, altn));

    if (type1.Is(TAG_TYPE_REP_UnionTypeRep))
    {
      SET<TYPE_REP_TypeRep> tp_s (type1.GetSet(pos_REP_UnionTypeRep_tps));
      bool exists = false;
      Generic tp;
      for (bool bb = tp_s.First(tp); bb && !exists; bb = tp_s.Next(tp))
        exists = IsMapType(tp);

      if (exists)
      {
        th = vdm_BC_GenBlock(GenMapIteration(vt1, vt2, resVT));
        stmt = vdm_BC_GenIfStmt(GenIsMap(var1), th, stmt);
      }
    }
    rb_l.ImpAppend(stmt);
    return rb_l;
  }
}

#ifdef VDMPP
// CGNewExpr
// ns : AS`NewExpr
// vt : CGMAIN`VT
// ==> seq of CPP`Stmt | CPP`Expr
Generic vdmcg::CGNewExpr(const TYPE_AS_NewExpr & ns, const TYPE_CGMAIN_VT & vt)
{
//  const TYPE_AS_Name & nm (ns.GetRecord(pos_AS_NewExpr_cls));
  TYPE_AS_Name nm (ns.GetRecord(pos_AS_NewExpr_cls));
  const SEQ<TYPE_AS_Expr> & exprs (ns.GetSequence(pos_AS_NewExpr_args));

  //  if (get_j2v_option() && (IsAbstractClass(nm) || isInterface(nm)))
  //    return GenNullExpr();

// 20120118 -->
  Tuple t (ExpandClassName(nm, GiveCurCASName(), Set()));
  if (t.GetBoolValue(1))
    nm = t.GetRecord(2);
// <-- 20120118

  const TYPE_CPP_Expr & resVar (vt.GetRecord(pos_CGMAIN_VT_name));

  SEQ<TYPE_CPP_Expr> l_tmpVars;
  Sequence l_tmpVarTps;

  size_t len_exprs = exprs.Length();
  for(size_t idx = 1; idx <= len_exprs; idx++)
  {
    l_tmpVars.ImpAppend(vdm_BC_GiveName(ASTAUX::MkId(L"arg")));
    l_tmpVarTps.ImpAppend(FindType(exprs[idx]));
  }

  Sequence l_cppL; //seq of ((seq of CPP`Stmt) | CPP`Expr)
  for(size_t i = 1; i <= len_exprs; i++)
  {
    l_cppL.ImpAppend(CGExpr_q(exprs[i], mk_CG_VT(l_tmpVars[i], l_tmpVarTps[i])));
  }

  SEQ<TYPE_CPP_Stmt> result;
  Sequence l_prepCode;
  SEQ<TYPE_CPP_Expr> l_argL;
  size_t len_l_cppL = l_cppL.Length();
  for(size_t j = 1; j <= len_l_cppL; j++)
  {
    const Generic & thisVal (l_cppL[j]);
    if (thisVal.IsSequence())
    {
      // seq of CPP`Stmt
      SEQ<TYPE_CPP_Stmt> thisDecl;
      thisDecl.ImpConc(GenDecl_DS(l_tmpVarTps[j], l_tmpVars[j], Nil()));
      thisDecl.ImpConc(thisVal);
      result.ImpConc(thisDecl);
      l_prepCode.ImpAppend(thisDecl);
      l_argL.ImpAppend(l_tmpVars[j]);
    }
    else
    {
      // CPP`Expr
      l_argL.ImpAppend(thisVal);
    }
  }

  TYPE_CPP_Expr constr (CallConstructor(nm, l_argL));
  IncludeClass (nm);
  if (!(get_j2v_option() && (nm == ASTAUX::MkNameFromId(ASTAUX::MkId(L"JavaLangClass"), NilContextId))))
      SetException(true);

  if (l_prepCode.IsEmpty())
    return constr;
  else
  {
    TYPE_CPP_Stmt constAsgn (vdm_BC_GenAsgnStmt(resVar, constr));
    result.ImpAppend(constAsgn);
    return result;
  }
}

// GenSelfExpr
// vt : CGMAIN`VT
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenSelfExpr(const TYPE_CGMAIN_VT & vt)
{
  if (vdm_CPP_isCPP())
  {
    TYPE_CPP_Identifier self (vdm_BC_GenIdentifier(ASTAUX::MkId(L"Self")));
    return vdm_BC_GenFctCall(self, SEQ<TYPE_CPP_Expr>());
  }
  else
  { // java
    return GenThis();
  }
}

// CGIsOfClassExpr
// ioc : AS`IsOfClassExpr
// vt : CGMAIN`VT
// ==> (seq of CPP`Stmt) | CPP`Expr
Generic vdmcg::CGIsOfClassExpr(const TYPE_AS_IsOfClassExpr & ioc, const TYPE_CGMAIN_VT & vt)
{
  const TYPE_AS_Name & cls (ioc.GetRecord(pos_AS_IsOfClassExpr_cls));
  const TYPE_AS_Expr & arg (ioc.GetRecord(pos_AS_IsOfClassExpr_arg));
  const TYPE_CPP_Expr & res (vt.GetRecord(pos_CGMAIN_VT_name));

  TYPE_REP_TypeRep objtp (FindType(arg));

  Tuple cgee (CGExprExcl(arg, ASTAUX::MkId(L"obj"), nil));
  const TYPE_CPP_Expr & obj (cgee.GetRecord(1));
  SEQ<TYPE_CPP_Stmt> rb_l (cgee.GetSequence(2));

  TYPE_CPP_Expr cast (IsObjRefType(objtp) ? obj
          //: vdm_CPP_isCPP() ? vdm_BC_GenCastExpr(GenObjRefType_DS(nil), obj)
          : vdm_CPP_isCPP() ? vdm_BC_GenFctCall(GenObjRefType_DS(nil).get_tp(), mk_sequence(obj))
                            : vdm_BC_GenCastExpr(GenObjRefType_DS(nil), obj));
  TYPE_CPP_Expr expr (GenIsOfClassExpr(cls, cast));
  TYPE_CPP_Expr e (vdm_CPP_isCPP() ? IsObjRefType(objtp)
                                       ? expr
                                       : vdm_BC_GenCondExpr(GenIsClass(obj), expr, GenBoolLit_DS(Bool(false)))
                                   : expr);
  if (rb_l.IsEmpty())
    return e;
  else {
    rb_l.ImpAppend(vdm_BC_GenAsgnStmt(res, e));
    return rb_l;
  }
}

// CGIsOfBaseClassExpr
// ioc : AS`IsOfBaseClassExpr
// vt : CGMAIN`VT
// ==> (seq of CPP`Stmt) | CPP`Expr
Generic vdmcg::CGIsOfBaseClassExpr(const TYPE_AS_IsOfBaseClassExpr & ioc, const TYPE_CGMAIN_VT & vt)
{
  const TYPE_AS_Name & cls (ioc.GetRecord(pos_AS_IsOfBaseClassExpr_cls));
  const TYPE_AS_Expr & arg (ioc.GetRecord(pos_AS_IsOfBaseClassExpr_arg));
  const TYPE_CPP_Expr & res (vt.GetRecord(pos_CGMAIN_VT_name));

  TYPE_REP_TypeRep objtp (FindType(arg));

  Tuple cgee (CGExprExcl(arg, ASTAUX::MkId(L"obj"), nil));
  const TYPE_CPP_Expr & obj (cgee.GetRecord(1));
  SEQ<TYPE_CPP_Stmt> rb_l (cgee.GetSequence(2));

  TYPE_CPP_Expr cast (IsObjRefType(objtp) ? obj
          //: vdm_CPP_isCPP() ? vdm_BC_GenCastExpr(GenObjRefType_DS(nil), obj)
          : vdm_CPP_isCPP() ? vdm_BC_GenFctCall(GenObjRefType_DS(nil).get_tp(), mk_sequence(obj))
                            : vdm_BC_GenCastExpr(GenObjRefType_DS(nil), obj));
  TYPE_CPP_Expr expr (GenIsOfBaseClassExpr(cls, cast));
  TYPE_CPP_Expr e (vdm_CPP_isCPP() ? IsObjRefType(objtp)
                                       ? expr
                                       : vdm_BC_GenCondExpr(GenIsClass(obj), expr, GenBoolLit_DS(Bool(false)))
                                   : expr);
  if (rb_l.IsEmpty())
    return e;
  else {
    rb_l.ImpAppend(vdm_BC_GenAsgnStmt(res, e));
    return rb_l;
  }
}

// CGSameClassExpr
// sc : AS`SameClassExpr
// vt : CGMAIN`VT
// ==> (seq of CPP`Stmt) | CPP`Expr
Generic vdmcg::CGSameClassExpr(const TYPE_AS_SameClassExpr & sc, const TYPE_CGMAIN_VT & vt)
{
  const TYPE_AS_Expr & e1 (sc.GetRecord(pos_AS_SameClassExpr_expr1));
  const TYPE_AS_Expr & e2 (sc.GetRecord(pos_AS_SameClassExpr_expr2));
  const TYPE_CPP_Expr & res (vt.GetRecord(pos_CGMAIN_VT_name));

  Tuple tmp (GenClassOrBase(e1, e2, false));
  const TYPE_CPP_Expr & obj1 (tmp.GetRecord(1));    // CPP`Expr
  const TYPE_CPP_Expr & obj2 (tmp.GetRecord(2));    // CPP`Expr
  SEQ<TYPE_CPP_Stmt> rb_l    (tmp.GetSequence(3));  // seq of CPP`Stmt
  const Bool & areObjRefs    (tmp.GetBool(4));      // bool
  const Generic & cond       (tmp.GetField(5));     // [CPP`Expr]

  TYPE_CPP_Expr my1 (GenMyClass(obj1));
  TYPE_CPP_Expr my2 (GenMyClass(obj2));

  TYPE_CPP_Expr expr;
  if (vdm_CPP_isCPP())
    expr = GenBoolExpr(vdm_BC_GenEq(my1, my2));
  else
  { // java
    if (areObjRefs.GetValue())
    {
      TYPE_CPP_Expr fctcall(vdm_BC_GenFctCallObjMemAcc(my1, ASTAUX::MkId(L"equals"), mk_sequence(my2)));
      expr = GenBoolExpr(fctcall);
    }
    else
    {
      TYPE_CPP_Expr fctcall(vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"UTIL.sameclass")),
                                              mk_sequence(obj1, obj2)));
      expr = GenBoolExpr(fctcall);
    }
  }

  TYPE_CPP_Expr e (cond.IsNil() ? expr : vdm_BC_GenCondExpr(cond, expr, GenBoolLit_DS(Bool(false))));
  if (rb_l.IsEmpty())
    return e;
  else {
    rb_l.ImpAppend(vdm_BC_GenAsgnStmt(res, e));
    return rb_l;
  }
}

// CGSameBaseClassExpr
// sc : AS`SameBaseClassExpr
// vt : CGMAIN`VT
// ==> (seq of CPP`Stmt) | CPP`Expr
Generic vdmcg::CGSameBaseClassExpr(const TYPE_AS_SameBaseClassExpr & sc, const TYPE_CGMAIN_VT & vt)
{
  const TYPE_AS_Expr & e1 (sc.GetRecord(pos_AS_SameBaseClassExpr_expr1));
  const TYPE_AS_Expr & e2 (sc.GetRecord(pos_AS_SameBaseClassExpr_expr2));
  const TYPE_CPP_Expr & res (vt.GetRecord(pos_CGMAIN_VT_name));

  Tuple tmp (GenClassOrBase(e1, e2, true));
  const TYPE_CPP_Expr & obj1 (tmp.GetRecord(1));   // CPP`Expr
  const TYPE_CPP_Expr & obj2 (tmp.GetRecord(2));   // CPP`Expr
  SEQ<TYPE_CPP_Stmt> rb_l    (tmp.GetSequence(3)); // seq of CPP`Stmt
  const Bool & areObjRefs    (tmp.GetBool(4));     // bool
  const Generic & cond       (tmp.GetField(5));    // [CPP`Expr]

  TYPE_CPP_Expr expr (GenSameBaseClassExpr(obj1, obj2, areObjRefs));
  TYPE_CPP_Expr e (cond.IsNil() ? expr : vdm_BC_GenCondExpr(cond, expr, GenBoolLit_DS(Bool(false))));
  if (rb_l.IsEmpty())
    return e;
  else {
    rb_l.ImpAppend(vdm_BC_GenAsgnStmt(res, e));
    return rb_l;
  }

// <-- 20150726
}

// GenClassOrBase
// e1 : AS`Expr
// e2 : AS`Expr
// cast : bool
// ==> CPP`Expr * CPP`Expr * seq of CPP`Stmt * bool * [CPP`Expr]
Tuple vdmcg::GenClassOrBase(const TYPE_AS_Expr & e1, const TYPE_AS_Expr & e2, bool cast)
{
  TYPE_REP_TypeRep obj1tp (FindType(e1));
  TYPE_REP_TypeRep obj2tp (FindType(e2));
  TYPE_CPP_TypeSpecifier oti (GenObjRefType_DS(nil));

  Tuple cgee(CGExprExcl(e1, ASTAUX::MkId(L"obj1"), nil));
  const TYPE_CPP_Expr & obj1_v(cgee.GetRecord(1));           // CPP`Expr
  const SEQ<TYPE_CPP_Stmt> & obj1_stmt(cgee.GetSequence(2)); // seq of CPP`Stmt

  Tuple cgee2 (CGExprExcl(e2, ASTAUX::MkId(L"obj2"), nil));
  const TYPE_CPP_Expr & obj2_v (cgee2.GetRecord(1));           // CPP`Expr
  const SEQ<TYPE_CPP_Stmt> & obj2_stmt (cgee2.GetSequence(2)); // seq of CPP`Stmt

  Generic cond (nil);
  bool is1 = IsObjRefType(obj1tp);
  bool is2 = IsObjRefType(obj2tp);
  TYPE_CPP_Expr obj1 (obj1_v);
  TYPE_CPP_Expr obj2 (obj2_v);

  SEQ<TYPE_CPP_Stmt> rb_l (obj1_stmt);
  rb_l.ImpConc(obj2_stmt);

  if (vdm_CPP_isCPP())
  {
    if (!is1 && !is2) {
// 20120224 -->
//      cond = vdm_BC_GenLogAnd(GenIsClass(obj1), GenIsClass(obj2));
      if (obj1 == obj2)
        cond = GenIsClass(obj1);
      else
        cond = vdm_BC_GenLogAnd(GenIsClass(obj1), GenIsClass(obj2));
// <--20120224
      //obj1 = vdm_BC_GenCastExpr(oti, obj1);
      //obj2 = vdm_BC_GenCastExpr(oti, obj2);
      obj1 = vdm_BC_GenFctCall(oti.get_tp(), mk_sequence(obj1));
      obj2 = vdm_BC_GenFctCall(oti.get_tp(), mk_sequence(obj2));
    }
    else if (!is1) {
      cond = GenIsClass(obj1);
      //obj1 = vdm_BC_GenCastExpr(oti, obj1);
      obj1 = vdm_BC_GenFctCall(oti.get_tp(), mk_sequence(obj1));
    }
    else if (!is2) {
      cond = GenIsClass(obj2);
      //obj2 = vdm_BC_GenCastExpr(oti, obj2);
      obj2 = vdm_BC_GenFctCall(oti.get_tp(), mk_sequence(obj2));
    }
  }
  else // Java
  {
    if (cast)
    {
      if (is1)
        if (!(obj1tp.Is(TAG_TYPE_REP_ObjRefTypeRep)))
          is1 = false;

      if (is2)
        if (!(obj2tp.Is(TAG_TYPE_REP_ObjRefTypeRep)))
          is2 = false;
    }
  }
  return mk_(obj1, obj2, rb_l, Bool(is1 && is2), cond);
}
#endif

// CGComprehension
// bind : AS`BindList
// pred : [ AS`Expr ]
// stmt : seq of CPP`Stmt
// contexpr : [ CPP`Expr ]
// notpred : [CPP`Stmt]
// pid_m : map AS`Name to REP`TypeRep
// nonstop : bool
// ==> CPP`Stmt
TYPE_CPP_Stmt vdmcg::CGComprehension(const SEQ<TYPE_AS_MultBind> & bind,
                                     const Generic & pred,
                                     const SEQ<TYPE_CPP_Stmt> & stmt,
                                     const Generic & contexpr,
                                     const Generic & notpred,
                                     const Map & pid_m,
                                     bool nonstop)
{
  size_t len_bind = bind.Length();
  for (size_t i = 1; i <= len_bind; i++)
  {
    if (bind[i].Is(TAG_TYPE_AS_MultTypeBind))
    {
      SetNotSupportedException(true);
      return NotSupported(L"type bind", bind[i]);
    }
  }

  // merge bind pat
  // a in set s, b in set s -> a, b in set s
  SEQ<TYPE_AS_MultBind> bind_l (MergeMultSetBind(bind));

  Tuple fmsb (FindMultSetBind2(bind_l)); // must be before DeclPatVars
  const SEQ<TYPE_CPP_Stmt> & stmts (fmsb.GetSequence(1)); // decl for set
  const Sequence & msb (fmsb.GetSequence(2)); // seq of (seq of AS`Pattern * CPP`Expr * REP`TypeRep)

  if (msb.Length() == 1) {
    const Tuple & t (msb[1]);
    const SEQ<TYPE_AS_Pattern> & p_l (t.GetSequence(1));
    size_t len_p_l = p_l.Length();
    bool allpnm = true;
    for (size_t idx = 1; (idx <= len_p_l) && allpnm; idx++)
      allpnm = p_l[idx].Is(TAG_TYPE_AS_PatternName);

    if (allpnm) {
      const TYPE_CPP_Expr & e_v (t.GetRecord(2));
      const TYPE_REP_TypeRep & e_type (t.GetRecord(3));
      TYPE_REP_TypeRep e_t (FindSetElemType(e_type));

      const TYPE_AS_Expr & setexpr (bind_l[1].GetRecord(pos_AS_MultSetBind_Set));
      bool seqindices = IsSeqIndicesSet(setexpr);
      Set seqapply_s;
      SEQ<TYPE_CPP_Identifier> id_l;
      for (size_t i = 1; i <= len_p_l; i++ ) {
        const TYPE_AS_Pattern & p (p_l[i]);
        const Generic & name (p.GetField(pos_AS_PatternName_nm));
        if (!name.IsNil()) {
          InsertName_CGAUX(name);
          id_l.ImpAppend(vdm_BC_Rename(name));
          if (seqindices)
            seqapply_s.Insert(mk_(vdm_BC_Rename(setexpr.GetRecord(pos_AS_PrefixExpr_arg)), vdm_BC_Rename(name)));
        }
        else 
          id_l.ImpAppend(vdm_BC_GiveName(ASTAUX::MkId(L"elem")));
      }
      AddNoCheckSeqApply(seqapply_s);
      SEQ<TYPE_CPP_Stmt> stmt_l (GenPredicateStmt(pred, stmt, notpred));
      RemNoCheckSeqApply(seqapply_s);
      for (size_t j = 1; j <= len_p_l; j++) {
        stmt_l = GenIterSet(mk_CG_VT(e_v, e_type), contexpr, mk_CG_VT(id_l[j], e_t), stmt_l);
      }
      SEQ<TYPE_CPP_Stmt> rb_l (stmts);
      rb_l.ImpConc(stmt_l);
      return GenCPPStmt(rb_l);
    }
  }
  // NOTE: DeclPatVars must be after FindMultSetBind2
  // NOTE: DeclPatVars must be before GenPredicateStmt

  SEQ<TYPE_CPP_Stmt> rb_p;
  rb_p.ImpConc(DeclPatVars(pid_m));

  SEQ<TYPE_CPP_Stmt> rb;
  rb.ImpConc(stmts);
  if (!nonstop)
    rb.ImpConc(rb_p);

  // NOTE: GenPredicateStmt must be after DeclPatVars
  // if pred = true then stmt else notpred
  TYPE_CPP_Identifier succ_v (vdm_BC_GiveName(ASTAUX::MkId(L"succ")));
  TYPE_CPP_Stmt asgn_succ_true (vdm_BC_GenAsgnStmt(succ_v, vdm_BC_GenBoolLit(true)));
  SEQ<TYPE_CPP_Stmt> inner (GenPredicateStmt(pred, stmt, notpred));
  Sequence pn_l (FindPatternIdInBindList(bind_l)); // seq of set of AS`Name
  bool need_succ(false);
  size_t len_msb = msb.Length();
  for (size_t idx = 1; idx <= len_msb; idx++)
  {
    const Tuple & tmp (msb[idx]); // seq of AS`Pattern * CPP`Expr * REP`TypeRep
    const SEQ<TYPE_AS_Pattern> & pat_l (tmp.GetSequence(1));
    const TYPE_CPP_Expr & e_v (tmp.GetRecord(2));
    const TYPE_REP_TypeRep & e_type (tmp.GetRecord(3));

    TYPE_CPP_Expr e1_v (e_v);
    size_t len_pat_l = pat_l.Length();
    for (size_t i = 1; i <= len_pat_l; i++)
    {
      SET<TYPE_AS_Name> pn_s (pn_l.Hd());
      pn_l.ImpTl();
      if (i > 1)
      {
        TYPE_CPP_Identifier e2_v (vdm_BC_GiveName(ASTAUX::MkId(L"e_set")));
        rb.ImpConc(GenDeclInit_DS(e_type, e2_v, e_v));
        e1_v = e2_v;
      }
      const TYPE_AS_Pattern & pat (pat_l[i]);
      //TYPE_CGMAIN_VT e_set_cg_vt (mk_CG_VT(e1_v, e_type));
      TYPE_CPP_Identifier e_g_v (vdm_BC_GiveName(ASTAUX::MkId(L"elem")));
      TYPE_REP_TypeRep e_t (FindSetElemType(e_type));

      Generic p = Nil();
      if (!inner.IsEmpty())
        p = inner;

      Tuple cgpme (CGPatternMatchExcl(pat, mk_CG_VT(e_g_v, e_t), pn_s, succ_v, pid_m, p, nonstop));
      const SEQ<TYPE_CPP_Stmt> & pm (cgpme.GetSequence(1));
      const Bool & Is_Excl (cgpme.GetBool(2));

      SEQ<TYPE_CPP_Stmt> pm1;
      if (Is_Excl && p.IsNil())
        pm1.ImpAppend(asgn_succ_true);
      pm1.ImpConc(pm);

      SEQ<TYPE_CPP_Stmt> this_bl;
      if (nonstop && (i == len_pat_l) && (idx == len_msb))
        this_bl.ImpConc(MergeStmts(rb_p, pm1));
      else
        this_bl.ImpConc(pm1);

      inner = GenIterSet(mk_CG_VT(e1_v, e_type), contexpr, mk_CG_VT(e_g_v, e_t), this_bl);
      need_succ = need_succ || !Is_Excl;
    }
  }

  rb.ImpConc(inner);

  if (need_succ) {
    rb.ImpPrepend(vdm_BC_GenDecl(GenSmallBoolType(), succ_v,
                                 vdm_BC_GenAsgnInit( vdm_BC_GenBoolLit(false) )));
  }

//  rb.ImpPrepend(vdm_BC_GenSingleLineComments(SEQ<Char>(L"CGComprehension Start")));
//  rb.ImpAppend(vdm_BC_GenSingleLineComments(SEQ<Char>(L"CGComprehension End")));

  return GenCPPStmt(rb);
}

// GenPredicateStmt
// pred : [AS`Expr]
// stmt : seq of CPP`Stmt
// notpred : [CPP`Stmt]
// ==>  seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenPredicateStmt(const Generic & pred,
                                           const SEQ<TYPE_CPP_Stmt> & stmt,
                                           const Generic & notpred)
{
  if ( pred.IsNil() ) {
    return stmt;
  }
  else
  {
    Tuple cgee (CGExprExcl(pred, ASTAUX::MkId(L"pred"),nil));
    const TYPE_CPP_Expr & res (cgee.GetRecord(1));
    const SEQ<TYPE_CPP_Stmt> & res_stmt (cgee.GetSequence(2));

    TYPE_REP_TypeRep predType (FindType(pred));

    SEQ<TYPE_CPP_Stmt> rb;
    rb.ImpConc(res_stmt);

    if (predType.Is(TAG_TYPE_REP_BooleanTypeRep))
    {
      if (stmt.IsEmpty())
      {
        if (!notpred.IsNil())
        {
// 20160531 -->
          //rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenNot(GenGetValue(res, predType)), notpred, nil));
          TYPE_CPP_Expr e (GenGetValue(res, predType));
          while( e.Is(TAG_TYPE_CPP_BracketedExpr) )
            e = e.GetRecord(pos_CPP_BracketedExpr_expr);
          if ( e.Is(TAG_TYPE_CPP_UnaryOpExpr ) &&
              ( e.GetRecord(pos_CPP_UnaryOpExpr_op).GetField(pos_CPP_UnaryOp_val) == Quote(L"NEG"))) {
              rb.ImpAppend(vdm_BC_GenIfStmt(e.GetRecord(pos_CPP_UnaryOpExpr_expr), notpred, nil));
          }
          else {
            rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenNot(vdm_BC_GenBracketedExpr(e)), notpred, nil));
          }
// <-- 20160531
        }
      }
      else
      {
        rb.ImpAppend(vdm_BC_GenIfStmt(GenGetValue(res, predType), vdm_BC_GenBlock(stmt), notpred));
      }
    }
    else
    {
      TYPE_CPP_Expr pred_v (res);
      if (!res.Is(TAG_TYPE_CPP_Identifier))
      {
        pred_v = vdm_BC_GiveName(ASTAUX::MkId(L"pred"));
        rb.ImpConc (GenDecl_DS (predType, pred_v, vdm_BC_GenAsgnInit(res)));
      }
      TYPE_CPP_Expr cond (vdm_CPP_isCPP() ? GenGetValue(vdm_BC_GenCastExpr(GenBoolType(), pred_v), predType)
                                          : GenGetValue(vdm_BC_GenCastExpr(GenBoolType(), pred_v),
                                                                                  mk_REP_BooleanTypeRep()));
      TYPE_CPP_Stmt ifpred (vdm_BC_GenIfStmt(cond, vdm_BC_GenBlock(stmt), notpred));
      rb.ImpAppend(vdm_BC_GenIfStmt(GenIsBool(pred_v), ifpred, RunTime(L"A boolean was expected")));
    }
    return rb;
  }
}

// DeclarePatVars
// pat : AS`Pattern
// ==> seq of CPP`Stmt
SEQ<TYPE_AS_Stmt> vdmcg::DeclarePatVars(const TYPE_AS_Pattern & pat)
{
  SEQ<TYPE_AS_Pattern> pat_l;
  pat_l.ImpAppend(pat);
  Map pid_m (SamePatternIds(pat_l).GetMap(2)); // bool * map AS`Name to set of REP`TypeRep
  return DeclarePatterns(pid_m);
}

// DeclPatVars
// pid_m : map AS`Name to REP`TypeRep
// ==> seq of CPP`Stmt
SEQ<TYPE_AS_Stmt> vdmcg::DeclPatVars(const MAP<TYPE_AS_Name, TYPE_REP_TypeRep> & pid_m)
{
  SEQ<TYPE_AS_Stmt> rb;
  SET<TYPE_AS_Name> sd (pid_m.Dom());
  Generic var;
  for (bool bb = sd.First(var); bb; bb = sd.Next(var))
  {
    InsertName_CGAUX(var);
    rb.ImpConc(GenDecl_DS(pid_m[var], vdm_BC_Rename(var), nil));
  }
  return rb;
}

// FindMultSetBind2
// bind : AS`BindList
// ==> seq of CPP`Stmt * seq of (seq of AS`Pattern * CPP`Expr * REP`TypeRep)
Tuple vdmcg::FindMultSetBind2(const SEQ<TYPE_AS_MultBind> & bind_l)
{
  SET<TYPE_AS_Name> allpnms;
  size_t len_bind_l = bind_l.Length();
  for (size_t idx = 1; idx <= len_bind_l; idx++)
  {
    const TYPE_AS_MultSetBind & msb (bind_l[idx]);
    const SEQ<TYPE_AS_Pattern> & pat_l (msb.GetSequence(pos_AS_MultSetBind_pat));
    size_t len_pat_l = pat_l.Length();
    for (size_t i = 1; i <= len_pat_l; i++)
    {
      allpnms.ImpUnion(FindPatternId(pat_l[i]).Dom());
    }
  }

  SEQ<TYPE_CPP_Stmt> rb;
  Sequence msb; // seq of (seq of AS`Pattern * CPP`Expr * REP`TypeRep)

  for (size_t ii = 1; ii <= len_bind_l; ii++)
  {
    const TYPE_AS_MultSetBind & e (bind_l[ii]);
    const SEQ<TYPE_AS_Pattern> & pat_l (e.GetSequence(pos_AS_MultSetBind_pat));
    const TYPE_AS_Expr & e_set (e.GetRecord(pos_AS_MultSetBind_Set));

    Map lastEnv (CurrentEnv_CGAUX());
    PopLastEnv_CGAUX();

    Tuple cgee (CGExprExcl(e_set, ASTAUX::MkId(L"e_set"), nil));
    const TYPE_CPP_Expr & e_v (cgee.GetRecord(1));
    const SEQ<TYPE_CPP_Stmt> & e_stmt (cgee.GetSequence(2));

    PushThisEnv_CGAUX(lastEnv);

    TYPE_REP_TypeRep e_type (FindType(e_set));
    SET<TYPE_AS_Name> allnames (FindAllNamesInExpr(e_set));

    if (! IsSetType(e_type))
    {
      TYPE_CPP_Identifier e1_v (vdm_BC_GiveName(ASTAUX::MkId(L"e1_set")));
      TYPE_CPP_Expr cond (GenIsSet(e_v));
      TYPE_CPP_Stmt alt1 (vdm_CPP_isCPP() ? vdm_BC_GenBlock(mk_sequence(vdm_BC_GenAsgnStmt(e1_v, e_v)))
                                          : vdm_BC_GenBlock(mk_sequence(vdm_BC_GenAsgnStmt(e1_v, GenCastSetType(e_v)))));
      TYPE_CPP_Stmt alt2 (vdm_BC_GenBlock(mk_sequence(RunTime(L"A set was expected"))));

      rb.ImpConc(e_stmt);
      rb.ImpConc(GenDeclSet(e1_v, nil));
      rb.ImpAppend(vdm_BC_GenIfStmt(cond, alt1, alt2));

      msb.ImpAppend(mk_(pat_l, e1_v, e_type));
    }
    else if (e_stmt.IsEmpty() &&
             (!(allnames.ImpIntersect(allpnms)).IsEmpty() || !e_set.Is(TAG_TYPE_AS_Name)))
    {
      TYPE_CPP_Identifier e1_v (vdm_BC_GiveName(ASTAUX::MkId(L"e1_set")));
      SEQ<TYPE_CPP_Stmt> s_init (GenConstDeclInit(e_type, e1_v, e_v));

      rb.ImpConc(e_stmt);
      rb.ImpConc(s_init);
      msb.ImpAppend(mk_(pat_l, e1_v, e_type));
    }
    else
    {
      rb.ImpConc(e_stmt);
      msb.ImpAppend(mk_(pat_l, e_v, e_type));
    }
  }
  return mk_(rb, msb);
}

// FindPatternIdInBindList
// bind : AS`BindList
//// ==> seq of set of AS`Name * set of AS`Name
// ==> seq of set of AS`Name
Sequence vdmcg::FindPatternIdInBindList(const SEQ<TYPE_AS_MultBind> & bind)
{
  SEQ<TYPE_AS_Pattern> p_l;

  size_t len_bind = bind.Length();
  for (size_t idx = 1; idx <= len_bind; idx++)
  {
    const TYPE_AS_MultSetBind & msb (bind[idx]);
    p_l.ImpConc(msb.GetSequence(pos_AS_MultSetBind_pat));
  }

  SEQ<Set> pn_l; // seq of set of AS`Name
  size_t len_p_l = p_l.Length();
  for (size_t i = 1; i <= len_p_l; i++)
  {
    SET<TYPE_AS_Name> pn_s;
    for (size_t j = i + 1; j <= len_p_l; j++)
    {
      pn_s.ImpUnion(FindPatternId(p_l[j]).Dom());
    }
    pn_l.ImpAppend(pn_s);
  }
  return pn_l;
}

SEQ<TYPE_AS_MultBind> vdmcg::MergeMultSetBind(const SEQ<TYPE_AS_MultBind> & bind)
{
  // a in set s, b in set s -> a, b in set s
  Map m; // map AS`Expr to AS`MultSetBind
  size_t len_bind = bind.Length();
  for (size_t idx = 1; idx <= len_bind; idx++)
  {
    const TYPE_AS_MultSetBind & e (bind[idx]);
    const SEQ<TYPE_AS_Pattern> & pat_l (e.GetSequence(pos_AS_MultSetBind_pat));
    const TYPE_AS_Expr & e_set (e.GetRecord(pos_AS_MultSetBind_Set));
    if (m.Dom().InSet(e_set))
    {
      TYPE_AS_MultSetBind b (m[e_set]);
      SEQ<TYPE_AS_Pattern> p_l (b.GetSequence(pos_AS_MultSetBind_pat));
      p_l.ImpConc(pat_l);
      b.SetField(pos_AS_MultSetBind_pat, p_l);
      m.ImpModify(e_set, b);
    }
    else
    {
      m.ImpModify(e_set, e);
    }
  }
  SEQ<TYPE_AS_MultBind> bind_l;
  Set dom_m (m.Dom());
  Generic g;
  for (bool bb = dom_m.First(g); bb; bb = dom_m.Next(g))
  {
    bind_l.ImpAppend(m[g]);
  }
  return bind_l;
}
