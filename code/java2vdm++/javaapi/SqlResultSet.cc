#include "SqlResultSet.h"
#include "SqlException.h"
#include "JavaString.h"
#include "j2vmain.h"
#include "tb_wstring.h"

#if defined(_MSC_VER) && defined(_DEBUG)
#include <crtdbg.h>
#endif

#ifdef _MSC_VER
#include <windows.h>
#endif // _MSC_VER

#include "sqlext.h"
#include "sqlucode.h"

#ifdef __linux__
#include <string.h>
#endif // __linux__

#ifdef Nil
#undef Nil
#endif

const wchar_t* vdm_SqlResultSet::name = L"SqlResultSet";

Generic vdm_SqlResultSet::DlMethodCall (const wchar_t* name, const Sequence &p)
{
  Generic res;

  if (!wcscmp (name, L"wasNull'")) {
    res = this->vdm_wasNull();
  } else if (!wcscmp(name, L"getStatement")) {
    res = this->vdm_getStatement();
  } else if (!wcscmp(name, L"getBytes")) {
    if(p[1].IsInt()) res=this->vdm_getBytes(p[1]);
    vdm_String fieldName = *vdm_String::GetDLPtr(p[1]);
    res = this->vdm_getBytes(fieldName);
  } else if (!wcscmp(name, L"getString'")) {
    if(p[1].IsInt()) {
      res = this->vdm_getString(p[1]);
    } else if(p[1].IsReal()) {
      res = this->vdm_getString(Int(p[1]));
    }
  } else if (!wcscmp(name, L"getString''")) {
    vdm_String fieldName = *vdm_String::GetDLPtr(p[1]);
    res = this->vdm_getString(fieldName);
  } else if (!wcscmp(name, L"getInt'")) {
    if(p[1].IsInt()) {
      res = this->vdm_getInt(p[1]);
    } else if(p[1].IsReal()) {
      res = this->vdm_getInt(Int(p[1]));
    }
  } else if (!wcscmp(name, L"getInt''")) {
    vdm_String fieldName = *vdm_String::GetDLPtr(p[1]);
    res = this->vdm_getInt(fieldName);
  } else if (!wcscmp(name, L"next'")) {
    res = this->vdm_next();
  } else if (!wcscmp(name, L"close")) {
    this->vdm_close();
  } else if (!wcscmp(name, L"getColumns")) {
    res = this->columns;
  } else {
      // the method does not exist
    throw 1;
  }
  return res;
}

vdm_SqlResultSet::vdm_SqlResultSet (vdm_SqlStatement* pstmt)
{
  this->pStmt = pstmt;
  this->lastWasNull = -1;
  if(this->pStmt != NULL) {
    create();
  }
}

#define MAX_COLNAME_SIZE 80

void vdm_SqlResultSet::create ()
{
  if(!this->columns.IsEmpty()) return;

  SQLRETURN nRetCode = SQL_SUCCESS;

  this->columnCount = 0;

  nRetCode = ::SQLNumResultCols(this->pStmt->m_hstmt, &(this->columnCount));

  if(nRetCode != SQL_SUCCESS && nRetCode != SQL_SUCCESS_WITH_INFO)
    throw new SqlException(SQL_HANDLE_STMT, this->pStmt->m_hstmt);

#ifdef _MSC_VER
  SQLWCHAR columnName[MAX_COLNAME_SIZE+1];
#else
  SQLCHAR columnName[MAX_COLNAME_SIZE+1];
#endif // _MSC_VER

  SQLSMALLINT nameLength = -1;
  SQLSMALLINT dataType = -1;
  SQLUSMALLINT columnNumber = 1;
  SQLULEN columnSize = 0;
  SQLSMALLINT decimalDigits = -1;
  SQLSMALLINT nullable = -1;

  this->columns = Map();
  this->columnsNames = Map();

  for(columnNumber = 1; columnNumber <= this->columnCount; columnNumber++) {
#ifdef _MSC_VER
    nRetCode = ::SQLDescribeColW(this->pStmt->m_hstmt, columnNumber, columnName, MAX_COLNAME_SIZE,
                                 &nameLength, &dataType, &columnSize, &decimalDigits, &nullable);
#else
    nRetCode = ::SQLDescribeCol(this->pStmt->m_hstmt, columnNumber, columnName, MAX_COLNAME_SIZE,
                                &nameLength, &dataType, &columnSize, &decimalDigits, &nullable);
#endif // _MSC_VER

    if(nRetCode != SQL_SUCCESS && nRetCode != SQL_SUCCESS_WITH_INFO)
      throw new SqlException(SQL_HANDLE_STMT,this->pStmt->m_hstmt);
/*
    int  sqlLength=0;
    SQLSMALLINT stringLength;
    SQLPOINTER  characterAttribute[256];

    nRetCode =
      ::SQLColAttribute (
          this->pStmt->m_hstmt,
          columnNumber,
          SQL_DESC_LENGTH,
          characterAttribute,
          256,
          &stringLength,
          (SQLPOINTER*)&sqlLength);

    if(nRetCode != SQL_SUCCESS && nRetCode != SQL_SUCCESS_WITH_INFO)
      throw new SqlException(SQL_HANDLE_STMT,this->pStmt->m_hstmt);
*/

/*
    SQLSMALLINT targetType = dataType;

    switch(dataType) {
      case SQL_VARCHAR:
        targetType = SQL_C_CHAR;
      break;
      case SQL_CHAR:
        targetType = SQL_C_CHAR;
      break;
      case SQL_INTEGER:
        targetType = SQL_C_LONG;
      break;
      case SQL_SMALLINT:
        targetType = SQL_C_LONG;
      break;
      case SQL_C_FLOAT:
        targetType = SQL_C_DOUBLE;
      break;
      case SQL_C_DOUBLE:
        targetType = SQL_C_DOUBLE;
      break;
    }
*/

    Tuple columnInfo(4);
#ifdef _MSC_VER
    Sequence name(wstring((wchar_t *)&columnName, nameLength));
#else
    Sequence name(TBWSTR::utf8str2wstring(string((char*)&columnName, nameLength)));
#endif // _MSC_VER

    columnInfo.SetField(1,Int(dataType));
    columnInfo.SetField(2,Int(columnSize));  //  sqlLength
    columnInfo.SetField(3,Int(decimalDigits));
    columnInfo.SetField(4,Int(nullable));

    this->columns.Insert(name, columnInfo);
    this->columnsNames.Insert(name, Int(columnNumber));
  }
}

vdm_SqlResultSet::~vdm_SqlResultSet ()
{
  vdm_close ();
}

Bool vdm_SqlResultSet::vdm_last ()
{ return Bool(false); }

Bool vdm_SqlResultSet::vdm_next ()
{
  if(this->pStmt == NULL) return Bool(false);

  SQLRETURN nRetCode = ::SQLFetchScroll(this->pStmt->m_hstmt, SQL_FETCH_NEXT, 0);
  if (nRetCode == SQL_SUCCESS || nRetCode == SQL_SUCCESS_WITH_INFO) {
    return Bool(true);
  }
  return Bool(false);
}

void vdm_SqlResultSet::vdm_close ()
{
  this->columns.Clear();
  this->columnsNames.Clear();
  this->columnCount = 0;
  this->lastWasNull = -1;
}

Bool vdm_SqlResultSet::vdm_first ()
{ return Bool(false); }

Int vdm_SqlResultSet::vdm_getRow ()
{ return Int(0); }

Bool vdm_SqlResultSet::vdm_isLast ()
{ return Bool(false); }

Int vdm_SqlResultSet::vdm_getType ()
{ return Int(0); }

Bool vdm_SqlResultSet::vdm_isFirst ()
{ return Bool(false); }

Tuple vdm_SqlResultSet::vdm_wasNull ()
{
  if (this->lastWasNull == -1) {
    Tuple res(4);
    res.SetField(1, Int(9999));
    res.SetField(2, Sequence(wstring(L"XXXXX")));
    res.SetField(3, Sequence(wstring(L"You must first call getXXX on a column!")));
    res.SetField(4, Nil());
    return res;
  } else {
    Tuple res(4);
    res.SetField(1, Int(SQL_SUCCESS));
    res.SetField(2, Sequence());
    res.SetField(3, Sequence());
    res.SetField(4, Bool(this->lastWasNull != 0));
    return res;
  }
}

Bool vdm_SqlResultSet::vdm_absolute (const Int &)
{ return Bool(false); }

Bool vdm_SqlResultSet::vdm_previous ()
{ return Bool(false); }

Bool vdm_SqlResultSet::vdm_relative (const Int &)
{ return Bool(false); }

void vdm_SqlResultSet::vdm_afterLast ()
{}

void vdm_SqlResultSet::vdm_deleteRow ()
{}

void vdm_SqlResultSet::vdm_insertRow ()
{}

void vdm_SqlResultSet::vdm_updateRow ()
{}

Int vdm_SqlResultSet::vdm_findColumn (const vdm_String &)
{ return Int(0); }

//
//  If SQLGetData is called more than one time in a row for a column
//  containing fixed-length data, it returns SQL_NO_DATA_FOUND
//  for all calls after the first!!!
//
Tuple vdm_SqlResultSet::vdm_getInt (const Int &vdm_columnIndex)
{
  try {
    SQLRETURN nRetCode;
    SQLUSMALLINT colIndex = vdm_columnIndex.GetValue();
    SQLULEN colsize;
    SQLSMALLINT datatype, size, decimal, null;
#ifdef _MSC_VER
    SQLWCHAR name[MAX_COLNAME_SIZE];
#else
    SQLCHAR name[MAX_COLNAME_SIZE];
#endif // _MSC_VER
    SQLSMALLINT namesize;

#ifdef _MSC_VER
    nRetCode =
      ::SQLDescribeColW(this->pStmt->m_hstmt, colIndex, name, MAX_COLNAME_SIZE,
                        &namesize, &datatype, &colsize, &decimal, &null);
#else
    nRetCode =
      ::SQLDescribeCol(this->pStmt->m_hstmt, colIndex, name, MAX_COLNAME_SIZE,
                       &namesize, &datatype, &colsize, &decimal, &null);
#endif // _MSC_VER

    if(nRetCode != SQL_SUCCESS && nRetCode != SQL_SUCCESS_WITH_INFO) {
      throw new SqlException(SQL_HANDLE_STMT, this->pStmt->m_hstmt);
    }
    Tuple res(4);
    switch(datatype) {
      case SQL_BIGINT:
      case SQL_INTEGER: {
        SQLINTEGER zbr;
        SQLLEN zbrsize;

//  Not supported by MSAccess ODBC driver!
//
//        nRetCode = SQL_REFRESH_RECORD(this->pStmt->m_hstmt, SQL_MAX_ROWS, SQL_LOCK_UNLOCK);
//        if(nRetCode != SQL_SUCCESS && nRetCode != SQL_SUCCESS_WITH_INFO)
//          throw new SqlException(SQL_HANDLE_STMT,this->pStmt->m_hstmt);

        nRetCode = ::SQLGetData(this->pStmt->m_hstmt, colIndex, SQL_C_SLONG, &zbr, sizeof(zbr), &zbrsize);

        if(nRetCode != SQL_SUCCESS && nRetCode != SQL_SUCCESS_WITH_INFO)
          throw new SqlException(SQL_HANDLE_STMT, this->pStmt->m_hstmt);

        if (zbrsize == SQL_NULL_DATA) {
          this->lastWasNull = 1;
          res.SetField(4, Nil());
        } else {
          this->lastWasNull = 0;
          res.SetField(4, Int(zbr));
        }
        break;
      }
      default:
        res.SetField(4, Nil());
      break;
    }
    res.SetField(1, Int(SQL_SUCCESS));
    res.SetField(2, Sequence());
    res.SetField(3, Sequence());
    return res;
  }
  catch(SqlException* e) {
    Tuple res (CheckError(e->handleType, e->handle));
    delete e;
    return res;
  }
}

void vdm_SqlResultSet::vdm_refreshRow ()
{}

Bool vdm_SqlResultSet::vdm_rowDeleted ()
{ return Bool(false); }

Bool vdm_SqlResultSet::vdm_rowUpdated ()
{ return Bool(false); }

void vdm_SqlResultSet::vdm_beforeFirst ()
{}

Int vdm_SqlResultSet::vdm_getByte (const Int &)
{ return Int(0); }

Int vdm_SqlResultSet::vdm_getLong (const Int &)
{ return Int(0); }

Bool vdm_SqlResultSet::vdm_isAfterLast ()
{ return Bool(false); }

Bool vdm_SqlResultSet::vdm_rowInserted ()
{ return Bool(false); }

type_iiM vdm_SqlResultSet::vdm_getBytes (const Int &)
{ return type_iiM(); }

Int vdm_SqlResultSet::vdm_getFetchSize ()
{ return Int(0); }

Real vdm_SqlResultSet::vdm_getFloat (const Int &)
{ return Real(0); }

Int vdm_SqlResultSet::vdm_getShort (const Int &)
{ return Int(0); }

DLObject vdm_SqlResultSet::vdm_getStatement ()
{
  return DLObject(L"SqlStatement", this->pStmt);
}

void vdm_SqlResultSet::vdm_setFetchSize (const Int &)
{}

void vdm_SqlResultSet::vdm_updateObject (const Int &, const vdm_Object &)
{}

void vdm_SqlResultSet::vdm_clearWarnings ()
{}

//DLObject vdm_SqlResultSet::vdm_getCursorName () ;

Real vdm_SqlResultSet::vdm_getDouble (const Int &)
{ return Real(0); }

Tuple vdm_SqlResultSet::vdm_getString (const Int &vdm_columnIndex)
{
  try {
    SQLUSMALLINT colIndex = vdm_columnIndex.GetValue();
    SQLULEN colsize;
    SQLSMALLINT datatype, size, decimal, null, namesize;

    SQLRETURN nRetCode;
#ifdef _MSC_VER
    SQLWCHAR name[MAX_COLNAME_SIZE];
    nRetCode = ::SQLDescribeColW(this->pStmt->m_hstmt, colIndex, name, MAX_COLNAME_SIZE,
                                 &namesize, &datatype, &colsize, &decimal, &null);
#else
    SQLCHAR name[MAX_COLNAME_SIZE];
    nRetCode = ::SQLDescribeCol(this->pStmt->m_hstmt, colIndex, name, MAX_COLNAME_SIZE,
                                &namesize, &datatype, &colsize, &decimal, &null);
#endif // _MSCVER
    if(nRetCode != SQL_SUCCESS && nRetCode != SQL_SUCCESS_WITH_INFO){
      throw new SqlException(SQL_HANDLE_STMT, this->pStmt->m_hstmt);
    }

    Tuple res(4);
    switch(datatype) {
      case SQL_CHAR:
      case SQL_VARCHAR:
      case SQL_LONGVARCHAR:
      case SQL_WVARCHAR:
      case SQL_WLONGVARCHAR:
      case SQL_TYPE_DATE:
      case SQL_TYPE_TIME:
      case SQL_TYPE_TIMESTAMP: {
#ifdef _MSC_VER
        SQLWCHAR* zbr = new SQLWCHAR[colsize+1];
        zbr[0] = '\0';
        SQLLEN zbrsize;

//  Not supported by MSAccess ODBC driver!
//
//        nRetCode = SQL_REFRESH_RECORD(this->pStmt->m_hstmt, SQL_MAX_ROWS, SQL_LOCK_UNLOCK);
//        if(nRetCode != SQL_SUCCESS && nRetCode != SQL_SUCCESS_WITH_INFO)
//          throw new SqlException(SQL_HANDLE_STMT, this->pStmt->m_hstmt);

        nRetCode = ::SQLGetData(this->pStmt->m_hstmt, colIndex, SQL_C_WCHAR, zbr, colsize, &zbrsize);
        if(nRetCode != SQL_SUCCESS && nRetCode != SQL_SUCCESS_WITH_INFO) {
          delete[] zbr;
          throw new SqlException(SQL_HANDLE_STMT, this->pStmt->m_hstmt);
        }

        if (zbrsize == SQL_NULL_DATA) {
          this->lastWasNull = 1;
          res.SetField(4, Nil());
        } else {
          this->lastWasNull = 0;
          res.SetField(4, Sequence(wstring((wchar_t *)zbr, zbrsize)));
          delete[] zbr;
        }
#else
        SQLCHAR* zbr = new SQLCHAR[colsize+1];
        zbr[0] = '\0';
        SQLLEN zbrsize;

//  Not supported by MSAccess ODBC driver!
//
//        nRetCode = SQL_REFRESH_RECORD(this->pStmt->m_hstmt, SQL_MAX_ROWS, SQL_LOCK_UNLOCK);
//        if(nRetCode != SQL_SUCCESS && nRetCode != SQL_SUCCESS_WITH_INFO)
//          throw new SqlException(SQL_HANDLE_STMT, this->pStmt->m_hstmt);

        nRetCode = ::SQLGetData(this->pStmt->m_hstmt, colIndex, SQL_C_CHAR, zbr, colsize, &zbrsize);
        if(nRetCode != SQL_SUCCESS && nRetCode != SQL_SUCCESS_WITH_INFO) {
          delete[] zbr;
          throw new SqlException(SQL_HANDLE_STMT, this->pStmt->m_hstmt);
        }

        if (zbrsize == SQL_NULL_DATA) {
          this->lastWasNull = 1;
          res.SetField(4, Nil());
        } else {
          this->lastWasNull = 0;
          res.SetField(4, Sequence(TBWSTR::utf8str2wstring(string((char*)zbr, zbrsize))));
          delete[] zbr;
        }
#endif // _MSC_VER
        break;
      }
      default: {
//        res.SetField(4, Nil());
        SQLCHAR zbr[256];
        SQLLEN zbrsize;
        nRetCode = ::SQLGetData(this->pStmt->m_hstmt, colIndex, SQL_C_CHAR, zbr, sizeof(zbr), &zbrsize);
        if(nRetCode != SQL_SUCCESS && nRetCode != SQL_SUCCESS_WITH_INFO) {
          throw new SqlException(SQL_HANDLE_STMT, this->pStmt->m_hstmt);
        }

        if (zbrsize == SQL_NULL_DATA) {
          this->lastWasNull = 1;
          res.SetField(4, Nil());
        } else {
          this->lastWasNull = 0;
          res.SetField(4, Sequence(TBWSTR::string2wstring(string((char *)zbr, zbrsize))));
        }
        break;
      }
    }
    res.SetField(1, Int(SQL_SUCCESS));
    res.SetField(2, Sequence());
    res.SetField(3, Sequence());
    return res;
  }
  catch(SqlException* e) {
    Tuple res (CheckError(e->handleType, e->handle));
    delete e;
    return res;
  }
}

Tuple vdm_SqlResultSet::CheckError(SQLSMALLINT handleType, SQLHANDLE handle)
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

Bool vdm_SqlResultSet::vdm_isBeforeFirst ()
{ return Bool(false); }

Bool vdm_SqlResultSet::vdm_getBoolean (const Int &)
{ return Bool(false); }

Int vdm_SqlResultSet::vdm_getConcurrency ()
{ return Int(0); }

void vdm_SqlResultSet::vdm_updateNull (const Int &)
{}

void vdm_SqlResultSet::vdm_moveToInsertRow ()
{}

void vdm_SqlResultSet::vdm_updateInt (const Int &, const Int &)
{}

void vdm_SqlResultSet::vdm_cancelRowUpdates ()
{}

void vdm_SqlResultSet::vdm_moveToCurrentRow ()
{}

void vdm_SqlResultSet::vdm_updateByte (const Int &, const Int &)
{}

void vdm_SqlResultSet::vdm_updateLong (const Int &, const Int &)
{}

Int vdm_SqlResultSet::vdm_getFetchDirection ()
{ return Int(0); }

Tuple vdm_SqlResultSet::vdm_getInt (const vdm_String &vdm_name)
{
  Sequence name(vdm_name.getValue());

  if ( this->columns.DomExists(name) ) {
    return vdm_getInt(this->columnsNames[name]);
  } else {
    return vdm_getInt(Int(-1));
  }
}

void vdm_SqlResultSet::vdm_setFetchDirection (const Int &)
{}

void vdm_SqlResultSet::vdm_updateFloat (const Int &, const Real &)
{}

void vdm_SqlResultSet::vdm_updateShort (const Int &, const Int &)
{}

Int vdm_SqlResultSet::vdm_getByte (const vdm_String &)
{ return Int(0); }

Int vdm_SqlResultSet::vdm_getLong (const vdm_String &)
{ return Int(0); }

void vdm_SqlResultSet::vdm_updateDouble (const Int &, const Real &)
{}

type_iiM vdm_SqlResultSet::vdm_getBytes (const vdm_String &)
{ return type_iiM(); }

Real vdm_SqlResultSet::vdm_getFloat (const vdm_String &)
{ return Real(0); }

Int vdm_SqlResultSet::vdm_getShort (const vdm_String &)
{ return Int(0); }

void vdm_SqlResultSet::vdm_updateBoolean (const Int &, const Bool &)
{}

Real vdm_SqlResultSet::vdm_getDouble (const vdm_String &)
{ return Real(0); }

Tuple vdm_SqlResultSet::vdm_getString (const vdm_String &vdm_name)
{
  Sequence name(vdm_name.getValue());

  if ( this->columns.DomExists(name) ) {
    return vdm_getString(this->columnsNames[name]);
  } else {
    return vdm_getString(Int(-1));
  }
}

Bool vdm_SqlResultSet::vdm_getBoolean (const vdm_String &)
{ return Bool(false); }

void vdm_SqlResultSet::vdm_updateBytes (const Int &, const type_iiM &)
{}

void vdm_SqlResultSet::vdm_updateNull (const vdm_String &)
{}

void vdm_SqlResultSet::vdm_updateInt (const vdm_String &, const Int &)
{}

void vdm_SqlResultSet::vdm_updateByte (const vdm_String &, const Int &)
{}

void vdm_SqlResultSet::vdm_updateLong (const vdm_String &, const Int &)
{}

void vdm_SqlResultSet::vdm_updateFloat (const vdm_String &, const Real &)
{}

void vdm_SqlResultSet::vdm_updateShort (const vdm_String &, const Int &)
{}

void vdm_SqlResultSet::vdm_updateDouble (const vdm_String &, const Real &)
{}

void vdm_SqlResultSet::vdm_updateString (const Int &, const vdm_String &)
{}

void vdm_SqlResultSet::vdm_updateBoolean (const vdm_String &, const Bool &)
{}

void vdm_SqlResultSet::vdm_updateBytes (const vdm_String &, const type_iiM &)
{}

void vdm_SqlResultSet::vdm_updateString (const vdm_String &, const vdm_String &)
{}

vdm_SqlResultSet *vdm_SqlResultSet::GetDLPtr(const DLObject& obj)
{
  vdm_SqlResultSet *objPtr = (vdm_SqlResultSet*) obj.GetPtr();
  return objPtr;
}
