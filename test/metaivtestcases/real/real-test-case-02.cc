//arithmetic operators testing for class Int
#include "metaiv.h"

void metaivtest()
{
  Real x(20.2), y=3.14, z(x);
  wcout << "x:" << x.GetRefCount() << " ";  // 2 (x, z) ... 20.2
  wcout << "y:" << y.GetRefCount() << " ";  // 1
  wcout << "z:" << z.GetRefCount() << endl; // 2 (x, z) ... 20.2

  x=x+Int(2);
  wcout << "x:" << x.GetRefCount() << " ";  // 1 ... 22.2 
  wcout << "y:" << y.GetRefCount() << " ";  // 1
  wcout << "z:" << z.GetRefCount() << endl; // 1
  
  x=x+Real(2.1);
  wcout << "x:" << x.GetRefCount() << " ";  // 1 ... 24.3
  wcout << "y:" << y.GetRefCount() << " ";  // 1
  wcout << "z:" << z.GetRefCount() << endl; // 1
  
  z=x+Real(4.4);
  wcout << "x:" << x.GetRefCount() << " ";  // 1
  wcout << "y:" << y.GetRefCount() << " ";  // 1
  wcout << "z:" << z.GetRefCount() << endl; // 1 ... 28.7
  
  y=x+z;
  wcout << "x:" << x.GetRefCount() << " ";  // 1
  wcout << "y:" << y.GetRefCount() << " ";  // 2 (y, cache(53.0))
  wcout << "z:" << z.GetRefCount() << endl; // 1

  x=z;
  z=z+x;
  wcout << "x:" << x.GetRefCount() << " ";
  wcout << "y:" << y.GetRefCount() << " ";  // 2 (y, cache(53.0))
  wcout << "z:" << z.GetRefCount() << endl;
  
  y=y+y;
  wcout << "x:" << x.GetRefCount() << " ";
  wcout << "y:" << y.GetRefCount() << " ";  // 2 (y, cache(106.0))
  wcout << "z:" << z.GetRefCount() << endl;
}
