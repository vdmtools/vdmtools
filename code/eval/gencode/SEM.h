/***
*  * WHAT
*  *    Type definitions from module SEM
*  * COPYRIGHT
*  *    (C) Kyushu University
*  * SPECIFICATION VERSION
*  *    SEM.vdm version 1.21.2.12
***/

#ifndef __sem_h__
#define __sem_h__

#include "metaiv.h"
#include "main_tag.h"

typedef Int TYPE_STKM_SubProgramId;
typedef Generic TYPE_STKM_EvalStackItem;
class TYPE_GLOBAL_ValueMap;

#undef INCLFILE
#ifdef VDMSL
#define INCLFILE "SEM-sl.h"
#endif

#ifdef VDMPP
#ifdef VICE
// avoid loop
#define INCLFILE "SEM-rt.h"
#else
#define INCLFILE "SEM-pp.h"
#endif // VICE
#endif // VDMPP

#if !defined(VDMSL) && !defined(VDMPP)
#error "VDMSL and VDMPP not defined"
#endif

#include INCLFILE

#define mk_SEM_NUM(v) Record(TAG_TYPE_SEM_NUM, length_SEM_NUM).SetField(pos_SEM_NUM_v, v)
#define mk_SEM_SET(v) Record(TAG_TYPE_SEM_SET, length_SEM_SET).SetField(pos_SEM_SET_v, v)
#define mk_SEM_SEQ(v) Record(TAG_TYPE_SEM_SEQ, length_SEM_SEQ).SetField(pos_SEM_SEQ_v, v)
#define mk_SEM_MAP(v) Record(TAG_TYPE_SEM_MAP, length_SEM_MAP).SetField(pos_SEM_MAP_v, v)
#define mk_SEM_BOOL(v) Record(TAG_TYPE_SEM_BOOL, length_SEM_BOOL).SetField(pos_SEM_BOOL_v, v)
#define mk_SEM_EXIT(v) Record(TAG_TYPE_SEM_EXIT, length_SEM_EXIT).SetField(pos_SEM_EXIT_v, v)
#define mk_SEM_CHAR(v) Record(TAG_TYPE_SEM_CHAR, length_SEM_CHAR).SetField(pos_SEM_CHAR_v, v)
#define mk_SEM_TUPLE(v) Record(TAG_TYPE_SEM_TUPLE, length_SEM_TUPLE).SetField(pos_SEM_TUPLE_v, v)
#define mk_SEM_QUOTE(v) Record(TAG_TYPE_SEM_QUOTE, length_SEM_QUOTE).SetField(pos_SEM_QUOTE_v, v)
#define mk_SEM_TOKEN(v) Record(TAG_TYPE_SEM_TOKEN, length_SEM_TOKEN).SetField(pos_SEM_TOKEN_v, v)

#define mk_SEM_BlkEnv(id_m, perm) TYPE_SEM_BlkEnv().Init(id_m, perm)
#define mk_SEM_ValTp(val, tp) TYPE_SEM_ValTp().Init(val, tp)

#endif // __Sem_h__
