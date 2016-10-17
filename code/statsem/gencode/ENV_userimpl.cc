#include "ENV.h"
#include "statsem.h"
#include "intconvquotes.h"

// InitEnv
// ==> ()
void vdm_ENV_InitEnv ()
{
  GetStatSem().InitEnv ();
}

// getRecMap
// ==> map AS`Name to FunctionInfo
Map vdm_ENV_getRecMap ()
{
  return INT2Q::h2gAS(GetStatSem().getRecMap());
}

#ifdef VDMSL
// LookUpTypeName
// nm : AS`Name
// ==> [REP`TypeRep]
Generic vdm_ENV_LookUpTypeName (const TYPE_AS_Name & tnm)
{
  return INT2Q::h2gAS(GetStatSem().LookUpTypeName(INT2Q::g2hAS(tnm)));
}

// LookUpTag
// i : TYPE`Ind
// tnm : AS`Name
// ==> [AS`Name * seq of REP`FieldRep]
Generic vdm_ENV_LookUpTag (const Quote & ind, const TYPE_AS_Name & tnm, const SET<TYPE_AS_Name> & nm_s)
{
  return INT2Q::h2gAS(GetStatSem().LookUpTag(INT2Q::g2hAS(ind), INT2Q::g2hAS(tnm), INT2Q::g2hAS(nm_s)));
}

// LookUp
// nm : AS`Name
// printErr : bool
// ==> [REP`TypeRep]
Generic vdm_ENV_LookUp (const TYPE_AS_Name & nm, const Bool & printErr)
{
  return INT2Q::h2gAS(GetStatSem().LookUp(INT2Q::g2hAS(nm), printErr.GetValue()));
}

// ExpandModule
// i : TYPE`Ind
// m : (AS`Module | AS`DLModule)
// ==> bool
Bool vdm_ENV_ExpandModule (const Quote & i, const Record & m)
{
  return GetStatSem().ExpandModule(INT2Q::g2hAS(i), INT2Q::g2hAS(m));
}
#endif // VDMSL

#ifdef VDMPP
// SetCurClass
// clnm : AS`Name
// ==> ()
void vdm_ENV_SetCurClass (const TYPE_AS_Name & clnm)
{
  GetStatSem().SetCurClass(clnm);
}

// IsAccessType
// tp : [AccessType|REP`TypeRep|TagRepElem|
//                 set of (AccessOpTypeRep | AccessFnTypeRep | AccessPolyTypeRep)|
//                 set of (REP`OpTypeRep | REP`FnTypeRep)]
// -> bool
Bool vdm_ENV_IsAccessType (const Generic & tp)
{
  return GetStatSem().IsAccessType(INT2Q::g2hAS(tp));
}

// ExpandClass
// i : TYPE`Ind
// cls : AS`Class
// ==> bool
Bool vdm_ENV_ExpandClass (const Quote & i, const TYPE_AS_Class & cls)
{
  return GetStatSem().ExpandClass(INT2Q::g2hAS(i), INT2Q::g2hAS(cls));
}

// NormaliseTypeName
// p_tp : REP`TypeRep
// ==> REP`TypeRep
TYPE_REP_TypeRep vdm_ENV_NormaliseTypeName(const TYPE_REP_TypeRep & p_tp)
{
  return INT2Q::h2gAS(GetStatSem().NormaliseTypeName(INT2Q::g2hAS(p_tp)));
}

// LookUpOperationName
// classnm : [AS`Name]
// metnm : AS`Name
// objnm : [AS`Expr]
// ==> set of (REP`TypeRep|AccessType)
Set vdm_ENV_LookUpOperationName (const Generic & classnm,
                                 const TYPE_AS_Name & metnm,
                                 const Generic & objnm)
{
  return GetStatSem().LookUpOperationName(classnm, INT2Q::g2hAS(metnm), objnm);
}

// ConstructImplDomType
// partps : AS`ParameterTypes
// -> seq of AS`Type
Sequence vdm_ENV_ConstructImplDomType (const TYPE_AS_ParameterTypes & partps)
{
  return INT2Q::h2gAS(GetStatSem().ConstructImplDomType(INT2Q::g2hAS(partps)));
}

// ConstructImplRngType
// nmtps : seq of AS`NameType
// -> [AS`Type]
Generic vdm_ENV_ConstructImplRngType (const type_11AS_NameTypeCL & nmtps)
{
  return INT2Q::h2gAS(GetStatSem().ConstructImplRngType(INT2Q::g2hAS(nmtps)));
}

// LookUpDefClass
// ==> [AS`Name]
Generic vdm_ENV_LookUpDefClass()
{
  return INT2Q::h2gAS(GetStatSem().LookUpDefClass());
}

// LookUpTypeName
// nm : AS`Name
// ==> [REP`TypeRep|AccessType]
Generic vdm_ENV_LookUpTypeName (const TYPE_AS_Name & tnm, const Bool & prerror)
{
  return INT2Q::h2gAS(GetStatSem().LookUpTypeName(INT2Q::g2hAS(tnm), prerror.GetValue()));
}

// LookUpTag
// i : TYPE`Ind
// tnm : AS`Name
// ==> [AS`Name * (AccessFieldRep|TagRepElem)]
Generic vdm_ENV_LookUpTag (const Quote & ind, const TYPE_AS_Name & tnm, const SET<TYPE_AS_Name> & nm_s)
{
  return INT2Q::h2gAS(GetStatSem().LookUpTag(INT2Q::g2hAS(ind), INT2Q::g2hAS(tnm), INT2Q::g2hAS(nm_s)));
}

// LookUp
// nm : AS`Name
// printErr : bool
// ==> [REP`TypeRep |
//      AccessType  |
//      set of (AccessOpTypeRep | AccessFnTypeRep | AccessPolyTypeRep) |
//      set of REP`TypeRep]
Generic vdm_ENV_LookUp (const TYPE_AS_Name & nm, const Bool & printErr)
{
  return INT2Q::h2gAS(GetStatSem().LookUp(INT2Q::g2hAS(nm), printErr.GetValue()));
}
#endif //VDMPP
