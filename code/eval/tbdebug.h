/***
*  * WHAT
*  *    Exported functions from debug.cc
*  * FILE
*  *    $Id: debug.h,v 1.8 2006/06/08 07:23:13 vdmtools Exp $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __tbdebug_h__
#define __tbdebug_h__

#include "metaiv.h"
#include "AS.h"
#include <string>
#ifdef VDMPP
#include "SCHDTP.h"
#endif // VDMPP
#include "projectval.h"
#include "stackeval.h"

class TBDEBUG
{
public:
  enum StepType {STEP, STEPIN, SINGLESTEP, CONTINUE, FINISH};
  enum BacktraceType {BT_PRINT, BT_UPDATE};

public:
  static Tuple ParseAndFullyEvalExprs (const Record & Expr, wostream & wos, const SEQ<Char> &);
  static Tuple ParseAndStartEvalExprs (const Record & Expr, wostream & wos, const SEQ<Char> &);
  static void BreakIP();

  static Tuple DebugStep (StepType t, wostream & wos);
  static Tuple EvalDebugStep(wostream & wos);
  static Tuple EvalDebugStepIn(wostream & wos);
  static Tuple EvalDebugSingleStep(wostream & wos);
  static Tuple EvalDebugContinue(wostream & wos);
  static Tuple EvalDebugFinish(wostream & wos);

  static int  EvalBreakOnName(const wstring &, wostream & wos);
  static int  EvalBreakOnPos(const wstring & filename, const wstring & lineStr,
                             const wstring & colStr, wostream & wos);
  static void EvalDeleteBreak (const wstring & numStr, wostream & wos);
  static void EvalEnableBreak(const wstring & numStr, wostream & wos);
  static void EvalDisableBreak(const wstring & numStr, wostream & wos);
  static void PrintBreakpoints(wostream & wos);
  static void PrintBreakSyntax(wostream & wos);
  static bool ExtractBreakNames (const wstring & str,
                                 TYPE_AS_Name & Break_Name,
                                 TYPE_AS_Name & Mod_Name,
                                 TYPE_AS_Name & Funop_Name,
                                 bool del, wostream & wos);
  static void EvalCondition(const wstring & args, wostream & wos);
  static Tuple GetFnOpPosInfo(const wstring & fnopnm, wostream & wos);
#ifdef VDMPP
  static bool ExtractClassName(const TYPE_AS_Name & break_nm, TYPE_AS_Name & mod_name);
#endif // VDMPP
#ifdef VDMSL
  static void EvalCurMod (wostream & wos);
#endif // VDMSL
#ifdef VDMPP
  static void EvalCurCls (wostream & wos);
#endif // VDMPP
  static type_bU2P GetCurrentModule ();
  static Bool EvalPopModule (wostream & wos);
  static Bool EvalPushModule (const TYPE_ProjectTypes_ModuleName & mod, wostream & wos);
#ifdef VDMPP
  static bool InitDObjs(wostream & wos);
  static void EvalPriorityfile(const wstring & args, wostream & wos);
#endif // VDMPP
  static Bool InitCurrentDefinition (bool force_init, wostream & wos);

  static wstring ExprOrStmt2Str (const Record & Expr);

  static Set FindCurrentFunctions ();
  static Set FindOperations ();
  static bool GetSupersOfClass(const wstring & args, Set& cl_s, wstring kind, wostream & wos);
  static void EvalFunctions(const wstring & args, wostream & wos);
  static void FindTypes(const TYPE_AS_Name & clnm, wostream & wos);
  static void EvalUnitList (wostream & wos);

  static void PrintEvaluatorStatus(wostream & os, bool print_ok);
  static void ExecuteUpDownCommand(const wstring& cmd, const wstring & args, wostream & wos);

  static void User_Init(const TYPE_AS_Document & ast, bool ast_is_new, wostream & wos);
  static Tuple DebugStep (StepType t);
  static void Execute_popd(wostream & wos);
  static Quote EvaluationStateToEvalState(const TYPE_STKM_EvaluationState & evalstate);
//  static Tuple DebugStep (TBDEBUG::StepType t, wostream & wos);
//  static void SetDefaultFilePosition ();
  static void PrintBreakPosition(wostream & wos);
  static Tuple AfterInterpreterEval(const TYPE_STKM_EvaluationState & evalstate,
                                    const Generic & val,
                                    wostream & wos);
  static void UpdateGui(bool printPos, wostream & wos);
  static void EvalBackTrace (BacktraceType bt_type, wostream & wos);
  static wstring GetTraceArgs(int index);

  static void ProcessRuntimeError(wostream & wos);

#ifdef VDMPP
  static void EvalCreate(const string & args, wostream & wos);
  static void EvalDestroy (const string & args, wostream & wos);
  static void EvalDestroyAllObjects ();
  static Map  FindClassFunctions (const TYPE_AS_Name & clnm);
  static void EvalInstVars(const wstring & args, wostream & wos);
  static void EvalOperations(const wstring & args, wostream & wos);
  static void EvalTypes(const wstring & args, wostream & wos);
  static void EvalValues(const wstring & args, wostream & wos);
  static void EvalSaveObjects(const wstring & args, wostream & wos);
  static void EvalLoadObjects(const wstring & args, wostream & wos);
  static void EvalPrintObjects(const wstring & args, wostream & wos);
#endif // VDMPP

#ifdef VDMSL
  static void EvalOperations(wostream & wos);
  static void EvalTypes (wostream & wos);
  static void EvalValues (wostream & wos);
  static void EvalStates (wostream & wos);
  static void EvalModStack (wostream & wos);
#endif // VDMSL

  static void EvalTraces(const wstring & args, wostream & wos);

  static void DisplaySigma (const wstring & args, wostream & wos);
  static void DisplayCode (const wstring & args, wostream & wos);

#ifdef VDMPP
  static void DisplayCurThread(wostream & wos);
  static void DisplayThreads(wostream & wos);
  static void ExeSelThread(const wstring & args, wostream & wos);
  static void DisplayObjectReferenceInfo (const wstring & args, wostream & wos);
  static void EvalObjects (const wstring & args, wostream & wos);
  static void DisplaySystem(wostream & wos);
  static void DisplaySchedule(wostream & wos);
  static void DisplayThreadInfo(const wstring & args, wostream & wos);
  static void DisplayState(const wstring & args, wostream & wos);
  static Tuple GetOperations(const TYPE_AS_Name & clsnm);
#endif //VDMPP
  static void DisplayCurrentState(wostream & wos);

private:
  static wstring PrefixExpr2Str (const Record & PrefixE);
  static wstring BinaryExpr2Str (const Record & BinaryE);
  static wstring Status2String (status_type status);
#ifdef VDMPP
  static void PrintSCHDStatus(const TYPE_SCHDTP_ThreadStatus &, wostream & wos);
#endif // VDMPP
  static void UpdateBreakpoints();
};

#endif // __tbdebug_h__

// Force emacs to use C++ mode for this file.
//// Local Variables:
//// mode: c++
//// End:

