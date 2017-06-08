#ifndef __CI_uAUX_userdef_h__
#define __CI_uAUX_userdef_h__

#include "main_tag.h"
#include "metaiv.h"
#include "AS.h"
#include "REP.h"
#include "contextinfo.h"

class TYPE_CI_uAUX_FuncReturnInfo;

#define DEF_CI_uAUX_USERIMPL

#define DEF_CI_uAUX_GetCid 1
#define DEF_CI_uAUX_GetASType 1
#define DEF_CI_uAUX_HasInvariant 1
//#define DEF_CI_uAUX_LookUpFieldsFromTag 1
//#define DEF_CI_uAUX_LookUpGroundType 1
//#define DEF_CI_uAUX_LookUpGroundTypeRep 1
//#define DEF_CI_uAUX_LookUpInvOrGroundTypeRep 1
//#define DEF_CI_uAUX_LookUpInvOrGroundType 1
//#define DEF_CI_uAUX_GetGroundTypeRepInfo 1
//#define DEF_CI_uAUX_HasMemberInvariant 1

#ifdef VDMSL
//#define DEF_CI_uAUX_GetMemberNameDef 1
#endif // VDMSL

#ifdef VDMPP
//#define DEF_CI_uAUX_LookUpNames 1
//#define DEF_CI_uAUX_IsSubType 1
//#define DEF_CI_uAUX_NormaliseTypeName 1
//#define DEF_CI_uAUX_IsClassName 1
//#define DEF_CI_uAUX_GetMemberNames 1
//#define DEF_CI_uAUX_CheckOperationCall 1
//#define DEF_CI_uAUX_GetMemberNameDef 1
#endif // VDMPP

//#define DEF_CI_uAUX_GetMangledFnOpName 1
//#define DEF_CI_uAUX_GetNameFromStateDesignator 1
//#define DEF_CI_uAUX_IsNamePartOfState 1
//#define DEF_CI_uAUX_isFunctionName 1
//#define DEF_CI_uAUX_getFuncReturnInfo 1
//#define DEF_CI_uAUX_isTypeName 1
//#define DEF_CI_uAUX_getTypeDef 1
//#define DEF_CI_uAUX_isValueName 1

#ifdef DEF_CI_uAUX_USERIMPL
class CIAUX
{
public:
  static TYPE_AS_Type GetASType (const TYPE_CI_ContextId & cid);
  static bool HasInvariant (const TYPE_AS_Type & type);
  static SEQ<TYPE_REP_TypeRep> LookUpFieldsFromTag (const TYPE_AS_Name & tag);
  static Record LookUpGroundType (const TYPE_AS_Name & nm);
  static TYPE_REP_TypeRep LookUpGroundTypeRep (const TYPE_AS_Name & nm);
  static TYPE_REP_TypeRep LookUpInvOrGroundTypeRep (const TYPE_AS_Name & nm);
  static TYPE_AS_Type LookUpInvOrGroundType (const TYPE_AS_Name & nm);
  static TYPE_REP_TypeRep GetGroundTypeRepInfo (const TYPE_CI_ContextId & cid);
  static bool HasMemberInvariant (const TYPE_AS_Name & clnm);
#ifdef VDMSL
  static Generic GetMemberNameDef (const TYPE_AS_Name & nm, const Generic & clnm, const Generic & tp);
#endif // VDMSL
#ifdef VDMPP
  static SET<TYPE_REP_TypeRep> LookUpNames (const SET<TYPE_REP_ObjRefTypeRep> & objrefs, const TYPE_AS_Name & metnm);
  static bool IsSubType (const TYPE_REP_TypeRep & TpR1, const TYPE_REP_TypeRep & TpR2,
                         const Set & assump);
  static TYPE_REP_TypeRep NormaliseTypeName (const TYPE_REP_TypeRep & tp);
  static bool IsClassName (const TYPE_AS_Name & nm);
  static SET<TYPE_AS_Name> GetMemberNames (const TYPE_AS_Name & clnm);
  static Tuple CheckOperationCall (const SET<TYPE_AS_Name> & classes,
                                   const TYPE_AS_Name & nm,
                                   const Generic & objnm,
                                   const SEQ<TYPE_REP_TypeRep> & etp);
  static Generic GetMemberNameDef (const TYPE_AS_Name & nm, const Generic & clnm, const Generic & tp);
#endif // VDMPP
  static TYPE_AS_Expr GetMangledFnOpName (const TYPE_AS_Expr & fct, const SEQ<TYPE_AS_Expr> & args);
  static TYPE_AS_Name GetNameFromStateDesignator (const TYPE_AS_StateDesignator & sd);
  static bool IsNamePartOfState (const TYPE_AS_Name & nm, const Generic & modnm);
  static bool isFunctionName (const type_cL & name);
  static TYPE_CI_uAUX_FuncReturnInfo getFuncReturnInfo (const type_cL & name);
  static bool isTypeName (const type_cL & name);
  static TYPE_AS_TypeDef getTypeDef (const type_cL & name);
  static bool isValueName (const type_cL & name);

private:
#ifdef VDMPP
  static TYPE_AS_Name StripClassName (const TYPE_AS_Name & nm);
#endif // VDMPP
};
#endif // DEF_CI_uAUX_USERIMPL

#endif // __CI_uAUX_userdef_h__
