#ifndef __env_h__
#define __env_h__

//#include "statsem.h"
#include "AS.h"

#define vdm_ENV_FunctionInfo         TAG_TYPE_SSENV_FunctionInfo
#define pos_ENV_FunctionInfo_rec     pos_SSENV_FunctionInfo_rec
#define pos_ENV_FunctionInfo_meas    pos_SSENV_FunctionInfo_meas
#define pos_ENV_FunctionInfo_printPO pos_SSENV_FunctionInfo_printPO
#define TYPE_ENV_FunctionInfo        TYPE_SSENV_FunctionInfo

Generic vdm_ENV_LookUp (const TYPE_AS_Name & nm, const Bool & printErr);

#ifdef VDMPP
void vdm_ENV_SetCurClass (const TYPE_AS_Name&);
Bool vdm_ENV_IsAccessType (const Generic &);
TYPE_REP_TypeRep vdm_ENV_NormaliseTypeName (const TYPE_REP_TypeRep &);
Bool vdm_ENV_ExpandClass (const Quote&, const TYPE_AS_Class &);
Set vdm_ENV_LookUpOperationName (const Generic &, const TYPE_AS_Name &, const Generic &);
Sequence vdm_ENV_ConstructImplDomType (const TYPE_AS_ParameterTypes &);
Generic vdm_ENV_ConstructImplRngType (const type_11AS_NameTypeCL &);
Generic vdm_ENV_LookUpDefClass();

#define TYPE_ENV_AccessTypeRep       TYPE_SSENV_AccessTypeRep
#define TYPE_ENV_AccessFieldRep      TYPE_SSENV_AccessFieldRep
#define TYPE_ENV_AccessFnTypeRep     TYPE_SSENV_AccessFnTypeRep
#define TYPE_ENV_AccessOpTypeRep     TYPE_SSENV_AccessOpTypeRep
#define TYPE_ENV_AccessPolyTypeRep   TYPE_SSENV_AccessPolyTypeRep
#define vdm_ENV_AccessTypeRep        TAG_TYPE_SSENV_AccessTypeRep
#define pos_ENV_AccessTypeRep_tp     pos_SSENV_AccessTypeRep_tp
#define vdm_ENV_AccessPolyTypeRep    TAG_TYPE_SSENV_AccessPolyTypeRep
#define pos_ENV_AccessPolyTypeRep_tp pos_SSENV_AccessPolyTypeRep_tp
#define vdm_ENV_AccessOpTypeRep      TAG_TYPE_SSENV_AccessOpTypeRep
#define pos_ENV_AccessOpTypeRep_tp   pos_SSENV_AccessOpTypeRep_tp
#define vdm_ENV_AccessFnTypeRep      TAG_TYPE_SSENV_AccessFnTypeRep
#define pos_ENV_AccessFnTypeRep_tp   pos_SSENV_AccessFnTypeRep_tp
#define vdm_ENV_AccessFieldRep       TAG_TYPE_SSENV_AccessFieldRep
#define pos_ENV_AccessFieldRep_tp    pos_SSENV_AccessFieldRep_tp
#endif // VDMPP

#ifdef VDMSL
Bool vdm_ENV_ExpandModule (const Quote &, const Record &);
#endif // VDMSL

Map vdm_ENV_getRecMap ();

Generic vdm_ENV_LookUpTag (const Quote &, const TYPE_AS_Name &, const SET<TYPE_AS_Name> &); 
#ifdef VDMSL
Generic vdm_ENV_LookUpTypeName (const TYPE_AS_Name &);
#endif // VDMSL
#ifdef VDMPP
Generic vdm_ENV_LookUpTypeName (const TYPE_AS_Name &, const Bool &);
#endif // VDMPP
void vdm_ENV_InitEnv ();

#endif // __env_h__
