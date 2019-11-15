/***
*  * WHAT
*  *    A function which Extract pretty printer information
        from  an Abstract Syntax Tree and inserts the info
        in a Token list. The following information will be extrected:
        - test coverage information 
        - index information 
            o definition of symbols, 
            o use - if type info is avaliable (future)
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/specfile/extr_pp_info.cc,v $
*  * VERSION
*  *    $Revision: 1.11 $
*  * DATE 
*  *    $Date: 2006/06/02 01:31:12 $
*  * STATUS
*  *    $State: Exp $
*  * REFERENCES
*  *    
*  * PROJECT
*  *    IDERES/AFRODITE
*  * AUTHOR
*  *   Lars + $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "extr_pp_info.h"

#include "asquotes.h"
#include "cgtag.h"
#include "astaux.h"
#include "ProjectTypes.h"
#include "contextinfo.h"
#include "tokenlist.h"


void extr_pp_info (const Generic& g,      // the AS tree to extr pp info from
                   ContextInfo  & c,      // The context info table
                   TokenList    & t,      // the token list to update
                   bool         cov_on    // allow covering - true if in Stmt or Expression
                      )
{
  if (g.IsSequence()) {
    // SEQUENCE CASE
    Sequence Sq (g);
    int len_Sq = Sq.Length();
    for (int i = 1; i<= len_Sq; i++) {
      extr_pp_info (Sq[i], c, t, cov_on);
    }
    return;
  }
  else if (g.IsTuple()) { 
    // TUPLE
    Tuple Tp(g);
    int Length = Tp.Length();
    for (int i = 1; i <= Length; i++) {
      extr_pp_info(Tp.GetField(i), c, t, cov_on);
    }
    return;
  }
  else if (g.IsSet()) {
    // SET CASE
    Set St(g);
    Generic Element;
    for (bool bb = St.First (Element); bb; bb = St.Next (Element)) {
      extr_pp_info (Element, c, t, cov_on);
    }
    return;
  }
  else if (g.IsMap()) {
    // MAP CASE
    Map Mp(g);
    Set dom_Mp(Mp.Dom());
    Generic Element;
    for (bool bb = dom_Mp.First (Element); bb; bb = dom_Mp.Next (Element)) {
      extr_pp_info (Element, c, t, cov_on);
      extr_pp_info (Mp[Element], c, t, cov_on);
    }
    return;
  }
  else if (g.IsRecord()) {
    // RECORD CASE 
    Generic dummy = g;
    Record Rc(dummy);
    int Tag = Rc.GetTag();

    ///////////////////////
    // TEST COVERAGE INFO
    // Unmark all non covered Stmt's and Expressions
    // Only Stmts and Expresssions may change the coverage
    ///////////////////////

    if (ASTAUX::IsASTRec(Rc)) {
      switch (Tag) {

          // special cases
#ifdef VDMSL
        case TAG_TYPE_AS_DLModule:
#endif
          // statements
        case TAG_TYPE_AS_DefStmt:
        case TAG_TYPE_AS_LetStmt:
        case TAG_TYPE_AS_LetBeSTStmt:
        case TAG_TYPE_AS_AssignStmt:
        case TAG_TYPE_AS_SeqForLoopStmt:
        case TAG_TYPE_AS_SetForLoopStmt:
        case TAG_TYPE_AS_IndexForLoopStmt:
        case TAG_TYPE_AS_WhileLoopStmt:
        case TAG_TYPE_AS_CallStmt:
        case TAG_TYPE_AS_ErrorStmt:
        case TAG_TYPE_AS_AlwaysStmt:
        case TAG_TYPE_AS_ExitStmt:
        case TAG_TYPE_AS_TrapStmt:
        case TAG_TYPE_AS_RecTrapStmt:
        case TAG_TYPE_AS_NonDetStmt:
        case TAG_TYPE_AS_ReturnStmt:
        case TAG_TYPE_AS_IfStmt:
        case TAG_TYPE_AS_ElseifStmt:
        case TAG_TYPE_AS_IdentStmt:
#ifdef VDMPP
        case TAG_TYPE_AS_SpecificationStmt:
        case TAG_TYPE_AS_StartStmt:
        case TAG_TYPE_AS_StartListStmt:
#endif
          // expressions
        case TAG_TYPE_AS_PrefixExpr:
        case TAG_TYPE_AS_BinaryExpr:
        case TAG_TYPE_AS_DefExpr:
        case TAG_TYPE_AS_LetExpr:
        case TAG_TYPE_AS_LetBeSTExpr:
        case TAG_TYPE_AS_IfExpr:
        case TAG_TYPE_AS_ElseifExpr:
        case TAG_TYPE_AS_AllOrExistsExpr:
        case TAG_TYPE_AS_ExistsUniqueExpr:
        case TAG_TYPE_AS_IotaExpr:
        case TAG_TYPE_AS_SetEnumerationExpr:
        case TAG_TYPE_AS_SetComprehensionExpr:
        case TAG_TYPE_AS_SetRangeExpr:
        case TAG_TYPE_AS_SeqEnumerationExpr:
        case TAG_TYPE_AS_SeqComprehensionExpr:
        case TAG_TYPE_AS_SubSequenceExpr:
        case TAG_TYPE_AS_SeqModifyMapOverrideExpr:
        case TAG_TYPE_AS_MapEnumerationExpr:
        case TAG_TYPE_AS_MapComprehensionExpr:
        case TAG_TYPE_AS_TupleConstructorExpr:
        case TAG_TYPE_AS_RecordModifierExpr:
        case TAG_TYPE_AS_ApplyExpr:
        case TAG_TYPE_AS_FieldSelectExpr:
        case TAG_TYPE_AS_TokenConstructorExpr:
        case TAG_TYPE_AS_LambdaExpr:
        case TAG_TYPE_AS_FctTypeInstExpr:
        case TAG_TYPE_AS_IsExpr:
        case TAG_TYPE_AS_BoolLit:
        case TAG_TYPE_AS_CharLit:
        case TAG_TYPE_AS_TextLit:
        case TAG_TYPE_AS_QuoteLit:
        case TAG_TYPE_AS_RealLit:
        case TAG_TYPE_AS_NumLit:
        case TAG_TYPE_AS_NilLit:
        case TAG_TYPE_AS_UndefinedExpr:
        case TAG_TYPE_AS_BracketedExpr:
#ifdef VDMPP
        case TAG_TYPE_AS_SelfExpr:
        case TAG_TYPE_AS_NewExpr:
        case TAG_TYPE_AS_IsOfClassExpr:
        case TAG_TYPE_AS_IsOfBaseClassExpr:
        case TAG_TYPE_AS_SameBaseClassExpr:
        case TAG_TYPE_AS_SameClassExpr:
        case TAG_TYPE_AS_ActExpr:
        case TAG_TYPE_AS_FinExpr:
        case TAG_TYPE_AS_ActiveExpr:
        case TAG_TYPE_AS_WaitingExpr:
        case TAG_TYPE_AS_ReqExpr:
#endif // VDMPP
        {
          TYPE_CI_ContextId ci = Rc.GetField(Rc.Length()); // get ci field
          if (ci == NilContextId) {
            break;
          }
          TYPE_CI_TokenSpan pos (c.GetPos(ci)); // get position for tokens

          // handle quote decl inside type decl
#ifdef VDMSL
          if ( (TAG_TYPE_AS_QuoteLit == Tag || TAG_TYPE_AS_DLModule == Tag) && ! cov_on ) {  
#endif // VDMSL
#ifdef VDMPP
          if ( (TAG_TYPE_AS_QuoteLit == Tag) && ! cov_on ) {  
#endif // VDMPP
            t.set_test_coverage(pos, 1);
          }
          else {
            t.set_test_coverage(pos, c.GetTestCoverageInfo(ci));
          }
          cov_on = true;  // allow covering
          break;
        }
        // Special cases compared to Expr
        // Names must reside inside an Expr or a Stmt

        case TAG_TYPE_AS_Name: {
          if (cov_on) {
           TYPE_CI_ContextId ci = Rc.GetInt(pos_AS_Name_cid); // get ci field
            if (ci != NilContextId) {
              if(c.HasTestCoverage(ci)) {
                TYPE_CI_TokenSpan pos (c.GetPos(ci)); // get position for tokens
                t.set_test_coverage(pos, c.GetTestCoverageInfo(ci));
              }
            }
          }
          return;
          break;
        }
        case TAG_TYPE_AS_OldName: {
          if (cov_on) {
            TYPE_CI_ContextId ci = Rc.GetInt(pos_AS_OldName_cid); // get ci field
            if (ci != NilContextId) {
              if(c.HasTestCoverage(ci)) {
                TYPE_CI_TokenSpan pos (c.GetPos(ci)); // get position for tokens
                t.set_test_coverage(pos, c.GetTestCoverageInfo(ci));
              }
            }
          }
          return;
          break;
        }
        case TAG_TYPE_AS_BlockStmt: {
          TYPE_AS_BlockStmt bs (g);
          SEQ<TYPE_AS_AssignDef> dcls (bs.get_dcls());
          SEQ<TYPE_AS_Stmt> stmts (bs.get_stmts());
          Generic dcl;
          for (bool bb = dcls.First(dcl); bb; bb = dcls.Next(dcl)) {
            extr_pp_info (dcl, c, t, cov_on);
          }
          Generic stmt;
          for (bool cc = stmts.First(stmt); cc; cc = stmts.Next(stmt)) {
            extr_pp_info (stmt, c, t, cov_on);
          }
          return;
          break;
        }
        case TAG_TYPE_AS_CasesStmt: {
          TYPE_AS_CasesStmt cs (g);
          TYPE_AS_Expr sel (cs.get_sel());
          SEQ<TYPE_AS_CasesStmtAltn> altns (cs.get_altns());
          Generic others (cs.get_Others());
          extr_pp_info (sel, c, t, cov_on);
          Generic altn;
          for (bool bb = altns.First(altn); bb; bb = altns.Next(altn)) {
            extr_pp_info (altn, c, t, cov_on);
          }
          if (!others.IsNil()) {
            extr_pp_info (others, c, t, cov_on);
          }
          return;
          break;
        }
        case TAG_TYPE_AS_CasesStmtAltn: {
          TYPE_AS_CasesStmtAltn altn (g);
          SEQ<TYPE_AS_Pattern> m (altn.get_match ());
          Generic pat;
          for (bool bb = m.First(pat); bb; bb = m.Next(pat)) {
            //extr_pp_info (pat, c, t, cov_on);
            TYPE_CI_ContextId ci = ASTAUX::GetCid(pat);
            if (ci != NilContextId) {
              if (c.HasTestCoverage(ci)) {
                if (c.GetTestCoverageInfo(ci) == 0) {
                  TYPE_CI_TokenSpan pos (c.GetPos(ci)); // get position for tokens
                  t.set_test_coverage(pos, 0);
                } 
              } 
            }
          }
          TYPE_AS_Stmt stmt (altn.get_body ());
          //extr_pp_info (stmt, c, t, cov_on);
          extr_pp_info (stmt, c, t, true);
          return;
          break;
        }
        case TAG_TYPE_AS_CasesExpr: {
          TYPE_AS_CasesExpr cs (g);
          const TYPE_AS_Expr & sel (cs.GetRecord(pos_AS_CasesExpr_sel));
          const SEQ<TYPE_AS_CaseAltn> & altns (cs.GetSequence(pos_AS_CasesExpr_altns));
          const Generic & others (cs.GetField(pos_AS_CasesExpr_Others));
          extr_pp_info (sel,c,t,cov_on);
          size_t len_altns = altns.Length();
          for (size_t idx = 1; idx <= len_altns; idx++) {
            extr_pp_info (altns[idx], c, t, cov_on);
          }
          if (!others.IsNil()) {
            extr_pp_info (others, c, t, cov_on);
          }
          return;
          break;
        }
        case TAG_TYPE_AS_CaseAltn: {
          TYPE_AS_CaseAltn altn (g);
          SEQ<TYPE_AS_Pattern> m (altn.get_match ());
          Generic pat;
          for (bool bb = m.First(pat); bb; bb = m.Next(pat)) {
            //extr_pp_info (pat, c, t, cov_on);
            TYPE_CI_ContextId ci = ASTAUX::GetCid(pat);
            if (ci != NilContextId) {
              if (c.HasTestCoverage(ci)) {
                if (c.GetTestCoverageInfo(ci) == 0) {
                  TYPE_CI_TokenSpan pos (c.GetPos(ci)); // get position for tokens
                  t.set_test_coverage(pos, 0);
                } 
              } 
            }
          }
          TYPE_AS_Expr expr (altn.get_body ());
          //extr_pp_info (expr, c, t, cov_on);
          extr_pp_info (expr, c, t, true);
          return;
          break;
        }
        case TAG_TYPE_AS_PatternName: {
//          TYPE_AS_PatternName pn (g);
//          Generic nm (pn.get_nm());
//          extr_pp_info (nm, c, t, cov_on);
          break;
        }
        case TAG_TYPE_AS_MatchVal: {
          TYPE_AS_MatchVal mv (g);
          TYPE_AS_Expr expr (mv.get_val());
          extr_pp_info (expr, c, t, cov_on);
          break;
        }
        case TAG_TYPE_AS_SetEnumPattern: {
          TYPE_AS_SetEnumPattern sep (g);
          SEQ<TYPE_AS_Pattern> m (sep.get_Elems());
          Generic pat;
          for (bool bb = m.First(pat); bb; bb = m.Next(pat)) {
            extr_pp_info (pat, c, t, cov_on);
          }
          break;
        }
        case TAG_TYPE_AS_SetUnionPattern: {
          TYPE_AS_SetUnionPattern sup (g);
          TYPE_AS_Pattern lp (sup.get_lp());
          TYPE_AS_Pattern rp (sup.get_rp());
          extr_pp_info (lp, c, t, cov_on);
          extr_pp_info (rp, c, t, cov_on);
          break;
        }
        case TAG_TYPE_AS_SeqEnumPattern: {
          TYPE_AS_SeqEnumPattern sep (g);
          SEQ<TYPE_AS_Pattern> m (sep.get_els());
          Generic pat;
          for (bool bb = m.First(pat); bb; bb = m.Next(pat)) {
            extr_pp_info (pat, c, t, cov_on);
          }
          break;
        }
        case TAG_TYPE_AS_SeqConcPattern: {
          TYPE_AS_SeqConcPattern sup (g);
          TYPE_AS_Pattern lp (sup.get_lp());
          TYPE_AS_Pattern rp (sup.get_rp());
          extr_pp_info (lp, c, t, cov_on);
          extr_pp_info (rp, c, t, cov_on);
          break;
        }
        case TAG_TYPE_AS_TuplePattern: {
          TYPE_AS_TuplePattern tp (g);
          SEQ<TYPE_AS_Pattern> m (tp.get_fields()); 
          Generic pat;
          for (bool bb = m.First(pat); bb; bb = m.Next(pat)) {
            extr_pp_info (pat, c, t, cov_on);
          }
          break;
        }
        case TAG_TYPE_AS_RecordPattern: {
          TYPE_AS_RecordPattern tp (g);
          SEQ<TYPE_AS_Pattern> m (tp.get_fields()); 
          Generic pat;
          for (bool bb = m.First(pat); bb; bb = m.Next(pat)) {
            extr_pp_info (pat, c, t, cov_on);
          }
          break;
        }
        case TAG_TYPE_AS_AtomicAssignStmt: {
          TYPE_AS_AtomicAssignStmt aas (g);
          SEQ<TYPE_AS_AssignStmt> atm (aas.get_atm ());
          Generic stmt_g;
          for( bool bb = atm.First(stmt_g); bb; bb = atm.Next(stmt_g)) {
            TYPE_AS_AssignStmt stmt (stmt_g);
            extr_pp_info (stmt, c, t, cov_on);
          }
          break;
        }
        case TAG_TYPE_AS_RecordConstructorExpr: {
          TYPE_AS_RecordConstructorExpr rce (g);
          Sequence fields (rce.get_fields ());
          extr_pp_info (fields, c, t, cov_on);
          break;
        }
        default : {
          cov_on = false;  // do not allwo covering
        }
      }
    } // end of if (ASTAUX::IsASTRec(Rc))

    ///////////////////////
    // INDEX INFORMATION
    ///////////////////////

    switch(Tag) {

#ifdef VDMSL
      case  TAG_TYPE_AS_StateDef : {
        // State def
        TYPE_AS_StateDef e(g);
        TYPE_CI_TokenSpan pos (c.GetPos(e.get_tp().get_name().get_cid())); 
        t.set_index_element(pos, TokenInfo::state_def);
        break;
      }
#endif // VDMSL

      case TAG_TYPE_AS_TypeDef: {
        // Types
        TYPE_AS_TypeDef e(g);
        TYPE_CI_TokenSpan pos (c.GetPos(e.get_nm().get_cid()));
        t.set_index_element(pos, TokenInfo::tp_def);
        break;
      }

      // Functions/Operations
      case TAG_TYPE_AS_ExplFnDef: {
        TYPE_AS_ExplFnDef e(g);
        TYPE_CI_TokenSpan pos (c.GetPos(e.get_nm().get_cid()));
        t.set_index_element(pos, TokenInfo::fct_def);
        break;
      }

      case TAG_TYPE_AS_ExplOpDef: {
        TYPE_AS_ExplOpDef e(g);
        TYPE_CI_TokenSpan pos (c.GetPos(e.get_nm().get_cid())); 
        t.set_index_element(pos, TokenInfo::op_def);
        break;
      }

      case TAG_TYPE_AS_ExtExplFnDef: {
        TYPE_AS_ExplFnDef e(g);
        TYPE_CI_TokenSpan pos (c.GetPos(e.get_nm().get_cid())); 
        t.set_index_element(pos, TokenInfo::fct_def);
        break;
      }

      case TAG_TYPE_AS_ExtExplOpDef: {
        TYPE_AS_ExtExplOpDef e(g);
        TYPE_CI_TokenSpan pos (c.GetPos(e.get_nm().get_cid())); 
        t.set_index_element(pos, TokenInfo::op_def);
        break;
      }

      case TAG_TYPE_AS_ImplFnDef: {
        TYPE_AS_ImplFnDef e(g);
        TYPE_CI_TokenSpan pos (c.GetPos(e.get_nm().get_cid())); 
        t.set_index_element(pos, TokenInfo::fct_def);
        break;
      }

      case TAG_TYPE_AS_ImplOpDef:{
        TYPE_AS_ImplOpDef e(g);
        TYPE_CI_TokenSpan pos (c.GetPos(e.get_nm().get_cid())); 
        t.set_index_element(pos, TokenInfo::op_def);
        break;
      }

#ifdef VDMSL
      case TAG_TYPE_AS_Module: {
        // Module
        TYPE_AS_Module e(g);
        TYPE_CI_TokenSpan pos (c.GetPos(e.get_nm().get_cid())); 
        t.set_index_element(pos, TokenInfo::mod_def);
        break;
      }
#endif

#ifdef VDMPP
      case TAG_TYPE_AS_Class : {
        // Class
        TYPE_AS_Class e(g);
        TYPE_CI_TokenSpan pos (c.GetPos(e.get_nm().get_cid())); 
        t.set_index_element(pos, TokenInfo::class_def);
        break;
      }
#endif //VDMPP
    }
    // ITERATE THROUGH THE RECORD FIELDS

    int Length;
    if (!ASTAUX::IsASTRec(Rc)) {
      Length = Rc.Length();
    }
    else {
      // In this case the last entry is the record is an context identifier
      // and it should not be transformed.
      Length = Rc.Length() -1;
    }
    for(int i = 1; i <= Length; i++) {
      extr_pp_info(Rc.GetField(i), c, t, cov_on);
    }
    return;
  }
  // DEFAULT
}

