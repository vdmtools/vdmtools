#include "metaiv.h"
#include <sstream>

void metaivtest()
{
  Token tok(L"Token");
  wcout << "tok:" << tok.GetRefCount() << endl;	

  wcout << "Type: " << tok.PrintType() << endl;
  wcout << "tok:" << tok.GetRefCount() << endl;	
  
  wcout << "Type: " << tok.get_mvp()->GetType() << endl;
  wcout << "tok:" << tok.GetRefCount() << endl;	

  wcout << "ASCII form: "; tok.pr_ascii(wcout);
  wcout << endl << "tok:" << tok.GetRefCount() << endl;

  //tok.WriteVal(cout);
  ostringstream os;
  tok.WriteVal(os);
  wcout << string2wstring(os.str());
  wcout << endl << "tok:" << tok.GetRefCount() << endl;
  
  tok.Clone();
  wcout << "tok:" << tok.GetRefCount() << endl;
  
  wcout << tok << endl;
  wcout << "tok:" << tok.GetRefCount() << endl;
}
