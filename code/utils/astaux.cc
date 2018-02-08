/***
*  * WHAT
*  *    Functions working on the AST nodes
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/utils/astaux.cc,v $
*  * VERSION
*  *    $Revision: 1.46 $
*  * DATE
*  *    $Date: 2006/07/03 05:21:15 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    Toolbox
*  * STATUS
*  *    Under development.
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
*  * SPECIFICATION VERSION
*  *    None -- This file does not correspond to any specifications
***/

#include "metaiv.h"
#include "asquotes.h"
#include "contextinfo.h"
#include "astaux.h"
#include "tb_exceptions.h"
#include "rt_errmsg.h"
#include "tbutils.h"
#include "UTIL.h"

#ifdef VDMSL
Generic ASTAUX::DefaultModASName = Nil();
#endif // VDMSL

// MkName
// tag : seq of char
// -> AS`Name
TYPE_AS_Name ASTAUX::MkName (const wstring & tag)
{
  // create a name record with a identifier sequence
  TYPE_AS_Ids ids (UTIL::split(SEQ<Char>(tag), Char(L'`')));
  return TYPE_AS_Name().Init(ids, NilContextId);
}

#ifdef VDMSL
// 
wstring ASTAUX::GetDefaultModName()
{
  return L"DefaultMod";
}

TYPE_AS_Name ASTAUX::GetDefaultModASName()
{
  if (ASTAUX::DefaultModASName.IsNil()) {
    ASTAUX::DefaultModASName = ASTAUX::MkName(ASTAUX::GetDefaultModName());
  }
  return DefaultModASName;
}
#endif // VDMSL

// Id2String
// id : AS`Id
// -> seq of char
wstring ASTAUX::Id2String(const TYPE_AS_Id & id) 
{
  return id.GetString();
}

// ASName2String
// asname : AS`Name
// -> seq of char
wstring ASTAUX::ASName2String(const TYPE_AS_Name & asname)
{
  const TYPE_AS_Ids & ids (asname.GetSequence(pos_AS_Name_ids));
  switch (ids.Length()) {
    case 0:
      return L"";
    case 1:
      return Id2String(ids.Index(1));
    case 2:
      return Id2String(ids.Index(1)) + L"`" + Id2String(ids.Index(2));
    default: { // ids.Length() > 2 : will not occor this situation
      wstring res (Id2String(ids.Index(1))); 
      for (int64_t i = 2; i <= ids.Length(); i++)
      {
        res += L'`'; 
        res += Id2String(ids.Index(i));
      }
      return res; 
    }
  }
}

// The function GetCid takes a node in the AST
// and returns the condtdxt info entry.

TYPE_CI_ContextId ASTAUX::GetCid(const Record & expr)
{
  if (IsASTRec(expr))
    return expr.GetInt(expr.Length());
  else {
#ifndef TESTSPEC
    vdm_err << L"Internal Error in GetCid: Not an Ast record" << endl << flush;
#endif //TESTSPEC
    return NilContextId;
  }
}

// This function decides if the Record rec is part a node in an AST.
bool ASTAUX::IsASTRec(const Record & rec)
{
  switch (rec.GetTag()) {
#ifdef VDMPP
    case TAG_TYPE_AS_Class:
    case TAG_TYPE_AS_InstanceInv:
    case TAG_TYPE_AS_InstAssignDef:
    case TAG_TYPE_AS_Permission:
    case TAG_TYPE_AS_Mutex:
    case TAG_TYPE_AS_ThreadIdExpr:
    case TAG_TYPE_AS_PerObl:
    case TAG_TYPE_AS_SpoObl:
#endif //VDMPP
#ifdef VDMSL
    case TAG_TYPE_AS_Module:
    case TAG_TYPE_AS_Interface:
    case TAG_TYPE_AS_ImportSig:
    case TAG_TYPE_AS_ExportSig:
    case TAG_TYPE_AS_DLModule:
    case TAG_TYPE_AS_DLInterface:
    case TAG_TYPE_AS_DLExportSig:
    case TAG_TYPE_AS_StateDef:
    case TAG_TYPE_AS_StateInit:
#endif //VDMSL
    case TAG_TYPE_AS_Definitions:
    case TAG_TYPE_AS_TypeDef:
    case TAG_TYPE_AS_Invariant:
    case TAG_TYPE_AS_Order:
    case TAG_TYPE_AS_Equal:

    case TAG_TYPE_AS_BracketedType:
    case TAG_TYPE_AS_BooleanType:
    case TAG_TYPE_AS_NumericType:
    case TAG_TYPE_AS_TokenType:
    case TAG_TYPE_AS_CharType:
    case TAG_TYPE_AS_VoidType:
    case TAG_TYPE_AS_QuoteType:
    case TAG_TYPE_AS_CompositeType:
    case TAG_TYPE_AS_Field:
    case TAG_TYPE_AS_UnionType:
    case TAG_TYPE_AS_ProductType:
    case TAG_TYPE_AS_OptionalType:
    case TAG_TYPE_AS_Set0Type:
    case TAG_TYPE_AS_Set1Type:
    case TAG_TYPE_AS_Seq0Type:
    case TAG_TYPE_AS_Seq1Type:
    case TAG_TYPE_AS_GeneralMap0Type:
    case TAG_TYPE_AS_GeneralMap1Type:
    case TAG_TYPE_AS_InjectiveMap0Type:
    case TAG_TYPE_AS_InjectiveMap1Type:
    case TAG_TYPE_AS_TypeName:
    case TAG_TYPE_AS_PartialFnType:
    case TAG_TYPE_AS_TotalFnType:
    case TAG_TYPE_AS_TypeVar:
    case TAG_TYPE_AS_NameType:
    case TAG_TYPE_AS_OpType:
    case TAG_TYPE_AS_AllType:

    case TAG_TYPE_AS_ExplFnDef:
    case TAG_TYPE_AS_ImplFnDef:
    case TAG_TYPE_AS_ExtExplFnDef:
    case TAG_TYPE_AS_ExplOpDef:
    case TAG_TYPE_AS_ImplOpDef:
    case TAG_TYPE_AS_ExtExplOpDef:

    case TAG_TYPE_AS_FnBody:
    case TAG_TYPE_AS_OpBody:
    case TAG_TYPE_AS_PatTypePair:
    case TAG_TYPE_AS_ExtInf:
    case TAG_TYPE_AS_ValueDef:

    case TAG_TYPE_AS_Name:
    case TAG_TYPE_AS_OldName:
    case TAG_TYPE_AS_BracketedExpr:
    case TAG_TYPE_AS_DefExpr:
    case TAG_TYPE_AS_LetExpr:
    case TAG_TYPE_AS_LetBeSTExpr:
    case TAG_TYPE_AS_IfExpr:
    case TAG_TYPE_AS_ElseifExpr:
    case TAG_TYPE_AS_CasesExpr:
    case TAG_TYPE_AS_CaseAltn:
    case TAG_TYPE_AS_PrefixExpr:
    case TAG_TYPE_AS_BinaryExpr:
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
    case TAG_TYPE_AS_Maplet:
    case TAG_TYPE_AS_MapComprehensionExpr:
    case TAG_TYPE_AS_TupleConstructorExpr:
    case TAG_TYPE_AS_TupleSelectExpr:
    case TAG_TYPE_AS_TypeJudgementExpr:
    case TAG_TYPE_AS_PreConditionApplyExpr:
    case TAG_TYPE_AS_AtomicAssignStmt:
    case TAG_TYPE_AS_TokenConstructorExpr:
    case TAG_TYPE_AS_RecordConstructorExpr:
    case TAG_TYPE_AS_RecordModifierExpr:
    case TAG_TYPE_AS_RecordModification:
    case TAG_TYPE_AS_FieldSelectExpr:
    case TAG_TYPE_AS_FctTypeInstExpr:
    case TAG_TYPE_AS_LambdaExpr:
    case TAG_TYPE_AS_ApplyExpr:
    case TAG_TYPE_AS_IsExpr:
    case TAG_TYPE_AS_NarrowExpr:
    case TAG_TYPE_AS_UndefinedExpr:
#ifdef VDMPP
    case TAG_TYPE_AS_NewExpr:
    case TAG_TYPE_AS_SelfExpr:
    case TAG_TYPE_AS_IsOfClassExpr:
    case TAG_TYPE_AS_IsOfBaseClassExpr:
    case TAG_TYPE_AS_SameBaseClassExpr:
    case TAG_TYPE_AS_SameClassExpr:
    case TAG_TYPE_AS_ActExpr:
    case TAG_TYPE_AS_FinExpr:
    case TAG_TYPE_AS_ActiveExpr:
    case TAG_TYPE_AS_WaitingExpr:
    case TAG_TYPE_AS_ReqExpr:
#ifdef VICE
    case TAG_TYPE_AS_CurTimeExpr:
#endif //VICE
#endif //VDMPP

    case TAG_TYPE_AS_DefStmt:
    case TAG_TYPE_AS_LetStmt:
    case TAG_TYPE_AS_LetBeSTStmt:
    case TAG_TYPE_AS_BlockStmt:
    case TAG_TYPE_AS_AssignDef:
    case TAG_TYPE_AS_AssignStmt:
    case TAG_TYPE_AS_FieldRef:
    case TAG_TYPE_AS_MapOrSeqRef:
    case TAG_TYPE_AS_NarrowRef:
    case TAG_TYPE_AS_SeqForLoopStmt:
    case TAG_TYPE_AS_SetForLoopStmt:
    case TAG_TYPE_AS_IndexForLoopStmt:
    case TAG_TYPE_AS_WhileLoopStmt:
    case TAG_TYPE_AS_ReturnStmt:
    case TAG_TYPE_AS_IfStmt:
    case TAG_TYPE_AS_ElseifStmt:
    case TAG_TYPE_AS_Error:
    case TAG_TYPE_AS_ErrorStmt:
    case TAG_TYPE_AS_AlwaysStmt:
    case TAG_TYPE_AS_TrapStmt:
    case TAG_TYPE_AS_RecTrapStmt:
    case TAG_TYPE_AS_Trap:
    case TAG_TYPE_AS_ExitStmt:
    case TAG_TYPE_AS_NonDetStmt:
    case TAG_TYPE_AS_CallStmt:
    case TAG_TYPE_AS_CasesStmt:
    case TAG_TYPE_AS_CasesStmtAltn:
    case TAG_TYPE_AS_IdentStmt:
    case TAG_TYPE_AS_SpecificationStmt:
    case TAG_TYPE_AS_AssertStmt:
#ifdef VDMPP
    case TAG_TYPE_AS_StartStmt:
    case TAG_TYPE_AS_StartListStmt:
    case TAG_TYPE_AS_StopStmt:
    case TAG_TYPE_AS_StopListStmt:
#ifdef VICE
    case TAG_TYPE_AS_DurationStmt:
    case TAG_TYPE_AS_CycleStmt:
#endif //VICE
#endif //VDMPP

    case TAG_TYPE_AS_PatternName:
    case TAG_TYPE_AS_MatchVal:
    case TAG_TYPE_AS_SetEnumPattern:
    case TAG_TYPE_AS_SetUnionPattern:
    case TAG_TYPE_AS_SeqEnumPattern:
    case TAG_TYPE_AS_SeqConcPattern:
    case TAG_TYPE_AS_MapEnumPattern:
    case TAG_TYPE_AS_MapMergePattern:
    case TAG_TYPE_AS_MapletPattern:
    case TAG_TYPE_AS_TuplePattern:
    case TAG_TYPE_AS_RecordPattern:
#ifdef VDMPP
    case TAG_TYPE_AS_ObjectPattern:
    case TAG_TYPE_AS_FieldPattern:
#endif // VDMPP

    case TAG_TYPE_AS_SetBind:
    case TAG_TYPE_AS_TypeBind:
    case TAG_TYPE_AS_SeqBind:
    case TAG_TYPE_AS_MultSetBind:
    case TAG_TYPE_AS_MultTypeBind:

    case TAG_TYPE_AS_BoolLit:
    case TAG_TYPE_AS_NilLit:
    case TAG_TYPE_AS_RealLit:
    case TAG_TYPE_AS_NumLit:
    case TAG_TYPE_AS_CharLit:
    case TAG_TYPE_AS_TextLit:
    case TAG_TYPE_AS_QuoteLit:

    case TAG_TYPE_AS_TraceDefAltn:
    case TAG_TYPE_AS_TraceApplyExpr:
    case TAG_TYPE_AS_TraceBracketedExpr:
    case TAG_TYPE_AS_TracePermuteExpr:
    case TAG_TYPE_AS_QualifiedTrace:
    case TAG_TYPE_AS_LocalTraceBind:
    case TAG_TYPE_AS_LetTraceBind:
    case TAG_TYPE_AS_LetBeTraceBind:
    case TAG_TYPE_AS_RepeatTrace:
    case TAG_TYPE_AS_RepeatFixed:
    case TAG_TYPE_AS_RepeatInterval:
    case TAG_TYPE_AS_QualifiedRepeatTrace:
    case TAG_TYPE_AS_Macro:
    case TAG_TYPE_AS_LastRes:
      return true;
    
    default:
      return false;
  }
}

// GetFirstName
// nm : AS`Name
// +> AS`Name
TYPE_AS_Name ASTAUX::GetFirstName(const TYPE_AS_Name & nm)
{
  return TYPE_AS_Name().Init(TYPE_AS_Ids().ImpAppend(GetFirstId(nm)), nm.GetInt(pos_AS_Name_cid));
}

// GetSecondName
// nm : AS`Name
// +> AS`Name
TYPE_AS_Name ASTAUX::GetSecondName(const TYPE_AS_Name & nm)
{
  return TYPE_AS_Name().Init(TYPE_AS_Ids().ImpAppend(GetSecondId(nm)), nm.GetInt(pos_AS_Name_cid));
}

// Combine2Names
// mod : AS`Name
// nm : AS`Name
// -> AS`Name
TYPE_AS_Name ASTAUX::Combine2Names(const TYPE_AS_Name & mod, const TYPE_AS_Name & nm)
{
  return TYPE_AS_Name().Init(TYPE_AS_Ids().ImpAppend(GetFirstId(mod)).ImpAppend(GetLastId(nm)),
                             nm.GetInt(pos_AS_Name_cid));
}

// GetOpAccess (not in spec)
// op : AS`OpDef
// -> AS`Access
TYPE_AS_Access ASTAUX::GetOpAccess(const TYPE_AS_OpDef& op)
{
  switch (op.GetTag()) {
    case TAG_TYPE_AS_ExplOpDef:    { return op.GetField(pos_AS_ExplOpDef_access); }
    case TAG_TYPE_AS_ImplOpDef:    { return op.GetField(pos_AS_ImplOpDef_access); }
    case TAG_TYPE_AS_ExtExplOpDef: { return op.GetField(pos_AS_ExtExplOpDef_access); }
    default:                       { return Int (DEFAULT_AS); }// dummy
  }
}

// GetFnAccess (not in spec)
// fn : AS`FnDef
// -> AS`Access
TYPE_AS_Access ASTAUX::GetFnAccess(const TYPE_AS_FnDef& fn)
{
  switch (fn.GetTag()) {
    case TAG_TYPE_AS_ExplFnDef:    { return fn.GetField(pos_AS_ExplFnDef_access); }
    case TAG_TYPE_AS_ImplFnDef:    { return fn.GetField(pos_AS_ImplFnDef_access); }
    case TAG_TYPE_AS_ExtExplFnDef: { return fn.GetField(pos_AS_ExtExplFnDef_access); }
    default:                       { return Int (DEFAULT_AS); } // dummy
  }
}

// GetOpBody (not in spec)
// op : AS`OpDef
// -> AS`OpBody
TYPE_AS_OpBody ASTAUX::GetOpBody(const TYPE_AS_OpDef& op)
{
  switch (op.GetTag()) {
    case TAG_TYPE_AS_ExplOpDef:    { return op.GetRecord(pos_AS_ExplOpDef_body); }
    case TAG_TYPE_AS_ExtExplOpDef: { return op.GetRecord(pos_AS_ExtExplOpDef_body); }
    case TAG_TYPE_AS_ImplOpDef:
    default:                       { return TYPE_AS_OpBody(); }// dummy
  }
}

// GetFnBody (not in spec)
// fn : AS`FnDef
// -> AS`FnBody
TYPE_AS_FnBody ASTAUX::GetFnBody(const TYPE_AS_FnDef& fn)
{
  switch (fn.GetTag()) {
    case TAG_TYPE_AS_ExplFnDef:    { return fn.GetRecord(pos_AS_ExplFnDef_body); }
    case TAG_TYPE_AS_ExtExplFnDef: { return fn.GetRecord(pos_AS_ExtExplFnDef_body); }
    case TAG_TYPE_AS_ImplFnDef:
    default:                       { return TYPE_AS_FnBody(); } // dummy
  }
}

// GetOpName (not in spec)
// op : AS`OpDef
// -> AS`Name
TYPE_AS_Name ASTAUX::GetOpName (const TYPE_AS_OpDef & op)
{
  switch (op.GetTag()) {
    case TAG_TYPE_AS_ExplOpDef:    { return op.GetRecord(pos_AS_ExplOpDef_nm); }
    case TAG_TYPE_AS_ImplOpDef:    { return op.GetRecord(pos_AS_ImplOpDef_nm); }
    case TAG_TYPE_AS_ExtExplOpDef: { return op.GetRecord(pos_AS_ExtExplOpDef_nm); }
    default:                       { return MkNameFromVoid(); } // dummy
  }
}

// GetFnName (not in spec)
// op : AS`OpDef
// -> AS`Name
TYPE_AS_Name ASTAUX::GetFnName (const TYPE_AS_FnDef & fn)
{
  switch (fn.GetTag()) {
    case TAG_TYPE_AS_ExplFnDef:    { return fn.GetRecord(pos_AS_ExplFnDef_nm); }
    case TAG_TYPE_AS_ImplFnDef:    { return fn.GetRecord(pos_AS_ImplFnDef_nm); }
    case TAG_TYPE_AS_ExtExplFnDef: { return fn.GetRecord(pos_AS_ExtExplFnDef_nm); }
    default:                       { return MkNameFromVoid();} // dummy
  }
}

// GetOpStatic (not in spec)
// op : AS`OpDef
// -> AS`Name
Bool ASTAUX::GetOpStatic (const TYPE_AS_OpDef & op)
{
  switch (op.GetTag()) {
    case TAG_TYPE_AS_ExplOpDef:    { return op.GetBool(pos_AS_ExplOpDef_stat); }
    case TAG_TYPE_AS_ImplOpDef:    { return op.GetBool(pos_AS_ImplOpDef_stat); }
    case TAG_TYPE_AS_ExtExplOpDef: { return op.GetBool(pos_AS_ExtExplOpDef_stat); }
    default:                       { return Bool(false); } // dummy
  }
}

// GetOpPureOp (not in spec)
// op : AS`OpDef
// -> AS`Name
Bool ASTAUX::GetOpPureOp (const TYPE_AS_OpDef & op)
{
  switch (op.GetTag()) {
    case TAG_TYPE_AS_ExplOpDef:    { return op.GetBool(pos_AS_ExplOpDef_oppure); }
    case TAG_TYPE_AS_ImplOpDef:    { return op.GetBool(pos_AS_ImplOpDef_oppure); }
    case TAG_TYPE_AS_ExtExplOpDef: { return op.GetBool(pos_AS_ExtExplOpDef_oppure); }
    default:                       { return Bool(false); } // dummy
  }
}

// GetFnStatic (not in spec)
// op : AS`FnDef
// -> bool
Bool ASTAUX::GetFnStatic (const TYPE_AS_FnDef & fn)
{
  switch (fn.GetTag()) {
    case TAG_TYPE_AS_ExplFnDef:    { return fn.GetBool(pos_AS_ExplFnDef_stat); }
    case TAG_TYPE_AS_ImplFnDef:    { return fn.GetBool(pos_AS_ImplFnDef_stat); }
    case TAG_TYPE_AS_ExtExplFnDef: { return fn.GetBool(pos_AS_ExtExplFnDef_stat); }
    default:                       { return Bool(false); } // dummy
  }
}

// GetConstr (not in spec)
// op : AS`FnDef
// -> bool
bool ASTAUX::GetConstr (const TYPE_AS_OpDef & opdef)
{
  switch (opdef.GetTag()) {
    case TAG_TYPE_AS_ExplOpDef:    { return opdef.GetBool(pos_AS_ExplOpDef_constr).GetValue(); }
    case TAG_TYPE_AS_ImplOpDef:    { return opdef.GetBool(pos_AS_ImplOpDef_constr).GetValue(); }
    case TAG_TYPE_AS_ExtExplOpDef: { return opdef.GetBool(pos_AS_ExtExplOpDef_constr).GetValue(); }
    default:                       { return false; } // dummy
  }
}

// GetOpPrecond (not in spec)
// op : AS`OpDef
// -> [AS`Expr]
Generic ASTAUX::GetOpPrecond (const TYPE_AS_OpDef & opdef)
{
  switch (opdef.GetTag()) {
    case TAG_TYPE_AS_ExplOpDef:    { return opdef.GetField(pos_AS_ExplOpDef_oppre); }
    case TAG_TYPE_AS_ImplOpDef:    { return opdef.GetField(pos_AS_ImplOpDef_oppre); }
    case TAG_TYPE_AS_ExtExplOpDef: { return opdef.GetField(pos_AS_ExtExplOpDef_oppre); }
    default:                       { return Nil(); } // dummy
  }
}

// GetFnPrecond (not in spec)
// op : AS`FnDef
// -> [AS`Expr]
Generic ASTAUX::GetFnPrecond (const TYPE_AS_FnDef & fndef)
{
  switch (fndef.GetTag()) {
    case TAG_TYPE_AS_ExplFnDef:    { return fndef.GetField(pos_AS_ExplFnDef_fnpre); }
    case TAG_TYPE_AS_ImplFnDef:    { return fndef.GetField(pos_AS_ImplFnDef_fnpre); }
    case TAG_TYPE_AS_ExtExplFnDef: { return fndef.GetField(pos_AS_ExtExplFnDef_fnpre); }
    default:                       { return Nil(); } // dummy
  }
}

// GetOpPostcond (not in spec)
// opdef : AS`OpDef
// -> [AS`Expr]
Generic ASTAUX::GetOpPostcond (const TYPE_AS_OpDef & opdef)
{
  switch (opdef.GetTag()) {
    case TAG_TYPE_AS_ExplOpDef:    { return opdef.GetField(pos_AS_ExplOpDef_oppost); }
    case TAG_TYPE_AS_ImplOpDef:    { return opdef.GetField(pos_AS_ImplOpDef_oppost); }
    case TAG_TYPE_AS_ExtExplOpDef: { return opdef.GetField(pos_AS_ExtExplOpDef_oppost); }
    default:                       { return Nil(); } // dummy
  }
}

// GetFnPostcond (not in spec)
// fndef : AS`FnDef
// -> [AS`Expr]
Generic ASTAUX::GetFnPostcond (const TYPE_AS_FnDef & fndef)
{
  switch (fndef.GetTag()) {
    case TAG_TYPE_AS_ExplFnDef:    { return fndef.GetField(pos_AS_ExplFnDef_fnpost); }
    case TAG_TYPE_AS_ImplFnDef:    { return fndef.GetField(pos_AS_ImplFnDef_fnpost); }
    case TAG_TYPE_AS_ExtExplFnDef: { return fndef.GetField(pos_AS_ExtExplFnDef_fnpost); }
    default:                       { return Nil(); } // dummy
  }
}

// UnqualiName
// p_nm : AS`Name
// -> AS`Name
TYPE_AS_Name ASTAUX::UnqualiName(const TYPE_AS_Name& p_nm)
{
  return TYPE_AS_Name().Init(TYPE_AS_Ids().ImpAppend(ASTAUX::GetLastId(p_nm)),
                             p_nm.GetInt(pos_AS_Name_cid));
}

// GetFnParms
// fn : AS`FnDef
// -> seq of AS`Type
SEQ<TYPE_AS_Type> ASTAUX::GetFnParms(const TYPE_AS_FnDef & fn)
{
  switch (fn.GetTag()) {
    case TAG_TYPE_AS_ExplFnDef: {
      return fn.GetRecord(pos_AS_ExplFnDef_tp).GetSequence(1);
    }
    case TAG_TYPE_AS_ImplFnDef: {
      return ConstructImplDomType(fn.GetSequence(pos_AS_ImplFnDef_partps));
    }
    case TAG_TYPE_AS_ExtExplFnDef: {
      return ConstructImplDomType(fn.GetSequence(pos_AS_ExtExplFnDef_partps));
    }
    default: {
#ifndef TESTSPEC
      vdm_err << L"internal error in GetFnParms.";
#endif //TESTSPEC
      throw TB_Exception(ERR_IP);
    }
  }
}

// GetOpParms
// op : AS`OpDef
// -> seq of AS`Type
SEQ<TYPE_AS_Type> ASTAUX::GetOpParms(const TYPE_AS_OpDef & op)
{
  switch (op.GetTag()) {
    case TAG_TYPE_AS_ExplOpDef: {
      return op.GetRecord(pos_AS_ExplOpDef_tp).GetSequence(pos_AS_OpType_opdom);
    }
    case TAG_TYPE_AS_ImplOpDef: {
      return ConstructImplDomType(op.GetSequence(pos_AS_ImplOpDef_partps));
    }
    case TAG_TYPE_AS_ExtExplOpDef: {
      return ConstructImplDomType(op.GetSequence(pos_AS_ExtExplOpDef_partps));
    }
    default: {
#ifndef TESTSPEC
      vdm_err << L"internal error in GetOpParms.";
#endif //TESTSPEC
      throw TB_Exception(ERR_IP);
    }
  }
}

// GetFnRestype
// fn : AS`FnDef
// -> AS`Type
TYPE_AS_Type ASTAUX::GetFnRestype(const TYPE_AS_FnDef& fn)
{
  switch (fn.GetTag()) {
    case TAG_TYPE_AS_ExplFnDef: {
      return fn.GetRecord(pos_AS_ExplFnDef_tp).GetRecord(2);
    }
    case TAG_TYPE_AS_ImplFnDef: {
      return ConstructImplRngType(fn.GetSequence(pos_AS_ImplFnDef_resnmtps));
    }
    case TAG_TYPE_AS_ExtExplFnDef: {
      return ConstructImplRngType(fn.GetSequence(pos_AS_ExtExplFnDef_resnmtps));
    }
    default: {
#ifndef TESTSPEC
      vdm_err << L"internal error in GetFnRestype.";
#endif //TESTSPEC
      throw TB_Exception(ERR_IP);
    }
  }
}

// ConstructImplDomType
// partps : seq of AS`PatTypePair
// -> seq of AS`Type
SEQ<TYPE_AS_Type> ASTAUX::ConstructImplDomType(const SEQ<TYPE_AS_PatTypePair> & partps)
{
  SEQ<TYPE_AS_Type> result;
  size_t len_partps = partps.Length();
  for (size_t i = 1; i <= len_partps; i++)
  {
    const TYPE_AS_PatTypePair & ptp (partps[i]);
    const TYPE_AS_Type & tp (ptp.GetRecord(pos_AS_PatTypePair_tp));
    size_t len_ptp = ptp.GetSequence(pos_AS_PatTypePair_pats).Length();
    for (size_t j = 1; j <= len_ptp; j++)
      result.ImpAppend(tp);
  }
  return result;
}

// ConstructImplRngType
// restps : seq of AS`NameType
// -> [AS`Type]
TYPE_AS_Type ASTAUX::ConstructImplRngType(const SEQ<TYPE_AS_NameType> & restps)
{
  switch(restps.Length()) {
    case 0:
      return TYPE_AS_VoidType().Init(NilContextId);
    case 1:
      return restps[1].GetRecord(pos_AS_NameType_tp);
    default: {
      SEQ<TYPE_AS_Type> tps;
      size_t len_restps = restps.Length();
      for (size_t idx = 1; idx <= len_restps; idx++)
        tps.ImpAppend(restps[idx].GetRecord(pos_AS_NameType_tp));
      return TYPE_AS_ProductType().Init(tps, NilContextId);
    }
  }
}

// GetFnMeasu
// fn : AS`FnDef
// -> [AS`Name]
Generic ASTAUX::GetFnMeasu(const TYPE_AS_FnDef & fn)
{
  switch (fn.GetTag()) {
    case TAG_TYPE_AS_ExplFnDef:    { return fn.GetField(pos_AS_ExplFnDef_measu); }
    case TAG_TYPE_AS_ExtExplFnDef: { return fn.GetField(pos_AS_ExtExplFnDef_measu); }
    case TAG_TYPE_AS_ImplFnDef:    { return Nil(); }
    default: {
#ifndef TESTSPEC
      vdm_err << L"internal error in GetFnMeasu.";
#endif //TESTSPEC
      throw TB_Exception(ERR_IP);
    }
  }
}


// Set -> Sequence (reverse order)
// SetToSequenceR (not in spec)
// s : set
// +> seq
Sequence ASTAUX::SetToSequenceR(const Set & s)
{
  return s.ToSequence().Reverse();
}

// BindToMultBind (not in spec)
// bind : AS`Bind
// +> AS`MultBind
TYPE_AS_MultBind ASTAUX::BindToMultBind(const TYPE_AS_Bind & bind)
{
  switch(bind.GetTag()) {
    case TAG_TYPE_AS_SetBind: {
      return TYPE_AS_MultSetBind().Init(SEQ<TYPE_AS_Pattern>().ImpAppend(bind.GetRecord(pos_AS_SetBind_pat)),
                                        bind.GetRecord(pos_AS_SetBind_Set),
                                        bind.GetInt(pos_AS_SetBind_cid));
      break;
    }
    case TAG_TYPE_AS_TypeBind: {
      return TYPE_AS_MultTypeBind().Init(SEQ<TYPE_AS_Pattern>().ImpAppend(bind.GetRecord(pos_AS_TypeBind_pat)),
                                         bind.GetRecord(pos_AS_TypeBind_tp),
                                         bind.GetInt(pos_AS_TypeBind_cid));
      break;
    }
    default: {
      return TYPE_AS_MultBind(); // dummy
      break;
    }
  }
}

// BindToBindList (not in spec)
// bind : AS`Bind
// +> AS`BindLst
SEQ<TYPE_AS_MultBind> ASTAUX::BindToBindList(const TYPE_AS_Bind & bind)
{
  return SEQ<TYPE_AS_MultBind>().ImpAppend(ASTAUX::BindToMultBind(bind));
}

// MultBindToBindSeq (not in spec)
// bind : AS`MultBind
// +> seq of AS`Bind
SEQ<TYPE_AS_Bind> ASTAUX::MultBindToBindSeq(const TYPE_AS_MultBind & bind)
{
  SEQ<TYPE_AS_Bind> bind_l;
  switch(bind.GetTag()) {
    case TAG_TYPE_AS_MultSetBind: {
      const SEQ<TYPE_AS_Pattern> & pat_l (bind.GetSequence(pos_AS_MultSetBind_pat));
      const TYPE_AS_Expr & e (bind.GetRecord(pos_AS_MultSetBind_Set));
      const TYPE_CI_ContextId & cid (bind.GetInt(pos_AS_MultSetBind_cid));
      int len_pat_l = pat_l.Length();
      for (int idx = 1; idx <= len_pat_l; idx++)
      {
        bind_l.ImpAppend(TYPE_AS_SetBind().Init(pat_l[idx], e, cid));
      }
      break;
    }
    case TAG_TYPE_AS_MultTypeBind: {
      const SEQ<TYPE_AS_Pattern> & pat_l (bind.GetSequence(pos_AS_MultTypeBind_pat));
      const TYPE_AS_Type & tp (bind.GetRecord(pos_AS_MultTypeBind_tp));
      const TYPE_CI_ContextId & cid (bind.GetInt(pos_AS_MultTypeBind_cid));
      int len_pat_l = pat_l.Length();
      for (int idx = 1; idx <= len_pat_l; idx++)
      {
        bind_l.ImpAppend(TYPE_AS_TypeBind().Init(pat_l[idx], tp, cid));
      }
      break;
    }
  }
  return bind_l;
}

// BindListToBindSeq (not in spec)
// binds : AS`BindList
// +> seq of AS`Bind
SEQ<TYPE_AS_Bind> ASTAUX::BindListToBindSeq(const SEQ<TYPE_AS_MultBind> & binds)
{
  SEQ<TYPE_AS_Bind> bind_l;
  size_t len_binds = binds.Length();
  for (size_t idx = 1; idx <= len_binds; idx++)
  {
    bind_l.ImpConc(MultBindToBindSeq(binds[idx]));
  }
  return bind_l;
}

// MargeBindList (not in spec)
// bind_l : AS`BindList
// +> AS`BindLst
SEQ<TYPE_AS_MultBind> ASTAUX::MargeBindList(const SEQ<TYPE_AS_MultBind> & bind_l)
{
// marge bind pat
// a, b in set s, c in set s -> a, b, c in set s
  Sequence expr_l; // seq of (AS`Expr | AS`Type)
  Map m; // map AS`Expr to AS`MultBind
  size_t len_bind_l = bind_l.Length();
  for (size_t idx = 1; idx <= len_bind_l; idx++)
  {
    const TYPE_AS_MultBind & bind (bind_l[idx]);
    switch(bind.GetTag()) {
      case TAG_TYPE_AS_MultSetBind: {
        const SEQ<TYPE_AS_Pattern> & pat_l (bind.GetSequence(pos_AS_MultSetBind_pat));
        const TYPE_AS_Expr & e_set (bind.GetRecord(pos_AS_MultSetBind_Set));
        if (expr_l.Elems().InSet(e_set))
        {
          TYPE_AS_MultSetBind b (m[e_set]);
          SEQ<TYPE_AS_Pattern> p_l (b.GetSequence(pos_AS_MultSetBind_pat));
          p_l.ImpConc(pat_l);
          b.SetField(pos_AS_MultSetBind_pat, p_l);
          m.ImpModify(e_set, b);
        }
        else
        {
          expr_l.ImpAppend(e_set);
          m.ImpModify(e_set, bind);
        }
        break;
      }
      case TAG_TYPE_AS_MultTypeBind: {
        const SEQ<TYPE_AS_Pattern> & pat_l (bind.GetSequence(pos_AS_MultTypeBind_pat));
        const TYPE_AS_Type & e_tp (bind.GetRecord(pos_AS_MultTypeBind_tp));
        if (expr_l.Elems().InSet(e_tp))
        {
          TYPE_AS_MultTypeBind b (m[e_tp]);
          SEQ<TYPE_AS_Pattern> p_l (b.GetSequence(pos_AS_MultTypeBind_pat));
          p_l.ImpConc(pat_l);
          b.SetField(pos_AS_MultTypeBind_pat, p_l);
          m.ImpModify(e_tp, b);
        }
        else
        {
          expr_l.ImpAppend(e_tp);
          m.ImpModify(e_tp, bind);
        }
        break;
      }
    }
  }
  SEQ<TYPE_AS_MultBind> new_bind;
  int len_expr_l = expr_l.Length();
  for (int i = 1; i <= len_expr_l; i++)
  {
    new_bind.ImpAppend(m[expr_l[i]]);
  }
  return new_bind;
}

#ifdef VDMPP
// ChangeDocumentToStatic (not in spec)
// cs : AS`Document
// +> AS`Document
TYPE_AS_Document ASTAUX::ChangeDocumentToStatic(const TYPE_AS_Document & cs)
{
  TYPE_AS_Document new_cs;
  size_t len_cs = cs.Length();
  for (size_t n = 1; n <= len_cs; n++)
  {
    if(cs[n].Is(TAG_TYPE_AS_Class))
    {
      new_cs.ImpAppend(ChangeClassToStatic(cs[n]));
    }
    else
    {
      new_cs.ImpAppend(cs[n]);
    }
  }
  return new_cs;
}

// ChangeClassToStatic (not in spec)
// cl : AS`Class
// +> AS`Class
TYPE_AS_Class ASTAUX::ChangeClassToStatic(const TYPE_AS_Class & cl)
{
  TYPE_AS_Class new_cl (cl);
  Generic defs_g (cl.GetField(pos_AS_Class_defs));

  if( !defs_g.IsNil() )
  {
    TYPE_AS_Definitions defs (defs_g);
    SEQ<TYPE_AS_ValueDef> valuem (defs.GetSequence(pos_AS_Definitions_valuem));
    Map fnm (defs.GetMap(pos_AS_Definitions_fnm));
    size_t len_valuem = valuem.Length();
    SEQ<TYPE_AS_ValueDef> new_valuem;
    for (size_t idx = 1; idx <= len_valuem; idx++)
    {
      TYPE_AS_ValueDef vd (valuem[idx]);
      vd.SetField(pos_AS_ValueDef_stat, Bool(true));
      new_valuem.ImpAppend(vd);
    }

    Set dom_fnm (fnm.Dom());
    Map new_fnm;
    Generic nm;
    for (bool bb = dom_fnm.First(nm); bb; bb = dom_fnm.Next(nm))
    {
      Record fn (fnm[nm]);
      switch(fn.GetTag()) {
        case TAG_TYPE_AS_ImplFnDef: {
          fn.SetField(pos_AS_ImplFnDef_stat, Bool(true));
          new_fnm.ImpModify(nm, fn);
          break;
        }
        case TAG_TYPE_AS_ExplFnDef: {
          fn.SetField(pos_AS_ExplFnDef_stat, Bool(true));
          new_fnm.ImpModify(nm, fn);
          break;
        }
        case TAG_TYPE_AS_ExtExplFnDef: {
          fn.SetField(pos_AS_ExtExplFnDef_stat, Bool(true));
          new_fnm.ImpModify(nm, fn);
          break;
        }
      }
    }
    defs.SetField(pos_AS_Definitions_valuem, new_valuem);
    defs.SetField(pos_AS_Definitions_fnm, new_fnm);
    new_cl.SetField(pos_AS_Class_defs, defs);
  }
  return new_cl;
}

bool ASTAUX::IsSubrespFnOp(const Record & def)
{
  Generic body = Nil();
  switch(def.GetTag()) {
    case TAG_TYPE_AS_ExplFnDef: {
      body = def.GetRecord(pos_AS_ExplFnDef_body).GetField(pos_AS_FnBody_body);
      break;
    }
    case TAG_TYPE_AS_ExtExplFnDef: {
      body = def.GetRecord(pos_AS_ExtExplFnDef_body).GetField(pos_AS_FnBody_body);
      break;
    }
    case TAG_TYPE_AS_ExplOpDef: {
      body = def.GetRecord(pos_AS_ExplOpDef_body).GetField(pos_AS_OpBody_body);
      break;
    }
    case TAG_TYPE_AS_ExtExplOpDef: {
      body = def.GetRecord(pos_AS_ExtExplOpDef_body).GetField(pos_AS_OpBody_body);
      break;
    }
  }
  return (body == Int(SUBRESP));
}
#endif // VDMPP
