// example.cpp
// generated

#include "dlclass_test1.h"

DlClass* DlClass_new (const wchar_t* name)
{
  //  cerr << L"DlClass_new called\n" << endl;
  if (!wcscmp (name, L"Example")) {
    return new Example ();
  }
  else {
    return 0; // nothing found
  }
}

int DlClass_delete (DlClass* c)
{
  //  cerr << L"DlClass_delete called\n" << endl;
  try {
    delete c;
  }
  catch (...) {
    return 0;
  }
  return 1;
}

Generic DlClass_call (DlClass* c, const wchar_t* name, const Sequence& params, int& success)
{
  // cerr << L"DlClass_call: " << name << "(" << params << ")" << endl;
  Generic result;
  try {
    result = c->DlMethodCall (name, params);
  }
  catch (...) {
    success = 0;
    return result;
  }
  success = 1;
  return result;
}

Generic Example::DlMethodCall (const wchar_t* name, const Sequence &p)
{
  Generic res;
	
  if (wcscmp(name, L"ReturnList")==0) {
    res = this->ReturnList(p);
  }
  else if (wcscmp(name, L"f")==0) {
    Sequence s;
    for (int i=0; i<10; i++) {
      s.ImpAppend(Int(i));
    }
    res = s;
  }
  else if (wcscmp(name, L"extop")==0) {
    Sequence s;
    for (int i=0; i<5; i++) {
      s.ImpAppend(Int(i));
    }
    res = s;
  }
  else {
    // the method does not exist - throw exception
  }
  return res;
}

// implementation, should be in an other file
Generic Example::ReturnList (const Sequence &p)
{
  //  cout << L"Example::ReturnList" << endl;
  Sequence s;
  Sequence arg1 = p[1];
  s=arg1;
  // add some new classes
  s.ImpAppend(DLObject(L"Example", new Example));
  return s;
}
