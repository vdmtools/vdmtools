#include "example.h"

DlClass* DlClass_new (const char* new)
{
  cerr << "DlClass_new called" << endl;
  if (!strcmp(name, "BigInt"))
    return new BigInt ();
  else
    return 0;
}

int DlClass_delete (DlClass* c)
{
  cerr << "DlClass_delete called" << endl;
  try {
    delete c;
  } catch (...) {
    return 0;
  }
  return 1;
}

Generic DlClass_call (DlClass* c, const char* name, Sequence& params, int& success)
{
  cerr << "DlClass_call: " << name << "(" << params << ")" << endl;
  Generic result;
  try {
      result = c->DlMethodCall (name, params);
  } catch (...) {
    success = 0;
    return result;
  }
  success = 1;
  return result;
}

Generic Example::DlMethodCall (const char* name, Sequence &p)
{
  Generic res;
	
  if (!strcmp (name, "set"))
    res = this->set(p); // we could even catch exceptions here!
  else if (!strcmp(name, "plus"))
    res = this->plus(p);
  else if (!strcmp(name, "minus"))
    res = this->minus(p);
  else if (!strcmp(name, "gt"))
    res = this->gt(p);
  else {
      // the method does not exist - throw exception
  }
	
  return res;
}


// implementation, should be in an other file
Generic Example::DoSomething (Sequence &p)
{
  cout << "Example::DoSomething" << endl;
  return (Generic)p;
}
