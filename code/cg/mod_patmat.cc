/***
*  * WHAT
*  *    VDM Code Generator.
*  *    mod_patmat.cc: Implementation of mod_patmat.vdm 1.40
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/mod_patmat.cc,v $
*  * VERSION
*  *    $Revision: 1.28 $
*  * DATE
*  *    $Date: 2001/10/05 10:57:55 $
*  * STATUS
*  *    $State: Exp $
*  * PROJECT
*  *    Afrodite - ESPRIT programme, project no. 6500.
*  * AUTHOR
*  *    Poul Boegh Lassen (PBL) + jojo
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "vdmcg.h"
#include "asquotes.h"
#include "astaux.h"
#include "BC.h"
#include "statsem.h"
//#include "mini-p-print.h"
#include "intconvquotes.h"

//wcout << L"intertp: " << INT2Q::h2gAS(intertp) << endl;

// ContainsNotSuppConstruct
// ==> bool
bool vdmcg::ContainsNotSuppConstruct()
{
  return this->ContainsNotSupportedConstruct;
}

// GetNotSuppStmt
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GetNotSuppStmt()
{
  return this->NotSupportedStmt;
}

// InitState_PM (InitState)
// ==> ()
void vdmcg::InitState_PM()
{
  this->ContainsNotSupportedConstruct = false;
  this->NotSupportedStmt = SEQ<TYPE_CPP_Stmt>();
}

// CGPatternMatchExcl
// p : AS`Pattern
// cg_vt : CGMAIN`VT
// pn_s : set of (AS`Name | AS`OldName)
// succ_v : CPP`Name
// pid_mvt
// inner : [seq of CPP`Stmt]
// ==> seq of CPP`Stmt * bool   // true :  not need check succ_v, false need check succ_v
Tuple vdmcg::CGPatternMatchExcl(const TYPE_AS_Pattern & p,
                                const TYPE_CGMAIN_VT & cg_vt,
                                const Set & pn_s,              // already matched
                                const TYPE_CPP_Name & succ_v,
                                const Map & pid_m,
                                const Generic & inner,
                                bool nonstop)
{
  const TYPE_CPP_Expr & varExpr_v (cg_vt.GetRecord(pos_CGMAIN_VT_name));
  const Generic & type (cg_vt.GetField(pos_CGMAIN_VT_type));

  TYPE_CPP_Expr tmpVar_v (varExpr_v);
  SEQ<TYPE_CPP_Stmt> pm;
  switch (p.GetTag()) {
    case TAG_TYPE_AS_SetEnumPattern:
    case TAG_TYPE_AS_SeqEnumPattern:
    case TAG_TYPE_AS_MapEnumPattern:
    case TAG_TYPE_AS_RecordPattern:
    case TAG_TYPE_AS_TuplePattern:
#ifdef VDMPP
    case TAG_TYPE_AS_ObjectPattern:
#endif // VDMPP
    case TAG_TYPE_AS_SeqConcPattern: {
      if (!varExpr_v.Is(TAG_TYPE_CPP_Identifier)) {
        tmpVar_v = vdm_BC_GiveName(ASTAUX::MkId(L"tmpVar"));
        pm.ImpConc(GenConstDeclInit(type, tmpVar_v, varExpr_v));
      }
      break;
    }
    case TAG_TYPE_AS_PatternName:
    case TAG_TYPE_AS_MatchVal:
    case TAG_TYPE_AS_SetUnionPattern:
    case TAG_TYPE_AS_MapMergePattern:
    default: {
      break;
    }
  }
  Tuple cgpm (CGPatternMatch(p, mk_CG_VT(tmpVar_v, type), pn_s, succ_v, pid_m, inner, nonstop));
  const SEQ<TYPE_CPP_Stmt> & pm0 (cgpm.GetSequence(1));
  const Bool & Is_excl (cgpm.GetBool(2));
  pm.ImpConc(pm0);
  return mk_(pm, Is_excl);
}

// CGPatternMatch
// p : AS`Pattern
// varExpr_v : CGMAIN`VT
// pn_s : set of (AS`Name | AS`OldName)
// succ_v : CPP`Name
// pid_m : [map AS`Name to REP`TypeRep]
// inner : [seq of CPP`Stmt]
// ==> seq of CPP`Stmt * bool
Tuple vdmcg::CGPatternMatch (const TYPE_AS_Pattern & p,
                             const TYPE_CGMAIN_VT & varExpr_v,
                             const Set & pn_s,
                             const TYPE_CPP_Name & succ_v,
                             const Map & pid_m,
                             const Generic & inner,
                             bool nonstop)
{
  Tuple res;
  switch (p.GetTag ()) {
    case TAG_TYPE_AS_PatternName: {
      res = CGMatchPatternName (p, varExpr_v, pn_s, succ_v, pid_m, inner, nonstop);
      break;
    }
    case TAG_TYPE_AS_MatchVal: {
      res = CGMatchVal (p, varExpr_v, pn_s, succ_v, inner, nonstop);
      break;
    }
    case TAG_TYPE_AS_SetEnumPattern: {
      res = CGMatchSetEnumPattern (p, varExpr_v, pn_s, succ_v, pid_m, inner, nonstop);
      break;
    }
    case TAG_TYPE_AS_SeqEnumPattern: {
      res = CGMatchSeqEnumPattern (p, varExpr_v, pn_s, succ_v, pid_m, inner, nonstop);
      break;
    }
    case TAG_TYPE_AS_MapEnumPattern: {
      res = CGMatchMapEnumPattern (p, varExpr_v, pn_s, succ_v, pid_m, inner, nonstop);
      break;
    }
    case TAG_TYPE_AS_RecordPattern: {
      res = CGMatchRecordPattern (p, varExpr_v, pn_s, succ_v, pid_m, inner, nonstop);
      break;
    }
    case TAG_TYPE_AS_TuplePattern: {
      res = CGMatchTuplePattern (p, varExpr_v, pn_s, succ_v, pid_m, inner, nonstop);
      break;
    }
    case TAG_TYPE_AS_SetUnionPattern: {
      TYPE_AS_Pattern pat (FlattenSetUnionPattern(p));
      if (pat.Is(TAG_TYPE_AS_SetEnumPattern)) {
        res = CGMatchSetEnumPattern (pat, varExpr_v, pn_s, succ_v, pid_m, inner, nonstop);
      }
      else {
        res = CGMatchSetUnionPattern (pat, varExpr_v, pn_s, succ_v, pid_m, inner, nonstop);
      }
      break;
    }
    case TAG_TYPE_AS_SeqConcPattern: {
      TYPE_AS_Pattern pat (FlattenSeqConcPattern(p));
      if (pat.Is(TAG_TYPE_AS_SeqEnumPattern)) {
        res = CGMatchSeqEnumPattern (pat, varExpr_v, pn_s, succ_v, pid_m, inner, nonstop);
      }
      else {
        res = CGMatchSeqConcPattern (pat, varExpr_v, pn_s, succ_v, pid_m, inner, nonstop);
      }
      break;
    }
    case TAG_TYPE_AS_MapMergePattern: {
      TYPE_AS_Pattern pat (FlattenMapMergePattern(p));
      if (pat.Is(TAG_TYPE_AS_MapEnumPattern)) {
        res = CGMatchMapEnumPattern (pat, varExpr_v, pn_s, succ_v, pid_m, inner, nonstop);
      }
      else {
        res = CGMatchMapMergePattern (pat, varExpr_v, pn_s, succ_v, pid_m, inner, nonstop);
      }
      break;
    }
#ifdef VDMPP
    case TAG_TYPE_AS_ObjectPattern: {
      res = CGMatchObjectPattern (p, varExpr_v, pn_s, succ_v, pid_m, inner, nonstop);
      break;
    }
#endif // VDMPP
    default: {
      ReportError (L"CGPatternMatch");
      break;
    }
  }
  return res;
}

// CGMatchPatternName
// p : AS`PatternName
// rc : CGMAIN`VT
// pn_s : set of (AS`Name |AS`OldName)
// succ_v : CPP`Name
// pid_m : [map (AS`Name | AS`OldName) to REP`TypeRep]
// inner : [seq of CPP`Stmt]
// ==> seq of CPP`Stmt * bool
Tuple vdmcg::CGMatchPatternName (const TYPE_AS_PatternName & patnm,
                                 const TYPE_CGMAIN_VT & rc,
                                 const Set & pn_s,
                                 const TYPE_CPP_Name & succ_v,
                                 const Map & pid_m,
                                 const Generic & inner,
                                 bool nonstop)
{
  const Generic & n (patnm.GetField(pos_AS_PatternName_nm));
  const TYPE_CPP_Expr & varExpr_v (rc.GetRecord(pos_CGMAIN_VT_name));
  const Generic & type (rc.GetField(pos_CGMAIN_VT_type));

  if (!n.IsNil ()) {
    if (pn_s.InSet (n)) { // patern is already binded
      if (!inner.IsNil()) {
        if (nonstop) {
          SEQ<TYPE_CPP_Stmt> rb;
          rb.ImpAppend(vdm_BC_GenIfStmt(GenEqVal(vdm_BC_Rename(n), varExpr_v), vdm_BC_GenBlock(inner), nil));
          return mk_(rb, Bool(true));
        }
        else {
          SEQ<TYPE_CPP_Stmt> rb;
          rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenAsgnExpr(succ_v,
                                               vdm_BC_GenBracketedExpr(GenEqVal(vdm_BC_Rename(n), varExpr_v))),
                                        vdm_BC_GenBlock(inner), nil));
          return mk_(rb, Bool(false));
        }
      }
      else {
        SEQ<TYPE_CPP_Stmt> rb;
        rb.ImpAppend(vdm_BC_GenAsgnStmt(succ_v, vdm_BC_GenBracketedExpr(GenEqVal(vdm_BC_Rename(n), varExpr_v))));
        return mk_(rb, Bool(false));
      }
    }
    //else if ( !pid_m.DomExists(n) || (pid_m.DomExists(n) && (pid_m[n] == type)) )
    else if ( !pid_m.DomExists(n) || (pid_m.DomExists(n) && MatchType(pid_m[n], type)) )
    { // patern is not binded yet
      // type for pattern is't defined or equals defind type

      SEQ<TYPE_CPP_Stmt> rb;
      TYPE_CPP_Expr varExpr_v_q (varExpr_v);
      TYPE_REP_TypeRep pttp (pid_m.DomExists(n) ? pid_m[n] : FindType(patnm));
      if (!IsSubType(type, pttp)) {
        TYPE_CPP_Identifier e1_v (vdm_BC_GiveName(ASTAUX::MkId(L"e1")));
        varExpr_v_q = e1_v;
        rb.ImpConc(GenConstDeclInit(type, e1_v, varExpr_v));
        rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenNot(GenAuxType(e1_v, pttp)),
                                      vdm_BC_GenBlock(mk_sequence(RunTime(L"Incompatible type"))),
                                      Nil()));
      }

#ifdef VDMPP
      if (vdm_CPP_isJAVA()) {
        if (IsStringType(pttp)) {
          if (varExpr_v.Is(TAG_TYPE_CPP_ClassInstanceCreationExpr) &&
              varExpr_v.GetRecord(pos_CPP_ClassInstanceCreationExpr_classtype) == GenStringType().get_tp()){
            const SEQ<TYPE_CPP_Expr> & arg (varExpr_v.GetSequence(pos_CPP_ClassInstanceCreationExpr_arg));
            if (arg.Length() == 1) {
              if (arg[1].Is(TAG_TYPE_CPP_WStringLit)) {
                varExpr_v_q = arg[1];
              }
            }
          }
        }
        rb.ImpAppend(vdm_BC_GenAsgnStmt(vdm_BC_Rename(n), GenExplicitCast(pttp, varExpr_v_q, type)));
      }
      else
#endif // VDMPP
      {
        rb.ImpAppend(vdm_BC_GenAsgnStmt(vdm_BC_Rename(n), varExpr_v_q));
      }

      if (!inner.IsNil()) {
        rb.ImpConc(inner);
      }
      return mk_(rb, Bool(true));
    }
    else
    { // (pid_m.DomExists(n) && (pid_m[n] != type))
      // type for pattern is defined and not equals defind type
      TYPE_AS_Expr newVarExpr_v (varExpr_v); 
      if (!type.IsNil()) {
        TYPE_REP_TypeRep t (type);
        switch (t.GetTag()) {
          case TAG_TYPE_REP_NumericTypeRep: {
            switch(t.GetIntValue(pos_REP_NumericTypeRep_qtp)) {
              case INTEGER:
              case NAT:
              case NATONE: {
                newVarExpr_v = RemoveIntCast(varExpr_v);
              break;
              }
            }
            break;
          }
        }
      }

      TYPE_REP_TypeRep tt;
      if (type.IsNil() || IsSubType(pid_m[n], type)) {
        tt = pid_m[n];
      }
      else {
        tt = type;
      } 
      TYPE_CPP_Expr istype (GenIsType(newVarExpr_v, tt));
      TYPE_REP_BooleanTypeRep btype;
      TYPE_CPP_Expr cond (GenGetValue(istype, btype));
      SEQ<TYPE_CPP_Stmt> alt1;

#ifdef VDMPP
      if (vdm_CPP_isJAVA()) {
        TYPE_REP_TypeRep resType (pid_m.DomExists(n) ? pid_m[n] : FindType(patnm));
        alt1.ImpAppend(vdm_BC_GenAsgnStmt(vdm_BC_Rename(n), GenExplicitCast(resType, varExpr_v, type)));
      }
      else
#endif // VDMPP
      {
        alt1.ImpAppend(vdm_BC_GenAsgnStmt(vdm_BC_Rename(n), varExpr_v));
      }
 
      if (!inner.IsNil()) {
        alt1.ImpConc(inner);
        SEQ<TYPE_CPP_Stmt> rb;
        rb.ImpAppend(vdm_BC_GenIfStmt(cond, vdm_BC_GenBlock(alt1), nil));
        return mk_(rb, Bool(true));
      }
      else {
        SEQ<TYPE_CPP_Stmt> rb;
        rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenAsgnExpr(succ_v, cond), vdm_BC_GenBlock(alt1), nil));
        return mk_(rb, Bool(false));
      }
    }
  }
  else { // n = nil // match always
    SEQ<TYPE_CPP_Stmt> rb;
    if (!inner.IsNil()) {
      rb.ImpConc(inner);
    }
    return mk_(rb, Bool(true));
  }
}

// MatchType
// ptp : REP`TypeRep
// vtp : REP`TypeRep
// -> bool
bool vdmcg::MatchType(const TYPE_REP_TypeRep & ptp, const TYPE_REP_TypeRep & vtp)
{
  if (ptp == vtp) {
    return true;
  }
  else if (ptp.Is(TAG_TYPE_REP_UnionTypeRep) && vtp.Is(TAG_TYPE_REP_UnionTypeRep)) {
    const SET<TYPE_REP_TypeRep> & ptps (ptp.GetSet(pos_REP_UnionTypeRep_tps));
    SET<TYPE_REP_TypeRep> vtps (vtp.GetSet(pos_REP_UnionTypeRep_tps));
    bool forall = (ptps.Card() == vtps.Card());
    Generic t;
    for (bool bb = vtps.First(t); bb && forall; bb = vtps.Next(t)) {
      Generic tp (RemoveInvType(t));
      forall = ptps.InSet(tp.Is(TAG_TYPE_REP_CompositeTypeRep) ? tp : t);
    }
    return forall;
  }
  else {
    return false;
  }
}

// CGMatchVal
// p : AS`MatchVal
// rc : CGMAIN`VT
// - : set of (AS`Name | AS`OldName)
// succ_v : CPP`Name
// inner : [seq of CPP`Stmt]
// ==> seq of CPP`Stmt * bool
Tuple vdmcg::CGMatchVal (const TYPE_AS_MatchVal & p,
                         const TYPE_CGMAIN_VT & rc,
                         const Set &,
                         const TYPE_CPP_Name & succ_v,
                         const Generic & inner,
                         bool nonstop)
{
  const TYPE_AS_Expr & e (p.GetRecord(pos_AS_MatchVal_val));
  const TYPE_CPP_Expr & varExpr_v (rc.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & type (rc.GetRecord(pos_CGMAIN_VT_type));

  Tuple cgee (CGExprExcl(e, ASTAUX::MkId(L"tmpVal_v"), nil));
  const TYPE_AS_Expr & tmpVal_v (cgee.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & e_stmt (cgee.GetSequence(2));

  TYPE_CPP_Expr cond (GenEqVal(varExpr_v, tmpVal_v));

  switch (e.GetTag()) {
    case TAG_TYPE_AS_SeqEnumerationExpr: {
      if (e.GetSequence(pos_AS_SeqEnumerationExpr_els).IsEmpty()) {
#ifdef VDMPP
        if (vdm_CPP_isJAVA()) {
          if (IsStringType(type)) {
            if ((varExpr_v.Is(TAG_TYPE_CPP_Identifier)) || (varExpr_v.Is(TAG_TYPE_CPP_FctCall))) {
              cond = GenSeqIsEmpty(varExpr_v);
            }
            else {
              cond = GenSeqIsEmpty(GenCastString(varExpr_v));
            }
          }
          else if (IsSeqType(type)) {
            if ((varExpr_v.Is(TAG_TYPE_CPP_Identifier)) || (varExpr_v.Is(TAG_TYPE_CPP_FctCall))) {
              cond = GenSeqIsEmpty(varExpr_v);
            }
            else {
              cond = GenSeqIsEmpty(GenCastSeq(varExpr_v, Nil()));
            }
          }
          else {
            cond = vdm_BC_GenLogOr(vdm_BC_GenLogAnd(GenIsSeq(varExpr_v),
                                                    GenSeqIsEmpty(GenCastSeq(varExpr_v, Nil()))),
                                   vdm_BC_GenLogAnd(GenIsString(varExpr_v),
                                                    GenSeqIsEmpty(GenCastString(varExpr_v))));
          }
        }
        else
#endif // VDMPP
        {
          if (IsSeqType(type)) {
            if ((varExpr_v.Is(TAG_TYPE_CPP_Identifier)) || (varExpr_v.Is(TAG_TYPE_CPP_FctCall))) {
              cond = GenSeqIsEmpty(varExpr_v);
            }
            else {
              cond = GenSeqIsEmpty(GenCastSeq(varExpr_v, Nil()));
            }
          }
          else {
            cond = vdm_BC_GenLogAnd(GenIsSeq(varExpr_v), GenSeqIsEmpty(GenCastSeq(varExpr_v, Nil())));
          }
        }
      }
      break;
    }
    case TAG_TYPE_AS_SetEnumerationExpr: {
      if (e.GetSequence(pos_AS_SetEnumerationExpr_els).IsEmpty()) {
        if (IsSetType(type)) {
          if ((varExpr_v.Is(TAG_TYPE_CPP_Identifier)) || (varExpr_v.Is(TAG_TYPE_CPP_FctCall)))
            cond = GenSetIsEmpty(varExpr_v);
          else
            cond = GenSetIsEmpty(GenCastSetType(varExpr_v));
        }
        else
          cond = vdm_BC_GenLogAnd(GenIsSet(varExpr_v), GenSetIsEmpty(GenCastSetType(varExpr_v)));
      }
      break;
    }
    case TAG_TYPE_AS_MapEnumerationExpr: {
      if (e.GetSequence(pos_AS_SetEnumerationExpr_els).IsEmpty()) {
        if (IsMapType(type)) {
          if ((varExpr_v.Is(TAG_TYPE_CPP_Identifier)) || (varExpr_v.Is(TAG_TYPE_CPP_FctCall)))
            cond = GenMapIsEmpty(varExpr_v);
          else
            cond = GenMapIsEmpty(GenCastMap(varExpr_v));
        }
        else
          cond = vdm_BC_GenLogAnd(GenIsMap(varExpr_v), GenMapIsEmpty(GenCastMap(varExpr_v)));
      }
      break;
    }
    default: {
      TYPE_REP_TypeRep tp (FindType(e));
      if (IsBoolType(tp) && IsBoolType(type)) {
#ifdef VDMPP
        if (vdm_CPP_isJAVA()) {
          if (varExpr_v.Is(TAG_TYPE_CPP_ObjectMemberAccess)) {
            if ((varExpr_v.GetRecord(pos_CPP_ObjectMemberAccess_object) == vdm_BC_GenIdentifier(ASTAUX::MkId(L"Boolean")))) {
              if (varExpr_v.GetRecord(pos_CPP_ObjectMemberAccess_name) == vdm_BC_GenIdentifier(ASTAUX::MkId(L"TRUE"))) {
                cond = GenGetValue(tmpVal_v, mk_REP_BooleanTypeRep());
              }
              else if (varExpr_v.GetRecord(pos_CPP_ObjectMemberAccess_name) == vdm_BC_GenIdentifier(ASTAUX::MkId(L"FALSE"))) {
                cond = vdm_BC_GenNot(GenGetValue(tmpVal_v, mk_REP_BooleanTypeRep()));
              }
            }
          }
        }
        else
#endif // VDMPP
        { // C++
          if (varExpr_v.Is(TAG_TYPE_CPP_FctCall)) {
            if(varExpr_v.GetRecord(pos_CPP_FctCall_fct) == vdm_BC_GenIdentifier(ASTAUX::MkId(L"Bool"))) {
              const SEQ<TYPE_CPP_Expr> & arg (varExpr_v.GetSequence(pos_CPP_FctCall_arg));
              if (arg.Length() == 1) {
                const TYPE_CPP_Expr & b (arg[1]);
                if (b.Is(TAG_TYPE_CPP_BoolLit)) {
                  bool bv = b.GetBoolValue(pos_CPP_BoolLit_val);
                  if (bv) {
                    cond = GenGetValue(tmpVal_v, mk_REP_BooleanTypeRep());
                  } else {
                    cond = vdm_BC_GenNot(GenGetValue(tmpVal_v, mk_REP_BooleanTypeRep()));
                  }
                }
              }
            }
          }
        }
      }
      break;
    }
  }

  if (!inner.IsNil()) {
    if (nonstop) {
      SEQ<TYPE_CPP_Stmt> rb (e_stmt);
      rb.ImpAppend(vdm_BC_GenIfStmt(cond, vdm_BC_GenBlock(inner), nil));
      return mk_(rb, Bool(true));
    }
    else {
      SEQ<TYPE_CPP_Stmt> rb (e_stmt);
      rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenAsgnExpr(succ_v, vdm_BC_GenBracketedExpr(cond)),
                                    vdm_BC_GenBlock(inner), nil));
      return mk_(rb, Bool(false));
    }
  }
  else {
    SEQ<TYPE_CPP_Stmt> rb (e_stmt);
    rb.ImpAppend(vdm_BC_GenAsgnStmt(succ_v, vdm_BC_GenBracketedExpr(cond)));
    return mk_(rb, Bool(false));
  }
}

// CGMatchSetEnumPattern
// p :AS`SetEnumPattern
// rc : CGMAIN`VT
// pn_s : set of (AS`Name | AS`OldName)
// succ_v : CPP`Name
// pid_m : [map (AS`Name | AS`OldName) to REP`TypeRep]
// inner : [seq of CPP`Stmt]
// ==> seq of CPP`Stmt * bool
Tuple vdmcg::CGMatchSetEnumPattern (const TYPE_AS_SetEnumPattern & pat,
                                    const TYPE_CGMAIN_VT & rc,
                                    const Set & pn_s,
                                    const TYPE_CPP_Name & succ_v,
                                    const Map & pid_m,
                                    const Generic & inner,
                                    bool nonstop)
{
  const TYPE_CPP_Expr & varExpr_v (rc.GetRecord(pos_CGMAIN_VT_name));
  const Generic & type (rc.GetField(pos_CGMAIN_VT_type));

  if (!type.IsNil() && !IsPosSetType(type)) {
    SEQ<TYPE_CPP_Stmt> rb;
    rb.ImpAppend(vdm_BC_GenSingleLineComments(SEQ<Char>(L"never match")));
    rb.ImpAppend(vdm_BC_GenAsgnStmt(succ_v, vdm_BC_GenBoolLit(false)));
    return mk_(rb, Bool(false));
  }

  SEQ<TYPE_CPP_Stmt> decls;
  TYPE_CPP_Expr varExpr_v_q (varExpr_v);
  if (!varExpr_v.Is(TAG_TYPE_CPP_Identifier)) {
    varExpr_v_q = vdm_BC_GiveName(ASTAUX::MkId(L"tmpVal_v"));
    decls.ImpConc(GenDeclInit_DS(type, varExpr_v_q, varExpr_v));
  }
  
  TYPE_CPP_Expr castVarExpr_v (IsSetType(type) ? varExpr_v_q : GenCastSetType(varExpr_v_q));

  const SEQ<TYPE_AS_Pattern> & p_l (pat.GetSequence(pos_AS_SetEnumPattern_Elems));

  size_t len_p_l = p_l.Length();
  bool all_nilname = true;
  for (size_t idx = 1; idx <= len_p_l; idx++) {
    const TYPE_AS_Pattern & p (p_l[idx]);
    if (!(p.Is(TAG_TYPE_AS_PatternName) && p.GetField(pos_AS_PatternName_nm).IsNil()))
      all_nilname = false;
  }

  if (all_nilname) {
    // special case: { - }, { -, - }, ...
    // check length only
    TYPE_CPP_Expr set_size (GenCard_int(castVarExpr_v));
    TYPE_CPP_Expr if_cond (vdm_BC_GenEq(vdm_BC_GenIntegerLit (len_p_l), set_size));
    TYPE_CPP_Stmt stmt;
    if (!inner.IsNil()) {
      //stmt = vdm_BC_GenIfStmt(if_cond, vdm_BC_GenBlock(inner), nil);
      if (nonstop) {
        stmt = vdm_BC_GenIfStmt(if_cond, vdm_BC_GenBlock(inner), nil);
      }
      else {
        stmt = vdm_BC_GenIfStmt(vdm_BC_GenAsgnExpr(succ_v, vdm_BC_GenBracketedExpr(if_cond)),
                                vdm_BC_GenBlock(inner), nil);
      }
    }
    else {
      stmt = vdm_BC_GenAsgnStmt(succ_v, vdm_BC_GenBracketedExpr(if_cond));
    }

    if (IsSetType(type)) {
      SEQ<TYPE_CPP_Stmt> rb (decls);
      rb.ImpAppend(stmt);
      return mk_(rb, Bool(!inner.IsNil() && nonstop)); 
    }
    else {
      SEQ<TYPE_CPP_Stmt> rb (decls);
      rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenAsgnExpr(succ_v, vdm_BC_GenBracketedExpr(GenIsSet(varExpr_v_q))),
                                    stmt, nil));
      return mk_(rb, Bool(false)); 
    }
  }

  TYPE_REP_TypeRep p_type (FindSetElemType(type));

  TYPE_CPP_Identifier valSeq_v (vdm_BC_GiveName(ASTAUX::MkId(L"valSeq")));
  TYPE_CPP_Identifier perm_v (vdm_BC_GiveName(ASTAUX::MkId(L"perm")));
  TYPE_REP_TypeRep perm_t (mk_REP_SeqTypeRep (p_type));
  TYPE_CPP_Identifier tmpvarExpr_v (vdm_BC_GiveName(ASTAUX::MkId(L"tmpvarExpr")));
  TYPE_REP_SetTypeRep newsp_type (mk_REP_SetTypeRep (p_type));

  if (len_p_l == 1) {
    TYPE_CPP_Identifier elem_v (vdm_BC_GiveName(ASTAUX::MkId(L"elem")));
    Tuple cgpme (CGPatternMatchExcl(p_l[1], mk_CG_VT(elem_v, p_type), pn_s, succ_v, pid_m, inner, nonstop));
    const SEQ<TYPE_CPP_Stmt> & pm (cgpme.GetSequence(1));
    const Bool & Is_excl (cgpme.GetBool(2));

    TYPE_CPP_Expr e;
#ifdef VDMPP
    if (vdm_CPP_isJAVA()) {
      e = GenExplicitCast(p_type, GenSetGetElemExpr(castVarExpr_v), Nil());
    }
    else
#endif // VDMPP
    {
      e = GenSetGetElemExpr(castVarExpr_v);
    }

    SEQ<TYPE_CPP_Stmt> ifBody;
    ifBody.ImpConc(GenDeclInit_DS(p_type, elem_v, e));
    ifBody.ImpConc(pm);

    TYPE_CPP_Expr cond (vdm_BC_GenEq (vdm_BC_GenIntegerLit (len_p_l), GenCard_int (castVarExpr_v)));
    if (!IsSetType(type)) {
      cond = vdm_BC_GenLogAnd(vdm_BC_GenBracketedExpr(GenIsSet(varExpr_v_q)), vdm_BC_GenBracketedExpr(cond));
    }

    if (Is_excl && nonstop) {
      SEQ<TYPE_CPP_Stmt> rb (decls);
      rb.ImpAppend(vdm_BC_GenIfStmt(cond, vdm_BC_GenBlock(ifBody), nil));
      return mk_(rb, Bool(true));
    }
    else {
      SEQ<TYPE_CPP_Stmt> rb (decls);
      rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenAsgnExpr(succ_v, vdm_BC_GenBracketedExpr(cond)),
                                    vdm_BC_GenBlock(ifBody), nil));
      return mk_(rb, Bool(false));
    }
  }
  else
  { // len_p_l > 1
    TYPE_CPP_Identifier permSL_v (vdm_BC_GiveName(ASTAUX::MkId(L"permSL")));
    TYPE_REP_TypeRep permSL_t (mk_REP_SetTypeRep (mk_REP_SeqTypeRep (p_type)));
    Tuple cgml (CGMatchList (p_l, mk_CG_VT (perm_v, perm_t), succ_v, pn_s, pid_m, inner, false));
    const SEQ<TYPE_CPP_Stmt> & pm (cgml.GetSequence(1));
    const Bool & Is_excl (cgml.GetBool(2)); // 20120406

    SEQ<TYPE_CPP_Stmt> inner_rb;
    if (Is_excl) {
      inner_rb.ImpAppend(vdm_BC_GenAsgnStmt(succ_v, vdm_BC_GenBoolLit(true)));
    }
    inner_rb.ImpConc(pm);

    SEQ<TYPE_CPP_Stmt> ifBody;
    ifBody.ImpConc (GenDeclSet (tmpvarExpr_v, castVarExpr_v));
    ifBody.ImpConc(GenValSetToSeq (valSeq_v, tmpvarExpr_v, newsp_type));
    ifBody.ImpConc (GenDeclSet (permSL_v, GenPermute(valSeq_v)));

    ifBody.ImpAppend(vdm_BC_GenAsgnStmt(succ_v, vdm_BC_GenBoolLit(false)));
    ifBody.ImpConc (GenIterSet (mk_CG_VT (permSL_v, permSL_t),
                                vdm_BC_GenNot(succ_v),
                                mk_CG_VT (perm_v, perm_t), inner_rb));

    TYPE_CPP_Expr cond (vdm_BC_GenEq (vdm_BC_GenIntegerLit (len_p_l), GenCard_int (castVarExpr_v)));
    if (!IsSetType(type)) {
      cond = vdm_BC_GenLogAnd(vdm_BC_GenBracketedExpr(GenIsSet(varExpr_v_q)), vdm_BC_GenBracketedExpr(cond));
    }
    SEQ<TYPE_CPP_Stmt> rb (decls);
    rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenAsgnExpr(succ_v, vdm_BC_GenBracketedExpr(cond)),
                                  vdm_BC_GenBlock(ifBody), nil));
    return mk_(rb, Bool(false));
  }
}

// GenValSetToSeq
// tovar_v : CPP`Name
// fromvar_v : CPP`Name
// str : REP`SetTypeRep
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenValSetToSeq (const TYPE_CPP_Name & tovar_v,
                                          const TYPE_CPP_Expr & fromvar_v,
                                          const TYPE_REP_SetTypeRep & str)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return GenDeclSeq(tovar_v, fromvar_v);
  }
  else
#endif // VDMPP
  {
    return GenDeclSeq(tovar_v, vdm_BC_GenFctCallObjMemAcc(fromvar_v,
                                                          ASTAUX::MkId(L"ToSequence"),
                                                          SEQ<TYPE_CPP_Expr>()));
  }
}

// CGMatchSeqEnumPattern
// pat : AS`SeqEnumPattern
// rc2 : CGMAIN`VT
// pn_s : set of (AS`Name | AS`OldName)
// succ_v : CPP`Name
// pid_m : [map (AS`Name | AS`OldName) to REP`TypeRep]
// inner : [seq of CPP`Stmt]
// ==> seq of CPP`Stmt * bool
Tuple vdmcg::CGMatchSeqEnumPattern (const TYPE_AS_SeqEnumPattern & pat,
                                    const TYPE_CGMAIN_VT & rc2,
                                    const Set & pn_s,
                                    const TYPE_CPP_Name & succ_v,
                                    const Map & pid_m,
                                    const Generic & inner,
                                    bool nonstop)
{
  const TYPE_CPP_Expr & varExpr_v (rc2.GetRecord(pos_CGMAIN_VT_name));
  const Generic & type (rc2.GetField(pos_CGMAIN_VT_type));

  if (!type.IsNil() && !IsPosSeqType(type)) {
    SEQ<TYPE_CPP_Stmt> rb;
    rb.ImpAppend(vdm_BC_GenSingleLineComments(SEQ<Char>(L"never match")));
    rb.ImpAppend(vdm_BC_GenAsgnStmt(succ_v, vdm_BC_GenBoolLit(false)));
    return mk_(rb, Bool(false));
  }

  TYPE_CPP_Expr castVarExpr_v (IsSeqType(type)
            ? varExpr_v
#ifdef VDMPP
            : vdm_CPP_isJAVA() ? (IsStringType(type) ? GenCastString(varExpr_v)
                                                     : GenCastSeq(varExpr_v, Nil()))
#endif // VDMPP
                                : GenCastSeq(varExpr_v, Nil()));
  const SEQ<TYPE_AS_Pattern> & p_l (pat.GetSequence(pos_AS_SeqEnumPattern_els));

  bool all_nilname = true;
  size_t len_p_l = p_l.Length();
  for (size_t idx = 1; idx <= len_p_l; idx++) {
    const TYPE_AS_Pattern & p (p_l[idx]);
    if (!(p.Is(TAG_TYPE_AS_PatternName) && p.GetField(pos_AS_PatternName_nm).IsNil()))
      all_nilname = false;
  }

  if (all_nilname) {
    // special case: [ - ], [ -, - ], ...
    // check length only
    TYPE_CPP_Expr seq_size;
#ifdef VDMPP
    if (vdm_CPP_isJAVA()) {
      seq_size = (IsStringType(type) ? GenLenString_int(castVarExpr_v)
                                     : GenLen_int(castVarExpr_v));
    }
    else
#endif // VDMPP
    {
      seq_size = GenLen_int(castVarExpr_v);
    }
    TYPE_CPP_Expr if_cond (vdm_BC_GenEq(vdm_BC_GenIntegerLit (len_p_l), seq_size));
    TYPE_CPP_Stmt stmt;
    if (!inner.IsNil()) {
      if (nonstop) {
        stmt = vdm_BC_GenIfStmt(if_cond, vdm_BC_GenBlock(inner), nil);
      }
      else {
        stmt = vdm_BC_GenIfStmt(vdm_BC_GenAsgnExpr(succ_v, vdm_BC_GenBracketedExpr(if_cond)),
                                vdm_BC_GenBlock(inner), nil);
      }
    }
    else {
      stmt = vdm_BC_GenAsgnStmt(succ_v, vdm_BC_GenBracketedExpr(if_cond));
    }

    if (IsSeqType(type)) {
      SEQ<TYPE_CPP_Stmt> rb;
      rb.ImpAppend(stmt);
      //return mk_(rb, Bool(false)); 
      return mk_(rb, Bool(!inner.IsNil() && nonstop)); 
    }
    else {
      SEQ<TYPE_CPP_Stmt> rb;
      rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenAsgnExpr(succ_v, vdm_BC_GenBracketedExpr(GenIsSeq(varExpr_v))),
                                    stmt, nil));
      return mk_(rb, Bool(false)); 
    }
  }
  else {
    TYPE_REP_TypeRep newtype_l (FindType(pat));
    TYPE_CPP_Expr tmpvarExpr_v (varExpr_v);
    if (newtype_l != type) {
      tmpvarExpr_v = vdm_BC_GiveName(ASTAUX::MkId(L"tmpvarExpr"));
    }

    TYPE_CGMAIN_VT cg_vt (mk_CG_VT(tmpvarExpr_v, newtype_l));
    Tuple cgml (CGMatchList (p_l, cg_vt, succ_v, pn_s, pid_m, inner, nonstop));
    const SEQ<TYPE_CPP_Stmt> & pm (cgml.GetSequence(1));
    const Bool & Is_excl (cgml.GetBool(2));

    SEQ<TYPE_CPP_Stmt> trb;
    if (Is_excl && !nonstop) {
      trb.ImpAppend(vdm_BC_GenAsgnStmt(succ_v, vdm_BC_GenBoolLit(true)));
    }
    trb.ImpConc(pm);

#ifdef VDMPP
    if (vdm_CPP_isJAVA()) {
      SEQ<TYPE_CPP_Stmt> stringstmt;
      if (newtype_l != type) {
        stringstmt.ImpConc(GenDeclInit_DS (newtype_l, tmpvarExpr_v,
                 GenExplicitCast(newtype_l, varExpr_v, mk_REP_SeqTypeRep(mk_REP_CharTypeRep()))));
      }
      stringstmt.ImpConc(trb);

      SEQ<TYPE_CPP_Stmt> seqstmt;
      if (newtype_l != type) {
        seqstmt.ImpConc(GenDeclInit_DS (newtype_l, tmpvarExpr_v, GenCastType(newtype_l, varExpr_v)));
      }
      seqstmt.ImpConc(trb);

      if (IsStringType(type)) {
        return mk_(stringstmt, Bool(Is_excl && nonstop));
      }
      else if (IsSeqType(type)) {
        SEQ<TYPE_CPP_Stmt> rb;
        rb.ImpConc(seqstmt);
        return mk_(seqstmt, Bool(Is_excl && nonstop));
      }
      else {
        TYPE_REP_TypeRep etp (FindSeqElemType(type));
        if (Is_excl && nonstop) {
          if (IsPosCharType(etp)) {
            SEQ<TYPE_CPP_Stmt> rb;
            rb.ImpAppend(vdm_BC_GenIfStmt (GenIsString (varExpr_v),
                                           vdm_BC_GenBlock (stringstmt),
                                           vdm_BC_GenIfStmt (GenIsSeq (varExpr_v), vdm_BC_GenBlock (seqstmt), nil)));
            return mk_(rb, Bool(true));
          }
          else {
            SEQ<TYPE_CPP_Stmt> rb;
            rb.ImpAppend(vdm_BC_GenIfStmt (GenIsSeq (varExpr_v), vdm_BC_GenBlock (seqstmt), nil));
            return mk_(rb, Bool(true));
          }
        }
        else {
          if (IsPosCharType(etp)) {
            SEQ<TYPE_CPP_Stmt> rb;
            rb.ImpAppend(vdm_BC_GenIfStmt (GenIsString (varExpr_v),
                                           vdm_BC_GenBlock (stringstmt),
                                           vdm_BC_GenIfStmt(vdm_BC_GenAsgnExpr(succ_v,
                                                                   vdm_BC_GenBracketedExpr(GenIsSeq(varExpr_v))),
                                                                                  vdm_BC_GenBlock (seqstmt), nil)));
            return mk_(rb, Bool(false));
          }
          else {
            SEQ<TYPE_CPP_Stmt> rb;
            rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenAsgnExpr(succ_v, vdm_BC_GenBracketedExpr(GenIsSeq(varExpr_v))),
                                          vdm_BC_GenBlock (seqstmt), nil));
            return mk_(rb, Bool(false));
          }
        }
      }
    }
    else
#endif // VDMPP
    { // C++
      SEQ<TYPE_CPP_Stmt> tmpstmt;
      if (newtype_l != type) {
        tmpstmt.ImpConc(GenDeclInit_DS (newtype_l, tmpvarExpr_v, varExpr_v));
      }
      tmpstmt.ImpConc(trb);

      if (IsSeqType(type)) {
        return mk_(tmpstmt, Bool(Is_excl && nonstop));
      }
      else {
        if (Is_excl && nonstop) {
          SEQ<TYPE_CPP_Stmt> rb;
          rb.ImpAppend(vdm_BC_GenIfStmt (GenIsSeq( varExpr_v ), vdm_BC_GenBlock (tmpstmt), nil));
          return mk_(rb, Bool(true));
        }
        else {
          SEQ<TYPE_CPP_Stmt> rb;
          rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenAsgnExpr(succ_v, vdm_BC_GenBracketedExpr(GenIsSeq(varExpr_v))),
                                        vdm_BC_GenBlock (tmpstmt), nil));
          return mk_(rb, Bool(false));
        }
      }
    }
  }
}

// CGMatchMapEnumPattern
// pat : AS`MapEnumPattern
// rc2 : CGMAIN`VT
// pn_s : set of (AS`Name | AS`OldName)
// succ_v : CPP`Name
// pid_m : [map (AS`Name | AS`OldName) to REP`TypeRep]
// inner : [seq of CPP`Stmt]
// ==> seq of CPP`Stmt * bool
Tuple vdmcg::CGMatchMapEnumPattern (const TYPE_AS_MapEnumPattern & pat,
                                    const TYPE_CGMAIN_VT & rc2,
                                    const Set & pn_s,
                                    const TYPE_CPP_Name & succ_v,
                                    const Map & pid_m,
                                    const Generic & inner,
                                    bool nonstop)
{
  const TYPE_CPP_Expr & varExpr_v (rc2.GetRecord(pos_CGMAIN_VT_name));
  const Generic & type (rc2.GetField(pos_CGMAIN_VT_type));

  if (!type.IsNil() && !IsPosMapType(type)) {
    SEQ<TYPE_CPP_Stmt> rb;
    rb.ImpAppend(vdm_BC_GenSingleLineComments(SEQ<Char>(L"never match")));
    rb.ImpAppend(vdm_BC_GenAsgnStmt(succ_v, vdm_BC_GenBoolLit(false)));
    return mk_(rb, Bool(false));
  }

  SEQ<TYPE_CPP_Stmt> decls;
  TYPE_CPP_Expr varExpr_v_q (varExpr_v);
  if (!varExpr_v.Is(TAG_TYPE_CPP_Identifier)) {
    varExpr_v_q = vdm_BC_GiveName(ASTAUX::MkId(L"tmpVal_v"));
    decls.ImpConc(GenDeclInit_DS(type, varExpr_v_q, varExpr_v));
  }
  
  TYPE_CPP_Expr castVarExpr_v (IsMapType(type) ? varExpr_v_q : GenCastMap(varExpr_v_q));

  const SEQ<TYPE_AS_MapletPattern> & mp_l (pat.GetSequence(pos_AS_MapEnumPattern_mls));

  TYPE_REP_TypeRep mp_type (FindType(pat));
  const TYPE_REP_TypeRep & dom_type (FindMapDomType(mp_type));
  const TYPE_REP_TypeRep & rng_type (FindMapRngType(mp_type));

  bool all_nilname = true;
  SEQ<TYPE_AS_Pattern> p_l;
  size_t len_mp_l = mp_l.Length();
  for (size_t idx = 1; idx <= len_mp_l; idx++) {
    const TYPE_AS_MapletPattern & mp (mp_l[idx]);
    const TYPE_AS_Pattern & dp (mp.GetRecord(pos_AS_MapletPattern_dp));
    const TYPE_AS_Pattern & rp (mp.GetRecord(pos_AS_MapletPattern_rp));
    p_l.ImpAppend(dp);
    p_l.ImpAppend(rp);
    if (!(dp.Is(TAG_TYPE_AS_PatternName) && dp.GetField(pos_AS_PatternName_nm).IsNil() &&
        rp.Is(TAG_TYPE_AS_PatternName) && rp.GetField(pos_AS_PatternName_nm).IsNil())) {
      all_nilname = false;
    }
  }
//
  if (all_nilname) {
    // special case: { - |-> - } or { - |-> - , - |-> - } ...
    // check length only
    TYPE_CPP_Expr if_cond (vdm_BC_GenEq(vdm_BC_GenIntegerLit (len_mp_l), GenSize_int(castVarExpr_v)));
    TYPE_CPP_Stmt stmt;
    if (!inner.IsNil()) {
      if (nonstop) {
        stmt = vdm_BC_GenIfStmt(if_cond, vdm_BC_GenBlock(inner), nil);
      }
      else {
        stmt = vdm_BC_GenIfStmt(vdm_BC_GenAsgnExpr(succ_v, vdm_BC_GenBracketedExpr(if_cond)),
                                vdm_BC_GenBlock(inner), nil);
      }
    }
    else {
      stmt = vdm_BC_GenAsgnStmt(succ_v, vdm_BC_GenBracketedExpr(if_cond));
    }
    if (IsMapType(type)) {
      SEQ<TYPE_CPP_Stmt> rb (decls);
      rb.ImpAppend(stmt);
      return mk_(rb, Bool(!inner.IsNil() && nonstop)); 
    }
    else {
      SEQ<TYPE_CPP_Stmt> rb (decls);
      rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenAsgnExpr(succ_v, vdm_BC_GenBracketedExpr(GenIsMap(varExpr_v_q))),
                                    stmt, nil));
      return mk_(rb, Bool(false)); 
    }
  }
//
  if (len_mp_l == 1) {
    TYPE_CPP_Identifier dom_v (vdm_BC_GiveName(ASTAUX::MkId(L"dom")));
    TYPE_CPP_Identifier rng_v (vdm_BC_GiveName(ASTAUX::MkId(L"rng")));

    TYPE_AS_Pattern dp (mp_l[1].GetRecord(pos_AS_MapletPattern_dp));
    TYPE_AS_Pattern rp (mp_l[1].GetRecord(pos_AS_MapletPattern_rp));

    Tuple cgpmer (CGPatternMatchExcl(rp, mk_CG_VT(rng_v, rng_type),
                                     pn_s.Union(FindPatternId (dp).Dom ()), succ_v, pid_m, inner, nonstop));
    Generic p = nil;
    if (!cgpmer.GetSequence(1).IsNil()) {
      p = cgpmer.GetSequence(1);
    }
    Tuple cgpmed (CGPatternMatchExcl(dp, mk_CG_VT(dom_v, dom_type), pn_s, succ_v, pid_m, p, nonstop));

    TYPE_CPP_Expr de;
#ifdef VDMPP
    if (vdm_CPP_isJAVA()) {
      de = GenExplicitCast(dom_type, GenSetGetElemExpr(GenDom(castVarExpr_v)), Nil());
    }
    else
#endif // VDMPP
    {
      de = GenSetGetElemExpr(GenDom(castVarExpr_v));
    }

    TYPE_CPP_Expr re (GenMapApply(mk_CG_VT(castVarExpr_v, mk_REP_GeneralMapTypeRep(dom_type, rng_type)), dom_v));

    SEQ<TYPE_CPP_Stmt> ifBody;
    ifBody.ImpConc(GenDeclInit_DS(dom_type, dom_v, de));
    ifBody.ImpConc(GenDeclInit_DS(rng_type, rng_v, re));

    bool Is_excl = cgpmed.GetBoolValue(2) && cgpmer.GetBoolValue(2);

    if (Is_excl && inner.IsNil()) { // false : need to check pattern match failed
      ifBody.ImpAppend(vdm_BC_GenAsgnStmt(succ_v, vdm_BC_GenBoolLit(true)));
    }
    ifBody.ImpConc(cgpmed.GetSequence(1));
      
    TYPE_CPP_Expr cond (vdm_BC_GenEq (vdm_BC_GenIntegerLit (len_mp_l), GenSize_int(castVarExpr_v)));
    if (!IsMapType(type)) {
      cond = vdm_BC_GenLogAnd(vdm_BC_GenBracketedExpr(GenIsMap(varExpr_v_q)), vdm_BC_GenBracketedExpr(cond));
    }
    if (Is_excl && nonstop) {
      SEQ<TYPE_CPP_Stmt> rb (decls);
      rb.ImpAppend(vdm_BC_GenIfStmt(cond, vdm_BC_GenBlock(ifBody), nil));
      return mk_(rb, Bool(true));
    }
    else {
      SEQ<TYPE_CPP_Stmt> rb (decls);
      rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenAsgnExpr(succ_v, vdm_BC_GenBracketedExpr(cond)),
                                    vdm_BC_GenBlock(ifBody), nil));
      return mk_(rb, Bool(false));
    }
  }
  else
  { // len_mp_l > 1
    TYPE_REP_TypeRep perm_t (mk_REP_GeneralMapTypeRep(dom_type, rng_type));
    TYPE_CPP_Identifier valSeq_v (vdm_BC_GiveName(ASTAUX::MkId(L"valSeq")));
    TYPE_CPP_Identifier perm_v (vdm_BC_GiveName(ASTAUX::MkId(L"perm")));
    TYPE_CPP_Identifier permSL_v (vdm_BC_GiveName(ASTAUX::MkId(L"permSL")));
    TYPE_REP_TypeRep permSL_t (mk_REP_SetTypeRep (mk_REP_SeqTypeRep (dom_type)));
    TYPE_REP_SetTypeRep newsp_type (mk_REP_SetTypeRep (dom_type));

    TYPE_CPP_Identifier tmpSeq_v (vdm_BC_GiveName(ASTAUX::MkId(L"tmpSeq")));
    TYPE_CPP_Identifier elem_v (vdm_BC_GiveName(ASTAUX::MkId(L"elem")));

    SEQ<TYPE_CPP_Stmt> stmt_l;
    stmt_l.ImpAppend(GenImpAppend(perm_v, elem_v));
    stmt_l.ImpAppend(GenImpAppend(perm_v, GenMapApply(mk_CG_VT(castVarExpr_v,
                                                        mk_REP_GeneralMapTypeRep(dom_type, rng_type)), elem_v)));
    Tuple cgml (CGMatchList (p_l, mk_CG_VT (perm_v, perm_t), succ_v, pn_s, pid_m, inner, false));
    const SEQ<TYPE_CPP_Stmt> & pm (cgml.GetSequence(1));
    const Bool & Is_excl (cgml.GetBool(2));

    SEQ<TYPE_CPP_Stmt> inner_rb;
    inner_rb.ImpConc (GenDeclEmptySeq (perm_v));
    inner_rb.ImpConc(GenIterSeq(mk_CG_VT (tmpSeq_v, mk_REP_SeqTypeRep(dom_type)), nil,
                                mk_CG_VT (elem_v, dom_type), stmt_l));
    if (Is_excl) {
      inner_rb.ImpAppend(vdm_BC_GenAsgnStmt(succ_v, vdm_BC_GenBoolLit(true)));
    }
    inner_rb.ImpConc(pm);

    SEQ<TYPE_CPP_Stmt> ifBody;
    //ifBody.ImpConc (GenDeclSet (tmpvarExpr_v, GenDom(castVarExpr_v)));
    //ifBody.ImpConc(GenValSetToSeq (valSeq_v, tmpvarExpr_v, newsp_type));
    ifBody.ImpConc(GenValSetToSeq (valSeq_v, GenDom(castVarExpr_v), newsp_type));
    ifBody.ImpConc (GenDeclSet (permSL_v, GenPermute(valSeq_v)));

    ifBody.ImpAppend(vdm_BC_GenAsgnStmt(succ_v, vdm_BC_GenBoolLit(false)));
    ifBody.ImpConc (GenIterSet (mk_CG_VT (permSL_v, permSL_t),
                                vdm_BC_GenNot(succ_v),
                                mk_CG_VT (tmpSeq_v, mk_REP_SeqTypeRep(dom_type)), inner_rb));

    TYPE_CPP_Expr cond (vdm_BC_GenEq (vdm_BC_GenIntegerLit (len_mp_l), GenSize_int(castVarExpr_v)));
    if (!IsMapType(type)) {
      cond = vdm_BC_GenLogAnd(vdm_BC_GenBracketedExpr(GenIsMap(varExpr_v_q)), vdm_BC_GenBracketedExpr(cond));
    }
    SEQ<TYPE_CPP_Stmt> rb (decls);
    rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenAsgnExpr(succ_v, vdm_BC_GenBracketedExpr(cond)),
                                  vdm_BC_GenBlock(ifBody), nil));
    return mk_(rb, Bool(false));
  }
}

// CGMatchRecordPattern
// pat : AS`RecordPattern
// rc2 : CGMAIN`VT
// pn_s : set of (AS`Name | AS`OldName)
// succ_v : CPP`Name
// pid_m : [map (AS`Name | AS`OldName) to REP`TypeRep]
// inner : [seq of CPP`Stmt]
// ==> seq of CPP`Stmt * bool
Tuple vdmcg::CGMatchRecordPattern (const TYPE_AS_RecordPattern & pat,
                                   const TYPE_CGMAIN_VT & rc2,
                                   const Set & pn_s,
                                   const TYPE_CPP_Name & succ_v,
                                   const Map & pid_m,
                                   const Generic & inner,
                                   bool nonstop)
{
  const SEQ<TYPE_AS_Pattern> & p_l (pat.GetSequence(pos_AS_RecordPattern_fields));

  const TYPE_CPP_Expr & varExpr_v (rc2.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & type (rc2.GetRecord(pos_CGMAIN_VT_type));

  Generic p_type (FindType(pat));

  if (!IsPosCompositeType(type))
  {
    SEQ<TYPE_CPP_Stmt> rb;
    rb.ImpAppend(vdm_BC_GenSingleLineComments(SEQ<Char>(L"never match")));
    rb.ImpAppend(vdm_BC_GenAsgnStmt(succ_v, vdm_BC_GenBoolLit(false)));
    return mk_(rb, Bool(false));
  }

  const TYPE_AS_Name & n (pat.GetRecord(pos_AS_RecordPattern_nm));


  TYPE_CPP_Expr newVarExpr;
  TYPE_CPP_Expr if_cond;
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    newVarExpr = (type.Is(TAG_TYPE_REP_CompositeTypeRep) ? varExpr_v : GenCastRecord (varExpr_v, n));
    if_cond = vdm_BC_GenTypeComp(vdm_BC_Rename(n), varExpr_v);
  }
  else
#endif // VDMPP
  {
    newVarExpr = (IsCompositeType(type) ? varExpr_v : GenCastRecord (varExpr_v, nil));
    if_cond = GenAuxType(varExpr_v, p_type);
  }

  size_t len_p_l = p_l.Length();
  bool all_nilname = true;
  for (size_t idx = 1; idx <= len_p_l; idx++)
  {
    const TYPE_AS_Pattern & p (p_l[idx]);
    if (!(p.Is(TAG_TYPE_AS_PatternName) && p.GetField(pos_AS_PatternName_nm).IsNil()))
      all_nilname = false;
  }
  
  if (p_l.IsEmpty() || all_nilname)
  {
    if (type.Is(TAG_TYPE_REP_CompositeTypeRep)) // only if tag is identical
    {
      SEQ<TYPE_CPP_Stmt> rb;
      if (!inner.IsNil())
        rb.ImpConc(inner);
      return mk_(rb, Bool(true));
    }
    else
    {
      if (!inner.IsNil())
      {
        if (nonstop)
        {
          SEQ<TYPE_CPP_Stmt> rb;
          rb.ImpAppend(vdm_BC_GenIfStmt(if_cond, vdm_BC_GenBlock(inner), nil));
          return mk_(rb, Bool(true));
        }
        else
        {
          SEQ<TYPE_CPP_Stmt> rb;
          rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenAsgnExpr(succ_v, vdm_BC_GenBracketedExpr(if_cond)),
                                        vdm_BC_GenBlock(inner), nil));
          return mk_(rb, Bool(false));
        }
      }
      else 
      {
        SEQ<TYPE_CPP_Stmt> rb;
        rb.ImpAppend(vdm_BC_GenAsgnStmt(succ_v, vdm_BC_GenBracketedExpr(if_cond)));
        return mk_(rb, Bool(false));
      }
    }
  }
  else
  {
    Tuple cgml (CGMatchList (p_l, mk_CG_VT (newVarExpr, p_type), succ_v, pn_s, pid_m, inner, nonstop));
    const SEQ<TYPE_CPP_Stmt> & pm (cgml.GetSequence(1));
    const Bool & Is_excl (cgml.GetBool(2));

    if (type.Is(TAG_TYPE_REP_CompositeTypeRep))
    {
      return mk_(pm, Is_excl);
    }
    else
    {
      if (Is_excl && nonstop)
      {
        SEQ<TYPE_CPP_Stmt> rb;
        rb.ImpAppend(vdm_BC_GenIfStmt(if_cond, vdm_BC_GenBlock(pm), nil));
        return mk_(rb, Bool(true));
      }
      else
      {
        SEQ<TYPE_CPP_Stmt> rb;
        rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenAsgnExpr(succ_v, vdm_BC_GenBracketedExpr(if_cond)),
                                      vdm_BC_GenBlock(pm), nil));
        return mk_(rb, Bool(false));
      }
    }
  }
}

// CGMatchTuplePattern
// pat : AS`TuplePattern
// rc2 : CGMAIN`VT
// pn_s : set of (AS`Name | AS`OldName)
// succ_v : CPP`Name
// pid_m : [map (AS`Name | AS`OldName) to REP`TypeRep]
// inner : [seq of CPP`Stmt]
// ==> seq of CPP`Stmt
Tuple vdmcg::CGMatchTuplePattern (const TYPE_AS_TuplePattern & pat,
                                  const TYPE_CGMAIN_VT & rc2,
                                  const Set & pn_s,
                                  const TYPE_CPP_Name & succ_v,
                                  const Map & pid_m,
                                  const Generic & inner,
                                  bool nonstop)
{
  const TYPE_CPP_Expr & varExpr_v (rc2.GetRecord(pos_CGMAIN_VT_name));
  const Generic & varExpr_tp      (rc2.GetField(pos_CGMAIN_VT_type));

  const SEQ<TYPE_AS_Pattern> & p_l (pat.GetSequence(pos_AS_TuplePattern_fields));
  Generic tp_l (FindProductElemType(varExpr_tp, p_l.Length()));

  if (tp_l.IsNil()) {
    SEQ<TYPE_CPP_Stmt> rb;
    rb.ImpAppend(vdm_BC_GenSingleLineComments(SEQ<Char>(L"never match")));
    rb.ImpAppend(vdm_BC_GenAsgnStmt(succ_v, vdm_BC_GenBoolLit(false)));
    return mk_(rb, Bool(false));
  }

  TYPE_REP_ProductTypeRep t_type (mk_REP_ProductTypeRep(tp_l));

  TYPE_CPP_Expr newVarExpr (IsProductType(varExpr_tp) ? varExpr_v : GenCastType (t_type, varExpr_v));

  Tuple cgml (CGMatchList (p_l, mk_CG_VT (newVarExpr, t_type), succ_v, pn_s, pid_m, inner, nonstop));
  const SEQ<TYPE_CPP_Stmt> & pm (cgml.GetSequence(1));
  const Bool & Is_excl (cgml.GetBool(2));

  TYPE_CPP_Expr len_perm_v (GenTupleLength(newVarExpr));
  TYPE_CPP_IntegerLit len_p_l (vdm_BC_GenIntegerLit(p_l.Length()));
  TYPE_CPP_Expr eq_len (vdm_BC_GenBracketedExpr(vdm_BC_GenEq(len_p_l, len_perm_v)));
  if (IsProductType(varExpr_tp)) {
    TYPE_REP_TypeRep tp (varExpr_tp.Is(TAG_TYPE_REP_InvTypeRep)
                               ? Record(varExpr_tp).GetRecord(pos_REP_InvTypeRep_shape)
                                                                : (Record)varExpr_tp);
    if (tp.Is(TAG_TYPE_REP_ProductTypeRep) && 
        (tp.GetSequence(pos_REP_ProductTypeRep_tps).Length() == p_l.Length())) {
      return mk_(pm, Is_excl);
    }
    else {
      SEQ<TYPE_CPP_Stmt> rb;
      if (pm.IsEmpty()) {
        rb.ImpAppend(vdm_BC_GenAsgnStmt(succ_v, eq_len));
      }
      else {
        rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenAsgnExpr(succ_v, eq_len), vdm_BC_GenBlock(pm), nil));
      }
      return mk_(rb, Bool(false));
    }
  }
  else {
    TYPE_CPP_Expr cond (vdm_BC_GenBracketedExpr(vdm_BC_GenLogAnd(GenIsTuple(varExpr_v), eq_len)));
    if (Is_excl && nonstop) {
      SEQ<TYPE_CPP_Stmt> rb;
      rb.ImpAppend(vdm_BC_GenIfStmt(cond, vdm_BC_GenBlock(pm), nil));
      return mk_(rb, Bool(true));
    }
    else {
      SEQ<TYPE_CPP_Stmt> rb;
      if (pm.IsEmpty()) {
        rb.ImpAppend(vdm_BC_GenAsgnStmt(succ_v, cond));
      }
      else {
        rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenAsgnExpr(succ_v, cond), vdm_BC_GenBlock(pm), nil));
      }
      return mk_(rb, Bool(false));
    }
  }
}

// CGMatchSetUnionPattern
// pat : AS`SetUnionPattern
// rc2 : CGMAIN`VT
// pn_s : set of (AS`Name | AS`OldName)
// succ_v : CPP`Name
// pid_m : [map (AS`Name | AS`OldName) to REP`TypeRep]
// inner : [seq of CPP`Stmt]
// ==> seq of CPP`Stmt
Tuple vdmcg::CGMatchSetUnionPattern (const TYPE_AS_SetUnionPattern & pat,
                                     const TYPE_CGMAIN_VT & rc2,
                                     const Set & pn_s,
                                     const TYPE_CPP_Name & succ_v,
                                     const Map & pid_m,
                                     const Generic & inner,
                                     bool nonstop)
{
  const TYPE_AS_Pattern & lp (pat.GetRecord(pos_AS_SetUnionPattern_lp));
  const TYPE_AS_Pattern & rp (pat.GetRecord(pos_AS_SetUnionPattern_rp));
  const TYPE_CPP_Expr & varExpr_v (rc2.GetRecord(pos_CGMAIN_VT_name));
  const Generic & type (rc2.GetRecord(pos_CGMAIN_VT_type));

  if (!type.IsNil() &&
      (lp.Is (TAG_TYPE_AS_PatternName) || lp.Is (TAG_TYPE_AS_SetEnumPattern)) &&
      (rp.Is (TAG_TYPE_AS_PatternName) || rp.Is (TAG_TYPE_AS_SetEnumPattern))) {
    if (!IsPosSetType(type)) {
      SEQ<TYPE_CPP_Stmt> rb;
      rb.ImpAppend(vdm_BC_GenSingleLineComments(SEQ<Char>(L"never match")));
      rb.ImpAppend(vdm_BC_GenAsgnStmt(succ_v, vdm_BC_GenBoolLit(false)));
      return mk_(rb, Bool(false));
    }

    TYPE_CPP_Expr castVarExpr_v (IsSetType(type) ? varExpr_v : GenCastSetType(varExpr_v));

    if (lp.Is (TAG_TYPE_AS_PatternName) && lp.GetField(pos_AS_PatternName_nm).IsNil() &&
        rp.Is (TAG_TYPE_AS_PatternName) && rp.GetField(pos_AS_PatternName_nm).IsNil() ) {
      // - union -
      if (!inner.IsNil()) {
        if (nonstop) {
          SEQ<TYPE_CPP_Stmt> rb;
          rb.ImpAppend(vdm_BC_GenIfStmt(GenIsSet(varExpr_v), vdm_BC_GenBlock(inner), nil));
          return mk_(rb, Bool(true));
        }
        else {
          SEQ<TYPE_CPP_Stmt> rb;
          rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenAsgnExpr(succ_v, vdm_BC_GenBracketedExpr(GenIsSet(varExpr_v))),
                                        vdm_BC_GenBlock(inner), nil));
          return mk_(rb, Bool(false));
        }
      }
      else {
        SEQ<TYPE_CPP_Stmt> rb;
        rb.ImpAppend(vdm_BC_GenAsgnStmt(succ_v, vdm_BC_GenBracketedExpr(GenIsSet(varExpr_v))));
        return mk_(rb, Bool(false));
      }
    }
    else if ((lp.Is(TAG_TYPE_AS_SetEnumPattern) &&
              (lp.GetSequence(pos_AS_SetEnumPattern_Elems).Length() == 1) &&
              lp.GetSequence(pos_AS_SetEnumPattern_Elems)[1].Is(TAG_TYPE_AS_PatternName) &&
              Record(lp.GetSequence(pos_AS_SetEnumPattern_Elems)[1]).GetField(pos_AS_PatternName_nm).IsNil() &&
              rp.Is (TAG_TYPE_AS_PatternName) && rp.GetField(pos_AS_PatternName_nm).IsNil())
             ||
             (lp.Is (TAG_TYPE_AS_PatternName) && lp.GetField(pos_AS_PatternName_nm).IsNil() &&
              rp.Is(TAG_TYPE_AS_SetEnumPattern) &&
              (rp.GetSequence(pos_AS_SetEnumPattern_Elems).Length() == 1) &&
              rp.GetSequence(pos_AS_SetEnumPattern_Elems)[1].Is(TAG_TYPE_AS_PatternName) &&
              Record(rp.GetSequence(pos_AS_SetEnumPattern_Elems)[1]).GetField(pos_AS_PatternName_nm).IsNil()))
    {
      // {-} union -
      // - union {-}
      TYPE_CPP_Expr cond (IsSetType(type) 
                            ? vdm_BC_GenNot(GenSetIsEmpty(castVarExpr_v))
                            : vdm_BC_GenLogAnd(GenIsSet(varExpr_v), vdm_BC_GenNot(GenSetIsEmpty(castVarExpr_v))));
      if (!inner.IsNil()) {
        if (nonstop) {
          SEQ<TYPE_CPP_Stmt> rb;
          rb.ImpAppend(vdm_BC_GenIfStmt(cond, vdm_BC_GenBlock(inner), nil));
          return mk_(rb, Bool(true));
        }
        else {
          SEQ<TYPE_CPP_Stmt> rb;
          rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenAsgnExpr(succ_v, vdm_BC_GenBracketedExpr(cond)),
                                        vdm_BC_GenBlock(inner), nil));
          return mk_(rb, Bool(false));
        }
      }
      else {
        SEQ<TYPE_CPP_Stmt> rb;
        rb.ImpAppend(vdm_BC_GenAsgnStmt(succ_v, vdm_BC_GenBracketedExpr(cond)));
        return mk_(rb, Bool(false));
      }
    }
    else // others
    {
      TYPE_CPP_Identifier elem_v (vdm_BC_GiveName(ASTAUX::MkId(L"elem")));
      TYPE_CPP_Identifier tempSet1_v (vdm_BC_GiveName(ASTAUX::MkId(L"tempSet1")));
      TYPE_CPP_Identifier tempSet2_v (vdm_BC_GiveName(ASTAUX::MkId(L"tempSet2")));

      TYPE_CPP_Identifier alreadySet_v (vdm_BC_GiveName(ASTAUX::MkId(L"alreadySet")));
      TYPE_CPP_Identifier tempSet3_v (vdm_BC_GiveName(ASTAUX::MkId(L"tempSet3")));
      TYPE_CPP_Identifier alelem_v (vdm_BC_GiveName(ASTAUX::MkId(L"alelem")));

      SEQ<TYPE_CPP_Stmt> inner_rb_l;
      inner_rb_l.ImpConc(GenDeclSet(tempSet1_v, alelem_v));
      inner_rb_l.ImpAppend(GenSetInsert(tempSet1_v, elem_v));
      inner_rb_l.ImpAppend(GenSetInsert(alreadySet_v, tempSet1_v));
      inner_rb_l.ImpConc(GenDeclSet(tempSet2_v, castVarExpr_v));
      inner_rb_l.ImpAppend(GenSetMinusStmt(tempSet2_v, tempSet1_v));

      TYPE_REP_TypeRep elemtp (FindSetElemType(type));

      TYPE_AS_Pattern lp_q, rp_q;
      if (!lp.Is(TAG_TYPE_AS_SetEnumPattern) && rp.Is(TAG_TYPE_AS_SetEnumPattern)) {
        lp_q = rp;
        rp_q = lp;
      }
      else {
        lp_q = lp;
        rp_q = rp;
      }

      Tuple cgpmel (CGPatternMatchExcl(lp_q, mk_CG_VT(tempSet1_v, mk_REP_SetTypeRep(elemtp)),
                                       pn_s, succ_v, pid_m, Nil(), false));
      Tuple cgpmer (CGPatternMatchExcl(rp_q, mk_CG_VT(tempSet2_v, mk_REP_SetTypeRep(elemtp)),
                                       pn_s.Union(FindPatternId (lp_q).Dom ()), succ_v, pid_m, inner, false));

      bool Is_excl = cgpmel.GetBoolValue(2) && cgpmer.GetBoolValue(2);

      if (Is_excl && inner.IsNil()) {
        inner_rb_l.ImpAppend(vdm_BC_GenAsgnStmt(succ_v, vdm_BC_GenBoolLit(true)));
      }
      inner_rb_l.ImpConc(cgpmel.GetSequence(1));
      
      if (!cgpmer.GetSequence(1).IsEmpty()) {
        if (cgpmel.GetBoolValue(2)) { // false : need to check pattern match failed
          inner_rb_l.ImpConc(cgpmer.GetSequence(1));
        }
        else {
          inner_rb_l.ImpAppend(vdm_BC_GenIfStmt(succ_v, vdm_BC_GenBlock(cgpmer.GetSequence(1)), nil));
        }
      }

      SEQ<TYPE_CPP_Stmt> outer_rb_l;
      outer_rb_l.ImpConc(GenDeclSet(tempSet3_v, alreadySet_v));
      outer_rb_l.ImpConc (GenIterSet (mk_CG_VT (tempSet3_v, mk_REP_SetTypeRep(mk_REP_SetTypeRep(elemtp))),
                          vdm_BC_GenNot(succ_v),
                          mk_CG_VT (alelem_v, mk_REP_SetTypeRep(elemtp)), inner_rb_l));

      SEQ<TYPE_CPP_Stmt> ifBody;
      ifBody.ImpConc(GenDeclEmptySet(alreadySet_v));
      ifBody.ImpAppend(GenSetInsert(alreadySet_v, GenEmptyValue(mk_REP_SetTypeRep(elemtp))));
      ifBody.ImpAppend(vdm_BC_GenAsgnStmt(succ_v, vdm_BC_GenBoolLit(false)));
      ifBody.ImpConc (GenIterSet (mk_CG_VT (castVarExpr_v, mk_REP_SetTypeRep(elemtp)),
                                  vdm_BC_GenNot(succ_v),
                                  mk_CG_VT (elem_v, elemtp), outer_rb_l));

      if (IsSetType(type)) {
        SEQ<TYPE_CPP_Stmt> rb;
        rb.ImpConc(ifBody);
        return mk_(rb, Bool(false)); 
      }
      else {
        SEQ<TYPE_CPP_Stmt> rb;
        rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenAsgnExpr(succ_v, vdm_BC_GenBracketedExpr(GenIsSet(varExpr_v))),
                                      vdm_BC_GenBlock(ifBody), nil));
        return mk_(rb, Bool(false)); 
      }
    }
  }
  this->ContainsNotSupportedConstruct = true;
  this->NotSupportedStmt = SEQ<TYPE_CPP_Stmt>().ImpAppend(NotSupported(L"Complex set union pattern", pat));
  return mk_(this->NotSupportedStmt, Bool(false));
}

// CGMatchSeqConcPattern
// pat : AS`SeqConcPattern
// rc2 : CGMAIN`VT
// pn_s : set of (AS`Name | AS`OldName)
// succ_v : CPP`Name
// pid_m : [map (AS`Name | AS`OldName) to REP`TypeRep]
// inner : [seq of CPP`Stmt]
// ==> seq of CPP`Stmt * bool
Tuple vdmcg::CGMatchSeqConcPattern (const TYPE_AS_SeqConcPattern & pat,
                                    const TYPE_CGMAIN_VT & rc2,
                                    const Set & pn_s,
                                    const TYPE_CPP_Name & succ_v,
                                    const Map & pid_m,
                                    const Generic & inner,
                                    bool nonstop)
{
  const TYPE_AS_Pattern & lp (pat.GetRecord(pos_AS_SeqConcPattern_lp));
  const TYPE_AS_Pattern & rp (pat.GetRecord(pos_AS_SeqConcPattern_rp));
  const TYPE_CPP_Expr & varExpr_v (rc2.GetRecord(pos_CGMAIN_VT_name));
  const Generic & type (rc2.GetRecord(pos_CGMAIN_VT_type));

  if (!type.IsNil() &&
      (lp.Is (TAG_TYPE_AS_PatternName) || lp.Is (TAG_TYPE_AS_SeqEnumPattern) ||
       lp.Is (TAG_TYPE_AS_MatchVal) || lp.Is (TAG_TYPE_AS_SeqConcPattern)) &&
      (rp.Is (TAG_TYPE_AS_PatternName) || rp.Is (TAG_TYPE_AS_SeqEnumPattern) ||
       rp.Is (TAG_TYPE_AS_MatchVal))) {
    if (!IsPosSeqType(type)) {
      SEQ<TYPE_CPP_Stmt> rb;
      rb.ImpAppend(vdm_BC_GenSingleLineComments(SEQ<Char>(L"never match")));
      rb.ImpAppend(vdm_BC_GenAsgnStmt(succ_v, vdm_BC_GenBoolLit(false)));
      return mk_(rb, Bool(false));
    }


    if ((lp.Is (TAG_TYPE_AS_PatternName) && lp.GetField(pos_AS_PatternName_nm).IsNil()) &&
        (rp.Is (TAG_TYPE_AS_PatternName) && rp.GetField(pos_AS_PatternName_nm).IsNil())) {
      // - ^ -
      TYPE_CPP_Expr cond;
      if (IsSeqType(type)) {
        cond = vdm_BC_GenBoolLit(true);
      }
      else {
#ifdef VDMPP
        if (vdm_CPP_isJAVA()) {
          cond = vdm_BC_GenLogOr(GenIsSeq(varExpr_v), GenIsString(varExpr_v));
        }
        else
#endif // VDMPP
        {
          cond = GenIsSeq(varExpr_v);
        }
      }
      TYPE_REP_BooleanTypeRep btype;
      if (!inner.IsNil()) {
        if (nonstop) {
          SEQ<TYPE_CPP_Stmt> rb;
          rb.ImpAppend(vdm_BC_GenIfStmt(cond, vdm_BC_GenBlock(inner), nil));
          return mk_(rb, Bool(true));
        }
        else {
          SEQ<TYPE_CPP_Stmt> rb;
          rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenAsgnExpr(succ_v, vdm_BC_GenBracketedExpr(cond)),
                                        vdm_BC_GenBlock(inner), nil));
          return mk_(rb, Bool(false));
        }
      }
      else {
        SEQ<TYPE_CPP_Stmt> rb;
        rb.ImpAppend(vdm_BC_GenAsgnStmt(succ_v, vdm_BC_GenBracketedExpr(cond)));
        return mk_(rb, Bool(false));
      }
    }
    else if ((lp.Is (TAG_TYPE_AS_SeqEnumPattern) &&
              (lp.GetSequence(pos_AS_SeqEnumPattern_els).Length() == 1) &&
              lp.GetSequence(pos_AS_SeqEnumPattern_els)[1].Is (TAG_TYPE_AS_PatternName) &&
              Record(lp.GetSequence(pos_AS_SeqEnumPattern_els)[1]).GetField(pos_AS_PatternName_nm).IsNil() &&
              (rp.Is (TAG_TYPE_AS_PatternName) && rp.GetField(pos_AS_PatternName_nm).IsNil()))
             ||
             (lp.Is (TAG_TYPE_AS_PatternName) && lp.GetField(pos_AS_PatternName_nm).IsNil() &&
              rp.Is (TAG_TYPE_AS_SeqEnumPattern) &&
              (rp.GetSequence(pos_AS_SeqEnumPattern_els).Length() == 1) &&
              rp.GetSequence(pos_AS_SeqEnumPattern_els)[1].Is (TAG_TYPE_AS_PatternName) &&
              Record(rp.GetSequence(pos_AS_SeqEnumPattern_els)[1]).GetField(pos_AS_PatternName_nm).IsNil())) {
      // [-] ^ -
      // - ^ [-]
      TYPE_CPP_Expr cond;
      if (IsSeqType(type)) {
         cond = vdm_BC_GenNot(GenSeqIsEmpty(varExpr_v));
      }
      else {
#ifdef VDMPP
        if (vdm_CPP_isJAVA()) {
          cond = vdm_BC_GenLogOr(vdm_BC_GenLogAnd(
                                   GenIsSeq(varExpr_v),
                                   vdm_BC_GenNot(GenSeqIsEmpty(GenCastSeq(varExpr_v, Nil())))),
                                 vdm_BC_GenLogAnd(
                                   GenIsString(varExpr_v),
                                   vdm_BC_GenNot(GenSeqIsEmpty(GenCastString(varExpr_v)))));
        }
        else
#endif // VDMPP
        {
          cond = vdm_BC_GenLogAnd(GenIsSeq(varExpr_v),
                                          vdm_BC_GenNot(GenSeqIsEmpty(GenCastSeq(varExpr_v, Nil()))));
        }
      }
      if (!inner.IsNil()) {
        if (nonstop) {
          SEQ<TYPE_CPP_Stmt> rb;
          rb.ImpAppend(vdm_BC_GenIfStmt(cond, vdm_BC_GenBlock(inner), nil));
          return mk_(rb, Bool(true));
        }
        else {
          SEQ<TYPE_CPP_Stmt> rb;
          rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenAsgnExpr(succ_v, vdm_BC_GenBracketedExpr(cond)),
                                        vdm_BC_GenBlock(inner), nil));
          return mk_(rb, Bool(false));
        }
      }
      else {
        SEQ<TYPE_CPP_Stmt> rb;
        rb.ImpAppend(vdm_BC_GenAsgnStmt(succ_v, vdm_BC_GenBracketedExpr(cond)));
        return mk_(rb, Bool(false));
      }
    }
    else {
      TYPE_CPP_Expr castVarExpr_v;
      if (IsSeqType(type)) {
        castVarExpr_v = varExpr_v;
      }
      else {
#ifdef VDMPP
        if (vdm_CPP_isJAVA()) {
          castVarExpr_v = (IsStringType(type) ? GenCastString(varExpr_v) : GenCastSeq(varExpr_v, Nil()));
        }
        else
#endif // VDMPP
        {
          castVarExpr_v = GenCastSeq(varExpr_v, Nil());
        }
      }
#ifdef VDMPP
      if (vdm_CPP_isJAVA()) {
        if (IsStringType(type)) {
          // startsWith and endWith can be used in this case
          if (lp.Is(TAG_TYPE_AS_MatchVal) && IsStringType(FindType(lp.GetRecord(pos_AS_MatchVal_val)))) {
            Tuple cgee (CGExprExcl(lp.GetRecord(pos_AS_MatchVal_val), ASTAUX::MkId(L"tmpVal_v"), nil));
            TYPE_AS_Expr tmpVal_v (cgee.GetRecord(1));
            SEQ<TYPE_CPP_Stmt> e_stmt (cgee.GetSequence(2));
            if (IsStringExpr(tmpVal_v)) {
              SEQ<TYPE_CPP_Expr> args (tmpVal_v.GetSequence(pos_CPP_ClassInstanceCreationExpr_arg));
              if (args.IsEmpty()) {
                tmpVal_v = vdm_BC_GenStringLit(ASTAUX::MkId(L""));
              }
              else {
                tmpVal_v = args[1];
              }
            }
            else if (!tmpVal_v.Is(TAG_TYPE_CPP_Identifier) && !tmpVal_v.Is(TAG_TYPE_CPP_ClassInstanceCreationExpr))
            {
              tmpVal_v = vdm_BC_GiveName(ASTAUX::MkId(L"tmpVal_v"));
              e_stmt.ImpAppend(vdm_BC_GenDecl(GenType(type), tmpVal_v, vdm_BC_GenAsgnInit(cgee.GetRecord(1))));
            }
            TYPE_CPP_Expr cond (vdm_BC_GenFctCallObjMemAcc(castVarExpr_v, ASTAUX::MkId(L"startsWith"),
                                                           mk_sequence(tmpVal_v)));
            TYPE_CPP_Identifier Max (GenLenString_int(castVarExpr_v));
            TYPE_CPP_Identifier index (GenLenString_int(tmpVal_v));
            TYPE_CPP_Expr sb2 (GenSubSequence(castVarExpr_v, type, index, Max));
            TYPE_REP_TypeRep seqtp (mk_REP_SeqTypeRep(FindSeqElemType(type)));
            Tuple cgpmer (CGPatternMatchExcl(rp, mk_CG_VT(sb2, seqtp), pn_s, succ_v, pid_m, Nil(), nonstop));
            if (cgpmer.GetBool(2) && nonstop) {
              SEQ<TYPE_CPP_Stmt> rb (e_stmt);
              rb.ImpAppend(vdm_BC_GenIfStmt(cond, vdm_BC_GenBlock(cgpmer.GetSequence(1)), nil));
              return mk_(rb, Bool(true));
            }
            else {
              SEQ<TYPE_CPP_Stmt> rb (e_stmt);
              if (cgpmer.GetSequence(1).IsEmpty()) {
                rb.ImpAppend(vdm_BC_GenAsgnStmt(succ_v, cond));
              }
              else {
                rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenAsgnExpr(succ_v, cond),
                                              vdm_BC_GenBlock(cgpmer.GetSequence(1)), nil));
              }
              return mk_(rb, Bool(false));
            }
          }
          if (rp.Is(TAG_TYPE_AS_MatchVal) && IsStringType(FindType(rp.GetRecord(pos_AS_MatchVal_val)))) {
            Tuple cgee (CGExprExcl(rp.GetRecord(pos_AS_MatchVal_val), ASTAUX::MkId(L"tmpVal_v"), nil));
            TYPE_AS_Expr tmpVal_v (cgee.GetRecord(1));
            SEQ<TYPE_CPP_Stmt> e_stmt (cgee.GetSequence(2));
            if (IsStringExpr(tmpVal_v))
            {
              SEQ<TYPE_CPP_Expr> args (tmpVal_v.GetSequence(pos_CPP_ClassInstanceCreationExpr_arg));
              if (args.IsEmpty()) {
                tmpVal_v = vdm_BC_GenStringLit(ASTAUX::MkId(L""));
              }
              else {
                tmpVal_v = args[1];
              }
            }
            else if (!tmpVal_v.Is(TAG_TYPE_CPP_Identifier) && !tmpVal_v.Is(TAG_TYPE_CPP_ClassInstanceCreationExpr)) {
              tmpVal_v = vdm_BC_GiveName(ASTAUX::MkId(L"tmpVal_v"));
              e_stmt.ImpAppend(vdm_BC_GenDecl(GenType(type), tmpVal_v, vdm_BC_GenAsgnInit(cgee.GetRecord(1))));
            }
            TYPE_CPP_Expr cond (vdm_BC_GenFctCallObjMemAcc(castVarExpr_v, ASTAUX::MkId(L"endsWith"),
                                                           mk_sequence(tmpVal_v)));
            TYPE_CPP_Identifier Max (GenLenString_int(castVarExpr_v));
            TYPE_CPP_Identifier index (vdm_BC_GenMinus(Max, GenLenString_int(tmpVal_v)));
            TYPE_CPP_Expr sb1 (GenSubSequence(castVarExpr_v, type, vdm_BC_GenIntegerLit(0), index));
            TYPE_REP_TypeRep seqtp (mk_REP_SeqTypeRep(FindSeqElemType(type)));
            Tuple cgpmel (CGPatternMatchExcl(lp, mk_CG_VT(sb1, seqtp), pn_s, succ_v, pid_m, inner, nonstop));
            if (cgpmel.GetBool(2) && nonstop) {
              SEQ<TYPE_CPP_Stmt> rb (e_stmt);
              rb.ImpAppend(vdm_BC_GenIfStmt(cond, vdm_BC_GenBlock(cgpmel.GetSequence(1)), nil));
              return mk_(rb, Bool(true));
            }
            else {
              SEQ<TYPE_CPP_Stmt> rb (e_stmt);
              if (cgpmel.GetSequence(1).IsEmpty()) {
                rb.ImpAppend(vdm_BC_GenAsgnStmt(succ_v, cond));
              }
              else {
                rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenAsgnExpr(succ_v, cond),
                                              vdm_BC_GenBlock(cgpmel.GetSequence(1)), nil));
              }
              return mk_(rb, Bool(false));
            }
          }
        }
      } // endif (vdm_CPP_isJAVA())
      else
#endif // VDMPP
      { // C++
        if (lp.Is(TAG_TYPE_AS_MatchVal)) {
          Tuple cgee (CGExprExcl(lp.GetRecord(pos_AS_MatchVal_val), ASTAUX::MkId(L"tmpVal_v"), nil));
          TYPE_AS_Expr tmpVal_v (cgee.GetRecord(1));
          SEQ<TYPE_CPP_Stmt> e_stmt (cgee.GetSequence(2));
          if (!tmpVal_v.Is(TAG_TYPE_CPP_Identifier)) {
            tmpVal_v = vdm_BC_GiveName(ASTAUX::MkId(L"tmpVal_v"));
            //e_stmt.ImpAppend(vdm_BC_GenDecl(GenType(type), tmpVal_v, vdm_BC_GenAsgnInit(cgee.GetRecord(1))));
            e_stmt.ImpAppend(vdm_BC_GenDecl(GenType(type), tmpVal_v,
                                            vdm_BC_GenObjectInit(mk_sequence(cgee.GetRecord(1)))));
          }
          TYPE_CPP_Expr cond (vdm_BC_GenFctCallObjMemAcc(castVarExpr_v, ASTAUX::MkId(L"StartsWith"),
                                                         mk_sequence(tmpVal_v)));
          TYPE_CPP_Identifier Max (GenLen_int(castVarExpr_v));
          TYPE_CPP_Identifier index (GenLen_int(tmpVal_v));
          TYPE_CPP_Expr sb2 (GenSubSequence(castVarExpr_v, type, index, Max));
          TYPE_REP_TypeRep seqtp (mk_REP_SeqTypeRep(FindSeqElemType(type)));
          Tuple cgpmer (CGPatternMatchExcl(rp, mk_CG_VT(sb2, seqtp), pn_s, succ_v, pid_m, Nil(), nonstop));
          if (cgpmer.GetBool(2) && nonstop) {
            SEQ<TYPE_CPP_Stmt> rb (e_stmt);
            rb.ImpAppend(vdm_BC_GenIfStmt(cond, vdm_BC_GenBlock(cgpmer.GetSequence(1)), nil));
            return mk_(rb, Bool(true));
          }
          else {
            SEQ<TYPE_CPP_Stmt> rb (e_stmt);
            if (cgpmer.GetSequence(1).IsEmpty()) {
              rb.ImpAppend(vdm_BC_GenAsgnStmt(succ_v, cond));
            }
            else {
              rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenAsgnExpr(succ_v, cond),
                                            vdm_BC_GenBlock(cgpmer.GetSequence(1)), nil));
            }
            return mk_(rb, Bool(false));
          }
        }
        if (rp.Is(TAG_TYPE_AS_MatchVal)) {
          Tuple cgee (CGExprExcl(rp.GetRecord(pos_AS_MatchVal_val), ASTAUX::MkId(L"tmpVal_v"), nil));
          TYPE_AS_Expr tmpVal_v (cgee.GetRecord(1));
          SEQ<TYPE_CPP_Stmt> e_stmt (cgee.GetSequence(2));
          if (!tmpVal_v.Is(TAG_TYPE_CPP_Identifier)) {
            tmpVal_v = vdm_BC_GiveName(ASTAUX::MkId(L"tmpVal_v"));
            //e_stmt.ImpAppend(vdm_BC_GenDecl(GenType(type), tmpVal_v, vdm_BC_GenAsgnInit(cgee.GetRecord(1))));
            e_stmt.ImpAppend(vdm_BC_GenDecl(GenType(type), tmpVal_v,
                                            vdm_BC_GenObjectInit(mk_sequence(cgee.GetRecord(1)))));
          }
          TYPE_CPP_Expr cond (vdm_BC_GenFctCallObjMemAcc(castVarExpr_v, ASTAUX::MkId(L"EndsWith"),
                                                         mk_sequence(tmpVal_v)));
          TYPE_CPP_Identifier Max (GenLen_int(castVarExpr_v));
          TYPE_CPP_Identifier index (vdm_BC_GenMinus(Max, GenLen_int(tmpVal_v)));
          TYPE_CPP_Expr sb1 (GenSubSequence(castVarExpr_v, type, vdm_BC_GenIntegerLit(0), index));
          TYPE_REP_TypeRep seqtp (mk_REP_SeqTypeRep(FindSeqElemType(type)));
          Tuple cgpmel (CGPatternMatchExcl(lp, mk_CG_VT(sb1, seqtp), pn_s, succ_v, pid_m, inner, nonstop));
          if (cgpmel.GetBool(2) && nonstop) {
            SEQ<TYPE_CPP_Stmt> rb (e_stmt);
            rb.ImpAppend(vdm_BC_GenIfStmt(cond, vdm_BC_GenBlock(cgpmel.GetSequence(1)), nil));
            return mk_(rb, Bool(true));
          }
          else {
            SEQ<TYPE_CPP_Stmt> rb (e_stmt);
            if (cgpmel.GetSequence(1).IsEmpty()) {
              rb.ImpAppend(vdm_BC_GenAsgnStmt(succ_v, cond));
            }
            else {
              rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenAsgnExpr(succ_v, cond),
                                            vdm_BC_GenBlock(cgpmel.GetSequence(1)), nil));
            }
            return mk_(rb, Bool(false));
          }
        }
      }

      TYPE_CPP_Identifier index (vdm_BC_GiveName(ASTAUX::MkId(L"index")));
      TYPE_CPP_Identifier to (vdm_BC_GiveName(ASTAUX::MkId(L"to")));
      TYPE_CPP_Identifier Max (vdm_BC_GiveName(ASTAUX::MkId(L"max")));

      SEQ<TYPE_CPP_Stmt> javarb;
      TYPE_CPP_Expr castVarExpr_v_q = castVarExpr_v;
      TYPE_CPP_Expr sb1;
      TYPE_CPP_Expr sb2;
      TYPE_CPP_Expr len_var;
#ifdef VDMPP
      if (vdm_CPP_isJAVA()) {
        if (!IsSeqType(type)) {
          TYPE_CPP_Expr tmpVal_v (vdm_BC_GiveName(ASTAUX::MkId(L"tmpVal_v")));
          javarb.ImpAppend(vdm_BC_GenDecl(GenSeq0Type(), tmpVal_v,
                                vdm_BC_GenAsgnInit(vdm_BC_GenCondExpr(GenIsString(varExpr_v),
                                                                      GetListVal(varExpr_v),
                                                                      GenCastSeq(varExpr_v, Nil())))));
          castVarExpr_v_q = tmpVal_v;
        }
        sb1 = GenSubSequence(castVarExpr_v_q, type, vdm_BC_GenIntegerLit(0), index);
        sb2 = GenSubSequence(castVarExpr_v_q, type, index, Max);
        len_var = (IsStringType(type) ? GenLenString_int(castVarExpr_v) : GenLen_int(castVarExpr_v_q));
      }
      else
#endif // VDMPP
      {
        sb1 = GenSubSequence(castVarExpr_v_q, type, vdm_BC_GenIntegerLit(1), index);
        sb2 = GenSubSequence(castVarExpr_v_q, type, vdm_BC_GenPlus(index, vdm_BC_GenIntegerLit(1)), Max);
        len_var = GenLen_int(castVarExpr_v);
      }

      TYPE_REP_TypeRep seqtp (mk_REP_SeqTypeRep(FindSeqElemType(type)));

      Tuple cgpmel (CGPatternMatchExcl(rp, mk_CG_VT(sb2, seqtp), pn_s, succ_v, pid_m, Nil(), false));
      Tuple cgpmer (CGPatternMatchExcl(lp, mk_CG_VT(sb1, seqtp),
                                       pn_s.Union(FindPatternId (rp).Dom ()), succ_v, pid_m, inner, false));

      bool Is_excl = cgpmel.GetBoolValue(2) && cgpmer.GetBoolValue(2);

      SEQ<TYPE_CPP_Stmt> inner_rb_l;
      if (Is_excl && inner.IsNil()) {
        inner_rb_l.ImpAppend(vdm_BC_GenAsgnStmt(succ_v, vdm_BC_GenBoolLit(true)));
      }
      inner_rb_l.ImpConc(cgpmel.GetSequence(1));
      if (!cgpmer.GetSequence(1).IsEmpty()) {
        if (cgpmel.GetBoolValue(2)) {
          inner_rb_l.ImpConc(cgpmer.GetSequence(1));
        }
        else {
          inner_rb_l.ImpAppend(vdm_BC_GenIfStmt(succ_v, vdm_BC_GenBlock(cgpmer.GetSequence(1)), nil));
        }
      }

      SEQ<TYPE_CPP_Stmt> ifBody (javarb); 
      ifBody.ImpAppend(vdm_BC_GenDecl(GenSmallIntType(), Max, vdm_BC_GenAsgnInit(len_var)));

      if (lp.Is(TAG_TYPE_AS_SeqEnumPattern)) {
        TYPE_CPP_AsgnInit ai (vdm_BC_GenAsgnInit(
                     vdm_BC_GenIntegerLit(lp.GetSequence(pos_AS_SeqEnumPattern_els).Length())));
        TYPE_CPP_Expr cond (vdm_BC_GenLeq(index, Max));
        ifBody.ImpAppend(vdm_BC_GenDecl(GenSmallIntType(), index, ai));
        ifBody.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenAsgnExpr(succ_v, vdm_BC_GenBracketedExpr(cond)),
                                          vdm_BC_GenBlock(inner_rb_l), nil));
      }
      else if (rp.Is(TAG_TYPE_AS_SeqEnumPattern)) {
        TYPE_CPP_AsgnInit ai (vdm_BC_GenAsgnInit(
           vdm_BC_GenMinus(Max, vdm_BC_GenIntegerLit(rp.GetSequence(pos_AS_SeqEnumPattern_els).Length()))));
        TYPE_CPP_Expr cond (vdm_BC_GenGeq(index, vdm_BC_GenIntegerLit(0)));
        ifBody.ImpAppend(vdm_BC_GenDecl(GenSmallIntType(), index, ai));
        ifBody.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenAsgnExpr(succ_v, vdm_BC_GenBracketedExpr(cond)),
                                          vdm_BC_GenBlock(inner_rb_l), nil));
      }
      else {
        TYPE_CPP_AsgnInit ai_from (vdm_BC_GenAsgnInit(
                   vdm_BC_GenIntegerLit(lp.Is(TAG_TYPE_AS_SeqConcPattern) ? 2 : 1 )));
        TYPE_CPP_AsgnInit ai_to (vdm_BC_GenAsgnInit(len_var));
        TYPE_CPP_Stmt ini (vdm_BC_GenDecl(GenSmallIntType(), index, ai_from));
        TYPE_CPP_Expr e1 (vdm_BC_GenLogAnd(vdm_BC_GenBracketedExpr(vdm_BC_GenLeq(index, to)), vdm_BC_GenNot(succ_v)));
        TYPE_CPP_Expr e2 (vdm_BC_GenPostPlusPlus(index));

        ifBody.ImpAppend(vdm_BC_GenAsgnStmt(succ_v, vdm_BC_GenBoolLit(false)));
        ifBody.ImpAppend(vdm_BC_GenDecl(GenSmallIntType(), to, ai_to));
        ifBody.ImpAppend(vdm_BC_GenForStmt(ini, e1, type_dL().ImpAppend(e2), vdm_BC_GenBlock(inner_rb_l)));
      }

      if (IsSeqType(type)) {
        return mk_(ifBody, Bool(false));
      }
      else {
        TYPE_CPP_Expr cond;
#ifdef VDMPP
        if (vdm_CPP_isJAVA()) {
          cond = vdm_BC_GenLogOr(GenIsSeq(varExpr_v), GenIsString(varExpr_v));
        }
        else
#endif // VDMPP
        {
          cond = GenIsSeq(varExpr_v);
        }
       
        SEQ<TYPE_CPP_Stmt> rb;
        rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenAsgnExpr(succ_v, vdm_BC_GenBracketedExpr(cond)),
                                      vdm_BC_GenBlock(ifBody), nil));
        return mk_(rb, Bool(false));
      }
    }
  }
  this->ContainsNotSupportedConstruct = true;
  this->NotSupportedStmt = SEQ<TYPE_CPP_Stmt>().ImpAppend(NotSupported(L"Complex sequence concatenation pattern", pat));
  return mk_(this->NotSupportedStmt, Bool(false));
}

// CGMatchMapMergePattern
// pat : AS`MapMergePattern
// rc2 : CGMAIN`VT
// pn_s : set of (AS`Name | AS`OldName)
// succ_v : CPP`Name
// pid_m : [map (AS`Name | AS`OldName) to REP`TypeRep]
// inner : [seq of CPP`Stmt]
// ==> seq of CPP`Stmt * bool
Tuple vdmcg::CGMatchMapMergePattern (const TYPE_AS_MapMergePattern & pat,
                                     const TYPE_CGMAIN_VT & rc2,
                                     const Set & pn_s,
                                     const TYPE_CPP_Name & succ_v,
                                     const Map & pid_m,
                                     const Generic & inner,
                                     bool nonstop)
{
  const TYPE_AS_Pattern & lp (pat.GetRecord(pos_AS_MapMergePattern_lp));
  const TYPE_AS_Pattern & rp (pat.GetRecord(pos_AS_MapMergePattern_rp));
  const TYPE_CPP_Expr & varExpr_v (rc2.GetRecord(pos_CGMAIN_VT_name));
  const Generic & type (rc2.GetRecord(pos_CGMAIN_VT_type));

  if (!type.IsNil() &&
      (lp.Is (TAG_TYPE_AS_PatternName) || lp.Is (TAG_TYPE_AS_MapEnumPattern)) &&
      (rp.Is (TAG_TYPE_AS_PatternName) || rp.Is (TAG_TYPE_AS_MapEnumPattern))) {
    if(!IsPosMapType(type)) {
      SEQ<TYPE_CPP_Stmt> rb;
      rb.ImpAppend(vdm_BC_GenSingleLineComments(SEQ<Char>(L"never match")));
      rb.ImpAppend(vdm_BC_GenAsgnStmt(succ_v, vdm_BC_GenBoolLit(false)));
      return mk_(rb, Bool(false));
    }

    TYPE_CPP_Expr castVarExpr_v (IsMapType(type) ? varExpr_v : GenCastMap(varExpr_v));

    if (lp.Is (TAG_TYPE_AS_PatternName) && lp.GetField(pos_AS_PatternName_nm).IsNil() &&
        rp.Is (TAG_TYPE_AS_PatternName) && rp.GetField(pos_AS_PatternName_nm).IsNil() ) {
      // - munion -
      if (!inner.IsNil()) {
        if (nonstop) {
          SEQ<TYPE_CPP_Stmt> rb;
          rb.ImpAppend(vdm_BC_GenIfStmt(GenIsMap(varExpr_v), vdm_BC_GenBlock(inner), nil));
          return mk_(rb, Bool(true));
        }
        else {
          SEQ<TYPE_CPP_Stmt> rb;
          rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenAsgnExpr(succ_v, vdm_BC_GenBracketedExpr(GenIsMap(varExpr_v))),
                                        vdm_BC_GenBlock(inner), nil));
          return mk_(rb, Bool(false));
        }
      }
      else {
        SEQ<TYPE_CPP_Stmt> rb;
        rb.ImpAppend(vdm_BC_GenAsgnStmt(succ_v, vdm_BC_GenBracketedExpr(GenIsMap(varExpr_v))));
        return mk_(rb, Bool(false));
      }
    }
    else if ((lp.Is (TAG_TYPE_AS_MapEnumPattern) &&
              (lp.GetSequence(pos_AS_MapEnumPattern_mls).Length() == 1) &&
              Record(lp.GetSequence(pos_AS_MapEnumPattern_mls)[1]).GetRecord(pos_AS_MapletPattern_dp).Is(TAG_TYPE_AS_PatternName) &&
              Record(lp.GetSequence(pos_AS_MapEnumPattern_mls)[1]).GetRecord(pos_AS_MapletPattern_dp).GetField(pos_AS_PatternName_nm).IsNil() &&
              Record(lp.GetSequence(pos_AS_MapEnumPattern_mls)[1]).GetRecord(pos_AS_MapletPattern_rp).Is(TAG_TYPE_AS_PatternName) &&
              Record(lp.GetSequence(pos_AS_MapEnumPattern_mls)[1]).GetRecord(pos_AS_MapletPattern_rp).GetField(pos_AS_PatternName_nm).IsNil() &&
             rp.Is (TAG_TYPE_AS_PatternName) && rp.GetField(pos_AS_PatternName_nm).IsNil() )
            ||
            (lp.Is (TAG_TYPE_AS_PatternName) && lp.GetField(pos_AS_PatternName_nm).IsNil() &&
             rp.Is (TAG_TYPE_AS_MapEnumPattern) &&
             (rp.GetSequence(pos_AS_MapEnumPattern_mls).Length() == 1) &&
             Record(rp.GetSequence(pos_AS_MapEnumPattern_mls)[1]).GetRecord(pos_AS_MapletPattern_dp).Is(TAG_TYPE_AS_PatternName) &&
             Record(rp.GetSequence(pos_AS_MapEnumPattern_mls)[1]).GetRecord(pos_AS_MapletPattern_dp).GetField(pos_AS_PatternName_nm).IsNil() &&
             Record(rp.GetSequence(pos_AS_MapEnumPattern_mls)[1]).GetRecord(pos_AS_MapletPattern_rp).Is(TAG_TYPE_AS_PatternName) &&
             Record(rp.GetSequence(pos_AS_MapEnumPattern_mls)[1]).GetRecord(pos_AS_MapletPattern_rp).GetField(pos_AS_PatternName_nm).IsNil()))
    {
      // { - |-> - } munion -
      // - munion { - |-> - }
      TYPE_CPP_Expr cond (IsMapType(type) 
                            ? vdm_BC_GenNot(GenMapIsEmpty(castVarExpr_v))
                            : vdm_BC_GenLogAnd(GenIsMap(varExpr_v), vdm_BC_GenNot(GenMapIsEmpty(castVarExpr_v))));
      if (!inner.IsNil()) {
        if (nonstop) {
          SEQ<TYPE_CPP_Stmt> rb;
          rb.ImpAppend(vdm_BC_GenIfStmt(cond, vdm_BC_GenBlock(inner), nil));
          return mk_(rb, Bool(true));
        }
        else {
          SEQ<TYPE_CPP_Stmt> rb;
          rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenAsgnExpr(succ_v, vdm_BC_GenBracketedExpr(cond)),
                                        vdm_BC_GenBlock(inner), nil));
          return mk_(rb, Bool(false));
        }
      }
      else {
        SEQ<TYPE_CPP_Stmt> rb;
        rb.ImpAppend(vdm_BC_GenAsgnStmt(succ_v, vdm_BC_GenBracketedExpr(cond)));
        return mk_(rb, Bool(false));
      }
    }
    else {
      TYPE_REP_TypeRep domtp(FindMapDomType(type));
      TYPE_REP_TypeRep rngtp(FindMapRngType(type));
      
      TYPE_CPP_Identifier elem_v (vdm_BC_GiveName(ASTAUX::MkId(L"elem")));
      TYPE_CPP_Identifier tempDom_v (vdm_BC_GiveName(ASTAUX::MkId(L"tempDom")));
      TYPE_CPP_Identifier tempSet1_v (vdm_BC_GiveName(ASTAUX::MkId(L"tempSet1")));
      TYPE_CPP_Identifier tempSet2_v (vdm_BC_GiveName(ASTAUX::MkId(L"tempSet2")));
      TYPE_CPP_Identifier tempSet3_v (vdm_BC_GiveName(ASTAUX::MkId(L"tempSet3")));
      TYPE_CPP_Identifier alreadySet_v (vdm_BC_GiveName(ASTAUX::MkId(L"alreadySet")));
      TYPE_CPP_Identifier alelem_v (vdm_BC_GiveName(ASTAUX::MkId(L"alelem")));

      SEQ<TYPE_CPP_Stmt> inner_rb_l;
      inner_rb_l.ImpConc(GenDeclSet(tempSet1_v, alelem_v));
      inner_rb_l.ImpAppend(GenSetInsert(tempSet1_v, elem_v));
      inner_rb_l.ImpAppend(GenSetInsert(alreadySet_v, tempSet1_v));
      inner_rb_l.ImpConc(GenDeclSet(tempSet2_v, tempDom_v));
      inner_rb_l.ImpAppend(GenSetMinusStmt(tempSet2_v, tempSet1_v));

      TYPE_CPP_Expr tm1v = vdm_BC_GiveName(ASTAUX::MkId(L"tempMap1"));
      TYPE_CPP_Expr tm2v = vdm_BC_GiveName(ASTAUX::MkId(L"tempMap2"));
      Generic res1_v (GenMapRestToBy(mk_CG_VT(tempSet1_v, mk_REP_SetTypeRep(domtp)),
                                     mk_CG_VT(castVarExpr_v, mk_REP_GeneralMapTypeRep(domtp, rngtp)),
                                     mk_CG_VT(tm1v, mk_REP_GeneralMapTypeRep(domtp, rngtp)),
                                     Int(MAPDOMRESTTO)));
      Generic res2_v (GenMapRestToBy(mk_CG_VT(tempSet2_v, mk_REP_SetTypeRep(domtp)),
                                     mk_CG_VT(castVarExpr_v, mk_REP_GeneralMapTypeRep(domtp, rngtp)),
                                     mk_CG_VT(tm2v, mk_REP_GeneralMapTypeRep(domtp, rngtp)),
                                     Int(MAPDOMRESTTO)));
   
      if (IsSeqOfCPPStmt(res1_v)) {
        inner_rb_l.ImpConc(GenDeclEmptyMap(mk_REP_GeneralMapTypeRep(domtp, rngtp), tm1v));
        inner_rb_l.ImpConc(res1_v);
      }
      else {
        tm1v = res1_v;
      }
      if (IsSeqOfCPPStmt(res2_v)) {
        inner_rb_l.ImpConc(GenDeclEmptyMap(mk_REP_GeneralMapTypeRep(domtp, rngtp), tm2v));
        inner_rb_l.ImpConc(res2_v);
      }
      else {
        tm2v = res2_v;
      }

      TYPE_AS_Pattern lp_q, rp_q;
      if (!lp.Is(TAG_TYPE_AS_MapEnumPattern) && rp.Is(TAG_TYPE_AS_MapEnumPattern)) {
        lp_q = rp;
        rp_q = lp;
      }
      else {
        lp_q = lp;
        rp_q = rp;
      }

      Tuple cgpmel (CGPatternMatchExcl(lp_q, mk_CG_VT(tm1v, mk_REP_GeneralMapTypeRep(domtp, rngtp)),
                                       pn_s, succ_v, pid_m, Nil(), false));
      Tuple cgpmer (CGPatternMatchExcl(rp_q, mk_CG_VT(tm2v, mk_REP_GeneralMapTypeRep(domtp, rngtp)),
                                       pn_s.Union(FindPatternId (lp_q).Dom ()), succ_v, pid_m, inner, false));

      bool Is_excl = cgpmel.GetBoolValue(2) && cgpmer.GetBoolValue(2);

      if (Is_excl && inner.IsNil()) {
        inner_rb_l.ImpAppend(vdm_BC_GenAsgnStmt(succ_v, vdm_BC_GenBoolLit(true)));
      }
      inner_rb_l.ImpConc(cgpmel.GetSequence(1));

      if (!cgpmer.GetSequence(1).IsEmpty()) {
        if (cgpmel.GetBoolValue(2)) {
          inner_rb_l.ImpConc(cgpmer.GetSequence(1));
        }
        else {
          inner_rb_l.ImpAppend(vdm_BC_GenIfStmt(succ_v, vdm_BC_GenBlock(cgpmer.GetSequence(1)), nil));
        }
      }

      SEQ<TYPE_CPP_Stmt> outer_rb_l;
      outer_rb_l.ImpConc(GenDeclSet(tempSet3_v, alreadySet_v));
      outer_rb_l.ImpConc (GenIterSet (mk_CG_VT (tempSet3_v, mk_REP_SetTypeRep(mk_REP_SetTypeRep(domtp))),
                          vdm_BC_GenNot(succ_v),
                          mk_CG_VT (alelem_v, mk_REP_SetTypeRep(domtp)), inner_rb_l));

      SEQ<TYPE_CPP_Stmt> ifBody;
      ifBody.ImpConc (GenDeclSet (tempDom_v, GenDom(castVarExpr_v)));
      ifBody.ImpConc (GenDeclEmptySet(alreadySet_v));
      ifBody.ImpAppend (GenSetInsert(alreadySet_v, GenEmptyValue(mk_REP_SetTypeRep(domtp))));
      ifBody.ImpAppend (vdm_BC_GenAsgnStmt(succ_v, vdm_BC_GenBoolLit(false)));
      ifBody.ImpConc (GenIterSet (mk_CG_VT (tempDom_v, mk_REP_SetTypeRep(domtp)),
                                  vdm_BC_GenNot(succ_v),
                                  mk_CG_VT (elem_v, domtp), outer_rb_l));

      if (!IsMapType(type)) {
        return mk_(ifBody, Bool(false)); 
      }
      else {
        SEQ<TYPE_CPP_Stmt> rb;
        rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenAsgnExpr(succ_v, vdm_BC_GenBracketedExpr(GenIsMap(varExpr_v))),
                                        vdm_BC_GenBlock(ifBody), nil));
        return mk_(rb, Bool(false)); 
      }
    }
  }
  this->ContainsNotSupportedConstruct = true;
  this->NotSupportedStmt = SEQ<TYPE_CPP_Stmt>().ImpAppend(NotSupported(L"Complex map merge pattern", pat));
  return mk_(this->NotSupportedStmt, Bool(false));
}

#ifdef VDMPP
// CGMatchObjectPattern
// pat : AS`ObjectPattern
// rc2 : CGMAIN`VT
// pn_s : set of (AS`Name | AS`OldName)
// succ_v : CPP`Name
// pid_m : [map (AS`Name | AS`OldName) to REP`TypeRep]
// inner : [seq of CPP`Stmt]
// ==> seq of CPP`Stmt * bool
Tuple vdmcg::CGMatchObjectPattern (const TYPE_AS_ObjectPattern & pat,
                                   const TYPE_CGMAIN_VT & rc2,
                                   const Set & pn_s,
                                   const TYPE_CPP_Name & succ_v,
                                   const Map & pid_m,
                                   const Generic & inner,
                                   bool nonstop)
{
  const TYPE_AS_Name & cls (pat.GetRecord(pos_AS_ObjectPattern_cls));
  const SEQ<TYPE_AS_FieldPattern> & fp_l (pat.GetSequence(pos_AS_ObjectPattern_fields));

  const TYPE_CPP_Expr & varExpr_v (rc2.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & type (rc2.GetRecord(pos_CGMAIN_VT_type));

  TYPE_CPP_Expr if_cond (GenAuxType(varExpr_v, mk_REP_ObjRefTypeRep(cls)));

  switch (fp_l.Length()) {
    case 0: {
      if (type.Is(TAG_TYPE_REP_ObjRefTypeRep)) // only if cls is identical
      {
        SEQ<TYPE_CPP_Stmt> rb;
        if (!inner.IsNil())
          rb.ImpConc(inner);
        return mk_(rb, Bool(true));
      }
      else
      {
        if (!inner.IsNil())
        {
          if (nonstop)
          {
            SEQ<TYPE_CPP_Stmt> rb;
            rb.ImpAppend(vdm_BC_GenIfStmt(if_cond, vdm_BC_GenBlock(inner), nil));
            return mk_(rb, Bool(true));
          }
          else
          {
            SEQ<TYPE_CPP_Stmt> rb;
            rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenAsgnExpr(succ_v, vdm_BC_GenBracketedExpr(if_cond)),
                                          vdm_BC_GenBlock(inner), nil));
            return mk_(rb, Bool(false));
          }
        }
        else 
        {
          SEQ<TYPE_CPP_Stmt> rb;
          rb.ImpAppend(vdm_BC_GenAsgnStmt(succ_v, vdm_BC_GenBracketedExpr(if_cond)));
          return mk_(rb, Bool(false));
        }
      }
      break;
    }
    case 1: {
      const TYPE_AS_Name & nm (fp_l[1].GetRecord(pos_AS_FieldPattern_nm));
      const TYPE_AS_Pattern & p (fp_l[1].GetRecord(pos_AS_FieldPattern_pat));

      TYPE_CPP_Expr cast;
      TYPE_CPP_Expr getfield;
      if (vdm_CPP_isJAVA()) {
        cast = (type.Is(TAG_TYPE_REP_ObjRefTypeRep) ? varExpr_v : CastToClassPtr(cls, varExpr_v));
        getfield = vdm_BC_GenObjectMemberAccess(vdm_BC_GenBracketedExpr(cast), vdm_BC_Rename2(nm));
      }
      else
      {
        cast = CastToClassPtr(cls, varExpr_v);
        getfield = vdm_BC_GenPointerToObjectMemberAccess(cast, vdm_BC_Rename(nm));
      }
      TYPE_REP_TypeRep f_type (FindType(p));
      Tuple cgpme (CGPatternMatchExcl(p, mk_CG_VT(getfield, f_type), pn_s, succ_v, pid_m, inner, nonstop));
      const Bool & Is_excl = cgpme.GetBool(2);
      const SEQ<TYPE_CPP_Stmt> & pm (cgpme.GetSequence(1));
      if (type.Is(TAG_TYPE_REP_ObjRefTypeRep)) {
        return mk_(pm, Bool(Is_excl));
      }
      else if (Is_excl.GetValue() && nonstop) {
        SEQ<TYPE_CPP_Stmt> rb;
        rb.ImpAppend(vdm_BC_GenIfStmt(if_cond, vdm_BC_GenBlock(pm), nil));
        return mk_(rb, Bool(true));
      }
      else {
        SEQ<TYPE_CPP_Stmt> rb;
        rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenAsgnExpr(succ_v, vdm_BC_GenBracketedExpr(if_cond)),
                                      vdm_BC_GenBlock(pm), nil));
        return mk_(rb, Bool(false));
      }
    }
    default: {
      TYPE_CPP_Identifier tmpTuple (vdm_BC_GiveName(ASTAUX::MkId(L"tmpTuple")));
      TYPE_CPP_Expr cast (type.Is(TAG_TYPE_REP_ObjRefTypeRep) && vdm_CPP_isJAVA() ? varExpr_v
                                                              : CastToClassPtr(cls, varExpr_v));

      SEQ<TYPE_AS_Pattern> p_l;
      SEQ<TYPE_REP_TypeRep> tp_l;
      SEQ<TYPE_CPP_Stmt> stmt_l;
      stmt_l.ImpConc(GenProductDecl_DS(tmpTuple, fp_l.Length(), Nil()));

      size_t len_fp_l = fp_l.Length();
      for (size_t i = 1; i <= len_fp_l; i++)
      {
        const TYPE_AS_FieldPattern & fp (fp_l[i]);
        const TYPE_AS_Name & nm (fp.GetRecord(pos_AS_FieldPattern_nm));
        const TYPE_AS_Pattern p (fp.GetRecord(pos_AS_FieldPattern_pat));
        TYPE_REP_TypeRep f_type (FindType(p));
        TYPE_CPP_Expr getfield (vdm_CPP_isJAVA()
                    ? vdm_BC_GenObjectMemberAccess(vdm_BC_GenBracketedExpr(cast), vdm_BC_Rename2(nm))
                    : vdm_BC_GenPointerToObjectMemberAccess(cast, vdm_BC_Rename(nm)));
        p_l.ImpAppend(p);
        tp_l.ImpAppend(f_type);
        stmt_l.ImpAppend(GenTupSetField(tmpTuple, vdm_BC_GenIntegerLit(i), getfield));
      }
      TYPE_REP_TypeRep new_p_type (mk_REP_ProductTypeRep(tp_l));
      Tuple cgml (CGMatchList (p_l, mk_CG_VT (tmpTuple, new_p_type), succ_v, pn_s, pid_m, inner, nonstop));
      const SEQ<TYPE_CPP_Stmt> & pm (cgml.GetSequence(1));
      const Bool & Is_excl (cgml.GetBool(2));

      stmt_l.ImpConc(pm);
      if (type.Is(TAG_TYPE_REP_ObjRefTypeRep))
        return mk_(stmt_l, Bool(Is_excl));
      else if (Is_excl.GetValue() && nonstop)
      {
        SEQ<TYPE_CPP_Stmt> rb;
        rb.ImpAppend(vdm_BC_GenIfStmt(if_cond, vdm_BC_GenBlock(stmt_l), nil));
        return mk_(rb, Bool(true));
      }
      else
      {
        SEQ<TYPE_CPP_Stmt> rb;
        rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenAsgnExpr(succ_v, vdm_BC_GenBracketedExpr(if_cond)),
                                      vdm_BC_GenBlock(stmt_l), nil));
        return mk_(rb, Bool(false));
      }
    }
  }
}
#endif // VDMPP

// CGMatchList
// p_l : seq of AS`Pattern
// vt : CGMAIN`VT
// succ_v : CPP`Name
// p_s : set of (AS`Name | AS`OldName)
// pid_m : [map (AS`Name | AS`OldName) to REP`TypeRep]
// inner : [seq of CPP`Stmt]
// ==> seq of CPP`Stmt * bool
Tuple vdmcg::CGMatchList (const SEQ<TYPE_AS_Pattern> & p_l,
                          const TYPE_CGMAIN_VT & vt,
                          const TYPE_CPP_Name & succ_v,
                          const Set & p_s,
                          const Map & pid_m,
                          const Generic & inner,
                          bool nonstop)
{
  const TYPE_CPP_Expr & perm1_v (vt.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & permtp (vt.GetRecord(pos_CGMAIN_VT_type));

#ifdef VDMPP
  bool charType = (permtp.Is(TAG_TYPE_REP_SeqTypeRep) &&
                  IsCharType(CleanFlatType(permtp.GetRecord(pos_REP_SeqTypeRep_elemtp))));
#endif // VDMPP
  
  TYPE_CPP_Expr perm_v;
  if (perm1_v.Is(TAG_TYPE_CPP_Identifier)) {
      perm_v = perm1_v;
  }
  else {
    switch (permtp.GetTag()) {
      case TAG_TYPE_REP_ProductTypeRep: {
        perm_v = perm1_v;
        break;
      }
      case TAG_TYPE_REP_CompositeTypeRep: {
        perm_v = perm1_v;
        break;
      }
      default: {
#ifdef VDMPP
        if (vdm_CPP_isJAVA()) {
          perm_v = (charType ? GenCastString(perm1_v) : GenCastSeq(perm1_v, permtp));
        }
        else
#endif // VDMPP
        {
          perm_v = GenCastSeq(perm1_v, permtp);
        }
      }
    }
  }

  if (p_l.IsEmpty ())
  {
    TYPE_CPP_Expr eq;
#ifdef VDMPP
    if (vdm_CPP_isJAVA()) {
      if (charType) {
        eq = vdm_BC_GenEq (vdm_BC_GenIntegerLit (0), GenLenString_int (perm_v));
      }
      else {
        eq = vdm_BC_GenEq (vdm_BC_GenIntegerLit (0), GenLen_int (perm_v));
      }
    }
    else
#endif // VDMPP
    {
      eq = vdm_BC_GenEq (vdm_BC_GenIntegerLit (0), GenLen (perm_v));
    }

    SEQ<TYPE_CPP_Stmt> rb;
    rb.ImpAppend(vdm_BC_GenAsgnStmt (succ_v, vdm_BC_GenBracketedExpr(eq)));
    return mk_(rb, Bool(false));
  }
  else
  {
    bool Is_excl = true;
    SEQ<TYPE_CPP_Stmt> ifBody;
    if (!inner.IsNil())
      ifBody.ImpConc(inner);
    size_t len_p_l_ = p_l.Length();
    for (size_t i = len_p_l_; i >= 1; i--)
    {
      const TYPE_AS_Pattern & pat (p_l[i]);

      TYPE_REP_TypeRep ptp (FindType(pat));

      TYPE_REP_TypeRep type;
      switch (permtp.GetTag()) {
        case TAG_TYPE_REP_ProductTypeRep: {
          const SEQ<TYPE_REP_TypeRep> & tp_l (permtp.GetSequence(pos_REP_ProductTypeRep_tps));
          type = CleanFlatType(tp_l[i]);
          break;
        }
        case TAG_TYPE_REP_CompositeTypeRep: {
          const SEQ<TYPE_REP_FieldRep> & field_l (permtp.GetField(pos_REP_CompositeTypeRep_fields));
          type = CleanFlatType(field_l[i].GetRecord(pos_REP_FieldRep_tp));
          break;
        }
        case TAG_TYPE_REP_SeqTypeRep: {
          type = CleanFlatType(permtp.GetRecord(pos_REP_SeqTypeRep_elemtp));
          break;
        }
        case TAG_TYPE_REP_SetTypeRep: {
          type = CleanFlatType(permtp.GetRecord(pos_REP_SetTypeRep_elemtp));
          break;
        }
        case TAG_TYPE_REP_GeneralMapTypeRep: {
          type = ((i % 2) == 1 ? CleanFlatType(permtp.GetRecord(pos_REP_GeneralMapTypeRep_mapdom))
                               : CleanFlatType(permtp.GetRecord(pos_REP_GeneralMapTypeRep_maprng)));
          break;
        }
      }

      TYPE_CPP_Expr permI_v;
#ifdef VDMPP
      if (vdm_CPP_isJAVA()) {
        switch (permtp.GetTag()) {
          case TAG_TYPE_REP_ProductTypeRep: {
            TYPE_REP_TypeRep tp (permtp.GetSequence(pos_REP_ProductTypeRep_tps)[i]);
            permI_v = GenExplicitCast(type, GenTupGetField(perm_v, vdm_BC_GenIntegerLit(i), tp, ptp), nil);
            break;
          }
          case TAG_TYPE_REP_CompositeTypeRep: {
            const SEQ<TYPE_REP_FieldRep> & fl (permtp.GetSequence(pos_REP_CompositeTypeRep_fields));
            const Generic & sel (fl[i].GetField(pos_REP_FieldRep_sel));

            TYPE_CPP_Name name;
            if (sel.IsNil())
              name = vdm_BC_GenIdentifier(ASTAUX::MkId(L"f").ImpConc(StringNumber(i)));
            else
              name = vdm_BC_Rename2(sel);
            TYPE_REP_TypeRep tp (fl[i].GetRecord(pos_REP_FieldRep_tp));
            permI_v = GenExplicitCast(type, GenRecGetField(perm_v, name, tp), nil);
            break;
          }
          default: {
            permI_v = (charType ? GenSeqApplyString(perm_v, vdm_BC_GenIntegerLit(i))
                                : GenSeqApply(type, perm_v, vdm_BC_GenIntegerLit(i)));
          }
        }
      }
      else
#endif // VDMPP
      { // C++
        switch (permtp.GetTag()) {
          case TAG_TYPE_REP_ProductTypeRep: {
            TYPE_REP_TypeRep tp (permtp.GetSequence(pos_REP_ProductTypeRep_tps)[i]);
            permI_v = GenTupGetField(perm_v, vdm_BC_GenIntegerLit(i), tp, ptp);
            break;
          }
          case TAG_TYPE_REP_CompositeTypeRep: {
            TYPE_REP_FieldRep fr (permtp.GetSequence(pos_REP_CompositeTypeRep_fields)[i]);
            const Generic & sel (fr.GetField(pos_REP_FieldRep_sel));
            if ( sel.IsNil() ) {
              TYPE_REP_TypeRep tp (fr.GetRecord(pos_REP_FieldRep_tp));
              permI_v = GenRecGetField(perm_v, vdm_BC_GenIntegerLit(i), tp);
            } else {
              permI_v = GenRecGetFieldNm(perm_v, permtp, sel);
            }
            break;
          }
          default: {
            TYPE_CPP_Expr apply (GenSeqApply(nil, perm_v, vdm_BC_GenIntegerLit(i)));
            switch (pat.GetTag()) {
              case TAG_TYPE_AS_PatternName: {
                Generic gtp (FindType(pat));
                if (gtp.IsNil() || IsSubType(type, gtp))
                  permI_v = apply;
                else
                  permI_v = GenCastType(type, apply);
                break;
              }
              case TAG_TYPE_AS_MatchVal: {
                permI_v = apply;
                break;
              }
              default: {
                permI_v = GenCastType(type, apply);
                break;
              }
            }
            break;
          }
        }
      }

      Set newp_s; // set of (AS`Name | AS`OldName)
      newp_s.ImpUnion(p_s);
      for (size_t ii = 1; ii < i; ii++)
        newp_s.ImpUnion (FindPatternId (p_l[ii]).Dom ());

      Generic p = Nil();
      if (!ifBody.IsEmpty())
        p = ifBody;

      Tuple cgpm (CGPatternMatchExcl(pat, mk_CG_VT(permI_v, type), newp_s, succ_v, pid_m, p, nonstop));
     
      Is_excl = Is_excl && cgpm.GetBoolValue(2);

      ifBody = SEQ<TYPE_CPP_Stmt>(cgpm.GetSequence(1));
    }

    if (permtp.Is(TAG_TYPE_REP_CompositeTypeRep))
    {
      return mk_(ifBody, Bool(Is_excl));
    }
    else if (permtp.Is(TAG_TYPE_REP_ProductTypeRep))
    {
      return mk_(ifBody, Bool(Is_excl));
    }
    else
    {
      TYPE_CPP_IntegerLit len_p_l (vdm_BC_GenIntegerLit(p_l.Length()));
      TYPE_CPP_Expr len_perm_v;
      switch (permtp.GetTag()) {
        //case TAG_TYPE_REP_ProductTypeRep: {
        //  len_perm_v = GenTupleLength(perm_v);
        //  break;
        //}
        default: {
#ifdef VDMPP
          if (vdm_CPP_isJAVA()) {
            len_perm_v = (charType ? GenLenString_int(perm_v) : GenLen_int(perm_v));
          }
          else
#endif // VDMPP
          {
            len_perm_v = GenLen_int(perm_v);
          }
          break;
        }
      }
      TYPE_CPP_Expr eq_len (vdm_BC_GenEq(len_p_l, len_perm_v));
      if (ifBody.IsEmpty())
      {
        SEQ<TYPE_CPP_Stmt> rb;
        rb.ImpAppend(vdm_BC_GenAsgnStmt(succ_v, vdm_BC_GenBracketedExpr(eq_len)));
        return mk_(rb, Bool(false));
      }
      else
      {
        if (Is_excl && nonstop)
        {
          SEQ<TYPE_CPP_Stmt> rb;
          rb.ImpAppend(vdm_BC_GenIfStmt(eq_len, vdm_BC_GenBlock(ifBody), nil));
          return mk_(rb, Bool(true));
        }
        else
        {
          SEQ<TYPE_CPP_Stmt> rb;
          rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenAsgnExpr(succ_v, vdm_BC_GenBracketedExpr(eq_len)),
                                        vdm_BC_GenBlock(ifBody), nil));
          return mk_(rb, Bool(false));
        }
      }
    }
  }
}

// FlattenSetUnionPattern
// p : AS`Pattern
// ==> AS`Pattern
TYPE_AS_Pattern vdmcg::FlattenSetUnionPattern(const TYPE_AS_Pattern & p)
{
  if (p.Is(TAG_TYPE_AS_SetUnionPattern)) {
    const TYPE_AS_Pattern & lp (FlattenSetUnionPattern(p.GetRecord(pos_AS_SetUnionPattern_lp)));
    const TYPE_AS_Pattern & rp (FlattenSetUnionPattern(p.GetRecord(pos_AS_SetUnionPattern_rp)));
    const TYPE_CI_ContextId & cid (p.GetInt(pos_AS_SetUnionPattern_cid));

    if (rp.Is(TAG_TYPE_AS_SetEnumPattern)) {
      switch (lp.GetTag()) {
        case TAG_TYPE_AS_SetEnumPattern: {
          SEQ<TYPE_AS_Pattern> els (lp.GetSequence(pos_AS_SetEnumPattern_Elems));
          els.ImpConc(rp.GetSequence(pos_AS_SetEnumPattern_Elems));
          return TYPE_AS_SetEnumPattern().Init(els, cid);
          break;
        }
        case TAG_TYPE_AS_SetUnionPattern: {
          const TYPE_AS_Pattern & llp (lp.GetRecord(pos_AS_SetUnionPattern_lp));
          const TYPE_AS_Pattern & lrp (lp.GetRecord(pos_AS_SetUnionPattern_rp));
          if (lrp.Is(TAG_TYPE_AS_SetEnumPattern)) {
            SEQ<TYPE_AS_Pattern> els (lrp.GetSequence(pos_AS_SetEnumPattern_Elems));
            els.ImpConc(rp.GetSequence(pos_AS_SetEnumPattern_Elems));
            return TYPE_AS_SetUnionPattern().Init(llp, TYPE_AS_SetEnumPattern().Init(els, cid), cid);
          }
          else if (llp.Is(TAG_TYPE_AS_SetEnumPattern)) {
            SEQ<TYPE_AS_Pattern> els (llp.GetSequence(pos_AS_SetEnumPattern_Elems));
            els.ImpConc(rp.GetSequence(pos_AS_SetEnumPattern_Elems));
            return TYPE_AS_SetUnionPattern().Init(llp, TYPE_AS_SetEnumPattern().Init(els, cid), cid);
          }
          else {
            return TYPE_AS_SetUnionPattern().Init(lp,rp,cid);
          }
          break;
        }
        default: {
          return TYPE_AS_SetUnionPattern().Init(lp,rp,cid);
        }
      }
    }
    else {
      return TYPE_AS_SetUnionPattern().Init(lp,rp,cid);
    }
  }
  else {
    return p;
  }
}

// FlattenSeqConcPattern
// p : AS`Pattern
// ==> AS`Pattern
TYPE_AS_Pattern vdmcg::FlattenSeqConcPattern(const TYPE_AS_Pattern & p)
{
  if (p.Is(TAG_TYPE_AS_SeqConcPattern)) {
    const TYPE_AS_Pattern & lp (FlattenSeqConcPattern(p.GetRecord(pos_AS_SeqConcPattern_lp)));
    const TYPE_AS_Pattern & rp (FlattenSeqConcPattern(p.GetRecord(pos_AS_SeqConcPattern_rp)));
    const TYPE_CI_ContextId & cid (p.GetInt(pos_AS_SeqConcPattern_cid));

    if (rp.Is(TAG_TYPE_AS_SeqEnumPattern)) {
      switch (lp.GetTag()) {
        case TAG_TYPE_AS_SeqEnumPattern: {
          SEQ<TYPE_AS_Pattern> els (lp.GetSequence(pos_AS_SeqEnumPattern_els));
          els.ImpConc(rp.GetSequence(pos_AS_SeqEnumPattern_els));
          return TYPE_AS_SeqEnumPattern().Init(els, cid);
          break;
        }
        case TAG_TYPE_AS_SeqConcPattern: {
          const TYPE_AS_Pattern & lrp (lp.GetRecord(pos_AS_SeqConcPattern_rp));
          TYPE_AS_SeqConcPattern scp (lp);
          if (lrp.Is(TAG_TYPE_AS_SeqEnumPattern)) {
            SEQ<TYPE_AS_Pattern> els (lrp.GetSequence(pos_AS_SeqEnumPattern_els));
            els.ImpConc(rp.GetSequence(pos_AS_SeqEnumPattern_els));
            scp.SetField(pos_AS_SeqConcPattern_rp, TYPE_AS_SeqEnumPattern().Init(els, cid));
            return scp;
          }
          else {
            return TYPE_AS_SeqConcPattern().Init(lp,rp,cid);
          }
          break;
        }
        default: {
          return TYPE_AS_SeqConcPattern().Init(lp,rp,cid);
        }
      }
    }
    else {
      return TYPE_AS_SeqConcPattern().Init(lp,rp,cid);
    }
  }
  else {
    return p;
  }
}

// FlattenMapMergePattern
// p : AS`Pattern
// ==> AS`Pattern
TYPE_AS_Pattern vdmcg::FlattenMapMergePattern(const TYPE_AS_Pattern & p)
{
  if (p.Is(TAG_TYPE_AS_MapMergePattern)) {
    const TYPE_AS_Pattern & lp (FlattenMapMergePattern(p.GetRecord(pos_AS_MapMergePattern_lp)));
    const TYPE_AS_Pattern & rp (FlattenMapMergePattern(p.GetRecord(pos_AS_MapMergePattern_rp)));
    const TYPE_CI_ContextId & cid (p.GetInt(pos_AS_MapMergePattern_cid));

    if (rp.Is(TAG_TYPE_AS_MapEnumPattern)) {
      switch (lp.GetTag()) {
        case TAG_TYPE_AS_MapEnumPattern: {
          SEQ<TYPE_AS_Pattern> mls (lp.GetSequence(pos_AS_MapEnumPattern_mls));
          mls.ImpConc(rp.GetSequence(pos_AS_MapEnumPattern_mls));
          return TYPE_AS_MapEnumPattern().Init(mls, cid);
          break;
        }
        case TAG_TYPE_AS_MapMergePattern: {
          const TYPE_AS_Pattern & llp (lp.GetRecord(pos_AS_MapMergePattern_lp));
          const TYPE_AS_Pattern & lrp (lp.GetRecord(pos_AS_MapMergePattern_rp));
          if (lrp.Is(TAG_TYPE_AS_MapEnumPattern)) {
            SEQ<TYPE_AS_Pattern> mls (lrp.GetSequence(pos_AS_MapEnumPattern_mls));
            mls.ImpConc(rp.GetSequence(pos_AS_MapEnumPattern_mls));
            return TYPE_AS_MapMergePattern().Init(llp, TYPE_AS_MapEnumPattern().Init(mls, cid), cid);
          }
          else if (llp.Is(TAG_TYPE_AS_MapEnumPattern)) {
            SEQ<TYPE_AS_Pattern> mls (llp.GetSequence(pos_AS_MapEnumPattern_mls));
            mls.ImpConc(rp.GetSequence(pos_AS_MapEnumPattern_mls));
            return TYPE_AS_MapMergePattern().Init(llp, TYPE_AS_MapEnumPattern().Init(mls, cid), cid);
          }
          else {
            return TYPE_AS_MapMergePattern().Init(lp,rp,cid);
          }
          break;
        }
        default: {
          return TYPE_AS_MapMergePattern().Init(lp,rp,cid);
        }
      }
    }
    else {
      return TYPE_AS_MapMergePattern().Init(lp,rp,cid);
    }
  }
  else {
    return p;
  }
}

// GenEqType
// typel : REP`TypeRep
// ==> (REP`SeqTypeRep | REP`ProductTypeRep)
TYPE_REP_TypeRep vdmcg::GenEqType (const TYPE_REP_TypeRep & type1)
{
  TYPE_REP_TypeRep type (CleanFlatType(type1));

  switch (type.GetTag()) {
    case TAG_TYPE_REP_CompositeTypeRep: {
      const SEQ<TYPE_REP_FieldRep> & field_l (type.GetField(pos_REP_CompositeTypeRep_fields));
      SEQ<TYPE_REP_TypeRep> type_l;
      size_t len_field_l = field_l.Length();
      for (size_t idx = 1; idx <= len_field_l; idx++)
        type_l.ImpAppend (field_l[idx].GetRecord(pos_REP_FieldRep_tp));

      if (len_field_l == 1)
        return mk_REP_SeqTypeRep(type_l.Hd());
      else
        return mk_REP_ProductTypeRep(type_l);
    }
    case TAG_TYPE_REP_ProductTypeRep: {
      return type;
    }
    default: {
      ReportError (L"GenEqType");
    }
  }
  return TYPE_REP_SeqTypeRep ().Init(TYPE_REP_AllTypeRep()); //to avoid g++ warnings
}

// HasComplexPattern
// p : AS`Pattern
// +> bool
bool vdmcg::HasComplexPattern(const TYPE_AS_Pattern & p)
{
  switch (p.GetTag ()) {
    case TAG_TYPE_AS_PatternName:
    case TAG_TYPE_AS_MatchVal: {
      return false;
    }
    case TAG_TYPE_AS_SetEnumPattern: {
      const SEQ<TYPE_AS_Pattern> & p_l (p.GetSequence(pos_AS_SetEnumPattern_Elems));
      size_t len_p_l = p_l.Length();
      bool exists = false;
      for (size_t idx = 1; (idx <= len_p_l) && !exists; idx++)
        exists = HasComplexPattern(p_l[idx]);
      return exists;
    }
    case TAG_TYPE_AS_SeqEnumPattern: {
      const SEQ<TYPE_AS_Pattern> & p_l (p.GetSequence(pos_AS_SeqEnumPattern_els));
      size_t len_p_l = p_l.Length();
      bool exists = false;
      for (size_t idx = 1; (idx <= len_p_l) && !exists; idx++)
        exists = HasComplexPattern(p_l[idx]);
      return exists;
    }
    case TAG_TYPE_AS_RecordPattern: {
      const SEQ<TYPE_AS_Pattern> & p_l (p.GetSequence(pos_AS_RecordPattern_fields));
      size_t len_p_l = p_l.Length();
      bool exists = false;
      for (size_t idx = 1; (idx <= len_p_l) && !exists; idx++)
        exists = HasComplexPattern(p_l[idx]);
      return exists;
    }
    case TAG_TYPE_AS_TuplePattern: {
      const SEQ<TYPE_AS_Pattern> & p_l (p.GetSequence(pos_AS_TuplePattern_fields));
      size_t len_p_l = p_l.Length();
      bool exists = false;
      for (size_t idx = 1; (idx <= len_p_l) && !exists; idx++)
        exists = HasComplexPattern(p_l[idx]);
      return exists;
    }
    case TAG_TYPE_AS_SetUnionPattern:
    case TAG_TYPE_AS_SeqConcPattern: {
      return true;
    }
    default: {
      return false;
    }
  }
}

// CountComplexPattern
// p AS`Pattern
// +> nat
int vdmcg::CountComplexPattern(const TYPE_AS_Pattern & p)
{
  switch (p.GetTag ()) {
    case TAG_TYPE_AS_PatternName:
    case TAG_TYPE_AS_MatchVal: {
      return 0;
    }
    case TAG_TYPE_AS_SetEnumPattern: {
      const SEQ<TYPE_AS_Pattern> & p_l (p.GetSequence(pos_AS_SetEnumPattern_Elems));
      int res = 0;
      size_t len_p_l = p_l.Length();
      for (size_t idx = 1; idx <= len_p_l; idx++)
        res += CountComplexPattern(p_l[idx]);
      return res;
    }
    case TAG_TYPE_AS_SeqEnumPattern: {
      const SEQ<TYPE_AS_Pattern> & p_l (p.GetSequence(pos_AS_SeqEnumPattern_els));
      int res = 0;
      size_t len_p_l = p_l.Length();
      for (size_t idx = 1; idx <= len_p_l; idx++)
        res += CountComplexPattern(p_l[idx]);
      return res;
    }
    case TAG_TYPE_AS_RecordPattern: {
      const SEQ<TYPE_AS_Pattern> & p_l (p.GetSequence(pos_AS_RecordPattern_fields));
      int res = 0;
      size_t len_p_l = p_l.Length();
      for (size_t idx = 1; idx <= len_p_l; idx++)
        res += CountComplexPattern(p_l[idx]);
      return res;
    }
    case TAG_TYPE_AS_TuplePattern: {
      const SEQ<TYPE_AS_Pattern> & p_l (p.GetSequence(pos_AS_TuplePattern_fields));
      int res = 0;
      size_t len_p_l = p_l.Length();
      for (size_t idx = 1; idx <= len_p_l; idx++)
        res += CountComplexPattern(p_l[idx]);
      return res;
    }
    case TAG_TYPE_AS_SetUnionPattern: {
      return CountComplexPattern(p.GetRecord(pos_AS_SetUnionPattern_lp)) +
             CountComplexPattern(p.GetRecord(pos_AS_SetUnionPattern_rp));
    }
    case TAG_TYPE_AS_SeqConcPattern: {
      return 1 + CountComplexPattern(p.GetRecord(pos_AS_SeqConcPattern_lp)) +
                 CountComplexPattern(p.GetRecord(pos_AS_SeqConcPattern_rp));
    }
#ifdef VDMPP
    case TAG_TYPE_AS_ObjectPattern: {
      const SEQ<TYPE_AS_FieldPattern> & p_l (p.GetSequence(pos_AS_ObjectPattern_fields));
      int res = 0;
      size_t len_p_l = p_l.Length();
      for (size_t idx = 1; idx <= len_p_l; idx++)
        res += CountComplexPattern(p_l[idx].GetRecord(pos_AS_FieldPattern_pat));
      return res;
    }
#endif // VDMPP
    default: {
      return 0;
    }
  }
}

// CountComplexPatternInBindList
// bl : AS`BindList
// +> nat
int vdmcg::CountComplexPatternInBindList(const SEQ<TYPE_AS_MultBind> & bl)
{
  int res = 0;
  size_t len_bl = bl.Length();
  for (size_t idx = 1; idx <= len_bl; idx++)
  {
    const TYPE_AS_MultBind & mb (bl[idx]);
    switch(mb.GetTag()) {
      case TAG_TYPE_AS_MultSetBind: {
        const SEQ<TYPE_AS_Pattern> & pat_l (mb.GetSequence(pos_AS_MultSetBind_pat));
        size_t len_pat_l = pat_l.Length();
        for (size_t i = 1; i <= len_pat_l; i++)
          res += CountComplexPattern(pat_l[i]);
        break;
      }
      case TAG_TYPE_AS_MultTypeBind: {
        // not here
        break;
      }
    }
  }
  return res;
}

