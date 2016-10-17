/***
*  * WHAT
*  *    Implementation of preliminary methods in class BaseTools
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/specman/code/BaseTools_userimpl.cc,v $
*  * VERSION
*  *    $Revision: 1.38 $
*  * DATE
*  *    $Date: 2006/06/12 08:36:17 $
*  * STATUS
*  *    Under development
*  * REFERENCES
*  *    IFAD-VDM28
*  * PROJECT
*  *    Toolbox
*  * AUTHOR
*  *    Henrik Voss + $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include <stdio.h>
#include <sys/stat.h>

#ifndef _MSC_VER
#include <unistd.h>
#endif // _MSC_VER

#include "BaseTools.h"
#include "tools.h"
#include "codegen_tools.h"
//#include "javagen_tools.h"
#include "projectval.h"
#include "StateStore.h"
#include "NoneSes.h"
#include "astaux.h"
#include "Interface.h"
#include "tbutils.h"
#include "tbdebug.h"
#include "pog-interface.h"
#include "settings.h"
#include "tb_exceptions.h"

//
// Implementation of preliminary methods in class BaseTools
//

// SetMediator
// vdm_m : ToolMediator
// ==> ()
void vdm_BaseTools::vdm_SetMediator(const type_ref_ToolMediator & vdm_m)
{
  vdm_mediator = vdm_m;
  vdm_ToolMediator* mediator = ObjGet_vdm_ToolMediator (vdm_mediator);
  ToolMediator::SetMediator (mediator);
}

// SyntaxCheck
// filenmae_l : seq1 of FileName
// ==> bool
Bool vdm_BaseTools::vdm_SyntaxCheck (const type_21ProjectTypes_FileNameCL & filename_l)
{
  bool succ = true;

  // Check to see if at least all parsed files in current project
  // is going to be parsed. If this is the case,
  // we disable the current session by setting it to NoneSes.
  // The session type will be restored  according to the type
  // of the first successfully parsed file in filename_l
  // (in UpdateRepository in tools.cc)
  if (!filename_l.IsEmpty())
  {
    SET<TYPE_ProjectTypes_FileName> parsedfiles_s (ToolMediator::ParsedFiles ());
    if (!parsedfiles_s.IsEmpty() && parsedfiles_s.SubSet (filename_l.Elems ())) {
      ToolMediator::DisableSession ();
    }

// 20120525 -->
    SET<TYPE_ProjectTypes_FileName> files (ToolMediator::Files());
    files.ImpIntersect(filename_l.Elems());
    files.ImpIntersect(ToolMediator::ParsedFiles ());
    if (!files.IsEmpty())
    {
      ToolMediator::RemoveFiles ( files );
#ifdef VDMPP
      TOOLS::CheckSSParseEnv();
#endif // VDMPP
    }
// <--20120525

    ToolMediator::Errs()->vdm_ClearAll();

    ToolMediator::Interf()->vdm_InitMeter(SEQ<Char>(L"Syntax Checking"), type_cL());
    ToolMediator::Interf()->vdm_SetMeterTotal(filename_l.Length());

    bool errlimit = false;
    int len_filename_l = filename_l.Length();
    for (int idx = 1; (idx <= len_filename_l) && !errlimit; idx++)
    {
      const TYPE_ProjectTypes_FileName & filenm (filename_l[idx]);
      // Update meter
      SEQ<Char> label (L"Syntax checking ");
      label.ImpConc(SEQ<Char>(PTAUX::ExtractFileName(filenm)));
      ToolMediator::Interf()->vdm_IncrementMeter(label);
      //
      succ = TOOLS::EvalRead (filenm) && succ;

      errlimit = (ToolMediator::Errs()->vdm_GetErrors().Length() > 100);
    }
#ifdef VDMPP
    ToolMediator::EvalInhTree();
#endif //VDMPP

    ToolMediator::UpdateSelections();
    PTAUX::SomethingIsTypeIncorrect();
    ToolMediator::Interf()->vdm_DestroyMeter();
    ToolMediator::Errs()->vdm_AllDone();

    if (errlimit)
    {
      vdm_log << L"Syntax error limit exceeded." << endl;
      vdm_log << L"Syntax checking aborted." << endl;
    }
  }
  return Bool (succ);
}

// TypeCheck
// mnm_l : seq1 of ModuleName
// ==> bool
Bool vdm_BaseTools::vdm_TypeCheck (const type_23ProjectTypes_ModuleNameCL & mnm_l)
{
  bool succ = true;

  if (!mnm_l.IsEmpty())
  {
    ToolMediator::Errs()->vdm_ClearAll();

    ToolMediator::Interf()->vdm_InitMeter(SEQ<Char>(L"Type Checking"), type_cL());
    ToolMediator::Interf()->vdm_SetMeterTotal(mnm_l.Length());

    TOOLS::ResetSSNames();

    bool errlimit = false;
    int len_mnm_l = mnm_l.Length();
    for (int idx = 1; (idx <= len_mnm_l) && !errlimit; idx++)
    {
      const TYPE_ProjectTypes_ModuleName & mnm (mnm_l[idx]);
      SEQ<Char> label(L"Type checking ");
      label.ImpConc(SEQ<Char>(PTAUX::ExtractModuleName(mnm)));
      ToolMediator::Interf()->vdm_IncrementMeter(label);
      // 
      succ = TOOLS::EvalTypeCheck (mnm, 0, vdm_log) && succ;

      errlimit = (ToolMediator::Errs()->vdm_GetErrors().Length() > 100);
    }
#ifdef VDMPP
    ToolMediator::EvalInhTree();
#endif //VDMPP

    ToolMediator::UpdateSelections();
    PTAUX::UpdateIsEverythingTypeCorrect();

    ToolMediator::Interf()->vdm_DestroyMeter();

    ToolMediator::Errs()->vdm_AllDone();

    if (errlimit)
    {
      vdm_log << L"Type error limit exceeded." << endl;
      vdm_log << L"Type checking aborted." << endl;
    }
  }
  return Bool (succ);
}

// CodeGenerate
// mnm_l : seq1 of ModuleName
// m : (<JAVA>|<CPP>)
// s : bool
// p : bool
// t : bool
// st : bool
// package_name : [seq of char]
// cop : bool
// testcond : bool
// ==> bool
Bool vdm_BaseTools::vdm_CodeGenerate (const type_23ProjectTypes_ModuleNameCL & mnm_l,
                                      const Generic & m,
                                      const Bool & s,
                                      const Bool & p,
                                      const Bool & t,
                                      const Bool & st,
                                      const Generic & package_name,
                                      const Bool & cop,
                                      const Bool & testcond)
{
  bool succ = true;

  if (!mnm_l.IsEmpty())
  {
    ToolMediator::Errs()->vdm_ClearAll();

    ToolMediator::Interf()->vdm_InitMeter(SEQ<Char>(L"Generating C++"), type_cL());
    ToolMediator::Interf()->vdm_SetMeterTotal(mnm_l.Length());

//  ResetCG(); // 20070921
    TOOLS::ResetSSNames();

    size_t len_mnm_l = mnm_l.Length();
    for (size_t idx = 1; idx <= len_mnm_l; idx++)
    {
      SEQ<Char> label;
      if (m == Quote(L"CPP"))
        label = SEQ<Char>(L"Generating C++ for ");
      else
        label = SEQ<Char>(L"Generating Java for ");

      label.ImpConc(SEQ<Char>(PTAUX::ExtractModuleName(mnm_l[idx])));
      ToolMediator::Interf()->vdm_IncrementMeter(label);
      //
      if (GetCGTools().ResetCG(m)) // 20070921
      {
        succ = GetCGTools().EvalCodeGen (mnm_l[idx],m,s,p,t,st, package_name,cop, testcond) && succ;
      }
      else
        succ = false;
    }
#ifdef VDMPP
    ToolMediator::EvalInhTree();
#endif //VDMPP
    ToolMediator::UpdateSelections();
    ToolMediator::Interf()->vdm_DestroyMeter();
    ToolMediator::Errs()->vdm_AllDone();
  }
  return Bool (succ);
}

// PrettyPrint
// fnm_l : seq1 of FileName
// ==> bool
Bool vdm_BaseTools::vdm_PrettyPrint (const type_21ProjectTypes_FileNameCL & fnm_l)
{
  bool succ = true;

  if (!fnm_l.IsEmpty())
  {
    ToolMediator::Errs()->vdm_ClearAll();

    ToolMediator::Interf()->vdm_InitMeter(SEQ<Char>(L"Pretty printing"), type_cL());
    ToolMediator::Interf()->vdm_SetMeterTotal(fnm_l.Length());

    size_t len_fnm_l = fnm_l.Length();
    for (size_t idx = 1; idx <= len_fnm_l; idx++)
    {
      SEQ<Char> label(L"Pretty printing ");
      label.ImpConc(SEQ<Char>(PTAUX::ExtractFileName(fnm_l[idx])));
      ToolMediator::Interf()->vdm_IncrementMeter(label);
      //
      succ = TOOLS::EvalLatex (fnm_l[idx]) && succ;
    }

    // The Tcl call to modules`selectFunctions has been removed from
    // modules`changeState in order to avoid the update of the VDM++
    // dependencies after each module/class being pretty printed.
    // UpdateSelections performs a callback to modules`selectFunctions.
    ToolMediator::UpdateSelections();

    ToolMediator::Interf()->vdm_DestroyMeter();

    ToolMediator::Errs()->vdm_AllDone();
  }
  return Bool (succ);
}

// ClassDepend
// nm : ModuleName
// ==> bool
Bool vdm_BaseTools::vdm_ClassDepend(const TYPE_ProjectTypes_ModuleName & nm)
{
#ifdef VDMSL
  return Bool(true);
#endif // VDMSL
#ifdef VDMPP
  return ToolMediator::EvalDependInfo(nm);
#endif // VDMPP
}

// InhTree
// ==>  bool
Bool vdm_BaseTools::vdm_InhTree()
{
#ifdef VDMSL
  return Bool(true);
#endif // VDMSL
#ifdef VDMPP
  return ToolMediator::EvalInhTree();
#endif // VDMPP
}

// Depend

// ParseAndEvalExprs
// expr : ToolCommand | FileName
// ==> bool * seq of [token]
type_bUL2P vdm_BaseTools::vdm_ParseAndEvalExprs(const Record & expr)
{
  return TBDEBUG::ParseAndFullyEvalExprs (expr, vdm_log, SEQ<Char>(L"---------"));
}

// ParseAndDebugExprs
// expr : ToolCommand | FileName
// ==>  EvalState * seq of [token]
type_UUL2P vdm_BaseTools::vdm_ParseAndDebugExprs(const Record & expr)
{
  return TBDEBUG::ParseAndStartEvalExprs (expr, vdm_log, SEQ<Char>(L"---------"));
}

// InitInterpreter
// ==> bool
Bool vdm_BaseTools::vdm_InitInterpreter()
{
  return TBDEBUG::InitCurrentDefinition(false, vdm_iplog);
}

// ExecuteCommand
// command : ToolCommand
// ==> bool
Bool vdm_BaseTools::vdm_ExecuteCommand (const TYPE_ProjectTypes_ToolCommand & command)
{
  return TOOLS::ExecuteCommand (command);
}

/**
 * sets a breakpoint and returns the number of the breakpoint
 * @param mod name of module
 * @param nm name of expression
 * @returns -1 on failure
 */
// SetBreakOnName
// mod : ModuleName
// nm : Name
// ==> nat
Int vdm_BaseTools::vdm_SetBreakOnName(const TYPE_ProjectTypes_ModuleName & mod, const TYPE_ProjectTypes_Name & nm)
{
  return Int (TBDEBUG::EvalBreakOnName(PTAUX::ExtractModuleName(mod) + L"`" + PTAUX::ExtractName(nm), vdm_iplog));
}

// sets a breakpoint and returns the number of the breakpoint
// @param file name of the file that contains the specified line
// @param line line number
// @param col column number
// @returns -1 on failure
// SetBreakOnPos
// file : FileName
// line : nat
// col : nat
// ==> nat
Int vdm_BaseTools::vdm_SetBreakOnPos(const TYPE_ProjectTypes_FileName & file, const Int & line, const Int & col)
{
  std::wstring lineStr (line.ascii());
  std::wstring colStr (col.ascii());

  std::wstring fileName;
  TYPE_ProjectTypes_String seq (file.get_nm());
  if (!seq.GetString (fileName))
    return -1;

  return Int (TBDEBUG::EvalBreakOnPos(fileName, lineStr, colStr, vdm_iplog));
}

// deletes a breakpoint
// @param num Number of breakpoint, returned by SetBreakPointBy[Pos|Name]
// DeleteBreakPoint
// num : nat
// ==> ()
void vdm_BaseTools::vdm_DeleteBreakPoint (const Int & num)
{
  TBDEBUG::EvalDeleteBreak (num.ascii(), vdm_iplog);
}

// DebugStep
// ==> EvalState * [seq of token]
type_UU2P vdm_BaseTools::vdm_DebugStep ()
{
  return TBDEBUG::EvalDebugStep(vdm_iplog);
}

// DebugStepIn
// ==> EvalState * [seq of token]
type_UU2P vdm_BaseTools::vdm_DebugStepIn ()
{
  return TBDEBUG::EvalDebugStepIn(vdm_iplog);
}

// DebugSingleStep
// ==> EvalState * [seq of token]
type_UU2P vdm_BaseTools::vdm_DebugSingleStep ()
{
  return TBDEBUG::EvalDebugSingleStep(vdm_iplog);
}

// DebugContinue
// ==> EvalState * [seq of token]
type_UU2P vdm_BaseTools::vdm_DebugContinue ()
{
  return TBDEBUG::EvalDebugContinue(vdm_iplog);
}

// InitToolbox
// ==> ()
void vdm_BaseTools::vdm_InitToolbox()
{
// 20081027
//  InitToolbox(true);
  TOOLS::InitToolbox(TOOLS::isBatchMode());
}

// SaveTypeCheckEnv
// stor : StateStore
// ==> bool
Bool vdm_BaseTools::vdm_SaveTypeCheckEnv (const type_ref_StateStore & stor)
{
#ifdef VDMPP
  Tuple env (TOOLS::SaveTypeCheckEnv());
  TYPE_ProjectTypes_TCEnv tcenv (PTAUX::mk_TCEnv (env));
  Bool ok (ObjGet_vdm_StateStore (stor)->vdm_WriteTCEnv (tcenv));
  return ok;
#endif // VDMPP
#ifdef VDMSL
  return Bool (true);
#endif // VDMSL
}

// LoadTypeCheckEnv
// stor : StateStore
// ==> bool
Bool vdm_BaseTools::vdm_LoadTypeCheckEnv (const type_ref_StateStore & stor)
{
#ifdef VDMPP
  Generic tcenv (ObjGet_vdm_StateStore (stor)->vdm_ReadTCEnv ());
  if (tcenv.IsRecord ()) {
    Tuple env (PTAUX::ExtractTCEnv (tcenv));
    return TOOLS::LoadTypeCheckEnv (env);
  }
  else
    return Bool (false);
#endif // VDMPP
#ifdef VDMSL
  return Bool (true);
#endif // VDMSL
}

// UpdateToolbox
// ==> ()
void vdm_BaseTools::vdm_UpdateToolbox()
{
  TOOLS::UpdateToolbox();
}

// CheckForModifiedFiles
// () ==> ()
void vdm_BaseTools::vdm_CheckForModifiedFiles ()
{
  ToolMediator::CheckForModifiedFiles ();
}

// SetPriorityFile
// fn : FileName
// ==> ()
void vdm_BaseTools::vdm_SetPriorityFile(const TYPE_ProjectTypes_FileName& fn)
{
#ifdef VDMPP
  TBDEBUG::EvalPriorityfile(PTAUX::ExtractFileName(fn), vdm_log);
#endif //VDMPP
}

// SetPrimarySchedulingAlgorithm
// nm : Name
// ==> ()
void vdm_BaseTools::vdm_SetPrimarySchedulingAlgorithm(const TYPE_ProjectTypes_Name &nm)
{
#ifdef VDMPP
  TOOLS::SetPrimarySchedulingAlgorithm(PTAUX::ExtractName(nm));
#endif //VDMPP
}

// SetTimeFile
// fn : FileName ==> ()
void vdm_BaseTools::vdm_SetTimeFile(const TYPE_ProjectTypes_FileName & fn)
{
#ifdef VICE
  TOOLS::EvalTimefile(PTAUX::ExtractFileName(fn));
#endif //VICE
}

// returns a tuple, holding a boolean value that
// indicates if the operation was successful and
// the name of the current module

// GetCurrentModule
// ==> bool * [ModuleName]
type_bU2P vdm_BaseTools::vdm_GetCurrentModule ()
{
  return TBDEBUG::GetCurrentModule();
}

// pops a module from the stack
// returns true if successful
// PopModule
// ==> bool
Bool vdm_BaseTools::vdm_PopModule ()
{
  return TBDEBUG::EvalPopModule (vdm_iplog);
}

// Pushes a module onto the stack
// returns true if successful
// PushModule
// name : ModuleName
// ==> bool
Bool vdm_BaseTools::vdm_PushModule (const TYPE_ProjectTypes_ModuleName & name)
{
  return TBDEBUG::EvalPushModule (name, vdm_iplog);
}

// GetPossibleInterfaces
// ==> set of ModuleName
type_23ProjectTypes_ModuleNameCS vdm_BaseTools::vdm_GetPossibleInterfaces()
{
#ifdef VDMPP
// 20110603 -->
  SET<TYPE_ProjectTypes_ModuleName> nms (PTAUX::ASNameSet2ModuleNameSet(ToolMediator::GetAllVDMModuleNames()));
  SEQ<TYPE_ProjectTypes_ModuleName> vdmModules;
  Generic mn;
  for (bool bb = nms.First(mn); bb; bb = nms.Next(mn))
  {
    status_type st = PTAUX::ExtractStatus(ToolMediator::Status(mn));
    switch (st.type) {
      case status_type::status_ok:
      case status_type::status_pos:
      case status_type::status_def: {
        break;
      }
      case status_type::status_error:
      case status_type::status_none:
      default: {
        vdmModules.ImpAppend(mn);
        break;
      }
    }
  }

  // save POS/DEF setting
  bool the_Setting_DEF = Settings.IsDEF();
  Settings.DefOff();

  bool succ = true;
  if (!vdmModules.IsEmpty())
    succ = ToolMediator::BTools()->vdm_TypeCheck(vdmModules);
  else if (!nms.IsEmpty()) // 20071025
    succ = true;

  // restore POS/DEF setting
  if(the_Setting_DEF)
    Settings.DefOn();
  else
    Settings.DefOff();

// <-- 20110603
  GetCGTools().ResetCG(Quote(L"JAVA"));
  TOOLS::ResetSSNames();
  SET<TYPE_AS_Name> possInterfaces (GetCGTools().EvalPossibleInterfaces());
  SET<TYPE_ProjectTypes_ModuleName> returnset (PTAUX::ASNameSet2ModuleNameSet(possInterfaces));
  return returnset;
#endif // VDMPP
#ifdef VDMSL
  return SET<TYPE_ProjectTypes_ModuleName>();
#endif // VDSLM
}

// ResetInterfaces
// ==> ()
void vdm_BaseTools::vdm_ResetInterfaces()
{
#ifdef VDMPP
  GetCGTools().ResetJCGInterfaces();
#endif //VDMPP
}

// JavaSyntaxCheck
// filename_l : seq1 of FileName
// ==> bool
Bool vdm_BaseTools::vdm_JavaSyntaxCheck(const type_21ProjectTypes_FileNameCL & filename_l)
{
  bool succ = true;
#ifdef VDMPP
  if (!filename_l.IsEmpty())
  {
    ToolMediator::Errs()->vdm_ClearAll();

    ToolMediator::Interf()->vdm_InitMeter(SEQ<Char>(L"Syntax Checking"), type_cL());
    ToolMediator::Interf()->vdm_SetMeterTotal(filename_l.Length());

    SET<TYPE_ProjectTypes_FileName> parsedfiles_s (ToolMediator::ParsedFiles ());
    if (!parsedfiles_s.IsEmpty() && parsedfiles_s.SubSet (filename_l.Elems ())) {
      ToolMediator::DisableSession ();
    }

    size_t len_filename_l = filename_l.Length();
    for (size_t idx = 1; idx <= len_filename_l; idx++)
    {
      const TYPE_ProjectTypes_FileName & filenm (filename_l[idx]);
      // Update meter
      SEQ<Char> label(L"Syntax checking ");
      label.ImpConc(SEQ<Char>(PTAUX::ExtractFileName(filenm)));
      ToolMediator::Interf()->vdm_IncrementMeter(label);
      //
      try {
        //succ = JavaGenTools::EvalJavaParse (filenm) && succ;
        succ = TOOLS::EvalJavaParse (filenm) && succ;
      }
      catch(TB_Exception & e)
      {
        vdm_log << L"Runtime Error: " << filenm.get_nm ().GetString() << endl << flush;
      }
    }
    ToolMediator::UpdateSelections();

    ToolMediator::Interf()->vdm_DestroyMeter();

    ToolMediator::Errs()->vdm_AllDone();
  }
#endif //VDMPP
  return Bool (succ);
}

// JavaTypeCheck
// mnm_l : seq1 of ModuleName
// ==> bool
Bool vdm_BaseTools::vdm_JavaTypeCheck(const type_23ProjectTypes_ModuleNameCL& mnm_l)
{
  bool succ = true;
#ifdef VDMPP
  if (!mnm_l.IsEmpty())
  {
    ToolMediator::Errs()->vdm_ClearAll();

    ToolMediator::Interf()->vdm_InitMeter(SEQ<Char>(L"Java type Checking"), type_cL());
    ToolMediator::Interf()->vdm_SetMeterTotal(mnm_l.Length());

    size_t len_mnm_l = mnm_l.Length();
    for (size_t idx =1; idx <= len_mnm_l; idx++)
    {
      const TYPE_ProjectTypes_ModuleName & mnm (mnm_l[idx]);
      SEQ<Char> label(L"Java type checking ");
      label.ImpConc(SEQ<Char>(PTAUX::ExtractModuleName(mnm)));
      ToolMediator::Interf()->vdm_IncrementMeter(label);
      //
      //succ = JavaGenTools::EvalJavaTypeCheck (mnm) && succ;
      succ = TOOLS::EvalJavaTypeCheck (mnm) && succ;
    }
    ToolMediator::UpdateSelections();
    //JavaGenTools::AfterJavaTypeCheck();
    TOOLS::AfterJavaTypeCheck();

    ToolMediator::Interf()->vdm_DestroyMeter();

    ToolMediator::Errs()->vdm_AllDone();
  }
#endif //VDMPP
  return Bool (succ);
}

// JavaGenerateVDM
// mnm_l : seq1 of ModuleName
// p_stubs : bool
// p_rename : bool
// strans : set of nat
// etrans : set of nat
// ==> bool
Bool vdm_BaseTools::vdm_JavaGenerateVDM(const type_23ProjectTypes_ModuleNameCL& mnm_l,
                                        const Bool & p_stubs,
                                        const Bool & p_rename,
                                        const Bool & p_trans)
{
  bool succ = true;
#ifdef VDMPP
  if (!mnm_l.IsEmpty())
  {
    ToolMediator::Errs()->vdm_ClearAll();

    ToolMediator::Interf()->vdm_InitMeter(SEQ<Char>(L"Generating VDM++"), type_cL());
    ToolMediator::Interf()->vdm_SetMeterTotal(mnm_l.Length());

    size_t len_mnm_l = mnm_l.Length();
    for (size_t idx = 1; idx <= len_mnm_l; idx++)
    {
      const TYPE_ProjectTypes_ModuleName & mnm (mnm_l[idx]);
      SEQ<Char> label(L"Generating VDM++ for ");
      label.ImpConc(SEQ<Char>(PTAUX::ExtractModuleName(mnm)));
      ToolMediator::Interf()->vdm_IncrementMeter(label);
      //
      //succ = JavaGenTools::EvalJavaGenerateVDM (mnm, p_stubs, p_rename, p_trans) && succ;
      succ = TOOLS::EvalJavaGenerateVDM (mnm, p_stubs, p_rename, p_trans) && succ;
    }
    ToolMediator::UpdateSelections();

    ToolMediator::Interf()->vdm_DestroyMeter();

    ToolMediator::Errs()->vdm_AllDone();
  }
#endif //VDMPP
  return Bool (succ);
}

// vdm_NewUnnamedProject
void vdm_BaseTools::vdm_NewUnnamedProject ()
{
#ifdef VDMPP
  TOOLS::EvalDestroyAllObjects();
#endif // VDMPP
  TOOLS::InitToolbox( false ); // 20051110
  ToolMediator::ClearAll(); // 20051110
  GetCI().clear(); // 20060123

  ToolMediator::NewUnnamedProject();

#ifdef VDMPP
  ToolMediator::UMLT()->vdm_ResetMapper();
#endif // VDMPP

  init_POG_uMEDIATOR();
}

// vdm_LoadProject
void vdm_BaseTools::vdm_LoadProject (const TYPE_ProjectTypes_FileName & pnm)
{
#ifdef VDMPP
  TOOLS::EvalDestroyAllObjects();
#endif // VDMPP
  TOOLS::InitToolbox( false ); // 20051110
  ToolMediator::ClearAll(); // 20051110
  GetCI().clear(); // 20060123

  try {
    ToolMediator::Open(pnm);
  }
  catch(TB_Exception & e)
  {
    vdm_log << L"Runtime Error: " << pnm.get_nm ().GetString() << endl << flush;
  }

  init_POG_uMEDIATOR();

  TOOLS::set_spec_init( false ); // 20051222

  wstring projectFile(PTAUX::ExtractFileName (pnm)); 
  wstring basedir (TBUTILS::tb_getbasedir(projectFile));
  TBUTILS::SetDefaultPath(basedir);
  if ((basedir != L".") && (TBUTILS::tb_getcwd() != basedir))
    TBUTILS::tb_chdir(basedir, vdm_log);

#ifdef VDMPP
  ToolMediator::UMLT()->vdm_ResetMapper();
#endif // VDMPP
}

// vdm_AddFiles
void vdm_BaseTools::vdm_AddFiles (const type_21ProjectTypes_FileNameCS & fnm_s)
{
  ToolMediator::AddFiles ( fnm_s );

  TOOLS::set_spec_init( false ); // 20051222

// 20121121 -->
  if (!fnm_s.IsEmpty() && ToolMediator::BTools()->vdm_GetProjectName().IsNil())
  {
    wstring file (PTAUX::ExtractFileName (fnm_s.GetElem())); 
    wstring basedir (TBUTILS::tb_getbasedir(file));
    TBUTILS::SetDefaultPath(basedir);
    if ((basedir != L".") && (TBUTILS::tb_getcwd() != basedir))
      TBUTILS::tb_chdir(basedir, vdm_log);
  }
// <-- 20121121
}

// vdm_RemoveFiles
void vdm_BaseTools::vdm_RemoveFiles (const type_21ProjectTypes_FileNameCS & fnm_s)
{
  ToolMediator::RemoveFiles ( fnm_s );

// 20120521 -->
#ifdef VDMPP
  TOOLS::CheckSSParseEnv();
#endif // VDMPP
  SET<TYPE_ProjectTypes_ModuleName> nms (PTAUX::ASNameSet2ModuleNameSet(ToolMediator::GetAllVDMModuleNames()));
  Generic mnm;
  for (bool cc = nms.First(mnm); cc; cc = nms.Next(mnm))
  {
    ToolMediator::UpdateTC (mnm, Quote (L"NONE"));
  }
// <-- 20120521

  // breakpoint
  // source
  TOOLS::set_spec_init( false ); // 20051222
}

// vdm_SaveProjectAs
Bool vdm_BaseTools::vdm_SaveProjectAs (const TYPE_ProjectTypes_FileName & pnm)
{
  Bool result(ToolMediator::SaveAs(pnm));
  if (result.GetValue())
  {
    wstring projectFile(PTAUX::ExtractFileName (pnm)); 
    wstring basedir (TBUTILS::tb_getbasedir(projectFile));
    TBUTILS::SetDefaultPath(basedir);
    TBUTILS::tb_chdir(basedir, vdm_log);
  }
  return result;
}

// vdm_SaveProject
Bool vdm_BaseTools::vdm_SaveProject (const TYPE_ProjectTypes_FileName & pnm)
{
  Bool result(ToolMediator::SaveAs(pnm));
  if (result.GetValue())
  {
    wstring projectFile(PTAUX::ExtractFileName (pnm)); 
    wstring basedir (TBUTILS::tb_getbasedir(projectFile));
    //TBUTILS::SetDefaultPath(basedir);
    TBUTILS::tb_chdir(basedir, vdm_log);
  }
  return result;
}

Generic vdm_BaseTools::vdm_GetProjectName()
{
  return ToolMediator::GetProjectName();
}

// PogGenerate
// mnm_l : seq1 of ModuleName
// ==> bool
Bool vdm_BaseTools::vdm_PogGenerate (const type_23ProjectTypes_ModuleNameCL & module_l)
{
  bool succ = true;

  if (!module_l.IsEmpty())
  {
    SEQ<TYPE_ProjectTypes_ModuleName> vdmModules;

    size_t len_module_l = module_l.Length();
    for (size_t index = 1; index<= len_module_l; index++)
    {
      const TYPE_ProjectTypes_ModuleName & mn (module_l[index]);
      status_type st = PTAUX::ExtractStatus(ToolMediator::Status(mn));
      switch (st.type) {
        case status_type::status_ok:
        case status_type::status_pos:
        case status_type::status_def: {
          break;
        }
        case status_type::status_error:
        case status_type::status_none:
        default: {
          vdmModules.ImpAppend(mn);
          break;
        }
      }
    }

    // save POS/DEF setting
    bool the_Setting_DEF = Settings.IsDEF();
    Settings.DefOff();
 
    if (!vdmModules.IsEmpty())
      succ = ToolMediator::BTools()->vdm_TypeCheck(vdmModules);
    else if (!module_l.IsEmpty()) // 20071025
      succ = true;

    // restore POS/DEF setting
    if(the_Setting_DEF)
      Settings.DefOn();
    else
      Settings.DefOff();

    if (succ)
    {
      ToolMediator::Interf()->vdm_CallBack(TYPE_ProjectTypes_PogCleanUp().Init(module_l));
      ToolMediator::Interf()->vdm_InitMeter((SEQ<Char>) Sequence(wstring(L"Generating integrity properties")), type_cL());
      ToolMediator::Interf()->vdm_SetMeterTotal(len_module_l);

      size_t total = 0;
      PogInterface & pog = GetPogInterface();
      for (size_t index = 1; index<= len_module_l; index++)
      {
        TYPE_ProjectTypes_String moduleName (module_l[index].get_nm());
        Sequence label(L"Generating integrity property for ");
        label.ImpConc(moduleName);
        ToolMediator::Interf()->vdm_IncrementMeter(label);

        vdm_log << L"Generating integrity property for " << moduleName.GetString() << L" ..." << endl;

        try
        {
          pog.setup();
          pog.genPO(mk_sequence(ASTAUX::MkNameFromId(moduleName, NilContextId)));
        }
        catch(TB_Exception & e)
        {
          vdm_log << L"Runtime Error: " << moduleName.GetString() << endl << flush;
          continue;
        }
        Sequence pogseq (pog.getTextPOs ());
        if (!pogseq.IsEmpty())
        {
          size_t len_pogseq = pogseq.Length();
          total += len_pogseq;
          if (len_pogseq == 1)
            vdm_log << L"1 property" << endl;
          else
            vdm_log << len_pogseq << L" properties" << endl;

          for (size_t idx = 1; idx <= len_pogseq; idx++)
          {
            Tuple t (pogseq[idx]);
            ToolMediator::Interf()->vdm_CallBack(TYPE_ProjectTypes_PogAdd().Init(t.GetSequence(1),
                                                                                 t.GetSequence(2),
                                                                                 t.GetSequence(3),
                                                                                 t.GetSequence(4),
                                                                                 t.GetSequence(5),
                                                                                 t.GetInt(6),
                                                                                 t.GetSequence(7),
                                                                                 t.GetSequence(12),
                                                                                 t.GetInt(8),
                                                                                 t.GetInt(9),
                                                                                 t.GetInt(10),
                                                                                 t.GetSequence(11)));
          }
        }
      }
      ToolMediator::Interf()->vdm_CallBack(TYPE_ProjectTypes_PogUpdateFilter());

      ToolMediator::Interf()->vdm_DestroyMeter();
      switch (total) {
        case 0: {
          vdm_log << L"total: no properties" << endl;
          break;
        }
        case 1: {
          vdm_log << L"total: 1 property" << endl;
          break;
        }
        default: {
          vdm_log << L"total: " << total << L" properties" << endl;
          break;
        }
      }
      vdm_log << L"done" << flush;
    }
  }
  return Bool(succ);
}

