/***
*  * WHAT
*  *    Top level interface to the base tools in the Tool-box
*  *    User's of the base tools
*  * FILE
*  *    $Source: /mnt/cvsroot/toolbox/code/utils/tools.cc,v $
*  * VERSION
*  *    $Revision: 1.408 $
*  * DATE
*  *    $Date: 2006/10/13 12:04:43 $
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

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif // _MSC_VER

#include "tools.h"
#include "statsem.h"
#include "rt_errmsg.h"
#include "tb_exceptions.h"
#include "stringext.h"
#include "tb_version.h"
#include "libman.h"
#include "tbdebug.h"
#include "intconvquotes.h"
#include "position.h"
#include "BaseTools.h"
#include "settings.h"
#include "val2x.h"
#include "astaux.h"
#include "specfile.h"
#include "pexpr.h"
#include "codegen_tools.h"
#include "evalaux.h"
#include "tb_wstring.h"
#include "tbutils.h"
#include "projectval.h"

#include "ProjectTypes.h"

#include "vdmpars.h" // orderly, modorder
#include "testcoverage.h" // EvalListTestSuite

#ifdef VDMSL
#include "pattern.h"
#endif // VDMSL
#ifdef VDMPP
#include "javagen_tools.h" // for UpdateJSS
#include "MANGLE.h" // for MANGLE
#include "vppstmt.h" // for pars_vpp_asgn_invoke_stmt
#ifdef VICE
#include "TIMETRACE.h"
#include "timeparser_aux.h" // for tp_main tp_OutputErrors
#endif // VICE
#endif // VDMPP

// for init
#include "CPP.h"
#include "CGTYPES.h"
#include "POG_uMEDIATOR.h"
#ifdef VDMPP
#include "J2V.h"
#include "JSSEXPR.h"
#include "JSSREP.h"
#endif // VDMPP

#ifdef CORBA_API
// NOTE: <windows.h> included in omnithread.h
#include "init_corba.h"
#include "cmd_queue.h"
#include "object_map.h"
#include "metaiv_impl.h"
// 20150319 -->
/*
#include "servicemanager.h"
#include "parserservice.h"
*/
// <-- 20150319
#endif // CORBA_API

#include "RTERR.h"
#include "random.h"
#include "binops.h"
#include <sstream>

// 20130617 -->
#include "tb_readline.h"
// <-- 20130617

ofstream TOOLS::calllogstream;

// Mediator object used to communicate back to repository, interface
// errors class etc.

vdm_ToolMediator * ToolMediator::mediator = NULL;

bool TOOLS::isinscript = false;

void TOOLS::SetIsInScript(bool isin)
{
  TOOLS::isinscript = isin;
}

bool TOOLS::IsInScript()
{
  return TOOLS::isinscript;
}

// spec_init indicates whether a specification has been changed
// since last init.
// print/debug && spec_init == false => warning to the user
bool TOOLS::spec_init = false;

void TOOLS::set_spec_init(bool v)
{
  TOOLS::spec_init = v;
}

bool TOOLS::get_ast_is_new()
{
  return !TOOLS::spec_init;
}

#ifdef VDMPP
bool TOOLS::dobjs_init = false;

void TOOLS::set_dobjs_init(bool v)
{
  dobjs_init = v;
}

bool TOOLS::get_dobjs_init()
{
  return dobjs_init;
}

#endif //VDMPP

// Now RT_INFO is always enabled from the start and not dependent on
// the '-r' option (task TB33).
//int RT_INFO  = 1;

//Records if the '-b' option was set at command-line
bool TOOLS::BATCHMODE = false;

bool TOOLS::isBatchMode()
{
  return TOOLS::BATCHMODE;
}

void TOOLS::setBatchMode(bool mode)
{
  TOOLS::BATCHMODE = mode;
}

bool TOOLS::TRACESMODE = false;

bool TOOLS::isTracesMode()
{
  return TOOLS::TRACESMODE;
}

void TOOLS::setTracesMode(bool mode)
{
  TOOLS::TRACESMODE = mode;
}
//An instance of the typechecker which will be used for type checking specs.

#ifdef VDMPP
wstring TOOLS::priorityfilename; // stores the name of the last priority file loaded

void TOOLS::SetPriorityFile(const wstring & filename)
{
  TOOLS::priorityfilename = filename;
}
wstring TOOLS::GetPriorityFile()
{
  return TOOLS::priorityfilename;
}
#endif //VDMPP

#ifdef VICE
wstring TOOLS::timefilename; // stores the name of the last timefile loaded
wstring TOOLS::logfilename = L"logfile";  // stores the name of the last logfile name set
wstring TOOLS::GetLogFile()
{
  return TOOLS::logfilename;
}
void TOOLS::SetTimeFile(const wstring & filename)
{
  TOOLS::timefilename = filename;
}
wstring TOOLS::GetTimeFile()
{
  return TOOLS::timefilename;
}
#endif // VICE

// ToolMediator
void ToolMediator::SetMediator (vdm_ToolMediator * m)
{
  ToolMediator::mediator = m;
}

vdm_Errors * ToolMediator::Errs ()
{
  return ObjGet_vdm_Errors (ToolMediator::mediator->vdm_GetErrors());
}

vdm_Errors * ToolMediator::ExprErrs ()
{
  return ObjGet_vdm_Errors (ToolMediator::mediator->vdm_GetExprErrors());
}

vdm_Repository * ToolMediator::Repos ()
{
  return ObjGet_vdm_Repository (ToolMediator::mediator->vdm_GetRepository ());
}

vdm_Interface * ToolMediator::Interf ()
{
  return ObjGet_vdm_Interface (ToolMediator::mediator->vdm_GetInterface ());
}

vdm_BaseTools * ToolMediator::BTools ()
{
  return ObjGet_vdm_BaseTools (ToolMediator::mediator->vdm_GetBaseTools ());
}

vdm_UMLTool * ToolMediator::UMLT ()
{
  return ObjGet_vdm_UMLTool (ToolMediator::mediator->vdm_GetUMLTool ());
}

SEQ<Record> ToolMediator::GetVDMASTs()
{
  SEQ<Record> ast_l;
  if (ToolMediator::mediator != NULL) {
    SEQ<TYPE_ProjectTypes_AstVal> astval (Repos()->vdm_GetVDMAsts ());
    ast_l = PTAUX::ExtractAstOrFlats(astval);
  }
  return ast_l;
}

SEQ<Record> ToolMediator::GetJavaASTs()
{
  SEQ<Record> ast_l;
  if (ToolMediator::mediator != NULL) {
    SEQ<TYPE_ProjectTypes_AstVal> astval (Repos()->vdm_GetJavaAsts ());
    ast_l = PTAUX::ExtractAstOrFlats(astval);
  }
  return ast_l;
}

SEQ<Record> ToolMediator::GetAllASTs ()
{
  SEQ<Record> ast_l;
  if (ToolMediator::mediator != NULL) {
    SEQ<TYPE_ProjectTypes_AstVal> astval (Repos()->vdm_GetAllAsts ());
    ast_l = PTAUX::ExtractAstOrFlats(astval);
  }
  return ast_l;
}

wstring ToolMediator::GetFileName(int fileid)
{
  Generic file (ToolMediator::mediator->vdm_GetFileName (fileid));
  if (file.IsRecord () && PTAUX::is_FileName(file))
    return PTAUX::ExtractFileName(file);
  else
    return L"";
}
// end of ToolMediator

/** Commands **************************************************************/
/** The debugger commands are initialized with the function              **/
/**   void InitDebugCommands ()                                          **/
/** New commands can be added with the function                          **/
/**   void AddCommand (wstring) -- used by InitDebugCommands              **/
/**   The wstring argument is constructed such that the full name is      **/
/**   written first followed by any abbreviations separated by blanks.   **/
/**   See 'InitDebugCommands' for an example.                            **/
/** For each new command, it is checked that all commands and their abb- **/
/** reviations are unique.                                               **/

/** abbrev ****************************************************************/
/** abbrev is a pointer to a map from command abbreviations to full names */

Map TOOLS::abbrev;

// CheckAbbrev
int TOOLS::CheckAbbrev (const Text & fullname, const Set & abbreviations)
{
  int whatswrong = 0;
  if (!abbreviations.InSet (fullname))
    whatswrong += 1;

  Set ab_dom (abbrev.Dom ());
  ab_dom.ImpIntersect (abbreviations);
  if (!ab_dom.IsEmpty())
    whatswrong += 2;
  // whatswrong == 0 <=> OK
  // whatswrong == 1 <=> fullname not a legal abbreviation
  // whatswrong == 2 <=> some abbreviation already defined
  // whatswrong == 3 <=> both 1 and 2
  return whatswrong;
}

// AddCommand
void TOOLS::AddCommand (const wchar_t * forms_)
{
  wstring forms (forms_);
  wstring allforms[10];
  int no_of_forms = STR_split (forms, allforms, 10, STR_RXwhite);

  Set abb;
  for (int i = 0; i < no_of_forms; abb.Insert (Text (allforms[i++])));

  int anythingwrong = CheckAbbrev (Text (allforms[0]), abb);
  if (anythingwrong > 0) {
    vdm_log << L"Error in command initialization for \"" << allforms[0]
            << L"\". Error code was : " << anythingwrong << endl << endl;
    exit (2);
  }

  Generic a;
  Text fn (allforms[0]);
  for (bool bb = abb.First (a); bb; bb = abb.Next (a))
    TOOLS::abbrev.Insert (a, fn);
}

void TOOLS::InitDebugCommands ()
{
  TOOLS::abbrev.Clear();

  AddCommand (L"#");
  AddCommand (L"backtrace bt");
  AddCommand (L"break b");
  AddCommand (L"condition cond");
  AddCommand (L"cont c");
  AddCommand (L"cquit q");
#ifdef VDMPP
  AddCommand (L"classes");
  AddCommand (L"create cr");
#endif //VDMPP
#ifdef VDMSL
  AddCommand (L"curmod");
#endif //VDMSL
#ifdef VDMPP
  AddCommand (L"curcls");
  AddCommand (L"curthread");
#endif //VDMPP
  AddCommand (L"date");
  AddCommand (L"debug d");
  AddCommand (L"delete dl");
#ifdef VDMPP
  AddCommand (L"destroy");
  AddCommand (L"uses");
#endif //VDMPP
  AddCommand(L"dlclose dlc");

  AddCommand(L"disable dis");
  AddCommand(L"enable ena");

  AddCommand (L"code");
  AddCommand (L"codegen cg");
  AddCommand (L"javacg jcg");

  AddCommand (L"dir");
  AddCommand (L"down");
  AddCommand (L"_evalstatus");
  AddCommand (L"finish");
  AddCommand (L"first f");
  AddCommand (L"functions");
  AddCommand (L"hinfo ??");
  AddCommand (L"info help ?");
  AddCommand (L"init i");
  AddCommand (L"ii");
#ifdef VDMPP
  AddCommand (L"instvars");
#endif //VDMPP
  AddCommand (L"last");
  AddCommand (L"latex l");
#ifdef VDMSL
  AddCommand (L"modules");
#endif //VDMSL
  AddCommand (L"next n");
  AddCommand (L"new");
#ifdef VDMPP
  AddCommand (L"nocheck nc");
  AddCommand (L"objects obj");
#endif //VDMPP
  AddCommand (L"operations");
  AddCommand (L"pe");
  AddCommand (L"pop");
  AddCommand (L"popd");
  AddCommand (L"previous pr");
  AddCommand (L"print p");
#ifdef VDMPP
  AddCommand (L"po");
#endif //VDMPP
  AddCommand (L"push");
  AddCommand (L"pwd");
  AddCommand (L"quit");
  AddCommand (L"read r");
  AddCommand (L"tcov");
  AddCommand (L"rtinfo");
  AddCommand (L"script !");
  AddCommand (L"search se");
#ifdef VDMPP
  AddCommand (L"selthread");
#endif //VDMPP
  AddCommand (L"set");
  AddCommand (L"sigma");
  AddCommand (L"singlestep g");
#ifdef VDMSL
  AddCommand (L"stack");
  AddCommand (L"states");
#endif //VDMSL
  AddCommand (L"step s");
  AddCommand (L"stepin si");
  AddCommand (L"system sys");
#ifdef VICE
  AddCommand (L"timefile tf");
  AddCommand (L"gettime gt");
#endif //VICE
#ifdef VDMPP
  AddCommand (L"threads");
#endif //VDMPP
  AddCommand (L"up");
#ifdef VDMPP
  AddCommand (L"priorityfile pf");
#endif //VDMPP
  AddCommand (L"typecheck tc");
  AddCommand (L"types");
  AddCommand (L"unset");
  AddCommand (L"values");
#ifdef CORBA_API
  AddCommand (L"object-table");
#endif //CORBA_API
  AddCommand (L"traces");

  AddCommand (L"encode");
  AddCommand (L"fscode");

  AddCommand (L"ast"); // for Debug 20051215
  AddCommand (L"contextinfo ci"); // for Debug 20051215
  AddCommand (L"tokeninfo ti"); // for Debug 20060607
  AddCommand (L"nodeinfo ni"); // for Debug 20060607
 
#ifdef VDMPP
  AddCommand (L"save");
  AddCommand (L"load");
  AddCommand (L"state");
  AddCommand (L"cpu");
  AddCommand (L"schedule");
  AddCommand (L"threadinfo");
#endif // VDMPP
  AddCommand (L"curstate");

  AddCommand (L"version v");
  AddCommand (L"find");
}

// UpdateSS
#ifdef VDMSL
// mod : AS`Module | AS`DLModule
#endif //VDMSL
#ifdef VDMPP
// mod : AS`Class
#endif //VDMPP
void TOOLS::UpdateSS(const Record & mod)
{
#ifdef VDMSL
  if (!TOOLS::isBatchMode())
    GetStatSem().ResetErrors();

  GetStatSem().UpdateModuleEnv (mod);

#endif //VDMSL
#ifdef VDMPP
  //Is only updated for checking the VDM++ parser.
  TYPE_AS_Document mod_l;
  mod_l.ImpAppend(mod);
  GetStatSem().UpdateParseEnvWithAST(mod_l);
#endif //VDMPP
}

// UpdateToolbox
void TOOLS::UpdateToolbox ()
{
#ifdef VDMSL
  if (ToolMediator::Repos()->vdm_IsSession (struct_session))
  {
    SEQ<Record> ast_l (ToolMediator::GetVDMASTs ());
    int len_ast_l = ast_l.Length();
    for (int idx = 1; idx <= len_ast_l; idx++)
    {
      UpdateSS(ast_l[idx]);
    }
  }
#endif //VDMSL
}

// Initialises the types defined in AS module. Setting the don't care fields.
void TOOLS::InitModuleTypes()
{
  init_AS();
  init_REP();
  init_CI();
#ifdef VDMPP
  init_MANGLE();
#endif // VDMPP

  init_CPP();
  init_CGTYPES();
  init_INSTRTP();
  init_SEM();
  init_GLOBAL();
#ifdef VDMPP
  init_SCHDTP();
#endif // VDMPP
  VAL2X::init_STKM();
  init_DEBUGTP();

  VAL2X::init_STAT_SEM();

  // Initialise the pretty printing functions of the semantic values.
  VAL2X::init_EVAL_SEM();

  // JSS

#ifdef VDMPP
  init_BUILD();
  init_ASTMERGE();
  init_JSSAUX();
  init_JSSENV();
  init_JSSERRMSG();
  init_JSSEXPR();
  init_JSSVCM1();
  init_JSSREP();

  init_J2VAUX();
  init_J2VCLASS();
  init_J2VENV();
  init_J2VERRMSG();
  init_J2VNS();
  init_J2VOP();
  init_J2VSTMT();

#ifdef VICE
  init_TIMEPARSER() ;
  init_TIMETRACETP();
#endif // VICE
#endif // VDMPP

  init_POG_uMEDIATOR();
  init_POGTP();
  init_FRESH();
  init_POG_uPAT();
  init_POG_uEXPR();
  init_POG_uSTMT();

  VAL2X::init_POG_TEST();
}

void TOOLS::InitToolbox (bool batch_mode)
{
  TOOLS::setBatchMode(batch_mode);

  TOOLS::InitDebugCommands ();

  LibraryManager::Init(); // 20071227

  theStackMachine().User_Init(TYPE_AS_Document(), true);

  theStackMachine().ResetBreakpoints ();

#ifndef NOSS
  GetStatSem().InitEnv ();
#endif //!NOSS

  TBUTILS::ResetSearchPath();

  TBUTILS::SetDefaultPath (TBUTILS::tb_getcwd());

  GetStatSem().InitParseEnv();
}

enum { C_flag_NONE = 0,
       C_flag_Interpreter = 1,
};

void TOOLS::PrintConsistencyStatus()
{
  if (Settings.GetCFlag() == 0)
  return;

  wofstream of("vdmrun.debug", ios::app);
  if (! of)
    return;

  of << L"Start " << TBUTILS::GetDateString( "%Y%m%d %H:%M:%S" ) << L" flag=" << Settings.GetCFlag() << endl;
  if (Settings.GetCFlag() & C_flag_Interpreter)
    TBDEBUG::PrintEvaluatorStatus(of, true);
  of << L"End" << endl;
  of.close();
}

void TOOLS::CleanUp()
{
  ToolMediator::NewUnnamedProject();

  ToolMediator::ClearAll ();

  LibraryManager::Finish();

  vdm_log.flush();
  vdm_iplog.flush();
  vdm_err.flush();

  GetCI().clear();

  // clone count
  {
    const char* c = getenv("VDMLIBDEBUG");
    if (c) {
      wofstream of("vdmlib.debug");
      VDMLibControl::print_copy_stats(of);
    }
  }

#ifdef CORBA_API
if (!TOOLS::isBatchMode())
  TOOLS::TerminateCorbaApi();
#endif //CORBA_API
}

void TOOLS::ExitToolbox(int i)
{
  CleanUp();
  PrintConsistencyStatus();
  TBUTILS::restoreLCNumeric();
  CloseCallLog(); // 20130530
  exit(i);
}

wstring TOOLS::ExpandCommand (wstring & what_the_user_wrote)
{
  int Max = what_the_user_wrote.length();
  int start = 0;
  while (start < Max && iswspace(what_the_user_wrote[start])) start++;
  int end = start;

  if (what_the_user_wrote[start] == '!') {
    end++;
  }
  else {
    while (end < Max && !iswspace(what_the_user_wrote[end])) end++;
  }

  wstring cmd (what_the_user_wrote.substr(start, end-start));
  what_the_user_wrote.replace(0, end, L"");

  if ((cmd.length() > 0) && ((cmd[0] == L'-') || (cmd[0] == L'#'))) {
    return L"#";
  }

  if (TOOLS::abbrev.DomExists (Text (cmd))) {
    return (Text (TOOLS::abbrev[Text (cmd)])).GetValue ();
  }
  else {
    return L"UNKNOWN";
  }
}

// true => success
bool TOOLS::ReadScriptFile (const wstring & short_name)
{
  bool tmpmode = TOOLS::isBatchMode();
  TOOLS::setBatchMode(true);

// 20091102-->
//  if (!TOOLS::IsInScript())
//    ToolMediator::ExprErrs()->vdm_ClearAll();
// <-- 20091102

  bool tmpisinscript = TOOLS::IsInScript();
  TOOLS::SetIsInScript(true);

  wstring fname;
  ifstream sp;
  int not_found;

  TBUTILS::Search_and_OpenFile(short_name, fname, sp, not_found);
  if (not_found == 1) {
    vdm_log << L"Couldn't open file '" << short_name << L"'" << endl;
    TOOLS::setBatchMode(tmpmode);
    return true; // is not considered as an error;
  }

  bool res = true;
  while ((sp.peek () != EOF) && res) {
    while ((sp.peek () != EOF) && ((sp.peek () == '\x0a') || (sp.peek () == '\x0d'))) {
      sp.get ();
    }
    string input;
    while ((sp.peek () != EOF) && (sp.peek () != '\x0a') && (sp.peek () != '\x0d')) {
      input += (char) sp.get (); // without cast OS/2 appends ascii: A->L"65"
    }
    if (input.empty()) break;

    wstring inputw (TBWSTR::mbstr2wstring(input));
    wstring cmd (inputw);
    wstring command (TOOLS::ExpandCommand (inputw));

    if (!(command == L"")) {
      vdm_iplog << cmd << endl;
      res = TOOLS::Execute (command, inputw);
    }
  }

  sp.close();
  TOOLS::setBatchMode(tmpmode);
  TOOLS::SetIsInScript(tmpisinscript);
  return res;
}

// true => success
bool TOOLS::ReadStartUpFile ()
{
#ifdef VDMSL
  const wstring dotvdm = L".vdmde";
#endif //VDMSL
#ifdef VDMPP
  const wstring dotvdm = L".vppde";
#endif // VDMPP
  const wstring homedotvdm = L"~/"+dotvdm;
  wifstream sp;
  sp.open (TBWSTR::wstring2fsstr(dotvdm).c_str());
  if (!(sp.good ())) {
    sp.clear ();
    sp.open (TBWSTR::wstring2fsstr(homedotvdm).c_str());
    if (!(sp.good ())) {
      sp.close();
      return true ;
    }
    else {
      ReadScriptFile (homedotvdm);
    }
  }
  else {
    ReadScriptFile (dotvdm);
  }
  return true;
}

//The following functions (RemoveConstructs,
//UpdateAST and AddModuleAST) are all auxilliary functions to the
//EvalRead function.

//
// RemoveConstructs removes all constructs which are no longer in
// the file denoted by file_id.
// The constructs are both removed form Origin and AST.

#ifdef VDMSL
Tuple ToolMediator::RemoveConstructs (int file_id, const Record & Origin, const TYPE_AS_Definitions & AST)
{
  Map types_origin (Origin.GetMap (OriginTypes));
  Int state_origin (Origin.GetInt (OriginState));
  Map values_origin (Origin.GetMap (OriginValues));
  Map functions_origin (Origin.GetMap (OriginFcts));
  Map operations_origin (Origin.GetMap (OriginOps));
  Map traces_origin (Origin.GetMap (OriginTraces));

  Record NewOrigin (OriginTag, OriginLength);
  TYPE_AS_Definitions NewAST (AST);

  // AST should be parameter to this function

  // Extract type map
  MAP<TYPE_AS_Name, TYPE_AS_TypeDef> typem (AST.GetMap(pos_AS_Definitions_typem));
  Map new_types_origin;
  Generic tpnm;
  for (bool bb = types_origin.First (tpnm); bb; bb = types_origin.Next (tpnm)) {
    if (types_origin[tpnm] == Int (file_id)) {
      typem.RemElem (tpnm);
    }
    else {
      new_types_origin.ImpModify (tpnm, types_origin[tpnm]);
    }
  }
  NewAST.SetField(pos_AS_Definitions_typem, typem);
  NewOrigin.SetField (OriginTypes, new_types_origin);

  // value map
  const SEQ<TYPE_AS_ValueDef> & valuem (AST.GetSequence(pos_AS_Definitions_valuem));
  SEQ<TYPE_AS_ValueDef> new_valuem;
  Map new_values_origin;
  int len_valuem = valuem.Length();
  for (int idx = 1; idx <= len_valuem; idx++) {
    const TYPE_AS_Pattern & pat (valuem[idx].GetRecord (pos_AS_ValueDef_pat));
    TYPE_AS_Pattern patStrip (PAT::StripPatternTypeInfo (pat));
    if (!(values_origin[patStrip] == Int (file_id))) {
      new_values_origin.ImpModify (patStrip, values_origin[patStrip]);
      new_valuem.ImpAppend (valuem[idx]);
    }
  }
  NewAST.SetField(pos_AS_Definitions_valuem, new_valuem);
  NewOrigin.SetField (OriginValues, new_values_origin);

  // function map
  MAP<TYPE_AS_Name, TYPE_AS_FnDef> fnm (AST.GetMap(pos_AS_Definitions_fnm));
  Map new_functions_origin;
  Generic fnnm;
  for (bool dd = functions_origin.First (fnnm); dd; dd = functions_origin.Next (fnnm)) {
    if (functions_origin[fnnm] == Int (file_id)) {
      fnm.RemElem (fnnm);
    }
    else {
      new_functions_origin.ImpModify (fnnm, functions_origin[fnnm]);
    }
  }
  NewAST.SetField(pos_AS_Definitions_fnm, fnm);
  NewOrigin.SetField (OriginFcts, new_functions_origin);

  // operation map
  MAP<TYPE_AS_Name, TYPE_AS_OpDef> opm (AST.GetMap(pos_AS_Definitions_opm));
  Map new_operations_origin;
  Generic opnm;
  for (bool ee = operations_origin.First (opnm); ee; ee = operations_origin.Next (opnm)) {
    if (operations_origin[opnm] == Int (file_id)) {
      opm.RemElem (opnm);
    }
    else {
      new_operations_origin.ImpModify (opnm, operations_origin[opnm]);
    }
  }
  NewAST.SetField(pos_AS_Definitions_opm, opm);
  NewOrigin.SetField (OriginOps, new_operations_origin);

  // state
  Int new_state_origin (state_origin);
  if (state_origin.GetValue () == file_id) {
    new_state_origin = Int (-1);
    NewAST.set_State (Nil ());
  }
  NewOrigin.SetField (OriginState, new_state_origin);

  // traces map
  Map tracem (AST.GetMap(pos_AS_Definitions_tracem));
  Map new_traces_origin;
  Generic trnm;
  for (bool ee = traces_origin.First (trnm); ee; ee = traces_origin.Next (trnm)) {
    if (traces_origin[trnm] == Int (file_id)) {
      tracem.RemElem (trnm);
    }
    else {
      new_traces_origin.ImpModify (trnm, traces_origin[trnm]);
    }
  }
  NewAST.SetField(pos_AS_Definitions_tracem, tracem);
  NewOrigin.SetField (OriginTraces, new_traces_origin);

  return mk_(NewOrigin, NewAST);
}

Record ToolMediator::EmptyOrigin ()
{
  Record Origin (OriginTag, OriginLength);
  Origin.SetField (OriginTypes, Map ());
  Origin.SetField (OriginState, Int (-1));
  Origin.SetField (OriginValues, Map ());
  Origin.SetField (OriginFcts, Map ());
  Origin.SetField (OriginOps, Map ());
  Origin.SetField (OriginTraces, Map ());
  return Origin;
}

TYPE_AS_Definitions ToolMediator::EmptyDefinitions ()
{
  TYPE_AS_Definitions ast;
  ast.SetField(pos_AS_Definitions_typem,  MAP<TYPE_AS_Name, TYPE_AS_TypeDef>());
  ast.SetField(pos_AS_Definitions_valuem, SEQ<TYPE_AS_ValueDef>());
  ast.SetField(pos_AS_Definitions_fnm,    MAP<TYPE_AS_Name, TYPE_AS_FnDef>());
  ast.SetField(pos_AS_Definitions_opm,    MAP<TYPE_AS_Name, TYPE_AS_OpDef>());
  ast.SetField(pos_AS_Definitions_State,  Nil());
  ast.SetField(pos_AS_Definitions_tracem, Map());
  ast.SetField(pos_AS_Definitions_cid,    NilContextId);
  return ast;
}

// create module for flat spec
TYPE_AS_Module ToolMediator::mk_DefaultMod (const TYPE_AS_Definitions & def)
{
  return TYPE_AS_Module().Init(ASTAUX::GetDefaultModASName(),
                               TYPE_AS_Interface().Init(MAP<TYPE_AS_Name, Generic>(), Nil(), NilContextId),
                               def,
                               NilContextId);
}

//
// UpdateAST should be called after RemoveConstructs.
// It checks and overrides all constructs which are already defined in an
// othher file.
//  Origin and AST is updated.
//
void ToolMediator::UpdateAST (int file_id, const TYPE_AS_Definitions & new_ast, Record & Origin, TYPE_AS_Definitions & AST)
{
  Map types_origin (Origin.GetField (OriginTypes));
  Int state_origin (Origin.GetField (OriginState));
  Map values_origin (Origin.GetField (OriginValues));
  Map functions_origin (Origin.GetField (OriginFcts));
  Map operations_origin (Origin.GetField (OriginOps));
  Map traces_origin (Origin.GetField (OriginTraces));

  Generic g;

  MAP<TYPE_AS_Name,TYPE_AS_TypeDef> tm1 (AST.get_typem());
  MAP<TYPE_AS_Name,TYPE_AS_TypeDef> tm2 (new_ast.get_typem());
  for (bool bb = tm2.First (g); bb; bb = tm2.Next (g)) {
    types_origin.ImpModify (g, Int (file_id));
    if (tm1.DomExists (g)) {
      vdm_log << L"  Type: \"" << ASTAUX::ASName2String (g) << L"\" overwrites previous definition." << endl;
    }
  }
  tm1.ImpOverride (tm2);
  AST.set_typem(tm1);

  if (new_ast.get_State().IsRecord ()) {
    state_origin = Int (file_id);
    if (AST.get_State().IsRecord ()) {
      vdm_log << L"  State definition overwrites previous definition." << endl;
    }
    AST.set_State (new_ast.get_State());
  }

  SEQ<TYPE_AS_ValueDef> sq1 (new_ast.get_valuem());
  SEQ<TYPE_AS_ValueDef> sq2 (AST.get_valuem());
  for (bool cc = sq1.First (g); cc; cc = sq1.Next (g)) {
    if (values_origin.DomExists (Record (g).GetField (1))) {
      for (int64_t j = 1; j <= sq2.Length (); j++) {
        if ( Record (sq2[j]).GetField (1) == Record (g).GetField (1)) {
          sq2.RemElem (j);
          j--;
          vdm_log << L"  Value overwrites previous definition." << endl;
        }
      }
    }
    values_origin.ImpModify (Record (g).GetField (1), Int (file_id));
  }
  sq2.ImpConc (sq1);
  AST.set_valuem (sq2);

  MAP<TYPE_AS_Name,TYPE_AS_FnDef> fm1 (AST.get_fnm());
  MAP<TYPE_AS_Name,TYPE_AS_FnDef> fm2 (new_ast.get_fnm());
  for (bool dd = fm2.First (g); dd; dd = fm2.Next (g)) {
    functions_origin.ImpModify (g, Int (file_id));
    if (fm1.DomExists (g)) {
      vdm_log << L"  Function: \"" << ASTAUX::ASName2String (g) << L"\" overwrites previous definition." << endl;
    }
  }
  fm1.ImpOverride (fm2);
  AST.set_fnm (fm1);

  MAP<TYPE_AS_Name,TYPE_AS_OpDef> om1 (AST.get_opm());
  MAP<TYPE_AS_Name,TYPE_AS_OpDef> om2 (new_ast.get_opm());
  for (bool ee = om2.First (g); ee; ee = om2.Next (g)) {
    operations_origin.ImpModify (g, Int (file_id));
    if (om1.DomExists (g)) {
      vdm_log << L"  Operation: \"" << ASTAUX::ASName2String (g) << L"\" overwrites previous definition." << endl;
    }
  }
  om1.ImpOverride (om2);
  AST.set_opm (om1);

  Map trm1 (AST.get_tracem());
  Map trm2 (new_ast.get_tracem());
  for (bool ff = trm2.First (g); ff; ff = trm2.Next (g)) {
    operations_origin.ImpModify (g, Int (file_id));
    if (trm1.DomExists (g)) {
      vdm_log << L"  Trace: \"" << ASTAUX::ASName2String (g) << L"\" overwrites previous definition." << endl;
    }
  }
  trm1.ImpOverride (trm2);
  AST.set_tracem (trm1);

  Origin.SetField (OriginTypes, types_origin);
  Origin.SetField (OriginState, state_origin);
  Origin.SetField (OriginValues, values_origin);
  Origin.SetField (OriginFcts, functions_origin);
  Origin.SetField (OriginOps, operations_origin);
  Origin.SetField (OriginOps, traces_origin);
}
#endif //VDMSL

//
// This function return the set of AST that are
// contained in the FileName
//
Set ToolMediator::GetAstsOfFileName (const TYPE_ProjectTypes_FileName & filename)
{
  SET<TYPE_ProjectTypes_ModuleName> mod_names (Repos()->vdm_ModulesInFile(filename));
  Set asts;
  Generic g;
  for (bool bb = mod_names.First(g); bb; bb = mod_names.Next(g) ) {
    Generic astval (Repos()->vdm_GetAST(g));
    if (astval.IsNil()) {
      // Must never happen
      vdm_err << L"Internal Error " << endl;
      return Set();
    }
    asts.Insert(PTAUX::ExtractAstOrFlat(astval));
  }
  return asts;
}

wstring TOOLS::ReadLine ()
{
  return TBWSTR::cinstr2wstring(TBReadLine::ReadLine(vdm_log));
}

void ToolMediator::PrintAstTuple(ostream & f, const Generic & ast_or_expr, ContextInfo & ci, const Map & fid_m )
{
  Generic g (INT2Q::TransformIntegers(ast_or_expr));
  f << "mk_AS`AstCI(" << endl;
  f << TBWSTR::wstring2string(g.ascii()) << "," << endl;
  ci.pp(f, true);
  f << ", " << endl;

  // Print the fid_m;
  f << "{ ";
  bool first = true;
  Set dom_fid_m (fid_m.Dom());
  Generic dom;
  for (bool bb = dom_fid_m.First(dom); bb; bb = dom_fid_m.Next(dom)) {
    if (first) {
      first = false;
    }
    else {
      f << "," << endl << "  ";
    }
    f << TBWSTR::wstring2string(dom.ascii()) << " |-> " << TBWSTR::wstring2string(fid_m[dom].ascii());
  }

  f << " }" << endl;
  f << ")" << endl << flush;
}

//////////////////////
// Name: ResetTestSuites
// Desc:
//    Resets all test coverage counters in
//    the context information table.
//////////////////////
void ToolMediator::ResetTestCoverage (ContextInfo & ci)
{
  ci.ResetTestCoverageInfo();
}

//////////////////////////////
/// Name: LoadTestCoverageFile
/// Descr: Loads the test coverage information
///        from the test coverage file L"coverage_file"
/// Input:
///   coverage_file: The name of the test coverage file to load coverage
///                  information from.
///   ci           : The context information table that the test coverage information
///                  should be loaded into.
///   file_nms     : A set of file names (short names). This describes the
///                  the name of the files in the test coverage file from
///                  which test coverage information should be loaded into the context
///                  information table.
///   all          : a boolean indicating if all the test coverage information in
///                  the coverage file should be written in.
/// Pre-condition  : (all=true <=> file_nms = {}) and
///                  (all=false <=> file_nms <> {})
//////////////////////////////
void ToolMediator::LoadTestCoverageFile(const wstring & coverage_file, ContextInfo & ci,
                                        const SET< SEQ<Char> > & file_nms, bool all)
{
  if (TBUTILS::file_exists(coverage_file)) {
    Tuple gfmnn (GetFileMapNameNum(file_nms, all));
    const Map & long_names (gfmnn.GetMap(1));
    const Map & short_names (gfmnn.GetMap(2));

    ci.LoadCounters(coverage_file, long_names, short_names, vdm_log, all);
  }
  else {
    vdm_log << L"Couldn't open test coverage file '" << coverage_file << L"' for reading" << endl;
  }
}

void ToolMediator::SaveCoverageCounters (const wstring & coverage_file, ContextInfo & ci)
{
  if (!coverage_file.empty()) {
    // Save coverage counters into coverage_file
    Map m (GetFileMapNumName());
    ci.SaveCounters(coverage_file, m, vdm_err);
  }
}

void ToolMediator::InitCoverageFile(const wstring & coverage_file, ContextInfo & ci)
{
  if (!coverage_file.empty()) {
    // If coverage file exists then read it in so we can accumulate data.
    if (TBUTILS::file_exists(coverage_file)) {
      Tuple gfmnn (GetFileMapNameNum(SET< SEQ<Char> >(), true));
      const Map & long_names (gfmnn.GetMap(1));
      const Map & short_names (gfmnn.GetMap(2));

      ci.LoadCounters(coverage_file, long_names, short_names, vdm_log, true);
    }
  }
}

// Create maps (map seq of wchar_t to int)
// GetFileMapNameNum
// 
Tuple ToolMediator::GetFileMapNameNum(const SET< SEQ<Char> > & file_nms, bool all)
{
  SET<TYPE_ProjectTypes_FileName> files (Repos()->vdm_Files());
  MAP< SEQ<Char>,TYPE_ProjectTypes_FileId > long_map, short_map;
  Generic fnm;
  for (bool bb = files.First(fnm); bb; bb = files.Next(fnm)) {
    wstring long_name (PTAUX::ExtractFileName(fnm));
    SEQ<Char> short_name (TBUTILS::tb_getbasename(long_name));
    TYPE_ProjectTypes_FileId fid (Repos()->vdm_GetFileId(fnm));
    if (fid.GetValue() == 0) {
      vdm_err << L"Failed to lookup file name " << long_name << endl;
    }
    else {
      if ( all || file_nms.InSet(short_name) ) {
        long_map.Insert(SEQ<Char>(long_name), fid);
        // Use ImpModify as some short_name's may be equal.
        short_map.ImpModify(short_name, fid);
      }
    }
  }
  return mk_(long_map, short_map);
}

// Create map (map int to seq of wchar_t)
MAP<TYPE_ProjectTypes_FileId, SEQ<Char> > ToolMediator::GetFileMapNumName()
{
  SET<TYPE_ProjectTypes_FileName> files (Repos()->vdm_Files());
  MAP<TYPE_ProjectTypes_FileId, SEQ<Char> > long_map;
  Generic file;
  for (bool bb = files.First(file); bb; bb = files.Next(file)) {
    SEQ<Char> long_name (PTAUX::ExtractFileName(file));
    TYPE_ProjectTypes_FileId fid (Repos()->vdm_GetFileId(file));
    if (fid.GetValue() == 0) {
      vdm_err << L"Failed to lookup file name: " << long_name.GetString() << endl;
      continue;
    }
    long_map.Insert(fid, long_name);
  }
  return long_map;
}

#ifdef VDMSL
void TOOLS::EvalModuleStack (wostream & wos)
{
  if (ToolMediator::Repos()->vdm_IsSession (struct_session)) {
    TBDEBUG::EvalModStack(wos);
  }
  else if (ToolMediator::Repos()->vdm_IsSession (flat_session)) {
    wos << L"No module stack for definitions" << endl;
  }
  else {
    wos << L"No specification present" << endl;
  }
}
#endif //VDMSL

void TOOLS::EvalBreak(const wstring & args, wostream & wos)
{
  bool initialized = theStackMachine().IsInitialized();

  if (!initialized) {
    if (ToolMediator::Repos()->vdm_IsSession (none_session)) {
      wos << L"No specification present" << endl << flush;
      return;
    }
    Bool res(TBDEBUG::InitCurrentDefinition (false, vdm_iplog));
    if (!res) {
      return;
    }
  }

  wstring bpkts[20];
  int nos = STR_split (args, bpkts, 20, STR_RXwhite_and_comma);

  switch(nos) {
    case 0: {
      TBDEBUG::PrintBreakpoints(wos);
      break;
    }
    case 1: {
      TBDEBUG::EvalBreakOnName(bpkts[0], wos);
      break;
    }
    case 2: {
      TBDEBUG::EvalBreakOnPos(bpkts[0], bpkts[1], wstring(L"1"), wos);
      break;
    }
    case 3: {
      TBDEBUG::EvalBreakOnPos(bpkts[0], bpkts[1], bpkts[2], wos);
      break;
    }
    default: {
      vdm_iplog << L"Invalid use of break!" << endl;
      TBDEBUG::PrintBreakSyntax(wos);
    }
  }
}

// This function invokes the Tcl/Tk CallBack function with super, sub,
// uses and usedby classes for the dependency tool.
#ifdef VDMPP
Bool ToolMediator::EvalDependInfo (const TYPE_ProjectTypes_ModuleName & nm)
{
  UpdateSelections();
  SET<TYPE_ProjectTypes_ModuleName> supers (Repos()->vdm_GetSuper(nm));
  SET<TYPE_ProjectTypes_ModuleName> subs (Repos()->vdm_GetSub(nm));
  SET<TYPE_ProjectTypes_ModuleName> uses (Repos()->vdm_GetUses(nm));
  SET<TYPE_ProjectTypes_ModuleName> usedby (Repos()->vdm_GetUsedBy(nm));
  // Must be updated such that also incomplete information is added.
  // For now complete is assumed:
  Bool complete(Repos()->vdm_IsAllTypeCorrect());

  Interf()->vdm_CallBack(PTAUX::mk_DependInfo(nm, supers, subs, uses, usedby, complete));

  return Bool(true);
}

Bool ToolMediator::EvalDependUpdate (const SET<TYPE_ProjectTypes_ModuleName> & modules)
{
  Interf()->vdm_CallBack(PTAUX::mk_DependUpdate(modules));
  return Bool(true);
}
#endif // VDMPP

// This function invokes the Tcl/Tk CallBack function in order to
// update the selection of modules/classes and files in the GUI.

Bool ToolMediator::UpdateSelections ()
{
  Interf()->vdm_CallBack(PTAUX::mk_UpdateSelections());
  return Bool(true);
}

// A specification has succesfully been parsed.
// Update the internal representation of the Toolbox.
// This should be done by the Repository.

bool ToolMediator::UpdateRepository (const TYPE_ProjectTypes_FileName & filename,
                                     int file_id,
                                     const Record & new_ast,
                                     const Sequence & astseq,
                                     bool isJava)
{
#ifdef VDMSL
  Quote session = (new_ast.Is (TAG_TYPE_AS_Definitions)) ? flat_session : struct_session;
#endif //VDMSL
#ifdef VDMPP
  Quote session = struct_session;
#endif //VDMPP

  bool init_flat = false;

  // If the current session is disabled this means that we have
  // the possiblity of changing the session type
  if (Repos ()->vdm_IsSession (disabled_session)) {
    if (Repos ()->vdm_OldSession () != session) {
      // Changing session type in current project
      // Remember files in project before clearing
      SET<TYPE_ProjectTypes_FileName> allfiles_s (Repos ()->vdm_Files ());
      Repos ()->vdm_ClearAll ();

      // And insert files again in empty project
      AddFiles(allfiles_s);
    }
    else
      Repos ()->vdm_EnableSession ();
  }

  if (Repos()->vdm_IsSession (none_session)) {
    if (session == flat_session) {
      init_flat = true;
    }
  }
  else if (!Repos()->vdm_IsSession (session)) {
    vdm_log << L"Cannot parse modules and definitions in one session" << endl << flush;
    if ((Repos ()->vdm_Files ()).InSet (filename)) {
      SET<TYPE_ProjectTypes_FileName> filenames;
      filenames.Insert(filename);
      RemoveFiles (filenames);
      AddFiles (filenames);
    }
    return false;
  }

  // Update the internal state of the base tools and the repository

#ifdef VDMSL
  // A flat specification was parsed.
  if(session == flat_session) {
    TYPE_AS_Definitions ast;
    Record Origin;

    if (init_flat) {
      // Empty origion of active constructs
      Origin = EmptyOrigin ();

      // Insert an empty definitions block in ast
      ast = EmptyDefinitions ();
    }
    else {
      // Retrieve Origin and ast from the repository (DefaultMod)
      Tuple efs (PTAUX::ExtractFlatSpec(Repos()->vdm_GetFlatSpec()));
      const TYPE_AS_Module & mod (efs.GetRecord(1));
      Tuple t (RemoveConstructs (file_id, efs.GetRecord(2), mod.GetRecord(pos_AS_Module_defs)));
      Origin = t.GetRecord(1);
      ast = t.GetRecord(2);
    }

    UpdateAST (file_id, (const TYPE_AS_Definitions &)new_ast, Origin, ast);

    // create module for flat spec
    TYPE_AS_Module new_mod (mk_DefaultMod (ast));

    TYPE_ProjectTypes_FlatSpec flatspec (PTAUX::mk_FlatSpec (new_mod.get_nm (), new_mod, Origin));
    UpdateSC (filename, flatspec);

    // The current initialised specification is outdated with respect to the Repository.
    TOOLS::set_spec_init(false);
  }
  else
#endif //VDMSL
  {
    // A sequence of modules was parsed.
#ifdef VDMSL
    Sequence ast_l;
    size_t len_astseq = astseq.Length();
    for (size_t idx = 1; idx <= len_astseq; idx++) {
      const Record & mod_ast (astseq[idx]);
      switch(mod_ast.GetTag()) {
        case TAG_TYPE_AS_Module: {
          const TYPE_AS_Name & nm (mod_ast.GetRecord(pos_AS_Module_nm));
          ast_l.ImpAppend (PTAUX::mk_Module (nm, mod_ast));
          break;
        }
        case TAG_TYPE_AS_DLModule: {
          const TYPE_AS_Name & nm (mod_ast.GetRecord(pos_AS_DLModule_nm));
          ast_l.ImpAppend (PTAUX::mk_Module (nm, mod_ast));
          break;
        }
      }
      TOOLS::UpdateSS (mod_ast);
    }
#endif // VDMSL
#ifdef VDMPP
    Sequence ast_l;
    if (isJava) {
      size_t len_astseq = astseq.Length();
      for (size_t idx = 1; idx <= len_astseq; idx++) {
        const TYPE_CPP_Module & mod_ast (astseq[idx]);
        const TYPE_AS_Name & nm (mod_ast.GetRecord(pos_CPP_Module_name));
        ast_l.ImpAppend (PTAUX::mk_Module (nm, mod_ast));
        JavaGenTools::UpdateJSS (mod_ast);
      }
    }
    else {
      size_t len_astseq = astseq.Length();
      for (size_t idx = 1; idx <= len_astseq; idx++) {
        const TYPE_AS_Class & mod_ast (astseq[idx]);
        const TYPE_AS_Name & nm (mod_ast.GetRecord(pos_AS_Class_nm));
        ast_l.ImpAppend (PTAUX::mk_Module (nm, mod_ast));
        TOOLS::UpdateSS (mod_ast);
      }
    }
#endif //VDMPP

    UpdateSC (filename, ast_l);

    // The current initialised specification is
    // outdated with respect to the Repository.
    TOOLS::set_spec_init (false);

#ifdef VDMPP
    TOOLS::CheckSSParseEnv(); // 20060614 for change of class name(s)
    TOOLS::set_dobjs_init(false);
#endif //VDMPP
  }
  return true;
}

///////////////////////////////////////////
// Name : EvalRead
// Descr:
// reads in and parses a specification file (filename).
///////////////////////////////////////////

// EvalRead
// filename : name of file to parse
bool TOOLS::EvalRead (const TYPE_ProjectTypes_FileName & filename)
{
  ////////////////////
  // Check whether syntax checking is allowed
  ////////////////////

  Tuple allowres (ToolMediator::Repos()->vdm_Allowed (filename, action_syntax));
  if (!allowres.GetBoolValue(1)) {
    const SEQ<Char> & mes (allowres.GetSequence (2));
    if (mes.IsEmpty()) {
      vdm_log << L"Syntax checking is not allowed" << endl << flush;
    }
    else {
      vdm_log << PTAUX::Seq2Str (mes) << endl << flush;
    }
    return false;
  }

  //////////////////
  // Create object to hold file
  //////////////////

  SpecFile spec_file (filename, GetCI());  // The specification file

  ///////////////
  // Init spec file
  //////////////

  if ( ! spec_file.init() ) {
    vdm_log << L"Couldn't open file '" << spec_file.get_fname() << L"'" << endl << flush;
    //ToolMediator::UpdateSC (filename, eseq);
    return false;
  }
  // Parse the VDM specification

  vdm_log << L"Parsing \"" << spec_file.get_fname() << L"\" (" << spec_file.get_format_name() << L") ... " << flush;
  Sequence asts;   // The resulting ast from parsing the spec
  bool parse_ok = spec_file.parse_vdm(asts);
  spec_file.setTimestamp();

  ToolMediator::Errs()->vdm_Done();

  if (!parse_ok) {
    ToolMediator::UpdateSC (filename, Sequence());
    vdm_log << L"done (Errors detected)" << endl << flush;
    return false;
  }
  else {
    Record new_ast (asts.Hd ());
    bool ok = ToolMediator::UpdateRepository (filename, spec_file.get_file_id(), new_ast, asts, false);
    vdm_log << L"done" << endl << flush;
    return ok;
  }
}

/**************************************************************************/
/****       parser part                                                ****/
/**************************************************************************/

////////////////////////////
// NAME : EvalLatex
// PARAMS:
//      Record filename      - name of the file to pretty print
//
////////////////////////////

bool TOOLS::EvalLatex (const TYPE_ProjectTypes_FileName & filename)
{
  /////////////////////////////////////
  // check whether pretty printing is allowed
  /////////////////////////////////////

  Tuple allowres (ToolMediator::Repos()->vdm_Allowed (filename, action_pp));
  if (!allowres.GetBoolValue(1)) {
    const SEQ<Char> & mes (allowres.GetSequence (2));
    if (mes.IsEmpty()) {
      vdm_log << L"Pretty printing not allowed" << endl << flush;
    }
    else {
      vdm_log << PTAUX::Seq2Str(mes) << endl << flush;
    }
    return false;
  }

  /////////////////////////////////////
  // Prepare the SpecFile
  /////////////////////////////////////
  SpecFile spec_file (filename, GetCI());  // The specification file

  if ( ! spec_file.init() ) {
    vdm_log << L"Couldn't open file '" << spec_file.get_fname() << L"' " << flush;
    ToolMediator::UpdatePP (filename, Bool(false));
    return false;
  }

  vdm_log << L"Generating " << spec_file.get_format_name() << L" ... " << endl << flush;

  ///////////////////////
  // Generate Latex
  ///////////////////////
  bool ppres = spec_file.pp();
  if ( ! ppres) {
    vdm_log << endl << L"Unable to pretty print specification file '"
            << spec_file.get_fname() << L"'" << endl << flush;
    ToolMediator::UpdatePP (spec_file.get_filename(), Bool(false));
    return false;
  }
  vdm_log << L"done" << endl << flush;

  //////////////////////////
  // UPDATE THE GUI
  //////////////////////////

  vdm_log << spec_file.get_format_name() << L" generated to file "
          << spec_file.get_pp_output_name() << endl << flush;

  ToolMediator::Errs()->vdm_Done();
  ToolMediator::UpdatePP (spec_file.get_filename(), Bool (true));

  return true;
}

#ifdef VDMPP
bool TOOLS::EvalJavaParse(const TYPE_ProjectTypes_FileName & filenm)
{
  return JavaGenTools::EvalJavaParse (filenm);
}

bool TOOLS::EvalJavaTypeCheck(const TYPE_ProjectTypes_ModuleName & mnm)
{
  return JavaGenTools::EvalJavaTypeCheck (mnm);
}

bool TOOLS::EvalJavaGenerateVDM(const TYPE_ProjectTypes_ModuleName & mnm,
                                const Bool & p_stubs, const Bool & p_rename, const Bool & p_trans)
{
  return JavaGenTools::EvalJavaGenerateVDM (mnm, p_stubs, p_rename, p_trans);
}

void TOOLS::AfterJavaTypeCheck()
{
  JavaGenTools::AfterJavaTypeCheck();
}
#endif // VDMPP

//////////////////////////////////////////////////////////////////////////
/////////// Evaluating Inheritance Tree  /////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifdef VDMPP
bool ToolMediator::EvalInhTree()
{
/*
  if (!Repos()->vdm_IsSession(none_session))
  {
    Set class_s = Repos()->vdm_Modules(); // class_s: set of ModuleName
    Map inh_strct;
    Generic cl;
    for (bool bb = class_s.First(cl); bb; bb = class_s.Next(cl))
    {
      Set subs(Repos()->vdm_GetSub(cl)); //subs: set of ModuleName

      //// Convert subs from set of ModuleName to set of Tokens.
      Set my_subs; // my_subs: set of Token
      Generic my_cl_g;
      for (int dd = subs.First(my_cl_g); dd; dd = subs.Next(my_cl_g))
      {
        Token my_cl (ExtractModuleName(my_cl_g));
        my_subs.Insert(my_cl);
      }

      Token my_cl (ExtractModuleName(cl_g));
      inh_strct.Insert(my_cl, my_subs);
    }
    TkTreeWin tree(inh_strct);
    Sequence graph_tree (tree.Layout()); // graph_tree: set of (Node | Line) (see ptypes.vpp)
    Interf()->vdm_CallBack(mk_DrawInhTree(graph_tree));
  }
  else {
    // Clear Window (empty inheritance tree)
    Interf()->vdm_CallBack(mk_ClearInhTree());
  }
*/
  return true;
}
#endif // VDMPP

// GetModulePosInfo
// modnm : ProjectTypes`ModuleName
// ==> bool * int * int * int
Tuple ToolMediator::GetModulePosInfo(const TYPE_ProjectTypes_ModuleName & modnm)
{
  Generic astval (Repos()->vdm_GetAST(modnm));
  if ( !astval.IsNil() ) {
    Record ast (PTAUX::ExtractAstOrFlat(astval));
#ifdef VDMSL
    if (ast.Is(TAG_TYPE_AS_Module)) {
      const TYPE_AS_Name & nm (ast.GetRecord(pos_AS_Module_nm));
#endif // VDMSL
#ifdef VDMPP
    if (ast.Is(TAG_TYPE_AS_Class)) {
      const TYPE_AS_Name & nm (ast.GetRecord(pos_AS_Class_nm));
#endif // VDMPP
      return CidToPosInfo(nm.get_cid());
    }
  }
  return mk_(Bool(false), Int(1), Int(1), Int(1));
}

// CidToPosInfo
// cid : CI`ContextId
// ==> bool * int * int * int
Tuple ToolMediator::CidToPosInfo(const TYPE_CI_ContextId & cid)
{
  if (cid != NilContextId) {
    Tuple gfp (GetCI().GetFilePos(cid));
    if (gfp.GetBoolValue(1)) {
      const TYPE_CI_TokenPos & spos (gfp.GetRecord(4));
      const TYPE_CI_TokenPos & epos (gfp.GetRecord(5));
      const TYPE_CI_Line & sl   (spos.GetInt(pos_CI_TokenPos_abs_uline));
      const TYPE_CI_Column & sc (spos.GetInt(pos_CI_TokenPos_column));
      const TYPE_CI_Line & el   (epos.GetInt(pos_CI_TokenPos_abs_uline));
      const TYPE_CI_Column & ec (epos.GetInt(pos_CI_TokenPos_column));

      int len = ((sl == el) ? ec.GetValue() - sc.GetValue() : 1);

      return mk_(Bool(true), sl, sc, Int(len));
    }
  }
  return mk_(Bool(false), Int(1), Int(1), Int(1));
}

Tuple ToolMediator::GetFnOpPosInfo(const wstring & fnopnm)
{
  return TBDEBUG::GetFnOpPosInfo(fnopnm, vdm_iplog); 
}

Bool ToolMediator::NewUnnamedProject ()
{
  Bool res (Repos()->vdm_NewUnnamedProject());
  return res;
}

Bool ToolMediator::Open (const TYPE_ProjectTypes_FileName & pnm)
{
  // dont'lock
  Bool res (Repos()->vdm_Open(pnm));
  return res;
}

Bool ToolMediator::SaveAs (const TYPE_ProjectTypes_FileName & pnm)
{
  Bool res (Repos()->vdm_SaveAs(pnm));
  return res;
}

Generic ToolMediator::GetProjectName ()
{
  Generic res (Repos()->vdm_GetProjectName());
  return res;
}

void ToolMediator::ClearAll ()
{
  Repos()->vdm_ClearAll();
}

void ToolMediator::AddFiles (const SET<TYPE_ProjectTypes_FileName> & filenames)
{
  Repos ()->vdm_AddFiles(filenames);
}

void ToolMediator::RemoveFiles (const SET<TYPE_ProjectTypes_FileName> & filenames)
{
  Repos ()->vdm_RemoveFiles (filenames);
}

void ToolMediator::UpdateSC (const TYPE_ProjectTypes_FileName & filename, const Generic & spec)
{
  Repos()->vdm_UpdateSC (filename, spec);
}

void ToolMediator::UpdateTC (const TYPE_ProjectTypes_ModuleName & modnm, const Generic & tc_type)
{
  Repos()->vdm_UpdateTC (modnm, tc_type);
}

void ToolMediator::UpdateCG (const TYPE_ProjectTypes_ModuleName & modnm, const Generic & kind,
                             const TYPE_ProjectTypes_Succes & suc)
{
  Repos()->vdm_UpdateCG (modnm, kind, suc);
}

void ToolMediator::UpdatePP (const TYPE_ProjectTypes_FileName & filename, const TYPE_ProjectTypes_Succes & suc)
{
  Repos()->vdm_UpdatePP (filename, suc);
}

TYPE_ProjectTypes_FileId ToolMediator::GetFileId (const TYPE_ProjectTypes_FileName & filename)
{
  TYPE_ProjectTypes_FileId res (Repos()->vdm_GetFileId(filename));
  return res;
}

TYPE_ProjectTypes_FileName ToolMediator::GetVDMTempFileName(const TYPE_ProjectTypes_FileName & fnm)
{
  TYPE_ProjectTypes_FileName res (Repos()->vdm_GetTempFileName (fnm));
  return res;
}

Tuple ToolMediator::Status (const Record & mname)
{
  Tuple res (Repos()->vdm_Status(mname));
  return res;
}

Generic ToolMediator::GetAST (const TYPE_ProjectTypes_ModuleName & modnm)
{
  Generic res (Repos()->vdm_GetAST(modnm));
  return res;
}

SET<TYPE_ProjectTypes_FileName> ToolMediator::Files ()
{
  SET<TYPE_ProjectTypes_FileName> res (Repos()->vdm_Files());
  return res;
}

SET<TYPE_ProjectTypes_FileName> ToolMediator::ParsedFiles ()
{
  SET<TYPE_ProjectTypes_FileName> res (Repos()->vdm_ParsedFiles());
  return res;
}

SET<TYPE_ProjectTypes_ModuleName> ToolMediator::AllModules()
{
  SET<TYPE_ProjectTypes_ModuleName> res (Repos()->vdm_AllModules());
  return res;
}

SET<TYPE_ProjectTypes_ModuleName> ToolMediator::VDMModules()
{
  SET<TYPE_ProjectTypes_ModuleName> res (Repos()->vdm_VDMModules());
  return res;
}

SET<TYPE_ProjectTypes_FileName> ToolMediator::FileOfModule (const TYPE_ProjectTypes_ModuleName & modnm)
{
  SET<TYPE_ProjectTypes_FileName> res (Repos()->vdm_FileOfModule(modnm));
  return res;
}

SET<TYPE_ProjectTypes_ModuleName> ToolMediator::ModulesInFile (const TYPE_ProjectTypes_FileName & filename)
{
  SET<TYPE_ProjectTypes_ModuleName> res (Repos()->vdm_ModulesInFile(filename));
  return res;
}

Map ToolMediator::GetFilesAndPackages (const Char & c)
{
  Map res (Repos()->vdm_GetFilesAndPackages(c));
  return res;
}

TYPE_ProjectTypes_FileName ToolMediator::GetPackageOfFile (const TYPE_ProjectTypes_FileName & filename, const Char & c)
{
  TYPE_ProjectTypes_FileName res (Repos()->vdm_GetPackageOfFile(filename, c));
  return res;
}

void ToolMediator::SetFileTimestamp (const TYPE_ProjectTypes_FileName & filename, const Int & ts)
{
  Repos ()->vdm_SetFileTimestamp (filename, ts);
}

Bool ToolMediator::StateIsSaved ()
{
  Bool res (Repos()->vdm_StateIsSaved());
  return res;
}

void ToolMediator::SetSavedFileState (const Bool & saved)
{
  Repos()->vdm_SetSavedFileState(saved);
}

void ToolMediator::EnableSession ()
{
  Repos ()->vdm_EnableSession ();
}

void ToolMediator::DisableSession ()
{
  Repos ()->vdm_DisableSession ();
}

bool ToolMediator::SyntaxCheckOk (const TYPE_ProjectTypes_FileName & filename)
{
  return Repos()->vdm_IsSyntaxCorrect(filename).GetValue();
}

void ToolMediator::SetOldReverseEnable(bool b) {
  if (b && !Settings.OldReverse()) {
    Settings.OldReverseOn();
    //TOOLS::set_spec_init(false);
    TBDEBUG::InitCurrentDefinition (true, vdm_iplog);
  }
  else if (!b && Settings.OldReverse()) {
    Settings.OldReverseOff();
    //TOOLS::set_spec_init(false);
    TBDEBUG::InitCurrentDefinition (true, vdm_iplog);
  }
}

// ParseExprsString
// str : seq of char
// ==> bool * seq os AS`Expr
Tuple TOOLS::ParseExprsString (const string & str)
{
  if (!isinscript) {
    ToolMediator::ExprErrs()->vdm_ClearAll();
  } 
  Sequence res_exprs;
  bool parseError = VDMEXPRPARSER::pars_vdm_expr_string(str,
                                                        true,
                                                        ToolMediator::ExprErrs(),
                                                        res_exprs,
                                                        GetCI(),
                                                        false,
                                                        TYPE_CI_FileId(CI_FID_CMD_LINE));
  if (!isinscript) {
    ToolMediator::ExprErrs()->vdm_AllDone();
  }
  return mk_(Bool(!parseError), res_exprs);
}

#ifdef VDMPP
// ParseAssignStmt
// str : seq of char
// ==> bool * seq os AS`Expr
Tuple TOOLS::ParseAssignStmt (const string & str)
{
  TYPE_AS_AssignStmt stmt;
  bool parse_error = VDMSTMTPARSER::pars_vpp_asgn_invoke_stmt (str, true, ToolMediator::ExprErrs(), stmt);
  return mk_(Bool(!parse_error), stmt);
}
#endif // VDMPP

/**************************************************************************/
/* The following Eval... functions all more or less corresponds to one of */
/* the commands which is supported by vdmde e.g. 'print', 'debug' etc.    */
/* The functions are therefore mainly called from the ParseCommand        */
/* function                                                               */
/**************************************************************************/

// ParseExprs
// expr : ProjectTypes`ToolCommand | ProjectTypes`FileName
// ==> bool * seq os AS`Expr
Tuple TOOLS::ParseExprs (const Record & expr, wostream & wos)
{
  bool not_initialized = false;
  if (TOOLS::get_ast_is_new() && (!ToolMediator::Repos()->vdm_IsSession (none_session))) {
    vdm_iplog << L"Note: specification has changed since last 'init'" << endl;
    //spec_init = true; // warning is now printed
    not_initialized = true; // 20060310
  }
#ifdef VDMPP
//  else if (!TOOLS::get_dobjs_init()) {
  else if (!TOOLS::get_dobjs_init() && !ToolMediator::Repos()->vdm_Files().IsEmpty()) {
    vdm_iplog << L"Warning: specification has changed since last 'init' of user defined objects" << endl << flush;
    TOOLS::set_dobjs_init(true);  // warning is now printed
    not_initialized = true; // 20060310
  }
#endif //VDMPP

  // 20060310
  if( not_initialized ) {
    SET<TYPE_ProjectTypes_FileName> files(ToolMediator::Repos()->vdm_Files());
    if( !files.IsEmpty() ) {
      Bool res (TBDEBUG::InitCurrentDefinition (false, vdm_iplog));
      if (!res) {
        return mk_(Bool(false), Sequence());
      }
    }
  }

  try {
    if (PTAUX::is_ToolCommand (expr)) {
      wstring input (PTAUX::ExtractToolCommand (TYPE_ProjectTypes_ToolCommand(expr)));
      //    source_files.ImpModify (Int (-10), Token (L""));
      return TOOLS::ParseExprsString(TBWSTR::wstring2string(input));
    }
    else {
      wstring filenm (PTAUX::ExtractFileName (TYPE_ProjectTypes_FileName(expr)));
      //    source_files.ImpModify (Int (-10), Token (filenm));
      ifstream istr (TBWSTR::wstring2fsstr(filenm).c_str());
      if (!istr) {
        vdm_err << L"Couldn't open argument file '" << filenm << L"'" << endl;
        return mk_(Bool(false), Sequence());
      }
      bool parseError = false;
      Sequence res_exprs;
      ostringstream unichar;
      if(ifstr2ustr( istr, unichar )) {
        if (!TOOLS::IsInScript()) {
          ToolMediator::ExprErrs()->vdm_ClearAll();
        }

        parseError = VDMEXPRPARSER::pars_vdm_expr_string(unichar.str(),
                                                         true,
                                                         ToolMediator::ExprErrs(),
                                                         res_exprs,
                                                         GetCI(),
                                                         true,
                                                         TYPE_CI_FileId(CI_FID_TMP_FILE));
        if (!TOOLS::IsInScript()) {
          ToolMediator::ExprErrs()->vdm_AllDone();
        }
      }
      else {
        vdm_err << L"Invalid multibyte string encounted" << endl;
        parseError = true;
      }
      istr.close();
      return mk_(Bool(!parseError), res_exprs);
    }
  }
  catch (TB_Exception & e)  {
    switch (e.GetExType()) {
    default:
      // It must not happen
      vdm_err << L"Internal Error" << endl;
      break;
    }
  }
  return mk_(Bool(false), Sequence());
}

int TOOLS::OutputStatSemErrors(const Generic & modnm)
{
  SEQ<TYPE_ProjectTypes_Message> err_l (PTAUX::QS (GetStatSem().GetErrors(), Settings.SEP()));

  int noOfErr = 0;
  int len_err_l = err_l.Length();
  for (int index = 1; index <= len_err_l; index++) {
    const TYPE_ProjectTypes_Message & mes (err_l[index]);
    if (!ToolMediator::Errs()->vdm_GetErrors().Elems().InSet(mes)) {
      ToolMediator::Errs()->vdm_AddMsg(mes);
      if (PTAUX::is_ErrMsg(mes)) {
        noOfErr++;
      }
    }
  }
  ToolMediator::Errs()->vdm_Done();
  return noOfErr;
}

// InitStatSem
#ifdef VDMSL
// ast_val : AS`Module | AS`DLModule
#endif // VDMSL
#ifdef VDMPP
// ast_val : AS`Class
#endif // VDMPP
Tuple TOOLS::InitStatSem(const Record & ast_val)
{
  GetStatSem().InitEnv();
#ifdef VDMSL
  (void) GetStatSem().ExpandModule (POS, ast_val);
  return GetStatSem().GetTCState ();
#endif // VDMSL
#ifdef VDMPP
  return Tuple();
#endif //VDMPP
}

// LocalEvalTypeCheck
// modnm : ProjectTypes`ModuleName
// opt : 
bool TOOLS::LocalEvalTypeCheck (const TYPE_ProjectTypes_ModuleName & modnm, int opt, wostream & wos)
{
  Tuple allowres (ToolMediator::Repos()->vdm_Allowed (modnm, action_type));
  if (!allowres.GetBoolValue(1)) {
    const SEQ<Char> & mes (allowres.GetSequence (2));
    if (mes.IsEmpty()) {
      wos << L"Type checking '" + PTAUX::ExtractModuleName(modnm) + L"' is not allowed" << endl << flush;
    }
    else {
      wos << PTAUX::Seq2Str (mes) << endl << flush;
    }
    ToolMediator::UpdateTC (modnm, Nil());
    return false;
  }

  wos << L"Type checking ";
  wstring mnm (PTAUX::ExtractModuleName (modnm));
#ifdef VDMSL
  if (mnm != ASTAUX::GetDefaultModName()) {
#endif // VDMSL
    wos << mnm << L" ";
#ifdef VDMSL
  }
#endif // VDMSL
  wos << L"... " << flush;

#ifndef NOSS
  GetStatSem().ResetErrors();
  GetStatSem().InitEnv ();
#endif // NOSS

  // UpdateSS should be there, otherwise some TC errors are never reported.
  // But I could not imagine all consequences of calling UpdateSS here.
  //  UpdateSS (AstVal.GetField(1));

  if (opt != POS && opt != DEF) {
    // Extract the POS/DEF from the options class
    opt = Settings.TypeCheck();
  }
  // Get ast of the module and the module global module environment
  TYPE_ProjectTypes_AstVal AstVal (ToolMediator::Repos()->vdm_GetAST (modnm));
  Record ast (PTAUX::ExtractAstOrFlat (AstVal));
  // HC: It is not safe to use wf_ok as an indication
  // of the result of the type check.
  // We use - untill the type checker is fully tested -
  // the length of the error messages to check if the
  // type errors was found during type checking.!!!!

  try {
#ifdef VDMSL
    (void) (GetStatSem().wf_Module(opt, ast));
#endif //VDMSL
#if VDMPP && !defined(NOSS)
    (void) (GetStatSem().wf_Class(opt, ast));
#endif //VDMPP
  }
  catch (TB_Exception &e) {
    vdm_err << L"Please report this error." << endl << flush;
    wos << L"done (Errors detected)" << endl << flush;
    ToolMediator::UpdateTC (modnm, Nil());
    return false;
  }

  // Setting_SEP should be extracted from the Options class
  int noOfErr = OutputStatSemErrors(modnm); // 

  if (noOfErr == 0) {
    wos << L"done" << endl << flush;

    Quote tc_type ((opt == POS) ? tc_pos : tc_def);

    // Remark vdm_UpdateTC should be changed only to update the status and not a modified ast.
    ToolMediator::UpdateTC(modnm, tc_type);
    return true;
  }
  else {
    wos << L"done (Errors detected)" << endl << flush;
    ToolMediator::UpdateTC (modnm, Nil());
    return false;
  }
}

void TOOLS::api_Periodic()
{
#ifdef CORBA_API
  if ( Settings.UsesCorba() ) {
    APICommandQueue::GetInstance ()->ExecuteQueuedCommand ();
  }
#endif // CORBA_API
}

bool TOOLS::EvalTypeCheck (const TYPE_ProjectTypes_ModuleName& modnm, int opt, wostream & wos)
{
#ifdef VDMPP
  if (ToolMediator::CheckModified()) {
    ToolMediator::ResetTCStatus();
  }
#endif // VDMPP

  wstring mnm (PTAUX::ExtractModuleName(modnm));
  if (mnm == L"*") {
    bool res = true;
    SET<TYPE_ProjectTypes_ModuleName> nms (PTAUX::ASNameSet2ModuleNameSet(ToolMediator::GetAllVDMModuleNames()));
    Generic nm;
    for (bool bb = nms.First(nm); bb; bb = nms.Next(nm)) {
#ifdef VDMSL
       res = res & LocalEvalTypeCheck (nm, opt, wos);
#endif // VDMSL
#ifdef VDMPP
       res = res & EvalProcessCmd (nm, opt, wos);
#endif // VDMPP
    }
    return res;
  }
  else
  {
#ifdef VDMSL
    return LocalEvalTypeCheck (modnm, opt, wos);
#endif //VDMSL
#ifdef VDMPP
    return EvalProcessCmd (modnm, opt, wos);
#endif //VDMPP
  }
}

void ToolMediator::ResetTCStatus()
{
  SET<TYPE_ProjectTypes_ModuleName> mods (Repos()->vdm_AllModules());
  Generic modnm;
  for(bool bb = mods.First(modnm); bb; bb = mods.Next(modnm)) {
    UpdateTC(modnm, Quote (L"NONE"));
  }
}

bool ToolMediator::CheckModified()
{
  SET<TYPE_ProjectTypes_FileName> files (Repos()->vdm_Files());
  bool exists = false;
  Generic filenm;
  for(bool bb = files.First(filenm); bb && !exists; bb = files.Next(filenm)) {
    exists = IsFileModified(filenm);
  }
  return exists;
}

////////////////////////////////////
/// Name: GetVDMModuleNames
/// Description: Extract the module names of VDM modules from the
//               specification manager and returns them as AS`Name
////////////////////////////////////

SET<TYPE_AS_Name> ToolMediator::GetAllVDMModuleNames()
{
  SET<TYPE_AS_Name> mod_as_nms;
  SET<TYPE_ProjectTypes_ModuleName> mod_s (Repos()->vdm_VDMModules());
  Generic modnm;
  for (bool bb = mod_s.First(modnm); bb; bb = mod_s.Next(modnm)) {
    mod_as_nms.Insert(PTAUX::ModuleName2ASName(modnm));
  }
  return mod_as_nms;
}

#ifdef VICE
void TOOLS::OutputTimemap(wostream & os, const Map & tm)
{
  // Output a timemap to an outputstream neatly

  // Recall that
  //   tm : map TIMEPARSER`Instruction * [REP`BasicTypeRep] to nat
  // so we decompose each maplet accordingly
  Set dom_tm (tm.Dom());
  Generic gkey;
  for (bool ii = dom_tm.First(gkey); ii; ii = dom_tm.Next(gkey)) {
    Generic gval (tm[gkey]);
    Int tval(gval);
    Tuple key(gkey);
    TYPE_TIMEPARSER_Instruction instr(key.GetField(1));
    Generic tprep(key.GetField(2));
    Sequence instrseq (vdm_TIMEPARSER_InstrMap(instr));
    wstring instrstr;
    instrseq.GetString(instrstr);
//    wstring instrstr (InstrMap(instr));

    // Use tabs to line up the output
    os << instrstr << L"\t";
    if (instrstr.length() < 16) {
      os << L"\t";
    }
    if (instrstr.length() < 8) {
      os << L"\t";
    }
    // The type field may be nil
    if (tprep.IsNil()) {
      os << L"nil";
    }
    else {
      TYPE_REP_BasicTypeRep btr (tprep);
      switch (btr.GetTag()) {
        case TAG_TYPE_REP_BooleanTypeRep: { os << L": bool"; break; }
        case TAG_TYPE_REP_NumericTypeRep: {
          switch (btr.GetIntValue(pos_REP_NumericTypeRep_qtp)) {
            case TAG_quote_NATONE:  { os << L": nat1"; break;}
            case TAG_quote_NAT :    { os << L": nat"; break;}
            case TAG_quote_INTEGER: { os << L": int"; break;}
            case TAG_quote_RAT:     { os << L": rat"; break;}
            case TAG_quote_REAL:    { os << L": real"; break;}
          } 
          break;
        }
        case TAG_TYPE_REP_TokenTypeRep: { os << L": token"; break; }
        case TAG_TYPE_REP_CharTypeRep: { os << L": char"; break; }
      }
    }
    // output actual time followed by EOL
    os << L"\t" << tval.ascii() << endl;
  }
}

void TOOLS::EvalTimefile(const wstring & args)
{
  // Extract name of time file
  wstring files[20];
  int nos = STR_split (args, files, 20, STR_RXwhite_and_comma);
  if (nos == 0) {
    Map timemap (theCompiler().GetTM());
    if (timemap.IsEmpty()) {
      vdm_log << L"No time file loaded" << endl << flush;
    }
    else {
      OutputTimemap(vdm_log, timemap);
    }
    return;
  }
  else if (nos != 1) {
    vdm_log << L"Warning: only first file in list used - " << files[0] << endl << flush;
  }

  // Check that file exists
  wstring fname;
  ifstream sp;
  int not_found;

  TBUTILS::Search_and_OpenFile(files[0], fname, sp, not_found);
  if (not_found==1) {
    vdm_log << L"Couldn't open time file '" << files[0] << endl << flush;
    return ;
  }
  sp.close();

  init_TIMEMAP();

  // Parse file
  Set errs;
  TYPE_TIMEMAP_Timemap timemap;
  if (!tp_main(&vdm_log, fname.c_str(), errs,timemap)) {
    tp_OutputErrors(vdm_log,errs);
    return;
  }

  // store the name of the file in case of re-init
  timefilename = files[0];

  // Add timemap to environment
  theCompiler().SetTM(timemap);
}
#endif //VICE

Sequence TOOLS::getGUITokenInfo( const SEQ<Char> & filename )
{
  return GetCI().getGUITokenInfo( filename );
}

void TOOLS::DisplayContextInfo(const wstring & args, bool token, bool node, wostream & wos)
{
  wstring params[20];
  int nos = STR_split (args, params, 20, STR_RXwhite_and_comma);
  if( 0 == nos ) {
    wos << L"contextinfo requires file name" << endl << flush;
    return;
  }

  Generic fid (GetCI().GetFidForFile( SEQ<Char>(params[0]) ));
  if ( fid.IsNil() ) {
    wos << L"File " << params[0] << L" not defined" << endl << flush;
    return;
  }
  if ( token ) {
    GetCI().DumpTokenList( fid, wos );
  }
  if ( node ) {
    GetCI().DumpNodeList( fid, wos );
  }
}

void TOOLS::DisplayAST(const wstring & args, wostream & wos)
{
  wstring params[20];
  int nos = STR_split (args, params, 20, STR_RXwhite_and_comma);
  wstring modnm (params[0]);
  if( 0 == nos ) {
#ifdef VDMSL
    modnm = ASTAUX::GetDefaultModName();
#endif // VDMSL
#ifdef VDMPP
    wos << L"ast command requires ";
    wos << L"class name" << endl << flush;
    return;
#endif // VDMPP
  }

  TYPE_ProjectTypes_ModuleName modulenm (PTAUX::mk_ModuleName (modnm));
  Generic astval (ToolMediator::Repos()->vdm_GetAST(modulenm));

  if (! astval.IsNil() ) {
    Record ast (PTAUX::ExtractAstOrFlat(astval));
    wos << INT2Q::h2gAS(ast) << endl << flush;
  }
  else {
#ifdef VDMSL
    wos << L"Module ";
#endif // VDMSL
#ifdef VDMPP
    wos << L"File ";
#endif // VDMPP
    wos << params[0] << L" is't defined" << endl << flush;
  }
}

void TOOLS::DisplayVersion(wostream & wos)
{
  wos << giveToolTitle() << L" " << giveToolVersionDate() << endl;
  wstring credit (getCredit());
  unsigned int max = 75;
  while (credit.length() > max) {
    std::string::size_type pos = credit.find_last_of(L" ", max);
    if (pos == std::string::npos) {
      wos << credit << endl;
      credit = L"";
    }
    else {
      wos << credit.substr(0, pos) << endl;
      credit = credit.substr(pos + 1);
    }
  }
  if (!credit.empty()) {
    wos << credit << endl;
  }
  wos << flush;
}

void TOOLS::GoFindType(const wstring & args, wostream & wos)
{
  wstring params[20];
  int nos = STR_split (args, params, 20, STR_RXwhite_and_comma);
  if( 2 != nos ) {
    wos << L"find requires name & class/module name" << endl;
    return;
  }
  TYPE_AS_Name nm (ASTAUX::MkName(params[0]));
  TYPE_AS_Name clnm (ASTAUX::MkName(params[1]));
  Generic dft (DependencyFindType(nm, clnm));
  wos << dft << endl;
}

// The function ConditionParse parses a file if it is not legal to
//  type check it.
#ifdef VDMPP
bool TOOLS::ConditionalParse(const TYPE_ProjectTypes_ModuleName & clnm)
{
  Tuple VdmStatus (ToolMediator::Repos()->vdm_Status(clnm));
  if ((VdmStatus.GetField(1)).IsNil()) {
    vdm_log << L"Class Name \'" << PTAUX::ExtractModuleName(clnm) << L"\' is unknown" << endl << flush;
//    return false;
  }
  bool forall = true;
  status_type status = PTAUX::ExtractStatus(VdmStatus);
  if (status.syntax == status_type::status_none || status.syntax == status_type::status_error) {
    SET<TYPE_ProjectTypes_FileName> files (ToolMediator::Repos()->vdm_FileOfModule(clnm));
    Generic fnm;
    for (bool bb = files.First(fnm); bb && forall; bb = files.Next(fnm)) {
      forall = EvalRead(fnm);
    }
  }
  return forall;
}

bool TOOLS::ParsAllSupers(const TYPE_ProjectTypes_ModuleName & clnm)
{
  if (! ConditionalParse(clnm)) {
    return false;
  }
  Set supers;
  if (!ToolMediator::Repos()->vdm_IsCyclic(clnm)) {
    supers = ToolMediator::Repos()->vdm_GetSuper(clnm);
  }

  bool forall = true;
  Generic clsnm;
  for (bool bb = supers.First(clsnm); bb && forall; bb = supers.Next(clsnm) ) {
    forall = ConditionalParse(clsnm);
  }
  for (bool cc = supers.First(clsnm); cc && forall; cc = supers.Next(clsnm)) {
    forall = ParsAllSupers(clsnm);
  }
  return forall;
}

bool TOOLS::EvalProcessCmd (const TYPE_ProjectTypes_ModuleName & clnm, int opt, wostream & wos)
{
  // Syntax check all the super classes and clnm
  // if they have been changed or is syntactical incorrect.
  if (!ParsAllSupers(clnm)) {
    return false;
  }

  Sequence order;
  if (ToolMediator::Repos()->vdm_IsCyclic(clnm)) {
    order = Sequence().ImpAppend(mk_set(clnm));
  }
  else {
    order = ToolMediator::Repos()->vdm_OrderOfProcess(clnm); // seq of set of AS`Name
  }

  int local_opt = (opt != POS && opt != DEF) ? Settings.TypeCheck() : opt;

  bool nothing_changed = true;
  bool result = true;
  size_t len_order = order.Length();
  for (size_t idx = 1; idx <= len_order; idx++) {
    SET<TYPE_ProjectTypes_ModuleName> nm_s (order[idx]);
    Generic nm;
    for (bool dd = nm_s.First(nm); dd; dd = nm_s.Next(nm)) {
      Tuple VdmStatus (ToolMediator::Repos()->vdm_Status(nm));
      status_type status = PTAUX::ExtractStatus(VdmStatus);
      if (!GetStatSem().namesTypeChecked.nameSeen(nm) &&
          ((status.type == status_type::status_none) ||
           (status.type == status_type::status_error) ||
           ((status.type == status_type::status_pos) && (local_opt != POS)) ||
           ((status.type == status_type::status_def) && (local_opt != DEF)))) {
        GetStatSem().namesTypeChecked.addName(nm);
        result &= LocalEvalTypeCheck(nm, opt, wos);
        nothing_changed = false;
      }
    }
  }
  if (nothing_changed) {
    wos << L"Class " << PTAUX::ExtractModuleName (clnm);
    if ((order.Length() == 1) && (order[1] == mk_set(clnm))) {
      wos << L" is ";
    }
    else {
      wos << L" with super classes are ";
    }
    wos << ((local_opt == POS) ? L"POS" : L"DEF") << L" type correct" << endl << flush;
  }
  return result;
}

bool TOOLS::EvalArgsProcess(const wstring & args, wostream & wos)
{
  wstring params[20];
  STR_split (args, params, 20, STR_RXwhite_and_comma);

  TYPE_ProjectTypes_ModuleName nm;
  if (( 0 != params[0].compare(wstring(L""))    ) &&
      ( 0 != params[0].compare(wstring(L"pos")) ) &&
      ( 0 != params[0].compare(wstring(L"def")))) {
        nm = PTAUX::mk_ModuleName (params[0]);
  }
  else {
    wos << L"Please, specifiy a class to be processed" << endl << flush;
    return false;
  }

  GetStatSem().namesTypeChecked.resetInfo();
  if (params[1] == L"pos") {
    EvalProcessCmd(nm, POS, wos);
  }
  else if (params[1] == L"def") {
    EvalProcessCmd(nm, DEF, wos);
  }
  else {
    wos << L"type check must be called with either pos or def option" << endl << flush;
  }
  return true;
}
#endif //VDMPP

bool TOOLS::EvalSet_Check_NoOf_Args(const wstring & opt, int nos, int nos_expected, wostream & wos)
{
  switch (nos_expected) {
    case 1: {
      if (nos_expected == 1) {
        if (nos != nos_expected) {
          wos << L"set " << opt << L": Only one setting can be set at a time." << endl << flush;
        }
      }
      break;
    }
    case 2: {
      if (nos != nos_expected) {
        wos << L"set " << opt << L": Expected 1 value" << endl << flush;
      }
      break;
    }
    default: {
      if (nos != nos_expected) {
        wos << L"set " << opt << L": Expected " << nos_expected - 1 << L" values" << endl << flush;
      }
      break;
    }
  }
  return (nos == nos_expected);
}

void TOOLS::EvalSet (const wstring & args, wostream & wos)
{
  wstring opts[20];
  int nos = STR_split (args, opts, 20, STR_RXwhite);

  wstring opt (opts[0]);
  if (opt == L"vdm10") {
    if (EvalSet_Check_NoOf_Args(opt, nos, 1, wos)) {
      Settings.VDM10On();
      wos << L"vdm10 set" << endl << flush;
      SEQ<TYPE_ProjectTypes_FileName> file_l (ToolMediator::Repos()->vdm_Files().ToSequence());
      if (!file_l.IsEmpty())
        ToolMediator::BTools ()->vdm_SyntaxCheck (file_l);
    }
  }
  else if (opt == L"dtc") {
    if (EvalSet_Check_NoOf_Args(opt, nos, 1, wos)) {
      Settings.DtcOn();
      wos << L"dtc set" << endl << flush;
    }
  }
  else if (opt == L"inv") {
    if (EvalSet_Check_NoOf_Args(opt, nos, 1, wos)) {
      Settings.InvOn();
      wos << L"inv set" << endl << flush;
      if (!Settings.DTC()) {
        Settings.DtcOn();
        wos << L"dtc set" << endl << flush;
      }
    }
  }
  else if (opt == L"pre") {
    if (EvalSet_Check_NoOf_Args(opt, nos, 1, wos)) {
      Settings.PreOn();
      wos << L"pre set" << endl << flush;
    }
  }
  else if (opt == L"post") {
    if (EvalSet_Check_NoOf_Args(opt, nos, 1, wos)) {
      Settings.PostOn();
      wos << L"post set" << endl << flush;
    }
  }
  else if (opt == L"assertion") {
    if (EvalSet_Check_NoOf_Args(opt, nos, 1, wos)) {
      Settings.AssertOn();
      wos << L"assertion set" << endl << flush;
    }
  }
  else if (opt == L"cases") {
    if (EvalSet_Check_NoOf_Args(opt, nos, 1, wos)) {
      Settings.CasesOn();
      wos << L"cases check set" << endl << flush;
    }
  }
  else if (opt == L"measure") {
    if (EvalSet_Check_NoOf_Args(opt, nos, 1, wos)) {
      Settings.MeasureOn();
      wos << L"measure check set" << endl << flush;
    }
  }
  else if (opt == L"elapsedtime") {
    if (EvalSet_Check_NoOf_Args(opt, nos, 1, wos)) {
      Settings.ElapsedTimeOn();
      wos << L"elapsed time set" << endl << flush;
    }
  }
  else if (opt == L"mapone") {
    if (EvalSet_Check_NoOf_Args(opt, nos, 1, wos)) {
      Settings.MapOneOn();
      wos << L"mapone set" << endl << flush;
    }
  }
  else if (opt == L"tex") {
    if (EvalSet_Check_NoOf_Args(opt, nos, 1, wos)) {
      wos << L"Note: the tex option is no longer used."
                << endl << flush;
    }
  }
  else if (opt == L"sep") {
    if (EvalSet_Check_NoOf_Args(opt, nos, 1, wos)) {
      Settings.SepOn();
      ToolMediator::ResetTCStatus();
      wos << L"sep set" << endl << flush;
    }
  }
  else if (opt == L"full") {
    if (EvalSet_Check_NoOf_Args(opt, nos, 1, wos)) {
      Settings.ErrorLevelPRF();
      ToolMediator::ResetTCStatus();
      wos << L"full set" << endl << flush;
    }
  }
  else if (opt == L"calllog") {
    if (EvalSet_Check_NoOf_Args(opt, nos, 1, wos)) {
      Settings.CallLogOn();
      wos << L"calllog set" << endl << flush;
      OpenCallLog();
    }
  }
#ifdef VDMPP
  else if (opt == L"priority") {
    if (EvalSet_Check_NoOf_Args(opt, nos, 1, wos)) {
      Settings.PriorityBasedOn();
      wos << L"priority set" << endl << flush;
    }
  }
  else if (opt == L"primaryalgorithm") {
    if (EvalSet_Check_NoOf_Args(opt, nos, 2, wos)){
      SetPrimarySchedulingAlgorithm(opts[1]);
    }
  }
  else if (opt == L"maxinstr") {
    if (EvalSet_Check_NoOf_Args(opt, nos, 2, wos)) {
      int val = 0;
      if(TBUTILS::ExpectNumericArg((opts[1]).c_str(), val)) {
        Settings.SetMaxInstr (val);
      }
      else {
        vdm_err << L"set maxinstr: expected integer value." << endl;
      }
    }
  }
  else if (opt == L"hackparser") {
    if (EvalSet_Check_NoOf_Args(opt, nos, 1, wos)) {
      Settings.SetJCGHackParser(true);
      wos << L"hackparser set" << endl << flush;
    }
  }
#ifdef VICE
  else if (opt == L"taskswitch") {
    if (EvalSet_Check_NoOf_Args(opt, nos, 2, wos)) {
      int val = 0;
      if(TBUTILS::ExpectNumericArg((opts[1]).c_str(), val)) {
        Settings.SetTaskSwitch(val);
      }
      else {
        vdm_err << L"set taskswitch: Expected integer value." << endl;
      }
    }
  }
  else if (opt == L"logfile") {
    if (EvalSet_Check_NoOf_Args(opt, nos, 2, wos)) {
      TOOLS::SetLogFile(opts[1]);
    }
  }
  else if (opt == L"maxtime") {
    if (EvalSet_Check_NoOf_Args(opt, nos, 2, wos)) {
      int maxtime = 0;
      if(TBUTILS::ExpectNumericArg((opts[1]).c_str(), maxtime)) {
        Settings.SetTimeSlice(maxtime);
      }
      else {
        vdm_err << L"set maxtime: expected integer value." << endl;
      }
    }
  }
  else if (opt == L"timefactor") {
    if (EvalSet_Check_NoOf_Args(opt, nos, 2, wos)) {
      int timefactor = 0;
      if(TBUTILS::ExpectNumericArg((opts[1]).c_str(), timefactor)) {
        Settings.SetTimeFactor(timefactor);
      }
      else {
        vdm_err << L"set timefactor: expected integer value." << endl;
      }
    }
  }
  else if (opt == L"stepsize") {
    if (EvalSet_Check_NoOf_Args(opt, nos, 2, wos)) {
      int stepsize = 0;
      if(TBUTILS::ExpectNumericArg((opts[1]).c_str(), stepsize)) {
        Settings.SetStepSize(stepsize);
      }
      else {
        vdm_err << L"set stepsize: expected integer value." << endl;
      }
    }
  }
  else if (opt == L"cpucap") {
    if (EvalSet_Check_NoOf_Args(opt, nos, 2, wos)) {
      int cpucap = 0;
      if(TBUTILS::ExpectNumericArg((opts[1]).c_str(), cpucap)) {
        Settings.SetDefaultCapacity(cpucap);
      }
      else {
        vdm_err << L"set cpucap: expected integer value." << endl;
      }
    }
  }
  else if (opt == L"vcpucap") {
    if (EvalSet_Check_NoOf_Args(opt, nos, 2, wos)) {
      int vcpucap = 0;
      if(TBUTILS::ExpectNumericArg((opts[1]).c_str(), vcpucap)) {
        Settings.SetVirtualCPUCapacity(Int(vcpucap));
      }
      else {
        vdm_err << L"set vcpucap: expected integer value." << endl;
      }
    }
  }
#endif //VICE
#endif //VDMPP
#ifdef VDMSL
  else if (opt == L"vdmsl") {
    if (EvalSet_Check_NoOf_Args(opt, nos, 1, wos)) {
        Settings.VDMSLmodeOn();
        Settings.VDM10Off();
      wos << L"vdmsl set" << endl << flush;
    }
  }
#endif //VDMSL
  else if (opt == L"ppr") {
    if (EvalSet_Check_NoOf_Args(opt, nos, 1, wos)) {
      Settings.PrintFormatOn(); //This is actually the set value for ppr
      wos << L"ppr set" << endl << flush;
    }
  }
  else if (opt == L"seed") {
    if (EvalSet_Check_NoOf_Args(opt, nos, 2, wos)) {
      int val = 0;
      if (TBUTILS::ExpectNumericArg((opts[1]).c_str(), val)) {
        Settings.RandomOn(val);
        wos << L"seed set to " << val << L". " << flush;
        if (val >= 0) {
          theStackMachine().InitRandom((unsigned long)val);
          wos << L"Random number generator restarted." << endl << flush;
        }
        else {
          wos << L"Random evaluation disabled." << endl << flush;
        }
      }
      else {
        vdm_err << L"set seed: expected integer value." << endl;
      }
    }
  }
#ifdef VDMPP
  else if (opt == L"interfaces") {
    if (EvalSet_Check_NoOf_Args(opt, nos, 2, wos)) {
      TOOLS::SetInterfacesOption(opts[1]);
    }
  }
#endif //VDMPP
  else if (opt == L"context") {
    if (EvalSet_Check_NoOf_Args(opt, nos, 1, wos)) {
      Settings.ContextOn();
      wos << L"context set" << endl << flush;
    }
  }
  else if (opt == L"exception") {
    if (EvalSet_Check_NoOf_Args(opt, nos, 1, wos)) {
      Settings.RTErrExceptionOn();
      wos << L"exception set" << endl << flush;
    }
  }
  else if (opt == L"profile") {
    if (EvalSet_Check_NoOf_Args(opt, nos, 1, wos)) {
      Settings.ProfileOn();
      wos << L"profile set" << endl << flush;
    }
  }
  else if (opt == L"oldreverse") {
    if (EvalSet_Check_NoOf_Args(opt, nos, 1, wos)) {
      if (!Settings.OldReverse()) {
        ToolMediator::SetOldReverseEnable(true);
        wos << L"oldreverse set" << endl << flush;
      }
    }
  }
  else {
    wos << opt << L" is not an internal option" << endl << flush;
  }
}

void TOOLS::EvalDir(const wstring & args, wostream & wos)
{
  wstring bpkts[20];
  int nos = STR_split (args, bpkts, 20, STR_RXwhite_and_comma);

  if (nos == 0) {
    wos << L"Directories in use are:" << endl << flush;
    TBUTILS::ShowPath(wos);
  }
  else {
    for (int i = 0; i < nos; i++) {
      TBUTILS::AddPath(bpkts[i]);
    }
  }
}

void TOOLS::EvalUnSet (const wstring & opt, wostream & wos)
{
  if (opt == L"vdm10") {
    Settings.VDM10Off();
    wos << L"vdm10 unset" << endl << flush;
    SEQ<TYPE_ProjectTypes_FileName> file_l (ToolMediator::Repos()->vdm_Files().ToSequence());
    if (!file_l.IsEmpty()) {
      ToolMediator::BTools ()->vdm_SyntaxCheck (file_l);
    }
  }
  else if (opt == L"dtc") {
    if (!Settings.INV()) {
      Settings.DtcOff();
      wos << L"dtc unset" << endl << flush;
    }
    else {    
      wos << L"Can't unset dtc because inv is on." << endl << flush;
    }
  }
  else if (opt == L"inv") {
    Settings.InvOff();
//    wos << L"inv unset" << endl << flush;
    wos << L"inv unset ... dtc is still on" << endl << flush;
  }
  else if (opt == L"pre") {
    Settings.PreOff();
    wos << L"pre unset" << endl << flush;
  }
  else if (opt == L"post") {
    Settings.PostOff();
    wos << L"post unset" << endl << flush;
  }
  else if (opt == L"assertion") {
    Settings.AssertOff();
    wos << L"assertion unset" << endl << flush;
  }
  else if (opt == L"cases") {
    Settings.CasesOff();
    wos << L"cases check unset" << endl << flush;
  }
  else if (opt == L"measure") {
    Settings.MeasureOff();
    wos << L"measure check unset" << endl << flush;
  }
  else if (opt == L"elapsedtime") {
    Settings.ElapsedTimeOff();
    wos << L"elapsed time unset" << endl << flush;
  }
  else if (opt == L"mapone") {
    Settings.MapOneOff();
    wos << L"mapone unset" << endl << flush;
  }
  else if (opt == L"calllog") {
    Settings.CallLogOff();
    wos << L"calllog unset" << endl << flush;
    CloseCallLog();
  }
#ifdef VDMPP
  else if (opt == L"priority") {
    Settings.PriorityBasedOff();
    wos << L"priority unset" << endl << flush;
  }
  else if (opt == L"hackparser") {
    Settings.SetJCGHackParser(false);
    wos << L"hackparser unset" << endl << flush;
  }
#ifdef VICE
  else if (opt == L"logfile") {
    TOOLS::SetLogFile(wstring(L"logfile"));
    wos << L"logfile unset - reverting to default" << endl << flush;
  }
  else if (opt == L"vcpucap") {
    Settings.SetVirtualCPUCapacityInfinite();
    wos << L"vcpucap unset - capacity is infinite" << endl << flush;
  }
#endif //VICE
#endif //VDMPP
  else if (opt == L"full") {
    Settings.ErrorLevelWRN1();
    ToolMediator::ResetTCStatus();
    wos << L"full unset" << endl << flush;
  }
#ifdef VDMSL
  else if (opt == L"vdmsl") {
    Settings.VDMSLmodeOff();
    wos << L"vdmsl unset" << endl << flush;
  }
#endif //VDMSL
  else if (opt == L"sep") {
    Settings.SepOff();
    ToolMediator::ResetTCStatus();
    wos << L"sep unset" << endl << flush;
  }
  else if (opt == L"tex") {
    wos << L"Note: the tex option is no longer used." << endl << flush;
  }
  else if (opt == L"seed") {
    Settings.RandomOff();
    wos << L"seed unset" << endl << flush;
  }
  else if (opt == L"ppr") {
    Settings.PrintFormatOff(); //this is actually the unset value for ppr
    wos << L"ppr unset" << endl << flush;
  }
#ifdef VDMPP
  else if (opt == L"interfaces") {
    Settings.SetJCGInterfaces(SET<TYPE_ProjectTypes_ModuleName>());
    wos << L"interfaces unset" << endl << flush;
  }
#endif //VDMPP
  else if (opt == L"context") {
    Settings.ContextOff();
    wos << L"context unset" << endl << flush;
  }
  else if (opt == L"exception") {
    Settings.RTErrExceptionOff();
    wos << L"exception unset" << endl << flush;
  }
  else if (opt == L"profile") {
    Settings.ProfileOff();
    wos << L"profile unset" << endl << flush;
  }
  else if (opt == L"oldreverse") {
    if (Settings.OldReverse()) {
      ToolMediator::SetOldReverseEnable(false);
      wos << L"oldreverse unset" << endl << flush;
    }
  }
  else {
    wos << opt << L" is not an internal option" << endl << flush;
  }
}

// Infos: Strings in the info table
// InfoSep: #blanks between columns
// InfoCol: #columns in the info table

#define INFOCOL 3
static int InfoSep = 2, InfoCol = INFOCOL;

struct InfoTab {
  const wchar_t* str;
};

static struct InfoTab
Infos[] = {
  {L"#"},
  {L"backtrace (bt)"},
  {L"break (b)"},
  {L"_section_break"},
#ifdef VDMSL
  {L"codegen (cg) [module] [opt]"},
#endif //VDMSL
#ifdef VDMPP
  {L"classes"},
  {L"codegen (cg) class [opt]"},
#endif //VDMPP
  {L"condition (cond) num[, expr]"},
  {L"cont (c)"},
  {L"cquit"},
//  {L"clearerror (ce)"},
#ifdef VDMPP
  {L"create (cr)"},
#endif //VDMPP
#ifdef VDMSL
  {L"curmod"},
#endif //VDMSL
#ifdef VDMPP
  {L"curcls"},
#endif //VDMPP
#ifdef VDMPP
  {L"curthread"},
#endif //VDMPP
  {L"debug (d) expr"},
  {L"delete (dl) ident,..."},
#ifdef VDMPP
  {L"destroy obj"},
#endif //VDMPP
  {L"dir path,..."},
  {L"encode"},
  {L"fscode"},
  {L"finish"},
  {L"first (f)"},
#ifdef VDMSL
  {L"functions"},
#else
  {L"functions class"},
#endif // VDMSL
#ifdef VICE
  {L"gettime (gt)"},
#endif //VICE
  {L"info (?) command"},
  {L"init (i)"},
  {L"dlclose (dlc)"},
  {L"disable (dis) ident"},
  {L"enable (ena) ident"},
#ifdef VDMPP
  {L"instvars class"},
  {L"javacg (jcg) class [opt]"},
#endif // VDMPP
  {L"last"},
  {L"latex (l) opt ident"},
#ifdef VDMSL
  {L"modules"},
#endif //VDMSL
  {L"next (n)"},
#ifdef VDMPP
  {L"nocheck (nc) option class"},
  {L"objects"},
#endif //VDMPP
#ifdef VDMSL
  {L"operations"},
#endif // VDMSL
#ifdef VDMPP
  {L"operations class"},
#endif //VDMPP
  {L"pop"},
#ifdef VDMPP
  {L"priorityfile (pf)"},
#endif //VDMPP
  {L"previous (pr)"},
  {L"print (p) expr,.."},
  {L"push name"},
  {L"pwd"},
  {L"quit (q)"},
  {L"read (r) file"},
  {L"rtinfo ident"},
  {L"tcov read file"},
  {L"tcov write file"},
  {L"tcov reset"},
  {L"script file"},
#ifdef VDMPP
  {L"selthread"},
#endif //VDMPP
  {L"set option"},
  {L"singlestep (g)"},
#ifdef VDMSL
  {L"stack"},
  {L"states"},
#endif //VDMSL
  {L"step (s)"},
  {L"stepin (si)"},
  {L"system (sys)"},
#ifdef VDMPP
  {L"threads"},
#endif //VDMPP
  {L"traces"},
#ifdef VICE
  {L"timefile (tf)"},
#endif //VICE
#ifdef VDMSL
  {L"typecheck (tc) [module] opt"},
#endif //VDMSL
#ifdef VDMPP
  {L"typecheck (tc) class opt"},
#endif //VDMPP
#ifdef VDMSL
  {L"types"},
#else
  {L"types class"},
#endif //VDMSL
  {L"unset option"},
#ifdef VDMSL
  {L"values"},
#else
  {L"values class"},
#endif //VDMSL
  {L"version (v)"},
  {0}
};

static struct InfoTab
HiddenInfos[] = {
  {L"ast"},
  {L"contextinfo (ci)"},
  {L"tokeninfo (ti)"},
  {L"nodeinfo (ni)"},
  {L"sigma"},
  {L"new"},

#ifdef VDMPP
  {L"load"},
  {L"save"},
  {L"state"},
  {L"cpu"},
  {L"schedule"},
  {L"threadinfo"},
#endif // VDMPP
  {L"curstate"},
  {0}
};

wstring TOOLS::GenBlanks(int n)
{
  wstring s;
  for (int i=0; i<n; i++)
    s += L" ";
  return s;
}

void TOOLS::EvalHInfo (wostream & wos)
{
  int maxlen[INFOCOL];
  int len;

  typedef vector<InfoTab> infotv;
  infotv shown_infos;
  int i;
  // Filter the commands to not show those starting with _
  for (i = 0; HiddenInfos[i].str != 0; i++) {
    if (HiddenInfos[i].str[0] != '_') {
      shown_infos.push_back(HiddenInfos[i]);
    }
  }
  shown_infos.push_back(HiddenInfos[i]);

  for (i = 0; i<InfoCol; i++) {
    maxlen[i] = 0;
  }
  for (i=0; shown_infos[i].str != 0; i++)
    ;
  int N = i;                    // Number of commands
  double d = N / InfoCol;
  int n;                        // Column height
  n = (int) floor(d);
  if (i % InfoCol) {
    n++;
  }

  for (i=0; i < n; i++) {
    int k=0;
    for (int j=i; j < N; j=j+n) {
      len = wstring (shown_infos[j].str).length ();
      if (len > maxlen[k]) {
        maxlen[k] = len;
      }
      k++;
    }
  }

  for (i=0; i < n; i++) {
    int k=0;
    for (int j=i; j < N; j=j+n) {
      wstring s (shown_infos[j].str);
      len = s.length ();
      wos << s << GenBlanks(maxlen[k] - len + InfoSep) << flush;
      k++;
    }
    wos << endl << flush;
  }
}

void TOOLS::EvalInfo (wostream & wos)
{
  int maxlen[INFOCOL];
  int len;

  typedef vector<InfoTab> infotv;
  infotv shown_infos;
  int i;
  // Filter the commands to not show those starting with _
  for (i = 0; Infos[i].str != 0; i++) {
    wstring cmd (Infos[i].str);

    if( cmd == L"#" ) continue;
    if ((cliOpt != NULL) && !cliOpt->IsCLI()) {
      if( cmd == L"new" ) continue;
    }

    if (Infos[i].str[0] != '_') {
      shown_infos.push_back(Infos[i]);
    }
  }
  shown_infos.push_back(Infos[i]);

  for (i = 0; i<InfoCol; i++) {
    maxlen[i] = 0;
  }
  for (i=0; shown_infos[i].str != 0; i++)
    ;
  int N = i;                    // Number of commands
  double d = N / InfoCol;
  int n;                        // Column height
  n = (int) floor(d);
  if (i % InfoCol)
    n++;

  for (i=0; i < n; i++) {
    int k=0;
    for (int j=i; j < N; j=j+n) {
      len = wstring (shown_infos[j].str).length ();
      if (len > maxlen[k])
        maxlen[k] = len;
      k++;
    }
  }

  for (i=0; i < n; i++) {
    int k=0;
    for (int j=i; j < N; j=j+n) {
      wstring s (shown_infos[j].str);
      len = s.length ();
      wos << s << GenBlanks(maxlen[k] - len + InfoSep) << flush;
      k++;
    }
    wos << endl << flush;
  }
}

int TOOLS::EvalHelp (const wstring & cmd_, wostream & wos)
{
// 20160104 -->
  wstring cmd(cmd_);
  if (TOOLS::abbrev.DomExists (Text (cmd_))) {
    cmd = Text(TOOLS::abbrev[Text (cmd_)]).GetValue ();
  }
// <-- 20160104
  const wchar_t * NEED_INIT = L"This command can only be called after the specification has been initialised.";
#ifdef VDMSL
  wstring VDM (L"VDM-SL");
#else
  wstring VDM (L"VDM++");
#endif

  if (cmd == L"dir") {
    wos << endl
    << L"dir [path, ...]" << endl
    << endl
    << L"Adds a directory to the list of active directories." << endl
    << L"These are the directories which will be searched" << endl
    << L"automatically when trying to locate a definition file." << endl
    << L"When calling this command with no arguments the list" << endl
    << L"of active directories is printed to the screen." << endl
    << L"The directories will be searched in the displayed order." << endl
    << endl;
  }
  else if ((cmd == L"help") || (cmd == L"info")) {
    wos << endl
    << cmd << L" [command]" << endl
    << endl
    <<  L"On-line help." << endl
    << endl;
  }
  else if (cmd == L"init") {
    wos << endl
    << L"init (i)" << endl
    << endl
#ifdef VDMSL
    << L"This command initialises the specification. This includes" << endl
    << L"initialising the state and all values." << endl
#else
    << L"This command initialises the specification. This includes" << endl
    << L"initialising the values, the instance variables of the" << endl
    << L"specification and the objects created during interpretation." << endl
#endif //VDMSL
    << L"If random evaluation of non-deterministic statements" << endl
    << L"is enabled then the controlling random number generator" << endl
    << L"is reinitialised with the given seed." << endl
    << L"If a value is multiple defined it will be reported during" << endl
    << L"this initialisation." << endl
    << endl;
  }
  else if (cmd == L"next") {
    wos << endl
    << L"next (n)" << endl
    << endl
    << L"In the source file window this command displays the" << endl
    << L"position of the next position recorded. This is used" << endl
    << L"to display e.g. syntax and type errors." << endl
    << endl;
  }
  else if (cmd == L"previous") {
    wos << endl
    << L"previous (pr)" << endl
    << endl
    << L"In the source file window this command displays the" << endl
    << L"position of the previous position recorded. This is used" << endl
    << L"to display e.g. syntax and type errors." << endl
    << endl;
  }
  else if (cmd == L"first") {
    wos << endl
    << L"first (f)" << endl
    << endl
    << L"In the source file window this command displays the" << endl
    << L"position of the first position recorded. This is used to" << endl
    << L"display e.g. syntax and type errors." << endl
    << endl;
  }
  else if (cmd == L"last") {
    wos << endl
    << L"last" << endl
    << endl
    << L"In the source file window this command displays the" << endl
    << L"position of the last position recorded. This is used to" << endl
    << L"display e.g. syntax and type errors." << endl
    << endl;
  }
  else if (cmd == L"print") {
    wos << endl
    << L"print (p) expr, ..." << endl
    << endl
    << L"Evaluates and prints the value of the " << VDM
    << L" expression(s) 'expr'" << endl
    << L"with all break points disabled. If a run-time error occurs" << endl
    << L"the execution will be stopped and the position of the error" << endl
    << L"is displayed in the source window." << endl
#ifdef VDMSL
    << L"In addition to the normal VDM-SL values the 'print' command" << endl
    << L"can also return the values 'FUNCTION_VAL' and 'OPERATION_VAL'."<< endl
    << L"This happens if the result of the evaluation is a function" << endl
    << L"or an operation. This situation occurs e.g. if you evaluate" << endl
    << L"a function just by typing the function name without supplying" << endl
    << L"any parameters enclosed in parenthesis." << endl
#endif //VDMSL
    << endl;
  }
  else if (cmd == L"quit") {
    wos << endl
    << L"quit (q)" << endl
    << endl
    << L"Quits the debugger immediately." << endl
    << endl;
  }
  else if (cmd == L"cquit") {
    wos << endl
    << L"cquit" << endl
    << endl
    << L"Quits the debugger immediately." << endl
    << endl;
  }
  else if (cmd == L"tcov") {
    wos << endl
    << L"tcov read file" << endl
    << L"Read test coverage from file." << endl
    << endl
    << L"tcov write file" << endl
    << L"Write test coverage to the given file." << endl
    << endl
    << L"tcov reset" << endl
    << L"Reset test coverage counters to zero" << endl
    << endl;
  }
  else if (cmd == L"read") {
    wos << endl
    << L"read (r) [file]" << endl
    << endl
    << L"Reads specifications from 'file'." << endl
    << L"If 'file' is no 'given, read specifications from files that" << endl
    << L"have been modified since last read." << endl
#ifdef VDMSL
    << L"The file must contain definitions of functions, values," << endl
    << L"operations, types and possibly a state definition, or the" << endl
    << L"file must contain modules definitions. It is not possible" << endl
    << L"to mix modular specifications and ordinary specifications." << endl
#endif //VDMSL
    << L"The 'file' is parsed as it is read.  If a parse error" << endl
    << L"occurs none of the vdm constructs in the file are included," << endl
    << L"and the screen will be split in order to indicate the first" << endl
    << L"parse error in the source window. Succeeding errors can be" << endl
    << L"displayed by the 'next' command. If a file is parsed" << endl
    << L"successfully, the constructs defined in another file which" << endl
    << L"is already read in, are re-defined and their names are" << endl
    << L"reported to the screen. Re-definition of values is only" << endl
    << L"reported by the 'init' command." << endl
    << endl;
  }
  else if (cmd == L"script") {
    wos << endl
    << L"script file" << endl
    << endl
    << L"Executes the script in 'file'. A script is a sequence of" << endl
    << L"'vdmde' commands. When the script has been executed, the" << endl
    << L"control is returned to the debugger." << endl
    << endl;
  }
  else if (cmd == L"set") {
    wos << endl
    << L"set option [parameter]" << endl
    << endl
    << L"Enables one to set options. If the" << endl
    << L"command is called without parameters it displays the current" << endl
    << L"settings. 'option' can be one of the following :" << endl
    << L" 'dtc'   -- enables dynamic typechecking." << endl
    << L" 'inv'   -- enables dynamic checks of invariants. In order for 'inv'" << endl
    << L"            to have any effect, 'dtc' must be enabled as well." << endl
    << L" 'pre'   -- enables check of pre-conditions." << endl
    << L" 'post'  -- enables check of post-conditions." << endl
    << L" 'ppr'   -- enables pretty print format. All values will be displayed " << endl
    << L"            with line-breaks and indentation according to the" << endl
    << L"            structure of the value." << endl
    << L" 'sep'   -- enables separation of error messages and warnings during" << endl
    << L"            type checking. Error messages are displayed before" << endl
    << L"            warnings." << endl
    << L" 'full'  -- enables extended type checks. This option has effect for" << endl
    << L"            both possibly and definitely well-formedness checks." << endl
    << flush
#ifdef VDMPP
    << L" 'priority'" << endl
    << L"         -- enables priority-based scheduling" << endl
#ifdef VICE
    << L" 'taskswitch' integer" << endl
    << L"         -- sets the task switching constant to be the given integer" << endl
#endif //VICE
    << L" 'maxinstr' integer" << endl
    << L"         -- sets the number of instructions per time slice to be the given" << endl
    << L"            integer (used for instruction number slice scheduling)" << endl
#ifdef VICE
    << L" 'maxtime' integer" << endl
    << L"         -- sets the amount of time per time slice to be the given" << endl
    << L"            integer (used for time slice scheduling)" << endl
    << L" 'timefactor' integer" << endl
    << L"         -- sets the time factor to be the given integer. This value is" << endl
    << L"            used to scale execution times." << endl
#endif //VICE
    << L" 'primaryalgorithm' wstring" << endl
    << L"         -- sets the primary scheduling algorithm to be the given wstring." << endl
    << L"            wstring may be one of" << endl
    << L"              pure_cooperative (pc) - Pure cooperative scheduling" << endl
#ifdef VICE
    << L"              timeslice (ts)        - Time sliced scheduling" << endl
#endif //VICE
    << L"              instruction_number_slice (in) " << endl
    << L"                                    - Instruction number sliced scheduling" << endl
#endif //VDMPP
#ifdef VICE
    << L" 'logfile' filename" << endl
    << L"         -- sets the logfile to be the given filename. If not set, the " << endl
    << L"            default (filename \"logfile\") is used." << endl
#endif //VICE

#ifdef VDMSL
    << L" 'vdmsl' -- enables vdmsl-extended type checks. This option will detect" << endl
    << L"            e.g. operation calls in expressions." << endl
#endif //VDMSL
    << L" 'seed' integer" << endl
    << L"         -- initialises a random number generator with the given integer. " << endl
    << L"            This causes a random order of evaluation of substatements in " << endl
    << L"            nondeterministic statement constructs. " << endl
    << L"            The integer must be >= 0. A negative number will disable" << endl
    << L"            random evaluation of nondeterministic statements." << endl
    
    << L"All options are per default disabled, except 'ppr' and 'sep'." << endl;
// context
// exception
  }
  else if (cmd == L"system") {
    wos << endl
    << L"system (sys) command" << endl
    << endl
    << L"Execute a shell command" << endl
    << endl;
  }
#ifdef VICE
  else if (cmd == L"timefile") {
    wos << endl
    << L"timefile (tf) [filename]" << endl
    << endl
    << L"If a filename is given, this command reads the contents of" << endl
    << L"a file containing timing information and merges it with the" << endl
    << L"default time map. Otherwise, it prints out the timing" << endl
    << L"information currently being used" << endl
    << endl;
  }
  else if (cmd == L"gettime") {
    wos << endl
    << L"gettime" << endl
    << endl
    << L"Returns the current time in the interpreter, as computed on" << endl
    << L"the basis of the time file and duration statements." << endl
    << endl;
  }
#endif //VICE
  else if (cmd == L"nocheck") {
    wos << endl
    << L"nocheck (nc) add class" << endl
    << endl
    << L"nocheck (nc) del class" << endl
    << endl
    << L"If the command is called without parameters"
    << L" it displays the current classes." << endl
    << endl;
  }
  else if (cmd == L"typecheck") {
#ifdef VDMSL
    wos << endl
    << L"typecheck (tc) [module] option" << endl
    << endl
    << L"This command makes a static typecheck of the module 'module'." << endl
    << L"If no module is supplied the current module will be checked." << endl
#else
    wos << endl
    << L"typecheck (tc) class option" << endl
    << endl
    << L"This command makes a static typecheck of the class 'class'." << endl
    << L"If necessary, the class will be syntax checked and all" << endl
    << L"super classes will be syntax and type checked." << endl
#endif //VDMSL
    << L"The 'option' can either be 'pos' or 'def' indicating" << endl
    << L"whether the specification should be checked for possibly" << endl
    << L"or definitely well-formedness. If a type error occur the" << endl
    << L"screen will be split in order to indicate the first" << endl
    << L"error in the source window. Succeeding errors can be" << endl
    << L"displayed by the 'next' command." << endl
    << endl;
  }
  else if (cmd == L"unset") {
    wos << endl
    << L"unset 'option'" << endl
    << endl
    << L"Disables one of vdmde's internal options. See 'set'." << endl
    << endl;
  }
  else if (cmd == L"backtrace") {
    wos << endl
    << L"backtrace (bt)" << endl
    << endl
    << L"Displays the function/operation call stack." << endl
    << endl << NEED_INIT << endl
    << endl;
  }
  else if (cmd == L"break") {
    wos << endl
    << L"break (b) name" << endl
    << endl
    << L"Sets a breakpoint at the function or operation"
    << L" with the name 'name'." << endl
#ifdef VDMPP
    << L"The name must consists of the class name and"
    << L" the function or operation name." << endl
#endif //VDMPP
    << endl
    << L"break (b) file line [column]" << endl
    << endl
    << L"break on line 'line' in column 'column' in file 'file'" << endl
    << endl
    << L"If the command is called without parameters"
    << L" it displays the current breakpoints." << endl
    << endl << NEED_INIT << endl
    << endl;
  }
  else if (cmd == L"disable") {
    wos << endl
    << L"disable (dis) breakpoint-number" << endl
    << endl
    << L"Disables the breakpoint with number 'breakpoint-number'." << endl
    << L"To see a list of all breakpoints use the break commands"
    << L" without any arguments." << endl
    << endl;
  }
  else if (cmd == L"enable") {
    wos << endl
    << L"enable (ena) breakpoint-number" << endl
    << endl
    << L"Enables the breakpoint with number 'breakpoint-number'." << endl
    << L"To see a list of all breakpoints use the break commands"
    << L" without any arguments." << endl
    << endl;
  }
  else if (cmd == L"condition") {
    wos << endl
    << L"condition (cond) breakpoint-number,expr" << endl
    << endl
    << L"Add condition to the breakpoint with number 'breakpoint-number'." << endl
    << endl
    << L"condition (cond) breakpoint-number" << endl
    << endl
    << L"Remove condition from the breakpoint with number 'breakpoint-number'." << endl
    << endl
    << L"If the command is called without parameters"
    << L" it displays the current conditions." << endl
    << endl << NEED_INIT << endl
    << endl;
  }
  else if (cmd == L"cont") {
    wos << endl
    << L"cont (c)" << endl
    << endl
    << L"Continues the execution after a break point." << endl
    << endl << NEED_INIT << endl
    << endl;
  }
  else if (cmd == L"rtinfo") {
    wos << endl
    << L"rtinfo file" << endl
    << endl
    << L"Before applying this command some run-time information must" << endl
    << L"have been collected in the test suite 'file'. The test suite" << endl
    << L"is read in and an overview of all functions and operations" << endl
    << L"are displayed. For each element in this list it is shown how" << endl
    << L"many times it has been evaluated. If some of the constructs" << endl
    << L"within the function/operation have not been evaluated the" << endl
    << L"function/operation is marked with an asterix (*) in the list." << endl
    << endl;
  }
#ifdef VDMSL
  else if (cmd == L"curmod") {
    wos << endl
    << L"curmod" << endl
    << endl
    << L"When the specification is structured into modules, this" << endl
    << L"command prints the name of the current module." << endl
    << endl << NEED_INIT << endl
    << endl;
  }
#endif //VDMSL
#ifdef VDMPP
  else if (cmd == L"curcls") {
    wos << endl
    << L"curcls" << endl
    << endl
    << L"When the specification is structured into classes, this" << endl
    << L"command prints the name of the current class." << endl
    << endl << NEED_INIT << endl
    << endl;
  }
#endif //VDMPP
  else if (cmd == L"debug") {
    wos << endl
    << L"debug (d) expr" << endl
    << endl
    << L"Evaluates and prints the value of the " << VDM
    << L" expression 'expr'." << endl
    << L"The execution will be stopped at all enabled break points," << endl
    << L"and the break point position is indicated in the source-code" << endl
    << L"window. If a run-time error occur the execution will be" << endl
    << L"stopped in the context where the error occurred and the" << endl
    << L"position of the error is displayed in the source window." << endl
    << endl << NEED_INIT << endl
    << endl;
  }
  else if (cmd == L"delete") {
    wos << endl
    << L"delete breakpoint-number" << endl
    << endl
    << L"Deletes the breakpoint with number 'breakpoint-number" << endl
    << L"To see a list of all breakpoints use the break commands"
    << L" without any arguments." << endl
    << endl << NEED_INIT << endl
    << endl;
  }
  else if (cmd == L"codegen") {
    wos << endl
#ifdef VDMSL
    << L"codegen (cg) [module] [option]" << endl
    << endl
    << L"This command generates C++ code of the module 'module'." << endl
    << L"If no module is supplied the current module is code generated." << endl
#else
    << L"codegen (cg) class [option]" << endl
    << endl
    << L"This command generates C++ code of the class 'class'." << endl
#endif //VDMSL
    << L" In order to include run-time position information in the" << endl
    << L"generated code, the option 'rti' must be used. The '-P'" << endl
    << L"option generates code that checks pre and post conditions" << endl
    << endl;
  }
  else if (cmd == L"encode") {
    wos << endl
    << L"encode [encoding]" << endl
    << endl
    << L"If a encoding is given, this command sets the encoding of" << endl
    << L"a file containing specification. Otherwise, it prints out" << endl
    << L"the encoding information currently being used." << endl
    << endl
#ifdef _MSC_VER
//    << L"encoding: 28605 ... ISO-8859-15" << endl
//    << L"          65001 ... UTF-8" << endl
//    << L"            932 ... SJIS" << endl
//    << L"          51932 ... eucJP" << endl
//    << L"            950 ... Big5" << endl
//    << L"          51949 ... eucKR" << endl
//    << L"            936 ... GBK" << endl
//    << L"          20866 ... KOI8-R" << endl
//    << L"          28598 ... ISO-8859-8" << endl
//    << L"          57004 ... TSCII-0" << endl
    << L"encoding: ISO-8859-15 UTF-8 SJIS eucJP Big5 eucKR GBK" << endl
    << L"          KOI8-R ISO-8859-8" << endl
#else
    << L"encoding: ISO-8859-15 UTF-8 SJIS eucJP Big5 eucKR GBK" << endl
    << L"          KOI8-R ISO-8859-8 TSCII-0" << endl
#endif // _MSC_VER
    << endl;
  }
#ifdef VDMPP
  else if (cmd == L"javacg") {
    wos << endl
    << L"javacg (jcg) class [opt]" << endl
    << endl
    << L"This command generates Java code of the class 'class'." << endl
    << L"The 's' option only generates skeleton java code." << endl
    << L"The 'u' option only generates java code for types." << endl
//  << L"The 'm' option generates java code with 'small' types." << endl
    << L"The 'e' option generates java code with concurrency"
    << L" constructs." << endl
    << L"The 'z packagename' option specifies a package name"
    << L" for the generated Java classes." << endl
    << L"The 'k' option generates code for pre and post functions." << endl
    << L"The 'P' option generates code that checks pre"
    << L" and post conditions" << endl;
  }
  else if (cmd == L"curthread") {
    wos << endl
    << L"curthread" << endl
    << endl
    << L"This command gives the current thread id." << endl
    << endl;
  }
  else if (cmd == L"threads") {
    wos << endl
    << L"threads" << endl
    << endl
    << L"This command gives information about all the threads." << endl
    << endl;
  }
  else if (cmd == L"priorityfile") {
    wos << endl
    << L"priorityfile (pf) [filename]" << endl
    << endl
    << L"If a filename is given, this command reads the contents of"
    << L" a file containing" << endl
    << L"priority information uses it for priority-based scheduling."
    << L" Otherwise," << endl
    << L"it prints out the priority information"
    << L" currently being used" << endl
    << endl;
  }
  else if (cmd == L"selthread") {
    wos << endl
    << L"selthread threadid" << endl
    << endl
    << L"With this command the user can force the scheduler to" << endl
    << L"swap in thread number \"threadid\"." << endl
    << endl;
 }
#endif //VDMPP
  else if (cmd == L"finish") {
    wos << endl
    << L"finish" << endl
    << endl
    << L"Finishes the evaluation of the current function or"
    << L" operation and return to" << endl
    << L"the caller. The command is used together with 'stepin'." << endl
    << endl << NEED_INIT << endl
    << endl;
  }
  else if (cmd == L"functions") {
    wos << endl
#ifdef VDMSL
    << L"functions" << endl
    << endl
    << L"Displays the names of all defined functions." << endl
#endif // VDMSL
#ifdef VDMPP
    << L"functions class" << endl
    << endl
    << L"Display the names of all defined functions in class 'class'." << endl
#endif // VDPPM
    << endl << NEED_INIT << endl
    << endl;
  }
  else if (cmd == L"latex") {
    wos << endl
    << L"latex (l) [-n|-N|-r] file" << endl
    << endl
    << L"The latex generator is invoked with 'file'." << endl
    << L"The result is merged with the original text into"
    << L" a new document where" << endl
    << L"the " << VDM << L" parts now are typeset in the mathematical font"
    << L" with the NPL" << endl
    << L"macros. Using the '-n' or '-N' option will generate indices"
    << L" for defined" << endl
    << L"or defined and used occurrences. The '-r' option inserts" << endl
    << L"\\color{not-covered} and \\color{covered}"
    << L" in the generated LaTeX file." << endl
    << endl;
  }
#ifdef VDMPP
  else if (cmd == L"classes") {
    wos << endl
    << L"classes" << endl
    << endl
    << L"Displays the names of defined classes." << endl
    << endl << NEED_INIT << endl
    << endl;
  }
  else if (cmd == L"create") {
    wos <<endl
    << L"create (cr) name := invoke statement | new statement." << endl
    << endl
    << L"This command creates an object of name 'name'." << endl
    << L"The object name is in the scope of the debugger" << endl
    << endl << NEED_INIT << endl
    << endl;
  }
  else if (cmd == L"instvars") {
    wos << endl
    << L"instvars class" << endl
    << endl
    << L"Displays the names of the instance variables"
    << L" of the class 'class'." << endl
    << endl << NEED_INIT << endl
    << endl;
  }
  else if (cmd == L"objects") {
    wos << endl
    << L"objects [-a]" << endl
    << endl
    << L"Displays the objects created within the debugger." << endl
    << L"Option -a causes the object table to be printed." << endl
    << endl << NEED_INIT << endl
    << endl;
  }
  else if (cmd == L"destroy") {
    wos << endl
    << L"destroy name" << endl
    << endl
    << L"Destroys the name 'name' from the created objects"
    << L" in the debugger." << endl
    << endl << NEED_INIT << endl
    << endl;
  }
#endif //VDMPP
#ifdef VDMSL
  else if (cmd == L"modules") {
    wos << endl
    << L"modules" << endl
    << endl
    << L"Displays the names of defined modules." << endl
    << endl << NEED_INIT << endl
    << endl;
  }
#endif // VDMSL
  else if (cmd == L"operations") {
    wos << endl
#ifdef VDMSL
    << L"operations" << endl
    << endl
    << L"Displays the names of all defined operations." << endl
#else
    << L"operations class" << endl
    << endl
    << L"Displays the names of all defined operations in class 'class'." << endl
#endif // VDMSL
    << endl << NEED_INIT << endl
    << endl;
  }
  else if (cmd == L"pop") {
    wos << endl
    << L"pop" << endl
    << endl
    << L"The current "
#ifdef VDMSL
    << "module ('curmod')"
#endif // VDMSL
#ifdef VDMPP
    << "class"
#endif // VDMPP
    << " is popped off the stack ('stack')." << endl
    << L"If there is no active module a warning is issued"
    << L" and nothing" << endl
    << L"happens." << endl
    << endl << NEED_INIT << endl
    << endl;
  }
  else if (cmd == L"push") {
    wos << endl
    << L"push name" << endl
    << endl
#ifdef VDMSL
    << L"The module 'name' is pushed onto the modules"
    << L" stack and becomes" << endl
    << L"the active module after initialisation." << endl
#endif // VDMSL
#ifdef VDMPP
    << L"The class 'name' is pushed onto the classes"
    << L" stack and becomes" << endl
    << L"the active class after initialisation." << endl
#endif // VDMPP
    << endl << NEED_INIT << endl
    << endl;
  }
  else if (cmd == L"pwd") {
    wos << endl
    << L"pwd" << endl
    << endl
    << L"Print working directory" << endl
    << endl;
  }
  else if (cmd == L"singlestep") {
    wos << endl
    << L"singlestep (g)" << endl
    << endl
    << L"Executes the next expression or statement and then breaks." << endl
    << endl << NEED_INIT << endl
    << endl;
  }
#ifdef VDMSL
  else if (cmd == L"stack") {
    wos << endl
    << L"stack" << endl
    << endl
    << L"Displays the names of the pushed modules." << endl
    << endl << NEED_INIT << endl
    << endl;
  }
  else if (cmd == L"states") {
    wos << endl
    << L"states" << endl
    << endl
    << L"Displays the names of defined global state names." << endl
    << endl << NEED_INIT << endl
    << endl;
  }
#endif //VDMSL
  else if (cmd == L"step") {
    wos << endl
    << L"step (s)" << endl
    << endl
    << L"Executes the next statement and then breaks. This command" << endl
    << L"will not step into function and operation calls." << endl
    << endl << NEED_INIT << endl
    << endl;
  }
  else if (cmd == L"stepin") {
    wos << endl
    << L"stepin (si)" << endl
    << endl
    << L"Executes the next expression or statement and then breaks."
    << L" This command" << endl
    << L"will also step into function and operation calls." << endl
    << endl << NEED_INIT << endl
    << endl;
  }
  else if (cmd == L"types") {
    wos << endl
#ifdef VDMSL
    << L"types" << endl
    << endl
    << L"Displays the names of all defined types." << endl
#else
    << L"types class" << endl
    << endl
    << L"Displays the names of all defined types in the class 'class'." << endl
#endif //VDMSL
    << endl << NEED_INIT << endl
    << endl;
  }
  else if (cmd == L"values") {
    wos << endl
#ifdef VDMSL
    << L"values" << endl
    << endl
    << L"Displays the names of all defined values." << endl
#else
    << L"values class" << endl
    << endl
    << L"Displays the names of all defined values in class 'class'." << endl
#endif //VDMSL
    << endl << NEED_INIT << endl
    << endl;
  }
  else if (cmd == L"version") {
    wos << endl
    << L"version" << endl
    << endl
    << L"Displays the VDMTools version." << endl
    << endl;
  }
// for special use
  else if (cmd == L"ast") {
    wos << endl
#ifdef VDMSL
    << L"ast module" << endl
    << endl
    << L"Displays the abstract syntax tree of module 'module'." << endl
#endif // VDMSL
#ifdef VDMPP
    << L"ast class" << endl
    << endl
    << L"Displays the abstract syntax tree of class 'class'." << endl
#endif // VDMPP
    << endl;
  }
  else if (cmd == L"contextinfo") {
    wos << endl
    << L"contextinfo filename" << endl
    << endl;
  }
  else if (cmd == L"tokeninfo") {
    wos << endl
    << L"tokeninfo filename" << endl
    << endl;
  }
  else if (cmd == L"nodeinfo") {
    wos << endl
    << L"nodeinfo filename" << endl
    << endl;
  }
  else if (cmd == L"sigma") {
    wos << endl
#ifdef VDMSL
    << L"sigma [modulename]" << endl
#endif // VDMSL
#ifdef VDMPP
    << L"sigma classname" << endl
#endif // VDMPP
    << endl;
  }
#ifdef VDMPP
  else if (cmd == L"load") {
    wos << endl
    << L"load filename" << endl
    << endl;
  }
  else if (cmd == L"save") {
    wos << endl
    << L"save filename" << endl
    << endl;
  }
  else if (cmd == L"state") {
    wos << endl
    << L"state [-cdfnrsu]" << endl
    << L"state -d [oid]" << endl
    << L"state -r [oid]" << endl
    << endl;
  }
  else if (cmd == L"cpu") {
    wos << endl
    << L"cpu" << endl
    << endl;
  }
  else if (cmd == L"schedule") {
    wos << endl
    << L"schedule" << endl
    << endl;
  }
  else if (cmd == L"threadinfo") {
    wos << endl
    << L"threadinfo threadid" << endl
    << endl;
  }
#endif // VDMPP
  else if (cmd == L"curstate") {
    wos << endl
    << L"curstate" << endl
    << endl;
  }
  else {
    wos << L"No help available for this command" << endl << flush;
    return 0;
  }
  wos << flush;
  return 1;
}

void TOOLS::cmd_tcov(const wstring & args, wostream & wos)
{
  wstring params[20];
  int nos = STR_split (args, params, 20, STR_RXwhite);
  if (nos == 0) {
    wos << L"Missing tcov sub-command. See 'help tcov'." << endl << flush;
  }
  else if (params[0] == L"read") {
    if (nos != 2) {
      wos << L"tcov read requires one file name" << endl << flush;
    }
    else {
      wstring coverage_file = params[1];
      // If coverage file exists then read it in.
      wifstream f(TBWSTR::wstring2fsstr(coverage_file).c_str());
      if (f) {
        f.close();
        Tuple gfmnn (ToolMediator::GetFileMapNameNum(SET< SEQ<Char> >(), true));
        const Map & long_names (gfmnn.GetField(1));
        const Map & short_names (gfmnn.GetField(2));

        GetCI().LoadCounters(coverage_file, long_names, short_names, wos, true);
      }
      else {
        wos << L"Could not read test coverage file " << L"'" << coverage_file << L"'" << endl;
      }
    }
  }
  else if (params[0] == L"write") {
    if (nos != 2) {
      wos << L"tcov read requires one file name" << endl;
    }
    else {
      wstring coverage_file = params[1];
      // Save coverage counters into coverage_file
      Map m (ToolMediator::GetFileMapNumName());
      GetCI().SaveCounters(coverage_file, m, wos);
    }
  }
  else if (params[0] == L"reset") {
    if (nos != 1) {
      wos << L"tcov reset does not take any parameters" << endl;
    }
    else {
      GetCI().ResetTestCoverageInfo();
    }
  }
  else {
    wos << L"Unknown tcov command" << endl;
  }
  wos << flush;
}

wstring TOOLS::giveToolTitle()
{
  return tb_version.GiveToolTitle();
}

wstring TOOLS::giveToolVersionDate()
{
  return tb_version.GiveToolVersionDate();
}

wstring TOOLS::getVersionId()
{
  return tb_version.GetVersionId();
}

wstring TOOLS::getCredit()
{
  return L"VDM Toolbox is currently developed with the cooperation of Research Center for Architecture-Oriented Formal Methods of Kyusyu University.";
}

wstring TOOLS::getCompilerVersion()
{
  return tb_version.GetCompilerVersion();
}

void TOOLS::OpenCallLog()
{
  if (!TOOLS::calllogstream.is_open()) {
    string fname = TBWSTR::wstring2fsstr(Settings.GetCallLogFileName());
    TOOLS::calllogstream.open( fname.c_str(), ios_base::out|ios_base::app );
  }
}

void TOOLS::CloseCallLog()
{
  if (TOOLS::calllogstream.is_open()) {
    TOOLS::calllogstream.close();
  }
}

void TOOLS::WriteCallLog(const wstring & id, const Sequence & arg_l, int level)
{
  if (TOOLS::calllogstream.good()) {
    for (int i = 0; i < level; i++) {
      TOOLS::calllogstream << " ";
    }
    TOOLS::calllogstream << TBWSTR::wstring2utf8str(id) << endl;
  }
}

#ifdef VDMPP
void TOOLS::EvalDestroyAllObjects()
{
  TBDEBUG::EvalDestroyAllObjects();
}
#endif // VDMPP

void TOOLS::BreakIP()
{
  TBDEBUG::BreakIP();
}

wstring TOOLS::GetTraceArgs(int index)
{
  return TBDEBUG::GetTraceArgs(index);
}

#ifdef VDMPP
// GetOperations
// clsnm : seq of char
// ==> bool * seq of seq of char
Tuple TOOLS::GetOperations(const wstring & clsnm)
{
  TYPE_AS_Name cls (ASTAUX::MkName(clsnm));
  Tuple t (TBDEBUG::GetOperations(cls));
  if (t.GetBoolValue(1)) {
    Set nm_s (t.GetSet(2));
    Sequence res;
    Generic nm;
    for (bool bb = nm_s.First(nm); bb; bb = nm_s.Next(nm)) {
      res.ImpAppend(Sequence(ASTAUX::ASName2String(nm)));
    }
    return mk_(Bool(false), res);
  }
  return mk_(Bool(false), Nil());
}
#endif // VDMPP

void TOOLS::EvalNoCheck (const wstring & args, wostream & wos)
{
  wstring params[20];
  int nos = STR_split (args, params, 20, STR_RXwhite);
  if (nos == 0) {
    SET<TYPE_AS_Name> cl_s (Settings.GetNoCheck());
    Generic cl;
    for (bool bb = cl_s.First(cl); bb; bb = cl_s.Next(cl)) {
      wos << ASTAUX::ASName2String(cl) << endl;
    }
  }
  else if (nos == 2) {
    wstring opt (params[0]);
    if (opt == L"add") {
      Settings.AddNoCheck(ASTAUX::MkName(params[1]));
    }
    else if (opt == L"del") {
      Settings.RemoveNoCheck(ASTAUX::MkName(params[1]));
    }
  }
}

void TOOLS::EvalSearch (const wstring & args, wostream & wos)
{
  wstring params[20];
  int nos = STR_split (args, params, 20, STR_RXwhite);
  if (nos > 0) {
    SEQ<TYPE_ProjectTypes_FileName> files (ToolMediator::Repos()->vdm_Files().ToSequence());
    Sequence res (SearchId(files, params[0], false, false));
    size_t len_res = res.Length();
    for (size_t idx = 1; idx <= len_res; idx++) {
      Tuple t (res[idx]);
      wos << (t.GetBoolValue(5) ? L"+ " : L"  ")
          << t.GetSequence(1)
          << L", line " << t.GetInt(3)
          << L", col " << t.GetInt(4) << endl;
    }
  }
}

// ParseCommand
// cmd : seq of char
// args : seq of char
// ==> bool (true: continue, false: exit)
bool TOOLS::ParseCommand (const wstring & cmd, const wstring & args_)
{
  wstring args (args_);
  try {
    while (args.length() > 0 && iswspace(args[0])) {
      args.replace(0, 1, L"");
    }

    string sargs = TBWSTR::ConvertToHexquad(args);
    if ((//cmd == L"backtrace" ||
         //cmd == L"break" ||
         cmd == L"enable" ||
         cmd == L"disabled" ||
         cmd == L"condition" ||
         cmd == L"cont" ||
#ifdef VDMPP
         cmd == L"create" ||
         cmd == L"objects" ||
         cmd == L"instvars" ||
         cmd == L"destroy" ||
         cmd == L"save" ||
         cmd == L"load" ||
#endif //VDMPP
#ifdef VICE
         cmd == L"timefile" ||
#endif //VICE
#ifdef VICE
         cmd == L"gettime" ||
#endif //VICE
#ifdef VDMSL
         cmd == L"curmod" ||
#endif // VDMSL
#ifdef VDMPP
         cmd == L"curcls" ||
#endif // VDMPP
         cmd == L"delete" ||
         cmd == L"finish" ||
         cmd == L"functions" ||
         cmd == L"operations" ||
         cmd == L"push" ||
         cmd == L"pop" ||
#ifdef VDMSL
         cmd == L"stack" ||
         cmd == L"states" ||
#endif //VDMSL
         cmd == L"step" ||
         cmd == L"stepin" ||
         cmd == L"singlestep" ||
         cmd == L"up" ||
         cmd == L"down" ||
         cmd == L"types" ||
         cmd == L"values") &&
         !theStackMachine().IsInitialized()
        ) {
      vdm_iplog << L"No initialised specification present" << endl << flush;
      return true; // is not considered as an error!!
    }

    if (cmd == L"break") {
      EvalBreak(args, vdm_iplog);
      return true;
    }
    else if (cmd == L"delete") {
      wstring bpkts[20];
      int nos = STR_split (args, bpkts, 20, STR_RXwhite_and_comma);

      if (nos == 0) {
        vdm_iplog << L"Specify breakpoints to be deleted" << endl << flush;
      }
      else {
        for (int i = 0; i < nos; TBDEBUG::EvalDeleteBreak (bpkts[i++], vdm_iplog));
      }
      return true;
    }
    else if (cmd == L"enable") {
      wstring bpkts[20];
      int nos = STR_split (args, bpkts, 20, STR_RXwhite_and_comma);

      if (nos == 0) {
        vdm_iplog << L"Specify breakpoint numbers to be enabled" << endl << flush;
      }
      else {
        for (int i = 0; i < nos; TBDEBUG::EvalEnableBreak (bpkts[i++], vdm_iplog));
      }
      return true;
    }
    else if (cmd == L"disable") {
      wstring bpkts[20];
      int nos = STR_split (args, bpkts, 20, STR_RXwhite_and_comma);

      if (nos == 0) {
        vdm_iplog << L"Specify breakpoint numbers to be disabled" << endl << flush;
      }
      else {
        for (int i = 0; i < nos; TBDEBUG::EvalDisableBreak (bpkts[i++], vdm_iplog));
      }
      return true;
    }
    else if (cmd == L"condition") {
      TBDEBUG::EvalCondition(args, vdm_iplog);
      return true;
    }
    else if (cmd == L"codegen") {
      wstring params[20];
      STR_split (args, params, 20, STR_RXwhite_and_comma);

      TYPE_ProjectTypes_ModuleName nm;
      if (( 0 != params[0].compare(wstring(L""))) &&
          ( 0 != params[0].compare(wstring(L"rti"))) &&
          ( 0 != params[0].compare(wstring(L"-P")))) {
        nm = PTAUX::mk_ModuleName(params[0]);
      }
#ifdef VDMSL
      else if (ToolMediator::Repos()->vdm_IsSession (flat_session)) {
        nm = PTAUX::mk_ModuleName(ASTAUX::GetDefaultModName());
      }
      else {
        Tuple t (TBDEBUG::GetCurrentModule()); // bool * [ProjectTypes`ModuleName]
        if (t.GetBoolValue(1)) {
          nm = t.GetRecord(2);
        }
        else {
          vdm_iplog << L"No module is pushed onto the stack" << endl << flush;
          return true; // is not considered as an error!!
        }
      }
#endif //VDMSL
#ifdef VDMPP
      else {
        vdm_iplog << L"Please specify a class to be code generated" << endl << flush;
        return true; // is not considered as an error!!
      }
#endif //VDMPP
      Quote kind (L"CPP");
      bool testcond = ((params[0] == L"-P") || (params[1] == L"-P") || (params[2] == L"-P"));
      if (GetCGTools().ResetCG(kind)) {
        GetStatSem().namesTypeChecked.resetInfo();

        if ((params[0] == L"rti") || (params[1] == L"rti") || (params[2] == L"rti")) {

          bool rti = Settings.GetCGRTI();
          Settings.SetCGRTI(true);
          GetCGTools().EvalCodeGen (nm,
                                    kind,        // CPP
                                    Bool(false),
                                    Bool(true),  // prePost
                                    Bool(false),
                                    Bool(false),
                                    Nil(),
                                    Bool(false),
                                    testcond);
          Settings.SetCGRTI(rti);
        }
        else {
          GetCGTools().EvalCodeGen (nm,
                                    kind,        // CPP
                                    Bool(false),
                                    Bool(true),  // prePost
                                    Bool(false),
                                    Bool(false),
                                    Nil(),
                                    Bool(false),
                                    testcond);
        }
      }
#ifdef VDMPP
      ToolMediator::EvalInhTree();
#endif //VDMPP

      // The Tcl call to modules`selectFunctions has been removed from
      // modules`changeState in order to avoid the update of the VDM++
      // dependencies after each module/class being type code generated.
      // UpdateSelections performs a callback to modules`selectFunctions.
      // Notice that it also makes sure that the Dependencies are update.
      // This is why only EvalInhTree and not EvalDependUpdate is called.
      ToolMediator::UpdateSelections();
      return true;
    }
    else if (cmd == L"uses") {
      wstring params[20];
      STR_split (args, params, 20, STR_RXwhite_and_comma);

      TYPE_ProjectTypes_ModuleName nm (PTAUX::mk_ModuleName(params[0]));
      ToolMediator::BTools()->vdm_Depend();
      Set uses (ToolMediator::Repos()->vdm_GetUses(nm));
      vdm_iplog << uses << endl << flush;
      return true;
    }

#ifdef VDMPP
    else if (cmd == L"javacg") {
      wstring params[20];
      STR_split (args, params, 20, STR_RXwhite_and_comma);

      TYPE_ProjectTypes_ModuleName nm;
      if (( 0 != params[0].compare(wstring(L""))) &&
          ( 0 != params[0].compare(wstring(L"-k"))) &&
          ( 0 != params[0].compare(wstring(L"-e"))) &&
          ( 0 != params[0].compare(wstring(L"-u"))) &&
          ( 0 != params[0].compare(wstring(L"-z"))) &&
          ( 0 != params[0].compare(wstring(L"-m"))) &&
          ( 0 != params[0].compare(wstring(L"-P"))) &&
          ( 0 != params[0].compare(wstring(L"-s"))) &&
          ( 0 != params[0].compare(wstring(L"-v")))) {
        nm = PTAUX::mk_ModuleName(params[0]);
      }
      else {
        vdm_iplog << L"Please specify a class to be code generated" << endl << flush;
        EvalHelp(L"javacg", vdm_iplog);
        return true;
      }

      Quote kind (L"JAVA");
      bool skeleton_option = false;
      bool preandpost_option = false;
      bool onlytypes_option = false;
      bool smalltypes_option = false;
      bool conc_option = false;
      bool testpreandpost_option = false;
      bool no_vdm_prefix = false;
      Generic package_name = Nil();
      for (int param_num = 0; param_num < 9; param_num++) {
        if (params[param_num] == L"-s") {
          skeleton_option = true;
        }
        else if (params[param_num] == L"-e") {
          conc_option = true;
        }
        else if (params[param_num] == L"-k") {
          preandpost_option = true;
        }
        else if (params[param_num] == L"-u") {
          onlytypes_option = true;
        }
        else if (params[param_num] == L"-v") {
          no_vdm_prefix = true;
        }
        else if (params[param_num] == L"-m") {
          smalltypes_option = true;
          skeleton_option = true;
        }
        else if (params[param_num] == L"-z") {
          param_num++;
          if ( 0 != params[param_num].compare(wstring(L""))) {
            package_name = SEQ<Char>(params[param_num]);
          }
        }
        else if (params[param_num] == L"-P") {
          testpreandpost_option = true;
          preandpost_option = true;
        }
      }

      if (GetCGTools().ResetCG(kind)) {
        GetStatSem().namesTypeChecked.resetInfo();

        bool vp = Settings.GetJCGVDMPrefix();
        if (no_vdm_prefix) {
          Settings.SetJCGVDMPrefix(false);
        }
        GetCGTools().EvalCodeGen (nm,
                                  kind,             // Java
                                  skeleton_option,
                                  preandpost_option,
                                  onlytypes_option,
                                  smalltypes_option,
                                  package_name,
                                  conc_option,
                                  testpreandpost_option);
        Settings.SetJCGVDMPrefix(vp);
        ToolMediator::EvalInhTree();
        ToolMediator::UpdateSelections();
      }
      return true;
    }
#endif //VDMPP
    else if (cmd == L"_evalstatus") {
      TBDEBUG::PrintEvaluatorStatus(vdm_iplog, false);
      return true;
    }
#ifdef VDMSL
    else if (cmd == L"curmod") {
      TBDEBUG::EvalCurMod (vdm_iplog);
      return true;
    }
#endif // VDMSL
#ifdef VDMPP
    else if (cmd == L"curcls") {
      TBDEBUG::EvalCurCls (vdm_iplog);
      return true;
    }
#endif // VDMPP
    else if (cmd == L"system") {
      //int ret = system(TBWSTR::wstring2string(args).c_str());
      FILE *ptr;

#ifndef _MSC_VER
      string cmdstr (TBWSTR::wstring2coutstr(args) + " 2>&1");
      if ((ptr = popen(cmdstr.c_str(), "r")) != NULL) {
        char buf[BUFSIZ];
        string str;
        while (fgets(buf, BUFSIZ, ptr) != NULL) {
          str += string(buf);
        }
        pclose(ptr);
        vdm_iplog << TBWSTR::cinstr2wstring(str) << endl;
      }
#else
      wstring cmdstr (args + L" 2>&1");
      if ((ptr = _wpopen(cmdstr.c_str(), L"r")) != NULL) {
        wchar_t buf[BUFSIZ];
        wstring str;
        while (fgetws(buf, BUFSIZ, ptr) != NULL) {
          str += wstring(buf);
        }
        _pclose(ptr);
        vdm_iplog << str << endl;
      }
#endif // _MSC_VER
      return true;
    }
    else if (cmd == L"push") {
      wstring bpkts[20];
      int nos = STR_split (args, bpkts, 20, STR_RXwhite_and_comma);
      if (nos == 0) {
#ifdef VDMSL
        vdm_iplog << L"Specify module to be pushed" << endl << flush;
#endif // VDMSL
#ifdef VDMPP
        vdm_iplog << L"Specify class to be pushed" << endl << flush;
#endif // VDMPP
      }
      else {
        TBDEBUG::EvalPushModule (PTAUX::mk_ModuleName(bpkts[0]), vdm_iplog);
      }
      return true;
    }
    else if (cmd == L"pop") {
      TBDEBUG::EvalPopModule (vdm_iplog);
      return true;
    }
    else if (cmd == L"pe") {
      Tuple t (TOOLS::ParseExprsString(TBWSTR::ConvertToHexquad(args)));
      if (t.GetBoolValue(1)) {
        vdm_iplog << INT2Q::h2gAS(t.GetField(2)) << endl;
      }
      return true;
    }
#ifdef VDMSL
    else if (cmd == L"modules") {
      TBDEBUG::EvalUnitList (vdm_iplog);
      return true;
    }
    else if (cmd == L"stack") {
      EvalModuleStack (vdm_iplog);
      return true;
    }
#endif //VDMSL
    else if (cmd == L"dir") {
      EvalDir(args, vdm_iplog);
      return true;
    }
    else if (cmd == L"popd") {
      TBDEBUG::Execute_popd(vdm_iplog);
      return true;
    }
    else if (cmd == L"print") {
      if (args.empty()) {
        if (Settings.GetExpression().IsNil()) {
          vdm_log << L"print command must be used with one or more expression(s)." << endl << flush;
          return true;
        }
        else {
          wstring expr (Sequence(Settings.GetExpression()).GetString());
          vdm_iplog << L"print " << expr << endl << flush;
          sargs = TBWSTR::ConvertToHexquad(expr);
        }
      }
      ToolMediator::Interf()->vdm_DisableUserInput();
      TBDEBUG::ParseAndFullyEvalExprs (PTAUX::mk_ToolCommand (sargs), vdm_iplog, SEQ<Char>(cmd + L" " + args));
      ToolMediator::Interf()->vdm_EnableUserInput();
      return true;
    } // end of L"print"
    else if (cmd == L"debug") {
      if (args.empty()) {
        if (Settings.GetExpression().IsNil()) {
          vdm_log << L"debug command must be used with one or more expression(s)." << endl << flush;
          return true;
        }
        else {
          wstring expr (Sequence(Settings.GetExpression()).GetString());
          vdm_iplog << L"debug " << expr << endl << flush;
          sargs = TBWSTR::ConvertToHexquad(expr);
        }
      }
      ToolMediator::Interf()->vdm_DisableUserInput();
      TBDEBUG::ParseAndStartEvalExprs (PTAUX::mk_ToolCommand (sargs), vdm_iplog, SEQ<Char>(cmd + L" " + args));
      ToolMediator::Interf()->vdm_EnableUserInput();
      return true;
    } // end of L"debug"
    else if (cmd == L"date") {
      Time now;
      vdm_iplog << now << endl;
      return true;
    }
#ifdef VDMPP
    else if (cmd == L"po" ) {
      TBDEBUG::EvalPrintObjects(args, vdm_iplog);
      return true;
    }
#endif // VDMPP
    else if (cmd == L"functions") {
      TBDEBUG::EvalFunctions(args, vdm_iplog);
      return true;
    }
    else if (cmd == L"operations") {
#ifdef VDMSL
      TBDEBUG::EvalOperations(vdm_iplog);
#endif // VDMSL
#ifdef VDMPP
      TBDEBUG::EvalOperations(args, vdm_iplog);
#endif //VDMSL
      return true;
    }
    else if (cmd == L"types") {
#ifdef VDMSL
      TBDEBUG::EvalTypes (vdm_iplog);
#endif // VDMSL
#ifdef VDMPP
      TBDEBUG::EvalTypes(args, vdm_iplog);
#endif //VDMPP
      return true;
    }
    else if (cmd == L"values") {
#ifdef VDMSL
      TBDEBUG::EvalValues (vdm_iplog);
#endif //VDMSL
#ifdef VDMPP
      TBDEBUG::EvalValues(args, vdm_iplog);
#endif //VDMPP
      return true;
    }

    else if (cmd == L"sigma") {
      TBDEBUG::DisplaySigma(args, vdm_iplog);
      return true;
    }
    else if (cmd == L"code") {
      TBDEBUG::DisplayCode(args, vdm_iplog);
      return true;
    }

#ifdef VDMPP
    else if (cmd == L"create") {
      TBDEBUG::EvalCreate(sargs, vdm_iplog);
      return true;
    }
    else if (cmd == L"destroy") {
      TBDEBUG::EvalDestroy(sargs, vdm_iplog);
      return true;
    }
    else if (cmd == L"objects") {
      TBDEBUG::EvalObjects(args, vdm_iplog);
      return true;
    }
    else if (cmd == L"save") {
      TBDEBUG::EvalSaveObjects(args, vdm_iplog);
      return true;
    }
    else if (cmd == L"load") {
      TBDEBUG::EvalLoadObjects(args, vdm_iplog);
      return true;
    }
    else if (cmd == L"classes") {
      TBDEBUG::EvalUnitList(vdm_iplog);
      return true;
    }
    else if (cmd == L"instvars") {
      TBDEBUG::EvalInstVars(args, vdm_iplog);
      return true;
    }
    else if (cmd == L"state") {
      TBDEBUG::DisplayState(args, vdm_iplog);
      return true;
    }
    else if (cmd == L"curthread") {
      TBDEBUG::DisplayCurThread(vdm_iplog);
      return true;
    }
    else if (cmd == L"threads") {
      TBDEBUG::DisplayThreads(vdm_iplog);
      return true;
    }
    else if (cmd == L"selthread") {
      TBDEBUG::ExeSelThread(args, vdm_iplog);
      return true;
    }
    else if (cmd == L"priorityfile") {
      TBDEBUG::EvalPriorityfile(args, vdm_iplog);
      return true;
    }
#endif //VDMPP
    else if ((cmd == L"cont") ||
             (cmd == L"finish") ||
             (cmd == L"step") ||
             (cmd == L"stepin") ||
             (cmd == L"singlestep")) {
      // Do nothing as this should be taken care of earlier
      vdm_iplog << L"Evaluation is finished" << endl << flush;
      return true;
    }
    else if ((cmd == L"up") || (cmd == L"down")) {
      TBDEBUG::ExecuteUpDownCommand(cmd, args, vdm_iplog);
      return true;
    }
    else if (cmd == L"typecheck") {
      int tcoption = POS;
      wstring params[20];
      STR_split (args, params, 20, STR_RXwhite_and_comma);

      TYPE_ProjectTypes_ModuleName nm;
      if ((0 != params[0].compare(wstring(L""))) &&
          (0 != params[0].compare(wstring(L"pos"))) &&
          (0 != params[0].compare(wstring(L"def")))) {
        nm = PTAUX::mk_ModuleName (params[0]);
      }
#ifdef VDMSL
      else if (ToolMediator::Repos()->vdm_IsSession (flat_session)) {
        nm = PTAUX::mk_ModuleName (ASTAUX::GetDefaultModName());
      }
      else {
        Tuple t (TBDEBUG::GetCurrentModule()); // bool * [ProjectTypes`ModuleName]
        if (t.GetBoolValue(1)) {
          nm = t.GetRecord(2);
        }
        else {
          vdm_iplog << L"No module is pushed onto the stack" << endl << flush;
          return true; // is not considered as an error!!
        }
      }
#endif //VDMSL
#ifdef VDMPP
      else {
        vdm_iplog << L"Please, specifiy a class to be type checked" << endl << flush;
        return true; // is not considered as an error!!
      }
#endif //VDMPP
      if ((params[0] == L"def") || (params[1] == L"def")) {
        tcoption = DEF;
      }
      else if ((params[0] == L"pos") || (params[1] == L"pos")) {
        tcoption = POS;
      }
      else {
        vdm_iplog << L"typecheck must be called with either pos or def option" << endl << flush;
      }
      GetStatSem().namesTypeChecked.resetInfo();
      EvalTypeCheck (nm, tcoption, vdm_iplog); //
      //      Set dependupdate;
      //      dependupdate.Insert(nm);
      //      Bool b = ToolMediator::EvalDependUpdate(dependupdate);
#ifdef VDMPP
      ToolMediator::EvalInhTree();
#endif // VDMPP

      // The Tcl call to modules`selectFunctions has been removed from
      // modules`changeState in order to avoid the update of the VDM++
      // dependencies after each module/class being type checked.
      // UpdateSelections performs a callback to modules`selectFunctions.
      // Notice that it also makes sure that the Dependencies are update.
      // This is why only EvalInhTree and not EvalDependUpdate is called.
      ToolMediator::UpdateSelections();
      return true;
    }
    else if (cmd == L"latex") {
      wstring params[20];
      int nos = STR_split (args, params, 20, STR_RXwhite_and_comma);

      if (nos == 0) {
        vdm_iplog << L"Specify file to be processed" << endl << flush;
      }
      else {
        bool rti = Settings.GetPrettyPrintRTI();
        int ind = Settings.GetIndex();
        Settings.SetIndex(0);
        for (int i = 0; i < nos - 1; i++) {
          if (params[i] == L"-n") {
            Settings.SetIndex(1);
          }
          else if (params[i] == L"-N") {
            Settings.SetIndex(2);
          }
          else if (params[i] == L"-r") {
            Settings.SetPrettyPrintRTI(true);
          }
          else {
            vdm_iplog << L"Valid options: '-n', '-N' and '-r'" << endl << flush;
            Settings.SetPrettyPrintRTI(rti);
            Settings.SetIndex(ind);
            return true; // is not considered as an error!!
          }
        }
        if (params[nos-1] == L"*") {
          SET<TYPE_ProjectTypes_FileName> files (ToolMediator::Repos()->vdm_Files());
          Generic file;
          for (bool bb = files.First(file); bb; bb = files.Next(file)) {
            EvalLatex(file);
          }
        }
        else {
          EvalLatex(PTAUX::mk_FileName(params[nos-1]));
        }
        Settings.SetIndex(ind);
        Settings.SetPrettyPrintRTI(rti);
      }

      // The Tcl call to modules`selectFunctions has been removed from
      // modules`changeState in order to avoid the update of the VDM++
      // dependencies after each module/class being pretty printed.
      // UpdateSelections performs a callback to modules`selectFunctions.
      ToolMediator::UpdateSelections();
      return true;
    }
#ifdef VDMSL
    else if (cmd == L"states") {
      TBDEBUG::EvalStates (vdm_iplog);
      return true;
    }
#endif //VDMSL
    else if (cmd == L"set") {
      wstring params[20];
      int nos = STR_split (args, params, 20, STR_RXwhite);
      if (nos == 0) {
        vdm_iplog
         << L"dtc              " << (Settings.DTC() ? L"on" : L"off") << endl
         << L"inv              " << (Settings.INV() ? L"on" : L"off") << endl
         << L"pre              " << (Settings.PreCheck() ? L"on" : L"off") << endl
         << L"post             " << (Settings.PostCheck() ? L"on" : L"off") << endl
         << L"assertion        " << (Settings.Assertion() ? L"on" : L"off") << endl
         << L"full             " << (Settings.IsErrorLevelPRF() ? L"on" : L"off")<< endl
#ifdef VDMSL
         << L"vdmsl            " << (Settings.VDMSLmode() ? L"on" : L"off") << endl
#endif //VDMSL
         << L"vdm10            " << (Settings.VDM10() ? L"on" : L"off") << endl
#ifdef VDMPP
         << L"priority         " << (Settings.PriorityBased() ? L"on" : L"off") << endl
         << L"maxinstr         " << Settings.GetMaxInstr () << endl
         << L"primaryalgorithm " << Settings.GetPrimaryAlgorithmStr() << endl
#ifdef VICE
         << L"taskswitch       " << Settings.GetTaskSwitch() << endl
         << L"maxtime          " << Settings.GetTimeSlice() << endl
         << L"timefactor       " << Settings.GetTimeFactor() << endl
         << L"stepsize         " << Settings.GetStepSize() << endl
         << L"logfile          " << TOOLS::GetLogFile() << endl
         << L"jitter mode      " << Settings.GetJitterModeStr() << endl
         << L"default cpu capacity " << Settings.GetDefaultCapacity() << endl
         << L"virtual cpu capacity " << Settings.GetVirtualCPUCapacityStr() << endl
#endif //VICE
#endif //VDMPP
         << L"sep              " << (Settings.SEP() ? L"on" : L"off") << endl
         << L"seed             " << Settings.Random() << endl
         << L"ppr              " << (Settings.PrintFormat() ? L"on" : L"off") << endl
#ifdef VDMPP
         << L"interfaces       " << GetInterfacesOption() << endl
#endif //VDMPP
         << L"context          " << (Settings.Context() ? L"on" : L"off") << endl
         << L"exception        " << (Settings.RTErrException() ? L"on" : L"off") << endl
         << L"profile          " << (Settings.Profile() ? L"on" : L"off") << endl
         << L"calllog          " << (Settings.CallLog() ? L"on" : L"off") << endl
         << L"measure          " << (Settings.Measure() ? L"on" : L"off") << endl
         << L"mapone           " << (Settings.MapOne() ? L"on" : L"off") << endl
         << L"oldreverse       " << (Settings.OldReverse() ? L"on" : L"off") << endl
         << flush;
      }
      else {
        EvalSet (args, vdm_iplog);
      }
      return true;
    }
    else if (cmd == L"unset") {
      wstring params[20];
      int nos = STR_split (args, params, 20, STR_RXwhite);

      switch (nos) {
        case 0: {
          vdm_iplog << L"Specify setting to be unset" << endl << flush;
          break;
        }
        case 1: {
          EvalUnSet (params[0], vdm_iplog);
          break;
        }
        default: {
          vdm_iplog << L"Specify only one setting to be unset" << endl << flush;
          break;
        }
      }
      return true;
    }
    else if (cmd == L"rtinfo") {
      wstring params[20];
      int nos = STR_split (args, params, 20, STR_RXwhite_and_comma);

      if (nos == 1) {
        TestCoverage::EvalListTestSuite (params[0],GetCI());
      }
      else {
        vdm_iplog << L"rtinfo must first be called with 1 parameter" << endl << flush;
      }
      return true;
    }
    else if (cmd == L"tcov") {
      cmd_tcov(args, vdm_iplog);
      return true;
    }
    else if (cmd == L"next") {
      ToolMediator::Errs()->vdm_Next();
      return true;
    }
    else if (cmd == L"previous") {
      ToolMediator::Errs()->vdm_Prev();
      return true;
    }
    else if (cmd == L"first") {
      ToolMediator::Errs()->vdm_First();
      return true;
    }
    else if (cmd == L"last") {
      ToolMediator::Errs()->vdm_Last();
      return true;
    }
    else if (cmd == L"read")
    {
      wstring files[20];
      int nos = STR_split (args, files, 20, STR_RXwhite_and_comma);

      SEQ<TYPE_ProjectTypes_FileName> file_l;
      if (nos == 0) {
        SET<TYPE_ProjectTypes_FileName> modif_s (ToolMediator::CheckForModifiedFiles ());
        if (modif_s.IsEmpty()) {
          vdm_iplog << L"No files modified since last parsing" << endl << flush;
          return true;
        }
        Generic g;
        for (bool bb = modif_s.First (g); bb; bb = modif_s.Next (g)) {
          file_l.ImpAppend (g);
        }
      }
      else {
        for (int i = 0; i < nos; i++) {
          TYPE_ProjectTypes_FileName fnm (PTAUX::mk_FileName (files[i]));
          file_l.ImpAppend (fnm);
        }
      }
      ToolMediator::BTools ()->vdm_SyntaxCheck (file_l);
      return true;
    } // end of L"read"
    else if (cmd == L"script") {
      wstring files[20];
      int nos = STR_split (args, files, 20, STR_RXwhite_and_comma);

      if (nos == 0) {
        vdm_iplog << L"Specify scriptfile to read" << endl << flush;
        return true; // is not considered as an error!!
      }
      bool res (ReadScriptFile (files[0]));
      return res;
    }
    else if (cmd == L"info") {
      wstring topics[10];
      int no_topics = STR_split (args, topics, 10, STR_RXwhite_and_comma);

      if (no_topics != 0) {
        EvalHelp (topics[0], vdm_iplog);
      }
      else {
        EvalInfo (vdm_iplog);
      }
      return true;
    }
    else if (cmd == L"hinfo") {
      wstring topics[10];
      int no_topics = STR_split (args, topics, 10, STR_RXwhite_and_comma);

      if (no_topics != 0) {
        EvalHelp (topics[0], vdm_iplog);
      }
      else {
        EvalHInfo (vdm_iplog);
      }
      return true;
    }
    else if (cmd == L"backtrace") {
      vdm_iplog << L"Currently evaluating:" << endl << flush;
      TBDEBUG::EvalBackTrace (TBDEBUG::BT_PRINT, vdm_iplog);
      vdm_iplog << endl << flush;
      return true;
    }
    else if (cmd == L"dlclose") {
      theState().dlclose();
      return true;
    }
    else if (cmd == L"init") {
      if (ToolMediator::Repos()->vdm_IsSession (none_session)) {
        vdm_iplog << L"No specification present" << endl << flush;
        InitToolbox( false );
        ToolMediator::Repos()->vdm_ClearAll();
        UpdateToolbox();
        return true;
      }
      TBDEBUG::InitCurrentDefinition (false, vdm_iplog);
      return true;
    }
    // Test if fast init works
    else if (cmd == L"ii") {
      if (ToolMediator::Repos()->vdm_IsSession (none_session)) {
        vdm_iplog << L"No specification present" << endl << flush;
        return true;
      }
      TBDEBUG::InitCurrentDefinition (true, vdm_iplog);
      return true;
    }
    else if ((cmd == L"new") && (cliOpt != NULL) && cliOpt->IsCLI()) {
      InitToolbox( false );
      ToolMediator::Repos()->vdm_ClearAll();
      UpdateToolbox();
      GetCI().clear();
      Settings.InitSettings();
      return true;
    }
    // End of fast init
    else if (cmd == L"pwd") {
      vdm_iplog << TBUTILS::tb_getcwd() << endl << flush;
      return true;
    }
#ifdef CORBA_API
    else if (cmd == L"object-table") {
      VDMApplication_i::echo_object();
      return true;
    }
#endif //CORBA_API
    else if (cmd == L"encode") {
      // Extract name of time file
      wstring files[20];
      int nos = STR_split (args, files, 20, STR_RXwhite_and_comma);
      if (nos == 0) {
        wstring encoding (TBWSTR::getIOCharSetEnv());
#ifdef _MSC_VER
        wstring enc (encoding);
        if (enc == wstring (L"65001")) {
          encoding = wstring (L"UTF8");
        }
        else if (enc == wstring (L"28605")) {
          encoding = wstring (L"ISO-8859-15");
        }
        else if (enc == wstring (L"932")) {
          encoding = wstring(L"SJIS");
        }
        else if (enc == wstring (L"51932")) {
          encoding = wstring(L"eucJP");
        }
        else if (enc == wstring (L"950")) {
          encoding = wstring(L"Big5");
        }
        else if (enc == wstring (L"51949")) {
          encoding = wstring(L"eucKR");
        }
        else if (enc == wstring (L"936")) {
          encoding = wstring(L"GBK");
        }
        else if (enc == wstring (L"20866")) {
          encoding = wstring(L"KOI8-R");
        }
        else if (enc == wstring (L"28598")) {
          encoding = wstring(L"ISO-8859-8");
        }
#endif // _MSC_VER
        vdm_iplog << L"Text Encoding: " << encoding << endl << flush;
      }
      else {
        if (nos != 1) {
          vdm_iplog << L"Warning: only first encoding in list used - " << files[0] << endl << flush;
        }
        wstring encoding (files[0]);
#ifdef _MSC_VER
        wstring enc;
        for( std::string::size_type i = 0; i < encoding.size(); i++ ) {
          if ((encoding[i] != L'-') && (encoding[i] != L'_') && (encoding[i] != L' ')) {
            enc += towlower(encoding[i]);
          }
        }
        if (enc == wstring(L"utf8")) {
          encoding = wstring (L"65001");
        }
        else if (enc == wstring(L"iso885915")) {
          encoding = wstring (L"28605");
        }
        else if ((enc == wstring(L"sjis")) || (enc == wstring(L"shiftjis"))) {
          encoding = wstring (L"932");
        }
        else if (enc == wstring(L"eucjp")) {
          encoding = wstring (L"51932");
        }
        else if (enc == wstring(L"big5")) {
          encoding = wstring (L"950");
        }
        else if (enc == wstring(L"euckr")) {
          encoding = wstring (L"51949");
        }
        else if (enc == wstring(L"gbk")) {
          encoding = wstring (L"936");
        }
        else if (enc == wstring(L"koi8r")) {
          encoding = wstring (L"20866");
        }
        else if (enc == wstring(L"iso88598")) {
          encoding = wstring (L"28598");
        }
        else if (enc == wstring(L"tscii0")) {
          encoding = wstring (L"65001");
        }
#endif // _MSC_VER

        TBWSTR::setIOCharSetEnv( encoding );
      }
      return true;
    }
    else if (cmd == L"fscode") {
      // Extract name of time file
      wstring files[20];
      int nos = STR_split (args, files, 20, STR_RXwhite_and_comma);
      if (nos == 0) {
        vdm_iplog << L"File System Code Set: " << TBWSTR::getCodePageEnv() << endl << flush;
      }
      else {
        if (nos != 1) {
          vdm_iplog << L"Warning: only first encoding in list used - " << files[0] << endl << flush;
        }
        TBWSTR::setCodePageEnv( files[0] );
      }
      return true;
    }
    else if (cmd == L"ast") {
      DisplayAST( args, vdm_iplog );
      return true;
    }
    else if (cmd == L"contextinfo") {
      DisplayContextInfo( args, true, true, vdm_iplog );
      return true;
    }
    else if (cmd == L"tokeninfo") {
      DisplayContextInfo( args, true, false, vdm_iplog );
      return true;
    }
    else if (cmd == L"nodeinfo") {
      DisplayContextInfo( args, false, true, vdm_iplog );
      return true;
    }
    else if (cmd == L"version") {
      DisplayVersion(vdm_iplog);
      return true;
    }
    else if (cmd == L"find") {
      GoFindType(args, vdm_iplog);
      return true;
    }
#ifdef VDMPP
    else if (cmd == L"cpu") {
      TBDEBUG::DisplaySystem(vdm_iplog);
      return true;
    }
    else if (cmd == L"schedule") {
      TBDEBUG::DisplaySchedule(vdm_iplog);
      return true;
    }
    else if (cmd == L"threadinfo") {
      TBDEBUG::DisplayThreadInfo( args, vdm_iplog );
      return true;
    }
#ifdef VICE
    else if (cmd == L"timefile") {
      TOOLS::EvalTimefile(args);
      return true;
    }
    else if (cmd == L"gettime") {
      vdm_iplog << L"current time: " << theSystem().GetTime()
                << endl << flush;
      return true;
    }
#endif //VICE
#endif // VDMPP
    else if (cmd == L"curstate") {
      TBDEBUG::DisplayCurrentState(vdm_iplog);
      return true;
    }
    else if ((cmd == L"#") || (cmd[0] == L'#')) {
      // do nothing
      return true;
    }
    else if (cmd == L"search") {
      EvalSearch (args, vdm_iplog);
      return true;
    }
    else if (cmd == L"traces") {
      TBDEBUG::EvalTraces (args, vdm_iplog);
      return true;
    }
#ifdef VDMPP
    else if (cmd == L"nocheck") {
      wstring params[20];
      int nos = STR_split (args, params, 20, STR_RXwhite);
      if (nos == 0) {
        SET<TYPE_AS_Name> cl_s (Settings.GetNoCheck());
        Generic cl;
        for (bool bb = cl_s.First(cl); bb; bb = cl_s.Next(cl)) {
          vdm_iplog << ASTAUX::ASName2String(cl) << endl;
        }
      }
      else {
        EvalNoCheck (args, vdm_iplog);
      }
      return true;
    }
#endif // VDMPP
    else {
      // command is't unknown
      vdm_iplog << L"Unknown command" << endl << flush;
      return true;
    }
    return true;
  }
  catch (TB_Exception & e) {
    vdm_iplog << L"Catch exception at ParseCommand!!" << endl;
    switch (e.GetExType()) {
      case ERR_IP: {
        TBDEBUG::ProcessRuntimeError(vdm_iplog);
        break;
      }
      default: {
        // It must not happen
        vdm_err << L"Internal Error" << endl;
        break;
      }
    }
    return false;
  }
}

// Execute
// command : seq of char
// arg : seq of seq of char
// ==> bool (true : continue, false : quit)
bool TOOLS::Execute (const wstring & command, const wstring & args)
{
  if (command == L"UNKNOWN") {
    vdm_iplog << L"Unknown command" << endl << flush;
    return true;
  }
  else if ( (command == L"cont") ||
            (command == L"step") ||
            (command == L"singlestep") ||
            (command == L"stepin") ||
            (command == L"finish") ) {
    if (!TOOLS::IsInScript()) {
      ToolMediator::Errs()->vdm_ClearAll();
    }

    try {
      Tuple r;
      if (command == L"cont") {
        r = TBDEBUG::DebugStep(TBDEBUG::CONTINUE);
      }
      else if (command == L"step") {
        r = TBDEBUG::DebugStep(TBDEBUG::STEP);
      }
      else if (command == L"singlestep") {
        r = TBDEBUG::DebugStep(TBDEBUG::SINGLESTEP);
      }
      else if (command == L"stepin") {
        r = TBDEBUG::DebugStep(TBDEBUG::STEPIN);
      }
      else { // (command == L"finish")
        r = TBDEBUG::DebugStep(TBDEBUG::FINISH);
      }

      if (r.GetBool(1)) {
        Tuple aie (TBDEBUG::AfterInterpreterEval(r.GetField(2), r.GetField(3), vdm_iplog));
        if (aie.GetBool(1)) {
          VAL2X::PrintSemValue (aie.GetField(2), vdm_iplog);
          vdm_iplog << flush;
        }
        TBDEBUG::UpdateGui(!aie.GetBool(1), vdm_iplog);
      } else {
        vdm_iplog << L"Construct is fully evaluated" << endl << flush;
      }
    }
    catch (TB_Exception &e) {
      switch (e.GetExType()) {
        case ERR_IP: {
          TBDEBUG::ProcessRuntimeError(vdm_iplog);
          break;
        }
        default: {
          // It must not happen
          vdm_err << L"Internal Error" << endl << flush;
          break;
        }
      }
    }
    if (!TOOLS::IsInScript()) {
      ToolMediator::Errs()->vdm_AllDone();
    }
    return true;
  }
  else if (command == L"quit" || command == L"cquit") {
    return false;
  }
  else {
    return TOOLS::ParseCommand (command, args);
  }
}

// true => return from main loop
// false => continue main loop (after break-point)
// ResetErrors
// command : seq of char
// args : seq of char
//  
bool TOOLS::ResetErrors(const wstring & command, const wstring & args)
{
  if (command == L"read" ||
      command == L"typecheck" ||
      command == L"codegen" ||
      command == L"javacg" ||
      command == L"latex") {
    ToolMediator::Errs()->vdm_ClearAll();
    TOOLS::Execute(command, args);
    ToolMediator::Errs()->vdm_AllDone();
    return true;
  }
  else if (command == L"script") {
    bool cont;
    ToolMediator::Errs()->vdm_ClearAll();
    ToolMediator::Errs()->vdm_ChangeState();
    cont = TOOLS::Execute(command, args);
    ToolMediator::Errs()->vdm_AllDone();
    return cont;
  }
  else {
    bool cont = TOOLS::Execute(command, args);
    return cont;
  }
}

/** Execute **************************************************************/
/** The main debugging functions.                                       **/
/** Commands are read, expanded, parsed and executed.                   **/

// true => return from main loop
// false => continue main loop (after break-point)
Bool TOOLS::ExecuteCommand (const TYPE_ProjectTypes_ToolCommand & ToolCommand)
{

  wstring input = PTAUX::ExtractToolCommand (ToolCommand);
  /* Skip blank command lines 930415. MIAN */
  int i = 0;
  while((i < (int) input.length()) && iswspace(input[i])) {
    i++;
  }

  bool cont = true;
  if (i != (int) input.length()) {
    wstring command (TOOLS::ExpandCommand (input));
    if (command == L"UNKNOWN") {
      vdm_iplog << L"Unknown command" << endl << flush;
    }
    else if ( 0 != command.compare(wstring(L""))) {
      cont = TOOLS::ResetErrors(command, input);
    }
  }
  return Bool(cont);
}

#ifdef VDMPP
void TOOLS::ExpandClassTC(const TYPE_AS_Class &c)
{
  // initialise the type checker with class c.
  (void) (GetStatSem().ExpandClass(POS, c));
  GetStatSem().ResetErrors();
}
#endif //VDMPP

Generic TOOLS::DependencyFindType (const TYPE_AS_Name & nm, const TYPE_AS_Name & clnm) {
#if !defined(NOSS) && VDMPP
  // Looking for object references in type ´nm´,
  // used by dependency computing. Called from UpdateSC
  // after the AST is inserted in the Specification Manager.
  // clnm is the name of the parsed class.
  Generic g (GetStatSem().PublicLookUpTypeName (nm, clnm));
  if (g.IsNil()) {
    return Nil();
  }
  else {
    return Tuple(g).GetField(1);
  }
#else
  return Nil ();
#endif // VDMPP
}

#if VDMPP
Tuple TOOLS::SaveTypeCheckEnv ()
{
  //  return GetStatSem().ExportTCEnv();
  return Tuple(1);
}

Bool TOOLS::LoadTypeCheckEnv (const Tuple & t)
{
  //  return GetStatSem().ImportTCEnv (t);
  return Bool(false);
}
#endif //VDMPP

bool ToolMediator::IsFileModified(const TYPE_ProjectTypes_FileName & fnm)
{
  struct stat buf;
  wstring fname (PTAUX::ExtractFileName(fnm));
  int s = stat (TBWSTR::wstring2fsstr(fname).c_str (), &buf);
  if (!s) {
    long newmtime = buf.st_mtime;
    int oldmtime = (Repos ()->vdm_GetFileTimestamp (fnm)).GetValue ();
    if (int (newmtime) > oldmtime) {
      return true;
    }
    else {
      return false;
    }
  }
  else {  // Could not get stat for file
    return false;
  }
}

SET<TYPE_ProjectTypes_FileName> ToolMediator::CheckForModifiedFiles ()
{
  // Returns with a set of file names that are changed compared to
  // the file system.
  SET<TYPE_ProjectTypes_FileName> files_s (Repos ()->vdm_Files ());
  SET<TYPE_ProjectTypes_FileName> modified_s;
  Generic file;
  for (bool bb = files_s.First (file); bb; bb = files_s.Next (file)) {
    wstring fn (PTAUX::ExtractFileName (file));
    struct stat buf;
    int s = stat (TBWSTR::wstring2fsstr(fn).c_str (), &buf);
    if (!s) {
      long newmtime = buf.st_mtime;
      int oldmtime = (Repos ()->vdm_GetFileTimestamp (file)).GetValue ();
      if (int (newmtime) > oldmtime) {
        modified_s.Insert (file);
      }
    }
    else { // Could not get stat for file
      modified_s.Insert (file);
    }
  }
  return modified_s;
}

#ifdef VDMPP
void TOOLS::SetPrimarySchedulingAlgorithm(const wstring & algorithm)
{
  if (!Settings.SetPrimaryAlgorithmStr(algorithm)) {
    vdm_err << endl
            << L"Unknown scheduling algorithm - " << algorithm << L" - ignored." << endl << flush;
  }
}

#ifdef VICE
void TOOLS::SetLogFile(const wstring & filename)
{
  wstring fname;
  ifstream sp;
  int not_found;

  // If file already exists, back up to be on the safe side
  TBUTILS::Search_and_OpenFile(filename, fname, sp, not_found);
  if (not_found == 0) {
    vdm_log << L"Backing up " << filename << L" to " << filename << L".org" << endl << flush;
    wofstream out(TBWSTR::wstring2fsstr(fname + L".org").c_str());
    out << sp.rdbuf();
    out.close();
  }
  sp.close();

  TOOLS::logfilename = fname;
  TIMETRACE::SetLogFile(fname.data());
}
#endif //VICE

void TOOLS::SetInterfacesOption(const wstring & interfacesList)
{
  wstring separator = L",";
  wstring classNames[20];
  int numClasses = STR_split(interfacesList, classNames, 20, STR_RXwhite_and_comma);
  SET<TYPE_AS_Name> nm_s;
  for (int i = 0; i < numClasses; i ++) {
    nm_s.Insert(ASTAUX::MkName(classNames[i]));
  }

  SET<TYPE_AS_Name> modNames (ToolMediator::GetAllVDMModuleNames());
  Set unknown (nm_s.Diff(modNames));
  nm_s.ImpIntersect(modNames);

  SET<TYPE_AS_Name> interfaces_s (PTAUX::ModuleNameSet2ASNameSet(Settings.GetJCGInterfaces()));
  Set possibleInterfaces (GetCGTools().EvalPossibleInterfaces());
  Set prevPossibleInterfaces;
  while (possibleInterfaces != prevPossibleInterfaces) {
    interfaces_s.ImpUnion(possibleInterfaces.Intersect(nm_s));
    Settings.SetJCGInterfaces(PTAUX::ASNameSet2ModuleNameSet(interfaces_s));
    prevPossibleInterfaces = possibleInterfaces;
    possibleInterfaces = GetCGTools().EvalPossibleInterfaces();
  }

  Set nointer (nm_s.Diff(interfaces_s));
  if (!nointer.IsEmpty()) {
    Generic nm;
    for (bool bb = nointer.First(nm); bb; bb = nointer.Next(nm)) {
      vdm_log << L"Can not generate class " << ASTAUX::ASName2String (nm)
              << L" as an interface - ignored" << endl << flush;
    }
  }
  if (!unknown.IsEmpty()) {
    Generic nm;
    for (bool bb = unknown.First(nm); bb; bb = unknown.Next(nm)) {
      vdm_log << L"Unknown class - " << ASTAUX::ASName2String (nm)
              << L" - specified as interface: ignored" << endl << flush;
    }
  }
}

wstring TOOLS::GetInterfacesOption()
{
  SET<TYPE_ProjectTypes_ModuleName> interfaces_s (Settings.GetJCGInterfaces());
  wstring interfacesList = L"";
  Generic g;
  for (bool bb = interfaces_s.First(g); bb; bb = interfaces_s.Next(g)) {
    if (interfacesList != L"") {
      interfacesList += L",";
    }
    interfacesList += PTAUX::Seq2Str(TYPE_ProjectTypes_ModuleName(g).get_nm());
  }
  return interfacesList;
}
#endif //VDMPP

void TOOLS::ResetSSNames()
{
  GetStatSem().namesTypeChecked.resetInfo();
}

#ifdef VDMPP
void TOOLS::CheckSSParseEnv()
{
  SET<TYPE_ProjectTypes_ModuleName> mods (ToolMediator::Repos()->vdm_AllModules());
  GetStatSem().RemoveClassFromParseEnv(PTAUX::ModuleNameSet2ASNameSet(mods));
}
#endif // VDMPP

//
bool TOOLS::ifstr2ufile( ifstream & ifs, string & ufname )
{
  ofstream ofs;
  wstring name (TBUTILS::tb_tempnam(L"vdmd7"));

  if( 0 == name.length() ) return false;

  ufname = TBWSTR::wstring2fsstr(name);
  ofs.open( ufname.c_str() );

  bool res = true;
  if (ofs.good()) {
    res = ifstr2ustr(ifs, ofs);
    ofs.close();
  }
  else {
    res = false;
  }
  return res;
}

bool TOOLS::ifstr2ustr( ifstream & ifs, ostream & ostr )
{
  bool res = true;
  while ( ifs.peek() != EOF ) {
    string line (TBWSTR::vdm_getline(ifs) + '\n');

    try {
      string converted (TBWSTR::ConvertToHexquad(TBWSTR::mbstr2wstring(line)));
      ostr.write( converted.c_str(), converted.length() );
    }
    catch (...) {
      res = false;
      break;
    }
  }
  return res;
}

void TOOLS::ExecuteMainLoop ()
{
#ifdef CORBA_API
  if( Settings.UsesCorba() ) {
    ExecuteMainLoop (true);
  }
  else {
    ExecuteMainLoop (false);
  }
#else
  ExecuteMainLoop (false);
#endif // CORBA_API
}

void TOOLS::ExecuteMainLoop (bool useCorba)
{
  vdm_log.flush();
  vdm_err.flush();

  if (useCorba) {
#ifdef CORBA_API
    APICommandQueue* queue = APICommandQueue::GetInstance ();
    queue->StartUserInputLoop ();
#endif // CORBA_API

    bool cont (true);
    while (cont) {
#ifdef CORBA_API
      // Execute CORBA-API calls
      queue->WaitForCommand ();
      queue->ExecuteQueuedCommand ();
      cont = !(queue->QuitCommandExecuted ());
#else // CORBA_API
      wstring input (ReadLine ());
      cont = (ToolMediator::BTools ()->vdm_ExecuteCommand (PTAUX::mk_ToolCommand (input))).GetValue();
#endif // CORBA_API
      vdm_log.flush();
      vdm_err.flush();
    }//end while
  }
  else {
    bool cont (true);
    while (cont) {
      wstring input (ReadLine ());
      cont = (ToolMediator::BTools ()->vdm_ExecuteCommand (PTAUX::mk_ToolCommand (input))).GetValue();
      vdm_log.flush();
      vdm_err.flush();
    }//end while
  }
}

bool TOOLS::InitCorbaApi(int argc, char *argv[], wstring &err)
{
#ifdef CORBA_API
  if( Settings.UsesCorba() ) {
    if( VDMCORBA::init_corba_api(argc, argv, err) ) {
      return true;
    }
    else {
      VDMCORBA::terminate_corba_api();
      return false;
    }
  }
  return true;
#else
  return true;
#endif //CORBA_API
}

void TOOLS::TerminateCorbaApi()
{
#ifdef CORBA_API
  if( Settings.UsesCorba() ) {
    VDMCORBA::terminate_corba_api();
  }
#endif // CORBA_API
}

void TOOLS::ExternalParseService(const CLIOptions & thisCliOpt)
{
// 20150319 -->
// not used
/*
#ifdef CORBA_API
  if( Settings.UsesCorba() )
  {
    ServiceManager sm;
    sm.CLIStartUp(thisCliOpt);
    sm.RegisterInternalService(new ParserService());
    sm.CLIRun();
  }
#endif // CORBA_API
*/
// <-- 20150319
}

// SearchId
// files : seq of ProjectTypes`FileName
// name : seq of char
// partial : bool
// defOnly : bool
// ==> seq of (seq of char * seq of char * nat * nat * bool)
Sequence TOOLS::SearchId(const SEQ<TYPE_ProjectTypes_FileName> & files,
                  const wstring & name, bool partial, bool defOnly)
{
  if (defOnly) {
    return SearchIdDef(files, name, partial);
  }
  Sequence res;
  if (!files.IsEmpty()) {
    int len_files = files.Length();
    for (int fc = 1; fc <= len_files; fc++) { 
      TYPE_ProjectTypes_FileName fnm (files[fc]);
      wstring filename (PTAUX::ExtractFileName(fnm));
      TYPE_ProjectTypes_FileName tfn (ToolMediator::GetVDMTempFileName (fnm));
      wstring tmpfn (PTAUX::ExtractFileName (tfn));
      if (tmpfn.empty()) {
        tmpfn = filename;
      }
      Sequence fnseq (filename);
      Sequence tfnseq (tmpfn);

      Generic tig (GetCI().GetTokenInfos(SEQ<Char>(filename)));
      if (!tig.IsNil()) {
        Sequence ti_l (tig);
        int len = ti_l.Length();
        for (int i = 1; i <= len; i++) {
          TYPE_CI_TokenInfo ti (ti_l[i]);
          if (BINOPS::isIdentifier(ti.get_lexid())) {
            wstring id (ti.get_text().GetString());
            int line = ti.get_pos_ust ().get_abs_uline();
            int col = ti.get_pos_ust ().get_column();
            if (id == name) {
              res.ImpAppend(mk_(fnseq, tfnseq, Int(line), Int(col), Bool(false)));
            }
            else if (partial) {
              std::string::size_type index = id.find(name);
              if (index != string::npos) {
                res.ImpAppend(mk_(fnseq, tfnseq, Int(line), Int(col + index), Bool(false)));
              }
            }
          }
        }
      }
    }
  }
// mark def
  Set def (SearchIdDef(files, name, partial).Elems());
  int len_res = res.Length();
  for (int idx = 1; idx <= len_res; idx++) {
    Tuple t (res[idx]);
    t.SetField(5, Bool(true));
    if (def.InSet(t)) {
      res.ImpModify(idx, t);
    }
  }
  return res;
}

Tuple CheckName(const TYPE_AS_Name & nm, const wstring & name, bool partial)
{
  const TYPE_AS_Ids ids (nm.GetSequence(pos_AS_Name_ids));
  const TYPE_CI_ContextId & cid  (nm.GetInt(pos_AS_Name_cid));

  if (!ids.IsEmpty() && (cid != NilContextId)) {
    wstring id (ids.Index(ids.Length()).GetString());
    if (id == name) {
      Tuple t (GetCI().GetFileLineColPos(cid));
      return mk_(Bool(true), t.GetInt(2), t.GetInt(3));
    }
    else if (partial) {
      std::string::size_type index = id.find(name);
      if (index != string::npos) {
        Tuple t (GetCI().GetFileLineColPos(cid));
        return mk_(Bool(true), t.GetInt(2), Int(t.GetIntValue(3) + index));
      }
    }
  }
  return mk_(Bool(false), Nil(), Nil()); 
}

// SearchIdDef
// files : seq of ProjectTypes`FileName
// name : seq of char
// partial : bool
// ==> seq of (seq of char * seq of char * nat * nat)
Sequence TOOLS::SearchIdDef(const SEQ<TYPE_ProjectTypes_FileName> & files, const wstring & name, bool partial)
{
  Sequence res;
  if (!files.IsEmpty()) {
    int len_files = files.Length();
    for (int fc = 1; fc <= len_files; fc++) { 
      TYPE_ProjectTypes_FileName fnm (files[fc]);
      wstring filename (PTAUX::ExtractFileName(fnm));
      TYPE_ProjectTypes_FileName tfn (ToolMediator::GetVDMTempFileName (fnm));
      wstring tmpfn (PTAUX::ExtractFileName (tfn));
      if (tmpfn.empty()) {
        tmpfn = filename;
      }
      Sequence fnseq (filename);
      Sequence tfnseq (tmpfn);

      SET<TYPE_ProjectTypes_ModuleName> nm_s (ToolMediator::Repos()->vdm_ModulesInFile(fnm));
      Generic nm;
      for (bool bb = nm_s.First(nm); bb; bb = nm_s.Next(nm)) {
        TYPE_ProjectTypes_AstVal astval (ToolMediator::Repos()->vdm_GetAST(nm));
#ifdef VDMSL
        TYPE_AS_Module mod (PTAUX::ExtractAstOrFlat(astval));
        // module
        Tuple t (CheckName(mod.GetRecord(pos_AS_Module_nm), name, partial));
        if (t.GetBool(1)) {
          res.ImpAppend(mk_(fnseq, tfnseq, t.GetInt(2), t.GetInt(3), Bool(true)));
        }
        // definitions
        Generic def_g (mod.GetField(pos_AS_Module_defs));
#endif // VDMSL
#ifdef VDMPP
        TYPE_AS_Class cls (PTAUX::ExtractAstOrFlat(astval));
        // class
        Tuple t (CheckName(cls.GetRecord(pos_AS_Class_nm), name, partial));
        if (t.GetBool(1)) {
          res.ImpAppend(mk_(fnseq, tfnseq, t.GetInt(2), t.GetInt(3), Bool(true)));
        }
        // definitions
        Generic def_g (cls.GetField(pos_AS_Class_defs));
#endif // VDMPP
        if (!def_g.IsNil()) {
          TYPE_AS_Definitions def (def_g);
          const MAP<TYPE_AS_Name,TYPE_AS_TypeDef> & typem (def.GetMap(pos_AS_Definitions_typem));
          const SEQ<TYPE_AS_ValueDef> & valuem (def.GetSequence(pos_AS_Definitions_valuem));
          const MAP<TYPE_AS_Name,TYPE_AS_FnDef> & fnm (def.GetMap(pos_AS_Definitions_fnm));
          const MAP<TYPE_AS_Name,TYPE_AS_OpDef> & opm (def.GetMap(pos_AS_Definitions_opm));
#ifdef VDMPP
          const SEQ<TYPE_AS_InstanceVarDef> & instvars (def.GetSequence(pos_AS_Definitions_instvars));
#endif // VDMPP

          // types
          Set dom_typem (typem.Dom());
          Generic id;
          for (bool cc = dom_typem.First(id); cc; cc = dom_typem.Next(id)) {
            Tuple t (CheckName(id, name, partial));
            if (t.GetBool(1)) {
              res.ImpAppend(mk_(fnseq, tfnseq, t.GetInt(2), t.GetInt(3), Bool(true)));
            }
          }
          // values
          int len_valuem = valuem.Length();
          for (int j = 1; j <= len_valuem; j++) {
            const TYPE_AS_ValueDef & vd (valuem[j]);
            const TYPE_AS_Pattern & p (vd.GetRecord(pos_AS_ValueDef_pat));
            if (p.Is(TAG_TYPE_AS_PatternName)) {
              Tuple t (CheckName(p.GetRecord(pos_AS_PatternName_nm), name, partial));
              if (t.GetBool(1)) {
                res.ImpAppend(mk_(fnseq, tfnseq, t.GetInt(2), t.GetInt(3), Bool(true)));
              }
            }
          }
          // functions
          Set dom_fnm (fnm.Dom());
          for (bool dd = dom_fnm.First(id); dd; dd = dom_fnm.Next(id)) {
#ifdef VDMSL
            TYPE_AS_Name real_id (id);
#endif // VDMSL
#ifdef VDMPP
            TYPE_AS_Name real_id (MANGLE::IsMangled(id) ? MANGLE::GetUnmangledName(id) : TYPE_AS_Name(id));
#endif // VDMPP
            Tuple t (CheckName(real_id, name, partial));
            if (t.GetBoolValue(1)) {
              res.ImpAppend(mk_(fnseq, tfnseq, t.GetInt(2), t.GetInt(3), Bool(true)));
            }
          }
          // operations
          Set dom_opm (opm.Dom());
          for (bool ee = dom_opm.First(id); ee; ee = dom_opm.Next(id)) {
#ifdef VDMSL
            TYPE_AS_Name real_id (id);
#endif // VDMSL
#ifdef VDMPP
            TYPE_AS_Name real_id (MANGLE::IsMangled(id) ? MANGLE::GetUnmangledName(id) : TYPE_AS_Name(id));
#endif // VDMPP
            Tuple t (CheckName(real_id, name, partial));
            if (t.GetBoolValue(1)) {
              res.ImpAppend(mk_(fnseq, tfnseq, t.GetInt(2), t.GetInt(3), Bool(true)));
            }
          }
#ifdef VDMPP
          // instance variables
          int len_instvars = instvars.Length();
          for (int k = 1; k <= len_instvars; k++) {
            const TYPE_AS_InstanceVarDef & ivd (instvars[k]);
            if (ivd.Is(TAG_TYPE_AS_InstAssignDef)) {
              const TYPE_AS_AssignDef & ad (ivd.GetRecord(pos_AS_InstAssignDef_ad));
              Tuple t (CheckName(ad.GetRecord(pos_AS_AssignDef_var), name, partial));
              if (t.GetBool(1)) {
                res.ImpAppend(mk_(fnseq, tfnseq, t.GetInt(2), t.GetInt(3), Bool(true)));
              }
            }
          }
#endif // VDMPP
        } // if (!def_g.IsNil())
      }   // loop of class/module
    }     // loop of file
  }       // if (!files.IsEmpty())
  return res;
}

