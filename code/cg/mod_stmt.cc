/***
*  * WHAT
*  *    mod_stmt.cc: Implementation of mod_stmt.vdm 1.85
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/mod_stmt.cc,v $
*  * VERSION
*  *    $Revision: 1.72 $
*  * DATE
*  *    $Date: 2006/07/03 05:21:15 $
*  * STATUS
*  *    Under development
*  * PROJECT
*  *    Toolbox
*  * AUTHOR
*  *    Henrik Voss + $Author: vdmtools $ + jojo
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "vdmcg.h"
#include "astaux.h"
#include "contextinfo.h"
#include "asquotes.h"
#include "statsem.h"
#include "BC.h"
#include "position.h"
#include "intconvquotes.h"

//wcout << L"intertp: " << INT2Q::h2gAS(intertp) << endl;

// STMT functions and operations
// GenStmt
// stmt : AS`Stmt
// isLast : bool
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenStmt(const TYPE_AS_Stmt & stmt, bool isLast)
{
//wcout << INT2Q::h2gAS(stmt) << endl;

  SEQ<TYPE_CPP_Stmt> cpp;
  switch (stmt.GetTag()) {
    case TAG_TYPE_AS_AssignStmt:
      cpp = CGAssignStmt(stmt);
#ifdef VDMPP
      if (!GetAtomic() && !GetConstr() && GetHasInv() && get_testpreandpost_option())
        cpp.ImpAppend(GenInvCall(GiveCurCASName(), false));
#endif // VDMPP
      break;
    case TAG_TYPE_AS_AtomicAssignStmt: {
      cpp = CGAtomicAssignStmt(stmt, isLast);
      break;
    }
    case TAG_TYPE_AS_IfStmt:
      cpp = CGIfStmt(stmt, isLast);
      break;
    case TAG_TYPE_AS_IdentStmt:
      cpp = CGIdentStmt();
      break;
    case TAG_TYPE_AS_CasesStmt:
      cpp = CGCasesStmt(stmt, isLast);
      break;
    case TAG_TYPE_AS_ReturnStmt:
      cpp = CGReturnStmt(stmt);
      break;
    case TAG_TYPE_AS_BlockStmt:
      cpp = CGBlockStmt(stmt, isLast);
      break;
    case TAG_TYPE_AS_LetStmt:
      cpp = CGLetStmt(stmt, isLast);
      break;
    case TAG_TYPE_AS_WhileLoopStmt:
      cpp = CGWhileLoopStmt(stmt, isLast);
      break;
    case TAG_TYPE_AS_SeqForLoopStmt:
      cpp = CGSeqForLoopStmt(stmt, isLast);
      break;
    case TAG_TYPE_AS_SetForLoopStmt:
      cpp = CGSetForLoopStmt(stmt, isLast);
      break;
    case TAG_TYPE_AS_IndexForLoopStmt:
      cpp = CGIndexedForLoopStmt(stmt, isLast);
      break;
    case TAG_TYPE_AS_LetBeSTStmt:
      cpp = CGLetBeSTStmt(stmt, isLast);
      break;
    case TAG_TYPE_AS_DefStmt:
      cpp = CGDefStmt(stmt, isLast);
      break;
    case TAG_TYPE_AS_NonDetStmt:
      cpp = CGNonDetStmt(stmt, isLast);
      break;
    case TAG_TYPE_AS_CallStmt:
      cpp = CGCallStmt(stmt);
      break;
    case TAG_TYPE_AS_ErrorStmt:
      cpp = CGErrorStmt();
      break;
    case TAG_TYPE_AS_AlwaysStmt:
      cpp = CGAlwaysStmt(stmt, isLast);
      break;
    case TAG_TYPE_AS_ExitStmt:
      cpp = CGExitStmt(stmt);
      break;
    case TAG_TYPE_AS_TrapStmt:
      cpp = CGTrapStmt(stmt, isLast);
      break;
    case TAG_TYPE_AS_RecTrapStmt:
      cpp = CGRecTrapStmt(stmt, isLast);
      break;

    case TAG_TYPE_AS_SpecificationStmt:
      cpp = GenSpecTopStmt(stmt);
      break;

#ifdef VDMPP
    case TAG_TYPE_AS_StartStmt:
      if (vdm_CPP_isJAVA() && get_conc_option())
        cpp = GenStartStmt(stmt, isLast);
      else {
        cpp.ImpConc(GenNotSupported(L"start statement", stmt, isLast));
      }
      break;
    case TAG_TYPE_AS_StartListStmt:
      if (vdm_CPP_isJAVA() && get_conc_option())
        cpp = GenStartlistStmt(stmt, isLast);
      else {
        cpp.ImpConc(GenNotSupported(L"start list statement", stmt, isLast));
      }
      break;
    case TAG_TYPE_AS_StopStmt:
      if (vdm_CPP_isJAVA() && get_conc_option())
        cpp = GenStopStmt(stmt, isLast);
      else {
        cpp.ImpConc(GenNotSupported(L"stop statement", stmt, isLast));
      }
      break;
    case TAG_TYPE_AS_StopListStmt:
      if (vdm_CPP_isJAVA() && get_conc_option())
        cpp = GenStoplistStmt(stmt, isLast);
      else {
        cpp.ImpConc(GenNotSupported(L"stop list statement", stmt, isLast));
      }
      break;
#endif // VDMPP
#ifdef VICE
    case TAG_TYPE_AS_DurationStmt: {
      cpp.ImpConc(GenStmt(stmt.GetRecord(pos_AS_DurationStmt_stmt), isLast));
    };
    break;
    case TAG_TYPE_AS_CycleStmt: {
      cpp.ImpConc(GenStmt(stmt.GetRecord(pos_AS_CycleStmt_stmt), isLast));
    };
    break;
#endif //VICE
    default:
      ReportError(L"GenStmt");
      return SEQ<TYPE_CPP_Stmt>(); // To avoid warnings
  }

//wcout << L"end: " << stmt.GetTag () << endl << flush;

  if (rti) {

    // Add run-time position information

    SEQ<TYPE_CPP_Stmt> pos_l;
    pos_l.ImpConc(GenUpdatePosInfo(stmt));
    pos_l.ImpConc(cpp);

    if (!stmt.Is(TAG_TYPE_AS_ReturnStmt))
      pos_l.ImpAppend(GenPopPosInfo());

    return SEQ<TYPE_CPP_Stmt>().ImpAppend(vdm_BC_GenBlock(pos_l));
  }
  else
    return cpp;
}

// GenNotSupported
// msg : seq of char
// stmt : AS`Stmt
// isLast : bool 
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenNotSupported(wstring msg, const TYPE_AS_Stmt & stmt, bool isLast)
{
  if (isLast)
  {
    SEQ<TYPE_CPP_Stmt> result;
    result.ImpAppend(NotSupported(msg, stmt));
    result.ImpAppend(vdm_BC_GenReturnStmt(GenEmptyValue(GiveCurrentRType())));

    return SEQ<TYPE_CPP_Stmt>().ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenBoolLit(true), vdm_BC_GenBlock(result), nil));
  }
  else
  {
    return SEQ<TYPE_CPP_Stmt>().ImpAppend(NotSupported(msg, stmt));
  }
}

// CGAssignStmt
// stmt : AS`AssignStmt
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGAssignStmt(const TYPE_AS_AssignStmt & stmt)
{
  const TYPE_AS_StateDesignator & sd (stmt.GetRecord(pos_AS_AssignStmt_lhs));
  const TYPE_AS_Expr & expr (stmt.GetRecord(pos_AS_AssignStmt_rhs));

  Tuple gcas (GenCommonAsgnStmt(sd, expr));
  if (gcas.GetBoolValue(1)) {
    SEQ<TYPE_CPP_Stmt> stmt (gcas.GetSequence(2));
    return stmt;
  }

  TYPE_REP_TypeRep rhsType (FindType(expr));

  Tuple cgee (CGExprExcl(expr, ASTAUX::MkId(L"rhs"), rhsType));
  const TYPE_CPP_Expr & e_v (cgee.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & rhs_stmt (cgee.GetSequence(2));

  TYPE_REP_TypeRep gtp (FindType(sd));

  SEQ<TYPE_CPP_Stmt> rb (rhs_stmt);

  TYPE_CPP_Expr rhs_v (e_v);

  if (!IsSubType(rhsType, gtp))
  {
    rhs_v = vdm_BC_GiveName(ASTAUX::MkId(L"rhs"));
    rb.ImpAppend(vdm_BC_GenDecl(GenType(rhsType), rhs_v, vdm_BC_GenAsgnInit(e_v)));
  }

  switch (sd.GetTag()) {
    case TAG_TYPE_AS_Name: {
      // sd := expr
#ifdef VDMPP
      if (vdm_CPP_isJAVA()) {
        const TYPE_AS_Ids & ids (sd.GetSequence(pos_AS_Name_ids));
        if (ids.Length() > 1) {
          TYPE_CPP_Identifier cast (GenThis());
          TYPE_CPP_Expr expr (vdm_BC_GenObjectMemberAccess(cast,
                                                 vdm_BC_GenIdentifier( GiveLastName(sd))));
          rb.ImpAppend(vdm_BC_GenAsgnStmt(expr, GenExplicitCast(gtp, rhs_v, nil)));
        }
        else {
          TYPE_CPP_Expr cast;
          if (rhs_v.Is(TAG_TYPE_CPP_ClassInstanceCreationExpr))
          {
            if (IsSubType(rhsType, gtp))
              cast = rhs_v;
            else
              cast = GenExplicitCast(gtp, rhs_v, nil);
          }
          else if (rhs_v.Is(TAG_TYPE_CPP_ObjectMemberAccess) &&
                   (rhs_v.GetRecord(pos_CPP_ObjectMemberAccess_object) == GenBoolType().get_tp()) &&
                   ((rhs_v.GetRecord(pos_CPP_ObjectMemberAccess_name)
                                 == vdm_BC_GenIdentifier(ASTAUX::MkId(L"TRUE"))) ||
                    (rhs_v.GetRecord(pos_CPP_ObjectMemberAccess_name)
                                 == vdm_BC_GenIdentifier(ASTAUX::MkId(L"FALSE")))))
          {
            cast = rhs_v;
          }
          else if (rhs_v.Is(TAG_TYPE_CPP_FctCall) &&
                   rhs_v.GetRecord(pos_CPP_FctCall_fct) == vdm_BC_GenIdentifier(ASTAUX::MkId(L"Arrays.asList")))
          {
            cast = GenSeqExpr(rhs_v);
          }
          else
          {
            cast = GenExplicitCast(gtp,
                       vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId((L"UTIL.clone"))),
                                         mk_sequence(rhs_v)),
                                     nil);
          }
          if (!IsSubType(rhsType, gtp))
          {
            TYPE_CPP_Expr e (rhsType.Is(TAG_TYPE_REP_UnionTypeRep) ? rhs_v
                                 : vdm_BC_GenBracketedExpr(vdm_BC_GenCastExpr(vdm_BC_GenGeneric(),
                                                                              rhs_v)));
              rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenNot(GenAuxType(e, gtp)),
                             vdm_BC_GenBlock(mk_sequence(RunTime(L"Incompatible type"))), Nil()));
          }
          rb.ImpAppend(vdm_BC_GenAsgnStmt(vdm_BC_Rename(sd), cast));
        }
      }
      else
#endif // VDMPP
      { // C++
#ifdef VDMSL
        Generic tp (GetStatSem().LookUpStateEnv(sd));
        if (!(tp.Is(TAG_TYPE_REP_CompositeTypeRep) &&
             (UnqualiName(Record(tp).GetRecord(pos_REP_CompositeTypeRep_nm)) == sd)))
        {
          if (!IsSubType(rhsType, gtp))
          {
            rb.ImpAppend(vdm_BC_GenIfStmt(GenAuxType(rhs_v, gtp),
                          vdm_BC_GenBlock(mk_sequence(vdm_BC_GenAsgnStmt(vdm_BC_Rename(sd), rhs_v))),
                          vdm_BC_GenBlock(mk_sequence(RunTime(L"Incompatible type")))));
          }
          else
            rb.ImpAppend(vdm_BC_GenAsgnStmt(vdm_BC_Rename(sd), rhs_v));
        }
        else
        {
          if (tp.Is(TAG_TYPE_REP_CompositeTypeRep))
          {
            const SEQ<TYPE_REP_FieldRep> & fl (Record(tp).GetSequence(pos_REP_CompositeTypeRep_fields));
            size_t len_fl = fl.Length();
            for (size_t i = 1; i <= len_fl; i++)
            {
              const TYPE_REP_FieldRep & fr (fl[i]);
              rb.ImpAppend(vdm_BC_GenAsgnStmt(vdm_BC_Rename(fr.GetField(pos_REP_FieldRep_sel)),
                                              GenRecGetField(rhs_v,vdm_BC_GenIntegerLit(i),
                                                             fr.GetRecord(pos_REP_FieldRep_tp))));
            }
          }
          else
            rb.ImpAppend(vdm_BC_GenAsgnStmt(vdm_BC_Rename(sd), rhs_v));
        }
#endif // VDMSL
#ifdef VDMPP
        if (!IsSubType(rhsType, gtp))
        {
          rb.ImpAppend(vdm_BC_GenIfStmt(GenAuxType(rhs_v, gtp),
                         vdm_BC_GenBlock(mk_sequence(vdm_BC_GenAsgnStmt(vdm_BC_Rename(sd), rhs_v))),
                         vdm_BC_GenBlock(mk_sequence(RunTime(L"Incompatible type")))));
        }
        else
        {
          rb.ImpAppend(vdm_BC_GenAsgnStmt(vdm_BC_Rename(sd), rhs_v));
        }
#endif // VDMPP
      }
      break;
    }
    case TAG_TYPE_AS_NarrowRef: {
      // narrow_(sd1, T) := expr
      const TYPE_AS_StateDesignator & sd1 (sd.GetRecord(pos_AS_NarrowRef_var));
      TYPE_AS_AssignStmt as (stmt);
      as.SetField(pos_AS_AssignStmt_lhs, sd1);
      return CGAssignStmt(as);
    }
    case TAG_TYPE_AS_FieldRef: {
      // sd1.sel := expr
      const TYPE_AS_StateDesignator & sd1 (sd.GetRecord(pos_AS_FieldRef_var));
      if (sd1.Is(TAG_TYPE_AS_NarrowRef))
      {
        // narrow_(sd2, T).sel := expr
        TYPE_AS_StateDesignator sd2;
#ifdef VDMPP
        if (vdm_CPP_isJAVA()) {
          sd2 = sd1.GetRecord(pos_AS_NarrowRef_var);
        }
        else
#endif // VDMPP
        {
          sd2 = sd1.GetRecord(pos_AS_NarrowRef_var);
          TYPE_CI_ContextId cid (GetCI().PushCGType(FindType(sd1)));
          switch (sd2.GetTag()) {
            case TAG_TYPE_AS_Name:        { sd2.SetField(pos_AS_Name_cid,        cid); break; }
            case TAG_TYPE_AS_MapOrSeqRef: { sd2.SetField(pos_AS_MapOrSeqRef_cid, cid); break; }
            case TAG_TYPE_AS_FieldRef:    { sd2.SetField(pos_AS_FieldRef_cid,    cid); break; }
          }
        }
        TYPE_AS_FieldRef fr (sd);
        fr.SetField(pos_AS_FieldRef_var, sd2); // sd2.sel := expr
        rb.ImpConc(CGStateDesignator(fr, rhs_v));
      }
      else
        rb.ImpConc(CGStateDesignator(sd, rhs_v));
      break;
    }
    case TAG_TYPE_AS_MapOrSeqRef: {
      // sd1(arg) := expr
      const TYPE_AS_StateDesignator & sd1 (sd.GetRecord(pos_AS_MapOrSeqRef_var));
      if (sd1.Is(TAG_TYPE_AS_NarrowRef))
      {
        // narrow_(sd2, T)(arg) := expr
        const TYPE_AS_StateDesignator & sd2 (sd1.GetRecord(pos_AS_NarrowRef_var));
        TYPE_AS_MapOrSeqRef mor (sd);
        mor.SetField(pos_AS_MapOrSeqRef_var, sd2);
        rb.ImpConc(CGStateDesignator(mor, rhs_v));
      }
      else
        rb.ImpConc(CGStateDesignator(sd, rhs_v));
      break;
    }
  }
  POSITION::SetPosition(stmt);
  return rb;
}

// CGAtomicAssignStmt
// stmt : AS`AtomicAssignStmt
// isLast : bool
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGAtomicAssignStmt(const TYPE_AS_AtomicAssignStmt & stmt, bool isLast)
{
  const SEQ<TYPE_AS_AssignStmt> & stmts (stmt.GetSequence(pos_AS_AtomicAssignStmt_atm));
  size_t len_stmts = stmts.Length();
  SEQ<TYPE_AS_AssignStmt> as_l;
  Sequence value_l;
  for (size_t idx = 1; idx <= len_stmts; idx++)
  {
    TYPE_AS_AssignStmt as (stmts[idx]);
    const TYPE_AS_Expr & rhs (as.GetRecord(pos_AS_AssignStmt_rhs));
    TYPE_AS_Name tmpnm (ASTAUX::MkNameFromId(ASTAUX::MkId(L"atom_").ImpConc(StringNumber(idx)), ASTAUX::GetCid(rhs)));
    value_l.ImpAppend(mk_(TYPE_AS_PatternName().Init(tmpnm, Nil(), ASTAUX::GetCid(rhs)), rhs));
    as.SetField(pos_AS_AssignStmt_rhs, tmpnm);
    as_l.ImpAppend(as);
  }
  TYPE_AS_BlockStmt bs;
  bs.Init(SEQ<TYPE_AS_AssignDef>(), as_l, stmt.GetInt(pos_AS_AtomicAssignStmt_cid));
  TYPE_AS_DefStmt ds;
  ds.Init(value_l, bs, stmt.GetInt(pos_AS_AtomicAssignStmt_cid));
  // 
  SEQ<TYPE_CPP_Stmt> rb;
  SetAtomic();
  rb.ImpConc(CGDefStmt(ds, isLast));
  UnsetAtomic();
#ifdef VDMPP
  if (!GetConstr() && GetHasInv() && get_testpreandpost_option())
    rb.ImpAppend(GenInvCall(GiveCurCASName(), false));
#endif // VDMPP
  return rb;
}

// CGStateDesignator
// sd : AS`StateDesignator
// rhs_v : CPP`Expr
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGStateDesignator(const TYPE_AS_StateDesignator & sd, const TYPE_CPP_Expr & rhs_v)
{
  // sd.sel := rhs_v
  // sd(arg) := rhs_v
  TYPE_CPP_Identifier level_nm (vdm_BC_GiveName(ASTAUX::MkId(L"level")));

  switch (sd.GetTag()) {
    case TAG_TYPE_AS_FieldRef: {
      const TYPE_AS_StateDesignator & sd1 (sd.GetRecord(pos_AS_FieldRef_var));
      const TYPE_AS_Name & sel (sd.GetRecord(pos_AS_FieldRef_sel));

      SEQ<TYPE_CPP_Stmt> rb1, rb2;
      TYPE_REP_TypeRep type_of_level;
      if (sd1.Is(TAG_TYPE_AS_Name))
        type_of_level = FindType(sd1);
#ifdef VDMPP
      else if (sd1.Is(TAG_TYPE_AS_SelfExpr)) {
        type_of_level = FindType(sd1);
      }
#endif // VDMPP
      else {
        Tuple sdul (CGSDUpperLevel(sd1, level_nm)); // seq of CPP`Stmt * seq of CPP`Stmt * REP`TypeRep
        rb1.ImpConc(sdul.GetSequence(1));
        rb2.ImpConc(sdul.GetSequence(2));
        type_of_level = sdul.GetRecord(3);
      }

      TYPE_CGMAIN_VT level_cgvt (! sd1.Is(TAG_TYPE_AS_Name)
#ifdef VDMSL
                        ? mk_CG_VT(level_nm, type_of_level)
#endif // VDMSL
#ifdef VDMPP
                        ? (sd1.Is(TAG_TYPE_AS_SelfExpr) ? mk_CG_VT(GenThis(), type_of_level)
                          : mk_CG_VT(level_nm, type_of_level))
#endif // VDMPP
                        : mk_CG_VT(CGName(sd1), type_of_level));

      SEQ<TYPE_CPP_Stmt> recmodify (GenRecImpModify(sel, level_cgvt, rhs_v));

      SEQ<TYPE_CPP_Stmt> rb;
      rb.ImpConc(rb1);
      rb.ImpConc(recmodify);
      rb.ImpConc(rb2);
      return rb;
    }
    case TAG_TYPE_AS_MapOrSeqRef: {
      const TYPE_AS_StateDesignator & sd1 (sd.GetRecord(pos_AS_MapOrSeqRef_var));
      const TYPE_AS_Expr & e (sd.GetRecord(pos_AS_MapOrSeqRef_arg));

      TYPE_REP_TypeRep e_tp (FindType(e));

// 20150703 -->
      Generic etp = Nil();
      Generic ftp (FindType(sd1));
      if (!ftp.IsNil()) {
        if (IsMapType(ftp)) {
          etp = FindMapDomType(ftp);
        }
        else if (IsSeqType(ftp)) {
          etp = mk_REP_NumericTypeRep(Int(NATONE));
        }
      }

      //Tuple cgee (CGExprExcl(e, ASTAUX::MkId(L"e"), nil));
      Tuple cgee (CGExprExcl(e, ASTAUX::MkId(L"e"), etp));
// <-- 20150703
      const TYPE_CPP_Expr & e_v (cgee.GetRecord(1));
      const SEQ<TYPE_CPP_Stmt> & e_stmt (cgee.GetSequence(2));

      SEQ<TYPE_CPP_Stmt> rb1, rb2;
      Generic tp_of_level_nm = Nil();
      if (! sd1.Is(TAG_TYPE_AS_Name)) {
        Tuple sudl (CGSDUpperLevel(sd1, level_nm)); // seq of CPP`Stmt * seq of CPP`Stmt * REP`TypeRep
        rb1.ImpConc(sudl.GetSequence(1));
        rb2.ImpConc(sudl.GetSequence(2));
        tp_of_level_nm = sudl.GetRecord(3);
      }

      TYPE_CGMAIN_VT cg_vt (sd1.Is(TAG_TYPE_AS_Name) ? mk_CG_VT(CGName(sd1), FindType(sd1))
                                                     : mk_CG_VT(level_nm, tp_of_level_nm));

      SEQ<TYPE_CPP_Stmt> inner_stmt (e_stmt);
      inner_stmt.ImpAppend(GenSeqOrMapImpModify(cg_vt, e_v, e_tp, rhs_v));

      SEQ<TYPE_CPP_Stmt> rb (rb1);
      rb.ImpConc(inner_stmt);
      rb.ImpConc(rb2);
      return rb;
    }
    default:
      ReportError(L"CGStateDesignator");
  }
  return SEQ<TYPE_CPP_Stmt>(); //to avoid warnings.
}

// CGSDUpperLevel
// sd : AS`StateDesignator
// i_lvl_nm : CPP`Name
// ==> seq of CPP`Stmt * seq of CPP`Stmt * REP`TypeRep
Tuple vdmcg::CGSDUpperLevel(const TYPE_AS_StateDesignator & sd, const TYPE_CPP_Name & i_lvl_nm)
{
  TYPE_CPP_Identifier level_nm (vdm_BC_GiveName(ASTAUX::MkId(L"level")));

  switch (sd.GetTag()) {
    case TAG_TYPE_AS_MapOrSeqRef: {
      const TYPE_AS_StateDesignator & sd1 (sd.GetRecord(pos_AS_MapOrSeqRef_var));
      const TYPE_AS_Expr & e (sd.GetRecord(pos_AS_MapOrSeqRef_arg));
      if (sd1.Is(TAG_TYPE_AS_Name)) {
        return CGSDMapOrSeqRef_Name(sd1, e, i_lvl_nm);         // sd1(e) := i_lvl_nm
      }
      else {
        return CGSDMapOrSeqRef_SD(sd1, e, i_lvl_nm, level_nm); // level_nm(e) := i_lvl_nm
      }
      break;
    }
    case TAG_TYPE_AS_FieldRef: {
      const TYPE_AS_StateDesignator & sd1 (sd.GetRecord(pos_AS_FieldRef_var));
      const TYPE_AS_Name & sel (sd.GetRecord(pos_AS_FieldRef_sel));
      if (sd1.Is(TAG_TYPE_AS_Name))
        return CGSDFieldRef_Name(sd1, sel, i_lvl_nm);          // sd1.sel = i_lvl_nm
      else
        return CGSDFieldRef_SD(sd1, sel, i_lvl_nm, level_nm);  // level_nm.sel = i_lvl_nm
      break;
    }
    case TAG_TYPE_AS_NarrowRef: {
      //const TYPE_AS_StateDesignator & sd1 (sd.GetRecord(pos_AS_NarrowExpr_expr));
      TYPE_AS_StateDesignator sd1 (sd.GetRecord(pos_AS_NarrowRef_var));
#ifdef VDMPP
      if (vdm_CPP_isJAVA()) {
        return CGSDUpperLevel(sd1, i_lvl_nm);
      }
      else
#endif //VDMPP
      {
        TYPE_CI_ContextId cid (GetCI().PushCGType(FindType(sd)));
        switch (sd1.GetTag()) {
          case TAG_TYPE_AS_Name:        { sd1.SetField(pos_AS_Name_cid,        cid); break; }
          case TAG_TYPE_AS_MapOrSeqRef: { sd1.SetField(pos_AS_MapOrSeqRef_cid, cid); break; }
          case TAG_TYPE_AS_FieldRef:    { sd1.SetField(pos_AS_FieldRef_cid,    cid); break; }
        }
        return CGSDUpperLevel(sd1, i_lvl_nm);
      }
      break;
    }
    default:
      ReportError(L"CGSDUpperLevel");
  }
  return Tuple(); // To avoid warnings.
}

// CGSDMapOrSeqRef_Name
// sdl : AS`Name
// e : AS`Expr
// i_lvl_nm : CPP`Name
// ==> seq of CPP`Stmt * seq of CPP`Stmt * REP`TypeRep
Tuple vdmcg::CGSDMapOrSeqRef_Name(const TYPE_AS_Name & sd1, const TYPE_AS_Expr & e, const TYPE_CPP_Name & i_lvl_nm)
{
  // sd1(e) := i_lvl_nm
  // narrow(sd1, T)(e) := i_lvl_nm 
  TYPE_REP_TypeRep type (FindType(sd1));

  TYPE_REP_TypeRep tp_i_lvl_nm (posRngMapOrIndsSeqType(type));
  TYPE_CI_TokenSpan pos_sd1 (GetCI().GetPos(ASTAUX::GetCid(sd1)));
  TYPE_CI_ContextId cid_i_lvl_nm (GetCI().PushCGPosType(pos_sd1, tp_i_lvl_nm));

  TYPE_CPP_Expr name (vdm_BC_Rename(sd1));

  // TODO:if narrow cast here

  TYPE_AS_ApplyExpr ASapplyexpr;
  ASapplyexpr.Init(sd1, mk_sequence(e), cid_i_lvl_nm);

  TYPE_CGMAIN_VT cg_vt (mk_CG_VT(i_lvl_nm, tp_i_lvl_nm));
  Generic ae (CGApplyExpr(ASapplyexpr, cg_vt, nil, nil, true));

  SEQ<TYPE_CPP_Stmt> rb1;
  if (IsSeqOfCPPStmt(ae)) {
    rb1.ImpConc(GenDecl_DS(tp_i_lvl_nm, i_lvl_nm, nil));
    rb1.ImpConc(ae);
  }
  else
  {
    rb1.ImpConc(GenDeclInit_DS(tp_i_lvl_nm, i_lvl_nm, ae));
  }

  Tuple cgee (CGExprExcl(e, ASTAUX::MkId(L"e"), nil));
  const TYPE_CPP_Expr & e_v (cgee.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & e_stmt (cgee.GetSequence(2));

  TYPE_REP_TypeRep e_tp (FindType(e));

  TYPE_CGMAIN_VT cg_vt2 (mk_CG_VT(name, type));
  SEQ<TYPE_CPP_Stmt> rb2 (e_stmt);
  rb2.ImpAppend(GenSeqOrMapImpModify(cg_vt2, e_v, e_tp, i_lvl_nm)); // "name.ImpModify(e_v, i_lvl_nm))"

  return mk_(rb1, rb2, tp_i_lvl_nm);
}

// CGSDMapOrSeqRef_SD
// sdl : AS`StateDesignator
// e : AS`Expr
// i_lvl_nm : CPP`Name
// level_nm : CPP`Name
// ==> seq of CPP`Stmt * seq of CPP`Stmt * REP`TypeRep
Tuple vdmcg::CGSDMapOrSeqRef_SD(const TYPE_AS_StateDesignator & sd1,
                                const TYPE_AS_Expr & e,
                                const TYPE_CPP_Name & i_lvl_nm,
                                const TYPE_CPP_Name & level_nm)
{
// ==> seq of CPP`Stmt * seq of CPP`Stmt * REP`TypeRep
  Tuple sudl (CGSDUpperLevel(sd1, level_nm)); // seq of CPP`Stmt * seq of CPP`Stmt * REP`TypeRep
  SEQ<TYPE_CPP_Stmt> rb_up1 (sudl.GetSequence(1));
  const SEQ<TYPE_CPP_Stmt> & rb_up2 (sudl.GetSequence(2));
  const TYPE_REP_TypeRep & tp_level_nm (sudl.GetRecord(3));

  TYPE_CI_ContextId cid_tp_level_nm(GetCI().PushCGType(tp_level_nm));

  TYPE_REP_TypeRep tp_i_lvl_nm (posRngMapOrIndsSeqType(tp_level_nm));
  TYPE_CI_ContextId cid_tp_i_lvl_nm (GetCI().PushCGType(tp_i_lvl_nm));

  TYPE_AS_Name asname_dummy (ASTAUX::MkNameFromId(ASTAUX::MkId(L""), cid_tp_level_nm));

  TYPE_AS_ApplyExpr ASapplyexpr;
  ASapplyexpr.Init(asname_dummy, mk_sequence(e), cid_tp_i_lvl_nm);

  TYPE_CGMAIN_VT cg_vt (mk_CG_VT(i_lvl_nm, tp_i_lvl_nm));
  Generic ae (CGApplyExpr(ASapplyexpr, cg_vt, level_nm, tp_level_nm, true));

// ==> CPP`Expr * seq of CPP`Stmt
  Tuple cgee (CGExprExcl(e, ASTAUX::MkId(L"e"), nil));
  const TYPE_CPP_Expr & e_v (cgee.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & e_stmt (cgee.GetSequence(2));

  SEQ<TYPE_CPP_Stmt> rb1 (e_stmt);
  if (IsSeqOfCPPStmt(ae))
  {
    rb1.ImpConc(GenDecl_DS(tp_i_lvl_nm, i_lvl_nm, nil));
    rb1.ImpConc(ae);
  }
  else
  {
    rb1.ImpConc(GenDeclInit_DS(tp_i_lvl_nm, i_lvl_nm, ae));
  }

  TYPE_REP_TypeRep e_tp (FindType(e));

  TYPE_CGMAIN_VT cg_vt2 (mk_CG_VT(level_nm, tp_level_nm));

  rb_up1.ImpConc(rb1);

  SEQ<TYPE_CPP_Stmt> rb2;
  rb2.ImpAppend(GenSeqOrMapImpModify(cg_vt2, e_v, e_tp, i_lvl_nm)); // "level_nm.ImpModify(e_v, i_lvl_nm)"
  rb2.ImpConc(rb_up2);

  return mk_(rb_up1, rb2, tp_i_lvl_nm);
}

// CGSDFieldRef_Name
// sdl : AS`Name
// sel : AS`Expr
// i_lvl_nm : CPP`Name
// ==> seq of CPP`Stmt * seq of CPP`Stmt * REP`TypeRep
Tuple vdmcg::CGSDFieldRef_Name(const TYPE_AS_Name & sd1, const TYPE_AS_Expr & sel, const TYPE_CPP_Name & i_lvl_nm)
{
  // sd1.sel := i_lvl_nm
  // narrow_(sd1, T).sel := i_lvl_nm
  TYPE_CPP_Expr name (vdm_BC_Rename(sd1));
  TYPE_REP_TypeRep type (FindType(sd1));

  TYPE_REP_TypeRep tp_i_lvl_nm (posFieldApplyTypes(type, TYPE_AS_Name(sel)));
  TYPE_CI_ContextId cid_tp_i_lvl_nm (GetCI().PushCGType(tp_i_lvl_nm));

  TYPE_AS_FieldSelectExpr ASFieldSelExpr;
  ASFieldSelExpr.Init(sd1, sel, cid_tp_i_lvl_nm);

  TYPE_CGMAIN_VT cg_vt (mk_CG_VT(i_lvl_nm, tp_i_lvl_nm));
  SEQ<TYPE_CPP_Stmt> fse (CGFieldSelectExpr(ASFieldSelExpr, cg_vt, nil, nil));

  TYPE_CGMAIN_VT level_cgvt(mk_CG_VT(name, type));

  SEQ<TYPE_CPP_Stmt> l_decl_nil (GenDecl_DS(tp_i_lvl_nm, i_lvl_nm, nil));

  SEQ<TYPE_CPP_Stmt> rb1;
  rb1.ImpConc(l_decl_nil);
  rb1.ImpConc(fse);

  SEQ<TYPE_CPP_Stmt> rb2 (GenRecImpModify(sel, level_cgvt, i_lvl_nm));

  return mk_(rb1, rb2, tp_i_lvl_nm);
}

// CGSDFieldRef_SD
// sdl : AS`StateDesignator
// sel : AS`Expr
// i_lvl_nm : CPP`Name
// level_nm : CPP`Name
// ==> seq of CPP`Stmt * seq of CPP`Stmt * REP`TypeRep
Tuple vdmcg::CGSDFieldRef_SD(const TYPE_AS_StateDesignator & sd1,
                             const TYPE_AS_Expr & sel,
                             const TYPE_CPP_Name & i_lvl_nm,
                             const TYPE_CPP_Name & level_nm)
{
  Tuple sudl (CGSDUpperLevel(sd1, level_nm)); // seq of CPP`Stmt * seq of CPP`Stmt * REP`TypeRep
  SEQ<TYPE_CPP_Stmt> rb_upper1 (sudl.GetSequence(1));
  const SEQ<TYPE_CPP_Stmt> & rb_upper2 (sudl.GetSequence(2));
  const TYPE_REP_TypeRep & tp_level_nm (sudl.GetRecord(3));

  TYPE_CI_ContextId cid_tp_level_nm(GetCI().PushCGType(tp_level_nm));

  TYPE_REP_TypeRep tp_i_lvl_nm (posFieldApplyTypes(tp_level_nm, TYPE_AS_Name(sel)));
  TYPE_CI_ContextId cid_tp_i_lvl_nm(GetCI().PushCGType(tp_i_lvl_nm));

  TYPE_AS_Name as_nm_dummy (ASTAUX::MkNameFromId(ASTAUX::MkId(L""), cid_tp_level_nm));

  TYPE_AS_FieldSelectExpr ASFSelExpr;
  ASFSelExpr.Init(as_nm_dummy, sel, cid_tp_i_lvl_nm);

  TYPE_CGMAIN_VT cg_vt (mk_CG_VT(i_lvl_nm, tp_i_lvl_nm));
  SEQ<TYPE_CPP_Stmt> fse (CGFieldSelectExpr(ASFSelExpr, cg_vt, level_nm, tp_level_nm));

  SEQ<TYPE_CPP_Stmt> l_decl_nil (GenDecl_DS(tp_i_lvl_nm, i_lvl_nm, nil));

  SEQ<TYPE_CPP_Stmt> rb1;
  rb1.ImpConc(l_decl_nil);
  rb1.ImpConc(fse);

  rb_upper1.ImpConc(rb1);

  TYPE_CGMAIN_VT level_cgvt (mk_CG_VT(level_nm, tp_level_nm));

  SEQ<TYPE_CPP_Stmt> rb2;
  rb2.ImpConc(GenRecImpModify(sel, level_cgvt, i_lvl_nm));
  rb2.ImpConc(rb_upper2);

  return mk_(rb_upper1, rb2, tp_i_lvl_nm);
}

// GenRecImpModify
// fsnm : AS`Name
// vt : CGMAIN`VT
// val : CPP`Expr
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenRecImpModify(const TYPE_AS_Name & fsnm,
                                          const TYPE_CGMAIN_VT & vt,
                                          const TYPE_CPP_Expr & val)
{
  const TYPE_CPP_Expr & level         (vt.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & level_type_q (vt.GetRecord(pos_CGMAIN_VT_type));

  TYPE_REP_TypeRep level_type (RemoveNil(level_type_q));
  SET<TYPE_REP_CompositeTypeRep> rectypes (FindAllRecTypes(level_type));
  SET<TYPE_REP_CompositeTypeRep> posRecTypes(FindPossibleRecTypes(rectypes, fsnm));
#ifdef VDMPP
  SET<TYPE_REP_ObjRefTypeRep> oreftypes (FindAllObjRefTypes(level_type));
  SET<TYPE_REP_ObjRefTypeRep> posORefTypes (FindPossibleObjRefTypes(oreftypes, fsnm));
#endif //VDMPP

  SEQ<TYPE_CPP_Stmt> alt;
  if (level_type.Is(TAG_TYPE_REP_CompositeTypeRep) && (posRecTypes.Card() == 1)) {
    TYPE_REP_CompositeTypeRep ctr (posRecTypes.GetElem());
    const TYPE_AS_Name & rec_nm (ctr.GetRecord(pos_REP_CompositeTypeRep_nm));
#ifdef VDMPP
    if (vdm_CPP_isJAVA()) {
      TYPE_CPP_Expr rec (level.Is(TAG_TYPE_CPP_CastExpr) ? vdm_BC_GenBracketedExpr(level) : level);
      TYPE_CPP_Stmt setfield (vdm_BC_GenAsgnStmt(vdm_BC_GenQualifiedName(rec, vdm_BC_Rename2(fsnm)), val));
      alt.ImpAppend(setfield);
    }
    else
#endif // VDMPP
    {
      TYPE_CPP_Stmt setfield (GenRecSetField(level, vdm_BC_GenPosName(rec_nm, fsnm), val));
      alt.ImpAppend(setfield);
    }
  }
#ifdef VDMPP
  else if (level_type.Is(TAG_TYPE_REP_ObjRefTypeRep) && (posORefTypes.Card() == 1)) {
    TYPE_REP_ObjRefTypeRep otr (posORefTypes.GetElem());
    const TYPE_AS_Name & clnm (otr.GetRecord(pos_REP_ObjRefTypeRep_nm));
    if(vdm_CPP_isJAVA()) {
      TYPE_REP_TypeRep tp (GetStatSem().LookUpInstanceVar(ASTAUX::Combine2Names(clnm, fsnm)));
      TYPE_CPP_Expr rec (level.Is(TAG_TYPE_CPP_CastExpr) ? vdm_BC_GenBracketedExpr(level) : level);
      TYPE_CPP_Stmt l_asgn (vdm_BC_GenAsgnStmt(vdm_BC_GenQualifiedName(rec, vdm_BC_Rename(fsnm)),
                                                               GenExplicitCast(tp, val, nil)));
      alt.ImpAppend(l_asgn);
    }
    else { // C++
      TYPE_CPP_Expr l_ptr (CastToClassPtr(clnm, level));
      TYPE_CPP_Expr l_memacc (vdm_BC_GenPointerToObjectMemberAccess(l_ptr, vdm_BC_Rename(fsnm)));
      TYPE_CPP_Stmt l_asgn (vdm_BC_GenAsgnStmt(l_memacc, val));
      alt.ImpAppend(l_asgn);
    }
  }
#endif // VDMPP
  else {
#ifdef VDMSL
    if (!level_type.Is(TAG_TYPE_REP_UnionTypeRep) ||
        (level_type.GetSet(pos_REP_UnionTypeRep_tps) != posRecTypes))  {
      alt.ImpAppend(RunTime(L"A record was expected"));
    }
#endif // VDMSL
#ifdef VDMPP
    if (!level_type.Is(TAG_TYPE_REP_UnionTypeRep) ||
        (level_type.GetSet(pos_REP_UnionTypeRep_tps) != posRecTypes.Union(posORefTypes))) {
      alt.ImpAppend(RunTime(L"An object or record was expected"));
    }
#endif // VDMPP
 
    Generic t;
    for (bool bb = posRecTypes.First(t); bb; bb = posRecTypes.Next(t)) {
      TYPE_REP_CompositeTypeRep ctr (t);
      const TYPE_AS_Name & rec_nm (ctr.GetRecord(pos_REP_CompositeTypeRep_nm));

      TYPE_CPP_Stmt setfield;
#ifdef VDMPP
      if (vdm_CPP_isJAVA()) {
        TYPE_CPP_Expr rec (vdm_BC_GenBracketedExpr(GenCastType(ctr, level)));
        setfield = vdm_BC_GenAsgnStmt(vdm_BC_GenQualifiedName(rec, vdm_BC_Rename2(fsnm)), val);
      }
      else
#endif // VDMPP
      {
        TYPE_CPP_Expr rec (GenCastRecordTypeForModify(level, rec_nm));
        setfield = GenRecSetField(rec, vdm_BC_GenPosName(rec_nm, fsnm), val);
      }
      TYPE_CPP_Expr isRecord (GenIsThisRecord(ctr, level));
      if (alt.IsEmpty()) {
        alt.ImpAppend(setfield);
      }
      else {
        alt = SEQ<TYPE_CPP_Stmt>().ImpAppend(vdm_BC_GenIfStmt(isRecord,
                            vdm_BC_GenBlock(mk_sequence(setfield)), vdm_BC_GenBlock(alt)));
      }
    }
#ifdef VDMPP
    if (!posORefTypes.IsEmpty()) {
      Generic t;
      for (bool bb = posORefTypes.First(t); bb; bb = posORefTypes.Next(t)) {
        TYPE_REP_ObjRefTypeRep otr (t);
        const TYPE_AS_Name & clnm (otr.GetRecord(pos_REP_ObjRefTypeRep_nm));
        if(vdm_CPP_isJAVA()) {
          TYPE_CPP_Expr if_cond (vdm_BC_GenTypeComp(vdm_BC_Rename(clnm), level));
          TYPE_REP_TypeRep tp (GetStatSem().LookUpInstanceVar(ASTAUX::Combine2Names(clnm, fsnm)));
          TYPE_CPP_Expr cast (vdm_BC_GenBracketedExpr(GenCastType(otr, level)));
          TYPE_CPP_Stmt l_asgn (vdm_BC_GenAsgnStmt(vdm_BC_GenQualifiedName(cast, vdm_BC_Rename(fsnm)),
                                                   GenExplicitCast(tp, val, nil)));
          if (alt.IsEmpty()) {
            alt.ImpAppend(l_asgn);
          }
          else {
            alt = SEQ<TYPE_CPP_Stmt>().ImpAppend(vdm_BC_GenIfStmt(if_cond,
                              vdm_BC_GenBlock(mk_sequence(l_asgn)), vdm_BC_GenBlock(alt)));
          }
        }
        else { // C++
          TYPE_CPP_Expr if_cond (GenAuxType(level, otr));
          TYPE_CPP_Expr l_ptr (CastToClassPtr(clnm, level));
          TYPE_CPP_Expr l_memacc (vdm_BC_GenPointerToObjectMemberAccess(l_ptr, vdm_BC_Rename(fsnm)));
          TYPE_CPP_Stmt l_asgn (vdm_BC_GenAsgnStmt(l_memacc, val));
          if (alt.IsEmpty()) {
            alt.ImpAppend(l_asgn);
          }
          else {
            alt = SEQ<TYPE_CPP_Stmt>().ImpAppend(vdm_BC_GenIfStmt(if_cond,
                              vdm_BC_GenBlock(mk_sequence(l_asgn)), vdm_BC_GenBlock(alt)));
          }
        }
      }
    }
#endif // VDMPP
  }
  return alt;
}

// GenCommonAsgnStmt
// sd : AS`StateDesignator
// expr : AS`Expr
// ==>  bool * [seq of CPP`Stmt]
Tuple vdmcg::GenCommonAsgnStmt(const TYPE_AS_StateDesignator & sd, const TYPE_AS_Expr & expr)
{
  if (sd.Is(TAG_TYPE_AS_Name))
  {
    switch(expr.GetTag()) {
      case TAG_TYPE_AS_SeqModifyMapOverrideExpr: {
        return CGCommonMapOverrideExpr(sd, expr);
        break;
      }
      case TAG_TYPE_AS_BinaryExpr: {
        const TYPE_AS_Expr & e1 (expr.GetRecord(pos_AS_BinaryExpr_left));
        if ( e1 == sd )
        {
          int opr (expr.GetIntValue(pos_AS_BinaryExpr_opr));
          const TYPE_AS_Expr & e2 (expr.GetRecord(pos_AS_BinaryExpr_right));
          switch (opr) {
            case TAG_quote_SEQCONC:
              return CGCommonAsgnSeqConc(sd, e2);
            case TAG_quote_SETUNION:
              return CGCommonAsgnSetUnion(sd, e2);
            default:
              return mk_(Bool(false), nil);
          }
        }
        break;
      }
      case TAG_TYPE_AS_NilLit: {
#ifdef VDMPP
        if (vdm_CPP_isJAVA()) {
          return mk_(Bool(true), SEQ<TYPE_CPP_Stmt>().ImpAppend(vdm_BC_GenAsgnStmt(vdm_BC_Rename(sd), GenNilLit())));
        }
        else
#endif //VDMPP
        {
          return mk_(Bool(false), nil);
        }
        break;
      }
    }
  }
  return mk_(Bool(false), nil);
}

// CGCommonMapOverrideExpr
// sd : AS`StateDesignator
// rc1 : AS`SeqModifyMapOverrideExpr
// ==> bool * [seq of CPP`Stmt]
Tuple vdmcg::CGCommonMapOverrideExpr(const TYPE_AS_StateDesignator & sd,
                                     const TYPE_AS_SeqModifyMapOverrideExpr & rc1)
{
  const TYPE_AS_Expr & seqmap (rc1.GetRecord(pos_AS_SeqModifyMapOverrideExpr_seqmap));
  const TYPE_AS_Expr & map_exp (rc1.GetRecord(pos_AS_SeqModifyMapOverrideExpr_mapexp));

  TYPE_REP_TypeRep type (FindType(seqmap));

  if (seqmap.Is(TAG_TYPE_AS_Name) &&
      (sd.GetSequence(pos_AS_Name_ids) == seqmap.GetSequence(pos_AS_Name_ids)) &&
      IsMapType(type) &&
      map_exp.Is(TAG_TYPE_AS_MapEnumerationExpr) &&
      (map_exp.GetSequence(pos_AS_MapEnumerationExpr_els).Length() == 1))
  {
    TYPE_AS_Maplet ml (map_exp.GetSequence(pos_AS_MapEnumerationExpr_els).Hd());
    const TYPE_AS_Expr & md (ml.GetRecord(pos_AS_Maplet_mapdom));
    const TYPE_AS_Expr & mr (ml.GetRecord(pos_AS_Maplet_maprng));

    Tuple cgee (CGExprExcl(md, ASTAUX::MkId(L"md"), nil));
    const TYPE_CPP_Expr & md_v (cgee.GetRecord(1));
    const SEQ<TYPE_CPP_Stmt> & md_stmt(cgee.GetSequence(2));

    Tuple cgee2 (CGExprExcl(mr, ASTAUX::MkId(L"mr"), nil));
    const TYPE_CPP_Expr & mr_v (cgee2.GetRecord(1));
    const SEQ<TYPE_CPP_Stmt> & mr_stmt (cgee2.GetSequence(2));

    TYPE_CPP_Expr name (vdm_BC_Rename(sd));
    TYPE_CPP_Expr insert (GenMapImpModify(name, md_v, mr_v));
    if (md_stmt.IsEmpty() && mr_stmt.IsEmpty() )
      return mk_(Bool(true), SEQ<TYPE_CPP_Stmt>().ImpAppend(insert));
    else {
      SEQ<TYPE_CPP_Stmt> rb (md_stmt);
      rb.ImpConc(mr_stmt);
      rb.ImpAppend(insert);
      return mk_(Bool(true), rb);
    }
  }
  else
    return mk_(Bool(false), nil);
}

// CGCommonAsgnSeqConc
// sd : AS`StateDesignator
// e2 : AS`Expr
// ==> bool * [seq of CPP`Stmt]
Tuple vdmcg::CGCommonAsgnSeqConc(const TYPE_AS_StateDesignator & sd, const TYPE_AS_Expr & e2)
{
  if (e2.Is(TAG_TYPE_AS_SeqEnumerationExpr) && (e2.GetSequence(pos_AS_SeqEnumerationExpr_els).Length() == 1))
  {
    TYPE_AS_Expr e (e2.GetSequence(pos_AS_SeqEnumerationExpr_els).Hd());

    Tuple cgee (CGExprExcl(e, ASTAUX::MkId(L"e"), nil));
    const TYPE_CPP_Expr & e_v (cgee.GetRecord(1));
    const SEQ<TYPE_CPP_Stmt> & e_stmt (cgee.GetSequence(2));

    TYPE_REP_TypeRep type (FindType(sd));

    TYPE_CPP_Stmt impapp;
#ifdef VDMPP
    if (vdm_CPP_isJAVA()) {
      if (IsPossibleStringType(type))
      {
        TYPE_CPP_Expr name (IsStringType(type) ? vdm_BC_Rename(sd) : GenCastString(vdm_BC_Rename(sd)));
        impapp  = vdm_BC_GenAsgnStmt(vdm_BC_Rename(sd), vdm_BC_GenPlus(name, e_v));
      }
      else
      {
        TYPE_CPP_Expr name (GenCastSeq(vdm_BC_Rename(sd), type));
        impapp = GenImpAppend(name, e_v);
      }
    }
    else
#endif //VDMPP
    { // C++
      TYPE_CPP_Expr name (GenCastSeq(vdm_BC_Rename(sd), type));

      if (IsSeqType(type)) {
        impapp = GenImpAppend(name, e_v);
      }
      else {
        impapp = vdm_BC_GenAsgnStmt(vdm_BC_Rename(sd), GenImpAppendExpr(name, e_v));
      }
    }

    SEQ<TYPE_CPP_Stmt> rb (e_stmt);
    rb.ImpAppend(impapp);
    return mk_(Bool(true), rb);
  }
  else {
    return mk_(Bool(false), nil);
  }
}

// CGCommonAsgnSetUnion
// sd : AS`StateDesignator
// e : AS`Expr
// ==> bool * [seq of CPP`Stmt]
Tuple vdmcg::CGCommonAsgnSetUnion(const TYPE_AS_StateDesignator & sd, const TYPE_AS_Expr & e)
{
  if (e.Is(TAG_TYPE_AS_SetEnumerationExpr) && (e.GetSequence(pos_AS_SetEnumerationExpr_els).Length() == 1))
  {
    TYPE_AS_Expr elem (e.GetSequence(pos_AS_SetEnumerationExpr_els).Hd());

    Tuple cgee (CGExprExcl(elem, ASTAUX::MkId(L"elem"),nil));
    const TYPE_CPP_Expr & e_v (cgee.GetRecord(1));
    const SEQ<TYPE_CPP_Stmt> & e_stmt (cgee.GetSequence(2));

    TYPE_REP_TypeRep type (FindType(sd));
    bool casting (!IsSetType(type));
    TYPE_CPP_Expr name (casting ? GenCastSetType(vdm_BC_Rename(sd)) : vdm_BC_Rename(sd));
    TYPE_CPP_Stmt ins;
#ifdef VDMPP
    if (vdm_CPP_isJAVA()) {
      ins = GenSetInsert(name, e_v);
    }
    else
#endif //VDMPP
    {
      ins = (casting ? vdm_BC_GenAsgnStmt(vdm_BC_Rename(sd), GenSetInsertExpr(name, e_v))
                     : GenSetInsert(name, e_v));
    }

    SEQ<TYPE_CPP_Stmt> rb (e_stmt);
    rb.ImpAppend(ins);
    return mk_(Bool(true), rb);
  }
  else {
    return mk_(Bool(false), nil);
  }
}

// CGReturnStmt
// ret : AS`ReturnStmt
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGReturnStmt(const TYPE_AS_ReturnStmt & ret)
{
#ifdef VDMPP
  if (GetConstr()) {
    return SEQ<TYPE_CPP_Stmt>();
  }
#endif //VDMPP

  const Generic & val (ret.GetField(pos_AS_ReturnStmt_val));

  if ( val.IsNil() ) {
    SEQ<TYPE_CPP_Stmt> rb;
    if (rti) {
      rb.ImpAppend(GenPopFile());
      rb.ImpAppend(GenPopPosInfo());
    }
#ifdef VDMPP
    if (vdm_CPP_isJAVA()) {
      rb.ImpAppend(vdm_BC_GenReturnStmt(nil));
    }
    else
#endif //VDMPP
    { // C++
      if (IsInAlways()) {
        rb.ImpAppend(vdm_BC_GenExpressionStmt(vdm_BC_GenThrowExpression(vdm_BC_GenIntegerLit(0))));
      } else {
        rb.ImpAppend(vdm_BC_GenReturnStmt(nil));
      }
    }
    return rb;
  }
  else
  {
    Tuple cgee (CGExprExcl(val, ASTAUX::MkId(L"rexpr"), GiveCurrentRType()));
    const TYPE_CPP_Expr & rexpr_v (cgee.GetRecord(1));
    const SEQ<TYPE_CPP_Stmt> & val_stmt (cgee.GetSequence(2));

    SEQ<TYPE_CPP_Stmt> rb;
    rb.ImpConc(val_stmt);

    if (rti) {
      rb.ImpAppend(GenPopFile());
      rb.ImpAppend(GenPopPosInfo());
    }

#ifdef VDMPP
    if (vdm_CPP_isJAVA()) {
      TYPE_REP_TypeRep rtp (RemoveNil(RemoveInvType(GiveCurrentRType())));
      TYPE_CPP_Expr cast;
      if (IsStringType(rtp) &&
          rexpr_v.Is(TAG_TYPE_CPP_ClassInstanceCreationExpr) &&
          (rexpr_v.GetRecord(pos_CPP_ClassInstanceCreationExpr_classtype) == GenStringType().get_tp()))
      {
        cast = rexpr_v.GetSequence(pos_CPP_ClassInstanceCreationExpr_arg)[1];
      }
      else
        cast = rexpr_v;

      rb.ImpAppend(vdm_BC_GenReturnStmt(cast));
      return rb;
    }
    else
#endif //VDMPP
    { // C++
      if (IsInAlways())
        rb.ImpAppend(vdm_BC_GenExpressionStmt(vdm_BC_GenThrowExpression(rexpr_v)));
      else
      {
        rb.ImpAppend(vdm_BC_GenReturnStmt(rexpr_v));
      }
      return rb;
    }
  }
}

bool vdmcg::IsReturnStmtAtLast (const TYPE_CPP_Stmt & stmt)
{
  switch (stmt.GetTag()) {
    case TAG_TYPE_CPP_Return: {
      return true;
    }
    case TAG_TYPE_CPP_CompoundStmt: {
      SEQ<TYPE_CPP_Stmt> stmts (stmt.GetSequence(pos_CPP_CompoundStmt_stms));
      if (stmts.IsEmpty())
        return false;
      else
        return IsReturnStmtAtLast(stmts[stmts.Length()]);
    }
    case TAG_TYPE_CPP_IfStmt: {
      if (stmt.GetField(pos_CPP_IfStmt_alt2).IsNil())
        return false;
      else
        return IsReturnStmtAtLast(stmt.GetRecord(pos_CPP_IfStmt_alt1)) &&
               IsReturnStmtAtLast(stmt.GetRecord(pos_CPP_IfStmt_alt2));
    }
    default: {
      return false;
    }
  }
}

// CGNonDetStmt
// stmt : AS`NonDetStmt
// isLast : bool
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGNonDetStmt(const TYPE_AS_NonDetStmt & stmt, bool isLast)
{
  const SEQ<TYPE_AS_Stmt> & stmt_l (stmt.GetSequence(pos_AS_NonDetStmt_stmts));
  TYPE_CPP_Expr e (vdm_BC_GenIntegerLit(stmt_l.Length()));
  TYPE_CPP_Identifier numSeq (vdm_BC_GiveName(ASTAUX::MkId(L"numSeq")));
  TYPE_CPP_Identifier i_v (vdm_BC_GiveName(ASTAUX::MkId(L"i")));
  TYPE_CPP_Expr max_v (GenLen_int(numSeq));
  TYPE_CPP_Expr fc;
  TYPE_CPP_AsgnInit ini_v;
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    fc = vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"UTIL.GetRandomSequence")), mk_sequence(e));
    ini_v = vdm_BC_GenAsgnInit(vdm_BC_GenIntegerLit (0));
  }
  else
#endif // VDMPP
  {
    fc = vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"CGUTIL::GetRandomSequence")),
                               mk_sequence(e));
    ini_v = vdm_BC_GenAsgnInit(vdm_BC_GenIntegerLit (1));
  }
  SEQ<TYPE_CPP_CaseStmt> case_l;
  size_t len_stmt_l = stmt_l.Length();
  for (size_t idx = 1; idx <= len_stmt_l; idx++) {
    SEQ<TYPE_CPP_Stmt> stmts (GenStmt(stmt_l[idx], false));
    if (stmts.IsEmpty() || !IsReturnStmtAtLast(stmts[stmts.Length()]))
      stmts.ImpAppend(vdm_BC_GenBreakStmt(Nil()));
    case_l.ImpAppend(vdm_BC_GenCaseStmt(vdm_BC_GenIntegerLit (idx), vdm_BC_GenBlock(stmts)));
  }
  TYPE_CPP_Expr seqApply;
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    seqApply = vdm_BC_GenFctCallObjMemAcc(numSeq, ASTAUX::MkId(L"get"), mk_sequence(i_v));
  }
  else
#endif // VDMPP
  {
    seqApply = vdm_BC_GenArrayApply(numSeq, i_v);
  }
  SEQ<TYPE_CPP_Stmt> trb;
  TYPE_CPP_Expr cast (vdm_BC_GenCastExpr(GenIntType(), seqApply));
  TYPE_CPP_Expr val;
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    val = vdm_BC_GenFctCallObjMemAcc(cast, ASTAUX::MkId(L"intValue"), SEQ<TYPE_CPP_Expr>());
  }
  else
#endif // VDMPP
  {
    val = GenGetValue(cast, mk_REP_NumericTypeRep(Int(INTEGER)));
  }
  trb.ImpAppend(vdm_BC_GenSwitchStmt(val, case_l));

  TYPE_CPP_Stmt inner (vdm_BC_GenBlock(trb));
  TYPE_CPP_Expr cond;
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    cond = vdm_BC_GenLt (i_v, max_v);
  }
  else
#endif // VDMPP
  {
    cond = vdm_BC_GenLeq (i_v, max_v);
  }

  SEQ<TYPE_CPP_Stmt> rb_l;
  rb_l.ImpAppend(vdm_BC_GenDecl(GenSeq0Type(), numSeq, vdm_BC_GenAsgnInit(fc)));
  rb_l.ImpAppend(vdm_BC_GenForStmt (vdm_BC_GenDecl (GenSmallIntType (), i_v, ini_v), cond,
                                                    mk_sequence(vdm_BC_GenPostPlusPlus(i_v)), inner));
  if (isLast && !GiveCurrentRType().Is(TAG_TYPE_REP_UnitTypeRep)) {
    rb_l.ImpAppend(vdm_BC_GenReturnStmt(GenEmptyValue(GiveCurrentRType())));
  }
  return rb_l;
}

#ifdef VDMSL
// CGCallStmt
// stmt : AS`CallStmt
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGCallStmt(const TYPE_AS_CallStmt & stmt)
{
  const TYPE_AS_Name & nm (stmt.GetRecord(pos_AS_CallStmt_oprt));
  const SEQ<TYPE_AS_Expr> & arg_l (stmt.GetSequence(pos_AS_CallStmt_args));
  const TYPE_CI_ContextId & cid (stmt.GetInt(pos_AS_CallStmt_cid));

  Generic nmtp (FindType (nm));
  Sequence fndom;       // seq of REP`RepType
  if (nmtp.IsRecord()) {
    TYPE_REP_TypeRep etp (nmtp);
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

  TYPE_CPP_TypeSpecifier gtype (vdm_BC_GenGeneric());
  SEQ<TYPE_CPP_Stmt> rbtmp_l;
  SEQ<TYPE_CPP_Expr> var_l;
  size_t len_arg_l = arg_l.Length();
  for (size_t idx = 1; idx <= len_arg_l; idx++) {
    Tuple cgee (CGExprExcl(arg_l[idx], ASTAUX::MkId(L"tmpArg_v"), nil));
    const TYPE_CPP_Expr & tmpArg_v (cgee.GetRecord(1));
    const SEQ<TYPE_CPP_Stmt> & arg_stmt (cgee.GetSequence(2));

    Generic lti (FindType(arg_l[idx]));
    if (IsSubType(lti, fndom[idx])) {
      var_l.ImpAppend(tmpArg_v);
      rbtmp_l.ImpConc(arg_stmt);
    }
    else if (GenType(lti) == gtype) {
      var_l.ImpAppend(tmpArg_v);
      rbtmp_l.ImpConc(arg_stmt);
    }
    else {
      var_l.ImpAppend(vdm_BC_GenCastExpr(gtype, tmpArg_v));
      rbtmp_l.ImpConc(arg_stmt);
    }
  }

  TYPE_CPP_Expr fcall (vdm_BC_GenFctCall(vdm_BC_Rename(nm), var_l));
  SEQ<TYPE_CPP_Stmt> rb_l (rbtmp_l);
  TYPE_REP_TypeRep ti (GetCI().GetTypeInfo(cid));

  if (rti) {
    rb_l.ImpAppend(GenPopPosInfo());
    rb_l.ImpAppend(GenPopFile());
  }

  if (ti.Is(TAG_TYPE_REP_UnitTypeRep)) {
    rb_l.ImpAppend(vdm_BC_GenExpressionStmt(fcall));
  }
  else {
    if (GetStatSem().IsOverlapping(ti, GiveCurrentRType(), Set())) {
      rb_l.ImpAppend(vdm_BC_GenReturnStmt(fcall));
    }
    else if (GenCppTpDecl(ti, Set()) == GenCppTpDecl(GiveCurrentRType(), Set())) {
      rb_l.ImpAppend(vdm_BC_GenReturnStmt(fcall));
    }
    else {
      rb_l.ImpAppend(vdm_BC_GenGenericReturnStmt(fcall));
    }
  }
  return rb_l;
}
#endif // VDMSL

#ifdef VDMPP
// CGCallStmt
// stmt : AS`CallStmt
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGCallStmt (const TYPE_AS_CallStmt & stmt)
{
  const Generic & obj             (stmt.GetField   (pos_AS_CallStmt_obj));
  const TYPE_AS_Name & nm         (stmt.GetRecord  (pos_AS_CallStmt_oprt));
  const SEQ<TYPE_AS_Expr> & arg_l (stmt.GetSequence(pos_AS_CallStmt_args));
  const TYPE_CI_ContextId & cid   (stmt.GetInt     (pos_AS_CallStmt_cid));

  TYPE_REP_TypeRep rettp (GiveCurrentRType());
  TYPE_REP_TypeRep nmtp (FindType (nm));
  TYPE_REP_TypeRep ti (GetCI().GetTypeInfo (cid));

  if (obj.IsNil ()) {
    Generic fntype = nil;  // [REP`TypeRep]
    Sequence fndom;        // seq of REP`TypeRep
    switch (nmtp.GetTag ()) {
      case TAG_TYPE_REP_PartialFnTypeRep: {
        fntype = nmtp.GetRecord(pos_REP_PartialFnTypeRep_fnrng);
        fndom = nmtp.GetSequence(pos_REP_PartialFnTypeRep_fndom);
        break;
      }
      case TAG_TYPE_REP_TotalFnTypeRep: {
        fntype = nmtp.GetRecord(pos_REP_TotalFnTypeRep_fnrng);
        fndom = nmtp.GetSequence(pos_REP_TotalFnTypeRep_fndom);
        break;
      }
      case TAG_TYPE_REP_OpTypeRep: {
        fntype = nmtp.GetRecord(pos_REP_OpTypeRep_Rng);
        fndom = nmtp.GetSequence(pos_REP_OpTypeRep_Dom);
        break;
      }
      default:
        fntype= nil;
    }

    SEQ<TYPE_CPP_Stmt> rb_l;
    SEQ<TYPE_CPP_Expr> var_l;
    size_t len_arg_l = arg_l.Length();
    for (size_t idx = 1; idx <= len_arg_l; idx++) {
      const TYPE_AS_Expr & arg (arg_l[idx]);
      Tuple cgee (CGExprExcl(arg, ASTAUX::MkId(L"tmpArg_v"), nil));
      const TYPE_CPP_Expr & tmpArg_v (cgee.GetRecord(1));
      const SEQ<TYPE_CPP_Stmt> & arg_stmt (cgee.GetSequence(2));

      if (vdm_CPP_isJAVA()) {
        switch (nmtp.GetTag ()) {
          case TAG_TYPE_REP_PartialFnTypeRep:
          case TAG_TYPE_REP_TotalFnTypeRep:
          case TAG_TYPE_REP_OpTypeRep: {
            TYPE_REP_TypeRep tp1 (fndom[idx]);
            TYPE_REP_TypeRep tp2 (FindType(arg));
            rb_l.ImpConc(arg_stmt);
            var_l.ImpAppend(GenExplicitCast(tp1, tmpArg_v, tp2));
            break;
          }
          default: {
            rb_l.ImpConc(arg_stmt);
            var_l.ImpAppend(tmpArg_v);
            break;
          }
        }
      } // end of java
      else { // C++
        var_l.ImpAppend(tmpArg_v);
        rb_l.ImpConc(arg_stmt);
      }
    } // end of for loop

    if (rti) {
      rb_l.ImpAppend(GenPopPosInfo());
      rb_l.ImpAppend(GenPopFile());
    }

    if (vdm_CPP_isJAVA()) {
      TYPE_CPP_Expr nmobj;
      if (nm.GetSequence(pos_AS_Name_ids).Length() > 1) {
        TYPE_AS_Name l_clnm (GiveFirstName(nm));
        TYPE_AS_Name l_mnm (GiveSecondName(nm));

        TYPE_AS_Ids ids (nm.GetSequence(pos_AS_Name_ids));
        if (ids.Hd() == GiveCurCName()) {
          nmobj = vdm_BC_Rename(l_mnm);
        }
        else if (GetOrderedSupers(GiveCurCASName()).Elems().InSet(l_clnm) && !isInterface(l_clnm)) {
          nmobj = vdm_BC_GenQualifiedName(GenSuper(), vdm_BC_Rename(l_mnm));
        }
        else {
          nmobj = vdm_BC_GenQualifiedName(vdm_BC_Rename(l_clnm), vdm_BC_Rename(l_mnm));
        }
      }
      else {
        if (IsClass(nm)) {
          if (nm == GiveCurCASName()) {
            nmobj = GenThis();
          }
          else {
            nmobj = GenSuper();
          }
        }
        else {
          nmobj = vdm_BC_Rename(nm);
        }
      }

      TYPE_CPP_Expr fc (vdm_BC_GenFctCall(nmobj, var_l));
      if (ti.Is(TAG_TYPE_REP_UnitTypeRep)) {
        rb_l.ImpAppend(vdm_BC_GenExpressionStmt(fc));
      }
      else {
        if (IsClass(nm)) {
          rb_l.ImpAppend(vdm_BC_GenExpressionStmt(fc));
        }
        else {
          rb_l.ImpAppend(vdm_BC_GenReturnStmt(GenExplicitCast(rettp, fc, fntype)));
        }
      }
      return rb_l;
    }
    else { // C++
      if (nm.GetSequence(pos_AS_Name_ids).Length() > 1) {
        TYPE_AS_Name clNm (GiveFirstName(nm));
        IncludeClass(clNm);
      }

      size_t len_fndom = fndom.Length();
      for (size_t idx = 1; idx <= len_fndom; idx++) {
        const TYPE_REP_TypeRep & tp(fndom[idx]);
        if (tp.Is(TAG_TYPE_REP_ObjRefTypeRep))
          IncludeClass(tp.GetRecord(pos_REP_ObjRefTypeRep_nm));
      }

      TYPE_CPP_Expr nmobj;
      if (nm.GetSequence(pos_AS_Name_ids).Length() > 1) {
        TYPE_AS_Name nm1 (GiveFirstName(nm));
        TYPE_AS_Name nm2 (GiveSecondName(nm));
        nmobj = vdm_BC_GenQualifiedName(vdm_BC_Rename(nm1), vdm_BC_Rename(nm2));
      }
      else {
        nmobj = vdm_BC_Rename(nm);
      }
      TYPE_CPP_Expr fc (vdm_BC_GenFctCall(nmobj, var_l));

      if (ti.Is(TAG_TYPE_REP_UnitTypeRep)) {
        rb_l.ImpAppend(vdm_BC_GenExpressionStmt(fc));
      }
      else {
        if (GetStatSem().IsOverlapping(ti, rettp, Set())) {
          rb_l.ImpAppend(vdm_BC_GenReturnStmt(fc));
        }
        else {
          rb_l.ImpAppend(vdm_BC_GenGenericReturnStmt(fc));
        }
      }
      return rb_l;
    }
  } // end of obj.IsNil()
  else { // !obj.IsNil()
    TYPE_CPP_Identifier objnm (vdm_BC_GiveName (ASTAUX::MkId(L"objnm")));
    TYPE_CI_TokenSpan pos (GetCI().GetPos (nm.GetInt(pos_AS_Name_cid)));
    TYPE_CI_ContextId ap_cid (GetCI().PushCGPosType (pos, nmtp));

    TYPE_AS_FieldSelectExpr fse;
    fse.Init(obj, nm, ap_cid);

    TYPE_AS_ApplyExpr applexpr;
    applexpr.Init(fse, arg_l, cid);

    if (ti.Is (TAG_TYPE_REP_UnitTypeRep)) {
      TYPE_CGMAIN_VT apptp (mk_CG_VT (objnm, ti));
      Generic cgexpr (CGExpr_q (applexpr, apptp));
      if (IsSeqOfCPPStmt (cgexpr)) {
        return cgexpr;
      }
      else {
        return SEQ<TYPE_CPP_Stmt>().ImpAppend(vdm_BC_GenExpressionStmt (cgexpr));
      }
    }
    else {
      Tuple cgres (CGExprExcl (applexpr, ASTAUX::MkId(L"tmpObj"), FindType(applexpr)));
      const TYPE_CPP_Expr & tmpobj (cgres.GetRecord (1));
      SEQ<TYPE_CPP_Stmt> cgexpr (cgres.GetSequence (2));

      if (vdm_CPP_isCPP()) {
        if (GetStatSem().IsOverlapping(ti, rettp, Set())) {
          cgexpr.ImpAppend (vdm_BC_GenReturnStmt (tmpobj));
        }
        else {
          cgexpr.ImpAppend (vdm_BC_GenGenericReturnStmt (tmpobj));
        }
      }
      else {
        cgexpr.ImpAppend (vdm_BC_GenReturnStmt (GenExplicitCast(rettp, tmpobj, FindType(applexpr))));
      }
      return cgexpr;
    }
  }
}
#endif // VDMPP

// CGBlockStmt
// block : AS`BlockStmt
// isLast : bool
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGBlockStmt(const TYPE_AS_BlockStmt & block, bool isLast)
{
  const SEQ<TYPE_AS_AssignDef> & dcls (block.GetSequence(pos_AS_BlockStmt_dcls));
  const SEQ<TYPE_AS_Stmt> & stmts (block.GetSequence(pos_AS_BlockStmt_stmts));

  PushEnv_CGAUX();

  SEQ<TYPE_CPP_Stmt> rb;
  size_t len_dcls = dcls.Length();
  for (size_t idx = 1; idx <= len_dcls; idx++)
  {
    const TYPE_AS_AssignDef & ad (dcls[idx]);
    const TYPE_AS_Name & var (ad.GetRecord(pos_AS_AssignDef_var));
    const TYPE_AS_Type & tp (ad.GetRecord(pos_AS_AssignDef_tp));
    const Generic & dclinit (ad.GetField(pos_AS_AssignDef_dclinit));

    InsertName_CGAUX(var);
    TYPE_CPP_Expr varname (vdm_BC_Rename(var));

    TYPE_REP_TypeRep type (RemoveNil(FromAS2RepType(tp)));

    SEQ<TYPE_CPP_Stmt> var_decl;
    var_decl.ImpConc(GenDecl_DS(type, varname, nil));

    if (dclinit.IsNil())
    {
      rb.ImpConc(var_decl);
    }
    else
    {
      TYPE_REP_TypeRep dclinit_type (FindType(dclinit));
      if (IsSubType(dclinit_type, type)
          && !(vdm_CPP_isJAVA() && IsStringType(dclinit_type) && !IsStringType(type)))
      {
        Tuple cgee (CGExprExcl(dclinit, varname, type));
        const TYPE_CPP_Expr & var_v (cgee.GetRecord(1));
        const SEQ<TYPE_CPP_Stmt> & var_stmt (cgee.GetSequence(2));

        if (var_stmt.IsEmpty())
        {
#ifdef VDMPP
          if (vdm_CPP_isJAVA()) {
            rb.ImpConc(GenDeclInit_DS(type, varname, var_v));
          }
          else
#endif // VDMPP
          {
            Generic init_v = vdm_BC_GenAsgnInit(var_v);
            switch (Record(dclinit).GetTag()) {
              case TAG_TYPE_AS_NilLit: {
                init_v = vdm_BC_GenAsgnInit(var_v);
                break;
              }
              case TAG_TYPE_AS_SeqEnumerationExpr: {
                if (IsSeqType(type) && Record(dclinit).GetSequence(pos_AS_SeqEnumerationExpr_els).IsEmpty())
                  init_v = Nil();
                break;
              }
              case TAG_TYPE_AS_SetEnumerationExpr: {
                if (IsSetType(type) && Record(dclinit).GetSequence(pos_AS_SetEnumerationExpr_els).IsEmpty())
                  init_v = Nil();
                break;
              }
              case TAG_TYPE_AS_MapEnumerationExpr: {
                if (IsMapType(type) && Record(dclinit).GetSequence(pos_AS_MapEnumerationExpr_els).IsEmpty())
                  init_v = Nil();
                break;
              }
            }
            rb.ImpConc(GenDecl_DS(type, varname, init_v));
          }
        }
        else {
           if ((var_stmt.Length() == 1) &&
               var_stmt[1].Is(TAG_TYPE_CPP_ExpressionStmt) &&
               !var_stmt[1].GetField(pos_CPP_ExpressionStmt_expr).IsNil() &&
               var_stmt[1].GetField(pos_CPP_ExpressionStmt_expr).Is(TAG_TYPE_CPP_AssignExpr) &&
               (var_stmt[1].GetRecord(pos_CPP_ExpressionStmt_expr).GetRecord(pos_CPP_AssignExpr_unary) == varname) &&
               (var_stmt[1].GetRecord(pos_CPP_ExpressionStmt_expr).GetRecord(pos_CPP_AssignExpr_assignop) 
                                .GetField(pos_CPP_AssignOp_op) == Quote(L"ASEQUAL")))
           {
             TYPE_CPP_Expr v (var_stmt[1].GetRecord(pos_CPP_ExpressionStmt_expr)
                                         .GetRecord(pos_CPP_AssignExpr_assignexpr));
             rb.ImpConc(GenDeclInit_DS(type, varname, v));
           }
           else
           {
             rb.ImpConc(var_decl);
             rb.ImpConc(var_stmt);
           }
        }
      }
      else
      {
#ifdef VDMPP
        if (vdm_CPP_isJAVA()) {
          Tuple cgee (CGExprExcl(dclinit, varname, type));
          const TYPE_CPP_Expr & var_v (cgee.GetRecord(1));
          const SEQ<TYPE_CPP_Stmt> & var_stmt (cgee.GetSequence(2));

          if (var_stmt.IsEmpty())
          {
            if (var_v.Is(TAG_TYPE_CPP_ClassInstanceCreationExpr))
            {
              if (IsSubType(FindType(dclinit), type))
                rb.ImpConc(GenDeclInit_DS(type, varname, var_v));

              else if (IsStringType(type) &&
                       (var_v.GetRecord(pos_CPP_ClassInstanceCreationExpr_classtype) == GenStringType().get_tp()))
                rb.ImpConc(GenDeclInit_DS(type, varname, var_v));
              else
                rb.ImpConc(GenDeclInit_DS(type, varname, GenExplicitCast(type, var_v, FindType(dclinit))));
            }
            else
              rb.ImpConc(GenDeclInit_DS(type, varname, GenExplicitCast(type, var_v, FindType(dclinit))));
          }
          else {
            if ((var_stmt.Length() == 1) &&
                var_stmt[1].Is(TAG_TYPE_CPP_ExpressionStmt) &&
                !var_stmt[1].GetField(pos_CPP_ExpressionStmt_expr).IsNil() &&
                var_stmt[1].GetField(pos_CPP_ExpressionStmt_expr).Is(TAG_TYPE_CPP_AssignExpr) &&
                (var_stmt[1].GetRecord(pos_CPP_ExpressionStmt_expr).GetRecord(pos_CPP_AssignExpr_unary) == varname) &&
                (var_stmt[1].GetRecord(pos_CPP_ExpressionStmt_expr).GetRecord(pos_CPP_AssignExpr_assignop) 
                                 .GetField(pos_CPP_AssignOp_op) == Quote(L"ASEQUAL")))
            {
              TYPE_CPP_Expr v (var_stmt[1].GetRecord(pos_CPP_ExpressionStmt_expr)
                                          .GetRecord(pos_CPP_AssignExpr_assignexpr));
              rb.ImpConc(GenDeclInit_DS(type, varname, v));
            }
            else
            {
              rb.ImpConc(var_decl);
              rb.ImpConc(var_stmt);
            }
          }
        }
        else
#endif // VDMPP
        { // C++
          Tuple cgee (CGExprExcl(dclinit, ASTAUX::MkId(L"varname"), type));
          const TYPE_CPP_Expr & var_v (cgee.GetRecord(1));
          const SEQ<TYPE_CPP_Stmt> & var_stmt (cgee.GetSequence(2));
          Generic init_v = vdm_BC_GenAsgnInit(var_v);
          switch (Record(dclinit).GetTag()) {
            case TAG_TYPE_AS_SeqEnumerationExpr: {
              if (IsSeqType(type) && Record(dclinit).GetSequence(pos_AS_SeqEnumerationExpr_els).IsEmpty())
                init_v = Nil();
              break;
            }
            case TAG_TYPE_AS_SetEnumerationExpr: {
              if (IsSetType(type) && Record(dclinit).GetSequence(pos_AS_SetEnumerationExpr_els).IsEmpty())
                init_v = Nil();
              break;
            }
            case TAG_TYPE_AS_MapEnumerationExpr: {
              if (IsMapType(type) && Record(dclinit).GetSequence(pos_AS_MapEnumerationExpr_els).IsEmpty())
                init_v = Nil();
              break;
            }
          }
          SEQ<TYPE_CPP_Stmt> var_decl (GenDecl_DS(type, varname, init_v));
          rb.ImpConc(var_stmt);
          rb.ImpConc(var_decl);
        }
      }
    }
  }

  PushEnv_CGAUX(); // 20130305

  int numStmts = stmts.Length();
  for (int i = 1; i <= numStmts; i++)
    rb.ImpConc(GenStmt(stmts[i], isLast && (i == numStmts)));

  PopEnv_CGAUX(); // 20130305
  PopEnv_CGAUX();

// 20120521 -->
  if (dcls.IsEmpty())
    return rb;
// <-- 20120521
  return SEQ<TYPE_CPP_Stmt>().ImpAppend(vdm_BC_GenBlock(rb));
}

// CGLetStmt
// ls : AS`LetStmt
// isLast : bool
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGLetStmt(const TYPE_AS_LetStmt & ls, bool isLast)
{
  const SEQ<TYPE_AS_LocalDef> & ld_l (ls.GetSequence(pos_AS_LetStmt_localdef));
  const TYPE_AS_Stmt & In (ls.GetRecord(pos_AS_LetStmt_In));

  PushEnv_CGAUX();

  SEQ<TYPE_AS_ValueDef> lvd_l;
  SEQ<TYPE_AS_FnDef> lfd_l;

  PushEnv();

  size_t len_ld_l = ld_l.Length();
  for (size_t idx = 1; idx <= len_ld_l; idx++)
  {
    const TYPE_AS_LocalDef & ld (ld_l[idx]);
    switch(ld.GetTag()) {
      case TAG_TYPE_AS_ValueDef: {
        lvd_l.ImpAppend(ld);
        break;
      }
      case TAG_TYPE_AS_ImplFnDef:
      case TAG_TYPE_AS_ExplFnDef:
      case TAG_TYPE_AS_ExtExplFnDef: {
        lfd_l.ImpAppend(ld);
        break;
      }
      default: {
        break;
      }
    }
  }

  GenLocalFcts(lfd_l);

  if (vdm_CPP_isJAVA()) InitState_PM();

  SEQ<TYPE_CPP_Stmt> rb_l;
  rb_l.ImpConc(GenLocalValDef(lvd_l));

  bool existsDecl = false;
  size_t len_rb_l = rb_l.Length();
  for (size_t i = 1; (i <= len_rb_l) && !existsDecl; i++ )
    existsDecl = rb_l[i].Is(TAG_TYPE_CPP_DeclarationStmt);

  if (!In.Is(TAG_TYPE_AS_IdentStmt))
    rb_l.ImpConc(GenStmt(In, isLast));

  PopEnv();
  PopEnv_CGAUX();
  DeleteLoc();

// 20121221 -->
//  if (rb_l.Length() > 1)
  if ((rb_l.Length() > 1) && existsDecl)
// <-- 20121221  
    return SEQ<TYPE_CPP_Stmt>().ImpAppend(vdm_BC_GenBlock(rb_l));
  else
    return rb_l;
}

// CGLetBeSTStmt
// stmt : AS`LetBeSTStmt
// isLast : bool
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGLetBeSTStmt(const TYPE_AS_LetBeSTStmt & stmt, bool isLast)
{
  const SEQ<TYPE_AS_MultBind> & bind_l (stmt.GetSequence(pos_AS_LetBeSTStmt_bind));

  bool exists = false;
  size_t len_bind_l = bind_l.Length();
  for (size_t idx = 1; (idx <= len_bind_l) && !exists; idx++) {
    exists = bind_l[idx].Is(TAG_TYPE_AS_MultTypeBind);
  }
  if (exists) {
    SEQ<TYPE_CPP_Stmt> res;
    res.ImpConc(GenNotSupported(L"type bind", stmt, isLast));
#ifdef VDMPP
    if (vdm_CPP_isJAVA()) {
      res.ImpAppend(vdm_BC_GenReturnStmt(GenEmptyValue(GiveCurrentRType())));
    }
    else
#endif // VDMPP
    {
      res.ImpAppend(vdm_BC_GenGenericReturnStmt(GenEmptyValue(GiveCurrentRType())));
    }
    return res;
  }
  else
  {
    PushEnv_CGAUX();

    SEQ<TYPE_CPP_Stmt> rb_l;
    rb_l.ImpConc(GenBindListVariables(bind_l, stmt.GetField(pos_AS_LetBeSTStmt_St)));
    rb_l.ImpConc(GenStmt(stmt.GetRecord(pos_AS_LetBeSTStmt_In), isLast));

    PopEnv_CGAUX();

    return SEQ<TYPE_CPP_Stmt>().ImpAppend(vdm_BC_GenBlock(rb_l));
  }
}

// CGDefStmt
// stmt : AS`DefStmt
// isLast : bool
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGDefStmt(const TYPE_AS_DefStmt & stmt, bool isLast)
{
  const Sequence & def_l (stmt.GetSequence(pos_AS_DefStmt_value)); // seq of (PatternBind * Expr)
  const TYPE_AS_Stmt & body (stmt.GetRecord(pos_AS_DefStmt_In));

  PushEnv();
  PushEnv_CGAUX();

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
                expr, Int(NOT_INITIALISED_AS), Bool(false), ASTAUX::GetCid(patb));
        lvd_l.ImpAppend(vd);
        break;
      }
      case TAG_TYPE_AS_SetBind: {
        const TYPE_AS_Pattern & pat (patb.GetRecord(pos_AS_SetBind_pat));
        const TYPE_AS_Expr & Set (patb.GetRecord(pos_AS_SetBind_Set));
        TYPE_CI_ContextId pcid (GetCI().PushCGType(mk_REP_SetTypeRep(FindType(pat))));
        TYPE_CI_ContextId vcid (GetCI().PushCGType(mk_REP_SetTypeRep(FindType(expr))));
        TYPE_REP_TypeRep tp (mk_REP_UnionTypeRep(mk_set(FindType(Set), mk_REP_SetTypeRep(FindType(expr)))));
        TYPE_CI_ContextId iscid (GetCI().PushCGType(tp));

        TYPE_AS_BinaryExpr iset;
        iset.Init(Set, Int(SETINTERSECT), TYPE_AS_SetEnumerationExpr().Init(mk_sequence(expr), vcid), iscid);
        TYPE_AS_SetEnumPattern sep;
        sep.Init(mk_sequence(pat), pcid);

        TYPE_AS_ValueDef vd;
        vd.Init(sep, Nil(), iset, Int(NOT_INITIALISED_AS), Bool(false), ASTAUX::GetCid(patb));
        lvd_l.ImpAppend(vd);
        break;
      }
      default: { // pattern
        TYPE_AS_ValueDef vd;
        vd.Init(patb, Nil(), expr, Int(NOT_INITIALISED_AS), Bool(false), ASTAUX::GetCid(patb));
        lvd_l.ImpAppend(vd);
        break;
      }
    }
  }

  SEQ<TYPE_CPP_Stmt> rb_l;
  rb_l.ImpConc(GenLocalValDef(lvd_l));
  
  if (!body.Is(TAG_TYPE_AS_IdentStmt))
    rb_l.ImpConc(GenStmt(body, isLast));

  PopEnv();
  PopEnv_CGAUX();
  DeleteLoc();

  return SEQ<TYPE_CPP_Stmt>().ImpAppend(vdm_BC_GenBlock(rb_l));
}

// CGWhileLoopStmt
// wls : AS`WhileLoopStmt
// isLast : bool
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGWhileLoopStmt(const TYPE_AS_WhileLoopStmt & wls, bool isLast)
{
  const TYPE_AS_Expr & test (wls.GetRecord(pos_AS_WhileLoopStmt_test));
  const TYPE_AS_Stmt & body (wls.GetRecord(pos_AS_WhileLoopStmt_body));

  TYPE_REP_TypeRep whCrtlType (FindType(test));
  TYPE_REP_BooleanTypeRep btype;

  Tuple cgee (CGExprExcl(test, ASTAUX::MkId(L"whCrtl"), nil));
  const TYPE_CPP_Expr & whCrtl_v (cgee.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & eval_stmt (cgee.GetSequence(2));

  TYPE_CPP_Stmt bodystmt (vdm_BC_GenBlock(GenStmt(body, isLast)));

  TYPE_CPP_Expr whCrtlval;
  SEQ<TYPE_CPP_Stmt> isbool;
  if (IsBoolType(whCrtlType))
    whCrtlval = GenGetValue(whCrtl_v, btype);
  else {
    whCrtlval = GenGetValue(vdm_BC_GenCastExpr(GenBoolType(), whCrtl_v), btype);
    isbool.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenNot(GenIsBool(whCrtl_v)),
                       vdm_BC_GenBlock(mk_sequence(RunTime(L"A boolean was expected in while loop"))),
                                      nil));
  }

  TYPE_CPP_Expr cond (vdm_BC_GenBoolLit(true));
  if (eval_stmt.IsEmpty() && IsBoolType(whCrtlType))
    cond = whCrtlval;
  SEQ<TYPE_CPP_Stmt> block_l (eval_stmt);
  block_l.ImpConc(isbool);
  block_l.ImpAppend(vdm_BC_GenIfStmt(whCrtlval, bodystmt, vdm_BC_GenBreakStmt(Nil())));
  TYPE_CPP_Stmt block (eval_stmt.IsEmpty() && IsBoolType(whCrtlType) ? bodystmt : vdm_BC_GenBlock(block_l));

  SEQ<TYPE_CPP_Stmt> rb;
  rb.ImpAppend(vdm_BC_GenWhileStmt(cond, block));
  return rb;
}

// CGSeqForLoopStmt
// stmt : AS`SeqForLoopStmt
// isLast : bool
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGSeqForLoopStmt(const TYPE_AS_SeqForLoopStmt & sflstmt, bool isLast)
{
  const TYPE_AS_PatternBind & cv (sflstmt.GetRecord(pos_AS_SeqForLoopStmt_cv));
  const TYPE_AS_Expr & fseq      (sflstmt.GetRecord(pos_AS_SeqForLoopStmt_fseq));
  const TYPE_AS_Stmt & body      (sflstmt.GetRecord(pos_AS_SeqForLoopStmt_body));

  TYPE_REP_TypeRep sqt (FindType(fseq));
  TYPE_CPP_Identifier elem (vdm_BC_GiveName(ASTAUX::MkId(L"elem")));

  TYPE_CPP_Identifier succ (vdm_BC_GiveName(ASTAUX::MkId(L"succ")));
  SET<TYPE_AS_Name> pid_cv_s (FindAllNamesInPatternBind(cv));
  SET<TYPE_AS_Name> pid_expr_s (FindAllNamesInExpr(fseq));

  Tuple cgee (CGExprExcl(fseq, ASTAUX::MkId(L"sq"), nil));
  const TYPE_CPP_Expr & sq1 (cgee.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & sq_stmt (cgee.GetSequence(2));

  TYPE_CPP_Expr sq (vdm_BC_GiveName(ASTAUX::MkId(L"sq")));
  SEQ<TYPE_CPP_Stmt> rb_l (sq_stmt);
  if ( (!(pid_cv_s.ImpIntersect(pid_expr_s)).IsEmpty()) || !fseq.Is(TAG_TYPE_AS_Name))
  {
    rb_l.ImpConc(GenDeclInit_DS(sqt, sq, sq1));
  }
  else
  {
    sq = sq1;
  }

  PushEnv_CGAUX();
  InsertNamesinEnv2_CGAUX(pid_cv_s);

  Tuple dv (DeclareVars(cv));

  const SEQ<TYPE_CPP_Stmt> & decl_l (dv.GetSequence(1));
  const TYPE_AS_Pattern & pat (dv.GetRecord(2));

  if (!IsSeqType(sqt)) {
    TYPE_CPP_Identifier sq_q(vdm_BC_GiveName(ASTAUX::MkId(L"tmpSeq")));
    TYPE_CPP_Stmt rti (vdm_BC_GenBlock(mk_sequence(RunTime(L"A sequence was expected in sequence for loop"))));
    rb_l.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenNot(GenIsSeq(sq)), rti, nil));
#ifdef VDMPP
    if (vdm_CPP_isJAVA()) {
      rb_l.ImpAppend(vdm_BC_GenDecl(GenSeq0Type(), sq_q, vdm_BC_GenAsgnInit(GenCastSeq(sq, nil))));
    }
    else
#endif // VDMPP
    {
      rb_l.ImpAppend(vdm_BC_GenDecl(GenSeq0Type(), sq_q, vdm_BC_GenObjectInit(mk_sequence(sq))));
    }
    sq = sq_q;
  }

  TYPE_CGMAIN_VT vt1 (mk_CG_VT(sq, sqt));
  TYPE_CGMAIN_VT vt2 (mk_CG_VT(elem, FindSeqElemType(sqt)));
  SEQ<TYPE_CPP_Stmt> stmt (GenStmt(body, isLast));
  TYPE_CPP_Stmt rti (vdm_BC_GenBlock(mk_sequence(RunTime(L"Pattern did not match in sequence for loop"))));

  Tuple cgpme(CGPatternMatchExcl(pat, vt2, Set(), succ, Map(), stmt, false));
  const SEQ<TYPE_CPP_Stmt> & pm (cgpme.GetSequence(1));
  bool Is_Excl (cgpme.GetBoolValue(2)); // false : need to check pattern match failed

  SEQ<TYPE_CPP_Stmt> pm1;
  if (Is_Excl) {
    pm1.ImpConc(pm);
  }
  else {
    pm1.ImpAppend(vdm_BC_GenDecl(GenSmallBoolType(), succ, vdm_BC_GenAsgnInit(vdm_BC_GenBoolLit(false))));
    pm1.ImpConc(pm);
    pm1.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenNot(succ), rti, nil));
  }
  SEQ<TYPE_CPP_Stmt> stmt_l (MergeStmts( decl_l, pm1 )); 

  rb_l.ImpConc(GenIterSeq(vt1, nil, vt2, stmt_l));

  PopEnv_CGAUX();

  return SEQ<TYPE_CPP_Stmt>().ImpAppend(vdm_BC_GenBlock(rb_l));
}

// DeclareVars
// p : AS`PatternBind
// ==> seq of CPP`Stmt * AS`Pattern
Tuple vdmcg::DeclareVars(const TYPE_AS_PatternBind & p)
{
  switch (p.GetTag()) {
    case TAG_TYPE_AS_SetBind: {
      const TYPE_AS_Pattern & pat (p.GetRecord(pos_AS_SetBind_pat));
      return mk_(DeclarePatVars(pat), pat);
      break;
    }
    case TAG_TYPE_AS_TypeBind: {
      const TYPE_AS_Pattern & pat (p.GetRecord(pos_AS_TypeBind_pat));
      return mk_(DeclarePatVars(pat), pat);
      break;
    }
    default:
      return mk_(DeclarePatVars(p), p);
      break;
  }
}

// CGSetForLoopStmt
// stmt : AS`SetForLoopStmt
// isLast : bool
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGSetForLoopStmt(const TYPE_AS_SetForLoopStmt & sflstmt, bool isLast)
{
  const TYPE_AS_Pattern & pat (sflstmt.GetRecord(pos_AS_SetForLoopStmt_cv));
  const TYPE_AS_Expr & fset   (sflstmt.GetRecord(pos_AS_SetForLoopStmt_fset));
  const TYPE_AS_Stmt & body   (sflstmt.GetRecord(pos_AS_SetForLoopStmt_body));

  TYPE_REP_TypeRep settp (FindType(fset));
  TYPE_CPP_Identifier elem (vdm_BC_GiveName(ASTAUX::MkId(L"elem")));
  TYPE_CPP_Identifier succ (vdm_BC_GiveName(ASTAUX::MkId(L"succ")));
  SET<TYPE_AS_Name> pid_pat_s (FindPatternId(pat).Dom());
  SET<TYPE_AS_Name> pid_expr_s (FindAllNamesInExpr(fset));

  Tuple cgee (CGExprExcl(fset, ASTAUX::MkId(L"iset"), nil));
  const TYPE_CPP_Expr & iset1 (cgee.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & set_stmt (cgee.GetSequence(2));

  TYPE_CPP_Expr iset (vdm_BC_GiveName(ASTAUX::MkId(L"iset")));

  SEQ<TYPE_CPP_Stmt> rb_l (set_stmt);
  if ((!(pid_pat_s.ImpIntersect(pid_expr_s)).IsEmpty()) || !fset.Is(TAG_TYPE_AS_Name))
  {
    rb_l.ImpConc(GenConstDeclInit(settp, iset, iset1));
  }
  else
  {
    iset = iset1;
  }

  if (!IsSetType(settp))
  {
    TYPE_CPP_Identifier iset_q (vdm_BC_GiveName(ASTAUX::MkId(L"tmpSet")));
    TYPE_CPP_Expr cond (GenIsSet(iset));

    TYPE_CPP_Stmt rti(vdm_BC_GenBlock(mk_sequence(RunTime(L"A set was expected in set for loop"))));
    rb_l.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenNot(cond), rti, nil));
#ifdef VDMPP
    if (vdm_CPP_isJAVA()) {
      rb_l.ImpAppend(vdm_BC_GenDecl(GenSetType(), iset_q, vdm_BC_GenAsgnInit(GenCastSetType(iset))));
    }
    else
#endif // VDMPP
    {
      rb_l.ImpAppend(vdm_BC_GenDecl(GenSetType(), iset_q, vdm_BC_GenAsgnInit(iset)));
    }
    iset = iset_q;
  }

  TYPE_CGMAIN_VT vt1(mk_CG_VT(iset, settp));
  TYPE_CGMAIN_VT vt2(mk_CG_VT(elem, FindSetElemType(settp)));

  PushEnv_CGAUX();

  if (pat.Is(TAG_TYPE_AS_PatternName)) {
    const Generic & name (pat.GetField(pos_AS_PatternName_nm));
    bool seqindices = IsSeqIndicesSet(fset);
    Set seqapply_s;
    if (!name.IsNil()) {
      InsertName_CGAUX(name);
      vt2.SetField(pos_CGMAIN_VT_name, vdm_BC_Rename(name));
      if (seqindices)
        seqapply_s.Insert(mk_(vdm_BC_Rename(fset.GetRecord(pos_AS_PrefixExpr_arg)), vdm_BC_Rename(name)));
    }
    AddNoCheckSeqApply(seqapply_s);
    SEQ<TYPE_CPP_Stmt> stmt_l;
    stmt_l.ImpConc(GenStmt(body, isLast));
    RemNoCheckSeqApply(seqapply_s);
    rb_l.ImpConc(GenIterSet(vt1, nil, vt2, stmt_l));
  }
  else {
    SEQ<TYPE_CPP_Stmt> decl_l (DeclarePatVars(pat));

    SEQ<TYPE_CPP_Stmt> stmt (GenStmt(body, isLast));

    Tuple cgpme (CGPatternMatchExcl(pat, vt2, Set(), succ, Map(), stmt, true));
    const SEQ<TYPE_CPP_Stmt> & pm (cgpme.GetSequence(1));
    bool Is_Excl (cgpme.GetBoolValue(2)); // false : need to check pattern match failed

    SEQ<TYPE_CPP_Stmt> pm1;
    if (Is_Excl)
      pm1.ImpConc(pm);
    else {
      TYPE_CPP_Stmt rti (vdm_BC_GenBlock(mk_sequence(RunTime(L"Pattern did not match in set for loop"))));
      pm1.ImpAppend(vdm_BC_GenDecl(GenSmallBoolType(), succ, vdm_BC_GenAsgnInit(vdm_BC_GenBoolLit(false))));
      pm1.ImpConc(pm);
      pm1.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenNot(succ), rti, nil));
    }
    SEQ<TYPE_CPP_Stmt> stmt_l;
    stmt_l.ImpConc(MergeStmts( decl_l, pm1 ));
    rb_l.ImpConc(GenIterSet(vt1, nil, vt2, stmt_l));
  }
  PopEnv_CGAUX();
  return SEQ<TYPE_CPP_Stmt>().ImpAppend(vdm_BC_GenBlock(rb_l));
}

// CGIndexedForLoopStmt
// stmt : AS`IndexForLoopStmt
// isLast : bool
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGIndexedForLoopStmt(const TYPE_AS_IndexForLoopStmt & stmt, bool isLast)
{
  const TYPE_AS_Name & nm (stmt.GetRecord(pos_AS_IndexForLoopStmt_cv));
  const TYPE_AS_Expr & lb (stmt.GetRecord(pos_AS_IndexForLoopStmt_lb));
  const TYPE_AS_Expr & ub (stmt.GetRecord(pos_AS_IndexForLoopStmt_ub));
  const Generic & By (stmt.GetField(pos_AS_IndexForLoopStmt_By));
  const TYPE_AS_Stmt & body (stmt.GetRecord(pos_AS_IndexForLoopStmt_body));

  Tuple gbav (GenBoundAndValue(ASTAUX::MkId(L"lb"), lb));
  const SEQ<TYPE_CPP_Stmt> & st1_l (gbav.GetSequence(1));
  const TYPE_CPP_Expr & val1 (gbav.GetRecord(2));

  Tuple gbav2 (GenBoundAndValue(ASTAUX::MkId(L"ub"), ub));
  const SEQ<TYPE_CPP_Stmt> & st2_l (gbav2.GetSequence(1));
  const TYPE_CPP_Expr & val2 (gbav2.GetRecord(2));

  TYPE_CPP_Identifier ilb (vdm_BC_GiveName(ASTAUX::MkId(L"ilb")));

  TYPE_CPP_Expr iub (st2_l.IsEmpty() ? val2 : TYPE_CPP_Expr(vdm_BC_GiveName(ASTAUX::MkId(L"iub"))));

  TYPE_CPP_TypeSpecifier idcl(GenSmallNumType());

  Tuple gbp (GenByPart(By, ilb, iub));
  const SEQ<TYPE_CPP_Stmt> & st3_l (gbp.GetSequence(1));
  const TYPE_CPP_Expr & inc (gbp.GetRecord(2));
  const TYPE_CPP_Expr & cont (gbp.GetField(3));   // CPP`Expr

  InsertName_CGAUX(nm);

  TYPE_REP_TypeRep intTp (mk_REP_NumericTypeRep(Int(INTEGER)));

  SEQ<TYPE_CPP_Stmt> stmt_l;
  stmt_l.ImpConc(GenDeclInit_DS(intTp, vdm_BC_Rename(nm), ilb));
  stmt_l.ImpConc(GenStmt(body, isLast));

  TYPE_CPP_Stmt fb (vdm_BC_GenBlock(stmt_l));
  TYPE_CPP_Stmt fs (vdm_BC_GenForStmt(vdm_BC_GenDecl(idcl, ilb, vdm_BC_GenAsgnInit(val1)),
                                      cont, mk_sequence(inc), fb));

  SEQ<TYPE_CPP_Stmt> rb_l (st1_l);
  if (!st2_l.IsEmpty())
  {
    rb_l.ImpConc(st2_l);
    rb_l.ImpAppend(vdm_BC_GenDecl(idcl, iub, vdm_BC_GenAsgnInit(val2)));
  }
  rb_l.ImpConc(st3_l);
  rb_l.ImpAppend(fs);
  return rb_l;
}

// GenBoundAndValue
// var : seq of char
// expr : AS`Expr
// ==> seq of CPP`Stmt * CPP`Expr
Tuple vdmcg::GenBoundAndValue(const SEQ<Char> & var, const TYPE_AS_Expr & expr)
{
  Generic tp (FindType(expr));

  Tuple cgee (CGExprExcl(expr, var, nil));
  const TYPE_CPP_Expr & var_v (cgee.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & var_stmt (cgee.GetSequence(2));

  if (!IsIntType(tp))
  {
    SEQ<TYPE_CPP_Stmt> rb_l (var_stmt);
    TYPE_CPP_Expr tmp (var_v);
    if (!var_v.Is(TAG_TYPE_CPP_Identifier)) {
      TYPE_CPP_Identifier v1 (vdm_BC_GiveName(ASTAUX::MkId(L"tmp")));
      rb_l.ImpConc(GenDecl_DS(tp, v1, vdm_BC_GenAsgnInit(var_v)));
      tmp = v1;
    }

    TYPE_CPP_Expr cond (vdm_BC_GenNot(GenIsInt(tmp)));
    TYPE_CPP_Stmt rti (vdm_BC_GenBlock(mk_sequence(RunTime(L"An integer was expected in indexed for loop"))));

    rb_l.ImpAppend(vdm_BC_GenIfStmt(cond, rti, nil));

    TYPE_CPP_Expr val;
#ifdef VDMPP
    if (vdm_CPP_isJAVA()) {
      val = GenGetValue(vdm_BC_GenCastExpr(GenNumType(), tmp), mk_REP_NumericTypeRep(Int(INTEGER)));
    }
    else
#endif // VDMPP
    {
      val = GenGetValue(vdm_BC_GenCastExpr(GenIntType(), tmp), nil);
    }

    return mk_(rb_l, val);
  }
  else if (expr.Is(TAG_TYPE_AS_RealLit))
  {
    TYPE_CPP_Expr val (vdm_BC_GenIntegerLit(expr.GetReal(pos_AS_RealLit_val)));
    return mk_(SEQ<TYPE_CPP_Stmt>(), val);
  }
  else if (expr.Is(TAG_TYPE_AS_NumLit))
  {
    TYPE_CPP_Expr val (vdm_BC_GenIntegerLit(expr.GetReal(pos_AS_NumLit_val)));
    return mk_(SEQ<TYPE_CPP_Stmt>(), val);
  }
  else
  {
    SEQ<TYPE_CPP_Stmt> rb_l (var_stmt);
    TYPE_CPP_Expr val (GenGetValue(var_v, mk_REP_NumericTypeRep(Int(INTEGER))));
    return mk_(rb_l, val);
  }
}

// GenByPart
// By : [AS`Expr]
// ilb : CPP`Name
// iub : CPP`Expr
// ==> seq of CPP`Stmt * CPP`Expr * CPP`Expr
Tuple vdmcg::GenByPart(const Generic & By, const TYPE_CPP_Name & ilb, const TYPE_CPP_Expr & iub)
{
  if (! By.IsNil())
  {
    Tuple gbav (GenBoundAndValue(ASTAUX::MkId(L"by"), By));
    const TYPE_CPP_Expr & val (gbav.GetRecord(2));

    if (val.Is(TAG_TYPE_CPP_IntegerLit))
    {
      int64_t by_v = val.GetInt(pos_CPP_IntegerLit_val).GetValue();
      if (by_v != 0)
      {
        TYPE_CPP_Expr inc (by_v == -1 ? vdm_BC_GenPostMinusMinus(ilb)
                                      : vdm_BC_GenAsgnPlusExpr(ilb, val));
        if (by_v > 0)
          return mk_(gbav.GetSequence(1), inc, vdm_BC_GenLeq(ilb, iub));
        else if (by_v == -1)
          return mk_(gbav.GetSequence(1), inc, vdm_BC_GenGeq(ilb, iub));
        else
          return mk_(gbav.GetSequence(1), inc, vdm_BC_GenGeq(ilb, iub));
      }
    }

    TYPE_CPP_Identifier iby (vdm_BC_GiveName(ASTAUX::MkId(L"iby")));
    TYPE_CPP_IntegerLit zero (vdm_BC_GenIntegerLit(0));

    TYPE_CPP_Stmt rti (vdm_BC_GenBlock(mk_sequence(RunTime(L"Step length in indexed for loop was 0"))));
    TYPE_CPP_Expr inc (vdm_BC_GenAsgnPlusExpr(ilb, iby));

    SEQ<TYPE_CPP_Stmt> rb_l (gbav.GetSequence(1));
    rb_l.ImpAppend(vdm_BC_GenDecl(GenSmallNumType(), iby, vdm_BC_GenAsgnInit(val)));
    rb_l.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenEq(iby, zero), rti, nil));
 
    TYPE_CPP_Expr cont (vdm_BC_GenCondExpr(vdm_BC_GenBracketedExpr(vdm_BC_GenLt(iby, zero)),
                                           vdm_BC_GenBracketedExpr(vdm_BC_GenGeq(ilb,iub)),
                                           vdm_BC_GenBracketedExpr(vdm_BC_GenLeq(ilb,iub))));

    return mk_(rb_l, inc, cont);
  }
  else {
    return mk_(SEQ<TYPE_CPP_Stmt>(), vdm_BC_GenPostPlusPlus(ilb), vdm_BC_GenLeq(ilb, iub));
  }
}

// CGCasesStmt
// cs : AS`CasesStmt
// isLast : bool
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGCasesStmt(const TYPE_AS_CasesStmt & cs, bool isLast)
{
  const TYPE_AS_Expr & sel                 (cs.GetRecord(pos_AS_CasesStmt_sel));
  const SEQ<TYPE_AS_CasesStmtAltn> & altns (cs.GetSequence(pos_AS_CasesStmt_altns));
  const Generic & Others                   (cs.GetField(pos_AS_CasesStmt_Others));

  TYPE_CPP_Identifier succ_v (vdm_BC_GiveName(ASTAUX::MkId(L"succ")));
  TYPE_REP_TypeRep selResType (FindType(sel));

  Tuple cgee (CGExprExcl (sel, ASTAUX::MkId(L"selRes"), nil));
  const TYPE_CPP_Expr & selRes1_v (cgee.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & sel_stmt (cgee.GetSequence(2));

  SET<TYPE_AS_Name> all_pat_ids (FindAllPatIdInCasesAlt(altns));
  SET<TYPE_AS_Name> all_nm_in_expr (FindAllNamesInExpr(sel));
  bool pat_in_expr (!((all_nm_in_expr.Intersect(all_pat_ids)).IsEmpty()));
  TYPE_CPP_Expr sel_v (vdm_BC_GiveName(ASTAUX::MkId(L"selRes")));

  bool need_decl = (pat_in_expr && sel_stmt.IsEmpty());

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

  SEQ<TYPE_CPP_Stmt> rb (sel_stmt);

  if (need_decl)
    rb.ImpConc(GenConstDeclInit(selResType, sel_v, selRes1_v));

  rb.ImpAppend(vdm_BC_GenDecl(GenSmallBoolType(), succ_v, vdm_BC_GenAsgnInit(vdm_BC_GenBoolLit(false))));
  rb.ImpConc(CGCasesStmtAltn(altns, mk_CG_VT(selRes_v, selResType), succ_v, Others, isLast));

  if (Others.IsNil())
  {
    if (isLast && !GiveCurrentRType().Is(TAG_TYPE_REP_UnitTypeRep))
    {
      rb.ImpAppend(RunTime (L"The operation did not return a value"));
#ifdef VDMPP
      if (vdm_CPP_isJAVA()) {
        rb.ImpAppend(vdm_BC_GenReturnStmt(GenEmptyValue(GiveCurrentRType())));
      }
      else
#endif // VDMPP
      {
        rb.ImpAppend(vdm_BC_GenGenericReturnStmt(GenEmptyValue(GiveCurrentRType())));
      }
    }
  }
  return rb;
}

// CGCasesStmtAltn
// altns : seq1 of AS`CasesStmtAltn
// selRes_v : CGMAIN`VT
// succ_v : CPP`Name
// Others : [AS`Stmt]
// isLast : bool
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGCasesStmtAltn(const SEQ<TYPE_AS_CasesStmtAltn> & altns,
                                          const TYPE_CGMAIN_VT & selRes_v,
                                          const TYPE_CPP_Name & succ_v,
                                          const Generic & Others,
                                          bool isLast)
{
  SEQ<TYPE_CPP_Stmt> rb;

  // NOTE: len_altns > 0
  size_t len_altns = altns.Length();
  for (size_t i = 1; i <= len_altns; i++)
  {
    PushEnv_CGAUX();
    TYPE_CPP_Stmt stmt (CGAltnStmt(altns[i], selRes_v, succ_v, Nil(), isLast));

    if (i == 1)
      rb.ImpAppend(stmt);
    else
      rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenNot(succ_v), stmt, nil));
    PopEnv_CGAUX();
  }
  
  if (!Others.IsNil())
  {
//#ifdef VDMPP
//    rb.ImpAppend(vdm_BC_GenSingleLineComments(SEQ<Char>(L"others")));
//#endif // VDMPP
    SEQ<TYPE_CPP_Stmt> othersStmt (GenStmt(Others, isLast));
    if (isLast && !GiveCurrentRType().Is(TAG_TYPE_REP_UnitTypeRep))
      rb.ImpConc(othersStmt);
    else
      rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenNot(succ_v), vdm_BC_GenBlock(othersStmt), nil));
  }
  return rb;
}

// CGAltnStmt
// csa : AS`CasesStmtAltn
// selRes_v : CGMAIN`VT
// succ_v : CPP`Name
// elseStmt : [CPP`Stmt]
// isLast : bool
// ==> CPP`Stmt
TYPE_CPP_Stmt vdmcg::CGAltnStmt(const TYPE_AS_CasesStmtAltn & csa,
                                const TYPE_CGMAIN_VT & selRes_v,
                                const TYPE_CPP_Name & succ_v,
                                const Generic & elseStmt,
                                bool isLast)
{
  const SEQ<TYPE_AS_Pattern> & p_l (csa.GetSequence(pos_AS_CasesStmtAltn_match));
  const TYPE_AS_Stmt & s           (csa.GetRecord(pos_AS_CasesStmtAltn_body));
  const TYPE_CI_ContextId & cid    (csa.GetInt(pos_AS_CasesStmtAltn_cid));

  // ==> bool * map AS`Name to set of REP`TypeRep
  Tuple spi (SamePatternIds(p_l));
  bool samepatternIds (spi.GetBoolValue(1));
  const Map & pid_m(spi.GetMap(2)); // map AS`Name to set of REP`TypeRep

  if (samepatternIds)
  {
    // each pattern in the pattern list contains the same set of PatternId.
    RememberPid_m(pid_m);

    SEQ<TYPE_CPP_Stmt> decl (DeclarePatterns(pid_m));
    SEQ<TYPE_CPP_Stmt> rb;
    if ( p_l.Length() > 1 )
      rb.ImpConc( decl );

    size_t len_p_l = p_l.Length();
    for (size_t i = 1; i <= len_p_l; i++)
    {
      Tuple cgpm (CGPatternMatchExcl(p_l[i], selRes_v, Set(), succ_v, Map(), Nil(), false));
      const SEQ<TYPE_CPP_Stmt> & pm (cgpm.GetSequence(1));
      bool Is_Excl (cgpm.GetBoolValue(2)); // false : need to check pattern match failed

      SEQ<TYPE_CPP_Stmt> pm1;
      if (Is_Excl)
        pm1.ImpAppend(vdm_BC_GenAsgnStmt(succ_v, vdm_BC_GenBoolLit(true)));
      pm1.ImpConc(pm);

      if (!pm1.IsEmpty())
      {
        if(i == 1) {
          if ( len_p_l > 1 )
            rb.ImpConc(pm1);
          else
            rb.ImpConc(MergeStmts( decl, pm1 ));
        }
        else
          rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenNot(succ_v), vdm_BC_GenBlock(pm1), nil));
      }
    }
    TYPE_CPP_Stmt stmt (vdm_BC_GenBlock(GenStmt(s, isLast)));
    DeletePid_m();
    rb.ImpAppend( vdm_BC_GenIfStmt(succ_v, stmt, elseStmt) );
    return vdm_BC_GenBlock(rb);
  }
  else
  {
    Generic elseStmt1 (elseStmt);

    size_t len_p_l = p_l.Length();
    for (size_t i = 0; i <= (len_p_l - 1); i++)
    {
      if (i > 0) PushEnv_CGAUX();

      SEQ<TYPE_AS_Pattern> tmp_l;
      tmp_l.ImpAppend(p_l[p_l.Length() - i]);

      TYPE_AS_CasesStmtAltn c_stmt;
      c_stmt.Init(tmp_l, s, cid);

      elseStmt1 = CGAltnStmt( c_stmt, selRes_v, succ_v, elseStmt1, isLast);
      if (i > 0) PopEnv_CGAUX();
    }
    return elseStmt1;
  }
}

// CGIdentStmt
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGIdentStmt()
{
//  return SEQ<TYPE_CPP_Stmt>().ImpAppend(vdm_BC_GenExpressionStmt(eseq));
  return SEQ<TYPE_CPP_Stmt>().ImpAppend(vdm_BC_GenExpressionStmt(Nil()));
}

// CGIfStmt
// ifs : AS`IfStmt
// isLast : bool
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGIfStmt(const TYPE_AS_IfStmt & ifs, bool isLast)
{
  const TYPE_AS_Expr & test            (ifs.GetRecord(pos_AS_IfStmt_test));
  const TYPE_AS_Stmt & cons            (ifs.GetRecord(pos_AS_IfStmt_cons));
  const SEQ<TYPE_AS_ElseifStmt> & elif (ifs.GetSequence(pos_AS_IfStmt_elsif));
  const Generic & altn                 (ifs.GetField(pos_AS_IfStmt_altn));

  Tuple cgee (CGExprExcl(test, ASTAUX::MkId(L"cond"), nil));
  const TYPE_CPP_Expr & cond1_v (cgee.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & cond_stmt (cgee.GetSequence(2));

  TYPE_REP_BooleanTypeRep btype;

  TYPE_REP_TypeRep condType (FindType(test));

  TYPE_CPP_Expr cond_v (IsBoolType(condType) ? cond1_v : GenCastType(btype, cond1_v));

// CastExpr 
// RelationalExpr

  SEQ<TYPE_CPP_Stmt> rb (cond_stmt);

  if (!IsBoolType(condType))
  {
    rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenNot(GenIsBool(cond1_v)),
                     vdm_BC_GenBlock(mk_sequence(RunTime (L"A boolean was expected"))), nil));
  }

  Generic tmpb; // [seq of CPP`Stmt]
  if (altn.IsNil())
  {
    tmpb = nil;
    if (isLast && !GiveCurrentRType().Is(TAG_TYPE_REP_UnitTypeRep))
    {
      SEQ<TYPE_CPP_Stmt> lrb;
      lrb.ImpAppend(RunTime (L"The operation did not return a value"));
#ifdef VDMPP
      if (vdm_CPP_isJAVA()) {
        lrb.ImpAppend(vdm_BC_GenReturnStmt(GenEmptyValue(GiveCurrentRType())));
      }
      else
#endif // VDMPP
      {
        lrb.ImpAppend(vdm_BC_GenGenericReturnStmt(GenEmptyValue(GiveCurrentRType())));
      }
      tmpb = lrb;
    }
  }
  else if (altn.Is(TAG_TYPE_AS_IdentStmt))
    tmpb = nil;
  else
    tmpb = GenStmt(altn, isLast);

  size_t len_elif = elif.Length();
  for (size_t i = len_elif; i >= 1; i--)
  {
    const TYPE_AS_ElseifStmt & eis (elif[i]);
    const TYPE_AS_Expr & elif_test (eis.GetRecord(pos_AS_ElseifStmt_test));
    const TYPE_AS_Stmt & elif_cons (eis.GetRecord(pos_AS_ElseifStmt_cons));

    Tuple cgee (CGExprExcl(elif_test, ASTAUX::MkId(L"cond"),nil));
    const TYPE_CPP_Expr & elif_cond1_v (cgee.GetRecord(1));
    const SEQ<TYPE_CPP_Stmt> & elif_cond_stmt (cgee.GetSequence(2));

    TYPE_REP_TypeRep elif_condType (FindType(elif_test));

    TYPE_CPP_Expr elif_cond_v (IsBoolType(elif_condType) ? elif_cond1_v : GenCastType(btype, elif_cond1_v));

    SEQ<TYPE_CPP_Stmt> body (elif_cond_stmt);

    if (!IsBoolType(elif_condType))
    {
      body.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenNot(GenIsBool(elif_cond1_v)),
                                      RunTime (L"A boolean was expected"), nil));
    }

    TYPE_CPP_Expr elif_cond (GenGetValue(elif_cond_v, btype));
    TYPE_CPP_Stmt elif_cons_stmt (GenIfConsCPPStmt(GenStmt(elif_cons, isLast)));

    Generic elif_cons2_stmt;
    if (tmpb.IsNil())
      elif_cons2_stmt = nil;
    else
      elif_cons2_stmt = vdm_BC_GenBlock(tmpb);

    TYPE_CPP_Stmt ifstmt (vdm_BC_GenIfStmt(elif_cond, elif_cons_stmt, elif_cons2_stmt));

    TYPE_CPP_Stmt ifseq (tmpb.IsNil() ? vdm_BC_GenBlock(type_dL().ImpAppend(ifstmt)) : ifstmt);

    body.ImpAppend(ifseq);

    tmpb = body;
  }

  TYPE_CPP_Expr cond (GenGetValue(cond_v, btype));
  TYPE_CPP_Stmt cons_stmt (GenIfConsCPPStmt(GenStmt(cons, isLast)));

  Generic alt2;
  if (tmpb.IsNil()) {
    alt2 = nil;
  }
  else {
    alt2 = vdm_BC_GenBlock(tmpb);
  }

  rb.ImpAppend( vdm_BC_GenIfStmt(cond, cons_stmt, alt2) );
  return rb;
}

// GenIfConsCPPStmt
// s_l : seq of CPP`Stmt
// ==> CPP`Stmt
//FIXME: Should be type_dL
TYPE_CPP_Stmt vdmcg::GenIfConsCPPStmt(const SEQ<TYPE_CPP_Stmt> & s_l)
{
/*
  if (s_l.Length() == 1 )
  {
    TYPE_CPP_Stmt stmt (s_l.Hd());
    if (stmt.Is(TAG_TYPE_CPP_IfStmt))
      return vdm_BC_GenBlock(s_l);
    else
      return stmt;
  }
  else
*/
    return vdm_BC_GenBlock(s_l);
}

// CGErrorStmt
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGErrorStmt()
{
  SEQ<TYPE_CPP_Stmt> res;
  res.ImpAppend(RunTime(L"Can not evaluate 'error' statement"));
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    res.ImpAppend(vdm_BC_GenReturnStmt(GenEmptyValue(GiveCurrentRType())));
  }
  else
#endif // VDMPP
  {
    res.ImpAppend(vdm_BC_GenGenericReturnStmt(GenEmptyValue(GiveCurrentRType())));
  }
  return res;
}

// GenSpecTopStmt
// -> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenSpecTopStmt(const TYPE_AS_SpecificationStmt & stmt)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    SEQ<TYPE_CPP_Stmt> s;
    s.ImpAppend(NotSupported(L"specification statement", stmt));
    return s;
  }
  else
#endif // VDMPP
  {
    Tuple gcmn (GiveCurMName());
    const TYPE_AS_Id & mthd (gcmn.GetSequence(1));
    const Int & num (gcmn.GetInt(2));

    TYPE_AS_Id name (ASTAUX::MkId(L"vdm_").ImpConc(GiveCurCName())
                                  .ImpConc(ASTAUX::MkId(L"_"))
                                  .ImpConc(mthd)
                                  .ImpConc(ASTAUX::MkId(L"-"))
                                  .ImpConc(StringNumber(num)));
    SEQ<TYPE_CPP_Stmt> s;
    s.ImpAppend( vdm_BC_GenInclStmt(GenFileExt(name)));
    return s;
  }
}

// CGAlwaysStmt
// p_as : AS`AlwaysStmt
// p_isLast :  bool
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGAlwaysStmt(const TYPE_AS_AlwaysStmt & p_as, bool p_isLast)
{
  const TYPE_AS_Stmt & post (p_as.GetRecord(pos_AS_AlwaysStmt_Post));
  const TYPE_AS_Stmt & body (p_as.GetRecord(pos_AS_AlwaysStmt_body));

  TYPE_REP_TypeRep tppost (RemoveExitType(GetCI().GetTypeInfo(ASTAUX::GetCid(post))));
  TYPE_REP_TypeRep tpbody (RemoveExitType(GetCI().GetTypeInfo(ASTAUX::GetCid(body))));

#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    TYPE_CPP_Stmt handlerStmt (vdm_BC_GenBlock(GenStmt(post, false)));
    SEQ<TYPE_CPP_Stmt> bodyStmt (GenStmt(body, p_isLast));

    // hack for java
    // in case of always ... in trap ... with ... in 
    // compose to one try block 
    if ((bodyStmt.Length() == 1) &&
        bodyStmt[1].Is(TAG_TYPE_CPP_TryBlock) &&
        bodyStmt[1].GetField(pos_CPP_TryBlock_fin).IsNil())
    {
      const TYPE_CPP_TryBlock & tb (bodyStmt[1]);
      SEQ<TYPE_CPP_Stmt> result;
      result.ImpAppend(vdm_BC_GenTryBlock(tb.GetRecord(pos_CPP_TryBlock_stmt).GetSequence(pos_CPP_CompoundStmt_stms),
                                          tb.GetSequence(pos_CPP_TryBlock_hdl),
                                          handlerStmt));
      return result;
    }

    SEQ<TYPE_CPP_Stmt> result;
    result.ImpAppend(vdm_BC_GenTryBlock(bodyStmt, SEQ<TYPE_CPP_Handler>(), handlerStmt));
    return result;
  }
  else
#endif // VDMPP
  { // C++
    TYPE_CPP_Identifier l_excName (vdm_BC_GiveName(ASTAUX::MkId(L"exc")));
    TYPE_CPP_Identifier l_excValNm (vdm_BC_GiveName(ASTAUX::MkId(L"excVal")));
    TYPE_CPP_Identifier l_excRaisedNm (vdm_BC_GiveName(ASTAUX::MkId(L"exceptionRaised")));
    TYPE_CPP_Identifier l_excId (vdm_BC_GenIdentifier(ASTAUX::MkId(L"VDMSpecException")));
    TYPE_CPP_Expr l_getMethod (vdm_BC_GenFctCallObjMemAcc(l_excName, ASTAUX::MkId(L"getValue"), SEQ<TYPE_CPP_Expr>()));
    TYPE_CPP_Stmt l_excValAsgn (vdm_BC_GenAsgnStmt(l_excValNm, l_getMethod));
    TYPE_CPP_Stmt l_excRaisedAsgn (vdm_BC_GenAsgnStmt(l_excRaisedNm, vdm_BC_GenBoolLit(true)));
    TYPE_CPP_TypeSpecifier l_excType (vdm_BC_GenTypeSpecifier(l_excId));
    TYPE_CPP_ExceptionDeclaration l_exDecl (vdm_BC_GenExceptionDeclaration( mk_sequence(l_excType), l_excName));
    TYPE_CPP_Expr l_excObj (vdm_BC_GenFctCall(l_excId, mk_sequence(l_excValNm)));
    TYPE_CPP_Stmt l_excHandlerBlock (vdm_BC_GenBlock(mk_sequence(l_excValAsgn, l_excRaisedAsgn)));

    TYPE_CPP_Identifier l_retValNm (vdm_BC_GiveName(ASTAUX::MkId(L"retVal")));
    TYPE_CPP_Identifier l_retCalledNm (vdm_BC_GiveName(ASTAUX::MkId(L"returnCalled")));
    TYPE_CPP_Identifier l_retId (vdm_BC_GenIdentifier(ASTAUX::MkId(L"Common")));
    TYPE_CPP_Stmt l_retValAsgn (vdm_BC_GenAsgnStmt(l_retValNm, l_excName));
    TYPE_CPP_Stmt l_retCalledAsgn (vdm_BC_GenAsgnStmt(l_retCalledNm, vdm_BC_GenBoolLit(true)));
    TYPE_CPP_TypeSpecifier l_retType (vdm_BC_GenTypeSpecifier(l_retId));
    TYPE_CPP_ExceptionDeclaration l_rtDecl (vdm_BC_GenExceptionDeclaration( mk_sequence(l_retType), l_excName));
    SEQ<TYPE_CPP_Stmt> handler_inner;
    if (!NoReturnValue(tpbody))
      handler_inner.ImpAppend(l_retValAsgn);
    if (!p_isLast && !tpbody.Is(TAG_TYPE_REP_UnitTypeRep))
      handler_inner.ImpAppend(l_retCalledAsgn);
    TYPE_CPP_Stmt l_retHandlerBlock (vdm_BC_GenBlock(handler_inner));

    SEQ<TYPE_CPP_Handler> handlers;
    handlers.ImpAppend(vdm_BC_GenHandler(l_exDecl, l_excHandlerBlock));
    if (!tpbody.Is(TAG_TYPE_REP_UnitTypeRep)) {
      if (NoReturnValue(tpbody))
        handlers.ImpAppend(vdm_BC_GenHandler(TYPE_CPP_All().Init(NilContextId), l_retHandlerBlock));
      else
        handlers.ImpAppend(vdm_BC_GenHandler(l_rtDecl, l_retHandlerBlock));
    }

    UpAlwaysLevel();
    SEQ<TYPE_CPP_Stmt> bodyStmt (GenStmt(body, p_isLast));
    DownAlwaysLevel();

    SEQ<TYPE_CPP_Stmt> handlerStmt;
    if (post.Is(TAG_TYPE_AS_IdentStmt))
    {
      handlerStmt.ImpAppend(vdm_BC_GenIfStmt(l_excRaisedNm, 
                                             vdm_BC_GenExpressionStmt(vdm_BC_GenThrowExpression(l_excObj)),
                                             nil));
    }
    else
    {
      handlerStmt.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenBoolLit(true), vdm_BC_GenBlock(GenStmt(post, false)), nil));
      handlerStmt.ImpAppend(vdm_BC_GenIfStmt(l_excRaisedNm, 
                                             vdm_BC_GenExpressionStmt(vdm_BC_GenThrowExpression(l_excObj)),
                                             nil));
    }

    if (!tpbody.Is(TAG_TYPE_REP_UnitTypeRep)) {
      TYPE_CPP_Stmt stmt;
      if (IsInAlways()) {
        if (NoReturnValue(tpbody))
          stmt = vdm_BC_GenExpressionStmt(vdm_BC_GenThrowExpression(vdm_BC_GenIntegerLit(0)));
        else
          stmt = vdm_BC_GenExpressionStmt(vdm_BC_GenThrowExpression(l_retValNm));
      } else {
        if (NoReturnValue(tpbody))
          stmt = vdm_BC_GenReturnStmt(nil);
        else
          stmt = vdm_BC_GenReturnStmt(l_retValNm);
      }
      if (p_isLast)
        handlerStmt.ImpAppend(stmt);
      else
        handlerStmt.ImpAppend(vdm_BC_GenIfStmt(l_retCalledNm, stmt, nil));
    }

    SEQ<TYPE_CPP_Stmt> rb_l;
    rb_l.ImpConc(GenDecl_DS(Nil(), l_excValNm, Nil()));
    if (!NoReturnValue(tpbody))
      rb_l.ImpConc(GenDecl_DS(Nil(), l_retValNm, Nil()));
    rb_l.ImpAppend(vdm_BC_GenDecl(GenSmallBoolType(), l_excRaisedNm,
                                         vdm_BC_GenAsgnInit(vdm_BC_GenBoolLit(false))));
    if (!p_isLast && !tpbody.Is(TAG_TYPE_REP_UnitTypeRep))
      rb_l.ImpAppend(vdm_BC_GenDecl(GenSmallBoolType(), l_retCalledNm,
                                         vdm_BC_GenAsgnInit(vdm_BC_GenBoolLit(false))));
    rb_l.ImpAppend(vdm_BC_GenTryBlock(bodyStmt, handlers, Nil()));
    rb_l.ImpConc(handlerStmt);
    return rb_l;
  }
}

// CGExitStmt
// p_es : AS`ExitStmt
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGExitStmt(const TYPE_AS_ExitStmt & p_es)
{
  SEQ<TYPE_CPP_Stmt> l_cppast;
  SEQ<TYPE_CPP_Expr> l_excArgs;

  if (!p_es.GetField(pos_AS_ExitStmt_expr).IsNil())
  {
    Tuple cgee (CGExprExcl(p_es.GetField(pos_AS_ExitStmt_expr), ASTAUX::MkId(L"exceptionValue"), Nil()));
    const TYPE_CPP_Expr & l_exName (cgee.GetRecord(1));
    const SEQ<TYPE_CPP_Stmt> & l_exStmts (cgee.GetSequence(2));

    l_cppast = l_exStmts;
    l_excArgs.ImpAppend(l_exName);

#ifdef VDMPP
    if (vdm_CPP_isJAVA() && isThrows(GiveCurMASName()))
    {
      Generic ntp (FindType(p_es.GetField(pos_AS_ExitStmt_expr)));
      if (ntp.Is(TAG_TYPE_REP_ObjRefTypeRep))
      {
        const TYPE_AS_Name & clnm (Record(ntp).GetRecord(pos_REP_ObjRefTypeRep_nm));
        TYPE_AS_Name jexcnm (ASTAUX::MkNameFromId(ASTAUX::MkId(L"JDK_Exception"), NilContextId)); 
        if ((clnm == jexcnm) || GetOrderedSupers(clnm).Elems().InSet(jexcnm))
        {
          l_cppast.ImpAppend(vdm_BC_GenExpressionStmt(vdm_BC_GenThrowExpression(l_exName)));
          return l_cppast;
        }
      }
    }
#endif // VDMPP
  }

  TYPE_CPP_Identifier id (vdm_BC_GenIdentifier(ASTAUX::MkId(L"VDMSpecException")));

#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    InsertImport(SEQ<Char>(L"jp.vdmtools.VDM.VDMSpecException"));
    const Generic & l_cause (GetCause());
    TYPE_CPP_TypeSpecifier l_excCls (vdm_BC_GenTypeSpecifier(id));
    if (!l_cause.IsNil()) {
      l_excArgs.ImpAppend(l_cause);
    }
    TYPE_CPP_Expr l_excObj (vdm_BC_GenNewExpr(l_excCls, l_excArgs));
    l_cppast.ImpAppend(vdm_BC_GenExpressionStmt(vdm_BC_GenThrowExpression(l_excObj)));
  }
  else
#endif // VDMPP
  {
    TYPE_CPP_Expr l_excObj (vdm_BC_GenFctCall(id, l_excArgs));
    l_cppast.ImpAppend(vdm_BC_GenExpressionStmt(vdm_BC_GenThrowExpression(l_excObj)));
  }

  return l_cppast;
}

// ConvertBind
// match : AS`PatternBind
// name : AS`Name
// excValNm : CPP`Expr
// ==> AS`Pattern * CGMAIN`VT
Tuple vdmcg::ConvertBind(const TYPE_AS_PatternBind & match, const TYPE_AS_Name & name, const TYPE_CPP_Expr & excValNm)
{
  switch (match.GetTag()) {
    case TAG_TYPE_AS_TypeBind:
    case TAG_TYPE_AS_SetBind: {
      TYPE_CI_ContextId bcid (GetCI().PushCGType(mk_REP_BooleanTypeRep()));
      TYPE_CI_ContextId scid (GetCI().PushCGType(mk_REP_ProductTypeRep(mk_sequence(mk_REP_BooleanTypeRep(),
                                                                                   mk_REP_AllTypeRep()))));
      TYPE_AS_Pattern pat;
      TYPE_AS_Expr cond;
      switch (match.GetTag()) {
        case TAG_TYPE_AS_TypeBind: {
          pat = match.GetRecord(pos_AS_TypeBind_pat);
          cond = TYPE_AS_TypeJudgementExpr().Init( name, match.GetRecord(pos_AS_TypeBind_tp), bcid );
          break;
        }
        case TAG_TYPE_AS_SetBind: {
          pat = match.GetRecord(pos_AS_SetBind_pat);
          cond = TYPE_AS_BinaryExpr().Init( name, Int(INSET), match.GetRecord(pos_AS_SetBind_Set), bcid );
          break;
        }
      }
      TYPE_AS_MatchVal mval;
      mval.Init( cond, bcid );
      TYPE_AS_TuplePattern spat;
      spat.Init( mk_sequence( mval, pat ), scid );
      TYPE_CPP_Expr fnm;
#ifdef VDMPP
      if (vdm_CPP_isJAVA()) {
        fnm = vdm_BC_GenIdentifier(ASTAUX::MkId(L"Tuple.mk_"));
      }
      else
#endif // VDMPP
      {
        fnm = vdm_BC_GenIdentifier(ASTAUX::MkId(L"mk_"));
      }
      TYPE_CPP_Expr tpl (vdm_BC_GenFctCall(fnm, mk_sequence( GenBoolLit_DS(Bool(true)), excValNm)));
      TYPE_CGMAIN_VT vt (mk_CG_VT(tpl, mk_REP_ProductTypeRep(mk_sequence(mk_REP_BooleanTypeRep(),
                                                                         mk_REP_AllTypeRep()))));
      return mk_( spat, vt );
    }
    default: {
      TYPE_CGMAIN_VT vt (mk_CG_VT(excValNm, mk_REP_AllTypeRep()));
      return mk_( match, vt );
    }
  }
}

// CGRecTrapStmt
// p_rts : AS`RecTrapStmt
// p_isLast : bool
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGRecTrapStmt(const TYPE_AS_RecTrapStmt & p_rts, bool p_isLast)
{
  TYPE_CI_ContextId acid (GetCI().PushCGType(mk_REP_AllTypeRep()));
  TYPE_AS_Name l_excValAsNm (ASTAUX::MkNameFromId(ASTAUX::MkId(L"_extVal"), acid)); 
  TYPE_CPP_Identifier l_excValNm (vdm_BC_Rename(l_excValAsNm));

  TYPE_CPP_Identifier l_matchNm (vdm_BC_GiveName(ASTAUX::MkId(L"matched")));
  TYPE_CPP_Identifier l_excRaisedNm (vdm_BC_GiveName(ASTAUX::MkId(L"exceptionRaised")));

  SEQ<TYPE_CPP_Stmt> l_bodyStmts (GenStmt(p_rts.GetRecord(pos_AS_RecTrapStmt_body), p_isLast));
  const SEQ<TYPE_AS_Trap> & p_traps (p_rts.GetSequence(pos_AS_RecTrapStmt_traps));

  SEQ<Sequence> l_traps; // seq of seq of CPP`Stmt
  size_t len_p_traps = p_traps.Length();
  for (size_t idx = 1; idx <= len_p_traps; idx++) {
    l_traps.ImpAppend(GenRecTrapTrap(p_traps[idx].GetRecord(pos_AS_Trap_trappost), false));
  }

  TYPE_CPP_Identifier id (vdm_BC_GenIdentifier(ASTAUX::MkId(L"VDMSpecException")));
  TYPE_CPP_TypeSpecifier l_excType (vdm_BC_GenTypeSpecifier(id));
  TYPE_CPP_ExceptionDeclaration l_exDecl (vdm_BC_GenExceptionDeclaration( mk_sequence(l_excType), l_excValNm));

  SEQ<TYPE_CPP_Stmt> rb;
  rb.ImpAppend(vdm_BC_GenAsgnStmt(l_matchNm, vdm_BC_GenBoolLit(false)));

  for (size_t l_idx = 1; l_idx <= len_p_traps; l_idx++) {
    const TYPE_AS_Trap & l_trap (p_traps[l_idx]);
    const TYPE_AS_PatternBind & l_pat (l_trap.GetRecord(pos_AS_Trap_match));

    Tuple t (ConvertBind( l_pat, l_excValAsNm, l_excValNm ));
    const TYPE_AS_Pattern & pat (t.GetRecord(1));
    const TYPE_CGMAIN_VT & vt (t.GetRecord(2));

    SEQ<TYPE_CPP_Stmt> decls (DeclarePatVars(pat)); // must be before CGPatternMatchExcl

    Tuple cgpme(CGPatternMatchExcl(pat, vt, Set(), l_matchNm, Map(), nil, false));

    SEQ<TYPE_CPP_Stmt> inner_rb (decls);
    inner_rb.ImpConc(cgpme.GetSequence(1));
    if (cgpme.GetBoolValue(2)) { // false : need to check pattern match failed
      inner_rb.ImpAppend(vdm_BC_GenAsgnStmt(l_matchNm, vdm_BC_GenBoolLit(true)));
    }

    if ((l_traps[l_idx].Length() == 1) && l_traps[l_idx][1].Is(TAG_TYPE_CPP_CompoundStmt)) {
      inner_rb.ImpAppend(vdm_BC_GenIfStmt(l_matchNm, l_traps[l_idx][1], nil));
    }
    else {
      inner_rb.ImpAppend(vdm_BC_GenIfStmt(l_matchNm, vdm_BC_GenBlock(l_traps[l_idx]), nil));
    }

    if (l_idx == 1) {
      rb.ImpConc(inner_rb);
    }
    else {
      rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenNot(l_matchNm), vdm_BC_GenBlock(inner_rb), nil));
    }
  }

  rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenNot(l_matchNm), 
                                vdm_BC_GenAsgnStmt(l_excRaisedNm, vdm_BC_GenBoolLit(false)), nil));
  
  SEQ<TYPE_CPP_Stmt> l_excVal;
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    l_excVal.ImpConc(GenObjRefDecl(Nil(), l_excValNm, Nil()));
  }
  else
#endif // VDMPP
    l_excVal.ImpConc(GenDecl_DS(Nil(), l_excValNm, Nil()));

  TYPE_CPP_DeclarationStmt l_match (vdm_BC_GenDecl(GenSmallBoolType(), l_matchNm,
                                                   vdm_BC_GenAsgnInit(vdm_BC_GenBoolLit(true))));
  TYPE_CPP_DeclarationStmt l_excRaised (vdm_BC_GenDecl(GenSmallBoolType(), l_excRaisedNm,
                                                       vdm_BC_GenAsgnInit(vdm_BC_GenBoolLit(false))));
  SEQ<TYPE_CPP_Stmt> l_tryBlockBody;
  l_tryBlockBody.ImpAppend(vdm_BC_GenIfStmt(l_excRaisedNm, vdm_BC_GenBlock(rb), vdm_BC_GenBlock(l_bodyStmts)));

  TYPE_CPP_Handler l_handler (GenRecTrapHandler(l_excValNm, l_excRaisedNm));
  TYPE_CPP_Stmt l_finally (GenRecTrapFinally(l_matchNm, l_excValNm, l_excRaisedNm, p_isLast));

  TYPE_CPP_TryBlock l_tryBlock (vdm_BC_GenTryBlock(l_tryBlockBody, mk_sequence(l_handler), l_finally));
  TYPE_CPP_Stmt l_whileStmt (vdm_BC_GenWhileStmt(vdm_BC_GenBoolLit(true), vdm_BC_GenBlock(mk_sequence(l_tryBlock))));

  SEQ<TYPE_CPP_Stmt> cppast;
  cppast.ImpConc(l_excVal);
  cppast.ImpAppend(l_match);
  cppast.ImpAppend(l_excRaised);
  cppast.ImpAppend(l_whileStmt);
  return cppast;
}

// FlowBrokenSwitchLabels
// lbls : [CPP`SwitchLabels]
// -> bool
bool vdmcg::FlowBrokenSwitchLabels(const Generic & lbls)
{
  if (lbls.IsNil())
    return true;
  else {
    TYPE_CPP_SwitchLabels sl (lbls);
    TYPE_CPP_Stmt slstmt (sl.get_stmt());
    Generic slstmtstmt;
    switch(slstmt.GetTag()) {
      case TAG_TYPE_CPP_CaseStmt: {
        slstmtstmt = slstmt.GetField(pos_CPP_CaseStmt_stmt);
        break;
      }
      case TAG_TYPE_CPP_DefaultStmt: {
        slstmtstmt = slstmt.GetField(pos_CPP_DefaultStmt_stmt);
        break;
      }
      default:
        break;
    } 
    return
      FlowBrokenSwitchLabels (sl.get_labels()) || //FIXME: logic in spec wrong
      (slstmtstmt.IsNil() ? false : FlowBroken(slstmtstmt));
  }
}

// FlowBroken
// stmt : CPP`Stmt
// ==> bool
bool vdmcg::FlowBroken(const TYPE_CPP_Stmt & stmt)
{
  switch(stmt.GetTag()) {
    case TAG_TYPE_CPP_LabelStmt: {
      return FlowBroken(stmt.GetField(pos_CPP_LabelStmt_stmt));
    }
    case TAG_TYPE_CPP_ExpressionStmt: {
      return stmt.GetRecord(pos_CPP_ExpressionStmt_expr).Is(TAG_TYPE_CPP_ThrowExpression);
    }
    case TAG_TYPE_CPP_CompoundStmt: {
      SEQ<TYPE_CPP_Stmt> stms (stmt.GetSequence(pos_CPP_CompoundStmt_stms));
      return FlowBroken (stms[stms.Length()]);
    }
    case TAG_TYPE_CPP_IfStmt: { //FIXME: Logic in spec is incorrect here
      return FlowBroken(stmt.GetField(pos_CPP_IfStmt_alt1)) ||
             FlowBroken(stmt.GetField(pos_CPP_IfStmt_alt2));
    }
    case TAG_TYPE_CPP_SwitchStmt: {
      const TYPE_CPP_SwitchBlock & sb (stmt.GetRecord(pos_CPP_SwitchStmt_block));
      Set gps (sb.get_groups().Elems());
      bool existsBroken = false;
      Generic g; //FIXME: Logic in spec is incorrect here
      for (bool bb = gps.First(g); bb && !existsBroken; bb = gps.Next(g)) {
        TYPE_CPP_SwitchBlockStmtGrp gp (g);
//        existsBroken = existsBroken || FlowBrokenSwitchLabels(gp.get_labels());
        existsBroken = FlowBrokenSwitchLabels(gp.get_labels());
        if (!gp.get_stmt().IsNil())
          existsBroken = existsBroken || FlowBroken(gp.get_stmt());
      }
      return existsBroken || FlowBrokenSwitchLabels(sb.get_labels());
    }
    case TAG_TYPE_CPP_Return: {
      return true;
    }
    case TAG_TYPE_CPP_WhileStmt:
      return FlowBroken(stmt.GetField(pos_CPP_WhileStmt_stmt));
    case TAG_TYPE_CPP_DoWhileStmt:
      return FlowBroken(stmt.GetField(pos_CPP_DoWhileStmt_stmt));
    case TAG_TYPE_CPP_ForStmt:
      return FlowBroken(stmt.GetField(pos_CPP_ForStmt_stmt));
    case TAG_TYPE_CPP_SynchronizedStmt:
      return FlowBroken(stmt.GetField(pos_CPP_SynchronizedStmt_block));
    case TAG_TYPE_CPP_TryBlock: {
      TYPE_CPP_TryBlock tb (stmt);
      SEQ<TYPE_CPP_Handler> hdls (tb.get_hdl());
      bool existsBroken = false;
      Generic g; //FIXME: Logic in spec is incorrect here
      for (bool bb = hdls.First(g); bb && !existsBroken; bb = hdls.Next(g))
//        existsBroken = existsBroken || FlowBroken(TYPE_CPP_Handler(g).get_stmt());
        existsBroken = FlowBroken(TYPE_CPP_Handler(g).get_stmt());
      return existsBroken || FlowBroken(tb.get_stmt());
    }
    default:
      return false;
  }
}

// GenRecTrapTrap
// p_stmts : AS`Stmt
// p_isLast : bool
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenRecTrapTrap(const TYPE_AS_Stmt& p_stmts, bool p_isLast)
{
  SEQ<TYPE_CPP_Stmt> l_trap (GenStmt(p_stmts, false));

  if (!FlowBroken(l_trap[l_trap.Length()]))
    l_trap.ImpAppend(vdm_BC_GenBreakStmt(Nil()));

  return l_trap;
}

// GenRecTrapHandler
// p_val : CPP`Identifier
// p_raised : CPP`Identifier
// ==> CPP`Handler
TYPE_CPP_Handler vdmcg::GenRecTrapHandler(const TYPE_CPP_Identifier & p_val,
                                          const TYPE_CPP_Identifier & p_raised)
{
  TYPE_CPP_Identifier l_excName (vdm_BC_GiveName(ASTAUX::MkId(L"exc")));
  TYPE_CPP_Expr l_getMethod (vdm_BC_GenFctCallObjMemAcc(l_excName, ASTAUX::MkId(L"getValue"), SEQ<TYPE_CPP_Expr>()));
  TYPE_CPP_Stmt l_excValAsgn (vdm_BC_GenAsgnStmt(p_val, l_getMethod));
  TYPE_CPP_Stmt l_excRaisedAsgn (vdm_BC_GenAsgnStmt(p_raised, vdm_BC_GenBoolLit(true)));
  SEQ<TYPE_CPP_Stmt> l_block;
  l_block.ImpAppend(l_excValAsgn);
  l_block.ImpAppend(l_excRaisedAsgn);
  l_block.ImpAppend(vdm_BC_GenContinueStmt(Nil()));

  TYPE_CPP_Stmt l_handlerBlock (vdm_BC_GenBlock(l_block));
  TYPE_CPP_Identifier id (vdm_BC_GenIdentifier(ASTAUX::MkId(L"VDMSpecException")));
  TYPE_CPP_TypeSpecifier l_excType (vdm_BC_GenTypeSpecifier(id));
  TYPE_CPP_ExceptionDeclaration l_exDecl (vdm_BC_GenExceptionDeclaration(
                                            SEQ<TYPE_CPP_TypeSpecifier>().ImpAppend(l_excType),
                                            l_excName));
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    InsertImport(SEQ<Char>(L"jp.vdmtools.VDM.VDMSpecException"));
  }
#endif // VDMPP

  return vdm_BC_GenHandler(l_exDecl, l_handlerBlock);
}

// GenRecTrapFinally
// p_matched : CPP`Identifier
// p_val : CPP`Identifier
// p_raised : CPP`Identifier
// p_isLast : bool
// ==> CPP`CompoundStmt
TYPE_CPP_CompoundStmt vdmcg::GenRecTrapFinally(const TYPE_CPP_Identifier& p_matched,
                                               const TYPE_CPP_Identifier& p_val,
                                               const TYPE_CPP_Identifier& p_raised,
                                               bool p_isLast)
{
  TYPE_CPP_Identifier id (vdm_BC_GenIdentifier(ASTAUX::MkId(L"VDMSpecException")));
  TYPE_CPP_Expr l_excObj;
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    l_excObj = vdm_BC_GenNewExpr(vdm_BC_GenTypeSpecifier(id), mk_sequence(p_val));
  }
  else
#endif // VDMPP
  {
    l_excObj = vdm_BC_GenFctCall(id, mk_sequence(p_val));
  }
        
  TYPE_CPP_Stmt l_throwStmt (vdm_BC_GenExpressionStmt(vdm_BC_GenThrowExpression(l_excObj)));
  TYPE_CPP_Expr l_innerTest (vdm_BC_GenNot(p_raised));
  TYPE_CPP_Expr l_outerTest (vdm_BC_GenNot(p_matched));

  TYPE_CPP_Stmt l_innerIf (vdm_BC_GenIfStmt(l_innerTest, vdm_BC_GenBreakStmt(Nil()), Nil()));
  TYPE_CPP_Stmt l_outerIf (vdm_BC_GenIfStmt(l_outerTest, vdm_BC_GenBlock(mk_sequence(l_throwStmt)), nil));

#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    InsertImport(SEQ<Char>(L"jp.vdmtools.VDM.VDMSpecException"));
  }
#endif // VDMPP

  SEQ<TYPE_CPP_Stmt> rb;
  rb.ImpAppend(l_outerIf);
  if (!p_isLast) {
    rb.ImpAppend(l_innerIf);
  }
  return vdm_BC_GenBlock(rb);
}

// ConvertHandler
// bind : AS`Bind
// stmt : AS`Stmt
// ==> AS`PatternName * AS`CasesStmt
Tuple vdmcg::ConvertHandler(const TYPE_AS_Bind & bind, const TYPE_AS_Stmt & stmt)
{
  TYPE_CI_ContextId bcid (GetCI().PushCGType(mk_REP_BooleanTypeRep()));
  TYPE_CI_ContextId acid (GetCI().PushCGType(mk_REP_AllTypeRep()));
  TYPE_CI_ContextId scid (GetCI().PushCGType(mk_REP_ProductTypeRep(mk_sequence(mk_REP_BooleanTypeRep(),
                                                                               mk_REP_AllTypeRep()))));
  TYPE_AS_Name name (ASTAUX::MkNameFromId(ASTAUX::MkId(L"_tmp"), acid)); 
  TYPE_AS_PatternName pnm;
  pnm.Init(name, Nil(), acid);
  TYPE_AS_TupleConstructorExpr sel;
  sel.Init(mk_sequence( TYPE_AS_BoolLit().Init( Bool(true), bcid ), name), scid );
  TYPE_AS_ExitStmt oths;
  oths.Init( name, NilContextId );
  TYPE_AS_Pattern pat;
  TYPE_AS_Expr cond;
  switch (bind.GetTag()) {
    case TAG_TYPE_AS_TypeBind: {
      pat = bind.GetRecord(pos_AS_TypeBind_pat);
      cond = TYPE_AS_TypeJudgementExpr().Init( name, bind.GetRecord(pos_AS_TypeBind_tp), bcid );
      break;
    }
    case TAG_TYPE_AS_SetBind: {
      pat = bind.GetRecord(pos_AS_SetBind_pat);
      cond = TYPE_AS_BinaryExpr().Init( name, Int(INSET), bind.GetRecord(pos_AS_SetBind_Set), bcid );
      break;
    }
  }
  TYPE_AS_MatchVal mval;
  mval.Init( cond, bcid );
  TYPE_AS_TuplePattern spat;
  spat.Init( mk_sequence( mval, pat ), scid );
  TYPE_AS_CasesStmtAltn altn;
  altn.Init( mk_sequence( spat ), stmt, NilContextId );
  TYPE_AS_CasesStmt cstm;
  cstm.Init( sel, mk_sequence( altn ), oths, NilContextId );
  return mk_( pnm, cstm );
}

// CGTrapStmt
// p_ts : AS`TrapStmt
// p_isLast : bool
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::CGTrapStmt(const TYPE_AS_TrapStmt & p_ts, bool p_isLast)
{
  const TYPE_AS_PatternBind & l_pat (p_ts.GetRecord(pos_AS_TrapStmt_pat));
  const TYPE_AS_Stmt & l_handler    (p_ts.GetRecord(pos_AS_TrapStmt_Post));
  const TYPE_AS_Stmt & l_body       (p_ts.GetRecord(pos_AS_TrapStmt_body));

  TYPE_AS_Pattern pat;
  TYPE_AS_Expr handler;
  switch (l_pat.GetTag()) {
    case TAG_TYPE_AS_TypeBind:
    case TAG_TYPE_AS_SetBind: {
      Tuple t (ConvertHandler(l_pat, l_handler));
      pat = t.GetRecord(1);
      handler = t.GetRecord(2);
      break;
    }
    default: {
      pat = l_pat;
      handler = l_handler;
      break;
    }
  }

  TYPE_CPP_Expr l_excName (vdm_BC_GiveName(ASTAUX::MkId(L"exc")));
  TYPE_CPP_Expr l_excValNm (vdm_BC_GiveName(ASTAUX::MkId(L"excVal")));
  TYPE_CPP_Expr l_matchNm (vdm_BC_GiveName(ASTAUX::MkId(L"matched")));
  TYPE_CPP_Expr l_getMethod (vdm_BC_GenFctCallObjMemAcc(l_excName, ASTAUX::MkId(L"getValue"), SEQ<TYPE_CPP_Expr>()));
  TYPE_CPP_TypeSpecifier l_excType (vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"VDMSpecException"))));
  
  SEQ<TYPE_CPP_Stmt> decls (DeclarePatVars(pat)); // must be before CGPatternMatchExcl

  TYPE_CGMAIN_VT vt (mk_CG_VT(l_excValNm, mk_REP_AllTypeRep()));

  Tuple cgpme(CGPatternMatchExcl(pat, vt, Set(), l_matchNm, Map(), nil, false));
  const SEQ<TYPE_CPP_Stmt> & pm (cgpme.GetSequence(1));
  bool Is_Excl (cgpme.GetBoolValue(2)); // false : need to check pattern match failed

  SEQ<TYPE_CPP_Stmt> pm1;
  if (Is_Excl)
    pm1.ImpConc(pm);
  else {
    pm1.ImpAppend(vdm_BC_GenDecl(GenSmallBoolType(), l_matchNm, vdm_BC_GenAsgnInit(vdm_BC_GenBoolLit(true))));
    pm1.ImpConc(pm);
    pm1.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenNot(l_matchNm),
         vdm_BC_GenBlock(mk_sequence(vdm_BC_GenExpressionStmt(vdm_BC_GenThrowExpression(l_excName)))),
                                   nil));
  }
  SEQ<TYPE_CPP_Stmt> rb;
  rb.ImpConc(GenConstDeclInit(Nil(), l_excValNm, l_getMethod));
  rb.ImpConc(MergeStmts( decls, pm1 ));

  TYPE_CPP_ExceptionDeclaration exDecl (
             vdm_BC_GenExceptionDeclaration(SEQ<TYPE_CPP_TypeSpecifier>().ImpAppend(l_excType), l_excName));

#ifdef VDMSL
  rb.ImpConc(GenStmt(handler, p_isLast));
#endif // VDMSL
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    // TODO : UnionType of Exception
    Generic l_cause = l_excName;
    bool importNeeded = true;
    if (l_pat.Is(TAG_TYPE_AS_TypeBind) &&
        l_pat.GetRecord(pos_AS_TypeBind_pat).Is(TAG_TYPE_AS_PatternName)) {
      const TYPE_AS_Pattern & l_patNm (l_pat.GetRecord(pos_AS_TypeBind_pat));
      const TYPE_AS_Type & tp (l_pat.GetRecord(pos_AS_TypeBind_tp));
      TYPE_REP_TypeRep l_patRepType (FromAS2RepType(tp));
      if (l_patRepType.Is(TAG_TYPE_REP_ObjRefTypeRep)) {
        const TYPE_AS_Name & clnm (l_patRepType.GetRecord(pos_REP_ObjRefTypeRep_nm));
        TYPE_AS_Name jexcnm (ASTAUX::MkNameFromId(ASTAUX::MkId(L"JDK_Exception"), NilContextId)); 
        TYPE_AS_Name jthrnm (ASTAUX::MkNameFromId(ASTAUX::MkId(L"JDK_Throwable"), NilContextId)); 
        if ((clnm == jexcnm) ||
            GetOrderedSupers(clnm).Elems().InSet(jexcnm) ||
            GetOrderedSupers(clnm).Elems().InSet(jthrnm)) {
          TYPE_CPP_Name e (l_patNm.GetField(pos_AS_PatternName_nm).IsNil()
                              ? vdm_BC_GiveName (ASTAUX::MkId(L"cause"))
                              : vdm_BC_Rename(l_patNm.GetField(pos_AS_PatternName_nm)));
          TYPE_CPP_TypeSpecifier l_patCppType (Rep2CppTypeSpecifier(l_patRepType, tp));
          rb.Clear();
          exDecl = vdm_BC_GenExceptionDeclaration(mk_sequence(l_patCppType), e);
          l_cause = e;
          importNeeded = false;
        }
      }
    }
    if (importNeeded) {
      InsertImport(SEQ<Char>(L"jp.vdmtools.VDM.VDMSpecException"));
    }

    PushCause(l_cause);
    rb.ImpConc(GenStmt(handler, p_isLast));
    PopCause();
  }
  else {
    rb.ImpConc(GenStmt(handler, p_isLast));
  }
#endif // VDMPP

  TYPE_CPP_Stmt handlerBlock (vdm_BC_GenBlock(rb));
  SEQ<TYPE_CPP_Stmt> bodyStmt (GenStmt(l_body, p_isLast));
  SEQ<TYPE_CPP_Handler> handlers;
  handlers.ImpAppend(vdm_BC_GenHandler(exDecl, handlerBlock));

  SEQ<TYPE_CPP_Stmt> result;
  result.ImpAppend(vdm_BC_GenTryBlock(bodyStmt, handlers, Nil()));
  return result;
}

#ifdef VDMPP
// GenInvCall
// nm : AS`Name
// stat : bool
// -> CPP`Stmt
TYPE_CPP_Stmt vdmcg::GenInvCall(const TYPE_AS_Name & nm, bool stat)
{
  TYPE_AS_Id invid (ASTAUX::MkId(L"inv_").ImpConc(ASTAUX::GetFirstId(nm)));
  TYPE_CPP_Name invname (vdm_BC_Rename(ASTAUX::MkNameFromId(invid, nm.get_cid())));

  TYPE_CPP_Expr invref;
  if (stat) {
    invref = vdm_BC_GenFctCall(invname, SEQ<TYPE_CPP_Expr>());
  }
  else
  {
    if (vdm_CPP_isCPP()) {
      invref = vdm_BC_GenFctCallPtrToObjMemAcc(GenThis(), invname, SEQ<TYPE_CPP_Expr>());
    }
    else { // java
      invref = vdm_BC_GenFctCall(vdm_BC_GenObjectMemberAccess(GenThis(), invname), SEQ<TYPE_CPP_Expr>());
    }
  }

  TYPE_CPP_Expr cond (GenGetValue(invref, mk_REP_BooleanTypeRep()));
  return vdm_BC_GenIfStmt(vdm_BC_GenNot(cond),
      vdm_BC_GenBlock(mk_sequence(RunTime(L"Instance invariant failure in " + ASTAUX::ASName2String(nm)))), nil);
}
#endif // VDMPP

