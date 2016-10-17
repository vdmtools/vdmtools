#include "SqlConnection.h"
#include "SqlStatement.h"
#include "SqlException.h"
#include "j2vmain.h"
#include "tb_wstring.h"
#include "sqlucode.h"

#if defined(_MSC_VER) && defined(_DEBUG)
#include <crtdbg.h>
#endif

#ifdef Nil
#undef Nil
#endif

const wchar_t* vdm_SqlConnection::name = L"SqlConnection";

Generic vdm_SqlConnection::DlMethodCall (const wchar_t* name, const Sequence &p)
{
  Generic res;

  if (!wcscmp (name, L"createStatement'")) {
    res = vdm_createStatement();
  } else if (!wcscmp (name, L"close")) {
    vdm_close();
    res = mk_(Int(0), Sequence(), Sequence(), Nil());
  } else {
      // the method does not exist
    throw 1;
  }
  return res;
}

vdm_SqlConnection::vdm_SqlConnection ()
{ 
  this->m_hdbc = SQL_NULL_HDBC;
  this->m_henv = SQL_NULL_HENV;
}

vdm_SqlConnection::vdm_SqlConnection (SQLHANDLE henv, SQLHDBC hdbc)
{
  this->m_henv = henv;
  this->m_hdbc = hdbc;
}

vdm_SqlConnection::~vdm_SqlConnection ()
{
  SQLRETURN nRetCode;

  vdm_close ();

  if(this->m_hdbc != SQL_NULL_HDBC)
    nRetCode = ::SQLFreeHandle(SQL_HANDLE_DBC, this->m_hdbc);

  if (nRetCode != SQL_SUCCESS && nRetCode != SQL_SUCCESS_WITH_INFO && this->m_henv != SQL_NULL_HENV)
  {
    ::SQLFreeHandle(SQL_HANDLE_ENV, this->m_henv);
  }
}

Tuple vdm_SqlConnection::vdm_createStatement ()
{
  try {
    SQLRETURN nRetCode;
    SQLHANDLE m_hstmt;

    nRetCode = ::SQLAllocHandle(SQL_HANDLE_STMT, this->m_hdbc, &m_hstmt);
    if (nRetCode != SQL_SUCCESS && nRetCode != SQL_SUCCESS_WITH_INFO){
      throw new SqlException(SQL_HANDLE_DBC, this->m_hdbc);
    }

    Tuple res(4);
    res.SetField(1, Int(0));
    res.SetField(2, Sequence());
    res.SetField(3, Sequence());
    res.SetField(4, DLObject(L"SqlStatement", new vdm_SqlStatement(m_hstmt)));
    return res;
  }
  catch(SqlException* e) {
    Tuple res(CheckError(e->handleType, e->handle));
    delete e;
    return res;
  }
}

Tuple vdm_SqlConnection::CheckError(SQLSMALLINT handleType, SQLHANDLE handle)
{
#ifdef _MSC_VER
  SQLSMALLINT textLength;
  SQLWCHAR    sqlstate[6];
  SQLWCHAR    messageText[1024];
  SQLINTEGER  nativeError;

  ::SQLGetDiagRecW(handleType, handle, 1, sqlstate, &nativeError, messageText, 1024, &textLength);

  Tuple res(4);
  res.SetField(1, Int(nativeError));
  res.SetField(2, Sequence(wstring((wchar_t *)sqlstate,5)));
  res.SetField(3, Sequence(wstring((wchar_t *)messageText,textLength)));
  res.SetField(4, Nil());
  return res;
#else
  SQLSMALLINT textLength;
  SQLCHAR     sqlstate[6];
  SQLCHAR     messageText[1024];
  SQLINTEGER  nativeError;

  ::SQLGetDiagRec(handleType, handle, 1, sqlstate, &nativeError, messageText, 1024, &textLength);

  Tuple res(4);
  res.SetField(1, Int(nativeError));
  res.SetField(2, Sequence(TBWSTR::utf8str2wstring(string((char*)sqlstate,5))));
  res.SetField(3, Sequence(TBWSTR::utf8str2wstring(string((char*)messageText,textLength))));
  res.SetField(4, Nil());
  return res;
#endif // _MSC_VER
}

void vdm_SqlConnection::vdm_close ()
{
  if(this->m_hdbc != SQL_NULL_HDBC) {
    ::SQLDisconnect(m_hdbc);
     this->m_hdbc = SQL_NULL_HDBC;
  }
}

void vdm_SqlConnection::vdm_commit ()
{}

Bool vdm_SqlConnection::vdm_isClosed ()
{ return Bool(false); }

void vdm_SqlConnection::vdm_rollback ()
{}

DLObject vdm_SqlConnection::vdm_nativeSQL (const vdm_String &)
{
  return DLObject(vdm_String::name, new vdm_String());
}

Bool vdm_SqlConnection::vdm_isReadOnly ()
{ return Bool(false); }

void vdm_SqlConnection::vdm_setCatalog (const vdm_String &)
{}

void vdm_SqlConnection::vdm_setReadOnly (const Bool &)
{}

void vdm_SqlConnection::vdm_clearWarnings ()
{}

Bool vdm_SqlConnection::vdm_getAutoCommit ()
{ return Bool(false); }

void vdm_SqlConnection::vdm_setAutoCommit (const Bool &)
{}

Int vdm_SqlConnection::vdm_getTransactionIsolation ()
{
  return Int(0);
}

void vdm_SqlConnection::vdm_setTransactionIsolation (const Int &)
{}

vdm_SqlConnection *vdm_SqlConnection::GetDLPtr(const DLObject& obj)
{
  vdm_SqlConnection *objPtr = (vdm_SqlConnection*) obj.GetPtr(); 
  return objPtr;
}

