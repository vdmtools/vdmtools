#include "dbi.h"
#include <sqlext.h>

extern string wstring2utf8str(const wstring & ws);
extern wstring utf8str2wstring(const string & s);

DlClass* DlClass_new (const wchar_t* name)
{
  if (!wcscmp (name, L"DBI")) {
    return new DBI ();
  }
  else {
    return NULL; // nothing found
  }
}

int DlClass_delete (DlClass* c)
{
  try {
    delete c;
  }
  catch (...) {
    return 0;
  }
  return 1;
}

Generic DlClass_call (DlClass* c, const wchar_t* name, const Sequence & params, int & success)
{
  Generic result;
  try {
    result = c->DlMethodCall (name, params);
    success = 1;
    return result;
  }
  catch (...) {
    success = 0;
    return result;
  }
}

Generic DBI::DlMethodCall (const wchar_t * name, const Sequence & p)
{
  // Temporary Hack for segmentation violation
  wostringstream wos;
  wos << p << endl;
  // <-- hack end

  Generic res;

  if (wcscmp(name, L"dl_connect")==0) {
    res = this->Connect(p);
  }
//  else if (wcscmp(name, L"disconnect")==0) {
//    res = this->Disconnect();
//  }
  else if (wcscmp(name, L"dl_execute")==0) {
    res = this->Execute(SEQ<Char>(p[1]));
  }
  else if (wcscmp(name, L"dl_executeQuery")==0) {
    res = this->ExecuteQuery(SEQ<Char>(p[1]));
  }
  else if (wcscmp(name, L"dl_fetch")==0) {
    res = this->Fetch();
  }
  else if (wcscmp(name, L"dl_getInt")==0) {
    res = this->GetInt(Int(p[1]));
  }
  else if (wcscmp(name, L"dl_getString")==0) {
    res = this->GetString(Int(p[1]));
  }
  else if (wcscmp(name, L"dl_getChar")==0) {
    res = this->GetChar(Int(p[1]));
  }
  else if (wcscmp(name, L"dl_getNumeric")==0) {
    res = this->GetNumeric(Int(p[1]));
  }
  else if (wcscmp(name, L"dl_getQuote")==0) {
    res = this->GetQuote(Int(p[1]));
  }
  else if (wcscmp(name, L"dl_getDate")==0) {
    res = this->GetDate(Int(p[1]));
  }
  else if (wcscmp(name, L"dl_getTime")==0) {
    res = this->GetTime(Int(p[1]));
  }
  else if (wcscmp(name, L"dl_getTimestamp")==0) {
    res = this->GetTimestamp(Int(p[1]));
  }
  else if (wcscmp(name, L"dl_getRecordMap")==0) {
    res = this->GetRecordMap();
  }
  else if (wcscmp(name, L"dl_getRecordSeq")==0) {
    res = this->GetRecordSeq();
  }
  else if (wcscmp(name, L"dl_startTransaction")==0) {
    res = this->StartTransaction();
  }
  else if (wcscmp(name, L"dl_commitTransaction")==0) {
    res = this->EndTransaction(true);
  }
  else if (wcscmp(name, L"dl_rollbackTransaction")==0) {
    res = this->EndTransaction(false);
  }
  else {
    // the method does not exist - throw exception
  }
  return res;
}

DBI::DBI ()
{
  this->m_hdbc = SQL_NULL_HDBC;
  this->m_henv = SQL_NULL_HENV;
  this->m_hstmt = SQL_NULL_HSTMT;
  this->m_inTransaction = false;
}

DBI::~DBI ()
{
  Disconnect();
}

Tuple DBI::Connect(const Sequence & arg)
{
  SQLRETURN nResult;

  if (this->m_henv == SQL_NULL_HENV) {
    nResult = ::SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &this->m_henv);
    if (nResult != SQL_SUCCESS && nResult != SQL_SUCCESS_WITH_INFO) {
      return CheckError(SQL_HANDLE_ENV, this->m_henv);
    }
    nResult = ::SQLSetEnvAttr(this->m_henv, SQL_ATTR_ODBC_VERSION, (void *)SQL_OV_ODBC3, 0);
    if (nResult != SQL_SUCCESS && nResult != SQL_SUCCESS_WITH_INFO) {
      return CheckError(SQL_HANDLE_ENV, this->m_henv);
    }
  }

  nResult = ::SQLAllocHandle(SQL_HANDLE_DBC, this->m_henv, &this->m_hdbc);
  if (nResult != SQL_SUCCESS && nResult != SQL_SUCCESS_WITH_INFO) {
    return CheckError(SQL_HANDLE_DBC, this->m_hdbc);
  }
#ifdef _MSC_VER
  wstring mds = Sequence(arg[1]).GetString();
//  nResult = ::SQLConnectW(this->m_hdbc, (SQLWCHAR *)mds.c_str(), SQL_NTS, (SQLWCHAR *)wstring(L"").c_str(), SQL_NTS,
//                                      (SQLWCHAR *)wstring(L"").c_str(), SQL_NTS);
  wstring connectionString;
  if (!mds.empty()) {
    connectionString = wstring(L"DSN=") + mds;
  }
  nResult = ::SQLDriverConnectW(this->m_hdbc, NULL, (SQLWCHAR *)connectionString.c_str(), SQL_NTS, NULL, 0, NULL,
                              SQL_DRIVER_COMPLETE);
#else
  string mds = wstring2string(Sequence(arg[1]).GetString());
//  nResult = ::SQLConnect(this->m_hdbc, (SQLCHAR *)mds.c_str(), SQL_NTS, (SQLCHAR *)string("").c_str(), SQL_NTS,
//                                     (SQLCHAR *)string("").c_str(), SQL_NTS);
  string connectionString;
  if (!mds.empty()) {
    connectionString = string("DSN=") + mds;
  }
  nResult = ::SQLDriverConnect(this->m_hdbc, NULL, (SQLCHAR *)connectionString.c_str(), SQL_NTS, NULL, 0, NULL,
                               SQL_DRIVER_COMPLETE);
#endif // _MSC_VER

  if (nResult != SQL_SUCCESS && nResult != SQL_SUCCESS_WITH_INFO) {
    return CheckError(SQL_HANDLE_DBC, this->m_hdbc);
  }
  return mk_(Int(SQL_SUCCESS), Sequence(), Sequence(), Nil());
}

Tuple DBI::Disconnect()
{
  if (this->m_hstmt != SQL_NULL_HSTMT) {
    ::SQLCloseCursor(this->m_hstmt);
    ::SQLFreeHandle(SQL_HANDLE_STMT, this->m_hstmt);
    this->m_hstmt = SQL_NULL_HSTMT;
  }
  if (this->m_hdbc != SQL_NULL_HDBC) {
    ::SQLDisconnect(this->m_hdbc);
    ::SQLFreeHandle(SQL_HANDLE_DBC, this->m_hdbc);
    this->m_hdbc = SQL_NULL_HDBC;
  }
  if (this->m_henv != SQL_NULL_HENV) {
    ::SQLFreeHandle(SQL_HANDLE_ENV, this->m_henv);
    this->m_henv = SQL_NULL_HENV;
  }
  return mk_(Int(SQL_SUCCESS), Sequence(), Sequence(), Nil());
}

Tuple DBI::CheckError(SQLSMALLINT handleType, SQLHANDLE handle)
{
#ifdef _MSC_VER
  SQLSMALLINT nSize;
  SQLWCHAR    szState[6];
  SQLWCHAR    szErrorMsg[1024];
  SQLINTEGER  nErrorCode;

  ::SQLGetDiagRecW(handleType, handle, 1, szState, &nErrorCode, szErrorMsg, 1024, &nSize);

  if (nErrorCode == SQL_SUCCESS) {
    nErrorCode = -1;
  }
  szState[5] = '\0';
  Tuple res(4);
  res.SetField(1, Int(nErrorCode));
  res.SetField(2, Sequence(wstring((wchar_t *)szState,5)));
  res.SetField(3, Sequence(wstring((wchar_t *)szErrorMsg,nSize)));
  res.SetField(4, Nil());
#else
  SQLSMALLINT nSize;
  SQLCHAR     szState[6];
  SQLCHAR     szErrorMsg[1024];
  SQLINTEGER  nErrorCode;

  ::SQLGetDiagRec(handleType, handle, 1, szState, &nErrorCode, szErrorMsg, 1024, &nSize);

  if (nErrorCode == SQL_SUCCESS) {
    nErrorCode = -1;
  }
  szState[5] = '\0';
  Tuple res(4);
  res.SetField(1, Int(nErrorCode));
  res.SetField(2, Sequence(utf8str2wstring(string((char *)szState,5))));
  res.SetField(3, Sequence(utf8str2wstring(string((char *)szErrorMsg,nSize))));
  res.SetField(4, Nil());
#endif // _MSC_VER

  switch (handleType) {
    case SQL_HANDLE_ENV: {
      if (this->m_henv != SQL_NULL_HENV) {
        SQLFreeHandle(SQL_HANDLE_ENV, this->m_henv);
        this->m_henv = SQL_NULL_HENV;
      }
      break;
    }
    case SQL_HANDLE_DBC: {
      if (this->m_hdbc != SQL_NULL_HDBC) {
        SQLFreeHandle(SQL_HANDLE_DBC, this->m_hdbc);
        this->m_hdbc = SQL_NULL_HDBC;
      }
      break;
    } 
    case SQL_HANDLE_STMT: {
      if (handle != SQL_NULL_HSTMT) {
        SQLCloseCursor(handle);
        if (handle == this->m_hstmt) {
          this->m_hstmt = SQL_NULL_HSTMT;
        }
        SQLFreeHandle(SQL_HANDLE_STMT, handle);
      }
    }
  }
  return res;
}

Tuple DBI::StartTransaction()
{
  if (!this->m_inTransaction) {
    SQLRETURN nResult;
    // do commit
    nResult = ::SQLEndTran(SQL_HANDLE_DBC, this->m_hdbc, SQL_COMMIT );
    if (nResult != SQL_SUCCESS && nResult != SQL_SUCCESS_WITH_INFO) {
      return CheckError(SQL_HANDLE_DBC, this->m_hdbc);
    }
    // set auto commit off
    nResult = ::SQLSetConnectAttr(this->m_hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF, SQL_NTS);
    if (nResult != SQL_SUCCESS && nResult != SQL_SUCCESS_WITH_INFO) {
      return CheckError(SQL_HANDLE_DBC, this->m_hdbc);
    }
    this->m_inTransaction = true;

    return mk_(Int(SQL_SUCCESS), Sequence(), Sequence(), Nil());
  }
  else {
    Tuple res(4);
    res.SetField(1, Int(9001));
    res.SetField(2, Sequence(L"XXXX"));
    res.SetField(3, Sequence(L"Transaction has already started."));
    res.SetField(4, Nil());
    return res;
  }
}

Tuple DBI::EndTransaction(bool commit)
{
  if (this->m_inTransaction) {
    SQLRETURN nResult;
    if (commit) {
      nResult = ::SQLEndTran(SQL_HANDLE_DBC, this->m_hdbc, SQL_COMMIT );   // commit
    }
    else {
      nResult = ::SQLEndTran(SQL_HANDLE_DBC, this->m_hdbc, SQL_ROLLBACK ); // rollback
    }
    if (nResult != SQL_SUCCESS && nResult != SQL_SUCCESS_WITH_INFO) {
      return CheckError(SQL_HANDLE_DBC, this->m_hdbc);
    }
    this->m_inTransaction = false;

    // set auto commit on
    nResult = ::SQLSetConnectAttr(this->m_hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);
    if (nResult != SQL_SUCCESS && nResult != SQL_SUCCESS_WITH_INFO) {
      return CheckError(SQL_HANDLE_DBC, this->m_hdbc);
    }
    return mk_(Int(SQL_SUCCESS), Sequence(), Sequence(), Nil());
  }
  else {
    Tuple res(4);
    res.SetField(1, Int(9002));
    res.SetField(2, Sequence(L"XXXX"));
    res.SetField(3, Sequence(L"Transaction has't started."));
    res.SetField(4, Nil());
    return res;
  }
}

Tuple DBI::Execute(const SEQ<Char> & arg)
{
  SQLHSTMT hstmt;
  SQLRETURN nResult;

  nResult = ::SQLAllocHandle(SQL_HANDLE_STMT, this->m_hdbc, &hstmt);
  if (nResult != SQL_SUCCESS && nResult != SQL_SUCCESS_WITH_INFO) {
    return CheckError(SQL_HANDLE_STMT, hstmt);
  } 
#ifdef _MSC_VER
  wstring sql = arg.GetString();
  nResult = ::SQLPrepareW(hstmt, (SQLWCHAR *)sql.c_str(), SQL_NTS);
#else
  string sql = wstring2utf8str(arg.GetString());
  nResult = ::SQLPrepare(hstmt, (SQLCHAR *)sql.c_str(), SQL_NTS);
#endif // _MSC_VER

  if (nResult != SQL_SUCCESS && nResult != SQL_SUCCESS_WITH_INFO) {
    return CheckError(SQL_HANDLE_STMT, hstmt);
  }
  switch (::SQLExecute(hstmt)) {
    case SQL_SUCCESS:
    case SQL_SUCCESS_WITH_INFO: {
      SQLLEN numrows;
      nResult = ::SQLRowCount(hstmt, &numrows);
      if (nResult != SQL_SUCCESS && nResult != SQL_SUCCESS_WITH_INFO) {
        return CheckError(SQL_HANDLE_STMT, hstmt);
      }
      ::SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
      return mk_(Int(SQL_SUCCESS), Sequence(), Sequence(), (numrows != -1) ? Int(numrows) : Int(0));
    }
    case SQL_NO_DATA_FOUND: {
      return mk_(Int(SQL_SUCCESS), Sequence(), Sequence(), Int(0));
    }
    default: {
      return CheckError(SQL_HANDLE_STMT, hstmt);
    }
  }
}

Tuple DBI::ExecuteQuery(const SEQ<Char> & arg)
{
  SQLRETURN nResult;

  if (this->m_hstmt != SQL_NULL_HSTMT) {
    nResult = ::SQLCloseCursor(this->m_hstmt);
  }
  if (this->m_hstmt == SQL_NULL_HSTMT) {
    nResult = ::SQLAllocHandle(SQL_HANDLE_STMT, this->m_hdbc, &this->m_hstmt);
    if (nResult != SQL_SUCCESS && nResult != SQL_SUCCESS_WITH_INFO) {
      return CheckError(SQL_HANDLE_STMT, this->m_hstmt);
    }
  }
  
#ifdef _MSC_VER
  wstring sql = arg.GetString();
  nResult = ::SQLPrepareW(this->m_hstmt, (SQLWCHAR *)sql.c_str(), SQL_NTS);
#else
  string sql = wstring2utf8str(arg.GetString());
  nResult = ::SQLPrepare(this->m_hstmt, (SQLCHAR *)sql.c_str(), SQL_NTS);
#endif // _MSC_VER

  if (nResult != SQL_SUCCESS && nResult != SQL_SUCCESS_WITH_INFO) {
    return CheckError(SQL_HANDLE_STMT, this->m_hstmt);
  }
  switch (::SQLExecute(this->m_hstmt)) {
    case SQL_SUCCESS:
    case SQL_SUCCESS_WITH_INFO: {
      break;
    }
    case SQL_NO_DATA_FOUND: {
      return mk_(Int(SQL_SUCCESS), Sequence(), Sequence(), Int(0));
    }
    default: {
      return CheckError(SQL_HANDLE_STMT, this->m_hstmt);
    }
  } 
  
  SQLSMALLINT columnCount;
  nResult = ::SQLNumResultCols(this->m_hstmt, &columnCount);
  if (nResult != SQL_SUCCESS && nResult != SQL_SUCCESS_WITH_INFO) {
    return CheckError(SQL_HANDLE_STMT, this->m_hstmt);
  }
  this->m_columnInfo.Clear();
  for (SQLUSMALLINT columnNumber = 1; columnNumber <= columnCount; columnNumber++) {
    SQLSMALLINT bufferLength = 256;
    SQLSMALLINT nameLength;
    SQLSMALLINT dataType;
    SQLULEN     columnSize;
    SQLSMALLINT decimalDigits;
    SQLSMALLINT nullable;

#ifdef _MSC_VER
    SQLWCHAR    columnName[256];

    nResult = ::SQLDescribeColW(this->m_hstmt, columnNumber, columnName, bufferLength, &nameLength,
                                &dataType, &columnSize, &decimalDigits, &nullable);

    SEQ<Char> name (SEQ<Char>(wstring((wchar_t *)columnName, nameLength)));
#else
    SQLCHAR    columnName[256];

    nResult = ::SQLDescribeCol(this->m_hstmt, columnNumber, columnName, bufferLength, &nameLength,
                               &dataType, &columnSize, &decimalDigits, &nullable);

    SEQ<Char> name (SEQ<Char>(utf8str2wstring(string((char *)columnName, nameLength))));
#endif // _MSC_VER
    if (nResult != SQL_SUCCESS && nResult != SQL_SUCCESS_WITH_INFO) {
      return CheckError(SQL_HANDLE_STMT, this->m_hstmt);
    }
    this->m_columnInfo.Insert(Int(columnNumber), mk_(name, Int(dataType), Int(columnSize),
                              Bool(decimalDigits == 1), Bool(nullable == 1)));
  }

  SQLLEN numrows;
  nResult = ::SQLRowCount(this->m_hstmt, &numrows);
  if (nResult != SQL_SUCCESS && nResult != SQL_SUCCESS_WITH_INFO) {
    return CheckError(SQL_HANDLE_STMT, this->m_hstmt);
  }
  //return mk_(Int(SQL_SUCCESS), Sequence(), Sequence(), (numrows != -1) ? Int(numrows) : Int(0));
  return mk_(Int(SQL_SUCCESS), Sequence(), Sequence(), Int(numrows));
}

Tuple DBI::Fetch()
{
  SQLRETURN nResult;
  //nResult = SQLFetch(this->m_hstmt);
  nResult = ::SQLFetchScroll(this->m_hstmt, SQL_FETCH_NEXT, 0);
  switch (nResult) {
    case SQL_SUCCESS:
    case SQL_SUCCESS_WITH_INFO: {
      return mk_(Int(SQL_SUCCESS), Sequence(), Sequence(), Bool(true));
    }
    case SQL_NO_DATA_FOUND: {
      //return mk_(Int(-1), Sequence(), Sequence(), Nil());
      return mk_(Int(SQL_SUCCESS), Sequence(), Sequence(), Bool(false));
    }
    default: {
      return CheckError(SQL_HANDLE_STMT, this->m_hstmt);
    }
  }

/* for pre fetch
  this->m_recordData.Clear();
  Set dom_columnInfo (this->m_columnInfo.Dom());
  Generic cn;
  for (bool bb = dom_columnInfo.First(cn); bb; bb = dom_columnInfo.Next(cn))
  {
    Tuple t (this->m_columnInfo[cn]);
    Tuple res;
    switch (t.GetIntValue(2)) {
      case DBI_INTEGER: { res = GetInt(cn); break; }
      case DBI_STRING: { res = GetString(cn); break; }
      case DBI_DATE: { res = GetDate(cn); break; }
      case DBI_TIME: { res = GetTime(cn); break; }
      case DBI_TIMESTAMP: { res = GetTimestamp(cn); break; }
      defaul: {
      }
    }
  }
*/
}

Tuple DBI::GetInt(const Int & arg)
{
  SQLRETURN nResult;

  SQLUSMALLINT columnNumber = arg.GetValue();

  SQLINTEGER  value;
  SQLLEN      nCol;

  nResult = ::SQLGetData(this->m_hstmt, columnNumber, SQL_INTEGER, &value, sizeof(value), &nCol);
  if (nResult != SQL_SUCCESS && nResult != SQL_SUCCESS_WITH_INFO) {
    return CheckError(SQL_HANDLE_STMT, this->m_hstmt);
  } 
  if (nCol == SQL_NULL_DATA) {
    return mk_(Int(SQL_SUCCESS), Sequence(), Sequence(), Nil());
  }
  else {
    return mk_(Int(SQL_SUCCESS), Sequence(), Sequence(), Int(value));
  }
}

Tuple DBI::GetString(const Int & arg)
{
  Tuple columnInfo (this->m_columnInfo[arg]);

  switch (columnInfo.GetIntValue(2)) {
    case SQL_CHAR:
    case SQL_WVARCHAR:
    case SQL_WLONGVARCHAR:
    case SQL_VARCHAR:
    case SQL_LONGVARCHAR:
    case SQL_TYPE_DATE:
    case SQL_TYPE_TIME:
    case SQL_TYPE_TIMESTAMP: {
      SQLRETURN nResult;
      SQLUSMALLINT columnNumber = arg.GetValue();

#ifdef _MSC_VER
      SQLWCHAR value[256];
      SQLLEN  nCol;

      nResult = ::SQLGetData(this->m_hstmt, columnNumber, SQL_C_WCHAR, value, sizeof(value), &nCol);
      if (nResult != SQL_SUCCESS && nResult != SQL_SUCCESS_WITH_INFO) {
        return CheckError(SQL_HANDLE_STMT, this->m_hstmt);
      } 
      if (nCol == SQL_NULL_DATA) {
        return mk_(Int(SQL_SUCCESS), Sequence(), Sequence(), Nil());
      }
      else {
        //SEQ<Char> res ((wstring((wchar_t *)value, nCol)));
        SEQ<Char> res ((wstring((wchar_t *)value, nCol/2)));
        return mk_(Int(SQL_SUCCESS), Sequence(), Sequence(), res);
      }
#else
      SQLCHAR value[256];
      SQLLEN  nCol;

      nResult = ::SQLGetData(this->m_hstmt, columnNumber, SQL_C_CHAR, value, sizeof(value), &nCol);
      if (nResult != SQL_SUCCESS && nResult != SQL_SUCCESS_WITH_INFO) {
        return CheckError(SQL_HANDLE_STMT, this->m_hstmt);
      } 
      if (nCol == SQL_NULL_DATA) {
        return mk_(Int(SQL_SUCCESS), Sequence(), Sequence(), Nil());
      }
      else {
        SEQ<Char> res (utf8str2wstring(string((char *)value, nCol))); // UTF-8 -> UTF-16
        return mk_(Int(SQL_SUCCESS), Sequence(), Sequence(), res);
      }
#endif // _MSC_VER
    }
    default: {
      SQLRETURN nResult;
      SQLUSMALLINT columnNumber = arg.GetValue();

      SQLCHAR value[256];
      SQLLEN  nCol;

      nResult = ::SQLGetData(this->m_hstmt, columnNumber, SQL_C_CHAR, value, sizeof(value), &nCol);
      if (nResult != SQL_SUCCESS && nResult != SQL_SUCCESS_WITH_INFO) {
        return CheckError(SQL_HANDLE_STMT, this->m_hstmt);
      } 
      if (nCol == SQL_NULL_DATA) {
        return mk_(Int(SQL_SUCCESS), Sequence(), Sequence(), Nil());
      }
      else {
        SEQ<Char> res (SEQ<Char>(string2wstring(string((char *)value, nCol))));;
        return mk_(Int(SQL_SUCCESS), Sequence(), Sequence(), res);
      }
    }
  }
}

Tuple DBI::GetChar(const Int & arg)
{
  SQLRETURN nResult;

  SQLUSMALLINT columnNumber = arg.GetValue();

  SQLWCHAR  value;
  SQLLEN      nCol;

  nResult = ::SQLGetData(this->m_hstmt, columnNumber, SQL_C_WCHAR, &value, sizeof(value), &nCol);
  if (nResult != SQL_SUCCESS && nResult != SQL_SUCCESS_WITH_INFO) {
    return CheckError(SQL_HANDLE_STMT, this->m_hstmt);
  } 
  if (nCol == SQL_NULL_DATA) {
    return mk_(Int(SQL_SUCCESS), Sequence(), Sequence(), Nil());
  }
  else {
    return mk_(Int(SQL_SUCCESS), Sequence(), Sequence(), Char(value));
  }
}

Tuple DBI::GetNumeric(const Int & arg)
{
  SQLRETURN nResult;

  SQLUSMALLINT columnNumber = arg.GetValue();

  SQLCHAR  value[256];
  SQLLEN  nCol;

  nResult = ::SQLGetData(this->m_hstmt, columnNumber, SQL_C_CHAR, &value, sizeof(value), &nCol);
  if (nResult != SQL_SUCCESS && nResult != SQL_SUCCESS_WITH_INFO) {
    return CheckError(SQL_HANDLE_STMT, this->m_hstmt);
  } 
  if (nCol == SQL_NULL_DATA) {
    return mk_(Int(SQL_SUCCESS), Sequence(), Sequence(), Nil());
  }
  else {
    SEQ<Char> res (SEQ<Char>(utf8str2wstring(string((char *)value,nCol))));;
    return mk_(Int(SQL_SUCCESS), Sequence(), Sequence(), res);
  }
}

Tuple DBI::GetQuote(const Int & arg)
{
  Tuple res (GetString(arg));

  if (res.GetIntValue(1) == SQL_SUCCESS) {
    Quote val (res.GetSequence(4).GetString());
    res.SetField(4, val);
  }
  return res;
}

Tuple DBI::GetDate(const Int & arg)
{
  SQLRETURN nResult;

  SQLUSMALLINT columnNumber = arg.GetValue();

  SQL_DATE_STRUCT value;
  SQLLEN  nCol;

  nResult = ::SQLGetData(this->m_hstmt, columnNumber, SQL_C_DATE, &value, sizeof(value), &nCol);
  if (nResult != SQL_SUCCESS && nResult != SQL_SUCCESS_WITH_INFO) {
    return CheckError(SQL_HANDLE_STMT, this->m_hstmt);
  } 
  if (nCol == SQL_NULL_DATA) {
    return mk_(Int(SQL_SUCCESS), Sequence(), Sequence(), Nil());
  }
  else {
    Tuple res (mk_(Int(value.year), Int(value.month), Int(value.day)));
    return mk_(Int(SQL_SUCCESS), Sequence(), Sequence(), res);
  }
}

Tuple DBI::GetTime(const Int & arg)
{
  SQLRETURN nResult;

  SQLUSMALLINT columnNumber = arg.GetValue();

  SQL_TIME_STRUCT value;
  SQLLEN  nCol;

  nResult = ::SQLGetData(this->m_hstmt, columnNumber, SQL_C_TIME, &value, sizeof(value), &nCol);
  if (nResult != SQL_SUCCESS && nResult != SQL_SUCCESS_WITH_INFO) {
    return CheckError(SQL_HANDLE_STMT, this->m_hstmt);
  } 
  if (nCol == SQL_NULL_DATA) {
    return mk_(Int(SQL_SUCCESS), Sequence(), Sequence(), Nil());
  }
  else {
    Tuple res (mk_(Int(value.hour), Int(value.minute), Int(value.second)));
    return mk_(Int(SQL_SUCCESS), Sequence(), Sequence(), res);
  }
}

Tuple DBI::GetTimestamp(const Int & arg)
{
  SQLRETURN nResult;

  SQLUSMALLINT columnNumber = arg.GetValue();

  SQL_TIMESTAMP_STRUCT value;
  SQLLEN  nCol;

  nResult = ::SQLGetData(this->m_hstmt, columnNumber, SQL_C_TIMESTAMP, &value, sizeof(value), &nCol);
  if (nResult != SQL_SUCCESS && nResult != SQL_SUCCESS_WITH_INFO) {
    return CheckError(SQL_HANDLE_STMT, this->m_hstmt);
  } 
  if (nCol == SQL_NULL_DATA) {
    return mk_(Int(SQL_SUCCESS), Sequence(), Sequence(), Nil());
  }
  else {
    Tuple res (mk_(Int(value.year), Int(value.month), Int(value.day),
                   Int(value.hour), Int(value.minute), Int(value.second), Int(value.fraction)));
    return mk_(Int(SQL_SUCCESS), Sequence(), Sequence(), res);
  }
}

Tuple DBI::GetRecordMap()
{
  Map m;
  int num = this->m_columnInfo.Size();
  for (int i = 1; i <= num; i++) {
    Tuple t (this->m_columnInfo[Int(i)]);
    SEQ<Char> name (t.GetSequence(1));
    Tuple res (GetString(Int(i)));
    if (res.GetIntValue(1) != SQL_SUCCESS) {
      return res;
    }
    if (!res.GetField(4).IsNil()) {
      m.Insert(name, res.GetSequence(2));
    }
  }
  return mk_(Int(SQL_SUCCESS), Sequence(), Sequence(), m);
}

Tuple DBI::GetRecordSeq()
{
  Sequence s;
  int num = this->m_columnInfo.Size();
  for (int i = 1; i <= num; i++) {
    Tuple t (this->m_columnInfo[Int(i)]);
    SEQ<Char> name (t.GetSequence(1));
    Tuple res (GetString(Int(i)));
    if (res.GetIntValue(1) != SQL_SUCCESS) {
      return res;
    }
    s.ImpAppend(res.GetField(4));
  }
  return mk_(Int(SQL_SUCCESS), Sequence(), Sequence(), s);
}
