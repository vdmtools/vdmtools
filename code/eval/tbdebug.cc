/***
*  * WHAT
*  *    Functions needed related to debugging facilities of the
*  *    Toolbox. These functions are not in the specification of the
*  *    dynamic semantics.
*  * FILE
*  *    $Source: /mnt/cvsroot/toolbox/code/eval/debug.cc,v $
*  * VERSION
*  *    $Revision: 1.91 $
*  * DATE
*  *    $Date: 2006/10/13 12:04:39 $
*  * STATUS
*  *    Under development
*  * REFERENCES
*  *    
*  * PROJECT
*  *    Toolbox
*  * AUTHOR
*  *    Henrik Voss + $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "tbdebug.h"
#include "libman.h"
#include "astaux.h"
#include "intconvquotes.h"
#include "stringext.h"
#include "settings.h"
#include "tb_wstring.h"
#include "tb_exceptions.h"
#include "rt_errmsg.h"
#include "val2x.h"
#include "position.h"
#include "tbutils.h"
#include "tools.h"
#ifdef VDMPP
#include "MANGLE.h"
#include "SCHDTP.h"
#include "priority_aux.h" // for pf_main
#ifdef VICE
#include "TIMETRACE.h"
#endif // VICE
#endif //VDMPP
#include <stdlib.h> // for atoi
#include "evalaux.h"
#include <sstream> //For wostringstream
#include "traces.h"
#include "as2ascii.h" // for MiniPP
#include "pattern.h"

// ParseAndFullyEvalExprs
// expr : ProjectTypes`ToolCommand | ProjectTypes`FileName
// ostr :
// debugString : seq of char
// ==> bool * [SEM`VAL]
Tuple TBDEBUG::ParseAndFullyEvalExprs (const Record & expr, wostream & wos, const SEQ<Char> & debugString)
{
  if (TOOLS::isBatchMode()) {
    if (!Settings.PostCheck()) {
      theStackMachine().SetUsesOldName(false);
    }
  }
  // Initialize the Stop button functionallity
  // For VDM++ this also includes invokation statement
  Tuple res (TOOLS::ParseExprs (expr, wos)); // bool * seq os AS`Expr
  if (!res.GetBoolValue(1)) {
    return res;
  }

  const SEQ<TYPE_AS_Expr> & exprs (res.GetSequence(2));

  if (!TOOLS::IsInScript()) {
    ToolMediator::Errs()->vdm_ClearAll();
  }
  // evaluate the arguments
  Sequence res_lv;

  res.SetField (1, Bool(true));

  if (Settings.Profile()) {
    theStackMachine().ClearProfile();
  }
  size_t len_exprs = exprs.Length();
  for (size_t idx = 1; idx <= len_exprs; idx++) {
    try {

      uint64_t pre = TBUTILS::GetCurTimeMil();

      Tuple res (theStackMachine().EvalPrint(exprs[idx], debugString)); // STKM`EvaluationState * [SEM`VAL]

      if (Settings.ElapsedTime()) {
        uint64_t post = TBUTILS::GetCurTimeMil();
        wos << L"elapsed time: " << (post - pre) << L" micro seconds" << endl;
      }

      Tuple aie (AfterInterpreterEval(res.GetRecord(1), res.GetField(2), wos)); // bool * [SEM`VAL]

      if (aie.GetBoolValue(1)) {
        res_lv.ImpAppend (aie.GetField(2));
        if ((!cliOpt) || (!cliOpt->IsInternalTest())) {
          VAL2X::PrintSemValue (aie.GetField(2), wos);
        }
      }
      else {
        res.SetField(1, Bool(false));
      }
      UpdateGui(!aie.GetBool(1), wos);
    }
    catch (TB_Exception &e) {
      switch (e.GetExType()) {
        case ERR_IP: {
          ProcessRuntimeError(wos);
          break;
        }
        default: {
          // It must not happen
          vdm_err << L"Internal Error" << endl;
          break;
        }
      }
      res_lv.ImpAppend (Nil ());
      res.SetField (1, Bool (false));
    }
  }
  if (Settings.Profile()) {
    theStackMachine().PrintProfile();
  }
  if (!TOOLS::IsInScript()) {
    ToolMediator::Errs()->vdm_AllDone();
  }
  res.SetField (2, res_lv);
  wos << flush;
  return res;
}

// ParseAndStartEvalExprs
// expr : ProjectTypes`ToolCommand | ProjectTypes`FileName
// ostr :
// debugString : seq of char
// ==>  EvalState * seq of [token]
Tuple TBDEBUG::ParseAndStartEvalExprs (const Record & expr, wostream & wos, const SEQ<Char> & debugString)
{
  //For VDM++ this also includes invokation statement
  Tuple res (TOOLS::ParseExprs (expr, wos)); // bool * seq of AS`Expr
  if (!res.GetBoolValue(1)) {
    return mk_(Quote (L"ERROR"), Sequence());
  }

  const SEQ<TYPE_AS_Expr> & exprs (res.GetSequence(2));

  if (!TOOLS::IsInScript()) {
    ToolMediator::Errs()->vdm_ClearAll();
  }
  Sequence res_lv;
  Quote evst;

  if (Settings.Profile()) {
    theStackMachine().ClearProfile();
  }
  size_t len_exprs = exprs.Length();
  for (size_t idx = 1; idx <= len_exprs; idx++) {
    try {
      uint64_t pre = TBUTILS::GetCurTimeMil();

      Tuple debugres (theStackMachine().EvalDebug(exprs[idx], debugString));

      if (Settings.ElapsedTime()) {
        uint64_t post = TBUTILS::GetCurTimeMil();
        wos << L"elapsed time: " << (post - pre) << L" micro seconds" << endl;
      }

      const TYPE_STKM_EvaluationState & evalstate (debugres.GetRecord(1));
      Tuple aie (AfterInterpreterEval(evalstate, debugres.GetRecord(2), wos));
      if (aie.GetBoolValue(1)) {
        res_lv.ImpAppend (aie.GetField(2));
        if ((!cliOpt) || (!cliOpt->IsInternalTest())) {
          VAL2X::PrintSemValue (aie.GetField(2), wos);
        }
      }
      evst = EvaluationStateToEvalState(evalstate);
      UpdateGui(!aie.GetBool(1), wos);
    }
    catch (TB_Exception &e) {
      switch (e.GetExType()) {
        case ERR_IP:
          ProcessRuntimeError(wos);
          break;
        default:
          // It must not happen
          vdm_err << L"Internal Error" << endl << flush;
          break;
      }
      evst = Quote (L"ERROR");
      res_lv = Sequence();
      break;
    }
  }

  if (Settings.Profile()) {
    theStackMachine().PrintProfile();
  }
  if (!TOOLS::IsInScript()) {
    ToolMediator::Errs()->vdm_AllDone();
  }
  wos << flush;
  return mk_(evst, res_lv);
}

void TBDEBUG::BreakIP()
{
  theStackMachine().SetUserBREAK();
}

// DebugStep
// t : StepType
// ==> EvalState * [seq of token]
Tuple TBDEBUG::DebugStep (TBDEBUG::StepType t, wostream & wos)
{
  if (!TOOLS::IsInScript()) {
    ToolMediator::Errs()->vdm_ClearAll();
  }
  Tuple res;
  try {
    Tuple debugres (DebugStep(t)); // bool * [STKM`EvaluationState] * [SEM`VAL]

    if (debugres.GetBoolValue(1)) {
      const TYPE_STKM_EvaluationState & evalstate (debugres.GetRecord (2));
      Tuple aie (AfterInterpreterEval (evalstate, debugres.GetField (3), wos));
      Sequence res_lv;
      if (aie.GetBoolValue(1)) {
        res_lv.ImpAppend (aie.GetField(2));
        if ((!cliOpt) || (!cliOpt->IsInternalTest())) {
          VAL2X::PrintSemValue (aie.GetField(2), wos);
        }
      }
      res = mk_(EvaluationStateToEvalState(evalstate), res_lv);
      UpdateGui(!aie.GetBool(1), wos);
    }
    else {
      res = mk_(Quote (L"ERROR"), Nil());
    }
  }
  catch (TB_Exception &e) {
    switch (e.GetExType()) {
      case ERR_IP: {
        ProcessRuntimeError(wos);
        break;
      }
      default: {
        // It must not happen
        vdm_err << L"Internal Error" << endl;
        break;
      }
    }
    res = mk_(Quote (L"ERROR"), Nil());

  }
  if (!TOOLS::IsInScript()) {
    ToolMediator::Errs()->vdm_AllDone();
  }
  wos << flush;
  return res;
}

Tuple TBDEBUG::EvalDebugStep(wostream & wos)
{
  return DebugStep(TBDEBUG::STEP, wos);
}

Tuple TBDEBUG::EvalDebugStepIn(wostream & wos)
{
  return DebugStep(TBDEBUG::STEPIN, wos);
}

Tuple TBDEBUG::EvalDebugSingleStep(wostream & wos)
{
  return DebugStep(TBDEBUG::SINGLESTEP, wos);
}

Tuple TBDEBUG::EvalDebugContinue(wostream & wos)
{
  return DebugStep(TBDEBUG::CONTINUE, wos);
}

Tuple TBDEBUG::EvalDebugFinish(wostream & wos)
{
  return DebugStep(TBDEBUG::FINISH, wos);
}

// set a breakpoint indicated by wstring break_str
int TBDEBUG::EvalBreakOnName (const wstring & str, wostream & wos)
{
  TYPE_AS_Name break_name, mod_name, funop_name;
  if (!ExtractBreakNames (str, break_name, mod_name, funop_name, false, wos)) {
    return -1;
  }

  if (!theStackMachine().ExistsBreakpointForName( mod_name, funop_name)) {
    Tuple ebn (theStackMachine().EvalBreakName(ASTAUX::Combine2Names(mod_name, funop_name),
                                          Int(-1), TYPE_DEBUGTP_Enabled()));
    if (!ebn.GetBool(1)) {
      wos << ebn.GetSequence(2).GetString() << endl;
      return -1;
    }
    else {
      Int num (ebn.GetInt(3));
      ToolMediator::Interf()->vdm_CallBack (PTAUX::mk_BreakNameSet (break_name, num));
      wos << L"Breakpoint number " << num << L" set for " << ASTAUX::ASName2String (funop_name)
#ifdef VDMSL
          << L" in module "
#endif //VDMSL
#ifdef VDMPP
          << L" in class "
#endif //VDMPP
          << ASTAUX::ASName2String (mod_name) << endl;
      return num;
    }
  }
  else {
    wos << L"Breakpoint already exists for " << ASTAUX::ASName2String (funop_name)
#ifdef VDMSL
        << L" in module "
#endif //VDMSL
#ifdef VDMPP
        << L" in class "
#endif //VDMPP
        << ASTAUX::ASName2String (mod_name) << endl;

    return theStackMachine().GetBreakpointNumForName (mod_name, funop_name).GetValue();
  }
}

int TBDEBUG::EvalBreakOnPos(const wstring & filename, const wstring & lineStr, const wstring & colStr, wostream & wos)
{
  wchar_t *endp;
  long line = wcstol(lineStr.c_str(), &endp, 10);
  if (lineStr[0] == '\0' || *endp != '\0') {
    wos << L"Error: line number `" << lineStr << L"' is not a number" << endl;
    PrintBreakSyntax(wos);
    return -1;
  }

  long col = wcstol(colStr.c_str(), &endp, 10);
  if (colStr[0] == '\0' || *endp != '\0') {
    wos << L"Error: column number `" << colStr << L"' is not a number" << endl;
    PrintBreakSyntax(wos);
    return -1;
  }

  if (theStackMachine().ExistsBreakpointForPos(SEQ<Char>(filename), Int(line), Int(col))) {
    wos << L"Breakpoint already exists for file " << filename << L" line " << line;
    if (col != 1) {
      wos << L" column " << col;
    }
    wos << endl;
    return theStackMachine().GetBreakpointNumForPos (SEQ<Char>(filename), Int(line), Int(col)).GetValue();
  }

  Tuple ebp ( theStackMachine().EvalBreakPos(SEQ<Char>(filename),
                                             Int(line),
                                             Int(col),
                                             Int(-1),
                                             TYPE_DEBUGTP_Enabled()) );
  if (!ebp.GetBool(1)) {
    wos << ebp.GetSequence(2).GetString() << endl << L"Breakpoint not set." << endl;
    return -1;
  }
  Int num (ebp.GetInt(3));
  ToolMediator::Interf()->vdm_CallBack (PTAUX::mk_BreakPosSet (filename, Int(line), Int(col), num));

  wos << L"Breakpoint number " << num << L" set for file " << filename << L" on line " << line;
  if (col != 1) {
    wos << L" on column " << col;
  }
  wos << endl;

  return num;
  // Update the GUI!
}

void TBDEBUG::PrintBreakpoints(wostream & wos)
{
  theStackMachine().PrintBreakpoints(wos);
}

void TBDEBUG::PrintBreakSyntax(wostream & wos)
{
  wos << L"Syntax:" << endl
#ifdef VDMSL
      << L"   break <function or operation name>" << endl
#endif // VDMSL
#ifdef VDMPP
      << L"   break <operation name>" << endl
#endif
      << L"   break <file> <line> [<col>]" << endl << flush;
}

void TBDEBUG::EvalDeleteBreak (const wstring & numStr, wostream & wos)
{
  wchar_t *endp;
  long num = wcstol(numStr.c_str(), &endp, 10);
  if (numStr[0] == '\0' || *endp != '\0') {
    wos << L"Error: You must specify a number. '" << numStr << L"' is not a number" << endl;
    return;
  }

  Tuple db (theStackMachine().DeleteBreakpoint(Int(num)));

  if (!db.GetBool(1)) {
    wos << L"Error: "<< db.GetSequence(2).GetString() << endl;
    return;
  }

  ToolMediator::Interf()->vdm_CallBack (PTAUX::mk_BreakRemove(Int(num)));
}

void TBDEBUG::EvalEnableBreak(const wstring & numStr, wostream & wos)
{
  wchar_t *endp;
  long num = wcstol(numStr.c_str(), &endp, 10);
  if (numStr[0] == '\0' || *endp != '\0') {
    wos << L"Error: You must specify a number. `" << numStr << L"' is not a number" << endl;
    return;
  }

  Tuple eb (theStackMachine().EnableBreakpoint(Int(num)));

  if (!Bool(eb.GetField(1))) {
    wos << L"Error: "<< Sequence(eb.GetField(2)).GetString() << endl;
    return;
  }

  ToolMediator::Interf()->vdm_CallBack (PTAUX::mk_BreakEnable(Int(num)));
}

void TBDEBUG::EvalDisableBreak(const wstring & numStr, wostream & wos)
{
  wchar_t *endp;
  long num = wcstol(numStr.c_str(), &endp, 10);
  if (numStr[0] == '\0' || *endp != '\0') {
    wos << L"Error: You must specify a number. `" << numStr << L"' is not a number" << endl;
    return;
  }

  Tuple db (theStackMachine().DisableBreakpoint(Int(num)));

  if (!db.GetBoolValue(1)) {
    wos << L"Error: "<< Sequence(db.GetField(2)).GetString() << endl;
    return;
  }

  ToolMediator::Interf()->vdm_CallBack (PTAUX::mk_BreakDisable(Int(num)));
}

// ExtractBreakNames is used by EvalBreakFun and EvalDeleteBreak
// to compute module and fun/op name indicated by break_str
// del = true => we are going to delete a break-point
bool TBDEBUG::ExtractBreakNames (const wstring & str,
                                 TYPE_AS_Name & Break_Name,
                                 TYPE_AS_Name & Mod_Name,
                                 TYPE_AS_Name & Funop_Name,
                                 bool del, wostream & wos)
{  
  // evaluate the wstring. this must be a legal module name and an
  // id for a function or
  Tuple t (TOOLS::ParseExprsString(TBWSTR::ConvertToHexquad(str)));
  if (!TOOLS::IsInScript()) {
    if (!t.GetBoolValue(1)) {
      wos << L"Invalid name for construct: \"" << str << endl;
      if (del) {
        wos << L"\"" << str << L"\" is not a breakpoint. " << endl;
      }
      else {
        wos << L"No breakpoint set for \"" << str << endl;
      }
      return false;
    }
  }

  const Sequence & exprs (t.GetSequence(2)) ;
  TYPE_AS_Expr break_nm_val (exprs.Hd ());
  if (! break_nm_val.Is (TAG_TYPE_AS_Name)) {
    wos << L"Invalid name for construct: \"" << str << endl;
    if (del) {
      wos << L"\"" << str << L"\" is not a breakpint" << endl;
    }
    else {
      wos << L"No breakpoint set for \"" << str << endl;
    }
    return false;
  }

  TYPE_AS_Name break_nm (break_nm_val);

#ifdef VDMSL
  //   We should return these values.
  //// Break_Name : a qualified name
  //// Mod_Name   : the first part of break_name
  //// Funop_Name : the last part of break_name
  // The break_nm may be a renamed function.

  Tuple name_t (AUX::RenameExtractModule(break_nm));
  TYPE_AS_Name stripped (name_t.GetField(2));
  Record smo (theState().GetAnyModule(name_t.GetField(1)));

  if (smo.Is(TAG_TYPE_GLOBAL_SigmaIMO)) {
    wos << L"Cannot set break point in dl module" << endl;
    return false;
  }

  TYPE_GLOBAL_SigmaMO sigmamo (smo);
  TYPE_CI_ContextId cid (break_nm.GetInt(pos_AS_Name_cid)); // We use this cid for the created names

  Mod_Name = name_t.GetField(1);
  Mod_Name.set_cid(cid);

  Funop_Name = stripped;
  Funop_Name.set_cid(cid);
  Break_Name = ASTAUX::Combine2Names(Mod_Name, Funop_Name);

  // Don't check for existance of
//  if (del)
//    return true;

  if (sigmamo.get_explfns().DomExists (Funop_Name) ||
      sigmamo.get_explops().DomExists (Funop_Name) ||
      sigmamo.get_explpolys().DomExists (Funop_Name)) {
    return true;
  }
#endif //VDMSL

#ifdef VDMPP
  if (! ExtractClassName(break_nm, Mod_Name)) {
    wos << L"Please specify breakpoint with class name " << endl;
    return false;
  }
  TYPE_AS_Ids name_seq (break_nm.GetSequence(pos_AS_Name_ids));
  TYPE_AS_Id funop_id (name_seq.Index (name_seq.Length ()));

  Funop_Name = ASTAUX::MkNameFromIds (mk_sequence(funop_id), break_nm.GetInt(pos_AS_Name_cid));

  MAP<TYPE_AS_Name,TYPE_GLOBAL_SigmaClass> classes (theState().GetClasses());
  if (!classes.DomExists(Mod_Name)) {
    wos << L"Class \"" << ASTAUX::ASName2String(Mod_Name) << L"\"" << L"is not defined. " << endl;
    return false;
  }
  const TYPE_GLOBAL_SigmaClass & sigmacl (classes[Mod_Name]);

  Break_Name = AUX::ConstructDoubleName(Mod_Name, Funop_Name);

  if (sigmacl.get_explfns().DomExists(Funop_Name)) {
    return true;
  }
  Set explops (sigmacl.get_explops().Dom());
  Generic nm;
  for (bool bb = explops.First(nm); bb; bb = explops.Next(nm)) {
    if (MANGLE::IsMangled(nm)) {
      if (MANGLE::GetUnmangledName(nm) == Funop_Name) {
        return true;
      }
    }
    else {
      if (nm == Funop_Name) {
        return true;
      }
    }
  }
#endif //VDMPP

  // then we delete a break-point it didn't matter that the
  // name didn't exist in the check above.
  if (del) {
    return true;
  }

  // Now we know that the name was a known function/method.

  wos << L"No explicit definition for \"" << ASTAUX::ASName2String (Funop_Name)
#ifdef VDMSL
            << L"\" in module " << ASTAUX::ASName2String (Mod_Name) << endl
#endif //VDMSL
#ifdef VDMPP
            << L"\" in class " << ASTAUX::ASName2String (Mod_Name) << endl
#endif //VDMPP
            << L"No breakpoint set for \""
            << ASTAUX::ASName2String (Funop_Name) << endl << flush;
  return false;
}

void TBDEBUG::EvalCondition(const wstring & args, wostream & wos)
{
  if (args.empty()) {
    // show all condition
    theStackMachine().PrintConditions(wos);
  }
  else {
    // set all condition
    Tuple t (TOOLS::ParseExprsString(TBWSTR::ConvertToHexquad(args)));
    if (!t.GetBoolValue(1)) {
      return;
    }
    SEQ<TYPE_AS_Expr> exprs (t.GetSequence(2));

    if (exprs.IsEmpty()) {
      return;
    }
    TYPE_AS_Expr e (exprs.Hd());
    //if (e.Is(TAG_TYPE_AS_RealLit))
    if (e.Is(TAG_TYPE_AS_NumLit)) {
      //Int num ((int)e.GetReal(pos_AS_RealLit_val).GetValue());
      Int num (e.GetInt(pos_AS_NumLit_val));
      exprs.ImpTl();
      if (exprs.IsEmpty()) {
        Tuple res (theStackMachine().RemoveCondition(num));
        if (!res.GetBoolValue(1)) {
          wos << res.GetSequence(2).GetString() << endl;
        }
      }
      else {
        TYPE_AS_Expr c (exprs.Hd());
        Tuple res (theStackMachine().AddCondition(num, c));
        if (!res.GetBoolValue(1)) {
          wos << res.GetSequence(2).GetString() << endl;
        }
      }
    }
    else {
      wos << L"Specify breakpoint number to add/remove condition" << endl << flush;
    }
  }
}

// GetFnOpPos
// fnopnm : seq of char
// ==> bool * int * int * int
Tuple TBDEBUG::GetFnOpPosInfo(const wstring & fnopnm, wostream & wos)
{
  Tuple t;
  try {
    // We can not ensure that there is no Runtime Error in this function.
    t  = theState().GetFirstCIDOfFctOp(ASTAUX::MkName(fnopnm));
  }
  catch (TB_Exception &e) {
    return mk_(Bool(false), Int(1), Int(1), Int(1));
  }
  if (t.GetBoolValue(1)) {
    TYPE_CI_ContextId cid (t.GetInt(3));
    return ToolMediator::CidToPosInfo(cid);
  }
  return mk_(Bool(false), Int(1), Int(1), Int(1));
}

#ifdef VDMPP
void TBDEBUG::EvalCreate(const string & args, wostream & wos)
{
  if (!TOOLS::get_dobjs_init()) {
    wos << L"Warning: specification has changed since last 'init' of user defined objects" << endl << flush;
    TOOLS::set_dobjs_init(true);  // warning is now printed

    SET<TYPE_ProjectTypes_FileName> files(ToolMediator::Repos()->vdm_Files());
    if (!files.IsEmpty()) {
      InitCurrentDefinition (false, wos);
    }
  }
  if (!TOOLS::IsInScript()) {
    ToolMediator::ExprErrs()->vdm_ClearAll();
  }
  Tuple t (TOOLS::ParseAssignStmt(args));

  if (!t.GetBoolValue(1)) {
    wos << L"Errors in command-line expression" << endl << flush;
    TOOLS::ParseCommand (wstring(L"next"), wstring(L""));
  }
  else {
    try {
      const TYPE_AS_AssignStmt & stmt (t.GetRecord(2));
      theState().CreateUserDefinedObject(stmt, SEQ<Char>(TBWSTR::string2wstring(args)), wos);
    }
    catch (TB_Exception &e) {
      switch (e.GetExType()) {
        case ERR_IP: {
          ProcessRuntimeError(wos);
          break;
        }
        default: {
          // It must not happen
          vdm_err << L"Internal Error" << endl;
          break;
        }
      }
    }
  }
  if (!TOOLS::IsInScript()) {
    ToolMediator::ExprErrs()->vdm_AllDone();
  }
}

void TBDEBUG::EvalDestroy (const string & args, wostream & wos)
{
  // static objects
  if (args.find("-d") != string::npos) {
    if ( theStackMachine().EvalStackLevel() == 0 ) {
      theState().DestroyDanglingObjects();
    }
    return;
  }

  Tuple t (TOOLS::ParseExprsString(args)); // bool * seq of AS`Expr

  if (!t.GetBoolValue(1)) {
    wos << L"Invalid name for object: \"" << TBWSTR::string2wstring(args) << L"\"" << endl << flush;
    return ;
  }

  const Sequence & exprs (t.GetSequence(2));
  TYPE_AS_Name obj_nm (exprs.Index(1));

  if (!obj_nm.Is(TAG_TYPE_AS_Name)) {
    wos << L"Invalid name for object: \"" << TBWSTR::string2wstring(args) << L"\"" << endl << flush;
    return;
  }

  if( !theState().DestroyObject( obj_nm ) ) {
    wos << L"\"" << ASTAUX::ASName2String(obj_nm) << L" is not an object" << endl
        <<L"The created objects are:" << endl << flush;
    EvalObjects(L"", wos);
  }
}

void TBDEBUG::EvalDestroyAllObjects ()
{
  theState().DestroyAllObjects();
}
#endif //VDMPP


#ifdef VDMPP
bool TBDEBUG::ExtractClassName(const TYPE_AS_Name & break_nm, TYPE_AS_Name & mod_name)
{
  TYPE_AS_Ids name (break_nm.GetSequence(pos_AS_Name_ids));
  int len = name.Length();

  if (len == 1) {
    return false;
  }
  else {
    mod_name = ASTAUX::MkNameFromIds( mk_sequence(name.Hd()), break_nm.GetInt(pos_AS_Name_cid));
    return true;
  }
}
#endif //VDMPP

#ifdef VDMSL
void TBDEBUG::EvalCurMod (wostream & wos)
{
  TYPE_AS_Name curmod (theStackMachine().CurrentModule ());
  if (!curmod.get_ids().IsEmpty()) {
    wos << L"Current module is now: " << ASTAUX::ASName2String (curmod) << endl;
  }
  else {
    wos << L"No module is pushed onto the stack" << endl;
  }
}

type_bU2P TBDEBUG::GetCurrentModule ()
{
  TYPE_AS_Name curmod (theStackMachine().CurrentModule ());
  if (!curmod.get_ids().IsEmpty()) {
    return Generic(mk_(Bool(true), PTAUX::ASName2ModuleName(curmod)));
  }
  else {
    return Generic(mk_(Bool(false), Nil()));
  }
}

Bool TBDEBUG::EvalPopModule (wostream & wos)
{
  if (ToolMediator::Repos()->vdm_IsSession (struct_session)) {
    if (theStackMachine().IsInitialized()) {
      theStackMachine().PopModule ();
      EvalCurMod (wos);
      return Bool(true);
    }
    else {
      wos << L"No modules are pushed onto the stack" << endl;
    }
  }
  else {
    wos << L"No modules to pop for definitions" << endl;
  }
  return Bool(false);
}

Bool TBDEBUG::EvalPushModule (const TYPE_ProjectTypes_ModuleName & mod, wostream & wos)
{
  if (ToolMediator::Repos()->vdm_IsSession (struct_session)) {
    TYPE_AS_Name mod_name (PTAUX::ModuleName2ASName(mod));

    try {
      Record mod (theState().GetAnyModule(mod_name)); // GLOBAL`SigmaMO | GLOBAL`SigmaIMO
      switch (mod.GetTag()) {
        case TAG_TYPE_GLOBAL_SigmaMO: {
          theStackMachine().PushModule (mod_name);
          wos << L"Current module is now: " << ASTAUX::ASName2String (mod_name)<< endl;
          return Bool(true);
        }
        case TAG_TYPE_GLOBAL_SigmaIMO: {
          wos << L"Cannot push dl module"<< endl;
          return Bool(false);
        }
        default: {
          wos << L"No such module is defined: " << ASTAUX::ASName2String (mod_name)<< endl;
          return Bool(false);
        }
      }
    }
    catch (TB_Exception &e) {
      switch (e.GetExType()) {
        case ERR_IP: {
          ProcessRuntimeError(wos);
          break;
        }
        default: {
          // It must not happen
          vdm_err << L"Internal Error" << endl;
          break;
        }
      }
      return Bool(false);
    }
  }
  else {
    wos << L"No modules to push for definitions" << endl;
  }
  return Bool(false);
}
#endif //VDMSL

#ifdef VDMPP
void TBDEBUG::EvalCurCls (wostream & wos)
{
  if (theStackMachine().HasCurCl()) {
    TYPE_GLOBAL_OrigCl cl (theStackMachine().GetCurCl());
    if (!cl.Is(TAG_TYPE_GLOBAL_Start)) {
      wos << L"Current class is now: "
          << ASTAUX::ASName2String (cl) << endl;
    }
    else {
      wos << L"No class is pushed onto the stack" << endl;
    }
  }
  else {
    wos << L"No class is pushed onto the stack" << endl;
  }
}

type_bU2P TBDEBUG::GetCurrentModule ()
{
  if (theStackMachine().HasCurCl()) {
    TYPE_GLOBAL_OrigCl cl (theStackMachine().GetCurCl());
    if (cl.Is(TAG_TYPE_AS_Name)) {
      return Generic(mk_(Bool(true), PTAUX::ASName2ModuleName(cl)));
    }
  }
  return Generic(mk_(Bool(false), Nil()));
}

Bool TBDEBUG::EvalPopModule (wostream & wos)
{
  if (ToolMediator::Repos()->vdm_IsSession (struct_session)) {
    if (theStackMachine().HasCurCl()) {
      theStackMachine().PopCurObj();
      theState().GC(true, !theStackMachine().UsesThreads());
      TYPE_GLOBAL_OrigCl cl (theStackMachine().GetCurCl());
      if (!cl.Is(TAG_TYPE_GLOBAL_Start)) {
        wos << L"Current class is now: " << ASTAUX::ASName2String (cl) << endl;
      }
      else {
        wos << L"No class is pushed onto the stack" << endl;
      }
      return Bool(true);
    }
    else {
      wos << L"No classes are pushed onto the stack" << endl;
    }
  }
  else {
    wos << L"No classes to pop for definitions" << endl;
  }
  return Bool(false);
}

Bool TBDEBUG::EvalPushModule (const TYPE_ProjectTypes_ModuleName & cls, wostream & wos)
{
  if (ToolMediator::Repos()->vdm_IsSession (struct_session)) {
    TYPE_AS_Name cls_name (PTAUX::ModuleName2ASName(cls));

    if (theState().IsAClass(cls_name)) {
      try {
        TYPE_AS_NewExpr ne;
        ne.Init(cls_name, SEQ<TYPE_AS_Expr>(), NilContextId);

        Tuple res (theStackMachine().EvalPrint(ne, Sequence(L"Create Pushed Object")));

        const TYPE_STKM_EvaluationState & eval_state (res.GetRecord(1));
        if (eval_state.Is(TAG_TYPE_STKM_Success)) {
          theStackMachine().PushCurObj(res.GetRecord(2), cls_name, cls_name);
          theStackMachine().SetLastRes(sem_undef);
          wos << L"Current class is now: " << ASTAUX::ASName2String (cls_name) << endl;
          return Bool(true);
        }
        else {
          wos << L"Create object is failed: " << ASTAUX::ASName2String (cls_name)<< endl;
          return Bool(false);
        }
      }
      catch (TB_Exception &e) {
        switch (e.GetExType()) {
          case ERR_IP: {
            ProcessRuntimeError(wos);
            break;
          }
          default: {
            // It must not happen
            vdm_err << L"Internal Error" << endl;
            break;
          }
        }
        return Bool(false);
      }
    }
    else {
      wos << L"No such class is defined: " << ASTAUX::ASName2String (cls_name)<< endl;
      return Bool(false);
    }
  }
  else {
    wos << L"No class to push for definitions" << endl;
  }
  return Bool(false);
}
#endif // VDMPP

#ifdef VDMPP
// The function InitDObjs takes the user created objects and
// initialises them with the expression/statements which
// they originally was set to by the user.
// The type def of d_objs are:
// map AS`Name to (SEM`VAL * (AS`Expr | AS`NewStmt | AS`InvokeStmt) * seq of wchar_t);
// This is done whenever the specification is initialised.
bool TBDEBUG::InitDObjs(wostream & wos)
{
  return theState().ReCreateDObjs(wos);
}

#endif //VDMPP

// InitCurrentDefinition was named EvalCurrentDefinition
Bool TBDEBUG::InitCurrentDefinition (bool force_init, wostream & wos)
{
  // Reset back trace window
  ToolMediator::Interf()->vdm_CallBack (PTAUX::mk_BackTrace(Sequence ()));
  // Reset Display window:
  ToolMediator::Interf()->vdm_CallBack (PTAUX::mk_ClearDebugWindow());

  SEQ<Record> ast_l (ToolMediator::GetVDMASTs ());
  if (!ast_l.IsEmpty ()) {
    if (Settings.Profile()) {
      theStackMachine().ClearProfile();
    }
    if (!TOOLS::IsInScript()) {
      ToolMediator::Errs()->vdm_ClearAll();
    }
    bool res = true;
    try {
      if (TOOLS::isBatchMode()) {
        theStackMachine().SetUsesOldName(false);
      }
      User_Init(ast_l, (force_init ? true : TOOLS::get_ast_is_new()), wos);
#ifdef VDMSL
      if (theState().IsEmptyMods()) {
        wos << L"Cannot initialise" << endl;
        theStackMachine().ResetCurMod(); // ETN FIXME 2000-01-04. Why is this needed
        TOOLS::set_spec_init(false);
        res = false;
      }
      else {
        TOOLS::set_spec_init(true);
      }
#endif //VDMSL

#ifdef VDMPP
      TOOLS::set_spec_init(true);
      if (InitDObjs(wos)) {
        TOOLS::set_dobjs_init(true);
        wstring priorityfilename (TOOLS::GetPriorityFile());
        if(priorityfilename != L"") {
          EvalPriorityfile(priorityfilename, wos);
        }
#ifdef VICE
        wstring timefilename (TOOLS::GetTimeFile());
        if(timefilename != L"") {
          TOOLS::EvalTimefile(timefilename);
        }
        wstring logfilename (TOOLS::GetLogFile());
        if(logfilename != L"") {
          TIMETRACE::SetLogFile(logfilename);
        }
#endif //VICE
      }
      else {
        res = false;
      }
#endif // VDMPP
    }
    catch (TB_Exception &e) {
      switch (e.GetExType()) {
        case ERR_IP: {
          ProcessRuntimeError(wos);
          break;
        }
        default: {
          // It must not happen
          vdm_err << L"Internal Error" << endl;
          break;
        }
      }
      theStackMachine().ResetEnvlInError();
      TOOLS::set_spec_init(false);
      res = false;
    }
    if (!TOOLS::IsInScript()) {
      ToolMediator::Errs()->vdm_AllDone();
    }
    if (Settings.Profile()) {
      theStackMachine().PrintProfile();
    }
    return Bool(res);
  }
  else {
    wos << L"No specification present" << endl;
    TOOLS::set_spec_init(false);
    return Bool(false);
  }
}

#ifdef VDMPP
void TBDEBUG::EvalPriorityfile(const wstring & args, wostream & wos)
{
  // Extract name of time file
  wstring files[20];
  int nos = STR_split (args, files, 20, STR_RXwhite_and_comma);
  if (nos == 0) {
    MAP<TYPE_AS_Id,Int> classPriorityMap (theScheduler().GetPriorityMap());
    if (classPriorityMap.IsEmpty()) {
      wos << L"No priority file loaded" << endl << flush;
    }
    else {
      wos << classPriorityMap.ascii() << endl << flush;
    }
      //OutputTimemap(vdm_log, timemap);
    return;
  }
  else if (nos != 1) {
    wos << L"Warning: only first file in list used - " << files[0] << endl << flush;
  }

  // Check that file exists
  wstring fname;
  ifstream sp;
  int not_found;

  TBUTILS::Search_and_OpenFile(files[0], fname, sp, not_found);
  if (not_found==1) {
    wos << L"Couldn't open priority file '" << files[0] << L"'" << endl << flush;
    return;
  }
  sp.close();

  // Parse file
  SEQ<TYPE_SCHDTP_PriorityEntry> pf (pf_main(wos, fname));
  if (!theScheduler().WellFormedPriorityFile(pf)) {
    wos << L"Priority file is not well formed - file ignored" << endl << flush;
    return;
  }

  MAP<TYPE_AS_Id,Int> classPriorityMap (theScheduler().MakePriorityMap(pf));

  // store the name of the file in case of re-init
  TOOLS::SetPriorityFile(files[0]);

  // Add priority map to environment
  theScheduler().SetPriorityMap(classPriorityMap);
}
#endif //VDMPP


// PrefixExpr2Str, BinaryExpr2Str are called by ExprOrStmt2Str.
wstring TBDEBUG::PrefixExpr2Str (const Record & PrefixE)
{
  Int opr (PrefixE.GetField (1));
  return L"PrefixExpr " + INT2Q::Integer2Quote (opr).GetValue ();
}

// PrefixExpr2Str, BinaryExpr2Str are called by ExprOrStmt2Str.
wstring TBDEBUG::BinaryExpr2Str (const Record & BinaryE)
{
  Int opr (BinaryE.GetField (2));
  return L"BinaryExpr " + INT2Q::Integer2Quote (opr).GetValue ();
}

//ExprOrStmt2Str are used by the step function. It return a wstring giving
//the name of an AST expr.
wstring TBDEBUG::ExprOrStmt2Str (const Record & Expr)
{
  switch(Expr.GetTag()) {
    case TAG_TYPE_AS_PrefixExpr: { return (PrefixExpr2Str (Expr)); }
    case TAG_TYPE_AS_BinaryExpr: { return (BinaryExpr2Str (Expr)); }
    case TAG_TYPE_AS_DefExpr: { return L"DefExpr"; }
    case TAG_TYPE_AS_LetExpr: { return L"LetExpr"; }
    case TAG_TYPE_AS_LetBeSTExpr: { return L"LetBeSTExpr"; }
    case TAG_TYPE_AS_IfExpr: { return L"IfExpr"; }
    case TAG_TYPE_AS_CasesExpr: { return L"CasesExpr"; }
    case TAG_TYPE_AS_AllOrExistsExpr: { return L"AllOrExistsExpr"; }
    case TAG_TYPE_AS_ExistsUniqueExpr: { return L"ExistsUniqueExpr"; }
    case TAG_TYPE_AS_IotaExpr: { return L"IotaExpr"; }
    case TAG_TYPE_AS_SetEnumerationExpr: { return L"SetEnumerationExpr"; }
    case TAG_TYPE_AS_SetComprehensionExpr: { return L"SetComprehensionExpr"; }
    case TAG_TYPE_AS_SetRangeExpr: { return L"SetRangeExpr"; }
    case TAG_TYPE_AS_SeqEnumerationExpr: { return L"SeqEnumerationExpr"; }
    case TAG_TYPE_AS_SeqComprehensionExpr: { return L"SeqComprehensionExpr"; }
    case TAG_TYPE_AS_SubSequenceExpr: { return L"SubSequenceExpr"; }
    case TAG_TYPE_AS_SeqModifyMapOverrideExpr: { return L"SeqModifyMapOverrideExpr"; }
    case TAG_TYPE_AS_MapEnumerationExpr: { return L"MapEnumerationExpr"; }
    case TAG_TYPE_AS_MapComprehensionExpr: { return L"MapComprehensionExpr"; }
    case TAG_TYPE_AS_TupleConstructorExpr: { return L"TupleConstructorExpr"; }
    case TAG_TYPE_AS_RecordConstructorExpr: { return L"RecordConstructorExpr"; }
    case TAG_TYPE_AS_RecordModifierExpr: { return L"RecordModifierExpr"; }
    case TAG_TYPE_AS_TokenConstructorExpr: { return L"TokenConstructorExpr"; }
    case TAG_TYPE_AS_ApplyExpr: { return L"ApplyExpr"; }
    case TAG_TYPE_AS_FieldSelectExpr: { return L"FieldSelectExpr"; }
    case TAG_TYPE_AS_IsExpr: { return L"IsExpr"; }
    case TAG_TYPE_AS_NarrowExpr: { return L"NarrowExpr"; }
    case TAG_TYPE_AS_BoolLit: { return L"Literal  " + Expr.GetBool(pos_AS_BoolLit_val).ascii(); }
    case TAG_TYPE_AS_CharLit: { return L"Literal  " + Expr.GetChar(pos_AS_CharLit_val).ascii(); }
    case TAG_TYPE_AS_TextLit: { return L"Literal  " + Expr.GetSequence(pos_AS_TextLit_val).ascii(); }
    case TAG_TYPE_AS_RealLit: { return L"Literal  " + Expr.GetReal(pos_AS_RealLit_val).ascii(); }
    case TAG_TYPE_AS_QuoteLit: {
      wstring str = L"Literal  <";
      SEQ<Char> tk (Expr.GetSequence(pos_AS_QuoteLit_val)); 
#if 0
      Sequence sq (Expr.GetField (1));
      Generic g;
      for (bool bb = sq.First(g); bb; bb = sq.Next(g)) {
        str += Char(g).GetValue ();
      }
#endif
      str += tk.GetString();
      str += L">";
      return str;
    }
    case TAG_TYPE_AS_NilLit: { return L"Literal  nil"; }
    case TAG_TYPE_AS_Name: { return L"Name '" + ASTAUX::ASName2String (Expr) + L"'"; }
    case TAG_TYPE_AS_OldName: {
      TYPE_AS_Name nm;
      nm.Init(Expr.GetSequence(pos_AS_OldName_ids), Expr.GetInt(pos_AS_OldName_cid));
      return L"OldName '" + ASTAUX::ASName2String (nm) + L"'";
    }
    case TAG_TYPE_AS_LambdaExpr: { return L"LambdaExpr"; }
    case TAG_TYPE_AS_FctTypeInstExpr: { return L"FctTypeInstExpr"; }
    case TAG_TYPE_AS_BracketedExpr: { return L"BracketedExpr"; }
    case TAG_TYPE_AS_UndefinedExpr: { return L"UndefinedExpr"; }
    case TAG_TYPE_AS_DefStmt: { return L"DefStmt"; }
    case TAG_TYPE_AS_LetStmt: { return L"LetStmt"; }
    case TAG_TYPE_AS_LetBeSTStmt: { return L"LetBeSTStmt"; }
    case TAG_TYPE_AS_AssignStmt: { return L"AssignStmt"; }
    case TAG_TYPE_AS_SeqForLoopStmt: { return L"SeqForLoopStmt"; }
    case TAG_TYPE_AS_SetForLoopStmt: { return L"SetForLoopStmt"; }
    case TAG_TYPE_AS_IndexForLoopStmt: { return L"IndexForLoopStmt"; }
    case TAG_TYPE_AS_WhileLoopStmt: { return L"WhileLoopStmt"; }
    case TAG_TYPE_AS_CallStmt: { return L"CallStmt"; }
    case TAG_TYPE_AS_ReturnStmt: { return L"ReturnStmt"; }
    case TAG_TYPE_AS_IfStmt: { return L"IfStmt"; }
    case TAG_TYPE_AS_CasesStmt: { return L"CasesStmt"; }
    case TAG_TYPE_AS_ErrorStmt: { return L"ErrorStmt"; }
    case TAG_TYPE_AS_ExitStmt: { return L"ExitStmt"; }
    case TAG_TYPE_AS_AlwaysStmt: { return L"AlwaysStmt"; }
    case TAG_TYPE_AS_AssertStmt: { return L"AssertStmt"; }
    case TAG_TYPE_AS_TrapStmt: { return L"TrapStmt"; }
    case TAG_TYPE_AS_RecTrapStmt: { return L"RecTrapStmt"; }
    case TAG_TYPE_AS_BlockStmt: { return L"BlockStmt"; }
    case TAG_TYPE_AS_NonDetStmt: { return L"NonDetStmt"; }
    case TAG_TYPE_AS_IdentStmt: { return L"IdentStmt"; }
    case TAG_TYPE_AS_SpecificationStmt: { return L"SpecificationStmt"; }
#ifdef VDMPP
    case TAG_TYPE_AS_StartStmt: { return L"StartStmt"; }
    case TAG_TYPE_AS_StartListStmt: { return L"StartListStmt"; }
    case TAG_TYPE_AS_StopStmt: { return L"StopStmt"; }
    case TAG_TYPE_AS_StopListStmt: { return L"StopListStmt"; }
    case TAG_TYPE_AS_SelfExpr: { return L"SelfExpr"; }
    case TAG_TYPE_AS_NewExpr: { return L"NewExpr"; }
    case TAG_TYPE_AS_IsOfClassExpr: { return L"IsOfClasExpr"; }
    case TAG_TYPE_AS_IsOfBaseClassExpr: { return L"IsOfBaseClassExpr"; }
    case TAG_TYPE_AS_SameBaseClassExpr: { return L"SameBaseClassExpr"; }
    case TAG_TYPE_AS_SameClassExpr: { return L"SameClassExpr"; }
    case TAG_TYPE_AS_ActExpr: { return L"ActExpr"; }
    case TAG_TYPE_AS_FinExpr: { return L"FinExpr"; }
    case TAG_TYPE_AS_ActiveExpr: { return L"ActiveExpr"; }
    case TAG_TYPE_AS_WaitingExpr: { return L"WaitingExpr"; }
    case TAG_TYPE_AS_ReqExpr: { return L"ReqExpr"; }
#endif //VDMPP
    default: {
      return L"UNKOWN";
    }
  }
}

// ETN // void Step (Record expr)
// ETN // {
// ETN //   step = 0;
// ETN //   
// ETN //   SetPosition (expr);
// ETN //   
// ETN //   vdm_iplog << ExprOrStmt2Str (expr) << endl;
// ETN //   
// ETN //   BREAK ();
// ETN // }

#ifdef VDMSL
Set TBDEBUG::FindCurrentFunctions ()
{
  TYPE_GLOBAL_SigmaMO sigmamo (theState().GetModule(theStackMachine().CurrentModule()));
  Map m (sigmamo.get_explfns());
  m.ImpOverride (sigmamo.get_explpolys());

  return m.Dom();
}
#endif //VDMSL


// The VDM++ version of the EvalFunctions return the set of
// function names being in the scope of the current object and
// in the current scope of the object.
// Example: if A is a superclass of B, and an object of class
// B is on the stack, but currently is evaluating a method in class
// A, then the set of functions from EvalFunctions will be the functions
// in A, and not the functions of class B.
// 
// This function is specified in the DEBUG module.
#ifdef VDMPP
Map TBDEBUG::FindClassFunctions (const TYPE_AS_Name & clnm)
{
  SET<TYPE_AS_Name> cl_s;
  cl_s.Insert(clnm);
  cl_s.ImpUnion(theState().GetAllSupers(clnm));

  Map res;
  Generic g;
  for (bool bb = cl_s.First(g); bb; bb = cl_s.Next(g)) {
    Map fns(theState().GetAllFns(g));
    res.Insert(g, fns.Dom());
  }
  return res;
}


// The function GetSupers takes a wstring, tests if the wstring
// is a class in the specification, and if so returns a set of
// the class name and the names of its super classes.
bool TBDEBUG::GetSupersOfClass(const wstring & args, Set& cl_s, wstring kind,
                               wostream & wos)
{
  wstring params[20];
  STR_split (args, params, 20, STR_RXwhite_and_comma);
  
  TYPE_AS_Name nm;
  if (params[0].length() > 0) {
    nm = ASTAUX::MkName(params[0]);
  }
  else {
    wos << L"Please, specify a class name." << endl << flush;
    return false;
  }
  
  if (! theState().IsAClass(nm)) {
    wos << L"Class " << ASTAUX::ASName2String(nm)
        << L" is not defined in current specification " << endl << flush;
    return false;
  }

  cl_s.Insert(nm);
  cl_s.ImpUnion(theState().GetAllSupers(nm));

  wos << kind <<L" defined in class " << ASTAUX::ASName2String(nm)
      << L" and its super classes:" << endl << flush;
    
  return true;
}
#endif //VDMPP

void TBDEBUG::EvalFunctions(const wstring & args, wostream & wos)
{
#ifdef VDMSL
  wos << L"Explicit functions defined in current module:" << endl;
  Set func (FindCurrentFunctions ());
  Generic bp;
  for (bool j = func.First(bp); j; j = func.Next(bp)) {
    wos << ASTAUX::ASName2String (bp) << L" ";
  }
  wos << endl << flush;
#endif //VDMSL
#ifdef VDMPP
  wstring params[20];
  STR_split (args, params, 20, STR_RXwhite_and_comma);

  TYPE_AS_Name nm;
  if (params[0].length() > 0) {
    nm = ASTAUX::MkName(params[0]);
  }
  else {
    wos << L"Please, specify a class name." << endl << flush;
    return;
  }

  if (! theState().IsAClass(nm)) {
    wos << L"Class " << ASTAUX::ASName2String(nm) 
            << L" is not defined in current specification "
            << endl << flush;
    return;
  }
          
  wos << L"Functions defined in current scope: " << endl;
  Map func (FindClassFunctions(nm));
  Set dom_func (func.Dom());
  Generic clnm;
  for (bool bb = dom_func.First(clnm); bb; bb = dom_func.Next(clnm) ) {
    wos << L"Functions in Class " << ASTAUX::ASName2String(clnm) << endl;
    Set already;
    Set fnm_s (func[clnm]);
    Generic fnm;
    for (bool cc = fnm_s.First(fnm); cc; cc = fnm_s.Next(fnm)) {
      TYPE_AS_Name nm (MANGLE::IsMangled(fnm) ? MANGLE::UnMangle(fnm).GetField(1) : fnm);
      if (!already.InSet(nm)) {
        wos << ASTAUX::ASName2String(nm) << L" ";
        already.Insert(nm);
      }
    }
    wos << endl << flush;
  }
#endif //VDMPP
}

#ifdef VDMPP
void TBDEBUG::EvalOperations(const wstring & args, wostream & wos)
{
  Set cl_s;
  if (! GetSupersOfClass(args, cl_s, L"Operations", wos)) {
    return;
  }
  Generic g;
  for (bool bb = cl_s.First(g); bb; bb = cl_s.Next(g)) {
    wos << L"Operations in class " << ASTAUX::ASName2String(g) << endl;
    Map ops (theState().GetAllOps(g));
    Set opnm_s (ops.Dom());

    Set already;
    Generic opnm;
    for (bool cc = opnm_s.First(opnm); cc; cc = opnm_s.Next(opnm)) {
      TYPE_AS_Name nm (MANGLE::IsMangled(opnm) ? MANGLE::UnMangle(opnm).GetField(1) : opnm);
      TYPE_SEM_ExplOP eop (ops[nm]);

      if (!already.InSet(nm) && !eop.GetBoolValue(pos_SEM_ExplOP_implicit)) {
        wos << ASTAUX::ASName2String(nm) << L" ";
        already.Insert(nm);
      }
    }
    wos << endl << flush;
  }
}

void TBDEBUG::EvalInstVars(const wstring & args, wostream & wos)
{
  wstring params[20];
  STR_split (args, params, 20, STR_RXwhite_and_comma);
  
  TYPE_AS_Name nm;
  if (params[0].length() > 0) {
    nm = ASTAUX::MkName(params[0]);
  }
  else {
    wos << L"Please, specify a class name." << endl << flush;
    return;
  }
  
  if (! theState().IsAClass(nm)) {
    wos << L"Class " << ASTAUX::ASName2String(nm) 
            << L" is not defined in current specification " << endl << flush;
    return;
  }

  wos << L"Instance variables defined in class " << ASTAUX::ASName2String(nm)
          << L" and its super classes:" <<endl << flush;

  SET<TYPE_AS_Name> cl_s;
  cl_s.Insert(nm);
  cl_s.ImpUnion(theState().GetAllSupers(nm));

  Generic g;
  for (bool bb = cl_s.First(g); bb; bb = cl_s.Next(g)) {
    wos << L"Instance variables in class " << ASTAUX::ASName2String(g) << endl;
    Sequence assigndefs(theState().GetInstVars(g));
    Generic ad_g;
    for (bool cc = assigndefs.First(ad_g); cc; cc = assigndefs.Next(ad_g)) {
      TYPE_AS_InstAssignDef iad (ad_g);
      TYPE_AS_AssignDef ad (iad.get_ad());
      Record var (ad.get_var());
      wos << ASTAUX::ASName2String(var) << L" ";
    }
    wos << endl << flush;
  }
}
#endif //VDMPP

#ifdef VDMSL
Set TBDEBUG::FindOperations ()
{
  TYPE_GLOBAL_SigmaMO sigmamo (theState().GetModule(theStackMachine().CurrentModule()));
  
  Map m (sigmamo.get_explops());
  return m.Dom();
}

void TBDEBUG::EvalOperations(wostream & wos)
{
  wos << L"Explicit operations defined in current module:" << endl;
  TYPE_GLOBAL_SigmaMO sigmamo (theState().GetModule(theStackMachine().CurrentModule()));
  Map m (sigmamo.get_explops());
  //Set func (FindOperations ());
  Set func (m.Dom ());
  Generic bp;
  for (bool bb = func.First(bp); bb; bb = func.Next(bp)) {
    TYPE_SEM_ExplOP eop (m[bp]);
    if (!eop.GetBoolValue(pos_SEM_ExplOP_implicit)) {
      wos << ASTAUX::ASName2String (bp) << L"  ";
    }
  }
  wos << endl << flush;
}

void TBDEBUG::EvalTypes (wostream & wos)
{
  wos << L"Types defined in current module:" << endl;

  TYPE_GLOBAL_SigmaMO sigmamo (theState().GetModule(theStackMachine().CurrentModule()));

  Map m (sigmamo.get_tps());
  Generic nm;
  for (bool bb = m.First(nm); bb; bb = m.Next(nm)) {
    wos << ASTAUX::ASName2String(nm) << L"  ";
  }
  wos << endl << flush;
}
#endif //VDMSL

#ifdef VDMPP
void TBDEBUG::FindTypes(const TYPE_AS_Name & clnm, wostream & wos)
{
  wos << L"Types defined in class " << ASTAUX::ASName2String(clnm)
          << L" and its super classes:" << endl;

  SET<TYPE_AS_Name> cl_s;
  cl_s.Insert(clnm);
  cl_s.ImpUnion(theState().GetAllSupers(clnm));

  Generic g;
  for (bool bb = cl_s.First(g); bb; bb = cl_s.Next(g)) {
    wos << L"Types in class " << ASTAUX::ASName2String(g) << endl;
    Set tps(theState().GetLocalTps(g).Dom());
    Generic type_g;
    for (bool cc = tps.First(type_g); cc; cc = tps.Next(type_g)) {
      wos << ASTAUX::ASName2String(type_g) << L" ";
    }
    wos << endl << flush;
  }
}

void TBDEBUG::EvalTypes(const wstring & args, wostream & wos)
{
  wstring params[20];
  STR_split (args, params, 20, STR_RXwhite_and_comma);
  
  TYPE_AS_Name nm;
  if (params[0].length() > 0 ) {
    nm = ASTAUX::MkName(params[0]);
  }
  else {
    wos << L"Please, specify a class name." << endl;
    return;
  }
  
  if (! theState().IsAClass(nm)) {
    wos << L"Class " << ASTAUX::ASName2String(nm) 
            << L" is not defined in current specification " << endl << flush;
    return;
  }
  
  FindTypes(nm, wos);
}
#endif //VDMPP

#ifdef VDMSL
void TBDEBUG::EvalValues (wostream & wos)
{
  wos << L"Values defined in current module:" << endl;

  TYPE_GLOBAL_SigmaMO sigmamo (theState().GetModule(theStackMachine().CurrentModule()));
  Map m (sigmamo.get_gv());

  Generic bp;
  for (bool j = m.First(bp); j; j = m.Next(bp)) {
    wos << ASTAUX::ASName2String (bp) << L"  ";
  }
  wos << endl << flush;
}
#endif //VDMSL

#ifdef VDMPP
void TBDEBUG::EvalValues(const wstring & args, wostream & wos)
{
  wstring params[20];
  STR_split (args, params, 20, STR_RXwhite_and_comma);
  
  TYPE_AS_Name nm;
  if (params[0].length() > 0) {
    nm = ASTAUX::MkName(params[0]);
  }
  else {
    wos << L"Please, specify a class name." << endl << flush;
    return;
  }
  
//  if (! IsAClass(nm)) {
  if (! theState().IsAClass(nm)) {
    wos << L"Class " << ASTAUX::ASName2String(nm) 
            << L" is not defined in current specification" << endl << flush;
    return;
  }

  wos << L"Values defined in class " << ASTAUX::ASName2String(nm)
          << L" and its super classes:" << endl << flush;

  SET<TYPE_AS_Name> cl_s;
  cl_s.Insert(nm);
  cl_s.ImpUnion(theState().GetAllSupers(nm));

  Generic g;
  for (bool bb = cl_s.First(g); bb; bb = cl_s.Next(g)) {
    wos << L"Values in class " << ASTAUX::ASName2String(g) << endl;
    Set vls(theState().GetVlsInit(g).Dom());
    Generic type_g;
    for (bool cc = vls.First(type_g); cc; cc = vls.Next(type_g)) {
      wos << ASTAUX::ASName2String(type_g) << L" ";
    }
    wos << endl << flush;
  }
}
#endif //VDMPP

#ifdef VDMSL
void TBDEBUG::EvalStates (wostream & wos)
{
  TYPE_AS_Name curmod (theStackMachine().CurrentModule ());
  wos << L"Global state components for module " + ASTAUX::ASName2String (curmod) << L":" << endl;

  Map mod_gs (theState().GetCurrentState (curmod));
  Generic bp;
  for (bool bb = mod_gs.First(bp); bb; bb = mod_gs.Next(bp)) {
    wos << ASTAUX::ASName2String (bp) << L"  ";
  }
  wos << endl << flush;
}

void TBDEBUG::EvalModStack (wostream & wos)
{
  wos << L"The current module stack is:" << endl;
//  Tuple curstate (theStackMachine().GetCurrentState());
//  EvaluatorStatusCt es (curstate.GetField(1));
  EvaluatorStatusCt es (theStackMachine().GetEvaluatorState());
  Sequence cur_mod (es.get_const_ref().cur_mod);
  Generic g;
  for (bool bb = cur_mod.First(g); bb; bb = cur_mod.Next(g)) {
    wos << L" " << ASTAUX::ASName2String (g)<< endl;
  }
}
#endif //VDMSL

void TBDEBUG::EvalUnitList (wostream & wos)
{
  if (!ToolMediator::Repos()->vdm_IsSession (none_session)) {
#ifdef VDMSL
    wos << L"The following modules are defined:" << endl;
#endif // VDMSL
#ifdef VDMPP
    wos << L"The following classes are defined:" << endl;
#endif //VDMPP
    SET<TYPE_ProjectTypes_ModuleName> mod_s (ToolMediator::Repos()->vdm_VDMModules ());
    // The following comments concern the check for modified files
    // and are commented out until it is decided what exactly to
    // do with this information (Bug Report 242).
    SET<TYPE_ProjectTypes_FileName> modified_s (ToolMediator::CheckForModifiedFiles ());
    Generic modnm;
    for (bool i = mod_s.First (modnm); i; i = mod_s.Next (modnm)) {
      SET<TYPE_ProjectTypes_FileName> file_s (ToolMediator::Repos ()->vdm_FileOfModule (modnm));
      file_s.ImpIntersect (modified_s);
      wstring changed (file_s.IsEmpty() ? L"   " : L" * ");
      Tuple VdmStatus (ToolMediator::Repos()->vdm_Status (modnm));
      status_type status = PTAUX::ExtractStatus (VdmStatus);
      wos << Status2String (status) << changed;
      wstring nm (PTAUX::ExtractModuleName (modnm)); 
#ifdef VDMSL
      if (nm == ASTAUX::GetDefaultModName()) {
        nm = L"";
      }
#endif // VDMSL
      wos << nm << endl << flush;
    }
  }
  else {
    wos << L"No specification present" << endl;
  }
}

#ifdef VDMPP
void TBDEBUG::DisplayCurThread(wostream & wos)
{
  wos << L"Current Thread Id: " << theStackMachine().EvalCurThread().GetValue() << endl;
}

void TBDEBUG::DisplayThreads(wostream & wos)
{
  // threadmap: map ThreadId to ThreadInfo
  Map threadmap (theStackMachine().EvalThreads());
  wos << L"Existing threads: " << endl;

  // Traverse existing threads. 
  Set dom_threadmap (threadmap.Dom());
  Generic threadid;
  for (bool bb = dom_threadmap.First(threadid); bb; bb = dom_threadmap.Next(threadid) ) {
    const TYPE_SCHDTP_ThreadInfo & threadinfo (threadmap[threadid]);
    Generic objref (threadinfo.get_obj());
 
    const TYPE_SCHDTP_ThreadStatus & status (threadinfo.GetRecord(pos_SCHDTP_ThreadInfo_status));

    wos << L"Thread id: " << threadid.ascii();
    wos << L", ";

#ifdef VICE
    Generic nm (theScheduler().GetCPUOfThread(threadid));
    wos << L"CPU : ";
    wos << (nm.IsNil() ? L"nil" : ASTAUX::ASName2String(nm));
    wos << L", ";
#endif // VICE

    wos << L"Object ref : ";
    if (!objref.IsNil()) {
      wos << L"objref" << Record(objref).GetInt(pos_SEM_OBJ_uRef_ref);
      const TYPE_AS_Name & clnm (Record(objref).GetRecord(pos_SEM_OBJ_uRef_tp));
      wos << L", Class : " << ASTAUX::ASName2String(clnm);
    }
    else {
      wos <<L"none";
    }

    wos << L", Status : ";
    PrintSCHDStatus(status, wos);
    if (status.Is(TAG_TYPE_SCHDTP_Blocked)) {
      EvaluatorStatusCt threadstate (threadinfo.get_stackeval());
      EvaluatorStatus & es = threadstate.get_shared_ref();
      Stack stack (es.call_stack);
      bool first = true;
      bool cont = true;
      while (!stack.IsEmpty() && cont) {
        TYPE_STKM_CallStackItem csi (stack.Pop());
        switch(csi.get_type().GetValue()) {
          case CallStackItemType::CS_FNOP: {
            const Generic & nmOrDesc (csi.GetField(pos_STKM_CallStackItem_nmOrDesc));
            if (nmOrDesc.IsRecord() && Record(nmOrDesc).Is(TAG_TYPE_AS_Name)) {
              if (first) {
                first = false;
                wos << endl;
                wos << L"          ";
              }
              wos << L" <- ";
              wos << ASTAUX::ASName2String(nmOrDesc) ;
            }
            break;
          }
          case CallStackItemType::CS_INTERNAL: {
            const Generic & nmOrDesc (csi.GetField(pos_STKM_CallStackItem_nmOrDesc));
            if (nmOrDesc.IsSequence() && (nmOrDesc == SEQ<Char>(L"Thread Start"))) {
              if (first) {
                first = false;
                wos << endl;
                wos << L"          ";
              }
              wos << L" <- ";
              wos << L"thread";
            }
            cont = false;
            break;
          }
          case CallStackItemType::CS_DEBUGCMD: {
            cont = false;
            break;
          }
        }
      }
    }
    wos << endl;
  }
  wos << flush;
}

void TBDEBUG::PrintSCHDStatus(const TYPE_SCHDTP_ThreadStatus & status, wostream& wos)
{
  switch (status.GetTag()) {
    case TAG_TYPE_SCHDTP_Running: {
      wos << L"Running";
      break;
    }
    case TAG_TYPE_SCHDTP_Blocked: {
      TYPE_SCHDTP_Blocked blocked (status);
      wos << L"Blocked";
      wos << L"(" << ASTAUX::ASName2String(blocked.get_opnm()) << L")";
      break;
    }
    case TAG_TYPE_SCHDTP_MaxReached: {
      wos << L"MaxReached";
      break;
    }
    case TAG_TYPE_SCHDTP_Sleeping: {
      wos << L"Sleeping";
      break;
    }
#ifdef VICE
    case TAG_TYPE_SCHDTP_SyncOp: {
      TYPE_SCHDTP_SyncOp so (status);
      wos << L"SyncOp";
      wos << L"(" << so.get_id() << L")";
      break;
    }
    case TAG_TYPE_SCHDTP_SyncOpRet: {
      wos << L"SyncOpRet";
      break;
    }
#endif // VICE
    default: {
      wos << L"Status Unknown";
      break;
    }
  }
}

void TBDEBUG::ExeSelThread(const wstring & args, wostream & wos)
{
  int threadid (atoi(TBWSTR::wstring2string(args).c_str()));
  if (!threadid) {
    wos << L"The command \"selthread\" takes an integer greater than 1 as argument." << endl;
    return;
  }
  theStackMachine().EvalSelThread(TYPE_SCHDTP_ThreadId(threadid));
  if (theStackMachine().GetCurCid() != NilContextId) {
    UpdateGui(true, wos);
  }
  else {
    // Reset back trace window
    ToolMediator::Interf()->vdm_CallBack (PTAUX::mk_BackTrace(Sequence ()));
    // Reset Display window:
    ToolMediator::Interf()->vdm_CallBack (PTAUX::mk_ClearDebugWindow());
  }
}

void TBDEBUG::DisplayObjectReferenceInfo (const wstring & args, wostream & wos)
{
  wstring params[20];
  int nos = STR_split (args, params, 20, STR_RXwhite_and_comma);
  if( nos > 1 ) {
    Set refs;
    for( int i = 1; i < nos; i++ ) {
      unsigned long objid = atol( TBWSTR::wstring2string( params[i] ).c_str() );
      if( objid > 0 ) {
        theState().GetObjectReferenceByID( Int(objid), refs );
      }
      else {
        theState().GetObjectReferenceByName(ASTAUX::MkName(params[i]), refs );
      }
    }
    Generic ref;
    for (bool bb = refs.First(ref); bb; bb = refs.Next(ref)) {
      theState().DisplayObjectReference( ref, wos );
    }
  }
  else {
    theState().DisplayLiveObjects(wos);
    theState().DisplayDanglingObjects(wos);
  }
}

void TBDEBUG::EvalObjects (const wstring & args, wostream & wos)
{
  if (args.find(L"-a") != string::npos) {
    wstring params[20];
    int nos = STR_split (args, params, 20, STR_RXwhite_and_comma);
    if( nos == 1 ) {
      wos << L"Objects (all): " << endl << flush;
      theState().ShowAllObjects(wos);
      return;
    }
    if( nos > 1 ) {
      wos << L"Objects (all): " << endl << flush;
      Set refs;
      for( int i = 1; i < nos; i++ ) {
        unsigned long objid = atol(TBWSTR::wstring2string(params[i]).c_str());
        if( objid > 0 ) {
          theState().GetObjectReferenceByID(Int(objid), refs);
        }
        else {
          theState().GetObjectReferenceByName(ASTAUX::MkName(params[i]), refs);
        }
      }
      Generic ref;
      for(bool bb = refs.First( ref ); bb; bb = refs.Next( ref ) ) {
        VAL2X::val2stream( ref, wos, 0);
        wos << endl << flush;
      }
      return;
    }
    return;
  }

  wos << L"Objects: " << endl << flush;
  theState().ShowUserObjects(wos);
}

void TBDEBUG::DisplayState(const wstring & args, wostream & wos)
{
  if (args.find(L"-n") != string::npos) {
    wos << L"Number of objects is " << theState().ObjectTabSize() << endl;
    return;
  }

  if (args.find(L"-f") != string::npos) {
    wos << L"spec_init=" << ( !TOOLS::get_ast_is_new() ? L"true" : L"false" ) << endl;
    wos << L"dobjs_init=" << ( TOOLS::get_dobjs_init() ? L"true" : L"false" ) << endl;
    wos << ToolMediator::Repos()->vdm_Files() << endl;
    wos << ToolMediator::Repos()->vdm_ParsedFiles() << endl;
    return;
  }

  if (args.find(L"-d") != string::npos) {
    wstring params[20];
    int nos = STR_split (args, params, 20, STR_RXwhite_and_comma);
    Sequence arg_s;
    if( nos > 1 ) {
      for( int i = 1; i < nos; i++ ) {
        unsigned long objid = atol( TBWSTR::wstring2string( params[i] ).c_str() );
        if( objid > 0 ) {
          arg_s.ImpAppend(Int(objid));
        }
        else {
          arg_s.ImpAppend(ASTAUX::MkName(params[i]));
        }
      }
    }
    theState().DisplayObjectsDetail( arg_s, wos ) ;
    return;
  }

  if (args.find(L"-u") != string::npos) {
    theState().DisplayUserObjects( wos );
    return;
  }

  if (args.find(L"-r") != string::npos) {
    TBDEBUG::DisplayObjectReferenceInfo ( args, wos );
    return;
  }

  // static objects
  if (args.find(L"-s") != string::npos) {
    theState().DisplayStaticObjects(wos);
    return;
  }
}

Tuple TBDEBUG::GetOperations(const TYPE_AS_Name & clsnm)
{
  if(theState().IsAClass(clsnm)) {
    try
    {
      return mk_(Bool(true), theState().GetAllOps(clsnm).Dom());
    }
    catch(...) {}
  }
  return mk_(Bool(false), Nil());
}

#endif //VDMPP

wstring TBDEBUG::Status2String (status_type status)
{
  wstring result;
  switch (status.syntax) {
    case status_type::status_none: {
      result = L"-- ";
      break;
    }
    case status_type::status_error: {
      result = L"/S ";
      break;
    }
    case status_type::status_ok: {
      result = L" S ";
      break;
    }
    default: {
      result = L"-- ";
      break;
    }
  }

  switch (status.type) {
    case status_type::status_none: {
      result += L"-- ";
      break;
    }
    case status_type::status_error: {
      result += L"/T ";
      break;
    }
    case status_type::status_ok:
    case status_type::status_pos:
    case status_type::status_def: {
      result += L" T ";
      break;
    }
    default: {
      result += L"-- ";
      break;
    }
  }

  switch (status.cg) {
    case status_type::status_none: {
      result += L"-- ";
      break;
    }
    case status_type::status_error: {
      result += L"/C ";
      break;
    }
    case status_type::status_ok: {
      result += L" C ";
      break;
    }
    default: {
      result += L"-- ";
      break;
    }
  }

#ifdef VDMPP
  switch (status.jcg) {
    case status_type::status_none: {
      result += L"-- ";
      break;
    }
    case status_type::status_error: {
      result += L"/J ";
      break;
    }
    case status_type::status_ok: {
      result += L" J ";
      break;
    }
    default: {
      result += L"-- ";
      break;
    }
  }
#endif //VDMPP

  switch (status.pp) {
    case status_type::status_none: {
      result += L"-- ";
      break;
    }
    case status_type::status_error: {
      result += L"/P ";
      break;
    }
    case status_type::status_ok: {
      result += L" P ";
      break;
    }
    default: {
      result += L"-- ";
      break;
    }
  }
  return result;
}

void TBDEBUG::PrintEvaluatorStatus(wostream & os, bool print_ok)
{
/*
  os << L"created EvaluatorStatus: " << EvaluatorStatus::created_ << endl
     << L"deleted EvaluatorStatus: " << EvaluatorStatus::deleted_ << endl;
  if (print_ok) {
    if (EvaluatorStatus::created_ == EvaluatorStatus::deleted_) {
      os << L"OK: EvaluatorStatus counts are equal." << endl;
    } else {
      os << L"ERROR: EvaluatorStatus counts are not equal." << endl;
    }
  }
  os << flush;
*/
}

// ExecuteUpDownCommand
void TBDEBUG::ExecuteUpDownCommand(const wstring & cmd, const wstring & args, wostream & wos)
{
  if (theStackMachine().CallStackLevel() == 0) {
    wos << L"Construct is fully evaluated" << endl << flush;
    return;
  }
  else {
    bool ok;
    if (cmd == L"up") {
      ok = theStackMachine().EvalStackUp();
    }
    else  { // "down"
      ok = theStackMachine().EvalStackDown();
    }
    UpdateGui(ok, wos);
  }
}

// UpdateBreakpoints
void TBDEBUG::UpdateBreakpoints()
{
  SET<Int> oldIds (theStackMachine().UpdateBreakPoint());
  Generic id;
  for (bool bb = oldIds.First(id); bb; bb = oldIds.Next(id)) {
    ToolMediator::Interf()->vdm_CallBack (PTAUX::mk_BreakRemove (id));
  }
}

// User_Init was named EvalDefinition
void TBDEBUG::User_Init(const TYPE_AS_Document & ast, bool ast_is_new, wostream & wos)
{
  try {
    if (!TOOLS::isTracesMode()) {
      wos << L"Initializing specification ... " << flush;

      if (TOOLS::isBatchMode()) {
        wos << endl << flush;
      }
    }

    // Reinitialize Random Number Generator for nondeterministic statements.
    if (Settings.Random() >= 0) {
      theStackMachine().InitRandom((unsigned long)Settings.Random());
    }
    theStackMachine().InitMathRandom();
    // initialize different sequences and maps

    // Initialize the stack interpreter.
    // Init calls InitScheduler and STATE_Init_SIGMA.
    theStackMachine().User_Init(ast, ast_is_new);
    UpdateBreakpoints();

    if (!TOOLS::isTracesMode()) {
      wos << L"done" << endl << flush;
    }
  }
  catch (TB_Exception & e)  {
    wos << L"failed" << endl << flush;
    throw e;
  }
}

// DebugStep
// t : 
// ==> bool * [STKM`EvaluationState] * [SEM`VAL]
Tuple TBDEBUG::DebugStep (StepType t)
{
  switch (t) {
    case STEPIN:     { return theStackMachine().EvalStepIn (); break; }
    case SINGLESTEP: { return theStackMachine().EvalSingleStep (); break; }
    case CONTINUE:   { return theStackMachine().EvalContinue (); break; }
    case FINISH:     { return theStackMachine().EvalFinish (); break; }
    case STEP:       { return theStackMachine().EvalStep (); break; }
    default:         { return theStackMachine().EvalStep (); break; }
  }
}

/*
void TBDEBUG::SetDefaultFilePosition ()
{
  TYPE_ProjectTypes_ErrMsg err (PTAUX::mk_ErrMsg (POSITION::GetErrFile (),
                                                  POSITION::GetErrLine (),
                                                  POSITION::GetErrCol (),
                                                  Sequence()));
  ToolMediator::Errs()->vdm_SetFilePosition (err);
}
*/

// AfterInterpreterEval
// ostr : 
// evalstate : STKM`EvaluationState
// val : [SEM`VAL]
// +> bool * [SEM`VAL]
Tuple TBDEBUG::AfterInterpreterEval(const TYPE_STKM_EvaluationState & evalstate,
                                    const Generic & val,
                                    wostream & wos)
{
  switch (evalstate.GetTag()) {
    case TAG_TYPE_STKM_Breakpoint: {
      return mk_(Bool(false), Nil());
      break;
    }
    case TAG_TYPE_STKM_Interrupt: {
      wos << L"Interrupt!" << flush << endl;
      return mk_(Bool(false), Nil());
      break; 
    }
    case TAG_TYPE_STKM_Success: {
      if (theStackMachine().IsRuntimeErrorException(val)) {
        ProcessRuntimeError(wos);
        return mk_(Bool(false), Nil());
      }
      if (val.Is(TAG_TYPE_SEM_EXIT)) {
        TYPE_CI_ContextId cid (theStackMachine().GetLastExitCid());
        if (cid != NilContextId) {
          Tuple t (GetCI().GetFileLineColPos(cid));
          wos << L"Exception raised at " << t.GetSequence(1) << L" l." << t.GetInt(2) << L" c." << t.GetInt(3) << endl;
        }
      }
      return mk_(Bool(true), val);
      break;
    }
    default: {
      wos << L"Internal Error!" << flush << endl;
      return mk_(Bool(false), Nil());
      break;
    }
  }
}

void TBDEBUG::PrintBreakPosition(wostream & wos)
{
  TYPE_ProjectTypes_ErrMsg err (PTAUX::mk_ErrMsg(POSITION::GetErrFile (),
                                                 POSITION::GetErrLine (),
                                                 POSITION::GetErrCol (),
                                                 type_cLL()));

  wstring file (ToolMediator::GetFileName(POSITION::GetErrFile()));

  if (!file.empty()) {
    file = L"In " + file;
  }

  ToolMediator::Errs()->vdm_SetFilePosition (err);

  wstring smsg (PTAUX::ExtractMessageAll(file, err));

  wos << smsg << flush;
}

void TBDEBUG::UpdateGui (bool printPos, wostream & wos)
{
  TYPE_CI_ContextId cid;
  if (theStackMachine().CallStackLevel() != 0) {
    cid = theStackMachine().GetCidForCurBacktraceLevel();
  }
  else {
    cid = NilContextId;
  }

  POSITION::SetPosition(cid);

  if (printPos && (cid != NilContextId)) {
    PrintBreakPosition(wos);
  }

  EvalBackTrace (BT_UPDATE, wos); // update the back brace window in the interface.
  ToolMediator::Interf()->vdm_CallBack (PTAUX::mk_BtGotoLevel(theStackMachine().CurrentBacktraceLevel()));
}

// BacktraceType is used to indicate whether EvalBackTrace
// must print or update the back trace window.
// enum BacktraceType {BT_PRINT, BT_UPDATE};

// This function uses the global calls
void TBDEBUG::EvalBackTrace (BacktraceType bt_type, wostream & wos)
{
  TYPE_STKM_CallStack backtrace (theStackMachine().GetBackTrace());

  Sequence bt_l;

  size_t depth = backtrace.Length();
  for (size_t i = 1 ; i <= depth; i++) {
    TYPE_STKM_CallStackItem callinfo (backtrace.GetNth(i));
    const Generic & nm (callinfo.GetField(pos_STKM_CallStackItem_nmOrDesc)); // AS`Name | seq of char
    const Generic & arg_l (callinfo.GetField(pos_STKM_CallStackItem_arg_ul)); // [seq of SEM`VAL]

    wstring name (nm.IsRecord() ? ASTAUX::ASName2String(nm) : Sequence(nm).GetString());

    switch (bt_type) {
      case BT_PRINT: {
        /* Frame number information added 930413. MIAN*/
        wos << L"#" << i << L": " << name;
        if (!arg_l.IsNil()) {
          SEQ<TYPE_SEM_VAL> args_lv (arg_l);
          wos << L" ( ";
          size_t maxj = args_lv.Length ();
          for (size_t j = 1; j <= maxj; j++) {
            if (j > 1) {
              wos << L", ";
            }
            if (Settings.PrintFormat()) {
              VAL2X::val2stream (Record(args_lv[j]), wos, 7 + (name.length()));
            }
            else {
              VAL2X::val2stream( Record(args_lv[j]), wos, -1);
            }
          }
          wos << L" )";
        }
        wos << endl;
        break;
      }
      case BT_UPDATE: {
        // update the back trace window
        bt_l.ImpAppend (mk_(Sequence(name),
                            arg_l,
                            Bool(callinfo.get_type() == CallStackItemType::CS_DEBUGCMD)));
        break;
      }
    }
  }

  if (bt_type == BT_UPDATE) {
    ToolMediator::Interf()->vdm_CallBack (PTAUX::mk_BackTrace(bt_l));
  }
}

wstring TBDEBUG::GetTraceArgs(int index)
{
  wostringstream os_args;
  Sequence loc_bt_seq (ToolMediator::Interf()->vdm_GetBtSeq());
  // This list contains tuples where the second element is either a sequence or Nil. 

  Tuple info (loc_bt_seq[index]);

  if (info.GetField(2).IsNil()) {
    return wstring(L"No arguments");
  }
  Sequence args_lv (info.GetSequence (2));
  size_t len_args_lv = args_lv.Length ();
  os_args << L"(";
  for (size_t j = 1; j <= len_args_lv; j++) {
    if (j > 1) {
      os_args << L", ";
    }
    VAL2X::val2stream(args_lv[j], os_args, (Settings.PrintFormat() ? 0 : -1));
  }
  os_args << L")";

  return os_args.str();
}

void TBDEBUG::Execute_popd(wostream & wos)
{
  theStackMachine().UserPopDS();
  UpdateGui(false, wos);
}

Quote TBDEBUG::EvaluationStateToEvalState(const TYPE_STKM_EvaluationState & evalstate)
{
  switch (evalstate.GetTag()) {
    case TAG_TYPE_STKM_Breakpoint: { return Quote (L"BREAKPOINT"); break; }
    case TAG_TYPE_STKM_Interrupt:  { return Quote (L"INTERRUPT"); break; }
    case TAG_TYPE_STKM_Success:    { return Quote (L"SUCCESS"); break; }
    default:                       { return Quote (L"ERROR"); break; }
  }
}

void TBDEBUG::ProcessRuntimeError(wostream & wos)
{
  TBDEBUG::EvalBackTrace(TBDEBUG::BT_UPDATE, wos);

  TYPE_ProjectTypes_ErrMsg err (RTERR::GetErrMsg());
  ToolMediator::Errs()->vdm_AddMsg(err);
  ToolMediator::Errs()->vdm_SetFilePosition (err);

  RTERR::Reset();

  ToolMediator::Interf()->vdm_EnableUserInput();
}

void TBDEBUG::DisplaySigma (const wstring & args, wostream & wos)
{
  wstring params[20];
  int nos = STR_split (args, params, 20, STR_RXwhite_and_comma);
 
  Generic clsnm = Nil();
  if ( nos > 0 ) {
    clsnm = ASTAUX::MkName(params[0]);
  }
#ifdef VDMPP
  else {
    wos << L"sigma requires class name" << endl << flush;
    return;
  }
#endif // VDMPP

  try {
#ifdef VDMSL
    theState().DumpSigmaModule(clsnm, wos);
#endif // VDMSL
#ifdef VDMPP
    theState().DumpSigmaClass(clsnm, wos);
#endif // VDMPP
  }
  catch (TB_Exception &e) {
  }
}

void TBDEBUG::DisplayCode (const wstring & args, wostream & wos)
{
  wstring params[20];
  int nos = STR_split (args, params, 20, STR_RXwhite_and_comma);

  Generic clsnm = Nil();
  if ( nos > 0 ) {
    clsnm = ASTAUX::MkName(params[0]);
  }
  else {
#ifdef VDMSL
    clsnm = ASTAUX::GetDefaultModASName();
#endif // VDMSL
#ifdef VDMPP
    wos << L"sigma requires class name" << endl << flush;
    return;
#endif // VDMPP
  }
  wos << INT2Q::h2gAS(theCompiler().DumpProgram(clsnm)) << endl << flush;
}

#ifdef VDMPP
void TBDEBUG::EvalSaveObjects (const wstring & args, wostream & wos)
{
  wstring params[20];
  int nos = STR_split (args, params, 20, STR_RXwhite_and_comma);
  if( 0 == nos ) {
    wos << L"save requires file name" << endl << flush;
    return;
  }
  wstring filename(params[0]);

  Tuple tmp (theState().GetState());

  Sequence saved_objs_order( tmp.GetField(1) );
  Map saved_objs( tmp.GetField(2) );
  Map saved_obj_tab( tmp.GetField(3) );
  Generic saved_lastres( tmp.GetField(4) );
  Map saved_statics_m( tmp.GetField(5) );
  Map saved_vls_init_m( tmp.GetField(6) );

  // write to file
  ofstream ofs( TBWSTR::wstring2string(filename).c_str() );
  saved_objs_order.WriteVal( ofs ); // Sequence
  saved_objs.WriteVal( ofs );       // Map
  saved_obj_tab.WriteVal( ofs );                    // Map
  saved_lastres.WriteVal( ofs );      // Generic
  saved_statics_m.WriteVal( ofs );                  // Statics
  saved_vls_init_m.WriteVal( ofs );                 // Generic
  ofs.flush();
  ofs.close();
}

void TBDEBUG::EvalLoadObjects (const wstring & args, wostream & wos)
{
  wstring params[20];
  int nos = STR_split (args, params, 20, STR_RXwhite_and_comma);
  if( 0 == nos ) {
    wos << L"load requires file name" << endl << flush;
    return;
  }
  wstring filename(params[0]);
  ifstream ifs;
  wstring fname;
  int not_found;

  TBUTILS::Search_and_OpenFile(filename, fname, ifs, not_found);

  if ( 1 == not_found ) {
    wos << L"Couldn't open file '" << filename << L"'" << endl;
    return;
  }

  // Read data
  SEQ<TYPE_AS_Name> saved_objs_order( ReadVal( ifs ) );
  Map saved_objs( ReadVal( ifs ) );
  Map saved_obj_tab( ReadVal( ifs ) );
  Generic saved_lastres( ReadVal( ifs ) );
  Map saved_statics_m( ReadVal( ifs ) );
  Map saved_vls_init_m( ReadVal( ifs ) );
  ifs.close();

  theState().SetState(saved_objs_order, saved_objs, saved_obj_tab, saved_lastres, saved_statics_m, saved_vls_init_m);
}

void TBDEBUG::EvalPrintObjects(const wstring & args, wostream & wos)
{
  wstring params[20];
  int nos = STR_split (args, params, 20, STR_RXwhite_and_comma);
  if( 0 == nos ) {
    wos << L"PrintObjects requires oid(s)" << endl << flush;
    return;
  }

  Set oid_s;
  for (int j = 0; j < nos; j++ ) {
    Int objid (atoi(TBWSTR::wstring2string(params[j]).c_str()));
    if( objid != Int(0) ) {
      oid_s.Insert(objid);
    }
  }
  Map m (theState().CreateRefMap(oid_s));
  Set dom_m (m.Dom());
  Generic i;
  for (bool bb = m.First(i); bb; bb = m.Next(i)) {
    if (m[i].IsNil()) {
      wos << L"Object(" << i << L") is't found" << endl << flush;
    }
    else {
      VAL2X::PrintSemValue(m[i], wos);
    }
  }
}

void TBDEBUG::DisplaySystem(wostream & wos)
{
  theSystem().Dump(wos);
}

void TBDEBUG::DisplaySchedule(wostream & wos)
{
  theScheduler().Dump(wos);
}

void TBDEBUG::DisplayThreadInfo(const wstring & args, wostream & wos)
{
  wstring params[20];
  int nos = STR_split (args, params, 20, STR_RXwhite_and_comma);
  if( 0 == nos ) {
    wos << L"threadinfo requires thread id" << endl << flush;
    return;
  }
  int id = 0;
  if (TBUTILS::ExpectNumericArg((params[0]).c_str(), id)) {
    TYPE_SCHDTP_ThreadId tid (id);
    theScheduler().ShowThreadInfo(tid);
  }
  else {
    vdm_err << L"threadifo: expected integer value." << endl;
  }
}
#endif // VDMPP

void TBDEBUG::DisplayCurrentState(wostream & wos)
{
#ifdef VDMPP
#ifdef VICE
  wos << L"cpu: " << theSystem().GetCurCPU() << endl;
#endif // VICE
  wos << L"thread: " << theScheduler().CurThreadId() << endl;
#endif // VDMPP
// h2gAS
  wos << theStackMachine().GetEvaluatorState() << endl;
  wos << INT2Q::h2gAS(theStackMachine().GetProgram()) << endl;
}

void TBDEBUG::EvalTraces(const wstring & args, wostream & wos)
{
  wstring params[20];
  int nos = STR_split (args, params, 20, STR_RXwhite_and_comma);

#ifdef VDMSL
  TYPE_AS_Name defmod (ASTAUX::GetDefaultModASName());
  Generic trnm = Nil();
  Generic tpnm = Nil();
  TYPE_AS_Name clmodnm (defmod);
  if ( 0 != nos ) {
    TYPE_AS_Name n (ASTAUX::MkName(params[0]));
    TYPE_AS_Ids ids (n.GetSequence(pos_AS_Name_ids));
    if (ids.Length() == 1) {
      if (ToolMediator::GetAllVDMModuleNames().InSet(n)) {
        clmodnm = n;
      }
      else {
        trnm = n; 
        tpnm = AUX::ConstructDoubleName(defmod, n);
      }
    }
    else {
      clmodnm = ASTAUX::GetFirstName(n);
      trnm = ASTAUX::GetSecondName(n);
      tpnm = n;
    }
  }
#endif // VDMSL
#ifdef VDMPP
  if( 0 == nos ) {
    wos << L"Please, specify a class name or test name." << endl << flush;
    return;
  }

  TYPE_AS_Name tpnm (ASTAUX::MkName(params[0]));
  TYPE_AS_Name clmodnm (ASTAUX::GetFirstName(tpnm));
  Generic trnm = Nil();
  if (tpnm.GetSequence(pos_AS_Name_ids).Length() > 1) {
    trnm = ASTAUX::GetSecondName(tpnm);
  }
#endif // VDMPP

  bool SavedSetting_DTC(Settings.DTC());
  bool SavedSetting_PRE(Settings.PreCheck());
  bool SavedSetting_POST(Settings.PostCheck());
  bool SavedSetting_INV(Settings.INV());
  bool SavedSetting_Assertion(Settings.Assertion());
  bool SavedSetting_RTEE(Settings.RTErrException());
  bool SavedSetting_UsesOldName(theStackMachine().UsesOldName());

/*
  Settings.DtcOff();
  Settings.PreOff();
  Settings.PostOff();
  Settings.InvOff();
  Settings.AssertOff();
  Settings.RTErrExceptionOff();
  theStackMachine().SetUsesOldName(false);
*/

  Map m;
  try {
    SEQ<Record> ast_l (ToolMediator::GetVDMASTs ());

    // initialize specification for expand trace
    User_Init(ast_l, TOOLS::get_ast_is_new(), wos);

    Map em (EXPANDED::ExpandSpecTraces(ast_l, mk_set(clmodnm), wos));

    Set dom_em (em.Dom());
    Generic nm;
    for (bool bb = dom_em.First(nm); bb; bb = dom_em.Next(nm)) {
      if (trnm.IsNil() || (nm == tpnm)) {
        m.ImpModify(nm, em[nm]);
      }
    }
  } 
  catch (TB_Exception &e) {
    TYPE_ProjectTypes_ErrMsg err (RTERR::GetErrMsg());
    wstring file (ToolMediator::GetFileName(POSITION::GetErrFile()));

    if (!file.empty()) {
      file = L"In " + file;
    }
    wstring smsg (PTAUX::ExtractMessageAll(file, err));
    wos << smsg << flush;
    wos << L"expand failed" << endl;
    return;
  }

  if(SavedSetting_DTC) {
    Settings.DtcOn();
  }
  if(SavedSetting_PRE) {
    Settings.PreOn();
  }
  if(SavedSetting_POST) {
    Settings.PostOn();
  }
  if(SavedSetting_INV) {
    Settings.InvOn();
  }
  if(SavedSetting_Assertion) {
    Settings.AssertOn();
  }
  if(SavedSetting_RTEE) {
    Settings.RTErrExceptionOn();
  }
  theStackMachine().SetUsesOldName(SavedSetting_UsesOldName);

  if (m.IsEmpty()) {
    wos << L"Testcase isn't defined." << endl;
    return;
  }

  TOOLS::setTracesMode(true);

  Map result;
  SEQ<Record> ast_l (ToolMediator::GetVDMASTs ());
  AS2ASCII conv;
  Set dom_m (m.Dom());
  Generic nm;
  for (bool bb = dom_m.First(nm); bb; bb = dom_m.Next(nm)) {
    Set e_l_s (m[nm]);
    int number_of_cases = e_l_s.Card();
    conv.Expr2ASCII(nm, wos);
    wos << L": " << number_of_cases << L" cases" << endl;
      
    int count_ok = 0;
    int count_ng = 0;
    int count_skip = 0;
    Map errs;
    int num = 1;
    Generic e_l;
    for (bool cc = e_l_s.First(e_l); cc; cc = e_l_s.Next(e_l)) {
      conv.Expr2ASCII(nm, wos);
      wos << L" Case: " << num << L" / " << number_of_cases << endl;
      num++;
      wostringstream wostr;
      Set dom_errs (errs.Dom());
      SEQ<TYPE_AS_Expr> expr_l (e_l);
      bool exists = false;
      size_t len_expr_l = expr_l.Length();
      for (size_t idx = 1; idx <= len_expr_l; idx++) {
        if (idx > 1) {
          wostr << L"; ";
        }
        conv.Expr2ASCII(INT2Q::h2gAS(expr_l[idx]), wostr);
        if (!dom_errs.IsEmpty()) {
          exists = dom_errs.InSet(SEQ<Char>(wostr.str())) | exists;
        }
      }
      wos << wostr.str() << endl;

      if (exists) {
        count_skip++;
        wos << L"<SKIP>" << endl;
        continue;
      }

      wstring estr;
      try {
        User_Init(ast_l, TOOLS::get_ast_is_new(), wos);

        bool forall = true;
#ifdef VDMSL
        theStackMachine().PushModule(clmodnm);
#endif // VDMSL
#ifdef VDMPP
        TYPE_AS_NewExpr ne;
        ne.Init(clmodnm, SEQ<TYPE_AS_Expr>(), NilContextId);
  
        Tuple res (theStackMachine().EvalPrint(ne, Sequence(L"Create Pushed Object")));

        const TYPE_STKM_EvaluationState & eval_state (res.GetRecord(1));
        if (!eval_state.Is(TAG_TYPE_STKM_Success)) {
          forall = false;
        }
        theStackMachine().PushCurObj(res.GetRecord(2), clmodnm, clmodnm);
#endif // VDMPP
        theStackMachine().PushEmptyEnv();

        for (size_t idx = 1; (idx <= len_expr_l) && forall; idx++) {
          wostringstream wostr;
          conv.Expr2ASCII(INT2Q::h2gAS(expr_l[idx]), wostr);
          wos << wostr.str() << L": ";
          if (idx > 1) {
            estr += L"; ";
          }
          estr += wostr.str();
                
          Tuple res (theStackMachine().EvalUninterruptedCmd(expr_l[idx],
                                                            TYPE_STKM_SubProgram(),
                                                            TYPE_STKM_SubProgram(),
                                                            SEQ<Char>(L"Traces Check")));

          const TYPE_STKM_EvaluationState & eval_state (res.GetRecord(1));
          if (eval_state.Is(TAG_TYPE_STKM_Success)) {
            const TYPE_SEM_VAL & val (res.GetRecord(2));
            wostringstream wostr;
#ifdef VDMSL
            if (clmodnm == defmod) {
              switch (val.GetTag()) {
                case TAG_TYPE_SEM_CONT: {
                  VAL2X::PrintSemValue(val, wostr);
                  break;
                }
                case TAG_TYPE_SEM_EXIT: {
                  wostr << L"exit";
                  const Generic & value (val.GetField(pos_SEM_EXIT_v));
                  if (!value.IsNil()) {
                    wostr << L" ";
                    conv.Expr2ASCII(SemRec::VAL2Expr(value), wostr);
                  }
                  wostr << endl;
                  break;
                }
                default: {
                  conv.Expr2ASCII(SemRec::VAL2Expr(val), wostr);
                  wostr << endl;
                }
              }
            }
            else {
              VAL2X::PrintSemValue(val, wostr);
            }
#endif // VDMSL
#ifdef VDMPP
            VAL2X::PrintSemValue(val, wostr);
#endif // VDMPP
            wos << wostr.str();
          }
          else {
            forall = false;
          }
        }
        theStackMachine().PopEnvL();
#ifdef VDMSL
        theStackMachine().PopModule();
#endif // VDMSL
#ifdef VDMPP
        theStackMachine().PopCurObj();
#endif // VDMPP
        if (forall) {
          count_ok++;
          wos << L"<OK>" << endl;
        }
        else
          wos << L"NG" << endl;
      }
      catch (TB_Exception &e) {
        TYPE_ProjectTypes_ErrMsg err (RTERR::GetErrMsg());
        wstring file (ToolMediator::GetFileName(POSITION::GetErrFile()));

        if (!file.empty()) {
          file = L"In " + file;
        }
        wstring smsg (PTAUX::ExtractMessageAll(file, err));
        wos << smsg << flush;

        wos << L"<NG>" << endl;
        errs.ImpModify(SEQ<Char>(estr), err);
        count_ng++;
      }
    }
    conv.Expr2ASCII(nm, wos);
    wos << L": OK(" << count_ok;
    wos << L") NG(" << count_ng;
    wos << L") SKIP(" << count_skip << L")" << endl;
    result.ImpModify(nm, mk_(Int(e_l_s.Card()), Int(count_ok), Int(count_ng), Int(count_skip)));
  }
  TOOLS::setTracesMode(false);
}
