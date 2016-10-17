#include "metaiv.h"
#include <sstream>

void metaivtest()
{
  Tuple tup(2);
  tup.SetField(1, Int(3.14));
  tup.SetField(2, Real(3.14));

  wcout << "tup:" << tup.GetRefCount() << endl;	

  wcout << "Type: " << tup.PrintType() << endl;
  wcout << "tup:" << tup.GetRefCount() << endl;	
  
  wcout << "Type: " << tup.get_mvp()->GetType() << endl;
  wcout << "tup:" << tup.GetRefCount() << endl;	

  wcout << "ASCII form: "; tup.pr_ascii(wcout);
  wcout << endl << "tup:" << tup.GetRefCount() << endl;

  //tup.WriteVal(cout);
  ostringstream os;
  tup.WriteVal(os);
  wcout << string2wstring(os.str());
  wcout << endl << "tup:" << tup.GetRefCount() << endl;
  
  tup.Clone();
  wcout << "tup:" << tup.GetRefCount() << endl;
  
  wcout << tup << endl;
  wcout << "tup:" << tup.GetRefCount() << endl;
}
