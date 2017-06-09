#include "metaiv.h"
#include <sstream>

void metaivtest()
{
  Nil n;
  wcout << "n:" << n.GetRefCount() << endl;	

  wcout << "Type: " << n.PrintType() << endl;
  wcout << "n:" << n.GetRefCount() << endl;	
  
  wcout << "Type: " << n.get_mvp()->GetType() << endl;
  wcout << "n:" << n.GetRefCount() << endl;	

  wcout << "ASCII form: "; n.pr_ascii(wcout);
  wcout << endl << "n:" << n.GetRefCount() << endl;

  //n.WriteVal(cout);
  ostringstream os;
  n.WriteVal(os);
  wcout << string2wstring(os.str());

  wcout << endl << "n:" << n.GetRefCount() << endl;
  
  n.Clone();
  wcout << "n:" << n.GetRefCount() << endl;
  
  wcout << n << endl;
  wcout << "n:" << n.GetRefCount() << endl;
}
