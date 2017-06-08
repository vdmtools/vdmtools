#include "servicemanager.h"
#include "service_startup_aux.h"
#include "tbutils.h"
#include "tools.h"
#include "tb_wstring.h"

ServiceManager::ServiceManager() :
#ifdef VDMPP
  m_strInitFileName(L".vpp_startup")
#endif //VDMPP
#ifdef VDMSL
  m_strInitFileName(L".vdm_startup")
#endif //VDMSL
{
}

void ServiceManager::CLIStartUp(const CLIOptions& cliOpt)
{
  wcout << L"Initializing Service Manager" << endl;
  ReadInitFile();
}

void ServiceManager::CLIRun()
{
  wcout << L"Started Running External Service" << endl;
  map<wstring, ExternalServiceHandle*>::iterator it;
  ExternalServiceHandle *esh;
  for (it = esMap.begin (); it != esMap.end (); ++it)
  {
    esh = (*it).second;
    esh->run(NULL);
  }
  wcout << L"Finished Running External Service" << endl;
}

void ServiceManager::RegisterInternalService(InternalService* parmIs)
{
  wcout << L"Registering Internal Service " << parmIs->GetName(); 
  map<wstring, InternalService*>::iterator it;

  if ((it = isMap.find (parmIs->GetName())) != isMap.end ())
    ; 
  else {
    isMap.insert(map<wstring,InternalService*>::value_type(parmIs->GetName(), parmIs));
  }
}

InternalService* ServiceManager::RequestInternalService(const wchar_t* parmName)
{
  InternalService * thisIs = 0;
  map<wstring, InternalService*>::iterator it;

  if ((it =isMap.find (parmName)) != isMap.end ())
    thisIs = (*it).second;
  return thisIs;
}

bool ServiceManager::ReadInitFile()
{
  if (TBUTILS::file_exists(m_strInitFileName)) {
    externalServicesList  = smd_main(&wcout, m_strInitFileName.c_str());
  }
  else {
    wcout << L"No service startup file found" << endl;
    return false;
  }
  
  Generic g;
  for (bool bb = externalServicesList.First(g); bb; bb = externalServicesList.Next(g))
  {
    TYPE_ServiceManagerDesc_ServiceDescription sd (g);
    RegisterExternalService(g);
  }
  return true;
}

void ServiceManager::RegisterExternalService(const TYPE_ServiceManagerDesc_ServiceDescription & sd)
{
  wstring path(L""), pathComponent;
  wstring name;
  Generic g;
  Sequence seqName (sd.get_name());
  seqName.GetString(name);
  Sequence seqLocation (sd.get_location());
  for (bool bb = seqLocation.First(g); bb; bb = seqLocation.Next(g))
  {
    Sequence seqComponent (g);
    seqComponent.GetString(pathComponent);
    path += L"/" + pathComponent;
  }

  if (TBUTILS::file_exists(path)) 
    wcout << L"Registering service " << name << L" at location " 
         << path << endl;
  else
    wcout << L"Unable to register service " << name << L" - " << path 
         << L" does not exist" << endl;

  ExternalServiceHandle *esh = new ExternalServiceHandle(this);
  if (!esh->Init(name, path))
    wcout << L"Unable to register service " << esh->GetDlError() << endl;
  else {
    wcout << L"Service " << name << L" registered" << endl;
    AddExternalService(esh);
  }
}

void ServiceManager::AddExternalService(ExternalServiceHandle* parmEsh) {
    // check if the class already exists
  map<wstring, ExternalServiceHandle*>::iterator it;

  if ((it = esMap.find (parmEsh->GetName())) != esMap.end ())
    ; 
  else {
    esMap.insert(map<wstring,ExternalServiceHandle*>::value_type(parmEsh->GetName(), parmEsh));
  }
}

// Implementation of ExternalServiceHandle

ExternalServiceHandle::ExternalServiceHandle(const ExternalServiceManager* parmManager) 
{
  manager = parmManager;
}

void ExternalServiceHandle::run(void* /*arg*/)
{
  service->Run();
}

const wstring ExternalServiceHandle::GetName()
{
  return name;
}

ExternalService* ExternalServiceHandle::GetService()
{
  return service;
}

bool ExternalServiceHandle::CheckPointer(void *p, const wchar_t* fname)
{
  if (! p) 
    return false;
  else
    return true;
}

static PROC_PTR DLSymAddr(DLL_HANDLE dllib, const wchar_t * name)
{
  return DLSYM( dllib, name );
}

bool ExternalServiceHandle::Init(const wstring & parmName, const wstring & parmLocation)
{
  name = parmName;
  location = parmLocation;
  
  handle = DLOPEN( location );
  if (handle == NULL) {
    wcerr << L"Unable to open " << location << endl;
    wcerr << GetDlError() << endl;
    return false;
  }
  else {
    registerPtr = (fun_es_register) DLSymAddr(handle, L"Register");
    if (!CheckPointer((DLL_HANDLE)registerPtr, L"Register"))
      return false;
    deregisterPtr = (fun_es_deregister) DLSymAddr(handle, L"Deregister");
    if (!CheckPointer((DLL_HANDLE)deregisterPtr, L"Deregister"))
      return false;
    service = (*registerPtr)(manager);
    if (!service)
      return false;
    return true;
  }
}

wstring ExternalServiceHandle::GetDlError()
{
  //return DLERROR().c_str();
#ifdef _MSC_VER
  wstring errmsg (L"Dynamic Library Error");
#else
  wstring errmsg (DLERROR());
#endif // _MSC_VER
  return errmsg;
}

