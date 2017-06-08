/***
*  * WHAT
*  *    mod_statedef.cc: Implementation of mod_statedef.vdm 1.17
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/mod_statedef.cc,v $
*  * VERSION
*  *    $Revision: 1.17 $
*  * DATE
*  *    $Date: 2001/10/05 10:58:12 $
*  * STATUS
*  *    $State: Exp $
*  * REFERENCES
*  *    
*  * PROJECT
*  *    Toolbox
*  * AUTHOR
*  *    Henrik Voss + $Author: paulm $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "vdmcg.h"
#include "astaux.h"
#include "asquotes.h"
#include "BC.h"
 
// SD functions and operations

// GenStateDef
// sd : AS`StateDef
// ==> CPP`CPPAS * seq of CPP`Stmt
Tuple vdmcg::GenStateDef(const TYPE_AS_StateDef & sd)
{
  const TYPE_AS_CompositeType & tp (sd.GetRecord(pos_AS_StateDef_tp));
  const Generic & Init (sd.GetField(pos_AS_StateDef_Init));

  TYPE_CPP_CPPAS cc_l;
  const SEQ<TYPE_AS_Field> & field_l (tp.GetSequence(pos_AS_CompositeType_fields));
  size_t len_field_l = field_l.Length();
  for (size_t idx = 1; idx <= len_field_l; idx++)
  {
    const TYPE_AS_Field & f(field_l[idx]);
    TYPE_CPP_Name id (vdm_BC_Rename(f.GetRecord(pos_AS_Field_sel)));
    TYPE_REP_TypeRep ti (FromAS2RepType(f.GetRecord(pos_AS_Field_type)));
    SEQ<TYPE_CPP_DeclSpecifier> ds_l;
    ds_l.ImpAppend(vdm_BC_GenModifier(quote_STATIC));
    ds_l.ImpAppend(GenType(ti));
    cc_l.ImpAppend(vdm_BC_GenIdentDecl(SEQ<TYPE_CPP_Annotation>(), ds_l, id, Nil()));
  }

  SEQ<TYPE_CPP_Stmt> init_l;
  if (!Init.IsNil())
    init_l = GenInitialization(tp, Init);

  return mk_(cc_l, init_l);
}

// GenInitialization
// ct : AS`CompositeType
// tup : AS`StateInit
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenInitialization(const TYPE_AS_CompositeType & ct, const TYPE_AS_StateInit & tup)
{
  SEQ<TYPE_CPP_Stmt> res;
  const TYPE_AS_Name & nm (ct.GetRecord(pos_AS_CompositeType_name));
  const SEQ<TYPE_AS_Field> & field_l (ct.GetSequence(pos_AS_CompositeType_fields));
  const TYPE_AS_Pattern & pat (tup.GetRecord(pos_AS_StateInit_pat));
  const TYPE_AS_Expr & expr (tup.GetRecord(pos_AS_StateInit_expr));

  if (pat.Is(TAG_TYPE_AS_PatternName) && expr.Is(TAG_TYPE_AS_BinaryExpr))
  {
    const TYPE_AS_Expr & lhs     (expr.GetRecord(pos_AS_BinaryExpr_left));
    const TYPE_AS_BinaryOp & opr (expr.GetField(pos_AS_BinaryExpr_opr));
    const TYPE_AS_Expr & rhs     (expr.GetRecord(pos_AS_BinaryExpr_right));

    if (lhs.Is(TAG_TYPE_AS_Name) &&
        (pat.GetField(pos_AS_PatternName_nm) == lhs) &&
	(opr == Int(EQ)) &&
	rhs.Is(TAG_TYPE_AS_RecordConstructorExpr) ) 
    {
      if (rhs.GetRecord(pos_AS_RecordConstructorExpr_tag) == nm)
      {
	const SEQ<TYPE_AS_Expr> & f_l (rhs.GetSequence(pos_AS_RecordConstructorExpr_fields));
	size_t len_f_l = f_l.Length();
	for (size_t i = 1; i <= len_f_l; i++)
	  res.ImpConc(InitStateVar(field_l[i], f_l[i]));
	return res;
      }
    }
  }
  res.ImpAppend(GenUserDefInit());
  return res;
}

// InitStateVar
// field : AS`Field
// expr : AS`Expr
// -> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::InitStateVar(const TYPE_AS_Field & field, const TYPE_AS_Expr & expr)
{
  TYPE_CPP_Expr sel_v (vdm_BC_Rename(field.get_sel()));
  TYPE_AS_Type type (field.get_type());
  TYPE_REP_TypeRep ti (FromAS2RepType(type));
  SEQ<TYPE_CPP_Stmt> cgexpr (CGExpr(expr, mk_CG_VT(sel_v, ti)));
  SEQ<TYPE_CPP_Stmt> res_l;

  if (ti.Is(TAG_TYPE_REP_CompositeTypeRep))
  {
    TYPE_AS_CompositeType ct (type);
    TYPE_CPP_Identifier recCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Record")));
    SEQ<TYPE_CPP_Expr> args_l;
    args_l.ImpAppend(vdm_BC_Rename(ct.get_name())).ImpAppend(vdm_BC_GenLengthName(ct.get_name()));
    TYPE_CPP_Stmt asg (vdm_BC_GenAsgnStmt(sel_v, vdm_BC_GenFctCall(recCall, args_l)));
    res_l.ImpAppend(asg);
  }
  res_l.ImpConc(cgexpr);
  return res_l;
}

// GenUserDefInit
// -> CPP`Stmt
TYPE_CPP_Stmt vdmcg::GenUserDefInit()
{
  TYPE_AS_Id id (GenFileExt(GiveCurCName().ImpConc(ASTAUX::MkId(L"_init"))));
  SEQ<TYPE_CPP_Stmt> l;
  l.ImpAppend(vdm_BC_GenInclStmt(id));
  return vdm_BC_GenBlock(l);
}

