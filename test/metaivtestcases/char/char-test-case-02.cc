#include "metaiv.h"
#include <sstream>

void metaivtest()
{
  Char c('a');
  wcout << "c:" << c.GetRefCount() << endl;	

  wcout << "Type: " << c.PrintType() << endl;
  wcout << "c:" << c.GetRefCount() << endl;	
  
  wcout << "Type: " << c.get_mvp()->GetType() << endl;
  wcout << "c:" << c.GetRefCount() << endl;	

  wcout << "ASCII form: "; c.pr_ascii(wcout);
  wcout << endl << "c:" << c.GetRefCount() << endl;

  //c.WriteVal(cout);
  ostringstream os;
  c.WriteVal(os);
  wcout << string2wstring(os.str());
  wcout << endl << "c:" << c.GetRefCount() << endl;
  
  c.Clone();
  wcout << "c:" << c.GetRefCount() << endl;
  
  wcout << c << endl;
  wcout << "c:" << c.GetRefCount() << endl;
}
