#include "metaiv.h"
#include <sstream>

void metaivtest()
{
  Int x(20), y(x), z;
  wcout << "x:" << x.GetRefCount() << " ";  // 3 (x + y + cache(20))
  wcout << "y:" << y.GetRefCount() << " ";  // 3 (x + y + cache(20))
  wcout << "z:" << z.GetRefCount() << endl; // 2 (z + cache(0))

  wcout << "Type: " << x.PrintType() << endl;
  wcout << "x:" << x.GetRefCount() << " ";
  wcout << "y:" << y.GetRefCount() << " ";
  wcout << "z:" << z.GetRefCount() << endl;

  wcout << "Type: " << x.get_mvp()->GetType() << endl;
  wcout << "x:" << x.GetRefCount() << " ";
  wcout << "y:" << y.GetRefCount() << " ";
  wcout << "z:" << z.GetRefCount() << endl;
  
  wcout << "ASCII form: "; x.pr_ascii(wcout);
  wcout << endl << "x:" << x.GetRefCount() << " ";
  wcout << "y:" << y.GetRefCount() << " ";
  wcout << "z:" << z.GetRefCount() << endl;

  //x.WriteVal(cout);
  ostringstream os;
  x.WriteVal(os);
  wcout << string2wstring(os.str());
  wcout << endl << "x:" << x.GetRefCount() << " ";
  wcout << "y:" << y.GetRefCount() << " ";
  wcout << "z:" << z.GetRefCount() << endl;
  
  x.Clone();
  wcout << "x:" << x.GetRefCount() << " ";  // 1 (x(20))
  wcout << "y:" << y.GetRefCount() << " ";  // 2 (y + cache(20))
  wcout << "z:" << z.GetRefCount() << endl; // 2 (z + cache(0))
  
  y.Clone();
  wcout << "x:" << x.GetRefCount() << " ";  // 1 (x(20))
  wcout << "y:" << y.GetRefCount() << " ";  // 1 (x(20))
  wcout << "z:" << z.GetRefCount() << endl; // 2 (z + cache(0))

  wcout << x << " " << y << endl;
  wcout << "x:" << x.GetRefCount() << " ";
  wcout << "y:" << y.GetRefCount() << " ";
  wcout << "z:" << z.GetRefCount() << endl;
}
