#include "intconvquotes.h"
#include "astaux.h"
#include "statsem.h"

#ifdef DEF_CI_uAUX_GetCid
TYPE_CI_ContextId vdm_CI_uAUX_GetCid (const Record & expr)
{
  return ASTAUX::GetCid(expr);
}
#endif // DEF_CI_uAUX_GetCid

#ifdef DEF_CI_uAUX_GetASType
TYPE_AS_Type vdm_CI_uAUX_GetASType (const Record & expr)
{
  return CIAUX::GetASType(ASTAUX::GetCid(expr));
}
#endif // DEF_CI_uAUX_GetASType

#ifdef DEF_CI_uAUX_HasInvariant
Bool vdm_CI_uAUX_HasInvariant (const TYPE_AS_Type & type)
{
  return Bool(CIAUX::HasInvariant(type));
}
#endif // DEF_CI_uAUX_HasInvariant

#ifdef DEF_CI_uAUX_LookUpFieldsFromTag
type_dL vdm_CI_uAUX_LookUpFieldsFromTag (const TYPE_AS_Name & tag)
{
  return CIAUX::LookUpFieldsFromTag(tag);
}
#endif // DEF_CI_uAUX_LookUpFieldsFromTag

#ifdef DEF_CI_uAUX_LookUpGroundType
Record vdm_CI_uAUX_LookUpGroundType (const TYPE_AS_Name & nm)
{
  return CIAUX::LookUpGroundType(nm);
}
#endif // DEF_CI_uAUX_LookUpGroundType

#ifdef DEF_CI_uAUX_LookUpGroundTypeRep
TYPE_REP_TypeRep vdm_CI_uAUX_LookUpGroundTypeRep (const TYPE_AS_Name & nm)
{
  return CIAUX::LookUpGroundTypeRep(nm);
}
#endif // DEF_CI_uAUX_LookUpGroundTypeRep

#ifdef DEF_CI_uAUX_LookUpInvOrGroundTypeRep
TYPE_REP_TypeRep vdm_CI_uAUX_LookUpInvOrGroundTypeRep (const TYPE_AS_Name & nm)
{
  return CIAUX::LookUpInvOrGroundTypeRep(nm);
}
#endif // DEF_CI_uAUX_LookUpInvOrGroundTypeRep

#ifdef DEF_CI_uAUX_LookUpInvOrGroundType
TYPE_AS_Type vdm_CI_uAUX_LookUpInvOrGroundType (const TYPE_AS_Name & nm)
{
  return CIAUX::LookUpInvOrGroundType(nm);
}
#endif // DEF_CI_uAUX_LookUpInvOrGroundType

#ifdef DEF_CI_uAUX_GetGroundTypeRepInfo
TYPE_REP_TypeRep vdm_CI_uAUX_GetGroundTypeRepInfo (const TYPE_CI_ContextId & cid)
{
  return CIAUX::GetGroundTypeRepInfo(cid);
}
#endif // DEF_CI_uAUX_GetGroundTypeRepInfo

#ifdef DEF_CI_uAUX_HasMemberInvariant
Bool vdm_CI_uAUX_HasMemberInvariant (const TYPE_AS_Name & clnm)
{
  return Bool(CIAUX::HasMemberInvariant(clnm));
}
#endif // DEF_CI_uAUX_HasMemberInvariant

#ifdef VDMSL
#ifdef DEF_CI_uAUX_GetMemberNameDef
Generic vdm_CI_uAUX_GetMemberNameDef (const TYPE_AS_Name & nm, const Generic & clnm, const Generic & tp)
{
  return CIAUX::GetMemberNameDef(nm, clnm, tp);
}
#endif // DEF_CI_uAUX_GetMemberNameDef
#endif // VDMSL

#ifdef VDMPP
#ifdef DEF_CI_uAUX_LookUpNames
type_dS vdm_CI_uAUX_LookUpNames (const type_17REP_ObjRefTypeRepCS & objrefs, const TYPE_AS_Name & metnm)
{
  return CIAUX::LookUpNames(objrefs, metnm);
}
#endif // DEF_CI_uAUX_LookUpNames

#ifdef DEF_CI_uAUX_IsSubType
Bool vdm_CI_uAUX_IsSubType (const TYPE_REP_TypeRep & TpR1, const TYPE_REP_TypeRep & TpR2,
                            const type_15REP_TypeNameRepC15REP_TypeNameRepC2PS & assump)
{
  return Bool(CIAUX::IsSubType(TpR1, TpR2, assump));
}
#endif // DEF_CI_uAUX_IsSubType

#ifdef DEF_CI_uAUX_NormaliseTypeName
TYPE_REP_TypeRep vdm_CI_uAUX_NormaliseTypeName (const TYPE_REP_TypeRep & tp)
{
  return CIAUX::NormaliseTypeName(tp);
}
#endif // DEF_CI_uAUX_NormaliseTypeName

#ifdef DEF_CI_uAUX_IsClassName
Bool vdm_CI_uAUX_IsClassName (const TYPE_AS_Name & nm)
{
  return Bool(CIAUX::IsClassName(nm));
}
#endif // DEF_CI_uAUX_IsClassName

#ifdef DEF_CI_uAUX_GetMemberNames
type_7AS_NameCS vdm_CI_uAUX_GetMemberNames (const TYPE_AS_Name & clnm)
{
  return CIAUX::GetMemberNames(clnm);
}
#endif // DEF_CI_uAUX_GetMemberNames

#ifdef DEF_CI_uAUX_CheckOperationCall
type_bUd3P vdm_CI_uAUX_CheckOperationCall (const type_7AS_NameCS & classes, const TYPE_AS_Name & nm,
                                           const Generic & objnm, const type_dL & etp)
{
  return CIAUX::CheckOperationCall(classes, nm, objnm, etp);
}
#endif // DEF_CI_uAUX_CheckOperationCall

#ifdef DEF_CI_uAUX_GetMemberNameDef
Generic vdm_CI_uAUX_GetMemberNameDef (const TYPE_AS_Name & nm, const Generic & clnm, const Generic & tp)
{
  return CIAUX::GetMemberNameDef(nm, slnm, tp);
}
#endif // DEF_CI_uAUX_GetMemberNameDef
#endif // VDMPP

#ifdef DEF_CI_uAUX_GetMangledFnOpName
TYPE_AS_Expr vdm_CI_uAUX_GetMangledFnOpName (const TYPE_AS_Expr & fct, const type_dL & args)
{
  return CIAUX::GetMangledFnOpName(fct, args);
}
#endif // DEF_CI_uAUX_GetMangledFnOpName

#ifdef DEF_CI_uAUX_GetNameFromStateDesignator
TYPE_AS_Name vdm_CI_uAUX_GetNameFromStateDesignator (const TYPE_AS_StateDesignator & sd)
{
  return CIAUX::GetNameFromStateDesignator(sd);
}
#endif // DEF_CI_uAUX_GetNameFromStateDesignator

#ifdef DEF_CI_uAUX_IsNamePartOfState
Bool vdm_CI_uAUX_IsNamePartOfState (const TYPE_AS_Name & nm, const Generic & modnm)
{
  return Bool(CIAUX::IsNamePartOfState(nm, modnm));
}
#endif // DEF_CI_uAUX_IsNamePartOfState

#ifdef DEF_CI_uAUX_isFunctionName
Bool vdm_CI_uAUX_isFunctionName (const type_cL & name)
{
  return Bool(CIAUX::isFunctionName(name));
}
#endif // DEF_CI_uAUX_isFunctionName

#ifdef DEF_CI_uAUX_getFuncReturnInfo
TYPE_CI_uAUX_FuncReturnInfo vdm_CI_uAUX_getFuncReturnInfo (const type_cL & name)
{
  return CIAUX::getFuncReturnInfo(name);
}
#endif // DEF_CI_uAUX_getFuncReturnInfo

#ifdef DEF_CI_uAUX_isTypeName
Bool vdm_CI_uAUX_isTypeName (const type_cL & name)
{
  return Bool(CIAUX::isTypeName(name));
}
#endif // DEF_CI_uAUX_isTypeName

#ifdef DEF_CI_uAUX_getTypeDef
TYPE_AS_TypeDef vdm_CI_uAUX_getTypeDef (const type_cL & name)
{
  return CIAUX::getTypeDef(name);
}
#endif // DEF_CI_uAUX_getTypeDef

#ifdef DEF_CI_uAUX_isValueName
Bool vdm_CI_uAUX_isValueName (const type_cL & name)
{
  return Bool(CIAUX::isValueName(name));
}
#endif // DEF_CI_uAUX_isValueName

// GetASType
// cid : CI`ContextId
// -> AS`Type
TYPE_AS_Type CIAUX::GetASType (const TYPE_CI_ContextId & cid)
{
  TYPE_REP_TypeRep tp_rep_q (INT2Q::h2gAS(GetCI().GetTypeInfo(cid))); // [REP`TypeRep]
  TYPE_REP_TypeRep tp_rep = tp_rep_q;
#ifdef VDMPP
  if (tp_rep_q.Is(TAG_TYPE_REP_TypeNameRep))
  {
    const TYPE_AS_Name & nm (tp_rep_q.GetRecord(pos_REP_TypeNameRep_nm));
    if (GetStatSem().IsClassName(StripClassName(nm)))
    {
      tp_rep = TYPE_REP_TypeNameRep().Init(nm);
    }
  }
#endif // VDMPP
  return REP2TYPE::TypeRepToType(tp_rep, NilContextId);
}

// GetGroundTypeRepInfo
// cid : CI`ContextId
// -> REP`TypeRep
TYPE_REP_TypeRep CIAUX::GetGroundTypeRepInfo (const TYPE_CI_ContextId & cid)
{
  TYPE_REP_TypeRep tp (INT2Q::h2gAS(GetCI().GetTypeInfo(cid))); // [REP`TypeRep]
  if (tp.Is(TAG_TYPE_REP_TypeNameRep))
    return LookUpGroundTypeRep(tp.GetRecord(pos_REP_TypeNameRep_nm));
  else
    return tp;
}

// HasInvariant
// type : AS`Type
// -> bool
bool CIAUX::HasInvariant (const TYPE_AS_Type & type)
{
  switch(type.GetTag()) {
    case TAG_TYPE_AS_TypeName: {
      const TYPE_AS_Name & name (type.GetRecord(pos_AS_TypeName_name));
#ifdef VDMSL
      Record td (SPECAST::GetTypeDef(name));
      switch(td.GetTag()) {
        case TAG_TYPE_AS_TypeDef:
          return !td.GetField(pos_AS_TypeDef_Inv).IsNil();
        case TAG_TYPE_AS_StateDef:
          return !td.GetField(pos_AS_StateDef_Inv).IsNil();
        default:
          return false;
      }
#endif // VDMSL
#ifdef VDMPP
      if (GetStatSem().IsClassName(name))
      {
        SEQ<TYPE_AS_InstanceVarDef> vars (SPECAST::GetInstVars(name));
        bool exists = false;
        int len_vars = vars.Length();
        for (int index = 1; (index <= len_vars) && !exists; index++)
        {
          exists = vars[index].Is(TAG_TYPE_AS_InstanceInv);
        }
        return exists;
      }
      else
      {
        TYPE_AS_TypeDef td (SPECAST::GetTypeDef(name));
        return !td.GetField(pos_AS_TypeDef_Inv).IsNil();
      }
#endif // VDMPP
    }
    default: {
      return false;
    }
  }
}

// LookUpFieldsFromTag
// tag : AS`Name
// -> seq of REP`TypeRep
SEQ<TYPE_REP_TypeRep> CIAUX::LookUpFieldsFromTag (const TYPE_AS_Name & tag)
{
  // TODO
  return SEQ<TYPE_REP_TypeRep>();
}

// LookUpGroundType
// nm : AS`Name
// -> AS`Type | POGTP`ClassType
Record CIAUX::LookUpGroundType (const TYPE_AS_Name & nm)
{
  // TODO
  return Record();
}

// LookUpGroundTypeRep
// nm : AS`Name
// -> REP`TypeRep
TYPE_REP_TypeRep CIAUX::LookUpGroundTypeRep (const TYPE_AS_Name & nm)
{
  // TODO
  return TYPE_REP_TypeRep();
}

// LookUpInvOrGroundTypeRep
// nm : AS`Name
// -> REP`TypeRep
TYPE_REP_TypeRep CIAUX::LookUpInvOrGroundTypeRep (const TYPE_AS_Name & nm)
{
  // TODO
  return TYPE_REP_TypeRep();
}

// LookUpInvOrGroundType
// nm : AS`Name
// -> AS`Type
TYPE_AS_Type CIAUX::LookUpInvOrGroundType (const TYPE_AS_Name & nm)
{
  // TODO
  return TYPE_AS_Type();
}

// HasMemberInvariant
// clnm : AS`Name
// -> bool
bool CIAUX::HasMemberInvariant (const TYPE_AS_Name & clnm)
{
  // TODO
  return false;
}

#ifdef VDMSL
// GetMemberNameDef
// nm : AS`Name
// clnm : [AS`Name]
// tp : [AS`Type]
// -> AS`FnDef | AS`OpDef | <MAYBE> | <NO> | <NOT_FOUND>
Generic CIAUX::GetMemberNameDef (const TYPE_AS_Name & nm, const Generic & clnm, const Generic & tp)
{
  // TODO
  return Nil();
}
#endif // VDMSL

#ifdef VDMPP
// LookUpNames
// objrefs : set of REP`ObjRefTypeRep
// metnm : AS`Name
// -> set of REP`TypeRep
SET<TYPE_REP_TypeRep> CIAUX::LookUpNames (const SET<TYPE_REP_ObjRefTypeRep> & objrefs,
                                          const TYPE_AS_Name & metnm)
{
  // TODO
  return SET<TYPE_REP_TypeRep>();
}

// IsSubType
// TpR1 : REP`TypeRep
// TpR2 : REP`TypeRep
// assump : set of (REP`TypeNameRep * REP`TypeNameRep)
// -> bool
bool CIAUX::IsSubType (const TYPE_REP_TypeRep & TpR1,
                       const TYPE_REP_TypeRep & TpR2,
                       const Set & assump)
{
  // TODO
  return false;
}

// NormaliseTypeName
// tp : REP`TypeRep
// -> REP`TypeRep
TYPE_REP_TypeRep CIAUX::NormaliseTypeName (const TYPE_REP_TypeRep & tp)
{
  // TODO
  return TYPE_REP_TypeRep();
}

// IsClassName
// nm : AS`Name
// -> bool
bool CIAUX::IsClassName (const TYPE_AS_Name & nm)
{
  // TODO
  return false;
}

// GetMemberNames
// clnm : AS`Name
// -> set of AS`Name
SET<TYPE_AS_Name> CIAUX::GetMemberNames (const TYPE_AS_Name & clnm)
{
  // TODO
  return SET<TYPE_AS_Name>();
}

// CheckOperationCall
// classes : set of AS`Name
// nm : AS`Name
// objnm : [AS`Expr]
// etp : seq of REP`TypeRep
// -> bool * [REP`TypeRep] * REP`TypeRep
Tuple CIAUX::CheckOperationCall (const SET<TYPE_AS_Name> & classes,
                                 const TYPE_AS_Name & nm,
                                 const Generic & objnm,
                                 const SEQ<TYPE_REP_TypeRep> & etp)
{
  // TODO
  return Tuple();
}

// GetMemberNameDef
// nm : AS`Name
// clnm : [AS`Name]
// tp : [AS`Type]
// -> AS`FnDef | AS`OpDef | AS`InstAssignDef | <MAYBE> | <NO> | <NOT_FOUND>
Generic CIAUX::GetMemberNameDef (const TYPE_AS_Name & nm, const Generic & clnm, const Generic & tp)
{
  // TODO
  return Nil();
}
#endif // VDMPP

// GetMangledFnOpName
// fct : AS`Expr
// args : seq of AS`Expr
// -> AS`Expr
TYPE_AS_Expr CIAUX::GetMangledFnOpName (const TYPE_AS_Expr & fct, const SEQ<TYPE_AS_Expr> & args)
{
  // TODO
  return TYPE_AS_Expr();
}

// GetNameFromStateDesignator
// sd : AS`StateDesignator
// -> AS`Name
TYPE_AS_Name CIAUX::GetNameFromStateDesignator (const TYPE_AS_StateDesignator & sd)
{
  // TODO
  return TYPE_AS_Name();
}

// IsNamePartOfState
// nm : AS`Name
// modnm : [AS`Name]
// -> bool
bool CIAUX::IsNamePartOfState (const TYPE_AS_Name & nm, const Generic & modnm)
{
  // TODO
  return false;
}

// isFunctionName
// name : seq of char
// -> bool
bool CIAUX::isFunctionName (const type_cL & name)
{
  // TODO
  return false;
}

// getFuncReturnInfo
// name : seq of char
// -> FuncReturnInfo
TYPE_CI_uAUX_FuncReturnInfo CIAUX::getFuncReturnInfo (const type_cL & name)
{
  // TODO
  return TYPE_CI_uAUX_FuncReturnInfo();
}

// isTypeName
// name : seq of char
// -> bool
bool CIAUX::isTypeName (const type_cL & name)
{
  // TODO
  return false;
}

// getTypeDef
// name : seq of char
// ->  AS`TypeDef
TYPE_AS_TypeDef CIAUX::getTypeDef (const type_cL & name)
{
  // TODO
  return TYPE_AS_TypeDef();
}

// isValueName
// name : seq of char
// -> bool
bool CIAUX::isValueName (const type_cL & name)
{
  // TODO
  return false;
}

#ifdef VDMPP
// StripClassName
//nm : AS`Name -> AS`Name
TYPE_AS_Name CIAUX::StripClassName (const TYPE_AS_Name & nm)
{
  if (nm.GetSequence(pos_AS_Name_ids).Length() < 2)
    return nm;

  TYPE_AS_Name nm_q (ASTAUX::GetSecondName(nm));   
  if (GetStatSem().IsClassName(nm_q))
    return nm_q;
  return nm;
}
#endif // VDMPP
