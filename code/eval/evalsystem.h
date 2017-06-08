#ifndef __evalsystem_h__
#define __evalsystem_h__

#include "metaiv.h"
#include "AS.h"
#include "SCHDTP.h"
#include "stackeval.h"

class SYSTEM
{
private:
  Map cpustate;     // map [AS`Name] to CPUSigma
#ifdef VICE
  Map syscpus;      // map AS`Name to set of AS`Name
  Map taskm;        // map AS`Name to SEM`OBJ_Ref
  Map busstate;     // map AS`Name to BUSSigma
  TYPE_STKM_MessageId nextmesid;    // nat
  Set vircpus;      // set of [AS`Name]
  Map allocated;    // map SEM`OBJ_Ref to [AS`Name]
  Map delayed;      // map SEM`OBJ_Ref to [AS`Name]
#endif // VICE
  Generic curcpu;   // [AS`Name]

public:
  SYSTEM();
  ~SYSTEM();
  void Init_Sigma(bool ast_is_new);
  SYSTEM & GetInitSigma(bool ast_is_new);

  void SaveCurrentState(const EvaluatorStatusCt & es);
  EvaluatorStatusCt GetCurrentState() const;
  void SaveCurrentProgram(const TYPE_STKM_SubProgram & code);
  TYPE_STKM_SubProgram GetCurrentProgram() const;
  void Dump(wostream & wos);

#ifdef VICE
  wstring PrintCurThread();
  static wstring PrintThread(const Generic & tid, const Generic & cpu);
  static wstring PrintCPU(const Generic & cpu);
#endif // VICE

public:
  TYPE_STKM_CPUSigma EmptyCPU(const Generic &);
  void SetCurThread(const TYPE_SCHDTP_ThreadId &);
  void DeleteThread(const TYPE_SCHDTP_ThreadId &);
  Generic GetCurThread() const; // for debug

#ifdef VDMPP
  void SetGuard(const Generic &, const Generic &);
  void ResetGuard();
  bool HasGuard() const;
  void RemoveAllGuards();
  void ResetSlice();
#endif // VDMPP

  void SetRunTimeError();
  bool RunTimeError() const;

#ifdef VICE
  TYPE_STKM_CPUSigma RemoveOldEval(const TYPE_STKM_CPUSigma &);
  TYPE_STKM_BUSSigma RemoveMesAndLog(const TYPE_STKM_BUSSigma &);
  void AddThreadIdtoCPU (const TYPE_SCHDTP_ThreadId & threadid, const Generic & objref);
  void IncrRelTime (const Real & t);
  void IncrAbsTime (const Int & t);
  void IncrCycleTime(const Real & t);
  void IncrIdleTime(const Generic & nm, int delta);
  bool InDuration () const;
  // void RecordTime();
  int GetTime () const;
  int GetTimeOfCPU (const Generic &) const;
  void PushDuration();
  void PopDuration(const Real & t);
  void PushCycle();
  void PopCycle(const Real & c);
  int TimeForClockCycles(const Real & c) const;
  int GetCPUTime(const Generic &) const;
  void SetCPUTime(const Generic &, int);
  bool CPUSwapNeeded(const Generic &) const;
  bool CPUSwapNeeded() const;
  bool MultCPUs() const;
  TYPE_SCHDTP_PrimarySchedulerAlgorithm GetPrimaryAlgorithm(const Generic & cpu) const;
  void SetPrimaryAlgorithm(const TYPE_SCHDTP_PrimarySchedulerAlgorithm & a,
                           const Generic & cpu);
  TYPE_SCHDTP_SecondarySchedulerAlgorithm GetSecondaryAlgorithm(const Generic & cpu) const;
  Generic GetCurCPU() const;
  Set GetAllCPUs() const;
  void SetCurCPU(const Generic &, const Int &);
  bool OnCurCPU(const Generic &) const;
  Generic CPUAllocated(const TYPE_SEM_OBJ_uRef &) const;
  TYPE_STKM_MessageId AddMessageToBUS(const Generic &, const TYPE_SEM_ExplOP &, const Sequence &);
  void InsertMessageRequest(const Record & mesreq);
  void AddBroadcastMessageToBUS(const Generic & objref,
                                const TYPE_SEM_ExplOP & opsem,
                                const SEQ<TYPE_SEM_VAL> & arg_lv);
  void AddReturnMessageToBUS(const TYPE_STKM_MessageId &,
                             const Int &, const Int &,
                             const TYPE_SEM_VAL &, const TYPE_AS_Name &,
                             const Generic &, const TYPE_SCHDTP_ThreadId &);
  void StepBUSTimeBack(const TYPE_AS_Name &, const Int &);
  void StepCPUTimeBack(const Generic &, const Int &);
  TYPE_STKM_MessageId AddNewMessageId();
  Generic FindBUSFromClass(const Generic &) const;
  Tuple FindFirstCPU() const;
  Tuple FindFirstBUS() const;
  TYPE_SEM_VAL UpdateObjRef(const TYPE_SEM_VAL &);
  Tuple SmallNameAndNextTime(const Map &) const;
  void SetTime (double new_time);

  TYPE_STKM_BUSSigma EmptyBUS(const Generic &);
  void DeployInst(const Generic & cpunm, const TYPE_SEM_OBJ_uRef & obj_ref);
  void DelayDeployInst(const Generic & cpunm, const TYPE_SEM_OBJ_uRef & obj_ref);
  void LateDeployInst();
  void AddPriorityEntry(const TYPE_AS_Name & cpunm, const TYPE_AS_Name & opnm, const Int & prio);
  bool CurCPUAnyThread(const Generic & nm) const;
  bool CurCPUAnyMessages(const Generic & nm) const;
  Tuple SelAndRunMessageCall(const Generic & cpunm);
  Generic FindFirstOpMes(const Map &) const;
  //Tuple RunMessage(const TYPE_STKM_Message&);
  Tuple RunMessage(const TYPE_STKM_Message &, const Generic & cpunm);
  Tuple RunBroadcastMessage(const TYPE_STKM_MessageId & id,
                            const Int & t,
                            const Int & p,
                            const TYPE_STKM_MessageBroadcast & mes);
  TYPE_CI_ContextId GetCID(const TYPE_SEM_VAL &) const;
  bool CPUWork(const Generic & cpunm, int swapt) const;
  void ProcessBUS(const TYPE_AS_Name & nm, int swapt);
  bool BUSWork(int t, int swapt,
               const MAP<TYPE_STKM_MessageId, TYPE_STKM_Message> & mes_m,
               const Sequence & waiting) const;
  Generic FindFirstMessage(const MAP<TYPE_STKM_MessageId, TYPE_STKM_Message> & mes_m) const;
  bool Earlier(const MAP<TYPE_STKM_MessageId, TYPE_STKM_Message> & mes_m,
               const TYPE_STKM_MessageId & mes,
               const Generic & id) const;
  void InsertMesInCPU(const Generic & tcpu, const TYPE_AS_Name & opnm, const TYPE_STKM_Message & mes, int time);
  void InsertBroadMesInCPU(const Map & tcpum,
                           const TYPE_AS_Name & opnm,
                           const TYPE_STKM_Message & mes,
                           int time,
                           const SET<TYPE_SEM_OBJ_uRef> & objs);
  int CalcDuration(int size, const Generic & capacity) const;
//  int MinVal(int x, int y) const;
  int MaxVal(int x, int y) const;
  bool IsSystem() const;
  Generic GetCPUCapacity(const Generic & cpunm) const;
  void AddNewObj(const TYPE_SEM_OBJ_uRef & ref);
  void AddCPU(const Generic & sysnm, const TYPE_AS_Name & cpunm, const Generic & decl);
  void AddBUS(const TYPE_AS_Name & busnm, const Generic & decl);
  void AddVirtualBUS();
  void AddToVirtualBUS(const TYPE_AS_Name & newcpuid); 
  bool MultiThreads(const Generic & cpu) const;
#endif // VICE
};

#endif // __evalsystem_h__
