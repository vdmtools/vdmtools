/***
*  * WHAT
*  *    Scheduler
*  * FILE
*  *    $Source:$
*  * PROJECT
*  *    VDM++ Toolbox
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __SCHD_H__
#define __SCHD_H__

#include "metaiv.h"
#include "AS.h"
#include "SCHDTP.h"
#include <iostream>

#ifdef VICE
#define TYPE_STKM_EvaluationState Record
#endif // VICE

class EvaluatorStatus;
typedef VDMContainer<EvaluatorStatus> EvaluatorStatusCt;

class SCHD 
{
private:
  Map threads; // SCHDTP`Threads  // Threads = map ThreadId to ThreadInfo;
  Map curthread_m; //map [AS`Name] to [SCHDTP`ThreadId]
#ifdef VICE
  Map perthreads; // map [AS`Name] to (map SEM`OBJ_Ref to (SCHDTP`PerThreadInfo | SCHDTP`SpoThreadInfo))
  Map cpualloc;   // map SCHDTP`ThreadId to [AS`Name]
  Set cpusblocked; // set of [AS`Name]
  Set potentialperchanged; // set of SEM`OBJ_Ref
  Map lastswapedin; // map [AS`Name] to [SCHDTP`ThreadId]
  Map lastswapedout; // map [AS`Name] to [SCHDTP`ThreadId]
  Map tobekilled; // map [AS`Name] to [SCHDTP`ThreadId]
  Map killed; // map [AS`Name] to bool
  Generic delayed; // [nat]
#endif // VICE
  TYPE_SCHDTP_SecondarySchedulerAlgorithm secondaryAlgorithm;
                           // SCHDTP`SecondarySchedulerAlgorithm
  Map classPriorityMap; // map AS`Id to nat
  Int maxPriority; //  nat
  TYPE_SCHDTP_ThreadId next_thread_id; // SCHDTP`ThreadId
  Bool checkingGuard; // bool
  Int x;

public:
  static const Int Default_priority;

public:
  SCHD();
  ~SCHD();

  void Dump(wostream & wos);

//
private:
  void ClearScheduler();

public:
  void InitScheduler(bool ast_is_new, bool ast_is_empty);
  Tuple EvalScheduler();
  Tuple DefaultEvalRes();

private:
  void StartDebugThread();
  bool RunGuard(const TYPE_STKM_SubProgram&, const Generic & objref);
  Tuple SelAndRunThread(const Generic &, bool);
#ifdef VICE
  Tuple RunNextPeriodicThread(const Generic & cpu);
#endif // VICE
  Tuple GetObjRefAndClass(const TYPE_SCHDTP_ThreadId & threadid);
#ifdef VICE
  int TimePerThread(const Record & thrinfo);
public:
  void AddCPU(const TYPE_AS_Name & cpunm);

private:
  int EvalValue(const TYPE_AS_Expr & expr, const TYPE_SEM_OBJ_uRef & objref);
  void SetPerThread(const TYPE_SEM_OBJ_uRef & objref, const TYPE_STKM_PerThread & t, const Int & prio);
  void SetSpoThread(const TYPE_SEM_OBJ_uRef & objref, const TYPE_STKM_SpoThread & t, const Int & prio);
  bool IsRunningThread(const TYPE_SCHDTP_ThreadId & threadid);
  bool IsPeriodicThread(const TYPE_SCHDTP_ThreadId & threadid);
  Generic ThreadObject(const TYPE_SCHDTP_ThreadId & threadid);
#endif // VICE
  Generic GetObjRef(const TYPE_SCHDTP_ThreadId & threadid);
  Generic FindNextThread();
  Sequence SortTheThreads ();
  Sequence Reverse(const Sequence & list);
#ifdef VICE
  int NextMax(int peri, int jit, int offset, int i, int curtime, int dist);
  int NextMin(int peri, int jit, int curtime, int offset, int dist, int i);
#endif // VICE
#ifdef VICE
  SEQ<TYPE_SCHDTP_ThreadId> SortSelectedThreads (const SET<TYPE_SCHDTP_ThreadId> & selected,
                                                 const Set & objrefs,
                                                 const Generic & cpu,
                                                 int time,
                                                 const Generic & prio);
#else
  SEQ<TYPE_SCHDTP_ThreadId> SortSelectedThreads(const SET<TYPE_SCHDTP_ThreadId> &);
#endif // VICE
#ifdef VICE

public:
  bool PeriodicReady( const Generic & cpunm, const Set & objrefs);

private:
#endif // VICE

public:
  void StartNewThread(const TYPE_SEM_OBJ_uRef &);
  void StopThread(const TYPE_SEM_OBJ_uRef &);

private:
  void SetThreadPriority(const TYPE_SCHDTP_ThreadId & threadid, const TYPE_AS_Name & clnm);
#ifdef VICE

public:
  Int GetThreadPriority() const;

private:
#endif // VICE
  void SetMaxPriority(const TYPE_SCHDTP_ThreadId & threadid);
  void KillThread(const TYPE_SCHDTP_ThreadId & threadid);
  void KillThreadById(const TYPE_SCHDTP_ThreadId & threadid);

public:
#ifdef VICE
  Generic CurThreadId() const;
#else
  TYPE_SCHDTP_ThreadId CurThreadId() const;
#endif // VICE
  void SelThread(const TYPE_SCHDTP_ThreadId &);
  bool Deschedule(const EvaluatorStatus &) const;
  void IncInstrnum(EvaluatorStatus &) const;
  bool EndOfSliceReached(const EvaluatorStatus &) const;
  EvaluatorStatusCt InitSlice(const EvaluatorStatusCt &) const;
  bool CheckingGuard() const;

private:
  void SetCheckingGuard(bool);
  void RestoreSTKM(const TYPE_SCHDTP_ThreadId & threadid);
#ifdef VICE

public:
  void SwapOut(const Generic & tid, const Generic & cpu, bool kill, bool tobelogged);
private:
  void RestoreInstrAndSTKM(const TYPE_SCHDTP_ThreadId & threadid);
public:
  void UpdateThreadWithReturnResult(const TYPE_SEM_VAL & val, const TYPE_SCHDTP_ThreadId & fromthread);
  void AddValToEvalStack(const TYPE_SCHDTP_ThreadId & threadid, const TYPE_SEM_VAL & val);
#endif // VICE

public:
  bool IsDebugThread(const TYPE_SCHDTP_ThreadId & threadid);
#ifdef VICE
  TYPE_SCHDTP_ThreadId AddNewThreadId(const Bool & debug_thread,
                                      const Generic & objref,
                                      const TYPE_STKM_SubProgram &instr,
                                      const Int & prio,
                                      const Bool & peri,
                                      const Generic & mesid);
  Generic GetCPUOfThread(const TYPE_SCHDTP_ThreadId & threadid);
#else
  TYPE_SCHDTP_ThreadId AddNewThreadId(const Bool & debug_thread,
                                      const Generic & objref,
                                      const TYPE_STKM_SubProgram&);
#endif // VICE
  void SetThreadStatus(const TYPE_SCHDTP_ThreadId & threadid, const TYPE_SCHDTP_ThreadStatus & tstatus);
  TYPE_SCHDTP_ThreadStatus GetThreadStatus(const TYPE_SCHDTP_ThreadId & threadid);
  void SetCurThreadState(const EvaluatorStatusCt & state);
private:
  void SetThreadState(const TYPE_SCHDTP_ThreadId & threadid);
  TYPE_SCHDTP_ThreadId GetNextThreadId();
  void SetCurThread(const TYPE_SCHDTP_ThreadId & id, bool log);
//#ifdef VICE
//  void SaveCurThreadState(const TYPE_STKM_EvaluationState &);

public:
  void SaveCurThreadState();

private:
#ifdef VICE
  Generic GetLastSwapedIn(const Generic &) const;
  void SetLastSwapedIn(const Generic &, const Generic &);

public:
  bool ASyncOpThread(const TYPE_SCHDTP_ThreadId &) const;
  SET<TYPE_AS_Name> BlockedObjRef(const TYPE_SEM_OBJ_uRef &);
  void PotentialPerChange(const TYPE_SEM_OBJ_uRef &);

private:
  Int ObjRefPriority(const TYPE_SEM_OBJ_uRef &) const;
  bool ThreadBlockedOnObjRefExists(const TYPE_SEM_OBJ_uRef &,
              const MAP<TYPE_SCHDTP_ThreadId,TYPE_SCHDTP_ThreadInfo> &) const;

public:
  Tuple RunMessageThread(const Generic & clnm,
                         const Generic & objref,
                         const TYPE_STKM_SubProgram & instr,
                         const Generic & prio,
                         const TYPE_STKM_MessageId & mesid);
  
  Tuple RunThread(const TYPE_SCHDTP_ThreadId & tid);
#endif // VICE

public:
  bool WellFormedPriorityFile (const SEQ<TYPE_SCHDTP_PriorityEntry> & pf);
  MAP<TYPE_AS_Id,Int> MakePriorityMap(const SEQ<TYPE_SCHDTP_PriorityEntry> & pf);
  MAP<TYPE_AS_Id,Int> GetPriorityMap();
  void SetPriorityMap(const MAP<TYPE_AS_Id,Int> & pm);

  MAP<TYPE_SCHDTP_ThreadId,TYPE_SCHDTP_ThreadInfo> GiveAllThreads() const;

private:
  void ProcessThreadRemoveQueue();

public:
#ifdef VICE
  bool IsSyncOpThread();
#endif // VICE
  void ShowThreadInfo(const TYPE_SCHDTP_ThreadId & tid);
};

#endif // __SCHD_H__
