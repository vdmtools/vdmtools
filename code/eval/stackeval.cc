// {{{ Header

/***
*  * WHAT
*  *    Implementation of the core stack evaluator.
*  * Id
*  *    $Id: stackeval.cc,v 1.158 2006/10/14 01:42:21 vdmtools Exp $
*  *    stackeval.cc: Implementation of stack-eval.vdm 1.76
*  *    What about 1.1.2.50 -> 1.1.2.51 ???
***/

// }}}
// {{{ Includes
#include "metaiv.h"
#include "AS.h"
#include "stackeval.h"
#include "INSTRTP.h"
#include "rt_errmsg.h"
#include "libman.h"
#include "evalaux.h"
#include "astaux.h"
#include "intconvquotes.h"
#include "settings.h"
#include "tb_wstring.h"
#include "tbutils.h"
#include "tools.h"
#ifdef VDMPP
#include "SCHDTP.h"
#ifdef VICE
#include "TIMETRACE.h"
#include "valcond.h"
#endif //VICE
#endif // VDMPP

//wcout << L"intertp: " << INT2Q::h2gAS(intertp) << endl;

//#define PROFILING 1
#ifdef PROFILING
#include <time.h>
#ifndef _MSC_VER
#include <sys/time.h>
#endif // _MSC_VER
#define NUMINSTR 200
uint64_t instrcnt[NUMINSTR];
uint64_t instrclk[NUMINSTR];
time_t sttime;
#endif // PROFILING

// }}}
// {{{ The SubProgram class definitions
// }}}
// [[[ EvaluatorStatus
// StackEval constructors.

EvaluatorStatus::EvaluatorStatus () :
  eval_stack(TYPE_STKM_EvalStack()),
  call_stack(TYPE_STKM_CallStack()),
  curCid(NilContextId),
  trap_stack(TYPE_STKM_TrapStack()),
  pc(Int(0)),
  debug_flag(TYPE_STKM_Continue()),
  upDnIndex(Int(0)),
#ifdef VDMPP
  instrno(Int(0)),                   // VDMPP
  guard(Nil()),
#ifdef VICE
  status(TYPE_SCHDTP_Running()),
//  dur(Int(0)),
//  cycle(Int(0)),
  release_time(Nil()),
#endif //VICE
#endif // VDMPP
  cf(TYPE_STKM_CFStack()),

  env_l(envl_init()),
  env_l_bak(TYPE_SEM_ENVL()),

  typeinst(typeinst_init()),
  typeinst_bak(Sequence()),

#ifdef VDMSL
  os(Sequence()),
  os_bak(Sequence()),

  cur_mod(SEQ<TYPE_AS_Name>()),
  cur_mod_bak(SEQ<TYPE_AS_Name>()),
#endif // VDMSL

#ifdef VDMPP
  os(Sequence()),
  os_bak(Sequence()),

  obj_l(SEQ<TYPE_GLOBAL_OBJscope>()),
  obj_l_bak(SEQ<TYPE_GLOBAL_OBJscope>()),
#endif // VDMPP
  rterror(false),
  context(mk_sequence(Int(ALL)))
{
}

EvaluatorStatus::~EvaluatorStatus()
{
  eval_stack.Clear();
  call_stack.Clear();
  trap_stack.Clear();
  cf.Clear();
  env_l.Clear();
  env_l_bak.Clear();
  typeinst.Clear();
  typeinst_bak.Clear();
  os.Clear();
  os_bak.Clear();

#ifdef VDMSL
  cur_mod.Clear();
  cur_mod_bak.Clear();
#endif // VDMSL
#ifdef VDMPP
  obj_l.Clear();
  obj_l_bak.Clear();
#endif // VDMPP
}

Sequence EvaluatorStatus::typeinst_init()
{
  Sequence s;
  s.ImpAppend(Map());
  return s;
}

TYPE_SEM_ENVL EvaluatorStatus::envl_init()
{
  TYPE_SEM_ENVL s;
  s.ImpAppend(SEQ<TYPE_SEM_BlkEnv>());
  return s;
}

TYPE_STKM_DebugCmd::TYPE_STKM_DebugCmd() : Record(TAG_TYPE_STKM_DebugCmd, 1) {}
TYPE_STKM_DebugCmd::TYPE_STKM_DebugCmd(const Common &c) : Record(c) {}
TYPE_STKM_DebugCmd &TYPE_STKM_DebugCmd::Init(const TYPE_STKM_SubProgram & p1) { SetField(1,p1); return *this; }
TYPE_STKM_SubProgram TYPE_STKM_DebugCmd::get_instr() const { return GetSequence(1); }
void TYPE_STKM_DebugCmd::set_instr(const TYPE_STKM_SubProgram &p) { SetField(1, p); }

// ]]]
// {{{ StackEval

#ifdef VDMPP
TYPE_GLOBAL_Start StackEval::global_start;
#endif // VDMPP

// {{{ Instantiating and Storing the State of the Evaluator

// Instantiate
// es2 : EvaluatorStatus
// instr : [SubProgram]
void StackEval::Instantiate(const EvaluatorStatusCt & es2, const Generic & instr)
{
//wcout << L"Instantiate: " << theScheduler().CurThreadId() << endl;
  // create new EvaluatorStatus
  EvaluatorStatusCt stateCt (es2);
  EvaluatorStatus & state (stateCt.get_shared_ref());  

  // setup program
  TYPE_STKM_SubProgram program;
  if ( !instr.IsNil() ) {
    program = instr;
    state.pc = 0;
  }
  else if (!state.call_stack.IsEmpty())
  {
    TYPE_STKM_CallStackItem csi (state.call_stack.Head());
    program = ExtractInstr(csi.GetField(pos_STKM_CallStackItem_code));
  }

  // 
  SetEvaluatorState(stateCt);
  SetProgram(program);
}

// ReplaceEvaluatorStatus
// p : EvaluatorStatus
void StackEval::ReplaceEvaluatorStatus(EvaluatorStatusCt & ev)
{
#ifdef VDMPP
  theScheduler().SetCurThreadState(ev);
#endif // VDMPP
  Instantiate(ev, Nil());
}

void StackEval::SetRunTimeError()
{
  this->cs_shared_p->rterror = true;
}

// RunTimeError
bool StackEval::RunTimeError() const
{
  return this->cs_shared_p->rterror;
}

// ThrowRuntimeErrorException
void StackEval::ThrowRuntimeErrorException()
{
  if (Settings.RTErrException())
  {
    // throw VDM level exception
    TYPE_SEM_VAL qe (TYPE_SEM_QUOTE().Init(SEQ<Char>(L"RuntimeError")));

    GotoTrapHandler(TYPE_SEM_EXIT().Init(qe));

    ToolMediator::Errs()->vdm_AddMsg(RTERR::GetErrMsg());
  }
  else
  {
    // throw C++ level exception
    SetRunTimeError();
    VDMErrorHandle(ERR_IP, 1);
  }
}

// IsRuntimeErrorException
// v : 
bool StackEval::IsRuntimeErrorException(const Generic & v) const
{
  if (v.Is(TAG_TYPE_SEM_EXIT))
  {
    Generic e (TYPE_SEM_EXIT(v).GetField(pos_SEM_EXIT_v));
//
    if (e.Is(TAG_TYPE_SEM_QUOTE))
    {
      SEQ<Char> str (TYPE_SEM_QUOTE(e).GetSequence(pos_SEM_QUOTE_v));
      return (str == SEQ<Char>(L"RuntimeError"));
    }
  }
  return false;
}

// RuntimeErrorExceptionOccurred
// ==> bool
bool StackEval::RuntimeErrorExceptionOccurred() const
{
  if (LenES(1)) {
    return IsRuntimeErrorException(Head());
  }
  return false;
}
 
// PushRuntimeErrorVal
// ==> SEM`EXIT
void StackEval::PushRuntimeErrorVal()
{
  TYPE_SEM_VAL qe (TYPE_SEM_QUOTE().Init(SEQ<Char>(L"RuntimeError")));
  Push(TYPE_SEM_EXIT().Init(qe));
}

// RuntimeErrorVal
// ==> SEM`EXIT
TYPE_SEM_EXIT StackEval::RuntimeErrorVal()
{
  TYPE_STKM_EvalStackItem item (Pop());  
  return item;
}

// GetEvaluatorState
// ==> STKM`EvaluatorState
EvaluatorStatusCt StackEval::GetEvaluatorState(void)
{
#ifdef VDMPP
  LoadCurrntEvaluatorStateFromCPUSigma();
#endif // VDMPP
  return this->curr_state;
}

// SetEvaluatorState (not in spec)
// st : STKM`EvaluatorStatus
void StackEval::SetEvaluatorState(const EvaluatorStatusCt & st)
{
  SetCurrentState(st);
#ifdef VDMPP
  SaveCurrntEvaluatorStateToCPUSigma();
#endif // VDMPP
}

// SetCurrentState (not in spec)
// st : STKM`EvaluatorStatus
void StackEval::SetCurrentState(const EvaluatorStatusCt & st)
{
  this->curr_state      = st;
  this->cs_shared_p     = &this->curr_state.get_shared_ref();
  this->cs_env_l_p      = &this->cs_shared_p->env_l;
  this->cs_eval_stack_p = &this->cs_shared_p->eval_stack;
  this->cs_call_stack_p = &this->cs_shared_p->call_stack;
  this->cs_typeinst_p   = &this->cs_shared_p->typeinst;
  this->cs_os_p         = &this->cs_shared_p->os;
#ifdef VDMSL
  this->cs_cur_mod_p    = &this->cs_shared_p->cur_mod;
#endif // VDMSL
#ifdef VDMPP
  this->cs_obj_l_p      = &this->cs_shared_p->obj_l;
#endif // VDMPP
  this->cs_context_p    = &this->cs_shared_p->context;
}

// SetCurrentProgram (not in spec)
// pr : STKM`SubProgram
void StackEval::SetCurrentProgram(const TYPE_STKM_SubProgram & pr)
{
  this->curr_program = pr;
  this->curr_program_length = pr.Length();
}

// GetProgram
TYPE_STKM_SubProgram StackEval::GetProgram()
{
#ifdef VDMPP
  LoadCurrntProgramFromCPUSigma();
#endif // VDMPP
  return this->curr_program;
}

// SetProgram (not in spec)
// pr : STKM`SubProgram
void StackEval::SetProgram(const TYPE_STKM_SubProgram & pr)
{
  SetCurrentProgram(pr);
#ifdef VDMPP
  SaveCurrntProgramToCPUSigma();
#endif // VDMPP
}

#ifdef VDMPP
// LoadCurrntEvaluatorStateFromCPUSigma (not in spec)
void StackEval::LoadCurrntEvaluatorStateFromCPUSigma()
{
#ifndef VICE
  if (UsesThreads()) // 20110328
#endif // VICE
    SetCurrentState(theSystem().GetCurrentState());
}

// SaveCurrntEvaluatorStateToCPUSigma (not in spec)
void StackEval::SaveCurrntEvaluatorStateToCPUSigma()
{
#ifndef VICE
  if (UsesThreads()) // 20110328
#endif // VICE
    theSystem().SaveCurrentState(this->curr_state);
}

// LoadCurrntProgramFromCPUSigma (not in spec)
void StackEval::LoadCurrntProgramFromCPUSigma()
{
#ifndef VICE
  if (UsesThreads()) // 20110328
#endif // VICE
    SetCurrentProgram(theSystem().GetCurrentProgram());
}

// SaveCurrntProgramToCPUSigma (not in spec)
void StackEval::SaveCurrntProgramToCPUSigma()
{
#ifndef VICE
  if (UsesThreads()) // 20110328
#endif // VICE
    theSystem().SaveCurrentProgram(this->curr_program);
}

// InitEvaluatorStatus
// instr : [ SubProgram ]
// objref : [ SEM`OBJ_Ref ]
// ==> EvaluatorStatus
EvaluatorStatusCt StackEval::InitEvaluatorStatus(const TYPE_STKM_SubProgram & instr, const Generic & objref) const
{
  // create new EvaluatorStatus
  EvaluatorStatusCt eCt;
  EvaluatorStatus & e (eCt.get_shared_ref());

  if (! objref.IsNil())
  {
    TYPE_AS_Name nm (Record(objref).GetRecord(pos_SEM_OBJ_uRef_tp));
    TYPE_GLOBAL_OBJscope objscope;
    objscope.Init(objref, SEQ<TYPE_AS_Name>().ImpAppend(nm), SEQ<TYPE_GLOBAL_OrigCl>().ImpAppend(nm));
    e.obj_l = SEQ<TYPE_GLOBAL_OBJscope>().ImpAppend(objscope);
  }
  else
  {
    // TODO
    // to create top environment object here ... ???
  }

  if( !instr.IsEmpty() )
  {
// 20130530 -->
    if (Settings.CallLog())
    {
      wstring id (L"Thread Start");
      if (! objref.IsNil())
        id += L" (" + ASTAUX::ASName2String(Record(objref).GetRecord(pos_SEM_OBJ_uRef_tp)) + L")";
      TOOLS::WriteCallLog(id, Sequence(), this->cs_call_stack_p->Length());
    }
// <-- 20130530

    TYPE_STKM_SubProgram code (instr);
    code.ImpAppend(TYPE_INSTRTP_EOCL());

    TYPE_STKM_CallStackItem csi;
    csi.Init(Int(CallStackItemType::CS_INTERNAL), // type
             TYPE_STKM_DebugCmd().Init(code),     // code
             Int(0),                              // pc
             SEQ<Char>(L"Thread Start"),          // nmOrDesc
             Nil(),                               // arg_l
             Nil(),                               // evalstate
             NilContextId,                        // oldCid
             Int(-1),                             // env_l_h
             Int(-1),                             // typeinst_h
             Int(-1),                             // os_h
             Int(-1),                             // mod_obj_l_h
             Bool(false),                         // rterror
             Nil());                              // measu

    e.call_stack.Push(csi);
    e.debug_flag = TYPE_STKM_Continue();
  }
  return eCt;
}
#endif //VDMPP

// ExtractInstr
// i : Code
// ==> SubProgram
TYPE_STKM_SubProgram StackEval::ExtractInstr(const TYPE_STKM_Code & i)
{
  switch(i.GetTag()) {
    case TAG_TYPE_STKM_DebugCmd: {
    return i.GetSequence(pos_STKM_DebugCmd_instr);
    }
    case TAG_TYPE_SEM_ExplFN: {
      return theCompiler().GetProgram(i.GetRecord(pos_SEM_ExplFN_modName), i.GetInt(pos_SEM_ExplFN_instr));
    }
    case TAG_TYPE_SEM_ExplOP: {
      return theCompiler().GetProgram(i.GetRecord(pos_SEM_ExplOP_modName), i.GetInt(pos_SEM_ExplOP_instr));
    }
    default: {
      RTERR::Error(L"CallStackItem::ExtractInstr", RTERR_INTERNAL_ERROR, Nil(), Nil(), Sequence());
      return TYPE_STKM_SubProgram(); // dummy
    }
  }
}

StackEval::StackEval()
{
#ifdef VDMSL
  // create new EvaluatorStatus
  SetCurrentState(EvaluatorStatusCt());
#endif // VDSLM

  SetCurrentProgram(TYPE_STKM_SubProgram());

#ifdef VDMPP
  this->lastres = sem_undef;
#endif //VDMPP

  this->lastexit = NilContextId;

  this->BREAK = false;

  this->void_name = ASTAUX::MkNameFromVoid();

  MUTEX_INIT( mutex_break );
}

StackEval::~StackEval()
{
  MUTEX_DESTROY( mutex_break );
}

// stackeval_Init
// ==> ()
void StackEval::stackeval_Init(bool ast_is_new)
{
  // initialize cache 
  // create new EvaluatorStatus
  SetCurrentState(EvaluatorStatusCt());
  SetCurrentProgram(TYPE_STKM_SubProgram());

//  if( ast_is_new )
    this->lastres = sem_undef;

  this->lastexit = NilContextId;

  this->BREAK = false;

  ResetInActivity();

#ifdef VDMPP
  SetUsesThreads(false);
#endif // VDMPP

  SetUsesOldName(true);
}

// User_Init
// ast : AS`Document
// ast_is_new : bool
// ==> ()
void StackEval::User_Init(const TYPE_AS_Document & ast, bool ast_is_new)
{
// 20071011
  ResetUserBREAK();

  this->stackeval_Init(ast_is_new);

#ifdef VDMPP
#ifdef VICE
  VC::InitState();
  if( !ast_is_new && !ast.IsEmpty() )
    TIMETRACE::NewTimeTraceInit();
#endif // VICE
  theCompiler().SetEmptyCompEnv();

  theSystem().Init_Sigma(ast_is_new);

  theScheduler().InitScheduler(ast_is_new, ast.IsEmpty());
#endif //VDMPP

  theState().Init_Sigma (ast_is_new);

  theState().TranslateAST (ast, ast_is_new);

  theState().InitializeGSGV (ast_is_new);

#ifdef VDMSL
  theState().PushModuleIfAny();
#endif //VDMSL
#ifdef VDMPP
  theSystem().RemoveAllGuards();
  theState().GC(true, !UsesThreads());
#endif // VDMPP
}

// envl_init
// ==> seq of SEM`ENV
TYPE_SEM_ENVL StackEval::envl_init()
{
  TYPE_SEM_ENVL s;
  s.ImpAppend(SEQ<TYPE_SEM_BlkEnv>());
  return s;
}

// ResetEnvL
// ==> ()
void StackEval::ResetEnvL()
{
  this->cs_env_l_p->Clear();
  this->cs_env_l_p->ImpAppend(SEQ<TYPE_SEM_BlkEnv>());
}

// ResetTypeInst
//  ==> ()
void StackEval::ResetTypeInst()
{
  this->cs_typeinst_p->Clear();
  this->cs_typeinst_p->ImpAppend(Map());
}

// typeinst_init
// ==> seq of map AS`TypeVar to AS`Type
Sequence StackEval::typeinst_init()
{
  Sequence s;
  s.ImpAppend(Map());
  return s;
}

bool StackEval::IsInitialized()
{
#ifdef VDMSL
  return (!this->cs_cur_mod_p->IsEmpty());
#endif // VDMSL
#ifdef VDMPP
  return (!theState().StillInit());
  // This means that the specification has been initialised. The Init flag
  // in the VDM++ dynsem is true if the specification has not been initialised.
#endif // VDMPP
}

void StackEval::ResetEnvlInError()
{
  if (this->cs_env_l_p->IsEmpty())
  {
    // The environment list must not be empty after init
    this->cs_env_l_p->ImpAppend(SEQ<TYPE_SEM_BlkEnv>());
  }
}

// }}}
// {{{ Eval Stack Operations

// The follwoing commands are only wrappers for the stack operations.
// The real operations is implemented as methods on the EvalStack object.

// Pop
// ==> EvalStackItem
TYPE_STKM_EvalStackItem StackEval::Pop(void)
{
  return this->cs_eval_stack_p->Pop();
}

// Pop
// n : nat
// ==> seq of EvalStackItem
SEQ<TYPE_STKM_EvalStackItem> StackEval::Pop(const Int & n)
{
  return this->cs_eval_stack_p->Pop(n.GetValue());
}

// Push
// e : EvalStackItem
// ==> ()
void StackEval::Push(const TYPE_STKM_EvalStackItem& e)
{
  this->cs_eval_stack_p->Push(e);
}

// Head
// ==> EvalStackItem
const TYPE_STKM_EvalStackItem & StackEval::Head() const
{
  return this->cs_eval_stack_p->GetNth(1);
}

// Note this functions is different from the specification, which would
//                        ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// return a list  with the first n'th elements.
// GetES
// n : nat
// ==> seq of EvalStackItem (spec)
// ==> EvalStackItem
const TYPE_STKM_EvalStackItem & StackEval::GetES(const Int & n) const
{
  return this->cs_eval_stack_p->GetNth(n.GetValue());
}

// LenES
// n : nat
// ==> bool
bool StackEval::LenES(const Int & n) const
{
  return (this->cs_eval_stack_p->Length() >= n.GetValue());
}

// RemES
// n : nat
// ==> ()
void StackEval::RemENth(const Int & n)
{
  this->cs_eval_stack_p->RemoveNth(n.GetValue());
}

// EvalStackLevel
// n : nat
// ==> bool
int StackEval::EvalStackLevel() const
{
  return (this->cs_eval_stack_p->Length());
}

// }}}
// {{{ Call Stack Operations

// PushCS
// item : Code = SEM`ExplFN | SEM`OP | DebugCmd
// nm : (AS`Name | seq of char)
// val_l : [seq of SEM`VAL]
// type : CSItemType
// ==> ()
void StackEval::PushCS(const TYPE_STKM_Code & item, const Generic & nm,
                       const Generic & val_l, CallStackItemType::CSItemType type)
{
// 20130530 -->
  if (Settings.CallLog())
  {
    wstring id;
    if (nm.IsSequence())
    {
      SEQ<Char> nm_q (nm);
      if (nm_q.StartsWith(SEQ<Char>(L"Check of Instance Invariant: ")))
      {
        wstring clnm (nm_q.SubSequence(30, nm_q.Length()).GetString()); 
        id = clnm + L"`inv_" + clnm;
      }
      else if (nm_q.StartsWith(SEQ<Char>(L"Running constructor for ")))
      {
        wstring clnm (nm_q.SubSequence(25, nm_q.Length()).GetString()); 
        id = clnm + L"`" + clnm;
      }
#ifdef VDMPP
      else if (nm_q.StartsWith(SEQ<Char>(L"Guard Evaluation")))
      {
        Map threadmap (theStackMachine().EvalThreads());
        const TYPE_SCHDTP_ThreadInfo & threadinfo (threadmap[theScheduler().CurThreadId()]);
        const TYPE_SCHDTP_ThreadStatus & status (threadinfo.GetRecord(pos_SCHDTP_ThreadInfo_status));
        id = nm_q.GetString() + L": " + theScheduler().CurThreadId().ascii();
        switch (status.GetTag()) {
          case TAG_TYPE_SCHDTP_Blocked: {
            wstring method = ASTAUX::ASName2String(status.GetRecord(pos_SCHDTP_Blocked_opnm));
            std::string::size_type n = method.find(L"`");
            while (n != std::string::npos) {
              method[n] = L'-';
              n = method.find(L"`");
            } 
            id += L" (" + method + L")";
            break;
          }
          default: {
            break;
          }
        }
      }
#endif // VDMPP
      else
        id = nm_q.GetString();
    }
    else
      id = ASTAUX::ASName2String(nm);
    // TODO : convert val_l
    TOOLS::WriteCallLog(id, Sequence(), this->cs_call_stack_p->Length());
  }
// <-- 20130530

  TYPE_STKM_CallStackItem csi;
  csi.Init(Int(type),                         // CSItemType
           item,                              // Code
           this->cs_shared_p->pc,             // ProgramCounter
           nm,                                // AS`Name | seq of char
           val_l,                             // [seq of SEM`VAL]
           Nil(),                             // [EvaluatorStatus]
           GetCurCid(),                       // CI`ContextId
           Int(this->cs_env_l_p->Length()),   // [nat]
           Int(this->cs_typeinst_p->Length()),// [nat]
           Int(this->cs_os_p->Length()),      // [nat]
#ifdef VDMSL
           Int(this->cs_cur_mod_p->Length()), // [nat]
#endif // VDMSL
#ifdef VDMPP
           Int(this->cs_obj_l_p->Length()),   // [nat]
#endif //VDMPP
           Bool(this->cs_shared_p->rterror),
           Nil());

  this->cs_call_stack_p->Push(csi);
  this->cs_shared_p->pc = 0;
  this->cs_shared_p->rterror = false;
  switch (item.GetTag()) {
    case TAG_TYPE_SEM_ExplFN: {
      PushContext(Int(AS_PURE));
      break;
    }
    case TAG_TYPE_SEM_ExplOP: {
      if (item.GetBoolValue(pos_SEM_ExplOP_oppure))
        PushContext(Int(PUREOP));
      else
        PushContext(Int(ALL));
      break;
    }
    default: {
      PushContext(Int(ALL));
      break;
    }
  }

  SetCurrentProgram(ExtractInstr(item));

#ifdef VDMPP
  SaveCurrntEvaluatorStateToCPUSigma();
  SaveCurrntProgramToCPUSigma();
#endif // VDMPP
}

// PopCS
// ==> ()
void StackEval::PopCS(void)
{
  TYPE_STKM_CallStackItem elm (this->cs_call_stack_p->Pop()); // Pop

  this->cs_shared_p->pc = elm.GetInt(pos_STKM_CallStackItem_pc);
  this->cs_shared_p->rterror = elm.GetBoolValue(pos_STKM_CallStackItem_rterror);

  if (elm.GetInt(pos_STKM_CallStackItem_type) == CallStackItemType::CS_DEBUGCMD)
  {
    EvaluatorStatusCt evalStateCt ((const EvaluatorStatusCt &)elm.GetField(pos_STKM_CallStackItem_evalstate));
    const EvaluatorStatus & evalState (evalStateCt.get_const_ref());

    this->cs_shared_p->curCid       = elm.GetInt(pos_STKM_CallStackItem_oldCid);

    this->cs_shared_p->env_l_bak    = evalState.env_l_bak;
    this->cs_shared_p->typeinst_bak = evalState.typeinst_bak;
    this->cs_shared_p->os_bak       = evalState.os_bak;
#ifdef VDMSL
    this->cs_shared_p->cur_mod_bak  = evalState.cur_mod_bak;
#endif //VDMSL
#ifdef VDMPP
    this->cs_shared_p->obj_l_bak    = evalState.obj_l_bak;
#endif //VDMPP
    this->cs_shared_p->context      = evalState.context;
  }
  else {
    PopContext();
  }

  TYPE_STKM_SubProgram program;
  if (!this->cs_call_stack_p->IsEmpty())
  {
    const TYPE_STKM_CallStackItem & csi (this->cs_call_stack_p->Head());
    program = ExtractInstr(csi.GetField(pos_STKM_CallStackItem_code));
  }
  SetCurrentProgram(program);

#ifdef VDMPP
  SaveCurrntEvaluatorStateToCPUSigma();
  SaveCurrntProgramToCPUSigma();
#endif // VDMPP

#ifdef VDMPP
  theState().GC(true, false);
#endif // VDMPP
}

// HeadCS
// ==> CallStackItem
TYPE_STKM_CallStackItem StackEval::HeadCS() const
{
  return this->cs_call_stack_p->Head();
}

// GetCS
// i : nat1
// ==> CallStackItem
TYPE_STKM_CallStackItem StackEval::GetCS(const Int & n) const
{
  return this->cs_call_stack_p->GetNth(n.GetValue());
}

// CallStackLevel
// ==> nat
int StackEval::CallStackLevel() const
{
  return this->cs_call_stack_p->Length();
}

// }}}
// {{{ PushDS, PopDS, UserPopDS

// PushDS
// evalst : STKM`EvaluatorStatus
// debugString : seq of char
// code : Code = SEM`ExplFN | SEM`OP | DebugCmd
// ==> ()
void StackEval::PushDS(const EvaluatorStatusCt & evalst, const SEQ<Char> & debugString,
                       const TYPE_STKM_Code & code)
{
// 20130530 -->
  if (Settings.CallLog())
  {
    TOOLS::WriteCallLog(debugString.GetString(), Sequence(), this->cs_call_stack_p->Length());
  }
// <-- 20130530

  const EvaluatorStatus & ostate (evalst.get_const_ref());

  TYPE_STKM_CallStackItem csi;
  csi.Init(Int(CallStackItemType::CS_DEBUGCMD), // CSItemType
           code,                                // Code
           ostate.pc,                           // ProgramCounter
           debugString,                         // AS`Name | seq of char
           Nil(),                               // [seq of SEM`VAL]
           evalst,                              // [EvaluatorStatus]
           GetCurCid(),                         // CI`ContextId
           Int(-1),                             // [nat]
           Int(-1),                             // [nat]
           Int(-1),                             // [nat]
           Int(-1),                             // [nat]
           Bool(ostate.rterror),
           Nil());

  // create new EvaluatorStatus
  EvaluatorStatusCt stateCt (evalst);
  EvaluatorStatus & state (stateCt.get_oneref_ref()); // must be oneref

  state.call_stack.Push(csi);

// 20100906 -->
  state.env_l.Clone();
  state.typeinst.Clone();
  state.os.Clone();
#ifdef VDMSL
  state.cur_mod.Clone();
#endif // VDMSL
#ifdef VDMPP
  state.obj_l.Clone();
#endif // VDMPP
// <-- 20100906

  state.env_l_bak.Clear();
  state.typeinst_bak.Clear();
  state.os_bak.Clear();
#ifdef VDMSL
  state.cur_mod_bak.Clear();
#endif // VDMSL
#ifdef VDMPP
  state.obj_l_bak.Clear();
#endif // VDMPP
  state.pc = 0;
  state.rterror = false;

  TYPE_STKM_SubProgram program (ExtractInstr(code));

  SetEvaluatorState(stateCt);
  SetProgram(program);

#ifdef VICE
  theScheduler().SaveCurThreadState();
#endif // VICE
}

// get_objref_from_fnop
// fnop : SEM`ExplOP | SEM`ExplFN
// ==> [SEM`OBJ_Ref]
Generic StackEval::get_objref_from_fnop(const TYPE_SEM_VAL & fnop)
{
  Generic objref = Nil();
  switch(fnop.GetTag()) {
    case TAG_TYPE_SEM_ExplOP: {
      objref = fnop.GetField(pos_SEM_ExplOP_objref);
      break;
    }
    case TAG_TYPE_SEM_ExplFN: {
      objref = fnop.GetField(pos_SEM_ExplFN_objref);
      break;
    }
    default: {
      RTERR::Error(L"StackEval::get_objref_from_fnop", RTERR_INTERNAL_ERROR, Nil(), Nil(), Sequence());
    }
  }

  if (!objref.IsNil() && !objref.Is(TAG_TYPE_SEM_OBJ_uRef))
    RTERR::Error(L"StackEval::get_objref_from_fnop", RTERR_INTERNAL_ERROR, Nil(), Nil(), Sequence());
  return objref;
}

// UserPopDS
// ==> ()
void StackEval::UserPopDS()
{
  EvaluatorStatusCt stateCt (GetEvaluatorState());
  const EvaluatorStatus & state (stateCt.get_const_ref());

  // First search for a debug element.
  size_t len_cs = state.call_stack.Length();

  bool exists = false;
  size_t index = 0;
  for (size_t j = 1; (j <= len_cs) && !exists; j++) {
    TYPE_STKM_CallStackItem elm (state.call_stack.GetNth(j));
    if (elm.get_type() == CallStackItemType::CS_DEBUGCMD) {
      exists = true;
      index = j;
    }
  }

  if (!exists) {
    vdm_iplog << L"Cannot pop more elements of the debug stack" << endl;
    return;
  }

  ResetInActivity();
  // Now run through the stack to do the update.

  for (size_t i = 1; i < index; i++)
  {
#ifdef VDMPP
    TYPE_STKM_CallStackItem elm (HeadCS());
    if (elm.get_type() == CallStackItemType::CS_FNOP)
    {
      // Only operations have history counters.
      const Generic & code (elm.GetField(pos_STKM_CallStackItem_code)); 
      if (code.Is(TAG_TYPE_SEM_ExplOP))
      {
#ifdef VICE
        TYPE_SEM_ExplOP eop (code);
        theState().UpdateHistCount(elm.GetField(pos_STKM_CallStackItem_nmOrDesc),
                                   EvalState::historyKind_fin, // TYPE_INSTRTP_fin()
                                   get_objref_from_fnop(eop),
                                   eop.get_sync(),
                                   Sequence());
#else
        theState().UpdateHistCount(elm.GetField(pos_STKM_CallStackItem_nmOrDesc),
                                   EvalState::historyKind_fin, // TYPE_INSTRTP_fin()
                                   get_objref_from_fnop(code));
#endif // VICE
      }
    }
#endif // VDMPP
    PopCS();
  }

  TYPE_STKM_CallStackItem elm (HeadCS());
  EvaluatorStatusCt ev ((const EvaluatorStatusCt &)elm.GetField(pos_STKM_CallStackItem_evalstate));
  ev.get_shared_ref().curCid = elm.GetInt(pos_STKM_CallStackItem_oldCid);
  ev.get_shared_ref().pc = elm.GetInt(pos_STKM_CallStackItem_pc);
  PopCS();
  ReplaceEvaluatorStatus(ev);

#ifdef VDMPP
  theState().GC(true, !UsesThreads());
#endif // VDMPP
}

// }}}
// {{{ Context Stack Operations

// SetCid
// CI`ContextId
// ==> ()
void StackEval::SetCid(const TYPE_CI_ContextId & cid)
{
  this->cs_shared_p->curCid = cid;
}

// GetCurCid
//  ==> CI`ContextId
TYPE_CI_ContextId StackEval::GetCurCid() const
{
  return this->cs_shared_p->curCid;
}

// }}}
// {{{ Trap Stack Operations

// PushTS
// handid : nat
// ==> ()
void StackEval::PushTS(const Int & handid)
{
  Tuple gell (GetEnvLLengths());

  TYPE_STKM_Trap trap;
  trap.Init(handid,
            Int(this->cs_eval_stack_p->Length()),
            Int(this->cs_call_stack_p->Length()),
            Int(this->cs_typeinst_p->Length()),
            Int(this->cs_os_p->Length()),
#ifdef VDMSL
            Int(this->cs_cur_mod_p->Length()),
#endif // VDMSL
#ifdef VDMPP
            Int(this->cs_obj_l_p->Length()),
#endif //VDMPP
            TYPE_CI_ContextId(this->cs_shared_p->curCid),
            gell.GetInt(1),
            gell.GetInt(2));

  this->cs_shared_p->trap_stack.Push(trap);
}

// PopTS
// ==> ()
void StackEval::PopTS()
{
  this->cs_shared_p->trap_stack.Pop();
}

// EmptyTS
// ==> bool
bool StackEval::EmptyTS() const
{
  return this->cs_shared_p->trap_stack.IsEmpty();
}

// GotoTrapHandler
// item ; SEM`EXIT
// ==> ()
void StackEval::GotoTrapHandler(const TYPE_SEM_EXIT & item)
{
  if (EmptyTS())
  {
    if (IsRuntimeErrorException(item))
    {
      // throw C++ level exception
      SetRunTimeError();
      VDMErrorHandle(ERR_IP, 1);
    }
    else
    {
      Push(item);
      ExeRETURN();
    }
  }
  else
  {
    EvaluatorStatus & state (*this->cs_shared_p);

    const TYPE_STKM_Trap & trap (state.trap_stack.Head());
    const Int & handid (trap.GetInt(pos_STKM_Trap_handid));
    int les = trap.GetIntValue(pos_STKM_Trap_lenes);
    int lcs = trap.GetIntValue(pos_STKM_Trap_lencs);
    int lti = trap.GetIntValue(pos_STKM_Trap_lentis);
    int los = trap.GetIntValue(pos_STKM_Trap_lenos);
#ifdef VDMSL
    int lcurm = trap.GetIntValue(pos_STKM_Trap_lencurm);
#endif // VDMSL
#ifdef VDMPP
    int lobjl = trap.GetIntValue(pos_STKM_Trap_lenobjl);
#endif // VDMPP
    const TYPE_CI_ContextId & cid (trap.GetInt(pos_STKM_Trap_cid));
    const Int & lenvl (trap.GetInt(pos_STKM_Trap_lenenvl));
    const Int & ltopenvl (trap.GetInt(pos_STKM_Trap_lentopenvl));

    state.pc = FindTrapHandler(handid, lcs);

    // updatet the eval_stack
    size_t len_es = state.eval_stack.Length() - les;
    (void) state.eval_stack.Pop(len_es);
    state.eval_stack.Push(item);

    // Update the call_stack.
    // Note that the operation UpdateHistoryCounters is incorporated in the code below.
    size_t len_cs = state.call_stack.Length() - lcs;
    for (size_t i = 0; i < len_cs; i++) {
      TYPE_STKM_CallStackItem elm (state.call_stack.Pop());
      if (elm.get_type() == CallStackItemType::CS_FNOP) {
        // Only operations have history counters.
        const Generic & code (elm.GetField(pos_STKM_CallStackItem_code));
        if (code.Is(TAG_TYPE_SEM_ExplOP)) {
#ifdef VDMPP
          const Generic & nm (elm.GetField(pos_STKM_CallStackItem_nmOrDesc));
#ifdef VICE
          TYPE_SEM_ExplOP eop (code);
          theState().UpdateHistCount(nm,
                                     //TYPE_INSTRTP_fin(),
                                     EvalState::historyKind_fin,
                                     get_objref_from_fnop(eop),
                                     eop.get_sync(),
                                     Sequence());
#else
          theState().UpdateHistCount(nm,
                                     //TYPE_INSTRTP_fin(),
                                     EvalState::historyKind_fin,
                                     get_objref_from_fnop(code));
#endif // VICE
#endif // VDMPP
          state.context.ImpTl();
        }
        else if (code.Is(TAG_TYPE_SEM_ExplFN)) {
          state.context.ImpTl();
        }
      }
    }
    Sequence typeinst (state.typeinst);
    size_t len_typeinst = typeinst.Length();
    state.typeinst = typeinst.SubSequence(len_typeinst - lti + 1, len_typeinst);
    Sequence os (state.os);
    size_t len_os = os.Length();
    state.os = os.SubSequence(len_os - los + 1, len_os);
#ifdef VDMSL
    Sequence curm (state.cur_mod);
    size_t len_curm = curm.Length();
    state.cur_mod = SEQ<TYPE_AS_Name>(curm.SubSequence(len_curm - lcurm + 1, len_curm));
#endif // VDMSL
#ifdef VDMPP
    Sequence obj_l (state.obj_l);
    size_t len_obj_l = obj_l.Length();
    state.obj_l = SEQ<TYPE_GLOBAL_OBJscope>(obj_l.SubSequence(len_obj_l - lobjl + 1, len_obj_l));
#endif //VDMPP

    state.env_l_bak.Clear();
    state.typeinst_bak.Clear();
    state.os_bak.Clear();
#ifdef VDMSL
    state.cur_mod_bak.Clear();
#endif // VDMSL
#ifdef VDMPP
    state.obj_l_bak.Clear();
#endif //VDMPP

    // Update the context_stack
    state.curCid = cid;

    UpgradeENVL(lenvl, ltopenvl); // modify this->curr_state
  }
}

// FindTrapHandler
// hadid : nat
// lengthcallstack : nat
// ==> ProgramCounter
TYPE_STKM_ProgramCounter StackEval::FindTrapHandler(const Int & handid, int lengthcallstack)
{
  EvaluatorStatusCt stateCt (this->curr_state);
  const EvaluatorStatus & state (stateCt.get_const_ref());

  TYPE_STKM_SubProgram program (this->curr_program);

  if (state.call_stack.Length() != lengthcallstack)
  {
    size_t index = state.call_stack.Length() + 1 - lengthcallstack;
    TYPE_STKM_CallStackItem csi (state.call_stack.GetNth(index));
    program = ExtractInstr(csi.GetField(pos_STKM_CallStackItem_code));

    SetEvaluatorState(stateCt);
    SetProgram(program);
  }
  size_t len_program = program.Length();
  for (size_t i = 1; i <= len_program; i++)
  {
    const TYPE_INSTRTP_Instruction & instr (program[i]);
    if (instr.Is(TAG_TYPE_INSTRTP_HANDID) && (handid == instr.GetInt(pos_INSTRTP_HANDID_handid))) {
      return i;
    }
  }
  RTERR::Error(L"StackEval::FindTrapHandler", RTERR_INTERNAL_ERROR, Nil(), Nil(), Sequence());
  return 0; // dummy
}

// }}}
// {{{ Measure
void StackEval::MeasureCheck(const TYPE_SEM_VAL & curr_mv)
{
  EvaluatorStatus & state (*this->cs_shared_p);
  size_t len_cs = state.call_stack.Length();
  //TYPE_STKM_CallStackItem curcsi (theStackMachine().HeadCS());
  TYPE_STKM_CallStackItem curcsi (state.call_stack.Head());
  const TYPE_AS_Name & fnnm (curcsi.GetRecord(pos_STKM_CallStackItem_nmOrDesc));
  //size_t len_cs = theStackMachine().CallStackLevel();
  bool found = false;
  for (size_t idx = 2; (idx <= len_cs) && !found; idx++) {
    //TYPE_STKM_CallStackItem csi (GetCS(Int(idx)));
    const TYPE_STKM_CallStackItem & csi (state.call_stack[idx]);
    if (csi.GetIntValue(pos_STKM_CallStackItem_type) == CallStackItemType::CS_FNOP) {
      if (fnnm == csi.GetField(pos_STKM_CallStackItem_nmOrDesc)) {
        const Generic & prev_mv (csi.GetField(pos_STKM_CallStackItem_measu));
        switch (curr_mv.GetTag()) {
          case TAG_TYPE_SEM_NUM: {
            if (prev_mv.Is(TAG_TYPE_SEM_NUM)) {
              const Real & prev_v (Record(prev_mv).GetReal(pos_SEM_NUM_v));
              const Real & curr_v (curr_mv.GetReal(pos_SEM_NUM_v));
              if (prev_v.LessEqual(curr_v)) {
                vdm_iplog << L"Measure value of " << ASTAUX::ASName2String(fnnm) << L": current("
                          << curr_v << L") previous(" << prev_v << L")"<< endl;
                  RTERR::Error(L"MeasureCheck",RTERR_MEASURE_ERROR, Nil(), Nil(), Sequence());
              }
            }
            break;
          }
          case TAG_TYPE_SEM_TUPLE: {
            if (prev_mv.Is(TAG_TYPE_SEM_TUPLE)) {
              SEQ<TYPE_SEM_VAL> prev_sv_l (Record(prev_mv).GetSequence(pos_SEM_TUPLE_v));
              SEQ<TYPE_SEM_VAL> curr_sv_l (curr_mv.GetSequence(pos_SEM_TUPLE_v));
              bool check_ok = (prev_sv_l.Length() == curr_sv_l.Length());
              size_t len_prev_sv_l = prev_sv_l.Length();
              for (size_t i = 1; (i <= len_prev_sv_l) && check_ok; i++) {
                const TYPE_SEM_VAL & prev_sv (prev_sv_l[i]);
                const TYPE_SEM_VAL & curr_sv (curr_sv_l[i]);
                if (prev_sv.Is(TAG_TYPE_SEM_NUM) && curr_sv.Is(TAG_TYPE_SEM_NUM)) {
                  const Real & prev_v (prev_sv.GetReal(pos_SEM_NUM_v));
                  const Real & curr_v (curr_sv.GetReal(pos_SEM_NUM_v));
                  if (prev_v.GreaterThan(curr_v)) {
                    break;
                  }
                  else {
                    check_ok = (prev_v == curr_v);
                  }
                }
                else {
                  RTERR::Error(L"MeasureCheck",RTERR_NAT_OR_TUPLE_OF_NAT_EXPECTED, Nil(), Nil(), Sequence());
                }
              }
              if (!check_ok) {
                vdm_iplog << L"Measure value of " << ASTAUX::ASName2String(fnnm) << L": current("
                          << EvalState::Convert2M4(curr_mv, NULL)
                          << L") previous(" << EvalState::Convert2M4(prev_mv, NULL) << L")"<< endl;
                RTERR::Error(L"MeasureCheck",RTERR_MEASURE_ERROR, Nil(), Nil(), Sequence());
              }
            }
            break;
          }
          default: {
            // num or num tuple expectrd
            RTERR::Error(L"MeasureCheck",RTERR_NUM_EXPECTED, Nil(), Nil(), Sequence());
            break;
          }
        }
        found = true;
        curcsi.SetField(pos_STKM_CallStackItem_measu, curr_mv);
        state.call_stack.ImpModify(1, curcsi);
      }
    }
  }
}
// }}}
// {{{ Various functions to modify the back trace stack

const TYPE_STKM_CallStack& StackEval::GetBackTrace()
{
  return *this->cs_call_stack_p;
}

// }}}
// {{{ Various Operations to Modify the State

// PushCF
//  e : STKM`CFStackItem
void StackEval::PushCF(const TYPE_STKM_CFStackItem & e)
{
  this->cs_shared_p->cf.Push(e);
}

// PopCF
void StackEval::PopCF()
{
  this->cs_shared_p->cf.Pop();
}

// GetCF
// ==> STKM`CFStackItem
const TYPE_STKM_CFStack & StackEval::GetCF()
{
  return this->cs_shared_p->cf;
}

// PrintCf
// ==> seq of (seq of char * seq of char * nat * nat)
Sequence StackEval::PrintCf()
{
  TYPE_STKM_CFStack cf (GetCF());
  Sequence res;
  size_t len_cf = cf.Length();
  for (size_t i = 1; i <= len_cf; i++)
  {
    Tuple elm (cf.GetNth(i));

    Sequence name (elm.GetSequence(1));

    TYPE_CI_ContextId cid (elm.GetInt(2));
    Tuple flc (GetCI().GetFileLineColPos(cid));
    if (!flc.GetField(1).IsNil()) {
      res.ImpAppend(mk_(name, flc.GetField(1), flc.GetField(2), flc.GetField(3)));     
    }
    else {
      res.ImpAppend(mk_(name, Sequence(), Int(-1), Int(-1)));     
    }
  }
  return res;
}

// IncrPC
// n : int
// ==> ()
void StackEval::IncrPC(const Int & n)
{
  this->cs_shared_p->pc += n.GetValue();
}

// SetProgramAtEnd
// ==> ()
void StackEval::TerminateProgram()
{
  this->cs_shared_p->pc = this->curr_program_length;
  if( !this->cs_call_stack_p->IsEmpty() )
    this->cs_call_stack_p->Pop();
}

// SetStep
//  ==> ()
void StackEval::SetStep()
{
  SetDebugFlag(TYPE_STKM_Step().Init(CallStackLevel()));
}

// SetStepIn
//  ==> ()
void StackEval::SetStepIn()
{
  SetDebugFlag(TYPE_STKM_StepIn());
}

// SetSingleStep
// ==> ()
void StackEval::SetSingleStep()
{
  SetDebugFlag(TYPE_STKM_SingleStep().Init(CallStackLevel()));
}

// SetFinish
// ==> ()
void StackEval::SetFinish()
{
  SetDebugFlag(TYPE_STKM_Finish().Init(CallStackLevel()));
}

// SetContinue
// ==> ()
void StackEval::SetContinue()
{
  SetDebugFlag(TYPE_STKM_Continue());
}

// GetDebugFlag
// ==> DebugFlag
TYPE_STKM_DebugFlag StackEval::GetDebugFlag() const
{
  return this->cs_shared_p->debug_flag;
}

// SetDebugFlag
// new_df : DebugFlag
// ==> ()
void StackEval::SetDebugFlag(const TYPE_STKM_DebugFlag & new_df)
{
  this->cs_shared_p->debug_flag = new_df;

#ifdef VDMPP
  SaveCurrntEvaluatorStateToCPUSigma();
#endif // VDMPP
}

// SetBREAK
// ==> ()
void StackEval::SetBREAK()
{
  this->BREAK = true;
}

// ResetBREAK
// ==> ()
void StackEval::ResetBREAK()
{
  this->BREAK = false;
}

// GetBREAK
// ==> bool
bool StackEval::GetBREAK()
{
  return this->BREAK;
}

// SetUserBREAK
// ==> ()
void StackEval::SetUserBREAK()
{
  MUTEX_LOCK( mutex_break );
  this->UserBREAK = true;
  MUTEX_UNLOCK( mutex_break );
}

// ResetUserBREAK
// ==> ()
void StackEval::ResetUserBREAK()
{
  MUTEX_LOCK( mutex_break );
  this->UserBREAK = false;
  MUTEX_UNLOCK( mutex_break );
}

// GetUserBREAK
// ==> bool
bool StackEval::GetUserBREAK()
{
  MUTEX_LOCK( mutex_break );
  bool ret = this->UserBREAK;
  MUTEX_UNLOCK( mutex_break );
  return ret;
}

// IsProgramAtEnd
// ==> bool
bool StackEval::IsProgramAtEnd() const
{
  return this->cs_shared_p->pc >= this->curr_program_length;
}

// }}}
// {{{ The Init Mapping Stack

#ifdef VDMPP
// SetGuard
// fullopnm : [AS`Name]
// obj : [SEM`OBJ_Ref]
// ==> ()
void StackEval::SetGuard(const Generic & fullopnm, const Generic & obj)
{
  this->cs_shared_p->guard = TYPE_STKM_Guard().Init(fullopnm, obj);

  theSystem().SetGuard(fullopnm, obj);
}
#endif //VDMPP

// }}}

void StackEval::ClearProfile()
{
#ifdef PROFILING
  for (int i = 0; i < NUMINSTR; i++)
  {
    instrcnt[i] = 0ULL;
    instrclk[i] = 0ULL;
  }
  time(&sttime);
#endif // PROFILING
}
void StackEval::PrintProfile()
{
#ifdef PROFILING
  int tbl[NUMINSTR];
  int count = 0;
  for (int i = 0; i < NUMINSTR; i++)
  {
    if (instrcnt[i] == 0ULL) continue;
    tbl[count] = i;
    count++;
  }

  if (count == 0) return;

  int sortedidx[NUMINSTR];
  for (int j = 0; j < count; j++)
  {
    int ii = j;
    unsigned long uclk = instrclk[tbl[ii]];
    for (int k = j + 1; k < count; k++)
    {
      if (uclk < instrclk[tbl[k]])
      {
        ii = k;
        uclk = instrclk[tbl[k]];
      }
    }
    sortedidx[j] = tbl[ii];
    tbl[ii] = tbl[j];
  }

  char buf[100];
  wstring tmptag;
  unsigned long long totalcnt = 0ULL;
  unsigned long long totalclk = 0ULL;
  for (int ii = 0; ii < count; ii++)
  {
    int index = sortedidx[ii];
    VDMGetDefaultRecInfoMap().GetSymTag(index + TAG_INSTRTP, tmptag);
#ifdef __APPLE_CC__
    sprintf(buf, "%-30s %12llu usec %12llu instrs %8llu ns/instr",
#else
    sprintf(buf, "%-30s %12lu usec %12lu instrs %8llu ns/instr",
#endif //__APPLE_CC__
            TBWSTR::wstring2string(tmptag).c_str(),
            instrclk[index],
            instrcnt[index],
            (instrclk[index] * 1000ULL)/instrcnt[index]); 
    vdm_log << TBWSTR::string2wstring(string(buf)) << endl;
  
    totalcnt += instrcnt[index]; 
    totalclk += instrclk[index]; 
  }
  vdm_log << L"--------------------------------------------------------------------------------------" << endl;
  sprintf(buf, "%-30s %12llu usec %12llu instrs %8llu ns/instr",
          "Total",
          totalclk, totalcnt, (totalclk * 1000ULL)/totalcnt); 
  vdm_log << TBWSTR::string2wstring(string(buf)) << endl;

  time_t endtime;
  time(&endtime);
  vdm_log << L"Erapse time: " << (endtime - sttime) << L" seconds." << endl;
#endif // PROFILING
}

// {{{ The Main Loop

// EvalMainLoop
// ==> STKM`EvaluationState * [SEM`VAL]
Tuple StackEval::EvalMainLoop()
{
  ResetBREAK();

#ifdef VDMSL
  if (this->cs_shared_p->rterror)
    RTERR::Error(L"EvalMainLoop", RTERR_CANNOT_PROCEED_AFTER_RUNTIME_ERROR, Nil(), Nil(), Sequence());

  // curr_state and curr_program may be changed in the loop
  while((this->cs_shared_p->pc < this->curr_program_length) && !GetBREAK())
  {
    // get the reference of current EvaluatorStatus
    EvaluatorStatus & state (*this->cs_shared_p); 

    // increment program counter
    state.pc += 1;

    // get instruction
    TYPE_INSTRTP_Instruction instr (this->curr_program.Index(state.pc));

    // execute instruction
    EvalInstr(instr);
  }
  TYPE_STKM_EvaluationState eval_state (MainLoopState());
  return (eval_state.Is(TAG_TYPE_STKM_Success) ? mk_(eval_state, Pop()) : mk_(eval_state, Nil()));
#endif // VDMSL

#ifdef VDMPP
  LoadCurrntEvaluatorStateFromCPUSigma();
  LoadCurrntProgramFromCPUSigma();

//wcout << L"EvalMainLoop: " << theSystem().GetCurCPU() << endl;
  // Consistency check, should never happen.
  if (!this->cs_shared_p->guard.IsNil())
    RTERR::Error(L"EvalMainLoop", RTERR_INTERNAL_ERROR, Nil(), Nil(), Sequence());

  if (this->cs_shared_p->rterror)
    RTERR::Error(L"EvalMainLoop", RTERR_CANNOT_PROCEED_AFTER_RUNTIME_ERROR, Nil(), Nil(), Sequence());

  if (UsesThreads())
  {
    // curr_state and curr_program may be changed in the loop
    while((this->cs_shared_p->pc < this->curr_program_length)
          && !GetBREAK()
          && this->cs_shared_p->guard.IsNil()
          && !theScheduler().Deschedule(*this->cs_shared_p)
#ifdef VICE
          && !theSystem().CPUSwapNeeded()
          && !theScheduler().IsSyncOpThread()
#endif // VICE
          )
    {
      // get the reference of current EvaluatorStatus
      EvaluatorStatus & state (*this->cs_shared_p); 

      // increment program counter
      state.pc += 1;

      // increment number of executed instructions
      theScheduler().IncInstrnum(state);

      // get instruction
      TYPE_INSTRTP_Instruction instr (this->curr_program.Index(state.pc));

      // execute instruction
      EvalInstr(instr);
    }
  }
  else
  {
    // curr_state and curr_program may be changed in the loop
    while((this->cs_shared_p->pc < this->curr_program_length) && !GetBREAK())
    {
      // get the reference of current EvaluatorStatus
      EvaluatorStatus & state (*this->cs_shared_p); 

      // increment program counter
      state.pc += 1;

      // get instruction
      TYPE_INSTRTP_Instruction instr (this->curr_program.Index(state.pc));

      // execute instruction
      EvalInstr(instr);
    }
  }

  // save current EvaluatorStatus to CPUSigma
  SaveCurrntEvaluatorStateToCPUSigma();
  SaveCurrntProgramToCPUSigma();

  TYPE_STKM_EvaluationState eval_state (MainLoopState());
#ifdef VICE
  if (eval_state.Is(TAG_TYPE_STKM_Success) && (EvalStackLevel() > 0))
    return mk_(eval_state, Pop());
  else
    return mk_(eval_state, Nil());
#else
  return (eval_state.Is(TAG_TYPE_STKM_Success) ? mk_(eval_state, Pop()) : mk_(eval_state, Nil()));
#endif // VICE
#endif // VDMPP
}

// EvalUninterruptedLoop
// ==> STKM`EvaluationState * SEM`VAL
Tuple StackEval::EvalUninterruptedLoop()
{
#ifdef VDMPP
  LoadCurrntEvaluatorStateFromCPUSigma();
  LoadCurrntProgramFromCPUSigma();
#endif // VDMPP

  if (this->cs_shared_p->rterror)
    RTERR::Error(L"EvalUninterruptedLoop", RTERR_CANNOT_PROCEED_AFTER_RUNTIME_ERROR, Nil(), Nil(), Sequence());

  // curr_state and curr_program may be changed in the loop
  while (this->cs_shared_p->pc < this->curr_program_length)
  {
    // get the reference of current EvaluatorStatus
    EvaluatorStatus & state (*this->cs_shared_p); 

    // increment program counter
    state.pc += 1;

    // get instruction 
    TYPE_INSTRTP_Instruction instr (this->curr_program.Index(state.pc));

    // execute instruction
    EvalInstr(instr);
  }

#ifdef VDMPP
  theSystem().ResetGuard();
#endif //VDMPP

  return mk_(TYPE_STKM_Success(), Pop());
}

#ifdef VDMPP
// ResetGuard
void StackEval::ResetGuard()
{
  this->cs_shared_p->guard = Nil();

  SaveCurrntEvaluatorStateToCPUSigma();
}

// EvalRunGuard (part of SCHD`RunGuard)
// instr : STKM`SubProgram
// objref : [SEM`OBJ_Ref]
// ==> STKM`EvaluationState * [SEM`VAL]
Tuple StackEval::EvalRunGuard(const TYPE_STKM_SubProgram & instr, const Generic & objref)
{
//  SaveCurrntEvaluatorStateToCPUSigma();

  if (!objref.IsNil()) {
    PushCurObj(objref, Nil(), Record(objref).GetRecord(pos_SEM_OBJ_uRef_tp));
  }
  // DebugCmd in spec is handled by PushCS.
  PushCS(TYPE_STKM_DebugCmd().Init(instr), SEQ<Char>(L"Guard Evaluation"), Nil(), CallStackItemType::CS_INTERNAL);

  DeActivateAllBreakpoints();
  TYPE_STKM_DebugFlag dflag (GetDebugFlag());
  SetContinue();

  //theScheduler().SetCheckingGuard(true);
  Tuple res (EvalMainLoop());
  //theScheduler().SetCheckingGuard(false);

  SetDebugFlag(dflag);
  ActivateAllBreakpoints();

  PopCS();

  if (!objref.IsNil()) {
    PopCurObj();
  }
  return res;
}

// EvalFieldOBJRefSelect
// objref : SEM`OBJ_Ref
// field : AS`Name
// ==> SEM`VAL
TYPE_SEM_VAL StackEval::EvalFieldOBJRefSelect(const TYPE_SEM_OBJ_uRef & objref, const TYPE_AS_Name & field) /*const*/
{
  PushEmptyEnv();
  PushCurObj(objref, objref.GetRecord(pos_SEM_OBJ_uRef_tp), GetCurCl());

  TYPE_SEM_ValTp valTp (theState().LookUp(field));
  TYPE_SEM_VAL res_v (valTp.GetRecord(pos_SEM_ValTp_val));

  if (!IsRuntimeErrorException(res_v)) {
    PopCurObj();
    PopEnvL();
  }
  return res_v;
}
#endif //VDMPP

// MainLoopState
// ==> EvaluationState
TYPE_STKM_EvaluationState StackEval::MainLoopState()
{
#ifdef VDMPP
  LoadCurrntEvaluatorStateFromCPUSigma();
  LoadCurrntProgramFromCPUSigma();

  TYPE_SCHDTP_ThreadId curthread (theSystem().GetCurThread());

  // DANGER ! We cannot interrupt the evaluation of a Guard here.
//  if(!state.guard.IsNil())
//    return state.guard;
  if(!this->cs_shared_p->guard.IsNil())
    return this->cs_shared_p->guard;
#endif // VDMPP

  if(GetBREAK())
    return TYPE_STKM_Breakpoint();

//  if(state.pc >= program.Length())
  if(this->cs_shared_p->pc >= this->curr_program_length)
  {
//    if ( !program.IsEmpty() && !RunTimeError() )
    if ( !this->curr_program.IsEmpty() && !RunTimeError() ) {
      this->lastres = Generic(Head());
    }
    return TYPE_STKM_Success();
  }
#ifdef VDMPP
#ifdef VICE
  if(theSystem().CPUSwapNeeded(theSystem().GetCurCPU()) || 
     theScheduler().GetThreadStatus(theScheduler().CurThreadId()).Is(TAG_TYPE_SCHDTP_SyncOp)) {
    if( theSystem().MultCPUs() ) {
      return TYPE_STKM_EndOfCPUSlice();
    }
    else {
      return TYPE_STKM_EndOfSlice();
    }
  }
#endif // VICE
//  if(theScheduler().EndOfSliceReached(state))
  if(theScheduler().EndOfSliceReached(*this->cs_shared_p)) {
    return TYPE_STKM_EndOfSlice();
  }
#endif //VDMPP
#ifdef VICE
  if(theScheduler().ASyncOpThread(theScheduler().CurThreadId())) {
    return TYPE_STKM_Success();
  }
#endif // VICE
  // Default
  return TYPE_STKM_Interrupt();
}

// EvalInstr
// i : INSTRTP`Instruction
void StackEval::EvalInstr(const TYPE_INSTRTP_Instruction & i)
{
#ifdef VICE
//wcout << theSystem().GetCurCPU() << L" " << theSystem().GetTime() << endl;
#endif // VICE
//vdm_iplog << INT2Q::h2gAS(i) << endl << flush;
#ifdef PROFILING
  uint64_t pretm = TBUTILS::GetCurTimeMil();
  int index = i.GetTag() - TAG_INSTRTP;
#endif // PROFILING

  switch (i.GetTag()) {

    case TAG_TYPE_INSTRTP_CONTEXT: {
      if (!Settings.Context())
        ExeCONTEXT(i.GetInt(pos_INSTRTP_CONTEXT_cid), i.GetBool(pos_INSTRTP_CONTEXT_isStmt));
      break;
    }

    case TAG_TYPE_INSTRTP_PUSH: {
      this->cs_eval_stack_p->Push(i.GetField(pos_INSTRTP_PUSH_val));
      break;
    }

    case TAG_TYPE_INSTRTP_LOOKUP: {
      ExeLOOKUP(i.GetRecord(pos_INSTRTP_LOOKUP_id));
      break;
    }

    case TAG_TYPE_INSTRTP_POP: {
      int n = i.GetIntValue(pos_INSTRTP_POP_n);
      this->cs_eval_stack_p->Pop(n);
      break;
    }

#ifdef VDMPP
    case TAG_TYPE_INSTRTP_CALLGUARD: {
      ExeCALLGUARD(i.GetBool(pos_INSTRTP_CALLGUARD_hasobj), i.GetRecord(pos_INSTRTP_CALLGUARD_oprt));
      break;
    }

    case TAG_TYPE_INSTRTP_PPCALL: {
      ExePPCALL();
      break;
    }
#endif //VDMPP

    case TAG_TYPE_INSTRTP_COPYVAL: {
      ExeCOPYVAL();
      break;
    }

    case TAG_TYPE_INSTRTP_UNOP: {
      EvalUNOP(i.GetInt(pos_INSTRTP_UNOP_op));
      break;
    }

    case TAG_TYPE_INSTRTP_BINOP: {
      EvalBINOP(i.GetInt(pos_INSTRTP_BINOP_op));
      break;
    }

    case TAG_TYPE_INSTRTP_PRE: {
      ExePRE();
      break;
    }

    case TAG_TYPE_INSTRTP_POST: {
      ExePOST();
      break;
    }

    case TAG_TYPE_INSTRTP_ASSERT: {
      ExeASSERT();
      break;
    }

    case TAG_TYPE_INSTRTP_CASES: {
      ExeCASES();
      break;
    }

    case TAG_TYPE_INSTRTP_MEASURE: {
      ExeMEASURE();
      break;
    }

    case TAG_TYPE_INSTRTP_MEASURETPINST: {
      ExeMEASURETPINST(i.GetSequence(pos_INSTRTP_MEASURETPINST_tpparms));
      break;
    }

    case TAG_TYPE_INSTRTP_MEASURECHECK: {
      ExeMEASURECHECK();
      break;
    }

    case TAG_TYPE_INSTRTP_DTCMEASURE: {
      ExeDTCMEASURE();
      break;
    }

    case TAG_TYPE_INSTRTP_PUSHOS: {
      break;
    }

    case TAG_TYPE_INSTRTP_POPOS: {
      break;
    }

    case TAG_TYPE_INSTRTP_POSTENV: {
      ExePOSTENV(i.GetSequence(pos_INSTRTP_POSTENV_resnmtps), i.GetInt(pos_INSTRTP_POSTENV_ci));
      break;
    }

    case TAG_TYPE_INSTRTP_DTC: {
      ExeDTC(i.GetRecord(pos_INSTRTP_DTC_tp));
      break;
    }

    case TAG_TYPE_INSTRTP_DTCSET: {
      ExeDTCSET();
      break;
    }

    case TAG_TYPE_INSTRTP_SIZE: {
      ExeSIZE(i.GetInt(pos_INSTRTP_SIZE_n));
      break;
    }

    case TAG_TYPE_INSTRTP_APPENDSEQ: {
      ExeAPPENDSEQ();
      break;
    }

    case TAG_TYPE_INSTRTP_APPENDMAP: {
      ExeAPPENDMAP();
      break;
    }

    case TAG_TYPE_INSTRTP_ADDSET: {
      ExeADDSET();
      break;
    }

    case TAG_TYPE_INSTRTP_SETRNG: {
      ExeSETRNG();
      break;
    }

    case TAG_TYPE_INSTRTP_SUBSEQ: {
      ExeSUBSEQ();
      break;
    }

    case TAG_TYPE_INSTRTP_SELSEQELEM: {
      ExeSELSEQELEM();
      break;
    }

    case TAG_TYPE_INSTRTP_TUPCONS: {
      ExeTUPCONS(i.GetInt(pos_INSTRTP_TUPCONS_length));
      break;
    }

    case TAG_TYPE_INSTRTP_SETCONS: {
      ExeSETCONS(i.GetInt(pos_INSTRTP_SETCONS_length));
      break;
    }

    case TAG_TYPE_INSTRTP_SEQCONS: {
      ExeSEQCONS(i.GetInt(pos_INSTRTP_SEQCONS_length));
      break;
    }

    case TAG_TYPE_INSTRTP_MAPCONS: {
      ExeMAPCONS(i.GetInt(pos_INSTRTP_MAPCONS_length));
      break;
    }

    case TAG_TYPE_INSTRTP_APPLY: {
      ExeAPPLY();
      break;
    }

    case TAG_TYPE_INSTRTP_DLCALL: {
      ExeDLCALL(i.GetRecord(pos_INSTRTP_DLCALL_cls), i.GetRecord(pos_INSTRTP_DLCALL_op));
      break;
    }

    case TAG_TYPE_INSTRTP_RETURN: {
      ExeRETURN();
      break;
    }

    case TAG_TYPE_INSTRTP_ISCHECK: {
      ExeISCHECK(i.GetRecord(pos_INSTRTP_ISCHECK_type));
      break;
    }

    case TAG_TYPE_INSTRTP_ISEMPTYSET: {
      ExeISEMPTYSET(i.GetInt(pos_INSTRTP_ISEMPTYSET_n));
      break;
    }

    case TAG_TYPE_INSTRTP_ISEMPTYSEQ: {
      ExeISEMPTYSEQ(i.GetInt(pos_INSTRTP_ISEMPTYSEQ_n));
      break;
    }

    case TAG_TYPE_INSTRTP_ISNOTEMPTYSET: {
      ExeISNOTEMPTYSET(i.GetInt(pos_INSTRTP_ISNOTEMPTYSET_n));
      break;
    }

    case TAG_TYPE_INSTRTP_ISNOTEMPTYSEQ: {
      ExeISNOTEMPTYSEQ(i.GetInt(pos_INSTRTP_ISNOTEMPTYSEQ_n));
      break;
    }

    case TAG_TYPE_INSTRTP_PUSHLIST: {
      ExePUSHLIST(i.GetInt(pos_INSTRTP_PUSHLIST_length));
      break;
    }

#ifdef VDMPP
    case TAG_TYPE_INSTRTP_LOOKUPSTATIC: {
      ExeLOOKUPSTATIC(i.GetRecord(pos_INSTRTP_LOOKUPSTATIC_id));
      break;
    }

    case TAG_TYPE_INSTRTP_LOOKUPOP: {
      ExeLOOKUPOP(i.GetRecord(pos_INSTRTP_LOOKUPOP_id));
      break;
    }
#endif //VDMPP

    case TAG_TYPE_INSTRTP_CBR: {
      ExeCBR(i.GetInt(pos_INSTRTP_CBR_length));
      break;
    }

    case TAG_TYPE_INSTRTP_CNBR: {
      ExeCNBR(i.GetInt(pos_INSTRTP_CNBR_length));
      break;
    }

    case TAG_TYPE_INSTRTP_BR: {
      ExeBR(i.GetInt(pos_INSTRTP_BR_length));
      break;
    }

    case TAG_TYPE_INSTRTP_POPBLKENV: {
      ExePOPBLKENV();
      break;
    }

    case TAG_TYPE_INSTRTP_EOCL: {
      ExeEOCL();
      break;
    }

    case TAG_TYPE_INSTRTP_MULTBINDL: {
      ExeMULTBINDL(i.GetInt(pos_INSTRTP_MULTBINDL_length), i.GetInt(pos_INSTRTP_MULTBINDL_part));
      break;
    }

    case TAG_TYPE_INSTRTP_SELBLKENV: {
      ExeSELBLKENV(i.GetInt(pos_INSTRTP_SELBLKENV_n));
      break;
    }

    case TAG_TYPE_INSTRTP_APPENDBLKENV: {
      ExeAPPENDBLKENV(i.GetRecord(pos_INSTRTP_APPENDBLKENV_id), i.GetField(pos_INSTRTP_APPENDBLKENV_tp));
      break;
    }

    case TAG_TYPE_INSTRTP_REMSTACKELEM: {
      ExeREMSTACKELEM(i.GetInt(pos_INSTRTP_REMSTACKELEM_n));
      break;
    }

    case TAG_TYPE_INSTRTP_SWAP: {
      ExeSWAP();
      break;
    }

    case TAG_TYPE_INSTRTP_EMPTYBLKENV: {
      ExeEmptyBlkEnv(i.GetField(pos_INSTRTP_EMPTYBLKENV_permis));
      break;
    }

    case TAG_TYPE_INSTRTP_MATCHANDBIND: {
      ExeMatchAndBind();
      break;
    }

    case TAG_TYPE_INSTRTP_TRYANYMATCH: {
      ExeTRYANYMATCH();
      break;
    }

    case TAG_TYPE_INSTRTP_CLOSENV: {
      ExeCLOSENV(i.GetRecord(pos_INSTRTP_CLOSENV_blkenv), i.GetMap(pos_INSTRTP_CLOSENV_body_um));
      break;
    }

    case TAG_TYPE_INSTRTP_ADDTOBLKENV: {
      ExeADDTOBLKENV();
      break;
    }

    case TAG_TYPE_INSTRTP_UPDATECLOSENV: {
      ExeUPDATECLOSENV(i.GetRecord(pos_INSTRTP_UPDATECLOSENV_expr));
      break;
    }

    case TAG_TYPE_INSTRTP_ERRINST: {
      ExeERRINST(i.GetInt(pos_INSTRTP_ERRINST_ident));
      break;
    }

    case TAG_TYPE_INSTRTP_MATCHVAL: {
      ExeMATCHVAL();
      break;
    }

    case TAG_TYPE_INSTRTP_SETENUM: {
      ExeSETENUM(i.GetInt(pos_INSTRTP_SETENUM_length));
      break;
    }

    case TAG_TYPE_INSTRTP_SEQENUM: {
      ExeSEQENUM(i.GetInt(pos_INSTRTP_SEQENUM_length));
      break;
    }

    case TAG_TYPE_INSTRTP_MAPLET: {
      ExeMAPLET();
      break;
    }

    case TAG_TYPE_INSTRTP_MAPENUM: {
      ExeMAPENUM(i.GetInt(pos_INSTRTP_MAPENUM_length));
      break;
    }

    case TAG_TYPE_INSTRTP_SETUNION: {
      ExeSetUnion();
      break;
    }

    case TAG_TYPE_INSTRTP_SEQCONC: {
      ExeSeqConc();
      break;
    }

    case TAG_TYPE_INSTRTP_MAPMERGE: {
      ExeMapMerge();
      break;
    }

    case TAG_TYPE_INSTRTP_RECPATCONS: {
      ExeRECPATCONS(i.GetRecord(pos_INSTRTP_RECPATCONS_tag), i.GetInt(pos_INSTRTP_RECPATCONS_length));
      break;
    }

    case TAG_TYPE_INSTRTP_TUPPATCONS: {
      ExeTUPPATCONS(i.GetInt(pos_INSTRTP_TUPPATCONS_length));
      break;
    }

#ifdef VDMPP
    case TAG_TYPE_INSTRTP_OBJECTPAT: {
      ExeOBJECTPAT(i.GetRecord(pos_INSTRTP_OBJECTPAT_cls),
                   i.GetInt(pos_INSTRTP_OBJECTPAT_length));
      break;
    }

    case TAG_TYPE_INSTRTP_FIELDPAT: {
      ExeFIELDPAT(i.GetRecord(pos_INSTRTP_FIELDPAT_nm));
      break;
    }
#endif // VDMPP

    case TAG_TYPE_INSTRTP_FREF: {
      ExeFREF(i.GetRecord(pos_INSTRTP_FREF_sel));
      break;
    }

    case TAG_TYPE_INSTRTP_MOSREF: {
      ExeMOSREF();
      break;
    }

    case TAG_TYPE_INSTRTP_ASSIGNSD: {
      ExeASSIGNSD();
      break;
    }

    case TAG_TYPE_INSTRTP_RECCONS: {
      ExeRECCONS(i.GetRecord(pos_INSTRTP_RECCONS_tag), i.GetInt(pos_INSTRTP_RECCONS_length));
      break;
    }

    case TAG_TYPE_INSTRTP_RECMOD: {
      ExeRECMOD(i.GetSequence(pos_INSTRTP_RECMOD_fid));
      break;
    }

    case TAG_TYPE_INSTRTP_FIELDSEL: {
      ExeFIELDSEL(i.GetRecord(pos_INSTRTP_FIELDSEL_nm));
      break;
    }

    case TAG_TYPE_INSTRTP_TOKENVAL: {
      ExeTOKENVAL();
      break;
    }

    case TAG_TYPE_INSTRTP_SEQCOMPBIND: {
      ExeSEQCOMPBIND(i.GetBool(pos_INSTRTP_SEQCOMPBIND_isSet));
      break;
    }

    case TAG_TYPE_INSTRTP_SEQFOR: {
      ExeSEQFOR();
      break;
    }

    case TAG_TYPE_INSTRTP_SEQELEMMATCH: {
      ExeSEQELEMMATCH(i.GetInt(pos_INSTRTP_SEQELEMMATCH_index));
      break;
    }

    case TAG_TYPE_INSTRTP_SEQMAPOVER: {
      ExeSEQMAPOVER();
      break;
    }

    case TAG_TYPE_INSTRTP_TUPSEL: {
      ExeTUPSEL(i.GetReal(pos_INSTRTP_TUPSEL_index));
      break;
    }

    case TAG_TYPE_INSTRTP_TYPEJUDGE: {
      ExeTYPEJUDGE(i.GetRecord(pos_INSTRTP_TYPEJUDGE_tp));
      break;
    }

    case TAG_TYPE_INSTRTP_POLYINST: {
      ExePOLYINST(i.GetSequence(pos_INSTRTP_POLYINST_inst),
                  i.GetRecord(pos_INSTRTP_POLYINST_nm));
      break;
    }

    case TAG_TYPE_INSTRTP_MKEXIT: {
      ExeMKEXIT(i.GetBool(pos_INSTRTP_MKEXIT_isnil), i.GetInt(pos_INSTRTP_MKEXIT_cid));
      break;
    }

    case TAG_TYPE_INSTRTP_EXITVAL: {
      ExeEXITVAL();
      break;
    }

    case TAG_TYPE_INSTRTP_ISCONT: {
      ExeISCONT();
      break;
    }

    case TAG_TYPE_INSTRTP_ISEXIT: {
      ExeISEXIT();
      break;
    }

    case TAG_TYPE_INSTRTP_ISNEEXIT: {
      ExeISNEEXIT();
      break;
    }

    case TAG_TYPE_INSTRTP_REMEXITVAL: {
      ExeREMEXITVAL();
      break;
    }

    case TAG_TYPE_INSTRTP_PUSHTH: {
      ExePUSHTH(i.GetInt(pos_INSTRTP_PUSHTH_handid));
      break;
    }

    case TAG_TYPE_INSTRTP_POPTH: {
      ExePOPTH();
      break;
    }

    case TAG_TYPE_INSTRTP_HANDID: {
      break;
    }

    case TAG_TYPE_INSTRTP_VERIFYINDEXARGS: {
      ExeVERIFYINDEXARGS();
      break;
    }

    case TAG_TYPE_INSTRTP_TESTCOUNTER: {
      ExeTESTCOUNTER();
      break;
    }

    case TAG_TYPE_INSTRTP_INCRCOUNTER: {
      ExeINCRCOUNTER();
      break;
    }

    case TAG_TYPE_INSTRTP_NONDETSTMT: {
      ExeNONDETSTMT();
      break;
    }

    case TAG_TYPE_INSTRTP_RANDOM: {
      ExeRANDOM();
      break;
    }

    case TAG_TYPE_INSTRTP_ATOMIC: {
      ExeATOMIC(i.GetInt(pos_INSTRTP_ATOMIC_number));
      break;
    }

    case TAG_TYPE_INSTRTP_LASTRES: {
      Push(this->lastres);
      break;
    }

    case TAG_TYPE_INSTRTP_MACRO: {
      ExeMACRO(i.GetInt(pos_INSTRTP_MACRO_tp), i.GetInt(pos_INSTRTP_MACRO_cid));
      break;
    }

    case TAG_TYPE_INSTRTP_GETPRE: {
      ExeGETPRE();
      break;
    }

    case TAG_TYPE_INSTRTP_NOBODY: {
  
#ifdef VDMPP
      SaveCurrntEvaluatorStateToCPUSigma();
#endif // VDMPP

      ExeNOBODY(i.GetInt(pos_INSTRTP_NOBODY_err),
                i.GetRecord(pos_INSTRTP_NOBODY_modname),
                i.GetRecord(pos_INSTRTP_NOBODY_name),
                i.GetSequence(pos_INSTRTP_NOBODY_parms));
      break;
    }

    case TAG_TYPE_INSTRTP_TPTOSET: {
      ExeTPTOSET(i.GetRecord(pos_INSTRTP_TPTOSET_tp));
      break;
    }

    case TAG_TYPE_INSTRTP_PUSHCONTEXT: {
      const Generic & context (i.GetField(pos_INSTRTP_PUSHCONTEXT_context));
      PushContext(context);
      break;
    }

    case TAG_TYPE_INSTRTP_POPCONTEXT: {
      PopContext();
      break;
    }

    case TAG_TYPE_INSTRTP_CHECKCONTEXT: {
      CheckContext(i.GetSet(pos_INSTRTP_CHECKCONTEXT_contexts),
                   i.GetInt(pos_INSTRTP_CHECKCONTEXT_err));
      break;
    }
    //////////////////////////////////////////////////////////////////
    // VDM++ Instructions
    //////////////////////////////////////////////////////////////////
#ifdef VDMPP
    case TAG_TYPE_INSTRTP_NEWOBJ: {
      ExeNEWOBJ(i.GetRecord(pos_INSTRTP_NEWOBJ_nm), i.GetField(pos_INSTRTP_NEWOBJ_dlobject));
      break;
    }

    case TAG_TYPE_INSTRTP_NEWPOSABSOBJ: {
      ExeNEWPOSABSOBJ(i.GetRecord(length_INSTRTP_NEWPOSABSOBJ), Nil());
      break;
    }

    case TAG_TYPE_INSTRTP_INITCLASS: {
      ExeINITCLASS(i.GetRecord(pos_INSTRTP_INITCLASS_nm), i.GetInt(pos_INSTRTP_INITCLASS_initno));
      break;
    }

    case TAG_TYPE_INSTRTP_BINDINSTVAR: {
      ExeBINDINSTVAR(i.GetRecord(pos_INSTRTP_BINDINSTVAR_cls), i.GetRecord(pos_INSTRTP_BINDINSTVAR_nm));
      break;
    }

    case TAG_TYPE_INSTRTP_NEWCOMPL: {
      ExeNEWCOMPL(i.GetBool(pos_INSTRTP_NEWCOMPL_checkinv));
      break;
    }

    case TAG_TYPE_INSTRTP_PUSHCLNMCUROBJ: {
      ExePUSHCLNMCUROBJ(i.GetRecord(pos_INSTRTP_PUSHCLNMCUROBJ_cl), i.GetRecord(pos_INSTRTP_PUSHCLNMCUROBJ_origcl));
      break;
    }

    case TAG_TYPE_INSTRTP_POPCLNMCUROBJ: {
      ExePOPCLNMCUROBJ();
      break;
    }

    case TAG_TYPE_INSTRTP_SELFEXPR: {
      ExeSELFEXPR();
      break;
    }

    case TAG_TYPE_INSTRTP_ISOFCLASS: {
      ExeISOFCLASS(i.GetRecord(pos_INSTRTP_ISOFCLASS_nm));
      break;
    }

    case TAG_TYPE_INSTRTP_ISOFBASECLASS: {
      ExeISOFBASECLASS(i.GetRecord(pos_INSTRTP_ISOFBASECLASS_nm));
      break;
    }

    case TAG_TYPE_INSTRTP_SAMEBASECLASS: {
      ExeSAMEBASECLASS();
      break;
    }

    case TAG_TYPE_INSTRTP_SAMECLASS: {
      ExeSAMECLASS();
      break;
    }

    case TAG_TYPE_INSTRTP_HISTORY: {
      ExeHISTORY(i.GetRecord(pos_INSTRTP_HISTORY_kind), i.GetSequence(pos_INSTRTP_HISTORY_opnms));
      break;
    }

    case TAG_TYPE_INSTRTP_STARTLIST: {
      ExeSTARTLIST(i.GetBool(pos_INSTRTP_STARTLIST_islist));
      break;
    }

    case TAG_TYPE_INSTRTP_STOPLIST: {
      ExeSTOPLIST(i.GetBool(pos_INSTRTP_STOPLIST_islist));
      break;
    }

    case TAG_TYPE_INSTRTP_GUARD: {
      ExeGUARD(i.GetBool(pos_INSTRTP_GUARD_log));
      break;
    }

    case TAG_TYPE_INSTRTP_THREADID : {
      ExeTHREADID();
      break;
    }

#ifdef VICE
    case TAG_TYPE_INSTRTP_RETMES: {
      ExeRETMES(i.GetInt(pos_INSTRTP_RETMES_id),
                i.GetInt(pos_INSTRTP_RETMES_prio),
                i.GetRecord(pos_INSTRTP_RETMES_opnm),
                i.GetField(pos_INSTRTP_RETMES_tocpu),
                i.GetInt(pos_INSTRTP_RETMES_threadid));
      break;
    }

    case TAG_TYPE_INSTRTP_INCRTIME: {
      ExeINCRTIME(i.GetInt(pos_INSTRTP_INCRTIME_time));
      break;
    }

    case TAG_TYPE_INSTRTP_RUNTIME_uINCRTIME_uPREF: {
      ExeINCRTIME_PREF(i.GetField(pos_INSTRTP_RUNTIME_uINCRTIME_uPREF_opr),
                       i.GetInt(pos_INSTRTP_RUNTIME_uINCRTIME_uPREF_oh));
      break;
    }

    case TAG_TYPE_INSTRTP_RUNTIME_uINCRTIME_uBIN: {
      ExeINCRTIME_BIN(i.GetField(pos_INSTRTP_RUNTIME_uINCRTIME_uBIN_opr),
                      i.GetInt(pos_INSTRTP_RUNTIME_uINCRTIME_uBIN_oh1),
                      i.GetField(pos_INSTRTP_RUNTIME_uINCRTIME_uBIN_oh2));
      break;
    }

    case TAG_TYPE_INSTRTP_RUNTIME_uINCRTIME_uSETSEQMAP: {
      ExeINCRTIME_SETSEQMAP(i.GetInt(pos_INSTRTP_RUNTIME_uINCRTIME_uSETSEQMAP_oh));
      break;
    }

    case TAG_TYPE_INSTRTP_RUNTIME_uINCRTIME_uNEW: {
      ExeINCRTIME_NEW(i.GetInt(pos_INSTRTP_RUNTIME_uINCRTIME_uNEW_oh),
                      i.GetRecord(pos_INSTRTP_RUNTIME_uINCRTIME_uNEW_nm));
      break;
    }

    case TAG_TYPE_INSTRTP_RUNTIME_uINCRTIME_uSTARTLIST: {
      ExeINCRTIME_STARTLIST(i.GetInt(pos_INSTRTP_RUNTIME_uINCRTIME_uSTARTLIST_oh));
      break;
    }

    case TAG_TYPE_INSTRTP_RUNTIME_uINCRTIME_uSTOPLIST: {
      ExeINCRTIME_STOPLIST(i.GetInt(pos_INSTRTP_RUNTIME_uINCRTIME_uSTOPLIST_oh));
      break;
    }

    case TAG_TYPE_INSTRTP_PUSHDURATION: {
      theSystem().PushDuration();
      break;
    }

    case TAG_TYPE_INSTRTP_POPDURATION: {
      const Generic & t (i.GetField(pos_INSTRTP_POPDURATION_time));
      if (t.IsNil()) {
        TYPE_SEM_NUM tm (this->cs_eval_stack_p->Pop());
        theSystem().PopDuration(tm.GetReal(pos_SEM_NUM_v));
      }
      else {
        theSystem().PopDuration(t);
      }
      break;
    }

    case TAG_TYPE_INSTRTP_CURTIME: {
      Push(TYPE_SEM_NUM().Init(Real(theSystem().GetTime())));
      break;
    }

    case TAG_TYPE_INSTRTP_PUSHCYCLE: {
      theSystem().PushCycle();
      break;
    }

    case TAG_TYPE_INSTRTP_POPCYCLE: {
      const Generic & c (i.GetField(pos_INSTRTP_POPCYCLE_time));
      if (c.IsNil()) {
        TYPE_SEM_NUM tm (this->cs_eval_stack_p->Pop());
        theSystem().PopCycle(tm.GetReal(pos_SEM_NUM_v));
      }
      else {
        theSystem().PopCycle(c);
      }
      break;
    }
#endif //VICE

#endif //VDMPP
    case TAG_TYPE_INSTRTP_COMMENT: {
      TYPE_CI_ContextId cid (i.GetSequence(pos_INSTRTP_COMMENT_cid));
      if (cid != NilContextId)
      {
        Tuple gflcp (GetCI().GetFileLineColPos(cid));
        vdm_iplog << gflcp.GetSequence(1) << L", l. " << gflcp.GetInt(2) << L", c. " << gflcp.GetInt(3) << endl;
      }
      vdm_iplog << i.GetSequence(pos_INSTRTP_COMMENT_txt).GetString() << endl;
      break;
    }
    case TAG_TYPE_INSTRTP_ISTART: {
      ExeISTART(i.GetSequence(pos_INSTRTP_ISTART_txt), i.GetInt(pos_INSTRTP_ISTART_cid));
      break;
    }

    case TAG_TYPE_INSTRTP_IEND: {
      ExeIEND();
      break;
    }

    default: {
      RTERR::Error(L"EvalInstr", RTERR_INVALID_INSTRUCTION, Nil(), Nil(), Sequence());
    }
  }
#ifdef PROFILING
  uint64_t posttm = TBUTILS::GetCurTimeMil();
  instrclk[index] += posttm - pretm;
  instrcnt[index] += 1;
#endif // PROFILING
}

// GetEnvLLengths
// ==> nat * nat
Tuple StackEval::GetEnvLLengths() const
{
  if( this->cs_env_l_p->IsEmpty() )
    return mk_(Int(0), Int(0));
  else
    return mk_(Int(this->cs_env_l_p->Length()), Int(this->cs_env_l_p->Hd().Length()));
}

// UpgradeENVL
// lenvl : nat
// ltopenvl : nat
// ==> ()
void StackEval::UpgradeENVL(const Int & lenvl, const Int & ltopenvl)
{
  if (!this->cs_env_l_p->IsEmpty())
  {
    size_t len_env_l = this->cs_env_l_p->Length();
    TYPE_SEM_ENVL new_env_l (this->cs_env_l_p->SubSequence(len_env_l - lenvl + 1, len_env_l));
    SEQ<TYPE_SEM_BlkEnv> blkenv_l (new_env_l[1]);
    size_t len_blkenv_l = blkenv_l.Length();
    new_env_l.ImpModify(1, blkenv_l.SubSequence(len_blkenv_l - ltopenvl + 1, len_blkenv_l));
    *this->cs_env_l_p = new_env_l;
  }
}

// PopEnvL
// ==> ()
void StackEval::PopEnvL ()
{
  this->cs_env_l_p->ImpTl();
}

// TopEnvL
// ==> SEM`ENV
SEQ<TYPE_SEM_BlkEnv> StackEval::TopEnvL() const
{
  return this->cs_env_l_p->Hd();
}

// PushEmptyBlkEnv
// permis : SEM`Permission
// ==> ()
void StackEval::PushEmptyBlkEnv(const TYPE_SEM_Permission & permis)
{
  TYPE_SEM_BlkEnv env (AUX::MkEmptyBlkEnv(permis));
  this->cs_env_l_p->GetIndexRef(1).ImpPrepend(env);
}

// PushEmptyEnv
// ==> ()
void StackEval::PushEmptyEnv ()
{
  this->cs_env_l_p->ImpPrepend(SEQ<TYPE_SEM_BlkEnv>());
}

// PushEnvL
// ==> ()
void StackEval::PushEnvL (const SEQ<TYPE_SEM_BlkEnv> & env_l)
{
  this->cs_env_l_p->ImpPrepend(env_l);
}

// IsEmptyEnvL
// ==> bool
bool StackEval::IsEmptyEnvL () const
{
  return this->cs_env_l_p->Hd().IsEmpty();
}

//Block environment operations
// AddToTopBlkEnv
// blkenv : SEM`BlkEnv
// ==> ()
void StackEval::AddToTopBlkEnv(const TYPE_SEM_BlkEnv& blkenv)
{
  SEQ<TYPE_SEM_BlkEnv> & blkenv_l (this->cs_env_l_p->GetIndexRef(1)); 
  TYPE_SEM_BlkEnv new_env (AUX::CombineBlkEnv(blkenv_l.Hd(), blkenv));
  blkenv_l.ImpModify(1, new_env);
}

#ifdef VDMPP
// GetObjLLen
// ==> nat
int StackEval::GetObjLLen() const
{
  return this->cs_obj_l_p->Length();
}
#endif // VDMPP

// PopBlkEnv
// ==> ()
void StackEval::PopBlkEnv ()
{
  this->cs_env_l_p->GetIndexRef(1).ImpTl();
}

// PushBlkEnv
// end : SEM`BlkEnv
// ==> ()
void StackEval::PushBlkEnv (const TYPE_SEM_BlkEnv & env)
{
  this->cs_env_l_p->GetIndexRef(1).ImpPrepend(env);
}

// AppendToTopBlkEnv
// id : AS`Name
// val : SEM`VAL
// tp : [AS`Type]
void StackEval::AppendToTopBlkEnv(const TYPE_AS_Name & id, const TYPE_SEM_VAL & val, const Generic & tp)
{
//  SEQ<TYPE_SEM_BlkEnv> & blkenv_l (this->cs_env_l_p->GetIndexRef(1)); 
//  TYPE_SEM_BlkEnv & blkenv (blkenv_l.GetIndexRef(1));
  TYPE_SEM_BlkEnv & blkenv (this->cs_env_l_p->GetIndexRef(1).GetIndexRef(1));

//  MAP<TYPE_AS_Name,TYPE_SEM_ValTp> id_m (blkenv.GetMap(pos_SEM_BlkEnv_id_um));
//  id_m.ImpModify(id, TYPE_SEM_ValTp().Init(val, tp));
//  blkenv.SetField(pos_SEM_BlkEnv_id_um, id_m);
  ((Map &)(blkenv.GetFieldRef(pos_SEM_BlkEnv_id_um))).ImpModify(id, TYPE_SEM_ValTp().Init(val, tp));
}

// TopBlkEnv
// ==> SEM`BlkEnv
TYPE_SEM_BlkEnv StackEval::TopBlkEnv() const
{
  return this->cs_env_l_p->Hd().Hd();
}

#ifdef VDMPP
// MakeNewObj
// semobj : SEM`OBJ
// dlclassp : [VDMContainer<DlClassInstanceHolder>]
// ==> SEM`OBJ_Ref
TYPE_SEM_OBJ_uRef StackEval::MakeNewObj(const TYPE_SEM_OBJ & semobj, const Generic & dlclassp)
{
  TYPE_SEM_OBJ_uRef ref (theState().global_obj_tab_insert(semobj, dlclassp));

#ifdef VICE
  theSystem().AddNewObj(ref);
#endif // VICE

  return ref;
}
#endif // VDMPP

// GoUp
// ==> bool
bool StackEval::GoUp()
{
#ifdef VDMPP
  LoadCurrntEvaluatorStateFromCPUSigma();
#endif // VDMPP

  Generic levels (GetNextStackLevelsUp());

  if (levels.IsNil()) {
    vdm_iplog << L"You cannot go further up." << endl << flush;
    return false;
  }
  else {
    UpdateStacks(levels);
  }

#ifdef VDMPP
  SaveCurrntEvaluatorStateToCPUSigma();
#endif // VDMPP

  return true;
}

// GoDown
// ==> bool
bool StackEval::GoDown()
{
#ifdef VDMPP
  LoadCurrntEvaluatorStateFromCPUSigma();
#endif // VDMPP

  switch(this->cs_shared_p->upDnIndex.GetValue()) {
    case 0: {
      vdm_iplog << L"You cannot go further down." << endl << flush;
      return false;
    }
    case 1: {
      ResetUpDn();
    }
    default: {
      Generic levels (GetNextStackLevelsDown());
      if (levels.IsNil()) {
        vdm_iplog << L"Internal error when going down" << flush;
        return false;
      }
      else {
        // current status modified if levels is't nil
        UpdateStacks(levels);
      }
    }
  }

#ifdef VDMPP
    SaveCurrntEvaluatorStateToCPUSigma();
#endif // VDMPP

  return true;
}

// UpdateStacks
// levels : (nat * nat * nat * nat)
// ==> ()
void StackEval::UpdateStacks(const Tuple & levels)
{
  int env_l_h         = levels.GetIntValue(1);
  int typeinst_h      = levels.GetIntValue(2);
  int os_h            = levels.GetIntValue(3);
  int cur_mod_obj_l_h = levels.GetIntValue(4);

  EvaluatorStatus & state (*this->cs_shared_p); 

  // Update env_l and env_l_bak
  TYPE_SEM_ENVL env_l (state.env_l_bak ^ state.env_l);
  size_t len_env_l = env_l.Length();
  state.env_l = env_l.SubSequence(len_env_l - env_l_h + 1, len_env_l);
  state.env_l_bak = env_l.SubSequence(1, len_env_l - env_l_h);

  // Update typeinst and typeinst_bak
  Sequence typeinst (state.typeinst_bak ^ state.typeinst);
  size_t len_typeinst = typeinst.Length();
  state.typeinst = typeinst.SubSequence(len_typeinst - typeinst_h + 1, len_typeinst);
  state.typeinst_bak = typeinst.SubSequence(1, len_typeinst - typeinst_h);

#ifdef VDMSL
  // Update os and os_bak
  Sequence os (state.os_bak ^ state.os);
  size_t len_os = os.Length();
  state.os = os.SubSequence(len_os - os_h + 1, len_os);
  state.os_bak = os.SubSequence(1, len_os - os_h);

  // Update cur_mod and cur_mod_bak
  Sequence cur_mod (state.cur_mod_bak ^ state.cur_mod);
  size_t len_cur_mod = cur_mod.Length();
  state.cur_mod = SEQ<TYPE_AS_Name>(cur_mod.SubSequence(len_cur_mod - cur_mod_obj_l_h + 1, len_cur_mod));
  state.cur_mod_bak = SEQ<TYPE_AS_Name>(cur_mod.SubSequence(1, len_cur_mod - cur_mod_obj_l_h));
#endif // VDMSL

#ifdef VDMPP
  // Update os and os_bak
  Sequence os (state.os_bak ^ state.os);
  size_t len_os = os.Length();
  state.os = os.SubSequence(len_os - os_h + 1, len_os);
  state.os_bak = os.SubSequence(1, len_os - os_h);

  // Update obj_l and obj_l_bak
  Sequence obj_l (state.obj_l_bak ^ state.obj_l);
  size_t len_obj_l = obj_l.Length();
  state.obj_l = SEQ<TYPE_GLOBAL_OBJscope>(obj_l.SubSequence(len_obj_l - cur_mod_obj_l_h + 1, len_obj_l));
  state.obj_l_bak = SEQ<TYPE_GLOBAL_OBJscope>(obj_l.SubSequence(1, len_obj_l - cur_mod_obj_l_h));
#endif //VDMPP
}

// ResetUpDn
// ==> ()
void StackEval::ResetUpDn()
{
  EvaluatorStatus & state (*this->cs_shared_p); 

  UpdateStacks(mk_(Int(state.env_l.Length() + state.env_l_bak.Length()),
                   Int(state.typeinst.Length() + state.typeinst_bak.Length()),
                   Int(state.os.Length() + state.os_bak.Length()),
#ifdef VDMSL
                   Int(state.cur_mod.Length() + state.cur_mod_bak.Length())
#endif //VDMSL
#ifdef VDMPP
                   Int(state.obj_l.Length() + state.obj_l_bak.Length())
#endif //VDMPP
                  )
               );
  state.upDnIndex = Int(0);
}

// GetNextStackLevelsUp
// ==> [nat * nat * nat * nat]
Generic StackEval::GetNextStackLevelsUp()
{
  EvaluatorStatus & state (*this->cs_shared_p); 

  size_t index = state.upDnIndex.GetValue();
  int indexFound = -1;

  size_t cslen = state.call_stack.Length();
  while (index < cslen) {
    index++;
    TYPE_STKM_CallStackItem csi (state.call_stack.GetNth(index));
    switch(csi.get_type()) {
      case CallStackItemType::CS_FNOP: {
        if (indexFound == -1) {
          indexFound = index;
        }
        else
        {
          const TYPE_STKM_CallStackItem & found (state.call_stack.GetNth(indexFound));
          state.upDnIndex = Int(indexFound);

          return mk_(found.GetInt(pos_STKM_CallStackItem_env_ul_uh),
                     found.GetInt(pos_STKM_CallStackItem_typeinst_uh),
                     found.GetInt(pos_STKM_CallStackItem_os_uh),
                     found.GetInt(pos_STKM_CallStackItem_cur_umod_uobj_ul_uh));
        }
        break;
      }
      case CallStackItemType::CS_DEBUGCMD: {
        return Nil();
      }
    }
  }
  return Nil();
}

// GetNextStackLevelsDown
// ==> [nat * nat * nat * nat]
Generic StackEval::GetNextStackLevelsDown()
{
  EvaluatorStatus & state (*this->cs_shared_p); 

  int index = state.upDnIndex.GetValue();

  while (index > 1) {
    index--;
    TYPE_STKM_CallStackItem csi (state.call_stack.GetNth(index));
    if (csi.get_type() == CallStackItemType::CS_FNOP)
    {
      state.upDnIndex = Int(index);

      return mk_(csi.GetInt(pos_STKM_CallStackItem_env_ul_uh),
                 csi.GetInt(pos_STKM_CallStackItem_typeinst_uh),
                 csi.GetInt(pos_STKM_CallStackItem_os_uh),
                 csi.GetInt(pos_STKM_CallStackItem_cur_umod_uobj_ul_uh));
    }
  }
  return Nil();
}

// CurrentBacktraceLevel
// ==> nat
int StackEval::CurrentBacktraceLevel() const
{
  return this->cs_shared_p->env_l_bak.Length() + 1;
}

// GetCidForCurBacktraceLevel
// ==> CI`ContextId
TYPE_CI_ContextId StackEval::GetCidForCurBacktraceLevel() const
{
  const EvaluatorStatus & state (*this->cs_shared_p); 

  size_t level = state.env_l_bak.Length();

  if (level == 0) {
    return GetCurCid();
  }
  else
  {
    size_t len = state.call_stack.Length();

    size_t curLevel = 1;
    for (size_t i = 1; i <= len; i++)
    {
      const TYPE_STKM_CallStackItem & elm (state.call_stack.GetNth(i));
      CallStackItemType::CSItemType type =
                     (CallStackItemType::CSItemType)(elm.GetIntValue(pos_STKM_CallStackItem_type));

      if (type == CallStackItemType::CS_FNOP) {
        if (level == curLevel) {
          return elm.GetInt(pos_STKM_CallStackItem_oldCid);
        }
        else {
          curLevel++;
        }
      }
    }
  }
  vdm_iplog << L"Internal error in function ";
  vdm_iplog << L"StackEval::GetCidForCurBacktraceLevel, please report";
  vdm_iplog << endl << flush;
  return NilContextId; // dummy
}

#ifdef VDMSL
// PushModule
// mod_name : AS`Name
// ==> ()
void StackEval::PushModule (const TYPE_AS_Name & mod_name)
{
  this->cs_cur_mod_p->ImpPrepend(mod_name);
}

// PopModule
// ==> ()
void StackEval::PopModule ()
{
  if (!this->cs_cur_mod_p->IsEmpty())
    this->cs_cur_mod_p->ImpTl ();
}

// ResetCurMod
// ==> ()
void StackEval::ResetCurMod()
{
  this->cs_cur_mod_p->Clear();
}

// CurrentModule
// ==> AS`Name
const TYPE_AS_Name & StackEval::CurrentModule (void) const
{
  if (!this->cs_cur_mod_p->IsEmpty())
    //return this->cs_cur_mod_p->Hd ();
    return this->cs_cur_mod_p->Index(1);
  else
    // this situation can occur if someone use print
    // before init is called, which is allowed
    // Tue Nov  7 11:45:32 1995 -- Jesper
    //return MkNameFromVoid();
    return this->void_name; // 20100906
}

// PrePushModule
// mkd_name : AS`Name
// ==> bool
bool StackEval::PrePushModule(const TYPE_AS_Name & mod_name)
{
  if (this->cs_cur_mod_p->IsEmpty())
    return false;
 
  if (AUX::EqualNames(mod_name, HeadModule()))
    return false;

  this->PushModule(mod_name);
  return true;
}

TYPE_AS_Name StackEval::HeadModule() const
{
  return this->cs_cur_mod_p->Hd ();
}

#endif //VDMSL

// IsLocalState
// id : AS`Name
// ==> bool * [SEM`ValTp]
Tuple StackEval::IsLocalState(const TYPE_AS_Name & id) const
{
  const SEQ<TYPE_SEM_BlkEnv> & blkenv_l (this->cs_env_l_p->GetIndexRef(1));
  size_t len = blkenv_l.Length();
  for (size_t i = 1; i <= len; i++)
  {
    const TYPE_SEM_BlkEnv & blkenv (blkenv_l[i]);
    const Map & id_m (blkenv.GetMap(pos_SEM_BlkEnv_id_um));
    if (id_m.DomExists(id))
    {
      if (blkenv.GetField(pos_SEM_BlkEnv_perm) == sem_read_write)
        return mk_(Bool(true), id_m[id]);
      else
      {
#ifdef VDMPP
        const TYPE_SEM_ValTp & val (id_m[id]);
        if (val.GetRecord(pos_SEM_ValTp_val).Is(TAG_TYPE_SEM_OBJ_uRef))
          return mk_(Bool(false), Nil());
#endif // VDMPP
        return mk_(Bool(true), Nil());
      }
    }
// <-- 20091102
  }
  return mk_(Bool(false), Nil());
}

// SetLocalState
// id : AS`Name
// valtp : SEM`ValTp
// ==> ()
void StackEval::SetLocalState(const TYPE_AS_Name & id, const TYPE_SEM_ValTp & valtp)
{
  SEQ<TYPE_SEM_BlkEnv> & blkenv_l (this->cs_env_l_p->GetIndexRef(1)); 
  size_t len_blkenv_l = blkenv_l.Length();
  for (size_t i = 1; i <= len_blkenv_l; i++)
  {
    TYPE_SEM_BlkEnv & blkenv (blkenv_l.GetIndexRef(i));
    if((blkenv.GetField(pos_SEM_BlkEnv_perm) == sem_read_write) &&
       (blkenv.GetMap(pos_SEM_BlkEnv_id_um).DomExists(id)))
    {
      Map & id_m ((Map &)blkenv.GetFieldRef(pos_SEM_BlkEnv_id_um));
      id_m.ImpModify(id, valtp);
      break;
    }
  }
}

// IsLocalVal
// name : AS`Name
// ==> bool * [SEM`ValTp]
Tuple StackEval::IsLocalVal (const TYPE_AS_Name & name)
{
  const SEQ<TYPE_SEM_BlkEnv> & blkenv_l (this->cs_shared_p->env_l[1]); 

  if (!blkenv_l.IsEmpty()) {
    size_t len_blkenv_l = blkenv_l.Length();
    for (size_t i = 1; i <= len_blkenv_l; i++) {
      const TYPE_SEM_BlkEnv & blkenv (blkenv_l[i]);
      const MAP<TYPE_AS_Name,TYPE_SEM_ValTp> & id_m (blkenv.GetMap(pos_SEM_BlkEnv_id_um));
      if(id_m.DomExists(name)) {
        return mk_(Bool(true), id_m[name]);
      }
    }
  }
  return mk_(Bool(false), Nil());
}

#ifdef VDMSL
// IsOldState
// id : AS`Name
// ==> bool
int StackEval::IsOldState (const TYPE_AS_Name & id) const
{
  return Map(this->cs_os_p->Hd()).DomExists (id);
}

// GetOldState
// id : AS`Name
// ==> GLOBAL`State
TYPE_GLOBAL_State StackEval::GetOldState (const TYPE_AS_Name & id) const
{
  return Map(this->cs_os_p->Hd())[id];
}

// EvalOldName
// oldid : AS`OldName
// ==> SEM`VAL
TYPE_SEM_VAL StackEval::EvalOldName (const TYPE_AS_OldName & oldnm)
{
  const TYPE_AS_Ids & name (oldnm.GetSequence(pos_AS_OldName_ids));
  const TYPE_AS_Id & id (name[name.Length()]);
  TYPE_AS_Name orig_name (ASTAUX::MkNameFromId(id, oldnm.GetInt(pos_AS_OldName_cid)));

  if (IsOldState (orig_name)) {
    TYPE_GLOBAL_State st (GetOldState(orig_name));
    return st.GetRecord(pos_GLOBAL_State_val);
  }
  else {
    return RTERR::ErrorVal (L"EvalOldName", RTERR_OLDID_NOT_IN_OS, Nil(), Nil(), Sequence());
  }
}
#endif //VDMSL

#ifdef VDMPP
// EvalOldName
// oldname : AS`OldName
// ==> SEM`VAL
TYPE_SEM_VAL StackEval::EvalOldName(const TYPE_AS_OldName & oldname)
{
  TYPE_AS_Name orig_name;
  orig_name.Init(oldname.GetSequence(pos_AS_OldName_ids), oldname.GetInt(pos_AS_OldName_cid));

  const TYPE_SEM_OBJ_uRef & ref (GetCurObjRef());
  const ObjTabCt & old_obj_tab ((const ObjTabCt &)(Tuple(this->cs_os_p->Hd()).GetMap(1)));
  if (old_obj_tab.DomExists(ref))
  {
    const TYPE_GLOBAL_OBJ_uDesc & desc (old_obj_tab[ref]);
    TYPE_SEM_OBJ old_obj_sem (desc.GetRecord(pos_GLOBAL_OBJ_uDesc_sem));
    MAP<TYPE_AS_Name,TYPE_GLOBAL_SigmaClass> classes_m (Tuple(this->cs_os_p->Hd()).GetMap(2));
    const TYPE_AS_Name & clsnm (old_obj_sem.GetRecord(pos_SEM_OBJ_tp));
    if (classes_m.DomExists(clsnm))
    {
      const TYPE_GLOBAL_ValueMap & statics (classes_m[clsnm].GetMap(pos_GLOBAL_SigmaClass_statics));
      if (!statics.IsEmpty())
      {
        Map ins (old_obj_sem.GetMap(pos_SEM_OBJ_ins));
        TYPE_GLOBAL_ValueMap vm (ins[clsnm]);
        vm.ImpOverride(statics);
        ins.ImpModify(clsnm, vm);
        old_obj_sem.SetField(pos_SEM_OBJ_ins, ins);
      }
    }

    // ==> bool * bool * [SEM`VAL] * [AS`Type] * [AS`Name] * [AS`Access]
    Tuple iios (theState().IsInObjScope(orig_name, old_obj_sem));
    if (iios.GetBoolValue(1))
      return iios.GetRecord(3);  // [SEM`VAL]
    else
    {
      return RTERR::ErrorVal(L"EvalOldName", RTERR_OLDID_NOT_IN_OS, Nil(), Nil(), Sequence());
    }
  }
  else
  {
    return RTERR::ErrorVal (L"EvalOldName", RTERR_INTERNAL_ERROR, Nil(), Nil(), Sequence());
  }
}
#endif //VDMPP

// PushTypeInst
// typemap : map AS`TypeVar to AS`Type
void StackEval::PushTypeInst(const Map & typemap)
{
  this->cs_typeinst_p->ImpPrepend (typemap);
}

// PopTypeInst
void StackEval::PopTypeInst()
{
  this->cs_typeinst_p->ImpTl();
}

// HdTypeInst
// ==> map AS`TypeVar to AS`Type
Map StackEval::HdTypeInst() const
{
  return this->cs_typeinst_p->Hd ();
}

// PushContext
// context : STKM`Context
void StackEval::PushContext(const TYPE_STKM_Context & context)
{
//wcout << L"PushContext: " << INT2Q::h2gAS(*this->cs_context_p);
  this->cs_context_p->ImpPrepend (context);
//wcout << L" -> " << INT2Q::h2gAS(*this->cs_context_p) << endl;
}

// PopContext
void StackEval::PopContext()
{
//wcout << L"PopContext: " << INT2Q::h2gAS(*this->cs_context_p);
  this->cs_context_p->ImpTl();
//wcout << L" -> " << INT2Q::h2gAS(*this->cs_context_p) << endl;
}

// HdContext
// ==> STKM`Context
TYPE_STKM_Context StackEval::HdContext() const
{
  return this->cs_context_p->Hd ();
}

// CheckContext
// contexts : set of Context
void StackEval::CheckContext(const SET<TYPE_STKM_Context> & contexts, const TYPE_RTERR_ERR & err)
{
  TYPE_STKM_Context context (HdContext());
  if (!contexts.InSet(context))
    RTERR::Error(L"StackEval::CheckContext", err, Nil(), Nil(), Sequence());
}

#ifdef VDMSL
// PushOS
// ==> ()
void StackEval::PushOS()
{
  if (UsesOldName())
    this->cs_os_p->ImpPrepend(theState().GetModuleState(CurrentModule()));
}
#endif //VDMSL
#ifdef VDMPP
// PushOS
// ==> ()
void StackEval::PushOS()
{
//wcout << L"PushOS: " << this->cs_os_p->Length() << L" -> " <<  this->cs_os_p->Length() + 1 << endl;
  if (UsesOldName())
    this->cs_os_p->ImpPrepend(theState().Get_obj_tab());
}
#endif //VDMPP

// PopOS
// ==> ()
void StackEval::PopOS()
{
  if (UsesOldName())
    this->cs_os_p->ImpTl ();
//wcout << L"PopOS:  " << this->cs_os_p->Length() + 1 <<  L" -> " << this->cs_os_p->Length() << endl;
}

#ifdef VDMPP
// HeadObjL
// ==> GLOBAL`OBJscope
const TYPE_GLOBAL_OBJscope & StackEval::HeadObjL() const
{
  //return this->cs_obj_l_p->Hd();
  return this->cs_obj_l_p->Index(1);
}

// GetCurObjRef
// ==> SEM`OBJ_Ref
const TYPE_SEM_OBJ_uRef & StackEval::GetCurObjRef() const
{
//  return this->cs_obj_l_p->Hd().GetRecord(pos_GLOBAL_OBJscope_obj);
  const TYPE_GLOBAL_OBJscope & os (this->cs_obj_l_p->Index(1));
  return (const TYPE_SEM_OBJ_uRef &)(os.GetRecord(pos_GLOBAL_OBJscope_obj));
}

// From stack-eval.vdm
// GetOrigCl
// ==> GLOBAL`OrigCl = AS`Name | GLOBAL`Start
const TYPE_GLOBAL_OrigCl & StackEval::GetOrigCl() const
{
  if(this->cs_obj_l_p->IsEmpty())
    //return TYPE_GLOBAL_Start();
    return StackEval::global_start;
  else
  {
    //return this->cs_obj_l_p->Hd().GetSequence(pos_GLOBAL_OBJscope_origcl).Hd();
    const TYPE_GLOBAL_OBJscope & os (this->cs_obj_l_p->Index(1));
    return ((const SEQ<TYPE_GLOBAL_OrigCl> &)(os.GetSequence(pos_GLOBAL_OBJscope_origcl))).Index(1);
  }
}

// From stack-eval.vdm
// GetOrigOldCl
// ==> GLOBAL`OrigCl
const TYPE_GLOBAL_OrigCl & StackEval::GetOrigOldCl() const
{
  //SEQ<TYPE_GLOBAL_OrigCl> orig_cll (this->cs_obj_l_p->Hd().GetSequence(pos_GLOBAL_OBJscope_origcl));
  const TYPE_GLOBAL_OBJscope & os (this->cs_obj_l_p->Index(1));
  const SEQ<TYPE_GLOBAL_OrigCl> & orig_cll (os.GetSequence(pos_GLOBAL_OBJscope_origcl));
  if (orig_cll.Length() > 1)
    //return orig_cll.Tl().Hd();
    return orig_cll.Index(2);
  else
    //return orig_cll.Hd();
    return orig_cll.Index(1);
}

// IsClientModifOk
// access :
// HasCurCl == true
//bool StackEval::IsClientModifOk(const TYPE_AS_Access& access)
//{
//  if (access == Int(PUBLIC_AS))
//    return true;
//
////  TYPE_GLOBAL_OBJscope objscope (this->curr_state.get_const_ref().obj_l.Hd());
////  SEQ<TYPE_AS_Name> fields (objscope.get_class());     // seq of AS`Name
//  SEQ<TYPE_AS_Name> fields (HeadObjL().get_class());     // seq of AS`Name
//
////  if (this->curr_state.get_const_ref().obj_l.Length() < 2 && fields.Length() < 2)
//  if (GetObjLLen() < 2)
//    return true;
//
//  TYPE_AS_Name curcl (GetCurCl()); // HasCurCl() == true
//  Generic callcl;
//  if (fields.Length() > 1)
//    callcl = fields[2];
//  else
//    callcl = Sequence(Record((this->curr_state.get_const_ref().obj_l)[2]).GetField(2)).Hd();
//
//  if (access == Int(PRIVATE_AS) && curcl == callcl)
//    return true;
//
//  if (access == Int(PROTECTED_AS) &&
//      (callcl == curcl || GetAllSupers(curcl).InSet(callcl)))
//    return true;
//
//  return false;
//}

// GetCurObjName
// ==> AS`Name
TYPE_AS_Name StackEval::GetCurObjName() const
{
  return GetCurObj().GetRecord(pos_SEM_OBJ_tp);
}

// GetCurCl
// ==> GLOBAL`OrigCl = AS`Name | GLOBAL`Start
const TYPE_GLOBAL_OrigCl & StackEval::GetCurCl() const
{
  if (this->cs_obj_l_p->IsEmpty())
    //return TYPE_GLOBAL_Start();
    return StackEval::global_start;
  else
  {
    //return this->cs_obj_l_p->Hd().GetSequence(pos_GLOBAL_OBJscope_class).Hd();
    const TYPE_GLOBAL_OBJscope & os (this->cs_obj_l_p->Index(1));
    return ((const SEQ<TYPE_GLOBAL_OrigCl> &)(os.GetSequence(pos_GLOBAL_OBJscope_class))).Index(1);
  }
}

// GetPrevCl
// ==> GLOBAL`OrigCl = AS`Name | GLOBAL`Start
TYPE_GLOBAL_OrigCl StackEval::GetPrevCl() const
{
  switch(this->cs_obj_l_p->Length()) {
    case 0: {
      //return TYPE_GLOBAL_Start();
      return StackEval::global_start;
    }
    case 1: {
      const TYPE_GLOBAL_OBJscope & osc  (this->cs_obj_l_p->Index(1));
      const SEQ<TYPE_AS_Name> & cls_l (osc.GetSequence(pos_GLOBAL_OBJscope_class));
      if (cls_l.Length() > 1)
//        return osc.GetRecord(pos_GLOBAL_OBJscope_obj).GetRecord(pos_SEM_OBJ_uRef_tp);
        return cls_l[2];
      else
        //return TYPE_GLOBAL_Start();
        return StackEval::global_start;
    }
    default: {
      const TYPE_GLOBAL_OBJscope & osc (this->cs_obj_l_p->Index(1));
      const SEQ<TYPE_AS_Name> & cls_l (osc.GetSequence(pos_GLOBAL_OBJscope_class));
      if (cls_l.Length() > 1)
//        return osc.GetRecord(pos_GLOBAL_OBJscope_obj).GetRecord(pos_SEM_OBJ_uRef_tp);
        return cls_l[2];
      else
//        return this->cs_obj_l_p->Index(2).GetRecord(pos_GLOBAL_OBJscope_obj).GetRecord(pos_SEM_OBJ_uRef_tp);
        return this->cs_obj_l_p->Index(2).GetSequence(pos_GLOBAL_OBJscope_class).Hd();
    }
  }
}

// HasCurCl
// ==> bool
bool StackEval::HasCurCl() const
{
  return !this->cs_obj_l_p->IsEmpty();
}

// HasCurObjRef
// ==> bool
bool StackEval::HasCurObjRef() const
{
  return !this->cs_obj_l_p->IsEmpty();
}

// GetCurObe
// ==> SEM`OBJ
TYPE_SEM_OBJ StackEval::GetCurObj() const
{
  return theState().GetSemObjInTab(GetCurObjRef());
}

// IsEmptyObjL
// ==> bool
bool StackEval::IsEmptyObjL() const
{
  return this->cs_obj_l_p->IsEmpty();
}

// PushCurObj
// objref : SEM`OBJ_Ref
// clnm : [AS`Name]
// origcl : [GLOBAL`OrigCl]
void StackEval::PushCurObj(const TYPE_SEM_OBJ_uRef & objref, const Generic & clnm, const Generic & origcl)
{
//wcout << L"PushCurObj: " << objref << endl;
//wcout << *(this->cs_obj_l_p) << endl;
  const TYPE_AS_Name & nmobjref (objref.GetRecord(pos_SEM_OBJ_uRef_tp));
  TYPE_AS_Name nm (clnm.IsNil() ? nmobjref : TYPE_AS_Name(clnm));
  TYPE_GLOBAL_OrigCl orignm (origcl.IsNil() ? TYPE_GLOBAL_OrigCl(nmobjref) : TYPE_GLOBAL_OrigCl(origcl));

  TYPE_GLOBAL_OBJscope objscope; // GLOBAL`OBJscope
  objscope.Init(objref, SEQ<TYPE_AS_Name>().ImpAppend(nm), SEQ<TYPE_GLOBAL_OrigCl>().ImpAppend(orignm));

  this->cs_obj_l_p->ImpPrepend(objscope);

  SaveCurrntEvaluatorStateToCPUSigma();
}

// PopCurObj
// ==> ()
void StackEval::PopCurObj()
{
//wcout << L"PopCurObj:" << this->cs_obj_l_p->Hd().GetRecord(pos_GLOBAL_OBJscope_obj) << endl;
//wcout << *(this->cs_obj_l_p) << endl;
// 20090128 -->
  //TYPE_SEM_OBJ_uRef ref (GetCurObjRef());
//  TYPE_SEM_OBJ_uRef ref (this->cs_obj_l_p->Hd().GetRecord(pos_GLOBAL_OBJscope_obj));
  Int oid (this->cs_obj_l_p->Hd().GetRecord(pos_GLOBAL_OBJscope_obj).GetInt(pos_SEM_OBJ_uRef_ref));
// <-- 20090128
  
  this->cs_obj_l_p->ImpTl();

  SaveCurrntEvaluatorStateToCPUSigma();
}

// PushClNmCurObj
// clnm : AS`Name
// origcl : AS`Name
void StackEval::PushClNmCurObj(const TYPE_AS_Name & clnm, const TYPE_GLOBAL_OrigCl & origcl)
{
//wcout << L"PushClNmCurObj: " << clnm << " " << origcl << endl;
// 20150226 -->
  // for no class (command line) environment
  if (clnm == this->void_name) return;
// <-- 20150226
//wcout << *(this->cs_obj_l_p) << endl;
  if( this->cs_obj_l_p->IsEmpty() )
  {
    Generic ref = theState().GetObjRefOfClass(clnm);
    if (ref.IsNil())
    {
      // create object for clnm
// 20090401 -->
//      TYPE_SEM_InsStrct insstrct (theState().GetInstInitVal(clnm));
//      TYPE_SEM_OBJ tmp_obj (TYPE_SEM_OBJ().Init(clnm, insstrct, MAP<TYPE_AS_Name,TYPE_SEM_History>()));
//      ref = MakeNewObj(tmp_obj, Nil());
      ref = MakeNewObj(theState().GetCachedOBJ(clnm), Nil());
// <-- 20090401
    }

    TYPE_GLOBAL_OBJscope objscope; // GLOBAL`OBJscope
    objscope.Init(ref,
                  SEQ<TYPE_AS_Name>().ImpAppend(clnm),
                  SEQ<TYPE_GLOBAL_OrigCl>().ImpAppend(origcl));

    this->cs_obj_l_p->ImpPrepend(objscope);
  }
  else
  {
    TYPE_GLOBAL_OBJscope objscope (this->cs_obj_l_p->Hd());
    SEQ<TYPE_AS_Name> cl_l (objscope.GetSequence(pos_GLOBAL_OBJscope_class));
    SEQ<TYPE_GLOBAL_OrigCl> orig_cll (objscope.GetSequence(pos_GLOBAL_OBJscope_origcl));

    cl_l.ImpPrepend(clnm);
    orig_cll.ImpPrepend(origcl);

    objscope.SetField(pos_GLOBAL_OBJscope_class, cl_l);
    objscope.SetField(pos_GLOBAL_OBJscope_origcl, orig_cll);

    this->cs_obj_l_p->ImpModify(1, objscope);
  }
}

// PopClNmCurObj
// ==>
void StackEval::PopClNmCurObj()
{
  if( this->cs_obj_l_p->IsEmpty() ) 
  {
//wcout << L"PopClNmCurObj: no stack" << endl;
    return;
  }

  TYPE_GLOBAL_OBJscope objscope (this->cs_obj_l_p->Hd());
  SEQ<TYPE_AS_Name> cl_l (objscope.GetSequence(pos_GLOBAL_OBJscope_class));
  SEQ<TYPE_GLOBAL_OrigCl> orig_cll (objscope.GetSequence(pos_GLOBAL_OBJscope_origcl));

//wcout << L"PopClNmCurObj: " << cl_l.Hd() << endl;
//wcout << *(this->cs_obj_l_p) << endl;

  cl_l.ImpTl();

// 20080120
  if (cl_l.IsEmpty())
  {
    this->cs_obj_l_p->ImpTl();
  }
  else
  {
    orig_cll.ImpTl();

    objscope.SetField(pos_GLOBAL_OBJscope_class, cl_l);
    objscope.SetField(pos_GLOBAL_OBJscope_origcl, orig_cll);
    this->cs_obj_l_p->ImpModify(1, objscope);
  }
}

#ifdef VICE
// TopClNmCurObj
// ==> [SEM`OBJ_Ref]
Generic StackEval::TopClNmCurObj()
{
  if( this->cs_obj_l_p->IsEmpty() )
    return Nil();
  else
    return this->cs_obj_l_p->Hd().GetField(pos_GLOBAL_OBJscope_obj);
}
#endif // VICE
#endif // VDMPP

void StackEval::InitRandom(unsigned long seed)
{
  this->per_rnd.set_seed (seed);
}

int StackEval::GetRandom(int limit)
{
  return this->per_rnd.get_random(limit);
}

void StackEval::InitMathRandom()
{
  this->stdlib.InitRandom();
}
// }}}

// }}}
// {{{ Container Functions

int vdm_Compare(const EvaluatorStatus & v1, const EvaluatorStatus & v2)
{
  return 0;
}

void vdm_ostream_out(const EvaluatorStatus & value, wostream & o)
{
  o << L"mk_EvaluatorStatusCt("
    << L"ptr=" << (void*)&value
    << L", ..." // I dont believe it is worth to develop prinout methods for
    // the other components, as this output is not uesed anyway, is it?
    << L")";
}

template <>
#if defined(__CYGWIN__)
inline
#endif // __CYGWIN__
void VDMContainerVal<EvaluatorStatus>::ostream_out(wostream & o, const VDMFormatter & v) const
{
  this->value.ostream_out(o, v);
}

void EvaluatorStatus::ostream_out(wostream & o, const VDMFormatter & v) const
{
  Record es (TAG_TYPE_STKM_EvaluatorStatus, length_STKM_EvaluatorStatus);

  es.SetField(pos_TYPE_STKM_EvaluatorStatus_eval_ustack,   this->eval_stack);
  es.SetField(pos_TYPE_STKM_EvaluatorStatus_call_ustack,   this->call_stack);
  es.SetField(pos_TYPE_STKM_EvaluatorStatus_curCid,        this->curCid);
  es.SetField(pos_TYPE_STKM_EvaluatorStatus_trap_ustack,   this->trap_stack);
  es.SetField(pos_TYPE_STKM_EvaluatorStatus_PC,            Int(this->pc));
  es.SetField(pos_TYPE_STKM_EvaluatorStatus_debug_flag,    this->debug_flag);
  es.SetField(pos_TYPE_STKM_EvaluatorStatus_upDnIndex,     this->upDnIndex);

#ifdef VDMPP
  es.SetField(pos_TYPE_STKM_EvaluatorStatus_instrno,       Int(this->instrno));
  es.SetField(pos_TYPE_STKM_EvaluatorStatus_guard,         this->guard);
#ifdef VICE
  es.SetField(pos_TYPE_STKM_EvaluatorStatus_status,        this->status);
  es.SetField(pos_TYPE_STKM_EvaluatorStatus_release_time,  this->release_time);
#endif // VICE
#endif // VDMPP

  es.SetField(pos_TYPE_STKM_EvaluatorStatus_cf,            this->cf);
  es.SetField(pos_TYPE_STKM_EvaluatorStatus_env_ul,        this->env_l);
  es.SetField(pos_TYPE_STKM_EvaluatorStatus_env_ul_ubak,   this->env_l_bak);
  es.SetField(pos_TYPE_STKM_EvaluatorStatus_typeinst,      this->typeinst);
  es.SetField(pos_TYPE_STKM_EvaluatorStatus_typeinst_ubak, this->typeinst_bak);

#ifdef VDMSL
  es.SetField(pos_TYPE_STKM_EvaluatorStatus_os,            this->os);
  es.SetField(pos_TYPE_STKM_EvaluatorStatus_os_ubak,       this->os_bak);
  es.SetField(pos_TYPE_STKM_EvaluatorStatus_cur_umod,      this->cur_mod);
  es.SetField(pos_TYPE_STKM_EvaluatorStatus_cur_umod_ubak, this->cur_mod_bak);
#endif // VDMSL
#ifdef VDMPP
  es.SetField(pos_TYPE_STKM_EvaluatorStatus_os,            this->os);
  es.SetField(pos_TYPE_STKM_EvaluatorStatus_os_ubak,       this->os_bak);
  es.SetField(pos_TYPE_STKM_EvaluatorStatus_obj_ul,        this->obj_l);
  es.SetField(pos_TYPE_STKM_EvaluatorStatus_obj_ul_ubak,   this->obj_l_bak);
#endif // VDMPP
  es.SetField(pos_TYPE_STKM_EvaluatorStatus_context,       this->context);

  INT2Q::h2gAS(es).ostream_out(o, v);
}

// }}}
// {{{ STKM_Init

// HUSK 16 Jul. 1999 15:59 -- Jesper K. Pedersen
// Move init_STKM from tools.cc to this location

// }}}
