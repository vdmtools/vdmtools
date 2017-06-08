/***
*  * WHAT
*  *    mod_ds.cc: Implementation of mod_vdm_ds.vdm 1.149
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/mod_ds.cc,v $
*  * VERSION
*  *    $Revision: 1.65 $
*  * DATE
*  *    $Date: 2006/03/17 08:29:12 $
*  * STATUS
*  *    Under development
*  * REFERENCES
*  *
*  * PROJECT
*  *    IDERES/AFRODITE
*  * AUTHOR
*  *    Henrik Voss + $Author: vdmtools $ + jojo
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "vdmcg.h"
#include "contextinfo.h"
#include "asquotes.h"
#include "astaux.h"
#include "BC.h"
#include <math.h>
#include "statsem.h"
#include "intconvquotes.h"

//wcout << INT2Q::h2gAS(expr) << endl << flush;

// DS functions and operations

TYPE_AS_Id vdmcg::GiveLibName() const
{
  return ASTAUX::MkId(L"metaiv.h");
}

// GenType
// typee : REP`TypeRep
// ==> CPP`TypeSpecifier
TYPE_CPP_TypeSpecifier vdmcg::GenType(const TYPE_REP_TypeRep & type)
{
  switch (type.GetTag()) {
    case TAG_TYPE_REP_BooleanTypeRep: {
      return GenBoolType();
    }
    case TAG_TYPE_REP_NumericTypeRep: {
      switch (type.GetIntValue(pos_REP_NumericTypeRep_qtp)) {
        case NATONE:
        case NAT:
        case INTEGER:    { return GenIntType(); }
        case RAT:
        case REAL:       { return GenRealType(); }
      }
    }
    case TAG_TYPE_REP_TokenTypeRep: {
      return GenTokenType();
    }
    case TAG_TYPE_REP_CharTypeRep: {
      return GenCharType();
    }
    case TAG_TYPE_REP_QuoteTypeRep: {
      return GenQuoteType(type.GetRecord(pos_REP_QuoteTypeRep_lit));
    }

    case TAG_TYPE_REP_UnionTypeRep: {
      if (IsCompositeType (type))
        return GenRecordType(nil);
      else if (IsProductType(type))
        return GenProductType();
      else if (IsNumType(type))
        return GenNumType();
#ifdef VDMPP
      else if (IsObjRefType(type))
        return GenObjRefType_DS(nil);
#endif // VDMPP
      else if (IsSetType(type))
        return GenSetType();
      else if (IsMapType(type))
        return GenMapType();
      else if (IsSeqType(type)) {
#ifdef VDMPP
        if (vdm_CPP_isJAVA()) {
          if (IsPossibleStringType(type)) {
            return GenStringType();
          }
          else {
            return GenSeq0Type();
          }
        }
        else
#endif // VDMPP
        {
          return GenSeq0Type();
        }
      }
      else if (IsQuoteType(type)) {
#ifdef VDMPP
        if (vdm_CPP_isJAVA()) {
          return vdm_BC_GenGeneric();
        }
        else
#endif // VDMPP
          return GenQuoteType(nil);
      }
      else {
        return vdm_BC_GenGeneric();
      }
    }
    case TAG_TYPE_REP_ProductTypeRep: {
      return GenProductType();
    }
    case TAG_TYPE_REP_EmptySetTypeRep:
    case TAG_TYPE_REP_SetTypeRep: {
      return GenSetType();
    }
    case TAG_TYPE_REP_EmptySeqTypeRep:
    case TAG_TYPE_REP_SeqTypeRep: {
#ifdef VDMPP
      if (vdm_CPP_isJAVA()) {
        if (IsStringType(type)) {
          return GenStringType();
        }
        else {
          return GenSeq0Type();
        }
      }
      else
#endif // VDMPP
      {
        return GenSeq0Type();
      }
    }
    case TAG_TYPE_REP_EmptyMapTypeRep:
    case TAG_TYPE_REP_GeneralMapTypeRep:
    case TAG_TYPE_REP_InjectiveMapTypeRep: {
      return GenMapType();
    }
    case TAG_TYPE_REP_CompositeTypeRep: {
      return GenRecordType(type.GetRecord(pos_REP_CompositeTypeRep_nm));
    }
#ifdef VDMPP
    case TAG_TYPE_REP_ObjRefTypeRep: {
      return GenObjRefType_DS(type.GetRecord(pos_REP_ObjRefTypeRep_nm));
    }
#endif // VDMPP
    case TAG_TYPE_REP_NilTypeRep:
    case TAG_TYPE_REP_AllTypeRep:
    case TAG_TYPE_REP_PartialFnTypeRep:
    case TAG_TYPE_REP_TotalFnTypeRep : {
      return vdm_BC_GenGeneric();
    }
    case TAG_TYPE_REP_UnitTypeRep: {
      return vdm_BC_GenTypeSpecifier(vdm_BC_GenVoid());
    }
    case TAG_TYPE_REP_TypeNameRep: {
#ifdef VDMPP
      if (vdm_CPP_isJAVA()) {
        const TYPE_AS_Name & nm (type.GetRecord(pos_REP_TypeNameRep_nm));
        const TYPE_AS_Ids & ids (nm.GetSequence(pos_AS_Name_ids));
        if (ids.Length() > 1)
          if (ids.Hd() == GiveCurCName())
            return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(GiveLastName(nm)));
          else
            return vdm_BC_GenTypeSpecifier(vdm_BC_Rename(nm));
        else
          return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(GiveLastName(nm)));
      }
      else
#endif // VDMPP
        return vdm_BC_GenGeneric();
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return GenType(type.GetRecord(pos_REP_InvTypeRep_shape));
    }
    default: {
      return vdm_BC_GenGeneric();
    }
  }
}

// GenDecl_DS (GenDecl)
// tp : [ REP`TypeRep ]
// name : CPP`Name
// initExpr : [ CPP`Initializer ]
// ==> seq1 of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenDecl_DS(const Generic & tp, const TYPE_CPP_Name & name,
                                     const Generic & initExpr)
{
  if (tp.IsNil()) {
    SEQ<TYPE_CPP_Stmt> rb_l;
    rb_l.ImpAppend(vdm_BC_GenDecl(vdm_BC_GenGeneric(), name, initExpr));
    return rb_l;
  }
  else
  {
    TYPE_REP_TypeRep type (RemoveNil(tp));
    switch (type.GetTag()) {
      case TAG_TYPE_REP_BooleanTypeRep: {
        return GenBoolDecl(name, initExpr);
      }
      case TAG_TYPE_REP_NumericTypeRep: {
        switch(type.GetIntValue(pos_REP_NumericTypeRep_qtp)) {
          case INTEGER:
          case NAT:        { return GenIntDecl(name, initExpr); }
          case NATONE:     { return GenNatOneDecl(name, initExpr); }
          case RAT:
          case REAL:       { return GenRealDecl(name, initExpr); }
          default: {
            SEQ<TYPE_CPP_Stmt> rb_l;
            if (vdm_CPP_isJAVA() && initExpr.IsNil()) {
              rb_l.ImpAppend(vdm_BC_GenDecl(GenType(type), name, vdm_BC_GenAsgnInit(GenNullExpr())));
            }
            else {
              rb_l.ImpAppend(vdm_BC_GenDecl(GenType(type), name, initExpr));
            }
            return rb_l;
          }
        }
      }
      case TAG_TYPE_REP_TokenTypeRep: {
        return GenTokenDecl(name, initExpr);
      }
      case TAG_TYPE_REP_CharTypeRep: {
        return GenCharDecl(name, initExpr);
      }
      case TAG_TYPE_REP_QuoteTypeRep:
      case TAG_TYPE_REP_UnionTypeRep: {
        SEQ<TYPE_CPP_Stmt> rb_l;
#ifdef VDMPP
        if (vdm_CPP_isJAVA() && initExpr.IsNil())
          rb_l.ImpAppend(vdm_BC_GenDecl(GenType(type), name, vdm_BC_GenAsgnInit(GenNullExpr())));
        else
#endif // VDMPP
          rb_l.ImpAppend(vdm_BC_GenDecl(GenType(type), name, initExpr));
        return rb_l;
      }

      case TAG_TYPE_REP_GeneralMapTypeRep:
      case TAG_TYPE_REP_InjectiveMapTypeRep:
        return GenMapDecl_DS(name, initExpr);

      case TAG_TYPE_REP_SetTypeRep:
        return GenSetDecl_DS(name, initExpr);

      case TAG_TYPE_REP_SeqTypeRep: {
#ifdef VDMPP
        if (vdm_CPP_isJAVA()) {
          if (IsStringType(type)) {
            return GenStringDecl(name, initExpr);
          }
          else {
            return GenSeqDecl_DS(name, initExpr);
          }
        }
        else
#endif // VDMPP
          return GenSeqDecl_DS(name, initExpr);
      }

      case TAG_TYPE_REP_CompositeTypeRep: {
        const TYPE_AS_Name & tagname (type.GetRecord(pos_REP_CompositeTypeRep_nm));
#ifdef VDMPP
        if ( tagname.get_ids().Length() == 2 ) {
          IncludeClass(GiveFirstName(tagname));
        }
#endif // VDMPP
        return GenRecordDecl( tagname, name, initExpr);
      }

      case TAG_TYPE_REP_ProductTypeRep: {
        const SEQ<TYPE_REP_TypeRep> & type_l (type.GetSequence(pos_REP_ProductTypeRep_tps));
        return GenProductDecl_DS(name, Int(type_l.Length()), initExpr);
      }

#ifdef VDMPP
      case TAG_TYPE_REP_ObjRefTypeRep: {
        const TYPE_AS_Name & nm (type.GetRecord(pos_REP_ObjRefTypeRep_nm));
        IncludeClass(nm);
        return GenObjRefDecl(nm, name, initExpr);
      }
#endif // VDMPP

      case TAG_TYPE_REP_EmptySetTypeRep: {
#ifdef VDMPP
        if (vdm_CPP_isJAVA()) {
          return GenSetDecl_DS(name,initExpr);
        }
        else
#endif // VDMPP
        {
          SEQ<TYPE_CPP_Stmt> rb_l;
          rb_l.ImpAppend(vdm_BC_GenDecl( GenSetType(), name, initExpr ));
          return rb_l;
        }
      }

      case TAG_TYPE_REP_EmptySeqTypeRep: {
#ifdef VDMPP
        if (vdm_CPP_isJAVA()) {
          return GenSeqDecl_DS(name, initExpr);
        }
        else
#endif // VDMPP
        {
          SEQ<TYPE_CPP_Stmt> rb_l;
          rb_l.ImpAppend(vdm_BC_GenDecl( GenSeq0Type(), name, initExpr ));
          return rb_l;
        }
      }

      case TAG_TYPE_REP_EmptyMapTypeRep: {
#ifdef VDMPP
        if (vdm_CPP_isJAVA()) {
          return GenMapDecl_DS(name, initExpr);
        }
        else
#endif // VDMPP
        {
          SEQ<TYPE_CPP_Stmt> rb_l;
          rb_l.ImpAppend(vdm_BC_GenDecl( GenMapType(), name, initExpr ));
          return rb_l;
        }
      }

      case TAG_TYPE_REP_OpTypeRep: {
        const TYPE_REP_TypeRep & rng (type.GetRecord(pos_REP_OpTypeRep_Rng));
#ifdef VDMPP
        if (vdm_CPP_isJAVA()) {
          return GenDecl_DS(CleanFlatType(rng), name, initExpr);
        }
        else
#endif // VDMPP
          return GenGenericDecl(name, initExpr);
      }

      case TAG_TYPE_REP_NilTypeRep:
      case TAG_TYPE_REP_AllTypeRep:
      case TAG_TYPE_REP_PartialFnTypeRep:
      case TAG_TYPE_REP_TotalFnTypeRep:
        return GenGenericDecl(name, initExpr);

      case TAG_TYPE_REP_TypeNameRep: {
#ifdef VDMPP
        if (vdm_CPP_isJAVA()) {
          const TYPE_AS_Name & tagname (type.GetRecord(pos_REP_TypeNameRep_nm));
          const TYPE_AS_Ids & ids (tagname.GetSequence(pos_AS_Name_ids));
          TYPE_CPP_Identifier tpname;
          if ( ids.Length() == 2 ) {
            if (ids.Hd() == GiveCurCName())
              tpname = vdm_BC_GenIdentifier(GiveLastName(tagname));
            else
              tpname = vdm_BC_Rename(tagname);
          }
          else
            tpname = vdm_BC_GenIdentifier(GiveLastName(tagname));

          TYPE_CPP_TypeSpecifier ts (vdm_BC_GenTypeSpecifier(tpname));

          SEQ<TYPE_CPP_Stmt> rb_l;
          if (initExpr.IsNil())
            rb_l.ImpAppend(vdm_BC_GenDecl(ts, name, vdm_BC_GenAsgnInit(GenNullExpr())));
          else
            rb_l.ImpAppend(vdm_BC_GenDecl(ts, name, initExpr));
          return rb_l;
        }
        else
#endif // VDMPP
          return GenGenericDecl(name, initExpr);
      }
      case TAG_TYPE_REP_InvTypeRep: {
        return GenDecl_DS(type.GetRecord(pos_REP_InvTypeRep_shape), name, initExpr);
      }
      default:
        return GenGenericDecl(name, initExpr);
    }
  }
}

// GenDeclInit_DS (GenDeclInit)
// type : [ REP`TypeRep ]
// name : CPP`Name
// initExpr : CPP`Expr
// ==> seq1 of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenDeclInit_DS(const Generic & type,
                                         const TYPE_CPP_Name & name,
                                         const TYPE_CPP_Expr & initExpr)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return GenDecl_DS(type, name, vdm_BC_GenAsgnInit(initExpr));
  }
  else
#endif // VDMPP
    if (initExpr.Is(TAG_TYPE_CPP_FctCall) &&
        initExpr.GetRecord(pos_CPP_FctCall_fct).Is(TAG_TYPE_CPP_Identifier) && 
        initExpr.GetSequence(pos_CPP_FctCall_arg).IsEmpty()) {
      const SEQ<Char> & id (initExpr.GetRecord(pos_CPP_FctCall_fct).GetSequence(pos_CPP_Identifier_id));
      if ( id.StartsWith( SEQ<Char>(L"vdm_")) )
        return GenDecl_DS(type, name, vdm_BC_GenObjectInit(mk_sequence(initExpr)));
      else
        return GenDecl_DS(type, name, vdm_BC_GenAsgnInit(initExpr));
    }
    else {
      TYPE_CPP_Expr e (initExpr);
      if ( e.Is(TAG_TYPE_CPP_FctCall) ) {
        const TYPE_CPP_Expr & fct (e.GetRecord(pos_CPP_FctCall_fct));
        const TYPE_CPP_Exprs & arg (e.GetSequence(pos_CPP_FctCall_arg));
        if (!type.IsNil() && 
            ((IsIntType(type) && (fct == GenImplIntType().get_tp())) ||
             (IsRealType(type) && (fct == GenImplRealType().get_tp())) ||
             (IsBoolType(type) && (fct == GenBoolType().get_tp()))) &&
             (arg.Length() == 1) ) {
          e = arg[1];
        }
      }
      return GenDecl_DS(type, name, vdm_BC_GenObjectInit(mk_sequence(e)));
    }
}

// GenConstDeclInit
// type : [ REP`TypeRep ]
// name : CPP`Name
// initExpr : CPP`Expr
// ==> seq1 of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenConstDeclInit(const Generic & type,
                                           const TYPE_CPP_Name & name,
                                           const TYPE_CPP_Expr & initExpr)
{
  SEQ<TYPE_CPP_Stmt> stmts (GenDeclInit_DS(type, name, initExpr));
  int pos = stmts.Length();
  TYPE_CPP_DeclarationStmt stmt (stmts[pos]);
  if ( stmt.Is(TAG_TYPE_CPP_DeclarationStmt) ) {
    // add const
    TYPE_CPP_IdentDeclaration idecl (stmt.GetRecord(pos_CPP_DeclarationStmt_decl));
    SEQ<TYPE_CPP_DeclSpecifier> ds (idecl.GetSequence(pos_CPP_IdentDeclaration_ds));
    ds.ImpPrepend(vdm_BC_GenTypeSpecifier(quote_CONST));
    idecl.SetField(pos_CPP_IdentDeclaration_ds, ds);
    stmt.SetField(pos_CPP_DeclarationStmt_decl, idecl);
    stmts.ImpModify(pos, stmt);
  } 
  return stmts;
}

// GenValDecl
// type : REP`TypeRep
// name : AS`Name
// ==> seq1 of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenValDecl(const TYPE_REP_TypeRep & type, const TYPE_AS_Name & name)
{
  switch (type.GetTag()) {
    case TAG_TYPE_REP_CompositeTypeRep: {
      const TYPE_AS_Name & nm (type.GetRecord(pos_REP_CompositeTypeRep_nm));
      return mk_sequence(vdm_BC_GenDecl(GenRecordType(nm), vdm_BC_Rename(name), nil));
    }
    case TAG_TYPE_REP_ProductTypeRep: {
      return mk_sequence(vdm_BC_GenDecl(GenProductType(), vdm_BC_Rename(name), nil));
    }
    default: {
      return GenDecl_DS(type, vdm_BC_Rename(name), nil);
    }
  }
}

// GenIsType
// e : CPP`Expr
// type : REP`TypeRep
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenIsType(const TYPE_CPP_Expr & e, const TYPE_REP_TypeRep & type)
{
  TYPE_CPP_Expr expr_c (GenAuxType (e,type));
  return GenBoolExpr(expr_c);
}

// GenIsUnionType
// e : CPP`Expr
// utr : REP`UnionTypeRep
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenIsUnionType(const TYPE_CPP_Expr & e, const TYPE_REP_UnionTypeRep & utr)
{
  SET<TYPE_REP_TypeRep> t_s (utr.GetSet(pos_REP_UnionTypeRep_tps));
  TYPE_CPP_Expr expr;
  bool test = true;

  SET<TYPE_CPP_Expr> s;
  Generic t;
  for (bool bb = t_s.First(t); bb; bb = t_s.Next(t))
  {
    TYPE_CPP_Expr expr_c (GenAuxType (e,t));
    if (!s.InSet(expr_c))
    {
      s.Insert(expr_c);
      if (test)
        expr = expr_c;
      else
      {
        if (expr_c.Is(TAG_TYPE_CPP_LogicalExpr))
          expr = vdm_BC_GenLogOr (expr, vdm_BC_GenBracketedExpr(expr_c));
        else
          expr = vdm_BC_GenLogOr (expr, expr_c);
      }
    }
    test = false;
  }
  return expr;
}

// GenAuxType
// e : CPP`Expr
// t : REP`TypeRep
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenAuxType (const TYPE_CPP_Expr & e, const TYPE_REP_TypeRep & t)
{
  switch(t.GetTag()) {
    case TAG_TYPE_REP_BooleanTypeRep: {
      return GenIsBool(e);
    }
    case TAG_TYPE_REP_NumericTypeRep: {
      switch(t.GetIntValue(pos_REP_NumericTypeRep_qtp)) {
        case NATONE: { return GenIsNatOne(e); }
        case NAT: { return GenIsNat_DS(e); }
        case INTEGER: { return GenIsInt(e); }
        case RAT:
        case REAL: {
#ifdef VDMPP
          if (vdm_CPP_isJAVA()) {
            return GenIsReal(e);
          }
          else
#endif // VDMPP
            return vdm_BC_GenLogOr(GenIsInt(e), GenIsReal(e));
        }
      }
      break;
    }
    case TAG_TYPE_REP_TokenTypeRep: {
      return GenIsToken(e);
    }
    case TAG_TYPE_REP_CharTypeRep: {
      return GenIsChar(e);
    }
    case TAG_TYPE_REP_QuoteTypeRep: {
      // TODO:
      return GenIsQuote(e, t);
    }
    case TAG_TYPE_REP_CompositeTypeRep: {
        return GenIsThisRecord(t, e);
    }
    case TAG_TYPE_REP_TypeNameRep: {
#ifdef VDMPP
        if (vdm_CPP_isJAVA()) {
          return GenAuxType(e, ExpandTypeRep(t, Set()));
        }
        else
#endif // VDMPP
          ReportUndefined(L"GenAuxType");
    }
    case TAG_TYPE_REP_ProductTypeRep: {
      // TODO
      return GenIsTuple(e);
    }
    case TAG_TYPE_REP_SetTypeRep:
    case TAG_TYPE_REP_EmptySetTypeRep: {
      // TODO
      return GenIsSet(e);
    }
    case TAG_TYPE_REP_SeqTypeRep:
    case TAG_TYPE_REP_EmptySeqTypeRep: {
      // TODO
#ifdef VDMPP
      if (vdm_CPP_isJAVA()) {
        if (IsStringType(t)) {
          return GenIsString(e);
        }
        else {
          return GenIsSeq(e);
        }
      }
      else
#endif // VDMPP
        return GenIsSeq(e);
    }
    case TAG_TYPE_REP_GeneralMapTypeRep:
    case TAG_TYPE_REP_InjectiveMapTypeRep:
    case TAG_TYPE_REP_EmptyMapTypeRep: {
      // TODO
      return GenIsMap(e);
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      return GenIsUnionType(e, t);
    }
    case TAG_TYPE_REP_NilTypeRep: {
      return GenIsNil(e);
   }
#ifdef VDMPP
    case TAG_TYPE_REP_ObjRefTypeRep: {
      if (vdm_CPP_isJAVA()) {
        const TYPE_AS_Name & nm (t.GetRecord(pos_REP_ObjRefTypeRep_nm));
        TYPE_CPP_TypeSpecifier cls (vdm_BC_GenTypeSpecifier(vdm_BC_Rename(nm)));
        return vdm_BC_GenBracketedExpr(vdm_BC_GenTypeComp(cls, e));
      }
      else
      {
        const TYPE_AS_Name & nm (t.GetRecord(pos_REP_ObjRefTypeRep_nm));
        return vdm_BC_GenBracketedExpr(vdm_BC_GenLogAnd(vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"IsObjectRef"), SEQ<TYPE_CPP_Expr>()),
                                GenIsOfClassExpr(nm, GenCastType(t, e))));
      }
    }
#endif // VDMPP
    case TAG_TYPE_REP_InvTypeRep: {
//      return GenAuxType(e, t.GetRecord(pos_REP_InvTypeRep_shape));
      if (get_testpreandpost_option()) {
        const TYPE_AS_Name & name (t.GetRecord(pos_REP_InvTypeRep_name));
        const TYPE_REP_TypeRep & shape (t.GetRecord(pos_REP_InvTypeRep_shape));

        TYPE_AS_Ids ids (name.GetSequence(pos_AS_Name_ids));
        TYPE_CPP_Name fnm;
        if ( ids.Length() == 2 ) {
          TYPE_CPP_Name nm (vdm_BC_GivePrePostName(
                    ASTAUX::MkNameFromId(GiveLastName(name), name.get_cid()), ASTAUX::MkId(L"inv")));
          fnm = ASTAUX::Combine2Names(GiveFirstName(name), nm);
        } 
        else {
          fnm = vdm_BC_GivePrePostName(name, ASTAUX::MkId(L"inv"));
        }
        TYPE_CPP_Expr cast (vdm_BC_GenCastExpr(GenType(shape), e));
        TYPE_CPP_Expr fncall (vdm_BC_GenFctCall(vdm_BC_Rename(fnm), mk_sequence(cast)));

        return vdm_BC_GenLogAnd(GenAuxType(e, shape), vdm_BC_GenBracketedExpr(fncall));
      }
      else {
        return GenAuxType(e, t.GetRecord(pos_REP_InvTypeRep_shape));
      }
    }
    default: {
      ReportUndefined(wstring(L"GenAuxType: ") + GetStatSem().Type2Ascii(t));
    }
  }
  return Record (0,0); // To avoid warnings.
}

// GenGetValue
// name : CPP`Expr
// tp : [REP`TypeRep]
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenGetValue(const TYPE_CPP_Expr & name, const Generic & tp)
{
  if ( vdm_CPP_isCPP() && tp.IsNil() )
    return vdm_BC_GenFctCallObjMemAcc(name, ASTAUX::MkId(L"GetValue"), SEQ<TYPE_CPP_Expr>() );
  else if (StripBracketedExpr(name).Is(TAG_TYPE_CPP_ConditionalExpr)) {
    TYPE_CPP_Expr expr (StripBracketedExpr(name));
    const TYPE_CPP_Expr & cond (expr.GetRecord(pos_CPP_ConditionalExpr_cond));
    const TYPE_CPP_Expr & expr1 (expr.GetRecord(pos_CPP_ConditionalExpr_expr1));
    const TYPE_CPP_Expr & expr2 (expr.GetRecord(pos_CPP_ConditionalExpr_expr2));
    return vdm_BC_GenBracketedExpr(vdm_BC_GenCondExpr(cond, GenGetValue(expr1, tp), GenGetValue(expr2, tp)));
  }
  else
  {
    TYPE_REP_TypeRep type (tp);
    switch ( type.GetTag() ) {
      case TAG_TYPE_REP_BooleanTypeRep: {
#ifdef VDMPP
        if (vdm_CPP_isJAVA()) {
          TYPE_CPP_Expr name_q (StripBracketedExpr(name)); 
          switch (name_q.GetTag()) {
            case TAG_TYPE_CPP_ClassInstanceCreationExpr: {
              TYPE_CPP_Expr e (name_q.GetSequence(pos_CPP_ClassInstanceCreationExpr_arg).Hd());
              return (e.Is(TAG_TYPE_CPP_FctCall) ? e : vdm_BC_GenBracketedExpr(e));
            }
            case TAG_TYPE_CPP_FctCall: {
              const TYPE_CPP_Expr & fct (name_q.GetRecord(pos_CPP_FctCall_fct));
              if (fct.Is(TAG_TYPE_CPP_ObjectMemberAccess)) {
                if (fct.GetRecord(pos_CPP_ObjectMemberAccess_object) == GenBoolType().get_tp()) {
                  const TYPE_CPP_Expr & nm (fct.GetRecord(pos_CPP_ObjectMemberAccess_name));
                  const TYPE_CPP_Expr & e (name_q.GetSequence(pos_CPP_FctCall_arg).Hd());
                  if (nm == vdm_BC_GenIdentifier(ASTAUX::MkId(L"valueOf"))) {
                    return (e.Is(TAG_TYPE_CPP_FctCall) ? e : vdm_BC_GenBracketedExpr(e));
                  }
                }
              }
              break;
            }
            case TAG_TYPE_CPP_ObjectMemberAccess: {
              if (name_q.GetRecord(pos_CPP_ObjectMemberAccess_object) == GenBoolType().get_tp()) {
                const TYPE_CPP_Expr & nm (name_q.GetRecord(pos_CPP_ObjectMemberAccess_name));
                if (nm == vdm_BC_GenIdentifier(ASTAUX::MkId(L"TRUE"))) {
                  return TYPE_CPP_BoolLit().Init(Bool(true), nm.GetInt(pos_CPP_Identifier_cid));
                }
                else if (nm == vdm_BC_GenIdentifier(ASTAUX::MkId(L"FALSE"))) {
                  return TYPE_CPP_BoolLit().Init(Bool(false), nm.GetInt(pos_CPP_Identifier_cid));
                }
              }
              break;
            }
          }
          return vdm_BC_GenFctCallObjMemAcc(name, ASTAUX::MkId(L"booleanValue"), SEQ<TYPE_CPP_Expr>() );
        }
        else
#endif // VDMPP
        { // C++
          TYPE_CPP_Expr expr (StripBracketedExpr(name));
          switch(expr.GetTag()) {
            case TAG_TYPE_CPP_CastExpr: {
              const TYPE_CPP_Expr & e (expr.GetRecord(pos_CPP_CastExpr_expr));
              switch (e.GetTag()) {
                case TAG_TYPE_CPP_UnaryOpExpr:
                case TAG_TYPE_CPP_EqualityExpr:
                case TAG_TYPE_CPP_RelationalExpr:
                case TAG_TYPE_CPP_LogicalExpr: {
                  return vdm_BC_GenBracketedExpr(e);
                }
                case TAG_TYPE_CPP_FctCall: {
                  if (e.GetRecord(pos_CPP_FctCall_fct).Is(TAG_TYPE_CPP_ObjectMemberAccess))
                  {
                    const TYPE_CPP_Expr & nm (e.GetRecord(pos_CPP_FctCall_fct)
                                               .GetRecord(pos_CPP_ObjectMemberAccess_name));
                    if ((nm == vdm_BC_GenIdentifier(ASTAUX::MkId(L"Is"))) ||
                        (nm == vdm_BC_GenIdentifier(ASTAUX::MkId(L"IsBool"))) ||
                        (nm == vdm_BC_GenIdentifier(ASTAUX::MkId(L"IsChar"))) ||
                        (nm == vdm_BC_GenIdentifier(ASTAUX::MkId(L"IsInt"))) ||
                        (nm == vdm_BC_GenIdentifier(ASTAUX::MkId(L"IsReal"))) ||
                        (nm == vdm_BC_GenIdentifier(ASTAUX::MkId(L"IsToken"))) ||
                        (nm == vdm_BC_GenIdentifier(ASTAUX::MkId(L"IsTuple"))) ||
                        (nm == vdm_BC_GenIdentifier(ASTAUX::MkId(L"IsRecord"))) ||
                        (nm == vdm_BC_GenIdentifier(ASTAUX::MkId(L"IsSet"))) ||
                        (nm == vdm_BC_GenIdentifier(ASTAUX::MkId(L"IsSequence"))) ||
                        (nm == vdm_BC_GenIdentifier(ASTAUX::MkId(L"IsMap"))) ||
                        (nm == vdm_BC_GenIdentifier(ASTAUX::MkId(L"InSet"))) ||
                        (nm == vdm_BC_GenIdentifier(ASTAUX::MkId(L"SubSet"))) ||
                        (nm == vdm_BC_GenIdentifier(ASTAUX::MkId(L"PSubSet"))) ||
                        (nm == vdm_BC_GenIdentifier(ASTAUX::MkId(L"IsEmpty"))))
                    {
                      return e;
                    }
                  }
                  else if (e.GetRecord(pos_CPP_FctCall_fct).Is(TAG_TYPE_CPP_Identifier))
                  {
                    const TYPE_CPP_Identifier & id (e.GetRecord(pos_CPP_FctCall_fct));
                    if ((id == vdm_BC_GenIdentifier(ASTAUX::MkId(L"CGUTIL::is_real"))) ||
                        (id == vdm_BC_GenIdentifier(ASTAUX::MkId(L"CGUTIL::is_int"))) ||
                        (id == vdm_BC_GenIdentifier(ASTAUX::MkId(L"CGUTIL::is_nat"))) ||
                        (id == vdm_BC_GenIdentifier(ASTAUX::MkId(L"CGUTIL::is_nat1"))))
                    {
                      return e;
                    }
                  }
                  break;
                }
              }
              return vdm_BC_GenFctCallObjMemAcc(name, ASTAUX::MkId(L"GetValue"), SEQ<TYPE_CPP_Expr>() );
            }
            case TAG_TYPE_CPP_UnaryOpExpr: {
              if (expr.GetRecord(pos_CPP_UnaryOpExpr_op).GetField(pos_CPP_UnaryOp_val) == Quote(L"NEG"))
                return vdm_BC_GenNot(vdm_BC_GenBracketedExpr(GenGetValue(expr.GetRecord(pos_CPP_UnaryOpExpr_expr), tp)));
              return vdm_BC_GenFctCallObjMemAcc(name, ASTAUX::MkId(L"GetValue"), SEQ<TYPE_CPP_Expr>() );
              break;
            }
            case TAG_TYPE_CPP_FctCall: {
              if (expr.GetRecord(pos_CPP_FctCall_fct) == GenBoolType().get_tp())
              {
                TYPE_CPP_Expr arg (expr.GetSequence(pos_CPP_FctCall_arg).Hd());
                if (arg.Is(TAG_TYPE_CPP_BoolLit))
                  return arg;
                else
                  return vdm_BC_GenBracketedExpr(arg);
              }
              return vdm_BC_GenFctCallObjMemAcc(name, ASTAUX::MkId(L"GetValue"), SEQ<TYPE_CPP_Expr>() );
            }
          }
          return vdm_BC_GenFctCallObjMemAcc(name, ASTAUX::MkId(L"GetValue"), SEQ<TYPE_CPP_Expr>() );
        }
      }
      case TAG_TYPE_REP_NumericTypeRep: {
#ifdef VDMPP
        if (vdm_CPP_isJAVA()) {
          switch (type.GetIntValue(pos_REP_NumericTypeRep_qtp)) {
            case INTEGER:
            case NAT:
            case NATONE: {
              TYPE_CPP_Expr name_q (StripBracketedExpr(name)); 
              switch (name_q.GetTag()) {     
                case TAG_TYPE_CPP_ClassInstanceCreationExpr: {
                  const TYPE_CPP_Expr & e (name_q.GetSequence(pos_CPP_ClassInstanceCreationExpr_arg).Hd());
                  switch (e.GetTag()) {
                    case TAG_TYPE_CPP_FctCall:       { return e; }
                    case TAG_TYPE_CPP_IntegerLit:    { return e; }
                    case TAG_TYPE_CPP_QualifiedName: { return e; }
                    case TAG_TYPE_CPP_UnaryOpExpr: {
                      if ((e.GetRecord(pos_CPP_UnaryOpExpr_op).GetField(pos_CPP_UnaryOp_val) == Quote(L"MINUS")) &&
                          e.GetRecord(pos_CPP_UnaryOpExpr_expr).Is(TAG_TYPE_CPP_IntegerLit))
                      {
                        TYPE_CPP_Expr arg (e.GetRecord(pos_CPP_UnaryOpExpr_expr));
                        return TYPE_CPP_IntegerLit().Init(-(arg.GetIntValue(pos_CPP_IntegerLit_val)), NilContextId);
                      }
                      return vdm_BC_GenBracketedExpr(e);
                    }
                  }
                  return vdm_BC_GenBracketedExpr(e);
                  break;
                }
                case TAG_TYPE_CPP_FctCall: {
                  const TYPE_CPP_Expr & fct(name_q.GetRecord(pos_CPP_FctCall_fct));
                  if (fct.Is(TAG_TYPE_CPP_ObjectMemberAccess)) {
                    if ((fct.GetRecord(pos_CPP_ObjectMemberAccess_object) == GenImplIntType().get_tp()) &&
                        (fct.GetRecord(pos_CPP_ObjectMemberAccess_name) == vdm_BC_GenIdentifier(ASTAUX::MkId(L"valueOf"))))
                    {
                      TYPE_CPP_Expr e (name_q.GetSequence(pos_CPP_FctCall_arg).Hd());
                      return (e.Is(TAG_TYPE_CPP_FctCall) ? e : vdm_BC_GenBracketedExpr(e));
                    }
                  }
                  break;
                }
              }
              if (get_longs_option())
                return vdm_BC_GenFctCallObjMemAcc(name, ASTAUX::MkId(L"longValue"), SEQ<TYPE_CPP_Expr>() );
              else
                return vdm_BC_GenFctCallObjMemAcc(name, ASTAUX::MkId(L"intValue"), SEQ<TYPE_CPP_Expr>() );
            }
            case RAT:
            case REAL: {
              TYPE_CPP_Expr name_q (StripBracketedExpr(name)); 
              switch (name_q.GetTag()) {     
                case TAG_TYPE_CPP_ClassInstanceCreationExpr: {
                  const TYPE_CPP_Expr & cl (name_q.GetRecord(pos_CPP_ClassInstanceCreationExpr_classtype));
                  if (cl == GenImplRealType().get_tp()) {
                    const TYPE_CPP_Expr & e (name_q.GetSequence(pos_CPP_ClassInstanceCreationExpr_arg).Hd());
                    return (e.Is(TAG_TYPE_CPP_FctCall) ? e : vdm_BC_GenBracketedExpr(e));
                  }
                  else if (cl == GenImplIntType().get_tp()) {
                    TYPE_CPP_Expr e (name_q.GetSequence(pos_CPP_ClassInstanceCreationExpr_arg).Hd());
                    return vdm_BC_GenCastExpr(GenSmallRealType(),
                                          (e.Is(TAG_TYPE_CPP_FctCall) ? e : vdm_BC_GenBracketedExpr(e)));
                  }
                  break;
                }
                case TAG_TYPE_CPP_FctCall: {
                  const TYPE_CPP_Expr & fct(name_q.GetRecord(pos_CPP_FctCall_fct));
                  if (fct.Is(TAG_TYPE_CPP_ObjectMemberAccess)) {
                    if ((fct.GetRecord(pos_CPP_ObjectMemberAccess_object) == GenImplRealType().get_tp()) &&
                        (fct.GetRecord(pos_CPP_ObjectMemberAccess_name) == vdm_BC_GenIdentifier(ASTAUX::MkId(L"valueOf"))))
                    {
                      TYPE_CPP_Expr e (name_q.GetSequence(pos_CPP_FctCall_arg).Hd());
                      return (e.Is(TAG_TYPE_CPP_FctCall) ? e : vdm_BC_GenBracketedExpr(e));
                    }
                  }
                  break;
                }
              }
              return vdm_BC_GenFctCallObjMemAcc(name, ASTAUX::MkId(L"doubleValue"), SEQ<TYPE_CPP_Expr>() );
            }
            default: {
              return vdm_BC_GenFctCallObjMemAcc(name, ASTAUX::MkId(L"GetValue"), SEQ<TYPE_CPP_Expr>() );
            }
          }
        }
        else
#endif // VDMPP
        { // C++
          switch (type.GetIntValue(pos_REP_NumericTypeRep_qtp)) {
            case INTEGER:
            case NAT:
            case NATONE: {
              switch(name.GetTag()) {
                case TAG_TYPE_CPP_CastExpr: {
                  const Record & r (name.GetRecord(pos_CPP_CastExpr_typenm));
                  if (r.Is(TAG_TYPE_CPP_TypeName))
                  {
                    const SEQ<TYPE_CPP_TypeSpecifier> & tps (r.GetSequence(pos_CPP_TypeName_tsp));
                    //if (tps[1] == vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Int"))))
                    if (tps[1] == GenIntType())
                    {
                      const TYPE_CPP_Expr & expr (name.GetRecord(pos_CPP_CastExpr_expr));
                      if (expr.Is(TAG_TYPE_CPP_FctCall) &&
                          expr.GetRecord(pos_CPP_FctCall_fct).Is(TAG_TYPE_CPP_ObjectMemberAccess))
                      {
                        const TYPE_CPP_Expr & nm (expr.GetRecord(pos_CPP_FctCall_fct)
                                                      .GetRecord(pos_CPP_ObjectMemberAccess_name));
                        if ((nm == vdm_BC_GenIdentifier(ASTAUX::MkId(L"Length"))) ||
                            (nm == vdm_BC_GenIdentifier(ASTAUX::MkId(L"Card"))))
                        {
                          return name.GetRecord(pos_CPP_CastExpr_expr);
                        }
                      }
                    }
                  }
                  return vdm_BC_GenFctCallObjMemAcc(name, ASTAUX::MkId(L"GetValue"), SEQ<TYPE_CPP_Expr>() );
                }
                case TAG_TYPE_CPP_FctCall: {
                  if (name.GetRecord(pos_CPP_FctCall_fct) == GenImplIntType().get_tp())
                  {
                    TYPE_CPP_Expr arg (name.GetSequence(pos_CPP_FctCall_arg).Hd());
                    if (arg.Is(TAG_TYPE_CPP_IntegerLit))
                      return arg;
                    else
                      return vdm_BC_GenBracketedExpr(arg);
                  }
                  return vdm_BC_GenFctCallObjMemAcc(name, ASTAUX::MkId(L"GetValue"), SEQ<TYPE_CPP_Expr>() );
                }
                case TAG_TYPE_CPP_UnaryOpExpr: {
                  if ((name.GetRecord(pos_CPP_UnaryOpExpr_op).GetField(pos_CPP_UnaryOp_val) == Quote(L"MINUS")) &&
                      name.GetRecord(pos_CPP_UnaryOpExpr_expr).Is(TAG_TYPE_CPP_FctCall) &&
                      (name.GetRecord(pos_CPP_UnaryOpExpr_expr).GetRecord(pos_CPP_FctCall_fct) == GenImplIntType().get_tp()))
                  {
                    TYPE_CPP_Expr arg (name.GetRecord(pos_CPP_UnaryOpExpr_expr).GetSequence(pos_CPP_FctCall_arg).Hd());
                    if (arg.Is(TAG_TYPE_CPP_IntegerLit))
                    {
                      return TYPE_CPP_IntegerLit().Init(-(arg.GetIntValue(pos_CPP_IntegerLit_val)), NilContextId);
                    }
                  }
                }
              }
              break;
            }
            case REAL: {
              switch(name.GetTag()) {
                case TAG_TYPE_CPP_FctCall: {
                  if (name.GetRecord(pos_CPP_FctCall_fct) == GenImplIntType().get_tp())
                  {
                    TYPE_CPP_Expr arg (name.GetSequence(pos_CPP_FctCall_arg).Hd());
                    if (arg.Is(TAG_TYPE_CPP_IntegerLit))
                      return arg;
                    else
                      return vdm_BC_GenBracketedExpr(arg);
                  }
                  return vdm_BC_GenFctCallObjMemAcc(name, ASTAUX::MkId(L"GetValue"), SEQ<TYPE_CPP_Expr>() );
                }
              }
              break;
            }
            default: {
              break;
            }
          }
          return vdm_BC_GenFctCallObjMemAcc(name, ASTAUX::MkId(L"GetValue"), SEQ<TYPE_CPP_Expr>() );
        }
      }
      case TAG_TYPE_REP_TokenTypeRep: {
        return vdm_BC_GenFctCallObjMemAcc(name, ASTAUX::MkId(L"GetValue"), SEQ<TYPE_CPP_Expr>() );
      }
      case TAG_TYPE_REP_CharTypeRep: {
#ifdef VDMPP
        if (vdm_CPP_isJAVA()) {
          return vdm_BC_GenFctCallObjMemAcc(name, ASTAUX::MkId(L"charValue"), SEQ<TYPE_CPP_Expr>() );
        }
        else
#endif // VDMPP
        {
          return vdm_BC_GenFctCallObjMemAcc(name, ASTAUX::MkId(L"GetValue"), SEQ<TYPE_CPP_Expr>() );
        }
      }
      case TAG_TYPE_REP_QuoteTypeRep: {
        return vdm_BC_GenFctCallObjMemAcc(name, ASTAUX::MkId(L"GetValue"), SEQ<TYPE_CPP_Expr>());
      }
      case TAG_TYPE_REP_CompositeTypeRep:
      case TAG_TYPE_REP_ProductTypeRep:
      case TAG_TYPE_REP_SetTypeRep:
      case TAG_TYPE_REP_SeqTypeRep:
      case TAG_TYPE_REP_GeneralMapTypeRep:
      case TAG_TYPE_REP_InjectiveMapTypeRep: {
        return name;
      }
      case TAG_TYPE_REP_TypeNameRep:
        return GenGetValue (name, type);
      case TAG_TYPE_REP_InvTypeRep:
        return GenGetValue (name, type.GetRecord(pos_REP_InvTypeRep_shape));
      case TAG_TYPE_REP_UnionTypeRep: {
#ifdef VDMPP
        if (vdm_CPP_isJAVA()) {
          if (IsNumType(type))
            return vdm_BC_GenFctCallObjMemAcc(name, ASTAUX::MkId(L"doubleValue"), SEQ<TYPE_CPP_Expr>() );
          else
            return vdm_BC_GenFctCallObjMemAcc(name, ASTAUX::MkId(L"GetValue"), SEQ<TYPE_CPP_Expr>() ); // error
        }
        else
#endif // VDMPP
          return vdm_BC_GenFctCallObjMemAcc(name, ASTAUX::MkId(L"GetValue"), SEQ<TYPE_CPP_Expr>() );
     }
      case TAG_TYPE_REP_TypeVarRep: {
        return vdm_BC_GenFctCallObjMemAcc(name, ASTAUX::MkId(L"GetValue"), SEQ<TYPE_CPP_Expr>() );
      }
      default: {
        return vdm_BC_GenFctCallObjMemAcc(name, ASTAUX::MkId(L"GetValue"), SEQ<TYPE_CPP_Expr>() );
      }
    }
  }
}

// GenValue
// lit : AS`Literal
// vt : CGMAIN`VT
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenValue(const TYPE_AS_Literal & lit, const TYPE_CGMAIN_VT & vt)
{
  switch (lit.GetTag()) {
    case TAG_TYPE_AS_TextLit: {
#ifdef VDMPP
      if (vdm_CPP_isJAVA()) {
        TYPE_CPP_Expr expr (GenTextLit(lit));
        if (IsStringType(vt.GetField(pos_CGMAIN_VT_type))) {
          return expr;
        }
        else {
          return vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"UTIL.ConvertToList")), mk_sequence(expr));
        }
      }
      else
#endif // VDMPP
        return GenTextLit(lit);
    }

    case TAG_TYPE_AS_BoolLit:
      return GenBoolLit_DS(lit.GetBool(pos_AS_BoolLit_val));

    case TAG_TYPE_AS_RealLit: {
      const Real & val (lit.GetReal(pos_AS_RealLit_val));
      Generic tp;
      if (vdm_CPP_isJAVA()) {
        tp = vt.GetField(pos_CGMAIN_VT_type);
      }
      else
      {
        tp = GetCI().GetTypeInfo(lit.GetInt(pos_AS_RealLit_cid));
      }
      if (IsRealType(tp))
        if (vdm_CPP_isJAVA() && val.IsInt()) {
          return GenNumLit(val);
        }
        else 
          return GenRealLit(val);
      else if (val.IsInt()) {
        return GenNumLit(val);
      }
      else {
        return GenRealLit(val);
      }
    }
    case TAG_TYPE_AS_NumLit: {
      const Real & val (lit.GetReal(pos_AS_NumLit_val));
      Generic tp;
      if (vdm_CPP_isJAVA()) {
        tp = vt.GetField(pos_CGMAIN_VT_type);
      }
      else
      {
        tp =  GetCI().GetTypeInfo(lit.GetInt(pos_AS_RealLit_cid));
      }
      if (IsRealType(tp)) {
        return GenRealLit(val);
      }
      else {
        return GenNumLit(val);
      }
    }
    case TAG_TYPE_AS_CharLit: {
      return GenCharLit_DS(lit.GetChar(pos_AS_CharLit_val));
    }
    case TAG_TYPE_AS_QuoteLit: {
      return GenQuoteLit(lit.GetSequence(pos_AS_QuoteLit_val));
    }
    case TAG_TYPE_AS_NilLit: {
      return GenNilLit();
    }
    default: {
      ReportUndefined(L"GenValue");
    }
  }
  return Record(0,0); // dummy
}

// GenCastType
// type : REP`TypeRep
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenCastType(const TYPE_REP_TypeRep & type, const TYPE_CPP_Expr & e)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenCastExpr(GenType(type), e);
  }
  else
#endif // VDMPP
  {
    TYPE_CPP_Expr e_q (StripCastExpr(e)); 
    SEQ<TYPE_CPP_TypeSpecifier> ds_l;
    ds_l.ImpAppend(vdm_BC_GenTypeSpecifier(quote_CONST));
    ds_l.ImpAppend(GenType(type));
    ds_l.ImpAppend(vdm_BC_GenTypeSpecifier(quote_REFERENCE));

    return vdm_BC_GenCastExpr2(ds_l, e_q);
  }
}

// GenGenericExpr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenGenericExpr()
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenClassInstanceCreationExpr(vdm_BC_GenGeneric().get_tp(), SEQ<TYPE_CPP_Expr>());
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCall(vdm_BC_GenGeneric().get_tp(), SEQ<TYPE_CPP_Expr>());
}

// GenBoolExpr
// expr : [CPP`Expr]
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenBoolExpr(const Generic & expr)
{
  SEQ<TYPE_CPP_Expr> args;
  if (!expr.IsNil()) {
    args.ImpAppend(expr);
  }
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenClassInstanceCreationExpr(GenBoolType().get_tp(), args);
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCall(GenBoolType().get_tp(), args);
}

// GenIntExpr
// expr : [CPP`Expr]
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenIntExpr(const Generic & expr)
{
  SEQ<TYPE_CPP_Expr> args;
  if (!expr.IsNil()) {
    args.ImpAppend(expr);
  }
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenClassInstanceCreationExpr(GenImplIntType().get_tp(), args);
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCall(GenImplIntType().get_tp(), args);
}

// GenRealExpr
// expr : [CPP`Expr]
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenRealExpr(const Generic & expr)
{
  SEQ<TYPE_CPP_Expr> args;
  if (!expr.IsNil()) {
    args.ImpAppend(expr);
  }
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenClassInstanceCreationExpr(GenImplRealType().get_tp(), args);
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCall(GenImplRealType().get_tp(), args);
}

// GenCharExpr
// expr : [CPP`Expr]
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenCharExpr(const Generic & expr)
{
  SEQ<TYPE_CPP_Expr> args;
  if (!expr.IsNil()) {
    args.ImpAppend(expr);
  }
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenClassInstanceCreationExpr(GenCharType().get_tp(), args);
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCall(GenCharType().get_tp(), args);
}

// GenTokenExpr
// expr : [CPP`Expr]
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenTokenExpr(const Generic & expr)
{
  SEQ<TYPE_CPP_Expr> args;
  if (!expr.IsNil()) {
    args.ImpAppend(expr);
  }
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenClassInstanceCreationExpr(GenTokenType().get_tp(), args);
  }
  else
#endif// VDMPP
    return vdm_BC_GenFctCall(GenTokenType().get_tp(), args);
}

// GenProductExpr
// expr : [CPP`Expr]
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenProductExpr(const Generic & expr)
{
  SEQ<TYPE_CPP_Expr> args;
  if (!expr.IsNil()) {
    args.ImpAppend(expr);
  }
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenClassInstanceCreationExpr(GenProductType().get_tp(), args);
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCall(GenProductType().get_tp(), args);
}

// GenRecordExpr
// tag : [AS`Name]
// expr : [seq of CPP`Expr]
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenRecordExpr(const Generic & tag, const Generic & exprs)
{
  SEQ<TYPE_CPP_Expr> args;
  if (!exprs.IsNil()) {
    args.ImpConc(exprs);
  }
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenClassInstanceCreationExpr(GenRecordType(tag).get_tp(), args);
  }
  else
#endif // VDMPP
  {
    if (tag.IsNil()) {
      return GenEmptyRecordExpr(nil);
    } 
    TYPE_CPP_Expr rc (vdm_BC_GenFctCall(GenRecordType(tag).get_tp(), Sequence()));
    return vdm_BC_GenFctCallObjMemAcc(rc, ASTAUX::MkId(L"Init"), args);
  }
}

// GenQuoteExpr
// nm : [AS`Name]
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenQuoteExpr(const Generic & nm)
{
  SEQ<TYPE_CPP_Expr> args;
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenClassInstanceCreationExpr(GenQuoteType(nm).get_tp(), args);
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCall(GenQuoteType(nm).get_tp(), args);
}

// RememberPid_m
// pidm : map AS`Name to set of REP`TypeRep
// ==> ()
void vdmcg::RememberPid_m(const Map & pidm)
{
  this->pid_m_ds = pidm;
}

// DeletePid_m
// ==> ()
void vdmcg::DeletePid_m()
{
  this->pid_m_ds = Map();
}

// DeclaredAs
// e : CPP`Expr
// ==> [REP`TypeRep]
Generic vdmcg::DeclaredAs(const TYPE_CPP_Expr & e)
{
  SET<TYPE_AS_Name> dom_pid_m (this->pid_m_ds.Dom());
  Generic result = Nil();
  if (e.Is(TAG_TYPE_CPP_Identifier))
  {
    Generic tmp;
    for (bool bb = dom_pid_m.First(tmp); bb; bb = dom_pid_m.Next(tmp))
    {
      TYPE_CPP_Identifier tmpI (vdm_BC_Rename(tmp));
      if (tmpI.get_id() == TYPE_CPP_Identifier(e).get_id())
      {
        SET<TYPE_REP_TypeRep> tp_s (this->pid_m_ds[tmp]);
        if (tp_s.Card () == 1)
          result = Nil();
        else
          result = TYPE_REP_UnionTypeRep().Init(tp_s);
      }
    }
  }
  return result;
}

#ifdef VDMPP
// GetIntVal
// v : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GetIntVal(const TYPE_CPP_Expr &v)
{
  if (IsIntExpr(v))
    return v;

  TYPE_AS_Id util_wstring;
  if (get_longs_option())
    util_wstring = ASTAUX::MkId(L"UTIL.NumberToLong");
  else
    util_wstring = ASTAUX::MkId(L"UTIL.NumberToInt");
  return vdm_BC_GenFctCall(vdm_BC_GenIdentifier(util_wstring), mk_sequence(v));
}

// GetRealVal
// v : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GetRealVal(const TYPE_CPP_Expr &v)
{
  if (IsRealExpr(v))
    return v;
  return vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"UTIL.NumberToReal")), mk_sequence(v));
}

// GetStringVal
// v : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GetStringVal(const TYPE_CPP_Expr &v)
{
  if (IsStringExpr(v))
    return v;
  return vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"UTIL.ConvertToString")), mk_sequence(v));
}

// GetListVal
// v : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GetListVal(const TYPE_CPP_Expr &v)
{
  if (v.Is(TAG_TYPE_CPP_ClassInstanceCreationExpr))
    if (v.GetRecord(pos_CPP_ClassInstanceCreationExpr_classtype) == GenImplSeq0Type().get_tp())
      return v;
  return vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"UTIL.ConvertToList")), mk_sequence(v));
}

// GenExplicitCast
// restp : REP`TypeRep
// e : CPP`Expr
// tp : [REP`TypeRep]
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenExplicitCast(const TYPE_REP_TypeRep & restp, const TYPE_CPP_Expr & e, const Generic & tp)
{
  bool isnil (e.Is(TAG_TYPE_CPP_Identifier) ? (vdm_BC_GenIdentifier(ASTAUX::MkId(L"null")) == e) : false);

  if (isnil)
    return e;

  Generic type;
  if (tp.IsNil())
    type = tp;
  else
  {
    Generic ptp = DeclaredAs(e);
    if (ptp.IsNil())
    {
      switch (((Record) tp).GetTag()) {
        case TAG_TYPE_REP_OpTypeRep: {
          TYPE_REP_OpTypeRep otr (tp);
          TYPE_REP_TypeRep Rng (otr.get_Rng());
          type = CleanFlatType(Rng);
          break;
        }
// 20120216 -->
//#ifdef VDMPP
        case TAG_TYPE_REP_OverTypeRep: {
          SET<TYPE_REP_TypeRep> tp_s (Record(tp).GetSet(pos_REP_OverTypeRep_tps));
          SET<TYPE_REP_TypeRep> rtp_s;
          Generic t;
          for (bool bb = tp_s.First(t); bb; bb = tp_s.Next(t))
          {
            TYPE_REP_TypeRep ti (t);
            switch (ti.GetTag()) {
              case TAG_TYPE_REP_OpTypeRep: {
                rtp_s.Insert(ti.GetRecord(pos_REP_OpTypeRep_Rng));
                break;
              }
              case TAG_TYPE_REP_PartialFnTypeRep: {
                rtp_s.Insert(ti.GetRecord(pos_REP_PartialFnTypeRep_fnrng));
                break;
              }
              case TAG_TYPE_REP_TotalFnTypeRep: {
                rtp_s.Insert(ti.GetRecord(pos_REP_TotalFnTypeRep_fnrng));
                break;
              }
            }
          }
          if (rtp_s.Card() == 1)
            type = CleanFlatType(rtp_s.GetElem());
          else
            type = CleanFlatType(mk_REP_UnionTypeRep(rtp_s));
          break;
        }
//#endif // VDMPP
// <-- 20120216
        default: {
          type = CleanFlatType(tp);
          break;
        }
      }
    }
    else
      type = ptp;
  }

  TYPE_REP_TypeRep restype_q;
  switch (restp.GetTag()) {
    case TAG_TYPE_REP_OpTypeRep:{
      const TYPE_REP_TypeRep & Rng (restp.GetRecord(pos_REP_OpTypeRep_Rng));
      restype_q = CleanFlatType(Rng);
      break;
    }
    case TAG_TYPE_REP_TypeNameRep: {
      TYPE_REP_TypeRep cftp (CleanFlatType(restp));
      if (cftp.Is(TAG_TYPE_REP_UnionTypeRep) && IsCompositeType(cftp))
        restype_q = restp;
      else
        restype_q = cftp;
      break;
    }
    default: {
      restype_q = CleanFlatType(restp);
      break;
    }
  }
  TYPE_REP_TypeRep restype (RemoveNil(restype_q));

  if(type.IsNil())
  {
    if (IsRealType(restype))
    {
      return GetRealVal(e);
    }
    else if (IsIntType(restype))
    {
      SetException(true);
      return GetIntVal(e);
    }
    else if (IsStringType(restype))
    {
      return GetStringVal(e);
    }
    else if (IsSeqType(restype))
    {
      if (IsPossibleStringType(restype))
        return GetStringVal(e);
      else
        return GetListVal(e);
    }
    else if (IsProductType(restype))
      return vdm_BC_GenCastExpr(GenType(restype),e);
    else if (! restype.Is(TAG_TYPE_REP_UnionTypeRep))
      return vdm_BC_GenCastExpr(GenType(restype),e);
    else if (IsCompositeType(restype))
      return GenCastRecord(e, Nil());
    else
      return e;
  }
  else
  { // !type.IsNil()
    if (IsRealType(restype) && (!IsRealType(type)))
    {
      if (IsIntExpr(e))
        //return GenRealExpr(e.GetSequence(pos_CPP_ClassInstanceCreationExpr_arg));
        return GenRealExpr(GenGetValue(e, mk_REP_NumericTypeRep(Int(INTEGER))));
      else
        return GetRealVal(e);
    }
    else if (IsIntType(restype) && (!IsIntType(type)))
    {
      SetException(true);
      return GetIntVal(e);
    }
    else if (IsStringType(restype) && (!(IsStringType(type) || type.Is(TAG_TYPE_REP_EmptySetTypeRep))))
    {
      return GetStringVal(e);
    }
    else if ((IsSeqType(restype) && !IsStringType(restype)) && ((!IsSeqType(type)) || IsStringType(type)))
    {
      if (IsStringType(type))
      {
        return GetListVal(e);
      }
      else
      {
        // hack for Arrays.asList
        if (e.Is(TAG_TYPE_CPP_FctCall) &&
            e.GetRecord(pos_CPP_FctCall_fct).Is(TAG_TYPE_CPP_ObjectMemberAccess) &&
            (e.GetRecord(pos_CPP_FctCall_fct).GetRecord(pos_CPP_ObjectMemberAccess_object)
                            == vdm_BC_GenIdentifier(ASTAUX::MkId(L"Arrays"))) &&
            (e.GetRecord(pos_CPP_FctCall_fct).GetRecord(pos_CPP_ObjectMemberAccess_name)
                            == vdm_BC_GenIdentifier(ASTAUX::MkId(L"asList"))))
        {
          return GenSeqExpr(e);
        }
        return vdm_BC_GenCastExpr(GenType(restype), e);
      }
    }
    else if (! AreOfSameType(restype,type))
    {
      if (restype.Is(TAG_TYPE_REP_UnionTypeRep) || restype.Is(TAG_TYPE_REP_UnitTypeRep))
      {
         if (IsStringType(type) && !IsPossibleStringType(restype))
           return GetListVal(e);
         else if (IsPossibleStringType(restype) && (IsSeqType(type) && !IsPossibleStringType(type)))
           return GetStringVal(e);
         else
           return e;
      }
//#ifdef VDMPP
      else if (IsSubType(type, RemoveInvType(restype)))
        return e;
//#endif // VDMPP
      else
        if (GenType(type) == vdm_BC_GenGeneric())
          return vdm_BC_GenCastExpr(GenType(restype),e);
        return vdm_BC_GenCastExpr(GenType(restype),vdm_BC_GenBracketedExpr(vdm_BC_GenCastExpr(vdm_BC_GenGeneric(),e)));
    }
    else
    {
      return e;
    }
  }
}
#endif // VDMPP

// GenEmptyValue
// type : REP`TypeRep
// ==> [CPP`Expr]
Generic vdmcg::GenEmptyValue(const TYPE_REP_TypeRep & type)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    switch(type.GetTag()) {
      case TAG_TYPE_REP_BooleanTypeRep: {
        return GenBoolLit_DS(false);
      }
      case TAG_TYPE_REP_NumericTypeRep: {
        switch (type.GetIntValue(pos_REP_NumericTypeRep_qtp)) {
          case NATONE:  { return GenNumLit(1); }
          case NAT:
          case INTEGER: { return GenNumLit(0); }
          case RAT:
          case REAL:    { return GenRealLit(0); }
        }
      }
      case TAG_TYPE_REP_TokenTypeRep: {
        return GenTokenExpr(nil);
      }
      case TAG_TYPE_REP_CharTypeRep: {
        return vdm_BC_GenCharLit('?');
      }
      case TAG_TYPE_REP_QuoteTypeRep: {
        TYPE_AS_Name n (type.GetRecord(pos_REP_QuoteTypeRep_lit));
        return GenTokenExpr(n);
      }
      case TAG_TYPE_REP_UnionTypeRep: {
        if (IsCompositeType(type))
          return GenNilLit();
        else if (IsProductType(type))
          return GenProductExpr(nil);
        else if (IsObjRefType(type)) {
          const SET<TYPE_REP_TypeRep> & tps (type.GetSet(pos_REP_UnionTypeRep_tps));
          TYPE_REP_ObjRefTypeRep ortr (tps.GetElem());
          const TYPE_AS_Name & nm (ortr.GetRecord(pos_REP_ObjRefTypeRep_nm));
          return CallDefaultConstructor(nm);
        }
        else
          return GenGenericExpr();
      }
      case TAG_TYPE_REP_ProductTypeRep: {
        return GenProductExpr(nil);
      }
      case TAG_TYPE_REP_SetTypeRep:
      case TAG_TYPE_REP_EmptySetTypeRep: {
        return GenEmptySetExpr();
      }
      case TAG_TYPE_REP_SeqTypeRep:
      case TAG_TYPE_REP_EmptySeqTypeRep: {
       if (IsStringType(type))
         return GenEmptyStringExpr();
       else
         return GenEmptySeqExpr();
      }
      case TAG_TYPE_REP_GeneralMapTypeRep:
      case TAG_TYPE_REP_InjectiveMapTypeRep:
      case TAG_TYPE_REP_EmptyMapTypeRep: {
        return GenEmptyMapExpr();
      }
      case TAG_TYPE_REP_CompositeTypeRep: {
        return GenEmptyRecordExpr(type.GetRecord(pos_REP_CompositeTypeRep_nm));
       }
      case TAG_TYPE_REP_ObjRefTypeRep: {
        const TYPE_AS_Name & nm (type.GetRecord(pos_REP_ObjRefTypeRep_nm));
        if (isInterface(nm))
          return GenNilLit();
        else
          return CallDefaultConstructor(nm);
      }

      case TAG_TYPE_REP_AllTypeRep:
      case TAG_TYPE_REP_PartialFnTypeRep:
      case TAG_TYPE_REP_TotalFnTypeRep:
      case TAG_TYPE_REP_NilTypeRep: {
        return GenNilLit();
      }
      case TAG_TYPE_REP_UnitTypeRep: {
        return nil;
      }
      case TAG_TYPE_REP_TypeNameRep: {
        return GenNilLit();
      }
      case TAG_TYPE_REP_InvTypeRep: {
        return GenEmptyValue(type.GetRecord(pos_REP_InvTypeRep_shape));
      }
      default: {
        return GenGenericExpr();
      }
    }
    //return Record (0,0); // To avoid warnings.
  }
  else
#endif //VDMPP
  { // C++
    switch(type.GetTag())
    {
      case TAG_TYPE_REP_BooleanTypeRep: {
        return GenBoolExpr(nil);
      }
      case TAG_TYPE_REP_NumericTypeRep: {
        switch (type.GetIntValue(pos_REP_NumericTypeRep_qtp))
        {
          case NATONE:
          case NAT:
          case INTEGER: { return GenIntExpr(nil); }
          case RAT:
          case REAL:    { return GenRealExpr(nil); }
        }
      }
      case TAG_TYPE_REP_TokenTypeRep: {
        return GenTokenExpr(nil);
      }
      case TAG_TYPE_REP_CharTypeRep: {
        return GenCharExpr(nil);
      }
      case TAG_TYPE_REP_QuoteTypeRep: {
        return GenQuoteExpr(nil);
      }
      case TAG_TYPE_REP_UnionTypeRep: {
        if (IsCompositeType(type))
          return GenEmptyRecordExpr(nil);
        else if (IsProductType(type))
          return GenProductExpr(nil);
#ifdef VDMPP
        else if (IsObjRefType(type)) {
          const SET<TYPE_REP_TypeRep> & tps (type.GetSet(pos_REP_UnionTypeRep_tps));
          TYPE_REP_ObjRefTypeRep ortr (tps.GetElem());
          const TYPE_AS_Name & nm (ortr.GetRecord(pos_REP_ObjRefTypeRep_nm));
          return CallDefaultConstructor(nm);
        }
#endif //VDMPP
        else
          return GenGenericExpr();
      }
      case TAG_TYPE_REP_ProductTypeRep: {
        return GenProductExpr(nil);
      }
      case TAG_TYPE_REP_SetTypeRep:
      case TAG_TYPE_REP_EmptySetTypeRep: {
        return GenEmptySetExpr();
      }
      case TAG_TYPE_REP_SeqTypeRep:
      case TAG_TYPE_REP_EmptySeqTypeRep: {
        return GenEmptySeqExpr();
      }
      case TAG_TYPE_REP_GeneralMapTypeRep:
      case TAG_TYPE_REP_InjectiveMapTypeRep:
      case TAG_TYPE_REP_EmptyMapTypeRep: {
        return GenEmptyMapExpr();
      }
      case TAG_TYPE_REP_CompositeTypeRep: {
        return GenEmptyRecordExpr(nil);
      }
#ifdef VDMPP
      case TAG_TYPE_REP_ObjRefTypeRep: {
        return CallDefaultConstructor(type.GetRecord(pos_REP_ObjRefTypeRep_nm));
      }
#endif // VDMPP

      case TAG_TYPE_REP_AllTypeRep:
      case TAG_TYPE_REP_PartialFnTypeRep:
      case TAG_TYPE_REP_TotalFnTypeRep:
      case TAG_TYPE_REP_NilTypeRep: {
        return GenGenericExpr();
      }
      case TAG_TYPE_REP_UnitTypeRep: {
        return nil;
      }
      case TAG_TYPE_REP_InvTypeRep: {
        return GenEmptyValue(type.GetRecord(pos_REP_InvTypeRep_shape));
      }
      default: {
        return GenGenericExpr();
      }
    }
    //return Record (0,0); // To avoid warnings.
  }
}

// GenTypeDef_DS (GenTypeDef)
// nm : AS`Name
// td_l : seq of AS`TypeDef
#ifdef VDMSL
// ==> CPP`CPPAS
#endif // VDMSL
#ifdef VDMPP
// ==> seq of CPP`MemberDeclaration
#endif // VDMPP
Sequence vdmcg::GenTypeDef_DS(const TYPE_AS_Name & /*nm*/, const SEQ<TYPE_AS_TypeDef> & td_l)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    size_t len_td_l = td_l.Length();
    for (size_t idx = 1; idx <= len_td_l;idx++)
    {
      const TYPE_AS_TypeDef & td (td_l[idx]);
      const TYPE_AS_Name & nm   (td.GetRecord(pos_AS_TypeDef_nm));
      const TYPE_AS_Type & type (td.GetRecord(pos_AS_TypeDef_shape));
      if (type.Is(TAG_TYPE_AS_UnionType))
      {
        TYPE_REP_TypeRep rtp (FromAS2RepType(type));
        if (IsCompositeType(rtp))
        {
          const SEQ<TYPE_AS_CompositeType> & typeseq (type.GetSequence(pos_AS_UnionType_tps));
          size_t len_typeseq = typeseq.Length();
          for (size_t i = 1; i <= len_typeseq; i++)
          {
            const TYPE_AS_CompositeType & t (typeseq[i]);
            const TYPE_AS_Name & enm (t.GetRecord(pos_AS_CompositeType_name));

            for (size_t j = 1; j <= len_td_l; j++)
            {
              const TYPE_AS_TypeDef & td_i (td_l[j]);
              const TYPE_AS_Name & nm_i   (td_i.GetRecord(pos_AS_TypeDef_nm));
              const TYPE_AS_Type & type_i (td_i.GetRecord(pos_AS_TypeDef_shape));
              if (enm == nm_i)
              {
                if (type_i.Is(TAG_TYPE_AS_UnionType))
                  AddUnionType(enm, nm);
                else
                  AddRecordType(enm, nm);
              }
            }
          }
        }
      }
    }

    Map cidtocpp;
    for (size_t i = 1; i <= len_td_l; i++)
    {
      const TYPE_AS_TypeDef & td (td_l[i]);
      const TYPE_AS_Name & nm       (td.GetRecord(pos_AS_TypeDef_nm));
      const TYPE_AS_Type & type     (td.GetRecord(pos_AS_TypeDef_shape));
      const TYPE_AS_Access & acc    (td.GetField(pos_AS_TypeDef_access));
      const TYPE_CI_ContextId & cid (td.GetInt(pos_AS_TypeDef_cid));
      cidtocpp.Insert(cid, GenCppTypeDef(nm, type, acc));
    }

    Sequence s;
    while (!cidtocpp.IsEmpty())
    {
      SET<TYPE_CI_ContextId> dom_cidtocpp (cidtocpp.Dom());
      TYPE_CI_ContextId cid (dom_cidtocpp.GetElem());
      Generic tempcid;
      for (bool ff = dom_cidtocpp.First (tempcid); ff; ff = dom_cidtocpp.Next (tempcid))
      {
        Tuple gfp1 (GetCI().GetFilePos(cid));
        const TYPE_CI_TokenPos & tst (gfp1.GetRecord(3));
        int ast_start = tst.GetIntValue(pos_CI_TokenPos_abs_uline);

        Tuple gfp2 (GetCI().GetFilePos(tempcid));
        const TYPE_CI_TokenPos & tst2 (gfp2.GetRecord(3));
        int ast_startf = tst2.GetIntValue(pos_CI_TokenPos_abs_uline);

        if (ast_start > ast_startf)
          cid = tempcid;
      }
      s.ImpConc(cidtocpp[cid]);
      cidtocpp.RemElem(cid);
    }
    return s;
  }
  else
#endif // VDMPP
  { // C++
    Sequence s;
    type_dL enum_l;
    size_t len_td_l = td_l.Length();
    for (size_t idx = 1; idx <= len_td_l; idx++)
    {
      const TYPE_AS_TypeDef & td (td_l[idx]);
      const TYPE_AS_Name & nm   (td.GetRecord(pos_AS_TypeDef_nm));
      const TYPE_AS_Type & type (td.GetRecord(pos_AS_TypeDef_shape));

      if (type.Is(TAG_TYPE_AS_CompositeType))
      {
        const TYPE_AS_Name & name (type.GetRecord(pos_AS_CompositeType_name));
        const SEQ<TYPE_AS_Field> & field_l (type.GetSequence(pos_AS_CompositeType_fields));

        TYPE_CPP_Identifier lid (vdm_BC_GenLengthName(name));
        TYPE_CPP_Identifier id (vdm_BC_Rename(name));
        //      Record tag(EnumTag(name));
        TYPE_CPP_Identifier tag (GenCompositeTypeTagId(name));
        TYPE_CPP_Identifier num (vdm_BC_GenIdentifier(vdm_CGAUX_StringNumber(field_l.Length())));

        enum_l.ImpAppend(vdm_BC_GenEnum(id, tag));
        enum_l.ImpAppend(vdm_BC_GenEnum(lid, num));

        size_t len_field_l = field_l.Length();
        for (size_t pos = 1; pos <= len_field_l; pos++)
        {
          const TYPE_AS_Field & f (field_l[pos]);
          const Generic & sel (f.GetField(pos_AS_Field_sel));
          if (!sel.IsNil())
          {
            TYPE_CPP_Identifier pid (vdm_BC_GenPosName(name, sel));
            TYPE_CPP_Identifier fid (vdm_BC_GenIdentifier(vdm_CGAUX_StringNumber(pos)));
            enum_l.ImpAppend(vdm_BC_GenEnum(pid, fid));
          }
        }
      }
      s.ImpConc(GenCppTypeDef(nm, type, nil));
    }
    if (!enum_l.IsEmpty()) {
      type_dL enumspec_l;
      enumspec_l.ImpAppend(vdm_BC_GenTypeSpecifier(vdm_BC_GenEnumSpec(nil, enum_l)));
      Sequence res;
#ifdef VDMSL
      res.ImpAppend(vdm_BC_GenIdentDeclaration(SEQ<TYPE_CPP_Annotation>(), enumspec_l, nil));
#endif //VDMSL
#ifdef VDMPP
      res.ImpAppend(vdm_BC_GenMemberSpec(enumspec_l, nil));
#endif //VDMPP
      res.ImpConc(s);
      return res;
    }
    else
      return s;
  }
}

// GenBoolDecl
// name : CPP`Name
// initExpr : [CPP`Initializer]
// ==> seq1 of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenBoolDecl(const TYPE_CPP_Name & name, const Generic & initExpr)
{
  SEQ<TYPE_CPP_Stmt> rb_l;
#ifdef VDMPP
  if (vdm_CPP_isJAVA() && initExpr.IsNil())
    rb_l.ImpAppend(vdm_BC_GenDecl(GenBoolType(), name, vdm_BC_GenAsgnInit(GenNullExpr())));
  else
#endif //VDMPP
    rb_l.ImpAppend(vdm_BC_GenDecl(GenBoolType(), name, initExpr));
  return rb_l;
}

// GenSmallBoolType
// ==> CPP`DeclSpecifier
TYPE_CPP_TypeSpecifier vdmcg::GenSmallBoolType()
{
  return vdm_BC_GenTypeSpecifier( vdm_BC_GenBool() );
}

// GenBoolType
// ==> CPP`DeclSpecifier
TYPE_CPP_TypeSpecifier vdmcg::GenBoolType()
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Boolean")));
  }
  else
#endif //VDMPP
    return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Bool")));
}

// GenBoolLit_DS (GenBoolLit)
// val : bool
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenBoolLit_DS(const Bool & val)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    if (get_smalltypes_option()) {
      return vdm_BC_GenBoolLit(val.GetValue());
    }
    else {
      return GenBoolExpr(vdm_BC_GenBoolLit(val));
    }
  }
  else
#endif //VDMPP
    return GenBoolExpr(vdm_BC_GenBoolLit(val));
}

// GenIsBool
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenIsBool(const TYPE_CPP_Expr & e)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenBracketedExpr(vdm_BC_GenTypeComp(GenBoolType(), e));
  }
  else
#endif //VDMPP
  {
    TYPE_CPP_Expr expr (StripBracketedExpr(e));
    if (expr.Is(TAG_TYPE_CPP_CastExpr) &&
        expr.GetRecord(pos_CPP_CastExpr_typenm).Is(TAG_TYPE_CPP_TypeName) &&
        (expr.GetRecord(pos_CPP_CastExpr_typenm).GetSequence(pos_CPP_TypeName_tsp).Hd() != GenBoolType()))
    {
      expr = expr.GetRecord(pos_CPP_CastExpr_expr);
      return vdm_BC_GenFctCallObjMemAcc(expr, ASTAUX::MkId(L"IsBool"), SEQ<TYPE_CPP_Expr>());
    }
    return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"IsBool"), SEQ<TYPE_CPP_Expr>());
  }
}

// GenNatOneDecl
// name : CPP`Name
// initExpr : [CPP`Initializer]
// ==> seq1 of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenNatOneDecl(const TYPE_CPP_Name & name, const Generic & initExpr)
{
  SEQ<TYPE_CPP_Stmt> rb_l;
  TYPE_CPP_TypeSpecifier tp (GenIntType());
#ifdef VDMPP
  if (vdm_CPP_isJAVA() && initExpr.IsNil())
    rb_l.ImpAppend(vdm_BC_GenDecl(tp, name, vdm_BC_GenAsgnInit(GenNullExpr())));
  else
#endif // VDMPP
    rb_l.ImpAppend(vdm_BC_GenDecl(tp, name, initExpr));
  return rb_l;
}

// GenNatType
// ==> CPP`DeclSpecifier
TYPE_CPP_TypeSpecifier vdmcg::GenNatType()
{
  return GenIntType();
}

// GenImplNatType
// ==> CPP`DeclSpecifier
TYPE_CPP_TypeSpecifier vdmcg::GenImplNatType()
{
  return GenImplIntType();
}

// GenIsNat_DS (GenIsNat)
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenIsNat_DS( const TYPE_CPP_Expr & e )
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"UTIL.IsNat")), mk_sequence(e));
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"CGUTIL::is_nat")), mk_sequence(e));
}

// GenIsNatOne (GenIsNat1)
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenIsNatOne( const TYPE_CPP_Expr & e )
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"UTIL.IsNatOne")), mk_sequence(e));
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"CGUTIL::is_nat1")), mk_sequence(e));
}

// GenNumType
// ==> CPP`DeclSpecifier
TYPE_CPP_TypeSpecifier vdmcg::GenNumType()
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Number")));
  }
  else
#endif // VDMPP
    return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Real")));
}

// GenIntDecl
// name : CPP`Name
// initExpr : [CPP`Initializer]
// ==> seq1 of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenIntDecl(const TYPE_CPP_Name & name, const Generic & initExpr)
{
  SEQ<TYPE_CPP_Stmt> rb_l;
#ifdef VDMPP
  if (vdm_CPP_isJAVA() && initExpr.IsNil())
    rb_l.ImpAppend(vdm_BC_GenDecl(GenIntType(), name, vdm_BC_GenAsgnInit(GenNullExpr())));
  else
#endif // VDMPP
    rb_l.ImpAppend(vdm_BC_GenDecl(GenIntType(), name, initExpr));
  return rb_l;
}

// GenSmallIntType
// ==> CPP`DeclSpecifier
TYPE_CPP_TypeSpecifier vdmcg::GenSmallIntType()
{
  return vdm_BC_GenTypeSpecifier( vdm_BC_GenInt() );
}

// GenSmallNumType
// ==> CPP`DeclSpecifier
TYPE_CPP_TypeSpecifier vdmcg::GenSmallNumType()
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    if (get_longs_option()) {
      return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"long")));
    }
    else {
      return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"int")));
    }
  }
  else
#endif // VDMPP
    return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"int")));
}

// GenIntType
// ==> CPP`DeclSpecifier
TYPE_CPP_TypeSpecifier vdmcg::GenIntType()
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Number")));
  }
  else
#endif // VDMPP
    return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Int")));
}

// GenImplIntType
// ==> CPP`DeclSpecifier
TYPE_CPP_TypeSpecifier vdmcg::GenImplIntType()
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    if (get_longs_option()) {
      return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Long")));
    }
    else {
      return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Integer")));
    }
  }
  else
#endif // VDMPP
    return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Int")));
}

// GenIsInt
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenIsInt(const TYPE_CPP_Expr & e)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"UTIL.IsInteger")), mk_sequence(e));
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"CGUTIL::is_int")), mk_sequence(e));
}

// GenNumLit
// val : int
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenNumLit(const Int & val)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    if (get_smalltypes_option()) {
      return vdm_BC_GenIntegerLit(val);
    }
    else {
      return GenIntExpr(vdm_BC_GenIntegerLit(val));
    }
  }
  else
#endif // VDMPP
    return GenIntExpr(vdm_BC_GenIntegerLit(val));
}

// GenRealDecl
// name : CPP`Name
// initExpr : [CPP`Initializer]
// ==> seq1 of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenRealDecl(const TYPE_CPP_Name & name, const Generic & initExpr)
{
  SEQ<TYPE_CPP_Stmt> rb_l;
#ifdef VDMPP
  if (vdm_CPP_isJAVA() && initExpr.IsNil()) {
    rb_l.ImpAppend(vdm_BC_GenDecl(GenRealType(), name, vdm_BC_GenAsgnInit(GenNullExpr())));
  }
  else
#endif // VDMPP
    rb_l.ImpAppend(vdm_BC_GenDecl(GenRealType(), name, initExpr));
  return rb_l;
}

// GenRatType
// ==> CPP`DeclSpecifier
TYPE_CPP_TypeSpecifier vdmcg::GenRatType()
{
  return GenRealType();
}

// GenImplRatType
// ==> CPP`DeclSpecifier
TYPE_CPP_TypeSpecifier vdmcg::GenImplRatType()
{
  return GenImplRealType();
}

// GenRealLit
// val : real
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenRealLit(const Real & val)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    if (get_smalltypes_option()) {
      return vdm_BC_GenFloatingLit(val);
    }
    else {
      return GenRealExpr(vdm_BC_GenFloatingLit(val));
    }
  }
  else
#endif // VDMPP
    return GenRealExpr(vdm_BC_GenFloatingLit(val));
}

// GenSmallRealType
// ==> CPP`DeclSpecifier
TYPE_CPP_TypeSpecifier vdmcg::GenSmallRealType()
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"double")));
  }
  else
#endif // VDMPP
    return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"double")));
}

// GenRealType
// ==> CPP`DeclSpecifier
TYPE_CPP_TypeSpecifier vdmcg::GenRealType()
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Number")));
  }
  else
#endif // VDMPP
    return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Real")));
}

// GenImplRealType
// ==> CPP`DeclSpecifier
TYPE_CPP_TypeSpecifier vdmcg::GenImplRealType()
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Double")));
  }
  else
#endif // VDMPP
    return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Real")));
}

// GenIsReal
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenIsReal(const TYPE_CPP_Expr & e)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"UTIL.IsReal")), SEQ<TYPE_CPP_Expr>().ImpAppend(e));
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"CGUTIL::is_real")), mk_sequence(e));
}

// GenCharDecl
// name : CPP`Name
// initExpr : [CPP`Initializer]
// ==> seq1 of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenCharDecl(const TYPE_CPP_Name & name, const Generic & initExpr)
{
  SEQ<TYPE_CPP_Stmt> rb_l;
#ifdef VDMPP
  if (vdm_CPP_isJAVA() && initExpr.IsNil())
    rb_l.ImpAppend(vdm_BC_GenDecl(GenCharType(), name, vdm_BC_GenAsgnInit(GenNullExpr())));
  else
#endif // VDMPP
    rb_l.ImpAppend(vdm_BC_GenDecl(GenCharType(), name, initExpr));
  return rb_l;
}

// GenCharType
// ==> CPP`DeclSpecifier
TYPE_CPP_TypeSpecifier vdmcg::GenCharType()
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    if (get_smalltypes_option()) {
      return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"char")));
    }
    else {
      return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Character")));
    }
  }
  else
#endif // VDMPP
    return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Char")));
}

// GenCharLit_DS (GenCharLit)
// val : char
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenCharLit_DS(const Char & val)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    if (get_smalltypes_option()) {
      return vdm_BC_GenCharLit(val);
    }
    else {
      return GenCharExpr(vdm_BC_GenCharLit(val));
    }
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCall(GenCharType().get_tp(), mk_sequence(vdm_BC_GenCharLit(val)));
}

// GenIsChar
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenIsChar(const TYPE_CPP_Expr & e)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenBracketedExpr(vdm_BC_GenTypeComp(GenCharType(), e));
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"IsChar"), SEQ<TYPE_CPP_Expr>());
}

// GenTokenType
// ==> CPP`DeclSpecifier
TYPE_CPP_TypeSpecifier vdmcg::GenTokenType()
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    InsertImport(SEQ<Char>(L"jp.vdmtools.VDM.Token"));
  }
#endif // VDMPP
  
  return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Token")));
}

// GenIsToken
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenIsToken(const TYPE_CPP_Expr & e)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA())
    return vdm_BC_GenBracketedExpr(vdm_BC_GenTypeComp(GenTokenType(), e));
  else
#endif // VDMPP
    return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"IsToken"), SEQ<TYPE_CPP_Expr>());
}

// GenTokenDecl
// name : CPP`Name
// initExpr : [CPP`Initializer]
// ==> seq1 of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenTokenDecl(const TYPE_CPP_Name & nm, const Generic & iexpr)
{
  SEQ<TYPE_CPP_Stmt> rb_l;
#ifdef VDMPP
  if (vdm_CPP_isJAVA() && iexpr.IsNil())
    rb_l.ImpAppend(vdm_BC_GenDecl(GenTokenType(), nm, vdm_BC_GenAsgnInit(GenNullExpr())));
  else
#endif // VDMPP
    rb_l.ImpAppend(vdm_BC_GenDecl(GenTokenType(), nm, iexpr));
  return rb_l;
}

// GenSetTokValExpr
// tok : CPP`Expr
// elem : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenSetTokValExpr(const TYPE_CPP_Expr & tok, const TYPE_CPP_Expr & elem )
{
  return vdm_BC_GenFctCallObjMemAcc(tok, ASTAUX::MkId(L"SetValue"), mk_sequence(elem));
}

// GenQuoteType
// nm : [AS`Name]
// ==> CPP`DeclSpecifier
TYPE_CPP_TypeSpecifier vdmcg::GenQuoteType(const Generic & nm)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    TYPE_AS_Name l_asnm (nm);
    TYPE_AS_Id className (ASTAUX::MkId(L"quotes."));
    className.ImpConc(GiveLastName(l_asnm));

    return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(className));
  }
  else
#endif // VDMPP
    return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Quote")));
}

// GenQuoteLit
// tk : seq of char
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenQuoteLit(const TYPE_AS_Id & ch_l)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    InsertQuote(ch_l);
    TYPE_AS_Id tkSeq (ASTAUX::MkId(L"quotes."));
    tkSeq.ImpConc(ch_l);
    return vdm_BC_GenClassInstanceCreationExpr(vdm_BC_GenIdentifier(tkSeq), SEQ<TYPE_CPP_Expr>());
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCall(GenQuoteType(nil).get_tp(), mk_sequence(vdm_BC_GenStringLit(ch_l)));
}

// GenIsQuote
// e : CPP`Expr
// qtr : REP`QuoteTypeRep
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenIsQuote(const TYPE_CPP_Expr & e, const TYPE_REP_QuoteTypeRep & qtr)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    const TYPE_AS_Name & nm (qtr.GetRecord(pos_REP_QuoteTypeRep_lit));
    TYPE_AS_Id qName (ASTAUX::MkId(L"quotes."));
    qName.ImpConc(GiveLastName(nm));

    TYPE_CPP_TypeSpecifier tp (vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(qName)));
    return vdm_BC_GenBracketedExpr(vdm_BC_GenTypeComp(tp, e));
  }
  else
#endif // VDMPP
  {
    const TYPE_AS_Name & nm (qtr.GetRecord(pos_REP_QuoteTypeRep_lit));
    const TYPE_AS_Ids & ids (nm.GetSequence(pos_AS_Name_ids));

    TYPE_CPP_Expr expr (StripBracketedAndCastExpr(e));
    return vdm_BC_GenBracketedExpr(vdm_BC_GenEq(expr, GenQuoteLit(ids[1])));
  }
}

// GenTextLit
// tl : AS`TextLit
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenTextLit(const TYPE_AS_TextLit & tl)
{
  const TYPE_AS_Id & ch_l (tl.GetSequence(pos_AS_TextLit_val));
  return GenStringExpr(vdm_BC_GenStringLit(ch_l));
}


// GenRecordType
// nm : [AS`Name]
// ==> CPP`DeclSpecifier
TYPE_CPP_TypeSpecifier vdmcg::GenRecordType(const Generic & nm)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    InsertImport(SEQ<Char>(L"jp.vdmtools.VDM.Record"));
    if (nm.IsNil())
      return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Record")));
    else
    {
      TYPE_AS_Ids ids (Record(nm).GetSequence(pos_AS_Name_ids));
      if ( ids.Length() == 2 )
      {
        if (ids.Hd() == GiveCurCName())
          return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier((GiveLastName(nm))));
        else
        {
          Generic qnm (GenQualifiedTypeName(GenQName(nm)));
          if (qnm.IsNil())
            return vdm_BC_GenTypeSpecifier(vdm_BC_Rename (nm));
          else
          {
            if (GiveFirstName(qnm) == GiveCurCASName())
              return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier((GiveLastName(nm))));
            else
              return vdm_BC_GenTypeSpecifier(vdm_BC_Rename(qnm));
          }
        }
      }
      else
      {
        Generic qnm (GenQualifiedTypeName(GenQName(nm)));
        if (qnm.IsNil())
          return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier((GiveLastName(nm))));
        else
        {
          if (GiveFirstName(qnm) == GiveCurCASName())
            return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier((GiveLastName(nm))));
          else
            return vdm_BC_GenTypeSpecifier(vdm_BC_Rename(qnm));
        }
      }
    }
  }
  else
#endif // VDMPP
  {
    if (nm.IsNil()) {
      return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Record")));
    }
    else {
      return vdm_BC_GenTypeSpecifier(Name2CppTpId(nm));
    }
  }
}

// GenRecordDecl
// tagnm : [AS`Name]
// name : CPP`Name
// initExpr : [CPP`Initializer]
// ==> seq1 of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenRecordDecl(const Generic & tagnm,
                                        const TYPE_CPP_Name & name,
                                        const Generic & initExpr)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    SEQ<TYPE_CPP_Stmt> rb_l;
    if (initExpr.IsNil())
      rb_l.ImpAppend(vdm_BC_GenDecl(GenRecordType(tagnm), name, vdm_BC_GenAsgnInit(GenNullExpr())));
    else
      rb_l.ImpAppend(vdm_BC_GenDecl(GenRecordType(tagnm), name, initExpr));
    return rb_l;
  }
  else
#endif // VDMPP
  {
    SEQ<TYPE_CPP_Stmt> rb_l;
    rb_l.ImpAppend(vdm_BC_GenDecl(GenRecordType(tagnm), name, initExpr));
    return rb_l;
  }
}

// GenCastRecord
// e : CPP`Expr
// nm : [AS`Name]
// ==> CPP`CastExpr
TYPE_CPP_Expr vdmcg::GenCastRecord(const TYPE_CPP_Expr & e, const Generic & nm)
{
  return vdm_BC_GenCastExpr(GenRecordType(nm), e);
}

// GenCastRecordTyepForModify
// e : CPP`Expr
// nm : [AS`Name]
// ==> CPP`CastExpr
TYPE_CPP_Expr vdmcg::GenCastRecordTypeForModify(const TYPE_CPP_Expr & e, const Generic & nm)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenCastExpr(GenRecordType(nm), e);
  }
  else
#endif // VDMPP
  {
    SEQ<TYPE_CPP_TypeSpecifier> cmds_l;
    cmds_l.ImpAppend(vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Common"))));
    cmds_l.ImpAppend(vdm_BC_GenTypeSpecifier(quote_REFERENCE));
    TYPE_CPP_Expr cmcast (vdm_BC_GenCastExpr2(cmds_l, e));
    SEQ<TYPE_CPP_TypeSpecifier> ds_l;
    ds_l.ImpAppend(GenRecordType(nm));
    ds_l.ImpAppend(vdm_BC_GenTypeSpecifier(quote_REFERENCE));
    return vdm_BC_GenCastExpr2(ds_l, cmcast);
  }
}

// GenEqTag
// tagnm : AS`Name
// varExpr_v : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenEqTag(const TYPE_AS_Name & tagnm, const TYPE_CPP_Expr & varExpr_v)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    TYPE_CPP_TypeSpecifier tp (vdm_BC_GenTypeSpecifier(vdm_BC_Rename(tagnm)));
    return vdm_BC_GenBracketedExpr(vdm_BC_GenTypeComp(tp, varExpr_v));
  }
  else
#endif // VDMPP
    return vdm_BC_GenEq(vdm_BC_Rename(tagnm), GenGetTag(varExpr_v));
}

// GenRecGetField
// e : CPP`Expr
// i : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenRecGetField(const TYPE_CPP_Expr & e, const TYPE_CPP_Expr & i,
                                    const TYPE_REP_TypeRep & tp)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenObjectMemberAccess(e, i);
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCallObjMemAcc(e, GenGetMethod(tp), mk_sequence(i));
}

// GenRecGetFieldNm
// e : CPP`Expr
// ctr : REP`CompositeTypeRep
// fsname : AS`Name
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenRecGetFieldNm(const TYPE_CPP_Expr & e,
                                      const TYPE_REP_CompositeTypeRep & ctr,
                                      const TYPE_AS_Name & fsname)
{
  const TYPE_AS_Name & tagname (ctr.GetRecord(pos_REP_CompositeTypeRep_nm));
  TYPE_CPP_Name pid (vdm_BC_GenPosName(tagname, fsname));

#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenObjectMemberAccess(e, pid);
  }
  else
#endif // VDMPP
  {
    const SEQ<TYPE_REP_FieldRep> & fields (ctr.GetSequence(pos_REP_CompositeTypeRep_fields));
    size_t len_fields = fields.Length();
    for (size_t i = 1; i <= len_fields; i++) {
      const Generic & sel (fields[i].GetField(pos_REP_FieldRep_sel));
      if ( fsname == sel ) {
        TYPE_REP_TypeRep tp (fields[i].GetRecord(pos_REP_FieldRep_tp));
        return vdm_BC_GenFctCallObjMemAcc(e, GenGetMethod(tp), mk_sequence(pid));
      }
    }
    return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"GetField"), mk_sequence(pid));
  }
}

// GenRecGetFieldNo
// ct : REP`CompositeTypeRep
// fsnm : AS`Name
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenRecGetFieldNo(const TYPE_REP_CompositeTypeRep & ct, const TYPE_AS_Name & fsnm)
{
  const TYPE_AS_Name & tagnm (ct.GetRecord(pos_REP_CompositeTypeRep_nm));
  return vdm_BC_GenPosName(tagnm, fsnm);
}

// GenGetMethod
// tp : REP`TypeRep
// ==> AS`Id
TYPE_AS_Id vdmcg::GenGetMethod(const TYPE_REP_TypeRep & tp_q)
{
  TYPE_REP_TypeRep tp (CleanFlatType(tp_q));
  if (IsBoolType(tp))
    return ASTAUX::MkId(L"GetBool");
  else if (IsNumType(tp)) {
    if (IsIntType(tp))
      return ASTAUX::MkId(L"GetInt");
    else
      return ASTAUX::MkId(L"GetReal");
  }
  else if (IsCharType(tp))
    return ASTAUX::MkId(L"GetChar");
  else if (IsSetType(tp))
    return ASTAUX::MkId(L"GetSet");
  else if (IsSeqType(tp))
    return ASTAUX::MkId(L"GetSequence");
  else if (IsMapType(tp))
    return ASTAUX::MkId(L"GetMap");
  else if (IsCompositeType(tp))
    return ASTAUX::MkId(L"GetRecord");
  else if (IsProductType(tp))
    return ASTAUX::MkId(L"GetTuple");

  return ASTAUX::MkId(L"GetField");
}

// GenIsThisRecord
// ctr : REP`CompositeTypeRep
// name : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenIsThisRecord(const TYPE_REP_CompositeTypeRep & ctr, const TYPE_CPP_Expr & name)
{
  const TYPE_AS_Name & tagnm (ctr.GetRecord(pos_REP_CompositeTypeRep_nm));
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    TYPE_CPP_TypeSpecifier tp (vdm_BC_GenTypeSpecifier(vdm_BC_Rename(tagnm)));
    return vdm_BC_GenBracketedExpr(vdm_BC_GenTypeComp(tp, name));
  }
  else
#endif // VDMPP
  {
    TYPE_CPP_Name tag (vdm_BC_Rename(tagnm));
    TYPE_CPP_Expr expr (StripBracketedAndCastExpr(name));
    return vdm_BC_GenFctCallObjMemAcc(expr, ASTAUX::MkId(L"Is"), mk_sequence(tag));
  }
}

// GenRecSetField
// tup : CPP`Expr
// field : CPP`Expr
// elem : CPP`Expr
// ==> CPP`Stmt
TYPE_CPP_Stmt vdmcg::GenRecSetField(const TYPE_CPP_Expr & rec, const TYPE_CPP_Expr & field, const TYPE_CPP_Expr & elem)
{
  return vdm_BC_GenExpressionStmt(GenRecSetFieldExpr(rec, field, elem));
}

// GenRecSetFieldonThis
// field : CPP`Expr
// elem : CPP`Expr
// ==> CPP`Stmt
TYPE_CPP_Stmt vdmcg::GenRecSetFieldonThis(const TYPE_CPP_Expr & field, const TYPE_CPP_Expr & elem)
{
  TYPE_CPP_Expr fc (vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"SetField")), mk_sequence(field, elem)));
  return vdm_BC_GenExpressionStmt(fc);
}

// GenRecSetFieldExpr
// tup : CPP`Expr
// field : CPP`Expr
// elem : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenRecSetFieldExpr(const TYPE_CPP_Expr & rec,
                                        const TYPE_CPP_Expr & field,
                                        const TYPE_CPP_Expr & elem)
{
  return vdm_BC_GenFctCallObjMemAcc(rec, ASTAUX::MkId(L"SetField"), mk_sequence(field, elem));
}

// GenIsRecord
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenIsRecord(const TYPE_CPP_Expr & e)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    InsertImport(SEQ<Char>(L"jp.vdmtools.VDM.Record"));
    TYPE_CPP_TypeSpecifier tp (vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Record"))));
    return vdm_BC_GenBracketedExpr(vdm_BC_GenTypeComp(tp, e));
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"IsRecord"), SEQ<TYPE_CPP_Expr>());
}

// GenGetTag
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenGetTag(const TYPE_CPP_Expr & e)
{
  return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"GetTag"), SEQ<TYPE_CPP_Expr>());
}

// GenRecLength
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenRecLength(const TYPE_CPP_Expr & e)
{
  return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"Length"), SEQ<TYPE_CPP_Expr>());
}

// GenEmptyRecordExpr
// tp : [AS`Name]
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenEmptyRecordExpr(const Generic & tp)
{
  TYPE_CPP_TypeSpecifier ts (GenRecordType(tp));
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenClassInstanceCreationExpr(ts.get_tp(), SEQ<TYPE_CPP_Expr>());
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCall(ts.get_tp(), SEQ<TYPE_CPP_Expr>());
}

// GenGenericDecl
// name : CPP`Name
// initExpr : [CPP`Initializer]
// ==> seq1 of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenGenericDecl(const TYPE_CPP_Name & name, const Generic & initExpr)
{
  SEQ<TYPE_CPP_Stmt> rb_l;
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    if (initExpr.IsNil()) {
      rb_l.ImpAppend(vdm_BC_GenDecl(vdm_BC_GenGeneric(), name, vdm_BC_GenAsgnInit(GenNullExpr())));
    }
    else {
      rb_l.ImpAppend(vdm_BC_GenDecl(vdm_BC_GenGeneric(), name, initExpr));
    }
  }
  else
#endif // VDMPP
    rb_l.ImpAppend(vdm_BC_GenDecl(vdm_BC_GenGeneric(), name, initExpr));
  return rb_l;
}

// GenProductType
// ==> CPP`DeclSpecifier
TYPE_CPP_TypeSpecifier vdmcg::GenProductType()
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    InsertImport(SEQ<Char>(L"jp.vdmtools.VDM.Tuple"));
  }
#endif // VDMPP
  return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Tuple")));
}

// GenCastProduct
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenCastProduct(const TYPE_CPP_Expr & e)
{
  return vdm_BC_GenCastExpr(GenProductType(), e);
}

// GenCastProductTypeForModify
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenCastProductTypeForModify(const TYPE_CPP_Expr & e)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenCastExpr(GenProductType(), e);
  }
  else
#endif // VDMPP
  {
    SEQ<TYPE_CPP_TypeSpecifier> cmds_l;
    cmds_l.ImpAppend(vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Common"))));
    cmds_l.ImpAppend(vdm_BC_GenTypeSpecifier(quote_REFERENCE));
    TYPE_CPP_Expr cmcast (vdm_BC_GenCastExpr2(cmds_l, e));
    SEQ<TYPE_CPP_TypeSpecifier> ds_l;
    ds_l.ImpAppend(GenProductType());
    ds_l.ImpAppend(vdm_BC_GenTypeSpecifier(quote_REFERENCE));
    return vdm_BC_GenCastExpr2(ds_l, cmcast);
  }
}

// GenProductDecl
// name : CPP`Name
// length : nat
// initExpr : [CPP`Initializer]
// ==> seq1 of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenProductDecl_DS(const TYPE_CPP_Name & name,
                                            const Int & length,
                                            const Generic & initExpr)
{
  SEQ<TYPE_CPP_Stmt>  rb_l;
  if (initExpr.IsNil())
  {
    TYPE_CPP_Expr arg (vdm_BC_GenIntegerLit(length));
#ifdef VDMPP
    if (vdm_CPP_isJAVA()) {
      rb_l.ImpAppend(vdm_BC_GenDecl(GenProductType(), name, vdm_BC_GenAsgnInit(GenProductExpr(arg))));
    }
    else
#endif // VDMPP
      rb_l.ImpAppend(vdm_BC_GenDecl(GenProductType(), name, vdm_BC_GenObjectInit(mk_sequence(arg))));
  }
  else {
    rb_l.ImpAppend(vdm_BC_GenDecl(GenProductType(), name, initExpr));
  }
  return rb_l;
}

// GenTupleLength
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenTupleLength(const TYPE_CPP_Expr & e)
{
  return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"Length"), SEQ<TYPE_CPP_Expr>());
}

// GenTupSetField
// tup : CPP`Expr
// field : CPP`Expr
// elem : CPP`Expr
// ==> CPP`Stmt
TYPE_CPP_Stmt vdmcg::GenTupSetField(const TYPE_CPP_Expr & tup, const TYPE_CPP_Expr & field, const TYPE_CPP_Expr & elem)
{
  return vdm_BC_GenExpressionStmt(GenTupSetFieldExpr(tup, field, elem));
}

// GenTupSetFieldExpr
// tup : CPP`Expr
// field : CPP`Expr
// elem : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenTupSetFieldExpr(const TYPE_CPP_Expr & tup,
                                        const TYPE_CPP_Expr & field,
                                        const TYPE_CPP_Expr & elem)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    SetException(true);
  }
#endif // VDMPP
  return vdm_BC_GenFctCallObjMemAcc(tup, ASTAUX::MkId(L"SetField"), mk_sequence(field, elem));
}

// GenTupGetField
// e : CPP`Expr
// i : CPP`Expr
// tp : REP`TypeRep
// ptp : REP`TypeRep
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenTupGetField(const TYPE_CPP_Expr & e, const TYPE_CPP_Expr & i,
                                    const TYPE_REP_TypeRep & tp, const TYPE_REP_TypeRep & ptp)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    SetException(true);
    return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"GetField"), mk_sequence(i));
  }
  else
#endif // VDMPP
    if ((IsNumType(tp) && (tp != ptp)) || IsCharType(tp)) {
      return GenCastType(tp, vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"GetField"), mk_sequence(i)));
    }
    else {
      return vdm_BC_GenFctCallObjMemAcc(e, GenGetMethod(tp), mk_sequence(i));
    }
}

// GenIsTuple
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenIsTuple(const TYPE_CPP_Expr & e)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenBracketedExpr(vdm_BC_GenTypeComp(GenProductType(), e));
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"IsTuple"), SEQ<TYPE_CPP_Expr>());
}

// GenNilLit
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenNilLit()
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return GenNullExpr();
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Nil")), SEQ<TYPE_CPP_Expr>());
}

// GenIsNil
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenIsNil(const TYPE_CPP_Expr & e)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenBracketedExpr(vdm_BC_GenEq(e, GenNullExpr()));
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"IsNil"), SEQ<TYPE_CPP_Expr>() );
}

#ifdef VDMPP
// GenLocalComparator
// cname : AS`Name
// ==> seq of CPP`MemberDeclaration
SEQ<TYPE_CPP_MemberDeclaration> vdmcg::GenLocalComparator(const TYPE_AS_Name & cname)
{
  SEQ<TYPE_CPP_MemberDeclaration> pr_l;
  TYPE_AS_Id compClass (ASTAUX::MkId(L"UTIL.VDMCompare"));

  TYPE_CPP_AsgnInit initComparator (vdm_BC_GenAsgnInit(
               vdm_BC_GenClassInstanceCreationExpr( vdm_BC_GenIdentifier(compClass), type_dL())));

  pr_l.ImpAppend(vdm_BC_GenIdentDeclaration(SEQ<TYPE_CPP_Annotation>(),
                   type_dL().ImpAppend(vdm_BC_GenStorageClassSpecifier(quote_STATIC))
                            .ImpAppend(vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(compClass))),
                   Sequence().ImpAppend(vdm_BC_GenInitDecl(vdm_BC_GenIdentifier(ASTAUX::MkId(L"vdmComp")), initComparator))));
  return pr_l;
}
#endif // VDMPP

// GenSetType
// ==> CPP`DeclSpecifier
TYPE_CPP_TypeSpecifier vdmcg::GenSetType()
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    InsertImport(SEQ<Char>(L"java.util.Set"));
    if(get_generics_option())
      return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Set<Object>")) );
    else
      return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Set")));
  }
  else
#endif // VDMPP
    return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Set")));
}

// GenImplSetType
// ==> CPP`DeclSpecifier
TYPE_CPP_TypeSpecifier vdmcg::GenImplSetType()
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    InsertImport(SEQ<Char>(L"java.util.HashSet"));
    if(get_generics_option())
      return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"HashSet<Object>")) );
    else
      return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"HashSet")));
  }
  else
#endif // VDMPP
    return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Set")));
}

// GenDeclEmptySet
// name : CPP`Name
// ==> seq1 of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenDeclEmptySet(const TYPE_CPP_Name & name)
{
  SEQ<TYPE_CPP_Stmt> rb_l;
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    rb_l.ImpAppend(vdm_BC_GenDecl(GenSetType(), name, vdm_BC_GenAsgnInit(GenEmptySetExpr())));
  }
  else
#endif // VDMPP
    rb_l.ImpAppend(vdm_BC_GenDecl(GenSetType(), name, nil));
  return rb_l;
}

// GenSetDecl
// name : CPP`Name
// initExpr : [CPP`Initializer]
// ==> seq1 of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenSetDecl_DS(const TYPE_CPP_Name & name, const Generic & initExpr)
{
  SEQ<TYPE_CPP_Stmt> rb_l;
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    if (initExpr.IsNil()) {
      rb_l.ImpAppend(vdm_BC_GenDecl(GenSetType(), name, vdm_BC_GenAsgnInit(GenEmptySetExpr())));
    }
    else {
      rb_l.ImpAppend(vdm_BC_GenDecl(GenSetType(), name, initExpr));
    }
  }
  else
#endif // VDMPP
    rb_l.ImpAppend(vdm_BC_GenDecl(GenSetType(), name, initExpr));
  return rb_l;
}

// GenDeclSet
// name : CPP`Name
// ie : [CPP`Expr]
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenDeclSet(const TYPE_CPP_Name & name, const Generic & ie)
{
  if (ie.IsNil ()) {
    return GenDeclEmptySet(name);
  }
  else {
#ifdef VDMPP
    if (vdm_CPP_isJAVA()) {
      return GenSetDecl_DS(name, vdm_BC_GenAsgnInit(GenSetExpr(ie)));
    }
    else
#endif // VDMPP
      return GenSetDecl_DS(name, vdm_BC_GenObjectInit (mk_sequence(StripBracketedAndCastExpr(ie))));
  }
}

#ifdef VDMPP
// GenDeclCollection
// name : CPP`Name
// ie : [CPP`Expr]
// ==> seq1 of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenDeclCollection(const TYPE_CPP_Name& name, const Generic& ie)
{
  InsertImport(SEQ<Char>(L"java.util.Collection"));
  TYPE_CPP_TypeSpecifier ts (vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Collection"))));
  SEQ<TYPE_CPP_Stmt> rb_l;
  rb_l.ImpAppend(vdm_BC_GenDecl(ts, name, vdm_BC_GenAsgnInit(ie)));
  return rb_l;
}
#endif // VDMPP

// GenSetUnion
// e1 : CPP`Expr
// e2 : CPP`Expr
// ==> CPP`Stmt
TYPE_CPP_Stmt vdmcg::GenSetUnion(const TYPE_CPP_Expr & e1, const TYPE_CPP_Expr & e2)
{
  return vdm_BC_GenExpressionStmt(GenSetUnionExpr(e1, e2));
}

// GenSetUnionExpr
// e1 : CPP`Expr
// e2 : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenSetUnionExpr(const TYPE_CPP_Expr & e1, const TYPE_CPP_Expr & e2)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenFctCallObjMemAcc(e1, ASTAUX::MkId(L"addAll"), SEQ<TYPE_CPP_Expr>().ImpAppend(e2));
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCallObjMemAcc(e1, ASTAUX::MkId(L"ImpUnion"), SEQ<TYPE_CPP_Expr>().ImpAppend(e2));
}

// GenCastSetType
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenCastSetType(const TYPE_CPP_Expr & e)
{
  return vdm_BC_GenCastExpr(GenSetType(), e);
}

// GenCastSetTypeForModify
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenCastSetTypeForModify(const TYPE_CPP_Expr & e)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenCastExpr(GenSetType(), e);
  }
  else
#endif // VDMPP
  {
    SEQ<TYPE_CPP_TypeSpecifier> cmds_l;
    cmds_l.ImpAppend(vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Common"))));
    cmds_l.ImpAppend(vdm_BC_GenTypeSpecifier(quote_REFERENCE));
    TYPE_CPP_Expr cmcast (vdm_BC_GenCastExpr2(cmds_l, e));
    SEQ<TYPE_CPP_TypeSpecifier> ds_l;
    ds_l.ImpAppend(GenSetType());
    ds_l.ImpAppend(vdm_BC_GenTypeSpecifier(quote_REFERENCE));
    return vdm_BC_GenCastExpr2(ds_l, cmcast);
  }
}

// GenIsSet
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenIsSet(const TYPE_CPP_Expr & e)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenBracketedExpr(vdm_BC_GenTypeComp(GenSetType(), e));
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"IsSet"), SEQ<TYPE_CPP_Expr>());
}

// GenSetIsEmpty
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenSetIsEmpty(const TYPE_CPP_Expr & e)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"isEmpty"), SEQ<TYPE_CPP_Expr>());
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"IsEmpty"), SEQ<TYPE_CPP_Expr>());
}

// GenCard_int
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenCard_int(const TYPE_CPP_Expr & e)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"size"), SEQ<TYPE_CPP_Expr>());
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"Card"), SEQ<TYPE_CPP_Expr>());
}

// GenCard
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenCard(const TYPE_CPP_Expr & e)
{
  return GenIntExpr(GenCard_int(e));
}

// GenDUnion
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenDUnion(const TYPE_CPP_Expr & e)
{
  return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"DUnion"), SEQ<TYPE_CPP_Expr>());
}

// GenDInter
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenDInter(const TYPE_CPP_Expr & e)
{
  return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"DInter"), SEQ<TYPE_CPP_Expr>());
}

// GenPower
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenPower(const TYPE_CPP_Expr & e)
{
  return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"Power"), SEQ<TYPE_CPP_Expr>());
}

// GenUnion
// e1 : CPP`Expr
// e2 : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenUnion(const TYPE_CPP_Expr & e1, const TYPE_CPP_Expr & e2)
{
  return vdm_BC_GenFctCallObjMemAcc(e1, ASTAUX::MkId(L"Union"), mk_sequence(e2));
}

// GenInter
// e1 : CPP`Expr
// e2 : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenInter(const TYPE_CPP_Expr & e1, const TYPE_CPP_Expr & e2)
{
  return vdm_BC_GenFctCallObjMemAcc(e1, ASTAUX::MkId(L"Inter"), mk_sequence(e2));
}

// GenDiff
// e1 : CPP`Expr
// e2 : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenDiff(const TYPE_CPP_Expr & e1, const TYPE_CPP_Expr & e2)
{
  return vdm_BC_GenFctCallObjMemAcc(e1, ASTAUX::MkId(L"Diff"), mk_sequence(e2));
}

// GenPSubSet_Bool
// e1 : CPP`Expr
// e2 : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenPSubSet_Bool(const TYPE_CPP_Expr & e1, const TYPE_CPP_Expr & e2)
{
  TYPE_CPP_Expr fcall (vdm_BC_GenFctCallObjMemAcc(e1, ASTAUX::MkId(L"PSubSet"), mk_sequence(e2)));
  return GenBoolExpr(fcall);
}

// GenInterSect
// e1 : CPP`Expr
// e2 : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenInterSect(const TYPE_CPP_Expr & e1, const TYPE_CPP_Expr & e2)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenFctCallObjMemAcc(e1, ASTAUX::MkId(L"retainAll"), mk_sequence(e2));
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCallObjMemAcc(e1, ASTAUX::MkId(L"ImpIntersect"), mk_sequence(e2));
}

// GenInterSectStmt
// e1 : CPP`Expr
// e2 : CPP`Expr
// ==> CPP`Stmt
TYPE_CPP_Stmt vdmcg::GenInterSectStmt(const TYPE_CPP_Expr & e1, const TYPE_CPP_Expr & e2)
{
  return vdm_BC_GenExpressionStmt(GenInterSect(e1, e2));
}

// GenSetMinus
// e1 : CPP`Expr
// e2 : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenSetMinus(const TYPE_CPP_Expr & e1, const TYPE_CPP_Expr & e2)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenFctCallObjMemAcc(e1, ASTAUX::MkId(L"removeAll"), mk_sequence(e2));
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCallObjMemAcc(e1, ASTAUX::MkId(L"ImpDiff"), mk_sequence(e2));
}

// GenSetMinusStmt
// e1 : CPP`Expr
// e2 : CPP`Expr
// ==> CPP`Stmt
TYPE_CPP_Stmt vdmcg::GenSetMinusStmt(const TYPE_CPP_Expr & e1, const TYPE_CPP_Expr & e2)
{
  return vdm_BC_GenExpressionStmt(GenSetMinus(e1, e2));
}

// GenSubSet
// e1 : CPP`Expr
// e2 : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenSubSet(const TYPE_CPP_Expr & e1, const TYPE_CPP_Expr & e2)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenFctCallObjMemAcc(e2, ASTAUX::MkId(L"containsAll"), mk_sequence(e1));
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCallObjMemAcc(e1, ASTAUX::MkId(L"SubSet"), mk_sequence(e2));
}

// GenSubSet_Bool
// e1 : CPP`Expr
// e2 : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenSubSet_Bool(const TYPE_CPP_Expr & e1, const TYPE_CPP_Expr & e2)
{
  TYPE_CPP_Expr fcall (GenSubSet(e1, e2));
  return GenBoolExpr(fcall);
}

// GenEmptySetExpr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenEmptySetExpr()
{
  return GenSetExpr(nil);
}

// GenSetExpr
// ie : [CPP`Expr]
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenSetExpr(const Generic & ie)
{
  SEQ<TYPE_CPP_Expr> args;
  if (!ie.IsNil())
    args.ImpAppend(ie);
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenClassInstanceCreationExpr(GenImplSetType().get_tp(), args);
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCall(GenImplSetType().get_tp(), args);
}

// GenSetGetElemExpr
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenSetGetElemExpr(const TYPE_CPP_Expr & e)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    SetException(true);
    return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"iterator().next"), SEQ<TYPE_CPP_Expr>());
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"GetElem"), SEQ<TYPE_CPP_Expr>());
}

// GenInSet
// vt1 : CGMAIN`VT
// vt2 : CGMAIN`VT
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenInSet(const TYPE_CGMAIN_VT & vt1, const TYPE_CGMAIN_VT & vt2)
{
  TYPE_CPP_Expr fctcall (GenInSet_bool(vt1,vt2));
  return GenBoolExpr(fctcall);
}

// GenNotInSet
// vt1 : CGMAIN`VT
// vt2 : CGMAIN`VT
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenNotInSet(const TYPE_CGMAIN_VT & vt1, const TYPE_CGMAIN_VT & vt2)
{
  TYPE_CPP_Expr fctcall (vdm_BC_GenNot(GenInSet_bool(vt1,vt2)));
  return GenBoolExpr(fctcall);
}

// GenInSet_bool
// vt1 : CGMAIN`VT
// vt2 : CGMAIN`VT
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenInSet_bool(const TYPE_CGMAIN_VT & vt1, const TYPE_CGMAIN_VT & vt2)
{
  const TYPE_CPP_Expr & e1 (vt1.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_CPP_Expr & e2 (vt2.GetRecord(pos_CGMAIN_VT_name));

#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    TYPE_CPP_Expr newe1;
    const TYPE_REP_TypeRep & tp1 (vt1.GetRecord(pos_CGMAIN_VT_type));
    const TYPE_REP_TypeRep & tp2 (vt2.GetRecord(pos_CGMAIN_VT_type));
    if (!IsSetType(tp2))
      newe1 = e1;
    else {
      const TYPE_REP_TypeRep & elemtype (tp2.GetRecord(pos_REP_SetTypeRep_elemtp));
      if (IsIntType(tp1) && IsPosNumType(elemtype) && !IsPosIntType(elemtype))
        newe1 = GenExplicitCast(elemtype, e1, nil);
      else
        newe1 = e1;
    }
    return vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"UTIL.Contains")), mk_sequence(e2, newe1));
  }
  else
#endif // VDMPP
  {
    return vdm_BC_GenFctCallObjMemAcc(e2, ASTAUX::MkId(L"InSet"), mk_sequence(e1));
  }
}

// GenSetInsert
// e1 : CPP`Expr
// e2 : CPP`Expr
// ==> CPP`Stmt
TYPE_CPP_Stmt vdmcg::GenSetInsert(const TYPE_CPP_Expr & e1, const TYPE_CPP_Expr & e2)
{
  return vdm_BC_GenExpressionStmt(GenSetInsertExpr(e1, e2));
}

// GenSetInsertExpr
// e1 : CPP`Expr
// e2 : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenSetInsertExpr(const TYPE_CPP_Expr & e1, const TYPE_CPP_Expr & e2)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenFctCallObjMemAcc(e1, ASTAUX::MkId(L"add"), mk_sequence(e2));
  }
  else
#endif // VDMPP
  {
    TYPE_CPP_Expr e_q (e2);
    if ( e2.Is(TAG_TYPE_CPP_CastExpr )) {
      if ( e2.GetRecord(pos_CPP_CastExpr_expr).Is(TAG_TYPE_CPP_ArrayApply))
        e_q = e2.GetRecord(pos_CPP_CastExpr_expr);
    }
    return vdm_BC_GenFctCallObjMemAcc(e1, ASTAUX::MkId(L"Insert"), mk_sequence(e_q));
  }
}

// GenIterSet
// vt1 : CGMAIN`VT
// cond : [CPP`Expr]
// vt2 : CGMAIN`VT
// stmt : seq of CPP`Stmt
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenIterSet(const TYPE_CGMAIN_VT & vt1,
                                     const Generic & cond,
                                     const TYPE_CGMAIN_VT & vt2,
                                     const SEQ<TYPE_CPP_Stmt> & stmt)
{
  const TYPE_CPP_Expr & s (vt1.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_CPP_Expr & e (vt2.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & type (vt2.GetRecord(pos_CGMAIN_VT_type));

#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    TYPE_CPP_Identifier bb_v (vdm_BC_GiveName (ASTAUX::MkId(L"enm")));
    TYPE_CPP_Identifier tmpSet_v (vdm_BC_GiveName(ASTAUX::MkId(L"tmpSet")));
    TYPE_CPP_AsgnInit ai (vdm_BC_GenAsgnInit(
                            vdm_BC_GenFctCallObjMemAcc(tmpSet_v, ASTAUX::MkId(L"iterator"), SEQ<TYPE_CPP_Expr>())));
    TYPE_CPP_DeclarationStmt decl (vdm_BC_GenDecl(vdm_BC_GenTypeSpecifier(
                                                    vdm_BC_GenIdentifier(ASTAUX::MkId(L"Iterator"))), bb_v, ai));

    TYPE_CPP_Expr temp (vdm_BC_GenFctCallObjMemAcc(bb_v, ASTAUX::MkId(L"hasNext"), SEQ<TYPE_CPP_Expr>()));
    TYPE_CPP_Expr expr1 (cond.IsNil() ? temp : vdm_BC_GenLogAnd(temp, cond));

    TYPE_CPP_Expr fcall (vdm_BC_GenFctCallObjMemAcc(bb_v, ASTAUX::MkId(L"next"), SEQ<TYPE_CPP_Expr>()));

    SEQ<TYPE_CPP_Stmt> stmt_l;
    stmt_l.ImpConc(GenConstDeclInit(type, e, GenExplicitCast(type, fcall, nil)));
    stmt_l.ImpConc(stmt);

    InsertImport(SEQ<Char>(L"java.util.Iterator"));

    SEQ<TYPE_CPP_Stmt> rb;
    rb.ImpConc(GenDeclSet(tmpSet_v, s));
    rb.ImpAppend(vdm_BC_GenForStmt(decl, expr1, type_dL(), vdm_BC_GenBlock(stmt_l)));
    return rb;
  }
  else
#endif // VDMPP
  { // C++
    TYPE_CPP_Expr bb_v (vdm_BC_GiveName (ASTAUX::MkId(L"bb")));
    TYPE_CPP_Identifier tmpe_v (vdm_BC_GiveName(ASTAUX::MkId(L"tmpe")));
    TYPE_CPP_Identifier tmpSet_v (vdm_BC_GiveName(ASTAUX::MkId(L"tmpSet")));

    // create argument for First and Next
    SEQ<TYPE_CPP_Expr> tmpe_l;
    tmpe_l.ImpAppend(tmpe_v);

    TYPE_CPP_Expr first (vdm_BC_GenFctCallObjMemAcc(tmpSet_v, ASTAUX::MkId(L"First"), tmpe_l));
    TYPE_CPP_AsgnInit ai (vdm_BC_GenAsgnInit(first));
    TYPE_CPP_DeclarationStmt decl (vdm_BC_GenDecl(GenSmallBoolType(), bb_v, ai));
    TYPE_CPP_Expr expr1 (cond.IsNil() ? bb_v : vdm_BC_GenLogAnd(bb_v, cond));

    TYPE_CPP_Expr ae (vdm_BC_GenAsgnExpr(bb_v, vdm_BC_GenFctCallObjMemAcc(tmpSet_v, ASTAUX::MkId(L"Next"), tmpe_l)));

    // create internal block of for statemannt
    SEQ<TYPE_CPP_Stmt> stmt_l;
    stmt_l.ImpConc(GenConstDeclInit(type, e, tmpe_v));
    stmt_l.ImpConc(stmt);

    SEQ<TYPE_CPP_Stmt> rb;
    rb.ImpConc(GenDeclSet(tmpSet_v, s));
    rb.ImpAppend(vdm_BC_GenDecl(vdm_BC_GenGeneric(), tmpe_v, nil));
    rb.ImpAppend(vdm_BC_GenForStmt(decl, expr1, type_dL().ImpAppend(ae), vdm_BC_GenBlock(stmt_l)));
    return rb;
  }
}

// GenSeq0Type
// ==> CPP`DeclSpecifier
TYPE_CPP_TypeSpecifier vdmcg::GenSeq0Type()
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    InsertImport(SEQ<Char>(L"java.util.List"));
    if(get_generics_option()) {
      return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"List<Object>")));
    }
    else {
      return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"List")));
    }
  }
  else
#endif // VDMPP
    return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Sequence")));
}

// GenImplSeq0Type
// ==> CPP`DeclSpecifier
TYPE_CPP_TypeSpecifier vdmcg::GenImplSeq0Type()
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    InsertImport(SEQ<Char>(L"java.util.ArrayList"));
    if(get_generics_option()) {
      return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"ArrayList<Object>")));
    }
    else {
      return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"ArrayList")));
    }
  }
  else
#endif // VDMPP
    return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Sequence")));
}

// GenStringType
// ==> CPP`DeclSpecifier
TYPE_CPP_TypeSpecifier vdmcg::GenStringType()
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"String")));
  }
  else
#endif // VDMPP
    return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"SEQ<Char>")));
}

// GenSeqApply
// rtype : [REP`TypeRep]
// sq : CPP`Expr
// i : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenSeqApply(const Generic & rtype, const TYPE_CPP_Expr & sq, const TYPE_CPP_Expr & i)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    SetException(true);

    SEQ<TYPE_CPP_Expr> res;

    TYPE_CPP_Expr fcall;
    TYPE_CPP_Expr expr;
    if (i.Is(TAG_TYPE_CPP_IntegerLit)) {
      expr = vdm_BC_GenIntegerLit(i.GetInt(pos_CPP_IntegerLit_val) - Int(1));
    }
    else {
      expr = vdm_BC_GenMinus(i, vdm_BC_GenIntegerLit(1));
    }

    if (get_longs_option())
      expr = vdm_BC_GenCastExpr(vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"int"))), expr); 

    fcall = vdm_BC_GenFctCallObjMemAcc(sq, ASTAUX::MkId(L"get"), mk_sequence(expr));

    if (!rtype.IsNil()) {
      return GenExplicitCast(rtype, fcall, nil);
    }
    else {
      return fcall;
    }
  }
  else
#endif // VDMPP
  { // C++
    //TYPE_CPP_Expr fcall (vdm_BC_GenArrayApply(sq, i));
    TYPE_CPP_Expr i_q (i);
    if (i.Is(TAG_TYPE_CPP_FctCall)) {
      if (i.GetRecord(pos_CPP_FctCall_fct).Is(TAG_TYPE_CPP_ObjectMemberAccess)) {
        const TYPE_CPP_ObjectMemberAccess & oma (i.GetRecord(pos_CPP_FctCall_fct));
        if (oma.GetRecord(pos_CPP_ObjectMemberAccess_name) == vdm_BC_GenIdentifier(ASTAUX::MkId(L"GetValue"))) {
          i_q = oma.GetRecord(pos_CPP_ObjectMemberAccess_object);
        }
      }
    }
    TYPE_CPP_Expr fcall (vdm_BC_GenArrayApply(sq, i_q));
    if (!rtype.IsNil()) {
      return vdm_BC_GenCastExpr(GenType(rtype), fcall);
    }
    else {
      return fcall;
    }
  }
}

#ifdef VDMPP
// GenSeqApplyString
// sq : CPP`Expr
// i : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenSeqApplyString(const TYPE_CPP_Expr & sq, const TYPE_CPP_Expr & i)
{
  TYPE_CPP_Expr pos (i.Is(TAG_TYPE_CPP_IntegerLit) 
                  ? vdm_BC_GenIntegerLit(Int(i.GetIntValue(pos_CPP_IntegerLit_val) - 1))
                  : vdm_BC_GenMinus(i, vdm_BC_GenIntegerLit(1)));
  TYPE_CPP_Expr fcall (vdm_BC_GenFctCallObjMemAcc(sq, ASTAUX::MkId(L"charAt"), mk_sequence(pos)));

  return GenCharExpr(fcall);
}
#endif // VDMPP

// GenHd
// rtype : REP`TypeRep
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenHd(const TYPE_REP_TypeRep & rtype, const TYPE_CPP_Expr & e)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    SetException(true);

    TYPE_CPP_Expr fcall (vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"get"),
                                SEQ<TYPE_CPP_Expr>().ImpAppend(vdm_BC_GenIntegerLit(0))));

    return GenExplicitCast(rtype, fcall, nil);
  }
  else
#endif // VDMPP
  {
    TYPE_CPP_TypeSpecifier tp (GenType(rtype));
    TYPE_CPP_Expr fcall (vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"Hd"), SEQ<TYPE_CPP_Expr>()));
    if (tp != vdm_BC_GenGeneric()) {
      return vdm_BC_GenCastExpr(tp, fcall);
    }
    else {
      return fcall;
    }
  }
}

#ifdef VDMPP
// GenHdString
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenHdString(const TYPE_CPP_Expr & e)
{
  TYPE_CPP_Expr fcall (vdm_BC_GenFctCallObjMemAcc(e,
                                                  ASTAUX::MkId(L"charAt"),
                                                  SEQ<TYPE_CPP_Expr>().ImpAppend(vdm_BC_GenIntegerLit(0))));
  return GenCharExpr(fcall);
}
#endif // VDMPP

// GenInds
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenInds(const TYPE_CPP_Expr & e)
{
  return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"Inds"), SEQ<TYPE_CPP_Expr>());
}

// GenDConc
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenDConc(const TYPE_CPP_Expr & e)
{
  return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"DConc"), SEQ<TYPE_CPP_Expr>());
}

// GenConc
// e1 : CPP`Expr
// e2 : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenConc(const TYPE_CPP_Expr & e1, const TYPE_CPP_Expr & e2)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenFctCallObjMemAcc(e1, ASTAUX::MkId(L"addAll"), SEQ<TYPE_CPP_Expr>().ImpAppend(e2));
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCallObjMemAcc(e1, ASTAUX::MkId(L"Conc"), SEQ<TYPE_CPP_Expr>().ImpAppend(e2));
}

#ifdef VDMPP
// GenConcString
// e1 : CPP`Expr
// e2 : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenConcString(const TYPE_CPP_Expr & e1, const TYPE_CPP_Expr & e2)
{
  if (IsStringExpr(e2))
  {
    TYPE_CPP_Expr arg (e2.GetSequence(pos_CPP_ClassInstanceCreationExpr_arg)[1]);
      return vdm_BC_GenFctCallObjMemAcc(e1, ASTAUX::MkId(L"concat"), SEQ<TYPE_CPP_Expr>().ImpAppend(arg));
  }
  return vdm_BC_GenFctCallObjMemAcc(e1, ASTAUX::MkId(L"concat"), SEQ<TYPE_CPP_Expr>().ImpAppend(e2));
}
#endif // VDMPP

// GenTl
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenTl(const TYPE_CPP_Expr & e)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    SetException(true);

    TYPE_CPP_Expr end_of_list (vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"size"), SEQ<TYPE_CPP_Expr>()));
    TYPE_CPP_Expr fcall (vdm_BC_GenFctCallObjMemAcc(e,
                                                    ASTAUX::MkId(L"subList"),
                                                    mk_sequence(vdm_BC_GenIntegerLit(1), end_of_list)));
    return GenSeqExpr(fcall);
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"Tl"), SEQ<TYPE_CPP_Expr>());
}

#ifdef VDMPP
// GenTlString
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenTlString(const TYPE_CPP_Expr & e)
{
  return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"substring"), mk_sequence(vdm_BC_GenIntegerLit(1)));
}
#endif // VDMPP

// GenReverse
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenReverse(const TYPE_CPP_Expr & e)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    SetException(true);
    TYPE_CPP_Expr fcall (vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"UTIL.Reverse")), mk_sequence(e)));
    return GenSeqExpr(fcall);
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"Reverse"), SEQ<TYPE_CPP_Expr>());
}

#ifdef VDMPP
// GenReverseString
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenReverseString(const TYPE_CPP_Expr & e)
{
  return vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"UTIL.ReverseString")), SEQ<TYPE_CPP_Expr>().ImpAppend(e));
}
#endif // VDMPP

// GenLen
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenLen(const TYPE_CPP_Expr & e)
{
  return GenIntExpr(GenLen_int(e));
}

#ifdef VDMPP
// GenLenString
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenLenString(const TYPE_CPP_Expr & e)
{
  TYPE_CPP_Expr fc (GenLenString_int(e));
  return GenIntExpr(fc);
}
#endif // VDMPP

// GenLen_int
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenLen_int(const TYPE_CPP_Expr & e)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"size"), SEQ<TYPE_CPP_Expr>());
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"Length"), SEQ<TYPE_CPP_Expr>());
}

#ifdef VDMPP
// GenLenString_int
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenLenString_int(const TYPE_CPP_Expr & e)
{
  if (e.Is(TAG_TYPE_CPP_WStringLit))
  {
    return vdm_BC_GenIntegerLit(e.GetSequence(pos_CPP_WStringLit_val).Length());
  }
  return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"length"), SEQ<TYPE_CPP_Expr>());
}
#endif // VDMPP

// GenCastSeq
// e : CPP`Expr
// t : [REP`TypeRep]
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenCastSeq(const TYPE_CPP_Expr & e, const Generic & t)
{
  if (t.IsNil ())
    return vdm_BC_GenCastExpr(GenSeq0Type (), e);
  else {
    if (IsSeqType (t))
      return e;
    else
      return vdm_BC_GenCastExpr(GenSeq0Type (), e);
  }
}

// GenCastSeqTypeForModify
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenCastSeqTypeForModify(const TYPE_CPP_Expr & e)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenCastExpr(GenSeq0Type (), e);
  }
  else
#endif // VDMPP
  {
    SEQ<TYPE_CPP_TypeSpecifier> cmds_l;
    cmds_l.ImpAppend(vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Common"))));
    cmds_l.ImpAppend(vdm_BC_GenTypeSpecifier(quote_REFERENCE));
    TYPE_CPP_Expr cmcast (vdm_BC_GenCastExpr2(cmds_l, e));
    SEQ<TYPE_CPP_TypeSpecifier> ds_l;
    ds_l.ImpAppend(GenSeq0Type());
    ds_l.ImpAppend(vdm_BC_GenTypeSpecifier(quote_REFERENCE));
    return vdm_BC_GenCastExpr2(ds_l, cmcast);
  }
}

#ifdef VDMPP
// GenCastString
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenCastString(const TYPE_CPP_Expr & e)
{
  return vdm_BC_GenCastExpr(GenStringType (), e);
}
#endif // VDMPP

// GenSubSequence
// e : CPP`Expr
// t : [REP`TypeRep]
// from : CPP`Expr
// to : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenSubSequence(const TYPE_CPP_Expr & e, const Generic & t,
                                    const TYPE_CPP_Expr & from, const TYPE_CPP_Expr & to)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    if (!t.IsNil () && IsStringType(t)) {
      return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"substring"), mk_sequence(from, to));
    }
    else {
      TYPE_CPP_Expr fc (vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"subList"), mk_sequence(from, to)));
      return GenSeqExpr(fc);
    }
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"SubSequence"), mk_sequence(from, to));
}

// GenPermute
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenPermute(const TYPE_CPP_Expr & e)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    TYPE_CPP_Identifier Permute_f (vdm_BC_GenIdentifier(ASTAUX::MkId(L"UTIL.Permute")));
    return vdm_BC_GenFctCall (Permute_f, mk_sequence(e));
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"Permute"), SEQ<TYPE_CPP_Expr>());
}

// GenImpAppend
// s : CPP`Expr
// e : CPP`Expr
// ==> CPP`Stmt
TYPE_CPP_Stmt vdmcg::GenImpAppend(const TYPE_CPP_Expr & s, const TYPE_CPP_Expr & e)
{
  return vdm_BC_GenExpressionStmt(GenImpAppendExpr(s, e));
}

// GenImpAppendExpr
// s : CPP`Expr
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenImpAppendExpr(const TYPE_CPP_Expr & s, const TYPE_CPP_Expr & e)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenFctCallObjMemAcc(s, ASTAUX::MkId(L"add"), SEQ<TYPE_CPP_Expr>().ImpAppend(e));
  }
  else
#endif // VDMPP
  {
    TYPE_CPP_Expr e_q (e);
    if ( e.Is(TAG_TYPE_CPP_CastExpr )) {
      if ( e.GetRecord(pos_CPP_CastExpr_expr).Is(TAG_TYPE_CPP_ArrayApply))
        e_q = e.GetRecord(pos_CPP_CastExpr_expr);
    }
    return vdm_BC_GenFctCallObjMemAcc(s, ASTAUX::MkId(L"ImpAppend"), SEQ<TYPE_CPP_Expr>().ImpAppend(e_q));
  }
}

// GenSeqModify
// s : CPP`Expr
// e1 : CPP`Expr
// e2 : CPP`Expr
// ==> CPP`Stmt
TYPE_CPP_Stmt vdmcg::GenSeqModify(const TYPE_CPP_Expr & s, const TYPE_CPP_Expr & e1, const TYPE_CPP_Expr & e2)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenExpressionStmt(GenSeqModifyExpr(s, e1, e2));
  }
  else
#endif // VDMPP
  {
    TYPE_CPP_Expr l_check (vdm_BC_GenFctCallObjMemAcc(s, ASTAUX::MkId(L"OutOfRange"), mk_sequence(e1)));
    TYPE_CPP_Stmt l_else (vdm_BC_GenBlock(mk_sequence(vdm_BC_GenExpressionStmt(GenSeqModifyExpr(s, e1, e2)))));
    TYPE_CPP_Stmt l_then (vdm_BC_GenBlock(mk_sequence(RunTime(L"Illegal index in sequence modifier"))));
    return vdm_BC_GenIfStmt(l_check, l_then, l_else);
  }
}

// GenSeqModifyExpr
// s : CPP`Expr
// e1 : CPP`Expr
// e2 : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenSeqModifyExpr(const TYPE_CPP_Expr & s, const TYPE_CPP_Expr & e1, const TYPE_CPP_Expr & e2)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    SetException(true);
    TYPE_CPP_Expr expr (vdm_BC_GenMinus(e1, vdm_BC_GenIntegerLit(1)));
    if (get_longs_option())
      expr = vdm_BC_GenCastExpr(vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"int"))), expr); 

    return vdm_BC_GenFctCallObjMemAcc(s, ASTAUX::MkId(L"set"), mk_sequence(expr, e2));
  }
  else
#endif // VDMPP
  {
    return vdm_BC_GenFctCallObjMemAcc(s, ASTAUX::MkId(L"ImpModify"), SEQ<TYPE_CPP_Expr>().ImpAppend(e1).ImpAppend(e2));
  }
}

// GenModify
// e1 : CPP`Expr
// e2 : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenModify(const TYPE_CPP_Expr & e1, const TYPE_CPP_Expr & e2)
{
  return vdm_BC_GenFctCallObjMemAcc(e1, ASTAUX::MkId(L"SeqModify"), SEQ<TYPE_CPP_Expr>().ImpAppend(e2));
}

// GenIsSeq
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenIsSeq(const TYPE_CPP_Expr & e)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenBracketedExpr(vdm_BC_GenTypeComp(GenSeq0Type(), e));
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"IsSequence"), SEQ<TYPE_CPP_Expr>());
}

// GenIsString
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenIsString(const TYPE_CPP_Expr & e)
{
  return vdm_BC_GenBracketedExpr(vdm_BC_GenTypeComp(GenStringType(), e));
}

// GenSeqIsEmpty
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenSeqIsEmpty(const TYPE_CPP_Expr & e)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"isEmpty"), SEQ<TYPE_CPP_Expr>());
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"IsEmpty"), SEQ<TYPE_CPP_Expr>());
}

// GenDeclEmptySeq
// name : CPP`Name
// ==> seq1 of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenDeclEmptySeq(const TYPE_CPP_Name & name)
{
  SEQ<TYPE_CPP_Stmt> rb_l;
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    rb_l.ImpAppend(vdm_BC_GenDecl(GenSeq0Type(), name, vdm_BC_GenAsgnInit( GenEmptySeqExpr() )));
  }
  else
#endif // VDMPP
    rb_l.ImpAppend(vdm_BC_GenDecl(GenSeq0Type(), name, nil));
  return rb_l;
}

// GenSeqDecl
// name : CPP`Name
// initExpr : [CPP`Initializer]
// ==> seq1 of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenSeqDecl_DS(const TYPE_CPP_Name & name, const Generic & initExpr)
{
  SEQ<TYPE_CPP_Stmt> rb_l;
#ifdef VDMPP
  if (vdm_CPP_isJAVA() && initExpr.IsNil())
    rb_l.ImpAppend(vdm_BC_GenDecl(GenSeq0Type(), name, vdm_BC_GenAsgnInit(GenNullExpr())));
  else
#endif // VDMPP
    rb_l.ImpAppend(vdm_BC_GenDecl(GenSeq0Type(), name, initExpr));
  return rb_l;
}

#ifdef VDMPP
// GenStringDecl
// name : CPP`Name
// initExpr : [CPP`Initializer]
// ==> seq1 of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenStringDecl(const TYPE_CPP_Name & name, const Generic & initExpr)
{
  if (initExpr.IsNil()) {
    SEQ<TYPE_CPP_Stmt> rb_l;
    rb_l.ImpAppend(vdm_BC_GenDecl(GenStringType(), name, vdm_BC_GenAsgnInit(GenNullExpr())));
    return rb_l;
  }
  else
    if (initExpr.Is(TAG_TYPE_CPP_AsgnInit) &&
       Record(initExpr).GetRecord(pos_CPP_AsgnInit_expr).Is(TAG_TYPE_CPP_ClassInstanceCreationExpr))
    {
      TYPE_CPP_ClassInstanceCreationExpr cice (Record(initExpr).GetRecord(pos_CPP_AsgnInit_expr));
      if (cice.GetRecord(pos_CPP_ClassInstanceCreationExpr_classtype) == GenStringType().get_tp())
      {
        SEQ<TYPE_CPP_Stmt> rb_l;
        const SEQ<TYPE_CPP_Expr> & arg (cice.GetSequence(pos_CPP_ClassInstanceCreationExpr_arg));
        if (arg.IsEmpty())
          rb_l.ImpAppend(vdm_BC_GenDecl(GenStringType(), name, vdm_BC_GenAsgnInit(vdm_BC_GenStringLit(ASTAUX::MkId(L"")))));
        else
          rb_l.ImpAppend(vdm_BC_GenDecl(GenStringType(), name, vdm_BC_GenAsgnInit(arg[1])));
        return rb_l;
      }
    }
    SEQ<TYPE_CPP_Stmt> rb_l;
    rb_l.ImpAppend(vdm_BC_GenDecl(GenStringType(), name, initExpr));
    return rb_l;
}
#endif // VDMPP

// GenDeclSeq
// name : CPP`Name
// ie : [CPP`Expr]
// ==> seq1 of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenDeclSeq(const TYPE_CPP_Name & name, const Generic & ie)
{
  if (ie.IsNil ())
    return GenDeclEmptySeq(name);
  else { 
#ifdef VDMPP
    if (vdm_CPP_isJAVA()) {
      return GenSeqDecl_DS(name, vdm_BC_GenAsgnInit(GenSeqExpr(ie)));
    }
    else
#endif // VDMPP
      return GenSeqDecl_DS(name, vdm_BC_GenObjectInit (mk_sequence(StripBracketedAndCastExpr(ie))));
  }
}

// GenSeqConc
// e1 : CPP`Expr
// e2 : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenSeqConc(const TYPE_CPP_Expr & e1, const TYPE_CPP_Expr & e2)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenFctCallObjMemAcc(e1, ASTAUX::MkId(L"addAll"), mk_sequence(e2));
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCallObjMemAcc(e1, ASTAUX::MkId(L"ImpConc"), mk_sequence(e2));
}

// GenSeqConcStmt
// e1 : CPP`Expr
// e2 : CPP`Expr
// ==> CPP`Stmt
TYPE_CPP_Stmt vdmcg::GenSeqConcStmt(const TYPE_CPP_Expr & e1, const TYPE_CPP_Expr & e2)
{
  return vdm_BC_GenExpressionStmt(GenSeqConc(e1,e2));
}

// GenSeqElems
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenSeqElems(const TYPE_CPP_Expr & e)
{
  return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"Elems"), SEQ<TYPE_CPP_Expr>());
}

// GenEmptySeqExpr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenEmptySeqExpr()
{
  return GenSeqExpr(nil);
}

// GenSeqExpr
// ie : [CPP`Expr]
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenSeqExpr(const Generic & ie)
{
  SEQ<TYPE_CPP_Expr> args;
  if (!ie.IsNil()) {
    args.ImpAppend(ie);
  }
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenClassInstanceCreationExpr(GenImplSeq0Type().get_tp(), args);
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCall(GenImplSeq0Type().get_tp(), args);
}

// GenEmptyStringExpr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenEmptyStringExpr()
{
  return GenStringExpr(nil);
}

// GenStringExpr
// ie : [CPP`Expr]
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenStringExpr(const Generic & ie)
{
  SEQ<TYPE_CPP_Expr> args;
  if (!ie.IsNil()) {
    args.ImpAppend(ie);
  }
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenClassInstanceCreationExpr(GenStringType().get_tp(), args);
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCall(GenStringType().get_tp(), args);
}

// GenIterSeq
// vt1 : CGMAIN`VT
// cond : [CPP`Expr]
// vt2 : CGMAIN`VT
// stmt : seq of CPP`Stmt
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenIterSeq(const TYPE_CGMAIN_VT & vt1,
                                     const Generic & cond,
                                     const TYPE_CGMAIN_VT & vt2,
                                     const SEQ<TYPE_CPP_Stmt> & stmt)
{
  const TYPE_CPP_Expr & s        (vt1.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & stype (vt1.GetRecord(pos_CGMAIN_VT_type));
  const TYPE_CPP_Expr & e        (vt2.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & type  (vt2.GetRecord(pos_CGMAIN_VT_type));

  TYPE_CPP_Identifier tmpe_v (vdm_BC_GiveName(ASTAUX::MkId(L"tmpe")));
  TYPE_CPP_Identifier tmpSeq_v (vdm_BC_GiveName(ASTAUX::MkId(L"tmpSeq"))); // 20101219

#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    TYPE_CPP_Identifier bb_v (vdm_BC_GiveName (ASTAUX::MkId(L"enm")));
    TYPE_CPP_Expr s_q (IsPossibleStringType(stype)
                         ? GenExplicitCast(mk_REP_SeqTypeRep(mk_REP_NumericTypeRep(Int(NAT))), s, stype)
                         : s);

    TYPE_CPP_AsgnInit ai (vdm_BC_GenAsgnInit(vdm_BC_GenFctCallObjMemAcc(tmpSeq_v,
                                                                        ASTAUX::MkId(L"iterator"),
                                                                        SEQ<TYPE_CPP_Expr>())));

    TYPE_CPP_DeclarationStmt first (vdm_BC_GenDecl(
                     vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Iterator"))), bb_v, ai));
    TYPE_CPP_Expr expr (vdm_BC_GenFctCallObjMemAcc(bb_v, ASTAUX::MkId(L"hasNext"), SEQ<TYPE_CPP_Expr>()));
    if (! cond.IsNil() )
      expr = vdm_BC_GenLogAnd(expr, cond);
    TYPE_CPP_Expr fcall (vdm_BC_GenFctCallObjMemAcc(bb_v, ASTAUX::MkId(L"next"), SEQ<TYPE_CPP_Expr>()));

    SEQ<TYPE_CPP_Expr> next;

    SEQ<TYPE_CPP_Stmt> stmt_l;
    stmt_l.ImpConc(GenConstDeclInit(type, e, GenExplicitCast(type, fcall, nil)));
    stmt_l.ImpConc(stmt);

    InsertImport(SEQ<Char>(L"java.util.Iterator"));

    SEQ<TYPE_CPP_Stmt> rb_l;
    rb_l.ImpConc(GenDeclSeq(tmpSeq_v, s_q));
    rb_l.ImpAppend(vdm_BC_GenForStmt(first, expr, next, vdm_BC_GenBlock(stmt_l)));
    return rb_l;
  }
  else
#endif // VDMPP
  { // C++
    TYPE_CPP_Expr bb_v (vdm_BC_GiveName (ASTAUX::MkId(L"bb")));

    SEQ<TYPE_CPP_Expr> tmpe_vl;
    tmpe_vl.ImpAppend(tmpe_v);

    TYPE_CPP_AsgnInit ai (vdm_BC_GenAsgnInit(vdm_BC_GenFctCallObjMemAcc(tmpSeq_v, ASTAUX::MkId(L"First"), tmpe_vl)));
    TYPE_CPP_DeclarationStmt first (vdm_BC_GenDecl(GenSmallBoolType(), bb_v, ai));

    TYPE_CPP_Expr expr (!cond.IsNil() ? vdm_BC_GenLogAnd(bb_v, cond) : bb_v);

    SEQ<TYPE_CPP_Expr> next;
    next.ImpAppend(vdm_BC_GenAsgnExpr(bb_v, vdm_BC_GenFctCallObjMemAcc(tmpSeq_v, ASTAUX::MkId(L"Next"), tmpe_vl)));

    SEQ<TYPE_CPP_Stmt> stmt_l;
    stmt_l.ImpConc(GenConstDeclInit(type, e, tmpe_v));
    stmt_l.ImpConc(stmt);

    SEQ<TYPE_CPP_Stmt> rb_l;
    rb_l.ImpConc(GenDeclSeq(tmpSeq_v, s));
    rb_l.ImpAppend(vdm_BC_GenDecl(vdm_BC_GenGeneric(), tmpe_v, nil));
    rb_l.ImpAppend(vdm_BC_GenForStmt(first, expr, next, vdm_BC_GenBlock(stmt_l)));
    return rb_l;
  }
}

// GenMapType
// ==> CPP`DeclSpecifier
TYPE_CPP_TypeSpecifier vdmcg::GenMapType()
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    InsertImport(SEQ<Char>(L"java.util.Map"));
    return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Map")) );
  }
  else
#endif // VDMPP
    return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Map")) );
}

// GenImplMapType
// ==> CPP`DeclSpecifier
TYPE_CPP_TypeSpecifier vdmcg::GenImplMapType()
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    InsertImport(SEQ<Char>(L"java.util.HashMap"));
    return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"HashMap")) );
  }
  else
#endif // VDMPP
    return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Map")) );
}

// GenDom
// m : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenDom(const TYPE_CPP_Expr & e)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"keySet"), SEQ<TYPE_CPP_Expr>());
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"Dom"), SEQ<TYPE_CPP_Expr>());
}

// GenDMerge
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenDMerge(const TYPE_CPP_Expr & e)
{
  return vdm_BC_GenFctCallObjMemAcc(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Map")),
                                    ASTAUX::MkId(L"DMerge"),
                                    SEQ<TYPE_CPP_Expr>().ImpAppend(e));
}

// GenRng
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenRng(const TYPE_CPP_Expr & e)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"values"), SEQ<TYPE_CPP_Expr>());
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"Rng"), SEQ<TYPE_CPP_Expr>());
}

// GenIsMap
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenIsMap(const TYPE_CPP_Expr & e)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenBracketedExpr(vdm_BC_GenTypeComp(GenMapType(), e));
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"IsMap"), SEQ<TYPE_CPP_Expr>());
}

// GenMapIsEmpty
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenMapIsEmpty(const TYPE_CPP_Expr & e)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"isEmpty"), SEQ<TYPE_CPP_Expr>());
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"IsEmpty"), SEQ<TYPE_CPP_Expr>());
}

// GenSize_int
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenSize_int(const TYPE_CPP_Expr & e)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"size"), SEQ<TYPE_CPP_Expr>());
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"Size"), SEQ<TYPE_CPP_Expr>());
}

// GenSize
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenSize(const TYPE_CPP_Expr & e)
{
  return GenIntExpr(GenSize_int(e));
}

// GenMapDecl
// name : CPP`Name
// initExpr : [CPP`Initializer]
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenMapDecl_DS(const TYPE_CPP_Name & name, const Generic & initExpr)
{
  SEQ<TYPE_CPP_Stmt> rb_l;
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    if (initExpr.IsNil()) {
      rb_l.ImpAppend(vdm_BC_GenDecl(GenMapType(), name, vdm_BC_GenAsgnInit(GenEmptyMapExpr())));
    }
    else {
      rb_l.ImpAppend(vdm_BC_GenDecl(GenMapType(), name, initExpr));
    }
  }
  else
#endif // VDMPP
    rb_l.ImpAppend(vdm_BC_GenDecl(GenMapType(), name, initExpr));
  return rb_l;
}

// GenDeclMap
// name : CPP`Name
// ie : [CPP`Expr]
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenDeclMap(const TYPE_CPP_Name & name, const Generic & ie)
{
  if (ie.IsNil ())
    return GenDeclEmptyMap(nil, name);
  else {
#ifdef VDMPP
    if (vdm_CPP_isJAVA()) {
      return GenMapDecl_DS(name, vdm_BC_GenAsgnInit (GenMapExpr(ie)));
    }
    else
#endif // VDMPP
      return GenMapDecl_DS(name, vdm_BC_GenObjectInit (mk_sequence(StripBracketedAndCastExpr(ie))));
  }
}

// GenCastMap
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenCastMap(const TYPE_CPP_Expr & e)
{
  return vdm_BC_GenCastExpr(GenMapType(), e);
}

// GenCastMapTypeForModify
// e : CPP`Expr
//  ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenCastMapTypeForModify(const TYPE_CPP_Expr & e)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenCastExpr(GenMapType(), e);
  }
  else
#endif // VDMPP
  {
    SEQ<TYPE_CPP_TypeSpecifier> cmds_l;
    cmds_l.ImpAppend(vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Common"))));
    cmds_l.ImpAppend(vdm_BC_GenTypeSpecifier(quote_REFERENCE));
    TYPE_CPP_Expr cmcast (vdm_BC_GenCastExpr2(cmds_l, e));
    SEQ<TYPE_CPP_TypeSpecifier> ds_l;
    ds_l.ImpAppend(GenMapType());
    ds_l.ImpAppend(vdm_BC_GenTypeSpecifier(quote_REFERENCE));
    return vdm_BC_GenCastExpr2(ds_l, cmcast);
  }
}

// GenMapApply
// vt1 : CGMAIN`VT
// e2 : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenMapApply(const TYPE_CGMAIN_VT & vt1, const TYPE_CPP_Expr & e2)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    SetException(true);
  }
#endif // VDMPP

  const TYPE_CPP_Expr & e1 (vt1.GetRecord(pos_CGMAIN_VT_name));
  const Generic & e1t (vt1.GetField(pos_CGMAIN_VT_type));

  if ( e1t.IsNil() )
  {
#ifdef VDMPP
    if (vdm_CPP_isJAVA()) {
      return vdm_BC_GenFctCallObjMemAcc(e1, ASTAUX::MkId(L"get"), mk_sequence(e2));
    }
    else
#endif // VDMPP
      return  vdm_BC_GenArrayApply(e1, e2);
  }
  else
  {
    TYPE_REP_TypeRep e1type (e1t);
    if (e1type.Is(TAG_TYPE_REP_GeneralMapTypeRep) || e1type.Is(TAG_TYPE_REP_InjectiveMapTypeRep) )
    {
      const TYPE_REP_TypeRep & maprng (e1type.GetRecord(2));
#ifdef VDMPP
      if (vdm_CPP_isJAVA()) {
        return GenExplicitCast(maprng, vdm_BC_GenFctCallObjMemAcc(e1, ASTAUX::MkId(L"get"), mk_sequence(e2)), nil);
      }
      else
#endif // VDMPP
      {
        TYPE_REP_TypeRep tp (CleanFlatType(maprng));
        return GenCastType(tp, vdm_BC_GenArrayApply( e1, e2 ));
      }
    }
    else
      ReportError(L"GenMapApply");
    return Record(0,0); // To avoid warnings
  }
}

// GenMapInsert
// m : CPP`Expr
// e1 : CPP`Expr
// e2 : CPP`Expr
// ==> CPP`Stmt
TYPE_CPP_Stmt vdmcg::GenMapInsert(const TYPE_CPP_Expr & m, const TYPE_CPP_Expr & e1, const TYPE_CPP_Expr & e2)
{
  return vdm_BC_GenExpressionStmt(GenMapInsertExpr(m, e1, e2));
}

// GenMapInsertExpr
// m : CPP`Expr
// e1 : CPP`Expr
// e2 : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenMapInsertExpr(const TYPE_CPP_Expr & m, const TYPE_CPP_Expr & e1, const TYPE_CPP_Expr & e2)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    SetException(true);
    return vdm_BC_GenFctCallObjMemAcc(m, ASTAUX::MkId(L"put"), mk_sequence(e1, e2));
  }
  else
#endif // VDMPP
  {
    return vdm_BC_GenFctCallObjMemAcc(m, ASTAUX::MkId(L"Insert"), mk_sequence(e1, e2));
  }
}

// GenMapImpModify
// m : CPP`Expr
// e1 : CPP`Expr
// e2 : CPP`Expr
// ==> CPP`Stmt
TYPE_CPP_Stmt vdmcg::GenMapImpModify(const TYPE_CPP_Expr & m, const TYPE_CPP_Expr & e1, const TYPE_CPP_Expr & e2)
{
  return vdm_BC_GenExpressionStmt(GenMapImpModifyExpr(m,e1,e2));
}

// GenMapImpModifyExpr
// m : CPP`Expr
// e1 : CPP`Expr
// e2 : CPP`Expr
// ==> CPP`Stmt
TYPE_CPP_Expr vdmcg::GenMapImpModifyExpr(const TYPE_CPP_Expr & m, const TYPE_CPP_Expr & e1, const TYPE_CPP_Expr & e2)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
   return vdm_BC_GenFctCallObjMemAcc(m, ASTAUX::MkId(L"put"), mk_sequence(e1, e2));
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCallObjMemAcc(m, ASTAUX::MkId(L"ImpModify"), mk_sequence(e1, e2));
}

// GenMapOverride
// m1 : CPP`Expr
// m2 : CPP`Expr
// ==> CPP`Stmt
TYPE_CPP_Stmt vdmcg::GenMapOverride(const TYPE_CPP_Expr & m1, const TYPE_CPP_Expr & m2)
{
  TYPE_CPP_Expr fc;
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    fc = vdm_BC_GenFctCallObjMemAcc(m1, ASTAUX::MkId(L"putAll"), SEQ<TYPE_CPP_Expr>().ImpAppend(m2));
  }
  else
#endif // VDMPP
    fc = vdm_BC_GenFctCallObjMemAcc(m1, ASTAUX::MkId(L"ImpOverride"), SEQ<TYPE_CPP_Expr>().ImpAppend(m2));
  return vdm_BC_GenExpressionStmt(fc);
}

// GenOverride
// e1 : CPP`Expr
// e2 : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenOverride(const TYPE_CPP_Expr & e1, const TYPE_CPP_Expr & e2)
{
  return vdm_BC_GenFctCallObjMemAcc(e1, ASTAUX::MkId(L"Override"), SEQ<TYPE_CPP_Expr>().ImpAppend(e2));
}

// GenInverse
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenInverse(const TYPE_CPP_Expr & e)
{
  return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"Inverse"), SEQ<TYPE_CPP_Expr>());
}

// GenDomExists
// m : CPP`Expr
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenDomExists(const TYPE_CPP_Expr & m, const TYPE_CPP_Expr & e)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenFctCallObjMemAcc(m, ASTAUX::MkId(L"containsKey"), SEQ<TYPE_CPP_Expr>().ImpAppend(e));
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCallObjMemAcc(m, ASTAUX::MkId(L"DomExists"), SEQ<TYPE_CPP_Expr>().ImpAppend(e));
}

// GenRngExists
// m : CPP`Expr
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenRngExists(const TYPE_CPP_Expr& m, const TYPE_CPP_Expr& e)
{
  return vdm_BC_GenFctCallObjMemAcc(m, ASTAUX::MkId(L"containsValue"), SEQ<TYPE_CPP_Expr>().ImpAppend(e));
}

// GenDeclEmptyMap
// - : [REP`TypeRep]
// name : CPP`Name
// ==> seq1 of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenDeclEmptyMap(const Generic &, const TYPE_CPP_Name & name)
{
  SEQ<TYPE_CPP_Stmt> rb_l;
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    rb_l.ImpAppend(vdm_BC_GenDecl(GenMapType(), name, vdm_BC_GenAsgnInit(GenEmptyMapExpr())));
  }
  else
#endif // VDMPP
    rb_l.ImpAppend(vdm_BC_GenDecl(GenMapType(), name, nil));
  return rb_l;
}

// GenEmptyMapExpr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenEmptyMapExpr()
{
  return GenMapExpr(nil);
}

// GenMapExpr
// ie : [CPP`Expr]
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenMapExpr(const Generic & ie)
{
  SEQ<TYPE_CPP_Expr> args;
  if (!ie.IsNil())
    args.ImpAppend(ie);
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenClassInstanceCreationExpr(GenImplMapType().get_tp(), args);
  }
  else
#endif // VDMPP
    return vdm_BC_GenFctCall(GenImplMapType().get_tp(), args);
}

// GenMapMerge
// vt1 : CGMAIN`VT
// vt2 : CGMAIN`VT
// vt3 : CGMAIN`VT
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenMapMerge(const TYPE_CGMAIN_VT & vt1, const TYPE_CGMAIN_VT & vt2,
                                      const TYPE_CGMAIN_VT & vt3)
{
  const TYPE_CPP_Expr & var1     (vt1.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_CPP_Expr & var2     (vt2.GetRecord(pos_CGMAIN_VT_name));
  const Generic & type1          (vt1.GetRecord(pos_CGMAIN_VT_type));
  const Generic & type2          (vt2.GetRecord(pos_CGMAIN_VT_type));
  const TYPE_CPP_Expr & resVar_v (vt3.GetRecord(pos_CGMAIN_VT_name));

  TYPE_CPP_Stmt rti (vdm_BC_GenBlock(mk_sequence(RunTime(L"Duplicate entries had different values"))));

#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    TYPE_CPP_Identifier com_v (vdm_BC_GiveName(ASTAUX::MkId(L"com")));
    TYPE_CPP_Identifier allApplies_v (vdm_BC_GiveName(ASTAUX::MkId(L"all_applies")));
    TYPE_CPP_Identifier d_v (vdm_BC_GiveName(ASTAUX::MkId(L"d")));
    TYPE_CPP_Identifier m1 (vdm_BC_GiveName(ASTAUX::MkId(L"m1")));
    TYPE_CPP_Identifier m2 (vdm_BC_GiveName(ASTAUX::MkId(L"m2")));

    SetException(true);

    TYPE_CPP_TypeSpecifier inttp (GenSmallBoolType());

    TYPE_CPP_Expr castVar1 (IsMapType(CleanAndFlattenType(type1)) ? var1 : GenCastMap(var1));
    TYPE_CPP_Expr mapclone (GenMapExpr(castVar1));
    TYPE_CPP_Expr castVar2 (IsMapType(CleanAndFlattenType(type2)) ? var2 : GenCastMap(var2));
    TYPE_CPP_Expr eq (GenEqVal(vdm_BC_GenFctCallObjMemAcc(m1, ASTAUX::MkId(L"get"), mk_sequence(d_v)),
                               vdm_BC_GenFctCallObjMemAcc(m2, ASTAUX::MkId(L"get"), mk_sequence(d_v))));
    TYPE_CPP_Stmt as2 (vdm_BC_GenAsgnStmt(allApplies_v, eq));

    SEQ<TYPE_CPP_Stmt> rb;
    rb.ImpConc(GenMapDecl_DS(m1, vdm_BC_GenAsgnInit(mapclone)));
    rb.ImpConc(GenMapDecl_DS(m2, vdm_BC_GenAsgnInit(castVar2)));
    rb.ImpConc(GenDeclSet(com_v, GenDom(m1)));
    rb.ImpAppend(GenInterSectStmt(com_v, GenDom(m2)));
    rb.ImpAppend(vdm_BC_GenDecl(inttp, allApplies_v, vdm_BC_GenAsgnInit( vdm_BC_GenBoolLit(true))));
    rb.ImpConc(GenIterSet(mk_CG_VT(com_v, mk_REP_SetTypeRep(mk_REP_AllTypeRep())), allApplies_v,
                          mk_CG_VT(d_v, mk_REP_AllTypeRep()), mk_sequence(as2)));
    rb.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenNot(allApplies_v), rti, nil));
    rb.ImpAppend(GenMapOverride(m1, m2));
    rb.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, m1));
    return rb;
  }
  else
#endif // VDMPP
  { // C++
    TYPE_CPP_Identifier m1 (vdm_BC_GiveName(ASTAUX::MkId(L"m1")));
    TYPE_CPP_Identifier m2 (vdm_BC_GiveName(ASTAUX::MkId(L"m2")));
    TYPE_CPP_Expr cond (vdm_BC_GenNot(vdm_BC_GenFctCallObjMemAcc(m1, ASTAUX::MkId(L"IsCompatible"),
                                                                 mk_sequence(m2))));

    SEQ<TYPE_CPP_Stmt> rb;
    rb.ImpAppend( vdm_BC_GenDecl( GenMapType(), m1, vdm_BC_GenObjectInit(mk_sequence(var1)) ) );
    rb.ImpAppend( vdm_BC_GenDecl( GenMapType(), m2, vdm_BC_GenObjectInit(mk_sequence(var2)) ) );
    rb.ImpAppend( vdm_BC_GenIfStmt(cond, rti, nil));
    rb.ImpAppend( GenMapOverride(m1, m2) );
    rb.ImpAppend( vdm_BC_GenAsgnStmt(resVar_v, m1) );
    return rb;
  }
}

// GenMapRestToBy
// vt1 : CGMAIN`VT
// vt2 : CGMAIN`VT
// opr : AS`BinaryOp
// ==> CPP`Expr | seq of CPP`Stmt
Generic vdmcg::GenMapRestToBy(const TYPE_CGMAIN_VT & vt1, const TYPE_CGMAIN_VT & vt2,
                              const TYPE_CGMAIN_VT & vt3, int opr)
{
  const TYPE_CPP_Expr & s (vt1.GetRecord(pos_CGMAIN_VT_name));
  const Generic & stp     (vt1.GetField(pos_CGMAIN_VT_type));
  const TYPE_CPP_Expr & m (vt2.GetRecord(pos_CGMAIN_VT_name));
  const Generic & mtp     (vt2.GetField(pos_CGMAIN_VT_type));
  const TYPE_CPP_Expr & resVar_v (vt3.GetRecord(pos_CGMAIN_VT_name));

#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    TYPE_CPP_Identifier tmpMap_v (vdm_BC_GiveName(ASTAUX::MkId(L"tmpMap")));
    TYPE_CPP_Identifier domM_v (vdm_BC_GiveName(ASTAUX::MkId(L"domM")));
    TYPE_CPP_Identifier elm_v (vdm_BC_GiveName(ASTAUX::MkId(L"elm")));

    TYPE_CPP_Expr ss (IsSetType(stp) ? s : vdm_BC_GenCastExpr(GenSetType(), s));
    TYPE_CPP_Expr mm (IsMapType(mtp) ? m : vdm_BC_GenCastExpr(GenMapType(), m));

    SEQ<TYPE_CPP_Stmt> rb_l;
    rb_l.ImpConc(GenDeclEmptyMap(nil, tmpMap_v));
    rb_l.ImpConc(GenDeclSet(domM_v, GenDom(mm)));
    rb_l.ImpConc(GenIterSet(mk_CG_VT(domM_v, mk_REP_SetTypeRep(mk_REP_AllTypeRep())), nil,
                            mk_CG_VT(elm_v, mk_REP_AllTypeRep()),
                            mk_sequence(GenMapRestrictStmt(ss, mm, elm_v, tmpMap_v, opr))));
    rb_l.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, tmpMap_v));
    return rb_l;
  }
  else
#endif // VDMPP
  { // C++
    TYPE_CPP_Expr ss (IsSetType(stp) ? s : GenCastSetType(s));
    TYPE_CPP_Expr mm (IsMapType(mtp) ? m : GenCastMap(m));

    switch (opr) {
      case MAPDOMRESTTO: {
        return vdm_BC_GenFctCallObjMemAcc(mm, ASTAUX::MkId(L"DomRestrictedTo"), mk_sequence(ss));
      }
      case MAPDOMRESTBY: {
        return vdm_BC_GenFctCallObjMemAcc(mm, ASTAUX::MkId(L"DomRestrictedBy"), mk_sequence(ss));
      }
      case MAPRNGRESTTO: {
        return vdm_BC_GenFctCallObjMemAcc(mm, ASTAUX::MkId(L"RngRestrictedTo"), mk_sequence(ss));
      }
      case MAPRNGRESTBY: {
        return vdm_BC_GenFctCallObjMemAcc(mm, ASTAUX::MkId(L"RngRestrictedBy"), mk_sequence(ss));
      }
      default: {
        return TYPE_CPP_Expr();
      }
    }
  }
}

#ifdef VDMPP
// GenMapRestrictStmt
// s : CPP`Expr
// m : CPP`Expr
// elm : CPP`Expr
// tm : CPP`Expr
// opr : AS`BinaryOp
// ==> CPP`Stmt
TYPE_CPP_Stmt vdmcg::GenMapRestrictStmt(const TYPE_CPP_Expr & s,
                                        const TYPE_CPP_Expr & m,
                                        const TYPE_CPP_Expr & elm,
                                        const TYPE_CPP_Expr & tm, int opr)
{
  TYPE_CPP_Expr mp (GenMapApply(mk_CG_VT(m, Nil()), elm));
  TYPE_CPP_Stmt stmt (GenMapInsert(tm, elm, mp));

  TYPE_CPP_Expr fcall1 (vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"UTIL.Contains")), mk_sequence(s, elm)));
  TYPE_CPP_Expr fcall2 (vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"UTIL.Contains")), mk_sequence(s, mp)));

  TYPE_CPP_Expr cond;
  switch (opr) {
    case MAPDOMRESTTO: { cond = fcall1; break; }
    case MAPDOMRESTBY: { cond = vdm_BC_GenNot(fcall1); break; }
    case MAPRNGRESTTO: { cond = fcall2; break; }
    case MAPRNGRESTBY: { cond = vdm_BC_GenNot(fcall2); break; }
  }
  SetException(true);
  return vdm_BC_GenIfStmt(cond, stmt, Nil());
}
#endif // VDMPP

// GenComposeExpr
// vt1 : CGMAIN`VT
// vt2 : CGMAIN`VT
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenComposeExpr(const TYPE_CGMAIN_VT & vt1,
                                         const TYPE_CGMAIN_VT & vt2,
                                         const TYPE_CGMAIN_VT & resVT)
{
  const TYPE_CPP_Expr & v1  (vt1.GetRecord(pos_CGMAIN_VT_name));
  const Generic & t1        (vt1.GetField(pos_CGMAIN_VT_type));
  const TYPE_CPP_Expr & v2  (vt2.GetRecord(pos_CGMAIN_VT_name));
  const Generic & t2        (vt2.GetField(pos_CGMAIN_VT_type));
  const TYPE_CPP_Expr & res (resVT.GetRecord(pos_CGMAIN_VT_name));

  TYPE_CPP_Identifier tmpMap (vdm_BC_GiveName(ASTAUX::MkId(L"tmpMap")));
  TYPE_CPP_Stmt rti1 (vdm_BC_GenBlock(mk_sequence(RunTime(L"Two maps were expected in composition expression"))));
//  TYPE_CPP_TypeSpecifier mtp (GenMapType());

  SEQ<TYPE_CPP_Stmt> rb_l;
  rb_l.ImpConc(GenDeclEmptyMap(nil, tmpMap));

  TYPE_CPP_Expr tmpMap1 (v1);
  if (! IsMapType(t1))
  {
    tmpMap1 = vdm_BC_GiveName(ASTAUX::MkId(L"tmpMap1"));
    TYPE_CPP_Expr cond (GenIsMap(v1));

    TYPE_CPP_Stmt asgn;
#ifdef VDMPP
    if (vdm_CPP_isJAVA()) {
      asgn = vdm_BC_GenAsgnStmt(tmpMap1, GenCastMap(v1));
    }
    else
#endif // VDMPP
      asgn = vdm_BC_GenAsgnStmt(tmpMap1, v1);

    rb_l.ImpConc(GenDeclEmptyMap(nil, tmpMap1));
    rb_l.ImpAppend(vdm_BC_GenIfStmt(vdm_BC_GenNot(cond), rti1, nil));
    rb_l.ImpAppend(asgn);
  }

  TYPE_CPP_Expr tmpMap2 (v2);
  if (! IsMapType(t2))
  {
    tmpMap2 = vdm_BC_GiveName(ASTAUX::MkId(L"tmpMap2"));

    TYPE_CPP_Stmt asgn;
#ifdef VDMPP
    if (vdm_CPP_isJAVA()) {
      asgn = vdm_BC_GenAsgnStmt(tmpMap2, GenCastMap(v2));
    }
    else
#endif // VDMPP
      asgn = vdm_BC_GenAsgnStmt(tmpMap2, v2);

    rb_l.ImpConc(GenDeclEmptyMap(nil, tmpMap2));
    rb_l.ImpAppend(vdm_BC_GenIfStmt(GenIsMap(v2), asgn, rti1));
  }

  TYPE_CPP_Identifier key (vdm_BC_GiveName(ASTAUX::MkId(L"key")));
  TYPE_CPP_Identifier elem (vdm_BC_GiveName(ASTAUX::MkId(L"elem")));
  TYPE_CPP_Expr cond (vdm_BC_GenNot(GenDomExists(tmpMap1, elem)));
  TYPE_CPP_Stmt st1 (GenMapImpModify(tmpMap, key, GenMapApply(mk_CG_VT(tmpMap1, nil), elem)));
  TYPE_CPP_Stmt rti (vdm_BC_GenBlock(mk_sequence(RunTime(L"The range is not a subset of the domain in composition expression"))));
  TYPE_CPP_Stmt ifstmt (vdm_BC_GenIfStmt(cond, rti, nil));

  //TYPE_CPP_Stmt stmt (vdm_BC_GenBlock(type_dL().ImpAppend(ifstmt).ImpAppend(st1)));
  TYPE_CPP_Stmt stmt (vdm_BC_GenBlock(mk_sequence(ifstmt, st1)));
  rb_l.ImpConc(GenIterMap(mk_CG_VT(tmpMap2, t2), nil, key, elem, stmt));
  rb_l.ImpAppend(vdm_BC_GenAsgnStmt(res, tmpMap));
  return rb_l;
}

// GenMapIteration
// vt1 : CGMAIN`VT
// vt2 : CGMAIN`VT
// resVT : CGMAIN`VT
// ==> seq1 of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenMapIteration(const TYPE_CGMAIN_VT & vt1,
                                          const TYPE_CGMAIN_VT & vt2,
                                          const TYPE_CGMAIN_VT & resVT)
{
  const TYPE_CPP_Expr & v1     (vt1.GetRecord(pos_CGMAIN_VT_name));
  const Generic & t1           (vt1.GetField(pos_CGMAIN_VT_type));
  const TYPE_CPP_Expr & v2     (vt2.GetRecord(pos_CGMAIN_VT_name));
  const Generic & t2           (vt2.GetField(pos_CGMAIN_VT_type));
  const TYPE_CPP_Expr & resVar (resVT.GetRecord(pos_CGMAIN_VT_name));

  SEQ<TYPE_CPP_Stmt> rb_l;
  TYPE_CPP_Identifier tmpMap (vdm_BC_GiveName (ASTAUX::MkId(L"tmpMap")));
  TYPE_CPP_Identifier n (vdm_BC_GiveName(ASTAUX::MkId(L"n")));

#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    TYPE_CPP_Expr cast (IsMapType(CleanAndFlattenType(t1)) ? v1 : GenCastMap(v1));
    TYPE_CPP_Expr mapclone (GenMapExpr(cast));
    rb_l.ImpConc(GenMapDecl_DS(tmpMap, vdm_BC_GenAsgnInit(mapclone)));
  }
  else
#endif //VDMPP
    rb_l.ImpConc(GenMapDecl_DS(tmpMap, vdm_BC_GenAsgnInit(v1)));

  if (IsIntType(t2))
  {
    rb_l.ImpAppend(vdm_BC_GenDecl(GenSmallNumType(), n, vdm_BC_GenAsgnInit(GenGetValue(v2, t2))));
  }
  else
  {
    TYPE_CPP_Expr cond (GenIsInt(v2));
    TYPE_CPP_Stmt rti (vdm_BC_GenBlock(mk_sequence(RunTime(L"A 'nat' was expected in map iteration expression"))));
    TYPE_CPP_Stmt ifstmt (vdm_BC_GenIfStmt(vdm_BC_GenNot(cond), rti, nil));

    TYPE_CPP_Expr val (GenGetValue(vdm_BC_GenCastExpr(GenIntType(), v2), mk_REP_NumericTypeRep(Int(INTEGER)))) ;

    rb_l.ImpAppend(ifstmt);
    rb_l.ImpAppend(vdm_BC_GenDecl(GenSmallNumType(), n, vdm_BC_GenAsgnInit(val)));
  }

  rb_l.ImpConc(GenMapIterIfPart(n, tmpMap, t1));
  rb_l.ImpAppend(vdm_BC_GenAsgnStmt(resVar, tmpMap));
  return rb_l;
}

// GenMapIterIfPart
// n : CPP`Name
// tmpMap : CPP`Name
// t1 : [REP`TypeRep]
// ==> seq1 of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenMapIterIfPart(const TYPE_CPP_Name & n, const TYPE_CPP_Name & tmpMap, const Generic & t1)
{
  TYPE_CPP_IntegerLit il (vdm_BC_GenIntegerLit(0));
  TYPE_CPP_Identifier mm (vdm_BC_GiveName (ASTAUX::MkId(L"mm")));
  TYPE_CPP_Identifier mm2 (vdm_BC_GiveName (ASTAUX::MkId(L"mm2")));

  TYPE_CPP_Expr c1 (vdm_BC_GenLt(n, il));
  TYPE_CPP_Expr c2 (vdm_BC_GenEq(n, il));
  TYPE_CPP_Identifier key (vdm_BC_GiveName(ASTAUX::MkId(L"key")));
  TYPE_CPP_Identifier elem (vdm_BC_GiveName(ASTAUX::MkId(L"elem")));
  TYPE_CPP_Identifier count (vdm_BC_GiveName(ASTAUX::MkId(L"count")));
  TYPE_CPP_AsgnInit ai (vdm_BC_GenAsgnInit (vdm_BC_GenIntegerLit (1)));
  TYPE_CPP_Expr mapp (GenMapApply (mk_CG_VT (mm2, nil), elem));
  TYPE_CPP_Stmt st1 (GenMapImpModify(tmpMap, key, key));
  TYPE_CPP_Stmt st2 (GenMapImpModify(tmpMap, key, mapp));
  TYPE_CPP_DeclarationStmt idcl (vdm_BC_GenDecl(GenSmallNumType(), count, ai));
  TYPE_CPP_Expr stop (vdm_BC_GenLt(count, n));

  TYPE_CGMAIN_VT vt2 (mk_CG_VT(mm, t1));

  type_dL inc;
  inc.ImpAppend(vdm_BC_GenPostPlusPlus(count));

  TYPE_CPP_AsgnInit ai_p;
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    ai_p = vdm_BC_GenAsgnInit(GenMapExpr(tmpMap));
  }
  else
#endif //VDMPP
    ai_p = vdm_BC_GenAsgnInit(tmpMap);

  TYPE_CPP_Stmt st1_p (vdm_BC_GenBlock(GenIterMap(vt2, nil, key, nil, st1)));
  SEQ<TYPE_CPP_Stmt> dm (GenMapDecl_DS (mm, ai_p));
  SEQ<TYPE_CPP_Stmt> dm2 (GenMapDecl_DS (mm2, ai_p));

  SEQ<TYPE_CPP_Stmt> st2_l;
  st2_l.ImpConc(dm2);
  st2_l.ImpConc(GenIterMap(vt2, nil, key, elem, st2));

  TYPE_CPP_Stmt rti (vdm_BC_GenBlock(mk_sequence(RunTime(L"A 'nat' was expected in map iteration expression"))));
  TYPE_CPP_Stmt ifstmt (vdm_BC_GenIfStmt(c1, rti, nil));

  TYPE_CPP_Stmt forstmt (vdm_BC_GenForStmt (idcl, stop, inc, vdm_BC_GenBlock (st2_l)));

  TYPE_CPP_Stmt estmt (vdm_BC_GenBlock(type_dL().ImpAppend(forstmt)));

  TYPE_CPP_Stmt ifstmt2 (vdm_BC_GenIfStmt(c2, st1_p, estmt));

  SEQ<TYPE_CPP_Stmt> s_l;
  s_l.ImpConc(dm).ImpAppend(ifstmt).ImpAppend(ifstmt2);
  return s_l;
}

// GenIterMap
// vt : CGMAIN`VT
// cond : [CPP`Name]
// key : [CPP`Name]
// elem : [CPP`Expr]
// stmt : CPP`Stmt
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenIterMap(const TYPE_CGMAIN_VT & vt,
                                     const Generic & cond,
                                     const Generic & key,
                                     const Generic & elem,
                                     const TYPE_CPP_Stmt & stmt)
{
  const TYPE_CPP_Expr & m (vt.GetRecord(pos_CGMAIN_VT_name));
  const Generic & tp (vt.GetField(pos_CGMAIN_VT_type));

  TYPE_CPP_Identifier tmpe_v (vdm_BC_GiveName(ASTAUX::MkId(L"tmpe")));
  TYPE_CPP_Identifier tmpMap_v (vdm_BC_GiveName(ASTAUX::MkId(L"tmpMap")));

  Tuple fmdr (FindMapDomRng(tp));
  const Generic & dtp (fmdr.GetField(1)); // [REP`TypeRep]
  const Generic & rtp (fmdr.GetField(2)); // [REP`TypeRep]

#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    TYPE_CPP_Identifier bb_v (vdm_BC_GiveName (ASTAUX::MkId(L"enm")));
    TYPE_CPP_Identifier untyped_key (vdm_BC_GiveName(ASTAUX::MkId(L"dom")));

    TYPE_CPP_AsgnInit ai (vdm_BC_GenAsgnInit(vdm_BC_GenFctCallObjMemAcc(
                              vdm_BC_GenFctCallObjMemAcc(tmpMap_v, ASTAUX::MkId(L"keySet"), SEQ<TYPE_CPP_Expr>()),
                              ASTAUX::MkId(L"iterator"),
                              SEQ<TYPE_CPP_Expr>())));
    TYPE_CPP_DeclarationStmt decl (vdm_BC_GenDecl(vdm_BC_GenTypeSpecifier(
                                            vdm_BC_GenIdentifier(ASTAUX::MkId(L"Iterator"))), bb_v, ai));

    InsertImport(SEQ<Char>(L"java.util.Iterator"));
    TYPE_CPP_Expr temp (vdm_BC_GenFctCallObjMemAcc(bb_v, ASTAUX::MkId(L"hasNext"), SEQ<TYPE_CPP_Expr>()));

    TYPE_CPP_Expr expr1 (cond.IsNil() ? temp : vdm_BC_GenLogAnd(temp, cond));

    TYPE_CPP_Expr fcall (vdm_BC_GenFctCallObjMemAcc(bb_v, ASTAUX::MkId(L"next"), SEQ<TYPE_CPP_Expr>()));

    SEQ<TYPE_CPP_Stmt> stmt_l;
    stmt_l.ImpConc(GenDecl_DS(nil, untyped_key, vdm_BC_GenAsgnInit(fcall)));
    if (! key.IsNil())
      stmt_l.ImpConc(GenDecl_DS(dtp, key, vdm_BC_GenAsgnInit(GenExplicitCast(dtp, untyped_key, nil))));
    if (! elem.IsNil()) {
      TYPE_CPP_Expr el (GenMapApply(mk_CG_VT(tmpMap_v, nil), untyped_key));
      stmt_l.ImpConc(GenDecl_DS(rtp, elem, vdm_BC_GenAsgnInit(GenExplicitCast(rtp,el,nil))));
    }
    stmt_l.ImpAppend(stmt);

    SEQ<TYPE_CPP_Stmt> rb_l;
    rb_l.ImpConc(GenDeclMap(tmpMap_v, m));
    rb_l.ImpAppend(vdm_BC_GenForStmt(decl, expr1, type_dL(), vdm_BC_GenBlock(stmt_l)));
    return rb_l;
  }
  else
#endif //VDMPP
  {
    TYPE_CPP_Expr bb_v (vdm_BC_GiveName (ASTAUX::MkId(L"bb")));

    TYPE_CPP_Expr expr1 (cond.IsNil() ? bb_v : vdm_BC_GenLogAnd(bb_v, cond));

    SEQ<TYPE_CPP_Expr> tmpe_vl;
    tmpe_vl.ImpAppend(tmpe_v);
    TYPE_CPP_Expr fcall1(vdm_BC_GenFctCallObjMemAcc(tmpMap_v, ASTAUX::MkId(L"First"), tmpe_vl));
    TYPE_CPP_Expr fcall2(vdm_BC_GenFctCallObjMemAcc(tmpMap_v, ASTAUX::MkId(L"Next"), tmpe_vl));
    TYPE_CPP_DeclarationStmt i (vdm_BC_GenDecl(GenSmallBoolType(), bb_v, vdm_BC_GenAsgnInit(fcall1)));
    TYPE_CPP_Expr e2(vdm_BC_GenAsgnExpr(bb_v, fcall2));

    SEQ<TYPE_CPP_Stmt> stmt_l;
    if (! key.IsNil())
      //stmt_l.ImpConc(GenDecl_DS(dtp, key, vdm_BC_GenAsgnInit(tmpe_v)));
      stmt_l.ImpConc(GenDecl_DS(dtp, key, vdm_BC_GenObjectInit(mk_sequence(tmpe_v))));

    if (! elem.IsNil()) {
      TYPE_CPP_Expr el (GenMapApply(mk_CG_VT(tmpMap_v, nil), key));
      //stmt_l.ImpConc(GenDecl_DS(rtp, elem, vdm_BC_GenAsgnInit(el)));
      stmt_l.ImpConc(GenDecl_DS(rtp, elem, vdm_BC_GenObjectInit(mk_sequence(el))));
    }
    stmt_l.ImpAppend(stmt);

    SEQ<TYPE_CPP_Stmt> rb_l;
    rb_l.ImpConc(GenDeclMap(tmpMap_v, m));
    rb_l.ImpAppend(vdm_BC_GenDecl(vdm_BC_GenGeneric(), tmpe_v, nil));
    rb_l.ImpAppend(vdm_BC_GenForStmt(i, expr1, mk_sequence(e2), vdm_BC_GenBlock(stmt_l)));
    return rb_l;
  }
}

// FindMapDomRng
// tp : REP`TypeRep
// ==> [REP`TypeRep] * [REP`TypeRep]
Tuple vdmcg::FindMapDomRng(const TYPE_REP_TypeRep & tp)
{
  switch (tp.GetTag()) {
    case TAG_TYPE_REP_InvTypeRep: {
      return FindMapDomRng(tp.GetRecord(pos_REP_InvTypeRep_shape));
    }
    case TAG_TYPE_REP_GeneralMapTypeRep: {
      return mk_(CleanFlatType(tp.GetRecord(pos_REP_GeneralMapTypeRep_mapdom)),
                 CleanFlatType(tp.GetRecord(pos_REP_GeneralMapTypeRep_maprng)));
      break;
    }
    case TAG_TYPE_REP_InjectiveMapTypeRep: {
      return mk_(CleanFlatType(tp.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom)),
                 CleanFlatType(tp.GetRecord(pos_REP_InjectiveMapTypeRep_maprng)));
      break;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tps (tp.GetSet(pos_REP_UnionTypeRep_tps));
      MAP<TYPE_REP_TypeRep, TYPE_REP_TypeRep> mtps;
      Generic tp;
      for (bool bb = tps.First(tp); bb; bb = tps.Next(tp)) {
        TYPE_REP_TypeRep t (CleanFlatType(tp));
        if (IsMapType(t)) {
          mtps.ImpModify(tp, t);
        }
      }
      switch(mtps.Dom().Card()) {
        case 0: {
          ReportError(L"FindMapDomRng");
          return mk_(Nil(), Nil());
        }
        case 1:
          return FindMapDomRng(mtps.Rng().GetElem());
        default:
          return mk_(Nil(), Nil());
      }
    }
    default:
      return mk_(Nil(), Nil());
      break;
  }
}

// GenNullExpr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenNullExpr()
{
  return vdm_BC_GenIdentifier(ASTAUX::MkId(L"null"));
}

// GenSuper
// ==> CPP`Identifier
TYPE_CPP_Identifier vdmcg::GenSuper()
{
  SetUsesSuper();
  return vdm_BC_GenIdentifier(ASTAUX::MkId(L"super"));
}

// GenThis
// ==> CPP`Identifier
TYPE_CPP_Identifier vdmcg::GenThis()
{
  if (IsStatic())
    return vdm_BC_GenIdentifier(GiveCurCName());
  else {
    SetUsesThis();
    return vdm_BC_GenIdentifier(ASTAUX::MkId(L"this"));
  }
}

// GenEqVal
// e1 : CPP`Expr
// e2 : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenEqVal(const TYPE_CPP_Expr & e1, const TYPE_CPP_Expr & e2)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"UTIL.equals")), mk_sequence(e1, e2));
  }
  else
#endif //VDMPP
    return vdm_BC_GenEq(e1, e2);
}

// GenNeqVal
// e1 : CPP`Expr
// e2 : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenNeqVal(const TYPE_CPP_Expr & e1, const TYPE_CPP_Expr & e2)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenNot(vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"UTIL.equals")), mk_sequence(e1, e2)));
  }
  else
#endif //VDMPP
    return vdm_BC_GenNeq(e1, e2);
}

#ifdef VDMPP
// GenIsClass
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenIsClass(const TYPE_CPP_Expr & e)
{
  TYPE_AS_Id classname (ASTAUX::MkId(L"IsObjectRef"));
  return vdm_BC_GenFctCallObjMemAcc(e, classname, SEQ<TYPE_CPP_Expr>());
}

// GenIsThisClass
// nm : AS`Name
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenIsThisClass(const TYPE_AS_Name & nm, const TYPE_CPP_Expr & e)
{
  TYPE_CPP_TypeSpecifier tp (vdm_BC_GenTypeSpecifier(vdm_BC_Rename(nm)));
  return vdm_BC_GenBracketedExpr(vdm_BC_GenTypeComp(tp, e));
}

// GenIsClasses
// nm_s : set of AS`Name
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenIsClasses(const SET<TYPE_AS_Name> & nm_s, const TYPE_CPP_Expr & e)
{
  TYPE_CPP_Expr expr;
  bool test(true);
  SET<TYPE_AS_Name> nm_s_q (nm_s);
  Generic nm;
  for (bool bb = nm_s_q.First(nm); bb; bb = nm_s_q.Next(nm))
  {
    TYPE_CPP_TypeSpecifier tp (vdm_BC_GenTypeSpecifier(vdm_BC_Rename(nm)));
    TYPE_CPP_Expr expr_c (vdm_BC_GenBracketedExpr(vdm_BC_GenTypeComp(tp, e)));

    if (test)
      expr = expr_c;
    else
      expr = vdm_BC_GenLogOr (expr, expr_c);
    test = false;
  }
  return expr;
}

// GenMyClass
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenMyClass(const TYPE_CPP_Expr & e)
{
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"getClass"), SEQ<TYPE_CPP_Expr>());
  }
  else {
    return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"MyObjectId"), SEQ<TYPE_CPP_Expr>());
  }
}

// GenMyBaseClass
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenMyBaseClass(const TYPE_CPP_Expr & e)
{
  return vdm_BC_GenFctCallObjMemAcc(e, ASTAUX::MkId(L"getBaseClass"), SEQ<TYPE_CPP_Expr>());
}

// GenObjRefType_DS
// nm : [AS`Name]
// ==> CPP`DeclSpecifier
TYPE_CPP_TypeSpecifier vdmcg::GenObjRefType_DS(const Generic & nm)
{
  if (vdm_CPP_isJAVA()) {
    if (!nm.IsNil()) {
      return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(GiveLastName(nm)));
    }
    else {
      return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Object")) );
    }
  }
  else { // C++
    return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"ObjectRef")));
  }
}

// GenObjRefDecl
// nm : [AS`Name]
// obj : CPP`Name
// initExpr : [CPP`Initializer]
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenObjRefDecl(const Generic & nm, const TYPE_CPP_Name & obj,
                                        const Generic & initExpr)
{
  if (vdm_CPP_isJAVA()) {
    SEQ<TYPE_CPP_Stmt> rb_l;
    TYPE_CPP_TypeSpecifier cname (GenObjRefType_DS(nm));
    if (initExpr.IsNil()) {
      rb_l.ImpAppend(vdm_BC_GenDecl(cname, obj, vdm_BC_GenAsgnInit(GenNullExpr())));
    }
    else {
      rb_l.ImpAppend(vdm_BC_GenDecl(cname, obj, initExpr));
    }
    return rb_l;
  }
  else {
    TYPE_CPP_TypeSpecifier ds (GenObjRefType_DS(nm));
    SEQ<TYPE_CPP_Stmt> rb_l;
    rb_l.ImpAppend(vdm_BC_GenDecl(ds, obj, initExpr));
    return rb_l;
  }
}

// CallDefaultConstructor
// nm : AS`Name
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::CallDefaultConstructor(const TYPE_AS_Name& nm)
{
  TYPE_CPP_TypeSpecifier cls(vdm_BC_GenTypeSpecifier(vdm_BC_Rename(nm)));
  TYPE_CPP_Expr alloc (vdm_BC_GenNewExpr(cls, SEQ<TYPE_CPP_Expr>()));
  if (vdm_CPP_isJAVA()) {
    return alloc;
  }
  else {
    TYPE_CPP_Identifier ds (Id2CppGTpId(GenObjRefType(mk_REP_ObjRefTypeRep(nm), Set())));
    IncludeClass(nm);
    return vdm_BC_GenFctCall(ds, mk_sequence(alloc));
  }
}

// CallConstructor
// nm : AS`Name
// p_argL : CPP`Exprs
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::CallConstructor(const TYPE_AS_Name & nm, const SEQ<TYPE_CPP_Expr> & p_argL)
{
  TYPE_CPP_TypeSpecifier cls (vdm_BC_GenTypeSpecifier(vdm_BC_Rename(nm)));
  TYPE_CPP_Expr alloc (vdm_BC_GenNewExpr(cls, p_argL));

  if (vdm_CPP_isJAVA()) {
    return alloc;
  }
  else {
    TYPE_CPP_Identifier ds (Id2CppGTpId(GenObjRefType(mk_REP_ObjRefTypeRep(nm), Set())));
    IncludeClass(nm);
    return vdm_BC_GenFctCall(ds, mk_sequence(alloc));
  }
}

// CastToClassPtr
// cls : AS`Name
// expr : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::CastToClassPtr(const Generic & cls, const TYPE_CPP_Expr & expr)
{
  if (! cls.IsNil()) {
    if (vdm_CPP_isJAVA()) {
      IncludeClass(cls);
      return vdm_BC_GenCastExpr( GenObjRefType_DS(cls), expr );
    }
    else {
      if (cls == GiveCurCASName() &&
          ((expr.Is(TAG_TYPE_CPP_FctCall) &&
            (expr.GetRecord(pos_CPP_FctCall_fct) == vdm_BC_GenIdentifier(ASTAUX::MkId(L"Self")))) ||
           (expr == vdm_BC_GenIdentifier(ASTAUX::MkId(L"this"))))) {
        return GenThis();
      }
      TYPE_CPP_Identifier id (vdm_BC_Rename(cls));
      TYPE_CPP_Identifier clid (vdm_BC_PrefixName(ASTAUX::MkId(L"ObjGet"), id));
      TYPE_CPP_Expr fct (vdm_BC_GenFctCall (clid, mk_sequence(expr)));
      IncludeClass(cls);
      return fct;
    }
  }
  else {
    return RunTime(L"Cannot get Object reference from nil");
  }
}

// GenIsOfClass
// cls : AS`Name
// obj : CPP`Expr
// res : CPP`Name
// ==> CPP`Stmt
TYPE_CPP_Stmt vdmcg::GenIsOfClass(const TYPE_AS_Name& cls, const TYPE_CPP_Expr & obj,
                                  const TYPE_CPP_Name & res)
{
  TYPE_CPP_Expr fcall (GenIsOfClassExpr(cls, obj));
  return vdm_BC_GenAsgnStmt(res, fcall);
}

// GenIsOfClassExpr
// cls : AS`Name
// obj : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenIsOfClassExpr(const TYPE_AS_Name& cls, const TYPE_CPP_Expr & obj)
{
  IncludeClass(cls);
  if (vdm_CPP_isJAVA()) {
    TYPE_CPP_TypeSpecifier mc (vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier((GiveLastName(cls)))));
    TYPE_CPP_Expr fcall (vdm_BC_GenTypeComp(mc, obj));
    return GenBoolExpr(fcall);
  }
  else {
    SEQ<TYPE_CPP_Expr> mc_l;
    mc_l.ImpAppend(vdm_BC_GiveMacroName(cls));
    TYPE_AS_Id fct (ASTAUX::MkId(L"IsOfClass"));
    TYPE_CPP_Expr fcall(vdm_BC_GenFctCallObjMemAcc(obj, fct, mc_l));
    return fcall;
  }
}

// GenIsOfBaseClass
// cls : AS`Name
// obj : CPP`Expr
// res : CPP`Name
// ==> CPP`Stmt
TYPE_CPP_Stmt vdmcg::GenIsOfBaseClass(const TYPE_AS_Name& cls, const TYPE_CPP_Expr & obj, const TYPE_CPP_Name & res)
{
  TYPE_CPP_Expr fcall (GenIsOfBaseClassExpr(cls, obj));
  return vdm_BC_GenAsgnStmt(res, fcall);
}

// GenIsOfBaseClassExpr
// cls : AS`Name
// obj : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenIsOfBaseClassExpr(const TYPE_AS_Name & cls, const TYPE_CPP_Expr & obj)
{
  IncludeClass(cls);
  if (vdm_CPP_isJAVA()) {
    TYPE_CPP_Identifier mc (vdm_BC_GenIdentifier((GiveLastName(cls))));
    TYPE_CPP_Expr expr1 (vdm_BC_GenTypeComp(vdm_BC_GenTypeSpecifier(mc), obj));
    TYPE_CPP_Expr expr2 (vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"UTIL.isbaseclass")),
          SEQ<TYPE_CPP_Expr>().ImpAppend(vdm_BC_GenQualifiedName(mc, vdm_BC_GenIdentifier(ASTAUX::MkId(L"class"))))));
    TYPE_CPP_Expr fcall (vdm_BC_GenLogAnd(expr1, expr2));
    SEQ<TYPE_CPP_Expr> res;
    return GenBoolExpr(fcall);
  }
  else {
    SEQ<TYPE_CPP_Expr>  mc_l;
    mc_l.ImpAppend(vdm_BC_GiveMacroName(cls));
    TYPE_AS_Id fct(ASTAUX::MkId(L"IsOfBaseClass"));
    TYPE_CPP_Expr fcall(vdm_BC_GenFctCallObjMemAcc(obj, fct, mc_l));
    return fcall;
  }
}

// GenSameBaseClass
// obj1 : CPP`Expr
// obj2 : CPP`Expr
// res : CPP`Name
// areObjectRefs : bool
// ==> CPP`Stmt
TYPE_CPP_Stmt vdmcg::GenSameBaseClass(const TYPE_CPP_Expr & obj1,
                                      const TYPE_CPP_Expr & obj2,
                                      const TYPE_CPP_Name & res,
                                      const Bool & areObjectRefs)
{
  TYPE_CPP_Expr fcall(GenSameBaseClassExpr(obj1, obj2, areObjectRefs));
  return vdm_BC_GenAsgnStmt(res, fcall);
}

// GenSameBaseClassExpr
// obj1 : CPP`Expr
// obj2 : CPP`Expr
// areObjRefs : bool
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenSameBaseClassExpr(const TYPE_CPP_Expr & obj1,
                                          const TYPE_CPP_Expr & obj2,
                                          const Bool & areObjRefs)
{
  if (vdm_CPP_isJAVA()) {
    TYPE_CPP_Expr expr1(GenMyBaseClass(obj1));
    TYPE_CPP_Expr expr2(GenMyBaseClass(obj2));
    SEQ<TYPE_CPP_Expr> res;
    res.ImpAppend(obj1);
    res.ImpAppend(obj2);

    TYPE_CPP_Expr fcall (vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"UTIL.samebaseclass")), res));
    return GenBoolExpr(fcall);
  }
  else {
    TYPE_AS_Id fct(ASTAUX::MkId(L"SameBaseClass"));
    SEQ<TYPE_CPP_Expr> obj2_l;
    obj2_l.ImpAppend(obj2);
    TYPE_CPP_Expr fcall (vdm_BC_GenFctCallObjMemAcc(obj1, fct, obj2_l));
    return fcall;
  }
}

// GenInvokeExpr
// ort : REP`ObjRefTypeRep
// obj : CPP`Expr
// mthd : CPP`Name
// parms : CPP`Exprs
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenInvokeExpr(const TYPE_REP_ObjRefTypeRep & ort,
                                   const TYPE_CPP_Expr & obj,
                                   const TYPE_CPP_Name & mthd,
                                   const SEQ<TYPE_CPP_Expr> & parms)
{
  if (vdm_CPP_isJAVA()) {
    TYPE_CPP_Expr fct(vdm_BC_GenObjectMemberAccess(obj, mthd));
    return vdm_BC_GenFctCall(fct, parms);
  }
  else {
    const TYPE_AS_Name & nm (ort.GetRecord(pos_REP_ObjRefTypeRep_nm));
    TYPE_CPP_Identifier cnm (vdm_BC_Rename(nm));
    TYPE_CPP_Identifier clid (vdm_BC_PrefixName (ASTAUX::MkId(L"ObjGet"), cnm));
    TYPE_CPP_Expr fct (vdm_BC_GenFctCall (clid, mk_sequence(obj)));
    return vdm_BC_GenGenericFctCallPtrToObjMemAcc(fct, mthd, parms);
  }
}

// GenCGBase
// cl_nms : set of AS`Name
// ==> set of CPP`File
SET<TYPE_CPP_File> vdmcg::GenCGBase(const SET<TYPE_AS_Name> & cl_nms)
{
  TYPE_CPP_CPPAS cppas;

  SEQ<TYPE_CPP_Header> pp;
  pp.ImpAppend(vdm_BC_GenInclusion(ASTAUX::MkId(L"cg.h")) );

  SEQ<TYPE_CPP_Header> ppcc;
  ppcc.ImpAppend(vdm_BC_GenInclusion (ASTAUX::MkId(L"metaiv.h")));

  TYPE_AS_Id basenameSeq ((Generic)vdm_BC_CGBasename);
  basenameSeq.ImpConc(ASTAUX::MkId(L".h"));

  ppcc.ImpAppend (vdm_BC_GenInclusion (basenameSeq));

  TYPE_CPP_CPPAS forward;
  TYPE_CPP_CPPAS functionsDecl_l;
  SEQ<TYPE_CPP_Enumerator> enum_v;
  SEQ<TYPE_CPP_MemberDeclaration> cgbaseclass_l;
  TYPE_CPP_CPPAS functions_l;

  SET<TYPE_AS_Name> cl_nms_q (cl_nms);
  Generic nm;
  for (bool bb = cl_nms_q.First(nm); bb; bb = cl_nms_q.Next(nm))
  {
    Tuple tmp (GenGet(nm));

    forward.ImpAppend (vdm_BC_GenClassTypeDecl((TYPE_CPP_Identifier)vdm_BC_Rename (nm)));
    enum_v.ImpAppend (vdm_BC_GiveMacroName (nm));

    cgbaseclass_l.ImpConc (tmp.GetSequence(1)); // seq of CPP`MemberDeclaration
    functionsDecl_l.ImpConc (tmp.GetSequence(2)); // seq of CPP`IdentDeclaration
    functions_l.ImpConc (tmp.GetSequence(3)); // seq of CPP`FunctionDefinition
  }

  TYPE_CPP_Identifier cgbase (vdm_BC_GenIdentifier(vdm_BC_CGBasename));
  TYPE_CPP_Identifier hascg_v (vdm_BC_GenIdentifier (ASTAUX::MkId(L"has_cg_base")));
  TYPE_CPP_Identifier bl (GenSmallBoolType());
  TYPE_CPP_Identifier tr (vdm_BC_GenIdentifier (ASTAUX::MkId(L"true")));
  TYPE_CPP_Stmt dstmt (vdm_BC_GenBlock (type_dL()));
  TYPE_CPP_AccCompleteClassSpecifier acchead (vdm_BC_GenAccBaseSpec(vdm_BC_GenIdentifier (ASTAUX::MkId(L"vdmBase"))));
  SEQ<TYPE_CPP_BaseSpecifier> acchead_l;
  acchead_l.ImpAppend (acchead);

  TYPE_CPP_ClassHead head (vdm_BC_GenClassHead (cgbase, acchead_l));

  SEQ<TYPE_CPP_MemberList> public_v (vdm_BC_GenPublic (cgbaseclass_l));
  TYPE_CPP_FctDecl destr (vdm_BC_GenFctDecl(vdm_BC_GenDest (cgbase), type_dL()));
  TYPE_CPP_FctDecl cgfct (vdm_BC_GenConstFctDecl(hascg_v, type_dL()));
  TYPE_CPP_Stmt rtst (vdm_BC_GenReturnStmt (tr));

  type_dL rttp;
  rttp.ImpAppend (vdm_BC_GenFctSpecifier(quote_VIRTUAL)).ImpAppend (bl);

  TYPE_CPP_FunctionDefinition fdef (vdm_BC_GenFctDef (rttp, cgfct, nil, rtst));
  SEQ<TYPE_CPP_MemberDeclaration> fdef_l;
  fdef_l.ImpAppend (fdef);

  SEQ<TYPE_CPP_MemberList> private_v;
  private_v.ImpConc(vdm_BC_GenPrivate (fdef_l));
  private_v.ImpConc(public_v);

  TYPE_CPP_TypeSpecifier cldef (vdm_BC_GenTypeSpecifier(vdm_BC_GenClassSpecifier (head, private_v)));
  type_dL virt;
  virt.ImpAppend (vdm_BC_GenFctSpecifier(quote_VIRTUAL));

  cgbaseclass_l.ImpAppend (vdm_BC_GenFctDef (virt, destr, nil, dstmt));

  type_dL cldef_l;
  cldef_l.ImpAppend (cldef);

  cppas.ImpConc (forward).ImpAppend (vdm_BC_GenIdentDeclaration (SEQ<TYPE_CPP_Annotation>(), cldef_l, nil));
  cppas.ImpConc (functionsDecl_l);

  type_dL es_l;
  es_l.ImpAppend (vdm_BC_GenTypeSpecifier(vdm_BC_GenEnumSpec (nil, enum_v)));
  cppas.ImpAppend (vdm_BC_GenIdentDeclaration (SEQ<TYPE_CPP_Annotation>(), es_l, nil));

  SET<TYPE_CPP_File> returnSet;

  TYPE_AS_Id filenm ((Generic) vdm_BC_CGBasename);
  filenm.ImpConc(ASTAUX::MkId(L".h"));

  returnSet.Insert (vdm_BC_GenFile(nil, filenm, pp, cppas));
  returnSet.Insert (vdm_BC_GenFile(nil, GenFileExt((Generic) vdm_BC_CGBasename), ppcc, functions_l));

  return returnSet;
}

// GenGet
// nm : AS`Name
// ==> seq of CPP`MemberDeclaration * seq of CPP`IdentDeclaration * seq of CPP`FunctionDefinition
Tuple vdmcg::GenGet (const TYPE_AS_Name & nm)
{
  TYPE_CPP_Identifier rnm (vdm_BC_Rename (nm));
  TYPE_CPP_Identifier cls (rnm);

  TYPE_CPP_IntegerLit zerolit (vdm_BC_GenIntegerLit (0));
  TYPE_CPP_Identifier p_v (vdm_BC_GenIdentifier (ASTAUX::MkId(L"p")));
  TYPE_CPP_Identifier obj_v (vdm_BC_GenIdentifier (ASTAUX::MkId(L"obj")));
  TYPE_CPP_TypeSpecifier cgbase_v (vdm_BC_GenIdentifier ((Generic) vdm_BC_CGBasename));
  SEQ<TYPE_CPP_DeclSpecifier> classnm;
  classnm.ImpAppend(vdm_BC_GenTypeSpecifier (vdm_BC_GenIdentifier (ASTAUX::MkId(L"vdmBase"))));
  TYPE_CPP_Identifier clid (vdm_BC_PrefixName (ASTAUX::MkId(L"Get"), rnm));
  TYPE_CPP_Stmt ifstmt (
        vdm_BC_GenIfStmt(vdm_BC_GenNot(vdm_BC_GenFctCallObjMemAcc(obj_v, ASTAUX::MkId(L"IsInitialized"),
                                                                        SEQ<TYPE_CPP_Expr>())),
                         vdm_BC_GenBlock(mk_sequence(RunTime(L"Identifier is undefined/not initialized"))),
                         nil));
  SEQ<TYPE_CPP_DeclSpecifier> cobj;
  cobj.ImpAppend(vdm_BC_GenTypeSpecifier(quote_CONST));
  cobj.ImpAppend(vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"ObjectRef"))));
  TYPE_CPP_Expr gref (vdm_BC_GenFctCallObjMemAcc (obj_v, ASTAUX::MkId(L"GetRef"), SEQ<TYPE_CPP_Expr>()));
  TYPE_CPP_DeclarationStmt basedecl (vdm_BC_GenPtrDecl (classnm, p_v, gref));
  TYPE_CPP_Identifier getprefix (vdm_BC_PrefixClassName (ASTAUX::MkId(L"Get"),cls));
  TYPE_CPP_Identifier objpre (vdm_BC_PrefixClassName (ASTAUX::MkId(L"ObjGet"),cls));
  TYPE_CPP_Expr castexpr (vdm_BC_GenCastPtrExpr (vdm_BC_GenTypeSpecifier(cgbase_v), p_v));
  TYPE_CPP_Expr cond2 (vdm_BC_GenGenericFctCallPtrToObjMemAcc (castexpr, clid, SEQ<TYPE_CPP_Expr>()));
  TYPE_CPP_Stmt returncond (vdm_BC_GenReturnStmt (vdm_BC_GenBracketedExpr(vdm_BC_GenCondExpr (p_v, cond2, zerolit))));
  SEQ<TYPE_CPP_Stmt> bst;
  bst.ImpAppend (ifstmt);
  bst.ImpAppend (basedecl).ImpAppend (returncond);
  TYPE_CPP_Stmt stmt2 (vdm_BC_GenBlock (bst));
  TYPE_CPP_Stmt stmt1 (vdm_BC_GenReturnStmt (zerolit));
  TYPE_CPP_ArgDecl args (vdm_BC_GenArgDecl (cobj, vdm_BC_GenRef (obj_v)));
  TYPE_CPP_IndirectionDecl decl1 (vdm_BC_GenIndirection (vdm_BC_GenFctDecl (getprefix, type_dL())));

  SEQ<TYPE_CPP_ArgumentDeclaration> args_l;
  args_l.ImpAppend (args);
  TYPE_CPP_IndirectionDecl decl2 (vdm_BC_GenIndirection (vdm_BC_GenFctDecl (objpre, args_l)));

  SEQ<TYPE_CPP_DeclSpecifier> vircls_l;
  vircls_l.ImpAppend (vdm_BC_GenFctSpecifier(quote_VIRTUAL));
  vircls_l.ImpAppend (vdm_BC_GenTypeSpecifier(cls));

  SEQ<TYPE_CPP_MemberDeclaration> cgbase;
  cgbase.ImpAppend (vdm_BC_GenFctDef (vircls_l, decl1, nil, stmt1));

  SEQ<TYPE_CPP_DeclSpecifier> cls_l;
  cls_l.ImpAppend (vdm_BC_GenTypeSpecifier(cls));

  SEQ<TYPE_CPP_FunctionDefinition> objref;
  objref.ImpAppend (vdm_BC_GenFctDef (cls_l, decl2, nil, stmt2));

  SEQ<TYPE_CPP_IdentDeclaration> objrefDecl;
  objrefDecl.ImpAppend (vdm_BC_GenIdentDecl (SEQ<TYPE_CPP_Annotation>(), cls_l, decl2, nil));

  return mk_(cgbase, objrefDecl, objref);
}
#endif //VDMPP

// GenSeqOrMapImpModify
// vt : CGMAIN`VT
// e1 : CPP`Expr
// el_tp : REP`TypeRep
// e2 : CPP`Expr
// ==> CPP`Stmt
TYPE_CPP_Stmt vdmcg::GenSeqOrMapImpModify(const TYPE_CGMAIN_VT & vt,
                                          const TYPE_CPP_Expr & e1,
                                          const TYPE_REP_TypeRep & e1_tp,
                                          const TYPE_CPP_Expr & e2)
{
  const TYPE_CPP_Expr & m (vt.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & mt (vt.GetField(pos_CGMAIN_VT_type));

  TYPE_CPP_Expr new_e1;
#ifdef VDMPP
  if (IsIntType(e1_tp) && vdm_CPP_isJAVA()) {
    new_e1 = vdm_BC_GenFctCallObjMemAcc(vdm_BC_GenCastExpr(GenIntType(), e1),
                                          ASTAUX::MkId(L"intValue"),
                                          SEQ<TYPE_CPP_Expr>());
  }
  else
#endif // VDMPP
  {
    new_e1 = e1;
  }

  if (IsMapType(mt)) {
    return GenMapImpModify(m, e1, e2);
  }
  else if (IsSeqType(mt))
  {
#ifdef VDMPP
    if (vdm_CPP_isJAVA()) {
      if (IsStringType(mt))
      {
        TYPE_CPP_Expr e1_int (IsIntType(e1_tp) ? e1 : vdm_BC_GenCastExpr(GenIntType(), e1));

        TYPE_CPP_Expr e1_index (vdm_BC_GenFctCallObjMemAcc(e1_int, ASTAUX::MkId(L"intValue"), SEQ<TYPE_CPP_Expr>()));

        SEQ<TYPE_CPP_Expr> larg;
        larg.ImpAppend(vdm_BC_GenIntegerLit(0));
        larg.ImpAppend(vdm_BC_GenMinus(e1_index, vdm_BC_GenIntegerLit(1)));
        TYPE_CPP_Expr lhs (vdm_BC_GenFctCallObjMemAcc(m, ASTAUX::MkId(L"substring"), larg));
        TYPE_CPP_Expr rhs (vdm_BC_GenFctCallObjMemAcc(m, ASTAUX::MkId(L"substring"), mk_sequence(e1_index)));

        return vdm_BC_GenAsgnStmt(m, vdm_BC_GenPlus(lhs, vdm_BC_GenPlus(e2, rhs)));
      }
      else
        return GenSeqModify(m, new_e1, e2);
    }
    else
#endif // VDMPP
      return GenSeqModify(m, new_e1, e2);
  }
  else
  {
    TYPE_CPP_Expr cond_m (GenIsMap(m));
    TYPE_CPP_Expr castmap (GenCastMap(m));
    TYPE_CPP_Expr cond_s (GenIsSeq(m));
    TYPE_CPP_Expr castseq (GenCastSeq(m, mt));

    TYPE_CPP_Stmt alt_m;
    TYPE_CPP_Stmt alt_s;
#ifdef VDMPP
    if (vdm_CPP_isJAVA()) {
      alt_m = GenMapImpModify(castmap, e1, e2);
      alt_s = GenSeqModify(castseq, new_e1, e2);
    }
    else
#endif // VDMPP
    {
      alt_m = vdm_BC_GenAsgnStmt(m, GenMapImpModifyExpr(castmap, e1, e2));
      alt_s = vdm_BC_GenAsgnStmt(m, GenSeqModifyExpr(castseq, new_e1, e2));
    }

    TYPE_CPP_Stmt alt_rre (vdm_BC_GenBlock(mk_sequence(RunTime(L"Sequence or Map expected in Map or Sequence Designator"))));
    TYPE_CPP_Stmt ifstmt1 (vdm_BC_GenIfStmt(vdm_BC_GenNot(cond_s), alt_rre, nil));

    //TYPE_CPP_Stmt if1(vdm_BC_GenBlock(type_dL().ImpAppend(ifstmt1).ImpAppend(alt_s)));
    TYPE_CPP_Stmt if1(vdm_BC_GenBlock(mk_sequence(ifstmt1, alt_s)));
    return vdm_BC_GenIfStmt(cond_m, alt_m, if1);
  }
}

// GenIntResult
// r : CGMAIN`VT
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenIntResult(const TYPE_CGMAIN_VT &r)
{
  const TYPE_CPP_Expr & expr (r.GetRecord(pos_CGMAIN_VT_name));
  const Generic & tp         (r.GetField(pos_CGMAIN_VT_type));

  if (IsIntType(tp)) {
    return GenIntExpr(expr);
  }
  else {
    return GenRealExpr(expr);
  }
}

// GenPlus_DS (GenPlus)
// vt1 : CGMAIN`VT
// vt2 : CGMAIN`VT
// result : [CGMAIN`VT]
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenPlus_DS(const TYPE_CGMAIN_VT & vt1, const TYPE_CGMAIN_VT & vt2, const Generic & result)
{
  const TYPE_CPP_Expr & var1     (vt1.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & type1 (vt1.GetRecord(pos_CGMAIN_VT_type));
  const TYPE_CPP_Expr & var2     (vt2.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & type2 (vt2.GetRecord(pos_CGMAIN_VT_type));

#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    TYPE_CGMAIN_VT result_ (result);
//    const TYPE_CPP_Expr & res (result_.GetRecord(pos_CGMAIN_VT_name));
    const Generic & restp     (result_.GetField(pos_CGMAIN_VT_type));

    TYPE_CPP_Expr v1;
    if (IsNumType(type1))
      v1 = GenGetValue(var1, type1);
    else
    {
      TYPE_CPP_Expr cast (vdm_BC_GenCastExpr(GenNumType(), var1));
      v1 = (IsIntType(restp) ? GenGetValue(cast, restp) : GenGetValue(cast, mk_REP_NumericTypeRep(Int(REAL))));
    }

    TYPE_CPP_Expr v2;
    if (IsNumType(type2))
      v2 = GenGetValue(var2, type2);
    else
    {
      TYPE_CPP_Expr cast (vdm_BC_GenCastExpr(GenNumType(), var2));
      v2 = (IsIntType(restp) ? GenGetValue(cast, restp) : GenGetValue(cast, mk_REP_NumericTypeRep(Int(REAL))));
    }

    TYPE_CPP_Expr pexpr (vdm_BC_GenPlus(v1, v2));

    if (vdm_CPP_isJAVA() && IsIntType(restp) && (!IsIntType(type1) || !IsIntType(type2)))
    {
      pexpr = vdm_BC_GenCastExpr(GenSmallNumType(), vdm_BC_GenBracketedExpr(pexpr));
    }
    return GenIntResult(mk_CG_VT(pexpr, restp));
  }
  else
#endif // VDMPP
  {
    TYPE_CPP_Expr v1 (IsNumType(type1) ? var1 : vdm_BC_GenCastExpr(GenRealType(), var1));
    TYPE_CPP_Expr v2 (IsNumType(type2) ? var2 : vdm_BC_GenCastExpr(GenRealType(), var2));

    return vdm_BC_GenPlus(v1, v2);
  }
}

// GenMinus_DS (GenMinus)
// vt1 : CGMAIN`VT
// vt2 : CGMAIN`VT
// result : [CGMAIN`VT]
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenMinus_DS(const TYPE_CGMAIN_VT & vt1, const TYPE_CGMAIN_VT & vt2, const Generic & result)
{
  const TYPE_CPP_Expr & var1     (vt1.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & type1 (vt1.GetRecord(pos_CGMAIN_VT_type));
  const TYPE_CPP_Expr & var2     (vt2.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & type2 (vt2.GetRecord(pos_CGMAIN_VT_type));

#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    TYPE_CGMAIN_VT result_ (result);
//    const TYPE_CPP_Expr & res (result_.GetRecord(pos_CGMAIN_VT_name));
    const TYPE_REP_TypeRep & restp (result_.GetRecord(pos_CGMAIN_VT_type));

    TYPE_CPP_Expr v1;
    if (IsNumType(type1))
      v1 = GenGetValue(var1, type1);
    else
    {
      TYPE_CPP_Expr cast (vdm_BC_GenCastExpr(GenNumType(), var1));
      v1 = (IsIntType(restp) ? GenGetValue(cast, restp) : GenGetValue(cast, mk_REP_NumericTypeRep(Int(REAL))));
    }
    TYPE_CPP_Expr v2;
    if (IsNumType(type2))
      v2 = GenGetValue(var2, type2);
    else
    {
      TYPE_CPP_Expr cast (vdm_BC_GenCastExpr(GenNumType(), var2));
      v2 = (IsIntType(restp) ? GenGetValue(cast, restp) : GenGetValue(cast, mk_REP_NumericTypeRep(Int(REAL))));
    }
    TYPE_CPP_Expr pexpr (vdm_BC_GenMinus(v1, v2));

    if (vdm_CPP_isJAVA() && IsIntType(restp) && (!IsIntType(type1) || !IsIntType(type2)))
    {
      pexpr = vdm_BC_GenCastExpr(GenSmallNumType(), vdm_BC_GenBracketedExpr(pexpr));
    }
    return GenIntResult(mk_CG_VT(pexpr, restp));
  }
  else
#endif // VDMPP
  { // for C++
    TYPE_CPP_Expr v1 (IsNumType(type1) ? var1 : vdm_BC_GenCastExpr(GenRealType(), var1));
    TYPE_CPP_Expr v2 (IsNumType(type2) ? var2 : vdm_BC_GenCastExpr(GenRealType(), var2));

    return vdm_BC_GenMinus(v1, v2);
  }
}

// GenMult_DS (GenMult)
// vt1 : CGMAIN`VT
// vt2 : CGMAIN`VT
// result : [CGMAIN`VT]
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenMult_DS(const TYPE_CGMAIN_VT & vt1, const TYPE_CGMAIN_VT & vt2,
                                const Generic & result)
{
  const TYPE_CPP_Expr & var1     (vt1.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & type1 (vt1.GetRecord(pos_CGMAIN_VT_type));
  const TYPE_CPP_Expr & var2     (vt2.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & type2 (vt2.GetRecord(pos_CGMAIN_VT_type));

#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    TYPE_CGMAIN_VT result_ (result);
//    const TYPE_CPP_Expr & res (result_.GetRecord(pos_CGMAIN_VT_name));
    const TYPE_REP_TypeRep & restp (result_.GetRecord(pos_CGMAIN_VT_type));

    TYPE_CPP_Expr v1;
    if (IsNumType(type1))
      v1 = GenGetValue(var1, type1);
    else
    {
      TYPE_CPP_Expr cast (vdm_BC_GenCastExpr(GenNumType(), var1));
      v1 = (IsIntType(restp) ? GenGetValue(cast, restp) : GenGetValue(cast, mk_REP_NumericTypeRep(Int(REAL))));
    }

    TYPE_CPP_Expr v2;
    if (IsNumType(type2))
      v2 = GenGetValue(var2, type2);
    else
    {
      TYPE_CPP_Expr cast (vdm_BC_GenCastExpr(GenNumType(), var2));
      v2 = (IsIntType(restp) ? GenGetValue(cast, restp) : GenGetValue(cast, mk_REP_NumericTypeRep(Int(REAL))));
    }

    TYPE_CPP_Expr pexpr (vdm_BC_GenMult(v1, v2));

    if (IsIntType(restp) && (!IsIntType(type1) || !IsIntType(type2)))
    {
      pexpr = vdm_BC_GenCastExpr(GenSmallNumType(), vdm_BC_GenBracketedExpr(pexpr));
    }
    return GenIntResult(mk_CG_VT(pexpr, restp));
  }
  else
#endif // VDMPP
  {
    TYPE_CPP_Expr v1 (IsNumType(type1) ? var1 : vdm_BC_GenCastExpr(GenRealType(), var1));
    TYPE_CPP_Expr v2 (IsNumType(type2) ? var2 : vdm_BC_GenCastExpr(GenRealType(), var2));

    return vdm_BC_GenMult(v1, v2);
  }
}

// GenNumDiv
// vt1 : CGMAIN`VT
// vt2 : CGMAIN`VT
// result : [CGMAIN`VT]
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenNumDiv(const TYPE_CGMAIN_VT & vt1, const TYPE_CGMAIN_VT & vt2, const Generic & result)
{
  const TYPE_CPP_Expr & var1     (vt1.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & type1 (vt1.GetRecord(pos_CGMAIN_VT_type));
  const TYPE_CPP_Expr & var2     (vt2.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & type2 (vt2.GetRecord(pos_CGMAIN_VT_type));

#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    TYPE_CPP_Expr va1 (IsNumType(type1) ? var1 : vdm_BC_GenCastExpr(GenNumType(), var1));
    TYPE_CPP_Expr va2 (IsNumType(type2) ? var2 : vdm_BC_GenCastExpr(GenNumType(), var2));
    TYPE_CPP_Expr v1 (GenGetValue(va1, mk_REP_NumericTypeRep(Int(REAL))));
    TYPE_CPP_Expr v2 (GenGetValue(va2, mk_REP_NumericTypeRep(Int(REAL))));

    return GenRealExpr(vdm_BC_GenDiv(v1, v2));
  }
  else
#endif // VDMPP
  {
    TYPE_CPP_Expr v1 (IsNumType(type1) ? var1 : vdm_BC_GenCastExpr(GenRealType(), var1));
    TYPE_CPP_Expr v2 (IsNumType(type2) ? var2 : vdm_BC_GenCastExpr(GenRealType(), var2));

    return vdm_BC_GenDiv(v1, v2);
  }
}

#ifdef VDMPP
// GenIntVal
// r : CGMAIN`VT
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenIntVal(const TYPE_CGMAIN_VT & r)
{
  const TYPE_CPP_Expr & v     (r.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & tp (r.GetRecord(pos_CGMAIN_VT_type));

  if (IsIntType(tp)) {
    return v;
  }
  else if (IsNumType(tp))
  {
    TYPE_CPP_Expr e (vdm_BC_GenFctCallObjMemAcc(v, ASTAUX::MkId(L"intValue"), SEQ<TYPE_CPP_Expr>()));
    return GenIntExpr(e);
  }
  else {
    return GetIntVal(v);
  }
}
#endif // VDMPP

// GenNumRem
// vt1 : CGMAIN`VT
// vt2 : CGMAIN`VT
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenNumRem(const TYPE_CGMAIN_VT & vt1, const TYPE_CGMAIN_VT & vt2)
{
  const TYPE_CPP_Expr & var1     (vt1.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & type1 (vt1.GetRecord(pos_CGMAIN_VT_type));
  const TYPE_CPP_Expr & var2     (vt2.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & type2 (vt2.GetRecord(pos_CGMAIN_VT_type));

#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    TYPE_CPP_Expr etp1 (IsNumType(type1) ? var1 : vdm_BC_GenCastExpr(GenNumType(), var1));
    TYPE_CPP_Expr etp2 (IsNumType(type2) ? var2 : vdm_BC_GenCastExpr(GenNumType(), var2));

    TYPE_CPP_Expr ee1 (GenIntVal(mk_CG_VT(etp1, type1)));
    TYPE_CPP_Expr ee2 (GenIntVal(mk_CG_VT(etp2, type2)));
    TYPE_CPP_Expr e1 (GenGetValue(ee1, mk_REP_NumericTypeRep(Int(INTEGER))));
    TYPE_CPP_Expr e2 (GenGetValue(ee2, mk_REP_NumericTypeRep(Int(INTEGER))));

    if (!IsIntType(type1) || !IsIntType(type2))
      SetException(true);

    return GenIntExpr(vdm_BC_GenMod(e1, e2));
  }
  else
#endif // VDMPP
  {
    TYPE_CPP_Expr v1 (IsIntType(type1) ? var1 : vdm_BC_GenCastExpr(GenIntType(), var1));
    TYPE_CPP_Expr v2 (IsIntType(type2) ? var2 : vdm_BC_GenCastExpr(GenIntType(), var2));

    return vdm_BC_GenCastExpr( GenIntType(), vdm_BC_GenMod(v1, v2) );
  }
}

// GenNumMod
// vt1 : CGMAIN`VT
// vt2 : CGMAIN`VT
// resvt : CGMAIN`VT
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenNumMod(const TYPE_CGMAIN_VT & vt1, const TYPE_CGMAIN_VT & vt2,
                               const TYPE_CGMAIN_VT & resvt)
{
  const TYPE_CPP_Expr & var1     (vt1.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & type1 (vt1.GetRecord(pos_CGMAIN_VT_type));
  const TYPE_CPP_Expr & var2     (vt2.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & type2 (vt2.GetRecord(pos_CGMAIN_VT_type));

#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
//    const TYPE_CPP_Expr & res_v      (resvt.GetRecord(pos_CGMAIN_VT_name));
    const TYPE_REP_TypeRep & restype (resvt.GetRecord(pos_CGMAIN_VT_type));

    TYPE_CPP_Expr etp1 (IsNumType(type1) ? var1 : vdm_BC_GenCastExpr(GenNumType(), var1));
    TYPE_CPP_Expr etp2 (IsNumType(type2) ? var2 : vdm_BC_GenCastExpr(GenNumType(), var2));

    TYPE_CPP_Expr ee1 (GenIntVal(mk_CG_VT(etp1, type1)));
    TYPE_CPP_Expr ee2 (GenIntVal(mk_CG_VT(etp2, type2)));

    TYPE_CPP_Expr e1r (GenGetValue(ee1, mk_REP_NumericTypeRep(Int(REAL))));
    TYPE_CPP_Expr e2r (GenGetValue(ee2, mk_REP_NumericTypeRep(Int(REAL))));
    TYPE_CPP_Expr e1 (GenGetValue(ee1, mk_REP_NumericTypeRep(Int(INTEGER))));
    TYPE_CPP_Expr e2 (GenGetValue(ee2, mk_REP_NumericTypeRep(Int(INTEGER))));
    TYPE_CPP_Expr de (vdm_BC_GenDiv(e1r,e2r));
    TYPE_CPP_Expr fl (vdm_BC_GenFctCall (vdm_BC_GenIdentifier(ASTAUX::MkId(L"Math.floor")),
                                         SEQ<TYPE_CPP_Expr>().ImpAppend(de)));

    if (!IsIntType(type1) || !IsIntType(type2)) {
      SetException(true);
    }

    TYPE_CPP_Expr pexpr (vdm_BC_GenMinus(e1, vdm_BC_GenMult (e2, vdm_BC_GenCastExpr(GenSmallNumType(), fl))));
    return GenIntResult(mk_CG_VT(pexpr, restype));
  }
  else
#endif // VDMPP
  {
    TYPE_CPP_Expr e1 (IsNumType(type1) ? var1 : vdm_BC_GenCastExpr(GenIntType(), var1));
    TYPE_CPP_Expr e2 (IsNumType(type2) ? var2 : vdm_BC_GenCastExpr(GenIntType(), var2));

    TYPE_CPP_Expr quotient (vdm_BC_GenFctCallObjMemAcc (vdm_BC_GenDiv (e1,e2), ASTAUX::MkId(L"Floor"), SEQ<TYPE_CPP_Expr>()));
    return vdm_BC_GenMinus(e1, vdm_BC_GenMult (e2, GenIntExpr (quotient)));
  }
}

// GenIntDiv
// vt1 : CGMAIN`VT
// vt2 : CGMAIN`VT
// resvt : CGMAIN`VT
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenIntDiv(const TYPE_CGMAIN_VT & vt1, const TYPE_CGMAIN_VT & vt2,
                               const TYPE_CGMAIN_VT & resvt)
{
  const TYPE_CPP_Expr & var1     (vt1.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & type1 (vt1.GetRecord(pos_CGMAIN_VT_type));
  const TYPE_CPP_Expr & var2     (vt2.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & type2 (vt2.GetRecord(pos_CGMAIN_VT_type));

#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    const TYPE_REP_TypeRep & restype (resvt.GetRecord(pos_CGMAIN_VT_type));

    TYPE_CPP_Expr etp1 (IsNumType(type1) ? var1 : vdm_BC_GenCastExpr(GenNumType(), var1));
    TYPE_CPP_Expr etp2 (IsNumType(type2) ? var2 : vdm_BC_GenCastExpr(GenNumType(), var2));

    TYPE_CPP_Expr ee1 (GenIntVal(mk_CG_VT(etp1, type1)));
    TYPE_CPP_Expr ee2 (GenIntVal(mk_CG_VT(etp2, type2)));
    TYPE_CPP_Expr e1 (GenGetValue(ee1, mk_REP_NumericTypeRep(Int(INTEGER))));
    TYPE_CPP_Expr e2 (GenGetValue(ee2, mk_REP_NumericTypeRep(Int(INTEGER))));
    TYPE_CPP_Expr pexpr (vdm_BC_GenDiv(e1, e2));

    if (!IsIntType(type1) || !IsIntType(type2))
      SetException(true);

    if (IsIntType(restype))
      return GenIntExpr(pexpr);
    else if (IsRealType(restype))
      return GenRealExpr(pexpr);
    else
      return GenIntExpr(pexpr);
  }
  else
#endif // VDMPP
  {
    TYPE_CPP_Expr e1 (IsNumType(type1) ? var1 : vdm_BC_GenCastExpr(GenIntType(), var1));
    TYPE_CPP_Expr e2 (IsNumType(type2) ? var2 : vdm_BC_GenCastExpr(GenIntType(), var2));

    return GenIntExpr(vdm_BC_GenDiv(e1, e2));
  }
}

// GenNumLT
// vt1 : CGMAIN`VT
// vt2 : CGMAIN`VT
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenNumLT(const TYPE_CGMAIN_VT & vt1, const TYPE_CGMAIN_VT & vt2)
{
  return GenNumOp(vt1, vt2, TAG_quote_NUMLT);
}

// GenNumLE
// vt1 : CGMAIN`VT
// vt2 : CGMAIN`VT
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenNumLE(const TYPE_CGMAIN_VT & vt1, const TYPE_CGMAIN_VT & vt2)
{
  return GenNumOp(vt1, vt2, TAG_quote_NUMLE);
}

// GenNumGT
// vt1 : CGMAIN`VT
// vt2 : CGMAIN`VT
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenNumGT(const TYPE_CGMAIN_VT & vt1, const TYPE_CGMAIN_VT & vt2)
{
  return GenNumOp(vt1, vt2, TAG_quote_NUMGT);
}

// GenNumGE
// vt1 : CGMAIN`VT
// vt2 : CGMAIN`VT
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenNumGE(const TYPE_CGMAIN_VT & vt1, const TYPE_CGMAIN_VT & vt2)
{
  return GenNumOp(vt1, vt2, TAG_quote_NUMGE);
}

// GenNumOp
// vt1 : CGMAIN`VT
// vt2 : CGMAIN`VT
// tp : OpType
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenNumOp(const TYPE_CGMAIN_VT & vt1, const TYPE_CGMAIN_VT & vt2, int tp)
{
  const TYPE_CPP_Expr & var1     (vt1.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & type1 (vt1.GetRecord(pos_CGMAIN_VT_type));
  const TYPE_CPP_Expr & var2     (vt2.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & type2 (vt2.GetRecord(pos_CGMAIN_VT_type));

  TYPE_CPP_Expr expr1 (IsNumType(type1) ? var1 : vdm_BC_GenCastExpr(GenNumType(), var1));
  TYPE_CPP_Expr expr2 (IsNumType(type2) ? var2 : vdm_BC_GenCastExpr(GenNumType(), var2));

  TYPE_CPP_Expr fc1 (IsNumType(type1) ? GenGetValue(expr1, type1)
                                      : GenGetValue(expr1, mk_REP_NumericTypeRep(Int(REAL))));
  TYPE_CPP_Expr fc2 (IsNumType(type2) ? GenGetValue(expr2, type2)
                                      : GenGetValue(expr2, mk_REP_NumericTypeRep(Int(REAL))));

  TYPE_CPP_Expr br1;
  if (fc1.Is(TAG_TYPE_CPP_IntegerLit) || fc1.Is(TAG_TYPE_CPP_FloatingLit) || fc1.Is(TAG_TYPE_CPP_FctCall))
    br1 = fc1;
  else
    br1 = vdm_BC_GenBracketedExpr(fc1);

  TYPE_CPP_Expr br2;
  if (fc2.Is(TAG_TYPE_CPP_IntegerLit) || fc2.Is(TAG_TYPE_CPP_FloatingLit) || fc2.Is(TAG_TYPE_CPP_FctCall))
    br2 = fc2;
  else
    br2 = vdm_BC_GenBracketedExpr(fc2);

  TYPE_CPP_Expr r;
  switch( tp ) {
    case TAG_quote_NUMLT: { r = vdm_BC_GenLt(br1, br2); break; }
    case TAG_quote_NUMLE: { r = vdm_BC_GenLeq(br1, br2); break; }
    case TAG_quote_NUMGT: { r = vdm_BC_GenGt(br1, br2); break; }
    case TAG_quote_NUMGE: { r = vdm_BC_GenGeq(br1, br2); break; }
    default: { ReportError(L"GenNumOp"); }
  }
  return GenBoolExpr(r);
}

// GenExp
// vt1 : CGMAIN`VT
// vt2 : CGMAIN`VT
// resVT : CGMAIN`VT
// ==> CPP`Stmt
TYPE_CPP_Stmt vdmcg::GenExp(const TYPE_CGMAIN_VT & vt1, const TYPE_CGMAIN_VT & vt2, const TYPE_CGMAIN_VT & resVT)
{
  const TYPE_CPP_Expr & var1     (vt1.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & type1 (vt1.GetRecord(pos_CGMAIN_VT_type));
  const TYPE_CPP_Expr & var2     (vt2.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & type2 (vt2.GetRecord(pos_CGMAIN_VT_type));
  const TYPE_CPP_Expr & resVar_v (resVT.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & restp (resVT.GetRecord(pos_CGMAIN_VT_type));

  SEQ<TYPE_CPP_Expr> var2_l;
  var2_l.ImpAppend(var2);

  TYPE_CPP_TypeSpecifier r (GenRealType());
  TYPE_CPP_TypeSpecifier i (GenIntType());

#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    TYPE_CPP_Expr ee1 (IsNumType(type1) ? var1 : vdm_BC_GenCastExpr(GenNumType(), var1));
    TYPE_CPP_Expr ee2 (IsNumType(type2) ? var2 : vdm_BC_GenCastExpr(GenNumType(), var2));
    TYPE_CPP_Expr e1 (GenGetValue(ee1, mk_REP_NumericTypeRep(Int(REAL))));
    TYPE_CPP_Expr e2 (GenGetValue(ee2, mk_REP_NumericTypeRep(Int(REAL))));

    TYPE_CPP_Expr fctcall (vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Math.pow")), mk_sequence(e1, e2) ));
    TYPE_CPP_Expr pexpr (vdm_BC_GenCastExpr(GenSmallNumType(), fctcall));

    TYPE_CPP_Expr cast;
    if (IsIntType(restp))
      cast = GenIntExpr(pexpr);
    else if (IsRealType(restp))
      cast = GenRealExpr(fctcall);
    else if (IsIntType(type1) && IsIntType(type2))
      cast = GenIntExpr(pexpr);
    else
      cast = GenRealExpr(fctcall);

    return vdm_BC_GenAsgnStmt(resVar_v, cast);
  }
  else
#endif // VDMPP
  {
    TYPE_CPP_Expr lhsReal (vdm_BC_GenCastExpr(r, var1));
    TYPE_CPP_Expr lhsInt (vdm_BC_GenCastExpr(i, var1));
    TYPE_CPP_Expr rhsReal (vdm_BC_GenCastExpr(r, var2));
    TYPE_CPP_Expr rhsInt (vdm_BC_GenCastExpr(i, var2));

    SEQ<TYPE_CPP_Expr> rhsReal_l;
    rhsReal_l.ImpAppend(rhsReal);
    SEQ<TYPE_CPP_Expr> rhsInt_l;
    rhsInt_l.ImpAppend(rhsInt);
    TYPE_CPP_Expr eRealReal(vdm_BC_GenAsgnExpr(resVar_v, vdm_BC_GenFctCallObjMemAcc(lhsReal, ASTAUX::MkId(L"Exp"), rhsReal_l)));
    TYPE_CPP_Expr eRealInt(vdm_BC_GenAsgnExpr(resVar_v, vdm_BC_GenFctCallObjMemAcc(lhsReal, ASTAUX::MkId(L"Exp"), rhsInt_l)));
    TYPE_CPP_Expr eIntReal(vdm_BC_GenAsgnExpr(resVar_v, vdm_BC_GenFctCallObjMemAcc(lhsInt, ASTAUX::MkId(L"Exp"), rhsReal_l)));
    TYPE_CPP_Expr eIntInt(vdm_BC_GenAsgnExpr(resVar_v, vdm_BC_GenFctCallObjMemAcc(lhsInt, ASTAUX::MkId(L"Exp"), rhsInt_l)));
    TYPE_CPP_Expr rhsIsReal(GenIsReal(var2));
    TYPE_CPP_Expr lhsIsReal(GenIsReal(var1));

    if (type1.Is(TAG_TYPE_REP_UnionTypeRep))
    {
      if (type2.Is(TAG_TYPE_REP_UnionTypeRep))
      {
        return vdm_BC_GenExpressionStmt(vdm_BC_GenBracketedExpr(vdm_BC_GenCondExpr(
                                          rhsIsReal,
                                          vdm_BC_GenBracketedExpr(vdm_BC_GenCondExpr(lhsIsReal, eRealReal, eRealInt)),
                                          vdm_BC_GenBracketedExpr(vdm_BC_GenCondExpr(lhsIsReal, eIntReal, eIntInt)))));
      }
      else // !type2.Is(TAG_TYPE_REP_UnionTypeRep)
      {
        return vdm_BC_GenExpressionStmt(vdm_BC_GenBracketedExpr(vdm_BC_GenCondExpr(
                                          lhsIsReal,
                                          vdm_BC_GenAsgnExpr(
                                            resVar_v,
                                            vdm_BC_GenFctCallObjMemAcc(lhsReal, ASTAUX::MkId(L"Exp"), var2_l)),
                                          vdm_BC_GenAsgnExpr(
                                            resVar_v,
                                            vdm_BC_GenFctCallObjMemAcc(lhsInt, ASTAUX::MkId(L"Exp"), var2_l)))));
      }
    }
    else // !type1.Is(TAG_TYPE_REP_UnionTypeRep)
    {
      if (type2.Is(TAG_TYPE_REP_UnionTypeRep))
      {
        return vdm_BC_GenExpressionStmt(vdm_BC_GenBracketedExpr(vdm_BC_GenCondExpr(
                                          rhsIsReal,
                                          vdm_BC_GenAsgnExpr(
                                            resVar_v,
                                            vdm_BC_GenFctCallObjMemAcc(var1, ASTAUX::MkId(L"Exp"), rhsReal_l)),
                                          vdm_BC_GenAsgnExpr(
                                            resVar_v,
                                            vdm_BC_GenFctCallObjMemAcc(var1, ASTAUX::MkId(L"Exp"), rhsInt_l)))));
      }
      else // !type2.Is(TAG_TYPE_REP_UnionTypeRep)
      {
        return vdm_BC_GenAsgnStmt(resVar_v, vdm_BC_GenFctCallObjMemAcc(var1,ASTAUX::MkId(L"Exp"), var2_l));
      }
    }
  }
}

// GenEq_DS (GenEq)
// vt1 : CGMAIN`VT
// vt2 : CGMAIN`VT
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenEq_DS(const TYPE_CGMAIN_VT & vt1, const TYPE_CGMAIN_VT & vt2)
{
  const TYPE_CPP_Expr & var1   (vt1.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & tp1 (vt1.GetRecord(pos_CGMAIN_VT_type));
  const TYPE_CPP_Expr & var2   (vt2.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & tp2 (vt2.GetRecord(pos_CGMAIN_VT_type));

#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    TYPE_CPP_Expr expr (IsNumType(tp1) && IsNumType(tp2)
              ? vdm_BC_GenEq(GenGetValue(var1, tp1), GenGetValue(var2, tp2))
              : vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"UTIL.equals")), mk_sequence(var1, var2)));
    return GenBoolExpr(expr);
  }
  else
#endif // VDMPP
  {
#ifdef VDMPP
    // Object ?
    if (tp1.Is(TAG_TYPE_REP_ObjRefTypeRep) && tp2.Is(TAG_TYPE_REP_ObjRefTypeRep))
    {
      return vdm_BC_GenCastExpr(GenBoolType(), vdm_BC_GenEq(var1, var2));
    }
#endif // VDMPP
    TYPE_CPP_Expr expr;
    if (IsNumType(tp1) && IsNumType(tp2))
      expr = vdm_BC_GenEq(GenGetValue(var1, tp1), GenGetValue(var2, tp2));
    else 
      expr = vdm_BC_GenEq(var1, var2);
    return GenBoolExpr(expr);
  }
}

// GenNeq_DS (GenNeq)
// vt1 : CGMAIN`VT
// vt2 : CGMAIN`VT
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenNeq_DS(const TYPE_CGMAIN_VT & vt1, const TYPE_CGMAIN_VT & vt2)
{
  const TYPE_CPP_Expr & var1   (vt1.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & tp1 (vt1.GetRecord(pos_CGMAIN_VT_type));
  const TYPE_CPP_Expr & var2   (vt2.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & tp2 (vt2.GetRecord(pos_CGMAIN_VT_type));

#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    TYPE_CPP_Expr expr (IsNumType(tp1) && IsNumType(tp2)
                         ? vdm_BC_GenNot(vdm_BC_GenEq(GenGetValue(var1, tp1), GenGetValue(var2, tp2)))
                         : vdm_BC_GenNot(vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"UTIL.equals")),
                                                           mk_sequence(var1, var2))));
    return GenBoolExpr(expr);
  }
  else
#endif // VDMPP
  {
    TYPE_CPP_Expr expr;
    if (IsNumType(tp1) && IsNumType(tp2))
      expr = vdm_BC_GenNot(vdm_BC_GenEq(GenGetValue(var1, tp1), GenGetValue(var2, tp2)));
    else 
      expr = vdm_BC_GenNot(vdm_BC_GenEq(var1, var2));
    return GenBoolExpr(expr);
  }
}

// UnaryPlus
// vt1 : CGMAIN`VT
// vt2 : CGMAIN`VT
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::UnaryPlus(const TYPE_CGMAIN_VT & vt1, const TYPE_CGMAIN_VT & vt2)
{
  const TYPE_CPP_Expr & resVar_v (vt1.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & restp (vt1.GetRecord(pos_CGMAIN_VT_type));
  const TYPE_CPP_Expr & arg      (vt2.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & type  (vt2.GetRecord(pos_CGMAIN_VT_type));

  switch(type.GetTag()) {
    case TAG_TYPE_REP_UnionTypeRep: {
#ifdef VDMPP
      if (vdm_CPP_isJAVA()) {
        TYPE_CPP_Expr cast (IsNumType(type) ? arg : vdm_BC_GenCastExpr(GenNumType(),arg));
        TYPE_CPP_Expr fctcall;
        if (IsIntType(restp)) {
          fctcall = vdm_BC_GenFctCallObjMemAcc(cast, ASTAUX::MkId(L"intValue"), SEQ<TYPE_CPP_Expr>());
        }
        else {
          fctcall = vdm_BC_GenFctCallObjMemAcc(cast, ASTAUX::MkId(L"doubleValue"), SEQ<TYPE_CPP_Expr>());
        }

        if (IsRealType(restp)) {
          return GenRealExpr(fctcall);
        }
        else if (IsIntType(restp)) {
          return GenIntExpr(fctcall);
        }
        else {
          TYPE_CPP_Expr realasgn (vdm_BC_GenAsgnExpr(resVar_v, GenRealExpr(fctcall)));
          TYPE_CPP_Expr intasgn (vdm_BC_GenAsgnExpr(resVar_v, GenIntExpr(fctcall)));
          return vdm_BC_GenBracketedExpr(vdm_BC_GenCondExpr(GenIsReal(arg), realasgn, intasgn));
        }
      }
      else
#endif // VDMPP
      {
        if (IsNumType(type)) {
          return arg;
        }

        if (IsIntType(restp)) {
          return vdm_BC_GenCastExpr(GenIntType(), arg);
        }
        else {
          return vdm_BC_GenCastExpr(GenRealType(), arg);
        }
      }
    }
    case TAG_TYPE_REP_NumericTypeRep: {
      return arg;
    }
    default: {
      ReportError(L"UnaryPlus");
      break;
    }
  }
  return Record(0,0); // To avoid warnings
}

// UnaryMinus
// vt1 : CGMAIN`VT
// vt2 : CGMAIN`VT
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::UnaryMinus(const TYPE_CGMAIN_VT & vt1, const TYPE_CGMAIN_VT & vt2)
{
  const TYPE_CPP_Expr & resVar_v (vt1.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & restp (vt1.GetRecord(pos_CGMAIN_VT_type));
  const TYPE_CPP_Expr & arg      (vt2.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & type  (vt2.GetRecord(pos_CGMAIN_VT_type));

  switch(type.GetTag()) {
    case TAG_TYPE_REP_UnionTypeRep: {
#ifdef VDMPP
      if (vdm_CPP_isJAVA()) {
        TYPE_CPP_Expr cast (IsNumType(type) ? arg : vdm_BC_GenCastExpr(GenNumType(),arg));
        TYPE_CPP_Expr fctcall;
        if (IsIntType(restp)) {
          fctcall = vdm_BC_GenUnaryMinus(vdm_BC_GenFctCallObjMemAcc(cast,
                                                                    ASTAUX::MkId(L"intValue"),
                                                                    SEQ<TYPE_CPP_Expr>()));
        }
        else {
          fctcall = vdm_BC_GenUnaryMinus(vdm_BC_GenFctCallObjMemAcc(cast,
                                                                    ASTAUX::MkId(L"doubleValue"),
                                                                    SEQ<TYPE_CPP_Expr>()));
        }
        if (IsRealType(restp)) {
          return GenRealExpr(fctcall);
        }
        else if (IsIntType(restp)) {
          return GenIntExpr(fctcall);
        }
        else {
          TYPE_CPP_Expr realasgn (vdm_BC_GenAsgnExpr(resVar_v, GenRealExpr(fctcall)));
          TYPE_CPP_Expr intasgn (vdm_BC_GenAsgnExpr(resVar_v, GenIntExpr(fctcall)));
          return vdm_BC_GenBracketedExpr(vdm_BC_GenCondExpr(GenIsReal(arg), realasgn, intasgn));
        }
      }
      else
#endif // VDMPP
      {
        if (IsNumType(type)) {
          return vdm_BC_GenUnaryMinus(arg);
        }

        if (IsIntType(restp)) {
          return vdm_BC_GenUnaryMinus(vdm_BC_GenCastExpr(GenIntType(), arg));
        }
        else {
          return vdm_BC_GenUnaryMinus(vdm_BC_GenCastExpr(GenRealType(), arg));
        }
      }
    }
    case TAG_TYPE_REP_NumericTypeRep: {
#ifdef VDMPP
      if (vdm_CPP_isJAVA()) {
        if (IsRealType(type)) {
          return GenRealExpr(vdm_BC_GenUnaryMinus(GenGetValue(arg,type)));
        }
        else {
          return GenIntExpr(vdm_BC_GenUnaryMinus(GenGetValue(arg,type)));
        }
      }
      else
#endif // VDMPP
        return vdm_BC_GenUnaryMinus(arg);
    }
    default: {
      ReportError(L"UnaryMinus");
      break;
    }
  }
  return Record(0,0); // To avoid warnings
}

// UnaryAbs
// vt1 : CGMAIN`VT
// vt2 : CGMAIN`VT
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::UnaryAbs(const TYPE_CGMAIN_VT & vt1, const TYPE_CGMAIN_VT & vt2)
{
  const TYPE_CPP_Expr & resVar_v   (vt1.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & type    (vt1.GetRecord(pos_CGMAIN_VT_type));
  const TYPE_CPP_Expr & arg        (vt2.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & argType (vt2.GetRecord(pos_CGMAIN_VT_type));

#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    TYPE_CPP_Expr fctcall;
    if (IsNumType(argType))
      fctcall = GenGetValue(arg, argType);
    else
      fctcall = GenGetValue(vdm_BC_GenCastExpr(GenNumType(), arg), mk_REP_NumericTypeRep(Int(REAL)));

    TYPE_CPP_Expr abscall (vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Math.abs")), mk_sequence(fctcall)));

    SEQ<TYPE_CPP_Stmt> res;
    if (IsIntType(type))
    {
      if (argType.Is(TAG_TYPE_REP_UnionTypeRep))
      {
        TYPE_CPP_Expr cast (vdm_BC_GenCastExpr(GenSmallNumType(), abscall));
        res.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, GenIntExpr(cast)));
      }
      else
        res.ImpAppend (vdm_BC_GenAsgnStmt(resVar_v, GenIntExpr(abscall)));
    }
    else
      res.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, GenRealExpr(abscall)));
    return res;
  }
  else
#endif // VDMPP
  {
    TYPE_CPP_Expr e (StripBracketedExpr(arg));
    TYPE_CPP_Expr fcall (IsIntType(type)
                   ? vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"CGUTIL::IAbs")), mk_sequence(e))
                   : vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"CGUTIL::RAbs")), mk_sequence(e)));
           
    SEQ<TYPE_CPP_Stmt> rb;
    rb.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, fcall));
    return rb;
  }
}

// UnaryFloor
// vt1 : CGMAIN`VT
// vt2 : CGMAIN`VT
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::UnaryFloor(const TYPE_CGMAIN_VT & vt1, const TYPE_CGMAIN_VT & vt2)
{
  const TYPE_CPP_Expr & resVar_v (vt1.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & restp (vt1.GetRecord(pos_CGMAIN_VT_type));
  const TYPE_CPP_Expr & arg      (vt2.GetRecord(pos_CGMAIN_VT_name));
  const TYPE_REP_TypeRep & argtp (vt2.GetRecord(pos_CGMAIN_VT_type) );

#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    TYPE_CPP_Expr ee1 (IsNumType(argtp) ? arg : vdm_BC_GenCastExpr(GenNumType(), arg));
    TYPE_CPP_Expr e1 (GenGetValue(ee1, mk_REP_NumericTypeRep(Int(REAL))));
    TYPE_CPP_Expr floorapply (vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Math.floor")), mk_sequence(e1)));
    TYPE_CPP_Expr getint (vdm_BC_GenCastExpr(GenSmallNumType(), floorapply));

    TYPE_CPP_Expr res_e;
    if (IsRealType(restp))
      res_e = GenRealExpr(getint);
    else
      res_e = GenIntExpr(getint);

    SEQ<TYPE_CPP_Stmt> rb;
    rb.ImpAppend(vdm_BC_GenAsgnStmt(resVar_v, res_e));
    return rb;
  }
  else
#endif // VDMPP
  {
    TYPE_CPP_Expr cast (IsRealType(argtp) ? arg : vdm_BC_GenCastExpr(GenRealType(), arg));
    TYPE_CPP_Expr fcall (vdm_BC_GenFctCallObjMemAcc(cast, ASTAUX::MkId(L"Floor"), SEQ<TYPE_CPP_Expr>()));
    SEQ<TYPE_CPP_Stmt> rb;
    rb.ImpAppend( vdm_BC_GenAsgnStmt(resVar_v, fcall) );
    return rb;
  }
}

// GenNewTag
// tag : CPP`Expr
// length : nat
// ==> CPP`Stmt
TYPE_CPP_Stmt vdmcg::GenNewTag(const TYPE_CPP_Expr & tag, int length)
{
  TYPE_CPP_Expr id (vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"VDMGetDefaultRecInfoMap")), SEQ<TYPE_CPP_Expr>()));
  TYPE_CPP_Identifier cpp_len (vdm_BC_GenIntegerLit(length));
  TYPE_CPP_Expr expr (vdm_BC_GenFctCallObjMemAcc(id, ASTAUX::MkId(L"NewTag"), mk_sequence(tag, cpp_len)));
  return vdm_BC_GenExpressionStmt(expr);
}

// GenSetDontCare
// tag : CPP`Expr
// field_no : nat1
// ==> CPP`Stmt
TYPE_CPP_Stmt vdmcg::GenSetDontCare(const TYPE_CPP_Expr & tag, int field_no)
{
  TYPE_CPP_Expr id (vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"VDMGetDefaultRecInfoMap")), SEQ<TYPE_CPP_Expr>()));
  TYPE_CPP_Identifier cpp_no (vdm_BC_GenIntegerLit(field_no));
  TYPE_CPP_Expr expr (vdm_BC_GenFctCallObjMemAcc(id, ASTAUX::MkId(L"SetDontCare"), mk_sequence(tag, cpp_no)));
  return vdm_BC_GenExpressionStmt(expr);
}

// GenSetSymTag
// tag : CPP`Expr
// tagname : AS`Name
// modname : AS`Name
// ==> CPP`Stmt
TYPE_CPP_Stmt vdmcg::GenSetSymTag(const TYPE_CPP_Expr & tag, const TYPE_AS_Name & tagname, const TYPE_AS_Name & modname)
{
  TYPE_CPP_Expr id (vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"VDMGetDefaultRecInfoMap")), SEQ<TYPE_CPP_Expr>()));

  TYPE_AS_Id mname (GiveLastName(modname).ImpConc(ASTAUX::MkId(L"`")).ImpConc(GiveLastName(tagname)));
  TYPE_CPP_StringLit cpp_tagname (vdm_BC_GenStringLit(mname));

  TYPE_CPP_Expr expr (vdm_BC_GenFctCallObjMemAcc(id, ASTAUX::MkId(L"SetSymTag"), mk_sequence(tag, cpp_tagname)));
  return vdm_BC_GenExpressionStmt(expr);
}

// GenSetRecordTag
// tag : CPP`Expr
// tagname : AS`Name
// modname : AS`Name
//  ==> CPP`Stmt
TYPE_CPP_Stmt vdmcg::GenSetRecordTag(const TYPE_CPP_Expr& tag, const TYPE_AS_Name& tagname, const TYPE_AS_Name& modname)
{
  TYPE_AS_Id mname (GiveLastName(modname).ImpConc(ASTAUX::MkId(L"`")).ImpConc(GiveLastName(tagname)));

  TYPE_CPP_StringLit cpp_tagname (vdm_BC_GenStringLit(mname));
  TYPE_CPP_Expr expr (vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"AddRecordTag")),
                                        mk_sequence(cpp_tagname, tag)));
  return vdm_BC_GenExpressionStmt(expr);
}

// GenCppCharType
// ==> seq of char
TYPE_AS_Id vdmcg::GenCppCharType()
{
  if (get_wchar())
    return ASTAUX::MkId(L"wchar_t *");
  else
    return ASTAUX::MkId(L"char *");
}

// RemoveIntCast
// e : CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::RemoveIntCast(const TYPE_CPP_Expr & e)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    if (e.Is(TAG_TYPE_CPP_FctCall))
    {
      const TYPE_CPP_Expr & fct (e.GetRecord(pos_CPP_FctCall_fct));
      if (fct.Is(TAG_TYPE_CPP_Identifier))
      {
        const SEQ<Char> & id (fct.GetSequence(pos_CPP_Identifier_id));
        if (id == SEQ<Char>(L"UTIL.NumberToInt"))
          return vdm_BC_GenBracketedExpr(e.GetSequence(pos_CPP_FctCall_arg).Hd());
      }
    }
    return e;
  }
  else
#endif // VDMPP
    return (e.Is(TAG_TYPE_CPP_CastExpr) ? vdm_BC_GenBracketedExpr(e.GetRecord(pos_CPP_CastExpr_expr)) : e);
}

