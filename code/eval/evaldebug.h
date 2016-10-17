#ifndef __evaldebug_h__
#define __evaldebug_h__

#include "metaiv.h"

class EvalDebugger
{
private:
  Map breakpoints;
  int inActiveLevel;
  int breakId;

public:
  EvalDebugger()
  {
    this->breakpoints.Clear();
    this->inActiveLevel = 0;
    this->breakId = 0;
  };
  
  ~EvalDebugger() {};

  Tuple EvalDebug(const Record& e,
                  const Sequence & debugString);
  Tuple EvalPrintDebugAux(const Record& e,
                          const Sequence & debugString);
  Tuple EvalRun(bool);
  Tuple EvalAuxCmd(const TYPE_AS_Expr &,
                   const TYPE_STKM_SubProgram &,
                   const TYPE_STKM_SubProgram &,
                   const Sequence &);
  Tuple EvalUninterruptedCmd(const TYPE_AS_Expr &,
                             const TYPE_STKM_SubProgram &,
                             const TYPE_STKM_SubProgram &,
                             const Sequence &);
  Tuple EvalAuxProgram(const TYPE_STKM_SubProgram & instr,
                       const Sequence & debugStr,
                       bool interruptible);
  Tuple EvalPrint(const Record & e,
                  const Sequence & debugString);
  Tuple EvalStep();
  Tuple EvalStepIn();
  Tuple EvalSingleStep();
  Tuple EvalContinue();
  Tuple EvalFinish();
  bool IsSteppingAllowed();
  Tuple RunIfAllowed();

public:
  Tuple EvalBreakName(const TYPE_AS_Name & name, const Int & id,
                      const TYPE_DEBUGTP_BreakStatus & staus);
  Tuple EvalBreakPos(const TYPE_AS_Id & fileName,
                     const Int & line,
                     const Int & col,
                     const Int & id,
                     const TYPE_DEBUGTP_BreakStatus& staus);
  Tuple DeleteBreakpoint(const Int & num);
  SET<Int> UpdateBreakPoint();
private:
  bool UpdateFnNameBreakInfo(const Int & id,
                             const TYPE_DEBUGTP_FnNameBreakInfo & info);
  bool UpdatePosBreakInfo(const Int & id,
                          const TYPE_DEBUGTP_PosBreakInfo & info);
public:
  bool ActiveBreakpoint(const TYPE_CI_ContextId & cid);
  bool ExistsBreakpointForName(const TYPE_AS_Name &modClNm,
                               const TYPE_AS_Name &name);
  bool ExistsBreakpointForPos(const TYPE_AS_Id & name,
                              const Int & line,
                              const Int & col);
  Int GetBreakpointNumForName(const TYPE_AS_Name & modClNm,
                              const TYPE_AS_Name & name);
  Int GetBreakpointNumForPos(const TYPE_AS_Id & name,
                             const Int & line,
                             const Int & col);
  Tuple EnableBreakpoint(const Int & num);
  Tuple DisableBreakpoint(const Int & num);
  Tuple EnableDisableBreakpoint(const Int & num, bool setIt);
  void ResetInActivity();
  void ActivateAllBreakpoints();
  void DeActivateAllBreakpoints();
  void PrintBreakpoints(wostream &out);

  void ResetBreakpoints ();

  bool EvalStackUp();
  bool EvalStackDown();

#ifdef VDMPP
  Map EvalThreads();
  void EvalSelThread(const TYPE_SCHDTP_ThreadId & id);
  TYPE_SCHDTP_ThreadId EvalCurThread();
#endif // VDMPP
};
#endif // __evaldebug_h__
