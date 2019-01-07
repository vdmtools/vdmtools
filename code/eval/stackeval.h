// {{{ Header

/***
*  * WHAT
*  *    Declaretion of fundamental types used by the core stack evaluator.
*  *
*  * FILE
*  *    $Source: /mnt/cvsroot/toolbox/code/eval/stackeval.h,v $
*  * VERSION
*  *    $Revision: 1.172 $
*  * DATE
*  *    $Date: 2006/10/13 12:04:41 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    VDM-SL Toolbox
*  * STATUS
*  *    Under development.
*  * AUTHOR
*  *    OS + $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

// }}}

// {{{ Includes

#ifndef __stackeval_h__
#define __stackeval_h__

#include <stdio.h>

#include "metaiv.h"
#include "AS.h"
#include "CI.h"
#include "INSTRTP.h"
#include "DEBUGTP.h"
#include "GLOBAL.h"
#include "semvalues.h"

#include "tb_pthread.h"
#include "random.h"
#include "vdmstdlib.h"

// }}}
// {{{ Code Generated Types

// {{{ Pattern

typedef Record TYPE_STKM_Pattern;

enum {
#ifdef VDMPP
  length_STKM_Guard = 2,
  pos_STKM_Guard_opnm = 1,
  pos_STKM_Guard_curobj = 2,
#endif // VDMPP
  length_STKM_CallStackItem = 13,
  pos_STKM_CallStackItem_type = 1,
  pos_STKM_CallStackItem_code = 2,
  pos_STKM_CallStackItem_pc = 3,
  pos_STKM_CallStackItem_nmOrDesc = 4,
  pos_STKM_CallStackItem_arg_ul = 5,
  pos_STKM_CallStackItem_evalstate = 6,
  pos_STKM_CallStackItem_oldCid = 7,
  pos_STKM_CallStackItem_env_ul_uh = 8,
  pos_STKM_CallStackItem_typeinst_uh = 9,
  pos_STKM_CallStackItem_os_uh = 10,
  pos_STKM_CallStackItem_cur_umod_uobj_ul_uh = 11,
  pos_STKM_CallStackItem_rterror = 12,
  pos_STKM_CallStackItem_measu = 13,
  length_STKM_PatternName = 2,
  pos_STKM_PatternName_nm = 1,
  pos_STKM_PatternName_tp = 2,
  length_STKM_MatchVal = 1,
  pos_STKM_MatchVal_val = 1,
  length_STKM_SetEnumPattern = 1,
  pos_STKM_SetEnumPattern_els = 1,
  length_STKM_SetUnionPattern = 2,
  pos_STKM_SetUnionPattern_lp = 1,
  pos_STKM_SetUnionPattern_rp = 2,
  length_STKM_SeqEnumPattern = 1,
  pos_STKM_SeqEnumPattern_els = 1,
  length_STKM_SeqConcPattern = 2,
  pos_STKM_SeqConcPattern_lp = 1,
  pos_STKM_SeqConcPattern_rp = 2,
  length_STKM_MapEnumPattern = 1,
  pos_STKM_MapEnumPattern_mls = 1,
  length_STKM_MapMergePattern = 2,
  pos_STKM_MapMergePattern_lp = 1,
  pos_STKM_MapMergePattern_rp = 2,
  length_STKM_MapletPattern = 2,
  pos_STKM_MapletPattern_dp = 1,
  pos_STKM_MapletPattern_rp = 2,
  length_STKM_TuplePattern = 1,
  pos_STKM_TuplePattern_fields = 1,
  length_STKM_RecordPattern = 2,
  pos_STKM_RecordPattern_nm = 1,
  pos_STKM_RecordPattern_fields = 2,
#ifdef VDMPP
  length_STKM_ObjectPattern = 2,
  pos_STKM_ObjectPattern_cls = 1,
  pos_STKM_ObjectPattern_fields = 2,
  length_STKM_FieldPattern = 2,
  pos_STKM_FieldPattern_nm = 1,
  pos_STKM_FieldPattern_pat = 2,
#endif // VDMPP
  length_STKM_FieldRef = 2,
  pos_STKM_FieldRef_var = 1,
  pos_STKM_FieldRef_sel = 2,
  length_STKM_MapOrSeqRef = 2,
  pos_STKM_MapOrSeqRef_var = 1,
  pos_STKM_MapOrSeqRef_arg = 2,
  length_STKM_Step = 1,
  pos_STKM_Step_level = 1,
  length_STKM_StepIn = 0,
  length_STKM_SingleStep = 1,
  pos_STKM_SingleStep_level = 1,
  length_STKM_Finish = 1,
  pos_STKM_Finish_level = 1,
  length_STKM_Continue = 0,
  length_STKM_Trap = 9,
  pos_STKM_Trap_handid = 1,
  pos_STKM_Trap_lenes = 2,
  pos_STKM_Trap_lencs = 3,
  pos_STKM_Trap_lentis = 4,
  pos_STKM_Trap_lenos = 5,
#ifdef VDMSL
  pos_STKM_Trap_lencurm = 6,
#endif // VDMSL
#ifdef VDMPP
  pos_STKM_Trap_lenobjl = 6,
#endif // VDMPP
  pos_STKM_Trap_cid = 7,
  pos_STKM_Trap_lenenvl = 8,
  pos_STKM_Trap_lentopenvl = 9,

#ifdef VDMPP
  length_STKM_ProcThread = 1,
  pos_STKM_ProcThread_instr = 1,
  length_STKM_PerThread = 5,
  pos_STKM_PerThread_instr = 1,
  pos_STKM_PerThread_period = 2,
  pos_STKM_PerThread_jitter = 3,
  pos_STKM_PerThread_delay = 4,
  pos_STKM_PerThread_offset = 5,
  length_STKM_SpoThread = 4,
  pos_STKM_SpoThread_instr = 1,
  pos_STKM_SpoThread_delay = 2,
  pos_STKM_SpoThread_bound = 3,
  pos_STKM_SpoThread_offset = 4,
#ifndef VICE
  length_STKM_CPUSigma = 3,
  pos_STKM_CPUSigma_threadstate = 1,
  pos_STKM_CPUSigma_curthread = 2,
  pos_STKM_CPUSigma_curr_uprogram = 3,
#else
  length_STKM_CPUSigma = 16,
  pos_STKM_CPUSigma_threadstate = 1,
  pos_STKM_CPUSigma_curthread = 2,
  pos_STKM_CPUSigma_curr_uprogram = 3,
  pos_STKM_CPUSigma_threads = 4,
  pos_STKM_CPUSigma_syncopcall = 5,
  pos_STKM_CPUSigma_objrefs = 6,
  pos_STKM_CPUSigma_schd = 7,
  pos_STKM_CPUSigma_prio = 8,
  pos_STKM_CPUSigma_prio_utab =9,
  pos_STKM_CPUSigma_messages = 10,
  pos_STKM_CPUSigma_capacity = 11,
  pos_STKM_CPUSigma_dur = 12,
  pos_STKM_CPUSigma_cycle = 13,
  pos_STKM_CPUSigma_time = 14,
  pos_STKM_CPUSigma_idletime = 15,
  pos_STKM_CPUSigma_swaptime = 16,
  length_STKM_BUSSigma = 7,
  pos_STKM_BUSSigma_kind = 1,
  pos_STKM_BUSSigma_capacity = 2,
  pos_STKM_BUSSigma_cpus = 3,
  pos_STKM_BUSSigma_time = 4,
  pos_STKM_BUSSigma_waiting = 5,
  pos_STKM_BUSSigma_mes_um = 6,
  pos_STKM_BUSSigma_log = 7,
  length_STKM_MessageReq = 9,
  pos_STKM_MessageReq_busid = 1,
  pos_STKM_MessageReq_curcpu = 2,
  pos_STKM_MessageReq_tcpu = 3,
  pos_STKM_MessageReq_mesid = 4,
  pos_STKM_MessageReq_tid = 5,
  pos_STKM_MessageReq_fullnm = 6,
  pos_STKM_MessageReq_objref = 7,
  pos_STKM_MessageReq_size = 8,
  pos_STKM_MessageReq_time = 9,
  length_STKM_MessageReplyReq = 9,
  pos_STKM_MessageReplyReq_busid = 1,
  pos_STKM_MessageReplyReq_curcpu = 2,
  pos_STKM_MessageReplyReq_tcpu = 3,
  pos_STKM_MessageReplyReq_newmesid = 4,
  pos_STKM_MessageReplyReq_mesid = 5,
  pos_STKM_MessageReplyReq_curthread = 6,
  pos_STKM_MessageReplyReq_fromthread = 7,
  pos_STKM_MessageReplyReq_size = 8,
  pos_STKM_MessageReplyReq_time = 9,
  length_STKM_Message = 5,
  pos_STKM_Message_id = 1,
  pos_STKM_Message_time = 2,
  pos_STKM_Message_prio = 3,
  pos_STKM_Message_mes = 4,
  pos_STKM_Message_part = 5,
  length_STKM_MessageInfo = 5,
  pos_STKM_MessageInfo_obj_uref = 1,
  pos_STKM_MessageInfo_opname = 2,
  pos_STKM_MessageInfo_args = 3,
  pos_STKM_MessageInfo_threadid = 4,
  pos_STKM_MessageInfo_respond = 5,
  length_STKM_MessageResponse = 4,
  pos_STKM_MessageResponse_opname = 1,
  pos_STKM_MessageResponse_res = 2,
  pos_STKM_MessageResponse_clnm = 3,
  pos_STKM_MessageResponse_threadid = 4,
  length_STKM_MessageBroadcast = 5,
  pos_STKM_MessageBroadcast_obj_urefs = 1,
  pos_STKM_MessageBroadcast_opname = 2,
  pos_STKM_MessageBroadcast_args = 3,
  pos_STKM_MessageBroadcast_tcpum = 4,
  pos_STKM_MessageBroadcast_curcpu = 5,
#endif // VICE
#endif // VDMPP
  length_STKM_DebugCmd = 1,
  pos_STKM_DebugCmd_instr = 1
};

// }}}
// {{{ PatternName

class TYPE_STKM_PatternName : public Record
{
public:
  TYPE_STKM_PatternName() : Record(TAG_TYPE_STKM_PatternName, 2) {}
  TYPE_STKM_PatternName& Init(const Generic & p1,
                              const Generic & p2)
  { SetField(1, p1); SetField(2, p2); return *this; }
  TYPE_STKM_PatternName(const Common &c) : Record(c) {}

  const Generic & get_nm() const { return GetField(1); }
  void set_nm(const Generic &p) { SetField(1, p); }
  const Generic & get_tp() const { return GetField(2); }
  void set_tp(const Generic &p) { SetField(2, p); }
};

// }}}
// {{{ MatchVal

class TYPE_STKM_MatchVal : public Record
{
public:

  TYPE_STKM_MatchVal() : Record(TAG_TYPE_STKM_MatchVal, 1) {}
  TYPE_STKM_MatchVal& Init(const Generic & p1)
  { SetField(1, p1); return *this; }
  TYPE_STKM_MatchVal(const Common &c) : Record(c) {}

  const Record & get_val() const { return GetRecord(1); }
  void set_val(const Record &p) { SetField(1, p); }
};

// }}}
// {{{ SetPattern

typedef TYPE_STKM_Pattern TYPE_STKM_SetPattern;

// }}}
// {{{ SetEnumPattern

class TYPE_STKM_SetEnumPattern : public Record
{
public:

  TYPE_STKM_SetEnumPattern() : Record(TAG_TYPE_STKM_SetEnumPattern, 1) {}
  TYPE_STKM_SetEnumPattern& Init(const type_dL & p1)
  { SetField(1, p1); return *this; }
  TYPE_STKM_SetEnumPattern(const Common &c) : Record(c) {}

  const SEQ<TYPE_STKM_Pattern> & get_els() const
  { return (const SEQ<TYPE_STKM_Pattern> &)GetSequence(1); }
  void set_els(const SEQ<TYPE_STKM_Pattern> &p) { SetField(1, p); }
};

// }}}
// {{{ SetUnionPattern

class TYPE_STKM_SetUnionPattern : public Record
{
public:
  TYPE_STKM_SetUnionPattern() : Record(TAG_TYPE_STKM_SetUnionPattern, 2) {}
  TYPE_STKM_SetUnionPattern& Init(const TYPE_STKM_Pattern & p1,
                                  const TYPE_STKM_Pattern & p2)
  { SetField(1, p1); SetField(2, p2); return *this; }
  TYPE_STKM_SetUnionPattern(const Common &c) : Record(c) {}

  const TYPE_STKM_Pattern & get_lp() const { return GetRecord(1); }
  void set_lp(const TYPE_STKM_Pattern &p) { SetField(1, p); }
  const TYPE_STKM_Pattern & get_rp() const { return GetRecord(2); }
  void set_rp(const TYPE_STKM_Pattern &p) { SetField(2, p); }
};

// }}}
// {{{ SeqPattern

typedef TYPE_STKM_Pattern TYPE_STKM_SeqPattern;

// }}}
// {{{ SeqEnumPattern

class TYPE_STKM_SeqEnumPattern : public Record
{
public:
  TYPE_STKM_SeqEnumPattern() : Record(TAG_TYPE_STKM_SeqEnumPattern, 1) {}
  TYPE_STKM_SeqEnumPattern& Init(const type_dL & p1)
  { SetField(1, p1); return *this; }
  TYPE_STKM_SeqEnumPattern(const Common &c) : Record(c) {}

  const SEQ<TYPE_STKM_Pattern> & get_els() const
  { return (const SEQ<TYPE_STKM_Pattern> &)GetSequence(1); }
  void set_els(const SEQ<TYPE_STKM_Pattern> &p) { SetField(1, p); }
};

// }}}
// {{{ SeqConcPattern

class TYPE_STKM_SeqConcPattern : public Record
{
public:

  TYPE_STKM_SeqConcPattern() : Record(TAG_TYPE_STKM_SeqConcPattern, 2) {}
  TYPE_STKM_SeqConcPattern& Init(const TYPE_STKM_Pattern & p1,
                                 const TYPE_STKM_Pattern & p2)
  { SetField(1, p1); SetField(2, p2); return *this; }
  TYPE_STKM_SeqConcPattern(const Common &c) : Record(c) {}

  const TYPE_STKM_Pattern & get_lp() const { return GetRecord(1); }
  void set_lp(const TYPE_STKM_Pattern &p) { SetField(1, p); }
  const TYPE_STKM_Pattern & get_rp() const { return GetRecord(2); }
  void set_rp(const TYPE_STKM_Pattern &p) { SetField(2, p); }
};

// }}}
// {{{ MapletPattern

class TYPE_STKM_MapletPattern : public Record
{
public:

  TYPE_STKM_MapletPattern() : Record(TAG_TYPE_STKM_MapletPattern, 2) {}
  TYPE_STKM_MapletPattern& Init(const TYPE_STKM_Pattern & p1,
                                const TYPE_STKM_Pattern & p2)
  { SetField(1, p1); SetField(2, p2); return *this; }
  TYPE_STKM_MapletPattern(const Common &c) : Record(c) {}

  const TYPE_STKM_Pattern & get_dp() const { return GetRecord(1); }
  void set_dp(const TYPE_STKM_Pattern &p) { SetField(1, p); }
  const TYPE_STKM_Pattern & get_rp() const { return GetRecord(2); }
  void set_rp(const TYPE_STKM_Pattern &p) { SetField(2, p); }
};

// }}}
// {{{ MapEnumPattern

class TYPE_STKM_MapEnumPattern : public Record
{
public:
  TYPE_STKM_MapEnumPattern() : Record(TAG_TYPE_STKM_MapEnumPattern, 1) {}
  TYPE_STKM_MapEnumPattern& Init(const type_dL & p1)
  { SetField(1, p1); return *this; }
  TYPE_STKM_MapEnumPattern(const Common &c) : Record(c) {}

  const SEQ<TYPE_STKM_MapletPattern> & get_mls() const
  { return (const SEQ<TYPE_STKM_MapletPattern> &)GetSequence(1); }
  void set_mls(const SEQ<TYPE_STKM_MapletPattern> &p) { SetField(1, p); }
};

// }}}
// {{{ MapMergePattern

class TYPE_STKM_MapMergePattern : public Record
{
public:

  TYPE_STKM_MapMergePattern() : Record(TAG_TYPE_STKM_MapMergePattern, 2) {}
  TYPE_STKM_MapMergePattern& Init(const TYPE_STKM_Pattern & p1,
                                  const TYPE_STKM_Pattern & p2)
  { SetField(1, p1); SetField(2, p2); return *this; }
  TYPE_STKM_MapMergePattern(const Common &c) : Record(c) {}

  const TYPE_STKM_Pattern & get_lp() const { return GetRecord(1); }
  void set_lp(const TYPE_STKM_Pattern &p) { SetField(1, p); }
  const TYPE_STKM_Pattern & get_rp() const { return GetRecord(2); }
  void set_rp(const TYPE_STKM_Pattern &p) { SetField(2, p); }
};

// }}}
// {{{ TuplePattern

class TYPE_STKM_TuplePattern : public Record
{
public:

  TYPE_STKM_TuplePattern() : Record(TAG_TYPE_STKM_TuplePattern, 1) {}
  TYPE_STKM_TuplePattern& Init(const type_dL & p1)
  { SetField(1, p1); return *this; }
  TYPE_STKM_TuplePattern(const Common &c) : Record(c) {}

  const SEQ<TYPE_STKM_Pattern> & get_fields() const
  { return (const SEQ<TYPE_STKM_Pattern> &)GetSequence(1); }
  void set_fields(const SEQ<TYPE_STKM_Pattern> &p) { SetField(1, p); }
};

// }}}
// {{{ RecordPattern

class TYPE_STKM_RecordPattern : public Record
{
public:

  TYPE_STKM_RecordPattern() : Record(TAG_TYPE_STKM_RecordPattern, 2) {}
  TYPE_STKM_RecordPattern& Init(const TYPE_AS_Name & p1,
                                const type_dL & p2)
  { SetField(1, p1); SetField(2, p2); return *this; }
  TYPE_STKM_RecordPattern(const Common &c) : Record(c) {}

  const TYPE_AS_Name & get_nm() const
  { return (const TYPE_AS_Name &)GetRecord(1); }
  void set_nm(const TYPE_AS_Name &p) { SetField(1, p); }
  const SEQ<TYPE_STKM_Pattern> & get_fields() const
  { return (const SEQ<TYPE_STKM_Pattern> &)GetSequence(2); }
  void set_fields(const SEQ<TYPE_STKM_Pattern> &p) { SetField(2, p); }
};

// }}}
#ifdef VDMPP
// {{{ ObjectPattern

class TYPE_STKM_ObjectPattern : public Record
{
public:

  TYPE_STKM_ObjectPattern() : Record(TAG_TYPE_STKM_ObjectPattern, 2) {}
  TYPE_STKM_ObjectPattern& Init(const TYPE_AS_Name & p1,
                                const type_dL & p2)
  { SetField(1, p1); SetField(2, p2); return *this; }
  TYPE_STKM_ObjectPattern(const Common &c) : Record(c) {}

  const TYPE_AS_Name & get_nm() const
  { return (const TYPE_AS_Name &)GetRecord(1); }
  void set_nm(const TYPE_AS_Name &p) { SetField(1, p); }
  const SEQ<TYPE_STKM_Pattern> & get_fields() const
  { return (const SEQ<TYPE_STKM_Pattern> &)GetSequence(2); }
  void set_fields(const SEQ<TYPE_STKM_Pattern> &p) { SetField(2, p); }
};

// }}}
// {{{ FieldPattern

class TYPE_STKM_FieldPattern : public Record
{
public:

  TYPE_STKM_FieldPattern() : Record(TAG_TYPE_STKM_FieldPattern, 2) {}
  TYPE_STKM_FieldPattern& Init(const TYPE_AS_Name & p1,
                               const TYPE_STKM_Pattern & p2)
  { SetField(1, p1); SetField(2, p2); return *this; }
  TYPE_STKM_FieldPattern(const Common &c) : Record(c) {}

  const TYPE_AS_Name & get_nm() const
  { return (const TYPE_AS_Name &)GetRecord(1); }
  void set_nm(const TYPE_AS_Name &p) { SetField(1, p); }
  const TYPE_STKM_Pattern & get_pat() const
  { return (const TYPE_STKM_Pattern &)GetRecord(2); }
  void set_pat(const TYPE_STKM_Pattern &p) { SetField(2, p); }
};

// }}}
#endif // VDMPP
// {{{ StateDesignator

typedef Record TYPE_STKM_StateDesignator;

// }}}
// {{{ FieldRef

class TYPE_STKM_FieldRef : public Record
{
public:

  TYPE_STKM_FieldRef() : Record(TAG_TYPE_STKM_FieldRef, 2) {}
  TYPE_STKM_FieldRef& Init(const TYPE_STKM_StateDesignator & p1,
                           const TYPE_AS_Name & p2)
  { SetField(1, p1); SetField(2, p2); return *this; }
  TYPE_STKM_FieldRef(const Common &c) : Record(c) {}

  const TYPE_STKM_StateDesignator & get_var() const { return GetRecord(1); }
  void set_var(const TYPE_STKM_StateDesignator &p) { SetField(1, p); }
  const TYPE_AS_Name & get_sel() const
  { return (const TYPE_AS_Name &)GetRecord(2); }
  void set_sel(const TYPE_AS_Name &p) { SetField(2, p); }
};

// }}}
// {{{ MapOrSeqRef

class TYPE_STKM_MapOrSeqRef : public Record
{
public:

  TYPE_STKM_MapOrSeqRef() : Record(TAG_TYPE_STKM_MapOrSeqRef, 2) {}
  TYPE_STKM_MapOrSeqRef& Init(const TYPE_STKM_StateDesignator & p1,
                              const Generic & p2)
  { SetField(1, p1); SetField(2, p2); return *this; }
  TYPE_STKM_MapOrSeqRef(const Common &c) : Record(c) {}

  const TYPE_STKM_StateDesignator & get_var() const { return GetRecord(1); }
  void set_var(const TYPE_STKM_StateDesignator &p) { SetField(1, p); }
  const Generic & get_arg() const { return GetField(2); }
  void set_arg(const Generic &p) { SetField(2, p); }
};

// }}}
// {{{ Trap

class TYPE_STKM_Trap : public Record
{
public:

  TYPE_STKM_Trap() : Record(TAG_TYPE_STKM_Trap, 9) {}
  TYPE_STKM_Trap & Init(const Int & p1, const Int & p2,
                        const Int & p3, const Int & p4,
                        const Int & p5, const Int & p6,
                        const TYPE_CI_ContextId & p7,
                        const Int & p8, const Int & p9) {
    SetField(1, p1); SetField(2, p2); SetField(3, p3);
    SetField(4, p4); SetField(5, p5); SetField(6, p6);
    SetField(7, p7); SetField(8, p8); SetField(9, p9);
    return *this; }
  TYPE_STKM_Trap(const Common &c) : Record(c) {}

  const Int & get_handid() const { return GetInt(1); }
  void set_handid(const Int &p) { SetField(1, p); }
  const Int & get_lenes() const { return GetInt(2); }
  void set_lenes(const Int &p) { SetField(2, p); }
  const Int & get_lencs() const { return GetInt(3); }
  void set_lencs(const Int &p) { SetField(3, p); }
  const Int & get_lentis() const { return GetInt(4); }
  void set_lentis(const Int &p) { SetField(4, p); }
  const Int & get_lenos() const { return GetInt(5); }
  void set_lenos(const Int &p) { SetField(5, p); }
#ifdef VDMSL
  const Int & get_lencurm() const { return GetInt(6); }
  void set_lencurm(const Int &p) { SetField(6, p); }
#endif // VDMSL
#ifdef VDMPP
  const Int & get_lenobjl() const { return GetInt(6);}
  void set_lenobjl(const Int &p) { SetField(6, p); }
#endif //VDMPP
  const TYPE_CI_ContextId & get_cid() const { return GetInt(7); }
  void set_cid(const TYPE_CI_ContextId &p) { SetField(7, p); }
  const Int & get_lenenvl() const { return GetInt(8); }
  void set_lenenvl(const Int &p) { SetField(8, p); }
  const Int & get_lentopenvl() const { return GetInt(9); }
  void set_lentopenvl(const Int &p) { SetField(9, p); }
} ;

// }}}
// {{{ DebugFlag, Step, StepIn, SingleStep, Finish, Continue

typedef Record TYPE_STKM_DebugFlag;

class TYPE_STKM_SingleStep : public Record
{
public:

  TYPE_STKM_SingleStep() : Record(TAG_TYPE_STKM_SingleStep, 1) {}
  TYPE_STKM_SingleStep(const Common &c) : Record(c) {}
  TYPE_STKM_SingleStep &Init(const Int & p1) { SetField(1, p1); return *this; }

  const Int & get_level() const { return GetInt(1); }
  void set_level(const Int &p) { SetField(1, p); }
};

class TYPE_STKM_Step : public Record 
{
public:
  TYPE_STKM_Step() : Record(TAG_TYPE_STKM_Step, 1) {}
  TYPE_STKM_Step(const Common &c) : Record(c) {}
  TYPE_STKM_Step &Init(const Int & p1) { SetField(1, p1); return *this; }

  const Int & get_level() const { return GetInt(1); }
  void set_level(const Int &p) { SetField(1, p); }
};

class TYPE_STKM_StepIn : public Record
{
public:
  TYPE_STKM_StepIn() : Record(TAG_TYPE_STKM_StepIn, 0) {}
  TYPE_STKM_StepIn(const Common &c) : Record(c) {}
  //TYPE_STKM_StepIn &Init() { return *this; }
};

class TYPE_STKM_Finish : public Record
{
public:
  TYPE_STKM_Finish() : Record(TAG_TYPE_STKM_Finish, 1) {}
  TYPE_STKM_Finish(const Common &c) : Record(c) {}
  TYPE_STKM_Finish &Init(Int p1) { SetField(1, p1); return *this; }

  const Int & get_level() const { return GetInt(1); }
  void set_level(const Int &p) { SetField(1, p); }
} ;

class TYPE_STKM_Continue : public Record
{
public:
  TYPE_STKM_Continue() : Record(TAG_TYPE_STKM_Continue, 0) {}
  TYPE_STKM_Continue(const Common &c) : Record(c) {}
  //TYPE_STKM_Continue &Init() { return *this; }
} ;

// {{{ SubProgram, Instructions, SubProgramId and ProgramCounter

// Instrcutions are implemented as code generated Records from the
// specification INSTRTP.

// The instruction sequence for a function or an operation is a vector
// of instructions.
typedef SEQ<TYPE_INSTRTP_Instruction> TYPE_STKM_SubProgram;

// All SubProgram's are maintained in a global table. Each sub program
// is identified by an id:
typedef Int TYPE_STKM_SubProgramId;
//typedef TYPE_STKM_SubProgram TYPE_STKM_DebugCmd;
typedef int TYPE_STKM_ProgramCounter;
typedef Record TYPE_STKM_Code;

#ifndef DECL_TYPE_STKM_DebugCmd
#define DECL_TYPE_STKM_DebugCmd
class TYPE_STKM_DebugCmd : public Record
{
public:
  TYPE_STKM_DebugCmd();
  TYPE_STKM_DebugCmd(const Common &c);
  TYPE_STKM_DebugCmd &Init(const TYPE_STKM_SubProgram & p1);
  TYPE_STKM_SubProgram get_instr() const;
  void set_instr(const TYPE_STKM_SubProgram &p);
};
#endif // DECL_TYPE_STKM_DebugCmd
// }}} SubProgram, Instructions, SubProgramId and ProgramCounter

#ifdef VDMPP
// [[[ MessageId, Message, MessageInfo, MessageResponse, MessageBroadcast
//     MessageReq, MessageReplyReq
#ifdef VICE
typedef Int TYPE_STKM_MessageId;

class TYPE_STKM_Message : public Record
{
public:
  TYPE_STKM_Message() : Record(TAG_TYPE_STKM_Message, 5) {}

  TYPE_STKM_Message(const Common &c) : Record(c) {}

  TYPE_STKM_Message &Init(const TYPE_STKM_MessageId & p1, const Int & p2,
    const Int & p3, const Generic & p4, const Int & p5)
 { SetField(1,p1); SetField(2,p2); SetField(3,p3);
   SetField(4,p4); SetField(5,p5); return *this; }

  TYPE_STKM_MessageId get_id() const { return GetField(1); }
  void set_id(const TYPE_STKM_MessageId &p) { SetField(1, p); }
  Int get_time() const { return GetField(2); }
  void set_time(const Int &p) { SetField(2, p); }
  Int get_prio() const { return GetField(3); }
  void set_prio(const Int &p) { SetField(3, p); }
  Record get_mes() const { return GetField(4); }
  void set_mes(const Record &p) { SetField(4, p); }
  Int get_part() const { return GetField(5); }
  void set_part(const Int &p) { SetField(5, p); }
};

class TYPE_STKM_MessageInfo : public Record
{
public:
  TYPE_STKM_MessageInfo() : Record(TAG_TYPE_STKM_MessageInfo, 5) {}

  TYPE_STKM_MessageInfo(const Common &c) : Record(c) {}

  TYPE_STKM_MessageInfo &Init(const Generic & p1, const TYPE_AS_Name & p2,
    const Sequence & p3, const Generic & p4, const Generic & p5)
 { SetField(1,p1); SetField(2,p2); SetField(3,p3);
   SetField(4,p4); SetField(5,p5); return *this; }

  Generic get_obj_ref() const { return GetField(1); }
  void set_obj_ref(const Generic &p) { SetField(1, p); }
  TYPE_AS_Name get_opname() const { return GetField(2); }
  void set_opname(const TYPE_AS_Name &p) { SetField(2, p); }
  Sequence get_args() const { return GetField(3); }
  void set_args(const Sequence &p) { SetField(3, p); }
  Generic get_threadid() const { return GetField(4); }
  void set_threadid(const Generic &p) { SetField(4, p); }
  Generic get_respond() const { return GetField(5); }
  void set_respond(const Generic &p) { SetField(5, p); }
};

class TYPE_STKM_MessageResponse : public Record
{
public:
  TYPE_STKM_MessageResponse() : Record(TAG_TYPE_STKM_MessageResponse, 4) {}

  TYPE_STKM_MessageResponse(const Common &c) : Record(c) {}

  TYPE_STKM_MessageResponse &Init(const TYPE_AS_Name & p1, const TYPE_SEM_VAL & p2,
    const Generic & p3, const TYPE_SCHDTP_ThreadId & p4)
 { SetField(1,p1); SetField(2,p2); SetField(3,p3);
   SetField(4,p4); return *this; }

  TYPE_AS_Name get_opname() const { return GetField(1); }
  void set_opname(const TYPE_AS_Name &p) { SetField(1, p); }
  TYPE_SEM_VAL get_res() const { return GetField(2); }
  void set_res(const TYPE_SEM_VAL &p) { SetField(2, p); }
  Generic get_clnm() const { return GetField(3); }
  void set_clnm(const Generic &p) { SetField(3, p); }
  TYPE_SCHDTP_ThreadId get_threadid() const { return GetField(4); }
  void set_threadid(const TYPE_SCHDTP_ThreadId &p) { SetField(4, p); }
};

class TYPE_STKM_MessageBroadcast : public Record
{
public:
  TYPE_STKM_MessageBroadcast() : Record(TAG_TYPE_STKM_MessageBroadcast, 5) {}

  TYPE_STKM_MessageBroadcast(const Common &c) : Record(c) {}

  TYPE_STKM_MessageBroadcast &Init(const SET<TYPE_SEM_OBJ_uRef> & p1,
    const TYPE_AS_Name & p2,
    const Sequence & p3, const Map & p4, const Generic & p5)
 { SetField(1,p1); SetField(2,p2); SetField(3,p3);
   SetField(4,p4); SetField(5,p5); return *this; }

  SET<TYPE_SEM_OBJ_uRef> get_obj_refs() const { return GetField(1); }
  void set_obj_refs(const SET<TYPE_SEM_OBJ_uRef> &p) { SetField(1, p); }
  TYPE_AS_Name get_opname() const { return GetField(2); }
  void set_opname(const TYPE_AS_Name &p) { SetField(2, p); }
  Sequence get_args() const { return GetField(3); }
  void set_args(const Sequence &p) { SetField(3, p); }
  Map get_tcpum() const { return GetField(4); }
  void set_tcpum(const Map &p) { SetField(4, p); }
  Generic get_curcpu() const { return GetField(5); }
  void set_curcpu(const Generic &p) { SetField(5, p); }
};

class TYPE_STKM_MessageReq : public Record
{
public:
  TYPE_STKM_MessageReq() : Record(TAG_TYPE_STKM_MessageReq, 9) {}

  TYPE_STKM_MessageReq(const Common &c) : Record(c) {}

  TYPE_STKM_MessageReq &Init(const Generic & p1, const Generic & p2,
    const Generic & p3, const TYPE_STKM_MessageId & p4,
    const TYPE_SCHDTP_ThreadId & p5, const TYPE_AS_Name & p6,
    const Generic & p7, const Int & p8, const Int & p9)
 { SetField(1,p1); SetField(2,p2); SetField(3,p3); SetField(4,p4);
   SetField(5,p5); SetField(6,p6); SetField(7,p7); SetField(8,p8);
   SetField(9,p9); return *this; }

  Generic get_busid() const { return GetField(1); }
  void set_busid(const Generic &p) { SetField(1, p); }
  Generic get_curcpu() const { return GetField(2); }
  void set_curcpu(const Generic &p) { SetField(2, p); }
  Generic get_tcpu() const { return GetField(3); }
  void set_tcpu(const Generic &p) { SetField(3, p); }
  TYPE_STKM_MessageId get_mesid() const { return GetField(4); }
  void set_mesid(const TYPE_STKM_MessageId &p) { SetField(4, p); }
  TYPE_SCHDTP_ThreadId get_tid() const { return GetField(5); }
  void set_tid(const TYPE_SCHDTP_ThreadId &p) { SetField(5, p); }
  TYPE_AS_Name get_fullnm() const { return GetField(6); }
  void set_fullnm(const TYPE_AS_Name &p) { SetField(6, p); }
  Generic get_objref() const { return GetField(7); }
  void set_objref(const Generic &p) { SetField(7, p); }
  Int get_size() const { return GetField(8); }
  void set_size(const Int &p) { SetField(8, p); }
  Int get_time() const { return GetField(9); }
  void set_time(const Int &p) { SetField(9, p); }
};

class TYPE_STKM_MessageReplyReq : public Record
{
public:
  TYPE_STKM_MessageReplyReq() : Record(TAG_TYPE_STKM_MessageReplyReq, 9) {}

  TYPE_STKM_MessageReplyReq(const Common &c) : Record(c) {}

  TYPE_STKM_MessageReplyReq &Init(const Generic & p1, const Generic & p2,
    const Generic & p3, const TYPE_STKM_MessageId & p4,
    const TYPE_STKM_MessageId & p5, const TYPE_SCHDTP_ThreadId& p6,
    const TYPE_SCHDTP_ThreadId & p7, const Int & p8, const Int & p9)
 { SetField(1,p1); SetField(2,p2); SetField(3,p3); SetField(4,p4);
   SetField(5,p5); SetField(6,p6); SetField(7,p7); SetField(8,p8);
   SetField(9,p9); return *this; }

  Generic get_busid() const { return GetField(1); }
  void set_busid(const Generic &p) { SetField(1, p); }
  Generic get_curcpu() const { return GetField(2); }
  void set_curcpu(const Generic &p) { SetField(2, p); }
  Generic get_tcpu() const { return GetField(3); }
  void set_tcpu(const Generic &p) { SetField(3, p); }
  TYPE_STKM_MessageId get_newmesid() const { return GetField(4); }
  void set_newmesid(const TYPE_STKM_MessageId &p) { SetField(4, p); }
  TYPE_STKM_MessageId get_mesid() const { return GetField(5); }
  void set_mesid(const TYPE_STKM_MessageId &p) { SetField(5, p); }
  TYPE_SCHDTP_ThreadId get_curthread() const { return GetField(6); }
  void set_curthread(const TYPE_SCHDTP_ThreadId &p) { SetField(6, p); }
  TYPE_SCHDTP_ThreadId get_fromthread() const { return GetField(7); }
  void set_fromthread(const TYPE_SCHDTP_ThreadId &p) { SetField(7, p); }
  Int get_size() const { return GetField(8); }
  void set_size(const Int &p) { SetField(8, p); }
  Int get_time() const { return GetField(9); }
  void set_time(const Int &p) { SetField(9, p); }
};
#endif // VICE

#ifdef VDMPP
class TYPE_STKM_ProcThread : public Record
{
public:
  TYPE_STKM_ProcThread() : Record(TAG_TYPE_STKM_ProcThread, 1) {}
  TYPE_STKM_ProcThread &Init(const TYPE_STKM_SubProgram & p1)
  { SetField(1,p1); return *this; }
  TYPE_STKM_ProcThread(const Common &c) : Record(c) {}
  const wchar_t * GetTypeName () const { return L"TYPE_STKM_ProcThread"; }
  const TYPE_STKM_SubProgram & get_instr() const
  { return static_cast<const TYPE_STKM_SubProgram &>(GetSequence(1)); }
  void set_instr(const TYPE_STKM_SubProgram &p) { SetField(1, p); }
};
class TYPE_STKM_PerThread : public Record
{
public:
  TYPE_STKM_PerThread() : Record(TAG_TYPE_STKM_PerThread, 5) {}
  TYPE_STKM_PerThread &Init(const TYPE_STKM_SubProgram & p1, const TYPE_AS_Expr & p2,
  const TYPE_AS_Expr & p3, const TYPE_AS_Expr & p4, const TYPE_AS_Expr & p5)
  { SetField(1,p1); SetField(2,p2); SetField(3,p3); SetField(4,p4); SetField(5,p5); return *this; }
  TYPE_STKM_PerThread(const Common &c) : Record(c) {}
  const wchar_t * GetTypeName () const { return L"TYPE_STKM_ProcThread"; }
  const TYPE_STKM_SubProgram & get_instr() const
  { return static_cast<const TYPE_STKM_SubProgram &>(GetSequence(1)); }
  void set_instr(const TYPE_STKM_SubProgram &p) { SetField(1, p); }
  const TYPE_AS_Expr & get_period() const
  { return static_cast<const TYPE_AS_Expr &>(GetRecord(2)); }
  void set_period(const TYPE_AS_Expr &p) { SetField(2, p); }
  const TYPE_AS_Expr & get_jitter() const
  { return static_cast<const TYPE_AS_Expr &>(GetRecord(3)); }
  void set_jitter(const TYPE_AS_Expr &p) { SetField(3, p); }
  const TYPE_AS_Expr & get_delay() const
  { return static_cast<const TYPE_AS_Expr &>(GetRecord(4)); }
  void set_delay(const TYPE_AS_Expr &p) { SetField(4, p); }
  const TYPE_AS_Expr & get_offset() const
  { return static_cast<const TYPE_AS_Expr &>(GetRecord(5)); }
  void set_offset(const TYPE_AS_Expr &p) { SetField(5, p); }
};
class TYPE_STKM_SpoThread : public Record
{
public:
  TYPE_STKM_SpoThread() : Record(TAG_TYPE_STKM_SpoThread, 4) {}
  TYPE_STKM_SpoThread &Init(const TYPE_STKM_SubProgram & p1, const TYPE_AS_Expr & p2,
  const TYPE_AS_Expr & p3, const TYPE_AS_Expr & p4)
  { SetField(1,p1); SetField(2,p2); SetField(3,p3); SetField(4,p4); return *this; }
  TYPE_STKM_SpoThread(const Common &c) : Record(c) {}
  const wchar_t * GetTypeName () const { return L"TYPE_STKM_SpoThread"; }
  const TYPE_STKM_SubProgram & get_instr() const
  { return static_cast<const TYPE_STKM_SubProgram &>(GetSequence(1)); }
  void set_instr(const TYPE_STKM_SubProgram &p) { SetField(1, p); }
  const TYPE_AS_Expr & get_delay() const
  { return static_cast<const TYPE_AS_Expr &>(GetRecord(2)); }
  void set_delay(const TYPE_AS_Expr &p) { SetField(2, p); }
  const TYPE_AS_Expr & get_bound() const
  { return static_cast<const TYPE_AS_Expr &>(GetRecord(3)); }
  void set_bound(const TYPE_AS_Expr &p) { SetField(3, p); }
  const TYPE_AS_Expr & get_offset() const
  { return static_cast<const TYPE_AS_Expr &>(GetRecord(4)); }
  void set_offset(const TYPE_AS_Expr &p) { SetField(4, p); }
};
#endif // VDMPP
// {{{ CPUSigma, BUSSigma
#ifndef VICE
class TYPE_STKM_CPUSigma : public Record
{
public:
  TYPE_STKM_CPUSigma() : Record(TAG_TYPE_STKM_CPUSigma, 3) {}

  TYPE_STKM_CPUSigma(const Common &c) : Record(c) {}

  TYPE_STKM_CPUSigma &Init(const Map & p1,
                           const Generic & p2,
                           const TYPE_STKM_SubProgram & p3)
 { SetField(1,p1); SetField(2,p2); SetField(3,p3); return *this; }

  const Map & get_threadstate() const { return GetMap(1); }
  void set_threadstate(const Map &p) { SetField(1, p); }
  const Generic & get_curthread() const { return GetField(2); }
  void set_curthread(const Generic &p) { SetField(2, p); }
  const TYPE_STKM_SubProgram & get_curr_program() const
  { return static_cast<const TYPE_STKM_SubProgram &>(GetSequence(3)); }
  void set_curr_program(const TYPE_STKM_SubProgram &p) { SetField(3, p); }
};
#else
class TYPE_STKM_CPUSigma : public Record
{
public:
  TYPE_STKM_CPUSigma() : Record(TAG_TYPE_STKM_CPUSigma, 16) {}

  TYPE_STKM_CPUSigma(const Common &c) : Record(c) {}

//  TYPE_STKM_CPUSigma &Init(const Map & p1, const Generic & p2,
//                           const TYPE_STKM_SubProgram & p3)
// { SetField(1,p1); SetField(2,p2); SetField(3,p3); return *this; }
  const Map & get_threadstate() const { return GetMap(1); }
  void set_threadstate(const Map &p) { SetField(1, p); }
  const Generic & get_curthread() const { return GetField(2); }
  void set_curthread(const Generic &p) { SetField(2, p); }
  const TYPE_STKM_SubProgram & get_curr_program() const
  { return static_cast<const TYPE_STKM_SubProgram &>(GetSequence(3)); }
  void set_curr_program(const TYPE_STKM_SubProgram &p) { SetField(3, p); }
  const Set & get_threads() const { return GetSet(4); }
  void set_threads(const Set &p) { SetField(4, p); }
  const Set & get_syncopcall() const { return GetSet(5); }
  void set_syncopcall(const Set &p) { SetField(5, p); }
  const Set & get_objrefs() const { return GetSet(6); }
  void set_objrefs(const Set &p) { SetField(6, p); }
  const TYPE_SCHDTP_PrimarySchedulerAlgorithm & get_schd() const { return GetRecord(7); }
  void set_schd(const TYPE_SCHDTP_PrimarySchedulerAlgorithm &p) { SetField(7, p); }
  const TYPE_SCHDTP_SecondarySchedulerAlgorithm & get_prio() const { return GetRecord(8); }
  void set_prio(const TYPE_SCHDTP_SecondarySchedulerAlgorithm &p) { SetField(8, p); }
  const Map & get_prio_tab() const { return GetMap(9); }
  void set_prio_tab(const Map &p) { SetField(9, p); }
  const Map & get_messages() const { return GetMap(10); }
  void set_messages(const Map &p) { SetField(10, p); }
  const Generic & get_capacity() const { return GetField(11); }
  void set_capacity(const Generic &p) { SetField(11, p); }
  const Int & get_dur() const { return GetInt(12); }
  void set_dur(const Int &p) { SetField(12, p); }
  const Int & get_cycle() const { return GetInt(13); }
  void set_cycle(const Int &p) { SetField(13, p); }
  const Int & get_time() const { return GetInt(14); }
  void set_time(const Int &p) { SetField(14, p); }
  const Int & get_idletime() const { return GetInt(15); }
  void set_idletime(const Int &p) { SetField(15, p); }
  const Int & get_swaptime() const { return GetInt(16); }
  void set_swaptime(const Int &p) { SetField(16, p); }
};

class TYPE_STKM_BUSSigma : public Record
{
public:
  TYPE_STKM_BUSSigma() : Record(TAG_TYPE_STKM_BUSSigma, 7) {}

  TYPE_STKM_BUSSigma(const Common &c) : Record(c) {}

  TYPE_STKM_BUSSigma &Init(const TYPE_SCHDTP_BUSKind & p1, const Generic & p2,
    const Set & p3, const Int & p4, const Sequence & p5,
    const MAP<TYPE_STKM_MessageId, TYPE_STKM_Message> & p6,
    const SEQ<TYPE_GLOBAL_ChanUsage> & p7)
 { SetField(1,p1); SetField(2,p2); SetField(3,p3); SetField(4,p4);
   SetField(5,p5); SetField(6,p6); SetField(7,p7); return *this; }

  const TYPE_SCHDTP_BUSKind & get_kind() const { return GetRecord(1); }
  void set_kind(const TYPE_SCHDTP_BUSKind &p) { SetField(1, p); }
  const Generic & get_capacity() const { return GetField(2); }
  void set_capacity(const Generic &p) { SetField(2, p); }
  const Set & get_cpus() const { return GetSet(3); }
  void set_cpus(const Set &p) { SetField(3, p); }
  const Int & get_time() const { return GetInt(4); }
  void set_time(const Int &p) { SetField(4, p); }
  const Sequence & get_waiting() const { return GetSequence(5); }
  void set_waiting(const Sequence &p) { SetField(5, p); }
  MAP<TYPE_STKM_MessageId, TYPE_STKM_Message> get_mes_m() const { return GetField(6); }
  void set_mes_m(const MAP<TYPE_STKM_MessageId, TYPE_STKM_Message> &p) { SetField(6, p); }
  SEQ<TYPE_GLOBAL_ChanUsage> get_log() const { return GetField(7); }
  void set_log(const SEQ<TYPE_GLOBAL_ChanUsage> &p) { SetField(7, p); }
};
// ]]] CPUSigma, BUSSigma
#endif // VICE
#endif // VDMPP


// }}}
// {{{ DEBUG`BreakInfo, DEBUG`BreakStatus

// definition af EvaluationState records:

typedef Record TYPE_STKM_EvaluationState;

class TYPE_STKM_Breakpoint : public Record
{
public:
  TYPE_STKM_Breakpoint() : Record(TAG_TYPE_STKM_Breakpoint, 0) {}
  //TYPE_STKM_Breakpoint &Init() { return *this; }
  TYPE_STKM_Breakpoint(const Common &c) : Record(c) {}
  const wchar_t * GetTypeName() const { return L"TYPE_STKM_Breakpoint"; }
};

class TYPE_STKM_Interrupt : public Record
{
public:
  TYPE_STKM_Interrupt() : Record(TAG_TYPE_STKM_Interrupt, 0) {}
  //TYPE_STKM_Interrupt &Init() { return *this; }
  TYPE_STKM_Interrupt(const Common &c) : Record(c) {}
  const wchar_t * GetTypeName() const { return L"TYPE_STKM_Interrupt"; }
};

class TYPE_STKM_Success : public Record
{
public:
  TYPE_STKM_Success() : Record(TAG_TYPE_STKM_Success, 0) {}
  //TYPE_STKM_Success &Init() { return *this; };
  TYPE_STKM_Success(const Common &c) : Record(c) {}
  const wchar_t * GetTypeName() const { return L"TYPE_STKM_Success"; }
};

class TYPE_STKM_Guard : public Record
{
public:

  TYPE_STKM_Guard() : Record(TAG_TYPE_STKM_Guard, 2) {}
  TYPE_STKM_Guard &Init(const TYPE_AS_Name & p2,
                        const Generic & p3)
  { SetField(1, p2); SetField(2, p3); return * this; }

  TYPE_STKM_Guard(const Common &c) : Record(c) {}

  const wchar_t * GetTypeName() const { return L"TYPE_STKM_Guard"; }
  TYPE_AS_Name get_opnm() const { return GetField(1); }
  void set_opnm(const TYPE_AS_Name &p) { SetField(1, p); }
  Generic get_curobj() const { return GetField(2); }
  void set_curobj(const Generic &p) { SetField(2, p); }
};

class TYPE_STKM_EndOfSlice : public Record
{
public:
  TYPE_STKM_EndOfSlice() : Record(TAG_TYPE_STKM_EndOfSlice, 0) {}
  //TYPE_STKM_EndOfSlice &Init() { return *this; }
  TYPE_STKM_EndOfSlice(const Common &c) : Record(c) {}
  const wchar_t * GetTypeName() const { return L"TYPE_STKM_EndOfSlice"; }
};

class TYPE_STKM_EndOfCPUSlice : public Record
{
public:
  TYPE_STKM_EndOfCPUSlice() : Record(TAG_TYPE_STKM_EndOfCPUSlice, 0) {}
  //TYPE_STKM_EndOfCPUSlice &Init() { return *this; }
  TYPE_STKM_EndOfCPUSlice(const Common &c) : Record(c) {}
  const wchar_t * GetTypeName() const { return L"TYPE_STKM_EndOfCPUSlice"; }
};
// }}}
// }}}

// {{{ Stacks
// {{{ EvalStack and EvalStackItem
//
// EvalStackItem = SEM`VAL |
//                 AS`Name |
//                 AS`OldName |
//                 SemArgList |  -- SemArgList = seq of SEM`VAL
//                 STKM`Pattern |
//                 STKM`StateDesignator |
//                 set of SEM`BlkEnv |
//                 seq of nat |
//                 AS`FctTypeInstExpr;

typedef Generic TYPE_STKM_EvalStackItem;

typedef Stack TYPE_STKM_EvalStack;

// }}}
// {{{ CallStack and CallStackItem
//
//  CallStackItem :: type : CSItemType
//                   code : Code
//                   pc : ProgramCounter
//                   nmOrDesc : AS`Name | seq of char
//                   arg_l  : [seq of SEM`VAL]
//                   evalstate : [EvaluatorStatus]
//                   oldCid : CI`ContextId
//                   env_l_h : [nat]
//                   typeinst_h : [nat]
//                   os_h : [nat]
//                   cur_mod_obj_l_h : [nat]
//                   rterror : bool

class CallStackItemType
{
public:
  // Which type of CallStack item is it
  enum CSItemType {CS_DEBUGCMD, CS_FNOP, CS_INTERNAL};
};

class TYPE_STKM_CallStackItem : public Record
{
public:
  TYPE_STKM_CallStackItem() : Record(TAG_TYPE_STKM_CallStackItem, 13) {}

  TYPE_STKM_CallStackItem(const Generic &c) : Record(c) {}

  TYPE_STKM_CallStackItem &Init(const Int & p1,
    const Record & p2, const Int & p3, const Generic & p4,
    const Generic & p5, const Generic & p6, const TYPE_CI_ContextId & p7,
    const Generic & p8, const Generic & p9, const Generic & p10,
    const Generic & p11, const Bool & p12, const Generic & p13)
  { SetField(1,p1); SetField(2,p2); SetField(3,p3); SetField(4,p4);
    SetField(5,p5); SetField(6,p6); SetField(7,p7); SetField(8,p8);
    SetField(9,p9); SetField(10,p10); SetField(11,p11); SetField(12,p12);
    SetField(13,p13);return * this; }

  const Int & get_type() const { return GetInt(1); }
  void set_type(const Int &p) { SetField(1, p); }
  const Record & get_code() const { return GetRecord(2); }
  void set_code(const Record &p) { SetField(2, p); }
  const Int & get_pc() const { return GetInt(3); }
  void set_pc(const Int &p) { SetField(3, p); }
  const Generic & get_nmOrDesc() const { return GetField(4); }
  void set_nmOrDesc(const Generic &p) { SetField(4, p); }
  const Generic & get_arg_l() const { return GetField(5); }
  void set_arg_l(const Generic &p) { SetField(5, p); }
  const Generic & get_evalstate() const { return GetField(6); }
  void set_evalstate(const Generic &p) { SetField(6, p); }
  const TYPE_CI_ContextId & get_oldCid() const { return GetInt(7); }
  void set_oldCid(const TYPE_CI_ContextId &p) { SetField(7, p); }
  const Generic & get_env_l_h() const { return GetField(8); }
  void set_env_l_h(const Generic &p) { SetField(8, p); }
  const Generic & get_typeinst_h() const { return GetField(9); }
  void set_typeinst_h(const Generic &p) { SetField(9, p); }
  const Generic & get_os_h() const { return GetField(10); }
  void set_os_h(const Generic &p) { SetField(10, p); }
  const Generic & get_cur_mod_obj_l_h() const { return GetField(11); }
  void set_cur_mod_obj_l_h(const Generic &p) { SetField(11, p); }
  const Bool & get_rterror() const { return GetBool(12); }
  void set_rterror(const Bool &p) { SetField(12, p); }
  const Generic & get_measu() const { return GetField(13); }
  void set_measu(const Generic &p) { SetField(13, p); }
};

//typedef TYPE_STKM_CallStackItem CallStackItem;

typedef Stack TYPE_STKM_CallStack;

// }}}
// {{{ TrapStack

typedef Stack TYPE_STKM_TrapStack;

// }}}
// {{{ CFStack

typedef Tuple TYPE_STKM_CFStackItem;

typedef Stack TYPE_STKM_CFStack;

// }}}
// }}}

// {{{ EvaluatorStatus

// This struct contains the entire state of the evaluator.

typedef int InstructionNo;

typedef Generic TYPE_STKM_Context;

enum {
  vdm_STKM_EvaluatorStatus = TAG_TYPE_STKM_EvaluatorStatus,
#ifdef VDMSL
  length_STKM_EvaluatorStatus = 17,
#endif // VDMSL
#ifdef VDMPP
#ifndef VICE
  length_STKM_EvaluatorStatus = 19,
#else
  length_STKM_EvaluatorStatus = 21,
#endif // VICE
#endif // VDMPP
  pos_TYPE_STKM_EvaluatorStatus_eval_ustack = 1,
  pos_TYPE_STKM_EvaluatorStatus_call_ustack = 2,
  pos_TYPE_STKM_EvaluatorStatus_curCid = 3,
  pos_TYPE_STKM_EvaluatorStatus_trap_ustack = 4,
  pos_TYPE_STKM_EvaluatorStatus_PC = 5,
  pos_TYPE_STKM_EvaluatorStatus_debug_flag = 6,
  pos_TYPE_STKM_EvaluatorStatus_upDnIndex = 7,

#ifdef VDMSL
  pos_TYPE_STKM_EvaluatorStatus_cf = 8,
  pos_TYPE_STKM_EvaluatorStatus_env_ul = 9,
  pos_TYPE_STKM_EvaluatorStatus_env_ul_ubak = 10,
  pos_TYPE_STKM_EvaluatorStatus_typeinst = 11,
  pos_TYPE_STKM_EvaluatorStatus_typeinst_ubak = 12,
  pos_TYPE_STKM_EvaluatorStatus_os = 13,
  pos_TYPE_STKM_EvaluatorStatus_os_ubak = 14,
  pos_TYPE_STKM_EvaluatorStatus_cur_umod = 15,
  pos_TYPE_STKM_EvaluatorStatus_cur_umod_ubak = 16,
  pos_TYPE_STKM_EvaluatorStatus_context = 17
#endif // VDMSL

#ifdef VDMPP
  pos_TYPE_STKM_EvaluatorStatus_instrno = 8,
  pos_TYPE_STKM_EvaluatorStatus_guard = 9,
#ifdef VICE
  pos_TYPE_STKM_EvaluatorStatus_status = 10,
  pos_TYPE_STKM_EvaluatorStatus_release_time = 11,
  pos_TYPE_STKM_EvaluatorStatus_cf = 12,
  pos_TYPE_STKM_EvaluatorStatus_env_ul = 13,
  pos_TYPE_STKM_EvaluatorStatus_env_ul_ubak = 14,
  pos_TYPE_STKM_EvaluatorStatus_typeinst = 15,
  pos_TYPE_STKM_EvaluatorStatus_typeinst_ubak = 16,
  pos_TYPE_STKM_EvaluatorStatus_os = 17,
  pos_TYPE_STKM_EvaluatorStatus_os_ubak = 18,
  pos_TYPE_STKM_EvaluatorStatus_obj_ul = 19,
  pos_TYPE_STKM_EvaluatorStatus_obj_ul_ubak = 20,
  pos_TYPE_STKM_EvaluatorStatus_context = 21 
#else
  pos_TYPE_STKM_EvaluatorStatus_cf = 10,
  pos_TYPE_STKM_EvaluatorStatus_env_ul = 11,
  pos_TYPE_STKM_EvaluatorStatus_env_ul_ubak = 12,
  pos_TYPE_STKM_EvaluatorStatus_typeinst = 13,
  pos_TYPE_STKM_EvaluatorStatus_typeinst_ubak = 14,
  pos_TYPE_STKM_EvaluatorStatus_os = 15,
  pos_TYPE_STKM_EvaluatorStatus_os_ubak = 16,
  pos_TYPE_STKM_EvaluatorStatus_obj_ul = 17,
  pos_TYPE_STKM_EvaluatorStatus_obj_ul_ubak = 18,
  pos_TYPE_STKM_EvaluatorStatus_context = 19 
#endif // VICE
#endif // VDMPP
}; 
  
class EvaluatorStatus
{
public:
  TYPE_STKM_EvalStack eval_stack;
  TYPE_STKM_CallStack call_stack;
  TYPE_CI_ContextId curCid;
  TYPE_STKM_TrapStack trap_stack;
  TYPE_STKM_ProgramCounter pc;
  TYPE_STKM_DebugFlag debug_flag;
  Int upDnIndex;

#ifdef VDMPP
  InstructionNo instrno;  // nat
  Generic guard;          // [Guard]
#ifdef VICE
  TYPE_SCHDTP_ThreadStatus status; // SCHDTP`ThreadStatus
  Generic release_time;   // [nat]
#endif //VICE
#endif //VDMPP

  TYPE_STKM_CFStack cf;   // seq of ( seq of char * CI`ContextId)

  TYPE_SEM_ENVL env_l;    // seq of seq of SEM`BlkEnv
  TYPE_SEM_ENVL env_l_bak;// seq of seq of SEM`BlkEnv

  Sequence typeinst;      // seq of map AS`TypeVar to AS`Type
  Sequence typeinst_bak;  // seq of map AS`TypeVar to AS`Type

#ifdef VDMSL
  Sequence os;            // seq of GLOBAL`StateMap
  Sequence os_bak;        // seq of GLOBAL`StateMap

  SEQ<TYPE_AS_Name> cur_mod;       // seq of AS`Name
  SEQ<TYPE_AS_Name> cur_mod_bak;   // seq of AS`Name
#endif //VDMSL

#ifdef VDMPP
  Sequence os;            // seq of ( GLOBAL`OBJ_tab * map AS`Name to GLOBAL`SigmaClass )
  Sequence os_bak;        // seq of ( GLOBAL`OBJ_tab * map AS`Name to GLOBAL`SigmaClass )

  SEQ<TYPE_GLOBAL_OBJscope> obj_l;         // seq of GLOBAL`OBJscope
  SEQ<TYPE_GLOBAL_OBJscope> obj_l_bak;     // seq of GLOBAL`OBJscope
#endif //VDMPP

  bool rterror;
  Sequence context;

public:
  EvaluatorStatus();
  ~EvaluatorStatus();

  void ostream_out(wostream & o, const VDMFormatter & v) const;

  static Sequence typeinst_init();
  static TYPE_SEM_ENVL envl_init();
};

int vdm_Compare(const EvaluatorStatus & v1, const EvaluatorStatus & v2);
void vdm_ostream_out(const EvaluatorStatus & value, wostream & o);
typedef VDMContainer<EvaluatorStatus> EvaluatorStatusCt;

// }}}
// {{{ StackEval

class StackEval
{
  // {{{ The state and constructor

  // The current state of the evaluator.

//////////////////////
//  stack-eval.vdm
//////////////////////
private:

  EvaluatorStatusCt curr_state;
  TYPE_STKM_SubProgram curr_program;

  // pointer for hack
  EvaluatorStatus * cs_shared_p;
  TYPE_SEM_ENVL * cs_env_l_p;
  TYPE_STKM_EvalStack * cs_eval_stack_p;
  TYPE_STKM_CallStack * cs_call_stack_p;
  Sequence * cs_typeinst_p;
#ifdef VDMSL
  Sequence * cs_os_p;
  SEQ<TYPE_AS_Name> * cs_cur_mod_p;
#endif // VDMSL
#ifdef VDMPP
  Sequence * cs_os_p;
  SEQ<TYPE_GLOBAL_OBJscope> * cs_obj_l_p;
#endif // VDMPP
  Sequence * cs_context_p;
  int curr_program_length;
  
  Generic lastres; // lastres: [SEM`VAL]

  TYPE_CI_ContextId lastexit;

  bool BREAK;
  bool UserBREAK;
  MUTEX_T mutex_break;

  // State component from the debug module.
  MAP<Int, TYPE_DEBUGTP_BreakInfo> breakpoints;
  //MAP<TYPE_CI_ContextId, TYPE_AS_Expr> conditions;
  MAP<TYPE_CI_ContextId, Tuple> conditions; // map CI`ContextId to (AS`Expr * STKM`SubProgram)
  int inActiveLevel;
  int breakId;

#ifdef VDMPP
  bool usesThreads;
#endif // VDMPP

  bool usesOldName;

#ifdef VDMPP
  static TYPE_GLOBAL_Start global_start;
#endif // VDMPP

  TYPE_AS_Name void_name;

public:
  // The constructor
  StackEval();
  ~StackEval();
  void DeleteMe(); // delete this;
  // }}}
  // {{{ Methods from stackeval.cc
#ifdef VDMPP
public:
  void SetUsesThreads(bool b) { this->usesThreads = b; };
  bool UsesThreads() { return this->usesThreads; };
#endif // VDMPP

  void SetUsesOldName(bool b) { this->usesOldName = b; };
  bool UsesOldName() { return this->usesOldName; };

public:
  bool IsInitialized();
  void ResetEnvlInError();

  void SetLastRes(const Generic & res) { this->lastres = res; };
  Generic GetLastRes() const { return this->lastres; };

  void SetLastExitCid(const TYPE_CI_ContextId & cid) { this->lastexit = cid; };
  TYPE_CI_ContextId GetLastExitCid() { return this->lastexit; };

//  void Instantiate(const EvaluatorStatusCt & st, const TYPE_STKM_SubProgram & instr);
  void Instantiate(const EvaluatorStatusCt & st, const Generic & instr);
  EvaluatorStatusCt GetEvaluatorState(void);
  void SetEvaluatorState(const EvaluatorStatusCt & st);
  TYPE_STKM_SubProgram GetProgram();
  void SetProgram(const TYPE_STKM_SubProgram & pr);
  void SetCurrentState(const EvaluatorStatusCt & st);
  void SetCurrentProgram(const TYPE_STKM_SubProgram & pr);
#ifdef VDMPP
  void LoadCurrntEvaluatorStateFromCPUSigma();
  void SaveCurrntEvaluatorStateToCPUSigma();
  void LoadCurrntProgramFromCPUSigma();
  void SaveCurrntProgramToCPUSigma();
  EvaluatorStatusCt InitEvaluatorStatus(const TYPE_STKM_SubProgram& instr_, const Generic & objref) const;
#endif // VDMPP
  TYPE_STKM_SubProgram ExtractInstr(const TYPE_STKM_Code &); 

  void stackeval_Init(bool ast_is_new);
  void User_Init(const TYPE_AS_Document & ast, bool ast_is_new);

  TYPE_SEM_ENVL envl_init();
  void ResetEnvL();
  void ResetTypeInst();
  Sequence typeinst_init();
  void PushOS();
  void PopOS();
#ifdef VDMPP
private:
  const TYPE_GLOBAL_OBJscope & HeadObjL() const;
#endif // VDMPP

public:
  void PushTypeInst(const Map & typemap);
  void PopTypeInst();
  Map HdTypeInst() const;
  void PushContext(const TYPE_STKM_Context & context);
  void PopContext();
  TYPE_STKM_Context HdContext() const;
  void CheckContext(const SET<TYPE_STKM_Context> & contexts, const TYPE_RTERR_ERR & err);
  Tuple GetEnvLLengths() const;
  void UpgradeENVL(const Int & lenvl, const Int & ltopenvl);
  void PopEnvL ();
  SEQ<TYPE_SEM_BlkEnv> TopEnvL() const;
  void PushEmptyBlkEnv(const TYPE_SEM_Permission & permis);
  void PushEmptyEnv ();
  void PushEnvL (const SEQ<TYPE_SEM_BlkEnv> & env_l);
  bool IsEmptyEnvL () const;
  void AddToTopBlkEnv(const TYPE_SEM_BlkEnv & blkenv);

#ifdef VDMPP
  int GetObjLLen() const;
  const TYPE_SEM_OBJ_uRef & GetCurObjRef() const;
  TYPE_AS_Name GetCurObjName() const;
  const TYPE_GLOBAL_OrigCl & GetCurCl() const;
  TYPE_GLOBAL_OrigCl GetPrevCl() const;
  const TYPE_GLOBAL_OrigCl & GetOrigCl() const;
  const TYPE_GLOBAL_OrigCl & GetOrigOldCl() const;
  bool HasCurCl() const;
  bool HasCurObjRef() const;
  TYPE_SEM_OBJ GetCurObj() const;
  bool IsEmptyObjL() const;
  void PushCurObj(const TYPE_SEM_OBJ_uRef & objref,
                  const Generic & clnm,
                  const Generic & origcl);
  void PopCurObj();
  void PushClNmCurObj(const TYPE_AS_Name & clnm, const TYPE_GLOBAL_OrigCl & origcl);
  void PopClNmCurObj();
#ifdef VICE
  Generic TopClNmCurObj();
#endif // VICE
#endif // VDMPP
  void PopBlkEnv ();
  void PushBlkEnv (const TYPE_SEM_BlkEnv & env);
  void AppendToTopBlkEnv(const TYPE_AS_Name & id, const TYPE_SEM_VAL & val, const Generic & tp);
private:
  TYPE_SEM_BlkEnv TopBlkEnv() const;

#ifdef VDMPP
public:
  TYPE_SEM_OBJ_uRef MakeNewObj(const TYPE_SEM_OBJ & semobj, const Generic & dlclassp);
private:
#endif // VDMPP

  bool GoUp();
  bool GoDown();
  void UpdateStacks(const Tuple &);
//  void UpdateSeqStack(Sequence & stk1, Sequence & stk2, const Int & len) const;
  void ResetUpDn();
  Generic GetNextStackLevelsUp();
  Generic GetNextStackLevelsDown();

public:
  int CurrentBacktraceLevel() const;
  TYPE_CI_ContextId GetCidForCurBacktraceLevel() const;

#ifdef VDMSL
  void PushModule (const TYPE_AS_Name & mod_name);
  void ResetCurMod(); // Used by tools.cc:User_Init.
  const TYPE_AS_Name & CurrentModule (void) const;
  bool PrePushModule(const TYPE_AS_Name &);
  // Used by EvalPushModule, EvalPopModule:
  void PopModule ();
  TYPE_AS_Name HeadModule() const;
#endif // VDMSL

  Tuple IsLocalState(const TYPE_AS_Name & id) const;
  void SetLocalState(const TYPE_AS_Name & id, const TYPE_SEM_ValTp & valtp);
  // Used by freevars.cc: LookUpInTopEnv
  Tuple IsLocalVal (const TYPE_AS_Name & name);
#ifdef VDMSL
  int  IsOldState (const TYPE_AS_Name & id) const;
  TYPE_GLOBAL_State GetOldState (const TYPE_AS_Name & id) const;
#endif //VDMSL

private:
  TYPE_SEM_VAL EvalOldName (const TYPE_AS_OldName & oldid);

  Sequence PrintCf();
  void PushCF(const TYPE_STKM_CFStackItem &);
  void PopCF();
  const TYPE_STKM_CFStack & GetCF();

  // Eval Stack 
public:
  TYPE_STKM_EvalStackItem Pop(void);

private:
  SEQ<TYPE_STKM_EvalStackItem> Pop(const Int & n);
  void Push(const TYPE_STKM_EvalStackItem& e);
  const TYPE_STKM_EvalStackItem & Head() const;
  const TYPE_STKM_EvalStackItem & GetES(const Int & n) const;
  bool LenES(const Int & n) const;
  void RemENth(const Int & n);

public:
  int EvalStackLevel() const;

  // Call Stack
  void PushCS(const TYPE_STKM_Code & item, const Generic & nm, const Generic & val_l, CallStackItemType::CSItemType);
  void PopCS(void);
  TYPE_STKM_CallStackItem HeadCS() const;
  TYPE_STKM_CallStackItem GetCS(const Int & n) const;
  const TYPE_STKM_CallStack& GetBackTrace();
  int CallStackLevel() const;

private:
  void PushDS(const EvaluatorStatusCt & evalst, const SEQ<Char> & debugString, const TYPE_STKM_Code & code);
  void DoPopDS();

public:
  void UserPopDS();

private:
  Generic get_objref_from_fnop(const TYPE_SEM_VAL & fnop);

public:
  void ReplaceEvaluatorStatus(EvaluatorStatusCt & ev);

  // curCid
  void SetCid(const TYPE_CI_ContextId & cid);
  TYPE_CI_ContextId GetCurCid() const;

  // trap_stack
  void PushTS(const Int & handid);
  void PopTS();
  bool EmptyTS() const;
  void GotoTrapHandler(const TYPE_SEM_EXIT & item);
  TYPE_STKM_ProgramCounter FindTrapHandler(const Int & handid, int lengthcallstack);

  // measure
  void MeasureCheck(const TYPE_SEM_VAL & mv);

  // pc
  void IncrPC(const Int & n);
  bool IsProgramAtEnd() const;
  void TerminateProgram();

  // debug_flag
  void SetStep();
  void SetStepIn();
  void SetSingleStep();
  void SetFinish();
  void SetContinue();
  TYPE_STKM_DebugFlag GetDebugFlag() const;
  void SetDebugFlag(const TYPE_STKM_DebugFlag &);

private:
  void SetBREAK();
  void ResetBREAK();
  bool GetBREAK();

public:
  void SetUserBREAK();
  void ResetUserBREAK();
  bool GetUserBREAK();

#ifdef VDMPP
  // guard
  void SetGuard(const Generic &, const Generic &);
  Tuple EvalRunGuard(const TYPE_STKM_SubProgram& instr, const Generic & objref);
  //TYPE_SEM_VAL EvalFieldOBJRefSelect(const TYPE_SEM_OBJ_uRef & objref, const TYPE_AS_Expr & field);
  TYPE_SEM_VAL EvalFieldOBJRefSelect(const TYPE_SEM_OBJ_uRef & objref, const TYPE_AS_Name & field);
  void ResetGuard();
#endif //VDMPP

  void SetRunTimeError();
  bool RunTimeError() const;

  void ThrowRuntimeErrorException();
  bool IsRuntimeErrorException(const Generic & v) const;
  bool RuntimeErrorExceptionOccurred() const;
  void PushRuntimeErrorVal();
  TYPE_SEM_EXIT RuntimeErrorVal();

  Tuple EvalMainLoop();
  Tuple EvalUninterruptedLoop();
  TYPE_STKM_EvaluationState MainLoopState();
  void EvalInstr(const TYPE_INSTRTP_Instruction & i) ;

  // }}}
  // {{{ Methods from instructions.cc
/////////////////////////
// instructions.vdm
/////////////////////////
public:
  void ExeCBR(const Int & n);
  void ExeCNBR(const Int & n);
  void ExeBR(const Int & n);
  void ExeERRINST(const TYPE_RTERR_ERR & err);
  void ExePRE();
  void ExePOST();
  void ExeASSERT();
  void ExeCASES();
  void ExeMEASURE();
  void ExeMEASURETPINST(const SEQ<TYPE_AS_TypeVar> & tpparms);
  void ExeMEASURECHECK();
  void ExeDTCMEASURE();
  void ExePOSTENV(const SEQ<TYPE_AS_NameType> & resnmtps, const TYPE_CI_ContextId & ci);
  void ExeNOBODY(const TYPE_RTERR_ERR & err,
                 const TYPE_AS_Name & modname,
                 const TYPE_AS_Name & name,
                 const SEQ<TYPE_AS_Pattern> & parms);
  SET<TYPE_SEM_VAL> TypeToSet(const TYPE_AS_Type & tp);
  bool IsIntNum(const TYPE_AS_Expr & expr);
  Generic GetIntNum(const TYPE_AS_Expr & expr);
  Tuple ExprToSet(const TYPE_AS_Name & name, const TYPE_AS_Expr & expr, const Generic & min, const Generic & max);
  void ExeTPTOSET(const TYPE_AS_Type & tp);
  void ExePUSHLIST(const Int & length);
  void ExeCONTEXT(const TYPE_CI_ContextId & cid, const Bool & isStmt);
  void ExeSELBLKENV(const Int & n);
  void ExeAPPENDBLKENV(const TYPE_AS_Name & id, const Generic & tp);
  void ExeREMSTACKELEM(const Int & n);
  void ExeSWAP();
  void ExeMULTBINDL(const Int & n, const Int & part);
  void ExePOPBLKENV();
  void ExeADDTOBLKENV();
  void ExeMKEXIT(const Bool & isnil, const TYPE_CI_ContextId & cid);
  void ExeEXITVAL();
  void ExeISCONT();
  void ExeISEXIT();
  void ExeISNEEXIT();
  void ExeREMEXITVAL();
  void ExePUSHTH(const Int & movePC);
  void ExePOPTH();
  void ExeLOOKUP(const TYPE_AS_Expr & nm);
#ifdef VDMPP
  void ExeLOOKUPSTATIC(const TYPE_AS_Name&);
  void ExeLOOKUPOP(const TYPE_AS_Name&);
#endif //VDMPP
  void ExeAPPLY();
  void ApplyOpFnMapSeq(const TYPE_SEM_VAL &, const Generic&);
  void EvalCompExplFnApply (const TYPE_SEM_CompExplFN & fns, const Generic & arg);
  void EvalExplFnApply(const TYPE_SEM_ExplFN & fct_v, const SEQ<TYPE_SEM_VAL> & arg_lv);
  SEQ<TYPE_SEM_VAL> UpdateFunctionValue(const SEQ<TYPE_SEM_VAL> & arg_lv);
  void EnvSetUpExplFnApply(const TYPE_SEM_ExplFN & fndef, const SEQ<TYPE_SEM_VAL> & arg_lv);
  void EvalExplOpApply(const TYPE_SEM_ExplOP & opsem, const SEQ<TYPE_SEM_VAL> & arg_l);
  void EnvSetUpExplOpApply(const TYPE_SEM_ExplOP & fndef, const SEQ<TYPE_SEM_VAL> & arg_l);
#ifdef VDMSL
  void EvalDLFnApply(const TYPE_SEM_DLFN & fndef, const SEQ<TYPE_SEM_VAL> & arg_l);
  void EvalDLOpApply(const TYPE_SEM_DLOP & fndef, const SEQ<TYPE_SEM_VAL> & arg_l);
#endif // VDMSL
#ifdef VDMPP
  void EvalOverOpFnApply(const TYPE_SEM_OverOPFN &, const SEQ<TYPE_SEM_VAL> &);
  bool CheckMultiOverLoaded(const MAP<TYPE_AS_Name, TYPE_AS_Name> & m,
                            const TYPE_SEM_OverOPFN & fct_v, const SEQ<TYPE_SEM_VAL> & arg_lv);
  void ExeCALLGUARD(const Bool & hasobj, const TYPE_AS_Name & oprt);
  Generic ConvertOverOPFNToExplOP(const Record & op, const Generic & obj,
                                  const Bool & hasobj, const SEQ<TYPE_SEM_VAL> & args);
  void ExePPCALL();
#endif // VDMPP
  void ExeDLCALL(const TYPE_AS_Name & clname, const TYPE_AS_Name & name);
  void ExeRETURN();
  void ExeGETPRE();
  void ExeISCHECK(const Record & Type);
  void ExeEOCL();
  void CleanFunctionApply();
  void CleanExplFnApply(const TYPE_SEM_ExplFN & fndef);
  void CleanExplOpApply(const TYPE_SEM_ExplOP & fndef);
  void EvalUNOP(const Int & opr);
  void EvalBINOP(const Int & opr);
  void ExeAPPENDSEQ();
  void ExeADDSET();
  void ExeAPPENDMAP();
  void ExeSELSEQELEM();
  void ExeSETRNG();
  void ExeSUBSEQ();
  void ExeRECCONS(const TYPE_AS_Name & tag, const Int & length);
  void ExeRECMOD(const SEQ<TYPE_AS_Name> & fid);
  void ExeFIELDSEL(const TYPE_AS_Name & field);
  void ExeISEMPTYSET(const Int & n);
  void ExeISEMPTYSEQ(const Int & n);
  void ExeISNOTEMPTYSET(const Int & n);
  void ExeISNOTEMPTYSEQ(const Int & n);
  void ExeTOKENVAL();
  void ExeTUPSEL(const Real & index);
  void ExeTYPEJUDGE(const TYPE_AS_Type & tp);
  void ExePOLYINST(const SEQ<TYPE_AS_Type> & inst, const TYPE_AS_Name & nm);
//  TYPE_SEM_POLY ExtendTypeInfo(const TYPE_SEM_POLY & polyfn, const TYPE_AS_Name & usedclass);
  TYPE_GLOBAL_Type UpdateTypeInfo(const TYPE_GLOBAL_Type & tp, const TYPE_AS_Name & usedclass) const;
  void ExeTUPCONS(const Int & length);
  void ExeSETCONS(const Int & length);
  void ExeSEQCONS(const Int & length);
  void ExeMAPCONS(const Int & length);
  void ExeSEQCOMPBIND(const Bool & isSeq);
  void ExeSEQFOR();
  SET<TYPE_AS_Name> NameInPattern(const TYPE_STKM_Pattern & pat);
  void ExeSEQMAPOVER();
  void ExeSETENUM(const Int & length);
  void ExeSEQENUM(const Int & length);
  void ExeMAPLET();
  void ExeMAPENUM(const Int & length);
  void ExeSetUnion();
  void ExeSeqConc();
  void ExeMapMerge();
#ifdef VDMPP
  void ExeOBJECTPAT(const TYPE_AS_Name & cls, const Int & length);
  void ExeFIELDPAT(const TYPE_AS_Name & nm);
#endif // VDMPP
  void ExeRECPATCONS(const TYPE_AS_Name & tag, const Int & length);
  void ExeTUPPATCONS(const Int & length);
  void ExeFREF(const TYPE_AS_Name & nm);
  void ExeMOSREF();
  void ExeATOMIC(const Int & no);
  void ExeMACRO(const Int & tp, const TYPE_CI_ContextId & cid);
  void ExeASSIGNSD();
  void InvOK() /*notconst*/;
  void ExeSEQELEMMATCH(const Int & index);
  void ExeEmptyBlkEnv(const TYPE_SEM_Permission & permis);
  void ExeMatchAndBind();
  void BindPat(const TYPE_STKM_Pattern & pat, const TYPE_SEM_VAL & val);
  void ExeCOPYVAL();
  void ExeDTC(const TYPE_AS_Type & tp);
  void ExeDTCSET();
  void ExeSIZE(const Int & n);
  void ExeTRYANYMATCH();
  void ExeMATCHVAL();
  void ExeCLOSENV (const TYPE_SEM_BlkEnv & blkenv, const Map & bodym);
  void ExeUPDATECLOSENV(const TYPE_AS_Expr & expr);
  void ExeVERIFYINDEXARGS();
  void ExeTESTCOUNTER();
  void ExeINCRCOUNTER();
  void ExeNONDETSTMT();
  void ExeRANDOM();
  SEQ<Int> Permute(const SEQ<Int> &);
#ifdef VDMPP
  void ExeNEWOBJ (const TYPE_AS_Name & name, const Generic & dlobject);
  Generic CreateNEWDLClass(const TYPE_AS_Name & name, const Generic & dlobject);
  void ExeNEWPOSABSOBJ (const TYPE_AS_Name & name, const Generic & dlobject);
  void ExeINITCLASS (const TYPE_AS_Name & nm, const Int &);
  void ExeBINDINSTVAR (const TYPE_AS_Name & clnm, const TYPE_AS_Name & nm);
  void ExeNEWCOMPL(const Bool & checkinv);
  void ExePUSHCLNMCUROBJ (const TYPE_AS_Name &, const TYPE_AS_Name &);
  void ExePOPCLNMCUROBJ();
  void ExeSELFEXPR();
  void ExeISOFCLASS (const TYPE_AS_Name & clnm);
  void ExeISOFBASECLASS (const TYPE_AS_Name & clnm);
  void ExeSAMEBASECLASS();
  void ExeSAMECLASS();
  void ExeHISTORY(const TYPE_INSTRTP_HistoryKind &, const SEQ<TYPE_AS_Name> &);
  void ExeSTARTLIST(const Bool &);
  void ExeSTOPLIST(const Bool &);
  void ExeGUARD(const Bool &);
  void GuardCheck(const TYPE_SEM_ExplOP &, const Generic &, const SEQ<TYPE_SEM_VAL> &, const Bool &);
  void ExeTHREADID();

#ifdef VICE
  void ExeINCRTIME (const Int & n);
  void ExeINCRTIME_PREF (const TYPE_AS_UnaryOp & opr, const Int & oh);
  Int dsetsize (const TYPE_SEM_SET & s);
  Int dseqsize (const TYPE_SEM_SEQ & s);
  Int dmapsize (const TYPE_SEM_SET & s);
  Generic GetVal(const TYPE_STKM_EvalStackItem &);
  void ExeINCRTIME_BIN (const TYPE_AS_BinaryOp & opr, const Int & oh1, const Generic & oh2);
  void ExeINCRTIME_NEW (const Int & oh, const TYPE_AS_Name & nm);
  Int num_instvars (const Sequence & ivs);
  Int map_sum (const Map & m);
  void ExeINCRTIME_SETSEQMAP (const Int & oh);
  void ExeINCRTIME_STARTLIST (const Int & oh);
  void ExeINCRTIME_STOPLIST (const Int & oh);
  void TimeError();
  void ExeRETMES(const TYPE_STKM_MessageId & id,
                 const Int & prio,
                 const TYPE_AS_Name & opnm,
                 const Generic & tocpu,
                 const TYPE_SCHDTP_ThreadId & threadid);
#endif //VICE
#endif // VDMPP
  void ExeISTART(const TYPE_AS_Id & txt, const TYPE_CI_ContextId & cid);
  void ExeIEND();
#ifdef VDMPP
  void ExeADDTMPOBJREF();
#endif //VDMPP

  // }}}
  // {{{ Methods from debug.cc

//////////////////////
// debug.vdm
/////////////////////
public:
  Tuple EvalDebug(const Record & e, const SEQ<Char> & debugString);
  Tuple EvalPrintDebugAux(const Record & e, const SEQ<Char> & debugString);
  Tuple EvalRun(bool);
  Tuple EvalAuxCmd(const TYPE_AS_Expr &, const TYPE_STKM_SubProgram &, const TYPE_STKM_SubProgram &, const SEQ<Char> &);
  Tuple EvalUninterruptedCmd(const TYPE_AS_Expr &,
                             const TYPE_STKM_SubProgram &,
                             const TYPE_STKM_SubProgram &,
                             const SEQ<Char> &);
  Tuple EvalAuxProgram(const TYPE_STKM_SubProgram & instr, const SEQ<Char> & debugStr, bool interruptible);
  Tuple EvalPrint(const Record & e, const SEQ<Char> & debugString);
  Tuple EvalStep();
  Tuple EvalStepIn();
  Tuple EvalSingleStep();
  Tuple EvalContinue();
  Tuple EvalFinish();
  Tuple RunIfAllowed();
  bool IsSteppingAllowed();

#ifdef VDMSL
  TYPE_SEM_VAL EvaluateExpression(const TYPE_AS_Name & clmod, const TYPE_AS_Expr & e, const TYPE_SEM_BlkEnv & env);
#endif // VDMSL
#ifdef VDMPP
  TYPE_SEM_VAL EvaluateExpression(const TYPE_AS_Name & clmod, const Generic & obj,
                                  const TYPE_AS_Expr & e, const TYPE_SEM_BlkEnv & env);
#endif // VDMPP

public:
  Tuple EvalBreakName(const TYPE_AS_Name & name, const Int & id, const TYPE_DEBUGTP_BreakStatus & staus);
  Tuple EvalBreakPos(const TYPE_AS_Id & fileName,
                     const Int & line,
                     const Int & col,
                     const Int & id,
                     const TYPE_DEBUGTP_BreakStatus& staus);
  Tuple DeleteBreakpoint(const Int & num);
  SET<Int> UpdateBreakPoint();

private:
  bool UpdateFnNameBreakInfo(const Int & id, const TYPE_DEBUGTP_FnNameBreakInfo & info);
  bool UpdatePosBreakInfo(const Int & id, const TYPE_DEBUGTP_PosBreakInfo & info);
  bool ActiveBreakpoint(const TYPE_CI_ContextId & cid);

public:
  bool ExistsBreakpointForName(const TYPE_AS_Name &modClNm, const TYPE_AS_Name &name);
  bool ExistsBreakpointForPos(const TYPE_AS_Id & name, const Int & line, const Int & col);
  Int GetBreakpointNumForName(const TYPE_AS_Name & modClNm, const TYPE_AS_Name & name);
  Int GetBreakpointNumForPos(const TYPE_AS_Id & name, const Int & line, const Int & col);
  Tuple EnableBreakpoint(const Int & num);
  Tuple DisableBreakpoint(const Int & num);

  void ResetInActivity();
  void ActivateAllBreakpoints();
  void DeActivateAllBreakpoints();
  void PrintBreakpoints(wostream & out);
  void ResetBreakpoints ();

  Tuple AddCondition(const Int & id, const TYPE_AS_Expr & e);
  Tuple RemoveCondition(const Int & id);
  bool CheckCondition(const TYPE_CI_ContextId & cid, wostream & wos);
  void PrintConditions(wostream & wos);
  void PrintCondition(const TYPE_CI_ContextId & cid, wostream & wos);

  bool EvalStackUp();
  bool EvalStackDown();

#ifdef VDMPP
  Map EvalThreads();
  void EvalSelThread(const TYPE_SCHDTP_ThreadId & id);
  TYPE_SCHDTP_ThreadId EvalCurThread();
#endif // VDMPP

  void ClearProfile();
  void PrintProfile();

// Random number generator for Permute
public:
  void InitRandom(unsigned long);
  void InitMathRandom();
  int GetRandom(int limit);

private:
  Random per_rnd;
  // }}}
private:
  VdmStdLib stdlib;
};

// }}}
// {{{ Footer

#endif // __stackeval_h__

// }}}

// Force emacs to use C++ mode for this file.
//// Local Variables:
//// mode: c++
//// End:
