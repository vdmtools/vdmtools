/***
*  * WHAT
*  *    Implementation of the CORBA API defined in corba_api.idl
*  *    
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/api/corba/corba_api.h,v $
*  * VERSION
*  *    $Revision: 1.14 $
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

#ifndef __corba_api_h__
#define __corba_api_h__

#include "projectval.h"
#include "corba_api.hh"
#include "internal_api.hh"
#include "object_map.h"
#include "settings.h"
#include <map>

// This file is included to get access to the *global* variables which
// is used to get and set the various options. The variables are named
// Setting_???

using namespace ToolboxAPI;

// This class is used to implement mutual exclusion of methods. The
// ctor of the TMutex class performs a lock on a global omni_mutex
// object, while the dtor performs an unlock on the same object. This
// class is used bu the MUTEX_METHOD macro defined below. The
// omni_mutex object MutexLock is declared in corba_api.cc
class TMutex {
public:
  TMutex(omni_mutex &m) : _m(m) { log(L"before lock"); _m.lock(); log(L"locked");}
  ~TMutex() { _m.unlock(); log(L"unlocked"); }
private:
  omni_mutex& _m;
  void log(const wchar_t *s) { 
    // cout << s << L" " << (void*)&_m << '\n'; 
  }
};

void ThrowApiError(const wchar_t * message, const wchar_t * location);

extern omni_mutex MutexLock;
extern omni_mutex EventQueuesLock; 

// Use the following define to declare an entire method as a protected
// region. MUTEX_METHOD should be used as the first instruction of the
// method.
#define MUTEX_METHOD TMutex _mutex_var(MutexLock)
#define MUTEX_EVENTQUEUE TMutex _mutex_evq_var(EventQueuesLock)

typedef map<VDM::ClientID, Sequence> ClientEventQueues;

class VDMApplication_i : public POA_ToolboxAPI::VDMApplication,
                         public PortableServer::RefCountServantBase
{
public:
  VDMApplication_i();
  virtual ~VDMApplication_i();

  VDMProject_ptr  GetProject () 
    { MUTEX_METHOD; return VDMProject::_duplicate(this->_proj); }
  VDMInterpreter_ptr  GetInterpreter ()
    { MUTEX_METHOD; return VDMInterpreter::_duplicate(this->_interp); }
  VDMCodeGenerator_ptr  GetCodeGenerator ()
    { MUTEX_METHOD; return VDMCodeGenerator::_duplicate(this->_cg); }
  VDMParser_ptr  GetParser ()
    { MUTEX_METHOD; return VDMParser::_duplicate(this->_parser); }
  VDMTypeChecker_ptr  GetTypeChecker ()
    { MUTEX_METHOD; return VDMTypeChecker::_duplicate(this->_tc); }
  VDMPrettyPrinter_ptr  GetPrettyPrinter ()
    { MUTEX_METHOD; return VDMPrettyPrinter::_duplicate(this->_pp); }
  VDMErrors_ptr  GetErrorHandler ()
    { MUTEX_METHOD; return VDMErrors::_duplicate(this->_err); }
  VDMModuleRepos_ptr  GetModuleRepos ()
    { MUTEX_METHOD; return VDMModuleRepos::_duplicate(this->_repos); }
  VDM::VDMFactory_ptr GetVDMFactory ()
    { MUTEX_METHOD; return VDM::VDMFactory::_duplicate(this->_fact); }
  void PushTag(VDM::ClientID);
  void DestroyTag(VDM::ClientID);

  VDM::ClientID Register ();
  void Unregister(VDM::ClientID id);
    
  ToolType Tool ();

  static void echo_object();

  static ObjectMap obj_map;

protected:
  VDMProject_var _proj;
  VDMInterpreter_var _interp;
  VDMCodeGenerator_var _cg;
  VDMParser_var _parser;
  VDMTypeChecker_var _tc;
  VDMPrettyPrinter_var _pp;
  VDMErrors_var _err;
  VDMModuleRepos_var _repos;
  VDM::VDMFactory_var _fact;
  VDM::ClientID _clientID;
  ClientEventQueues _event_queues;

  VDM::ClientID DoRegister ();

};

class VDMProject_i : public POA_ToolboxAPI::VDMProject,
                     public PortableServer::RefCountServantBase
{
public:
  VDMProject_i() {}
  ~VDMProject_i();
  void New ();
  void Open ( const char * );
  void Save ();
  void SaveAs ( const char * );
  CORBA::UShort GetModules ( ModuleList_out );
  CORBA::UShort GetFiles ( FileList_out );
  void AddFile ( const char * );
  void RemoveFile ( const char * );
};

class VDMInterpreter_i : public POA_ToolboxAPI::VDMInterpreter,
                         public PortableServer::RefCountServantBase
{
public:
  ~VDMInterpreter_i();
  VDMInterpreter_i();
  CORBA::Boolean DynTypeCheck () { MUTEX_METHOD; return Settings.DTC(); }
  void DynTypeCheck (CORBA::Boolean  _value)
  { MUTEX_METHOD; if (_value) Settings.DtcOn(); else Settings.DtcOff(); }

  CORBA::Boolean DynInvCheck () { MUTEX_METHOD; return Settings.INV(); }
  void DynInvCheck (CORBA::Boolean  _value);

  CORBA::Boolean DynPreCheck () { MUTEX_METHOD; return Settings.PreCheck(); }
  void DynPreCheck (CORBA::Boolean  _value)
  { MUTEX_METHOD; if (_value) Settings.PreOn(); else Settings.PreOff(); }

  CORBA::Boolean DynPostCheck () { MUTEX_METHOD; return Settings.PostCheck(); }
  void DynPostCheck (CORBA::Boolean  _value)
  { MUTEX_METHOD; if (_value) Settings.PostOn(); else Settings.PostOff(); }

  CORBA::Boolean PPOfValues () { MUTEX_METHOD; return Settings.PrintFormat(); }
  void PPOfValues (CORBA::Boolean  _value) 
  { MUTEX_METHOD; if (_value) Settings.PrintFormatOn();
                         else Settings.PrintFormatOff(); }

  CORBA::Boolean Verbose () { MUTEX_METHOD; return this->_verbose; }
  void Verbose (CORBA::Boolean _value) 
    { MUTEX_METHOD; this->_verbose = _value; }

  CORBA::Boolean Debug () { MUTEX_METHOD; return this->_debug; }
  void Debug (CORBA::Boolean _value) { MUTEX_METHOD; this->_debug = _value; }

  void Initialize ();
  VDM::VDMGeneric_ptr DoEval ( VDM::ClientID id, const char * expr );
  VDM::VDMGeneric_ptr EvalExpression ( VDM::ClientID id, const char *  expr );
  VDM::VDMGeneric_ptr Apply ( VDM::ClientID id, const char * f, VDM::VDMSequence_ptr arg );
  void EvalCmd ( const char *  cmd );
  CORBA::Long SetBreakPointByPos(const char* file, CORBA::Long line, CORBA::Long col);
  CORBA::Long SetBreakPointByName(const char* mod, const char* func);
  void DeleteBreakPoint (CORBA::Long num);
  VDM::VDMTuple_ptr StartDebugging (VDM::ClientID id, const char* expr);
  VDM::VDMTuple_ptr DebugStep (VDM::ClientID id);
  VDM::VDMTuple_ptr DebugStepIn (VDM::ClientID id);
  VDM::VDMTuple_ptr DebugSingleStep (VDM::ClientID id);
  VDM::VDMTuple_ptr DebugContinue (VDM::ClientID id);

private:
  VDM::VDMTuple_ptr DebugState(VDM::ClientID id, const Tuple & res);

protected:
  bool _verbose, _debug;
};

class VDMCodeGenerator_i : public POA_ToolboxAPI::VDMCodeGenerator,
                           public PortableServer::RefCountServantBase
{
 public:
  VDMCodeGenerator_i() { };
  ~VDMCodeGenerator_i();
  CORBA::Boolean  GeneratePosInfo () { MUTEX_METHOD; return Settings.GetCGRTI(); }
  void GeneratePosInfo (CORBA::Boolean  _value) 
    { MUTEX_METHOD; Settings.SetCGRTI((int) _value); }

  CORBA::Boolean  GenerateCode ( const char * name, 
                                 ToolboxAPI::VDMCodeGenerator::LanguageType targetLang);
  CORBA::Boolean  GenerateCodeList ( const ModuleList & names,
                                     ToolboxAPI::VDMCodeGenerator::LanguageType targetLang);
};

class VDMParser_i : public POA_ToolboxAPI::VDMParser,
                    public PortableServer::RefCountServantBase
{
public:
  VDMParser_i() {};
  ~VDMParser_i();
  CORBA::Boolean  Parse ( const char *  name );
  CORBA::Boolean  ParseList ( const FileList & names );
};

class VDMTypeChecker_i : public POA_ToolboxAPI::VDMTypeChecker,
                         public PortableServer::RefCountServantBase
{
public:
  VDMTypeChecker_i() {};
  ~VDMTypeChecker_i();

  CORBA::Boolean  DefTypeCheck () { MUTEX_METHOD; return Settings.IsDEF(); }
  void DefTypeCheck (CORBA::Boolean  _value) 
    { MUTEX_METHOD; _value ? Settings.DefOn() : Settings.DefOff(); }

  CORBA::Boolean  ExtendedTypeCheck () 
    { MUTEX_METHOD; return Settings.IsErrorLevelPRF(); }
  void ExtendedTypeCheck (CORBA::Boolean  _value) 
    { MUTEX_METHOD; _value ? Settings.ErrorLevelPRF()
                           : Settings.ErrorLevelWRN1(); }

  CORBA::Boolean  TypeCheck ( const char *  name );
  CORBA::Boolean  TypeCheckList ( const ModuleList & names );
};

class VDMPrettyPrinter_i : public POA_ToolboxAPI::VDMPrettyPrinter,
                           public PortableServer::RefCountServantBase
{
public:
  VDMPrettyPrinter_i() {};
  ~VDMPrettyPrinter_i();
  CORBA::Boolean  PrettyPrint ( const char * name );
  CORBA::Boolean  PrettyPrintList ( const FileList & names );
};

class VDMErrors_i : public POA_ToolboxAPI::VDMErrors,
                    public PortableServer::RefCountServantBase
{
public:
  VDMErrors_i() {};
  ~VDMErrors_i();
  CORBA::UShort  NumErr ();
  CORBA::UShort  NumWarn ();
  CORBA::UShort GetErrors ( ErrorList_out err );
  CORBA::UShort GetWarnings ( ErrorList_out err );

private:
  int ErrorsToIDL ( ErrorList *& err, const Sequence & vdmerr_l ) const;
};

class VDMModuleRepos_i : public POA_ToolboxAPI::VDMModuleRepos,
                         public PortableServer::RefCountServantBase
{
public:
  VDMModuleRepos_i() {};
  ~VDMModuleRepos_i();
  CORBA::UShort FilesOfModule ( FileList_out, const char * );
  void Status ( ModuleStatus &, const char * );
  CORBA::UShort SuperClasses ( ClassList_out classes, const char * name );
  CORBA::UShort SubClasses ( ClassList_out classes, const char * name );
  CORBA::UShort UsesOf ( ClassList_out classes, const char * name );
  CORBA::UShort UsedBy ( ClassList_out classes, const char * name );
  char* GetCurrentModule ();
  void PopModule ();
  void PushModule (const char* name);

  static int Set_to_FileList( FileList *& fl, const Set & files );
  static int Set_to_ModuleList( ModuleList *& fl, const Set & modules );
  static int Set_to_ClassList( ClassList *& fl, const Set & modules );

private:
  int GetStatus(status_type::status s) const;
};

///////////////////////////////////////////////////////////////
//
// The following classes implement the interfaces of internal_api.idl
//
///////////////////////////////////////////////////////////////

class Application_i : public VDMApplication_i
{
public:
  Application_i();
  virtual ~Application_i();

  VDM::ClientID RegisterEventListener();

  VDM::VDMSequence_ptr GetEvents ( VDM::ClientID );

  // The AddEvent method is only used _from_ inside the Toolbox. It is
  // used to add an event to all event queues. It is not exposed in
  // the idl interface and can consequently not be called by client
  // processes.
  void AddEvent(const Record & r);

  void ReleaseLock();
};

#endif // __corba_api_h__

