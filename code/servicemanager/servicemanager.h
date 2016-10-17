#ifndef _SERVICEMANAGER_H_
#define _SERVICEMANAGER_H_

#include "tb_dlfcn.h"
#include "cli_options.h"
#include "externalservicemanager.h"
#include "ServiceManagerDesc.h"
#include <map>
#include "omniconfig.h"
#include "omnithread.h"
#include "metaiv.h"

class ExternalServiceHandle;

class ServiceManager : public ExternalServiceManager {

 public:
  ServiceManager();
  void CLIStartUp(const CLIOptions&);
  void CLIRun();
  ExternalService* RequestExternalService(const wchar_t*) { return (ExternalService*) 0;}
  InternalService* RequestInternalService(const wchar_t*);
  void RegisterInternalService(InternalService*);
  

 private:
  // Methods
  bool ReadInitFile();
  void RegisterExternalService(const TYPE_ServiceManagerDesc_ServiceDescription&);
  void AddExternalService(ExternalServiceHandle*);

  // Attributes  
  wstring m_strInitFileName;
  Sequence externalServicesList;
  std::map<wstring, ExternalServiceHandle*> esMap;
  std::map<wstring, InternalService*> isMap;
  
};

// Definitions for external services
typedef ExternalService* (*fun_es_register)(const ExternalServiceManager*);
typedef void (*fun_es_deregister)();

class ExternalServiceHandle : public omni_thread {

 public:
  ExternalServiceHandle(const ExternalServiceManager*);
  void run(void* /*arg*/);
  void* run_undetached(void* arg){ run( arg ); return NULL; } ;
  const wstring GetName();
  ExternalService* GetService();
  bool Init(const wstring &, const wstring &);
  wstring GetDlError();

 private:
  bool CheckPointer(void *, const wchar_t *);

  wstring name;
  wstring location;
  DLL_HANDLE handle;
  fun_es_register registerPtr;
  fun_es_deregister deregisterPtr;
  ExternalService* service;
  const ExternalServiceManager* manager;
};

#endif //_SERVICEMANAGER_H_


