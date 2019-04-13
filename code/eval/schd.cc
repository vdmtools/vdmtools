/***
*  * WHAT
*  *   Scheduler.
*  * FILE
*  *    $Source: /mnt/cvsroot/toolbox/code/eval/schd.cc,v $
*  * VERSION
*  *    $Revision: 1.98 $
*  * DATE
*  *    $Date: 2006/10/06 11:56:59 $
*  * STATUS
*  *    $State: Exp $
*  * PROJECT
*  *    VDM++ Toolbox
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
*  * SPECIFICATION VERSION
*  *    schd.cc: Implementation of schd.vdm 1.40
*  *       from 1.22->1.25 is the KillThread operation not implemented.
*  *                       It is not currently used in spec
*  *
***/

#include "stackeval.h"
#include "SCHDTP.h"
#include "libman.h"
#include "astaux.h"
#include "rt_errmsg.h"
#ifdef VICE
#include "TIMETRACE.h"
#endif // VICE
#include "settings.h"
#include "schd.h"
#include "tbutils.h"

const Int SCHD::Default_priority = Int(1);

static Generic dbthres = Nil();

SCHD::SCHD()
{
  this->threads = Map();
  this->curthread_m = Map();
#ifdef VICE
  this->perthreads = Map();
  this->cpualloc = Map();
  this->cpusblocked = Set();
  this->potentialperchanged = Set();
  this->lastswapedin = Map();
  this->lastswapedout = Map();
  this->tobekilled = Map();
  this->killed = Map();
  this->delayed = Nil();
#endif // VICE
  this->secondaryAlgorithm = TYPE_SCHDTP_RoundRobin();
  this->classPriorityMap = Map();
  this->maxPriority = Int(100);
  this->next_thread_id = Int(1);
  this->checkingGuard = Bool(false);
  this->x = Int(1);

//  this->prev_id = Nil();
}

SCHD::~SCHD()
{
}

void SCHD::Dump(wostream & wos)
{
  wos << L"threads: " << this->threads << endl;
  wos << L"curthread_m: " << this->curthread_m << endl;
#ifdef VICE
  wos << L"perthreads: " << this->perthreads << endl;
  wos << L"cpualloc: " << this->cpualloc << endl;
  wos << L"cpublocked: " << this->cpusblocked << endl;
  wos << L"potentialperchanged: " << this->potentialperchanged << endl;
  wos << L"lastswapedin: " << this->lastswapedin << endl;
  wos << L"lastswapedout: " << this->lastswapedout << endl;
  wos << L"tobekilled: " << this->tobekilled << endl;
  wos << L"killed: " << this->killed << endl;
  wos << L"delayed: " << this->delayed << endl;
#endif // VICE
  wos << L"secondaryAlgorithm: " << this->secondaryAlgorithm << endl;
  wos << L"classPriorityMap: " << this->classPriorityMap << endl;
  wos << L"maxPriority: " << this->maxPriority << endl;
  wos << L"next_thread_id: " << this->next_thread_id << endl;
  wos << L"checkingGuard: " << this->checkingGuard << endl;
}

// The purpose of ClearScheduler is to empty all of the schedulers
// datastructures.
// These include: the threads map, ...
//
// The ClearScheduler is called from the StackEval destructor and from
// InitScheduler.

// It will be called from the StackEval destructor before the
// destructor deletes all the class pointers.
//
// The reason for separating ClearScheduler and InitScheduler is that
// InitScheduler may set up datastructures to prepare for an
// evaluation.

// ClearScheduler
void SCHD::ClearScheduler()
{
  // remove the existing threads.
//  while (!this->threads.IsEmpty()) {
//    Generic id, h;
//    this->threads.First(id, h);
//    KillThreadById(id);
//  }
  if (!this->threads.IsEmpty())
  {
    Set dom_threads (this->threads.Dom());
    Generic id;
    for (bool bb = dom_threads.First(id); bb; bb = dom_threads.Next(id))
      KillThreadById(id);
  }

  this->curthread_m.Clear();
#ifdef VICE
  this->lastswapedin.Clear();
  this->lastswapedout.Clear();
  this->potentialperchanged.Clear();
  this->tobekilled.Clear();
#endif // VICE

  dbthres = Nil();
}

// This operation is not specified yet. It initialiases the scheduler.
// InitScheduler
// ast_is_new : bool
// ==> ()
void SCHD::InitScheduler(bool ast_is_new, bool ast_is_empty)
{
  ClearScheduler();

#ifdef VICE
  if(ast_is_empty)
    TIMETRACE::LoggingOff();
  else
    TIMETRACE::LoggingOn();

  if(!ast_is_new)
  {
//    Set ids (this->tobekilled.Dom());
//    Generic id;
//    for(bool bb = ids.First(id); bb; bb = ids.Next(id))
//    {
//      this->tobekilled.ImpModify(id, Nil());
//    }
    this->tobekilled.Clear();
    this->killed.Clear();
    this->cpualloc.Clear();
    this->curthread_m.Clear();
    this->cpusblocked.Clear();
    this->potentialperchanged.Clear();
    this->lastswapedin.Clear();
    this->lastswapedout.Clear();

    this->delayed = Nil();
  }
  Set dom_perthreads (this->perthreads.Dom());
  Generic nm;
  for(bool bb = dom_perthreads.First(nm); bb; bb = dom_perthreads.Next(nm))
  {
    this->perthreads.ImpModify(nm, Map());
  }
#endif //VICE

  // Note that this code is different to the specification,
  // since at the specification level the priority map is
  // set by the test environment before this operation is called.
  if (!this->classPriorityMap.IsEmpty())
    this->classPriorityMap.Clear();
  else if (Settings.PriorityBased())
    this->maxPriority = SCHD::Default_priority + Int(1);
  else
    this->maxPriority = SCHD::Default_priority;

//wcout << L"InitScheduler" << endl;
  SetCurThread(Int(0), false);

  if (Settings.PriorityBased())
    this->secondaryAlgorithm = TYPE_SCHDTP_PriorityBased();
  else
    this->secondaryAlgorithm = TYPE_SCHDTP_RoundRobin();

  this->next_thread_id = 1;

  this->checkingGuard = Bool(false);

  StartDebugThread();
}

// KillThreadById
// threadid : SCHDTP`ThreadId
void SCHD::KillThreadById(const TYPE_SCHDTP_ThreadId & threadid)
{
  Generic gthri;
  if (! this->threads.DomExists(threadid, gthri)) {
    RTERR::Error(L"KillThreadById", RTERR_SEL_NONVALID_THREADID, NilContextId, Nil(), Sequence());
  }

  TYPE_SCHDTP_ThreadInfo threadinfo (gthri);
  Generic stackeval = threadinfo.get_stackeval();

  if (stackeval.IsNil()) {
    RTERR::Error(L"KillThreadById", RTERR_SEL_NONVALID_THREADID, NilContextId, Nil(), Sequence());
  }

  EvaluatorStatusCt es;
  threadinfo.set_stackeval(es);
  this->threads.ImpModify(threadid, threadinfo);

  this->threads.RemElem(threadid);
#ifdef VICE
  this->cpualloc.RemElem(threadid);
#endif // VICE
//  if(!IsDebugThread(threadid))
//    this->system.DeleteThread(threadid);
}

// EvalScheduler
// ==> ()
Tuple SCHD::EvalScheduler()
{
  theSystem().ResetSlice();

  Tuple evalres (theStackMachine().EvalMainLoop());

  while (true)
  {
//wcout << CurThreadId() << endl;

#ifdef VICE
//wcout << evalres << endl;
//wcout << L"#######################################################" << endl;
//  if( theSystem().GetTime(MkName(L"vCPU")) > 500 )
//  if( theSystem().GetTime(Nil()) > 500 )
//    return mk_(TYPE_STKM_Interrupt(), Nil());
//wcout << theSystem().GetTime(MkName(L"vCPU")) << endl;

//    SaveCurThreadState(evalres.GetField(1));
    SaveCurThreadState();
////    -- temp insertion
//wcout << evalres << endl;
#endif // VICE

    const TYPE_STKM_EvaluationState & stack_state (evalres.GetRecord(1));

    switch (stack_state.GetTag()) {
      case TAG_TYPE_STKM_EndOfSlice: {
#ifdef VICE
        if( this->cpusblocked == theSystem().GetAllCPUs())
        {
          //if( !this->perthreads.IsEmpty() )
          if (this->perthreads.DomExists(theSystem().GetCurCPU()) &&
              !Map(this->perthreads[theSystem().GetCurCPU()]).IsEmpty())
          {
            evalres = RunNextPeriodicThread(theSystem().GetCurCPU());
          }
          else {
            bool found = false;
            if (!this->potentialperchanged.IsEmpty()) {
              Set dom_threads (this->threads.Dom());
              Generic tid;
              for(bool bb = dom_threads.First(tid); bb && !found; bb = dom_threads.Next(tid)) {
                TYPE_SCHDTP_ThreadInfo thr (this->threads[tid]);
                if( !thr.get_status().Is(TAG_TYPE_SCHDTP_Blocked) ) continue;
                TYPE_SCHDTP_Blocked bs (thr.get_status());
                if( ( bs.get_objref().IsNil() ) ||
                    this->potentialperchanged.InSet(bs.get_objref()) ) {
                  found = true;
                }
              }
            }
            if(found) {
              evalres = SelAndRunThread(TYPE_SCHDTP_MaxReached(), false);
            }
            else {
              RTERR::Error(L"EvalScheduler", RTERR_DEADLOCK_DETECTED, Nil(), Nil(), Sequence());
            }
          }
        }
        else
#endif // VICE
        evalres = SelAndRunThread(TYPE_SCHDTP_MaxReached(), false);
        break;
      } // TAG_TYPE_STKM_EndOfSlice
#ifdef VICE
      case TAG_TYPE_STKM_EndOfCPUSlice: {
        if(!CheckingGuard() && theStackMachine().GetUserBREAK()) {
          theStackMachine().ResetUserBREAK();
          return mk_(TYPE_STKM_Interrupt(), Nil());
        }
        Tuple ffcpu (theSystem().FindFirstCPU());
        const Generic & cpunm (ffcpu.GetField(1));
        int cput = ffcpu.GetIntValue(2);
        const Generic & cswapt_g (ffcpu.GetField(3));

        Tuple ffbus (theSystem().FindFirstBUS());
        const TYPE_AS_Name & busnm (ffbus.GetRecord(1));
        int bust = ffbus.GetIntValue(2);
        const Generic & bswapt_g (ffbus.GetField(3));
//wcout << ffcpu << endl;
//wcout << ffbus << endl;
        if( bust <= cput ) {
          int swapt = cput;
          if( bswapt_g.IsNil() ) {
            swapt = cput;
          }
          else {
            int bswapt = Int(bswapt_g).GetValue();
            if (bust == bswapt) {
              swapt = cput;
            }
            else if (bswapt < cput) {
              swapt = bswapt;
            }
          }
//wcout << L"BUS: " << ASTAUX::ASName2String(busnm) << L" " << theSystem().GetTime() << L" " << swapt << endl;
//wcout << L"BUS: " << ASTAUX::ASName2String(busnm) << L" " << bust << L" " << swapt << endl;
          theSystem().ProcessBUS(busnm, swapt);
          evalres = DefaultEvalRes();
        }
        else {
          int swapt = bust;
          if( cswapt_g.IsNil() ) {
            swapt = bust;
          }
          else {
            int cswapt = Int(cswapt_g).GetValue();
            if (cput == cswapt) {
              swapt = bust;
            }
            else if (cswapt < bust) {
              swapt = cswapt;
            }
          }
//wcout << L"CPU: " << SYSTEM::PrintCPU(cpunm) << L" " << theSystem().GetTime() << L" " << swapt << endl;
//wcout << L"CPU: " << SYSTEM::PrintCPU(cpunm) << L" " << cput << L" " << swapt << endl;
//wcout << cpunm << endl;
//wcout << swapt << endl;
//wcout << (theSystem().CPUWork(cpunm,swapt) ? L"true" : L"false") << endl;
          if(!theSystem().CPUWork(cpunm,swapt)) {
            theSystem().IncrIdleTime(cpunm, Settings.GetStepSize());
            evalres = DefaultEvalRes();
          }
          else {
            Generic curcpu (theSystem().GetCurCPU());
            theSystem().SetCurCPU(cpunm, swapt);

            if( theSystem().CurCPUAnyMessages(cpunm) ) {
  Generic cpu (theSystem().GetCurCPU());
  Generic cur (CurThreadId());
/*
wcout << L"CPUWork: curcpu: " << theSystem().PrintCPU(curcpu) << endl;
*/
              //SwapOut(false, true);
              SwapOut(cur, cpu, false, true);
              //SwapOut(cur, curcpu, false, true);
              evalres = theSystem().SelAndRunMessageCall(cpunm);
            }
            else if( theSystem().CurCPUAnyThread(cpunm) )
            //else if( theSystem().CurCPUAnyThread(cpunm) ||
            //         (this->perthreads.DomExists(cpunm) && !Map(this->perthreads[cpunm]).IsEmpty()) )
            {
              //this->cpusblocked.Clear();
              this->cpusblocked.ImpDiff(mk_set(cpunm));
              evalres = SelAndRunThread(Nil(), true);
            }
            else if( this->cpusblocked == theSystem().GetAllCPUs() ) {
              RTERR::Error(L"EvalScheduler", RTERR_DEADLOCK_DETECTED, Nil(), Nil(), Sequence());
            }
            else if( PeriodicReady(cpunm, Map(this->perthreads[cpunm]).Dom()) ) {
              evalres = RunNextPeriodicThread(cpunm);
            }
            else {
              bool mainloop = false;
              Generic curcpu (theSystem().GetCurCPU());
              Generic tid;
              Generic g;
              if( this->curthread_m.DomExists(curcpu, tid) && this->threads.DomExists(tid, g) )
              {
                TYPE_SCHDTP_ThreadInfo thr (g);
                if( thr.get_status().Is(TAG_TYPE_SCHDTP_Running) ||
                    thr.get_status().Is(TAG_TYPE_SCHDTP_MaxReached) )
                {
                  evalres = theStackMachine().EvalMainLoop();
                  mainloop = true;
                }
              }
              if(!mainloop)
              {
                this->cpusblocked.Insert(cpunm);
                theSystem().IncrIdleTime(cpunm,Settings.GetStepSize());
                evalres = DefaultEvalRes();
              }
            }
          }
        }
        break;
      } // TAG_TYPE_STKM_EndOfSlice
#endif // VICE
      case TAG_TYPE_STKM_Guard: {
        const TYPE_AS_Name & fullopnm (stack_state.GetRecord(pos_STKM_Guard_opnm));
        const Generic & obj (stack_state.GetField(pos_STKM_Guard_curobj));
        theSystem().ResetGuard();
        Generic instr (theState().LookUpPermis(fullopnm));

#ifdef VICE
        if (instr.IsNil())
        {
          if( obj.IsNil() || theSystem().OnCurCPU(obj) )
          {
            evalres = theStackMachine().EvalMainLoop();
          }
          else
          {
            Generic cur (CurThreadId());
            if( !cur.IsNil() )
            {
              TYPE_SCHDTP_ThreadInfo thr (this->threads[cur]);
              thr.set_status(TYPE_SCHDTP_MaxReached());
              this->threads.ImpModify(cur, thr);
            }
            evalres = SelAndRunThread(Nil(), false);
          }
        }
#else
        if (instr.IsNil())
        {
          evalres = theStackMachine().EvalMainLoop();
        }
#endif // VICE
        else
        {
          if (RunGuard(instr, obj)) {
            evalres = theStackMachine().EvalMainLoop();
          } else {
            TYPE_SCHDTP_Blocked blocked = TYPE_SCHDTP_Blocked().Init(fullopnm, obj);
            evalres = SelAndRunThread(blocked, false);
          }
        }
        break;
      } // TAG_TYPE_STKM_Guard
      case TAG_TYPE_STKM_Breakpoint:
      case TAG_TYPE_STKM_Interrupt: {
          return evalres;
      }
      case TAG_TYPE_STKM_Success: {
#ifdef VICE
        // for debug command execution
        //if( theStackMachine().CallStackLevel() > 0 )
        //if ( theStackMachine().CallStackLevel() > (theStackMachine().UsesThreads() ? 1 : 0) )
        //  return evalres;
        if ( IsDebugThread(CurThreadId()) &&
             ( theStackMachine().CallStackLevel() > (theStackMachine().UsesThreads() ? 1 : 0) ) ) {
          return evalres;
        }
  Generic cpu (theSystem().GetCurCPU());
  Generic cur (CurThreadId());
        //SwapOut(true, true);
        SwapOut(cur, cpu, true, true);

        if (IsDebugThread(CurThreadId())) {
          return evalres;
        } else {
          Generic cur (CurThreadId());
          if (!IsPeriodicThread(cur)) {
            TYPE_SCHDTP_ThreadInfo thr (this->threads[cur]);
            const Generic & mesid (thr.GetField(pos_SCHDTP_ThreadInfo_mesid));

            this->threads.RemElem(cur);
            this->cpualloc.RemElem(cur);
            theSystem().DeleteThread(cur);

            Generic cpu (theSystem().GetCurCPU());
            if( !mesid.IsNil() || this->tobekilled.DomExists(cpu) ) {
              TIMETRACE::LogKillThread(this->tobekilled[cpu], cpu);
            }
            this->killed.ImpModify(cpu, Bool(true));
            evalres = DefaultEvalRes();
          }
          else {
            if( !cur.IsNil() ) {
              SetThreadStatus(cur, TYPE_SCHDTP_Completed());
            }
            TYPE_SCHDTP_ThreadInfo thr (this->threads[cur]);
            bool running = ( !cur.IsNil() && IsPeriodicThread(cur) && thr.get_status().Is(TAG_TYPE_SCHDTP_Running));
            evalres = SelAndRunThread(Nil(), !running);
          }
        }
#else
        TYPE_SCHDTP_ThreadId cur (CurThreadId());
        if (IsDebugThread(cur)) {
          return evalres;
        }
        else {
          this->threads.RemElem(cur);
          theSystem().DeleteThread(cur); //
          evalres = SelAndRunThread(Nil(), true);
        }
#endif //VICE
        break;
      } // TAG_TYPE_STKM_Success
      default: {
          RTERR::Error(L"EvalScheduler", RTERR_INTERNAL_ERROR, Nil(), Nil(), Sequence());
      }
    } // end of switch
  } // end of while
  return evalres; // to the compiler silent
}

// DefaultEvalRes
// ==> STKM`EvaluationState * [SEM`VAL]
Tuple SCHD::DefaultEvalRes()
{
#ifdef VICE
  if(theSystem().MultCPUs())
    return mk_(TYPE_STKM_EndOfCPUSlice(), Nil());
  else
    return mk_(TYPE_STKM_EndOfSlice(), Nil());
#else
  return mk_(TYPE_STKM_EndOfSlice(), Nil());
#endif // VICE
}

// StartDebugThread
// ==> ()
void SCHD::StartDebugThread ()
{
#ifdef VICE
  Generic cur = CurThreadId();
  TYPE_SCHDTP_ThreadId threadid (AddNewThreadId(Bool(true), Nil(), TYPE_STKM_SubProgram(),
                                                SCHD::Default_priority, Bool(false), Nil()));
  SetCurThread(threadid, true);
  if (!cur.IsNil())
  {
    SetThreadStatus(cur, TYPE_SCHDTP_Running());
    SetMaxPriority(cur);
    RestoreSTKM(cur);
  }
#else
  TYPE_SCHDTP_ThreadId threadid (AddNewThreadId(Bool(true), Nil(), TYPE_STKM_SubProgram()));
  SetCurThread(threadid, true);
  TYPE_SCHDTP_ThreadId cur (CurThreadId());
  SetThreadStatus(cur, TYPE_SCHDTP_Running());
  SetMaxPriority(cur);
  RestoreSTKM(cur);
#endif // VICE
}

// RunGuard
// instr : STKM`SubProgram
// objref : [SEM`OBJ_Ref]
// ==> bool
bool SCHD::RunGuard(const TYPE_STKM_SubProgram& instr, const Generic & objref)
{
  SetCheckingGuard(true);
  Tuple res (theStackMachine().EvalRunGuard(instr, objref));
  SetCheckingGuard(false);

  const TYPE_STKM_EvaluationState & main_state (res.GetRecord(1));
  switch(main_state.GetTag()) {
    case TAG_TYPE_STKM_Success: {
      const Generic & eval_res (res.GetField(2));
      if( eval_res.Is(TAG_TYPE_SEM_BOOL) )
        return Record(eval_res).GetBoolValue(pos_SEM_BOOL_v);
      else
        RTERR::Error(L"RunGuard", RTERR_INTERNAL_ERROR, Nil(), Nil(), Sequence());
      break;
    }
    case TAG_TYPE_STKM_Guard: {
      RTERR::Error(L"RunGuard", RTERR_OP_IN_GUARD, Nil(), Nil(), Sequence());
      break;
    }
#ifdef VICE
    case TAG_TYPE_STKM_EndOfCPUSlice: {
      return false;
    }
#endif // VICE
    default: {
      RTERR::Error(L"RunGuard", RTERR_INTERNAL_ERROR, Nil(), Nil(), Sequence());
      break;
    }
  }

  return false; // dummy to keep the compiler silent.
}

// SelAndRunThread
// threadstatus : [SCHDTP`ThreadStatus]
// curthread_terminated : bool
// ==> STKM`EvaluationState * [SEM`VAL]
Tuple SCHD::SelAndRunThread(const Generic & threadstatus, bool curthread_terminated)
{
#ifdef VICE
  Generic cur (CurThreadId());
#else
  TYPE_SCHDTP_ThreadId cur (CurThreadId());
#endif // VICE

  if( !curthread_terminated && !threadstatus.IsNil() && this->threads.DomExists(cur) )
  {
    SetThreadStatus(cur, threadstatus);
    SetThreadState(cur);
  }

#ifdef VICE
  if (!cur.IsNil()) {
    if (IsPeriodicThread(cur) && curthread_terminated)
    {
      TYPE_SCHDTP_ThreadStatus status (GetThreadStatus(cur));
      switch (status.GetTag()) {
        case TAG_TYPE_SCHDTP_Completed: {
          SetThreadState(cur);
          KillThread(this->tobekilled[theSystem().GetCurCPU()]);
          break;
        }
        case TAG_TYPE_SCHDTP_Blocked:
        case TAG_TYPE_SCHDTP_SyncOp: {
          break;
        }
        case TAG_TYPE_SCHDTP_SyncOpRet:
        case TAG_TYPE_SCHDTP_MaxReached:
        case TAG_TYPE_SCHDTP_Sleeping:
        case TAG_TYPE_SCHDTP_Running: {
          SetThreadState(cur);
          SetCurThread(cur, true);
          RestoreSTKM(cur);
          theSystem().ResetSlice();
          return theStackMachine().EvalMainLoop();
          break;
        }
      }
    }

    if( !curthread_terminated && IsRunningThread(cur) )
    {
      RestoreInstrAndSTKM(cur);
      theSystem().ResetSlice();
      return theStackMachine().EvalMainLoop();
    }
  }
#endif //VICE

  Generic nextthread (FindNextThread());

//wcout << L"next thread: " << nextthread << endl;

  if (nextthread.IsNil()) {
#ifdef VICE
    Generic curcpu (theSystem().GetCurCPU());
    theSystem().IncrIdleTime(curcpu, Settings.GetStepSize());
    this->cpusblocked.Insert(curcpu);
    //if (this->perthreads.IsEmpty())
    if (this->perthreads.IsEmpty() ||
        (this->perthreads.DomExists(curcpu) && Map(this->perthreads[curcpu]).IsEmpty()) ) {
      if (this->cpusblocked == theSystem().GetAllCPUs()) {
        RTERR::Error(L"EvalScheduler", RTERR_DEADLOCK_DETECTED, Nil(), Nil(), Sequence());
      }
    }
    return DefaultEvalRes();
#else
    RTERR::Error(L"SelAndRunThread", RTERR_DEADLOCK_DETECTED, Nil(), Nil(), Sequence());
    return DefaultEvalRes(); // dummy
#endif // VICE
  }
  else {
    SetCurThread(nextthread, true);
    SetThreadStatus(nextthread, TYPE_SCHDTP_Running());
    RestoreSTKM(nextthread);
    theSystem().ResetSlice();
#ifdef VICE
    this->killed.ImpModify(theSystem().GetCurCPU(), Bool(false));
#endif //VICE
    return theStackMachine().EvalMainLoop();
  }
}

#ifdef VICE
// RunNextPeriodicThread
// cpu : [AS`Name]
// ==> STKM`EvaluationState * [SEM`VAL]
Tuple SCHD::RunNextPeriodicThread(const Generic & cpu)
{
  Map orpt_m (this->perthreads[cpu]); // map SEM`OBJ_Ref to (SCHDTP`PerThreadInfo | SCHDTP`SpoThreadInfo)
  Set dom_orpt_m (orpt_m.Dom());
  Map perthrm;       // map SEM`OBJ_Ref to nat
  Generic objref;
  for(bool bb = dom_orpt_m.First(objref); bb; bb = dom_orpt_m.Next(objref)) {
    perthrm.Insert(objref, Int(TimePerThread(orpt_m[objref])));
  }

  //
  Set objs (perthrm.Dom());
  Set dom_perthrm (perthrm.Dom());
  TYPE_SEM_OBJ_uRef first (objs.GetElem());
  Generic o;
  for(bool cc = dom_perthrm.First(o); cc; cc = dom_perthrm.Next(o)) {
    if( Int(perthrm[first]).GetValue() > Int(perthrm[o]).GetValue() ) {
      first = o;
    }
  }

//  int time = theSystem().GetTime();
  int time = theSystem().GetTimeOfCPU(cpu);

  if( Int(perthrm[first]).GetValue() > time ) {
    theSystem().IncrIdleTime(cpu, Int(perthrm[first]).GetValue() - time);
  }
  return SelAndRunThread(Nil(), true);
}

// TimePerThread
// thrinfo : SCHDTP`PerThreadInfo | SCHDTP`SpoThreadInfo
// ==> nat
int SCHD::TimePerThread(const Record & thrinfo)
{
  if (thrinfo.Is(TAG_TYPE_SCHDTP_PerThreadInfo)) {
    JitterModeType::JMType mode = Settings.GetJitterMode();
    switch (mode) { 
      case JitterModeType::JM_Early: {
        return thrinfo.GetIntValue(pos_SCHDTP_PerThreadInfo_next_umin);
      }
      case JitterModeType::JM_Late: {
        return thrinfo.GetIntValue(pos_SCHDTP_PerThreadInfo_next_umax);
      }
      case JitterModeType::JM_Random: {
        int max = thrinfo.GetIntValue(pos_SCHDTP_PerThreadInfo_next_umax);
        int min = thrinfo.GetIntValue(pos_SCHDTP_PerThreadInfo_next_umin);

        if (max == min) {
          return max;
        }
        int num = theStackMachine().GetRandom(max - min + 1);
        return (min + num);
      }
      default: {
        return thrinfo.GetIntValue(pos_SCHDTP_PerThreadInfo_next_umin);
      }
    }
  }
  else
  {
    int max = thrinfo.GetIntValue(pos_SCHDTP_SpoThreadInfo_next_umax);
    int min = thrinfo.GetIntValue(pos_SCHDTP_SpoThreadInfo_next_umin);

    if (max == min)
      return max;

    int num = theStackMachine().GetRandom(max - min + 1);
    return (min + num);
  }
}

// AddCPU
// cpunm : AS`Name
// ==> ()
void SCHD::AddCPU(const TYPE_AS_Name & cpunm)
{
  this->perthreads.ImpModify(cpunm, Map());
}

// EvalValue
// expr : AS`Expr
// objref : SEM`OBJ_Ref
// ==> nat
int SCHD::EvalValue(const TYPE_AS_Expr & expr, const TYPE_SEM_OBJ_uRef & objref)
{
  theStackMachine().SetCid(ASTAUX::GetCid(expr));
  theStackMachine().PushCurObj(objref, Nil(), Nil());
  theStackMachine().PushEmptyEnv();
  Tuple res (theStackMachine().EvalAuxCmd(expr,
                                          TYPE_STKM_SubProgram(),
                                          TYPE_STKM_SubProgram(),
                                          SEQ<Char>(L"Eval param for thread")));

  theStackMachine().PopEnvL();
  theStackMachine().PopCurObj();

  const TYPE_STKM_EvaluationState & eval_state (res.GetRecord(1));
  if (!eval_state.Is(TAG_TYPE_STKM_Success))
    RTERR::Error(L"EvalValue", RTERR_INTERNAL_ERROR, Nil(), Nil(), Sequence());
  else
  {
    const TYPE_SEM_VAL & exp_v (res.GetField(2));
    if (!exp_v.Is(TAG_TYPE_SEM_NUM))
      RTERR::Error(L"EvalValue", RTERR_NAT_EXPECTED, Nil(), Nil(), Sequence());
    else
    {  
      const Real & v (exp_v.GetReal(pos_SEM_NUM_v));
      if (!v.IsNat())
        RTERR::Error(L"EvalValue", RTERR_NAT_EXPECTED, Nil(), Nil(), Sequence());
      return v.GetIntValue();
    }
  }
  return 0; // dummy
}

// SetPerThread
// objref : SEM`OBJ_Ref
// thread : STKM`PerThread
// instr : STKM`SubProgram
// prio : nat
// ==> ()
void SCHD::SetPerThread(const TYPE_SEM_OBJ_uRef & objref, const TYPE_STKM_PerThread & t, const Int & prio)
{
  const TYPE_STKM_SubProgram & instr (t.GetSequence(pos_STKM_PerThread_instr));
  int period = EvalValue(t.GetRecord(pos_STKM_PerThread_period),objref);
  int jitter = EvalValue(t.GetRecord(pos_STKM_PerThread_jitter),objref);
  int delay = EvalValue(t.GetRecord(pos_STKM_PerThread_delay),objref);
  int offset = EvalValue(t.GetRecord(pos_STKM_PerThread_offset),objref);
  Generic cpu (theSystem().OnCurCPU(objref) ? theSystem().GetCurCPU() : theSystem().CPUAllocated(objref));

  // TODO: check 0 < period and delay < period
  int time = theSystem().GetCPUTime(cpu);

  TYPE_SCHDTP_PerThreadInfo tinfo;
  tinfo.Init(Int(period),
             Int(jitter),
             Int(delay),
             Int(offset),
             Int(1),                      // iteration
             instr,                       // periodBody
             Int(time),                   // release_time
             Int(time + offset),          // next_min
             Int(time + offset + jitter), // next_max
             prio); // 

  Map perthread (this->perthreads.DomExists(cpu) ? Map(this->perthreads[cpu]) : Map());
  perthread.ImpModify(objref, tinfo);
  this->perthreads.ImpModify(cpu, perthread);
}

// SetSpoThread
// objref : SEM`OBJ_Ref
// thread : STKM`SpoThread
// instr : STKM`SubProgram
// prio : nat
// ==> ()
void SCHD::SetSpoThread(const TYPE_SEM_OBJ_uRef & objref, const TYPE_STKM_SpoThread & t, const Int & prio)
{
  const TYPE_STKM_SubProgram & instr (t.GetSequence(pos_STKM_SpoThread_instr));
  int delay = EvalValue(t.GetRecord(pos_STKM_SpoThread_delay),objref);
  int bound = EvalValue(t.GetRecord(pos_STKM_SpoThread_bound),objref);
  int offset = EvalValue(t.GetRecord(pos_STKM_SpoThread_offset),objref);
  Generic cpu (theSystem().OnCurCPU(objref) ? theSystem().GetCurCPU() : theSystem().CPUAllocated(objref));

  // TODO : check delay < bound
  int time = theSystem().GetCPUTime(cpu);

  TYPE_SCHDTP_SpoThreadInfo tinfo;
  tinfo.Init(Int(delay),
             Int(bound),
             Int(offset),
             instr,                      // periodBody
             Int(time),                  // release_time
             Int(time + offset),         // next_min
             Int(time + offset + bound), // next_max
             prio); // 

  Map perthread (this->perthreads.DomExists(cpu) ? Map(this->perthreads[cpu]) : Map());
  perthread.ImpModify(objref, tinfo);
  this->perthreads.ImpModify(cpu, perthread);
}

// IsRunningThread
// threadid : SCHDTP`ThreadId
// ==> bool
bool SCHD::IsRunningThread(const TYPE_SCHDTP_ThreadId & threadid)
{
  if(this->threads.DomExists(threadid))
  {
    TYPE_SCHDTP_ThreadInfo tinfo (this->threads[threadid]);
    return tinfo.get_status().Is(TAG_TYPE_SCHDTP_Running);
  }
  return false;
}

// IsPeriodicThread
// threadid : SCHDTP`ThreadId
// ==> bool
bool SCHD::IsPeriodicThread(const TYPE_SCHDTP_ThreadId & threadid)
{
  if(this->threads.DomExists(threadid))
  {
    TYPE_SCHDTP_ThreadInfo tinfo (this->threads[threadid]);
    return tinfo.GetBoolValue(pos_SCHDTP_ThreadInfo_periodic);
  }
  return false;
}

// ThreadObject
// threadid : SCHDTP`ThreadId
// ==> [SEM`OBJ_Ref]
Generic SCHD::ThreadObject(const TYPE_SCHDTP_ThreadId & threadid)
{
  Generic g;
  if(this->threads.DomExists(threadid,g))
  {
    TYPE_SCHDTP_ThreadInfo tinfo (g);
    return tinfo.get_obj();
  }
  return Nil();
}
#endif // VICE

// GetObjRefAndClass
// threadid : SCHDTP`ThreadId
// ==> [SEM`OBJ_Ref] * [AS`Name]
Tuple SCHD::GetObjRefAndClass(const TYPE_SCHDTP_ThreadId & threadid)
{
  Generic obj_ref;
  if (IsDebugThread(threadid))
    obj_ref = Nil();
  else
    obj_ref = GetObjRef(threadid);

  Generic cl;
  if (obj_ref.IsNil())
    cl = Nil();
  else
    cl = Record(obj_ref).GetRecord(pos_SEM_OBJ_uRef_tp);

  return mk_(obj_ref, cl);
}

#ifdef VICE
// AdvanceTime
// threadid : SCHDTP`ThreadId
//void SCHD::AdvanceTime(const TYPE_SCHDTP_ThreadId & threadid)
//{
//  double run_time = GetNextRunTime(threadid);
//  SetTime(run_time);
//}
#endif //VICE

// GetObjRef
// threadid : SCHDTP`ThreadId
// ==> [SEM`OBJ_Ref]
Generic SCHD::GetObjRef(const TYPE_SCHDTP_ThreadId & threadid)
{
  if(this->threads.DomExists(threadid))
  {
    TYPE_SCHDTP_ThreadInfo tinfo (this->threads[threadid]);
    return tinfo.get_obj();
  }
  return Nil();
}

// FindNextThread
// ==> [SCHDTP`ThreadId]
Generic SCHD::FindNextThread()
{
  SEQ<Sequence> orders (SortTheThreads()); // seq of seq of SCHDTP`ThreadId
  size_t len_orders = orders.Length();
  for (size_t idx = 1; idx <= len_orders; idx++)
  {
    const SEQ<TYPE_SCHDTP_ThreadId> & order (orders[idx]); // seq of SCHDTP`ThreadId

    size_t len_order = order.Length();
    for (size_t index = 1; index <= len_order; index++)
    {
      const TYPE_SCHDTP_ThreadId & threadno (order[index]);
#ifdef VICE
      Generic cpu (this->cpualloc[threadno]);
      if( cpu == theSystem().GetCurCPU() )
#endif // VICE
      {
        TYPE_SCHDTP_ThreadStatus status (GetThreadStatus(threadno));

        switch(status.GetTag()) {
          case TAG_TYPE_SCHDTP_Blocked: {
            const TYPE_AS_Name & fullopnm (status.GetRecord(pos_SCHDTP_Blocked_opnm));
            const Generic & obj (status.GetField(pos_SCHDTP_Blocked_objref));
            Generic instr (theState().LookUpPermis(fullopnm));

            RestoreSTKM(threadno);
            Int org_thread = CurThreadId();

            SetCurThread(threadno, false); //
            bool guardOK = RunGuard(instr, obj);
#ifdef VICE
            if(!org_thread.IsNil())
#endif // VICE
              SetCurThread(org_thread, false); //

            if (guardOK) {
              return threadno;
            } else {
            // skip;
            }
            break;
          }
#ifdef VICE
          case TAG_TYPE_SCHDTP_MaxReached: {
            return threadno;
          }
          case TAG_TYPE_SCHDTP_SyncOp: {
            // skip;
            break;
          }
#endif //VICE
          //case TAG_TYPE_SCHDTP_Running:
          //case TAG_TYPE_SCHDTP_Sleeping:
          //case TAG_TYPE_SCHDTP_Completed:
          //case TAG_TYPE_SCHDTP_SyncOpRet:
          default:
            return threadno;
        }
      }
    }
  }
  return Nil();
}

// SortTheThreads
// ==> seq of seq of SCHDTP`ThreadId
Sequence SCHD::SortTheThreads()
{
  TYPE_SCHDTP_SecondarySchedulerAlgorithm ssa = this->secondaryAlgorithm;

#ifdef VICE
  int curtime = theSystem().GetTime();
  Generic cpu (theSystem().GetCurCPU());
  Set objrefs;
  if( this->perthreads.DomExists(cpu)) {
    objrefs = Map(this->perthreads[cpu]).Dom();
  }
  if (!cpu.IsNil() || !theSystem().MultiThreads(cpu)) {
//    ssa = theSystem().GetSecondaryAlgorithm(cpu);
    ssa = TYPE_SCHDTP_RoundRobin();
  }
#endif // VICE

  switch (ssa.GetTag()) {
    case TAG_TYPE_SCHDTP_RoundRobin: {
      Sequence res; // seq of seq of SCHDTP`ThreadId
#ifdef VICE
      SET<TYPE_SCHDTP_ThreadId> dom_threads (this->threads.Dom());
      SET<TYPE_SCHDTP_ThreadId> thrs;
      Generic tid;
      for(bool bb = dom_threads.First(tid); bb; bb = dom_threads.Next(tid)) {
        if(!this->cpualloc.DomExists(tid)) continue;
        if( this->cpualloc[tid] != cpu ) continue;
        if( this->tobekilled.DomExists(cpu) ) {
          if (tid == this->tobekilled[cpu] ) continue;
        }
        thrs.Insert(tid);
      }
      res.ImpAppend(SortSelectedThreads(thrs, objrefs, cpu, curtime, SCHD::Default_priority));
#else
      res.ImpAppend(SortSelectedThreads(this->threads.Dom()));
#endif // VICE
      return res;
      break;
    }
    case TAG_TYPE_SCHDTP_PriorityBased: {
#ifdef VICE
      Map locres; // map nat to set of SCHDTP`ThreadId
      if (this->perthreads.DomExists(cpu)) {
        Map orpt (this->perthreads[cpu]); // map SEM`OBJ_Ref toSCHDTP`PerThreadInfo
        Generic objref;
        for(bool bb = objrefs.First(objref); bb; bb = objrefs.Next(objref)) {
          //TYPE_SCHDTP_PerThreadInfo ptinfo (orpt[objref]);
          Record ptinfo (orpt[objref]);
          Int prio (ptinfo.Is(TAG_TYPE_SCHDTP_PerThreadInfo) ? ptinfo.GetInt(pos_SCHDTP_PerThreadInfo_priority)
                                                             : ptinfo.GetInt(pos_SCHDTP_SpoThreadInfo_priority));
          if( !locres.DomExists(prio) ) {
            locres.ImpModify(prio, SET<TYPE_SCHDTP_ThreadId>());
          }
        }
      }
      else {
        // for old model (thread on nil cpu)
        if (cpu.IsNil()) {
          Set dom_cpualloc (this->cpualloc.Dom());
          bool forall = true;
          Map locres_tmp; // map nat to set of SCHDTP`ThreadId
          Generic id;
          for(bool cc = dom_cpualloc.First(id); cc && forall; cc = dom_cpualloc.Next(id)) {
            forall = this->cpualloc[id].IsNil();
            if (forall) {
              TYPE_SCHDTP_ThreadInfo ti (this->threads[id]);
              Int prio (ti.get_priority());
              if( !locres_tmp.DomExists(prio) ) {
                locres_tmp.ImpModify(prio, SET<TYPE_SCHDTP_ThreadId>());
              }
            } 
          }
          if (forall) {
            locres = locres_tmp;
          }
        }
      } 

      if (!locres.IsEmpty()) {
        Set dom_threads (this->threads.Dom());
        Generic id;
        for(bool cc = dom_threads.First(id); cc; cc = dom_threads.Next(id)) {
          TYPE_SCHDTP_ThreadInfo ti (this->threads[id]);
          Int prio (ti.get_priority());

          Generic tmpcpu, tmpid;
          if( this->cpualloc.DomExists(id, tmpcpu) &&
              (tmpcpu == cpu) &&
              (!this->tobekilled.DomExists(cpu, tmpid) || (tmpid != id))) {
            SET<TYPE_SCHDTP_ThreadId> id_s (locres[prio]);
            id_s.Insert(id);
            locres.ImpModify(prio, id_s);
          }
        }

        Sequence res; // seq of seq of SCHDTP`ThreadId
// differ from spec
        SET<Int> pr_s (locres.Dom());
        while(!pr_s.IsEmpty()) {
          // find max value
          Int prio (pr_s.GetElem());
          Generic g;
          for(bool bb = pr_s.First(g); bb; bb = pr_s.Next(g)) {
            Int p (g);
            if( p.GetValue() > prio.GetValue() ) {
              prio = p;
            }
          }
          pr_s.RemElem(prio);

          res.ImpAppend(SortSelectedThreads(locres[prio], objrefs, cpu, curtime, prio)); 
        }
        return res;
      }
      return Sequence();
      break;
#else
      // vdm++
      Sequence res; // seq of seq of SCHDTP`ThreadId
      int maxp = this->maxPriority.GetValue();
      for(int i = 1; i <= maxp; i++) {
        res.ImpAppend(Sequence());
      }
      for(int p = maxp; p >= 1; p--) {
        SET<TYPE_SCHDTP_ThreadId> threadsWithThisPriority;
        Set dom_threads (this->threads.Dom());
        Generic id;
        for(bool bb = dom_threads.First(id); bb; bb = dom_threads.Next(id)) {
          TYPE_SCHDTP_ThreadInfo ti (this->threads[id]);
          if (ti.get_priority() != Int(p)) continue;
          threadsWithThisPriority.Insert(id);
        }
        res.ImpModify(Int(maxp + 1 - p), SortSelectedThreads(threadsWithThisPriority));
      }
      return res;
      break;
#endif // VICE
    }
  }
  return Sequence();
}

// Reverse
// list : seq of seq of SCHDTP`ThreadId
// ==> seq of seq of SCHDTP`ThreadId
Sequence SCHD::Reverse(const Sequence & list)
{
  return list.Reverse();
}

#ifdef VICE
// NextMax
// peri : nat
// jit : nat
// offset : nat
// i : nat
// curtime : nat
// dist : nat
// -> nat
int SCHD::NextMax(int peri, int jit, int offset, int i, int curtime, int dist)
{
  int ideal = offset + i * peri + jit;
  int pos = curtime + dist;
  return ( ideal < pos ? pos : ideal );
}

// NextMin
// peri : nat
// jit : nat
// curtime : nat
// offset : nat
// dist : nat
// i : nat
// -> nat
int SCHD::NextMin(int peri, int jit, int curtime, int offset, int dist, int i)
{
  int ideal = offset + i * peri - jit;
  int pos = curtime + dist;
  return ( ideal < pos ? pos : ideal );
}

// SortSelectedThreads
// selected : set of SCHDTP`ThreadId
// objrefs : set of [SEM`OBJ_Ref]
// cpu : [AS`Name]
// time : nat
// prio : [nat]
// ==> seq of SCHDTP`ThreadId
SEQ<TYPE_SCHDTP_ThreadId> SCHD::SortSelectedThreads (const SET<TYPE_SCHDTP_ThreadId> & selected,
                                                     const Set & objrefs,
                                                     const Generic & cpu,
                                                     int time,
                                                     const Generic & prio)
{
  Generic cur (CurThreadId());
  Generic curcpu (theSystem().GetCurCPU());   // [AS`Name]
  SEQ<TYPE_SCHDTP_ThreadId> threadseq2; // seq of SCHDTP`ThreadId
  SEQ<TYPE_SCHDTP_ThreadId> threadseq1; // seq of SCHDTP`ThreadId
  if( !cur.IsNil() )
  {
    SET<TYPE_SCHDTP_ThreadId> selected_q (selected);
    Generic id;
    for(bool bb = selected_q.First(id); bb; bb = selected_q.Next(id))
    {
      if( this->cpualloc[id] != curcpu ) continue;
      if( Int(id).GetValue() > Int(cur).GetValue() )
        threadseq2.ImpAppend(id);
      else
        threadseq1.ImpAppend(id);
    }
  }

  Set perobjs;                     // set of [SEM`OBJ_Ref]
  Set objrefs_q (objrefs);
  Generic objref;
  for(bool bb = objrefs_q.First(objref); bb; bb = objrefs_q.Next(objref))
  {
    //  Map perthreads; // map [AS`Name] to (map SEM`OBJ_Ref to SCHDTP`PerThreadInfo)
    Map orpt (this->perthreads[cpu]);
    //TYPE_SCHDTP_PerThreadInfo ptinfo(orpt[objref]);
    Record ptinfo(orpt[objref]);
    Int priority (ptinfo.Is(TAG_TYPE_SCHDTP_PerThreadInfo) ? ptinfo.GetInt(pos_SCHDTP_PerThreadInfo_priority)
                                                           : ptinfo.GetInt(pos_SCHDTP_SpoThreadInfo_priority));
    if ( ( time >= TimePerThread(ptinfo) ) && (prio.IsNil() || ( priority == prio ) ) )
      perobjs.Insert(objref);
  }

  if (!perobjs.IsEmpty())
  {
    Map orpt (this->perthreads[cpu]);
    Generic obj_ref (perobjs.GetElem());
    Generic o_g;
    for(bool bb = perobjs.First(o_g); bb; bb = perobjs.Next(o_g))
    {
      if( TimePerThread(orpt[obj_ref]) > TimePerThread(orpt[o_g]) )
        obj_ref = o_g;
    }

    int maxn = 0;
    Generic body = Nil();
    if (orpt[obj_ref].Is(TAG_TYPE_SCHDTP_PerThreadInfo)) {
      const TYPE_SCHDTP_PerThreadInfo & ptinfo (orpt[obj_ref]);
      int period = ptinfo.GetIntValue(pos_SCHDTP_PerThreadInfo_period);
      int jitter = ptinfo.GetIntValue(pos_SCHDTP_PerThreadInfo_jitter);
      int delay = ptinfo.GetIntValue(pos_SCHDTP_PerThreadInfo_distance);
      int offset = ptinfo.GetIntValue(pos_SCHDTP_PerThreadInfo_offset);
      int iteration = ptinfo.GetIntValue(pos_SCHDTP_PerThreadInfo_iteration);
      body = (ptinfo.GetField(pos_SCHDTP_PerThreadInfo_periodBody));
      maxn = ptinfo.GetIntValue(pos_SCHDTP_PerThreadInfo_next_umax);
      const Int & pri (ptinfo.GetInt(pos_SCHDTP_PerThreadInfo_priority));

      TYPE_SCHDTP_PerThreadInfo new_ptinfo;
      new_ptinfo.Init(Int(period),
                      Int(jitter),
                      Int(delay),
                      Int(offset),
                      Int(iteration + 1),
                      body,                                // periodBody
                      Int(time),                           // release_time
                      Int(NextMin(period, jitter, time, offset, delay, iteration)), // next_min
                      Int(NextMax(period, jitter, offset, iteration, time, delay)), // next_max
                      pri);                                // priority

      orpt.ImpModify(obj_ref, new_ptinfo);
      this->perthreads.ImpModify(cpu,orpt);
    } else {
      const TYPE_SCHDTP_SpoThreadInfo & ptinfo (orpt[obj_ref]);
      int delay = ptinfo.GetIntValue(pos_SCHDTP_SpoThreadInfo_delay);
      int bound = ptinfo.GetIntValue(pos_SCHDTP_SpoThreadInfo_bound);
      int offset = ptinfo.GetIntValue(pos_SCHDTP_SpoThreadInfo_offset);
      body = (ptinfo.GetField(pos_SCHDTP_SpoThreadInfo_periodBody));
      maxn = ptinfo.GetIntValue(pos_SCHDTP_SpoThreadInfo_next_umax);
      const Int & pri (ptinfo.GetInt(pos_SCHDTP_SpoThreadInfo_priority));

      TYPE_SCHDTP_SpoThreadInfo new_ptinfo;
      new_ptinfo.Init(Int(delay),                       // delay
                      Int(bound),                       // bound
                      Int(offset),                      // offset
                      body,                             // periodBody
                      Int(time),                        // release_time
                      Int(time + delay),                // next_min
                      Int(time + bound),                // next_max
                      pri);                             // priority

      orpt.ImpModify(obj_ref, new_ptinfo);
      this->perthreads.ImpModify(cpu,orpt);
    }

    TYPE_SCHDTP_ThreadId tid (AddNewThreadId(Bool(false), obj_ref, body, Int(prio), Bool(true), Nil()));
    if( time > maxn )
    {
      this->delayed = (Int(time - maxn));
      this->killed.ImpModify(curcpu, Bool(false));
    }

    SEQ<TYPE_SCHDTP_ThreadId> res; // seq of SCHDTP`ThreadId
    res.ImpConc(threadseq2);
    res.ImpAppend(tid);
    res.ImpConc(threadseq1);

    return res;
  }
  else
  {
    SEQ<TYPE_SCHDTP_ThreadId> res; // seq of SCHDTP`ThreadId
    res.ImpConc(threadseq2);
    res.ImpConc(threadseq1);
    return res;
  }
}
#else
// SortSelectedThreads
// selected : set of SCHDTP`ThreadId
// ==> seq of SCHDTP`ThreadId
SEQ<TYPE_SCHDTP_ThreadId> SCHD::SortSelectedThreads (const SET<TYPE_SCHDTP_ThreadId> & selected_)
{
  SET<TYPE_SCHDTP_ThreadId> selected (selected_); // for safe
  SEQ<TYPE_SCHDTP_ThreadId> res, seq1, seq2;

  TYPE_SCHDTP_ThreadId curtid (CurThreadId());
  Generic g;
  for (bool bb = selected.First(g); bb; bb = selected.Next(g))
  {
    TYPE_SCHDTP_ThreadId id (g);

    if (id > curtid) {
      seq2.ImpAppend(id);
    } else {
      seq1.ImpAppend(id);
    }
  }
  res.ImpConc(seq2).ImpConc(seq1);
  return res;
}
#endif // VICE

#ifdef VICE
// PeriodicReady
// cpunm : [AS`Name]
// objrefs : set of SEM`OBJ_Ref
// ==> bool
bool SCHD::PeriodicReady(const Generic & cpunm, const Set & objrefs_)
{
  if( this->perthreads.DomExists(cpunm) )
  {
    Set objrefs (objrefs_); // for safe
    int time = theSystem().GetTime();
    Map tm (this->perthreads[cpunm]);
    bool exists = false;
    Generic objref;
    for(bool bb = objrefs.First(objref); bb && !exists; bb = objrefs.Next(objref))
    {
      if( tm.DomExists(objref) )
      {
        exists = ( time >= TimePerThread(tm[objref]) );
      }
    }
    return exists;
  }
  return false;
}
#endif // VICE


// StartNewThread
// objref : SEM`OBJ_Ref
void SCHD::StartNewThread(const TYPE_SEM_OBJ_uRef & objref)
{
  const TYPE_AS_Name & clnm (objref.GetRecord(pos_SEM_OBJ_uRef_tp));
  Generic thread (theState().LookUpThread(clnm));
  if (thread.IsNil()) {
    RTERR::Error(L"StartNewThread", RTERR_NO_THREAD, Nil(), Nil(), Sequence());
  }
  else {
    switch (Record(thread).GetTag()) {
#ifdef VICE
      case TAG_TYPE_STKM_ProcThread: {
        TYPE_STKM_ProcThread t (thread);
        const TYPE_STKM_SubProgram & instr (t.GetSequence(pos_STKM_ProcThread_instr));
        TYPE_SCHDTP_ThreadId threadid (AddNewThreadId(Bool(false), objref, instr, SCHD::Default_priority, false, Nil()));
        theState().SetObjectThreadId(objref, threadid);
        SetThreadPriority(threadid, clnm);
        SetThreadStatus(threadid, TYPE_SCHDTP_Sleeping());
        break;
      }
      case TAG_TYPE_STKM_PerThread: {
        SetPerThread(objref, thread, SCHD::Default_priority);
        break;
      }
      case TAG_TYPE_STKM_SpoThread: {
        SetSpoThread(objref, thread, SCHD::Default_priority);
        break;
      }
#else
      case TAG_TYPE_STKM_ProcThread: {
        TYPE_STKM_ProcThread t (thread);
        const TYPE_STKM_SubProgram & instr (t.GetSequence(1));
        TYPE_SCHDTP_ThreadId threadid (AddNewThreadId(Bool(false), objref, instr));
        theState().SetObjectThreadId(objref, threadid);
        SetThreadPriority(threadid, clnm);
        SetThreadStatus(threadid, TYPE_SCHDTP_Sleeping());
        break;
      }
#endif // VICE
    }
  }
}

// StopThread
// objref : SEM`OBJ_Ref
void SCHD::StopThread(const TYPE_SEM_OBJ_uRef & objref)
{
  SET<TYPE_SCHDTP_ThreadId> dom_threads (this->threads.Dom());
  SET<TYPE_SCHDTP_ThreadId> tostop;
  bool found = false;
  Generic tid;
  for (bool bb = dom_threads.First(tid); bb; bb = dom_threads.Next(tid))
  {
    if (!IsDebugThread(tid)) {
      TYPE_SCHDTP_ThreadInfo threadinfo (this->threads[tid]);
      if (threadinfo.GetField(pos_SCHDTP_ThreadInfo_obj) == objref) {
#ifdef VICE
        if (this->cpualloc.DomExists(tid)) {
          if (this->cpualloc[tid] == theSystem().GetCurCPU()) {
            found = true;
            KillThread(tid); 
          }
        }
#else
        found = true;
        KillThread(tid); 
#endif // VISE
      } 
    } 
  }
#ifdef VICE
  Set dom_perthreads (this->perthreads.Dom());
  Generic cpu;
  bool perfound = false;
  for (bool bb = dom_perthreads.First(cpu); bb && !perfound; bb = dom_perthreads.Next(cpu)) {
    Map pth_m (this->perthreads[cpu]);
    if (pth_m.DomExists(objref)) {
      perfound = true;
      Map new_pth_m (pth_m.RemElem(objref));
      this->perthreads.ImpModify(cpu, new_pth_m);
    }
  }
  found |= perfound;
#endif // VICE
  if (!found)
    RTERR::Error(L"StopThread", RTERR_NO_RUNNING_THREAD, Nil(), Nil(), Sequence());
}

// SetThreadPriority
// threadid : SCHDTP`ThreadId
// clnm : AS`Name
void SCHD::SetThreadPriority(const TYPE_SCHDTP_ThreadId & threadid, const TYPE_AS_Name & clnm)
{
  TYPE_AS_Id id (ASTAUX::GetFirstId(clnm));
  Int priority (this->classPriorityMap.DomExists(id) ? Int(this->classPriorityMap[id])
                                                     : SCHD::Default_priority);

  TYPE_SCHDTP_ThreadInfo threadinfo (this->threads[threadid]);
  threadinfo.set_priority(priority);
  this->threads.ImpModify(threadid, threadinfo);
}

#ifdef VICE
// GetThreadPriority
// ==> nat
Int SCHD::GetThreadPriority() const
{
  Generic cpu (theSystem().GetCurCPU());
  Generic tid;
  if( !this->curthread_m.DomExists(cpu, tid) )
    RTERR::Error(L"GetThreadPriority", RTERR_INTERNAL_ERROR, Nil(), Nil(), Sequence());
  Generic g;
  if( !this->threads.DomExists(tid, g) )
    RTERR::Error(L"GetThreadPriority", RTERR_INTERNAL_ERROR, Nil(), Nil(), Sequence());
  TYPE_SCHDTP_ThreadInfo tinfo(g);
  return tinfo.get_priority();
}
#endif // VICE

// SetMaxPriority
// threadid : SCHDTP`ThreadId
void SCHD::SetMaxPriority(const TYPE_SCHDTP_ThreadId & threadid)
{
  Generic g;
  if( this->threads.DomExists(threadid, g) )
  {
    TYPE_SCHDTP_ThreadInfo threadinfo(g);
    threadinfo.set_priority(this->maxPriority);
    this->threads.ImpModify(threadid, threadinfo);
  }
  else
  {
    EvaluatorStatusCt ev (theStackMachine().InitEvaluatorStatus(TYPE_STKM_SubProgram(), Nil()));
    TYPE_SCHDTP_ThreadInfo threadinfo;

#ifdef VICE
    threadinfo.Init(Bool(false), Nil(), TYPE_SCHDTP_Sleeping(), ev, this->maxPriority, Nil(), Bool(false), Nil());
#else
    threadinfo.Init(Bool(false), Nil(), TYPE_SCHDTP_Sleeping(), ev, this->maxPriority);
#endif //VICE
    this->threads.ImpModify(threadid, threadinfo);
  }
}

// KillThread
// threadid : SCHDTP`ThreadId
// ==> ()
void SCHD::KillThread(const TYPE_SCHDTP_ThreadId & threadid)
{
  this->threads.RemElem(threadid);
#ifdef VICE
  this->cpualloc.RemElem(threadid);
//  theSystem().KillThreadId(threadid);
  theSystem().DeleteThread(threadid);
  this->killed.ImpModify(theSystem().GetCurCPU(), Bool(true));
  TIMETRACE::LogKillThread(threadid, theSystem().GetCurCPU());
#endif //VICE
}

#ifdef VICE
// CurThreadId
// ==> [SCHDTP`ThreadId]
Generic SCHD::CurThreadId() const
{
  Generic cpu (theSystem().GetCurCPU());  // [As`Name]
  Generic thr (this->curthread_m.DomExists(cpu) ? this->curthread_m[cpu] : (Generic)TYPE_SCHDTP_ThreadId(-1));

  if( thr.IsNil() )
    return this->tobekilled[cpu];
  else if( TYPE_SCHDTP_ThreadId(-1) == thr )
    return Nil();
  else
    return thr;
}
#else
// CurThreadId
// SCHDTP`ThreadId
TYPE_SCHDTP_ThreadId SCHD::CurThreadId() const
{
  return this->curthread_m[Nil()];
}
#endif //VICE

// GiveAllThreads
// ==> SCHDTP`Threads
MAP<TYPE_SCHDTP_ThreadId,TYPE_SCHDTP_ThreadInfo> SCHD::GiveAllThreads() const
{
  return this->threads;
}

// SelThread
// selid : SCHDTP`ThreadId
void SCHD::SelThread(const TYPE_SCHDTP_ThreadId & selid)
{
  if (! this->threads.DomExists(selid)) {
    RTERR::Error(L"SelThread", RTERR_SEL_NONVALID_THREADID, NilContextId, Nil(), Sequence());
  }

  Generic cur = CurThreadId();
#ifdef VICE
  if( !cur.IsNil() )
#endif // VICE
  {
    SetThreadState(cur);
    SetThreadStatus(cur, TYPE_SCHDTP_Sleeping());

    SetCurThread(selid, true);

//    RestoreSTKM(cur);
//    SetThreadStatus(cur, TYPE_SCHDTP_Running());
    RestoreSTKM(selid);
    SetThreadStatus(selid, TYPE_SCHDTP_Running());
  }
}

// Deschedule
// threadstate : STKM`EvaluatorStatus
// ==> bool
bool SCHD::Deschedule(const EvaluatorStatus & threadstate) const
{
  if (CheckingGuard()) {
    return false;
  }
  else {
    return EndOfSliceReached(threadstate);
  }
}

// IncInstrnum
// STKM`EvaluatorStatus
// ==> STKM`EvaluatorStatus
void SCHD::IncInstrnum(EvaluatorStatus & threadstate) const
{
//  TYPE_SCHDTP_PrimarySchedulerAlgorithm pa (Settings.GetPrimaryAlgorithm ());
  // Note that this is different from the specification as
  // we are able to work with references here
//  if (!CheckingGuard() && pa.Is(TAG_TYPE_SCHDTP_InstrnumSlice))
  if (!CheckingGuard() && Settings.GetPrimaryAlgorithm ().Is(TAG_TYPE_SCHDTP_InstrnumSlice)) {
    threadstate.instrno++;
  }
}

// EndOfSliceReached
// tstate : STKM`EvaluatorStatus
// ==> bool
bool SCHD::EndOfSliceReached(const EvaluatorStatus & threadstate) const
{
#ifdef VICE
  TYPE_SCHDTP_SecondarySchedulerAlgorithm ssa = this->secondaryAlgorithm;

  Generic cpu (theSystem().GetCurCPU());

  if (!cpu.IsNil() || !theSystem().MultiThreads(cpu)) {
//    ssa = theSystem().GetSecondaryAlgorithm(cpu);
    ssa = TYPE_SCHDTP_RoundRobin();
  }

  if( ssa.Is(TAG_TYPE_SCHDTP_PriorityBased) ) {
//    Generic cpu (theSystem().GetCurCPU());
    TYPE_SCHDTP_ThreadId tid (this->curthread_m[cpu]);
    TYPE_SCHDTP_ThreadInfo tinfo(this->threads[tid]);
    int priority = tinfo.GetIntValue(pos_SCHDTP_ThreadInfo_priority);
    Set potentialperchanged_q (this->potentialperchanged);
    Generic objref;
    for(bool bb = potentialperchanged_q.First(objref); bb; bb = potentialperchanged_q.Next(objref))
    {
      if(ThreadBlockedOnObjRefExists(objref, this->threads))
      {
        if( priority < ObjRefPriority(objref) )
          return true;
      }
    }
  }
#endif // VICE
  TYPE_SCHDTP_PrimarySchedulerAlgorithm pa (Settings.GetPrimaryAlgorithm ());
  switch(pa.GetTag()){
    case TAG_TYPE_SCHDTP_PureCooperative: {
      return false;
      break;
    }
#ifdef VICE
    case TAG_TYPE_SCHDTP_TimeSlice: {
//wcout << L"cpu: " << cpu << endl;
      return ((theSystem().GetTime() - Int(threadstate.release_time).GetValue()) >
              Settings.GetTimeSlice());
      break;
    }
#endif //VICE
    case TAG_TYPE_SCHDTP_InstrnumSlice: {
      return threadstate.instrno >= Int(Settings.GetMaxInstr());
    }
  }
  return false; // dummy
}

// InitSlice
// thread_state : STKM`EvaluatorStatus
// ==> STKM`EvaluatorStatus
EvaluatorStatusCt SCHD::InitSlice(const EvaluatorStatusCt & threadstate) const
{
  EvaluatorStatusCt localstateCt (threadstate);
  EvaluatorStatus & localstate = localstateCt.get_shared_ref();

  // Note that this is different from the specification as
  // we are able to work with references here
  TYPE_SCHDTP_PrimarySchedulerAlgorithm algorithm (Settings.GetPrimaryAlgorithm ());
  switch(algorithm.GetTag()){
#ifdef VICE
    case TAG_TYPE_SCHDTP_TimeSlice: {
      localstate.release_time = Int(theSystem().GetTime());
      break;
    }
#endif //VICE
    case TAG_TYPE_SCHDTP_InstrnumSlice: {
      localstate.instrno = 0;
      break;
    }
    default:
      break;
  }
  return localstateCt;
}

// CheckingGuard
// ==> bool
bool SCHD::CheckingGuard() const
{
  return this->checkingGuard.GetValue();
}

// SetCheckingGuard
// b : bool
void SCHD::SetCheckingGuard(bool b)
{
  this->checkingGuard = Bool(b);
}

// RestoreSTKM
// threadid : SCHDTP`ThreadId
void SCHD::RestoreSTKM(const TYPE_SCHDTP_ThreadId & threadid)
{
  if( !this->threads.DomExists(threadid) )
    RTERR::Error(L"RestoreSTKM", RTERR_INTERNAL_ERROR, Nil(), Nil(), Sequence());

  theSystem().SetCurThread(threadid);
  TYPE_SCHDTP_ThreadInfo threadinfo (this->threads[threadid]);

//  theStackMachine().Instantiate( threadinfo.get_stackeval(), TYPE_STKM_SubProgram());
  theStackMachine().Instantiate( threadinfo.get_stackeval(), Nil());
}

#ifdef VICE
// SwapOut
// kill : bool
// ==> ()
void SCHD::SwapOut(const Generic & cur, const Generic & cpu, bool kill, bool tobelogged)
{
//  Generic cpu (theSystem().GetCurCPU());
//  Generic cur (CurThreadId());
  if( this->curthread_m.DomExists(cpu) && this->threads.DomExists(cur) )
  {
    Tuple gorac (GetObjRefAndClass(cur));
    const Generic & org_obj_ref (gorac.GetField(1));
    const Generic & org_cl (gorac.GetField(2));
    if(tobelogged)
      TIMETRACE::LogThreadSwapOut(cur, org_obj_ref, org_cl, Settings.GetTaskSwitch());

    if( kill )
//    if( kill && !IsDebugThread(cur) )
    {
      this->tobekilled.ImpModify(cpu, cur);
      this->curthread_m.ImpModify(cpu, Nil());
    }

    int ts = Settings.GetTaskSwitch();
    if ( ts > 0 )
    {
      theSystem().IncrAbsTime(Int(ts));
    }
    this->lastswapedout.ImpModify(cpu, cur);
  }
  else {
/*
wcout << L"*****************************************************" << endl;
if( !this->curthread_m.DomExists(cpu) )
wcout << L"thread id: " << cur << L" (" << theSystem().PrintCPU(cpu) << L")" << L" !this->curthread_m.DomExists(cpu)" << endl;
if( !this->threads.DomExists(cur) )
wcout << L"thread id: " << cur << L" (" << theSystem().PrintCPU(cpu) << L")" << L" !this->threads.DomExists(cur)" << endl;
*/
  }
}

// RestoreInstrAndSTKM
// threadid : SCHDTP`ThreadId
// ==> ()
void SCHD::RestoreInstrAndSTKM(const TYPE_SCHDTP_ThreadId & threadid)
{
  TYPE_SCHDTP_ThreadInfo threadinfo (this->threads[threadid]);
  EvaluatorStatusCt threadstate (threadinfo.get_stackeval());
  EvaluatorStatus & es = threadstate.get_shared_ref();

  const Generic & instr (threadinfo.GetField(pos_SCHDTP_ThreadInfo_periodBody));

  if (es.call_stack.IsEmpty())
  {
    TYPE_STKM_CallStackItem csi;
    csi.Init(CallStackItemType::CS_INTERNAL, // CSItemType
             instr,                          // Code
             Int(0),                         // ProgramCounter
             Sequence(L"Thread Start"),      // AS`Name | seq of char
             Nil(),                          // [seq of SEM`VAL]
             Nil(),                          // [EvaluatorStatus]
             es.curCid,                      // CI`ContextId
             Int(es.env_l.Length()),         // [nat]
             Int(es.typeinst.Length()),      // [nat]
             Int(es.os.Length()),            // [nat]
             Int(es.obj_l.Length()), // [nat]
             Bool(false),
             Nil());

    es.call_stack.Push(csi);

    threadinfo.set_stackeval(threadstate);
    this->threads.ImpModify(threadid, threadinfo);
  }
  
  theSystem().SetCurThread(threadid);
  theStackMachine().Instantiate(threadstate, instr);
}

// UpdateThreadWithReturnResult
// val : SEM`VAL
// fromthread : SCHDTP`ThreadId
// ==> ()
void SCHD::UpdateThreadWithReturnResult(const TYPE_SEM_VAL & val, const TYPE_SCHDTP_ThreadId & fromthread)
{
  TYPE_SCHDTP_ThreadInfo threadinfo (this->threads[fromthread]);
  EvaluatorStatusCt stack_eval (threadinfo.get_stackeval());
  EvaluatorStatus & state = stack_eval.get_oneref_ref(); // must oneref
  state.eval_stack.Pop();
  state.eval_stack.Pop();
  state.pc += 3;
  threadinfo.set_stackeval(stack_eval);
  this->threads.ImpModify(fromthread, threadinfo);
}

// AddValToEvalStack
// threadid : SCHDTP`ThreadId
// val : SEM`VAL
// ==> ()
void SCHD::AddValToEvalStack(const TYPE_SCHDTP_ThreadId & threadid, const TYPE_SEM_VAL & val)
{
  TYPE_SCHDTP_ThreadInfo threadinfo (this->threads[threadid]);
  EvaluatorStatusCt stack_eval (threadinfo.get_stackeval());
  EvaluatorStatus & state = stack_eval.get_oneref_ref(); // must oneref
  if(!state.call_stack.IsEmpty())
  {
    state.eval_stack.Push(val);
    threadinfo.set_stackeval(stack_eval);
    this->threads.ImpModify(threadid, threadinfo);
  }
}
#endif //VICE

// IsDebugThread
// threadid : SCHDTP`ThreadId
// ==>  bool
bool SCHD::IsDebugThread(const TYPE_SCHDTP_ThreadId & threadid)
{
  if( this->threads.DomExists(threadid) )
  {
    TYPE_SCHDTP_ThreadInfo threadinfo (this->threads[threadid]);
    return threadinfo.GetBoolValue(pos_SCHDTP_ThreadInfo_debug_uthread);
  }
  return false;
}

// AddNewThreadId
#ifdef VICE
// debug_thread : bool
// objref : [SEM`OBJ_Ref]
// instr : STKM`SubProgram
// prio : nat
// peri : bool
// mesid : [STKM`MessageId]
// ==> SCHDTP`ThreadId
TYPE_SCHDTP_ThreadId SCHD::AddNewThreadId(const Bool & debug_thread,
                                          const Generic & objref,
                                          const TYPE_STKM_SubProgram & instr,
                                          const Int & prio,
                                          const Bool & peri,
                                          const Generic & mesid)
{
  TYPE_SCHDTP_ThreadId threadid (GetNextThreadId());
  Generic clnm = Nil();
  if( !objref.IsNil() )
    clnm = Record(objref).GetRecord(pos_SEM_OBJ_uRef_tp);
  EvaluatorStatusCt ev (theStackMachine().InitEvaluatorStatus(instr, objref));
  TYPE_SCHDTP_ThreadInfo threadinfo;
  threadinfo.Init(debug_thread, objref, TYPE_SCHDTP_Sleeping(), ev, Int(prio), instr, peri, mesid);
  this->threads.ImpModify(threadid, threadinfo);
  theSystem().AddThreadIdtoCPU(threadid, objref);

  Generic cpu (theSystem().OnCurCPU(objref) ? theSystem().GetCurCPU() : theSystem().CPUAllocated(objref));
  this->cpualloc.ImpModify(threadid, cpu);

  TIMETRACE::LogThreadCreate(threadid, peri, objref, clnm, cpu);
  theSystem().SetCPUTime(cpu, theSystem().GetCPUTime(cpu));
  return threadid;
}
#else
// debug_thread : bool
// objref : [SEM`OBJ_Ref]
// instr : STKM`SubProgram
TYPE_SCHDTP_ThreadId SCHD::AddNewThreadId(const Bool & debug_thread,
                                          const Generic & objref,
                                          const TYPE_STKM_SubProgram & instr)
{
  TYPE_SCHDTP_ThreadId threadid (GetNextThreadId());
  EvaluatorStatusCt ev (theStackMachine().InitEvaluatorStatus(instr, objref));
  TYPE_SCHDTP_ThreadInfo threadinfo;
  threadinfo.Init(debug_thread, objref, TYPE_SCHDTP_Sleeping(), ev, SCHD::Default_priority);
  this->threads.ImpModify(threadid, threadinfo);
  return threadid;
}
#endif // VICE

#ifdef VICE
// GetCPUOfThread
// threadid : SCHDTP`ThreadId
// ==> [AS`Name]
Generic SCHD::GetCPUOfThread(const TYPE_SCHDTP_ThreadId & threadid)
{
  if(this->cpualloc.DomExists(threadid))
    return this->cpualloc[threadid];
  else
    // TODO
    vdm_iplog << L"thread: " << threadid << L" is't found." << endl << flush;
  return Nil();
}
#endif // VICE

// SetThreadStatus
// threadid : SCHDTP`ThreadId
// tstatus : SCHDTP`ThreadStatus
void SCHD::SetThreadStatus(const TYPE_SCHDTP_ThreadId & threadid, const TYPE_SCHDTP_ThreadStatus & tstatus)
{
//wcout << L"SetThreadStatus: " << threadid << L" " << tstatus << endl;
//wcout << this->cpusblocked << endl;
  if(this->threads.DomExists(threadid))
  {
    TYPE_SCHDTP_ThreadInfo threadinfo (this->threads[threadid]);
    threadinfo.set_status(tstatus);
#ifdef VICE
    EvaluatorStatusCt stack_eval (threadinfo.get_stackeval());
//    stack_eval.get_oneref_ref().status = tstatus;
    stack_eval.get_shared_ref().status = tstatus;
    threadinfo.set_stackeval(stack_eval);
#endif // VICE
    this->threads.ImpModify(threadid, threadinfo);
  }
}

// GetThreadStatus
// threadid : SCHDTP`ThreadId
// ==> SCHDTP`ThreadStatus
TYPE_SCHDTP_ThreadStatus SCHD::GetThreadStatus(const TYPE_SCHDTP_ThreadId & threadid)
{
  if(this->threads.DomExists(threadid))
  {
    TYPE_SCHDTP_ThreadInfo threadinfo (this->threads[threadid]);
    return threadinfo.GetRecord(pos_SCHDTP_ThreadInfo_status);
  }
  RTERR::Error(L"GetThreadStatus", RTERR_INTERNAL_ERROR, Nil(), Nil(), Sequence());
  return TYPE_SCHDTP_Sleeping();
}

void SCHD::SetCurThreadState(const EvaluatorStatusCt & state)
{
  TYPE_SCHDTP_ThreadInfo threadinfo (this->threads[CurThreadId()]);
  threadinfo.set_stackeval(state);
  this->threads.ImpModify(CurThreadId(), threadinfo);
}

// SetThreadState
// threadid : SCHDTP`ThreadId
void SCHD::SetThreadState(const TYPE_SCHDTP_ThreadId & threadid)
{
  if(this->threads.DomExists(threadid))
  {
    TYPE_SCHDTP_ThreadInfo threadinfo (this->threads[threadid]);
    threadinfo.set_stackeval(theStackMachine().GetEvaluatorState());
    this->threads.ImpModify(threadid, threadinfo);
  }
  else
  {
    RTERR::Error(L"SetThreadState", RTERR_INTERNAL_ERROR, Nil(), Nil(), Sequence());
  }
}

// GetNextThreadId
// ==> SCHDTP`ThreadId
TYPE_SCHDTP_ThreadId SCHD::GetNextThreadId()
{
  TYPE_SCHDTP_ThreadId the_id = this->next_thread_id;
  this->next_thread_id = this->next_thread_id + Int(1);

  return the_id;
}

// SetCurThread
// id : SCHDTP`ThreadId
// log : bool
void SCHD::SetCurThread(const TYPE_SCHDTP_ThreadId & tid, bool log)
{
#ifdef VICE
  Generic curcpu = theSystem().GetCurCPU();
#else
  Generic curcpu = Nil();
#endif // VICE

  int id = tid.GetValue();
#ifdef VICE
  if( this->curthread_m.DomExists(curcpu) ||
      (this->threads.DomExists(tid) &&
       Record(this->threads[tid]).GetBoolValue(pos_SCHDTP_ThreadInfo_periodic)) ||
      ( id == 1 && this->curthread_m.IsEmpty() )) {
    Tuple org_objcl (GetObjRefAndClass(id));
    const Generic & org_obj_ref (org_objcl.GetField(1));
    const Generic & org_cl (org_objcl.GetField(2));

    if( log && ( id != 0 ) && ( !this->lastswapedin.DomExists(curcpu) || tid != this->lastswapedin[curcpu])) {
      int ts = Settings.GetTaskSwitch();
      if( this->curthread_m.DomExists(curcpu) && !this->curthread_m[curcpu].IsNil() ) {
  Generic cpu (theSystem().GetCurCPU());
  Generic cur (CurThreadId());
        //SwapOut(false,true);
        SwapOut(cur, cpu, false,true);
      }

      if( this->delayed.IsNil() ) {
        TIMETRACE::LogThreadSwapIn(id, org_obj_ref, org_cl, ts);
      }
      else {
        TIMETRACE::LogDelayedThreadSwapIn(id, org_obj_ref, org_cl, this->delayed, ts);
        this->delayed = Nil();
      }
      this->killed.ImpModify(curcpu, Bool(false));
      this->lastswapedin.ImpModify(curcpu, tid);
    }
  }
#endif // VICE
  if( id != 0 )
  {
    this->curthread_m.ImpModify(curcpu, tid);
    theSystem().SetCurThread(tid);
  }
}

//#ifdef VICE
// SaveCurThreadState
// evalstate : STKM`EvaluationState
// ==> ()
void SCHD::SaveCurThreadState()
{
  Generic cur (CurThreadId());
  if( this->threads.DomExists(cur) ) {
    TYPE_SCHDTP_ThreadInfo threadinfo (this->threads[cur]);
    EvaluatorStatusCt es (theStackMachine().GetEvaluatorState());
#ifdef VICE
    EvaluatorStatusCt es2 (threadinfo.get_stackeval());
    es.get_shared_ref().status = es2.get_const_ref().status;
#endif // VICE
    threadinfo.set_stackeval(es);
    this->threads.ImpModify(cur, threadinfo);
  }
}

#ifdef VICE
// GetLastSwapedIn
// cpuid : [AS`Name]
// ==> [SCHDTP`ThreadId]
Generic SCHD::GetLastSwapedIn(const Generic & cpuid) const
{
  if(this->lastswapedin.DomExists(cpuid))
    return this->lastswapedin[cpuid];
  else
    return Nil();
}

// SetLastSwapedIn
// tid : [SCHDTP`ThreadId]
// cpuid : [AS`Name]
// ==> ()
void SCHD::SetLastSwapedIn(const Generic & tid, const Generic & cpuid)
{
  this->lastswapedin.ImpModify(cpuid, tid);
  this->killed.ImpModify(cpuid, Bool(false));
}

// ASyncOpThread
// SCHDTP`ThreadId
// ==> bool
bool SCHD::ASyncOpThread(const TYPE_SCHDTP_ThreadId & curthread) const
{
  TYPE_SCHDTP_ThreadInfo threadinfo (this->threads[curthread]);
  EvaluatorStatusCt ev (threadinfo.get_stackeval());
  if (ev.get_const_ref().call_stack.Length() > 0) {
    TYPE_STKM_CallStackItem csi (ev.get_const_ref().call_stack.Head());
    if (csi.get_type() == CallStackItemType::CS_DEBUGCMD) {
      const TYPE_STKM_SubProgram & debug_cmd (csi.GetSequence(pos_STKM_CallStackItem_code));
      const TYPE_INSTRTP_Instruction & instr (debug_cmd[1]);
      if(instr.Is(TAG_TYPE_INSTRTP_PUSH)) {
        const Generic & val (instr.GetField(pos_INSTRTP_PUSH_val));
        if( val.Is(TAG_TYPE_SEM_ExplOP)) {
          TYPE_SEM_ExplOP eop (val);
          return (!eop.GetBoolValue(pos_SEM_ExplOP_sync));
        }
      }
    }
  }
  return false;
}

// BlockedObjRef
// obj_ref : SEM`OBJ_Ref
// ==> set of AS`Name
SET<TYPE_AS_Name> SCHD::BlockedObjRef(const TYPE_SEM_OBJ_uRef & obj_ref)
{
  Set dom_threads (this->threads.Dom());
  SET<TYPE_AS_Name> res;
  Generic tid;
  for(bool bb = dom_threads.First(tid); bb; bb = dom_threads.Next(tid)) {
    Generic cpu;
    if( (!this->cpualloc.DomExists(tid, cpu)) ||
        ( cpu != theSystem().GetCurCPU() ) ) continue;

    TYPE_SCHDTP_ThreadInfo thr (this->threads[tid]);
    const TYPE_SCHDTP_ThreadStatus & status (thr.GetRecord(pos_SCHDTP_ThreadInfo_status));
    if (status.Is(TAG_TYPE_SCHDTP_Blocked)) {
      const Generic & objref (status.GetField(pos_SCHDTP_Blocked_objref));
      if((objref == obj_ref) ||
         (objref.IsNil() && (thr.GetField(pos_SCHDTP_ThreadInfo_obj) == obj_ref))) {
        res.Insert(status.GetRecord(pos_SCHDTP_Blocked_opnm));
      }
    }
  }
  return res;
}

// PotentialPerChange
// obj_ref : SEM`OBJ_Ref
// ==> ()
void SCHD::PotentialPerChange(const TYPE_SEM_OBJ_uRef & obj_ref)
{
  this->potentialperchanged.Insert(obj_ref);
}

// ObjRefPriority
// obj_ref : SEM`OBJ_Ref
//  ==> nat
Int SCHD::ObjRefPriority(const TYPE_SEM_OBJ_uRef & obj_ref) const
{
  if ( ThreadBlockedOnObjRefExists(obj_ref, this->threads) ) {
    Set dom_threads (this->threads.Dom());
    Generic tid;
    for (bool bb = dom_threads.First(tid); bb; bb = dom_threads.Next(tid)) {
      TYPE_SCHDTP_ThreadInfo thr (this->threads[tid]);
      if ( thr.GetField(pos_SCHDTP_ThreadInfo_obj) == obj_ref ) {
        return thr.GetInt(pos_SCHDTP_ThreadInfo_priority);
      }
      else {
        const TYPE_SCHDTP_ThreadStatus & status (thr.GetRecord(pos_SCHDTP_ThreadInfo_status));
        if (status.Is(TAG_TYPE_SCHDTP_Blocked)) {
          if ( status.GetField(pos_SCHDTP_Blocked_objref) == obj_ref ) {
            return thr.GetInt(pos_SCHDTP_ThreadInfo_priority);
          }
        }
      }
    }
  }
  return Default_priority; // dummy
}

// ThreadBlockedOnObjRefExists
// obj_ref : SEM`OBJ_Ref
// t_m : SCHDTP`Threads
// -> bool
bool SCHD::ThreadBlockedOnObjRefExists(const TYPE_SEM_OBJ_uRef & obj_ref,
                                       const MAP<TYPE_SCHDTP_ThreadId,TYPE_SCHDTP_ThreadInfo> & t_m) const
{
  Set dom_t_m (t_m.Dom());
  bool exists = false;
  Generic tid;
  for (bool bb = dom_t_m.First(tid); bb && !exists; bb = dom_t_m.Next(tid)) {
    TYPE_SCHDTP_ThreadInfo thr (t_m[tid]);
    if ( thr.GetField(pos_SCHDTP_ThreadInfo_obj) == obj_ref ) {
      exists = true;
    }
    else {
      const TYPE_SCHDTP_ThreadStatus & status (thr.GetRecord(pos_SCHDTP_ThreadInfo_status));
      if ( status.Is(TAG_TYPE_SCHDTP_Blocked) ) {
        if ( status.GetField(pos_SCHDTP_Blocked_objref) == obj_ref ) {
          exists = true;
        }
      }
    }
  }
  return exists;
}
#endif //VICE

// WellFormedPriorityFile
// pf : SCHDTP`PriorityFile
// -> bool
bool SCHD::WellFormedPriorityFile (const SEQ<TYPE_SCHDTP_PriorityEntry> & pf)
{
  SET<TYPE_AS_Id> clnms;
  size_t len_pf = pf.Length();
  for (size_t index = 1; index <= len_pf; index++) {
    const TYPE_SCHDTP_PriorityEntry & pe (pf[index]);
    clnms.Insert(pe.GetSequence(pos_SCHDTP_PriorityEntry_clnm));
  }
  return ((size_t)(clnms.Card()) == len_pf);
}

// MakePriorityMap
// pf : SCHDTP`PriorityFile
// -> map AS`Id to nat
MAP<TYPE_AS_Id,Int> SCHD::MakePriorityMap(const SEQ<TYPE_SCHDTP_PriorityEntry> & pf)
{
  MAP<TYPE_AS_Id,Int> pm;
  size_t len_pf = pf.Length();
  for (size_t index = 1; index <= len_pf; index++) {
    const TYPE_SCHDTP_PriorityEntry & pe (pf[index]);
    pm.Insert(pe.GetSequence(pos_SCHDTP_PriorityEntry_clnm),
              pe.GetInt(pos_SCHDTP_PriorityEntry_priority));
  }
  return pm;
}

// GetPriorityMap
// ==> map AS`Id to nat
MAP<TYPE_AS_Id,Int> SCHD::GetPriorityMap()
{
  return this->classPriorityMap;
}

// SetPriorityMap
// pm : map AS`Id to nat
void SCHD::SetPriorityMap(const MAP<TYPE_AS_Id,Int> & pm)
{
  this->classPriorityMap.Clear();
  this->classPriorityMap = pm;
  SET<Int> pmRng (pm.Rng());
  Int maxP (SCHD::Default_priority);

  Generic pr;
  for (bool bb = pmRng.First(pr); bb; bb = pmRng.Next(pr)) {
    Int priority (pr);
    if (priority > maxP)
      maxP = priority;
  }
  this->maxPriority = maxP + Int(1);

// The following is only at the implementation level, because it is included
// to deal with the case in which SetPriorityMap is called after InitScheduler -
// without this code, the priority of the debug thread would not be set to
// the maximum. This scenario can not be reproduced at the specification level.

  // Set priority of debug thread to maxPriority
  SET<TYPE_SCHDTP_ThreadId> dom_threads (this->threads.Dom());
  Generic tid;
  for (bool cc = dom_threads.First(tid); cc; cc = dom_threads.Next(tid))
  {
    if (IsDebugThread(tid)) {
      SetMaxPriority(tid);
      break;
    }
  }
}

#ifdef VICE
// RunMessageThread
// clnm : [AS`Name]
// objref : [SEM`OBJ_Ref]
// instr : STKM`SubProgram
// prio : [nat]
// mesid : STKM`MessageId
// ==> STKM`EvaluationState * [SEM`VAL]
Tuple SCHD::RunMessageThread(const Generic & clnm,
                             const Generic & objref,
                             const TYPE_STKM_SubProgram & instr,
                             const Generic & prio,
                             const TYPE_STKM_MessageId & mesid)
{
  Int p (prio.IsNil() ? SCHD::Default_priority : Int(prio));

  TYPE_SCHDTP_ThreadId tid (AddNewThreadId(Bool(false), objref, instr, p, Bool(false), mesid));

  if ( tid != GetLastSwapedIn(theSystem().GetCurCPU()) ) {
    TIMETRACE::LogThreadSwapIn(tid, objref, clnm, Settings.GetTaskSwitch());
    SetLastSwapedIn(tid,theSystem().GetCurCPU());
  }
  return RunThread(tid);
}

// RunThread
// threaid : SCHDTP`ThreadId
// ==> STKM`EvaluationState * [SEM`VAL]
Tuple SCHD::RunThread(const TYPE_SCHDTP_ThreadId & tid)
{
  SetCurThread(tid, true);
  SetThreadStatus(CurThreadId(), TYPE_SCHDTP_Running());
  RestoreInstrAndSTKM(CurThreadId());
  theSystem().ResetSlice();
  return theStackMachine().EvalMainLoop();
}

bool SCHD::IsSyncOpThread()
{
  return GetThreadStatus(CurThreadId()).Is(TAG_TYPE_SCHDTP_SyncOp);
}
#endif // VICE

void SCHD::ShowThreadInfo(const TYPE_SCHDTP_ThreadId & tid)
{
  if (this->threads.DomExists(tid)) {
    vdm_iplog << this->threads[tid] << endl;
  }
  else {
    vdm_iplog << L"thread with id: " << tid << L" is't exists."  << endl;
  }
}

