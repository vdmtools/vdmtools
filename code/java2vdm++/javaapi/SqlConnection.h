#ifndef __SqlConnection_h__
#define __SqlConnection_h__

#include "Object.h"
#include "JavaString.h"

#ifdef _MSC_VER
#include <windows.h>
#endif // _MAS_VER

#include "sql.h"

class vdm_SqlConnection : public vdm_Object
{
  SQLHENV m_henv;
  SQLHDBC m_hdbc;

public:
  static const wchar_t * name;

public:

  vdm_SqlConnection ();
  vdm_SqlConnection (SQLHENV henv, SQLHDBC m_hdbc);

  virtual ~vdm_SqlConnection ();

private:

  Generic DlMethodCall (const wchar_t* name, const Sequence &p);

  Tuple CheckError(SQLSMALLINT handleType, SQLHANDLE handle);

  virtual void vdm_close ();
  virtual void vdm_commit ();
  virtual Bool vdm_isClosed ();
  virtual void vdm_rollback ();
  virtual DLObject vdm_nativeSQL (const vdm_String &);
//  virtual DLObject vdm_getCatalog ();
//  virtual vdm_Map vdm_getTypeMap ();
  virtual Bool vdm_isReadOnly ();
  virtual void vdm_setCatalog (const vdm_String &);
  virtual void vdm_setReadOnly (const Bool &);
  virtual void vdm_clearWarnings ();
  virtual Bool vdm_getAutoCommit ();
  virtual void vdm_setAutoCommit (const Bool &);
  virtual Tuple vdm_createStatement ();
  virtual Int vdm_getTransactionIsolation ();
  virtual void vdm_setTransactionIsolation (const Int &);

private:
  static vdm_SqlConnection *GetDLPtr(const DLObject& obj);
}
;

#endif // __SqlConnection_h__

