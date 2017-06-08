#ifndef __SqlStatement_h__
#define __SqlStatement_h__

#include "Object.h"
#include "JavaString.h"

#ifdef _MSC_VER
#include <windows.h>
#endif // _MSC_VER

#include "sql.h"
#include "sqltypes.h"

class vdm_SqlStatement : public vdm_Object
{
  SQLHSTMT m_hstmt;

public:
  static const wchar_t * name;

public:

  vdm_SqlStatement ();
  vdm_SqlStatement (SQLHANDLE hstmt);

  virtual ~vdm_SqlStatement ();

public:

  Generic DlMethodCall (const wchar_t* name, const Sequence &p);

  virtual const wchar_t * GetTypeName () const { return L"SqlStatement"; }

  Tuple CheckError(SQLSMALLINT handleType, SQLHANDLE handle);

  virtual void vdm_close ();
  virtual Tuple vdm_executeQuery (const vdm_String &);
  virtual Tuple vdm_executeUpdate (const vdm_String &);

  static vdm_SqlStatement *GetDLPtr(const DLObject& obj);

  friend class vdm_SqlResultSet;
};

#endif // __SqlStatement_h__

