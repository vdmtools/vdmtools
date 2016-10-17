#include "metaiv.h"
#include <sstream>

void metaivtest()
{
  Text t(L"Text");
  wcout << "t:" << t.GetRefCount() << endl;	

  wcout << "Type: " << t.PrintType() << endl;
  wcout << "t:" << t.GetRefCount() << endl;	
  
  wcout << "Type: " << t.get_mvp()->GetType() << endl;
  wcout << "t:" << t.GetRefCount() << endl;	

  wcout << "ASCII form: "; t.pr_ascii(wcout);
  wcout << endl << "t:" << t.GetRefCount() << endl;

  //t.WriteVal(cout);
  ostringstream os;
  t.WriteVal(os);
  wcout << string2wstring(os.str());
  wcout << endl << "t:" << t.GetRefCount() << endl;
  
  t.Clone();
  wcout << "t:" << t.GetRefCount() << endl;
  
  wcout << t << endl;
  wcout << "t:" << t.GetRefCount() << endl;
}
