#include "metaiv.h"

void metaivtest()
{
  ObjectRef or1;
  wcout << "or:" << or1.GetRefCount() << endl;	

  wcout << "Type: " << or1.PrintType() << endl;
  wcout << "or:" << or1.GetRefCount() << endl;	
  
  wcout << "Type: " << or1.get_mvp() ->GetType() << endl;
  wcout << "or:" << or1.GetRefCount() << endl;	

  wcout << "ASCII form: "; or1.pr_ascii(wcout);
  wcout << endl << "or:" << or1.GetRefCount() << endl;

  //or1.Clone();
  //wcout << "or:" << or1.GetRefCount() << endl;
  
  wcout << or1 << endl;
  wcout << "or:" << or1.GetRefCount() << endl;
}
