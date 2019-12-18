#include "DriverManager.h"
#include "SqlConnection.h"
#include "SqlException.h"
#include "JavaString.h"
#include "j2vmain.h"
#include "tb_wstring.h"

#if defined(_MSC_VER) && defined(_DEBUG)
#include <crtdbg.h>
#endif

#include "sqlext.h"

#ifdef Nil
#undef Nil
#endif

const wchar_t* vdm_DriverManager::name = L"JavaSqlDriverManager";

Generic vdm_DriverManager::DlMethodCall (const wchar_t* name, const Sequence &p)
{
  Generic res;

  if (!wcscmp (name, L"getConnection'''")) {
    vdm_String url = *vdm_String::GetDLPtr(p[1]);
    vdm_String username = *vdm_String::GetDLPtr(p[2]);
    vdm_String password = *vdm_String::GetDLPtr(p[3]);
    res = this->vdm_getConnection_q_q_q(url, username, password); 
  } else if (!wcscmp(name, L"getConnection'")) {
    vdm_String url = *vdm_String::GetDLPtr(p[1]);
    res = this->vdm_getConnection_q(url);
  } else {
      // the method does not exist
    throw 1;
  }
  return res;
}

vdm_DriverManager::vdm_DriverManager ()
{
  this->m_hdbc = SQL_NULL_HDBC;
  this->m_henv = SQL_NULL_HENV;
}

#ifndef DEF_DriverManager_initialize

void vdm_DriverManager::vdm_initialize ()
{
}

#endif

#ifndef DEF_DriverManager_getConnection_q

Tuple vdm_DriverManager::vdm_getConnection_q (const vdm_String &vdm_url)
{
  return vdm_DriverManager::vdm_getConnection_q_q_q(vdm_url, vdm_String(), vdm_String());
}

#endif

#ifndef DEF_DriverManager_getConnection_q_q_q

Tuple vdm_DriverManager::vdm_getConnection_q_q_q
  (const vdm_String &vdm_url, const vdm_String &vdm_user, const vdm_String &vdm_password)
{
  SQLRETURN nRetCode;

  try {
    if (this->m_henv == SQL_NULL_HENV) {
      nRetCode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &this->m_henv);
      if (nRetCode != SQL_SUCCESS && nRetCode != SQL_SUCCESS_WITH_INFO)
        throw new SqlException(SQL_HANDLE_ENV, this->m_henv);

      nRetCode = SQLSetEnvAttr(m_henv, SQL_ATTR_ODBC_VERSION, (void *)SQL_OV_ODBC3, 0);
      if (nRetCode != SQL_SUCCESS && nRetCode != SQL_SUCCESS_WITH_INFO)
        throw new SqlException(SQL_HANDLE_ENV, this->m_henv);
    }
    nRetCode = SQLAllocHandle(SQL_HANDLE_DBC, this->m_henv, &this->m_hdbc);
    if (nRetCode != SQL_SUCCESS && nRetCode != SQL_SUCCESS_WITH_INFO)
      throw new SqlException(SQL_HANDLE_ENV, this->m_henv);

#ifdef _MSC_VER
    wstring cstr;
    wstring url = vdm_url.getValue();
    if (!url.empty()) {
      cstr += L"DSN=" + url + L";"; 
    }
    wstring user = vdm_user.getValue();
    if (!user.empty()) {
      cstr += L"UID=" + user + L";"; 
    }
    wstring password = vdm_password.getValue();
    if (!password.empty()) {
      cstr += L"PWD=" + password + L";"; 
    }
    SQLWCHAR *connectionString = (SQLWCHAR*)cstr.c_str();

    nRetCode = ::SQLDriverConnectW(this->m_hdbc, NULL, connectionString, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);
#else
    string cstr;
    string url = TBWSTR::wstring2string(vdm_url.getValue());
    if (!url.empty()) {
      cstr += "DSN=" + url + ";"; 
    }
    string user = TBWSTR::wstring2string(vdm_user.getValue());
    if (!user.empty()) {
      cstr += "UID=" + user + ";"; 
    }
    string password = TBWSTR::wstring2string(vdm_password.getValue());
    if (!password.empty()) {
      cstr += "PWD=" + password + ";"; 
    }
    SQLCHAR *connectionString = (SQLCHAR*)cstr.c_str();

    nRetCode = ::SQLDriverConnect(this->m_hdbc, NULL, connectionString, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);
#endif // _MSC_VER

    if (nRetCode != SQL_SUCCESS && nRetCode != SQL_SUCCESS_WITH_INFO) {
      throw new SqlException(SQL_HANDLE_DBC, this->m_hdbc);
    }
    Tuple res(4);
    res.SetField(1, Int(0));
    res.SetField(2, Sequence());
    res.SetField(3, Sequence());
    res.SetField(4, DLObject(L"SqlConnection", new vdm_SqlConnection(this->m_henv, this->m_hdbc)));

    this->m_hdbc = SQL_NULL_HDBC;
    this->m_henv = SQL_NULL_HENV;

    return res;
  }
  catch(SqlException* e) {
    Tuple res (CheckError(e->handleType, e->handle));
    delete e;
    return res;
  }
}
#endif

Tuple vdm_DriverManager::CheckError(SQLSMALLINT handleType, SQLHANDLE handle)
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

vdm_DriverManager *vdm_DriverManager::GetDLPtr(const DLObject& obj)
{
  vdm_DriverManager *objPtr = (vdm_DriverManager*) obj.GetPtr(); 
  return objPtr;
}
