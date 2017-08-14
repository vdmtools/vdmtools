#include "j2vmain.h"
#include "dlclass.h"
//#include <crtdbg.h>
#include <stdarg.h>

#include "Object.h"
#include "J2VUTIL.h"
#include "JavaLocale.h"
#include "Character.h"
#include "JavaString.h"
#include "Integer.h"
#include "StringBuffer.h"
#include "FileOutputStream.h"
#include "FileInputStream.h"
#include "FileDescriptor.h"
#include "File.h"
#include "Double.h"
#include "DriverManager.h"
#include "SqlConnection.h"
#include "SqlStatement.h"
#include "SqlResultSet.h"
#include "StreamTokenizer.h"

#if defined(_MSC_VER) && defined(_DEBUG)
#include <crtdbg.h>
#endif

#if defined( __linux__ ) || defined( __CYGWIN__ )
#include <string.h>
#endif // __linux__ || __CYGWIN__

FILE* pLog = NULL;

DlClass* DlClass_new (const wchar_t* name)
{
#ifdef DEBUG
  cerr << L"DlClass_new called" << endl;
#endif //DEBUG
  if (!wcscmp(name, vdm_J2VUTIL::name))
    return new vdm_J2VUTIL ();
  else if (!wcscmp(name, vdm_Object::name))
    return new vdm_Object ();
  else if (!wcscmp(name, vdm_String::name))
    return new vdm_String ();
  else if (!wcscmp(name, vdm_Character::name))
    return new vdm_Character ();
  else if (!wcscmp(name, vdm_StringBuffer::name))
    return new vdm_StringBuffer ();
  else if (!wcscmp(name, vdm_Locale::name))
    return new vdm_Locale ();
  else if (!wcscmp(name, vdm_Integer::name))
    return new vdm_Integer ();
  else if (!wcscmp(name, vdm_FileDescriptor::name))
    return new vdm_FileDescriptor ();
  else if (!wcscmp(name, vdm_FileOutputStream::name))
    return new vdm_FileOutputStream ();
  else if (!wcscmp(name, vdm_FileInputStream::name))
    return new vdm_FileInputStream ();
  else if (!wcscmp(name, vdm_DriverManager::name))
    return new vdm_DriverManager ();
  else if (!wcscmp(name, vdm_SqlConnection::name))
    return new vdm_SqlConnection ();
  else if (!wcscmp(name, vdm_SqlStatement::name))
    return new vdm_SqlStatement ();
  else if (!wcscmp(name, vdm_SqlResultSet::name))
    return new vdm_SqlResultSet();
  else if (!wcscmp(name, vdm_File::name))
    return new vdm_File ();
  else if (!wcscmp(name, StreamTokenizer::name))
    return new StreamTokenizer();
  else if (!wcscmp(name, vdm_Double::name))
    return new vdm_Double();
  else
    return 0;
}

int DlClass_delete (DlClass* c)
{
#ifdef DEBUG
  cerr << L"DlClass_delete called" << endl;
#endif //DEBUG
  //  try {
  //    delete c;
  //  } catch (...) {
  //    return 0;
  //  }
  return 1;
}

Generic DlClass_call (DlClass* c, const wchar_t* name, const Sequence& params, int& success)
{
#ifdef J2VLIB_LOG
  string sname = wstring2string(wstring(name));
  string sparams;
  bool printParams = true;
  for(int i = 1; i <= params.Length(); i++) {
    if (params[i].IsSequence() && Sequence(params[i]).Length() == 0) 
      printParams = false;
  }
  try {
    if (printParams) 
      sparams = wstring2string(params.ascii());
  } catch (...) {
    sparams = "Can not convert parameters to text!";
  }

  LOG("DlClass_call(...,%s,%s...)\n", sname.c_str(), sparams.c_str());
#endif // J2VLIB_LOG

#ifdef DEBUG
  cerr << L"DlClass_call: " << name << L"(" << params << L")" << endl;
#endif //DEBUG

  Generic result;
  success = 1;
  try {
      if(c != NULL) {
        result = c->DlMethodCall (name, params);
      }
  }
  catch (...) {
    success = 0;
    LOG("Exception ... is caught\n");
  }

#ifdef J2VLIB_LOG
  string sresult = wstring2string(result.ascii());
  LOG("DlClass_call(...,%s,...): success=%d result=%s\n",sname.c_str(),success,sresult.c_str());
#endif

  return result;
}

#ifndef _MSC_VER
#define _vsnprintf vsnprintf
#endif // _MSC_VER

void Log (const char* format, ...)
{
  char buf [1024]; // Increase the buffer size if necessary

  va_list args; 
  va_start(args, format);
  if (_vsnprintf(buf, sizeof(buf), format, args) < 0)
  {
    string str ("Out of buffer space");
    memcpy (buf, str.c_str(), str.length() + 1);
  }
  va_end(args);

//  fprintf(pLog,"%s",buf); fflush(pLog);

#ifdef _MSC_VER 
#ifdef _DEBUG
  OutputDebugString(buf);
#endif //_DEBUG
#endif //_MSC_VER
}
