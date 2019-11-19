/***
*  * WHAT
*  *    Gnerates information for run-time analysis
*  * FILE
*  *    $Source: /mnt/cvsroot/toolbox/code/eval/rt_info.cc,v $
*  * VERSION
*  *    $Revision: 1.59 $
*  * DATE
*  *    $Date: 2006/09/22 11:14:06 $
*  * STATUS
*  *    $State: Exp $
*  * REFERENCES
*  *    
*  * PROJECT
*  *    Afrodite - ESPRIT programme, project no. 6500.
*  * AUTHOR
*  *    Poul Boegh Lassen (PBL) + $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "rt_info.h"
#include "astaux.h"
#include "contextinfo.h"
#include "tools.h"

#include "AS.h"


//RT_REMOVED void Set_RT_INFO (Record rc)
//RT_REMOVED {
//RT_REMOVED   Int cid (GetCid(rc));
//RT_REMOVED   if (cid != NilContextId) {
//RT_REMOVED     GetCI().IncTestCoverageInfo(cid);
//RT_REMOVED   }
//RT_REMOVED }

//////////////////////////
/// Name: Is_Covered
/// Descr: Takes an ast node as input.
///        If test coverage information has
///        has been loaded the test coverage counter
///        for the node is returned.
///        Otherwise, if no test coverage information
///        has been loaded 1 (true) is always returned.
//////////////////////////
int RTINFO::Is_Covered (const Record & rc, ContextInfo & ci)
{
  Int cid (ASTAUX::GetCid(rc));
  if (ci.HasTestCoverage(cid)) {
    int cov  (ci.GetTestCoverageInfo(cid));
    return cov;
  }
  else {
    // No test coverage information has been loaded
    // for the rc node.
    return 1; 
  }
}

int RTINFO::exprs = 0;
int RTINFO::covered = 0;

Tuple RTINFO::Calc_Coverage(const Generic & ast, ContextInfo & ci)
{
  RTINFO::exprs = 0;
  RTINFO::covered = 0;
  Calc_Coverage_Aux(ast,ci);
  return mk_(Int((RTINFO::exprs > 0) ? RTINFO::exprs : 1), Int(RTINFO::covered));
}

void RTINFO::IncComposite(const Generic & expr, ContextInfo & ci)
{
  // Do only increment if test coverage info is in context table.
  if (ci.HasTestCoverage(ASTAUX::GetCid(expr))) {
    if (Is_Covered (expr,ci)) {
      RTINFO::covered++;
    }
    RTINFO::exprs++;
  }
}

void RTINFO::Calc_Coverage_Aux(const Generic & ast, ContextInfo & ci)
{
  if (ast.IsSequence()) {
    Sequence s (ast);
    size_t len_s = s.Length();
    for (size_t index = 1; index <= len_s; index++) {
      Calc_Coverage_Aux(s[index], ci);
    }
    return;
  }
  else if (ast.IsRecord ()) {
    Record expr (ast);
  
    switch (expr.GetTag ()) {
      case TAG_TYPE_AS_CharLit:
      case TAG_TYPE_AS_BoolLit:
      case TAG_TYPE_AS_QuoteLit:
      case TAG_TYPE_AS_TextLit:
      case TAG_TYPE_AS_RealLit:
      case TAG_TYPE_AS_NumLit:
      case TAG_TYPE_AS_NilLit:
      case TAG_TYPE_AS_Name:
      case TAG_TYPE_AS_IdentStmt:
      case TAG_TYPE_AS_ErrorStmt:
#ifdef VDMPP
      case TAG_TYPE_AS_SelfExpr:
#endif //VDMPP
        IncComposite(expr, ci);
        break;
#ifdef VDMSL
      case TAG_TYPE_AS_StateDef: {
        const Generic & Inv_g (expr.GetField(pos_AS_StateDef_Inv));
        if (!Inv_g.IsNil ()) {
          TYPE_AS_Invariant inv (Inv_g);
          Calc_Coverage_Aux(inv.GetRecord(pos_AS_Invariant_pat), ci);
          Calc_Coverage_Aux(inv.GetRecord(pos_AS_Invariant_expr), ci);
        }
        const Generic & Init_g (expr.GetField(pos_AS_StateDef_Init));
        if (!Init_g.IsNil ()) {
          TYPE_AS_StateInit init (Init_g);
          Calc_Coverage_Aux(init.GetRecord(pos_AS_StateInit_pat), ci);
          Calc_Coverage_Aux(init.GetRecord(pos_AS_StateInit_expr), ci);
        }
        break;
      }
#endif // VDMSL
      case TAG_TYPE_AS_TypeDef: {
        const Generic & Inv_g (expr.GetField(pos_AS_TypeDef_Inv));
        if (!Inv_g.IsNil ()) {
          TYPE_AS_Invariant inv (Inv_g);
          Calc_Coverage_Aux(inv.GetRecord(pos_AS_Invariant_pat), ci);
          Calc_Coverage_Aux(inv.GetRecord(pos_AS_Invariant_expr), ci);
        }
        break;
      }
      case TAG_TYPE_AS_ExplFnDef: {
        Calc_Coverage_Aux(expr.GetSequence(pos_AS_ExplFnDef_parms), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_ExplFnDef_body), ci);
        Calc_Coverage_Aux(expr.GetField(pos_AS_ExplFnDef_fnpre), ci);
        Calc_Coverage_Aux(expr.GetField(pos_AS_ExplFnDef_fnpost), ci);
        break;
      }
      case TAG_TYPE_AS_ExtExplFnDef: {
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_ExtExplFnDef_body), ci);
        Calc_Coverage_Aux(expr.GetField(pos_AS_ExtExplFnDef_fnpre), ci);
        Calc_Coverage_Aux(expr.GetField(pos_AS_ExtExplFnDef_fnpost), ci);
        break;
      }
      case TAG_TYPE_AS_ImplFnDef: {
        Calc_Coverage_Aux(expr.GetField(pos_AS_ImplFnDef_fnpre), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_ImplFnDef_fnpost), ci);
        break;
      }
      case TAG_TYPE_AS_ExplOpDef: {
        Calc_Coverage_Aux(expr.GetSequence(pos_AS_ExplOpDef_parms), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_ExplOpDef_body), ci);
        Calc_Coverage_Aux(expr.GetField(pos_AS_ExplOpDef_oppre), ci);
        Calc_Coverage_Aux(expr.GetField(pos_AS_ExplOpDef_oppost), ci);
        break;
      }
      case TAG_TYPE_AS_ExtExplOpDef: {
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_ExtExplOpDef_body), ci);
        Calc_Coverage_Aux(expr.GetField(pos_AS_ExtExplOpDef_oppre), ci);
        Calc_Coverage_Aux(expr.GetField(pos_AS_ExtExplOpDef_oppost), ci);
        Calc_Coverage_Aux(expr.GetSequence(pos_AS_ExtExplOpDef_excps), ci);
        break;
      }
      case TAG_TYPE_AS_ImplOpDef: {
        Calc_Coverage_Aux(expr.GetField(pos_AS_ImplOpDef_oppre), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_ImplOpDef_oppost), ci);
        Calc_Coverage_Aux(expr.GetSequence(pos_AS_ImplOpDef_excps), ci);
        break;
      }
      case TAG_TYPE_AS_FnBody: {
        Calc_Coverage_Aux(expr.GetField(pos_AS_FnBody_body), ci);
        break;
      }
      case TAG_TYPE_AS_OpBody: {
        Calc_Coverage_Aux(expr.GetField(pos_AS_OpBody_body), ci);
        break;
      }
  
      case TAG_TYPE_AS_Error: {
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_Error_cond), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_Error_action), ci);
        break;
      }
      case TAG_TYPE_AS_ValueDef: {
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_ValueDef_pat), ci);
        Calc_Coverage_Aux(expr.GetField(pos_AS_ValueDef_tp), ci);
        break;
      }
      case TAG_TYPE_AS_DefStmt: {
        const type_dd2PL & def_l (expr.GetSequence(pos_AS_DefStmt_value));
        IncComposite(expr, ci);
        size_t len_def_l = def_l.Length();
        for (size_t index = 1; index <= len_def_l; index++) {
          const Tuple & t (def_l[index]);
          Calc_Coverage_Aux(t.GetRecord(1), ci);
          Calc_Coverage_Aux(t.GetRecord(2), ci);
        }
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_DefStmt_In), ci);
        break;
      }
      case TAG_TYPE_AS_LetStmt: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetSequence(pos_AS_LetStmt_localdef), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_LetStmt_In), ci);
        break;
      }
      case TAG_TYPE_AS_LetBeSTStmt: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetSequence(pos_AS_LetBeSTStmt_bind), ci);
        Calc_Coverage_Aux(expr.GetField(pos_AS_LetBeSTStmt_St), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_LetBeSTStmt_In), ci);
        break;
      }
      case TAG_TYPE_AS_BlockStmt: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetSequence(pos_AS_BlockStmt_dcls), ci);
        Calc_Coverage_Aux(expr.GetSequence(pos_AS_BlockStmt_stmts), ci);
        break;
      }
      case TAG_TYPE_AS_AssignDef: {
        Calc_Coverage_Aux(expr.GetField(pos_AS_AssignDef_dclinit), ci);
        break;
      }
      case TAG_TYPE_AS_AssignStmt: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_AssignStmt_rhs), ci);
        break;
      }
      case TAG_TYPE_AS_AtomicAssignStmt: {
        const SEQ<TYPE_AS_AssignStmt> & atm (expr.GetSequence(pos_AS_AtomicAssignStmt_atm));
        size_t len_atm = atm.Length();
        for (size_t idx = 1; idx <= len_atm; idx++) {
          Calc_Coverage_Aux(atm[idx], ci);
        }
        break;
      }
      case TAG_TYPE_AS_SeqForLoopStmt: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_SeqForLoopStmt_cv), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_SeqForLoopStmt_fseq), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_SeqForLoopStmt_body), ci);
        break;
      }
      case TAG_TYPE_AS_SetForLoopStmt: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_SetForLoopStmt_cv), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_SetForLoopStmt_fset), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_SetForLoopStmt_body), ci);
        break;
      }
      case TAG_TYPE_AS_IndexForLoopStmt: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_IndexForLoopStmt_lb), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_IndexForLoopStmt_ub), ci);
        Calc_Coverage_Aux(expr.GetField(pos_AS_IndexForLoopStmt_By), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_IndexForLoopStmt_body), ci);
        break;
      }
      case TAG_TYPE_AS_WhileLoopStmt: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_WhileLoopStmt_test), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_WhileLoopStmt_body), ci);
        break;
      }
      case TAG_TYPE_AS_CallStmt: {
        IncComposite(expr, ci);
#ifdef VDMPP
        Calc_Coverage_Aux(expr.GetField(pos_AS_CallStmt_obj), ci);
#endif // VDMPP
        Calc_Coverage_Aux(expr.GetSequence(pos_AS_CallStmt_args), ci);
        break;
      }
      case TAG_TYPE_AS_ReturnStmt: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetField(pos_AS_ReturnStmt_val), ci);
        break;
      }
      case TAG_TYPE_AS_IfStmt: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetSequence(pos_AS_IfStmt_elsif), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_IfStmt_test), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_IfStmt_cons), ci);
        Calc_Coverage_Aux(expr.GetField(pos_AS_IfStmt_altn), ci);
        break;
      }
      case TAG_TYPE_AS_ElseifStmt: {
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_ElseifStmt_test), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_ElseifStmt_cons), ci);
        break;
      }
      case TAG_TYPE_AS_CasesStmt: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetSequence(pos_AS_CasesStmt_altns), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_CasesStmt_sel), ci);
        Calc_Coverage_Aux(expr.GetField(pos_AS_CasesStmt_Others), ci);
        break;
      }
      case TAG_TYPE_AS_CasesStmtAltn: {
        Calc_Coverage_Aux(expr.GetSequence(pos_AS_CasesStmtAltn_match), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_CasesStmtAltn_body), ci);
        break;
      }
      case TAG_TYPE_AS_AlwaysStmt: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_AlwaysStmt_Post), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_AlwaysStmt_body), ci);
        break;
      }
      case TAG_TYPE_AS_ExitStmt: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetField(pos_AS_ExitStmt_expr), ci);
        break;
      }
      case TAG_TYPE_AS_TrapStmt: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_TrapStmt_pat), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_TrapStmt_Post), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_TrapStmt_body), ci);
        break;
      }
      case TAG_TYPE_AS_RecTrapStmt: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetSequence(pos_AS_RecTrapStmt_traps), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_RecTrapStmt_body), ci);
        break;
      }
      case TAG_TYPE_AS_Trap: {
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_Trap_match), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_Trap_trappost), ci);
        break;
      }
      case TAG_TYPE_AS_NonDetStmt: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetSequence(pos_AS_NonDetStmt_stmts), ci);
        break;
      }
      case TAG_TYPE_AS_SpecificationStmt: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetField(pos_AS_SpecificationStmt_oppre), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_SpecificationStmt_oppost), ci);
        Calc_Coverage_Aux(expr.GetSequence(pos_AS_SpecificationStmt_excps), ci);
        break;
      }
      case TAG_TYPE_AS_SetEnumerationExpr: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetSequence(pos_AS_SetEnumerationExpr_els), ci);
        break;
      }
      case TAG_TYPE_AS_SeqEnumerationExpr: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetSequence(pos_AS_SeqEnumerationExpr_els), ci);
        break;
      }
      case TAG_TYPE_AS_MapEnumerationExpr: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetSequence(pos_AS_MapEnumerationExpr_els), ci);
        break;
      }
      case TAG_TYPE_AS_Maplet: {
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_Maplet_mapdom), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_Maplet_maprng), ci);
        break;
      }
      case TAG_TYPE_AS_BracketedExpr: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_BracketedExpr_expr), ci);
        break;
      }
#ifdef VDMPP
      case TAG_TYPE_AS_InstanceInv: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_InstanceInv_expr), ci);
        break;
      }
      case TAG_TYPE_AS_Permission: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_Permission_guard), ci);
        break;
      }
#endif // VDMPP
    
      case TAG_TYPE_AS_FctTypeInstExpr: {
        IncComposite(expr, ci);
        break;
      }
      case TAG_TYPE_AS_TupleConstructorExpr: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetSequence(pos_AS_TupleConstructorExpr_fields), ci);
        break;
      }
      case TAG_TYPE_AS_TokenConstructorExpr: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_TokenConstructorExpr_field), ci);
        break;
      }
      case TAG_TYPE_AS_PrefixExpr: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_PrefixExpr_arg), ci);
        break;
      }
      case TAG_TYPE_AS_DefExpr: {
        const type_dd2PL & def_l (expr.GetSequence(pos_AS_DefExpr_Def));
        IncComposite(expr,ci);
        size_t len_def_l = def_l.Length();
        for (size_t index = 1; index <= len_def_l; index++) {
          const Tuple & t (def_l[index]);
          Calc_Coverage_Aux(t.GetRecord (1), ci);
          Calc_Coverage_Aux(t.GetRecord (2), ci);
        }
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_DefExpr_In), ci);
        break;
      }
      case TAG_TYPE_AS_LetExpr: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetSequence(pos_AS_LetExpr_localdef), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_LetExpr_body), ci);
        break;
      }
      case TAG_TYPE_AS_ExistsUniqueExpr: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_ExistsUniqueExpr_bind), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_ExistsUniqueExpr_pred), ci);
        break;
      }
      case TAG_TYPE_AS_IotaExpr: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_IotaExpr_bind), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_IotaExpr_pred), ci);
        break;
      }
      case TAG_TYPE_AS_SetRangeExpr: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_SetRangeExpr_lb), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_SetRangeExpr_ub), ci);
        break;
      }
      case TAG_TYPE_AS_SeqModifyMapOverrideExpr: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_SeqModifyMapOverrideExpr_seqmap), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_SeqModifyMapOverrideExpr_mapexp), ci);
        break;
      }
      case TAG_TYPE_AS_RecordConstructorExpr: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetSequence(pos_AS_RecordConstructorExpr_fields), ci);
        break;
      }
      case TAG_TYPE_AS_RecordModifierExpr: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetSequence(pos_AS_RecordModifierExpr_modifiers), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_RecordModifierExpr_rec), ci);
        break;
      }
      case TAG_TYPE_AS_RecordModification: {
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_RecordModification_newexpr), ci);
        break;
      }
      case TAG_TYPE_AS_ApplyExpr: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetSequence(pos_AS_ApplyExpr_arg), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_ApplyExpr_fct), ci);
        break;
      }
      case TAG_TYPE_AS_FieldSelectExpr: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_FieldSelectExpr_rec), ci);
        break;
      }
      case TAG_TYPE_AS_LambdaExpr: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_LambdaExpr_body), ci);
        break;
      }
      case TAG_TYPE_AS_IsExpr: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_IsExpr_arg), ci);
        break;
      }
      case TAG_TYPE_AS_TypeJudgementExpr: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_TypeJudgementExpr_expr), ci);
        break;
      }
      case TAG_TYPE_AS_NarrowExpr: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_NarrowExpr_expr), ci);
        break;
      }
      case TAG_TYPE_AS_BinaryExpr: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_BinaryExpr_left), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_BinaryExpr_right), ci);
        break;
      }
      case TAG_TYPE_AS_LetBeSTExpr: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetSequence(pos_AS_LetBeSTExpr_lhs), ci);
        Calc_Coverage_Aux(expr.GetField(pos_AS_LetBeSTExpr_St), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_LetBeSTExpr_In), ci);
        break;
      }
      case TAG_TYPE_AS_CasesExpr: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetSequence(pos_AS_CasesExpr_altns), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_CasesExpr_sel), ci);
        Calc_Coverage_Aux(expr.GetField(pos_AS_CasesExpr_Others), ci);
        break;
      }
      case TAG_TYPE_AS_CaseAltn: {
        Calc_Coverage_Aux(expr.GetSequence(pos_AS_CaseAltn_match), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_CaseAltn_body), ci);
        break;
      }
      case TAG_TYPE_AS_AllOrExistsExpr: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetSequence(pos_AS_AllOrExistsExpr_bind), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_AllOrExistsExpr_pred), ci);
        break;
      }
      case TAG_TYPE_AS_SetComprehensionExpr: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_SetComprehensionExpr_elem), ci);
        Calc_Coverage_Aux(expr.GetSequence(pos_AS_SetComprehensionExpr_bind), ci);
        Calc_Coverage_Aux(expr.GetField(pos_AS_SetComprehensionExpr_pred), ci);
        break;
      }
      case TAG_TYPE_AS_SeqComprehensionExpr: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_SeqComprehensionExpr_elem), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_SeqComprehensionExpr_bind), ci);
        Calc_Coverage_Aux(expr.GetField(pos_AS_SeqComprehensionExpr_pred), ci);
        break;
      }
      case TAG_TYPE_AS_SubSequenceExpr: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_SubSequenceExpr_sequence), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_SubSequenceExpr_frompos), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_SubSequenceExpr_topos), ci);
        break;
      }
      case TAG_TYPE_AS_MapComprehensionExpr: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_MapComprehensionExpr_elem), ci);
        Calc_Coverage_Aux(expr.GetSequence(pos_AS_MapComprehensionExpr_bind), ci);
        Calc_Coverage_Aux(expr.GetField(pos_AS_MapComprehensionExpr_pred), ci);
        break;
      }
      case TAG_TYPE_AS_IfExpr: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetSequence(pos_AS_IfExpr_elsif), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_IfExpr_test), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_IfExpr_cons), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_IfExpr_altn), ci);
        break;
      }
      case TAG_TYPE_AS_ElseifExpr: {
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_ElseifExpr_test), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_ElseifExpr_cons), ci);
        break;
      }
      case TAG_TYPE_AS_TupleSelectExpr: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_TupleSelectExpr_tuple), ci);
        //Calc_Coverage_Aux(expr.GetRecord(pos_AS_TupleSelectExpr_no), ci);
        break;
      }
      // patterns and binds are not included in the test coverage
      // only expressions nested inside MatchVal are considered.
      case TAG_TYPE_AS_TuplePattern: {
        Calc_Coverage_Aux(expr.GetSequence(pos_AS_TuplePattern_fields), ci);
        break;
      }
      case TAG_TYPE_AS_SetEnumPattern: {
        Calc_Coverage_Aux(expr.GetSequence(pos_AS_SetEnumPattern_Elems), ci);
        break;
      }
      case TAG_TYPE_AS_SeqEnumPattern: {
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_SeqEnumPattern_els), ci);
        break;
      }
      case TAG_TYPE_AS_MapEnumPattern: {
        const SEQ<TYPE_AS_MapletPattern> & mls (expr.GetSequence(pos_AS_MapEnumPattern_mls));
        size_t len_mls = mls.Length();
        for (size_t i = 1; i <= len_mls; i++) {
          const TYPE_AS_MapletPattern & mp (mls[i]);
          Calc_Coverage_Aux(mp.GetRecord(pos_AS_MapletPattern_dp), ci);
          Calc_Coverage_Aux(mp.GetRecord(pos_AS_MapletPattern_rp), ci);
        }
        break;
      }
      case TAG_TYPE_AS_RecordPattern: {
        Calc_Coverage_Aux(expr.GetSequence(pos_AS_RecordPattern_fields), ci);
        break;
      }
      case TAG_TYPE_AS_SeqConcPattern: {
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_SeqConcPattern_lp), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_SeqConcPattern_rp), ci);
        break;
      }
      case TAG_TYPE_AS_SetUnionPattern: {
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_SetUnionPattern_lp), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_SetUnionPattern_rp), ci);
        break;
      }
      case TAG_TYPE_AS_MapMergePattern: {
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_MapMergePattern_lp), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_MapMergePattern_rp), ci);
        break;
      }
#ifdef VDMPP
      case TAG_TYPE_AS_ObjectPattern: {
        const SEQ<TYPE_AS_FieldPattern> fields (expr.GetSequence(pos_AS_ObjectPattern_fields));
        size_t len_fields = fields.Length();
        for (size_t i = 1; i <= len_fields; i++) {
          Calc_Coverage_Aux(fields[i].GetRecord(pos_AS_FieldPattern_pat), ci);
        }
        break;
      }
#endif // VDMPP
      case TAG_TYPE_AS_MatchVal: {
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_MatchVal_val), ci);
        break;
      }
      case TAG_TYPE_AS_PatternName:
        break;
      case TAG_TYPE_AS_SetBind: {
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_SetBind_pat), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_SetBind_Set), ci);
        break;
      }
      case TAG_TYPE_AS_TypeBind: {
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_TypeBind_pat), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_TypeBind_tp), ci);
        break;
      }
      case TAG_TYPE_AS_SeqBind: {
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_SeqBind_pat), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_SeqBind_Seq), ci);
        break;
      }
      case TAG_TYPE_AS_MultSetBind: {
        Calc_Coverage_Aux(expr.GetSequence(pos_AS_MultSetBind_pat), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_MultSetBind_Set), ci);
        break;
      }
      case TAG_TYPE_AS_MultSeqBind: {
        Calc_Coverage_Aux(expr.GetSequence(pos_AS_MultSeqBind_pat), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_MultSeqBind_Seq), ci);
        break;
      }
      case TAG_TYPE_AS_MultTypeBind: {
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_MultTypeBind_pat), ci);
        break;
      }
    
#ifdef VDMPP
      case TAG_TYPE_AS_SameClassExpr: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_SameClassExpr_expr1), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_SameClassExpr_expr2), ci);
        break;
      }
      case TAG_TYPE_AS_SameBaseClassExpr: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_SameBaseClassExpr_expr1), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_SameBaseClassExpr_expr2), ci);
        break;
      }
      case TAG_TYPE_AS_IsOfClassExpr: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_IsOfClassExpr_cls), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_IsOfClassExpr_arg), ci);
        break;
      }
      case TAG_TYPE_AS_IsOfBaseClassExpr: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_IsOfBaseClassExpr_cls), ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_IsOfBaseClassExpr_arg), ci);
        break;
      }
      case TAG_TYPE_AS_NewExpr: {
        IncComposite(expr, ci);
        Calc_Coverage_Aux(expr.GetRecord(pos_AS_NewExpr_cls), ci);
        break;
      }
#endif //VDMPP
      default: {
  //wcout << expr << endl;
      }
    }
  }
}

#ifdef PROTO
static Map ModMap;

Map RTINFO::AddClassType (int tp, Generic g, Map mp)
{
  Record rc (tp, 1);
  rc.SetField (1, g);
  if (mp.DomExists (rc)) {
    mp.ImpModify (rc, Int (Int (mp[rc]) + Int (1)));
  }
  else {
    mp.ImpModify (rc, Int (1));
  }
  return mp;
}

void RTINFO::Set_RT_Val_Cov (Record mod, Record fn, Sequence vals)
{ Map FnMap;
  if (ModMap.DomExists (mod)) {
    FnMap = ModMap[mod];
  }
  vdm_iplog << L"\n\nmod : "; mod.pr_ascii (vdm_iplog, 0);
  vdm_iplog << L"\n\nfn  : "; fn.pr_ascii (vdm_iplog, 0);
  vdm_iplog << L"\n\nvals: "; vals.pr_ascii (vdm_iplog, 0);

  int i;
  Sequence FnParms;
  if (FnMap.DomExists (fn)) {
    FnParms = FnMap[fn];
  }
  else {
    for (i = 1; i <= vals.Length (); i++) {
      FnParms.ImpAppend (Map ());
    }
  }
  for (i = 1; i <= vals.Length (); i++) {

    Record val       (vals.Index(i));
    Map recorded_val (FnParms.Index (i));
    
    switch (val.GetTag ()) {
    case TAG_TYPE_SEM_BOOL:
      if (Bool (val.GetField (1)).GetValue ()) {
        recorded_val = AddClassType (bool_t, Nil (), recorded_val);
      }
      else {
        recorded_val = AddClassType (bool_f, Nil (), recorded_val);
      }
      break;
    case TAG_TYPE_SEM_NUM: {
      Real r (((const TYPE_SEM_NUM &)val).get_v());
      if (r.GetValue () < 0) {
	recorded_val = AddClassType (real_m, Nil (), recorded_val);
      }
      else if (r.GetValue () == 0) {
	recorded_val = AddClassType (real_0, Nil (), recorded_val);
      }
      else {
	recorded_val = AddClassType (real_p, Nil (), recorded_val);
      }
      break;
    }
    case TAG_TYPE_SEM_CHAR:
      recorded_val = AddClassType (char_, Nil (), recorded_val);
      break;
    case TAG_TYPE_SEM_QUOTE: {
      TYPE_SEM_QUOTE qv (val);
      recorded_val = AddClassType (quote_, qv.get_v(), recorded_val);
      break;
    }
    case TAG_TYPE_SEM_NIL:
      recorded_val = AddClassType (nil_, Nil (), recorded_val);
      break;
    case TAG_TYPE_SEM_TOKEN:
      recorded_val = AddClassType (token_, Nil (), recorded_val);
      break;
    case TAG_TYPE_SEM_CONT:
      recorded_val = AddClassType (cont_, Nil (), recorded_val);
      break;
    case TAG_TYPE_SEM_EXIT:
      recorded_val = AddClassType (exit_, Nil (), recorded_val);
      break;
    case TAG_TYPE_SEM_ExplFN:
    case TAG_TYPE_SEM_ExplOP:
    case TAG_TYPE_SEM_ExplPOLY:
      recorded_val = AddClassType (fct_, Nil (), recorded_val);
      break;
    case TAG_TYPE_SEM_SET: {
      SET<TYPE_SEM_VAL> sv (((const TYPE_SEM_SET &)val).get_v());
      if (sv.Card () == 0) {
        recorded_val = AddClassType (set_0, Nil (), recorded_val);
      }
      else if (sv.Card () == 1) {
        recorded_val = AddClassType (set_1, Nil (), recorded_val);
      }
      else {
        recorded_val = AddClassType (set_2, Nil (), recorded_val);
      }
      break;
    }
    case TAG_TYPE_SEM_SEQ: {
      SEQ<TYPE_SEM_VAL> lv (((const TYPE_SEM_SEQ &)val).get_v());
      if (lv.Length () == 0) {
        recorded_val = AddClassType (seq_0, Nil (), recorded_val);
      }
      else if (lv.Length () == 1) {
        recorded_val = AddClassType (seq_1, Nil (), recorded_val);
      }
      else {
        recorded_val = AddClassType (seq_2, Nil (), recorded_val);
      }
      break;
    }
    case TAG_TYPE_SEM_MAP: {
      MAP<TYP_SEM_VAL,TYPE_SEM_VAL> mv (((const TYPE_SEM_MAP &)val).get_v());
      if (mv.Size () == 0) {
        recorded_val = AddClassType (map_0, Nil (), recorded_val);
      }
      else if (mv.Size () == 1) {
        recorded_val = AddClassType (map_1, Nil (), recorded_val);
      }
      else {
        recorded_val = AddClassType (map_2, Nil (), recorded_val);
      }
      break;
    }
    case TAG_TYPE_SEM_TUPLE: {
      SEQ<TYPE_SEM_VAL> lv (((const TYPE_SEM_TUPLE &)val).get_v());
      recorded_val = AddClassType (tuple_, Int (lv.Length ()), recorded_val);
      break;
    }
    case TAG_TYPE_DYNSEM_SEM_REC:
      recorded_val = AddClassType (record_, val.GetField (1), recorded_val);
      break;
    case TAG_TYPE_SEM_UNDEF:
      recorded_val = AddClassType (undef_, Nil (), recorded_val);
      break;
    }
    FnParms.ImpModify (i, recorded_val);
  }

  FnMap.ImpModify (fn, FnParms);
  ModMap.ImpModify (mod, FnMap);
  vdm_iplog << L"\n\nSet_RT_Val_Cov: ";
  ModMap.pr_ascii (vdm_iplog, 0);
  vdm_iplog << L"\n\n\n";
}


int RTINFO::Save_Val_Cov (wstring str)
{
  ofstream ostr (str);
  if (!ostr) {
    vdm_iplog << L"Couldn't open file '" << str << L"'" << endl << flush;
    return 1;
  }
  
  ModMap.WriteVal(ostr);
  return 0;
}

int RTINFO::Set_Val_Cov (wstring str)
{
  ifstream istr (str);
  if (!istr) {
    vdm_iplog << L"Couldn't open file '" << str << L"'" << endl << flush;
    return 1;
  }

  ModMap = ReadVal (istr);
  return 0;
}

Sequence RTINFO::CalculateValCov (Record mod, Record fn, ofstream& ostr,
                          const ContextInfo&ci)
{ Record fn_name (fn.GetField (1));
  Record tp;

  if (fn.Is(ExplOpDef)) {
    tp = fn.GetField (2);
  }
  else {
    tp = fn.GetField (3);
  }
  Map FnMap;
  Sequence res;
  if (ModMap.DomExists (mod)) {
    FnMap = ModMap[mod];
  }
  else {
    vdm_iplog << L"Module/Class '" << mod.ascii () << L"' is not defined in value coverage" << endl << flush;
    ModMap.pr_ascii (vdm_iplog, 0);
    return res;
  }
  
  Sequence FnParms;
  if (FnMap.DomExists (fn_name)) {
    FnParms = FnMap[fn_name];
  }
  else {
    vdm_iplog << L"Function '" << fn_name.ascii () << L"' is not defined in module '"
              << mod.ascii () << L"' in value coverage" << endl << flush;
    ModMap.pr_ascii (vdm_iplog, 0);
    return res;
  }

  int num = Is_Covered (fn_name, ci);

  if (!num) {
    vdm_iplog << L"PROBLEM for function '" << fn_name.ascii () << L"'!!" << endl << flush;
    return res;
  }

  FnParms.pr_ascii (vdm_iplog, 0);

  Generic g;
  for (bool b = FnParms.First(g); b; b = FnParms.Next(g)) {
    ostr << L"Parameter: ?? " << endl;
    Map mp (g);
    Generic gg;
    for (bool bb = mp.First(gg); bb; bb = mp.Next(gg)) {
        ostr << L"  " << Record (gg).GetTag () << L"  " << mp[gg].ascii () << L"/" << num << endl;
    }
    ostr << endl << endl << flush;
  }

  return Sequence ();
}
#endif //PROTO

