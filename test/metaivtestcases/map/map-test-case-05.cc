#include "metaiv.h"
#include <sstream>

void metaivtest()
{
  Map m;
  m.Insert(Int(3), Real(3.14));
  wcout << "m:" << m.GetRefCount() << endl;	

  wcout << "Type: " << m.PrintType() << endl;
  wcout << "m:" << m.GetRefCount() << endl;	
  
  wcout << "Type: " << m.get_mvp()->GetType() << endl;
  wcout << "m:" << m.GetRefCount() << endl;	

  wcout << "ASCII form: "; m.pr_ascii(wcout);
  wcout << endl << "m:" << m.GetRefCount() << endl;

  //m.WriteVal(cout);
  ostringstream os;
  m.WriteVal(os);
  wcout << string2wstring(os.str());

  wcout << endl << "m:" << m.GetRefCount() << endl;
  
  m.Clone();
  wcout << "m:" << m.GetRefCount() << endl;
  
  wcout << m << endl;
  wcout << "m:" << m.GetRefCount() << endl;
}
