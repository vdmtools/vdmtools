/***
*  * WHAT
*  *    Top level interface to the base tools in the Toolbox
*  *    (User's of the base tools (syntax chec
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/utils/tools.h,v $
*  * VERSION
*  *    $Revision: 1.78 $
*  * DATE
*  *    $Date: 2006/06/12 08:36:17 $
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

#ifndef __tools_h__
#define __tools_h__

#include "metaiv.h"
#include "contextinfo.h" 

#include "ToolMediator.h"
#include "Errors.h"
#include "ErrorState.h"
#include "Repository.h"
#include "Interface.h"
#include "UMLTool.h"

#include "cli_options.h"
#include <string>

class ToolMediator
{
#ifdef VDMSL
enum {
  OriginTag    = TAG_TYPE_TOOLS_OriginTag,
  OriginTypes  = 1,  // map name to fileid  (fileid = nat)
  OriginState  = 2,  // fileid              (-1 => no state)
  OriginValues = 3,  // map name to fileid
  OriginFcts   = 4,  // map name to fileid
  OriginOps    = 5,  // map name to fileid
  OriginTraces = 6,  // map name to fileid
  OriginLength = 6
};
#endif // VDMSL

public:
  // Mediator
  static void SetMediator (vdm_ToolMediator * m);
  static vdm_Errors * Errs ();
  static vdm_Errors * ExprErrs ();
  static vdm_Repository * Repos ();
  static vdm_Interface * Interf ();
  static vdm_BaseTools * BTools ();
  static vdm_UMLTool * UMLT ();
  static SEQ<Record> GetAllASTs ();
  static SEQ<Record> GetJavaASTs();
  static SEQ<Record> GetVDMASTs();
  static wstring GetFileName(int fileid); 
  //
  static Tuple GetFileMapNameNum(const SET< SEQ<Char> > &, bool); 
  static MAP<TYPE_ProjectTypes_FileId, SEQ<Char> > GetFileMapNumName(); 
  // 
  static bool CheckModified();
  static bool IsFileModified(const TYPE_ProjectTypes_FileName & fnm);
  static SET<TYPE_ProjectTypes_FileName> CheckForModifiedFiles ();
  static Set GetAstsOfFileName (const TYPE_ProjectTypes_FileName & filename);
  //
  static void ResetTCStatus();
  //
#ifdef VDMPP
  static Bool EvalDependInfo (const TYPE_ProjectTypes_ModuleName & nm);
  static Bool EvalDependUpdate (const SET<TYPE_ProjectTypes_ModuleName> & modules);
  static bool EvalInhTree();
#endif // VDMPP
  static Bool UpdateSelections ();
  static bool UpdateRepository (const TYPE_ProjectTypes_FileName & filename,
                                int file_id,
                                const Record & new_ast,
                                const Sequence & astseq,
                                bool isJava);
  static SET<TYPE_AS_Name> GetAllVDMModuleNames();

  static void InitCoverageFile(const wstring & coverage_file, ContextInfo & ci);
  static void SaveCoverageCounters(const wstring & coverage_file, ContextInfo & ci);
  static void ResetTestCoverage (ContextInfo & ci);
  static void LoadTestCoverageFile(const wstring &, ContextInfo &, const SET< SEQ<Char> > &, bool);

  static void PrintAstTuple(ostream& f, const Generic & ast_or_expr, ContextInfo & ci, const Map & fid_m); 

  static Tuple GetModulePosInfo(const TYPE_ProjectTypes_ModuleName & modnm);
  static Tuple CidToPosInfo(const TYPE_CI_ContextId & cid);
  static Tuple GetFnOpPosInfo(const wstring & fnopnm);

  static Bool NewUnnamedProject ();
  static Bool Open (const TYPE_ProjectTypes_FileName &);
  static Bool SaveAs (const TYPE_ProjectTypes_FileName &);
  static Generic GetProjectName ();
  static void ClearAll ();
  static void AddFiles (const SET<TYPE_ProjectTypes_FileName> &);
  static void RemoveFiles (const SET<TYPE_ProjectTypes_FileName> &);
  static void UpdateSC (const TYPE_ProjectTypes_FileName &, const Generic &);
  static void UpdateTC (const TYPE_ProjectTypes_ModuleName &, const Generic &);
  static void UpdateCG (const TYPE_ProjectTypes_ModuleName &, const Generic &, const TYPE_ProjectTypes_Succes &);
  static void UpdatePP (const TYPE_ProjectTypes_FileName &, const TYPE_ProjectTypes_Succes &);
  static TYPE_ProjectTypes_FileId GetFileId (const TYPE_ProjectTypes_FileName &);
  static TYPE_ProjectTypes_FileName GetVDMTempFileName(const TYPE_ProjectTypes_FileName & fnm);
  static Tuple Status (const Record &);
  static Generic GetAST (const TYPE_ProjectTypes_ModuleName &);
  static SET<TYPE_ProjectTypes_FileName> Files ();
  static SET<TYPE_ProjectTypes_FileName> ParsedFiles ();
  static SET<TYPE_ProjectTypes_ModuleName> AllModules();
  static SET<TYPE_ProjectTypes_ModuleName> VDMModules();
  static SET<TYPE_ProjectTypes_FileName> FileOfModule (const TYPE_ProjectTypes_ModuleName &);
  static SET<TYPE_ProjectTypes_ModuleName> ModulesInFile (const TYPE_ProjectTypes_FileName &);
  static Map GetFilesAndPackages (const Char &);
  static TYPE_ProjectTypes_FileName GetPackageOfFile (const TYPE_ProjectTypes_FileName &, const Char &);
  static void SetFileTimestamp (const TYPE_ProjectTypes_FileName &, const Int &);
  static Bool StateIsSaved ();
  static void SetSavedFileState (const Bool &);
  static void EnableSession ();
  static void DisableSession ();

  static bool SyntaxCheckOk (const TYPE_ProjectTypes_FileName & filename);
#ifdef VDMSL
private:
  static Tuple RemoveConstructs (int file_id, const Record & Origin, const TYPE_AS_Definitions & AST) ;
  static Record EmptyOrigin ();
  static TYPE_AS_Definitions EmptyDefinitions ();
  static TYPE_AS_Module mk_DefaultMod (const TYPE_AS_Definitions & def);
  static void UpdateAST (int file_id, const TYPE_AS_Definitions & new_ast, Record & Origin, TYPE_AS_Definitions & AST);
#endif // VDMSL

private:
  static vdm_ToolMediator * mediator;
};

class TOOLS
{
public:
  static void set_spec_init(bool);
  static bool isBatchMode();
  static void setBatchMode(bool mode);
  static bool isTracesMode();
  static void setTracesMode(bool mode);
  static Tuple ParseExprs (const Record & Expr, wostream & wos);
  static Tuple ParseExprsString (const string & str);
#ifdef VDMPP
  static Tuple ParseAssignStmt(const string & str);
#endif // VDMPP
  static bool ParseCommand (const wstring & cmd, const wstring & args);
  static bool Execute (const wstring &, const wstring &);
  static bool ResetErrors(const wstring & command, const wstring & args);
  static Bool ExecuteCommand (const TYPE_ProjectTypes_ToolCommand & ToolCommand);

  static void SetIsInScript(bool isin);
  static bool IsInScript();
  static bool get_ast_is_new();
#ifdef VDMPP
  static void set_dobjs_init(bool v);
  static bool get_dobjs_init();
  static void SetPriorityFile(const wstring & filename);
  static wstring GetPriorityFile();
#ifdef VICE
  static void EvalTimefile(const wstring & args);
  static void SetLogFile(const wstring & filename);
  static wstring GetLogFile();
  static void SetTimeFile(const wstring & filename);
  static wstring GetTimeFile();
#endif // VICE
#endif // VDMPP

  static int CheckAbbrev (const Text & fullname, const Set & abbreviations);
  static void AddCommand (const wchar_t * forms_);
  static void InitDebugCommands ();
  static wstring ExpandCommand (wstring & what_the_user_wrote);

  static bool ReadStartUpFile ();
  static bool ReadScriptFile (const wstring& short_name);

  static void InitModuleTypes();
  static void InitToolbox (bool batch_mode);
  static void ExitToolbox(int i);
  static void ExecuteMainLoop();

  static bool InitCorbaApi(int argc, char *argv[], wstring &err);
  static void TerminateCorbaApi();

  static void ExternalParseService(const CLIOptions & thisCliOpt);

#ifdef VDMPP
  static void SetPrimarySchedulingAlgorithm(const wstring &);
  static void SetInterfacesOption(const wstring & interfacesList);
  static wstring GetInterfacesOption();
#endif // VDMPP

// from BaseTools
  static bool EvalRead (const TYPE_ProjectTypes_FileName & filename);
  static bool EvalTypeCheck (const TYPE_ProjectTypes_ModuleName & nm, int opt, wostream & wos);
  static bool EvalLatex (const TYPE_ProjectTypes_FileName & filename);
#ifdef VDMPP
  static bool EvalJavaParse(const TYPE_ProjectTypes_FileName &);
  static bool EvalJavaTypeCheck(const TYPE_ProjectTypes_ModuleName &);
  static bool EvalJavaGenerateVDM(const TYPE_ProjectTypes_ModuleName &,
                                  const Bool &,
                                  const Bool &,
                                  const Bool &);
  static void AfterJavaTypeCheck();
#endif // VDMPP
  static void UpdateToolbox ();

// interface to statsem
  static Tuple InitStatSem(const Record & ast_val);
  static void ResetSSNames();
  static Generic DependencyFindType (const TYPE_AS_Name &,const TYPE_AS_Name &);
  static int OutputStatSemErrors(const Generic &);
//void ResetTCStatus();
  static void UpdateSS(const Record & mod);
#ifdef VDMPP
  static void CheckSSParseEnv();
  static Tuple SaveTypeCheckEnv ();
  static Bool LoadTypeCheckEnv (const Tuple &);
  static void ExpandClassTC(const TYPE_AS_Class &);
#endif //VDMPP

// from GUI
  static Sequence getGUITokenInfo( const SEQ<Char> & filename );
  static Sequence SearchId(const SEQ<TYPE_ProjectTypes_FileName> & files, const wstring & name, bool partial, bool defOnly);
  static void api_Periodic();
  static void CleanUp();

#ifdef VDMPP
  static void EvalDestroyAllObjects();
  static Tuple GetOperations(const wstring & clsnm);
#endif // VDMPP
  static void BreakIP();
  static wstring GetTraceArgs(int index);

// from corba
  static wstring ReadLine ();

  static bool ifstr2ufile( ifstream & ifs, string & ufname );
  static bool ifstr2ustr( ifstream & ifs, ostream & ostr );

  static wstring giveToolTitle();
  static wstring giveToolVersionDate();
  static wstring getVersionId();
  static wstring getCredit();
  static bool canCG();
  static bool canJavaCG();
  static bool canJ2V();
  static bool isDLModuleEnabled();
  static void OpenCallLog();
  static void CloseCallLog();
  static void WriteCallLog(const wstring & id, const Sequence & arg_l, int level);

  static void ExecuteMainLoop (bool useCorba);

  static void EvalDir(const wstring & args, wostream & wos);

  static void EvalSet (const wstring & opts, wostream & wos);
  static bool EvalSet_Check_NoOf_Args(const wstring & opts, int nos, int nos_expected, wostream & wos);
  static void EvalUnSet (const wstring & opt, wostream & wos);
  static void EvalNoCheck (const wstring & opts, wostream & wos);
  static void EvalSearch (const wstring & opts, wostream & wos);
#ifdef VDMPP
  static bool EvalProcessCmd (const TYPE_ProjectTypes_ModuleName &, int, wostream & wos);
  static bool EvalArgsProcess(const wstring & args, wostream & wos);
  static bool ConditionalParse(const TYPE_ProjectTypes_ModuleName& clnm);
  static bool ParsAllSupers(const TYPE_ProjectTypes_ModuleName & clnm);
#endif // VDMPP

  static bool LocalEvalTypeCheck (const TYPE_ProjectTypes_ModuleName & modnm, int opt, wostream & wos);

  static void EvalBreak(const wstring & args, wostream & wos);

  static void DisplayAST(const wstring & args, wostream & wos);
  static void DisplayContextInfo(const wstring & args, bool token, bool node, wostream & wos);
  static void DisplayVersion(wostream & wos);
  static void GoFindType(const wstring & args, wostream & wos);

  static Sequence SearchIdDef(const SEQ<TYPE_ProjectTypes_FileName> & files, const wstring & name, bool partial);
  static void PrintConsistencyStatus();

#ifdef VDMSL
  static void EvalModuleStack (wostream & wos);
#endif // VDMSL
#ifdef VDMPP
#ifdef VICE
  static void OutputTimemap(wostream & os, const Map & tm);
#endif // VICE
#endif // VDMPP

  static wstring GenBlanks(int n);
  static void EvalHInfo (wostream & wos);
  static void EvalInfo (wostream & wos);
  static int EvalHelp (const wstring & cmd, wostream & wos);

  static void cmd_tcov(const wstring & args, wostream & wos);

private:
  static bool isinscript;
  static bool spec_init;
#ifdef VDMPP
  static bool dobjs_init;
#endif // VDMPP
  static bool BATCHMODE;
  static bool TRACESMODE;

#ifdef VDMPP
  static wstring priorityfilename; // stores the name of the last priority file loaded
#endif //VDMPP

#ifdef VICE
  static wstring timefilename; // stores the name of the last timefile loaded
  static wstring logfilename;  // stores the name of the last logfile name set
#endif // VICE
  static Map abbrev;

  static ofstream calllogstream;
};

#endif // __tools_h__

