
// WHAT
//   dlfactory
// FILE
//    $Id: dlfactory.cc,v 1.51 2006/09/22 11:14:03 vdmtools Exp $
// COPYRIGHT
//    (C) Kyushu University
//

#include "metaiv.h"
#include "dlfactory.h"
#include "evaldl.h"
#include "RTERR.h"
#include "stackeval.h"
#include "tb_dlfcn.h"
#include "astaux.h"
#include "tbutils.h"
#include "rt_errmsg.h"
#include "tb_wstring.h"
#include "evalstate.h"
#include "settings.h"
#include <sstream>

class DlLibInfo
{
private:
  const wstring libName;
  const wstring libPath;
  wstring       path;
  DLL_HANDLE    dlLib;      // handle to the library
  DLFunctions   dlFunctions;
  DLRecInfoData dlrecinfodata;

private:
  void CheckPointer(void *, const wstring& opname);
  DlLibInfo(const DlLibInfo &); // not implemented
  void operator=(const DlLibInfo &); // not implemented
  PROC_PTR DLSymAddr(DLL_HANDLE dllib, const wstring & name);

public:
  DlLibInfo(const wstring & libname, const wstring & libpath);
  ~DlLibInfo();
  void OpenLibrary();
  void CloseLibrary();
  DLFunctions   & GetDLFunctions();
  DLRecInfoData & GetDLRecInfoData();
  wstring GetLibName() { return this->libName; };
  wstring FullLibName();
#ifdef VDMSL
  void AddOpFnVal(const wstring & name);
  PROC_PTR GetOpFnValHandle(const wstring & name);
  Generic CallOp(const wstring& module, const wstring& op, bool isvoid, const Generic & args);
private:
  typedef std::map<wstring, PROC_PTR> opfnvalmap_t;
  opfnvalmap_t opfnvalmap;
#endif // VDMSL
#ifdef _MSC_VER
  wstring GetLoadErrorStr();
#endif // _MSC_VER
};

#ifdef VDMPP
const wstring DLCLASS_NEW    =  L"DlClass_new";
const wstring DLCLASS_DELETE =  L"DlClass_delete";
const wstring DLCLASS_CALL   =  L"DlClass_call";
const wstring DLCLASS_INIT   =  L"DlClassInit";
#endif // VDMPP
#ifdef VDMSL
const wstring DLCLASS_INIT   =  L"InitDLModule";
#endif // VDMSL

#ifdef _MSC_VER
const wstring VDMDLGET =  L"dll_vdmdlgetfunctions";
#else
const wstring VDMDLGET =  L"vdmdlgetfunctions";
#endif // _MSC_VER

// DlMethodCall
// name :
// params :
Generic DlClassInstance::DlMethodCall (const wstring & name, Sequence & params)
{
  Generic result;
  int success;

  result = (*this->_dlclassinfo->GetDLLibInfo().GetDLFunctions().dlCall)(this->_dlClassRef, name.c_str (), params, success);

  if (!success) {
    throw DLException (DLException::DLCALL_ERR, wstring (L"Could not call method ") + name);
  }
  return result;
}

DLRecInfoData & DlClassInstance::Get_DLRecInfoData ()
{
  return this->_dlclassinfo->GetDLLibInfo().GetDLRecInfoData();
}

DlLibInfo::DlLibInfo(const wstring & libnm, const wstring & libpath) : libName(libnm), libPath(libpath), dlLib(NULL)
{
  OpenLibrary();
}

DlLibInfo::~DlLibInfo()
{
  CloseLibrary();
}

PROC_PTR DlLibInfo::DLSymAddr(DLL_HANDLE dllib, const wstring & name)
{
  return DLSYM( dllib, name );
}

#ifdef _MSC_VER
wstring DlLibInfo::GetLoadErrorStr()
{
  wchar_t * lnm = new wchar_t[libName.size() + 1];
  wcscpy(lnm, libName.c_str());
  LPWSTR args[1];
  args[0] = lnm;

  LPWSTR lpMsgBuf;
  FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                 FORMAT_MESSAGE_FROM_SYSTEM |
                 FORMAT_MESSAGE_ARGUMENT_ARRAY,
                 NULL,
                 GetLastError(),
                 MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
                 (LPWSTR) &lpMsgBuf,
                 0,
                 (va_list *)args);

  wstring res (lpMsgBuf);

  LocalFree(lpMsgBuf);
  delete[] lnm;
  return res;
}
#endif // _MSC_VER

void DlLibInfo::OpenLibrary()
{
  if (this->dlLib == NULL) {
    // We catch and rethrow any errors relating to opening the
    // dllibrary since we can then close it properly.
    try {
      this->path = this->FullLibName();
      this->dlLib = DLOPEN( path );
      if (this->dlLib == NULL) {
        wstring err (RTERR::GetWhatStr(RTERR_LIB_NOT_DEFINED).GetString() + L"'" + path + L"': " + DLERROR());
        throw DLException(DLException::DLL_LOAD_ERR, err);
      }
      else {
        vdm_log << L"\"" << this->path << L"\" loaded." << endl;
      }
#ifdef VDMPP
      this->dlFunctions.dlNew =    (fun_dlClass_new) DLSymAddr (this->dlLib, DLCLASS_NEW   );
      this->dlFunctions.dlDelete = (fun_dlClass_del) DLSymAddr (this->dlLib, DLCLASS_DELETE);
      this->dlFunctions.dlCall =   (fun_dlClass_call)DLSymAddr (this->dlLib, DLCLASS_CALL  );

      CheckPointer((DLL_HANDLE)(this->dlFunctions.dlNew),    DLCLASS_NEW   );
      CheckPointer((DLL_HANDLE)(this->dlFunctions.dlDelete), DLCLASS_DELETE);
      CheckPointer((DLL_HANDLE)(this->dlFunctions.dlCall),   DLCLASS_CALL  );
#endif // VDMPP

      {
        typedef void (*getfunctions_t)(vdmdlfunctions*);
        getfunctions_t getf = (getfunctions_t) DLSymAddr (this->dlLib, VDMDLGET);
        CheckPointer((DLL_HANDLE)getf, VDMDLGET);
        (*getf)(&(this->dlrecinfodata.functions));
        this->dlrecinfodata.namemap = this->dlrecinfodata.functions.vdmgetdefaultrecinfomapptr();

        this->dlFunctions.dlInit = (fun_dlClass_init)DLSymAddr (this->dlLib, DLCLASS_INIT);
      }
      {
        unsigned int version = (*(this->dlrecinfodata.functions.vdmgetvdmlibversion))();
        //extern unsigned int GetVDMLibVersion();
        if (GetVDMLibVersion() != version) {
          throw DLException(DLException::DLL_LOAD_ERR, RTERR::GetWhatStr(RTERR_LIB_WRONG_VERSION).GetString());
        }
      }

#ifdef VDMSL
      // reset the operation/function/value handles in opfnvalmap.
      for (opfnvalmap_t::iterator it = this->opfnvalmap.begin(); it != this->opfnvalmap.end(); ++it) {
        wstring name ((*it).first);
        PROC_PTR h = DLSymAddr(this->dlLib, name);
        CheckPointer(h, name);
        (*it).second = h;
      }
#endif //VDMSL

      if (this->dlFunctions.dlInit) {
        (*(this->dlFunctions.dlInit))(true);
      }
      else {
        // vdm_err << L"Warning: no InitDLModule function in " << s << endl << flush;
      }
    }
    catch (DLException & e) {
      if ( NULL != this->dlLib ) {
        CloseLibrary();
      }
      throw e;
    }
  }
}

void DlLibInfo::CheckPointer(void * p, const wstring& opname)
{
  if (p == NULL) {
    throw DLException(DLException::DLL_LOAD_ERR,
                      wstring(L"Name `") + opname + L"' not found in file `" + this->path + L"'");
  }
}

#ifdef VDMSL
void DlLibInfo::AddOpFnVal(const wstring & name)
{
  OpenLibrary();
  PROC_PTR h = DLSymAddr(this->dlLib, name);
  CheckPointer(h, name);
  this->opfnvalmap.insert(opfnvalmap_t::value_type(name, h));
  //wcerr << L"AddOpFnVal name=" << name << L" h=" << (void*)h << endl << flush;
}

PROC_PTR DlLibInfo::GetOpFnValHandle(const wstring & name)
{
  opfnvalmap_t::iterator it = this->opfnvalmap.find(name);
  if (it == this->opfnvalmap.end()) {
    throw DLException(DLException::DLL_LOAD_ERR,
                      wstring(L"Internal error: unknown name to GetOpFnValHandle\n: ") + name);
  }
  //wcerr << L"GetOpFnValHandle: name=" << name << L" =" << (void*) (*it).second << endl << flush;
  return (*it).second;
}

Generic DlLibInfo::CallOp(const wstring& module, const wstring& op, bool isvoid, const Generic & arg_l)
{
  //typedef Generic (* ValueFuncType) ();
  typedef void (* ValueFuncType) (Generic &);
  //typedef Generic (* FuncType) (Sequence);
  typedef void (* FuncType) (const Sequence &, Generic &);
  typedef void (* VoidFunc) (const Sequence &);

  DLRecInfoData & dldata = GetDLRecInfoData();

  if (arg_l.IsNil()) {
    // values
    ValueFuncType valfunc = (ValueFuncType) GetOpFnValHandle(op);
    //Generic res((*valfunc)());
    Generic res;
    (*valfunc)(res);
    return EvalState::M42Sem(res, &dldata);
  }
  else {
    // functions or operations
    Sequence m4arg_l(EvalState::Sem2M4(arg_l, &dldata));
    if (isvoid) {
      VoidFunc vfunc = (VoidFunc) GetOpFnValHandle(op);
      (*vfunc)(m4arg_l);
      return sem_cont;
    }
    else {
      FuncType func = (FuncType) GetOpFnValHandle(op);
      //Generic res((*func)(m4arg_l));
      Generic res;
      (*func)(m4arg_l, res);
      return EvalState::M42Sem(res, &dldata);
    }
  }
}

Generic DlClassFactory::CallOp(const TYPE_AS_Name& module, const TYPE_AS_Name& op, bool isvoid, const Generic & args)
{
  wstring modulestr (ASTAUX::ASName2String(module));
  wstring opstr (ASTAUX::ASName2String(op));

  map<wstring, DlClassInfo*>::iterator it = this->_classPath.find (modulestr);
  if (it == this->_classPath.end ()) {
    throw DLException(DLException::INTERNAL_ERR,
                      wstring(L"Internal error: unknown name to CallOp\n: ") + modulestr + wstring(L" ") + opstr);
  }
  DlLibInfo & libinfo = (*it).second->GetDLLibInfo();
  return libinfo.CallOp(modulestr, opstr, isvoid, args);
}

#endif // VDMSL

void DlLibInfo::CloseLibrary()
{
  if (this->dlLib != NULL) {
    vdm_log << L"\"" << this->path << L"\" unloaded." << endl;

    if (this->dlFunctions.dlInit) {
      (*(this->dlFunctions.dlInit))(false);
    }
    int success = DLCLOSE( this->dlLib );
#ifdef _MSC_VER
    if (success == 0)
#else
    if (success != 0)
#endif // _MSC_VER
    {
    }
    this->dlFunctions.dlNew    = NULL;
    this->dlFunctions.dlDelete = NULL;
    this->dlFunctions.dlCall   = NULL;
    this->dlLib = NULL;
  }
}

DlClassInstance::~DlClassInstance ()
{
  DestroyInstance ();
}

// InsertClassLib
// clnm : AS`Name
// userslib : AS`TextLit
// name : AS`Name
void DlClassFactory::InsertClassLib (const TYPE_AS_Name & clnm,
                                     const TYPE_AS_TextLit & useslib,
                                     const TYPE_AS_Name & name)
{
//wcout << L"InsertClassLib: " << clnm << L" " << name << endl;
  wstring className (ASTAUX::ASName2String(clnm));
  wstring libName (useslib.GetSequence(pos_AS_TextLit_val).GetString());

  if ( libName.empty() ) {
    vdm_iplog << L"warning: dlclass " << className << L" has no uselib" << endl << flush;
  }
  else {
    wstring opfnval (ASTAUX::ASName2String(name));

    // get file path
    Tuple gflcp (GetCI().GetFileLineColPos(useslib.GetInt(pos_AS_TextLit_cid)));
    wstring libPath (TBUTILS::tb_getbasedir(gflcp.GetSequence(1).GetString()));

    DlLibInfo * libinfo = OpenLibrary(libName, libPath);
    // now this->_libMap[libName] exists

    // check if the class already exists
    map<wstring, DlClassInfo*>::iterator it (this->_classPath.find (className));
    if (it == this->_classPath.end ()) {
      // remeber class information
      DlClassInfo* classInfo (new DlClassInfo (className, libinfo));
      this->_classPath.insert(map<wstring,DlClassInfo*>::value_type(className, classInfo));
    }
#ifdef VDMSL
    if (!opfnval.empty()) {
      libinfo->AddOpFnVal(opfnval);
    }
#endif //VDMSL
  }
}

void DlClassFactory::DLClose()
{
  for (map<wstring, DlLibInfo*>::iterator libIt = this->_libMap.begin ();
                                          libIt != this->_libMap.end (); ++libIt) {
    (*libIt).second->CloseLibrary();
  }
}

void DlClassFactory::clear()
{
  for (map<wstring, DlClassInfo*>::iterator pathIt = this->_classPath.begin ();
                                            pathIt != this->_classPath.end (); ++pathIt) {
    delete (*pathIt).second;
  }
  this->_classPath.erase(this->_classPath.begin(), this->_classPath.end());

  for (map<wstring, DlLibInfo*>::iterator libIt = this->_libMap.begin ();
                                          libIt != this->_libMap.end (); ++libIt) {
    delete (*libIt).second;
  }
  this->_libMap.erase(this->_libMap.begin(), this->_libMap.end());
}

DlClassFactory::~DlClassFactory ()
{
  clear();
}

DlLibInfo * DlClassFactory::OpenLibrary (const wstring & libName, const wstring & libPath)
{
  dllibinfomap_t::iterator libIt = this->_libMap.find(libName);

  if (libIt == this->_libMap.end()) { // not registerd
    pair<dllibinfomap_t::iterator, bool> res;
    res = this->_libMap.insert(dllibinfomap_t::value_type(libName, new DlLibInfo(libName, libPath)));
    libIt = res.first;
  }
  return (*libIt).second;
}

DLRecInfoData & DlLibInfo::GetDLRecInfoData()
{
  OpenLibrary();
  return this->dlrecinfodata;
}

DLFunctions & DlLibInfo::GetDLFunctions()
{
  OpenLibrary();
  return this->dlFunctions;
}

bool DlClassFactory::CloseLibrary (const wstring& libName)
{
  this->_libMap[libName]->CloseLibrary();
  return true;
}

// CreateDLObject
// clnm : AS`Name
// dlclassp : [DlClassInstance(VDMContainer<DlClassInstanceHolder>)]
// ==> DLObject
Generic DlClassFactory::CreateDLObject(const TYPE_AS_Name & clnm, const Generic & dlclassp)
{
  if (dlclassp.IsNil()) {
    return Nil();
  }
  else {
    return DLObject(ASTAUX::ASName2String(clnm),
                    ((const VDMContainer<DlClassInstanceHolder>&)dlclassp).get_const_ref().dcip->GetDlClassRef());
  }
}

// DLClose
// dlclassp : [DlClassInstance(VDMContainer<DlClassInstanceHolder>)]
void DlClassFactory::DLClose(const Generic & dlclassp)
{
  if (dlclassp.IsNil()) return;

  VDMContainer<DlClassInstanceHolder> dcih (((const VDMContainer<DlClassInstanceHolder>&)dlclassp));
  //DlClassInstance * dcip (dcih.get_const_ref().dcip);
  DlClassInstance * dcip (dcih.get_shared_ref().dcip);
  // prevent the DlClassInstanceHolder destructor from deleting p again
  dcih.get_shared_ref().dcip = NULL;
  delete dcip;
}

// DLClassEqual
// dlclassp : [DlClassInstance(VDMContainer<DlClassInstanceHolder>)]
// obj : DLObject
// ==> boot
bool DlClassFactory::DLClassEqual(const Generic & dlclassp, const DLObject & obj)
{
  if (dlclassp.IsNil()) return false;

  DlClassInstance * dcip (((const VDMContainer<DlClassInstanceHolder>&)dlclassp).get_const_ref().dcip);
  return ((dcip != NULL) && (dcip->GetDlClassRef() == obj.GetPtr()));
}

// DlMethodCall
// dlclassp : [DlClassInstance(VDMContainer<DlClassInstanceHolder>)]
// clname : AS`Name
// name : AS`Name
// arg_l : seq of SEM`VAL
// bool * [SEM`VAL]
Tuple DlClassFactory::DlMethodCall(const VDMContainer<DlClassInstanceHolder> & dcih,
                                   const TYPE_AS_Name & clname,
                                   const TYPE_AS_Name & name,
                                   const SEQ<TYPE_SEM_VAL> & arg_l)
{
  try {
    DlClassInstance * dcip (dcih.get_const_ref().dcip);
    Sequence m4arg_l (EvalState::Sem2M4(arg_l, &dcip->Get_DLRecInfoData()));
    // Log apply here to cname.opnm with args m4arg_l
    Generic res (dcip->DlMethodCall(ASTAUX::ASName2String(name), m4arg_l));
    if (res == Generic()) { // no return value
      return mk_(Bool(true), sem_cont);
    }
    else {
      return mk_(Bool(true), EvalState::M42Sem(res, &dcip->Get_DLRecInfoData()));
    }
  }
  catch (DLException e) {
    wstring cname (ASTAUX::ASName2String(clname));
    vdm_iplog << L"dlclass call to " << ASTAUX::ASName2String(clname) << L"`"
              << ASTAUX::ASName2String(name) << L"() failed" << endl;
    vdm_iplog << L"reason: " << e.GetErrorMessage() << endl << flush;
  }
  return mk_(Bool(false), Nil());
}

// GetInstance
// clnm : AS`Name
// dlobject : [DLObject]
// ==> [VDMContainer<DlClassInstanceHolder>]
Generic DlClassFactory::GetInstance (const TYPE_AS_Name & clnm, const Generic & dlobject)
{
  try {
    DlClass * dlclassp (dlobject.IsNil() ? (DlClass *)NULL : DLObject(dlobject).GetPtr());
    wstring className (ASTAUX::ASName2String(clnm));

    // check if the className exists in our L"repository"
    map<wstring, DlClassInfo*>::iterator it = this->_classPath.find (className);

    if (it == this->_classPath.end ()) {
      throw DLException(DLException::COULD_NOT_INSTANTIATE, wstring (L"Unknown class name: ") + className);
    }

    // p may be NUL in which case we must as the DLL to create a DlClass pointer.
    DlClassInfo* classInfo = (*it).second;
    if (dlclassp == NULL) {
      wstring libName (classInfo->GetDLLibInfo().GetLibName());
      DlLibInfo & libInfo = classInfo->GetDLLibInfo();
  
      // try to create the class using the DlClass_new function
      dlclassp = (*(libInfo.GetDLFunctions().dlNew))(className.c_str ());

      if (dlclassp == NULL) {
        wstring where (L"Instantiation 'DlClass_new (\""+ className + L"\")' in \"" +
                            libName + wstring (L"\" returned zero pointer"));
        throw DLException(DLException::COULD_NOT_INSTANTIATE, where);
      }
    }
    VDMContainer<DlClassInstanceHolder> dcih;
    dcih.get_shared_ref().dcip = new DlClassInstance (dlclassp, classInfo);
    return dcih;
  }
  catch (DLException e) {
    vdm_iplog << e.GetErrorMessage() << endl;
    RTERR::Error(L"CreateNEW", RTERR_DLCLASS_NEW_ERROR, Nil(), Nil(), Sequence());
  }
  return Nil();
}

void DlClassInstance::DestroyInstance ()
{
  // delete the instance. ignore the result.
  (void)(*this->_dlclassinfo->GetDLLibInfo().GetDLFunctions().dlDelete)(this->_dlClassRef);
}

// This function convert a name to a name with path.
wstring DlLibInfo::FullLibName()
{
  if (TBUTILS::HasPathSpec(this->libName)) {
    return this->libName;
  }

  wstring lib (TBUTILS::GetEnv("VDM_DYNLIB"));
  wstring library (lib.empty() ? wstring(L".") : lib);

  if (library != this->libPath) {
#ifdef _MSC_VER
    library += L";" + this->libPath;
#else
    library += L":" + this->libPath;
#endif // _MSC_VAR 
  }
  // library : aaa:bbb:ccc for unix
  // library : aaa;bbb;ccc for windows
  wstring res (TBUTILS::tb_Search(library, this->libName));
  if (res.empty()) {
    wstring err (RTERR::GetWhatStr(RTERR_FILE_DOES_NOT_EXISTS).GetString() + L" (" + this->libName + L")");
    throw DLException(DLException::DLL_LOAD_ERR, err);
  }
  return res;
}

int vdm_Compare (const DlClassInstanceHolder &, const DlClassInstanceHolder &)
{
  return 0;
}

DlClassInstanceHolder::DlClassInstanceHolder(const DlClassInstanceHolder & d) : dcip(NULL)
{
  if (d.dcip == NULL) {
    vdm_iplog << L"Internal error: someone is using DlClassInstanceHolder copycons" << endl << flush;
  }
}

