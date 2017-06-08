/***
*  * WHAT
*  *    Implementation of the CORBA API defined in corba_api.idl
*  * 
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/api/corba/corba_api.cc,v $
*  * VERSION
*  *    $Revision: 1.28 $
*  * DATE
*  *    $Date: 2006/06/06 01:09:54 $
*  * STATUS
*  *    Under development
*  * REFERENCES
*  * 
*  * PROJECT
*  *    INFORMA and Toolbox
*  * AUTHOR
*  *    Ole Storm + $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "corba_api.h"
#include "metaiv_impl.h"
#include "val2x.h"
#include "omniconfig.h"
#include "omnithread.h" // For omni_mutex
#include "cmd_queue.h" // for the command queue
#include "tb_wstring.h"
#include "tbutils.h"
#include "tools.h"

#include "init_corba.h" // Decl of poa

#include <iostream>
#include <string>


omni_mutex EventQueuesLock;
// One single instance of a condition variable.
omni_condition CondVar(&EventQueuesLock);

// One single instance of a mutex lock:
omni_mutex MutexLock;
// Enclose regions in MutexLock.lock()/MutexLock.unlock() to exclude
// other threads from mutaul execution of this region.

static std::string errmes;

void ThrowApiError(const wchar_t * message, const wchar_t * location)
{
  errmes = TBWSTR::wstring2utf8str(std::wstring(message));
  throw APIError(errmes.c_str());
}

// A dummy function used when redirecting the output to vdm_log
static int NullLog(const wchar_t * msg, int n)
{
  return 1;
}

//
// Implementation of Application_i
//

ObjectMap VDMApplication_i::obj_map(13);

VDMApplication_i::~VDMApplication_i()
{
#ifdef _PRINT_DESTRUCTION
  cerr << L"Destroying object VDMApplication_i" << endl;
#endif // _PRINT_DESTRUCTION
}

VDMApplication_i::VDMApplication_i()
{
  // Create all interface instances:
  PortableServer::POA_var poa = VDMCORBA::GetPOAvar();

  VDMProject_i *p = new VDMProject_i();
  //  p->_obj_is_ready(this->_boa());
  poa->activate_object(p);
  this->_proj = p->_this();

  VDMInterpreter_i *it = new VDMInterpreter_i();
  //  it->_obj_is_ready(this->_boa());
  poa->activate_object(it);
  this->_interp = it->_this();

  VDMCodeGenerator_i *cg = new VDMCodeGenerator_i();
  //  cg->_obj_is_ready(this->_boa());
  poa->activate_object(cg);
  this->_cg = cg->_this();

  VDMParser_i *ps = new VDMParser_i();
  //  ps->_obj_is_ready(this->_boa());
  poa->activate_object(ps);
  this->_parser = ps->_this();

  VDMTypeChecker_i *tc = new VDMTypeChecker_i();
  //  tc->_obj_is_ready(this->_boa());
  poa->activate_object(tc);
  this->_tc = tc->_this();

  VDMPrettyPrinter_i *pp = new VDMPrettyPrinter_i();
  //  pp->_obj_is_ready(this->_boa());
  poa->activate_object(pp);
  this->_pp = pp->_this();

  VDMErrors_i *err = new VDMErrors_i();
  //  err->_obj_is_ready(this->_boa());
  poa->activate_object(err);
  this->_err = err->_this();

  VDMModuleRepos_i *mr = new VDMModuleRepos_i();
  //  mr->_obj_is_ready(this->_boa());
  poa->activate_object(mr);
  this->_repos = mr->_this();

  VDMFactory_i *fact = new VDMFactory_i();
  //  fact->_obj_is_ready(this->_boa());
  poa->activate_object(fact);
  this->_fact = fact->_this();

  // The initial client identificator
  this->_clientID = INITIAL_CLIENT_ID;
}

void VDMApplication_i::PushTag(VDM::ClientID id)
{
  MUTEX_METHOD;

#ifndef DYN_CAST
  obj_map.Tag(id);
#else
  //throw APIError("PushTag() failed. No object table available.");
#endif //DYN_CAST
}

void VDMApplication_i::DestroyTag(VDM::ClientID id)
{
  MUTEX_METHOD;

#ifndef DYN_CAST
  obj_map.DestroyTag(id);
#else
  //throw APIError("DestroyTag() failed. No object table available.");
#endif //DYN_CAST
}

ToolType VDMApplication_i::Tool()
{
  MUTEX_METHOD;

#ifdef VDMSL
  return SL_TOOLBOX;
#endif //VDMSL
#ifdef VDMPP
  return PP_TOOLBOX;
#endif //VDMPP
}

void VDMApplication_i::Unregister(VDM::ClientID id)
  // remove the event queue possibly associated with client id. Remove
  // all objects owned by this client from the object table.
{
  MUTEX_EVENTQUEUE;

  ClientEventQueues::iterator eq_i = _event_queues.find(id);
  if(eq_i != _event_queues.end()){
    _event_queues.erase(eq_i);
  }
  obj_map.CleanUpClient(id);

  // Can be used for debugging:
  // obj_map.Echo();
}

VDM::ClientID VDMApplication_i::Register ()
{
  MUTEX_METHOD;

  return DoRegister();
}

VDM::ClientID VDMApplication_i::DoRegister ()
  // This registration function must NOT be declared as MUTEX!!! Used
  // by Register() and RegisterEventListener()
{
  VDM::ClientID tmp_id = this->_clientID++;

  // Register the client id in the object table:
  obj_map.Register(tmp_id);
  return tmp_id;
}

void VDMApplication_i::echo_object()
{
  VDMApplication_i::obj_map.Echo();
}

VDMProject_i::~VDMProject_i()
{
  MUTEX_METHOD;

#ifdef _PRINT_DESTRUCTION
  cerr << L"Destroying object VDMProject_i" << endl;
#endif // _PRINT_DESTRUCTION
}

VDMInterpreter_i::VDMInterpreter_i()
{
  MUTEX_METHOD;

  this->_verbose = false;
  this->_debug = false;
}

VDMInterpreter_i::~VDMInterpreter_i()
{
  MUTEX_METHOD;

#ifdef _PRINT_DESTRUCTION
  cerr << L"Destroying object VDMInterpreter_i" << endl;
#endif // _PRINT_DESTRUCTION
}

VDMCodeGenerator_i::~VDMCodeGenerator_i()
{
  MUTEX_METHOD;

#ifdef _PRINT_DESTRUCTION
  cerr << L"Destroying object VDMCodeGenerator_i" << endl;
#endif // _PRINT_DESTRUCTION
}

VDMParser_i::~VDMParser_i()
{
  MUTEX_METHOD;

#ifdef _PRINT_DESTRUCTION
  cerr << L"Destroying object VDMParser_i" << endl;
#endif // _PRINT_DESTRUCTION
}

VDMTypeChecker_i::~VDMTypeChecker_i()
{
  MUTEX_METHOD;

#ifdef _PRINT_DESTRUCTION
  cerr << L"Destroying object VDMTypeChecker_i" << endl;
#endif // _PRINT_DESTRUCTION
}

VDMPrettyPrinter_i::~VDMPrettyPrinter_i()
{
  MUTEX_METHOD;

#ifdef _PRINT_DESTRUCTION
  cerr << L"Destroying object VDMPrettyPrinter_i" << endl;
#endif // _PRINT_DESTRUCTION
}

VDMErrors_i::~VDMErrors_i()
{
  MUTEX_METHOD;

#ifdef _PRINT_DESTRUCTION
  cerr << L"Destroying object VDMErrors_i" << endl;
#endif // _PRINT_DESTRUCTION
}

VDMModuleRepos_i::~VDMModuleRepos_i()
{
  MUTEX_METHOD;

#ifdef _PRINT_DESTRUCTION
  cerr << L"Destroying object VDMModuleRepos_i" << endl;
#endif // _PRINT_DESTRUCTION
}

// Used by several methods to test if a given file exists
void checkValidFile ( const char * name, std::wstring func )
{
  FILE *exists = fopen(name, "r");
  if(exists != NULL)
    fclose(exists);
  else {
    std::wstring s = func +
                    std::wstring(L" failed. File does not exist: ") +
                    std::wstring(TBWSTR::fsstr2wstring(string(name)));
    ThrowApiError(s.c_str(), L"checkValidFile");
  }
}

// Creates a sequence of FileNames (wchar_t *) based on the set of file names passed to it.
int VDMModuleRepos_i::Set_to_FileList( FileList *& fl, const Set & files )
{
  fl = new FileList;
  fl->length(files.Card());
  int n = 0;
  Set files_q (files);
  Generic g;
  for(bool bb = files_q.First(g); bb; bb = files_q.Next(g))
    (*fl)[n++] = CORBA::string_dup(TBWSTR::wstring2utf8str(PTAUX::ExtractFileName(g)).c_str());
  return files.Card();
}

// Creates a sequence of ModuleNames (wchar_t *) based on the set of module names passed to it.
int VDMModuleRepos_i::Set_to_ModuleList( ModuleList *& fl, const Set & modules )
{
  fl = new ModuleList;
  fl->length(modules.Card());
  int n = 0;
  Set modules_q (modules);
  Generic g;
  for(bool bb = modules_q.First(g); bb; bb = modules_q.Next(g))
    (*fl)[n++] = CORBA::string_dup(TBWSTR::wstring2utf8str(PTAUX::ExtractModuleName(g)).c_str());
  return modules.Card();
}

// Creates a sequence of ClassNames (char *) based on the set of module names passed to it.
int VDMModuleRepos_i::Set_to_ClassList( ClassList *& fl, const Set & modules )
{
  fl = new ClassList;
  fl->length(modules.Card());
  int n = 0;
  Set modules_q (modules);
  Generic g;
  for(bool bb = modules_q.First(g); bb; bb = modules_q.Next(g))
    (*fl)[n++] = CORBA::string_dup(TBWSTR::wstring2utf8str(PTAUX::ExtractModuleName(g)).c_str());
  return modules.Card();
}

// Converts the status in s to a boolean value. 0 = none or error, 1 = OK
int VDMModuleRepos_i::GetStatus(status_type::status s) const
{
  switch (s) {
    case status_type::status_none  :
    case status_type::status_error : {
      return 0;
    }
    case status_type::status_ok :
    case status_type::status_pos :
    case status_type::status_def : {
      return 1;
    }
    default: {
      return 0;
    }
  }
}

/*
void checkValidModuleName(const char * name, std::string func)
  // Determines if the module 'name' is known to the toolbox.
{
  if(!((Set) Repos()->vdm_AllModules()).InSet(PTAUX::mk_ModuleName(TBWSTR::utf8str2wstring(string(name))))) {
    std::string s = func +
                    std::string(" failed. Module does not exist: ") +
                    std::string(name);
    ThrowApiError(s.c_str(), "checkValidModuleName");
  }
}
*/

void _flush_buffers()
{
  vdm_log.flush();
  vdm_err.flush();
}

//
// Implementation of methods of the Project interface:
//

void VDMProject_i::New()
{
  MUTEX_METHOD;

  APICommand command (VDM_PROJECT_NEW, Sequence());

  APICommandQueue::GetInstance()->EnqueueCommand (command);
}

void VDMProject_i::Open ( const char *  name )
{
  MUTEX_METHOD;

  APICommand command (VDM_PROJECT_OPEN, mk_sequence(PTAUX::mk_FileName(TBWSTR::utf8str2wstring(string(name)))));

  // add command to queue and wait for return
  APICommandQueue::GetInstance()->EnqueueCommand (command);
}

void VDMProject_i::Save ()
{
  MUTEX_METHOD;

  // following call does not influence the GUI, so we don't have to queue it
  Generic fileName (ToolMediator::GetProjectName());
  if (fileName.IsNil())
    ThrowApiError(L"VDMProject::Save() failed. Unable to save a project with no name. Use SaveAs in stead",
                  L"VDMProject_i::Save");

  APICommand command (VDM_PROJECT_SAVEAS, mk_sequence(fileName));

  // add command to queue and wait for return
  APICommandQueue::GetInstance()->EnqueueCommand (command);
}

void VDMProject_i::SaveAs ( const char *  name )
{
  MUTEX_METHOD;

  APICommand command (VDM_PROJECT_SAVEAS,
                      mk_sequence(PTAUX::mk_FileName(TBWSTR::utf8str2wstring(string(name)))));

  // add command to queue and wait for return
  APICommandQueue::GetInstance()->EnqueueCommand (command);
}

CORBA::UShort VDMProject_i::GetModules ( ModuleList_out ml )
{
  MUTEX_METHOD;

  APICommand command (VDM_PROJECT_GETMODULES, Sequence());

  Set modules (APICommandQueue::GetInstance()->EnqueueCommand (command));

  return VDMModuleRepos_i::Set_to_ModuleList(ml, modules);
}

CORBA::UShort VDMProject_i::GetFiles ( FileList_out fl)
{
  MUTEX_METHOD;

  APICommand command (VDM_PROJECT_GETFILES, Sequence());

  Set files (APICommandQueue::GetInstance()->EnqueueCommand (command));

  return VDMModuleRepos_i::Set_to_FileList(fl, files);
}

void VDMProject_i::AddFile ( const char *  name )
{
  MUTEX_METHOD;

  APICommand command (VDM_PROJECT_ADDFILE,
                      mk_sequence(PTAUX::mk_FileName(TBWSTR::utf8str2wstring(std::string(name)))));

  APICommandQueue::GetInstance()->EnqueueCommand (command);
}

void VDMProject_i::RemoveFile ( const char *  name )
{
  MUTEX_METHOD;

  APICommand command (VDM_PROJECT_REMOVEFILE,
                      mk_sequence(PTAUX::mk_FileName(TBWSTR::utf8str2wstring(std::string(name)))));

  APICommandQueue::GetInstance()->EnqueueCommand (command);
}

//
// Implementation of methods of the VDMModuleRepos interface:
//

CORBA::UShort VDMModuleRepos_i::FilesOfModule ( FileList_out fl, const char * name )
{
  MUTEX_METHOD;

  APICommand command (VDM_REPOSITORY_FILESOFMODULE,
                      mk_sequence(PTAUX::mk_ModuleName (TBWSTR::utf8str2wstring(std::string(name)))));

  Set files (APICommandQueue::GetInstance()->EnqueueCommand (command));

  return Set_to_FileList(fl, files);
}

void VDMModuleRepos_i::Status ( ModuleStatus & ms, const char *  name )
{
  MUTEX_METHOD;

  APICommand command (VDM_REPOSITORY_STATUS,
                      mk_sequence(PTAUX::mk_ModuleName (TBWSTR::utf8str2wstring(std::string(name)))));

  Generic status = APICommandQueue::GetInstance()->EnqueueCommand (command);

  status_type st = PTAUX::ExtractStatus(status);
  ms.SyntaxChecked = GetStatus(st.syntax);
  ms.TypeChecked = GetStatus(st.type);
  ms.CodeGenerated = GetStatus(st.cg);
  ms.PrettyPrinted = GetStatus(st.pp);
}

CORBA::UShort VDMModuleRepos_i::SuperClasses ( ClassList_out classes, const char *  name )
{
  MUTEX_METHOD;

#ifdef VDMSL
  ThrowApiError(L"VDMModuleRepos::SuperClasses() failed. This method is only available in the VDM++ Toolbox",
                L"VDMModuleRepos_i::SuperClasses");
  return 0; // dummy
#endif // VDMSL
#ifdef VDMPP
  APICommand command (VDM_REPOSITORY_SUPERCLASSES,
                      mk_sequence(PTAUX::mk_ModuleName (TBWSTR::utf8str2wstring(std::string(name)))));

  Set cl_s (APICommandQueue::GetInstance()->EnqueueCommand (command));

  return Set_to_ClassList(classes, cl_s);
#endif // VDMPP
}

CORBA::UShort VDMModuleRepos_i::SubClasses ( ClassList_out classes, const char * name )
{
  MUTEX_METHOD;

#ifdef VDMSL
  ThrowApiError(L"VDMModuleRepos::SubClasses() failed. This method is only available in the VDM++ Toolbox",
                L"VDMModuleRepos_i::SubClasses");
  return 0; // dummy
#endif // VDMSL
#ifdef VDMPP
  APICommand command (VDM_REPOSITORY_SUBCLASSES,
                      mk_sequence(PTAUX::mk_ModuleName (TBWSTR::utf8str2wstring(std::string(name)))));

  Set cl_s (APICommandQueue::GetInstance()->EnqueueCommand (command));

  return Set_to_ClassList(classes, cl_s);
#endif // VDMPP
}

CORBA::UShort VDMModuleRepos_i::UsesOf ( ClassList_out classes, const char * name )
{
  MUTEX_METHOD;

#ifdef VDMSL
  ThrowApiError(L"VDMModuleRepos::UsesOf() failed. This method is only available in the VDM++ Toolbox",
                L"VDMModuleRepos_i::UsesOf");
  return 0; // dummy
#endif // VDMSL
#ifdef VDMPP
  APICommand command (VDM_REPOSITORY_USES,
                      mk_sequence(PTAUX::mk_ModuleName (TBWSTR::utf8str2wstring(std::string(name)))));

  Set cl_s (APICommandQueue::GetInstance()->EnqueueCommand (command));

  return Set_to_ClassList(classes, cl_s);
#endif // VDMPP
}

CORBA::UShort VDMModuleRepos_i::UsedBy ( ClassList_out classes, const char * name )
{
  MUTEX_METHOD;

#ifdef VDMSL
  ThrowApiError(L"VDMModuleRepos::UsedBy() failed. This method is only available in the VDM++ Toolbox",
                L"VDMModuleRepos_i::UsedBy");
  return 0; // dummy
#endif // VDMSL
#ifdef VDMPP
  APICommand command (VDM_REPOSITORY_USEDBY,
                      mk_sequence(PTAUX::mk_ModuleName (TBWSTR::utf8str2wstring(std::string(name)))));

  Set cl_s (APICommandQueue::GetInstance()->EnqueueCommand (command));

  return Set_to_ClassList(classes, cl_s);
#endif // VDMPP
}

char* VDMModuleRepos_i::GetCurrentModule ()
{
  MUTEX_METHOD;

  APICommand command (VDM_REPOSITORY_GETCURRENTMOD, Sequence());

  TYPE_ProjectTypes_ModuleName res (APICommandQueue::GetInstance()->EnqueueCommand (command));

  string name (TBWSTR::wstring2utf8str(PTAUX::ExtractModuleName(res)));

  return CORBA::string_dup (name.c_str());
}

void VDMModuleRepos_i::PopModule ()
{
  MUTEX_METHOD;

  APICommand command (VDM_REPOSITORY_POPMODULE, Sequence());

  APICommandQueue::GetInstance()->EnqueueCommand (command);
}

void VDMModuleRepos_i::PushModule (const char* name)
{
  MUTEX_METHOD;

  APICommand command (VDM_REPOSITORY_PUSHMODULE,
                      mk_sequence(PTAUX::mk_ModuleName (TBWSTR::utf8str2wstring(std::string(name)))));

  APICommandQueue::GetInstance()->EnqueueCommand (command);
}

//
// Implementation of methods of the VDMInterpreter interface:
//

// If the DynInvCheck attribute is set, the attribute DynTypeCheck
// is also set to true.
void VDMInterpreter_i::DynInvCheck (CORBA::Boolean _value)
{
  MUTEX_METHOD;

  if(_value)
  {
    Settings.DtcOn();
    Settings.InvOn();
  }
  else
    Settings.InvOff();
}

void VDMInterpreter_i::Initialize ( )
{
  MUTEX_METHOD;

  APICommand command (VDM_INTERPRETER_INITIALIZE, Sequence());

  APICommandQueue::GetInstance()->EnqueueCommand (command);
}

// StartDebugging starts a debug session.
VDM::VDMTuple_ptr VDMInterpreter_i::StartDebugging ( VDM::ClientID id, const char * expr )
{
  MUTEX_METHOD;

  if(!_verbose) {
    ebuf.ReDirectLog(NullLog);
    lbuf.ReDirectLog(NullLog);
  }

  APICommand command (VDM_INTERPRETER_PARSEANDDEBUG,
                      mk_sequence(PTAUX::mk_ToolCommand (TBWSTR::MakeAsciiWstring(std::string(expr)))));

  Tuple res (APICommandQueue::GetInstance()->EnqueueCommand (command));

  if(!_verbose) {
    ebuf.RestoreLog();
    lbuf.RestoreLog();
  }
  return DebugState(id, res);
}

VDM::VDMGeneric_ptr VDMInterpreter_i::DoEval ( VDM::ClientID id, const char * expr )
  // This function must NOT be declared as MUTEX. It is used by
  // EvalExpression and Apply which are already declared as mutex.
{
  if(!_verbose) {
    ebuf.ReDirectLog(NullLog);
    lbuf.ReDirectLog(NullLog);
  }

  APICommand command (VDM_INTERPRETER_DOEVAL,
                      mk_sequence(PTAUX::mk_ToolCommand (TBWSTR::MakeAsciiWstring(expr))));

  Tuple res (APICommandQueue::GetInstance()->EnqueueCommand (command));

  if(!_verbose) {
    ebuf.RestoreLog();
    lbuf.RestoreLog();
  }

  // The tuple res is of type (Bool * Sequence). The first field
  // indicates if the expression was evaluated and the seccond fiels
  // is the semantic value of the result.
  if(res.GetBoolValue(1)) {
    const Sequence & v_lv (res.GetSequence(2));
    // We only consider the result of the *first* expression:
    VDM::VDMGeneric_ptr p = Generic_i::metaiv_to_idl(id, VAL2X::val2generic(v_lv[1], Set()));
    //    cout << "DoEval(): successful completion" << endl;
    return p;
  }
  else {
    std::wstring s (L"VDMInterpreter::DoEval() failed. The expression could not be evaluated: ");
    s += TBWSTR::utf8str2wstring(string(expr));
    //    cout << s << endl;
    ThrowApiError(s.c_str(), L"VDMInterpreter_i::DoEval");
    return (VDM::VDMGeneric_ptr)NULL;
  }
}

VDM::VDMGeneric_ptr VDMInterpreter_i::EvalExpression ( VDM::ClientID id, const char *  expr )
{
  MUTEX_METHOD;

  return DoEval(id, expr);
}

VDM::VDMGeneric_ptr VDMInterpreter_i::Apply ( VDM::ClientID id, const char * f, VDM::VDMSequence_ptr arg )
  // For the time being this function simply constructs a wstring
  // representing the application of f to the arguments and
  // subsequently calls DoEval. When the Language Adjustments
  // are implemented this function should be re-written.
{
  MUTEX_METHOD;

  Sequence_i *p = dyn_cast(Sequence_i, arg);

  if(p == NULL) {
    std::wstring msg (L"VDMInterpreter::Apply() failed. The argument list must be of type VDM::Sequence.");
    std::wcerr << msg << std::endl;
    ThrowApiError(msg.c_str(), L"VDMInterpreter_i::Apply");
  }

  std::wstring s (TBWSTR::utf8str2wstring(std::string(f)));
  Sequence arg_l = p->GetMetaivValue();
  s += L"(";
  size_t len_arg_l = arg_l.Length();
  for (size_t idx = 1; idx <= len_arg_l; idx++)
  {
    if (idx >= 2) s += L", ";
    s += arg_l[idx].ascii();
  }
  s += L")";

  return DoEval( id, TBWSTR::wstring2string(s).c_str() );
}

void VDMInterpreter_i::EvalCmd ( const char * cmd )
{
  MUTEX_METHOD;

  APICommand command (VDM_INTERPRETER_EVALCMD,
                      mk_sequence(PTAUX::mk_ToolCommand (TBWSTR::utf8str2wstring(std::string(cmd)))));

  APICommandQueue::GetInstance()->EnqueueCommand (command);
}

CORBA::Long VDMInterpreter_i::SetBreakPointByPos(const char* file, CORBA::Long line, CORBA::Long col)
{
  MUTEX_METHOD;

  APICommand command (VDM_INTERPRETER_SETBREAKBYPOS,
                      mk_sequence(PTAUX::mk_FileName (TBWSTR::utf8str2wstring(std::string(file))),
                                  Int(line),
                                  Int(col)));

  Int res (APICommandQueue::GetInstance()->EnqueueCommand (command));

  return (CORBA::Long)res.GetValue ();
}

CORBA::Long VDMInterpreter_i::SetBreakPointByName(const char* mod, const char* func)
{
  MUTEX_METHOD;

  APICommand command (VDM_INTERPRETER_SETBREAKBYNAME,
                      mk_sequence(PTAUX::mk_ModuleName(TBWSTR::utf8str2wstring(std::string(mod))),
                                  PTAUX::mk_Name(TBWSTR::utf8str2wstring(std::string(func)))));

  Int res (APICommandQueue::GetInstance()->EnqueueCommand (command));

  return (CORBA::Long)res.GetValue ();
}

void VDMInterpreter_i::DeleteBreakPoint (CORBA::Long num)
{
  MUTEX_METHOD;

  APICommand command (VDM_INTERPRETER_DELETEBREAK, mk_sequence(Int (num)));

  APICommandQueue::GetInstance()->EnqueueCommand (command);
}

VDM::VDMTuple_ptr VDMInterpreter_i::DebugStep (VDM::ClientID id)
{
  MUTEX_METHOD;

  APICommand command (VDM_INTERPRETER_DEBUGSTEP, Sequence());

  return DebugState(id, APICommandQueue::GetInstance()->EnqueueCommand (command));
}

VDM::VDMTuple_ptr VDMInterpreter_i::DebugStepIn (VDM::ClientID id)
{
  MUTEX_METHOD;

  APICommand command (VDM_INTERPRETER_DEBUGSTEPIN, Sequence());

  return DebugState(id, APICommandQueue::GetInstance()->EnqueueCommand (command));
}

VDM::VDMTuple_ptr VDMInterpreter_i::DebugSingleStep (VDM::ClientID id)
{
  MUTEX_METHOD;

  APICommand command (VDM_INTERPRETER_DEBUGSINGLESTEP, Sequence());

  return DebugState(id, APICommandQueue::GetInstance()->EnqueueCommand (command));
}

VDM::VDMTuple_ptr VDMInterpreter_i::DebugContinue (VDM::ClientID id)
{
  MUTEX_METHOD;

  APICommand command (VDM_INTERPRETER_DEBUGCONTINUE, Sequence());

  return DebugState(id, APICommandQueue::GetInstance()->EnqueueCommand (command));
}

VDM::VDMTuple_ptr VDMInterpreter_i::DebugState(VDM::ClientID id, const Tuple & res)
{
  // The tuple res is of type (Quote * Sequence). The first field
  // indicates if the expression was evaluated and the seccond fiels
  // is the semantic value of the result.
  Quote evalstate (res.GetField (1));
  Generic val (res.GetField(2));

  VDM::VDMGeneric_ptr g = Generic_i::metaiv_to_idl (id, Tuple(2)); // factory is unknown here, workaround
  VDM::VDMTuple_ptr p = VDM::VDMTuple::_narrow (g);
  CORBA::release (g);

  if((evalstate.GetValue() != L"ERROR") && val.IsSequence () && !Sequence(val).IsEmpty())
  {
    const Sequence & v_lv (res.GetField(2));
    // We only consider the result of the *first* expression:
    p->SetField (2, Generic_i::metaiv_to_idl(id, VAL2X::val2generic(v_lv[1], Set())));
  }
  p->SetField (1, Generic_i::metaiv_to_idl(id, evalstate));

  return p;
}

//
// Implementation of methods of the VDMCodeGenerator interface:
//

CORBA::Boolean VDMCodeGenerator_i::GenerateCode ( const char *  name,
                                   ToolboxAPI::VDMCodeGenerator::LanguageType targetLang )
{
  MUTEX_METHOD;

  int tag;
  switch (targetLang) {
    case ToolboxAPI::VDMCodeGenerator::JAVA: {
#ifdef VDMSL
      ThrowApiError(L"Generation of Java-Code is not supported by the VDM-SL Toolbox",
                    L"VDMCodeGenerator_i::GenerateCode");
#endif // VDMSL
      tag = VDM_CODEGENERATOR_CODEGENERATEJAVA;
      break;
    }
    case ToolboxAPI::VDMCodeGenerator::CPP:
    default: {
      tag = VDM_CODEGENERATOR_CODEGENERATECPP;
      break;
    }
  }

  APICommand command (tag, mk_sequence(PTAUX::mk_ModuleName(TBWSTR::utf8str2wstring(std::string(name)))));

  Bool res (APICommandQueue::GetInstance()->EnqueueCommand (command));

  return res.GetValue ();
}

CORBA::Boolean VDMCodeGenerator_i::GenerateCodeList ( const ModuleList & names,
                                       ToolboxAPI::VDMCodeGenerator::LanguageType targetLang )
{
  MUTEX_METHOD;

  Sequence s;
  for(unsigned int i = 0; i < names.length(); i++)
    s.ImpAppend(PTAUX::mk_ModuleName(TBWSTR::utf8str2wstring(std::string((const char *)names[i]))));

  int tag;

  switch (targetLang) {
    case ToolboxAPI::VDMCodeGenerator::JAVA: {
#ifdef VDMSL
      ThrowApiError(L"Generation of Java-Code is not supported by the VDM-SL Toolbox",
                    L"VDMCodeGenerator_i::GenerateCodeList");
#endif // VDMSL
      tag = VDM_CODEGENERATOR_CODEGENERATEJAVA;
      break;
    }
    case ToolboxAPI::VDMCodeGenerator::CPP:
    default: {
      tag = VDM_CODEGENERATOR_CODEGENERATECPP;
      break;
    }
  }
  APICommand command (tag, s);

  Bool res (APICommandQueue::GetInstance()->EnqueueCommand (command));

  return res.GetValue ();
}

//
// Implementation of methods of the VDMParser interface:
//

CORBA::Boolean VDMParser_i::Parse ( const char * name )
{
  MUTEX_METHOD;

  APICommand command (VDM_PARSER_PARSE,
                      mk_sequence(PTAUX::mk_FileName(TBWSTR::utf8str2wstring(std::string(name)))));

  Bool res (APICommandQueue::GetInstance()->EnqueueCommand (command));

  _flush_buffers();
  return res.GetValue();
}

CORBA::Boolean VDMParser_i::ParseList ( const FileList & names )
{
  MUTEX_METHOD;

  Sequence s;
  for(unsigned int i = 0; i < names.length(); i++) {
    s.ImpAppend(PTAUX::mk_FileName(TBWSTR::utf8str2wstring(std::string((const char *)names[i]))));
  }
  APICommand command (VDM_PARSER_PARSE, s);

  Bool res (APICommandQueue::GetInstance()->EnqueueCommand (command));

  _flush_buffers();
  return res.GetValue();
}

//
// Implementation of methods of the VDMTypeChecker interface:
//

CORBA::Boolean VDMTypeChecker_i::TypeCheck ( const char * name )
{
  MUTEX_METHOD;

  APICommand command (VDM_TYPECHECKER_TYPECHECK,
                      mk_sequence(PTAUX::mk_ModuleName(TBWSTR::utf8str2wstring(std::string(name)))));

  Bool res (APICommandQueue::GetInstance()->EnqueueCommand (command));

  _flush_buffers();
  return res.GetValue();
}
 
CORBA::Boolean VDMTypeChecker_i::TypeCheckList ( const ModuleList & names )
{
  MUTEX_METHOD;

  Sequence s;
  for(unsigned int i = 0; i < names.length(); i++){
    s.ImpAppend(PTAUX::mk_ModuleName(TBWSTR::utf8str2wstring(std::string((const char *)names[i]))));
  }
  APICommand command (VDM_TYPECHECKER_TYPECHECK, s);

  Bool res (APICommandQueue::GetInstance()->EnqueueCommand (command));

  _flush_buffers();
  return res.GetValue();
}

CORBA::Boolean VDMPrettyPrinter_i::PrettyPrint ( const char * name )
{
  MUTEX_METHOD;

  APICommand command (VDM_PRETTYPRINTER_PRETTYPRINT,
                      mk_sequence(PTAUX::mk_FileName(TBWSTR::utf8str2wstring(std::string(name)))));

  Bool res (APICommandQueue::GetInstance()->EnqueueCommand (command));

  return res.GetValue();
}

CORBA::Boolean VDMPrettyPrinter_i::PrettyPrintList ( const FileList & names )
{
  MUTEX_METHOD;

  Sequence s;
  for(unsigned int i = 0; i < names.length(); i++) {
    s.ImpAppend(PTAUX::mk_FileName(TBWSTR::utf8str2wstring(std::string((const char *)names[i]))));
  }

  APICommand command (VDM_PRETTYPRINTER_PRETTYPRINT, s);

  Bool res (APICommandQueue::GetInstance()->EnqueueCommand (command));

  return res.GetValue();
}

CORBA::UShort VDMErrors_i::NumErr ()
{
  MUTEX_METHOD;

  APICommand command (VDM_ERRORS_GETSTATUS, Sequence());

  Tuple st (APICommandQueue::GetInstance()->EnqueueCommand (command));

  return st.GetInt(1).GetValue();
}

CORBA::UShort VDMErrors_i::NumWarn ()
{
  MUTEX_METHOD;

  APICommand command (VDM_ERRORS_GETSTATUS, Sequence());

  Tuple st (APICommandQueue::GetInstance()->EnqueueCommand (command));

  return st.GetInt(2).GetValue();
}

int VDMErrors_i::ErrorsToIDL ( ErrorList *& err, const Sequence & vdmerr_l ) const
{
  err = new ErrorList;
  err->length(vdmerr_l.Length());

  int len = vdmerr_l.Length();
  for (int index = 0; index < len; index++)
  {
    const TYPE_ProjectTypes_Message & r (vdmerr_l[index + 1]);

    // Extract file name and position information:
    switch (r.GetTag()) {
      case TAG_TYPE_ProjectTypes_ErrMsg: {
        Error item;
        int fileid = r.GetInt (vdm_ProjectTypes::pos_ErrMsg_fid).GetValue();
        std::wstring file (ToolMediator::GetFileName (fileid));
        item.fname = CORBA::string_dup(TBWSTR::wstring2utf8str(file).c_str());
        item.line = r.GetInt (vdm_ProjectTypes::pos_ErrMsg_line).GetValue();
        item.col = r.GetInt (vdm_ProjectTypes::pos_ErrMsg_col).GetValue();
 
        // Extract the error message:
        const type_cLL & descr_l (r.GetSequence(vdm_ProjectTypes::pos_ErrMsg_msg));

        std::wstring s;
        size_t len_descr_l = descr_l.Length();
        for (size_t idx = 1; idx <= len_descr_l; idx++)
          s += descr_l[idx].GetString() + L" "; 
        item.msg = CORBA::string_dup(TBWSTR::wstring2utf8str(s).c_str());
        (*err)[index] = item;
        break;
      }
      case TAG_TYPE_ProjectTypes_WarnMsg: {
        Error item;
        int fileid = r.GetInt (vdm_ProjectTypes::pos_WarnMsg_fid).GetValue();
        std::wstring file (ToolMediator::GetFileName (fileid));
        item.fname = CORBA::string_dup(TBWSTR::wstring2utf8str(file).c_str());
        item.line = r.GetInt (vdm_ProjectTypes::pos_WarnMsg_line).GetValue();
        item.col = r.GetInt (vdm_ProjectTypes::pos_WarnMsg_col).GetValue();
 
        // Extract the error message:
        const type_cLL & descr_l (r.GetSequence(vdm_ProjectTypes::pos_WarnMsg_msg));

        std::wstring s;
        size_t len_descr_l = descr_l.Length();
        for (size_t idx = 1; idx <= len_descr_l; idx++)
          s += descr_l[idx].GetString() + L" "; 
        item.msg = CORBA::string_dup(TBWSTR::wstring2utf8str(s).c_str());
        (*err)[index] = item;
        break;
      }
    }
  }
  return err->length();
}


CORBA::UShort VDMErrors_i::GetErrors ( ErrorList_out err )
{
  MUTEX_METHOD;

  APICommand command (VDM_ERRORS_GETERRORS, Sequence());

  SEQ<TYPE_ProjectTypes_ErrMsg> res (APICommandQueue::GetInstance()->EnqueueCommand (command));

  return ErrorsToIDL(err, res);
}

CORBA::UShort VDMErrors_i::GetWarnings ( ErrorList_out err )
{
  MUTEX_METHOD;

  APICommand command (VDM_ERRORS_GETWARNINGS, Sequence());

  SEQ<TYPE_ProjectTypes_WarnMsg> res (APICommandQueue::GetInstance()->EnqueueCommand (command));

  return ErrorsToIDL(err, res);
}

///////////////////////////////////////////////////////////////
//
// The following are implementations of the interfaces of
// internal_api.idl
//
///////////////////////////////////////////////////////////////

// Adds the event r to _all_ event queues
void Application_i::AddEvent(const Record & r)
{
  MUTEX_EVENTQUEUE;

  ClientEventQueues::iterator eq_i = _event_queues.begin();
  while(eq_i != _event_queues.end() ) {
    // Append the event
    (*eq_i).second.ImpAppend(r);
    ++eq_i;
  }
  // Wake up one all the threads that may be waiting for events:
  CondVar.broadcast();
}

// Returns thw sequence of events associated with client id. After
// the call the event queue is emptied.
VDM::VDMSequence_ptr Application_i::GetEvents ( VDM::ClientID id )
{
  MUTEX_EVENTQUEUE;

  VDM::VDMSequence_ptr p = NULL;

  ClientEventQueues::iterator eq_i = _event_queues.find(id);
  if(eq_i != _event_queues.end())
  {
    if(!(*eq_i).second.Length())
    {
      // There are not any events yet, wait until one event arrive:
      CondVar.wait();
    }
    // Get the list of events and return:
    p = VDM::VDMSequence::_narrow(Generic_i::metaiv_to_idl(id, (*eq_i).second));
    CORBA::release(p); // necessary because _narrow() increments the reference count
    (*eq_i).second = Sequence();
  }
  else{
    ThrowApiError(L"Application::GetEvents() failed. Client id unknown. Event listeners must be registered by a call to RegisterEventListener().", 
                  L"GetEvents");
  }
  return p;
}

void Application_i::ReleaseLock()
  // Will set the event queue to the empty sequence for all queues and
  // then signal all waiting processes. This will then be interpreted
  // as a request to the event listener to terminate its main event
  // loop. This is necessary to terminate the toolbox properly.
{
  MUTEX_EVENTQUEUE;

  ClientEventQueues::iterator eq_i = _event_queues.begin();
  while(eq_i != _event_queues.end() ) {
    // Reset the event queue:
    (*eq_i).second = Sequence();
    ++eq_i;
  }
  // Wake up one all the threads that may be waiting for events:
  CondVar.broadcast();
}

VDM::ClientID Application_i::RegisterEventListener()
{
  MUTEX_EVENTQUEUE;

  // First register as usual:
  VDM::ClientID id = DoRegister();
  // Create a new event queue for the client:
  _event_queues[id] = Sequence();

  return id;
}

Application_i::Application_i()
{}

Application_i::~Application_i()
{}

