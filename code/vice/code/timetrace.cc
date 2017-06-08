/***
*  * WHAT
*  *    Implementation of module TIMETRACE (VICE)
*  * COPYRIGHT
*  *    (C) Kyushu University
*  * SPECIFICATION VERSION
*  *    implementation of timetrace.vdm 1.5
*  *                      
***/

#include "metaiv.h"
#include "AS.h"
#include "stackeval.h"
#include "astaux.h"
#include "TIMETRACE.h"
#include "tb_wstring.h"
#include "rt_errmsg.h"
#include "TIMETRACETP.h"
#include <fstream>
#include "libman.h"
#include "valcond.h"
#include "MANGLE.h"
#include "statsem.h"

//
Map lasttime;
//
//
bool TIMETRACE::new_log = true;
int TIMETRACE::new_log_id = 0;
Sequence TIMETRACE::log;
wstring TIMETRACE::logfile = L"logfile";
Map TIMETRACE::id_m;
Map TIMETRACE::busid_m;
Int TIMETRACE::nextid = Int(0);
Int TIMETRACE::nextbusid = Int(0);
Generic TIMETRACE::logargs = Set();
MAP<TYPE_SCHDTP_ThreadId, Int> TIMETRACE::threadcreate;

bool TIMETRACE::logging = false;
ofstream TIMETRACE::tracelogstream;

Map TIMETRACE::lasttime;

void TIMETRACE::LoggingOn()
{
  TIMETRACE::logging = true;
  TIMETRACE::lasttime.Clear();
}

void TIMETRACE::LoggingOff()
{
  TIMETRACE::logging = false;
}

// SetLogFile
// newlogfile : seq of char
void TIMETRACE::SetLogFile(const wstring &newlogfile)
{
  TIMETRACE::logfile = newlogfile;
  TIMETRACE::new_log = true;
  TIMETRACE::lasttime.Clear();
}

// AddCPU
// sysnm : [AS`Name]
// cpunm : [AS`Name]
// expl : bool
// ==> ()
void TIMETRACE::AddCPU(const Generic & sysnm, const Generic & cpunm,
                       const Bool & expl)
{
  Int num (ConvertNameToNum(cpunm));
  if (num.GetValue() > 0)
  {
    TYPE_TIMETRACETP_CPUdecl decl;
    decl.set_id(num);
    decl.set_sys(sysnm);
    decl.set_name(cpunm);
    decl.set_expl(expl);
    AddToLogfile(decl);
  }
}

// AddBUS
// busnm : [AS`Name]
// cpus : set of [AS`Name]
// ==> ()
void TIMETRACE::AddBUS(const Generic & busnm, const Set & cpus)
{
  Int num (ConvertBUSNameToNum(busnm));
  SET<Int> converted;
  Set cpus_q (cpus);
  Generic cpu;
  for(bool bb = cpus_q.First(cpu); bb; bb = cpus_q.Next(cpu))
  {
    converted.Insert(ConvertNameToNum(cpu));
  }
  if (num.GetValue() > 0)
  {
    TYPE_TIMETRACETP_BUSdecl decl;
    decl.set_id(num);
    decl.set_topo(converted);
    decl.set_name(busnm);
    AddToLogfile(decl);
  }
}

// LogHistEvent
// thireadid : SCHDTP`ThreadId
// clnm : AS`Name
// opname : AS`Name
// kind : INSTRTP`HistoryKind
// objref : SEM`OBJ_Ref
// async : bool
// vals : [seq of SEM`VAL]
// ==> ()
void TIMETRACE::LogHistEvent(const TYPE_SCHDTP_ThreadId & threadid,
                             const TYPE_AS_Name & clnm,
                             const TYPE_AS_Name & opname, 
                             const TYPE_INSTRTP_HistoryKind & kind,
                             const TYPE_SEM_OBJ_uRef & objref_,
                             const Bool & async,
                             const Sequence & vals_)
{
  TYPE_SEM_OBJ_uRef objref (ConvertObjRef(objref_));
  Sequence vals (ConvertObjRef(vals_));

  Int curtime (GetTime());
  Int cpu (ConvertNameToNum(GetCurCPU()));
  Sequence val;
  if( !vals.IsEmpty() && kind.Is(TAG_TYPE_INSTRTP_fin))
    val.ImpAppend(vals.Hd());
  else
    val = vals; 

  TYPE_AS_Ids opids (opname.get_ids());
  TYPE_AS_Name opname2 (opname);
// 20090529 -->
  if (MANGLE::IsMangled(opname))
  {
    Tuple t (MANGLE::UnMangle(opname));
    const TYPE_AS_Name & nm (t.GetRecord(1));
    const SEQ<TYPE_AS_Type> & tp_l (t.GetSequence(3));
    TYPE_AS_Ids ids (nm.GetSequence(pos_AS_Name_ids));
    size_t len_ids = ids.Length();
    TYPE_AS_Id id (ids[len_ids]);
    id.ImpConc(SEQ<Char>(L"("));
    size_t len_tp_l = tp_l.Length();
    for (size_t i = 1; i <= len_tp_l; i++)
    {
      if (i > 1)
        id.ImpConc(SEQ<Char>(L", "));
      id.ImpConc(SEQ<Char>(GetStatSem().ASType2Ascii(tp_l[i])));
    }
    id.ImpConc(SEQ<Char>(L")"));
    ids.ImpModify(len_ids, id);
    opids = ids;
    opname2 = nm;
  }
//wcout << L"opname:  " << ASTAUX::ASName2String(opname) << endl;
//wcout << L"opname2: " << ASTAUX::ASName2String(opname2) << endl;
//wcout << L"kind: " << kind << endl;
// <-- 20090529
  
  switch(kind.GetTag()) {
    case TAG_TYPE_INSTRTP_req:
    {
      TYPE_TIMETRACETP_OpRequest new_event;
      new_event.Init(threadid, opids, objref, clnm.get_ids(), cpu, val, async, curtime);
      VC::AddOpReq(opname2, val, threadid, curtime);
      AddToLogfile(new_event);
      break;
    }
    case TAG_TYPE_INSTRTP_act:
    {
      TYPE_TIMETRACETP_OpActivate new_event;
      new_event.Init(threadid, opids, objref, clnm.get_ids(), cpu, async, curtime);
      VC::AddOpAct(opname2, threadid, curtime);
      AddToLogfile(new_event);
      break;
    }
    case TAG_TYPE_INSTRTP_fin:
    {
      TYPE_TIMETRACETP_OpCompleted new_event;
      new_event.Init(threadid, opids, objref, clnm.get_ids(), cpu, val, async, curtime);
      VC::AddOpFin(opname2, val, threadid, curtime);
      AddToLogfile(new_event);
      break;
    }
    default:
    {
      RTERR::Error(L"TIMETRACE::LogHistEvent", RTERR_UNEXPECTED_HISTORY_EVENT, Nil(), Nil(), Sequence());
      break;
    }
  }
}

// LogThreadSwapIn
// threadid : SCHDTP`ThreadId
// objref : [SEM`OBJ_Ref]
// clnm : [AS`Name]
// overhead : nat
void TIMETRACE::LogThreadSwapIn(const TYPE_SCHDTP_ThreadId & threadid,
                                const Generic & objref_,
                                const Generic & clnm,
                                const Int & overhead)
{
  Generic objref (ConvertObjRef(objref_));

  Generic ids;
  if (clnm.IsNil())
    ids = Nil();
  else
    ids = ((TYPE_AS_Name) clnm).get_ids();
//  Int curtime (GetTime());
//  Int curtime (SCHD`MAX(TIMETRACE::threadcreate(threadid), GetTime()));
  
// TODO
  Int curtime (GetTime());
  if( curtime < TIMETRACE::threadcreate[threadid] )
    curtime = TIMETRACE::threadcreate[threadid];
  
  Int cpu (ConvertNameToNum(GetCurCPU()));

  TYPE_TIMETRACETP_ThreadSwapIn t_in;
  t_in.Init(threadid, objref, ids, cpu, overhead, curtime);
  
  AddToLogfile(t_in);
}

// LogDelayedThreadSwapIn
// threadid : SCHDTP`ThreadId
// objref : [SEM`OBJ_Ref]
// clnm : [AS`Name]
// delay : nat
// overhead : nat
void TIMETRACE::LogDelayedThreadSwapIn(const TYPE_SCHDTP_ThreadId & threadid,
                                       const Generic & objref_,
                                       const Generic & clnm,
                                       const Int & delay,
                                       const Int & overhead)
{
  Generic objref (ConvertObjRef(objref_));

  Int curtime (GetTime());
  Int cpu (ConvertNameToNum(GetCurCPU()));

  Generic ids;
  if (clnm.IsNil())
    ids = Nil();
  else
    ids = ((TYPE_AS_Name) clnm).get_ids();

  TYPE_TIMETRACETP_DelayedThreadSwapIn t_in;
  t_in.Init(threadid, objref, ids, delay, cpu, overhead, curtime);
  
  AddToLogfile(t_in);
}

// LogThreadSwapOut
// threadid : SCHDTP`ThreadId
// objref : [SEM`OBJ_Ref]
// clnm : [AS`Name]
// overhead : nat
void TIMETRACE::LogThreadSwapOut(const TYPE_SCHDTP_ThreadId & threadid,
                                 const Generic & objref_,
                                 const Generic & clnm,
                                 const Int & overhead)
{
  Generic objref (ConvertObjRef(objref_));

  Int curtime (GetTime());
  Int cpu (ConvertNameToNum(GetCurCPU()));

  Generic ids;
  if (clnm.IsNil())
    ids = Nil();
  else
    ids = ((TYPE_AS_Name) clnm).get_ids();

  TYPE_TIMETRACETP_ThreadSwapOut t_in;
  t_in.Init(threadid, objref, ids, cpu, overhead, curtime);
  
  AddToLogfile(t_in);
}

// LogThreadCreate
// tid : SCHDTP`ThreadId
// peri : bool
// objref : [SEM`OBJ_Ref]
// clnm : [AS`Name]
// cpunm : [AS`Name]
// ==> ()
void TIMETRACE::LogThreadCreate(const TYPE_SCHDTP_ThreadId & tid,
                                const Bool & peri,
                                const Generic & objref_,
                                const Generic & clnm,
                                const Generic & cpunm)
{
  Generic objref (ConvertObjRef(objref_));

  Int cpu (ConvertNameToNum(cpunm));
  Int now (theSystem().GetCPUTime(cpunm));
//  Int now (GetTime(cpunm)); // TODO

  TIMETRACE::threadcreate.ImpModify(tid, now);
  TYPE_TIMETRACETP_ThreadCreate t_in;
  t_in.Init(tid, peri, objref, clnm, cpu, now);
  AddToLogfile(t_in);
}

// LogKillThread
// tid : SCHDTP`ThreadId
// cpunm : [AS`Name]
// ==> ()
void TIMETRACE::LogKillThread(const TYPE_SCHDTP_ThreadId & tid,
                              const Generic & cpunm)
{
  Int cpu (ConvertNameToNum(cpunm));
  TYPE_TIMETRACETP_ThreadKill t_in;
  t_in.Init(tid, cpu, Int(GetTime()));

  AddToLogfile(t_in);
}

// LogMessageRequest
// busid : [AS`Name]
// fromcpu : [AS`Name]
// tocpu : [AS`Name]
// msgid : STKM`MessageId
// callthr : SCHDTP`ThreadId
// opname : AS`Name
// objref : [SEM`OBJ_Ref]
// size : nat
// time : nat
// ==> ()
void TIMETRACE::LogMessageRequest(const Generic & busid,
                                  const Generic & fromcpu,
                                  const Generic & tocpu,
                                  const TYPE_STKM_MessageId & msgid,
                                  const TYPE_SCHDTP_ThreadId & callthr,
                                  const TYPE_AS_Name & opname,
                                  const Generic & objref_,
                                  const Int & size,
                                  const Int & time)
{
  Generic objref (ConvertObjRef(objref_));

  Int bid (ConvertBUSNameToNum(busid));
  Int fcpu (ConvertNameToNum(fromcpu));
  Int tcpu (ConvertNameToNum(tocpu));
  TYPE_AS_Ids opids (opname.get_ids());
  
  TYPE_TIMETRACETP_MessageRequest t_in;
  t_in.Init(bid, fcpu, tcpu, msgid, callthr, opids, objref, size, time);

  AddToLogfile(t_in);
}

// LogReplyRequest
// busid : [AS`Name]
// fromcpu : [AS`Name]
// tocpu : [AS`Name]
// msgid : STKM`MessageId
// origmsgid : STKM`MessageId
// callthr : SCHDTP`ThreadId
// caleethr : SCHDTP`ThreadId
// size : nat
// time : nat
// ==> ()
void TIMETRACE::LogReplyRequest(const Generic & busid,
                                const Generic & fromcpu,
                                const Generic & tocpu,
                                const TYPE_STKM_MessageId & msgid,
                                const TYPE_STKM_MessageId & origmsgid,
                                const TYPE_SCHDTP_ThreadId & callthr,
                                const TYPE_SCHDTP_ThreadId & caleethr,
                                const Int & size,
                                const Int & time)
{
  Int bid (ConvertBUSNameToNum(busid));
  Int fcpu (ConvertNameToNum(fromcpu));
  Int tcpu (ConvertNameToNum(tocpu));
  
  TYPE_TIMETRACETP_ReplyRequest t_in;
  t_in.Init(bid,fcpu,tcpu,msgid, origmsgid,caleethr,callthr,size,time);

  AddToLogfile(t_in);
}
      
// LogMessageActivate
// id : STKM`MessageId
// time : nat
// ==> ()
void TIMETRACE::LogMessageActivate(const TYPE_STKM_MessageId & id,
                                   const Int & time)
{
  TYPE_TIMETRACETP_MessageActivate t_in;
  t_in.Init(id,time);

  AddToLogfile(t_in);
}

// LogMessageCompleted
// id : STKM`MessageId
// time : nat
// ==> ()
void TIMETRACE::LogMessageCompleted(const TYPE_STKM_MessageId & id,
                                    const Int & time)
{
  TYPE_TIMETRACETP_MessageCompleted t_in;
  t_in.Init(id,time);

  AddToLogfile(t_in);
}

// LogObjAlloc
// objref : SEM`OBJ_Ref
// clnm : AS`Name
// cpunm : [AS`Name]
// time : nat
// ==> ()
void TIMETRACE::LogObjAlloc(const TYPE_SEM_OBJ_uRef & objref_,
                            const Generic & cpunm,
                            const Int & time)
{
  const TYPE_AS_Name & clnm(objref_.GetRecord(pos_SEM_OBJ_uRef_tp));
  Generic objref (ConvertObjRef(objref_));

  Int clid (ConvertNameToNum(cpunm));

  TYPE_TIMETRACETP_DeployObj t_in;
  t_in.Init(objref, clnm, clid, time);

  AddToLogfile(t_in);
} 

// LogBroadMessageReq
// busid : [AS`Name]
// fromcpu : [AS`Name]
// tocpus : set of [AS`Name]
// msgid : STKM`MessageId
// opname : AS`Name
// objrefs : set of SEM`OBJ_Ref
// size : nat
// time : nat
// ==> ()
void TIMETRACE::LogBroadMessageReq(const Generic & busid,
                                   const Generic & fromcpu,
                                   const Set & tocpus,
                                   const TYPE_STKM_MessageId & msgid,
                                   const TYPE_AS_Name & opname,
                                   const SET<TYPE_SEM_OBJ_uRef> & objrefs_,
                                   const Int & size,
                                   const Int & time)
{
  SET<TYPE_SEM_OBJ_uRef> objrefs (ConvertObjRef(objrefs_));

  Int bid (ConvertBUSNameToNum(busid));
  Int fcpu (ConvertNameToNum(fromcpu));
  SET<Int> tcpus;
  Set tocpus_q (tocpus);
  Generic cpu;
  for (bool bb = tocpus_q.First(cpu); bb; bb = tocpus_q.Next(cpu))
  {
    tcpus.Insert(ConvertNameToNum(cpu));
  }
  TYPE_AS_Ids opids (opname.get_ids());

  TYPE_TIMETRACETP_MessageBroadReq t_in;
  t_in.Init(bid, fcpu, tcpus, msgid, opids, objrefs, size, time);
  AddToLogfile(t_in);
}
  
// AddArgLog
// arg : <ALL> | AS`Name
// ==> ()
void TIMETRACE::AddArgLog(const Generic & arg)
{
  if( arg.IsQuote() || TIMETRACE::logargs.IsQuote() )
    TIMETRACE::logargs = Quote(L"ALL");
  else
  {
    Set log_s (TIMETRACE::logargs);
    log_s.ImpUnion(Set(arg));
    TIMETRACE::logargs = log_s;
  }
}

Generic TIMETRACE::GetArgLog()
{
  return TIMETRACE::logargs;
}

void TIMETRACE::ResetArgLog()
{
  TIMETRACE::logargs = Set();
}

// ConvertNameToNum
// nm : [AS`Name]
// ==> nat
Int TIMETRACE::ConvertNameToNum(const Generic & nm)
{
  if (!TIMETRACE::id_m.DomExists(nm))
  {
    TIMETRACE::id_m.Insert(nm, TIMETRACE::nextid);
    TIMETRACE::nextid = TIMETRACE::nextid + Int(1);
  }
  return TIMETRACE::id_m[nm];
}

// ConvertBUSNameToNum
// nm : [AS`Name]
// ==> nat
Int TIMETRACE::ConvertBUSNameToNum(const Generic & nm)
{
  if (nm.IsNil()) return Int(-1);
  if (!TIMETRACE::busid_m.DomExists(nm))
  {
    TIMETRACE::busid_m.Insert(nm, TIMETRACE::nextbusid);
    TIMETRACE::nextbusid = Int(TIMETRACE::nextbusid.GetValue() + 1);
  }
  return TIMETRACE::busid_m[nm];
}

// NewTimeTraceInit
// ==> ()
void TIMETRACE::NewTimeTraceInit()
{
  TIMETRACE::new_log_id += 1;
  TIMETRACE::new_log = true;
  TIMETRACE::log.Clear();
  TIMETRACE::logfile = wstring(L"logfile") + Int2Char(Int(TIMETRACE::new_log_id));
}

// TraceEvent2String
// event : TIMETRACETP`TraceEvent
// logargs : (<ALL> | set of AS`Name)
// -> seq of char
wstring TIMETRACE::TraceEvent2String(const TYPE_TIMETRACETP_TraceEvent & event,
                                     const Generic & logargs)
{
  switch(event.GetTag()){
  case TAG_TYPE_TIMETRACETP_ThreadSwapIn: {
    return ThreadSwapIn2String(event);
    break;
  }
  case TAG_TYPE_TIMETRACETP_ThreadSwapOut: {
    return ThreadSwapOut2String(event);
    break;
  }
  case TAG_TYPE_TIMETRACETP_DelayedThreadSwapIn: {
    return DelayedThreadSwapIn2String(event);
    break;
  }
  case TAG_TYPE_TIMETRACETP_ThreadCreate: {
    return ThreadCreate2String(event);
    break;
  }
  case TAG_TYPE_TIMETRACETP_ThreadKill: {
    return ThreadKill2String(event);
    break;
  }
  case TAG_TYPE_TIMETRACETP_OpRequest: {
    return OpRequest2String(event, logargs);
    break;
  }
  case TAG_TYPE_TIMETRACETP_OpActivate: {
    return OpActivate2String(event);
    break;
  }
  case TAG_TYPE_TIMETRACETP_OpCompleted: {
    return OpCompleted2String(event, logargs);
    break;
  }
  case TAG_TYPE_TIMETRACETP_MessageRequest: {
    return MessageRequest2String(event);
    break;
  }
  case TAG_TYPE_TIMETRACETP_ReplyRequest: {
    return ReplyRequest2String(event);
    break;
  }
  case TAG_TYPE_TIMETRACETP_MessageActivate: {
    return MessageActivate2String(event);
    break;
  }
  case TAG_TYPE_TIMETRACETP_MessageCompleted: {
    return MessageCompleted2String(event);
    break;
  }
  case TAG_TYPE_TIMETRACETP_MessageBroadReq: {
    return MessageBroadReq2String(event);
    break;
  }
  case TAG_TYPE_TIMETRACETP_CPUdecl: {
    return CPUdecl2String(event);
    break;
  }
  case TAG_TYPE_TIMETRACETP_BUSdecl: {
    return BUSdecl2String(event);
    break;
  }
  case TAG_TYPE_TIMETRACETP_DeployObj: {
    return DeployObj2String(event);
    break;
  }
  case TAG_TYPE_TIMETRACETP_InstVarChange: {
    return InstVarChange2String(event);
    break;
  }
  default: {
  }
    RTERR::Error(L"TIMETRACE::TraceEvent2String", RTERR_UNEXPECTED_TRACE_EVENT, Nil(), Nil(), Sequence());
    return L"";
  }
}

// LogOpName
// opname : AS`Ids
// logargs : (<ALL> | set of AS`Name)
// -> bool
bool TIMETRACE::LogOpName(const TYPE_AS_Ids & opname, const Generic & logargs)
{
  if( logargs.IsQuote() ) return true;
  return Set(logargs).InSet(ASTAUX::MkNameFromIds(opname, NilContextId));
}

// Num2String
// r : real
// -> seq of char
wstring TIMETRACE::Num2String(const Real & r )
{
//  return Int2String(Int((int)floor(r.GetValue())));
  return Int2String(Int(r.GetIntValue()));
}

// Int2String
// i : int
// -> seq of char
wstring TIMETRACE::Int2String(const Int & i)
{
  return i.ascii();
}

// Clnm2String
// ids : [AS`Ids]
// -> seq of char
wstring TIMETRACE::Clnm2String(const Generic & ids)
{
  if( ids.IsNil() )
    return L"nil";
  else
    return Ids2String((const TYPE_AS_Ids &)ids);
}

// Ids2String
// ids : AS`Ids
// -> seq of char
wstring TIMETRACE::Ids2String(const TYPE_AS_Ids & ids_)
{
  wstring res = L"\"";
  TYPE_AS_Ids ids (ids_);
  while (!ids.IsEmpty())
  {
    res += ids[1].GetString();
    ids.ImpTl();
    if (!ids.IsEmpty())
      res += L"`";
  }
  res += L"\"";
  return res;
}

// Int2Char
// i : int
// -> seq of char
wstring TIMETRACE::Int2Char(const Int & i)
{
  int num = i.GetValue();
  if( num >= 0 || num <= 9 )
    return i.ascii();
  else
    return L"";
}

// Bool2Char
// b : bool
// -> seq of char
wstring TIMETRACE::Bool2Char(const Bool & b )
{
  return (b.GetValue() ? L"true" : L"false");
}

// Objref2String
// objref : [SEM`OBJ_Ref]
// wstring -> seq of char
wstring TIMETRACE::Objref2String(const Generic & objref)
{
  if(objref.IsNil())
    return L"nil";
  
  return Int2String(((const TYPE_SEM_OBJ_uRef &)objref).get_ref());
}

// ObjrefSet2String
// objrefs : set of SEM`OBJ_Ref
// -> seq of char
wstring TIMETRACE::ObjrefSet2String(const SET<TYPE_SEM_OBJ_uRef> & objrefs)
{
  SET<Int> oids;
  SET<TYPE_SEM_OBJ_uRef> objrefs_q (objrefs);
  Generic ref;
  for(bool bb = objrefs_q.First(ref); bb; bb = objrefs_q.Next(ref))
  {
    oids.Insert(((const TYPE_SEM_OBJ_uRef &)ref).get_ref());
  }
  return NatSet2String(oids);
}

// NatSet2String
// ns : set of nat
// -> seq of char
wstring TIMETRACE::NatSet2String(const SET<Int> & ns)
{
  if( ns.IsEmpty() ) return L"";
  Set tmp (ns);
  wstring ret;
  while( !tmp.IsEmpty() )
  {
    Int i (tmp.GetElem());
    ret += Int2Char(i);
    tmp.RemElem(i);
    if( !tmp.IsEmpty() )
      ret += L", "; 
  }
  return ret;
}

// TimeTraceInit
// -> TraceState
void TIMETRACE::TimeTraceInit()
{
  TIMETRACE::new_log = true;
  TIMETRACE::new_log_id = Int(0);
  TIMETRACE::log.Clear();
  TIMETRACE::logfile = L"logfile";
  TIMETRACE::id_m.Clear();
  TIMETRACE::busid_m.Clear();
  TIMETRACE::nextid = Int(0);
  TIMETRACE::nextbusid = Int(0);
  TIMETRACE::logargs = Set();
  TIMETRACE::threadcreate.Clear();
}

// AddToLogfile
// te : TIMETRACETP`TraceEvent
// ==> ()
void TIMETRACE::AddToLogfile(const TYPE_TIMETRACETP_TraceEvent & te)
{
  if( !TIMETRACE::logging ) return;
//wcout << TraceEvent2String(te, TIMETRACE::logargs) << endl;
//wcout << te << endl;
//  TIMETRACE::log.ImpAppend(te);
  if ( TIMETRACE::new_log )
  {
    if( TIMETRACE::tracelogstream.is_open() )
      TIMETRACE::tracelogstream.close();
    TIMETRACE::new_log = false;
  }

  if( !TIMETRACE::tracelogstream.is_open() )
  {
    TIMETRACE::tracelogstream.open(TBWSTR::wstring2fsstr(TIMETRACE::logfile).c_str(), ios::out);
  }

  if( TIMETRACE::tracelogstream )
  {
    wstring logstr (TraceEvent2String(te, TIMETRACE::logargs));
    TIMETRACE::tracelogstream << TBWSTR::wstring2utf8str(logstr) << endl << flush;
  }
  else
    RTERR::Error(L"TIMETRACE::AddToLogfile", RTERR_UNABLE_OPEN_LOGFILE, Nil(), Nil(), Sequence());
}

// FlushLogfile
void TIMETRACE::FlushLogfile()
{
  if( TIMETRACE::tracelogstream.is_open() )
    TIMETRACE::tracelogstream.flush();
}

wstring TIMETRACE::ThreadSwapIn2String(const TYPE_TIMETRACETP_ThreadSwapIn & e)
{
  wstring ret (L"ThreadSwapIn -> ");
  ret += L"id: " + Num2String(e.get_id());
  ret += L" objref: " + Objref2String(e.get_objref());
  ret += L" clnm: " + Clnm2String(e.get_clnm());
  ret += L" cpunm: " +  Num2String(e.get_cpunm());
  ret += L" overhead: " + Num2String(e.get_overhead());
  ret += L" time: " + Num2String(e.get_time());

  if (!CheckTime(e.get_cpunm(), e.get_time()))
    ret += L" ERROR";

  return ret;
}

wstring TIMETRACE::ThreadSwapOut2String(const TYPE_TIMETRACETP_ThreadSwapOut & e)
{
  wstring ret (L"ThreadSwapOut -> ");
  ret += L"id: " + Num2String(e.get_id());
  ret += L" objref: " + Objref2String(e.get_objref());
  ret += L" clnm: " + Clnm2String(e.get_clnm());
  ret += L" cpunm: " +  Num2String(e.get_cpunm());
  ret += L" overhead: " + Num2String(e.get_overhead());
  ret += L" time: " + Num2String(e.get_time());

  if (!CheckTime(e.get_cpunm(), e.get_time()))
    ret += L" ERROR";

  return ret;
}

wstring TIMETRACE::DelayedThreadSwapIn2String(const TYPE_TIMETRACETP_DelayedThreadSwapIn & e)
{
  wstring ret (L"DelayedThreadSwapIn -> ");
  ret += L"id: " + Num2String(e.get_id());
  ret += L" objref: " + Objref2String(e.get_objref());
  ret += L" clnm: " + Clnm2String(e.get_clnm());
  ret += L" delay: " + Num2String(e.get_delay());
  ret += L" cpunm: " +  Num2String(e.get_cpunm());
  ret += L" overhead: " + Num2String(e.get_overhead());
  ret += L" time: " + Num2String(e.get_time());

  if (!CheckTime(e.get_cpunm(), e.get_time()))
    ret += L" ERROR";

  return ret;
}

wstring TIMETRACE::ThreadCreate2String(const TYPE_TIMETRACETP_ThreadCreate & e)
{
  Generic objref (e.get_objref());
  wstring obj (objref.IsNil() ? L"nil" : Objref2String(objref));
  Generic clnm (e.get_clnm());
  wstring cl (clnm.IsNil() ? L"nil" : 
                             Clnm2String(((TYPE_AS_Name) clnm).get_ids()));

  wstring ret (L"ThreadCreate -> ");
  ret += L"id: " + Num2String(e.get_id());
  ret += L" period: " + Bool2Char(e.get_period());
  ret += L" objref: " + obj;
  ret += L" clnm: " + cl;
  ret += L" cpunm: " +  Num2String(e.get_cpunm());
  ret += L" time: " + Num2String(e.get_time());

  if (!CheckTime(e.get_cpunm(), e.get_time()))
    ret += L" ERROR";

  return ret;
}

wstring TIMETRACE::ThreadKill2String(const TYPE_TIMETRACETP_ThreadKill & e)
{
  wstring ret (L"ThreadKill -> ");
  ret += L"id: " + Num2String(e.get_id());
  ret += L" cpunm: " +  Num2String(e.get_cpunm());
  ret += L" time: " + Num2String(e.get_time());

  if (!CheckTime(e.get_cpunm(), e.get_time()))
    ret += L" ERROR";

  return ret;
}
wstring TIMETRACE::OpRequest2String(const TYPE_TIMETRACETP_OpRequest & e,
                                    const Generic & logargs)
{
  wstring ret (L"OpRequest -> ");
  ret += L"id: " + Num2String(e.get_id());
  ret += L" opname: " + Ids2String(e.get_opname());
  ret += L" objref: " + Objref2String(e.get_objref());
  ret += L" clnm: " + Clnm2String(e.get_clnm());
  ret += L" cpunm: " + Num2String(e.get_cpunm());

  if(LogOpName(e.get_opname(),logargs))
    ret += L" args: \"[" + SemRec::ValSeq2String(e.get_args()) + L"]\"";

  ret += L" async: " + Bool2Char(e.get_async());
  ret += L" time: " + Num2String(e.get_time());

  if (!CheckTime(e.get_cpunm(), e.get_time()))
    ret += L" ERROR";

  return ret;
}

wstring TIMETRACE::OpActivate2String(const TYPE_TIMETRACETP_OpActivate & e)
{
  wstring ret (L"OpActivate -> ");
  ret += L"id: " + Num2String(e.get_id());
  ret += L" opname: " + Ids2String(e.get_opname());
  ret += L" objref: " + Objref2String(e.get_objref());
  ret += L" clnm: " + Clnm2String(e.get_clnm());
  ret += L" cpunm: " +  Num2String(e.get_cpunm());
  ret += L" async: " + Bool2Char(e.get_async());
  ret += L" time: " + Num2String(e.get_time());

  if (!CheckTime(e.get_cpunm(), e.get_time()))
    ret += L" ERROR";

  return ret;
}

wstring TIMETRACE::OpCompleted2String(const TYPE_TIMETRACETP_OpCompleted & e,
                                      const Generic & logargs)
{
  wstring ret (L"OpCompleted -> ");
  ret += L"id: " + Num2String(e.get_id());
  ret += L" opname: " + Ids2String(e.get_opname());
  ret += L" objref: " + Objref2String(e.get_objref());
  ret += L" clnm: " + Clnm2String(e.get_clnm());
  ret += L" cpunm: " +  Num2String(e.get_cpunm());

  if(LogOpName(e.get_opname(), logargs))
    ret += L" result: \"" + SemRec::ValSeq2String(e.get_res()) + L"\"";

  ret += L" async: " + Bool2Char(e.get_async());
  ret += L" time: " + Num2String(e.get_time());

  if (!CheckTime(e.get_cpunm(), e.get_time()))
    ret += L" ERROR";

  return ret;
}

wstring TIMETRACE::MessageRequest2String(const TYPE_TIMETRACETP_MessageRequest & e)
{
  wstring ret (L"MessageRequest -> ");
  ret += L"busid: " + Num2String(e.get_busid());
  ret += L" fromcpu: " + Num2String(e.get_fromcpu());
  ret += L" tocpu: " + Num2String(e.get_tocpu());
  ret += L" msgid: " + Num2String(e.get_msgid());
  ret += L" callthr: " + Num2String(e.get_callthr());
  ret += L" opname: " + Ids2String(e.get_opname());
  ret += L" objref: " + Objref2String(e.get_objref());
  ret += L" size: " + Num2String(e.get_size());
  ret += L" time: " + Num2String(e.get_time());
  return ret;
}

wstring TIMETRACE::ReplyRequest2String(const TYPE_TIMETRACETP_ReplyRequest & e)
{
  wstring ret (L"ReplyRequest -> ");
  ret += L"busid: " + Num2String(e.get_busid());
  ret += L" fromcpu: " + Num2String(e.get_fromcpu());
  ret += L" tocpu: " + Num2String(e.get_tocpu());
  ret += L" msgid: " + Num2String(e.get_msgid());
  ret += L" origmsgid: " + Num2String(e.get_origmsgid());
  ret += L" callthr: " + Num2String(e.get_callthr());
  ret += L" calleethr: " + Num2String(e.get_calleethr());
  ret += L" size: " + Num2String(e.get_size());
  ret += L" time: " + Num2String(e.get_time());
  return ret;
}
wstring TIMETRACE::MessageActivate2String(const TYPE_TIMETRACETP_MessageActivate & e)
{
  wstring ret (L"MessageActivate -> ");
  ret += L"msgid: " + Num2String(e.get_msgid());
  ret += L" time: " + Num2String(e.get_time());
  return ret;
}
wstring TIMETRACE::MessageCompleted2String(const TYPE_TIMETRACETP_MessageCompleted & e)
{
  wstring ret (L"MessageCompleted -> ");
  ret += L"msgid: " + Num2String(e.get_msgid());
  ret += L" time: " + Num2String(e.get_time());
  return ret;
}

wstring TIMETRACE::MessageBroadReq2String(const TYPE_TIMETRACETP_MessageBroadReq & e)
{
  wstring ret (L"MessageBroadReq -> ");
  ret += L"busid: " + Num2String(e.get_busid());
  ret += L" fromcpu: " + Num2String(e.get_fromcpu());
  ret += L" tocpus: " + NatSet2String(e.get_tocpu());
  ret += L" msgid: " + Num2String(e.get_msgid());
  ret += L" opname: " + Ids2String(e.get_opname());
  ret += L" objrefs: " + ObjrefSet2String(e.get_objref());
  ret += L" size: " + Num2String(e.get_size());
  ret += L" time: " + Num2String(e.get_time());
  return ret;
}

wstring TIMETRACE::CPUdecl2String(const TYPE_TIMETRACETP_CPUdecl & e)
{
  Generic sys (e.get_sys());
  wstring system (sys.IsNil() ? L"\"none\""
                              : Ids2String(TYPE_AS_Name(sys).get_ids()));
  Generic name (e.get_name());
  wstring cpunm (name.IsNil() ? L"\"vCPU\""
                              : Ids2String(TYPE_AS_Name(name).get_ids()));

  wstring ret (L"CPUdecl -> ");
  ret += L" id: " + Num2String(e.get_id());
  ret += L" expl: " + Bool2Char(e.get_expl());
  ret += L" sys: " + system;
  ret += L" name: " + cpunm;
  return ret;
}
wstring TIMETRACE::BUSdecl2String(const TYPE_TIMETRACETP_BUSdecl & e)
{
  Generic name (e.get_name());
  wstring cpunm (name.IsNil() ? L"nil"
                              : Ids2String(TYPE_AS_Name(name).get_ids()));

  wstring ret (L"BUSdecl -> ");
  ret += L" id: " + Num2String(e.get_id());
  ret += L" topo: {" + NatSet2String(e.get_topo()) + L"}";
  ret += L" name: " + cpunm;
  return ret;
}

wstring TIMETRACE::DeployObj2String(const TYPE_TIMETRACETP_DeployObj & e)
{
  wstring ret (L"DeployObj -> ");
  ret += L" objref: " + Objref2String(e.get_objref());
  ret += L" clnm: " + Ids2String(e.get_clnm().get_ids());
  ret += L" cpunm: " + Num2String(e.get_cpunm());
  ret += L" time: " + Num2String(e.get_time());

  if (!CheckTime(e.get_cpunm(), e.get_time()))
    ret += L" ERROR";

  return ret;
}

wstring TIMETRACE::InstVarChange2String(
                           const TYPE_TIMETRACETP_InstVarChange & e)
{
  wstring ret (L"InstVarChange -> ");
  ret += L" instnm: " + Ids2String(e.get_instnm().get_ids());
  ret += L" val: \"" + SemRec::Val2String(e.get_val()) + L"\"";
  ret += L" objref: " + Objref2String(e.get_objref());
  ret += L" time: " + Num2String(e.get_time());
  ret += L" id: " + Num2String(e.get_thrid());
  return ret;
}

// ConvertObjRef (not in spec)
// val : [SEM`VAL | seq of SEM`VAL | set of SEM`VAL ]
// ==> [SEM`VAL  | seq of SEM`VAL | set of SEM`VAL ]
Generic TIMETRACE::ConvertObjRef(const Generic & val_)
{
  if (val_.IsNil())
    return Nil();
  else if (val_.IsSequence())
  {
    SEQ<TYPE_SEM_VAL> s (val_);
    SEQ<TYPE_SEM_VAL> ns;
    Generic g;
    for (bool bb = s.First(g); bb; bb = s.Next(g))
      ns.ImpAppend(ConvertObjRef(g));
    return ns;
  }
  else if (val_.IsSet())
  {
    SET<TYPE_SEM_VAL> s (val_);
    SET<TYPE_SEM_VAL> ns;
    Generic g;
    for (bool bb = s.First(g); bb; bb = s.Next(g))
      ns.Insert(ConvertObjRef(g));
    return ns;
  }
  else if (val_.IsMap())
  {
    MAP<TYPE_SEM_VAL,TYPE_SEM_VAL> m (val_);
    MAP<TYPE_SEM_VAL,TYPE_SEM_VAL> nm;
    Generic g, h;
    for (bool bb = m.First(g, h); bb; bb = m.Next(g, h))
      nm.ImpModify(ConvertObjRef(g), ConvertObjRef(h));
    return mk_SEM_MAP(nm);
  }
  else if (val_.IsRecord())
  {
    Record val (val_);
    switch(val.GetTag()) {
      case TAG_TYPE_SEM_OBJ_uRef: {
        TYPE_SEM_OBJ_uRef objref (val);   
    
        TYPE_SEM_OBJ_uRef ref;
        ref.set_ref(objref.GetInt(pos_SEM_OBJ_uRef_ref));
        return ref;
      }
      case TAG_TYPE_SEM_SET: {
        SET<TYPE_SEM_VAL> s (val.GetField(pos_SEM_SET_v));
        SET<TYPE_SEM_VAL> ns;
        Generic g;
        for (bool bb = s.First(g); bb; bb = s.Next(g))
          ns.Insert(ConvertObjRef(g));
        return mk_SEM_SET(ns);
      }
      case TAG_TYPE_SEM_SEQ: {
        SEQ<TYPE_SEM_VAL> s (val.GetField(pos_SEM_SEQ_v));
        SEQ<TYPE_SEM_VAL> ns;
        Generic g;
        for (bool bb = s.First(g); bb; bb = s.Next(g))
          ns.ImpAppend(ConvertObjRef(g));
        return mk_SEM_SEQ(ns);
      }
      case TAG_TYPE_SEM_TUPLE: {
        SEQ<TYPE_SEM_VAL> s (val.GetField(pos_SEM_TUPLE_v));
        SEQ<TYPE_SEM_VAL> ns;
        Generic g;
        for (bool bb = s.First(g); bb; bb = s.Next(g))
          ns.ImpAppend(ConvertObjRef(g));
        return mk_SEM_TUPLE(ns);
      }
      case TAG_TYPE_SEM_MAP: {
        MAP<TYPE_SEM_VAL,TYPE_SEM_VAL> m (val.GetField(pos_SEM_MAP_v));
        MAP<TYPE_SEM_VAL,TYPE_SEM_VAL> nm;
        Generic g, h;
        for (bool bb = m.First(g, h); bb; bb = m.Next(g, h))
          nm.ImpModify(ConvertObjRef(g), ConvertObjRef(h));
        return mk_SEM_MAP(nm);
      }
      // NOTE: SEM`REC implimentation is different from spec.
      case TAG_TYPE_DYNSEM_SEM_REC: {
        TYPE_DYNSEM_SEM_SemRecord srec (val);
        Record value (srec.get_value());
        SEQ<TYPE_SEM_VAL> fields_v (value.GetFields());
        SEQ<TYPE_SEM_VAL> nfields_v;
        Generic g;
        for ( bool bb = fields_v.First( g ); bb; bb = fields_v.Next( g ) )
        {
          nfields_v.ImpAppend(ConvertObjRef(g));
        }
        value.SetFields(nfields_v);
        TYPE_DYNSEM_SEM_SemRecord nsrec (val);
        nsrec.set_value(value);
        return nsrec;
      }
      default:
        return val;
    }
  }
  else
    return val_;
}

Int TIMETRACE::GetTime()
{
  return Int(theSystem().GetTime());
}

Int TIMETRACE::GetTime(const Generic & cpu)
{
  return Int(theSystem().GetTimeOfCPU(cpu));
}

Generic TIMETRACE::GetCurCPU()
{
  return theSystem().GetCurCPU();
}

bool TIMETRACE::CheckTime(const Int & cpunm, const Int & time)
{
/*
  if (lasttime.DomExists(cpunm))
  {
    int ltime = Int(lasttime[cpunm]).GetValue();
    if (ltime > time.GetValue())
    {
//wcout << L"time error cpu " << cpunm << L" " << ltime << L" " << time << endl;
//wcout << lasttime << endl;
      lasttime.ImpModify(cpunm, time);
      return false;
    }
  }
  lasttime.ImpModify(cpunm, time);
*/
  return true;
}
