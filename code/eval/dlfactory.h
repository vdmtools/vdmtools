//
// WHAT
//   dlfactory
// FILE
//    $Id: dlfactory.h,v 1.13 2006/04/04 07:45:58 vdmtools Exp $
// COPYRIGHT
//    (C) Kyushu University
//

#ifndef __dlfactory_h__
#define __dlfactory_h__

#include "metaiv.h"
#include "AS.h"
#include "evaldl.h"
#include <map>

class DlClass; // so we don't need to include dlclass.h 
class DlClassInstance;

typedef DlClass* (*fun_dlClass_new)(const wchar_t*);
typedef int (*fun_dlClass_del)(void*);
typedef Generic (*fun_dlClass_call)(DlClass*, const wchar_t*, Sequence&, int&);
typedef void (*fun_dlClass_init)(bool);

class DLException
{
public:
  enum ErrCode 
  {
    CLASS_NOT_FOUND, 
    DLL_LOAD_ERR,
    COULD_NOT_INSTANTIATE,
    DLCALL_ERR,
    DLDELETE_ERR,
    INTERNAL_ERR
  };
private:
  ErrCode _errCode;
  wstring _errMsg;

public:
  DLException (ErrCode code, const wstring& msg) : _errCode (code), _errMsg (msg)  { }
  ErrCode GetErrorCode () { return this->_errCode; }
  wstring GetErrorMessage () { return this->_errMsg; }
};

/**
 * DlLibInfo is used to store the information about the dynamic link
 * library that is used by the classes
 */
class DLFunctions
{
public:
  DLFunctions() : dlNew(NULL), dlDelete(NULL), dlCall(NULL), dlInit(NULL) {};
  fun_dlClass_new  dlNew;
  fun_dlClass_del  dlDelete;
  fun_dlClass_call dlCall;
  fun_dlClass_init dlInit; // is not required to exist in .dll/.so
};
  
class DlLibInfo;

/**
 * DlClassInfo is used to keep track
 * of the classes that use a specific
 * library
 */
class DlClassInfo
{
private:
  wstring dlclassname;
  DlLibInfo * dlLibInfo;
public:
  DlClassInfo(const wstring & classname, DlLibInfo * libInfo) : dlclassname(classname), dlLibInfo(libInfo) {};
  DlLibInfo & GetDLLibInfo() { return *this->dlLibInfo; };
};

class DlClassInstanceHolder;
/**
* This factory class is used to create and
* destroy DLClass-classes. It loads and unloads
* dll-resources
* Use one for every namespace
*/
class DlClassFactory
{
  friend class DlClassInstance;
private:
  typedef std::map<wstring, DlClassInfo*> classinfomap_t;
  typedef std::map<wstring, DlLibInfo*>   dllibinfomap_t;
  classinfomap_t _classPath;
  dllibinfomap_t _libMap;

#ifdef VDMSL
public: 
  void AddOpFnVal(const wstring & name);
  Generic CallOp(const TYPE_AS_Name & module, const TYPE_AS_Name & op, bool isvoid, const Generic & args);
private:
#endif // VDMSL

  /**
   * This method is used internally to load libraries
   */
  DlLibInfo * OpenLibrary (const wstring & libName, const wstring & libPath);	

  /**
   * This method is used internally to unload libraries
   */
  bool CloseLibrary (const wstring & libName);	

public:
  /**
   * Constructor
   */
  DlClassFactory () {};

  /**
   * Destructor
   */
  ~DlClassFactory ();
  
  /**
   * clear: closes all libraries and empties datastructures.
   */
  void clear();

  /**
   * Used to insert a L"class library" into a factory
   * @param libName name of the corresponding library
   * @param className name of the class
   * @exception ClassAlreadyExistsException if the class already exists
   */
  void InsertClassLib (const TYPE_AS_Name & clnm, const TYPE_AS_TextLit & useslib, const TYPE_AS_Name & name);
		
  /**
   * Returns an instance of the specified DlClass
   * @returns pointer to class
   * @exception InstantiationException
   */
public:
  Generic GetInstance (const TYPE_AS_Name & clnm, const Generic & dlobject);

  static Generic CreateDLObject(const TYPE_AS_Name & clnm, const Generic & dlclassp);
  static void DLClose(const Generic & dlclassp);
  static bool DLClassEqual(const Generic & dlclassp, const DLObject & obj);
  static Tuple DlMethodCall(const VDMContainer<DlClassInstanceHolder> & dcih,
                            const TYPE_AS_Name & clname,
                            const TYPE_AS_Name & name,
                            const SEQ<TYPE_SEM_VAL> & arg_l);

  /**
   * Closes the opened libraries but 
   * any access through DlLibInfo::GetDLLibInfo,GetDLFunctions will reopen  the library.
   */
  void DLClose();
};

class DlClassInstance
{
  friend class DlClassFactory;
protected:
  DlClass * _dlClassRef;
  DlClassInfo * _dlclassinfo;

public:
  /**
   * Constructor
   * @param dlClass Instance of the L"server class"
   * @param className the name of the class
   * @param factory reference to the factory
   * @param callFunction pointer to the call function for this class
   */
  DlClassInstance (DlClass * dlClass, DlClassInfo * dlclassinfo) : _dlClassRef(dlClass), _dlclassinfo(dlclassinfo) {};

  /**
   * Used to call a method of the L"server class"
   * @exception MethodCallException, if the call fails
   */
  Generic DlMethodCall (const wstring & name, Sequence & p);
  void DestroyInstance ();

  DLRecInfoData & Get_DLRecInfoData();
  DlClass * GetDlClassRef () { return this->_dlClassRef; }

  ~DlClassInstance ();
};

// The DlClassInstanceHolder is intended to delete its DlClassInstance
// pointer when its deleted itself.
class DlClassInstanceHolder
{
public:
  DlClassInstance * dcip;

  DlClassInstanceHolder() : dcip(NULL) {};
  ~DlClassInstanceHolder() {
    if (this->dcip != NULL) delete this->dcip;
  }
  DlClassInstanceHolder(const DlClassInstanceHolder & d);
};
#endif // __dlfactory_h__
