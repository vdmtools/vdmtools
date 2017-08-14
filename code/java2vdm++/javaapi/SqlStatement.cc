#include "SqlStatement.h"
#include "SqlResultSet.h"
#include "SqlException.h"
#include "j2vmain.h"
#include "tb_wstring.h"

#include "sqlext.h"

#if defined( __linux__ ) || defined( __CYGWIN__ )
#include <stdlib.h>
#include <string.h>
#endif // __linux__

#ifdef Nil
#undef Nil
#endif

const wchar_t* vdm_SqlStatement::name = L"SqlStatement";

Generic vdm_SqlStatement::DlMethodCall (const wchar_t* name, const Sequence &p)
{
  Generic res;

  if (!wcscmp (name, L"executeQuery'")) {
    vdm_String query = *vdm_String::GetDLPtr(p[1]);
    res = this->vdm_executeQuery(query);
  } else if (!wcscmp(name, L"executeUpdate'")) {
    vdm_String query = *vdm_String::GetDLPtr(p[1]);
    res = this->vdm_executeUpdate(query);
  } else if (!wcscmp(name, L"close")) {
    this->vdm_close();
    res = mk_(Int(SQL_SUCCESS), Sequence(), Sequence(), Nil());
  } else {
      // the method does not exist
    throw 1;
  }
  return res;
}

vdm_SqlStatement::vdm_SqlStatement ()
{
  this->m_hstmt = SQL_NULL_HSTMT; 
}

vdm_SqlStatement::vdm_SqlStatement (SQLHANDLE hstmt)
{
  this->m_hstmt = hstmt;
}

vdm_SqlStatement::~vdm_SqlStatement ()
{
  vdm_close ();
}

void vdm_SqlStatement::vdm_close ()
{
  if(this->m_hstmt != SQL_NULL_HSTMT) {
    ::SQLCloseCursor(this->m_hstmt);
    ::SQLFreeHandle(SQL_HANDLE_STMT, this->m_hstmt);
     this->m_hstmt = SQL_NULL_HSTMT;
  }
}

Tuple vdm_SqlStatement::vdm_executeQuery (const vdm_String &vdm_sql)
{
  try {
    SQLRETURN nRetCode;

#ifdef _MSC_VER
    wstring sql = vdm_sql.getValue();
    nRetCode = ::SQLPrepareW(this->m_hstmt, (SQLWCHAR*)sql.c_str(), SQL_NTS);
#else
    string sql = TBWSTR::wstring2utf8str(vdm_sql.getValue());
    nRetCode = ::SQLPrepare(this->m_hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS);
#endif // _MSC_VER
    if (nRetCode != SQL_SUCCESS && nRetCode != SQL_SUCCESS_WITH_INFO)
      throw new SqlException(SQL_HANDLE_STMT,this->m_hstmt);

    nRetCode = ::SQLExecute(this->m_hstmt);
    if (nRetCode != SQL_SUCCESS && nRetCode != SQL_SUCCESS_WITH_INFO)
      throw new SqlException(SQL_HANDLE_STMT, this->m_hstmt);

    vdm_SqlResultSet* pRs = new vdm_SqlResultSet(this);
    //pRs->create();

    Tuple res(4);
    res.SetField(1, Int(SQL_SUCCESS));
    res.SetField(2, Sequence());
    res.SetField(3, Sequence());
    //res.SetField(4, DLObject(L"SqlResultSet", pRs));
    res.SetField(4, DLObject(pRs->GetTypeName(), pRs)); // SqlResultSet
    return res;
  }
  catch(SqlException* e) {
    Tuple res (CheckError(e->handleType, e->handle));
    delete e;
    return res;
  }
}

char *uppercase(char* pstr)
{
  for(char* p = pstr; *p!='\0'; p++) *p = toupper(*p);
  return pstr;
}

bool checkoperation(const char * qry, const char * op) {
  int i = 0;
  while (i < strlen(qry) && isspace(qry[i])) {
    i++;
  }
// aux = (char *) malloc(sizeof(char) * (strlen(qry)-i+1));
#ifdef _MSC_VER
  char * aux = (char *)&qry[i];
  if (strncmp(strupr(aux), uppercase((char *)op), strlen(op)) == 0) {
#else
  const char * aux = &qry[i];
  if (strncasecmp(aux, op, strlen(op)) == 0) {
#endif // _MSC_VER
    return true;
  } else {
    return false;
  }
}

Tuple vdm_SqlStatement::CheckError(SQLSMALLINT handleType, SQLHANDLE handle)
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

Tuple vdm_SqlStatement::vdm_executeUpdate(const vdm_String &vdm_sql)
{
  string qry = TBWSTR::wstring2utf8str(vdm_sql.getValue());

  if (checkoperation(qry.c_str(), "select")) {
    Tuple res(4);
    res.SetField(1, Int(9999));
    res.SetField(2, Sequence(wstring(L"XXXXX")));
    res.SetField(3, Sequence(wstring(L"To execute SELECT, use executeQuery!")));
    res.SetField(4, Nil());
    return res;
  } else {
    try {
      SQLRETURN nRetCode;
#ifdef _MSC_VER
      wstring query = vdm_sql.getValue();
      nRetCode = ::SQLExecDirectW(this->m_hstmt, (SQLWCHAR*)query.c_str(), SQL_NTS);
#else
      string query = TBWSTR::wstring2utf8str(vdm_sql.getValue());
      nRetCode = ::SQLExecDirect(this->m_hstmt, (SQLCHAR*)query.c_str(), SQL_NTS);
#endif // _MSC_VER
      if (nRetCode != SQL_SUCCESS && nRetCode != SQL_SUCCESS_WITH_INFO)
        throw new SqlException(SQL_HANDLE_STMT, this->m_hstmt);

      SQLLEN numrows;
      nRetCode = ::SQLRowCount(this->m_hstmt, &numrows);
      if (nRetCode != SQL_SUCCESS && nRetCode != SQL_SUCCESS_WITH_INFO)
        throw new SqlException(SQL_HANDLE_STMT, this->m_hstmt);

      Tuple res(4);
      res.SetField(1, Int(SQL_SUCCESS));
      res.SetField(2, Sequence());
      res.SetField(3, Sequence());
      res.SetField(4, (numrows != -1) ? Int(numrows) : Int(0));
      return res;
    }
    catch(SqlException* e) {
      Tuple res (CheckError(e->handleType, e->handle));
      delete e;
      return res;
    }
  }
}

vdm_SqlStatement *vdm_SqlStatement::GetDLPtr(const DLObject& obj)
{
  vdm_SqlStatement *objPtr = (vdm_SqlStatement*) obj.GetPtr();
  return objPtr;
}
