/***
*  * WHAT
*  *    Implementation of module TIMETRACE (VICE)
*  * COPYRIGHT
*  *    (C) Kyushu University
*  * SPECIFICATION VERSION
*  *    implementation of timetrace.vdm 1.5
*  *
***/

#ifndef __TIMETRACE_H__
#define __TIMETRACE_H__

#include "metaiv.h"
#include "AS.h"
#include "stackeval.h"
#include "TIMETRACETP.h"
#include <fstream>

class TIMETRACE
{
private:
  static bool new_log;       // bool
  static int new_log_id;     // nat
  static Sequence log;       // TIMETRACETP`Trace
  static wstring logfile;    // seq of char
  static Map id_m;           // map [AS`Name] to nat
  static Map busid_m;        // map [AS`Name] to nat
  static Int nextid;         // nat
  static Int nextbusid;      // nat
  static Generic logargs;    // <ALL> | set of AS`Name
  static MAP<TYPE_SCHDTP_ThreadId, Int> threadcreate;
                             // map SCHDTP`ThreadId to nat

  static bool logging;

  static ofstream tracelogstream;

  // 20081023
  static Map lasttime;

public:
  static void SetNewLog(bool b) { new_log = b; }; 
  static void LoggingOn();
  static void LoggingOff();
  static void SetLogFile(const wstring &newlogfile);
  static void AddCPU(const Generic & sysnm,
                     const Generic & cpunm,
                     const Bool & expl);
  static void AddBUS(const Generic & busnm,
                     const Set & cpus);
  static void AddToLogfile(const TYPE_TIMETRACETP_TraceEvent & te);
  static void LogHistEvent(const TYPE_SCHDTP_ThreadId & threadid,
                           const TYPE_AS_Name & clnm,
                           const TYPE_AS_Name & opname,
                           const TYPE_INSTRTP_HistoryKind & kind,
                           const TYPE_SEM_OBJ_uRef & objref,
                           const Bool & async,
//                           const Generic & vals);
                           const Sequence & vals);
  static void LogDelayedThreadSwapIn(const TYPE_SCHDTP_ThreadId & threadid,
                                     const Generic & objRef,
                                     const Generic & clnm,
                                     const Int & delay,
                                     const Int & overhead);
  static void LogThreadSwapIn(const TYPE_SCHDTP_ThreadId & threadid,
                              const Generic & objRef,
                              const Generic & clnm,
                              const Int & overhead);
  static void LogThreadSwapOut(const TYPE_SCHDTP_ThreadId & threadid,
                               const Generic & objRef,
                               const Generic & clnm,
                               const Int & overhead);
  static void LogThreadCreate(const TYPE_SCHDTP_ThreadId & tid,
                              const Bool & peri,
                              const Generic & objref,
                              const Generic & clsnm,
                              const Generic & cpunm);
  static void LogKillThread(const TYPE_SCHDTP_ThreadId & tid,
                            const Generic & cpunm);
  static void LogMessageRequest(const Generic & busid,
                                const Generic & fromcpu,
                                const Generic & tocpu,
                                const TYPE_STKM_MessageId & msgid,
                                const TYPE_SCHDTP_ThreadId & callthr,
                                const TYPE_AS_Name & opname,
                                const Generic & objref,
                                const Int & size,
                                const Int & time);
  static void LogBroadMessageReq(const Generic & busid,
                                 const Generic & fromcpu,
                                 const Set & tocpus,
                                 const TYPE_STKM_MessageId & msgid,
                                 const TYPE_AS_Name & opname,
                                 const SET<TYPE_SEM_OBJ_uRef> & objrefs,
                                 const Int & size,
                                 const Int & time);
  static void LogReplyRequest(const Generic & busid,
                              const Generic & fromcpu,
                              const Generic & tocpu,
                              const TYPE_STKM_MessageId & msgid,
                              const TYPE_STKM_MessageId & origmsgid,
                              const TYPE_SCHDTP_ThreadId & callthr,
                              const TYPE_SCHDTP_ThreadId & caleethr,
                              const Int & size,
                              const Int & time);
  static void LogMessageActivate(const TYPE_STKM_MessageId & id, const Int & time);
  static void LogMessageCompleted(const TYPE_STKM_MessageId & id, const Int & time);
  static void LogObjAlloc(const TYPE_SEM_OBJ_uRef & objref, const Generic & cpunm, const Int & time);
  static void NewTimeTraceInit();
  static void TimeTraceInit();
  static void FlushLogfile();
  static void AddArgLog(const Generic & arg);
  static Generic GetArgLog();
  static void ResetArgLog();
  static wstring Ids2String(const TYPE_AS_Ids & ids);

  static Generic ConvertObjRef(const Generic & val);
private:
  static Int GetTime();
  static Int GetTime(const Generic &);
  static Generic GetCurCPU();
  static Int ConvertNameToNum(const Generic & nm);
  static Int ConvertBUSNameToNum(const Generic & nm);
  static wstring TraceEvent2String(const TYPE_TIMETRACETP_TraceEvent & event, const Generic & logargs);
  static bool LogOpName(const TYPE_AS_Ids & opname, const Generic & logargs);
  static wstring Num2String(const Real & r );
  static wstring Int2String(const Int & i);
  static wstring Clnm2String(const Generic & ids);
  static wstring Int2Char(const Int & i);
  static wstring Bool2Char(const Bool & b );
  static wstring Objref2String(const Generic & objref);
  static wstring NatSet2String(const SET<Int> & ns);
  static wstring ObjrefSet2String(const SET<TYPE_SEM_OBJ_uRef> & objrefs);

  static wstring ThreadSwapIn2String(const TYPE_TIMETRACETP_ThreadSwapIn & e);
  static wstring ThreadSwapOut2String(const TYPE_TIMETRACETP_ThreadSwapOut & e);
  static wstring DelayedThreadSwapIn2String(const TYPE_TIMETRACETP_DelayedThreadSwapIn & e);
  static wstring ThreadCreate2String(const TYPE_TIMETRACETP_ThreadCreate & e);
  static wstring ThreadKill2String(const TYPE_TIMETRACETP_ThreadKill & e);
  static wstring OpRequest2String(const TYPE_TIMETRACETP_OpRequest & e,
                           const Generic & logargs);
  static wstring OpActivate2String(const TYPE_TIMETRACETP_OpActivate & e);
  static wstring OpCompleted2String(const TYPE_TIMETRACETP_OpCompleted & e,
                             const Generic & logargs);
  static wstring MessageRequest2String(const TYPE_TIMETRACETP_MessageRequest & e);
  static wstring ReplyRequest2String(const TYPE_TIMETRACETP_ReplyRequest & e);
  static wstring MessageActivate2String(const TYPE_TIMETRACETP_MessageActivate & e);
  static wstring MessageCompleted2String(const TYPE_TIMETRACETP_MessageCompleted & e);
  static wstring MessageBroadReq2String(const TYPE_TIMETRACETP_MessageBroadReq & e);
  static wstring CPUdecl2String(const TYPE_TIMETRACETP_CPUdecl & e);
  static wstring BUSdecl2String(const TYPE_TIMETRACETP_BUSdecl & e);
  static wstring DeployObj2String(const TYPE_TIMETRACETP_DeployObj & e);
  static wstring InstVarChange2String(const TYPE_TIMETRACETP_InstVarChange & e);

  static bool CheckTime(const Int & cpunm, const Int & time);
};

inline
void vdm_TIMETRACE_AddToLogfile(const TYPE_TIMETRACETP_TraceEvent & te)
{
  TIMETRACE::AddToLogfile(te);
}

#endif // __TIMETRACE_H__

