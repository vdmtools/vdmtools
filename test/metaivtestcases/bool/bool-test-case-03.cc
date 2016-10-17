#include "metaiv.h"
#include <sstream>

void metaivtest()
{
  Bool b(true);
  wcout << "b:" << b.GetRefCount() << endl;	

  wcout << "Type: " << b.PrintType() << endl;
  wcout << "b:" << b.GetRefCount() << endl;	
  
  wcout << "Type: " << b.get_mvp()->GetType() << endl;
  wcout << "b:" << b.GetRefCount() << endl;	

  wcout << "ASCII form: "; b.pr_ascii(wcout);
  wcout << endl << "b:" << b.GetRefCount() << endl;

//  b.WriteVal(cout);
  ostringstream os;
  b.WriteVal(os);
  wcout << string2wstring(os.str());
  wcout << endl << "b:" << b.GetRefCount() << endl;
  
  b.Clone();
  wcout << "b:" << b.GetRefCount() << endl;
  
  wcout << b << endl;
  wcout << "b:" << b.GetRefCount() << endl;
}
