#include "metaiv.h"

void metaivtest()
{
  Real x(20.2), y=3.14, z(x);
  wcout << "x:" << x.GetRefCount() << " ";  // 2 (x, z) ... 20.2
  wcout << "y:" << y.GetRefCount() << " ";  // 1 ... 3.14
  wcout << "z:" << z.GetRefCount() << endl; // 2 (x, z) ... 20.2

  x=x*Int(2);
  wcout << "x:" << x.GetRefCount() << " ";  // 1
  wcout << "y:" << y.GetRefCount() << " ";  // 1
  wcout << "z:" << z.GetRefCount() << endl; // 1
  
  x=x*Real(2.1);
  wcout << "x:" << x.GetRefCount() << " ";  // 1
  wcout << "y:" << y.GetRefCount() << " ";  // 1
  wcout << "z:" << z.GetRefCount() << endl; // 1

  z=x*Real(0.0);
  wcout << "x:" << x.GetRefCount() << " ";  // 1
  wcout << "y:" << y.GetRefCount() << " ";  // 1
  wcout << "z:" << z.GetRefCount() << endl; // 2 (z, cache(0.0))
  
  y=x*z;
  wcout << "x:" << x.GetRefCount() << " ";  // 1
  wcout << "y:" << y.GetRefCount() << " ";  // 3 (y, z, cache(0.0))
  wcout << "z:" << z.GetRefCount() << endl; // 3 (y, z, cache(0.0))

  x=z;
  z=z*x;
  wcout << "x:" << x.GetRefCount() << " ";  // 4 (x, y, z, cache(0.0))
  wcout << "y:" << y.GetRefCount() << " ";  // 4 (x, y, z, cache(0.0))
  wcout << "z:" << z.GetRefCount() << endl; // 4 (x, y, z, cache(0.0))

  y=y*y;
  wcout << "x:" << x.GetRefCount() << " ";
  wcout << "y:" << y.GetRefCount() << " ";
  wcout << "z:" << z.GetRefCount() << endl;
}
