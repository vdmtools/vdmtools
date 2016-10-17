#include "metaiv.h"
#include <sstream>

void metaivtest()
{
  Quote q(L"quote");
  wcout << "q:" << q.GetRefCount() << endl;	

  wcout << "Type: " << q.PrintType() << endl;
  wcout << "q:" << q.GetRefCount() << endl;	
  
  wcout << "Type: " << q.get_mvp()->GetType() << endl;
  wcout << "q:" << q.GetRefCount() << endl;	

  wcout << "ASCII form: "; q.pr_ascii(wcout);
  wcout << endl << "q:" << q.GetRefCount() << endl;

  //q.WriteVal(cout);
  ostringstream os;
  q.WriteVal(os);
  wcout << string2wstring(os.str());

  wcout << endl << "q:" << q.GetRefCount() << endl;
  
  q.Clone();
  wcout << "q:" << q.GetRefCount() << endl;
  
  wcout << q << endl;
  wcout << "q:" << q.GetRefCount() << endl;
}
