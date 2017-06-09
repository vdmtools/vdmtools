#include "metaiv.h"
#include <sstream>

void metaivtest()
{
  Record r(1,2);
  r.SetField(1, Int(3));
  r.SetField(2, Real(3.14));
  
  wcout << "r:" << r.GetRefCount() << endl;	

  wcout << "Type: " << r.PrintType() << endl;
  wcout << "r:" << r.GetRefCount() << endl;	
  
  wcout << "Type: " << r.get_mvp()->GetType() << endl;
  wcout << "r:" << r.GetRefCount() << endl;	

  wcout << "ASCII form: "; r.pr_ascii(wcout);
  wcout << endl << "r:" << r.GetRefCount() << endl;

  //r.WriteVal(cout);
  ostringstream os;
  r.WriteVal(os);
  wcout << string2wstring(os.str());
  wcout << endl << "r:" << r.GetRefCount() << endl;
  
  r.Clone();
  wcout << "r:" << r.GetRefCount() << endl;
  
  wcout << r << endl;
  wcout << "r:" << r.GetRefCount() << endl;
}
