#include "metaiv.h"
#include <sstream>

void metaivtest()
{
  Real x(20.2), y(x), z;
  wcout << "x:" << x.GetRefCount() << " ";
  wcout << "y:" << y.GetRefCount() << " ";
  wcout << "z:" << z.GetRefCount() << endl; // 2 (z, cache(0.0))

  wcout << "Type: " << x.PrintType() << endl;
  wcout << "x:" << x.GetRefCount() << " ";
  wcout << "y:" << y.GetRefCount() << " ";
  wcout << "z:" << z.GetRefCount() << endl; // 2 (z, cache(0.0))

  wcout << "Type: " << x.get_mvp()->GetType() << endl;
  wcout << "x:" << x.GetRefCount() << " ";
  wcout << "y:" << y.GetRefCount() << " ";
  wcout << "z:" << z.GetRefCount() << endl; // 2 (z, cache(0.0))

  wcout << "ASCII form: "; x.pr_ascii(wcout);
  wcout << endl << "x:" << x.GetRefCount() << " ";
  wcout << "y:" << y.GetRefCount() << " ";
  wcout << "z:" << z.GetRefCount() << endl; // 2 (z, cache(0.0))

  //x.WriteVal(cout);
  ostringstream os;
  x.WriteVal(os);
  wcout << string2wstring(os.str());
  wcout << endl << "x:" << x.GetRefCount() << " ";
  wcout << "y:" << y.GetRefCount() << " ";
  wcout << "z:" << z.GetRefCount() << endl; // 2 (z, cache(0.0))
  
  x.Clone();
  wcout << "x:" << x.GetRefCount() << " ";
  wcout << "y:" << y.GetRefCount() << " ";
  wcout << "z:" << z.GetRefCount() << endl; // 2 (z, cache(0.0))
  
  y.Clone();
  wcout << "x:" << x.GetRefCount() << " ";
  wcout << "y:" << y.GetRefCount() << " ";
  wcout << "z:" << z.GetRefCount() << endl; // 2 (z, cache(0.0))

  wcout << x << " " << y << endl;
  wcout << "x:" << x.GetRefCount() << " ";
  wcout << "y:" << y.GetRefCount() << " ";
  wcout << "z:" << z.GetRefCount() << endl;
}
