#include "metaiv.h"
#include <sstream>

void metaivtest()
{
  Generic g(Int(12));
  wcout << "g:" << g.GetRefCount() << endl;	

  wcout << "Type: " << g.PrintType() << endl;
  wcout << "g:" << g.GetRefCount() << endl;	
  
  wcout << "Type: " << g.get_mvp()->GetType() << endl;
  wcout << "g:" << g.GetRefCount() << endl;	

  wcout << "ASCII form: "; g.pr_ascii(wcout);
  wcout << endl << "g:" << g.GetRefCount() << endl;

  //g.WriteVal(cout);
  ostringstream os;
  g.WriteVal(os);
  wcout << string2wstring(os.str());
  wcout << endl << "g:" << g.GetRefCount() << endl;
  
  g.Clone();
  wcout << "g:" << g.GetRefCount() << endl;
  
  wcout << g << endl;
  wcout << "g:" << g.GetRefCount() << endl;
}
