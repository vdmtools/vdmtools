#ifndef __dbi_h__
#define __dbi_h__

#ifdef _MSC_VER
#include <windows.h>
#endif // _MSC_VER

#include "dlclass.h"
#include "sql.h"

class DBI : public DlClass
{
public:
  Generic DlMethodCall (const wchar_t* name, const Sequence &p);

  DBI ();
  virtual ~DBI ();

private:
  Tuple Connect(const Sequence & arg);
  Tuple Disconnect();

  Tuple CheckError(SQLSMALLINT HandleType, SQLHANDLE Handle);

  Tuple Execute(const SEQ<Char> & arg);
  Tuple ExecuteQuery(const SEQ<Char> & arg);
  Tuple Fetch();
  Tuple GetInt(const Int & arg);
  Tuple GetString(const Int & arg);
  Tuple GetChar(const Int & arg);
  Tuple GetNumeric(const Int & arg);
  Tuple GetQuote(const Int & arg);
  Tuple GetDate(const Int & arg);
  Tuple GetTime(const Int & arg);
  Tuple GetTimestamp(const Int & arg);
  Tuple GetRecordMap();
  Tuple GetRecordSeq();

  Tuple StartTransaction();
  Tuple EndTransaction(bool commit);

private:
  SQLHENV m_henv;
  SQLHDBC m_hdbc;
  SQLHSTMT m_hstmt;

  bool m_inTransaction;
  Map m_columnInfo;
  Map m_recordData;
};

#endif // __dbi_h__
