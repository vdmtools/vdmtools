#include "metaiv.h"
#include <sstream>

void metaivtest()
{
  Sequence sq(L"nikola");
  wcout << "sq:" << sq.GetRefCount() << endl;	

  wcout << "Type: " << sq.PrintType() << endl;
  wcout << "sq:" << sq.GetRefCount() << endl;	
  
  wcout << "Type: " << sq.get_mvp()->GetType() << endl;
  wcout << "sq:" << sq.GetRefCount() << endl;	

  wcout << "ASCII form: " << sq.ascii() << endl;
  wcout << "sq:" << sq.GetRefCount() << endl;

  Sequence sq1(L"nikola");
  wcout << "sq:" << sq.GetRefCount() << endl;	
  wcout << "sq1:" << sq1.GetRefCount() << endl;	

  //sq1.ImpModify(4, Char(L'O'));
  sq1.ImpModify(4, Int(0));
  wcout << "ASCII form: " << sq.ascii() << endl;
  wcout << "ASCII form: " << sq1.ascii() << endl;
  wcout << "sq:" << sq.GetRefCount() << endl;	

  //sq.WriteVal(cout);
  ostringstream os;
  sq.WriteVal(os);
  wcout << string2wstring(os.str()) << endl;
  wcout << "sq:" << sq.GetRefCount() << endl;
  
  sq.Clone();
  wcout << "sq:" << sq.GetRefCount() << endl;
  
  wcout << sq << endl;
  wcout << "sq:" << sq.GetRefCount() << endl;
}
