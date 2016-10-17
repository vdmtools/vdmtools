#include "SqlException.h"

SqlException::SqlException(SQLSMALLINT t, SQLHANDLE h)
{
  this->handleType = t;
  this->handle = h;
}

