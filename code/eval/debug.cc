// {{{ Header

/***
*  * WHAT
*  *    Implements the stackmachine debuger.
*  * Id
*  *    $Id: STKM-debug.cc,v 1.86 2006/09/22 11:14:03 vdmtools Exp $
*  * SPECIFICATION VERSION
*  *    STKM-debug.cc: Implementation of debug.vdm 1.44
***/

// }}}

// {{{ Include

#ifdef VDMPP
#include "SCHDTP.h"
#endif //VDMPP

#include "stackeval.h"
#include "compile.h"
#include "INSTRTP.h"
#include "DEBUGTP.h"
#include "rt_errmsg.h"
#include "libman.h"
#include "position.h"
#include "astaux.h"
#include "tb_exceptions.h"
#include "Interface.h"
#include "evalaux.h"
#include "as2ascii.h"
#include "intconvquotes.h"
#include "tbdebug.h"
#include "tbutils.h"
#include "settings.h"

// }}}
// {{{ EvalDebug, EvalPrint

// EvalPrintDebugAux
// e : (AS`Expr|AS`Stmt)
// debugString : seq of char
// ==> STKM`EvaluationState * [SEM`VAL]
Tuple StackEval::EvalPrintDebugAux(const Record & e, const SEQ<Char> & debugString)
{
  TYPE_STKM_SubProgram instr;
  if (AUX::IsStmt(e))
    instr.ImpConc(theCompiler().S2I(e));
  else
    instr.ImpConc(theCompiler().E2I(e));

  SetContinue();
  SetCid(NilContextId);

  if (Settings.RTErrException())
    PushDS(GetEvaluatorState(), debugString,
           TYPE_STKM_DebugCmd().Init(theCompiler().AddTrap(instr, TYPE_STKM_SubProgram())));
  else
    PushDS(GetEvaluatorState(), debugString, TYPE_STKM_DebugCmd().Init(instr));

  return EvalRun(true);
}

// EvalDebug
// e : (AS`Expr|AS`Stmt)
// debugString : seq of char
// ==> STKM`EvaluationState * [SEM`VAL]
Tuple StackEval::EvalDebug(const Record & e, const SEQ<Char> & debugString)
{
  ResetInActivity();
  return EvalPrintDebugAux(e, debugString);
}

// EvalAuxProgram
// instr : STKM`SubProgram
// debugStr : seq of char
// interruptible : bool
Tuple StackEval::EvalAuxProgram(const TYPE_STKM_SubProgram & instr,
                                const SEQ<Char> & debugStr,
                                bool interruptible)
{
  DeActivateAllBreakpoints();

  if (Settings.RTErrException())
    PushCS(TYPE_STKM_DebugCmd().Init(theCompiler().AddTrap(instr, TYPE_STKM_SubProgram())),
           debugStr, Nil(), CallStackItemType::CS_INTERNAL);
  else
    PushCS(TYPE_STKM_DebugCmd().Init(instr), debugStr, Nil(), CallStackItemType::CS_INTERNAL);

#ifdef VDMPP
  theSystem().ResetSlice();
#endif //VDMPP

  Tuple res (interruptible ? EvalMainLoop() : EvalUninterruptedLoop());

  ActivateAllBreakpoints();
  
  if (res.GetRecord(1).Is(TAG_TYPE_STKM_Success)) {
    PopCS();
#ifdef VDMPP
    theScheduler().SaveCurThreadState();
#endif //VDMPP
  }
#ifdef VDMPP
  theState().GC(true, false);
#endif //VDMPP
  return res;
}

// EvalAuxCmd
// e : AS`Expr
// instr_pre : STKM`SubProgram
// instr_post : STKM`SubProgram
// debugStr : seq of char
// ==> STKM`EvaluationState * [SEM`VAL]
Tuple StackEval::EvalAuxCmd(const TYPE_AS_Expr & e,
                            const TYPE_STKM_SubProgram & instr_pre,
                            const TYPE_STKM_SubProgram & instr_post,
                            const SEQ<Char> & debugStr)
{
  TYPE_STKM_SubProgram instr;
  instr.ImpConc(instr_pre);
  instr.ImpConc(theCompiler().E2I(e));
  instr.ImpConc(instr_post);
  return EvalAuxProgram(instr, debugStr, true);
}

// EvalUninterruptedCmd
// e : AS`Expr
// instr_pre : STKM`SubProgram
// instr_post : STKM`SubProgram
// debugStr : seq of char
// ==> STKM`EvaluationState * SEM
Tuple StackEval::EvalUninterruptedCmd(const TYPE_AS_Expr & e,
                                      const TYPE_STKM_SubProgram & instr_pre, 
                                      const TYPE_STKM_SubProgram & instr_post,
                                      const SEQ<Char> & debugStr)
{
  TYPE_STKM_SubProgram instr;
  instr.ImpConc(instr_pre);
#ifdef VICE
  bool old_compiling = theCompiler().GetCompilingTime();
  theCompiler().SetCompilingTime(false); 
#endif //VICE
  instr.ImpConc(theCompiler().E2I(e));
#ifdef VICE
  theCompiler().SetCompilingTime(old_compiling);
#endif //VICE
  instr.ImpConc(instr_post);
  return EvalAuxProgram(instr, debugStr, false);
}

#ifdef VDMSL
// EvaluateExpression
// clmod : AS`Name
// e : AS`Expr
// env " SEM`BlkEnv
// ==> SEM`VAL
TYPE_SEM_VAL StackEval::EvaluateExpression(const TYPE_AS_Name & clmod,
                                           const TYPE_AS_Expr & e, const TYPE_SEM_BlkEnv & env)
#endif // VDMSL
#ifdef VDMPP
// EvaluateExpression
// clmod : AS`Name
// obj : [SEM`OBJ_Ref]
// e : AS`Expr
// env " SEM`BlkEnv
// ==> SEM`VAL
TYPE_SEM_VAL StackEval::EvaluateExpression(const TYPE_AS_Name & clmod, const Generic & obj,
                                           const TYPE_AS_Expr & e, const TYPE_SEM_BlkEnv & env)
#endif // VDMPP
{
#ifdef VDMSL
  PushModule (clmod);
#endif // VDMSL
#ifdef VDMPP
  if (obj.IsNil())
    PushClNmCurObj(clmod, clmod);
  else
    PushCurObj(obj, clmod, clmod);
#endif // VDMPP

  PushEmptyEnv();
  PushBlkEnv(env);
  Tuple res (EvalUninterruptedCmd(e,
                                  TYPE_STKM_SubProgram(),
                                  TYPE_STKM_SubProgram(),
                                  SEQ<Char>(L"EvaluateExpression")));
  PopEnvL();

#ifdef VDMSL
  PopModule ();
#endif // VDMSL
#ifdef VDMPP
  if (obj.IsNil())
    PopClNmCurObj();
  else
    PopCurObj();
#endif // VDMPP

  theStackMachine().SetLastRes(sem_undef);

  TYPE_STKM_EvaluationState eval_state (res.GetRecord(1));
  if (eval_state.Is(TAG_TYPE_STKM_Success))
    return res.GetRecord(2);
  else
    return RTERR::ErrorVal(L"EvaluateExpression", RTERR_INTERNAL_ERROR, Nil(), Nil(), Sequence());
//  return TYPE_SEM_VAL(); // dummy;
}

// EvalRun
// ismain : [bool]
// ==> STKM`EvaluationState * [SEM`VAL]
Tuple StackEval::EvalRun(bool ismain)
{
  ResetUserBREAK(); // 20071010

#ifdef VDMSL
  Tuple res (EvalMainLoop());
  if (res.GetRecord(1).Is(TAG_TYPE_STKM_Success)) {
    if( CallStackLevel() > 0 )
      PopCS();
  }
  return res;
#endif //VDMSL

#ifdef VDMPP
  theSystem().ResetSlice();

#ifdef VICE
  if(theSystem().IsSystem())
  {
    Tuple res (theScheduler().EvalScheduler());
    if (res.GetRecord(1).Is(TAG_TYPE_STKM_Success))
    {
      if( this->CallStackLevel() > 0 )
      {
        PopCS();
        theScheduler().SaveCurThreadState(); // 20070423
      }
    }
//    theState().GC(true);
    theState().GC(true, !UsesThreads() || (theScheduler().GiveAllThreads().Dom().Card() == 1));
    return res;
  }
#endif // VICE

  Tuple res (theScheduler().EvalScheduler());
  if (res.GetRecord(1).Is(TAG_TYPE_STKM_Success))
  {
    if( this->CallStackLevel() > 0 )
    {
      PopCS();
      theScheduler().SaveCurThreadState(); // 20070423
    }

// 20111201 -->
//    if(UsesThreads() && (theScheduler().GiveAllThreads().Dom().Card() > 1))
//    {
//      vdm_iplog << L"*** Warning *** User defined thread(s) exists." << endl;
//      TBDEBUG::DisplayThreads(vdm_iplog);
//    }
// <-- 20111201
  }

//  theState().GC(true);
  theState().GC(true, !UsesThreads() || (theScheduler().GiveAllThreads().Dom().Card() == 1));
  return res;
#endif //VDMPP
}

// EvalPrint
// e : (AS`Expr|AS`Stmt)
// debugString : seq of char
// ==> STKM`EvaluationState * [SEM`VAL]
Tuple StackEval::EvalPrint(const Record & e, const SEQ<Char> & debugString)
{
  ResetInActivity();
  DeActivateAllBreakpoints();
  
  Tuple res (EvalPrintDebugAux(e, debugString));

  ActivateAllBreakpoints();
  return res;
}

// }}}
// {{{ Eval{Step,StepIn,SingleStep,Continue,Finish}

// EvalStep
// ==> bool * [STKM`EvaluationState] * [SEM`VAL] 
Tuple StackEval::EvalStep()
{
  SetStep();
  return RunIfAllowed();
}

// EvalStepIn
// ==> bool * [STKM`EvaluationState] * [SEM`VAL]
Tuple StackEval::EvalStepIn()
{
  SetStepIn();
  return RunIfAllowed();
}

// EvalSingleStep
// ==> bool * [STKM`EvaluationState] * [SEM`VAL]
Tuple StackEval::EvalSingleStep()
{
  SetSingleStep();
  return RunIfAllowed();
}

// EvalContinue
// ==> bool * [STKM`EvaluationState] * [SEM`VAL]
Tuple StackEval::EvalContinue()
{
  SetContinue();
  return RunIfAllowed();
}

// EvalFinish
// ==> bool * [STKM`EvaluationState] * [SEM`VAL]
Tuple StackEval::EvalFinish()
{
  SetFinish();
  return RunIfAllowed();
}

// RunIfAllowed
// ==> bool * [STKM`EvaluationState] * [SEM`VAL]
Tuple StackEval::RunIfAllowed()
{
  ResetUpDn();
  if (IsSteppingAllowed())
  {
    Tuple t (EvalRun(false));
    return mk_(Bool(true), t.GetField(1), t.GetField(2));
  }
  else
    return mk_(Bool(false), Nil(), Nil());
}

// IsSteppingAllowed
// ==> bool
bool StackEval::IsSteppingAllowed() 
{
  bool allow1 = CallStackLevel() > 0;
  bool allow2 = !IsProgramAtEnd(); 
  bool allow3 = MainLoopState().Is(TAG_TYPE_STKM_Interrupt) || 
                MainLoopState().Is(TAG_TYPE_STKM_Breakpoint);
  bool allow = allow1 && allow2 && allow3; 
  return allow; 
}

// }}}
// {{{ Breakpoint

// {{{ Set/Remove/Update

// id = nil is implementation as id = -1
// EvalBreakName
// name : AS`Name
// id : [nat]
// status : DEBUGTP`BreakStatus
// ==> bool * seq of char * [nat]
Tuple StackEval::EvalBreakName(const TYPE_AS_Name & name, const Int & id, const TYPE_DEBUGTP_BreakStatus & status)
{
  Tuple t;
  try {
    // We can not ensure that there is no Runtime Error in this function.
    t  = theState().GetFirstCIDOfFctOp(name);
  }
  catch (TB_Exception &e) {
    switch (e.GetExType()) {
      case ERR_IP: {
        return mk_(Bool(false), Sequence(RTERR::GetWhat()), Nil());
        break;
      }
      default: {
        // It must not happen
        vdm_iplog << L"Internal Error" << endl;
        break;
      }
    }
    return mk_(Bool(false), Sequence(L"Internal Error"), Nil());
  }

  if (t.GetBool(1)) {

    TYPE_CI_ContextId cid (t.GetInt(3));
    TYPE_AS_Name modcl (t.GetRecord(4));
    TYPE_AS_Name loc_nm (t.GetRecord(5));

    Int id_ (id);
    TYPE_DEBUGTP_BreakStatus status_ (status);
    if (id_ == -1) {
      this->breakId++;
      id_ = this->breakId;
      status_ = TYPE_DEBUGTP_Enabled();
    }
    
    TYPE_DEBUGTP_FnNameBreakInfo breakInfo;
    breakInfo.Init(modcl, loc_nm, status_, cid);
    this->breakpoints.ImpModify(Int(id_), breakInfo);

    if (status_.Is(TAG_TYPE_DEBUGTP_Enabled))
      GetCI().SetBreakpoint(cid);

    return mk_(Bool(true), Sequence(), Int(id_));
  }
  else
  {
    return mk_(Bool(false), t.GetSequence(2), Nil());
  }
}

// id = nil is implemented at id = -1

// EvalBreakPos
// fileName : seq of char
// line : nat
// col : nat
// id : [nat]
// status : DEBUGTP`BreakStatus
// ==> bool * seq of char * [nat]
Tuple StackEval::EvalBreakPos(const TYPE_AS_Id & fileName,
                              const Int & line,
                              const Int & col,
                              const Int & id,
                              const TYPE_DEBUGTP_BreakStatus & status)
{
  Tuple gcap (GetCI().GetCidAtPos(fileName, line, col));

  if (gcap.GetBool(1)) {
    // 20060605
    TYPE_CI_ContextId cid (gcap.GetInt(3));
    Tuple flc (GetCI().GetFileLineColPos(cid));
    Int line_new (flc.GetInt(2));

    if( line != line_new )
      return mk_(Bool(false),
                 SEQ<Char>(L"Could not find a break position"), Nil());

    Int id_ (id);
    if (id_ == -1) {
      this->breakId++;
      id_ = this->breakId;
    }
        
    TYPE_DEBUGTP_PosBreakInfo breakInfo;
    breakInfo.Init(fileName, line, col, status, cid);
    this->breakpoints.ImpModify(id_, breakInfo);

    if (status.Is(TAG_TYPE_DEBUGTP_Enabled))
      GetCI().SetBreakpoint(cid);

    return mk_(Bool(true), SEQ<Char>(), id_);
  }
  else
    return mk_(Bool(false), gcap.GetField(2), Nil());
}

// DeleteBreakpoint
// num : nat
// ==> bool * [seq of char]
Tuple StackEval::DeleteBreakpoint(const Int & num)
{
  if (!this->breakpoints.DomExists(num))
    return mk_(Bool(false), SEQ<Char>(L"No such break point"));
  else {
    TYPE_DEBUGTP_BreakInfo info (this->breakpoints[num]);
    switch (info.GetTag()) {
      case TAG_TYPE_DEBUGTP_FnNameBreakInfo: {
        TYPE_CI_ContextId cid (info.GetInt(pos_DEBUGTP_FnNameBreakInfo_cid));
        GetCI().RemoveBreakpoint(cid);
        this->breakpoints.RemElem(num);

        if (this->conditions.DomExists(cid))
          this->conditions.RemElem(cid);
        break;
      }
      case TAG_TYPE_DEBUGTP_PosBreakInfo: {
        TYPE_CI_ContextId cid (info.GetInt(pos_DEBUGTP_PosBreakInfo_cid));
        GetCI().RemoveBreakpoint(cid);
        this->breakpoints.RemElem(num);

        if (this->conditions.DomExists(cid))
          this->conditions.RemElem(cid);
        break;
      }
    }
    return mk_(Bool(true), SEQ<Char>());
  }
}

// UpdateBreakPoint
// ==> set of nat
SET<Int> StackEval::UpdateBreakPoint() 
{
  SET<Int> ids;
  Set dom_breakpoints (this->breakpoints.Dom());
  Generic id;
  for (bool bb = dom_breakpoints.First(id); bb; bb = dom_breakpoints.Next(id))
  {
    TYPE_DEBUGTP_BreakInfo info (this->breakpoints[id]);
    switch (info.GetTag()) {
      case TAG_TYPE_DEBUGTP_FnNameBreakInfo: {
        if (!UpdateFnNameBreakInfo(id, info))
        {
          this->breakpoints.RemElem(id);
          ids.Insert(id);

          TYPE_CI_ContextId cid (info.GetInt(pos_DEBUGTP_FnNameBreakInfo_cid));
          if (this->conditions.DomExists(cid))
            this->conditions.RemElem(cid);
        }
        break;
      }
      case TAG_TYPE_DEBUGTP_PosBreakInfo: {
        if (!UpdatePosBreakInfo(id, info))
        {
          this->breakpoints.RemElem(id);
          ids.Insert(id);

          TYPE_CI_ContextId cid (info.GetInt(pos_DEBUGTP_PosBreakInfo_cid));
          if (this->conditions.DomExists(cid))
            this->conditions.RemElem(cid);
        }
        break;
      }
    }
  }
  return ids;
}

// UpdateFnNameBreakInfo
// id : nat
// info : DEBUGTP`FnNameBreakInfo
// ==> bool
bool StackEval::UpdateFnNameBreakInfo(const Int & id, const TYPE_DEBUGTP_FnNameBreakInfo & info)
{
  const TYPE_AS_Name & modcl (info.GetRecord(pos_DEBUGTP_FnNameBreakInfo_modcl));
  const TYPE_AS_Name & name (info.GetRecord(pos_DEBUGTP_FnNameBreakInfo_name));
  const TYPE_DEBUGTP_BreakStatus & status (info.GetRecord(pos_DEBUGTP_FnNameBreakInfo_status));
  
  Tuple ebn (EvalBreakName(AUX::ConstructDoubleName(modcl,name), id, status));
  return ebn.GetBoolValue(1);
}

// UpdatePosBreakInfo
// id : nat
// info : DEBUGTP`PosBreakInfo
// ==> bool
bool StackEval::UpdatePosBreakInfo(const Int & id, const TYPE_DEBUGTP_PosBreakInfo & info)
{
  const TYPE_AS_Id & file (info.GetSequence(pos_DEBUGTP_PosBreakInfo_fileName));
  const Int & line (info.GetInt(pos_DEBUGTP_PosBreakInfo_line));
  const Int & col (info.GetInt(pos_DEBUGTP_PosBreakInfo_col));
  const TYPE_DEBUGTP_BreakStatus & status (info.GetRecord(pos_DEBUGTP_PosBreakInfo_status));
  
  Tuple ebp (EvalBreakPos(file, line, col, id, status));
  return ebp.GetBoolValue(1);
}

// }}}
// {{{ Query functions

// ActiveBreakpoint
// cid : CI`ContextId
// ==> bool
bool StackEval::ActiveBreakpoint(const TYPE_CI_ContextId & cid)
{
// 20081127 for conditional break
//  return (inActiveLevel == 0) && GetCI().IsBreakpointAtCid(cid);
  return (this->inActiveLevel == 0) &&
         GetCI().IsBreakpointAtCid(cid) &&
         CheckCondition(cid, vdm_iplog);
}

// ExistsBreakpointForName
// modClName : AS`Name
// name : AS`Name
// ==> bool
bool StackEval::ExistsBreakpointForName(const TYPE_AS_Name &modClName, 
                                        const TYPE_AS_Name &name) 
{
  Set dom_breakpoints (this->breakpoints.Dom());
  Generic id;
  for (bool bb = dom_breakpoints.First(id); bb; bb = dom_breakpoints.Next(id))
  {
    TYPE_DEBUGTP_BreakInfo info (this->breakpoints[id]);
    if (info.Is(TAG_TYPE_DEBUGTP_FnNameBreakInfo))
    {
      if ((info.GetRecord(pos_DEBUGTP_FnNameBreakInfo_modcl) == modClName) &&
          (info.GetRecord(pos_DEBUGTP_FnNameBreakInfo_name) == name))
        return true;
    }
  }
  return false;
}

// ExistsBreakpointForPos
// name : seq of char
// line : nat
// col : nat
// ==> bool
bool StackEval::ExistsBreakpointForPos(const TYPE_AS_Id & name,
                                       const Int & line,
                                       const Int & col)
{
  Set dom_breakpoints (this->breakpoints.Dom());
  Generic id;
  for (bool bb = dom_breakpoints.First(id); bb; bb = dom_breakpoints.Next(id))
  {
    TYPE_DEBUGTP_BreakInfo info (this->breakpoints[id]);
    if (info.Is(TAG_TYPE_DEBUGTP_PosBreakInfo))
    {
      if ((info.GetSequence(pos_DEBUGTP_PosBreakInfo_fileName) == name) &&
          (info.GetInt(pos_DEBUGTP_PosBreakInfo_line) == line) &&
          (info.GetInt(pos_DEBUGTP_PosBreakInfo_col) == col))
        return true;
    }
  }
  return false;
}

// returns the number of the breakpoint
// returns -1 if breakpoint does not exist

// GetBreakpointNumForName
// modClName : AS`Name
// name : AS`Name
// ==> nat
Int StackEval::GetBreakpointNumForName(const TYPE_AS_Name &modClName, 
                                       const TYPE_AS_Name &name) 
{
  Set dom_breakpoints (this->breakpoints.Dom());
  Generic id;
  for (bool bb = dom_breakpoints.First(id); bb; bb = dom_breakpoints.Next(id))
  {
    TYPE_DEBUGTP_BreakInfo info (this->breakpoints[id]);
    if (info.Is(TAG_TYPE_DEBUGTP_FnNameBreakInfo))
    {
      if ((info.GetRecord(pos_DEBUGTP_FnNameBreakInfo_modcl) == modClName) &&
          (info.GetRecord(pos_DEBUGTP_FnNameBreakInfo_name) == name))
        return (Int(id));
    }
  }
  return Int(-1);
}

// returns the number of the breakpoint
// returns -1 if breakpoint does not exist
// GetBreakpointNumForPos
// name : seq of char
// line : nat
// col : nat
// ==> nat
Int StackEval::GetBreakpointNumForPos(const TYPE_AS_Id & name,
                                      const Int & line,
                                      const Int & col)
{
  Set dom_breakpoints (this->breakpoints.Dom());
  Generic id;
  for (bool bb = dom_breakpoints.First(id); bb; bb = dom_breakpoints.Next(id))
  {
    TYPE_DEBUGTP_BreakInfo info (this->breakpoints[id]);
    if (info.Is(TAG_TYPE_DEBUGTP_PosBreakInfo))
    {
      if ((info.GetSequence(pos_DEBUGTP_PosBreakInfo_fileName) == name) &&
          (info.GetInt(pos_DEBUGTP_PosBreakInfo_line) == line) &&
          (info.GetInt(pos_DEBUGTP_PosBreakInfo_col) == col))
        return (Int(id));
    }
  }
  return Int(-1);
}

// }}}
// {{{ Interface functions used by tools.cc

//---------------------------------------------------------------
// NOTE: None of these functions are specified !!!
//---------------------------------------------------------------

// Initialize the breakpoints.
void StackEval::ResetBreakpoints ()
{
  this->breakpoints.Clear();
  this->conditions.Clear();
  this->breakId = 0;
}

// PrintBreakpoints
void StackEval::PrintBreakpoints(wostream & out)
{
  if (this->breakpoints.IsEmpty()) {
    out << L"No breakpoints are set" << endl;
    return;
  }
  
  Set dom_breakpoints (this->breakpoints.Dom());
  Generic id;
  for (bool bb = dom_breakpoints.First(id); bb; bb = dom_breakpoints.Next(id))
  {
    TYPE_DEBUGTP_BreakInfo info (this->breakpoints[id]);
    switch (info.GetTag()) {
      case TAG_TYPE_DEBUGTP_FnNameBreakInfo: {
        TYPE_AS_Name modcl (info.GetRecord(pos_DEBUGTP_FnNameBreakInfo_modcl));
        TYPE_AS_Name name (info.GetRecord(pos_DEBUGTP_FnNameBreakInfo_name));
        TYPE_DEBUGTP_BreakStatus status (info.GetRecord(pos_DEBUGTP_FnNameBreakInfo_status));
        TYPE_CI_ContextId cid (info.GetInt(pos_DEBUGTP_FnNameBreakInfo_cid));

        out << L"#" << Int(id) << L": ";
        out << (status.Is(TAG_TYPE_DEBUGTP_Enabled) ? L"+ " : L"- ");
        out << ASTAUX::ASName2String(ASTAUX::Combine2Names(modcl, name));

        if (this->conditions.DomExists(cid)) 
        {
          out << L" : ";
          PrintCondition(cid, out);
        }
        out << endl;

        break;
      }
      case TAG_TYPE_DEBUGTP_PosBreakInfo: {
        SEQ<Char> fileName (info.GetSequence(pos_DEBUGTP_PosBreakInfo_fileName));
        Int line (info.GetInt(pos_DEBUGTP_PosBreakInfo_line));
        Int col (info.GetInt(pos_DEBUGTP_PosBreakInfo_col));
        TYPE_DEBUGTP_BreakStatus status (info.GetRecord(pos_DEBUGTP_PosBreakInfo_status));
        TYPE_CI_ContextId cid (info.GetInt(pos_DEBUGTP_PosBreakInfo_cid));
      
        out << L"#" << Int(id) << L": ";
        out << (status.Is(TAG_TYPE_DEBUGTP_Enabled) ? L"+ " : L"- ");
        out << ASTAUX::Id2String(fileName);
        out << L" line " << line;
      
        if (col != 1 )
          out << L" column " << col;

        if (this->conditions.DomExists(cid)) 
        {
          out << L" : ";
          PrintCondition(cid, out);
        }
        out << endl;

        break;
      }
    }
  }
}

// }}}
// {{{ Enable / Disable

//------------------------------------------------------------------------------
// Note the two functions have been combined to one on the specificaiton level.
// The reason for this is that there is much more code which is duplicated
// on implementation level than on the specification level.
// 13 Mar. 2000 16:31 -- Jesper K. Pedersen
//------------------------------------------------------------------------------

// EnableBreakpoint
// num : nat
// ==> bool * [seq of char]
Tuple StackEval::EnableBreakpoint(const Int & num)
{
// 20081126
//  return EnableDisableBreakpoint(num, true);

  if (!this->breakpoints.DomExists(num))
    return mk_(Bool(false), Sequence(L"No such break point"));

  TYPE_DEBUGTP_BreakInfo info (this->breakpoints[num]);
  switch (info.GetTag()) {
    case TAG_TYPE_DEBUGTP_FnNameBreakInfo: {
      TYPE_DEBUGTP_BreakStatus status (info.GetRecord(pos_DEBUGTP_FnNameBreakInfo_status));
      if (status.Is(TAG_TYPE_DEBUGTP_Enabled))
        return mk_(Bool(false), Sequence(L"Breakpoint is already enabled"));

      TYPE_CI_ContextId cid (info.GetInt(pos_DEBUGTP_FnNameBreakInfo_cid));
      GetCI().SetBreakpoint(cid);
      info.SetField(pos_DEBUGTP_FnNameBreakInfo_status, TYPE_DEBUGTP_Enabled());
      break;
    }
    case TAG_TYPE_DEBUGTP_PosBreakInfo: {
      TYPE_DEBUGTP_BreakStatus status (info.GetRecord(pos_DEBUGTP_PosBreakInfo_status));
      if (status.Is(TAG_TYPE_DEBUGTP_Enabled))
        return mk_(Bool(false), Sequence(L"Breakpoint is already enabled"));

      TYPE_CI_ContextId cid (info.GetInt(pos_DEBUGTP_PosBreakInfo_cid));
      GetCI().SetBreakpoint(cid);
      info.SetField(pos_DEBUGTP_PosBreakInfo_status, TYPE_DEBUGTP_Enabled());
      break;
    }
  }
  
  this->breakpoints.ImpModify(num, info);
  return mk_(Bool(true), Sequence());
}

// DisableBreakpoint
// num : nat
// ==> bool * [seq of char]
Tuple StackEval::DisableBreakpoint(const Int & num)
{
// 20081126
//  return EnableDisableBreakpoint(num, false);

  if (!this->breakpoints.DomExists(num))
    return mk_(Bool(false), Sequence(L"No such break point"));

  TYPE_DEBUGTP_BreakInfo info (this->breakpoints[num]);
  switch (info.GetTag()) {
    case TAG_TYPE_DEBUGTP_FnNameBreakInfo: {
      TYPE_DEBUGTP_BreakStatus status (info.GetRecord(pos_DEBUGTP_FnNameBreakInfo_status));
      if (status.Is(TAG_TYPE_DEBUGTP_Disabled))
        return mk_(Bool(false), Sequence(L"Breakpoint is already disabled"));

      TYPE_CI_ContextId cid (info.GetInt(pos_DEBUGTP_FnNameBreakInfo_cid));
      GetCI().RemoveBreakpoint(cid);
      info.SetField(pos_DEBUGTP_FnNameBreakInfo_status, TYPE_DEBUGTP_Disabled());
      break;
    }
    case TAG_TYPE_DEBUGTP_PosBreakInfo: {
      TYPE_DEBUGTP_BreakStatus status (info.GetRecord(pos_DEBUGTP_PosBreakInfo_status));
      if (status.Is(TAG_TYPE_DEBUGTP_Disabled))
        return mk_(Bool(false), Sequence(L"Breakpoint is already disabled"));

      TYPE_CI_ContextId cid (info.GetInt(pos_DEBUGTP_PosBreakInfo_cid));
      GetCI().RemoveBreakpoint(cid);
      info.SetField(pos_DEBUGTP_PosBreakInfo_status, TYPE_DEBUGTP_Disabled());
      break;
    }
  }
  this->breakpoints.ImpModify(num, info);
  return mk_(Bool(true), Sequence());
}

// AddCondition
// id : nat
// e : AS`Expr
// ==> bool * seq of char * [nat]
Tuple StackEval::AddCondition(const Int & id, const TYPE_AS_Expr & e)
{
  if (!this->breakpoints.DomExists(id))
    return mk_(Bool(false), SEQ<Char>(L"No such break point"));
  else {
    TYPE_DEBUGTP_BreakInfo info (this->breakpoints[id]);
    switch (info.GetTag()) {
      case TAG_TYPE_DEBUGTP_FnNameBreakInfo: {
        TYPE_CI_ContextId cid (info.GetInt(pos_DEBUGTP_FnNameBreakInfo_cid));
        //this->conditions.ImpModify(cid, e);
        this->conditions.ImpModify(cid, mk_(e, theCompiler().E2I(e)));
        break;
      }
      case TAG_TYPE_DEBUGTP_PosBreakInfo: {
        TYPE_CI_ContextId cid (info.GetInt(pos_DEBUGTP_PosBreakInfo_cid));
        //this->conditions.ImpModify(cid, e);
        this->conditions.ImpModify(cid, mk_(e, theCompiler().E2I(e)));
        break;
      }
    }
    return mk_(Bool(true), SEQ<Char>());
  }
}

// RemoveCondition
// id : nat
// ==> bool * seq of char * [nat]
Tuple StackEval::RemoveCondition(const Int & id)
{
  if (!this->breakpoints.DomExists(id))
    return mk_(Bool(false), SEQ<Char>(L"No such break point"));
  else {
    TYPE_DEBUGTP_BreakInfo info (this->breakpoints[id]);
    switch (info.GetTag()) {
      case TAG_TYPE_DEBUGTP_FnNameBreakInfo: {
        TYPE_CI_ContextId cid (info.GetInt(pos_DEBUGTP_FnNameBreakInfo_cid));
        if (this->conditions.DomExists(cid))
          this->conditions.RemElem(cid);
        else
          return mk_(Bool(false), SEQ<Char>(L"No condition"));
        break;
      }
      case TAG_TYPE_DEBUGTP_PosBreakInfo: {
        TYPE_CI_ContextId cid (info.GetInt(pos_DEBUGTP_PosBreakInfo_cid));
        if (this->conditions.DomExists(cid))
          this->conditions.RemElem(cid);
        else
          return mk_(Bool(false), SEQ<Char>(L"No condition"));
        break;
      }
    }
    return mk_(Bool(true), SEQ<Char>());
  }
}

// CheckCondition
// cid : CI`ContextId
// ==> bool
bool StackEval::CheckCondition(const TYPE_CI_ContextId & cid, wostream & wos)
{
  if (!this->conditions.DomExists(cid))
    return true;

  int oldLevel = this->inActiveLevel;
  try {
// 20130709 -->
    //TYPE_AS_Expr e (this->conditions[cid]);
    //Tuple res (theStackMachine().EvalUninterruptedCmd(e,
    //                                                TYPE_STKM_SubProgram(),
    //                                                TYPE_STKM_SubProgram(),
    //                                                SEQ<Char>(L"Checking break condition")));
    Tuple t (this->conditions[cid]);
    SEQ<Char> id (L"Checking break condition");
    Tuple res (theStackMachine().EvalAuxProgram(t.GetSequence(2), id, false));
// <-- 20130709
    SetCid(cid);
    TYPE_STKM_EvaluationState eval_state (res.GetRecord(1));
    if (eval_state.Is(TAG_TYPE_STKM_Success))
    {
      TYPE_SEM_VAL expr_v (res.GetRecord(2));  // SEM`VAL
      if (expr_v.Is(TAG_TYPE_SEM_BOOL))
      {
        return expr_v.GetBool(pos_SEM_BOOL_v);
      }
      else
      {
        wos << L"Warning: condition must return bool value." << endl;
      }
    }
    else
    {
      wos << L"Warning: condition check failed." << endl;
    }
  }
  catch (TB_Exception & e) {
    switch (e.GetExType()) {
      case ERR_IP: {
        // Report error
        wos << L"Warning: check break condition is failed." << endl;
//        RTERR::PrintRunTimeError(wos);
        TBDEBUG::ProcessRuntimeError(wos);

        // Recover internal state
        bool more = (CallStackLevel() > 0);
        while (more) 
        {
          TYPE_STKM_CallStackItem item (HeadCS());
          if (item.get_nmOrDesc() != SEQ<Char>(L"Checking break condition"))
          {
            PopCS();
            more = (CallStackLevel() > 0);
          }
          else
            more = false; 
        }
        PopCS();
        this->inActiveLevel = oldLevel;
        break;
      }
      default: {
        // It must not happen
        vdm_err << L"Internal Error" << endl;
        break;
      }
    }
  }
  SetCid(cid);
  return true;
}

// PrintConditions
void StackEval::PrintConditions(wostream & out)
{
  if (this->breakpoints.IsEmpty()) {
    return;
  }
  
  Set dom_breakpoints (this->breakpoints.Dom());
  Generic id;
  for (bool bb = dom_breakpoints.First(id); bb; bb = dom_breakpoints.Next(id))
  {
    TYPE_DEBUGTP_BreakInfo info (this->breakpoints[id]);
    switch (info.GetTag()) {
      case TAG_TYPE_DEBUGTP_FnNameBreakInfo: {
        TYPE_CI_ContextId cid (info.GetInt(pos_DEBUGTP_FnNameBreakInfo_cid));
        out << L"#" << Int(id) << L": ";
        PrintCondition(cid, out);
        out << endl;
        break;
      }
      case TAG_TYPE_DEBUGTP_PosBreakInfo: {
        TYPE_CI_ContextId cid (info.GetInt(pos_DEBUGTP_PosBreakInfo_cid));
        out << L"#" << Int(id) << L": ";
        PrintCondition(cid, out);
        out << endl;
        break;
      }
    }
  }
}

void StackEval::PrintCondition(const TYPE_CI_ContextId & cid, wostream & out)
{
  if (this->conditions.DomExists(cid))
  {
    AS2ASCII conv;
// 20130709 -->
    //TYPE_AS_Expr e (this->conditions[cid]);
    //conv.Expr2ASCII(INT2Q::h2gAS(e), out);
    Tuple t (this->conditions[cid]);
    conv.Expr2ASCII(INT2Q::h2gAS(t.GetRecord(1)), out);
// <-- 20130709
  }
}

// }}}
// {{{ Activate / Deactivate
// ResetInActivity
void StackEval::ResetInActivity()
{
  inActiveLevel = 0;
}

// ActivateAllBreakpoints
void StackEval::ActivateAllBreakpoints()
{
  inActiveLevel--;
}

// DeActivateAllBreakpoints
void StackEval::DeActivateAllBreakpoints()
{
  inActiveLevel++;
}

// }}}

// }}}
// {{{ Up/down commands

// EvalStackUp
bool StackEval::EvalStackUp()
{
  return GoUp();
}

// EvalStackDown
bool StackEval::EvalStackDown()
{
  return GoDown();
}

// }}}
// {{{ Thread Releated Debug Commands

#ifdef VDMPP
// EvalThreads
// map SCHDTP`ThreadId to SCHDTP`ThreadInfo
Map StackEval::EvalThreads()
{
  return theScheduler().GiveAllThreads();
}

// EvalSelThread
// id : SCHDTP`ThreadId
void StackEval::EvalSelThread(const TYPE_SCHDTP_ThreadId & id)
{
//  SelThread(id);
  theScheduler().SelThread(id);
}

// EvalCurThread
// ==> SCHDTP`ThreadId
TYPE_SCHDTP_ThreadId StackEval::EvalCurThread()
{
//  return CurThreadId();
  return theScheduler().CurThreadId();
}
#endif //VDMPP

// }}}
