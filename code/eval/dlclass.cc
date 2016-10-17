#include "dlclass.h"
#include <sstream>
#include "string.h"

Generic DlClassUtil::CharToVal (const char * params)
{
  // deserialize params
  string paramstring (params);
  istringstream is (paramstring);
  return ReadVal(is);
}

void DlClassUtil::ValToChar (const Generic & val, char * & resbuf)
{
  // serialize result
  ostringstream os;
  val.WriteVal(os);
  string resstr (os.str());
  resbuf = new char[resstr.length() + 1];
  strcpy(resbuf, resstr.c_str());
}

//DLLFUN void DlClass_callp (DlClass* c, const wchar_t * name, const char * params, int & success, char * & resbuf)
//{
//  Generic paramseq (DlClassUtil::CharToVal(params));
//
//  Generic res (DlClass_call(c, name, paramseq, success));
//
//  DlClassUtil::ValToChar(res, resbuf);
//}
