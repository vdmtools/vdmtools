#include "metaiv.h"
#include <sstream>

void metaivtest()
{
  Set s;
  s.Insert(Int(3));
  wcout << "s:" << s.GetRefCount() << endl;	

  wcout << "Type: " << s.PrintType() << endl;
  wcout << "s:" << s.GetRefCount() << endl;	
  
  wcout << "Type: " << s.get_mvp()->GetType() << endl;
  wcout << "s:" << s.GetRefCount() << endl;	

  wcout << "ASCII form: "; s.pr_ascii(wcout);
  wcout << endl << "s:" << s.GetRefCount() << endl;

  //s.WriteVal(cout);
  ostringstream os;
  s.WriteVal(os);
  wcout << string2wstring(os.str());
  wcout << endl << "s:" << s.GetRefCount() << endl;
  
  s.Clone();
  wcout << "s:" << s.GetRefCount() << endl;
  
  wcout << s << endl;
  wcout << "s:" << s.GetRefCount() << endl;
}
