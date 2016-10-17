// example.cpp
// generated

#include "dlexample.h"

DlClass* DlClass_new (const char* name)
{
  cerr << "DlClass_new called\n" << endl;
  if (!strcmp (name, "Example"))
    return new Example ();
  else
    return 0; // nothing found
}

int DlClass_delete (DlClass* c)
{
  cerr << "DlClass_delete called\n" << endl;
  try 
    {
      delete c;
    }
  catch (...)
    {
      return 0;
    }
  return 1;
}

Generic DlClass_call (DlClass* c, const char* name, const Sequence& params, int& success)
{
  cerr << "DlClass_call: " << name << "(" << params << ")" << endl;
  Generic result;
  try
    {
      result = c->DlMethodCall (name, params);
    }
  catch (...)
    {
      success = 0;
      return result;
    }
  success = 1;
  return result;
}

Generic Example::DlMethodCall (const char* name, const Sequence &p)
{
  Generic res;
	
  if (!strcmp (name, "DoSomething"))
    res = this->DoSomething (p); // we could even catch exceptions here!
  else
    {
      // the method does not exist - throw exception
    }
	
  return res;
}


// implementation, should be in an other file
Generic Example::DoSomething (const Sequence &p)
{
  cout << "Example::DoSomething" << endl;
  return (Generic)p;
}
