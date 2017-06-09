#include "metaiv.h"

void metaivtest()
{
  Real x(20.12), y=3.14, z(x);
  wcout << "x:" << x.GetRefCount() << " ";  // 2 (x,y) ... 20.12
  wcout << "y:" << y.GetRefCount() << " ";  // 1       ... 3.14
  wcout << "z:" << z.GetRefCount() << endl; // 2 (x,y) ... 20.12
  
  x=x-Int(2);
  wcout << "x:" << x.GetRefCount() << " ";  // 1 ... 18.12
  wcout << "y:" << y.GetRefCount() << " ";  // 1 ... 3.14
  wcout << "z:" << z.GetRefCount() << endl; // 1 ... 20.12
  
  x=x-Real(2.7);
  wcout << "x:" << x.GetRefCount() << " ";  // 1 ... 15.42
  wcout << "y:" << y.GetRefCount() << " ";  // 1 ... 3.14
  wcout << "z:" << z.GetRefCount() << endl; // 1 ... 20.12

  z=x-Real(4.4);
  wcout << "x:" << x.GetRefCount() << " ";  // 1 ... 15.42
  wcout << "y:" << y.GetRefCount() << " ";  // 1 ... 3.14
  wcout << "z:" << z.GetRefCount() << endl; // 1 ... 11.02
  
  y=x-z;
  wcout << "x:" << x.GetRefCount() << " ";  // 1 ... 15.42
  wcout << "y:" << y.GetRefCount() << " ";  // 1 ... 4.4
  wcout << "z:" << z.GetRefCount() << endl; // 1 ... 11.02
  
  x=z;
  z=z-x;
  wcout << "x:" << x.GetRefCount() << " ";  // 1 ... 11.02
  wcout << "y:" << y.GetRefCount() << " ";  // 1 ... 4.4
  wcout << "z:" << z.GetRefCount() << endl; // 2 (z, cache(0.0))
  
  y=y-y;
  wcout << "x:" << x.GetRefCount() << " ";  // 1 ... 11.02
  wcout << "y:" << y.GetRefCount() << " ";  // 3 (y, z, cache(0.0))
  wcout << "z:" << z.GetRefCount() << endl; // 3 (y, z, cache(0.0))

  z=-x;
  wcout << "x:" << x.GetRefCount() << " ";  // 1 ... 11.02
  wcout << "y:" << y.GetRefCount() << " ";  // 2 (y cache(0.0))
  wcout << "z:" << z.GetRefCount() << endl; // 1 ... -11.02
  
  y=-x;
  wcout << "x:" << x.GetRefCount() << " ";  // 1 ... 11.02
  wcout << "y:" << y.GetRefCount() << " ";  // 1 ... -11.02
  wcout << "z:" << z.GetRefCount() << endl; // 1 ... -11.02
}
