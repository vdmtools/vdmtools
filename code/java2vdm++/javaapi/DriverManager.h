#ifndef __DriverManager_h__
#define __DriverManager_h__

#include "Object.h"
#include "JavaString.h"

#ifdef _MSC_VER
#include <windows.h>
#endif // _MSC_VER

#include "sql.h"

class vdm_DriverManager : public vdm_Object
{
  SQLHENV m_henv;
  SQLHDBC m_hdbc;

public:
  static const wchar_t * name;

public:
  Generic DlMethodCall (const wchar_t* name, const Sequence &p);

  virtual const wchar_t * GetTypeName () const { return vdm_DriverManager::name; }

  vdm_DriverManager ();

  virtual ~vdm_DriverManager () {}

  void vdm_initialize ();

private:

  virtual Tuple vdm_getConnection_q (const vdm_String &);
  virtual Tuple vdm_getConnection_q_q_q (const vdm_String &, const vdm_String &, const vdm_String &);
  Tuple CheckError(SQLSMALLINT handleType, SQLHANDLE handle);

  static vdm_DriverManager *GetDLPtr(const DLObject& obj);
};

#endif // __DriverManager_h__

