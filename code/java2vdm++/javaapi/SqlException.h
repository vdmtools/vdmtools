#ifndef __SqlException_h__
#define __SqlException_h__

#ifdef _MSC_VER
#include <windows.h>
#endif // _MSC_VER

#include "sql.h"

class SqlException {
public:
  SQLSMALLINT handleType;
  SQLHANDLE handle;  

  SqlException(SQLSMALLINT t, SQLHANDLE h);
};

#endif // __SqlException_h__
