#include "metaiv.h"
#include "AS.h"
#include "stackeval.h"
#include "evalsystem.h"
#include "rt_errmsg.h"
#include "libman.h"
#include "astaux.h"
#include "evalaux.h"
#include "settings.h"
#include "tbutils.h"
#ifdef VICE
#include "TIMETRACE.h"
#endif // VICE

//wstring VCPU (L"vCPU");
wstring VBUS (L"vBUS");

SYSTEM::SYSTEM()
{
  GetInitSigma(true);
}

SYSTEM::~SYSTEM()
{
}

// GetInitSigma
// ast_is_new : bool
// ==> ()
SYSTEM & SYSTEM::GetInitSigma (bool ast_is_new)
{
  this->cpustate.Clear();
  this->cpustate.Insert(Nil(), EmptyCPU(Nil()));
#ifdef VICE
  this->syscpus.Clear();
  this->taskm.Clear();
  this->busstate.Clear();
  this->nextmesid = Int(0);
//  this->nextvircpu = Int(0);
  this->vircpus.Clear();
  this->vircpus.Insert(Nil());
  this->allocated.Clear();
  this->delayed.Clear();
#endif // VICE
  this->curcpu = Nil();
  return *this;
}

// Init_Sigma
// ast_is_new : bool
void SYSTEM::Init_Sigma(bool ast_is_new)
{
  GetInitSigma(ast_is_new);
}

void SYSTEM::Dump(wostream & wos)
{
  wos << L"cpustate:" << endl;
  wos << this->cpustate << endl;
#ifdef VICE
  wos << L"syscpus:" << endl;
  wos << this->syscpus << endl;
  wos << L"taskm:" << endl;
  wos << this->taskm << endl;
  wos << L"busstate:" << endl;
  wos << this->busstate << endl;
  wos << L"nextmesid:" << endl;
  wos << this->nextmesid << endl;
//  wos << L"nextvircpu:" << endl;
//  wos << this->nextvircpu << endl;
  wos << L"vircpus:" << endl;
  wos << this->vircpus << endl;
  wos << L"allocated:" << endl;
  wos << this->allocated << endl;
  wos << L"delayed:" << endl;
  wos << this->delayed << endl;
#endif // VICE
  wos << L"curcpu:" << endl;
  wos << this->curcpu << endl;
}

#ifdef VICE
// PrintThread
// tid : [SCHDTP`ThreadId]
// cpu : [AS`Name]
// -> ceq of char
wstring SYSTEM::PrintThread(const Generic & tid, const Generic & cpu)
{
  wstring ret;
  ret += L"TID:";
  if( tid.IsNil() ) {
    ret += L"nil";
  }
  else {
    ret += Int(tid).ascii();
  }
  ret += L"(" + PrintCPU(cpu) + L")";
  return ret;
}

wstring SYSTEM::PrintCurThread()
{
  return PrintThread(theScheduler().CurThreadId(), GetCurCPU());
}

wstring SYSTEM::PrintCPU(const Generic & cpu)
{
  wstring ret;
  if (cpu.IsNil()) {
    ret += L"nil";
  }
  else {
    ret += ASTAUX::ASName2String(cpu);
  }
  return ret;
}
#endif // VICE

//#ifdef VDMPP
// EmptyCPU
// decl : [AS`Expr]
// ==> CPUSigma
TYPE_STKM_CPUSigma SYSTEM::EmptyCPU(const Generic & decl)
{
#ifdef VICE
  TYPE_SCHDTP_SecondarySchedulerAlgorithm prio = TYPE_SCHDTP_RoundRobin();
  Generic capacity = Settings.GetVirtualCPUCapacity(); 
  if( !decl.IsNil() ) {
    capacity = Int(Settings.GetDefaultCapacity());
  }
  if( !decl.IsNil() ) {
    TYPE_AS_Expr r (decl);
    if( r.Is(TAG_TYPE_AS_NewExpr) ) {
      TYPE_AS_NewExpr ne (decl);
      GetCI().IncTestCoverageInfo(ne.get_cid());
      if( (ASTAUX::ASName2String(ne.get_cls()) == L"CPU") &&
          (ne.get_args().Length() == 2) ) {
        GetCI().IncTestCoverageInfo(ne.get_cls().get_cid());
        Generic first (ne.get_args().Index(1));
        Generic second (ne.get_args().Index(2));
        if( first.Is(TAG_TYPE_AS_QuoteLit) ) {
          TYPE_AS_QuoteLit ql (first);
          GetCI().IncTestCoverageInfo(ql.get_cid());
          if( ql.get_val().GetString() == L"FP" ) {
            prio = TYPE_SCHDTP_PriorityBased();
          }
          else if( ql.get_val().GetString() == L"FCFS" ) {
            prio = TYPE_SCHDTP_RoundRobin();
          }
          else {
            RTERR::InitError (L"EmptyCPU", RTERR_UNSUPPRIO, Nil(), Nil(), ASTAUX::GetCid(first), Sequence());
          }
        }
        else {
          RTERR::InitError (L"EmptyCPU", RTERR_WRONGCPUCONS, Nil(), Nil(), ASTAUX::GetCid(decl), Sequence());
        }
        if( second.Is(TAG_TYPE_AS_RealLit) ) {
          TYPE_AS_RealLit rl (second);
          GetCI().IncTestCoverageInfo(rl.get_cid());
          if( rl.get_val().GetValue() >= 1 ) {
            capacity = Int(rl.get_val());
          }
          else {
            RTERR::InitError (L"EmptyCPU", RTERR_UNSUPCPUCAP, Nil(), Nil(), ASTAUX::GetCid(second), Sequence());
          }
        }
        else if( second.Is(TAG_TYPE_AS_NumLit) ) {
          TYPE_AS_NumLit rl (second);
          GetCI().IncTestCoverageInfo(rl.get_cid()); // 20081030
          if( rl.get_val().GetValue() >= 1 ) {
            capacity = Int(rl.get_val());
          }
          else {
            RTERR::InitError (L"EmptyCPU", RTERR_UNSUPCPUCAP, Nil(), Nil(), ASTAUX::GetCid(second), Sequence());
          }
        }
        else {
          RTERR::InitError (L"EmptyCPU", RTERR_UNSUPCPUCAP, Nil(), Nil(), ASTAUX::GetCid(second), Sequence());
        }
      }
      else {
        RTERR::InitError (L"EmptyCPU", RTERR_WRONGCPUCONS, Nil(), Nil(), ASTAUX::GetCid(decl), Sequence());
      }
    }
    else {
      RTERR::InitError (L"EmptyCPU", RTERR_WRONGCPUCONS, Nil(), Nil(), ASTAUX::GetCid(decl), Sequence());
    }
  }
  TYPE_SCHDTP_PrimarySchedulerAlgorithm psa = TYPE_SCHDTP_PureCooperative();
  if (decl.IsNil()) {
    psa = Settings.GetPrimaryAlgorithm();
  }
#endif // VICE

  TYPE_STKM_CPUSigma csigma;
  csigma.SetField(pos_STKM_CPUSigma_threadstate,   Map());
  csigma.SetField(pos_STKM_CPUSigma_curthread,     Nil());
  csigma.SetField(pos_STKM_CPUSigma_curr_uprogram, TYPE_STKM_SubProgram());
#ifdef VICE
  csigma.SetField(pos_STKM_CPUSigma_threads,       Set());
  csigma.SetField(pos_STKM_CPUSigma_syncopcall,    Set());
  csigma.SetField(pos_STKM_CPUSigma_objrefs,       Set());
  
  csigma.SetField(pos_STKM_CPUSigma_schd,          psa);
  csigma.SetField(pos_STKM_CPUSigma_prio,          prio);
  csigma.SetField(pos_STKM_CPUSigma_prio_utab,     Map());
  csigma.SetField(pos_STKM_CPUSigma_messages,      Map());
  csigma.SetField(pos_STKM_CPUSigma_capacity,      capacity);
  csigma.SetField(pos_STKM_CPUSigma_dur,           Int(0));
  csigma.SetField(pos_STKM_CPUSigma_cycle,         Int(0));
  csigma.SetField(pos_STKM_CPUSigma_time,          Int(0));
  csigma.SetField(pos_STKM_CPUSigma_idletime,      Int(0));
  csigma.SetField(pos_STKM_CPUSigma_swaptime,      Int(100));
#endif // VICE
  return csigma;
}

#ifdef VICE
TYPE_STKM_CPUSigma SYSTEM::RemoveOldEval(const TYPE_STKM_CPUSigma & cs)
{
  TYPE_STKM_CPUSigma csigma;
  csigma.SetField(pos_STKM_CPUSigma_threadstate,   Map());
  csigma.SetField(pos_STKM_CPUSigma_curthread,     Nil());
  csigma.SetField(pos_STKM_CPUSigma_curr_uprogram, TYPE_STKM_SubProgram());
  csigma.SetField(pos_STKM_CPUSigma_threads,       Set());
  csigma.SetField(pos_STKM_CPUSigma_syncopcall,    Set());
  csigma.SetField(pos_STKM_CPUSigma_objrefs,       cs.GetSet(pos_STKM_CPUSigma_objrefs));  //
  csigma.SetField(pos_STKM_CPUSigma_schd,          cs.GetRecord(pos_STKM_CPUSigma_schd));     //
  csigma.SetField(pos_STKM_CPUSigma_prio,          cs.GetRecord(pos_STKM_CPUSigma_prio));     //
  csigma.SetField(pos_STKM_CPUSigma_prio_utab,     cs.GetMap(pos_STKM_CPUSigma_prio_utab)); //
  csigma.SetField(pos_STKM_CPUSigma_messages,      Map());
  csigma.SetField(pos_STKM_CPUSigma_capacity,      cs.GetField(pos_STKM_CPUSigma_capacity)); //
  csigma.SetField(pos_STKM_CPUSigma_dur,           Int(0));
  csigma.SetField(pos_STKM_CPUSigma_cycle,         Int(0));
  csigma.SetField(pos_STKM_CPUSigma_time,          Int(0));
  csigma.SetField(pos_STKM_CPUSigma_idletime,      Int(0));
  csigma.SetField(pos_STKM_CPUSigma_swaptime,      Int(100));
  return csigma;
}

TYPE_STKM_BUSSigma SYSTEM::RemoveMesAndLog(const TYPE_STKM_BUSSigma & bs)
{
  TYPE_STKM_BUSSigma bsigma;
  bsigma.SetField(pos_STKM_BUSSigma_kind,     bs.get_kind());     //
  bsigma.SetField(pos_STKM_BUSSigma_capacity, bs.get_capacity()); //
  bsigma.SetField(pos_STKM_BUSSigma_cpus,     bs.get_cpus());     //
  bsigma.SetField(pos_STKM_BUSSigma_time,     Int(0));
  bsigma.SetField(pos_STKM_BUSSigma_waiting,  Sequence());
  bsigma.SetField(pos_STKM_BUSSigma_mes_um,   Map());
  bsigma.SetField(pos_STKM_BUSSigma_log,      Sequence());
  return bsigma;
}

// EmptyBUS
// decl : [AS`Expr]
// ==> BUSSigma
TYPE_STKM_BUSSigma SYSTEM::EmptyBUS(const Generic & decl)
{
  if( !decl.IsNil() ) {
    TYPE_SCHDTP_BUSKind kind = TYPE_SCHDTP_FCFS();
    Generic cap = Quote(L"INFINITE");
    Set cpus;
    cpus.Insert(Nil());

    TYPE_AS_Expr r (decl);
    if( r.Is(TAG_TYPE_AS_NewExpr) ) {
      TYPE_AS_NewExpr ne (decl);
      GetCI().IncTestCoverageInfo(ne.get_cid()); // 20081030
      if( (ASTAUX::ASName2String(ne.get_cls()) == L"BUS") &&
          (ne.get_args().Length() == 3) ) {
        GetCI().IncTestCoverageInfo(ne.get_cls().get_cid()); // 20081030
        Generic first (ne.get_args().Index(1));
        Generic second (ne.get_args().Index(2));
        Generic third (ne.get_args().Index(3));
        if( first.Is(TAG_TYPE_AS_QuoteLit) ) {
          TYPE_AS_QuoteLit ql (first);
          GetCI().IncTestCoverageInfo(ql.get_cid()); // 20081030
          if( ql.get_val().GetString() == L"CSMACD" ) {
            kind = TYPE_SCHDTP_CSMACD();
          }
          else if( ql.get_val().GetString() == L"TDMA" ) {
            kind = TYPE_SCHDTP_TDMA();
          }
          else if( ql.get_val().GetString() == L"FCFS" ) {
            kind = TYPE_SCHDTP_FCFS();
          }
          else {
            RTERR::InitError (L"EmptyBUS", RTERR_UNSUPPKIND, Nil(), Nil(), ASTAUX::GetCid(first), Sequence());
          }
        }
        else {
          RTERR::InitError (L"EmptyBUS", RTERR_UNSUPPKIND, Nil(), Nil(), ASTAUX::GetCid(first), Sequence());
        }

        if( second.Is(TAG_TYPE_AS_RealLit) ) {
          TYPE_AS_RealLit rl (second);
          GetCI().IncTestCoverageInfo(rl.get_cid());
          if( rl.get_val().GetValue() >= 1 ) {
            cap = rl.get_val();
          }
          else {
            RTERR::InitError (L"EmptyBUS", RTERR_UNSUPBUSCAP, Nil(), Nil(), ASTAUX::GetCid(second), Sequence());
          }
        }
        else if( second.Is(TAG_TYPE_AS_NumLit) ) {
          TYPE_AS_NumLit rl (second);
          GetCI().IncTestCoverageInfo(rl.get_cid());
          if( rl.get_val().GetValue() >= 1 ) {
            cap = rl.get_val();
          }
          else {
            RTERR::InitError (L"EmptyBUS", RTERR_UNSUPBUSCAP, Nil(), Nil(), ASTAUX::GetCid(second), Sequence());
          }
        }
        else {
          RTERR::InitError (L"EmptyBUS", RTERR_UNSUPBUSCAP, Nil(), Nil(), ASTAUX::GetCid(second), Sequence());
        }
        if( third.Is(TAG_TYPE_AS_SetEnumerationExpr) ) {
          TYPE_AS_SetEnumerationExpr see (third);
          GetCI().IncTestCoverageInfo(see.get_cid());
          const SEQ<TYPE_AS_Expr> & els (see.GetSequence(pos_AS_SetEnumerationExpr_els));
          bool allok = true;
          size_t len_els = els.Length();
          for (size_t index= 1; index <= len_els; index++) {
            const TYPE_AS_Expr & el (els[index]);
            GetCI().IncTestCoverageInfo(ASTAUX::GetCid(el));
            allok = el.Is(TAG_TYPE_AS_Name);
          }
          if(allok) {
            cpus = els.Elems();
          }
          else {
            RTERR::InitError (L"EmptyBUS", RTERR_NOSETOFCPUS, Nil(), Nil(), ASTAUX::GetCid(third), Sequence());
          }
        }
        else {
          RTERR::InitError (L"EmptyBUS", RTERR_NOSETOFCPUS, Nil(), Nil(), ASTAUX::GetCid(third), Sequence());
        }
      }
      else {
        RTERR::InitError (L"EmptyBUS", RTERR_WRONGBUSCONS, Nil(), Nil(), ASTAUX::GetCid(decl), Sequence());
      }
    }
    TYPE_STKM_BUSSigma bsigma;
    bsigma.SetField(pos_STKM_BUSSigma_kind,     kind);
    bsigma.SetField(pos_STKM_BUSSigma_capacity, cap);
    bsigma.SetField(pos_STKM_BUSSigma_cpus,     cpus);
    bsigma.SetField(pos_STKM_BUSSigma_time,     Int(0));
    bsigma.SetField(pos_STKM_BUSSigma_waiting,  Sequence());
    bsigma.SetField(pos_STKM_BUSSigma_mes_um,   Map());
    bsigma.SetField(pos_STKM_BUSSigma_log,      Sequence());
    return bsigma;
  }
  else { // if( decl.IsNil() )
    TYPE_STKM_BUSSigma bsigma;
    bsigma.SetField(pos_STKM_BUSSigma_kind,     TYPE_SCHDTP_FCFS());
    bsigma.SetField(pos_STKM_BUSSigma_capacity, Quote(L"INFINITE"));
    bsigma.SetField(pos_STKM_BUSSigma_cpus,     GetAllCPUs());
    bsigma.SetField(pos_STKM_BUSSigma_time,     Int(0));
    bsigma.SetField(pos_STKM_BUSSigma_waiting,  Sequence());
    bsigma.SetField(pos_STKM_BUSSigma_mes_um,   Map());
    bsigma.SetField(pos_STKM_BUSSigma_log,      Sequence());
    return bsigma;
  }
}

// DeployInst
// cpunm : [AS`Name]
// obj_ref : SEM`OBJ_Ref
// ==> ()
void SYSTEM::DeployInst(const Generic & cpunm, const TYPE_SEM_OBJ_uRef & obj_ref)
{
//wcout << L"DeployInst: " << cpunm << endl;
  TYPE_STKM_CPUSigma csigma (this->cpustate[cpunm]);
  Set objrefs (csigma.GetSet(pos_STKM_CPUSigma_objrefs));
  objrefs.Insert(obj_ref);
  csigma.SetField(pos_STKM_CPUSigma_objrefs, objrefs);
  this->cpustate.ImpModify(cpunm, csigma);

  if( !cpunm.IsNil() ) {
    theState().SetCPUOfObjRef(cpunm, obj_ref);
  }
//vdm_iplog << L"DeployInst:    " << PrintCPU(cpunm) << L" OID:" << theState().PrintOID(obj_ref) << endl;
//  if( !this->allocated.DomExists(obj_ref) ) //// %%%%%%%%%%%%%%%%%%%
    this->allocated.ImpModify(obj_ref, cpunm);

//  const TYPE_AS_Name & clnm (obj_ref.GetRecord(pos_SEM_OBJ_uRef_tp));

//  TYPE_STKM_CPUSigma nsigma (this->cpustate[Nil()]);
//  Int time (nsigma.GetInt(pos_STKM_CPUSigma_time));
  Int time (csigma.GetInt(pos_STKM_CPUSigma_time));

  TIMETRACE::LogObjAlloc(obj_ref, cpunm, time);
}

//// DelayDeployInst
//// cpunm : [AS`Name]
//// obj_ref : SEM`OBJ_Ref
//// ==> ()
//void SYSTEM::DelayDeployInst(const Generic & cpunm,
//                             const TYPE_SEM_OBJ_uRef & obj_ref)
//{
//  const TYPE_AS_Name & clnm (obj_ref.GetRecord(pos_SEM_OBJ_uRef_tp));
//  TYPE_STKM_CPUSigma nsigma (this->cpustate[Nil()]);
//  TIMETRACE::LogObjAlloc(obj_ref, Nil(), nsigma.get_time());
//  this->delayed.ImpModify(obj_ref, cpunm);
//}

// LateDeployInst
// ==> ()
void SYSTEM::LateDeployInst()
{
  if( !this->delayed.IsEmpty() ) {
/*
    Set dom_delayed (this->delayed.Dom());
    Generic obj_ref;
    for(bool bb = dom_delayed.First(obj_ref); bb; bb = dom_delayed.Next(obj_ref))
    {
      DeployInst(this->delayed[obj_ref], obj_ref);
    }
*/

    // change deploy order for comparison to spec
    Sequence o_seq (ASTAUX::SetToSequenceR(this->delayed.Dom()));
    Generic obj_ref;
    for(bool cc = o_seq.First(obj_ref); cc; cc= o_seq.Next(obj_ref)) {
      DeployInst(this->delayed[obj_ref], obj_ref);
    }
    this->delayed.Clear();
  }
}

// AddPriorityEntry
// cpunm : AS`Name
// opnm : AS`Name
// prio : nat
// ==> ()
void SYSTEM::AddPriorityEntry(const TYPE_AS_Name & cpunm, const TYPE_AS_Name & opnm, const Int & prio)
{
  if( this->cpustate.DomExists(cpunm) ) {
    TYPE_STKM_CPUSigma csigma (this->cpustate[cpunm]);
    Map prio_tab (csigma.GetMap(pos_STKM_CPUSigma_prio_utab));
    prio_tab.ImpModify(opnm, prio);
    csigma.SetField(pos_STKM_CPUSigma_prio_utab, prio_tab);
    this->cpustate.ImpModify(cpunm, csigma);
  }
}

/*
// IncrIdleTime
// cpunm : [AS`Name]
// delta : nat
// ==> ()
void SYSTEM::IncrIdleTime(const Generic & cpunm, int delta)
{
  Generic g;
  if( this->cpustate.DomExists(cpunm,g) )
  {
    TYPE_STKM_CPUSigma csigma (g);
    int idletime (csigma.get_idletime().GetValue());
    int curtime (csigma.get_time().GetValue());
    if( curtime < idletime )
      curtime = idletime;
    idletime = curtime + delta;
    csigma.set_idletime(Int(idletime));
    this->cpustate.ImpModify(cpunm,csigma);
  }
}
*/

// CurCPUAnyThread
// nm : [AS`Name]
// ==> bool
bool SYSTEM::CurCPUAnyThread(const Generic & nm) const
{
  if ( this->cpustate.DomExists(nm) ) {
    TYPE_STKM_CPUSigma csigma (this->cpustate[nm]);
    return !csigma.GetSet(pos_STKM_CPUSigma_threads).IsEmpty();
  }
  return false;
}

// CurCPUAnyMessages
// nm : [AS`Name]
// ==> bool
bool SYSTEM::CurCPUAnyMessages(const Generic & nm) const
{
  if ( this->cpustate.DomExists(nm) ) {
    TYPE_STKM_CPUSigma csigma (this->cpustate[nm]);
//    return !csigma.GetMap(pos_STKM_CPUSigma_messages).IsEmpty();
    const Map & mes_m (csigma.GetMap(pos_STKM_CPUSigma_messages));
    if (!mes_m.IsEmpty()) {
      Generic opnm (FindFirstOpMes(mes_m));
      if ( opnm.IsNil() ) {
        opnm = mes_m.Dom().GetElem();
      }
      SEQ<TYPE_STKM_Message> mes_s (mes_m[opnm]);
      TYPE_STKM_Message mes (mes_s.Hd());
      const Int & t (mes.GetInt(pos_STKM_Message_time));
      return ( t <= csigma.GetInt(pos_STKM_CPUSigma_swaptime) );
    }
  }
  return false;
}

// SelAndRunMessageCall
// cpunm : [AS`Name]
// ==> STKM`EvaluationState * [SEM`VAL]
Tuple SYSTEM::SelAndRunMessageCall(const Generic & cpunm)
{
//wcout << PrintCPU(this->curcpu) << L" " << PrintCPU(cpunm) << endl;
  if ( this->cpustate.DomExists(cpunm) && CurCPUAnyMessages(cpunm) ) {
    TYPE_STKM_CPUSigma csigma (this->cpustate[cpunm]);
    Map mes_m (csigma.GetMap(pos_STKM_CPUSigma_messages));

    Generic opnm (FindFirstOpMes(mes_m));

    if ( opnm.IsNil() ) {
      opnm = mes_m.Dom().GetElem();
    }
    SEQ<TYPE_STKM_Message> mes_s (mes_m[opnm]);
    TYPE_STKM_Message mes (mes_s.Hd());
    
    mes_s.ImpTl();
    if ( mes_s.IsEmpty() ) {
      mes_m.RemElem(opnm);
    }
    else {
      mes_m.ImpModify(opnm, mes_s);
    }
    csigma.SetField(pos_STKM_CPUSigma_messages, mes_m);
    this->cpustate.ImpModify(cpunm, csigma);
    return RunMessage(mes,cpunm);
  }
  return theScheduler().DefaultEvalRes();
}

// FindFirstOpMes
// mes_m : map AS`Name to seq of Message
// ==> [AS`Name]
Generic SYSTEM::FindFirstOpMes(const Map & mes_m) const
{
  if ( !mes_m.IsEmpty() ) {
    TYPE_STKM_CPUSigma csigma (this->cpustate[this->curcpu]);
    const MAP<TYPE_AS_Name, Int> & prio_tab (csigma.GetMap(pos_STKM_CPUSigma_prio_utab));
    Set ops (prio_tab.Dom());

    ops.ImpIntersect(mes_m.Dom());

    if ( !ops.IsEmpty() ) {
      TYPE_AS_Name opnm (ops.GetElem());
      Int prio (prio_tab[opnm]);
      Generic op;
      for (bool bb = ops.First(op); bb; bb = ops.Next(op)) {
        if ( prio < Int(prio_tab[op]) ) {
          opnm = op;
          prio = prio_tab[op];
        }
      }
      return opnm;
    }
    return mes_m.Dom().GetElem();
  }
  return Nil();
}

// RunMessage
// mes : Message
// ==> STKM`EvaluationState * [SEM`VAL]
Tuple SYSTEM::RunMessage(const TYPE_STKM_Message & m, const Generic & cpunm)
{
  const TYPE_STKM_MessageId & id (m.GetInt(pos_STKM_Message_id));
  const Int & t (m.GetInt(pos_STKM_Message_time));
  const Int & p (m.GetInt(pos_STKM_Message_prio));
  const Record & mes (m.GetRecord(pos_STKM_Message_mes));

  TYPE_STKM_CPUSigma csigma (this->cpustate[cpunm]);

  if ( t <= csigma.GetInt(pos_STKM_CPUSigma_swaptime) ) {
    SetCurCPU(cpunm, csigma.GetInt(pos_STKM_CPUSigma_swaptime));
    switch (mes.GetTag()) {
      case TAG_TYPE_STKM_MessageBroadcast: {
        return RunBroadcastMessage(id, t, p, mes);
      }
      case TAG_TYPE_STKM_MessageInfo: {
        const Generic & obj_ref (mes.GetField(pos_STKM_MessageInfo_obj_uref));

        Generic clnm = Nil();
        if(!obj_ref.IsNil() && theState().IsInObjTab(obj_ref)) {
          clnm = Record(obj_ref).GetRecord(pos_SEM_OBJ_uRef_tp);
        }
        const TYPE_AS_Name & opnm (mes.GetRecord(pos_STKM_MessageInfo_opname));
        TYPE_AS_Name fullnm (!clnm.IsNil() ? AUX::ConstructDoubleName(clnm, opnm) : opnm);
        if (!obj_ref.IsNil()) {
          theStackMachine().PushCurObj(obj_ref, clnm, clnm);
        }
        TYPE_SEM_ValTp valTp (theState().LookUp(fullnm));
        TYPE_SEM_ExplOP opval (valTp.GetRecord(pos_SEM_ValTp_val));
        if (!obj_ref.IsNil()) {
          theStackMachine().PopCurObj();
        }
        const MAP<TYPE_AS_Name, Int> & prio_tab (csigma.GetMap(pos_STKM_CPUSigma_prio_utab));
        Int prio (prio_tab.DomExists(opnm) ? prio_tab[opnm] : SCHD::Default_priority);

        switch(opval.GetTag()) {
          case TAG_TYPE_SEM_ExplOP: {
            opval.SetField(pos_SEM_ExplOP_objref, obj_ref);
            break;
          }
          case TAG_TYPE_SEM_OverOPFN: {
            opval.SetField(pos_SEM_OverOPFN_objref, obj_ref);
            break;
          }
          default:
            break;
        }

        TYPE_STKM_SubProgram instr;
        instr.ImpAppend(TYPE_INSTRTP_PUSHCONTEXT().Init(Int(ALL))) //
             .ImpAppend(TYPE_INSTRTP_PUSH().Init(opval))
             .ImpAppend(TYPE_INSTRTP_PUSH().Init(mes.GetSequence(pos_STKM_MessageInfo_args)))
             .ImpAppend(TYPE_INSTRTP_GUARD().Init(Bool(false)))
             .ImpAppend(TYPE_INSTRTP_CONTEXT().Init(GetCID(opval), Bool(false)))
             .ImpAppend(TYPE_INSTRTP_APPLY());

        const Generic & threadid (mes.GetField(pos_STKM_MessageInfo_threadid));
        if ( !threadid.IsNil() ) {
          instr.ImpAppend(TYPE_INSTRTP_RETMES().Init(id, prio, fullnm,
                                                     mes.GetField(pos_STKM_MessageInfo_respond),
                                                     threadid));
        }
        instr.ImpAppend(TYPE_INSTRTP_EOCL());

        return theScheduler().RunMessageThread(clnm, obj_ref, instr, prio, id);
      }
      default: {
        return Tuple(); // dummy
      }
    }
  }
  else {
    TYPE_STKM_CPUSigma csigma (this->cpustate[cpunm]);
    Int swaptime (csigma.GetInt(pos_STKM_CPUSigma_swaptime));
    csigma.SetField(pos_STKM_CPUSigma_time, swaptime);
    this->cpustate.ImpModify(cpunm, csigma);
    return theScheduler().DefaultEvalRes();
  }
}

// RunBroadcastMessage
// id : MessageId
// t : nat
// p : nat
// mes : MessageBroadcast
// ==> STKM`EvaluationState * [SEM`VAL]
Tuple SYSTEM::RunBroadcastMessage(const TYPE_STKM_MessageId & id,
                                  const Int & t,
                                  const Int & p,
                                  const TYPE_STKM_MessageBroadcast & mes)
{
  const SEQ<TYPE_SEM_OBJ_uRef> & objrefs (mes.GetSequence(pos_STKM_MessageBroadcast_obj_urefs));
  const TYPE_AS_Name & opnm (mes.GetRecord(pos_STKM_MessageBroadcast_opname));
  const SEQ<TYPE_SEM_VAL> & args (mes.GetSequence(pos_STKM_MessageBroadcast_args));
  const Map & tcpum (mes.GetMap(pos_STKM_MessageBroadcast_tcpum));
  const Generic & cpu (mes.GetField(pos_STKM_MessageBroadcast_curcpu));

  TYPE_STKM_CPUSigma csigma (this->cpustate[cpu]); 
  SetCurCPU(cpu, csigma.GetInt(pos_STKM_CPUSigma_swaptime));
  
  TYPE_SCHDTP_ThreadId tid;

  size_t len_objrefs = objrefs.Length();
  for (size_t index = 1; index <= len_objrefs; index++) {
    const TYPE_SEM_OBJ_uRef & objref (objrefs[index]);
    //Generic cpunm (tcpum[objref]);
    TYPE_AS_Name fullnm (opnm);
    TYPE_SEM_ValTp valTp (theState().LookUp(fullnm));
    TYPE_SEM_ExplOP opval (valTp.GetRecord(pos_SEM_ValTp_val));

    TYPE_STKM_CPUSigma cursigma (this->cpustate[this->curcpu]);
    const MAP<TYPE_AS_Name, Int> & prio_tab (csigma.GetMap(pos_STKM_CPUSigma_prio_utab));
    Int prio (prio_tab.DomExists(opnm) ? prio_tab[opnm] : SCHD::Default_priority);
      
    opval.SetField(pos_SEM_ExplOP_objref, objref);
    opval.SetField(pos_SEM_ExplOP_sync, Bool(true));

    TYPE_STKM_SubProgram instr;
    instr.ImpAppend(TYPE_INSTRTP_PUSHCONTEXT().Init(Int(ALL))) //
         .ImpAppend(TYPE_INSTRTP_PUSH().Init(opval))
         .ImpAppend(TYPE_INSTRTP_PUSH().Init(args))
         .ImpAppend(TYPE_INSTRTP_GUARD().Init(Bool(false)))
         .ImpAppend(TYPE_INSTRTP_CONTEXT().Init(GetCID(opval), Bool(false)))
         .ImpAppend(TYPE_INSTRTP_APPLY())
         .ImpAppend(TYPE_INSTRTP_EOCL());

    tid = theScheduler().AddNewThreadId(Bool(false), objref, instr, prio, Bool(false), id);
    theScheduler().SetThreadStatus(tid, TYPE_SCHDTP_Running());
  }
  Generic cpuofthread (theScheduler().GetCPUOfThread(tid));
  TYPE_STKM_CPUSigma thsigma (this->cpustate[cpuofthread]);
  SetCurCPU(cpuofthread, thsigma.GetInt(pos_STKM_CPUSigma_swaptime));

  return theScheduler().RunThread(tid);
}

// val : SEM`OP | SEM`FN
// ==> CI`ContextId
TYPE_CI_ContextId SYSTEM::GetCID(const TYPE_SEM_VAL & val) const
{
  switch (val.GetTag()) {
    case TAG_TYPE_SEM_OverOPFN: {
      return NilContextId;
    }
    case TAG_TYPE_SEM_ExplFN: {
      const Generic & fnm (val.GetField(pos_SEM_ExplFN_fnName));
      if (fnm.IsNil()) {
        return NilContextId;
      }
      else {
        return TYPE_AS_Name(fnm).GetInt(pos_AS_Name_cid);
      }
    }
    case TAG_TYPE_SEM_ExplOP: {
      return val.GetRecord(pos_SEM_ExplOP_fnName).GetInt(pos_AS_Name_cid);
    }
    case TAG_TYPE_SEM_ExplPOLY: {
      const Generic & fnm (val.GetField(pos_SEM_ExplPOLY_fnName));
      if (fnm.IsNil()) {
        return NilContextId;
      }
      else {
        return TYPE_AS_Name(fnm).GetInt(pos_AS_Name_cid);
      }
    }
    default: {
      return NilContextId;
    }
  }
}

// CPUWork
// cpunm : [AS`Name]
// swapt : nat ==> bool
bool SYSTEM::CPUWork(const Generic & cpunm, int swapt) const
{
  if( this->cpustate.DomExists( cpunm ) ) {
    TYPE_STKM_CPUSigma csigma (this->cpustate[cpunm]);
    SET<TYPE_SCHDTP_ThreadId> threads (csigma.GetSet(pos_STKM_CPUSigma_threads));
    Set objrefs (csigma.GetSet(pos_STKM_CPUSigma_objrefs));
    bool exists = false;
    Generic tid;
    for(bool bb = threads.First(tid); bb && !exists; bb = threads.Next(tid)) {
      TYPE_SCHDTP_ThreadStatus ts (theScheduler().GetThreadStatus(tid));
      exists = !ts.Is(TAG_TYPE_SCHDTP_SyncOp) && !ts.Is(TAG_TYPE_SCHDTP_SyncOpRet);
    }
    return exists ||
           !csigma.GetMap(pos_STKM_CPUSigma_messages).IsEmpty() ||
           theScheduler().PeriodicReady(cpunm, objrefs);
  }
  return false;
}

// ProcessBUS
// nm : AS`Name
// swapt : nat
//// ==> STKM`EvaluationState * [SEM`VAL]
// ==> ()
void SYSTEM::ProcessBUS(const TYPE_AS_Name & nm, int swapt)
{
  if( !this->busstate.DomExists(nm) ) {
    RTERR::InitError (L"ProcessBUS", RTERR_INTERNAL_ERROR, Nil(), Nil(), nm.get_cid(), Sequence());
    return;
  }

  TYPE_STKM_BUSSigma bsigma (this->busstate[nm]);
  TYPE_SCHDTP_BUSKind kind (bsigma.GetRecord(pos_STKM_BUSSigma_kind));
  Generic cap (bsigma.GetField(pos_STKM_BUSSigma_capacity));
//  Set cpus (bsigma.GetSet(pos_STKM_BUSSigma_cpus)); // not used
  int t = bsigma.GetIntValue(pos_STKM_BUSSigma_time);
  Sequence waiting (bsigma.GetSequence(pos_STKM_BUSSigma_waiting)); // seq of
                                           //   (MessageReq | MessageReplyReq)
  MAP<TYPE_STKM_MessageId, TYPE_STKM_Message> mes_m (bsigma.GetMap(pos_STKM_BUSSigma_mes_um));
//  SEQ<TYPE_GLOBAL_ChanUsage> log (bsigma.GetSequence(pos_STKM_BUSSigma_log));

  int newt = t;

  if( !BUSWork(newt, swapt, mes_m, waiting) ) {
    Int time (t + Settings.GetStepSize());
    bsigma.SetField(pos_STKM_BUSSigma_time, time);
    this->busstate.ImpModify(nm, bsigma);
//    return theStackMachine().DefaultEvalRes();
    return;
  }

  while( newt <= swapt && BUSWork(newt, swapt, mes_m, waiting) ) {
    Generic first_mes (FindFirstMessage(mes_m));
//
    while( !waiting.IsEmpty() ) {
      Record req (waiting.Hd()); // (MessageReq | MessageReplyReq)

      Int time;
      switch(req.GetTag()) {
        case TAG_TYPE_STKM_MessageReq: {
          time = req.GetInt(pos_STKM_MessageReq_time);
          break;
        }
        case TAG_TYPE_STKM_MessageReplyReq: {
          time = req.GetInt(pos_STKM_MessageReplyReq_time);
          break;
        }
        default: {
          break;
        }
      }

      if ( !first_mes.IsNil() &&  (time.GetValue() > mes_m[first_mes].get_time().GetValue())) {
        break;
      }
      SEQ<TYPE_GLOBAL_ChanUsage> log (bsigma.GetSequence(pos_STKM_BUSSigma_log));
      Int lastuse (0);
      if (!log.IsEmpty())
        lastuse = log.Index(log.Length()).get_tot();
 
      Int treq (MaxVal(time.GetValue(), lastuse.GetValue()));

      Generic curcpu_;
      Generic tcpu;
      switch(req.GetTag()) {
        case TAG_TYPE_STKM_MessageReq: {
          curcpu_ = req.GetField(pos_STKM_MessageReq_curcpu);
          tcpu = req.GetField(pos_STKM_MessageReq_tcpu);
          TIMETRACE::LogMessageRequest(
                       req.GetField (pos_STKM_MessageReq_busid),
                       req.GetField (pos_STKM_MessageReq_curcpu),
                       req.GetField (pos_STKM_MessageReq_tcpu),
                       req.GetInt   (pos_STKM_MessageReq_mesid),
                       req.GetInt   (pos_STKM_MessageReq_tid),
                       req.GetRecord(pos_STKM_MessageReq_fullnm),
                       req.GetField (pos_STKM_MessageReq_objref),
                       req.GetInt   (pos_STKM_MessageReq_size),
                       treq);
          break;
        }
        case TAG_TYPE_STKM_MessageReplyReq: {
          curcpu_ = req.GetField(pos_STKM_MessageReplyReq_curcpu);
          tcpu = req.GetField(pos_STKM_MessageReplyReq_tcpu);
          TIMETRACE::LogReplyRequest(
                       req.GetField(pos_STKM_MessageReplyReq_busid),
                       req.GetField(pos_STKM_MessageReplyReq_curcpu),
                       req.GetField(pos_STKM_MessageReplyReq_tcpu),
                       req.GetInt  (pos_STKM_MessageReplyReq_newmesid),
                       req.GetInt  (pos_STKM_MessageReplyReq_mesid),
                       req.GetInt  (pos_STKM_MessageReplyReq_curthread),
                       req.GetInt  (pos_STKM_MessageReplyReq_fromthread),
                       req.GetInt  (pos_STKM_MessageReplyReq_size),
                       treq);
          break;
        }
        default: {
          break;
        }
      }

//      bsigma.SetField(pos_STKM_BUSSigma_waiting, waiting.Tl());
      waiting.ImpTl();
      bsigma.SetField(pos_STKM_BUSSigma_waiting, waiting);

      TYPE_GLOBAL_ChanUsage entry;
      entry.Init(treq, treq, curcpu_, tcpu);

      log.ImpAppend(entry);
      bsigma.SetField(pos_STKM_BUSSigma_log, log);
      bsigma.SetField(pos_STKM_BUSSigma_time, treq);
      this->busstate.ImpModify(nm, bsigma);

//      waiting = bsigma.GetField(pos_STKM_BUSSigma_waiting);
      newt = treq;
    }

    if (first_mes.IsNil()) {
//      return theScheduler().DefaultEvalRes();
      return;
    }
    SEQ<TYPE_GLOBAL_ChanUsage> log (bsigma.GetSequence(pos_STKM_BUSSigma_log));
//
    TYPE_STKM_Message mes ( mes_m[first_mes] );
    Record m (mes.get_mes());

    int size = 0;
    switch(m.GetTag()) {
      case TAG_TYPE_STKM_MessageResponse: {
        size = SemRec::Size(m.GetRecord(pos_STKM_MessageResponse_res));
        break;
      }
      case TAG_TYPE_STKM_MessageBroadcast: {
        size = SemRec::SizeValSeq(m.GetSequence(pos_STKM_MessageBroadcast_args));
        break;
      }
      case TAG_TYPE_STKM_MessageInfo: {
        size = SemRec::SizeValSeq(m.GetSequence(pos_STKM_MessageInfo_args));
        break;
      }
      default: {
        break;
      }
    }

    int dura = (CalcDuration(size, cap));
    Generic tcpu;
    switch(m.GetTag()) {
      case TAG_TYPE_STKM_MessageResponse: {
        tcpu = m.GetField(pos_STKM_MessageResponse_clnm);
        break;
      }
      case TAG_TYPE_STKM_MessageBroadcast: {
        tcpu = m.GetMap(pos_STKM_MessageBroadcast_tcpum);
        break;
      }
      case TAG_TYPE_STKM_MessageInfo: {
        Generic obj_ref (m.GetField(pos_STKM_MessageInfo_obj_uref));
        if( obj_ref.IsNil() ) {
          tcpu = Nil();
        }
        else {
          if ( this->allocated.DomExists( obj_ref ) ) {
            tcpu = this->allocated[obj_ref];
          }
          else {
            tcpu = theState().GetCPUOfObjRef(obj_ref);
          }
        }
      }
      default: {
        break;
      }
    }

    TYPE_GLOBAL_ChanUsage entry;
    entry.Init(Int(newt), Int(newt + dura), this->curcpu, tcpu);

    TYPE_AS_Name opnm;
    switch(m.GetTag()) {
      case TAG_TYPE_STKM_MessageResponse: {
        opnm = m.GetRecord(pos_STKM_MessageResponse_opname);
        break;
      }
      case TAG_TYPE_STKM_MessageBroadcast: {
        opnm = m.GetRecord(pos_STKM_MessageBroadcast_opname);
        break;
      }
      case TAG_TYPE_STKM_MessageInfo: {
        opnm = m.GetRecord(pos_STKM_MessageInfo_opname);
        break;
      }
      default: {
        break;
      }
    }

    int time = mes.GetIntValue(pos_STKM_Message_time);
    int realt = MaxVal(newt, time);
    TIMETRACE::LogMessageActivate(mes.get_id(), realt);
    newt = realt + dura;

    mes_m.RemElem(first_mes);
    log.ImpAppend(entry);

    // should be at out of loop ?
    bsigma.SetField(pos_STKM_BUSSigma_mes_um, mes_m);
    bsigma.SetField(pos_STKM_BUSSigma_log, log);
    this->busstate.ImpModify(nm, bsigma);

    switch(m.GetTag()) {
      case TAG_TYPE_STKM_MessageBroadcast: {
        TYPE_STKM_MessageBroadcast mb (m);
        InsertBroadMesInCPU(tcpu, opnm, mes, newt,
                            mb.get_obj_refs());
        break;
      }
      default: {
        InsertMesInCPU(tcpu, opnm, mes, newt);
        break;
      }
    }
  }
  bsigma.SetField(pos_STKM_BUSSigma_time, Int(newt));
  this->busstate.ImpModify(nm, bsigma);
//  return theStackMachine().DefaultEvalRes();
  return;
}

// BUSWork
// t : nat
// swapt : nat
// mes_m : map MessageId to Message
// waiting : seq of (MessageReq | MessageRepl)
// ==> bool
bool SYSTEM::BUSWork(int t, int swapt,
                     const MAP<TYPE_STKM_MessageId, TYPE_STKM_Message> & mes_m,
                     const Sequence & waiting) const
{
  if( t <= swapt ) {
    if(!waiting.IsEmpty()) {
      return true;
    }
    Set dom_mes_m (mes_m.Dom());
    Generic id;
    for(bool bb = dom_mes_m.First(id); bb; bb = dom_mes_m.Next(id)) {
      if( mes_m[id].get_time().GetValue() <= swapt ) {
        return true;
      }
    }
  }
  return false;
}

// FindFirstMessage
// mes_m : map MessageId to Message
// ==> [MessageId]
Generic SYSTEM::FindFirstMessage(const MAP<TYPE_STKM_MessageId, TYPE_STKM_Message> & mes_m) const
{
  Generic firstm = Nil();

  Sequence mid_s (ASTAUX::SetToSequenceR(mes_m.Dom()));
  Generic mid;
//  for(bool bb = mes_m.First(mid); bb; bb = mes_m.Next(mid))
  for(bool bb = mid_s.First(mid); bb; bb = mid_s.Next(mid)) {
    if( Earlier(mes_m, mid, firstm) ) {
      firstm = mid;
    }
  }
  return firstm;
}

// Earlier
// mes_m : (map MessageId to Message)
// mes : MessageId
// id : [nat]
// ==> bool
bool SYSTEM::Earlier(const MAP<TYPE_STKM_MessageId, TYPE_STKM_Message> & mes_m,
                     const TYPE_STKM_MessageId & mes,
                     const Generic & id) const
{
  if ( id.IsNil() ) {
    return true;
  }
  const TYPE_STKM_Message & m1 (mes_m[mes]);
  const TYPE_STKM_Message & m2 (mes_m[id]);
  return ( (m1.get_time() < m2.get_time() ) ||
     ( (m1.get_time() == m2.get_time()) &&
       (m1.get_prio() < m2.get_prio()) ) );
}

// InsertMesInCPU
// tcpu : [AS`Name]
// opnm : AS`Name
// mes : Message
// time : nat
// ==> ()
void SYSTEM::InsertMesInCPU(const Generic & tcpu,
                            const TYPE_AS_Name & opnm,
                            const TYPE_STKM_Message & mes,
                            int time)
{
//wcout << L"InsertMesInCPU: " << PrintCPU(tcpu) << L" " << ASTAUX::ASName2String(opnm) << endl;
//wcout << L"mes: " << mes << endl;
//wcout << L"curcpu: " << PrintCPU(GetCurCPU()) << endl;

  TYPE_STKM_CPUSigma to_csigma (this->cpustate[tcpu]);
  Map messages (to_csigma.GetMap(pos_STKM_CPUSigma_messages));

  SEQ<TYPE_STKM_Message> mes_s;
  if( messages.DomExists(opnm) ) {
    mes_s = messages[opnm];
  }
  int i = 1;
  if( !mes_s.IsEmpty() ) {
    bool found = false;
    while ( !found && i <= mes_s.Length() ) {
      if(mes_s[i].GetIntValue(pos_STKM_Message_time) <= mes.GetIntValue(pos_STKM_Message_time)) {
        i++;
      }
      else {
        found = true;
      }
    }
  }
  SEQ<TYPE_STKM_Message> res_s;
  res_s.ImpConc(mes_s.SubSequence(1, i - 1));
  res_s.ImpAppend(mes);
  res_s.ImpConc(mes_s.SubSequence(i, mes_s.Length()));

  TIMETRACE::LogMessageCompleted(mes.get_id(), time);
  StepCPUTimeBack(tcpu,time); // Note: modify sigma in Method

  Record m (mes.get_mes());
  switch(m.GetTag()) {
    case TAG_TYPE_STKM_MessageResponse: {
      const TYPE_SCHDTP_ThreadId & tid (m.GetInt(pos_STKM_MessageInfo_threadid));

//wcout << L"MessageResponse -> " << PrintCPU(tcpu) << L" " << tid << endl;

      TYPE_STKM_CPUSigma t_csigma (this->cpustate[tcpu]);
      Map threadstate (t_csigma.GetMap(pos_STKM_CPUSigma_threadstate)); // %%%%
      EvaluatorStatusCt ev ((const EvaluatorStatusCt &)threadstate[tid]);
//wcout << mr.get_res() << endl;
      const TYPE_SEM_VAL & res (m.GetRecord(pos_STKM_MessageResponse_res));
      ev.get_oneref_ref().eval_stack.Push(res);
      threadstate.ImpModify(tid, ev);
      t_csigma.SetField(pos_STKM_CPUSigma_threadstate, threadstate); // %%%%%
      this->cpustate.ImpModify(tcpu, t_csigma);

      theScheduler().AddValToEvalStack(tid, res);
      theScheduler().SetThreadStatus(tid, TYPE_SCHDTP_Running());
      break;
    }
    case TAG_TYPE_STKM_MessageBroadcast:
    case TAG_TYPE_STKM_MessageInfo: {
//wcout << L"Message -> " << PrintCPU(tcpu) << endl;
      TYPE_STKM_CPUSigma t_csigma (this->cpustate[tcpu]);
      Map messages (t_csigma.GetMap(pos_STKM_CPUSigma_messages));
      messages.ImpModify(opnm, res_s);
      t_csigma.SetField(pos_STKM_CPUSigma_messages, messages);
      this->cpustate.ImpModify(tcpu, t_csigma);
      break;
    }
    default: {
      break;
    }
  }
}

// InsertBroadMesInCPU
// tcpum : (map SEM`OBJ_Ref to [AS`Name])
// opnm : AS`Name
// mes : Message
// time : nat
// objs : set of SEM`OBJ_Ref
// ==> ()
void SYSTEM::InsertBroadMesInCPU(const Map & tcpum,
                                 const TYPE_AS_Name & opnm,
                                 const TYPE_STKM_Message & mes,
                                 int time,
                                 const SET<TYPE_SEM_OBJ_uRef> & objs)
{
  TYPE_STKM_MessageInfo mi (mes.GetRecord(pos_STKM_Message_mes));
  SET<TYPE_SEM_OBJ_uRef> objs_q (objs);
  Generic objref;
  for( bool bb = objs_q.First(objref); bb; bb = objs_q.Next(objref)) {
    Generic tcpu (tcpum[objref]); // [AS`Name]
    TYPE_STKM_MessageInfo m_info;
    m_info.Init(objref, opnm, mi.get_args(), Nil(), Nil());
    TYPE_STKM_Message newmes (mes);
    newmes.SetField(pos_STKM_Message_mes, m_info);
    
    TYPE_STKM_CPUSigma csigma (this->cpustate[tcpu]);
    Map messages (csigma.GetMap(pos_STKM_CPUSigma_messages));

    SEQ<TYPE_STKM_Message> mes_s;
    if( messages.DomExists(opnm) ) {
      mes_s = messages[opnm];
    }
    int i = 1;
    if( !mes_s.IsEmpty() ) {
      bool found = false;
      while ( !found && i <= mes_s.Length() ) {
        if(mes_s[i].GetIntValue(pos_STKM_Message_time) <= mes.GetIntValue(pos_STKM_Message_time)) {
          i++;
        }
        else {
          found = true;
        }
      }
    }
    SEQ<TYPE_STKM_Message> res_s;
    res_s.ImpConc(mes_s.SubSequence(1, i - 1));
    res_s.ImpAppend(newmes);
    res_s.ImpConc(mes_s.SubSequence(i, mes_s.Length()));

    StepCPUTimeBack(tcpu, time);

    messages.ImpModify(opnm, res_s);
    csigma.SetField(pos_STKM_CPUSigma_messages, messages);
    this->cpustate.ImpModify(tcpu, csigma);

    TIMETRACE::LogMessageCompleted(mes.get_id(), time);
  }
}

// CalcDuration
// size : nat
// capacity : (nat1 | <INFINITE>)
// -> nat
int SYSTEM::CalcDuration(int size, const Generic & capacity) const
{
  if ( capacity.IsQuote() ) { // <INFINITE>
    return 0;
  }
  else {
    int dura = size / Int(capacity).GetValue();
    if( dura == 0 ) {
      return 1;
    }
    else {
      return dura;
    }
  }
}

// MinVal
// x : nat
// y : nat
// -> nat
//int SYSTEM::MinVal(int x, int y) const
//{
//  return ( y < x ? y : x );
//}

// MaxVal
// x : nat
// y : nat
// -> nat
int SYSTEM::MaxVal(int x, int y) const
{
  return ( y < x ? x : y );
}

// IsSystem
// ==> bool
bool SYSTEM::IsSystem() const
{
//  return ( this->cpustate.Dom().Card() > 1 );
  return ( this->cpustate.Size() > 1 );
}

// GetCPUCapacity
// cpunm : [AS`Name]
// ==> nat | <INFINITE>
Generic SYSTEM::GetCPUCapacity(const Generic & cpunm) const
{
  if( !cpunm.IsNil() ) {
    if( this->cpustate.DomExists(cpunm) ) {
      TYPE_STKM_CPUSigma csigma (this->cpustate[cpunm]);
      return csigma.GetField(pos_STKM_CPUSigma_capacity);
    }
  }
  return Int(Settings.GetDefaultCapacity());
}
#endif // VICE

// SetCurThread
// id : SCHDTP`ThreadId
// ==> ()
void SYSTEM::SetCurThread(const TYPE_SCHDTP_ThreadId & id)
{
//wcout << L"SYSTEM::SetCurThread: " << id << endl;
  TYPE_STKM_CPUSigma csigma (this->cpustate[this->curcpu]);
  csigma.SetField(pos_STKM_CPUSigma_curthread, id);
  this->cpustate.ImpModify(this->curcpu, csigma);
}

Generic SYSTEM::GetCurThread() const
{
  TYPE_STKM_CPUSigma csigma (this->cpustate[this->curcpu]);
  return csigma.GetField(pos_STKM_CPUSigma_curthread);
}

#ifdef VDMPP
// SetGuard
// fullopnm : [AS`Name]
// obj : [SEM`OBJ_Ref]
// ==> ()
void SYSTEM::SetGuard(const Generic & fullopnm, const Generic & obj)
{
  TYPE_STKM_CPUSigma csigma (this->cpustate[this->curcpu]);
  const Generic & curthread (csigma.GetField(pos_STKM_CPUSigma_curthread));
  if ( !curthread.IsNil() ) {
    Map threadstate (csigma.GetMap(pos_STKM_CPUSigma_threadstate));
    EvaluatorStatusCt es ((const EvaluatorStatusCt &)threadstate[curthread]);
    es.get_shared_ref().guard = TYPE_STKM_Guard().Init(fullopnm, obj);
    threadstate.ImpModify(curthread, es);
    csigma.SetField(pos_STKM_CPUSigma_threadstate, threadstate);
    this->cpustate.ImpModify(this->curcpu, csigma);
  }
}

// ResetGuard
// ==> ()
void SYSTEM::ResetGuard()
{
#ifndef VICE
  if (!theStackMachine().UsesThreads()) return;
#endif // VICE
  TYPE_STKM_CPUSigma csigma (this->cpustate[this->curcpu]);
  const Generic & curthread (csigma.GetField(pos_STKM_CPUSigma_curthread));
  if ( !curthread.IsNil() ) {
    Map threadstate (csigma.GetMap(pos_STKM_CPUSigma_threadstate));
    EvaluatorStatusCt es ((const EvaluatorStatusCt &)threadstate[curthread]);
    es.get_shared_ref().guard = Nil();
    threadstate.ImpModify(curthread, es);
    csigma.SetField(pos_STKM_CPUSigma_threadstate, threadstate);
    this->cpustate.ImpModify(this->curcpu, csigma);
  }
}

// HasGuard
// ==> ()
bool SYSTEM::HasGuard() const
{
  TYPE_STKM_CPUSigma csigma (this->cpustate[this->curcpu]);
  const Generic & curthread (csigma.GetField(pos_STKM_CPUSigma_curthread));
  if ( !curthread.IsNil() ) {
    EvaluatorStatusCt es ((const EvaluatorStatusCt &)csigma.GetMap(pos_STKM_CPUSigma_threadstate)[curthread]);
    return !es.get_const_ref().guard.IsNil();
  }
  return false;
}

// RemoveAllGuards
// ==> ()
void SYSTEM::RemoveAllGuards()
{
  Set cpu_s (this->cpustate.Dom());
  Generic cpu;
  for (bool bb = cpu_s.First(cpu); bb; bb = cpu_s.Next(cpu)) {
    TYPE_STKM_CPUSigma csigma (this->cpustate[cpu]);
    Map t_m (csigma.GetMap(pos_STKM_CPUSigma_threadstate));
    Set tid_s (t_m.Dom());
    Generic tid;
    for (bool cc = tid_s.First(tid); cc; cc = tid_s.Next(tid)) {
      EvaluatorStatusCt es ((const EvaluatorStatusCt &)t_m[tid]);
      es.get_shared_ref().guard = Nil();
      t_m.ImpModify(tid, es);
    }
    csigma.SetField(pos_STKM_CPUSigma_threadstate, t_m);
    this->cpustate.ImpModify(cpu, csigma);
  }
}

// ResetSlice
// ==> ()
void SYSTEM::ResetSlice()
{
//wcout << L"ResetSlice: " << this->curcpu  << " (" << GetTimeOfCPU(this->curcpu) << ")" << endl;
#ifndef VICE
  if (!theStackMachine().UsesThreads()) return;
#endif // VICE
  TYPE_STKM_CPUSigma csigma (this->cpustate[this->curcpu]);
  const Generic & curthread (csigma.GetField(pos_STKM_CPUSigma_curthread));
//wcout << L"curcpu: " << this->curcpu << endl;
//wcout << L"curthread: " << curthread << endl;
  if ( !curthread.IsNil() ) {
    Map threadstate (csigma.GetMap(pos_STKM_CPUSigma_threadstate));
    EvaluatorStatusCt es ((const EvaluatorStatusCt &)threadstate[curthread]);
    threadstate.ImpModify(curthread, theScheduler().InitSlice(es));
    csigma.SetField(pos_STKM_CPUSigma_threadstate, threadstate);
    this->cpustate.ImpModify(this->curcpu, csigma);
  }
}
#endif // VDMPP

void SYSTEM::SetRunTimeError()
{
  TYPE_STKM_CPUSigma csigma (this->cpustate[this->curcpu]);
  const Generic & curthread (csigma.GetField(pos_STKM_CPUSigma_curthread));
  if ( !curthread.IsNil() ) {
    Map threadstate (csigma.GetMap(pos_STKM_CPUSigma_threadstate));
    EvaluatorStatusCt es ((const EvaluatorStatusCt &)threadstate[curthread]);
    es.get_shared_ref().rterror = true;
    threadstate.ImpModify(curthread, es);
    csigma.SetField(pos_STKM_CPUSigma_threadstate, threadstate);
    this->cpustate.ImpModify(this->curcpu, csigma);
  }
}

bool SYSTEM::RunTimeError() const
{
  TYPE_STKM_CPUSigma csigma (this->cpustate[this->curcpu]);
  const Generic & curthread (csigma.GetField(pos_STKM_CPUSigma_curthread));
  if ( !curthread.IsNil() ) {
    EvaluatorStatusCt es ((const EvaluatorStatusCt &)csigma.GetMap(pos_STKM_CPUSigma_threadstate)[curthread]);
    return es.get_const_ref().rterror;
  }
  return false;
}

// DeleteThread
// id : SCHDTP`ThreadId
// ==> ()
void SYSTEM::DeleteThread(const TYPE_SCHDTP_ThreadId & id)
{
//vdm_iplog << L"DeleteThread TID:" << id << L" " << PrintCPU(this->curcpu) << endl;
  TYPE_STKM_CPUSigma csigma (this->cpustate[this->curcpu]);

  if (id == csigma.GetField(pos_STKM_CPUSigma_curthread)) {
    csigma.SetField(pos_STKM_CPUSigma_curthread, Nil());
  }
  if (((Map &)(csigma.GetFieldRef(pos_STKM_CPUSigma_threadstate))).DomExists(id)) {
//  Map threadstate (csigma.GetMap(pos_STKM_CPUSigma_threadstate)); // %%%%%
//  threadstate.RemElem(id);
//  csigma.SetField(pos_STKM_CPUSigma_threadstate, threadstate);
    ((Map &)(csigma.GetFieldRef(pos_STKM_CPUSigma_threadstate))).RemElem(id);
  }

#ifdef VICE
//  Set threads (csigma.GetSet(pos_STKM_CPUSigma_threads));
//  threads.RemElem(id);
//  csigma.SetField(pos_STKM_CPUSigma_threads, threads);
  ((Set &)(csigma.GetFieldRef(pos_STKM_CPUSigma_threads))).ImpDiff(mk_set(id));
#endif // VICE

  this->cpustate.ImpModify(this->curcpu, csigma);
}

// SaveCurrentState
// es : STKM`EvaluatorStatus
// ==> ()
void SYSTEM::SaveCurrentState(const EvaluatorStatusCt & es)
{
//#ifdef VICE
//wcout << L"SaveCurrentState: " << PrintCPU(GetCurCPU()) << L" " << theScheduler().CurThreadId() << endl;
//#endif // VICE
  TYPE_STKM_CPUSigma csigma (this->cpustate[this->curcpu]);
  const Generic & curthread (csigma.GetField(pos_STKM_CPUSigma_curthread));
  if ( !curthread.IsNil() ) {
//    Map threadstate (csigma.GetMap(pos_STKM_CPUSigma_threadstate)); // %%%%%
//    threadstate.ImpModify(curthread, es);
//    csigma.SetField(pos_STKM_CPUSigma_threadstate, threadstate); // %%%%%
    ((Map &)(csigma.GetFieldRef(pos_STKM_CPUSigma_threadstate))).ImpModify(curthread, es);
    this->cpustate.ImpModify(this->curcpu, csigma);
  }
  else {
    vdm_iplog << L"SaveCurrentState::no curthread." << endl;
  }
}

// GetCurrentState
// ==> bool * [EvaluatorStatus] * [SubProgram]
EvaluatorStatusCt SYSTEM::GetCurrentState() const
{
//#ifdef VICE
//wcout << L"GetCurrentState: " << PrintCPU(GetCurCPU()) << L" " << theScheduler().CurThreadId() << endl;
//#endif // VICE
  if( this->cpustate.DomExists(this->curcpu) ) {
    TYPE_STKM_CPUSigma csigma (this->cpustate[this->curcpu]);
    const Generic & curthread (csigma.GetField(pos_STKM_CPUSigma_curthread));
    if( !curthread.IsNil() ) {
      const Map & threadstate (csigma.GetMap(pos_STKM_CPUSigma_threadstate));
      if (threadstate.DomExists(curthread)) {
        return (const EvaluatorStatusCt &)(threadstate[curthread]);
      }
    }
  }
  vdm_iplog << L"GetCurrentState::no curthread." << endl;
  return EvaluatorStatusCt();
}

// SaveCurrentProgram
// code : STKM`SubProgram
// ==> ()
void SYSTEM::SaveCurrentProgram(const TYPE_STKM_SubProgram & code)
{
  TYPE_STKM_CPUSigma csigma (this->cpustate[this->curcpu]);
  csigma.SetField(pos_STKM_CPUSigma_curr_uprogram, code);
  this->cpustate.ImpModify(this->curcpu, csigma);
}

// GetCurrentProgram
// ==> STKM`SubProgram
TYPE_STKM_SubProgram SYSTEM::GetCurrentProgram() const
{
  TYPE_STKM_CPUSigma csigma (this->cpustate[this->curcpu]);
  return csigma.GetSequence(pos_STKM_CPUSigma_curr_uprogram);
}

#ifdef VICE
// AddCPU
// sysnm : [AS`Name]
// cpunm : AS`Name
// decl : [AS`Expr]
// ==> ()
void SYSTEM::AddCPU(const Generic & sysnm, const TYPE_AS_Name & cpunm, const Generic & decl)
{
  this->cpustate.ImpModify(cpunm, EmptyCPU(decl));
  if( !decl.IsNil() && !sysnm.IsNil() ) {
    Set cpus (this->syscpus.DomExists(sysnm) ? Set(this->syscpus[sysnm]) : Set());

    cpus.Insert(cpunm);
    this->syscpus.ImpModify(sysnm, cpus);
  }
  theScheduler().AddCPU(cpunm);
  TIMETRACE::AddCPU(sysnm, cpunm, Bool(!decl.IsNil()));
}

// AddBUS
// busnm : AS`Name
// decl : [AS`Expr]
// ==> ()
void SYSTEM::AddBUS(const TYPE_AS_Name & busnm, const Generic & decl)
{
  TYPE_STKM_BUSSigma bsigma (EmptyBUS(decl));
  Set cpus (bsigma.GetSet(pos_STKM_BUSSigma_cpus));
  this->busstate.ImpModify(busnm, bsigma);
  TIMETRACE::AddBUS(busnm, cpus);
}

// AddVirtualBUS
// ==> AS`Name
void SYSTEM::AddVirtualBUS()
{
  TYPE_AS_Name virtualbusid (ASTAUX::MkName(VBUS));
  AddBUS(virtualbusid, Nil());
}

// AddToVirtualBUS
// newcpuid : AS`Name
// ==> ()
void SYSTEM::AddToVirtualBUS(const TYPE_AS_Name & newcpuid)
{
  TYPE_AS_Name virtualbusid (ASTAUX::MkName(VBUS));
  TYPE_STKM_BUSSigma bsigma (this->busstate[virtualbusid]);
  Set cpus (bsigma.GetSet(pos_STKM_BUSSigma_cpus));
  cpus.Insert(newcpuid);
  bsigma.SetField(pos_STKM_BUSSigma_cpus, cpus);
  this->busstate.ImpModify(virtualbusid, bsigma);
}

// AddThreadIdtoCPU
// threadid : SCHDTP`ThreadId
// objref : [SEM`OBJ_Ref]
// ==> ()
void SYSTEM::AddThreadIdtoCPU (const TYPE_SCHDTP_ThreadId & threadid, const Generic & objref)
{
  Generic cpu = this->curcpu;
  if( !objref.IsNil() && this->allocated.DomExists(objref) ) {
    cpu = this->allocated[objref];
  }
//vdm_iplog << L"AddThreadIdtoCPU: TID:" << threadid << L" " << PrintCPU(cpu) << L" OID:" << theState().PrintOID(objref) << endl;

  TYPE_STKM_CPUSigma csigma (this->cpustate[cpu]);

  Set threads (csigma.GetSet(pos_STKM_CPUSigma_threads));
  threads.Insert(threadid);
  csigma.SetField(pos_STKM_CPUSigma_threads, threads);

  if( !objref.IsNil() && !this->allocated.DomExists(objref) ) {
//vdm_iplog << L"Modify allocated:    " << PrintCPU(cpu) << L" OID:" << theState().PrintOID(objref) << endl;

    this->allocated.ImpModify(objref, cpu);

    Set objrefs (csigma.GetSet(pos_STKM_CPUSigma_objrefs));
    objrefs.Insert(objref);
    csigma.SetField(pos_STKM_CPUSigma_objrefs, objrefs);
  }
  this->cpustate.ImpModify(cpu, csigma);
}

// IncrRelTime
// t : real
void SYSTEM::IncrRelTime (const Real & t)
{
  this->IncrAbsTime(Int((int)(Settings.GetTimeFactor() * t.GetValue())));
}

// IncrAbsTime
// t : nat
void SYSTEM::IncrAbsTime (const Int & t)
{
  TYPE_STKM_CPUSigma csigma (this->cpustate[this->curcpu]);
  int time = csigma.GetIntValue(pos_STKM_CPUSigma_time);
  int idletime = csigma.GetIntValue(pos_STKM_CPUSigma_idletime);
  int curtime = (idletime > time) ? idletime : time;
  csigma.SetField(pos_STKM_CPUSigma_time, Int(curtime + t.GetValue()));
  this->cpustate.ImpModify(this->curcpu, csigma);

  // this->RecordTime();
}

// IncrCycleTime
// c : nat
// ==> ()
void SYSTEM::IncrCycleTime(const Real & c)
{
  TYPE_STKM_CPUSigma csigma (this->cpustate[this->curcpu]);
  int time = csigma.GetIntValue(pos_STKM_CPUSigma_time);
  int idletime = csigma.GetIntValue(pos_STKM_CPUSigma_idletime);
  int curtime = (idletime > time) ? idletime : time;
  csigma.SetField(pos_STKM_CPUSigma_time, Int(curtime + TimeForClockCycles(c)));
  this->cpustate.ImpModify(this->curcpu, csigma);

  // this->RecordTime();
}

// TimeForClockCycles
// c : nat
// ==> nat
int SYSTEM::TimeForClockCycles(const Real & c) const
{
  Generic cap (GetCPUCapacity(this->curcpu));
  if( this->curcpu.IsNil() ||
      cap.IsQuote() ) { // <INFINITE> 
    return 0;
  }
  else {
    double ct = c.GetValue();
    double cc = Int(cap).GetValue();
    return ((int)(ct/cc) + 1);
  }
}

// IncrIdleTime
// cpunm : [AS`Name]
// delta : nat
// ==> ()
void SYSTEM::IncrIdleTime(const Generic & cpunm, int delta)
{
  TYPE_STKM_CPUSigma csigma (this->cpustate[cpunm]);
  int curtime = MaxVal(csigma.GetIntValue(pos_STKM_CPUSigma_idletime), csigma.GetIntValue(pos_STKM_CPUSigma_time));
  csigma.SetField(pos_STKM_CPUSigma_idletime, Int(curtime + delta));
  this->cpustate.ImpModify(cpunm, csigma);
}

// InDuration
// ==> bool
bool SYSTEM::InDuration () const
{
  TYPE_STKM_CPUSigma csigma (this->cpustate[this->curcpu]);
  int dur = csigma.GetIntValue(pos_STKM_CPUSigma_dur);
  int cycle = csigma.GetIntValue(pos_STKM_CPUSigma_cycle);
  return ( (dur > 0) || (cycle > 0) );
}

// GetTime
// ==> real
int SYSTEM::GetTime () const
{
  return GetTimeOfCPU(this->curcpu);
}

int SYSTEM::GetTimeOfCPU (const Generic & cpu) const
{
  if ( !this->cpustate.DomExists(cpu) ) {
    return GetTimeOfCPU(this->curcpu);
  }
  TYPE_STKM_CPUSigma csigma (this->cpustate[cpu]);
  int idletime = csigma.GetIntValue(pos_STKM_CPUSigma_idletime);
  int time = csigma.GetIntValue(pos_STKM_CPUSigma_time);

  if ( idletime < time ) {
    return time;
  }
  else {
    return idletime;
  }
}

void SYSTEM::PushDuration()
{
  TYPE_STKM_CPUSigma csigma (this->cpustate[this->curcpu]);
  int dur = csigma.GetIntValue(pos_STKM_CPUSigma_dur);
  csigma.SetField(pos_STKM_CPUSigma_dur, Int(dur + 1));
  this->cpustate.ImpModify(this->curcpu, csigma);
}

void SYSTEM::PopDuration(const Real & t)
{
  TYPE_STKM_CPUSigma csigma (this->cpustate[this->curcpu]);
  int dur = csigma.GetIntValue(pos_STKM_CPUSigma_dur);
  csigma.SetField(pos_STKM_CPUSigma_dur, Int(dur - 1));
  this->cpustate.ImpModify(this->curcpu, csigma);
  if ( !InDuration() ) {
    IncrAbsTime(Int(t));
  }
}

void SYSTEM::PushCycle()
{
  TYPE_STKM_CPUSigma csigma (this->cpustate[this->curcpu]);
  int cycle = csigma.GetIntValue(pos_STKM_CPUSigma_cycle);
  csigma.SetField(pos_STKM_CPUSigma_cycle, Int(cycle + 1));
  this->cpustate.ImpModify(this->curcpu, csigma);
}

void SYSTEM::PopCycle(const Real & c)
{
  TYPE_STKM_CPUSigma csigma (this->cpustate[this->curcpu]);
  int cycle = csigma.GetIntValue(pos_STKM_CPUSigma_cycle);
  csigma.SetField(pos_STKM_CPUSigma_cycle, Int(cycle - 1));
  this->cpustate.ImpModify(this->curcpu, csigma);

  if ( !InDuration() ) {
    IncrCycleTime(c);
  }
}

int SYSTEM::GetCPUTime(const Generic & cpunm) const
{
  if(this->cpustate.DomExists(cpunm)) {
    const TYPE_STKM_CPUSigma & csigma (this->cpustate[cpunm]);
    return MaxVal(csigma.GetIntValue(pos_STKM_CPUSigma_time), csigma.GetIntValue(pos_STKM_CPUSigma_idletime));
  }
  return -1;
}

void SYSTEM::SetCPUTime(const Generic & cpunm, int newtime)
{
  if(this->cpustate.DomExists(cpunm)) {
    TYPE_STKM_CPUSigma csigma (this->cpustate[cpunm]);
    int time = csigma.GetIntValue(pos_STKM_CPUSigma_time);
    csigma.SetField(pos_STKM_CPUSigma_time, Int(MaxVal(time, newtime)));
    this->cpustate.ImpModify(cpunm, csigma);
  }
}

// CPUSwapNeeded
// cpuid : AS`Name
// ==> bool
bool SYSTEM::CPUSwapNeeded(const Generic & cpuid) const
{
  if (this->cpustate.DomExists(cpuid)) {
    const TYPE_STKM_CPUSigma & csigma (this->cpustate[cpuid]);
    return ( !theScheduler().CheckingGuard() &&
             (csigma.GetIntValue(pos_STKM_CPUSigma_time) > csigma.GetIntValue(pos_STKM_CPUSigma_swaptime) + 2) &&
             MultCPUs());
  }
  return false;
}

bool SYSTEM::CPUSwapNeeded() const
{
  return CPUSwapNeeded(this->curcpu);
}

// MultCPUs
// ==> bool
bool SYSTEM::MultCPUs() const
{
  return (this->cpustate.Size() > 1);
}

// GetPrimaryAlgorithm
// cpu : [AS`Name]
// ==> SCHDTP`PrimarySchedulerAlgorithm
TYPE_SCHDTP_PrimarySchedulerAlgorithm SYSTEM::GetPrimaryAlgorithm(const Generic & cpu) const
{
  if (this->cpustate.DomExists(cpu)) {
    const TYPE_STKM_CPUSigma & csigma (this->cpustate[cpu]);
    return csigma.GetRecord(pos_STKM_CPUSigma_schd);
  }
  else {
    return TYPE_SCHDTP_PureCooperative();
  }
}

// SetPrimaryAlgorithm
// a : SCHDTP`PrimarySchedulerAlgorithm
// cpu : [AS`Name]
void SYSTEM::SetPrimaryAlgorithm(const TYPE_SCHDTP_PrimarySchedulerAlgorithm & a, const Generic & cpu)
{
  if (this->cpustate.DomExists(cpu)) {
    TYPE_STKM_CPUSigma csigma (this->cpustate[cpu]);
    csigma.SetField(pos_STKM_CPUSigma_schd, a);
    this->cpustate.ImpModify(cpu, csigma);
  }
}

// GetSecondaryAlgorithm (not in spec)
// cpu : [AS`Name]
// ==> SCHDTP`SecondarySchedulerAlgorithm
TYPE_SCHDTP_SecondarySchedulerAlgorithm SYSTEM::GetSecondaryAlgorithm(const Generic & cpu) const
{
  if (this->cpustate.DomExists(cpu)) {
    const TYPE_STKM_CPUSigma & csigma (this->cpustate[cpu]);
    return csigma.GetRecord(pos_STKM_CPUSigma_prio);
  }
  else {
    return TYPE_SCHDTP_RoundRobin();
  }
}

// GetCurCPU
// ==> [AS`Name]
Generic SYSTEM::GetCurCPU() const
{
  return this->curcpu;
}

// GetAllCPUs
// ==> set of [AS`Name]
Set SYSTEM::GetAllCPUs() const
{
  return this->cpustate.Dom();
}

// SetCurCPU
// cpu : [AS`Name]
// swap : nat
// ==> ()
void SYSTEM::SetCurCPU(const Generic & cpu, const Int & swap )
{
//wcout << L"SetCurCPU: " << PrintCPU(this->curcpu) << L" -> " << PrintCPU(cpu) << endl;
//wcout << theScheduler().CurThreadId() << endl;
  this->curcpu = cpu;
  TYPE_STKM_CPUSigma csigma (this->cpustate[this->curcpu]);
  csigma.SetField(pos_STKM_CPUSigma_swaptime, swap);
  this->cpustate.ImpModify(this->curcpu, csigma);
//wcout << theScheduler().CurThreadId() << endl;
}

// OnCurCPU
// obj_ref : [SEM`OBJ_Ref]
// ==> bool
bool SYSTEM::OnCurCPU(const Generic & obj_ref) const
{
  if( this->curcpu.IsNil() && !this->allocated.DomExists(obj_ref)) {
    return true;
  }
  const TYPE_STKM_CPUSigma & csigma (this->cpustate[this->curcpu]);
  return csigma.GetSet(pos_STKM_CPUSigma_objrefs).InSet(obj_ref);
}

// CPUAllocated
// objref : SEM`OBJ_Ref
// ==> [AS`Name]
Generic SYSTEM::CPUAllocated(const TYPE_SEM_OBJ_uRef & objref) const
{
  if( this->allocated.DomExists(objref) ) {
    return this->allocated[objref];
  }
  else {
    RTERR::Error(L"SYSTEM::CPUAllocated", RTERR_INTERNAL_ERROR, Nil(), Nil(), Sequence());
  }
  return Nil(); // dummy
}

// AddMessageToBUS
// objref : [SEM`OBJ_Ref]
// opsem : SEM`ExplOP
// arg_lv : seq of SEM`VAL
// ==> MessageId
TYPE_STKM_MessageId SYSTEM::AddMessageToBUS(const Generic & objref,
                                            const TYPE_SEM_ExplOP & opsem,
                                            const Sequence & arg_lv)
{
//wcout << L"AddMessageToBUS: " << endl;
  TYPE_STKM_MessageId mesid (AddNewMessageId());

  TYPE_STKM_CPUSigma csigma (this->cpustate[this->curcpu]);

  Generic threadid = Nil();
  if ( opsem.GetBoolValue(pos_SEM_ExplOP_sync) ) {
    threadid = csigma.GetField(pos_STKM_CPUSigma_curthread);
  }
  Int prio (theScheduler().GetThreadPriority());

//  TYPE_STKM_MessageInfo m_info;
//  m_info.Init(objref, opsem.get_fnName(), arg_lv, threadid, this->curcpu);
  TYPE_AS_Name fullnm (AUX::ConstructDoubleName(opsem.get_modName(), opsem.get_fnName()));
  TYPE_STKM_MessageInfo m_info;
  m_info.Init(objref, fullnm, arg_lv, threadid, this->curcpu);

  TYPE_STKM_Message mes;
  mes.Init(mesid, GetTime(), prio, m_info, Int(1));

  Generic calledcpunm;
  if ( !this->allocated.DomExists(objref, calledcpunm) ) {
    calledcpunm = theState().GetCPUOfObjRef(objref);
  }
  Generic busid (FindBUSFromClass(calledcpunm));
  Int size (SemRec::SizeValSeq(arg_lv));

  Generic tcpu = Nil();
  if ( !objref.IsNil() ) {
    if ( this->allocated.DomExists(objref) ) {
      tcpu = this->allocated[objref];
    }
    else {
      tcpu = theState().GetCPUOfObjRef(objref);
    }
  }

//  TYPE_AS_Name fullnm (AUX::ConstructDoubleName(opsem.get_modName(), opsem.get_fnName()));
  Int time (theSystem().GetTime());

//wcout << L"AddMessageToBUS:       MID:" << mesid << L" " << PrintCurThread() << L" ==> " << PrintCPU(tcpu) << L" " << ASTAUX::ASName2String(fullnm) << endl;

  TYPE_STKM_MessageReq mr;
  mr.Init(busid, this->curcpu, tcpu, mesid, csigma.GetField(pos_STKM_CPUSigma_curthread), fullnm, objref, size, time);

  InsertMessageRequest(mr);  // Modify busstate !!

  TYPE_STKM_BUSSigma bsigma (this->busstate[busid]);
  MAP<TYPE_STKM_MessageId, TYPE_STKM_Message> mes_m (bsigma.GetMap(pos_STKM_BUSSigma_mes_um));
  mes_m.ImpModify(mesid, mes);
  bsigma.SetField(pos_STKM_BUSSigma_mes_um, mes_m);
  this->busstate.ImpModify(busid, bsigma);

  StepBUSTimeBack(busid, time);

  if( !threadid.IsNil() ) {
    Set syncopcall (csigma.GetSet(pos_STKM_CPUSigma_syncopcall));
    syncopcall.Insert(mesid);
    csigma.SetField(pos_STKM_CPUSigma_syncopcall, syncopcall);
    this->cpustate.ImpModify(this->curcpu, csigma);
  }
  return mesid;
}

// InsertMessageRequest
// mesreq : MessageReq | MessageReplyReq
// ==> ()
void SYSTEM::InsertMessageRequest(const Record & mesreq)
{
  Generic busid;
  Int time;
  switch(mesreq.GetTag()) {
    case TAG_TYPE_STKM_MessageReq: {
      busid = mesreq.GetField(pos_STKM_MessageReq_busid);
      time = mesreq.GetInt(pos_STKM_MessageReq_time);
      break;
    }
    case TAG_TYPE_STKM_MessageReplyReq: {
      busid = mesreq.GetField(pos_STKM_MessageReplyReq_busid);
      time = mesreq.GetInt(pos_STKM_MessageReplyReq_time);
      break;
    }
    default: {
      break;
    }
  }
  TYPE_STKM_BUSSigma bsigma (this->busstate[busid]);
  Sequence waiting (bsigma.GetSequence(pos_STKM_BUSSigma_waiting));

  if( waiting.IsEmpty() ) {
    waiting.ImpAppend(mesreq);
  }
  else {
    int index = 0;
    Generic g;
    for(bool bb = waiting.First(g); bb; bb = waiting.Next(g)) {
      Record r (g);
      Int ttime;
      switch(r.GetTag()) {
        case TAG_TYPE_STKM_MessageReq: {
          ttime = r.GetInt(pos_STKM_MessageReq_time);
          break;
        }
        case TAG_TYPE_STKM_MessageReplyReq: {
          ttime = r.GetInt(pos_STKM_MessageReplyReq_time);
          break;
        }
        default: {
          break;
        }
      }
      
      if (ttime.GetValue() > time.GetValue()) {
        break;
      }
      
      index++;
    }
    Sequence new_waiting;
    new_waiting.ImpConc(waiting.SubSequence(1, index));
    new_waiting.ImpAppend(mesreq);
    new_waiting.ImpConc(waiting.SubSequence(index + 1, waiting.Length()));
    waiting = new_waiting;
  }
  bsigma.SetField(pos_STKM_BUSSigma_waiting, waiting);
  this->busstate.ImpModify(busid, bsigma);
}

// AddBroadcastMessageToBUS
// objref : [SEM`OBJ_Ref]
// opsem : SEM`ExplOP
// arg_lv : seq of SEM`VAL
// ==> ()
void SYSTEM::AddBroadcastMessageToBUS(const Generic & objref,
                                      const TYPE_SEM_ExplOP & opsem,
                                      const SEQ<TYPE_SEM_VAL> & arg_lv)
{
  TYPE_STKM_MessageId mesid (AddNewMessageId());
  Int prio (theScheduler().GetThreadPriority());

  const TYPE_AS_Name & clnm (opsem.GetRecord(pos_SEM_ExplOP_modName));
  SET<TYPE_SEM_OBJ_uRef> objs (theState().GetObjRefsOfClass(clnm));

  Map busm; // map SEM`OBJ_Ref to [AS`Name]
  Map tcpum; // map SEM`OBJ_Ref to [AS`Name]
  Generic obj;
  for (bool bb = objs.First(obj); bb; bb = objs.Next(obj)) {
    Generic cpunm;
    if (this->allocated.DomExists(obj)) {
      cpunm = this->allocated[obj];
    }
    else {
      cpunm = theState().GetCPUOfObjRef(obj);
    }
    busm.ImpModify(obj, FindBUSFromClass(cpunm));
    tcpum.ImpModify(obj, cpunm);
  }

  Map busobjs; // map [AS`Name] to set of SEM`OBJ_Ref
  Set dom_busm (busm.Dom());
  Generic ref;
  for (bool cc = dom_busm.First(ref); cc; cc = dom_busm.Next(ref)) {
    const Generic & name (busm[ref]);
    Set s (busobjs.DomExists(name) ? Set(busobjs[name]) : Set());
    s.Insert(ref);
    busobjs.ImpModify(name, s);
  }


//  Map tcpum; // map SEM`OBJ_Ref to [AS`Name]
//  for (bool dd = objs.First(obj); dd; dd = objs.Next(obj))
//  {
//    Generic cpunm;
//    if (this->allocated.DomExists(obj))
//      cpunm = this->allocated[obj];
//    else
//      cpunm = theState().GetCPUOfObjRef(objref);
//    tcpum.ImpModify(obj, cpunm);
//  }

  Map buscpus; // map [AS`Name] to set of [AS`Name]
  Set dom_busobjs (busobjs.Dom());
  Generic busid;
  for (bool ee = dom_busobjs.First(busid); ee; ee = dom_busobjs.Next(busid)) {
    SET<TYPE_SEM_OBJ_uRef> objs (busobjs[busid]);
    Set tcpus; //set of [AS`Name]
    Generic g;
    for (bool ff = objs.First(g); ff; ff = objs.Next(g)) {
      tcpus.Insert(tcpum[g]);
    }
    buscpus.ImpModify(busid, tcpus);
  }

  TYPE_AS_Name fullnm (AUX::ConstructDoubleName(opsem.get_modName(),opsem.get_fnName()));
  int time = GetTime();
  int size = SemRec::SizeValSeq(arg_lv);
  
  for (bool gg = dom_busobjs.First(busid); gg; gg = dom_busobjs.Next(busid)) {
    SET<TYPE_SEM_OBJ_uRef> obj_s (busobjs[busid]); 
    Set tcpus (buscpus[busid]);
    if ( tcpus.InSet(this->curcpu) ) {
      tcpus.RemElem(this->curcpu);
    }
    TIMETRACE::LogBroadMessageReq(busid, this->curcpu, tcpus, mesid, fullnm, obj_s, size, time);

    SET<TYPE_SEM_OBJ_uRef> objsothercpus ;
    Generic obj;
    for (bool hh = obj_s.First(obj); hh; hh = obj_s.Next(obj)) {
      if( tcpum[obj] != this->curcpu ) {
        objsothercpus.Insert(obj);
      }
    }
    TYPE_STKM_MessageBroadcast m_info;
    m_info.Init(objsothercpus, fullnm, arg_lv, tcpum, curcpu);
    TYPE_STKM_Message mes;
    mes.Init(mesid, Int(time), prio, m_info, Int(1));

    if (!objsothercpus.IsEmpty()) {
      TYPE_STKM_BUSSigma bsigma (this->busstate[busid]);
      MAP<TYPE_STKM_MessageId, TYPE_STKM_Message> mes_m (bsigma.GetMap(pos_STKM_BUSSigma_mes_um));
      mes_m.ImpModify(mesid, mes);
      bsigma.SetField(pos_STKM_BUSSigma_mes_um, mes_m);
      this->busstate.ImpModify(busid, bsigma);
    }
    StepBUSTimeBack(busid, time);
  }
}

// AddReturnMessageToBUS
// mesid : MessageId
// t : nat
// prio : nat
// val : SEM`VAL
// opnm : AS`Name
// tcpu : [AS`Name]
// fromthread : SCHDTP`ThreadId
// ==> ()
void SYSTEM::AddReturnMessageToBUS(const TYPE_STKM_MessageId & mesid,
                                   const Int & t, const Int & prio,
                                   const TYPE_SEM_VAL & val, const TYPE_AS_Name & opnm,
                                   const Generic & tcpu,
                                   const TYPE_SCHDTP_ThreadId & fromthread)
{
//wcout << L"AddReturnMessageToBUS: MID:" << mesid << L" " << PrintCurThread() << L" ==> " << PrintThread(fromthread,tcpu) << L" " << ASTAUX::ASName2String(opnm) << endl;

  TYPE_STKM_CPUSigma csigma (this->cpustate[this->curcpu]);
  const Generic & curthread (csigma.GetField(pos_STKM_CPUSigma_curthread));
  TYPE_STKM_MessageResponse mesres;
  mesres.Init(opnm, val, tcpu, fromthread);
  Generic busid (FindBUSFromClass(tcpu));
  Int size (SemRec::Size(val));
  TYPE_STKM_BUSSigma bsigma (this->busstate[busid]);
  Int dura (CalcDuration(size, bsigma.get_capacity()));
  TYPE_STKM_MessageId newmesid (AddNewMessageId());
  TYPE_STKM_Message mes;
  mes.Init(newmesid, t + dura, prio, mesres, Int(1));
  Int time (theSystem().GetTime());

  TYPE_STKM_MessageReplyReq mrr;
  mrr.Init(busid, this->curcpu, tcpu, newmesid, mesid, curthread, fromthread, size, time);

  InsertMessageRequest(mrr); // Modify busstate !!

//wcout << L"AddReturnMessageToBUS 2" << endl;
  TYPE_STKM_BUSSigma nbsigma (this->busstate[busid]);
  Map mes_m (nbsigma.GetMap(pos_STKM_BUSSigma_mes_um));
  mes_m.ImpModify(newmesid, mes);
  nbsigma.SetField(pos_STKM_BUSSigma_mes_um, mes_m);
  this->busstate.ImpModify(busid, nbsigma);

//wcout << L"AddReturnMessageToBUS 3" << endl;
  StepBUSTimeBack(busid,time);

//wcout << L"AddReturnMessageToBUS 4" << endl;
/*
//  Map threadstate (csigma.get_threadstate());    // %%%%%
//  EvaluatorStatusCt ev_cur (threadstate[curthread]);
//  ev_cur.get_shared_ref().pc = csigma.get_curr_program().Length();
//  threadstate.ImpModify(curthread, ev_cur);
//  csigma.set_threadstate(threadstate);           // %%%%%
//  this->cpustate.ImpModify(this->curcpu, csigma);
*/

//wcout << L"AddReturnMessageToBUS 5" << endl;
  theScheduler().UpdateThreadWithReturnResult(val, fromthread);

//wcout << L"AddReturnMessageToBUS 6" << endl;
  TYPE_STKM_CPUSigma csigma_to (this->cpustate[tcpu]);
  Map tothreadstate (csigma_to.GetMap(pos_STKM_CPUSigma_threadstate));
  EvaluatorStatusCt ev_from ((const EvaluatorStatusCt &)tothreadstate[fromthread]);
  EvaluatorStatus & state = ev_from.get_oneref_ref(); // OK
  state.eval_stack.Pop();
  state.eval_stack.Pop();
  state.pc += 3;
  tothreadstate.ImpModify(fromthread, ev_from);
  csigma_to.SetField(pos_STKM_CPUSigma_threadstate, tothreadstate);
  this->cpustate.ImpModify(tcpu, csigma_to);

/*
//  if( !ev_cur.get_const_ref().call_stack.IsEmpty() )
//  {
//    ev_cur.get_shared_ref().call_stack.Pop();
//    threadstate.ImpModify(curthread, ev_cur);
//  }
//  csigma.set_threadstate(threadstate);
//  this->cpustate.ImpModify(this->curcpu, csigma);
*/
//wcout << L"AddReturnMessageToBUS End" << endl;
}

// StepBUSTimeBack
// busid : AS`Name
// time : nat
// ==> ()
void SYSTEM::StepBUSTimeBack(const TYPE_AS_Name & busid, const Int & time)
{
  TYPE_STKM_BUSSigma bsigma (this->busstate[busid]);
  const Sequence & log (bsigma.GetSequence(pos_STKM_BUSSigma_log));

  if ( log.IsEmpty() ) {
    bsigma.SetField(pos_STKM_BUSSigma_time, time);
    this->busstate.ImpModify(busid, bsigma);
  }
  else {
    TYPE_GLOBAL_ChanUsage cu (log.Index(log.Length()));
    if ((cu.get_tot() < time) && (time < bsigma.GetInt(pos_STKM_BUSSigma_time))) {
      bsigma.SetField(pos_STKM_BUSSigma_time, time);
      this->busstate.ImpModify(busid, bsigma);
    }
  }
}

// StepCPUTimeBack
// cpuid : [AS`Name]
// time : nat ==> ()
void SYSTEM::StepCPUTimeBack(const Generic & cpuid, const Int & time)
{
  TYPE_STKM_CPUSigma csigma (this->cpustate[cpuid]);
  if( csigma.GetIntValue(pos_STKM_CPUSigma_time) < csigma.GetIntValue(pos_STKM_CPUSigma_idletime) ) {
    int t = MaxVal(csigma.GetIntValue(pos_STKM_CPUSigma_time), time.GetValue());
    csigma.SetField(pos_STKM_CPUSigma_time, Int(t));
    csigma.SetField(pos_STKM_CPUSigma_idletime, Int(t));
    this->cpustate.ImpModify(cpuid, csigma);
  }
}

// AddNewMessageId
// ==> nat
TYPE_STKM_MessageId SYSTEM::AddNewMessageId()
{
  this->nextmesid = this->nextmesid + Int(1);
  return this->nextmesid;
}

// FindBUSFromClass
// calledcpunm : [AS`Name]
// ==> [AS`Name]
Generic SYSTEM::FindBUSFromClass(const Generic & calledcpunm) const
{
  Generic cur_cpu = this->curcpu;

  Set dom_busstate (this->busstate.Dom());
  Set dom_syscpus (this->syscpus.Dom());
  Generic busid;
  for(bool bb = dom_busstate.First(busid); bb; bb = dom_busstate.Next(busid)) {
    TYPE_STKM_BUSSigma bsigma (this->busstate[busid]);
    const Set & cpus (bsigma.GetSet(pos_STKM_BUSSigma_cpus));
    if( cpus.InSet(calledcpunm) && cpus.InSet(cur_cpu) ) {
      Set c;
      c.Insert(cur_cpu);
      c.Insert(calledcpunm);
      
      Generic sysnm;
      bool check = true;
      for( bool cc = dom_syscpus.First(sysnm); cc && check; cc = dom_syscpus.Next(sysnm) ) {
        check &= !(c.SubSet(Set(this->syscpus[sysnm])));
      }

      TYPE_AS_Name virtualbusid (ASTAUX::MkName(VBUS));
      if (((busid == virtualbusid) && check) || ((busid != virtualbusid) && !check)) {
        return busid;
      }
    }
  }
  
  RTERR::Error (L"FindBUSFromClass", RTERR_NOBUS, Nil(), Nil(), Sequence());
  return Nil(); // dummy;
}

// FindFirstCPU
// ==> [AS`Name] * nat * [nat]
Tuple SYSTEM::FindFirstCPU() const
{
  Set dom_cpustate (this->cpustate.Dom());
  Map t_m;
  Generic cpunm;
  for(bool bb = dom_cpustate.First(cpunm); bb; bb = dom_cpustate.Next(cpunm) ) {
    const TYPE_STKM_CPUSigma & csigma (this->cpustate[cpunm]);
    if( csigma.GetIntValue(pos_STKM_CPUSigma_time) < csigma.GetIntValue(pos_STKM_CPUSigma_idletime) ) {
      t_m.Insert(cpunm, csigma.GetInt(pos_STKM_CPUSigma_idletime));
    }
    else {
      t_m.Insert(cpunm, csigma.GetInt(pos_STKM_CPUSigma_time));
    }
  }
//wcout << t_m << endl;
  return SmallNameAndNextTime(t_m);
}

// FindFirstBUS
// ==> AS`Name * nat * [nat]
Tuple SYSTEM::FindFirstBUS() const
{
  Set dom_busstate (this->busstate.Dom());
  Map t_m;
  Generic busnm;
  for(bool bb = dom_busstate.First(busnm); bb; bb = dom_busstate.Next(busnm)) {
    const TYPE_STKM_BUSSigma & bsigma (this->busstate[busnm]);
    t_m.Insert(busnm, bsigma.GetInt(pos_STKM_BUSSigma_time));
  }
  return SmallNameAndNextTime(t_m);
}

// UpdateObjRef
// val : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL SYSTEM::UpdateObjRef(const TYPE_SEM_VAL & val_)
{
  TYPE_SEM_VAL val (val_);
  if( (val.Is(TAG_TYPE_SEM_CompExplFN) ||
       val.Is(TAG_TYPE_SEM_ExplPOLY) ||
       val.Is(TAG_TYPE_SEM_ExplOP)) &&
      !this->allocated.IsEmpty() ) {
    TYPE_AS_Name cl;
    switch(val.GetTag()) {
      case TAG_TYPE_SEM_CompExplFN: {
        SEQ<TYPE_SEM_ExplFN> fl (val.GetSequence(pos_SEM_CompExplFN_fl));
        TYPE_SEM_ExplFN efn (fl.Hd());
        cl = efn.GetRecord(pos_SEM_ExplFN_modName);
        break;
      }
      case TAG_TYPE_SEM_ExplPOLY: {
        cl = val.GetRecord(pos_SEM_ExplPOLY_modName);
        break;
      }
      case TAG_TYPE_SEM_ExplOP: {
        cl = val.GetRecord(pos_SEM_ExplOP_modName);
        break;
      }
      default: {
        RTERR::Error(L"SYSTEM::UpdateObjRef", RTERR_INTERNAL_ERROR, Nil(), Nil(), Sequence());
        break;
      }
    }
    
    Set dom_allocated (this->allocated.Dom());
    Generic obj_ref;
    for(bool cc = dom_allocated.First(obj_ref); cc; cc = dom_allocated.Next(obj_ref) ) {
      if( cl == Record(obj_ref).GetRecord(pos_SEM_OBJ_uRef_tp)) {
        switch(val.GetTag()) {
          case TAG_TYPE_SEM_CompExplFN: {
            val.SetField(pos_SEM_CompExplFN_objref, obj_ref);
            return val;
            break;
          }
          case TAG_TYPE_SEM_ExplPOLY: {
            val.SetField(pos_SEM_ExplPOLY_objref, obj_ref);
            return val;
            break;
          }
          case TAG_TYPE_SEM_ExplOP: {
            val.SetField(pos_SEM_ExplOP_objref, obj_ref);
            return val;
            break;
          }
          default: {
            break;
          }
        }
      }
    }
  }
  return val;
}

// SmallNameAndNextTime
// t_m : map [AS`Name] to nat
// -> [AS`Name] * nat * [nat]
Tuple SYSTEM::SmallNameAndNextTime(const Map & t_m) const
{
// change schan direction
//  Generic first (t_m.Dom().GetElem());
//  Int t_f (t_m[first]);
//  Generic f_g, ft_g;
//  for(bool bb = t_m.First(f_g,ft_g); bb; bb = t_m.Next(f_g,ft_g))
  Sequence t_l (ASTAUX::SetToSequenceR(t_m.Dom()));
  Generic first (t_l[1]);
  
// hack
  if (t_m.Dom().InSet(Nil())) {
    first = Nil();
  }
  Int t_f (t_m[first]);
  Generic f_g;
  for (bool bb = t_l.First(f_g); bb; bb = t_l.Next(f_g)) {
    Generic ft_g = t_m[f_g];
    if ( Int(ft_g) < t_f ) {
      first = f_g;
      t_f = ft_g;
    }
  }

  if ( t_m.Dom().Card() > 1) {
    Map t_m_2(t_m);
    t_m_2.RemElem(first);

    Set dom_t_m_2 (t_m_2.Dom());
    Generic next (dom_t_m_2.GetElem());
    Int t_n (t_m_2[next]);

    Generic n;
    for (bool bb = dom_t_m_2.First(n); bb; bb = dom_t_m_2.Next(n)) {
      Int nt (t_m_2[n]);
      if( nt < t_n ) {
        next = n;
        t_n = nt;
      }
    }
    return mk_(first, t_f, t_n);
  }
  else {
    return mk_(first, t_f, Nil());
  }
}

// AddNewObj
// ref : SEM`OBJ_Ref
// ==> ()
void SYSTEM::AddNewObj(const TYPE_SEM_OBJ_uRef & ref)
{
//wcout << L"AddNewObj: " << ref << endl;
//vdm_iplog << L"AddNewObj:                 OID:" << theState().PrintOID(ref) << endl;
  if (theStackMachine().HasCurObjRef()) {
    TYPE_SEM_OBJ_uRef curref (theStackMachine().GetCurObjRef());
    if (this->allocated.DomExists(curref)) {
      DeployInst(this->allocated[curref], ref);
    }
    else {
      TYPE_STKM_CPUSigma ncsigma (this->cpustate[Nil()]);
      TIMETRACE::LogObjAlloc(ref, Nil(), ncsigma.GetInt(pos_STKM_CPUSigma_time));
    }
  }
  else if ( !this->allocated.DomExists(ref) ) {
    TYPE_STKM_CPUSigma ncsigma (this->cpustate[Nil()]);
    TIMETRACE::LogObjAlloc(ref, Nil(), ncsigma.GetInt(pos_STKM_CPUSigma_time));
  }
}

bool SYSTEM::MultiThreads(const Generic & cpu) const
{
  if (this->cpustate.DomExists(cpu)) {
    TYPE_STKM_CPUSigma csigma (this->cpustate[cpu]);
    return (csigma.GetSet(pos_STKM_CPUSigma_threads).Card() > 1);
  }
  else {
    return false;
  }
}
#endif // VICE

