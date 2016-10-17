/***
*  * WHAT
*  *  
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/gencode/REP.h,v $
*  * VERSION
*  *    $Revision: 1.2 $
*  * DATE
*  *    $Date: 1998/03/28 15:12:45 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    VDM-SL Toolbox.
*  * AUTHOR
*  *    $Author: erik $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __rep_h__
#define __rep_h__

#undef INCLFILE
#ifdef VDMSL
#define INCLFILE "REP-sl.h"
#endif // VDMSL

#ifdef VDMPP
#ifdef VICE
#define INCLFILE "REP-rt.h"
#else
#define INCLFILE "REP-pp.h"
#endif // VICE
#endif // VDMPP

#if !defined(VDMSL) && !defined(VDMPP)
#error "VDMSL and VDMPP not defined"
#endif

#include INCLFILE

#define mk_REP_InvTypeRep(name,shape, invariant)   TYPE_REP_InvTypeRep().Init(name, shape, invariant)
#define mk_REP_BooleanTypeRep()                    TYPE_REP_BooleanTypeRep()
#define mk_REP_NumericTypeRep(type)                TYPE_REP_NumericTypeRep().Init(type)
#define mk_REP_TokenTypeRep()                      TYPE_REP_TokenTypeRep()
#define mk_REP_CharTypeRep()                       TYPE_REP_CharTypeRep()
#define mk_REP_QuoteTypeRep(lit)                   TYPE_REP_QuoteTypeRep().Init(lit)
#define mk_REP_CompositeTypeRep(nm, fields)        TYPE_REP_CompositeTypeRep().Init(nm, fields)
#define mk_REP_FieldRep(sel, tp, dc)               TYPE_REP_FieldRep().Init(sel, tp, dc)
#define mk_REP_UnionTypeRep(tps)                   TYPE_REP_UnionTypeRep().Init(tps)
#define mk_REP_ProductTypeRep(tpl)                 TYPE_REP_ProductTypeRep().Init(tpl)
#define mk_REP_SetTypeRep(elemtp)                  TYPE_REP_SetTypeRep().Init(elemtp)
#define mk_REP_SeqTypeRep(elemtp)                  TYPE_REP_SeqTypeRep().Init(elemtp)
#define mk_REP_GeneralMapTypeRep(mapdom, maprng)   TYPE_REP_GeneralMapTypeRep().Init(mapdom, maprng)
#define mk_REP_InjectiveMapTypeRep(mapdom, maprng) TYPE_REP_InjectiveMapTypeRep().Init(mapdom, maprng)
#define mk_REP_TypeNameRep(nm)                     TYPE_REP_TypeNameRep().Init(nm)
#define mk_REP_TypeVarRep(nm)                      TYPE_REP_TypeVarRep().Init(nm)
#define mk_REP_TypeParRep(nm)                      TYPE_REP_TypeParRep().Init(nm)
#define mk_REP_PartialFnTypeRep(fndom, fnrng)      TYPE_REP_PartialFnTypeRep().Init(fndom, fnrng)
#define mk_REP_TotalFnTypeRep(fndom, fnrng)        TYPE_REP_TotalFnTypeRep().Init(fndom, fnrng)
#ifdef VDMPP
#define mk_REP_ObjRefTypeRep(nm)                   TYPE_REP_ObjRefTypeRep().Init(nm)
#define mk_REP_OverTypeRep(tps)                    TYPE_REP_OverTypeRep().Init(tps)
#endif // VDMPP
#define mk_REP_OpTypeRep(Dom, Rng)                 TYPE_REP_OpTypeRep().Init(Dom, Rng)
#define mk_REP_ExitTypeRep(tp)                     TYPE_REP_ExitTypeRep().Init(tp)
#define mk_REP_PolyTypeRep(vars, tp)               TYPE_REP_PolyTypeRep().Init(vars, tp)
#define mk_REP_AllTypeRep()                        TYPE_REP_AllTypeRep()
#define mk_REP_TmpTypeRep()                        TYPE_REP_TmpTypeRep()
#define mk_REP_UnitTypeRep()                       TYPE_REP_UnitTypeRep()
#define mk_REP_RetTypeRep()                        TYPE_REP_RetTypeRep()
#define mk_REP_NilTypeRep()                        TYPE_REP_NilTypeRep()
#define mk_REP_EmptySetTypeRep(elemtp)             TYPE_REP_EmptySetTypeRep().Init(elemtp)
#define mk_REP_EmptySeqTypeRep(elemtp)             TYPE_REP_EmptySeqTypeRep().Init(elemtp)
#define mk_REP_EmptyMapTypeRep(mapdom, maprng)     TYPE_REP_EmptyMapTypeRep().Init(mapdom, maprng)


#endif // __rep_h__
